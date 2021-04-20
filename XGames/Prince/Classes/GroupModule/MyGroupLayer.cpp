#include "MyGroupLayer.h"
#include "MainModule/MainCityScene.h"
#include "RankTopModule/RankTopScene.h"
#include "RankTopModule/RankTopModel.h"
#include "MyGroupBuildLayer.h"
#include "MyGroupSetLayer.h"
#include "MyGroupPlayerInfoLayer.h"
#include "JoinGroupScene.h"
#include "NewShopModule/ShopScene.h"
#include "Widget/LayerGameRules.h"
#include "MyGroupActivity.h"
#include "GroupBossModule/GroupBossScene.h"

//成员列表tag
#define TAG_ITEM_LIST	1001
//建筑数量
#define NUM_GROUP_BUILDING	2
//建筑列表间距
#define GAP_GROUP_BUILDING	10.0f
//时间子控件间隔
#define GAP_GROUP_EVENT		4.0f

//事件文本的高度
const float fEventTxtHeight = 20.0f;
const float fEventContSize = 20.0f;

MyGroupLayer::MyGroupLayer()
{
	m_tMemberList = nullptr;
	m_bIsInittedDynEvent = false;
	m_bIsInittedBuilding = false;
	m_bIsInittedActivity = false;
	m_memListScrollBar = nullptr;
	m_flagListScrollBar = nullptr;
	m_eventListScrollBar = nullptr;
	m_buildListScrollBar = nullptr;
	m_ActbuildListScrollBar = nullptr;
	xunlongIn = nullptr;
	groupBoss = nullptr;
	MyGroupModel::getInstance()->addObserver(this);
}

MyGroupLayer::~MyGroupLayer()
{
	MyGroupModel::getInstance()->removeObserver(this);
}

bool MyGroupLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void MyGroupLayer::onEnter()
{
	Layer::onEnter();

	auto callBack = [this](float dt)->void
	{
		MyGroupController::getInstance()->sendGetGroupMainMsg();
		
		this->getScheduler()->unschedule("MyGroupLayer::sendMsg", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "MyGroupLayer::sendMsg");
}

void MyGroupLayer::onExit()
{
	Layer::onExit();
}

void MyGroupLayer::initUI()
{
	// 加载 UI;
	string strFileName;
	strFileName.assign("Image/UIGang/UIGang_Main.ExportJson");

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
// 			if (SceneManager::getInstance()->isPushScene())
// 			{
// 				Director::getInstance()->popScene();
// 				SceneManager::getInstance()->setPushScene(false);
// 			}
// 			else
// 			{
				scheduleUpdate();
/*			}*/
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_GANG);
	this->addChild(_layerCommHeadInfo);

	//初始化公共UI
	initCommonUI();

	//初始化帮派旗帜弹窗
	initFlagListUI();

	//初始化成员列表UI
	initMemListUI();

	//初始化帮派动态UI
	initDynEventUI();

	//初始化帮派建筑UI
	//initBuildingUI();

	//初始化公告弹窗
	initNoticeUI();

}

//初始化公共UI
void MyGroupLayer::initCommonUI()
{
	//公共UI
	Layout* parentImg = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_Main"));

	m_ui.groupIconPnl = (Layout*)parentImg->getChildByName("Pnl_GangIcon");
// 	m_ui.groupLv = (Text*)(((Layout*)parentImg->getChildByName("Pnl_Name"))->getChildByName("Lab_GangLevel"));
// 	m_ui.groupName = (Text*)(((Layout*)parentImg->getChildByName("Pnl_Name"))->getChildByName("Lab_GangName"));
	m_ui.groupLv = (Text*)parentImg->getChildByName("Lab_GangLevel");
	m_ui.groupName = (Text*)parentImg->getChildByName("Lab_GangName");
	m_ui.groupId = (Text*)parentImg->getChildByName("Lab_GangID");
	m_ui.leaderName = (Text*)parentImg->getChildByName("Lab_WangName");
	m_ui.memberNum = (Text*)parentImg->getChildByName("Lab_Member");
	m_ui.myPostPower = (Text*)parentImg->getChildByName("Lab_MyJob");				
	m_ui.groupRank = (Text*)parentImg->getChildByName("Lab_GangRank");					

	m_ui.groupLv->setString("");
	m_ui.groupLv->setFontName(FONT_FZZHENGHEI);
	m_ui.groupName->setString("");
	m_ui.groupName->setFontName(FONT_FZZHENGHEI);
	m_ui.groupId->setString("");
	m_ui.groupId->setFontName(FONT_FZZHENGHEI);
	m_ui.leaderName->setString("");
	m_ui.leaderName->setFontName(FONT_FZZHENGHEI);
	m_ui.memberNum->setString("");
	m_ui.memberNum->setFontName(FONT_FZZHENGHEI);
	m_ui.myPostPower->setString("");
	m_ui.myPostPower->setFontName(FONT_FZZHENGHEI);
	m_ui.groupRank->setString("");
	m_ui.groupRank->setFontName(FONT_FZZHENGHEI);

	m_ui.exitBtn = (Button*)parentImg->getChildByName("Btn_Quit");
	m_ui.exitBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_EXIT));
	m_ui.settingBtn = (Button*)parentImg->getChildByName("Btn_Manage");
	m_ui.settingBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_SETTING));
	m_ui.helperBtn = (Button*)parentImg->getChildByName("Btn_Help");
	m_ui.helperBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_HELP));
	//设置设置按钮小红点
	//setJoinInFlag();
	//管理设置按钮默认隐藏
	m_ui.settingBtn->setVisible(false);

	//页签按钮
	Layout* tabParentImg = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Button"));

	m_ui.vecTabsBtn.clear();

	m_ui.memberListTabBtn = (Button*)tabParentImg->getChildByName("Btn_List");
	m_ui.memberListTabBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_MEMBER_TAB));
	m_ui.vecTabsBtn.push_back(m_ui.memberListTabBtn);

	m_ui.dynEventTabBtn = (Button*)tabParentImg->getChildByName("Btn_Event");
	m_ui.dynEventTabBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_EVENT_TAB));
	m_ui.vecTabsBtn.push_back(m_ui.dynEventTabBtn);
	
	m_ui.buildingTabBtn = (Button*)tabParentImg->getChildByName("Btn_Building");
	m_ui.buildingTabBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_BUILDING_TAB));
	m_ui.vecTabsBtn.push_back(m_ui.buildingTabBtn);
	
	m_ui.activityTabBtn = (Button*)tabParentImg->getChildByName("Btn_Activity");
	m_ui.activityTabBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_ACTIVITY_TAB));
	m_ui.vecTabsBtn.push_back(m_ui.activityTabBtn);
	
	m_ui.shopBtn = (Button*)tabParentImg->getChildByName("Btn_Shop");
	m_ui.shopBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_SHOP_TAB));
	
	m_ui.rankBtn = (Button*)tabParentImg->getChildByName("Btn_Rank");
	m_ui.rankBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_RANK_TAB));

	//页签视图
	Layout* parentPnl = (Layout*)m_ui.pRoot->getChildByName("Pnl_Gang");
	m_ui.vecViewList.clear();

	m_ui.memListViewPnl = (Layout*)parentPnl->getChildByName("Panel_memberList");
	m_ui.vecViewList.push_back(m_ui.memListViewPnl);
	m_ui.memListViewPnl->setVisible(true);
	m_eCurTabs = eTABS_MEMBERLIST_TYPE;						//默认是成员列表页签
	m_ui.memListPnl = (Layout*)(Helper::seekWidgetByName(m_ui.memListViewPnl, "Pnl_scroll"));

	m_ui.dynEventViewPnl = (Layout*)parentPnl->getChildByName("Panel_Event");
	m_ui.vecViewList.push_back(m_ui.dynEventViewPnl);
	m_ui.dynEventViewPnl->setVisible(false);

	m_ui.buildingViewPnl = (Layout*)parentPnl->getChildByName("Panel_Building");
	m_ui.vecViewList.push_back(m_ui.buildingViewPnl);
	m_ui.buildingViewPnl->setVisible(false);

	m_ui.actbuildingViewPnl = (Layout*)parentPnl->getChildByName("Panel_ActBuilding");
	m_ui.vecViewList.push_back(m_ui.actbuildingViewPnl);
	m_ui.actbuildingViewPnl->setVisible(false);

	m_ui.dynEventScroll = (ui::ListView*)(Helper::seekWidgetByName(m_ui.dynEventViewPnl, "ListView_Event"));
	ui::ScrollView* scroll = (ui::ScrollView*)m_ui.dynEventScroll;
	scroll->addEventListener(CC_CALLBACK_2(MyGroupLayer::onScrollViewEvent, this));
	m_ui.buildingScroll = (ui::ScrollView*)(Helper::seekWidgetByName(m_ui.buildingViewPnl, "Scroll_build"));
	m_ui.buildingScroll->addEventListener(CC_CALLBACK_2(MyGroupLayer::onScrollViewEvent, this));
	m_ui.actbuildingScroll = (ui::ScrollView*)(Helper::seekWidgetByName(m_ui.actbuildingViewPnl, "Scroll_build"));
	m_ui.actbuildingScroll->addEventListener(CC_CALLBACK_2(MyGroupLayer::onScrollViewEvent, this));

	//m_ui.activityViewImg = (ImageView*)parentPnl->getChildByName("Img_Activity");
	//m_ui.vecViewList.push_back(m_ui.activityViewImg);

	//其它的一些描边和字体设置
	Text* modifyTxt = (Text*)parentImg->getChildByName("Lab_Revise");
	modifyTxt->setFontName(FONT_FZZHENGHEI);
	modifyTxt->setVisible(false);

	//建筑UI滚动条
	m_buildListScrollBar = CustomScrollBar::create();
	m_buildListScrollBar->initScrollBar(m_ui.buildingScroll->getContentSize().height);
	m_buildListScrollBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_buildListScrollBar->setPosition(Point(30, m_ui.buildingScroll->getPositionY() + m_ui.buildingScroll->getContentSize().height/2));
	m_ui.buildingViewPnl->addChild(m_buildListScrollBar);
	m_buildListScrollBar->setScrollBarLengthPercent(1.0f);
	m_buildListScrollBar->setScrollBarPercentY(0.0f);

	//活动建筑UI滚动条
	m_ActbuildListScrollBar = CustomScrollBar::create();
	m_ActbuildListScrollBar->initScrollBar(m_ui.actbuildingScroll->getContentSize().height);
	m_ActbuildListScrollBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ActbuildListScrollBar->setPosition(Point(30, m_ui.actbuildingScroll->getPositionY() + m_ui.actbuildingScroll->getContentSize().height/2));
	m_ui.actbuildingViewPnl->addChild(m_ActbuildListScrollBar);
	m_ActbuildListScrollBar->setScrollBarLengthPercent(1.0f);
	m_ActbuildListScrollBar->setScrollBarPercentY(0.0f);
}

void MyGroupLayer::initFlagListUI()
{
	MyGroupModel* pInstance = MyGroupModel::getInstance();

	//帮派图标弹窗
	m_ui.headIconLayer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIGang/UIGang_Flag.ExportJson"));
	this->addChild(m_ui.headIconLayer, 99);
	m_ui.headIconLayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.headIconLayer->setPosition(this->getContentSize()/2);
	ImageView* parentFlag = (ImageView*)m_ui.headIconLayer->getChildByName("Img_Flag");
	m_ui.headIconScroll = (ui::ScrollView*)parentFlag->getChildByName("ScrollView_Icon");
	m_ui.headIconScroll->addEventListener(CC_CALLBACK_2(MyGroupLayer::onScrollViewEvent, this));

	m_flagListScrollBar = CustomScrollBar::create();
	m_flagListScrollBar->initScrollBar(m_ui.headIconScroll->getContentSize().height);
	m_flagListScrollBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_flagListScrollBar->setPosition(Point(60, m_ui.headIconScroll->getPositionY() + m_ui.headIconScroll->getContentSize().height/2));
	parentFlag->addChild(m_flagListScrollBar);
	m_flagListScrollBar->setScrollBarLengthPercent(1.0f);
	m_flagListScrollBar->setScrollBarPercentY(0.0f);

	m_ui.closeBtn = (Button*)parentFlag->getChildByName("Btn_Close");
	m_ui.closeBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_CLOSEFLAGLAYER));

	DataTable* flagDT = DataManager::getInstance()->getTableEmblem();
	int row = ceil(pInstance->getFlagNum()/4.0f);
	int scrollViewHeight = row * 130+10;
	scrollViewHeight = scrollViewHeight >  m_ui.headIconScroll->getContentSize().height ? scrollViewHeight : m_ui.headIconScroll->getContentSize().height;
	int scrollViewWidth = m_ui.headIconScroll->getInnerContainerSize().width;
	m_ui.headIconScroll->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	//旗帜面板
	m_ui.headIconScroll->removeAllChildren();
	for (int i = 0; i < pInstance->getFlagNum(); i++)
	{
		int rowNumber = i/4;
		int colNumber = i%4;

		Button* item = Button::create(pInstance->getGroupIconPath(i+1));
		item->setAnchorPoint(Vec2(0,0));
		item->setScale(0.5f);
		item->setPosition(Vec2(colNumber*130 + 20, scrollViewHeight-(rowNumber+1)*130+10 ));
		m_ui.headIconScroll->addChild(item, 1, i+1);

		auto call = [this,i](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::ENDED)
			{
				SoundUtils::playSoundEffect("dianji");

				Button* flagBtn = (Button*)sender;
				if(flagBtn->getTag() != MyGroupModel::getInstance()->getMyGroupInfo()->resId)
				{
					//CustomPop::show("当前正在使用该旗帜");
					//return;
					MyGroupController::getInstance()->sendModifyHeadIconMsg(i+1);
				}
				else
				{
					m_ui.headIconLayer->setVisible(false);
				}
				MyGroupModel::getInstance()->setFlagTempID(i+1);		
			}
		};
		item->addTouchEventListener(call);
	}
	
	m_flagListScrollBar->setScrollBarLengthPercent(m_ui.headIconScroll->getContentSize().height / m_ui.headIconScroll->getInnerContainerSize().height);
	m_flagListScrollBar->setScrollBarPercentY(0.0f);

	m_ui.headIconScroll->jumpToTop();

	m_ui.headIconLayer->setVisible(false);
}

//初始化成员列表UI
void MyGroupLayer::initMemListUI()
{
	//成员列表
	m_ui.memItemPnl = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIGang/UIGang_MemberList.ExportJson"));
	this->addChild(m_ui.memItemPnl);
	m_ui.memItemPnl->setVisible(false);

	m_tMemberList = new extension::TableView();
	if(m_tMemberList)
	{
		m_tMemberList->initWithViewSize(m_ui.memListPnl->getContentSize(), NULL);
		m_tMemberList->autorelease();
		m_tMemberList->setDataSource(this);
	}
	m_tMemberList->setDirection(extension::ScrollView::Direction::VERTICAL);
	m_tMemberList->setVerticalFillOrder(extension::TableView::VerticalFillOrder::TOP_DOWN);
	m_tMemberList->reloadData();
	m_tMemberList->setDelegate(this);

	m_tMemberList->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_tMemberList->setPositionY(0);
	m_ui.memListPnl->addChild(m_tMemberList);
}

/*
void MyGroupLayer::scrollViewDidScroll(ScrollView* view)
{
	CCLOG("#######");
}*/

//初始化帮派动态UI
void MyGroupLayer::initDynEventUI()
{
	m_ui.sEventUI.declaration = (Text*)(Helper::seekWidgetByName(m_ui.dynEventViewPnl, "Lab_Public"));
	m_ui.sEventUI.declaration->setString("");
	m_ui.sEventUI.declaration->setFontName(FONT_FZZHENGHEI);

	m_ui.sEventUI.modifyBtn = (Button*)(Helper::seekWidgetByName(m_ui.dynEventViewPnl, "Btn_Revise"));
	m_ui.sEventUI.modifyBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_MODIFY));
	m_ui.sEventUI.modifyBtn->setVisible(false);

	//滚动条
	m_eventListScrollBar = CustomScrollBar::create();
	m_eventListScrollBar->initScrollBar(m_ui.dynEventScroll->getContentSize().height);
	m_eventListScrollBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_eventListScrollBar->setPosition(Point(m_ui.dynEventScroll->getPositionX()+10, 
		m_ui.dynEventScroll->getPositionY() + m_ui.dynEventScroll->getContentSize().height/2));
	m_ui.dynEventViewPnl->addChild(m_eventListScrollBar);
	m_eventListScrollBar->setScrollBarLengthPercent(1.0f);
	m_eventListScrollBar->setScrollBarPercentY(0.0f);
}

//初始化帮派建筑UI
void MyGroupLayer::initBuildingUI()
{
	MyGroupModel* pInstance = MyGroupModel::getInstance();
	int buildNum = pInstance->getGroupBuildInfo()->size();

	//建筑列表
	m_ui.buildiingItemPnl = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIGang/UIGang_BuildList.ExportJson"));
	if(m_ui.buildiingItemPnl->getParent() == nullptr)
	{
		this->addChild(m_ui.buildiingItemPnl);
	}
	m_ui.buildiingItemPnl->setVisible(false);

	int scrollViewHeight = buildNum*(m_ui.buildiingItemPnl->getContentSize().height + GAP_GROUP_BUILDING);
	scrollViewHeight = scrollViewHeight >  m_ui.buildingScroll->getContentSize().height ? scrollViewHeight : m_ui.buildingScroll->getContentSize().height;
	int scrollViewWidth = m_ui.buildingScroll->getInnerContainerSize().width;
	m_ui.buildingScroll->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	m_ui.buildingScroll->removeAllChildren();

	vector<sBuildInfo>* allBuildInfo = pInstance->getGroupBuildInfo();

	for(int i=0; i<buildNum; i++)
	{
		sBuildInfo buildInfo = allBuildInfo->at(i);

		Layout* buildPnl = (Layout*)m_ui.buildiingItemPnl->clone();
		Layout* parent = (Layout*)buildPnl->getChildByName("Pnl_List");
		Layout* buildIcon = (Layout*)parent->getChildByName("Pnl_Building");
		Text*	buildName = (Text*)parent->getChildByName("Lab_Name");
		Text*   buildLv = (Text*)parent->getChildByName("Lab_Level");
		Text*	buildCurEfect = (Text*)parent->getChildByName("Lab_Now");
		Text*	buildNextEfect = (Text*)parent->getChildByName("Lab_Next");
		Button*	lvUpBtn = (Button*)parent->getChildByName("Btn_Up");
		auto call = [this,i](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::ENDED)
			{
				SoundUtils::playSoundEffect("dianji");

				//发送拉取建筑相关信息协议
				MyGroupController::getInstance()->sendGetBuildInfoMsg(MyGroupModel::getInstance()->getGroupBuildInfo()->at(i).type);
				MyGroupModel::getInstance()->setTempBuildType(MyGroupModel::getInstance()->getGroupBuildInfo()->at(i).type);
			}
		};
		lvUpBtn->addTouchEventListener(call);

		ImageView* icon = ImageView::create(pInstance->getBuildIconPath(buildInfo.resId));
		if(icon)
		{
			icon->setPosition(buildIcon->getContentSize()/2);
			buildIcon->addChild(icon);
		}
		buildName->setString(buildInfo.name);
		buildName->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(buildName, Color3B(0x5c, 0x0f, 0x0f), 2);

		buildLv->setString("Lv." + _TO_STR(buildInfo.level));
		if(buildInfo.nextExp == -1)
		{
			//buildLv->setString(STR_UTF8("最高等级"));
			buildLv->setString(DataManager::getInstance()->searchCommonTexdtById(10186));
		}
		buildLv->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(buildLv, Color3B(0x5c, 0x0f, 0x0f), 2);

		buildCurEfect->setString(buildInfo.curEffect);
		buildCurEfect->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(buildCurEfect, Color3B(0x5c, 0x0f, 0x0f), 2);

		buildNextEfect->setString(buildInfo.nextEffect);
		buildNextEfect->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(buildNextEfect, Color3B(0x5c, 0x0f, 0x0f), 2);

		buildPnl->setVisible(true);
		buildPnl->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		buildPnl->setPosition(Vec2(m_ui.buildingScroll->getContentSize().width/2, 
			m_ui.buildingScroll->getInnerContainerSize().height - buildPnl->getContentSize().height/2 - 
			i*(buildPnl->getContentSize().height + GAP_GROUP_BUILDING) - 10.0f));
		m_ui.buildingScroll->addChild(buildPnl);
	}

	m_buildListScrollBar->setScrollBarLengthPercent(m_ui.buildingScroll->getContentSize().height / m_ui.buildingScroll->getInnerContainerSize().height);
	m_buildListScrollBar->setScrollBarPercentY(0.0f);
}

void MyGroupLayer::initAcitvityBuildingUI()
{
	MyGroupModel* pInstance = MyGroupModel::getInstance();
	int buildNum = pInstance->getGroupActivityBuildInfo()->size();

	//建筑列表
	m_ui.actbuildiingItemPnl = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIGang/UIGang_ActBuildList.ExportJson"));
	if(m_ui.actbuildiingItemPnl->getParent() == nullptr)
	{
		this->addChild(m_ui.actbuildiingItemPnl);
	}
	m_ui.actbuildiingItemPnl->setVisible(false);

	int scrollViewHeight = buildNum*(m_ui.actbuildiingItemPnl->getContentSize().height + GAP_GROUP_BUILDING);
	scrollViewHeight = scrollViewHeight >  m_ui.actbuildingScroll->getContentSize().height ? scrollViewHeight : m_ui.actbuildingScroll->getContentSize().height;
	int scrollViewWidth = m_ui.actbuildingScroll->getInnerContainerSize().width;
	m_ui.actbuildingScroll->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	m_ui.actbuildingScroll->removeAllChildren();

	vector<sActivityBuildInfo>* allBuildInfo = pInstance->getGroupActivityBuildInfo();

	for(int i=0; i<buildNum; i++)
	{
		sActivityBuildInfo buildInfo = allBuildInfo->at(i);

		Layout* buildPnl = (Layout*)m_ui.actbuildiingItemPnl->clone();

		Layout* parent = (Layout*)buildPnl->getChildByName("Pnl_List");
		Layout* buildIcon = (Layout*)parent->getChildByName("Pnl_ActIcon");
		Text*	buildName = (Text*)parent->getChildByName("Lab_Name");

		Text*	tips = (Text*)parent->getChildByName("Lab_Now");
		auto buttonSon = (Button*)parent->getChildByName("Btn_Enter");


		for (int j = 0; j < buildInfo.showId.size(); j++)
		{
			Layout* showitem = (Layout*)parent->getChildByName("Pnl_ShowItem"+TO_STR(j+1));

			showitem->removeAllChildren();
			UIToolHeader* _item = UIToolHeader::create(buildInfo.showId.at(j));
			_item->setAnchorPoint(Vec2(0,0));
			_item->setPosition(Vec2(0,0));
			_item->setScale(0.87f);
			_item->setNumEx(-1);//temp.nCount);
			_item->setTipsEnabled(true);
			_item->setToolId(buildInfo.showId.at(j));;
			showitem->addChild(_item);
		}

		buttonSon->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClickByActId,this,i));
		if (i == 0)
		{
			xunlongIn = buttonSon;
		}
		if (i == 1)
		{
			groupBoss = buttonSon;
		}
		ImageView* icon = ImageView::create(pInstance->getBuildIconPath(buildInfo.resId));
		if(icon)
		{
			icon->setPosition(buildIcon->getContentSize()/2);
			buildIcon->addChild(icon);
		}
		buildName->setString(buildInfo.name);
		buildName->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(buildName, Color3B(0x5c, 0x0f, 0x0f), 2);

		tips->setString(buildInfo.tips);
		tips->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(tips, Color3B(0x5c, 0x0f, 0x0f), 2);

		buildPnl->setVisible(true);
		buildPnl->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		buildPnl->setPosition(Vec2(m_ui.actbuildingScroll->getContentSize().width/2, 
			m_ui.actbuildingScroll->getInnerContainerSize().height - buildPnl->getContentSize().height/2 - 
			i*(buildPnl->getContentSize().height + GAP_GROUP_BUILDING) - 10.0f));
		m_ui.actbuildingScroll->addChild(buildPnl);
	}

	m_ActbuildListScrollBar->setScrollBarLengthPercent(m_ui.actbuildingScroll->getContentSize().height / m_ui.actbuildingScroll->getInnerContainerSize().height);
	m_ActbuildListScrollBar->setScrollBarPercentY(0.0f);
}

void MyGroupLayer::initNoticeUI()
{
	m_ui.noticeLayerPnl = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIGang/UIGang_Public.ExportJson"));
	m_ui.noticeLayerPnl->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.noticeLayerPnl->setPosition(this->getContentSize()/2);
	this->addChild(m_ui.noticeLayerPnl, 99);

	ImageView* parentBg = (ImageView*)m_ui.noticeLayerPnl->getChildByName("Img_Public");

	m_ui.btnInside = (Button*)parentBg->getChildByName("Btn_Inside");
	m_ui.btnInside->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_INSIDE_NOTICE));

	m_ui.btnOutSide = (Button*)parentBg->getChildByName("Btn_Outside");
	m_ui.btnOutSide->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_OUTSIDE_NOTICE));

	m_ui.btnSave = (Button*)parentBg->getChildByName("Btn_Save");
	m_ui.btnSave->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_SAVE_NOTICE));

	m_ui.closeBtn = (Button*)parentBg->getChildByName("Btn_Close");
	m_ui.closeBtn->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_CLOSE_NOTICE));

	//利用输入框+TextField来代理文字输入，由于editBox不支持多行显示，所以显示采用Text
	TextField* feildInNotice = (TextField*)parentBg->getChildByName("TextField_Public_In");
	m_ui.editBoxInNotice = EditBoxEx::create(feildInNotice);
	m_ui.editBoxInNotice->setText("");
	m_ui.editBoxInNotice->setFontName(SystemUtils::getFontFileName().c_str());
	m_ui.editBoxInNotice->setVisible(true);
	m_ui.editBoxInNotice->setPosition(parentBg->getContentSize()/2);
	m_ui.editBoxInNotice->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.editBoxInNotice->setDelegate(this);
	parentBg->addChild(m_ui.editBoxInNotice);

	TextField* feildOutNotice = (TextField*)parentBg->getChildByName("TextField_Public_Out");
	m_ui.editBoxOutNotice = EditBoxEx::create(feildOutNotice);
	m_ui.editBoxOutNotice->setText("");
	m_ui.editBoxOutNotice->setFontName(SystemUtils::getFontFileName().c_str());
	m_ui.editBoxOutNotice->setVisible(false);
	m_ui.editBoxOutNotice->setPosition(parentBg->getContentSize()/2);
	m_ui.editBoxOutNotice->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.editBoxOutNotice->setDelegate(this);
	parentBg->addChild(m_ui.editBoxOutNotice);

	m_ui.txtInNotice = (Text*)parentBg->getChildByName("Label_Inside");
	m_ui.txtOutNotice = (Text*)parentBg->getChildByName("Label_Outside");

	m_ui.txtInNotice->setString("");
	m_ui.txtOutNotice->setString("");

	m_ui.txtInNotice->setFontName(FONT_FZZHENGHEI);
	m_ui.txtOutNotice->setFontName(FONT_FZZHENGHEI);

	m_ui.txtInNotice->setVisible(true);
	m_ui.txtOutNotice->setVisible(false);

	m_ui.noticeLayerPnl->setVisible(false);
}

void MyGroupLayer::editBoxEditingDidBegin(extension::EditBox* editBox)
{
	if(m_ui.txtInNotice->isVisible())
	{
		m_ui.editBoxInNotice->setText(m_ui.txtInNotice->getString().c_str());
	}
	else if(m_ui.txtOutNotice->isVisible())
	{
		m_ui.editBoxOutNotice->setText(m_ui.txtOutNotice->getString().c_str());
	}
}

void MyGroupLayer::editBoxReturn(extension::EditBox* editBox)
{
	if(m_ui.txtInNotice->isVisible())
	{
		m_ui.txtInNotice->setString(m_ui.editBoxInNotice->getText());
		m_ui.editBoxInNotice->setText("");
	}
	else if(m_ui.txtOutNotice->isVisible())
	{
		m_ui.txtOutNotice->setString(m_ui.editBoxOutNotice->getText());
		m_ui.editBoxOutNotice->setText("");
	}
}

void MyGroupLayer::updateNoticeUI(eNoticeTabs tabs)
{
	MyGroupModel* pInstance = MyGroupModel::getInstance();

	m_ui.sEventUI.declaration->setString(pInstance->getGroupDeclaration()->insideDec);
	setTextAddStroke(m_ui.sEventUI.declaration, Color3B(0x5c, 0x0f, 0x0f), 2);

	if(pInstance->getMyInfo()->info.power == ePOWERLEADER || pInstance->getMyInfo()->info.power == ePOWERVICE)
	{
		m_ui.sEventUI.modifyBtn->setVisible(true);
	}

	switch (tabs)
	{
	case eNOTICE_TABS_INSIDE:
		{
			m_ui.txtInNotice->setText(pInstance->getGroupDeclaration()->insideDec.c_str());
			m_ui.txtOutNotice->setText(pInstance->getGroupDeclaration()->outsideDec.c_str());
			m_ui.editBoxInNotice->setVisible(true);
			m_ui.editBoxOutNotice->setVisible(false);

			m_ui.btnOutSide->setHighlighted(false);
			m_ui.btnOutSide->setEnabled(true);

			m_ui.btnInside->setHighlighted(true);
			m_ui.btnInside->setEnabled(false);

			m_ui.txtInNotice->setVisible(true);
			m_ui.txtOutNotice->setVisible(false);
		}
		break;
	case eNOTICE_TABS_OUTSIDE:
		{
			m_ui.txtInNotice->setText(pInstance->getGroupDeclaration()->insideDec.c_str());
			m_ui.txtOutNotice->setText(pInstance->getGroupDeclaration()->outsideDec.c_str());
			m_ui.editBoxInNotice->setVisible(false);
			m_ui.editBoxOutNotice->setVisible(true);

			m_ui.btnInside->setHighlighted(false);
			m_ui.btnInside->setEnabled(true);

			m_ui.btnOutSide->setHighlighted(true);
			m_ui.btnOutSide->setEnabled(false);

			m_ui.txtOutNotice->setVisible(true);
			m_ui.txtInNotice->setVisible(false);
		}
		break;
	default:
		break;
	}
}

void MyGroupLayer::update(float dt)
{
	this->unscheduleUpdate();
	Director::getInstance()->replaceScene(MainCityScene::create());
}

void MyGroupLayer::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case BTN_MODIFYHEAD:
			{
				if(MyGroupModel::getInstance()->getMyInfo()->info.power != ePOWERLEADER)
				{
					return;
				}
				//调用修改头像弹窗
				m_ui.headIconLayer->setVisible(true);
				ActionCreator::runCommDlgAction(m_ui.headIconLayer->getChildByName("Img_Flag"));
				setFlagBg();
			}
			break;
		case BTN_CLOSEFLAGLAYER:
			{
				m_ui.headIconLayer->setVisible(false);
			}
			break;
		case BTN_EXIT:
			{
				if(MyGroupModel::getInstance()->getMyInfo()->info.power == ePOWERLEADER)
				{
					//CustomTips::show("帮主不能退出帮派!");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10187));
					return;
				}
				auto call = [this](Ref* ref, CustomRetType type)->void
				{
					if (type == RET_TYPE_OK)
					{
						MyGroupController::getInstance()->sendExitGroupMsg();
					}
				};
				//提示要不要退出帮派
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10188), call, CUSTOM_YES_NO_CANCEL);
			}
			break;
		case BTN_SETTING:
			{
				//更新申请人列表，收到回报后调用弹窗, 就为了个小红点~
				MyGroupController::getInstance()->sendGetApplicantMsg();
			}
			break;
		case BTN_HELP:
			{
				LayerGameRules* groupRule = nullptr;

				groupRule = LayerGameRules::create(GROUP_RULE_ID);

				if (nullptr != groupRule)
				{
					Director::getInstance()->getRunningScene()->addChild(groupRule, 100);
					ActionCreator::runCommDlgAction(groupRule);
				}
			}
			break;
		case BTN_MEMBER_TAB:
			switchTabsType(eTABS_MEMBERLIST_TYPE);
			break;
		case BTN_EVENT_TAB:
			switchTabsType(eTABS_DYNAMICEVENT_TYPE);
			break;
		case BTN_BUILDING_TAB:
			switchTabsType(eTABS_BUILDING_TYPE);
			break;
		case BTN_ACTIVITY_TAB:
			switchTabsType(eTABS_ACTIVITY_TYPE);
			break;
		case BTN_SHOP_TAB:
			Director::getInstance()->pushScene(ShopScene::createScene(ST_SEHGNWANG,true));
			//CustomPop::show("暂未开放!");
			break;
		case BTN_RANK_TAB:
			{
				RankTopModel::getInstance()->setCurRankTopType(GROUP_RANK_TOP);
				Director::getInstance()->pushScene(RankTopScene::createScene(true));
			}
			break;
		case BTN_MODIFY:
			{
				//调用宣言界面弹窗
				m_ui.noticeLayerPnl->setVisible(true);
				updateNoticeUI(eNOTICE_TABS_INSIDE);
				ActionCreator::runCommDlgAction(m_ui.noticeLayerPnl->getChildByName("Img_Public"));
			}
			break;
		case BTN_INSIDE_NOTICE:
			{
				updateNoticeUI(eNOTICE_TABS_INSIDE);
			}
			break;
		case BTN_OUTSIDE_NOTICE:
			{
				updateNoticeUI(eNOTICE_TABS_OUTSIDE);
			}
			break;
		case BTN_SAVE_NOTICE:
			{
				MyGroupModel* pInstance = MyGroupModel::getInstance();
				string tempInNotice = m_ui.txtInNotice->getString();
				string tempOutNotice = m_ui.txtOutNotice->getString();
				pInstance->setTempInNotice(tempInNotice);
				pInstance->setTempOutNotice(tempOutNotice);
				MyGroupController::getInstance()->sendModifyDeclarationMsg(tempInNotice, tempOutNotice);
			}
			break;
		case BTN_CLOSE_NOTICE:
			m_ui.noticeLayerPnl->setVisible(false);
			break;
		default:
			break;
		}
	}
}


void MyGroupLayer::onBtnClickByActId( Ref* ref, Widget::TouchEventType type, int actId )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (actId)
		{
		case 0:
			{
				this->addChild(MyGroupActivity::create());
			}
			break;
		case 1:
			{
				Director::getInstance()->replaceScene(GroupBossScene::create(false));
			}
			break;
		default:
			break;
		}
	}
}


//更新事件列表
void MyGroupLayer::updateEventListUI()
{
	MyGroupModel* pInstance = MyGroupModel::getInstance();

	m_ui.dynEventScroll->removeAllChildren();

	//事件描述
	m_ui.dynEventScroll->jumpToTop();

	vector<sEventDate>* vecEventDate = pInstance->getGroupEventDate();

	Size eventScrollSize(0, 0);
	eventScrollSize.width = m_ui.dynEventScroll->getInnerContainerSize().width;

	for (auto iter = vecEventDate->begin(); iter != vecEventDate->end(); ++iter)
	{
		vector<sDynamicEvent> vecEvent;
		pInstance->getGroupEventByDate(*iter, &vecEvent);

		//日期及底图
		ImageView* dateBg = ImageView::create("Image/Icon/global/img_di3.png");
		string date = _TO_STR(iter->month) + DataManager::getInstance()->searchCommonTexdtById(10189)
			+ _TO_STR(iter->day) + DataManager::getInstance()->searchCommonTexdtById(10190);
		Text* txtDate = Text::create(date, FONT_FZZHENGHEI, fEventContSize);
		dateBg->addChild(txtDate);
		txtDate->setPosition(Point(50, dateBg->getContentSize().height/2));
		m_ui.dynEventScroll->pushBackCustomItem(dateBg);

		eventScrollSize.height += dateBg->getContentSize().height;

		//当日事件
		for(auto eve = vecEvent.begin(); eve != vecEvent.end(); ++eve)
		{
			SimpleRichText* pTextLine = SimpleRichText::create();
			pTextLine->setVerticalSpace(2.0);
			pTextLine->ignoreContentAdaptWithSize(false);//固定大小
			//float textWidth = m_ui.dynEventScroll->getContentSize().width- 20;
			float textWidth = 1200.0f;

			//事件时间 
			string dateContent = "[" + date + " " + eve->eventTime + "]" + " ";
			//事件内容
			string eventDetails = eve->eventContent;
			//每条事件所占高度
			string allContent = dateContent + eventDetails;

			Text* text = Text::create(allContent, FONT_FZZHENGHEI, fEventContSize);
			float fLines = 1.0f;
			fLines += text->getContentSize().width/textWidth;
			fLines = ceil(fLines);
			float w = text->getContentSize().width;
			float textHeight = fLines*(fEventTxtHeight-2);
			pTextLine->setContentSize(Size(textWidth, textHeight));

			//time 时间独立显示
			RichElementText *pTextTime = RichElementText::create(0, cocos2d::Color3B(0xec, 0xc2, 0x70), GLubyte(255), dateContent, FONT_FZZHENGHEI, fEventContSize);
			pTextLine->pushBackElement(pTextTime);

			pTextLine->setString(eve->eventContent, fEventContSize, Color3B::WHITE);				
			pTextLine->formatText();	
			m_ui.dynEventScroll->pushBackCustomItem(pTextLine);

			eventScrollSize.height += pTextLine->getContentSize().height;
		}	
	}

	eventScrollSize.height = eventScrollSize.height > m_ui.dynEventScroll->getContentSize().height ? eventScrollSize.height : m_ui.dynEventScroll->getContentSize().height;
	m_ui.dynEventScroll->setInnerContainerSize(eventScrollSize);

	m_eventListScrollBar->setScrollBarLengthPercent(m_ui.dynEventScroll->getContentSize().height / m_ui.dynEventScroll->getInnerContainerSize().height);
	m_eventListScrollBar->setScrollBarPercentY(0.0f);

	m_ui.dynEventScroll->refreshView();
	//m_ui.dynEventScroll->scrollToBottom(0.5, true);

}

//设置旗帜背景
void MyGroupLayer::setFlagBg()
{
	m_ui.headIconScroll->jumpToTop();
	if(m_ui.headIconScroll->getChildByName("flagTempBg") != nullptr)
	{
		m_ui.headIconScroll->removeChildByName("flagTempBg");
	}
	Button* curFlag = (Button*)m_ui.headIconScroll->getChildByTag(MyGroupModel::getInstance()->getMyGroupInfo()->resId);
	if(curFlag)
	{
		ImageView* imbBg = ImageView::create("Image/Icon/global/img_di7.png");
		if(imbBg)
		{
			imbBg->setPosition(curFlag->getPosition());
			imbBg->setAnchorPoint(Vec2(0.12, 0.24));
			imbBg->setName("flagTempBg");
			m_ui.headIconScroll->addChild(imbBg, -1);
		}
	}
}

//切页签
void MyGroupLayer::switchTabsType(eTabs tabType)
{
	m_eCurTabs = tabType;

	for(auto *btnItem : m_ui.vecTabsBtn)
	{
		btnItem->setEnabled(true);
		btnItem->setHighlighted(false);
	}

	for(auto *viewItem : m_ui.vecViewList)
	{
		viewItem->setVisible(false);
	}

	switch (m_eCurTabs)
	{
	case eTABS_MEMBERLIST_TYPE:
		{
			m_ui.memberListTabBtn->setEnabled(false);
			m_ui.memberListTabBtn->setHighlighted(true);

			m_ui.memListViewPnl->setVisible(true);
			m_tMemberList->reloadData();
		}
		break;
	case eTABS_DYNAMICEVENT_TYPE:
		{
			m_ui.dynEventTabBtn->setEnabled(false);
			m_ui.dynEventTabBtn->setHighlighted(true);

			m_ui.dynEventViewPnl->setVisible(true);

			m_ui.dynEventScroll->jumpToTop();

			if(!m_bIsInittedDynEvent)
			{
				MyGroupController::getInstance()->sendGetDynamicEventMsg();
			}
			else
			{
				updateNoticeUI(eNOTICE_TABS_INSIDE);
				updateEventListUI();
			}
		}
		break;
	case eTABS_BUILDING_TYPE:
		{
			m_ui.buildingTabBtn->setEnabled(false);
			m_ui.buildingTabBtn->setHighlighted(true);

			m_ui.buildingViewPnl->setVisible(true);

			m_ui.buildingScroll->jumpToTop();

			initBuildingUI();

			if(!m_bIsInittedBuilding)
			{
				//MyGroupController::getInstance()->sendGetBuildMsg();
			}
		}
		break;
	case eTABS_ACTIVITY_TYPE:
		{
			m_ui.activityTabBtn->setEnabled(false);
			m_ui.activityTabBtn->setHighlighted(true);

			m_ui.actbuildingViewPnl->setVisible(true);

			m_ui.actbuildingScroll->jumpToTop();

			initAcitvityBuildingUI();
			setActivityFlag();
		}
		break;
	default:
		break;
	}
}

//设置滚动层cell内容
void  MyGroupLayer::setCellContent(extension::TableViewCell* cell, ssize_t idx)
{
	Layout* item = (Layout*)cell->getChildByTag(TAG_ITEM_LIST);
	if(!item)
	{
		return;
	}
	Layout* parent = (Layout*)item->getChildByName("Pnl_ManageList");
	Layout* pnlHead = (Layout*)parent->getChildByName("Pnl_HeadIcon");
	Text*	playerName = (Text*)parent->getChildByName("Lab_PlayerName");
	ImageView* vipLv = (ImageView*)parent->getChildByName("Img_VIP");
	Text*	playerLv = (Text*)parent->getChildByName("Lab_PlayerLevel");
	Text*	playerPost = (Text*)parent->getChildByName("Lab_Job");
	Text*	playerContri = (Text*)parent->getChildByName("Lab_Contribution");
	Text*	playerOnline = (Text*)parent->getChildByName("Lab_LoginTime");
	
	pnlHead->removeAllChildren();				//清空头像容器

	playerName->setString("");
	playerName->setFontName(FONT_FZZHENGHEI);

	playerLv->setString("");
	playerLv->setFontName(FONT_FZZHENGHEI);

	playerPost->setString("");
	playerPost->setFontName(FONT_FZZHENGHEI);

	playerContri->setString("");
	playerContri->setFontName(FONT_FZZHENGHEI);

	playerOnline->setString("");
	playerOnline->setFontName(FONT_FZZHENGHEI);

	MyGroupModel* pInstance = MyGroupModel::getInstance();

	vector<sPlayerInfo>* vecPlayerInfo = pInstance->getAllPlayerInfo();
	if(idx >= vecPlayerInfo->size())
	{
		return;
	}

	sPlayerInfo playerInfo = vecPlayerInfo->at(idx);

	//根据idx来排列列表
	UIToolHeader* header = UIToolHeader::create(playerInfo.iconResId);
	header->setAnchorPoint(Point(0.5, 0.5));
	header->setPosition(pnlHead->getContentSize()/2);
	header->setScale(0.5);
	pnlHead->addChild(header);

	playerName->setString(playerInfo.name);
	string strVipLvFile = "Image/UIVip/VipLv/btn_VIP" + _TO_STR(playerInfo.vipLevel) + ".png";
	vipLv->loadTexture(strVipLvFile);
	playerLv->setString(_TO_STR(playerInfo.level));
	playerPost->setString(pInstance->getPostPowerName(playerInfo.power));
	playerContri->setString(_TO_STR(playerInfo.contributeVal));
	playerOnline->setString(pInstance->getLastLoginTimeDes(playerInfo.uId));
	if(playerInfo.isOnLine)
	{
		playerOnline->setColor(Color3B(0x8b, 0xe7, 0x40));
	}
	else
	{
		playerOnline->setColor(Color3B(0x82, 0x6A, 0x56));
	}
	setTextAddStroke(playerName, Color3B(0x7d, 0x3f, 0x1c), 2);
	setTextAddStroke(playerPost, Color3B(0x7d, 0x3f, 0x1c), 2);
	setTextAddStroke(playerContri, Color3B(0x7d, 0x3f, 0x1c), 2);
}


//滑动事件
void MyGroupLayer::onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	if(pSender == m_ui.headIconScroll)
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.headIconScroll->getInnerContainerSize();
				Vec2 vec = m_ui.headIconScroll->getInnerContainer()->getPosition();

				//滚动条重设进度
				float percentY = -vec.y / (size.height - m_ui.headIconScroll->getContentSize().height);
				m_flagListScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);			
			}
			break;

		default:
			break;
		}
	}
	else if(pSender == m_ui.dynEventScroll)
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.dynEventScroll->getInnerContainerSize();
				Vec2 vec = m_ui.dynEventScroll->getInnerContainer()->getPosition();

				//滚动条重设进度
				float percentY = -vec.y / (size.height - m_ui.dynEventScroll->getContentSize().height);
				m_eventListScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);		
			}
			break;
	
		default:
			break;
		}
	}
	else if(pSender == m_ui.buildingScroll)
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.buildingScroll->getInnerContainerSize();
				Vec2 vec = m_ui.buildingScroll->getInnerContainer()->getPosition();

				//滚动条重设进度
				float percentY = -vec.y / (size.height - m_ui.buildingScroll->getContentSize().height);
				m_buildListScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);	
			}
			break;
		default:
			break;
		}
	}
	else if(pSender == m_ui.actbuildingScroll)
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.actbuildingScroll->getInnerContainerSize();
				Vec2 vec = m_ui.actbuildingScroll->getInnerContainer()->getPosition();

				//滚动条重设进度
				float percentY = -vec.y / (size.height - m_ui.actbuildingScroll->getContentSize().height);
				m_ActbuildListScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);	
			}
			break;
		default:
			break;
		}
	}
}

//实现基类函数
void MyGroupLayer::tableCellTouched(extension::TableView* table, extension::TableViewCell* cell)
{
	int idx = cell->getIdx();
	int uId = MyGroupModel::getInstance()->getAllPlayerInfo()->at(idx).uId;
	//调用用户信息接口
	Layer* layer = MyGroupPlayerInfoLayer::create(uId);
	this->addChild(layer, 1);
	ImageView* uiBg = (ImageView*)((Layout*)layer->getChildByName("UI_ROOT"))->getChildByName("Img_Information");
	ActionCreator::runCommDlgAction(uiBg);
}

Size MyGroupLayer::cellSizeForTable(extension::TableView *table)
{
	return m_ui.memItemPnl->getContentSize();
}

extension::TableViewCell* MyGroupLayer::tableCellAtIndex(extension::TableView *table, ssize_t idx)
{
	extension::TableViewCell* cell = table->dequeueCell();
	if(!cell)
	{
		//创建cell
		cell = new extension::TableViewCell();
		cell->autorelease();

		Layout* item = (Layout*)m_ui.memItemPnl->clone();
		item->setPosition(cell->getContentSize()/2);
		item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		item->setVisible(true);
		cell->addChild(item, 1, TAG_ITEM_LIST);
	}

	//设置cell
	setCellContent(cell, idx);

	return cell;
}

ssize_t MyGroupLayer::numberOfCellsInTableView(extension::TableView *table)
{
	MyGroupModel* pInstance = MyGroupModel::getInstance();
	return pInstance->getAllPlayerInfo()->size();
}


//更新公共UI
void MyGroupLayer::updateCommonUI()
{
	MyGroupModel* pInstance = MyGroupModel::getInstance();
	sMyGroupData* groupInfo = pInstance->getMyGroupInfo();
	sMyInfo* myInfo = pInstance->getMyInfo();

	//帮派头像
	if(m_ui.groupIconPnl->getChildByName("groupHeadIcon") != nullptr)
	{
		m_ui.groupIconPnl->removeChildByName("groupHeadIcon");
	}
	ImageView*	header = ImageView::create();
	header->setAnchorPoint(Point(0.5, 0.5));
	header->setPosition(Point(m_ui.groupIconPnl->getContentSize()/2) + Point(0, -40));
	header->loadTexture(MyGroupModel::getInstance()->getGroupIconPath(groupInfo->resId));
	header->setTouchEnabled(true);
	header->addTouchEventListener(CC_CALLBACK_2(MyGroupLayer::onBtnClick, this, BTN_MODIFYHEAD));
	m_ui.groupIconPnl->addChild(header);
	header->setName("groupHeadIcon");

	m_ui.groupLv->setString("Lv." + _TO_STR(groupInfo->level));
	m_ui.groupName->setString(groupInfo->name);
	m_ui.groupLv->setPositionX(m_ui.groupName->getPositionX() + m_ui.groupName->getContentSize().width + 10.0f);
	string strId = "(ID:" + _TO_STR(groupInfo->id) + ")";
	m_ui.groupId->setString(strId);
	m_ui.leaderName->setString(groupInfo->leaderName);
	m_ui.memberNum->setString(_TO_STR(groupInfo->curMemberNum) + "/" + _TO_STR(groupInfo->totalMemberNum));
	m_ui.myPostPower->setString(pInstance->getPostPowerName(myInfo->info.power));
	m_ui.groupRank->setString(_TO_STR(groupInfo->rank));

	setTextAddStroke(m_ui.groupLv, Color3B(0x7d, 0x3f, 0x1c), 2);
	setTextAddStroke(m_ui.groupName, Color3B(0x7d, 0x3f, 0x1c), 2);
	setTextAddStroke(m_ui.groupId, Color3B(0x7d, 0x3f, 0x1c), 2);

	setTextAddStroke(m_ui.leaderName, Color3B(0x5c, 0x0f, 0x0f), 2);
	setTextAddStroke(m_ui.memberNum, Color3B(0x5c, 0x0f, 0x0f), 2);
	setTextAddStroke(m_ui.myPostPower, Color3B(0x5c, 0x0f, 0x0f), 2);
	setTextAddStroke(m_ui.groupRank, Color3B(0x5c, 0x0f, 0x0f), 2);

	//管理按钮
	if(myInfo->info.power == ePOWERLEADER || myInfo->info.power == ePOWERVICE)
	{
		m_ui.settingBtn->setVisible(true);
		setJoinInFlag();
	}
	else
	{
		m_ui.settingBtn->setVisible(false);
	}

	if(myInfo->info.power == ePOWERLEADER)
	{
		ImageView* parentImg = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_Main"));
		Text* modifyTxt = (Text*)parentImg->getChildByName("Lab_Revise");
		modifyTxt->setVisible(true);
	}
}

//更新帮派旗帜
void MyGroupLayer::updateHeadIcon()
{
	m_ui.headIconLayer->setVisible(false);

	MyGroupModel* pInstance = MyGroupModel::getInstance();
	ImageView* head = (ImageView*)m_ui.groupIconPnl->getChildByName("groupHeadIcon");
	head->loadTexture(pInstance->getGroupIconPath(pInstance->getMyGroupInfo()->resId));
}

//更新成员列表
void MyGroupLayer::updateMemberListUI()
{
	m_tMemberList->reloadData();
}

//更新建筑列表
void MyGroupLayer::updateBuildingUI()
{
	//更新帮派等级
	m_ui.groupLv->setString(_TO_STR(MyGroupModel::getInstance()->getMyGroupInfo()->level));
	setTextAddStroke(m_ui.groupLv, Color3B(0x7d, 0x3f, 0x1c), 2);

	//更新帮派建筑列表数据
	vector<sBuildInfo>* buildInfo = MyGroupModel::getInstance()->getGroupBuildInfo();	
	Vector<Node*> buildList = m_ui.buildingScroll->getChildren();
	for(auto &build : buildList)
	{
		Layout* parent = (Layout*)((Layout*)build)->getChildByName("Pnl_List");
		Text*	buildName = (Text*)parent->getChildByName("Lab_Name");
		Text*   buildLv = (Text*)parent->getChildByName("Lab_Level");
		Text*	buildCurEfect = (Text*)parent->getChildByName("Lab_Now");
		Text*	buildNextEfect = (Text*)parent->getChildByName("Lab_Next");

		for(auto &info : *buildInfo)
		{
			if(buildName->getString() == info.name)
			{
				buildLv->setString("Lv." + _TO_STR(info.level));
				buildCurEfect->setString(info.curEffect);
				buildNextEfect->setString(info.nextEffect);

				setTextAddStroke(buildLv, Color3B(0x5c, 0x0f, 0x0f), 2);
				setTextAddStroke(buildCurEfect, Color3B(0x5c, 0x0f, 0x0f), 2);
				setTextAddStroke(buildNextEfect, Color3B(0x5c, 0x0f, 0x0f), 2);
				break;
			}
		}
	}
}

//更新活动列表
void MyGroupLayer::updateActivityUI()
{

}

void MyGroupLayer::doExitFromGroup(eExitGroupEvent event)
{
	auto call = [this](Ref* ref, CustomRetType type)->void
	{
		//Director::getInstance()->replaceScene(MainCityScene::create());
		Director::getInstance()->replaceScene(JoinGroupScene::create());
	};

	switch (event)
	{
	case eEXIT_LEAVE_EVENT:
		//CustomPop::show("你已退出帮派!", call, CUSTOM_ID_YES);
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10191), call, CUSTOM_ID_YES);
		break;
	case eEXIT_EXPEL_EVENT:
		//CustomPop::show("很遗憾, 你已被逐出帮派!", call, CUSTOM_ID_YES);
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10192), call, CUSTOM_ID_YES);
		break;
	case eEXIT_DISBANDED_EVENT:
		//CustomPop::show("很遗憾, 你所在的帮派已被解散!", call, CUSTOM_ID_YES);
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10193), call, CUSTOM_ID_YES);
		break;
	case eEXIT_DISMISS_EVENT:	
		//CustomPop::show("帮派已解散, 请加入其它帮派!", call, CUSTOM_ID_YES);
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10194), call, CUSTOM_ID_YES);
		break;
	default:
		break;
	}
}

void MyGroupLayer::addBuildLayer()
{
	//下一帧创建添加建筑层，防止监听着迭代器崩溃

	auto callBack = [this](float dt)->void
	{
		MyGroupModel* pInstance = MyGroupModel::getInstance();

		Layer* layer = MyGroupBuildLayer::create(pInstance->getTempBuildType());
		this->addChild(layer, 1);
		ImageView* uiBg = (ImageView*)((Layout*)layer->getChildByName("UI_ROOT"))->getChildByName("Img_Building");
		ActionCreator::runCommDlgAction(uiBg);

		this->getScheduler()->unschedule("MyGroupLayer::addBuildLayer", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "MyGroupLayer::addBuildLayer");
}

void MyGroupLayer::addSettingLayer()
{
	//下一帧创建添加管理设置层，防止监听着迭代器崩溃

	auto callBack = [this](float dt)->void
	{
		Layer* layer = MyGroupSetLayer::create();
		this->addChild(layer, 1);
		ImageView* uiBg = (ImageView*)((Layout*)layer->getChildByName("UI_ROOT"))->getChildByName("Img_Set_0");
		ActionCreator::runCommDlgAction(uiBg);

		this->getScheduler()->unschedule("MyGroupLayer::addSettingLayer", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "MyGroupLayer::addSettingLayer");
}

void MyGroupLayer::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;
	if(!obParam)
	{
		return;
	}
	
	switch (obParam->id)
	{
	case nMAIN_CMD_GROUP_MAIN_INFO:
		{
			updateCommonUI();
			switchTabsType(m_eCurTabs);
			setJoinInFlag();
			setActivityFlag();

			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_GROUP_MAIN);
		}
		break;
	case nMAIN_CMD_GROUP_CHANGE_ICON:
		{
			updateHeadIcon();
		}
		break;
	case nMAIN_CMD_GROUP_DYN_EVENT:
		{
			updateNoticeUI(eNOTICE_TABS_INSIDE);
			updateEventListUI();
		}
		break;
	case nMAIN_CMD_GROUP_MODIFY_NOTICE:
		{
			m_ui.sEventUI.declaration->setString(MyGroupModel::getInstance()->getGroupDeclaration()->insideDec);
			setTextAddStroke(m_ui.sEventUI.declaration, Color3B(0x5c, 0x0f, 0x0f), 2);
			//CustomTips::show("修改成功!");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10195));
		}
		break;
	case nMAIN_CMD_GROUP_EXIT:
	case nMAIN_CMD_GROUP_DISBAND:
	case nMAIN_CMD_GROUP_PUSH:
		{	
			eExitGroupEvent event = eEXIT_DEFAULT_EVENT;
			if(obParam->id == nMAIN_CMD_GROUP_EXIT)
			{
				event = eEXIT_LEAVE_EVENT;		//主动退出帮派
			}
			else if(obParam->id == nMAIN_CMD_GROUP_DISBAND)
			{
				event = eEXIT_DISMISS_EVENT;	//解散帮派
			}
			else
			{
				int type = MyGroupModel::getInstance()->getFlagPushMsg();
				if(type == 1 || type == 3 || type == 4)
				{
					MyGroupController::getInstance()->sendGetGroupMainMsg();
					return;
				}
				if(type == 2)
				{
					event = eEXIT_EXPEL_EVENT;	//被逐出帮派
				}
				else if(type == 5)
				{
					event = eEXIT_DISBANDED_EVENT;	//被解散了帮派
				}
			}

			doExitFromGroup(event);
		}
		break;
	case nMAIN_CMD_GROUP_BUILD_INFO:
		{
			//调用建筑弹窗 
			addBuildLayer();
		}
		break;
	case nMAIN_CMD_GROUP_BUILD_DONATE:
		{	
			updateBuildingUI();
		}
		break;
	case nMAIN_CMD_GROUP_APPLICANT_LIST:
		{
			addSettingLayer();
		}
	case nMAIN_CMD_GROUP_ACTIVITY_REFRESH:
	case nMAIN_CMD_GROUP_ACTIVITY_CLICK:
		{
			setActivityFlag();
		}
		break;
	default:
		break;
	}
}


void MyGroupLayer::setJoinInFlag()
{
	//int mVecSize = 0;
	//Vectorm_ui.settingBtn
	int mVecSize = MyGroupModel::getInstance()->getAllProposerInfo()->size();
	Node* FriendFlag = m_ui.settingBtn->getChildByName("mail_flg");
	if (!FriendFlag)
	{
		FriendFlag = cocos2d::ui::ImageView::create("Image/Icon/global/img_dian.png");
		FriendFlag->setPosition(cocos2d::Vec2(m_ui.settingBtn->getContentSize().width - FriendFlag->getContentSize().width/2, 
			m_ui.settingBtn->getContentSize().height - FriendFlag->getContentSize().height/2));
		FriendFlag->setName("mail_flg");
		m_ui.settingBtn->addChild(FriendFlag, 9);
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


void MyGroupLayer::setActivityFlag()
{

	auto isfree = MyGroupModel::getInstance()->getGroupActivityInfo()->showedIdflag.size();

	//活动按钮是进入按钮
	Node* FriendFlag = m_ui.activityTabBtn->getChildByName("mail_flg");
	if (!FriendFlag)
	{

		FriendFlag = cocos2d::ui::ImageView::create("Image/Icon/global/img_dian.png");
		FriendFlag->setPosition(cocos2d::Vec2(m_ui.activityTabBtn->getContentSize().width - FriendFlag->getContentSize().width/2, 
			m_ui.activityTabBtn->getContentSize().height - FriendFlag->getContentSize().height/2));
		FriendFlag->setName("mail_flg");
		m_ui.activityTabBtn->addChild(FriendFlag, 9);
		//进入按钮


	}
	if (isfree == 0)
	{
		FriendFlag->setVisible(true);
	}
	else
	{
		FriendFlag->setVisible(false);
	}

	if (xunlongIn)
	{
		Node* FriendFlag1 = xunlongIn->getChildByName("mail_flg");
		if (!FriendFlag1)
		{
			FriendFlag1 = cocos2d::ui::ImageView::create("Image/Icon/global/img_dian.png");
			FriendFlag1->setPosition(cocos2d::Vec2(xunlongIn->getContentSize().width - FriendFlag1->getContentSize().width/2, 
				xunlongIn->getContentSize().height - FriendFlag1->getContentSize().height/2));
			FriendFlag1->setName("mail_flg");
			xunlongIn->addChild(FriendFlag1, 9);
		}
		if (isfree == 0)
		{
			FriendFlag1->setVisible(true);
		}
		else
		{
			FriendFlag1->setVisible(false);
		}
	}
}

