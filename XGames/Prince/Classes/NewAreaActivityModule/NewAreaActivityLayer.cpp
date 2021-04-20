#include "NewAreaActivityLayer.h"
#include "NewAreaActivityModel.h"
#include "MainModule/MainCityScene.h"
#include "Widget/LayerCommHeadInfo.h"
#include "Common/Common.h"
#include "ChapterModule/SceneChapter.h"
#include "ToolModule/SceneTool.h"
#include "PvpModule/PvpScene.h"
#include "LoveModule/LoveScene.h"
#include "TowerModule/SceneTower.h"
#include "RobModule/SceneRob.h"
#include "FriendModule/SceneFriend.h"
#include "MazeModule/SceneMaze.h"
#include "TakeCardModule/SceneTakeCard.h"
#include "TacticsModule/SceneTactics.h"
#include "StarsSuModule/StarsSuScene.h"
#include "BossModule/BossScene.h"
#include "NewStarsCampModule/StarsCampScene.h"
#include "HeroModule/SceneHero.h"
#include "HeroModule/SceneHeroFate.h"
#include "Temp/CustomGoldTips.h"
#include "MazeModule/MazeModel.h"
#include "TakeCardModule/TakeCardModel.h"
#include "ArtifactModule/ArtifactScene.h"
//滚动列表子项之间的间距
#define GAP_ITEM_LIST		10.0f
#define REWARD_TOOL_TAG		1001
#define GOLD_ITEM_ID		11001
#define COIN_ITEM_ID		11002


NewAreaActivityLayer::NewAreaActivityLayer()
{
	m_vBtnLabel.clear();
	m_vBtnDays.clear();
	m_vItemList.clear();
	m_iCurSelectedDay = 0;
	m_iResTime = 0;
	m_iCurSelectedLabel = 0;
	m_iSelectedId = 0;
	m_iLeaveBtnId = -1;
	m_iLeaveLabId = -1;
	m_pnlGettedRewardItem = nullptr;
	NewAreaActivityModel::getInstance()->addObserver(this);
}

NewAreaActivityLayer::~NewAreaActivityLayer()
{
	m_BarActItemList = nullptr;
	if(m_pnlGettedRewardItem)
	{
		m_pnlGettedRewardItem->release();
	}
	NewAreaActivityModel::getInstance()->removeObserver(this);
}

bool NewAreaActivityLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void NewAreaActivityLayer::onEnter()
{
	Layer::onEnter();

	auto callBack = [this](float dt)->void
	{
		NewAreaActivityController::getInstance()->sendGetMainInfoMsg();			
		this->getScheduler()->unschedule("NewAreaActivityLayer::sendMsg", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "NewAreaActivityLayer::sendMsg");
}

void NewAreaActivityLayer::onExit()
{
	Layer::onExit();
	LocalTimer::getInstance()->removeLocalTimerListener(this);
	m_iLeaveBtnId = m_iCurSelectedDay;
	m_iLeaveLabId = m_iCurSelectedLabel;
}

void NewAreaActivityLayer::initUI()
{
	//加载新服活动主界面
	string strFileName;
	strFileName.assign("Image/UINewActivity/UINewActivity.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_ui.pRoot); 

	//公用返回
	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");
			scheduleUpdate();		
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_NEWAREA_ACT);
	m_ui.pRoot->addChild(_layerCommHeadInfo);

	//活动倒计时
	m_ui.txtActTime = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Time"));
	m_ui.txtActTime->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtActTime, Color3B(0x09, 0x4e, 0x51), 2);

	//四个标签按钮
	for(int i=0; i<4; i++)
	{
		Button* btn = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, string("Btn_Label")+_TO_STR(i+1)));
		btn->addTouchEventListener(CC_CALLBACK_2(NewAreaActivityLayer::onBtnClicked, this, Btn_pnl1+i));
		Text* txt = (Text*)btn->getChildByName("Lab_name");
		txt->setFontName(FONT_FZZHENGHEI);
		m_vBtnLabel.push_back(btn);
	}

	//七个活动天数按钮
	for(int i=0; i<7; i++)
	{
		Button* btn = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, string("Btn_Day")+_TO_STR(i+1)));
		btn->addTouchEventListener(CC_CALLBACK_2(NewAreaActivityLayer::onBtnClicked, this, Btn_Day1+i));
		m_vBtnDays.push_back(btn);
	}

	//滚动面板
	m_ui.pnlScrollView = (ScrollView*)(Helper::seekWidgetByName(m_ui.pRoot, "ScrollView_Act"));
	m_ui.pnlScrollView->addEventListener(CC_CALLBACK_2(NewAreaActivityLayer::onScrollViewEvent, this));
	//列表子项
	m_ui.pnlItem = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Reward"));
	m_ui.pnlItem->retain();
	m_ui.pnlItem->setVisible(false);

	//滚动条
	m_BarActItemList = CustomScrollBar::create();
	m_BarActItemList->initScrollBar(m_ui.pnlScrollView->getContentSize().height);
	m_BarActItemList->setPosition(m_ui.pnlScrollView->getPosition() + Point(-2, 0));
	m_ui.pnlScrollView->getParent()->addChild(m_BarActItemList);
	m_BarActItemList->setScrollBarLengthPercent(1.0f);
	m_BarActItemList->setScrollBarPercentY(0.0f);

	//初始化特价抢购面板
	initBuyPropView();
}

void NewAreaActivityLayer::initBuyPropView()
{
	m_ui.imgBuyProp = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_FlashSale"));
	m_ui.pnlProp = (Layout*)m_ui.imgBuyProp->getChildByName("Pnl_Item");
	m_ui.txtPropName = (Text*)m_ui.imgBuyProp->getChildByName("Lab_ItemName");
	m_ui.txtPropName->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtPropName, Color3B(0x79, 0x03, 0x03), 3);
	m_ui.txtOldPrice = (Text*)m_ui.imgBuyProp->getChildByName("Lab_CostPrice");
	m_ui.txtOldPrice->setFontName(FONT_FZZHENGHEI);
	m_ui.txtNewPrice = (Text*)m_ui.imgBuyProp->getChildByName("Lab_CurrentPrice");
	m_ui.txtNewPrice->setFontName(FONT_FZZHENGHEI);
	m_ui.btnBuy = (Button*)m_ui.imgBuyProp->getChildByName("Btn_Buy");
	m_ui.btnBuy->addTouchEventListener(CC_CALLBACK_2(NewAreaActivityLayer::onBtnClicked, this, Btn_Buy));
	m_ui.imgBought = (ImageView*)m_ui.imgBuyProp->getChildByName("Img_bought");
	m_ui.imgBought->setVisible(false);
	m_ui.txtLimitAmount = (Text*)m_ui.imgBuyProp->getChildByName("Lab_Limit1");
	m_ui.txtLimitAmount->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtLimitAmount, Color3B(0x54, 0x14, 0x00), 2);
	m_ui.txtLimitSurplus = (Text*)m_ui.imgBuyProp->getChildByName("Lab_Limit2");
	m_ui.txtLimitSurplus->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtLimitSurplus, Color3B(0x54, 0x14, 0x00), 2);
	//其它一些描边
	Text* txt1 = (Text*)m_ui.imgBuyProp->getChildByName("Lab_LimitTips1");
	txt1->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(txt1, Color3B(0x45, 0x14, 0x00), 2);
	Text* txt2 = (Text*)m_ui.imgBuyProp->getChildByName("Lab_LimitTips2");
	txt2->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(txt2, Color3B(0x45, 0x14, 0x00), 2);
	Text* txt3 = (Text*)m_ui.imgBuyProp->getChildByName("Lab_LimitTips3");
	txt3->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(txt3, Color3B(0x45, 0x14, 0x00), 2);
}

void NewAreaActivityLayer::showRedFlag()
{
	NewAreaActivityModel* pInstance = NewAreaActivityModel::getInstance();
	//7天
	for(int i=0; i<7; i++)
	{
		m_vBtnDays.at(i)->getChildByName("Img_GetFlagByDay")->setVisible(false);
		for(int j=0; j<pInstance->m_vecNewAreaActData.size(); j++)
		{
			if(pInstance->m_vecNewAreaActData.at(j).isCanGet &&
			pInstance->m_vecNewAreaActData.at(j).actOpenTime == i+1 && 
			pInstance->m_vecNewAreaActData.at(j).actOpenTime <= pInstance->m_iCurActDay)
			{
				m_vBtnDays.at(i)->getChildByName("Img_GetFlagByDay")->setVisible(true);
				break;
			}
		}
	}
}

void NewAreaActivityLayer::updateScrollView(int nWidgetName)
{
	m_ui.pnlScrollView->jumpToTop();
	NewAreaActivityModel* pInstance = NewAreaActivityModel::getInstance();
	vector<sActItem> vecGetted;
	vector<sActItem> vecWaitGetted;
	vecGetted.clear();
	vecWaitGetted.clear();
	m_ui.pnlScrollView->removeAllChildren();
	for(int i=0; i<m_vBtnLabel.size(); i++)
	{
		m_vBtnLabel.at(i)->getChildByName("Img_RedDot")->setVisible(false);
	}
	for(int i=0; i<pInstance->m_vecNewAreaActData.size(); i++)
	{
		//显示标签上的红点
		if(pInstance->m_vecNewAreaActData.at(i).actOpenTime == m_iCurSelectedDay+1 && m_iCurSelectedDay < pInstance->m_iCurActDay)
		{
			if(pInstance->m_vecNewAreaActData.at(i).isCanGet && 
				m_vBtnLabel.at(pInstance->m_vecNewAreaActData.at(i).actShow-1)->getChildByName("Img_RedDot")->isVisible() == false)
			{
				m_vBtnLabel.at(pInstance->m_vecNewAreaActData.at(i).actShow-1)->getChildByName("Img_RedDot")->setVisible(true);
			}
		}
		if(pInstance->m_vecNewAreaActData.at(i).actShow == nWidgetName &&
			pInstance->m_vecNewAreaActData.at(i).actOpenTime == m_iCurSelectedDay+1)
		{
			sActItem item;
			item.vecPnlProp.clear();
			item.pnlItem = (Layout*)m_ui.pnlItem->clone();
			item.taskContent = (Text*)item.pnlItem->getChildByName("Img_TaskTipsBg")->getChildByName("Lab_TaskTips");
			item.taskGoal = (Text*)item.pnlItem->getChildByName("Img_TaskTipsBg")->getChildByName("Lab_Task");
			for(int m=0; m<3; m++)
			{
				Layout* pnlProp = (Layout*)item.pnlItem->getChildByName(string("pnl_prop")+_TO_STR(m+1));
				item.vecPnlProp.push_back(pnlProp);
			}
			item.btnGet = (Button*)item.pnlItem->getChildByName("Btn_Get");
			item.btnGet->addTouchEventListener(CC_CALLBACK_2(NewAreaActivityLayer::onBtnClicked, this, Btn_Get));
			item.btnGo = (Button*)item.pnlItem->getChildByName("Btn_Go");
			item.btnGo->addTouchEventListener(CC_CALLBACK_2(NewAreaActivityLayer::onBtnClicked, this, Btn_Go));
			item.imgGettedFlag = (ImageView*)item.pnlItem->getChildByName("Img_GetFlag");

			item.isGetted = false;
			item.isCanGet = false;
			item.rewardId = pInstance->m_vecNewAreaActData.at(i).actId;
			Text* txtRewardId = Text::create();
			txtRewardId->setString(_TO_STR(item.rewardId));
			txtRewardId->setName("rewardId");
			txtRewardId->setVisible(false);
			item.pnlItem->addChild(txtRewardId);							//记录一下奖励ID，方便点击领取时发给服务器

			item.taskContent->setFontName(FONT_FZZHENGHEI);
			item.taskGoal->setFontName(FONT_FZZHENGHEI);

			item.taskContent->setString(pInstance->m_vecNewAreaActData.at(i).actDes);
			setTextAddStroke(item.taskContent, Color3B(0x9e, 0x0f, 0x0f), 2);
			if(pInstance->m_vecNewAreaActData.at(i).taskType == 8)
			{
				//竞技场不显示
				//item.taskGoal->setString(string("(")+ _TO_STR(pInstance->m_vecNewAreaActData.at(i).amount1.done) + string(")"));
				item.taskGoal->setString("");
			}
			else if(pInstance->m_vecNewAreaActData.at(i).taskType == 22)
			{
				//如果任务类型是战力值，则需要格式化成万
				string curPower = StringFormat::formatNumber(pInstance->m_vecNewAreaActData.at(i).amount1.done, false);
				string goalPower = StringFormat::formatNumber(pInstance->m_vecNewAreaActData.at(i).amount1.cnt, false);
				item.taskGoal->setString(curPower + "/" + goalPower);
			}
			else
			{
				item.taskGoal->setString(string("(")+ _TO_STR(pInstance->m_vecNewAreaActData.at(i).amount1.done) + string("/") +
					_TO_STR(pInstance->m_vecNewAreaActData.at(i).amount1.cnt) + string(")"));
			}
			
			if(pInstance->m_vecNewAreaActData.at(i).amount1.done < pInstance->m_vecNewAreaActData.at(i).amount1.cnt)
			{
				item.taskGoal->setColor(Color3B::WHITE);
			}
			else
			{
				item.taskGoal->setColor(Color3B(0x54, 0xdd, 0x51));
			}
			for(int j=0; j<pInstance->m_vecNewAreaActData.at(i).vecPropInfo.size(); j++)
			{
				int propId = pInstance->m_vecNewAreaActData.at(i).vecPropInfo.at(j).propId;
				int propCnt = pInstance->m_vecNewAreaActData.at(i).vecPropInfo.at(j).propCnt;
				UIToolHeader* tool = UIToolHeader::create(propId);
				tool->setToolId(propId);
				tool->setNumEx(propCnt);
				tool->setTipsEnabled(true);
				tool->setAnchorPoint(Point(0, 0));
				tool->setScale(0.9);
				item.vecPnlProp.at(j)->addChild(tool, 1, REWARD_TOOL_TAG);
			}
			if(pInstance->m_vecNewAreaActData.at(i).isCanGet && m_iCurSelectedDay < pInstance->m_iCurActDay)
			{
				//可以领取
				item.btnGet->setVisible(true);
				item.btnGet->setBright(true);
				item.btnGet->setEnabled(true);
				item.btnGo->setVisible(false);
				item.imgGettedFlag->setVisible(false);	
				item.isCanGet = true;
			}
			else if(pInstance->m_vecNewAreaActData.at(i).isGetted)
			{
				//已领取
				item.btnGet->setVisible(false);
				item.btnGo->setVisible(false);
				item.imgGettedFlag->setVisible(true);
				item.isGetted = true;
				//item.taskGoal->setString(STR_UTF8("已完成"));
				item.taskGoal->setString("");
			}
			else
			{
				//活动奖励未激活
				item.imgGettedFlag->setVisible(false);
				if(pInstance->m_vecNewAreaActData.at(i).skipType == -1)
				{
					//无可前往，显示灰色领取
					item.btnGet->setVisible(true);
					item.btnGet->setEnabled(false);
					item.btnGet->setBright(false);
					item.btnGo->setVisible(false);
				}
				else
				{
					//显示前往，跳转按钮
					item.btnGet->setVisible(false);
					item.btnGet->setEnabled(false);
					item.btnGet->setPosition(Point(-1000, -1000));
					item.btnGo->setVisible(true);
					item.btnGo->setBright(true);
					item.btnGo->setEnabled(true);
				}
			}	
			if(item.isGetted)
			{
				vecGetted.push_back(item);
			}
			else
			{
				vecWaitGetted.push_back(item);
			}
		}
	}
	
	//按照ID把已领取和未领取的作排序
	std::sort(vecGetted.begin(), vecGetted.end(), 
		[](const sActItem& p1, const sActItem& p2)->bool
	{
		return p1.rewardId < p2.rewardId;
	});
	std::sort(vecWaitGetted.begin(), vecWaitGetted.end(), 
		[](const sActItem& p1, const sActItem& p2)->bool
	{		
		//可领取的放前面
		if(p1.isCanGet)
		{
			if(p2.isCanGet)
			{
				return p1.rewardId < p2.rewardId;
			}
			else
			{
				return true;
			}
		}
		else
		{
			if(p2.isCanGet)
			{
				return false;
			}
			else
			{
				return p1.rewardId < p2.rewardId;
			}
		}		
	});

	//未领取的放前面，已领取的放后面
	m_vItemList = vecWaitGetted;
	for(int i=0; i<vecGetted.size(); i++)
	{
		m_vItemList.push_back(vecGetted.at(i));
	}

	//设置滚动面板大小
	Size size(0, 0);
	size = m_ui.pnlScrollView->getContentSize();
	if(m_vItemList.size() >= 3)
	{
		size.height = (m_ui.pnlItem->getContentSize().height + GAP_ITEM_LIST)*m_vItemList.size();
	}
	m_ui.pnlScrollView->setInnerContainerSize(size);

	//把列表放入滚动层中
	for(int i=0; i<m_vItemList.size(); i++)
	{
		m_ui.pnlScrollView->addChild(m_vItemList.at(i).pnlItem);
		m_vItemList.at(i).pnlItem->setVisible(true);
		m_vItemList.at(i).pnlItem->setAnchorPoint(Point(0, 1));
		m_vItemList.at(i).pnlItem->setPositionX(m_ui.pnlItem->getPositionX());
		if(i==0)
		{
			//第一条
			m_vItemList.at(i).pnlItem->setPositionY(m_ui.pnlScrollView->getInnerContainerSize().height - 10);
		}
		else
		{
			m_vItemList.at(i).pnlItem->setPositionY(m_ui.pnlScrollView->getInnerContainerSize().height - 10 - 
				i*(m_ui.pnlItem->getContentSize().height + GAP_ITEM_LIST));
		}		
	}	

	m_BarActItemList->setScrollBarLengthPercent(m_ui.pnlScrollView->getContentSize().height/m_ui.pnlScrollView->getInnerContainerSize().height);
	m_BarActItemList->setScrollBarPercentY(0);
}

void NewAreaActivityLayer::updateBtnLabel()
{
	NewAreaActivityModel* pInstance = NewAreaActivityModel::getInstance();

	for(int j=0; j<m_vBtnLabel.size(); j++)
	{
		for(int m=0; m<pInstance->m_vecNewAreaActData.size(); m++)
		{
			if(pInstance->m_vecNewAreaActData.at(m).actOpenTime == m_iCurSelectedDay+1 &&
				pInstance->m_vecNewAreaActData.at(m).actShow == j+1)
			{
				Text* txtName = (Text*)m_vBtnLabel.at(j)->getChildByName("Lab_name");
				txtName->setString(pInstance->m_vecNewAreaActData.at(m).actName);
				txtName->setFontSize(30);
				setTextAddStroke(txtName, Color3B(0x94, 0x6f, 0x17), 1);
				if(m_vBtnLabel.at(j)->isHighlighted())
				{
					txtName->setFontSize(33);
					setTextAddStroke(txtName, Color3B(0xff, 0xb4, 0x00), 1);
				}
				break;
			}
		}			
	}
}

void NewAreaActivityLayer::updateBtnLabelHighLight(Button* btn)
{
	for(int i=0; i<m_vBtnLabel.size(); i++)
	{
		Text* txtName = (Text*)m_vBtnLabel.at(i)->getChildByName("Lab_name");
		txtName->setFontSize(30);
		setTextAddStroke(txtName, Color3B(0x94, 0x6f, 0x17), 1);
	}
	Text* txtSeledName = (Text*)btn->getChildByName("Lab_name");
	txtSeledName->setFontSize(33);
	setTextAddStroke(txtSeledName, Color3B(0xff, 0xb4, 0x00), 1);
}

void NewAreaActivityLayer::updateBuyView()
{
	NewAreaActivityModel* pInstance = NewAreaActivityModel::getInstance();
	for(int i=0; i<pInstance->m_vecNewAreaActData.size(); i++)
	{
		if(pInstance->m_vecNewAreaActData.at(i).actShow == Btn_pnl4 && 
			pInstance->m_vecNewAreaActData.at(i).actOpenTime == m_iCurSelectedDay+1)
		{
			RowData* rowData = DataManager::getInstance()->getDTTool()->searchDataById(pInstance->m_vecNewAreaActData.at(i).amount1.id);
			UIToolHeader* tool = UIToolHeader::create(pInstance->m_vecNewAreaActData.at(i).amount1.id);
			tool->setToolId(pInstance->m_vecNewAreaActData.at(i).amount1.id);
			tool->setNumEx(pInstance->m_vecNewAreaActData.at(i).amount1.cnt);
			tool->setAnchorPoint(Point(0, 0));
			tool->setTipsEnabled(true);
			m_ui.pnlProp->removeAllChildren();
			m_ui.pnlProp->addChild(tool);
			//物品名称
			m_ui.txtPropName->setString(rowData->getStringData("itemName"));
			setTextAddStroke(m_ui.txtPropName, Color3B(0x79, 0x03, 0x03), 3);
			//原价、特价
			m_ui.txtOldPrice->setString(_TO_STR(pInstance->m_vecNewAreaActData.at(i).oldPrice));
			m_ui.txtNewPrice->setString(_TO_STR(pInstance->m_vecNewAreaActData.at(i).newPrice));
			//限购文本
			m_ui.txtLimitAmount->setString(_TO_STR(pInstance->m_vecNewAreaActData.at(i).amount2));
			setTextAddStroke(m_ui.txtLimitAmount, Color3B(0x54, 0x14, 0x00), 2);
			//剩余限购个数
			m_ui.txtLimitSurplus->setString(_TO_STR(pInstance->m_vecNewAreaActData.at(i).amount3));
			setTextAddStroke(m_ui.txtLimitSurplus, Color3B(0x54, 0x14, 0x00), 2);
			//如果已经购买了则按钮显示已购买
			if(pInstance->m_vecNewAreaActData.at(i).isGetted)
			{
				m_ui.btnBuy->setVisible(false);
				m_ui.imgBought->setVisible(true);
				m_vBtnLabel.at(3)->getChildByName("Img_RedDot")->setVisible(false);
			}
			else
			{
				m_ui.btnBuy->setVisible(true);
				m_ui.imgBought->setVisible(false);
				if(m_iCurSelectedDay+1 <= pInstance->m_iCurActDay && pInstance->m_vecNewAreaActData.at(i).isCanGet)
				{
					m_vBtnLabel.at(3)->getChildByName("Img_RedDot")->setVisible(true);
				}
				else
				{
					m_vBtnLabel.at(3)->getChildByName("Img_RedDot")->setVisible(false);
				}
			}
			break;
		}
	}			
}

void NewAreaActivityLayer::updatePnlAct(int nWidgetName)
{	
	//由面板标签触发刷新面板
	if(nWidgetName <= Btn_pnl4)
	{
		//特价抢购		
		if(nWidgetName == Btn_pnl4)
		{
			m_ui.pnlScrollView->setVisible(false);
			m_BarActItemList->setVisible(false);
			m_ui.imgBuyProp->setVisible(true);
			updateBuyView();
		}
		else
		{
			//滚动列表面板
			m_ui.pnlScrollView->setVisible(true);
			m_BarActItemList->setVisible(true);
			m_ui.imgBuyProp->setVisible(false);
			updateScrollView(nWidgetName);		
		}
	}
	else
	{
		//由选择天数来触发刷新面板的，默认显示第一项标签
		m_ui.pnlScrollView->setVisible(true);
		m_BarActItemList->setVisible(true);
		m_ui.imgBuyProp->setVisible(false);
		updateScrollView(Btn_pnl1);		
	}
}

void NewAreaActivityLayer::goToOtherScene(int sceneId)
{
	if(sceneId == -1)
	{
		return;
	}
	switch (sceneId)
	{
	case CHAPTER_SCENE:
		{
			Director::getInstance()->replaceScene(SceneChapter::create());
		}
		break;
	case HERO_SCENE:
		{
			Director::getInstance()->replaceScene(SceneAllHero::create());
		}
		break;
	case PVP_SCENE:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVP))
			{
				Director::getInstance()->replaceScene(PvpScene::create());
			}
		}
		break;
	case LOVE_SCENE:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_LOVE))
			{
				Director::getInstance()->replaceScene(LoveScene::create());
			}
		}
		break;
	case REFINE_SCENE:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TRAIL))
			{
				Director::getInstance()->replaceScene(SceneTower::create());
			}		
		}
		break;
	case ROB_SCENE:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_ROB))
			{
				Director::getInstance()->replaceScene(SceneRob::create());
			}
		}
		break;
	case FRIEND_SCENE:
		{
			Director::getInstance()->replaceScene(SceneFriend::create());
		}
		break;
	case MAZE_SCENE:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_MAZE))
			{
				Director::getInstance()->replaceScene(SceneMaze::create());
			}
		}
		break;
	case SHOP_SCENE:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_SHOP))
			{
				Director::getInstance()->replaceScene(SceneTakeCard::create());
			}
		}
		break;
	case TACTICS_SCENE:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TACTICS))
		{
			Director::getInstance()->pushScene(SceneTactics::create(true));
		}
		break;
	case STAR_SU_SCENE:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_STAR_SU))
			{
				Director::getInstance()->replaceScene(StarsSUScene::create());
			}
		}
		break;
	case PRAY_SCENE:
		{
			//祈福弹窗
			LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
			Director::getInstance()->getRunningScene()->addChild(buyEnergy);
		}
		break;
	case BOSS_SCENE:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_BOSS))
		{
			Director::getInstance()->replaceScene(BossScene::create());
		}
		break;
	case STAR_CAMP_SCENE:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_STAR_PLATE))
			{
				Director::getInstance()->replaceScene(StarsCampScene::create());
			}
		}
		break;
	case VIP_SCENE:
		{
			Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));		
		}
		break;
	case ARTIFACT_SCENE:
		{
			if(GuideManager::getInstance()->isFunctionOpen(FUNCTION_SHENQI))
			{
				Director::getInstance()->replaceScene(ArtifactScene::create());
			}
		}
		break;
	default:
		break;
	}
}

void NewAreaActivityLayer::onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName)
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case Btn_Buy:
			{
				Button* btn = (Button*)ref;
				
				NewAreaActivityModel* pInstance = NewAreaActivityModel::getInstance();
				int price = 0;
				int actId = 0;
				for(int i=0; i<pInstance->m_vecNewAreaActData.size(); i++)
				{
					if(pInstance->m_vecNewAreaActData.at(i).actOpenTime == m_iCurSelectedDay+1 &&
						pInstance->m_vecNewAreaActData.at(i).actShow == Btn_pnl4)
					{
						price = pInstance->m_vecNewAreaActData.at(i).newPrice;
						actId = pInstance->m_vecNewAreaActData.at(i).actId;
						break;
					}
				}
				if(m_iCurSelectedDay+1 > pInstance->m_iCurActDay)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20261));
				}
				else if(MainModel::getInstance()->getMainParam()->mGold < price)
				{

					CustomGoldTips *_tips = CustomGoldTips::create();
					if (_tips)
					{
						Director::getInstance()->getRunningScene()->addChild(_tips);
					}
				}
				else
				{
					//先记录一下购买的礼包信息
					sTeJiaBox tjBox;
					tjBox.isBox = true;
					tjBox.days = m_iCurSelectedDay+1;
					tjBox.price = price;
					NewAreaActivityModel::getInstance()->setCurBuyBox(tjBox);
					NewAreaActivityController::sendGetRewardMsg(actId);
				}			
			}
			break;
		case Btn_pnl1:
		case Btn_pnl2:
		case Btn_pnl3:
		case Btn_pnl4:
			{
				for(int i=0; i<m_vBtnLabel.size(); i++)
				{
					m_vBtnLabel.at(i)->setHighlighted(false);
					m_vBtnLabel.at(i)->setEnabled(true);
				}
				m_vBtnLabel.at(nWidgetName-Btn_pnl1)->setHighlighted(true);
				m_vBtnLabel.at(nWidgetName-Btn_pnl1)->setEnabled(false);
				m_iCurSelectedLabel = nWidgetName;
				updateBtnLabelHighLight((Button*)ref);
				updatePnlAct(nWidgetName);
			}
			break;
		case Btn_Day1:
		case Btn_Day2:
		case Btn_Day3:
		case Btn_Day4:
		case Btn_Day5:
		case Btn_Day6:
		case Btn_Day7:
			{
				int selectedDay = nWidgetName - Btn_Day1;
				if(selectedDay+1 > NewAreaActivityModel::getInstance()->m_iCurActDay)
				{
					ostringstream str;
					//str << "第" << selectedDay+1 << "天开启";
					str << DataManager::getInstance()->searchCommonTexdtById(10159);
					CustomTips::showUtf8(str.str());
					return;
				}

				m_iCurSelectedDay = nWidgetName-Btn_Day1;
				for(int i=0; i<m_vBtnDays.size(); i++)
				{
					m_vBtnDays.at(i)->setHighlighted(false);
					m_vBtnDays.at(i)->setEnabled(true);
				}
				m_vBtnDays.at(m_iCurSelectedDay)->setHighlighted(true);	
				m_vBtnDays.at(m_iCurSelectedDay)->setEnabled(false);

				for(int i=0; i<m_vBtnLabel.size(); i++)
				{
					m_vBtnLabel.at(i)->setHighlighted(false);
					m_vBtnLabel.at(i)->setEnabled(true);
				}
				m_vBtnLabel.at(0)->setHighlighted(true);
				m_vBtnLabel.at(0)->setEnabled(false);
				m_iCurSelectedLabel = Btn_pnl1;
				updateBtnLabel();
				updatePnlAct(nWidgetName);
			}
			break;
		case Btn_Get:
			{
				if(m_iCurSelectedDay+1 > NewAreaActivityModel::getInstance()->m_iCurActDay)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20261));
					break;
				}
				Button* btn = (Button*)ref;
				m_pnlGettedRewardItem = (Layout*)btn->getParent();
				m_pnlGettedRewardItem->retain();			//防止被莫名析构
				Text* ID = (Text*)btn->getParent()->getChildByName("rewardId");
				NewAreaActivityController::sendGetRewardMsg(atoi(ID->getString().c_str()));
			}
			break;
		case Btn_Go:
			{
				Button* btn = (Button*)ref;
				Text* ID = (Text*)btn->getParent()->getChildByName("rewardId");
				int actId = atoi(ID->getString().c_str());
				RowData* rowData = DataManager::getInstance()->searchNewAreaActivityById(actId);
				if(m_iCurSelectedDay + 1 > NewAreaActivityModel::getInstance()->m_iCurActDay)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20261));
				}
				else
				{
					int sceneId = -1;
					for(int i=0; i<NewAreaActivityModel::getInstance()->m_vecNewAreaActData.size(); i++)
					{
						if(NewAreaActivityModel::getInstance()->m_vecNewAreaActData.at(i).actId == atoi(ID->getString().c_str()))
						{
							sceneId = NewAreaActivityModel::getInstance()->m_vecNewAreaActData.at(i).skipType;
							break;
						}
					}
					goToOtherScene(sceneId);
				}				
			}
			break;
		default:
			break;
		}
	}
}

void NewAreaActivityLayer::onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			{
				Size size = m_ui.pnlScrollView->getInnerContainerSize();
				Vec2 vec = m_ui.pnlScrollView->getInnerContainer()->getPosition();

				//滚动条重设进度
				float percentY = -vec.y / (size.height - m_ui.pnlScrollView->getContentSize().height);
				m_BarActItemList->setScrollBarPercentY((1.0f - percentY)*100.0f);			
			}
		}
		break;

	default:
		break;
	}
}

void NewAreaActivityLayer::showGetReward()
{
	vector<PopRewardItem*> vecRewardItem;

	//最多三个道具
	for(int i=1; i<=3; i++)
	{
		Layout* pnlProp = (Layout*)(m_pnlGettedRewardItem->getChildByName("pnl_prop" + _TO_STR(i)));
		if(pnlProp->getChildrenCount() != 0)
		{
			UIToolHeader* tool = (UIToolHeader*)pnlProp->getChildByTag(REWARD_TOOL_TAG);
			//金币
			if(tool->getToolId() == GOLD_ITEM_ID)
			{
				PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(tool->getNum());
				vecRewardItem.push_back(rewardItem);
			}
			//银币
			else if(tool->getToolId() == COIN_ITEM_ID)
			{
				PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(tool->getNum());
				vecRewardItem.push_back(rewardItem);
			}
			else
			{
				PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(tool->getToolId(), tool->getNum());
				vecRewardItem.push_back(rewardItem);
			}
		}
	}

	PopReward::show(vecRewardItem);

}

string NewAreaActivityLayer::formatTime(int second)
{
	string strResTime = "";
	string day;
	string ht;
	string mint;
	string sect;
	int days = second/(24*3600);
	day = _TO_STR(days);

	int st = second%(24*3600);

	int h = st/3600;
	ht = _TO_STR(h);

	int min = (st%3600)/60;
	mint = _TO_STR(min);

	int sec = st%60;
	sect = _TO_STR(sec);
	auto miao = DataManager::getInstance()->searchCommonTexdtById(10153);
	auto fen = DataManager::getInstance()->searchCommonTexdtById(10152);
	auto xiaoshi = DataManager::getInstance()->searchCommonTexdtById(20030);
	auto tian = DataManager::getInstance()->searchCommonTexdtById(10150);

	strResTime = day + tian + ht +xiaoshi + mint + fen + sect + miao;

	return strResTime;
}

void NewAreaActivityLayer::updateResTime(bool isActEnd)
{
	if(isActEnd)
	{
		NewAreaActivityModel::getInstance()->setActStates(false);
		Director::getInstance()->replaceScene(MainCityScene::create());
	}
	else
	{
		int resTime = NewAreaActivityModel::getInstance()->m_iRemainTime;
		string timeText = formatTime(resTime);
		m_ui.txtActTime->setString(STR_UTF8(timeText));
		setTextAddStroke(m_ui.txtActTime, Color3B(0x09, 0x4e, 0x51), 2);
	}	
}

void NewAreaActivityLayer::updateCurDayState()
{
	m_iCurSelectedDay = m_iLeaveBtnId >= 0 ? m_iLeaveBtnId : (NewAreaActivityModel::getInstance()->m_iCurActDay-1);
	m_iCurSelectedLabel = m_iLeaveLabId > 0 ? m_iLeaveLabId : Btn_pnl1;
	if(m_iCurSelectedDay > 6 || m_iCurSelectedDay < 0)
	{
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20263));
		return;
	}
	for(int i=0; i<m_vBtnDays.size(); i++)
	{
		m_vBtnDays.at(i)->setHighlighted(false);
		m_vBtnDays.at(i)->setEnabled(true);
	}
	for(int i=0; i<m_vBtnLabel.size(); i++)
	{
		m_vBtnLabel.at(i)->setHighlighted(false);
		m_vBtnLabel.at(i)->setEnabled(true);
	}

	if(m_iLeaveBtnId >= 0 && m_iLeaveBtnId < m_vBtnDays.size())			//按钮是0~3
	{
		m_vBtnDays.at(m_iLeaveBtnId)->setHighlighted(true);
		m_vBtnDays.at(m_iLeaveBtnId)->setEnabled(false);
		m_iLeaveBtnId = -1;				//重置为invalid
	}
	else
	{
		if(m_iCurSelectedDay >= 0 && m_iCurSelectedDay < m_vBtnDays.size())
		{
			m_vBtnDays.at(m_iCurSelectedDay)->setHighlighted(true);	
			m_vBtnDays.at(m_iCurSelectedDay)->setEnabled(false);
		}	
	}

	if(m_iLeaveLabId > 0 && m_iLeaveLabId <= m_vBtnLabel.size())		//标签是1~4
	{
		m_vBtnLabel.at(m_iLeaveLabId-1)->setHighlighted(true);
		m_vBtnLabel.at(m_iLeaveLabId-1)->setEnabled(false);
		m_iLeaveLabId = -1;				//重置为invalid
	}
	else
	{
		if(m_vBtnLabel.size() > 0)
		{
			m_vBtnLabel.at(0)->setHighlighted(true);
			m_vBtnLabel.at(0)->setEnabled(false);
		}
	}
	
	m_iResTime = NewAreaActivityModel::getInstance()->m_iRemainTime;
	string timeText = formatTime(m_iResTime);
	m_ui.txtActTime->setString(STR_UTF8(timeText));
	setTextAddStroke(m_ui.txtActTime, Color3B(0x09, 0x4e, 0x51), 2);

	//未到时间的按钮加锁, 到时间的解锁
	int needLockDay = NewAreaActivityModel::getInstance()->m_iCurActDay-1;
	for(int i=0; i<7; i++)
	{
		ImageView* imgLock = (ImageView*)m_vBtnDays.at(i)->getChildByName("IMG_LOCK");
		if(imgLock == nullptr)
		{
			imgLock = ImageView::create("Image/Icon/global/img_lock.png");
			imgLock->setScale(0.6f);
			imgLock->setPosition(Vec2(m_vBtnDays.at(i)->getContentSize()) + Vec2(-16, -20));
			imgLock->setName("IMG_LOCK");
			m_vBtnDays.at(i)->addChild(imgLock);
		}		
		imgLock->setVisible(i > needLockDay);
	}
}

void NewAreaActivityLayer::update(float dt)
{
	this->unscheduleUpdate();
	Director::getInstance()->replaceScene(MainCityScene::create());
}

void NewAreaActivityLayer::openUITimer()
{
	if(NewAreaActivityModel::getInstance()->m_iCurActDay != 0 
		&& NewAreaActivityModel::getInstance()->m_iRemainTime > 0)
	{
		//开启时间戳定时器
		setEndTimeStamp(NewAreaActivityModel::getInstance()->m_iActEndedTime);
		LocalTimer::getInstance()->addLocalTimerListener(this);
	}
}

void NewAreaActivityLayer::timeTick( int dt )
{
	NewAreaActivityModel::getInstance()->m_iRemainTime -= dt;

	if(NewAreaActivityModel::getInstance()->m_iRemainTime <= 0)
	{
		NewAreaActivityModel::getInstance()->m_bIsActOpen = false;
	}

	if(NewAreaActivityModel::getInstance()->m_iRemainTime > 0)
	{
		updateResTime(false);
	}
	else
	{
		updateResTime(true);
	}
}

void NewAreaActivityLayer::updateSelf(void* data)
{
	if(!data)
	{
		return;
	}
	ObserverParam* obParam = (ObserverParam*) data;

	switch (obParam->id)
	{
	case nMAIN_CMD_GET_TASK_NEW_INFO:
		{
			updateCurDayState();
			updateBtnLabel();		
			updateScrollView(m_iCurSelectedLabel > 0 ? m_iCurSelectedLabel : Btn_pnl1);
			showRedFlag();

			openUITimer();
		}
		break;
	case nMAIN_CMD_GET_TASK_NEW_REWARD:
		{
			if(m_ui.imgBuyProp->isVisible())
			{
				updateBuyView();
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20234));
			}
			else
			{
				//领取奖励成功
				showGetReward();
				updateScrollView(m_iCurSelectedLabel);			
			}
			showRedFlag();		
		}
		break;
	case nMAIN_CMD_NEWAREA_ACT_PUSH_INFO:
		{
			if(m_ui.imgBuyProp->isVisible())
			{
				updateBuyView();
			}
			else
			{
				updateScrollView(m_iCurSelectedLabel);
			}		
		}
		break;
	default:
		break;
	}
}




