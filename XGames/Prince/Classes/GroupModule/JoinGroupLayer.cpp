#include "JoinGroupLayer.h"
#include "Widget/EditBoxEx.h"
#include "DataManager/DataManager.h"
#include "JoinGroupModel.h"
#include "MyGroupScene.h"

JoinGroupLayer::JoinGroupLayer()
{
	m_customScrollBar = nullptr;
	JoinGroupModel::getInstance()->addObserver(this);
	m_groupResId = 1;
	m_curPage = 1;
	m_chooseApplyCancleId = -1;
	m_findState = SEARCH_TYPE_ALL;
}

JoinGroupLayer::~JoinGroupLayer()
{
	JoinGroupModel::getInstance()->removeObserver(this);
}

bool JoinGroupLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	//进入我的帮派主页

	initUI();
	initGroupInfoPanel();
	return true;
}

void JoinGroupLayer::onEnter()
{
	Layer::onEnter();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(JoinGroupLayer::updateNextFram),this,0,false);
}

void JoinGroupLayer::updateNextFram(float ft)
{
	m_curPage = 1;
	m_JoinGroupController.getGroupInfo();
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(JoinGroupLayer::updateNextFram), this);
}

void JoinGroupLayer::onExit()
{
	Layer::onExit();
	ResourceLoader::getInstance()->removeUIResource("UIGang");
}

void JoinGroupLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;

	switch (observerParam->id)
	{
	case nMAIN_CMD_GROUP_MAIN_INFO:
		{
			m_curPage = 1;
			addGroupList();

			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_GROUP_APPLY);
		}
		break;
	case nMAIN_CMD_GROUP_CREATE:
		{
			//创建成功
			//切换到有帮派界面
			Director::getInstance()->replaceScene(MyGroupScene::create());
		}
		break;
	case nMAIN_CMD_GROUP_SEARCH:
		{
			addGroupList();
		}
		break;
	case nMAIN_CMD_GROUP_APPLY_JOIN:
		{
			bool flag = *(bool *)observerParam->updateData;

			if (!flag) //不需要审核
			{
				//切换到有帮派界面
				GROUP_INFO groupInfo = JoinGroupModel::getInstance()->getGroupInfo().at(m_chooseApplyCancleId);
				MainModel::getInstance()->getMainParam()->mGroupId = groupInfo.id;
				MainModel::getInstance()->getMainParam()->mGroupName = groupInfo.name;

				Director::getInstance()->replaceScene(MyGroupScene::create());
			}
			else
			{
				setApplyCancleNodeState(true);
			}
		}
		break;
	case nMAIN_CMD_GROUP_CANCE_APLLY:
		{
			setApplyCancleNodeState(false);
		}
		break;
	case nMAIN_CMD_GROUP_PUSH:
		{
			auto call = [](Ref* ref, CustomRetType type)->void
			{
				Director::getInstance()->replaceScene(MyGroupScene::create());
			};

			stringstream ss;
			string groupName = MainModel::getInstance()->getMainParam()->mGroupName;

			ss << DataManager::getInstance()->searchCommonTexdtById(10167) << groupName 
				<< DataManager::getInstance()->searchCommonTexdtById(10168);
			CustomPop::showUtf8(ss.str(), call, CUSTOM_ID_YES);
		}
		break;
	}
}

void JoinGroupLayer::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIGang/UIGang_Join.ExportJson"));
	this->addChild(m_ui.pRoot);	
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");

	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			Director::getInstance()->replaceScene(MainCityScene::create());
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_GANG);
	_layerCommHeadInfo->setPosition(Vec2(-pos.x,pos.y));
	m_ui.pRoot->addChild(_layerCommHeadInfo);
	//主界面
	m_ui.ImgTips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Tips"));
	m_ui.Img_di1 = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_List"));
	m_ui.bTnFind = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Img_di1,"Button_45"));
	m_ui.bTnCreate = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Img_di1,"Btn_Create"));
	m_ui.bTnViewAll = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Img_di1,"Btn_AllList"));
	m_ui.bTnApplyList = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Img_di1,"Btn_ApplyList"));
	m_ui.bTnViewLeft = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Img_di1,"Btn_Left"));
	m_ui.bTnViewRight = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Img_di1,"Btn_Right"));

	m_ui.bTnFind->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	m_ui.bTnCreate->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	m_ui.bTnViewAll->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	m_ui.bTnApplyList->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	
	m_ui.labPage = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_di1,"Lab_Page"));
	m_ui.labPage->setFontName(FONT_FZZHENGHEI);
	for (int i = 0; i < GROUP_COUNT; i++)
	{
		m_ui.group[i] = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.Img_di1,"group"+TO_STR(i+1)));
	}

	//帮派徽记
	m_ui.Panel_Flag =dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIGang/UIGang_Flag.ExportJson"));
	this->addChild(m_ui.Panel_Flag);	
	m_ui.Panel_Flag->setSize(size);
	m_ui.Panel_Flag->setPosition(pos);
	m_ui.groupIcon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Panel_Flag,"Img_Flag"));
	m_ui.btnCloseIcon = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.groupIcon,"Btn_Close"));
	m_ui.iconScrollView = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.groupIcon,"ScrollView_Icon"));
	m_ui.btnCloseIcon->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	m_customScrollBar = CustomScrollBar::create();
	m_customScrollBar->initScrollBar(m_ui.iconScrollView->getContentSize().height);
	m_customScrollBar->setPosition(m_ui.iconScrollView->getPosition() + Point(-12, 0));
	m_ui.groupIcon->addChild(m_customScrollBar);
	m_customScrollBar->setScrollBarLengthPercent(1.0f);
	m_customScrollBar->setScrollBarPercentY(0.0f);
	m_ui.iconScrollView->addEventListener(CC_CALLBACK_2(JoinGroupLayer::onScrollViewEvent, this));

	//创建
	m_ui.Panel_create = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Panel_create"));
	m_ui.groupCreate = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Create"));
	m_ui.BtnCloseCreate = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.groupCreate,"Btn_Close"));
	m_ui.BtnModifyCreate = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.groupCreate,"Btn_Modify"));
	m_ui.BtnCreateEnter = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.groupCreate,"Btn_Create"));
	m_ui.BtnCloseCreate->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	m_ui.BtnModifyCreate->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	m_ui.BtnCreateEnter->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.groupCreate,"Lab_Cost"))->setFontName(FONT_FZZHENGHEI);
	
	TextField *inputName = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.groupCreate,"TextField_Name"));
	m_ui.inputName = EditBoxEx::create(inputName);
	m_ui.inputName->setPosition(inputName->getPosition());
	m_ui.inputName->setContentSize(inputName->getContentSize());
	inputName->getParent()->addChild(m_ui.inputName);
	m_ui.inputName->setMaxLength(30);
	m_ui.inputName->setText("");	m_ui.inputName->setFontName(SystemUtils::getFontFileName().c_str());

	m_ui.createPrize = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.groupCreate,"Lab_Price"));
	m_ui.createPrize->setFontName(FONT_FZZHENGHEI);
	m_ui.PnlCreateIcon = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.groupCreate,"Pnl_Icon"));

	//查找
	m_ui.Panel_Found = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Panel_Found"));
	m_ui.findGroup = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Found"));
	m_ui.btnCloseFind = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.findGroup,"Btn_Close"));
	m_ui.btnFind = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.findGroup,"Btn_Found"));
	m_ui.btnCloseFind->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	m_ui.btnFind->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	TextField *_inputName = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.findGroup,"TextField_Name"));
	m_ui.inputNameFind = EditBoxEx::create(_inputName);
// 	m_ui.inputNameFind->setFont(FONT_FZZHENGHEI,28);
	m_ui.inputNameFind->setPosition(_inputName->getPosition());
	m_ui.inputNameFind->setContentSize(_inputName->getContentSize());
	_inputName->getParent()->addChild(m_ui.inputNameFind);
	m_ui.inputNameFind->setMaxLength(30);
	m_ui.inputNameFind->setText("");
	m_ui.inputNameFind->setFontName(SystemUtils::getFontFileName().c_str());

	m_ui.Img_di1->setVisible(true);
	m_ui.ImgTips->setVisible(false);
	m_ui.Panel_Flag->setVisible(false);
	m_ui.Panel_create->setVisible(false);
	m_ui.Panel_Found->setVisible(false);
}

void JoinGroupLayer::initGroupInfoPanel()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.infoPanle = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIGang/UIGang_Details.ExportJson"));
	this->addChild(m_ui.infoPanle);	
	m_ui.infoPanle->setSize(size);
	m_ui.infoPanle->setPosition(pos);
	m_ui.infoPanle->setVisible(false);

	m_ui.infoPanledi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.infoPanle,"Img_di"));
	m_ui.infoPanleCloseLine = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.infoPanle,"Btn_Close"));
	auto call = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			m_ui.infoPanle->setVisible(false);
		}

	};
	m_ui.infoPanleCloseLine->addTouchEventListener(call);

	//头像
	m_ui.infoPanleFlag = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.infoPanledi,"Pnl_Flag"));
	m_ui.infoPanleGangLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.infoPanledi,"Lab_GangLevel"));
	m_ui.infoPanleGangLevel->setFontName(FONT_FZZHENGHEI);

	m_ui.infoPanleGangName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.infoPanledi,"Lab_GangName"));
	m_ui.infoPanleGangName->setFontName(FONT_FZZHENGHEI);

	m_ui.infoPanleWang = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.infoPanledi,"Lab_Wang"));
	m_ui.infoPanleWang->setFontName(FONT_FZZHENGHEI);

	m_ui.infoPanleManage = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.infoPanledi,"Lab_Manage"));
	m_ui.infoPanleManage->setFontName(FONT_FZZHENGHEI);

	m_ui.infoPanleID = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.infoPanledi,"Lab_ID"));
	m_ui.infoPanleID->setFontName(FONT_FZZHENGHEI);

	m_ui.infoPanleRank = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.infoPanledi,"Lab_Rank"));
	m_ui.infoPanleRank->setFontName(FONT_FZZHENGHEI);

	m_ui.infoPanlePublic = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.infoPanledi,"Lab_Public"));
	m_ui.infoPanlePublic->setFontName(FONT_FZZHENGHEI);
}

void JoinGroupLayer::onBtnNotify(Ref* sender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (sender == m_ui.bTnFind)//查找
		{
			m_ui.inputNameFind->setText("");
			m_ui.Panel_Found->setVisible(true);
			ActionCreator::runCommDlgAction(m_ui.findGroup);
		}
		else if (sender == m_ui.bTnCreate)//创建
		{
			m_ui.Panel_create->setVisible(true);
			ActionCreator::runCommDlgAction(m_ui.groupCreate);
			setCreateGroupFlag();
			m_ui.inputName->setText("");
			m_ui.createPrize->setString(TO_STR(JoinGroupModel::getInstance()->getCreateSpend()));
		}
		else if (sender == m_ui.bTnViewAll)//显示全部
		{
			m_findState = SEARCH_TYPE_ALL;
			m_curPage = 1;
			m_JoinGroupController.getGroupInfo();
		}
		else if (sender == m_ui.bTnApplyList)//正在申请
		{
			m_findState = SEARCH_TYPE_APPLY;
			m_curPage = 1;
			m_JoinGroupController.groupFind(m_findState);
		}
		else if (sender == m_ui.bTnViewLeft)//左翻页
		{
			m_curPage--;
			m_JoinGroupController.groupFind(m_findState,0,m_curPage);
		}
		else if (sender == m_ui.bTnViewRight)//右翻页
		{
			m_curPage++;
			m_JoinGroupController.groupFind(m_findState,0,m_curPage);
		}
		else if (sender == m_ui.btnCloseIcon) //徽记关闭
		{
			m_ui.Panel_Flag->setVisible(false);
		}
		else if (sender == m_ui.BtnCloseCreate) //创建关闭
		{
			m_ui.Panel_create->setVisible(false);
		}
		else if (sender == m_ui.BtnModifyCreate) //创建修改图标
		{
			m_ui.Panel_Flag->setVisible(true);
			ActionCreator::runCommDlgAction(m_ui.groupIcon);
			addFlagList();
		}
		else if (sender == m_ui.BtnCreateEnter) //确认创建
		{
			MainParam* pMainParam= MainModel::getInstance()->getMainParam();
			string inputName = m_ui.inputName->getText();
			if (pMainParam->mGold < JoinGroupModel::getInstance()->getCreateSpend())
			{
				CustomGoldTips *_tips = CustomGoldTips::create();
				if (_tips)
				{
					Director::getInstance()->getRunningScene()->addChild(_tips);
				}
				//充值界面
				return;
			}
			else if (inputName.length() == 0)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10169));
			}
			else if(StringFormat::getStringLenth(inputName) > NICK_NAME_LENGTH_LIMIT)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10170));
			}
			else
			{
				m_JoinGroupController.createGroup( m_ui.inputName->getText(),m_groupResId);
				m_ui.Panel_create->setVisible(false);
			}
		}
		else if (sender == m_ui.btnCloseFind) //查找关闭
		{
			m_ui.Panel_Found->setVisible(false);
		}
		else if (sender == m_ui.btnFind) //确认查找
		{
			string inputName = m_ui.inputNameFind->getText();
			if (inputName.length() == 0)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10171));
			}
			else if (!StringFormat::machGroupId(inputName.c_str()))
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10172));
			}
			else
			{
				m_findState = SEARCH_TYPE_ID;
				m_curPage = 1;
				m_JoinGroupController.groupFind(m_findState,atoi(inputName.c_str()));
				m_ui.Panel_Found->setVisible(false);
			}
		}
	}
}

void JoinGroupLayer::addGroupList()
{
	if (JoinGroupModel::getInstance()->getTotalNum() <= 0 )
	{
		switch (m_findState)
		{
		case SEARCH_TYPE_ALL:
			m_ui.ImgTips->setVisible(true);

			break;
		case SEARCH_TYPE_APPLY:
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10173));
			break;
		case SEARCH_TYPE_ID:
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10174));
			break;
		default:
			break;
		}

		//清空
		for (int i = 0; i < GROUP_COUNT; i++)
		{
			m_ui.group[i]->removeAllChildren();
		}

		m_ui.labPage->setString(TO_STR(0) + "/" +TO_STR(0));
		setTextAddStroke(m_ui.labPage,Color3B(0x80,0x0c,0x00),2);

		m_ui.bTnViewLeft->setBright(false);
		m_ui.bTnViewRight->setBright(false);
		return;
	}
	else
	{
		m_ui.ImgTips->setVisible(false);
	}

	int maxPage = ceil(JoinGroupModel::getInstance()->getTotalNum()/4.0f);
	m_ui.labPage->setString(TO_STR(m_curPage) + "/" +TO_STR(maxPage));
	setTextAddStroke(m_ui.labPage,Color3B(0x80,0x0c,0x00),2);

	if (m_curPage <= 1)
	{
		m_ui.bTnViewLeft->setBright(false);
		m_ui.bTnViewLeft->addTouchEventListener(nullptr);
	}
	else
	{
		m_ui.bTnViewLeft->setBright(true);
		m_ui.bTnViewLeft->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	}

	if (m_curPage >= maxPage)
	{
		m_ui.bTnViewRight->setBright(false);
		m_ui.bTnViewRight->addTouchEventListener(nullptr);
	}
	else
	{
		m_ui.bTnViewRight->setBright(true);
		m_ui.bTnViewRight->addTouchEventListener(CC_CALLBACK_2(JoinGroupLayer::onBtnNotify,this));
	}

	//清空
	for (int i = 0; i < GROUP_COUNT; i++)
	{
		m_ui.group[i]->removeAllChildren();
	}
	
	for (int i = 0; i < GROUP_COUNT; i++)
	{
		if (i < JoinGroupModel::getInstance()->getGroupInfo().size())
		{
			GROUP_INFO groupInfo = JoinGroupModel::getInstance()->getGroupInfo().at(i);
			string strFileName;
			strFileName.assign("Image/UIGang/UIGang_GangList.ExportJson");
			Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

			Layout *Pnl_GangList =  dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_GangList"));
			ImageView *Img_Rank = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_Rank"));
			Text *rank = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_Rank,"rank"));
			Text *Lab_GangName = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_GangName"));
			Text *Lab_GangLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_GangLevel"));
			Text *Lab_Count = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Count"));
			Text *Lab_JoinLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_JoinLevel"));
			Text *Lab_Limit = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Limit"));
			Button *Btn_Apply = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Apply"));
			Button *Btn_Cancel = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Cancel"));
			Layout *Pnl_Flag = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_Flag"));
			rank->setFontName(FONT_FZZHENGHEI);
			Lab_GangName->setFontName(FONT_FZZHENGHEI);
			Lab_GangLevel->setFontName(FONT_FZZHENGHEI);
			Lab_Count->setFontName(FONT_FZZHENGHEI);
			Lab_JoinLevel->setFontName(FONT_FZZHENGHEI);
			Lab_Limit->setFontName(FONT_FZZHENGHEI);

			//排名
			if (groupInfo.orderNum <= 3 && groupInfo.orderNum > 0)
			{
				rank->setVisible(false);
				Img_Rank->loadTexture("Image/Icon/global/img_" + TO_STR(groupInfo.orderNum) + "ming.png");
			}
			else
			{
				rank->setVisible(true);
				rank->setString(TO_STR(groupInfo.orderNum));
				setTextAddStroke(rank,Color3B(0x00,0x00,0x00),2);
			}
			//帮派名
			Lab_GangName->setString(groupInfo.name);
			setTextAddStroke(Lab_GangName,Color3B(0x7d,0x3f,0x1c),2);

			//帮派等级
			Lab_GangLevel->setString(TO_STR(groupInfo.level));
			setTextAddStroke(Lab_GangLevel,Color3B(0x7d,0x3f,0x1c),2);

			//数量
			if (groupInfo.curNum < groupInfo.maxNum)
			{
				Lab_Count->setString(TO_STR(groupInfo.curNum) + "/" + TO_STR(groupInfo.maxNum));
				Lab_Count->setColor(Color3B(0xe8,0xde,0xb5));
				setTextAddStroke(Lab_Count,Color3B(0x7d,0x3f,0x1c),2);
			}
			else
			{
				Lab_Count->setString(TO_STR(groupInfo.curNum) + "/" + TO_STR(groupInfo.maxNum));
				Lab_Count->setColor(Color3B(0xff,0x37,0x28));
				setTextAddStroke(Lab_Count,Color3B(0x5a,0x07,0x07),2);
			}
			//加入等级
			Lab_JoinLevel->setString(TO_STR(groupInfo.needLvl)+DataManager::getInstance()->searchCommonTexdtById(10175));
			setTextAddStroke(Lab_JoinLevel,Color3B(0x7d,0x3f,0x1c),2);

			//是否需要审核
			if (groupInfo.checkFlag)
			{
				Lab_Limit->setString(DataManager::getInstance()->searchCommonTexdtById(10176));
			}
			else
			{
				Lab_Limit->setString(DataManager::getInstance()->searchCommonTexdtById(10177));
			}
			setTextAddStroke(Lab_Limit,Color3B(0x7d,0x3f,0x1c),2);

			//旗帜
			stringstream ss;
			RowData *flagData = DataManager::getInstance()->searchEmblemById(groupInfo.resId);
			ss << "Image/Icon/Flag/" << flagData->getStringData("resId") << ".png";
			ImageView *imgFlag = ImageView::create(ss.str());
			imgFlag->setAnchorPoint(Vec2(0,0));
			imgFlag->setScale(0.35f);
			Pnl_Flag->addChild(imgFlag);

			vector<int>& ApplyId = JoinGroupModel::getInstance()->getApplyId();
			vector<int>::iterator result = find( ApplyId.begin( ), ApplyId.end( ), groupInfo.id); //查找3
			if ( result == ApplyId.end( ) ) //没找到 能领取
			{
				Btn_Cancel->setVisible(false);
				Btn_Apply->setVisible(true);
			}
			else
			{
				Btn_Cancel->setVisible(true);
				Btn_Apply->setVisible(false);
			}

			//确认
			auto callEnter = [this,groupInfo,i](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					if (JoinGroupModel::getInstance()->getApplyId().size() >= GROUP_APPLY_COUNT)
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10178) + TO_STR(GROUP_APPLY_COUNT)
							+ DataManager::getInstance()->searchCommonTexdtById(10179));
					}
					else if (groupInfo.curNum >= groupInfo.maxNum)
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10180));
					}
					else if (HeroModel::getInstance()->getMainHeroLevel() < groupInfo.level)
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10181));
					}
					else
					{
						m_chooseApplyCancleId = i;
						m_JoinGroupController.applyJoinGroup(groupInfo.id);
					}
				}
			};
			Btn_Apply->addTouchEventListener(callEnter);

			//取消
			auto callCancle = [this,groupInfo,i](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					m_chooseApplyCancleId = i;
					m_JoinGroupController.cancleApplyGroup(groupInfo.id);
				}
			};
			Btn_Cancel->addTouchEventListener(callCancle);


			auto callGroupInfo = [this,groupInfo](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					viewGroupInfo(groupInfo);
				}
			};
			Pnl_GangList->addTouchEventListener(callGroupInfo);
			m_ui.group[i]->addChild(layer,0,"UIGang_GangList");
		}
	}
}

void JoinGroupLayer::setApplyCancleNodeState(bool isAplly)
{
	if (m_ui.group[m_chooseApplyCancleId] && m_ui.group[m_chooseApplyCancleId]->getChildByName("UIGang_GangList"))
	{
		Layout* layer =  dynamic_cast<Layout*>(m_ui.group[m_chooseApplyCancleId]->getChildByName("UIGang_GangList"));
		Button *Btn_Apply = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Apply"));
		Button *Btn_Cancel = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Cancel"));

		if (isAplly)
		{
			Btn_Apply->setVisible(false);
			Btn_Cancel->setVisible(true);
		}
		else
		{
			Btn_Apply->setVisible(true);
			Btn_Cancel->setVisible(false);
		}
	}
}

void JoinGroupLayer::addFlagList()
{
	DataTable* flagDT = DataManager::getInstance()->getTableEmblem();
	int row = ceil(flagDT->getTableData().size()/4.0f);
	int scrollViewHeight = row * 130+10;
	scrollViewHeight = scrollViewHeight >  m_ui.iconScrollView->getContentSize().height ? scrollViewHeight : m_ui.iconScrollView->getContentSize().height;
	int scrollViewWidth = m_ui.iconScrollView->getInnerContainerSize().width;
	m_ui.iconScrollView->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	//添加奖励面板控件
	m_ui.iconScrollView->removeAllChildren();
	for (int i = 0; i < flagDT->getTableData().size(); i++)
	{
		int rowNumber = i/4;
		int colNumber = i%4;

		stringstream ss;
		string resId = flagDT->searchDataById(i+1)->getStringData("resId");
		ss << "Image/Icon/Flag/" << resId << ".png";

		Button* item = Button::create(ss.str());
		if (i+1 == m_groupResId)
		{
			ImageView *imgDi = ImageView::create("Image/Icon/Flag/img_di7.png");
			imgDi->setAnchorPoint(Vec2(0.15f,0.20f));
			imgDi->setPosition(Vec2(colNumber*130 + 20, scrollViewHeight-(rowNumber+1)*130+10 ));
			m_ui.iconScrollView->addChild(imgDi);

			item->setAnchorPoint(Vec2(0,0));
			item->setScale(0.5f);
			item->setPosition(Vec2(25,30));
			imgDi->addChild(item);
		}
		else
		{
			item->setAnchorPoint(Vec2(0,0));
			item->setScale(0.5f);
			item->setPosition(Vec2(colNumber*130 + 20, scrollViewHeight-(rowNumber+1)*130+10 ));
			m_ui.iconScrollView->addChild(item);
		}

		auto call = [this,i](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (type == Widget::TouchEventType::ENDED)
			{
				m_ui.Panel_Flag->setVisible(false);

				m_groupResId = i+1;
				setCreateGroupFlag(m_groupResId);
			}
		};
		item->addTouchEventListener(call);
	}

	m_customScrollBar->setScrollBarLengthPercent(
		m_ui.iconScrollView->getContentSize().height / m_ui.iconScrollView->getInnerContainerSize().height);
	m_customScrollBar->setScrollBarPercentY(0.0f);
	m_ui.iconScrollView->jumpToTop();
}

void JoinGroupLayer::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	switch(type)
	{
	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			if (pSender == m_ui.iconScrollView)
			{
				Size size = m_ui.iconScrollView->getInnerContainerSize();
				Vec2 vec = m_ui.iconScrollView->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.iconScrollView->getContentSize().height);

				m_customScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
		}

		break;
	default:
		break;
	}
}

void JoinGroupLayer::setCreateGroupFlag(int id)
{
	m_ui.PnlCreateIcon->removeAllChildren();
	RowData *flagData = DataManager::getInstance()->searchEmblemById(id);
	stringstream ss;
	string resId = flagData->getStringData("resId");
	ss << "Image/Icon/Flag/" << resId << ".png";

	ImageView *imgFlag = ImageView::create(ss.str());
	imgFlag->setAnchorPoint(Vec2(0,0));
	imgFlag->setScale(0.5f);
	m_ui.PnlCreateIcon->addChild(imgFlag);
}

void JoinGroupLayer::viewGroupInfo(GROUP_INFO info)
{
	m_ui.infoPanle->setVisible(true);
	ActionCreator::runCommDlgAction(m_ui.infoPanledi);

	//头像
	//如果有头像需先移除
	if(m_ui.infoPanleFlag->getChildByName("ImageFlag") != nullptr)
	{
		m_ui.infoPanleFlag->removeChildByName("ImageFlag");
	}
	RowData *flagData = DataManager::getInstance()->searchEmblemById(info.resId);
	stringstream ss;
	string resId = flagData->getStringData("resId");
	ss << "Image/Icon/Flag/" << resId << ".png";
	ImageView *imgFlag = ImageView::create(ss.str());
	imgFlag->setAnchorPoint(Vec2(0.2,0.2));
	imgFlag->setName("ImageFlag");
	imgFlag->setScale(1.0f);
	m_ui.infoPanleFlag->addChild(imgFlag);

	m_ui.infoPanleGangLevel->setString("Lv" + TO_STR(info.level));
	setTextAddStroke(m_ui.infoPanleGangLevel,Color3B(0x7d,0x3f,0x1c),2);

	m_ui.infoPanleGangName->setString(info.name);
	setTextAddStroke(m_ui.infoPanleGangName,Color3B(0x7d,0x3f,0x1c),2);

	//等级位置跟着帮派名称走
	m_ui.infoPanleGangLevel->setPositionX(m_ui.infoPanleGangName->getPositionX() + m_ui.infoPanleGangName->getContentSize().width + 10.0f);

	m_ui.infoPanleWang->setString(info.leader);
	setTextAddStroke(m_ui.infoPanleWang,Color3B(0x5a,0x07,0x07),2);

	m_ui.infoPanleManage->setString(TO_STR(info.curNum) + "/" + TO_STR(info.maxNum));
	setTextAddStroke(m_ui.infoPanleManage,Color3B(0x5a,0x07,0x07),2);

	m_ui.infoPanleID->setString(TO_STR(info.id));
	setTextAddStroke(m_ui.infoPanleID,Color3B(0x5a,0x07,0x07),2);

	m_ui.infoPanleRank->setString(TO_STR(info.orderNum));
	setTextAddStroke(m_ui.infoPanleRank,Color3B(0x5a,0x07,0x07),2);

	m_ui.infoPanlePublic->setString(info.remark2);
	setTextAddStroke(m_ui.infoPanlePublic,Color3B(0x5a,0x07,0x07),2);
}