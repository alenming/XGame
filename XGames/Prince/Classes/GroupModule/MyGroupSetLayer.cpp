#include "MyGroupSetLayer.h"
#include "Temp/CustomTips.h"
#include "Temp/CustomPop.h"
#include "DataManager/DataManager.h"
#include "MyGroupModel.h"
#include "MainModule/MainCityLoadingScene.h"
#include "MyGroupController.h"
#include "Communication/Command.h"


MyGroupSetLayer::MyGroupSetLayer():
mJoinInLevelMax(0),
mJoinInLevelMin(0),
//firstRunToAuty(true),
isCanLa(true), 
isLongPressed(false)
{
	MyGroupModel::getInstance()->addObserver(this);
}

MyGroupSetLayer::~MyGroupSetLayer()
{
	MyGroupModel::getInstance()->removeObserver(this);
}

Layer* MyGroupSetLayer::create( )
{ 
	auto pRet = new MyGroupSetLayer(); 
	if (pRet && pRet->init()) 
	{
		pRet->autorelease(); 
		return pRet; 
	}
 
	else 
	{ 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
}

bool MyGroupSetLayer::init( )
{
	if(!ModalLayer::init())
	{
		return false;
	}
	//获取最大最小值
	RowData* rowData = DataManager::getInstance()->searchFunctionLevelById(USER_OPEN_FUNCTION);
	if (rowData)
	{
		mJoinInLevelMin = rowData->getIntData("avlLv");

	}

	RowData* rowData1 = DataManager::getInstance()->searchToolSystemById(USER_MAX_LEVEL);
	if (rowData1)
	{
		mJoinInLevelMax = rowData1->getIntData("value");

	}

	initUI();
	return true;
}

void MyGroupSetLayer::onEnter()
{
	ModalLayer::onEnter();
}

void MyGroupSetLayer::onExit()
{
	ModalLayer::onExit();
}

void MyGroupSetLayer::initUI()
{

	//获取帮派审核资料
	mJoinlimit.levelLimit = MyGroupModel::getInstance()->getJoinLimit()->levelLimit;
	mJoinlimit.type = MyGroupModel::getInstance()->getJoinLimit()->type;

	// 加载 UI;
	string strFileName;
	strFileName.assign("Image/UIGang/UIGang_Set.ExportJson");

	// Root;
	m_ui.uiRoot= dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.uiRoot->setName("UI_ROOT");
	this->addChild(m_ui.uiRoot);

	m_ui.uiSetRoot = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Panel_Set"));
	m_ui.uiAuditRoot = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Panel_Approve"));

	m_ui.uiTexMember = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Label_186_0"));
	setMemberText();

	m_ui.memberList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.uiAuditRoot,"ListView_memberList"));  //挑战玩家列表

	m_ui.uiExit = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Close"));
	m_ui.uiExit->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnClick,this,SETLAYER_CALLBACK_TYPE::BTN_SETLAYER_EXIT));

	m_ui.uiSave = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Button_178_0_1"));
	m_ui.uiSave->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnClick,this,SETLAYER_CALLBACK_TYPE::BTN_SAVE));

	m_ui.uiDissolved = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Button_178_0_1_2"));
	m_ui.uiDissolved->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnClick,this,SETLAYER_CALLBACK_TYPE::BTN_DISSOLVE_FAC));

	m_ui.uiAdd = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Button_178"));
	m_ui.uiAdd->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnClickADDorSub,this,SETLAYER_CALLBACK_TYPE::BTN_ADD));

	m_ui.uiSubtract = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Button_178_0"));
	m_ui.uiSubtract->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnClickADDorSub,this,SETLAYER_CALLBACK_TYPE::BTN_SUBTRACT));

	m_ui.uiTexJoinInLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Label_186"));
	setJoinInLevelText(mJoinlimit.levelLimit);

	m_ui.uiSet = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Set"));
	m_ui.uiSet->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnClick,this,SETLAYER_CALLBACK_TYPE::BTN_SET));

	m_ui.uiAudit = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Approve"));
	m_ui.uiAudit->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnClick,this,SETLAYER_CALLBACK_TYPE::BTN_AUDIT));

	m_ui.uiSetRoot->setVisible(true);
	m_ui.uiSet->setHighlighted(true);
	m_ui.uiSet->setEnabled(false);
	m_ui.uiAudit->setHighlighted(false);
	m_ui.uiAudit->setEnabled(true);

	setJoinInFlag();

	m_ui.uiCheckBoxOn = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.uiRoot,"CheckBox_175"));
	m_ui.uiCheckBoxOff = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.uiRoot,"CheckBox_175_0"));

	//自由or审核
	setJoinLimitCheckBox(mJoinlimit.type);

	m_ui.uiCheckBoxOn->addEventListenerCheckBox(this, checkboxselectedeventselector(MyGroupSetLayer::onCheckBoxCallBackON));
	m_ui.uiCheckBoxOff->addEventListenerCheckBox(this, checkboxselectedeventselector(MyGroupSetLayer::onCheckBoxCallBackOFF));

	setMemberText();
	initAutyUi();

}
void MyGroupSetLayer::initAutyUi()
{
	auto memberVec = MyGroupModel::getInstance()->getAllProposerInfo();


	m_RankPalyerBar = CustomScrollBar::create();
	auto lenghaa = m_ui.memberList->getContentSize().height;
	m_RankPalyerBar->initScrollBar(lenghaa);
	m_RankPalyerBar->setPosition(m_ui.memberList->getPosition() + Point(-12, 0));
	m_ui.uiAuditRoot->addChild(m_RankPalyerBar);
   
	m_RankPalyerBar->setScrollBarLengthPercent(1.0f);
	m_RankPalyerBar->setScrollBarPercentY(0.0f);


	m_ui.memberList->removeAllItems();
	m_ui.memberList->setDirection(ScrollView::Direction::VERTICAL);
	m_ui.memberList->setBounceEnabled( true);

	dynamic_cast<ScrollView*>(m_ui.memberList)->addEventListener(CC_CALLBACK_2(MyGroupSetLayer::onScrollViewEvent, this));

	for (int i = 0; i < memberVec->size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIGang/UIGang_ApprovalList.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		Layout *imgHero = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_HeadIcon"));
		UIToolHeader *icon = UIToolHeader::create(memberVec->at(i).iconResId);
		icon->setScale(0.8f);
		icon->setLvl(memberVec->at(i).level);
		icon->setAnchorPoint(Vec2(0,0));
		imgHero->addChild(icon);
		icon->setTouchEnabled(true);

		auto _vipIma = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_VIP"));
		_vipIma->loadTexture("Image/Icon/vip/btn_VIP" + TO_STR(memberVec->at(i).vipLv) + ".png",TextureResType::LOCAL);

		Text *Label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Name"));
		Label_name->setString(memberVec->at(i).name);
		Label_name->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(Label_name, Color3B(0x7d, 0x3f, 0x1c), 2);

		Text *Label_force = dynamic_cast<Text*>(Helper::seekWidgetByName(layer, "Lab_Power"));
		Label_force->setString(TO_STR(memberVec->at(i).fightPower));
		Label_force->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(Label_force, Color3B(0x7d, 0x3f, 0x1c), 2);

		Button *Button_no = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Refuse"));
		Button *Button_yes = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Accept"));
		Button_no->setTag(memberVec->at(i).uId);
		Button_yes->setTag(memberVec->at(i).uId);

		Button_no->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnRefuseCallBack, this,memberVec->at(i).uId));
		Button_yes->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnAcceptCallBack,this, memberVec->at(i).uId));

		m_ui.memberList->pushBackCustomItem(layer);
		
	}
	m_RankPalyerBar->setVisible(m_ui.memberList->getInnerContainerSize().height > m_ui.memberList->getContentSize().height);
	m_RankPalyerBar->setScrollBarLengthPercent(m_ui.memberList->getContentSize().height / m_ui.memberList->getInnerContainerSize().height);
	m_RankPalyerBar->setScrollBarPercentY(0.0f);
}

void  MyGroupSetLayer::onCheckBoxCallBackON(Ref* reff,CheckBoxEventType type)
{
	if (type == CheckBoxEventType::CHECKBOX_STATE_EVENT_SELECTED)
	{
		setJoinLimitCheckBox(eJoinType::eJOIN_CHECK_TYPE);
	}
	else if (type ==CheckBoxEventType::CHECKBOX_STATE_EVENT_UNSELECTED)
	{
		m_ui.uiCheckBoxOff->setSelectedState(false);
	}
}

void  MyGroupSetLayer::onCheckBoxCallBackOFF(Ref* reff,CheckBoxEventType type)
{
	if (type == CheckBoxEventType::CHECKBOX_STATE_EVENT_SELECTED)
	{
		setJoinLimitCheckBox(eJoinType::eJOIN_FREE_TYPE);
	}
	else if (type ==CheckBoxEventType::CHECKBOX_STATE_EVENT_UNSELECTED)
	{
		m_ui.uiCheckBoxOn->setSelectedState(false);
	}
}

void MyGroupSetLayer::judgeLongPressed( float delta )
{
	//isLongPressed = true;
	this->unschedule(schedule_selector(MyGroupSetLayer::judgeLongPressed));

	if (isLongPressed)
	{
		this->schedule(schedule_selector(MyGroupSetLayer::myupdateADD), 0.1f, kRepeatForever, 0.0f);
	}
	else
	{
		this->schedule(schedule_selector(MyGroupSetLayer::myupdateSUB), 0.1f, kRepeatForever, 0.0f);
	}
	
}

void  MyGroupSetLayer::onBtnClick( Ref* reff, Widget::TouchEventType type,SETLAYER_CALLBACK_TYPE mBtnCallBackType)
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
		
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		switch (mBtnCallBackType)
		{
			case SETLAYER_CALLBACK_TYPE::BTN_SETLAYER_EXIT:
				{
					this->dispose();
					//isCanLa为true 说明没有对审核成员进行过操作,不要再摘取帮派信息
					if (!isCanLa)
					{
						MyGroupController::getInstance()->sendGetGroupMainMsg();
					}
					break;
				}
			case SETLAYER_CALLBACK_TYPE::BTN_DISSOLVE_FAC:
				onBtnDissolveCallBack();
				break;
			case SETLAYER_CALLBACK_TYPE::BTN_SAVE:
				onBtnSaveCallBack();
				break;
			case SETLAYER_CALLBACK_TYPE::BTN_SET:
				{
					m_ui.uiAuditRoot->setVisible(false);
					m_ui.uiSetRoot->setVisible(true);
					m_ui.uiSet->setHighlighted(true);
					m_ui.uiSet->setEnabled(false);
					m_ui.uiAudit->setHighlighted(false);
					m_ui.uiAudit->setEnabled(true);
				}
				break;
			case SETLAYER_CALLBACK_TYPE::BTN_AUDIT:
				{
					m_ui.uiAuditRoot->setVisible(true);
					m_ui.uiSetRoot->setVisible(false);
					m_ui.uiSet->setHighlighted(false);
					m_ui.uiSet->setEnabled(true);
					m_ui.uiAudit->setHighlighted(true);
					m_ui.uiAudit->setEnabled(false);
				}
				break;
			default:
				break;
		}
	}
}

void MyGroupSetLayer::setJoinInLevelText(int _JoinInLevel)
{
	m_ui.uiTexJoinInLevel->setText(TO_STR(_JoinInLevel));
	m_ui.uiTexJoinInLevel->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexJoinInLevel, Color3B(0x7d, 0x3f, 0x1c),2);
}

void MyGroupSetLayer::setAddOrSubtract(bool _bb)
{
	if (_bb)
	{
		mJoinlimit.levelLimit++;
		mJoinlimit.levelLimit = mJoinlimit.levelLimit<=mJoinInLevelMax?mJoinlimit.levelLimit:mJoinInLevelMax;
		setJoinInLevelText(mJoinlimit.levelLimit);
		if (mJoinlimit.levelLimit == mJoinInLevelMax)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10213));
		}
		if (mJoinlimit.levelLimit == mJoinInLevelMin+1)
		{
			//m_ui.uiSubtract->setEnabled(true);
		}
	}
	else
	{
		mJoinlimit.levelLimit--;
		mJoinlimit.levelLimit = mJoinlimit.levelLimit>=mJoinInLevelMin?mJoinlimit.levelLimit:mJoinInLevelMin;
		setJoinInLevelText(mJoinlimit.levelLimit);
		if (mJoinlimit.levelLimit == mJoinInLevelMin)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10214));
		}
		if (mJoinlimit.levelLimit == mJoinInLevelMax-1)
		{
			//m_ui.uiAdd->setEnabled(true);
		}
	}
}

void MyGroupSetLayer::setMemberText()
{
	int totalMember = MyGroupModel::getInstance()->getMyGroupInfo()->totalMemberNum;
	int curMember = MyGroupModel::getInstance()->getMyGroupInfo()->curMemberNum;
	m_ui.uiTexMember->setString(TO_STR(curMember)+"/"+TO_STR(totalMember));
	m_ui.uiTexMember->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexMember, Color3B(0x7d, 0x3f, 0x1c),2);
}

void MyGroupSetLayer::setJoinLimitCheckBox(eJoinType _first)
{
	if (_first == eJoinType::eJOIN_CHECK_TYPE)
	{
		m_ui.uiCheckBoxOn->setSelectedState(true);
		m_ui.uiCheckBoxOff->setSelectedState(false);
	}
	else if (_first == eJoinType::eJOIN_FREE_TYPE)
	{
		m_ui.uiCheckBoxOn->setSelectedState(false);
		m_ui.uiCheckBoxOff->setSelectedState(true);
	}
	mJoinlimit.type = _first;
}

void  MyGroupSetLayer::onBtnDissolveCallBack()
{
	auto isBangZhu = MyGroupModel::getInstance()->getMyInfo()->info.power;
	//是帮主才能调用
	if (isBangZhu ==  ePostPower::ePOWERVICE)
	{
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10215));
	}
	else if (isBangZhu ==  ePostPower::ePOWERLEADER)
	{
		//确定解散?
		CustomPopCallback cpCallBack = [=](Ref* rre, CustomRetType type)->void
		{
			if (type ==  CustomRetType::RET_TYPE_OK)
			{
				//发送请求
				MyGroupController::getInstance()->sendDisbandGroupMsg();
			}
			else if (type ==  CustomRetType::RET_TYPE_CANCEL)
			{
			}
		};
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10216),cpCallBack, CustomPopType::CUSTOM_YES_NO);
	
	}
}

void  MyGroupSetLayer::onBtnSaveCallBack()
{
	//发送设置好的数据
	MyGroupController::getInstance()->sendSetJoinLimitMsg(mJoinlimit);
}

void MyGroupSetLayer::setJoinInFlag()
{
	//int mVecSize = 0;
	//Vector的人数
	int mVecSize = MyGroupModel::getInstance()->getAllProposerInfo()->size();
	Node* FriendFlag = m_ui.uiAudit->getChildByName("mail_flg");
	if (!FriendFlag)
	{
		FriendFlag = cocos2d::ui::ImageView::create("Image/Icon/global/img_dian.png");
		FriendFlag->setPosition(cocos2d::Vec2(m_ui.uiAudit->getContentSize().width - FriendFlag->getContentSize().width/2, 
			m_ui.uiAudit->getContentSize().height - FriendFlag->getContentSize().height/2));
		FriendFlag->setName("mail_flg");
		m_ui.uiAudit->addChild(FriendFlag, 9);
	}
	if (mVecSize>0)
	{
		FriendFlag->setVisible(true);
	}
	else
	{
		FriendFlag->setVisible(false);
	}
}

void MyGroupSetLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	observerParam->updateData;
	switch (observerParam->id)
	{
	case nMAIN_CMD_GROUP_SET:
		{
			//CustomTips::show("保存设置成功!");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10217));
		}
		break;
	case nMAIN_CMD_GROUP_APPLY_CHECK:
		{
			//同意或者拒绝申请后刷新flage
			setJoinInFlag();
			setMemberText();
			ReflushListView();
		}
		break;
	case nMAIN_CMD_GROUP_MAIN_INFO:
		{
			m_ui.uiAuditRoot->setVisible(true);
			m_ui.uiSetRoot->setVisible(false);
			m_ui.uiSet->setHighlighted(false);
			m_ui.uiSet->setEnabled(true);
			m_ui.uiAudit->setHighlighted(true);
			m_ui.uiAudit->setEnabled(false);
			setJoinInFlag();
			setMemberText();
			ReflushListView();
		}
		break;
		/*
	case nMAIN_CMD_GROUP_DISBAND:
		{
			auto call = [this](Ref* ref, CustomRetType type)->void
			{
				Director::getInstance()->replaceScene(MainCityScene::create());
			};
			CustomPop::show("帮派已解散，请加入其它帮派", call, CUSTOM_ID_YES);
		}
			break;
			*/
		/*
	case nMAIN_CMD_GROUP_APPLICANT_LIST:
		{
			setJoinInFlag();
			setMemberText();
			initAutyUi();
			//ReflushListView();
		}
		break;*/
	default:
		break;
	}
}

void MyGroupSetLayer::myupdateADD(float dt)
{
	setAddOrSubtract(true);
}

void MyGroupSetLayer::myupdateSUB(float dt)
{
	setAddOrSubtract(false);
}

void MyGroupSetLayer:: onBtnAcceptCallBack(Ref* reff, Widget::TouchEventType type,int uid)
{
	isCanLa = false;

	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		MyGroupModel::getInstance()->setApplicantID(uid);
		MyGroupModel::getInstance()->setTempIsAllow(true);
		MyGroupController::getInstance()->sendCheckJoinMsg(uid,true);
	}
}

void  MyGroupSetLayer::onBtnRefuseCallBack(Ref* reff, Widget::TouchEventType type, int uid)
{
	isCanLa = false;
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		MyGroupModel::getInstance()->setApplicantID(uid);
		MyGroupModel::getInstance()->setTempIsAllow(false);
		MyGroupController::getInstance()->sendCheckJoinMsg(uid,false);
	}
}

void MyGroupSetLayer::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	switch(type)
	{

	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			if (pSender == m_ui.memberList)
			{
				Size size = m_ui.memberList->getInnerContainerSize();
				Vec2 vec = m_ui.memberList->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.memberList->getContentSize().height);
				m_RankPalyerBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
		}
	}
}

void MyGroupSetLayer::ReflushListView()
{
	auto memberVec = MyGroupModel::getInstance()->getAllProposerInfo();

	m_ui.memberList->removeAllItems();

	for (int i = 0; i < memberVec->size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIGang/UIGang_ApprovalList.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));



		Layout *imgHero = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_HeadIcon"));
		UIToolHeader *icon = UIToolHeader::create(memberVec->at(i).iconResId);
		icon->setScale(0.8f);
		icon->setLvl(memberVec->at(i).level);
		icon->setAnchorPoint(Vec2(0,0));
		imgHero->addChild(icon);
		icon->setTouchEnabled(true);

		auto _vipIma = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_VIP"));
		_vipIma->loadTexture("Image/Icon/vip/btn_VIP" + TO_STR(memberVec->at(i).vipLv) + ".png",TextureResType::LOCAL);

		Text *Label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Name"));
		Label_name->setString(memberVec->at(i).name);
		Label_name->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(Label_name, Color3B(0x7d, 0x3f, 0x1c), 2);

		Text *Label_force = dynamic_cast<Text*>(Helper::seekWidgetByName(layer, "Lab_Power"));
		Label_force->setString(TO_STR(memberVec->at(i).fightPower));
		Label_force->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(Label_force, Color3B(0x7d, 0x3f, 0x1c), 2);

		Button *Button_no = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Refuse"));
		Button *Button_yes = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Accept"));
		Button_no->setTag(memberVec->at(i).uId);
		Button_yes->setTag(memberVec->at(i).uId);

		Button_no->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnRefuseCallBack, this,memberVec->at(i).uId));
		Button_yes->addTouchEventListener(CC_CALLBACK_2(MyGroupSetLayer::onBtnAcceptCallBack,this, memberVec->at(i).uId));

		m_ui.memberList->pushBackCustomItem(layer);

	}
	m_RankPalyerBar->setVisible(m_ui.memberList->getInnerContainerSize().height > m_ui.memberList->getContentSize().height);
	m_RankPalyerBar->setScrollBarLengthPercent(m_ui.memberList->getContentSize().height / m_ui.memberList->getInnerContainerSize().height);
	m_RankPalyerBar->setScrollBarPercentY(0.0f);
}

void  MyGroupSetLayer::onBtnClickADDorSub( Ref* reff, Widget::TouchEventType type, SETLAYER_CALLBACK_TYPE mBtnCallBackType )
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (mBtnCallBackType)
		{
		case BTN_ADD:
			isLongPressed = true;
			break;
		case BTN_SUBTRACT:
			isLongPressed = false;
			break;
		default:	
			break;
		}
		// 启动定时器，判定长按;
		this->schedule(schedule_selector(MyGroupSetLayer::judgeLongPressed), 0.5f);

	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		switch (mBtnCallBackType)
		{
		case BTN_ADD:
			{
				setAddOrSubtract(true);			
				break;
			}
		case BTN_SUBTRACT:
			{
				setAddOrSubtract(false);
				break;
			}
		default:
			break;
		}
		this->unschedule(schedule_selector(MyGroupSetLayer::myupdateADD));
		this->unschedule(schedule_selector(MyGroupSetLayer::judgeLongPressed));
		this->unschedule(schedule_selector(MyGroupSetLayer::myupdateSUB));
	}
}
