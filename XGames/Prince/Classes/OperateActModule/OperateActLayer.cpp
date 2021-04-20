#include "OperateActLayer.h"
#include "Widget/ActionCreator.h"
#include "Widget/LayerGameRules.h"
#include "RankTopModule/RankTopScene.h"
#include "Temp/CustomGoldTips.h"
#include "RankTopModule/RankTopModel.h"
#include "TakeCardModule/SceneTakeCard.h"
#include "ChapterModule/Activity/SceneActivity.h"

OperateActLayer::OperateActLayer()
{
	OperateActModel::getInstance()->addObserver(this);
	m_OpListLicaiBar = nullptr;
	m_OpSevenBar = nullptr;  //七日列表
	m_OpNationWellBar = nullptr;  //全民福利列表
	m_OpComomABar = nullptr;
	m_OpRankABar = nullptr;
	m_pCommonTabView = nullptr;
	m_sOprateItemData = nullptr;
	isFirstCheckBox = true;
	m_bIsSelect = false;
	isFirstOperateType = OPERATE_ACT_TYPE_NONE;
	chooseCommonOprateId = -1;
	m_vipCheckBoxIdx = 0;
	m_OpListViewPos = Vec2::ZERO;
	m_operatLocalSort.clear();
	m_operatSort.clear();
	m_isHavaSuperFriend = false;
	m_isShowSuperFriendTiShi = true;
	m_isChooseRemaday = false;
}

OperateActLayer::~OperateActLayer()
{
	OperateActModel::getInstance()->removeObserver(this);
	LocalTimer::getInstance()->removeLocalTimerListener(this);
}

bool OperateActLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUi();
	setAllPnlOut();
	return true;
}

void OperateActLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIProActivity/UIProActivity.ExportJson"));
	this->addChild(m_ui.pRoot);	
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");

	auto backCall = [](Ref* sender, Widget::TouchEventType type)->void
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
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_OPERATE_ACT);
	m_ui.pRoot->getChildByName("ProActivity")->addChild(_layerCommHeadInfo);



	m_ui.Img_diList = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_di"));
	m_ui.Img_diListPos = m_ui.Img_diList->getPosition();
	m_ui.OpListView = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.Img_diList,"Op_ListView"));
	m_ui.ImgUp = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Img_diList,"Img_Up"));
	m_ui.ImgDown = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Img_diList,"Img_Down"));
	m_ui.ImgUp->setVisible(true);
	m_ui.ImgDown->setVisible(true);
	dynamic_cast<ui::ScrollView*>(m_ui.OpListView)->addEventListener(CC_CALLBACK_2(OperateActLayer::onScrollViewEvent, this));

	//#1 七日礼包
	m_ui.PnlSeven = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Seven"));
	m_ui.PnlSevenPos = m_ui.PnlSeven->getPosition();
	m_ui.SevenCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlSeven,"Lab_Count"));
	m_ui.SVSevenList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.PnlSeven,"SV_Seven"));
	m_ui.SevenCount->setFontName(FONT_FZZHENGHEI);
	ImageView* ImgSevendi2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlSeven,"Img_di2"));
	m_OpSevenBar = CustomScrollBar::create();
	m_OpSevenBar->initScrollBar(m_ui.SVSevenList->getContentSize().height);
	m_OpSevenBar->setPosition(m_ui.SVSevenList->getPosition() + Point(-12, 0));
	ImgSevendi2->addChild(m_OpSevenBar);
	m_OpSevenBar->setScrollBarLengthPercent(1.0f);
	m_OpSevenBar->setScrollBarPercentY(0.0f);
	dynamic_cast<ui::ScrollView*>(m_ui.SVSevenList)->addEventListener(CC_CALLBACK_2(OperateActLayer::onScrollViewEvent, this));

	//#2 月卡
	m_ui.PnlCard = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Card"));
	m_ui.PnlCardPos = m_ui.PnlCard->getPosition();
	m_ui.LabCardTime = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlCard,"Lab_Time"));
	m_ui.BtnCardBuy1 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.PnlCard,"Btn_Buy1"));
	m_ui.BtnCardBuy2 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.PnlCard,"Btn_Buy2"));
	m_ui.ImgState_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlCard,"ImgState_1"));
	m_ui.ImgState_2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlCard,"ImgState_2"));
	m_ui.LabCardTime->setFontName(FONT_FZZHENGHEI);
	ImageView *Cardicon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlCard,"icon"));
	Cardicon->loadTexture("Image/UIProActivity/Image/10002.png");
	ImageView *CardImgitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlCard,"Img_Title"));
	CardImgitle->loadTexture("Image/UIProActivity/Image/img_di5.png");

	//#3 战力排行
	m_ui.PnlPowerRank = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_PowerRank"));
	m_ui.PnlPowerRankPos = m_ui.PnlPowerRank->getPosition();
	m_ui.rankTimeLab = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlPowerRank,"Lab_Time"));
	m_ui.rankListView = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.PnlPowerRank,"ListView"));
	ImageView *Img_rankListdi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlPowerRank,"Img_di3"));
	ImageView *Rankicon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlPowerRank,"icon"));
	Rankicon->loadTexture("Image/UIProActivity/Image/10009.png");
	ImageView *RankImgTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlPowerRank,"Img_Title"));
	RankImgTitle->loadTexture("Image/UIProActivity/Image/img_di5.png");
	m_OpRankABar = CustomScrollBar::create();
	m_OpRankABar->initScrollBar(m_ui.rankListView->getContentSize().height);
	m_OpRankABar->setPosition(m_ui.rankListView->getPosition() + Point(-12, 0));
	Img_rankListdi->addChild(m_OpRankABar);
	m_OpRankABar->setScrollBarLengthPercent(1.0f);
	m_OpRankABar->setScrollBarPercentY(0.0f);
	dynamic_cast<ui::ScrollView*>(m_ui.rankListView)->addEventListener(CC_CALLBACK_2(OperateActLayer::onScrollViewEvent, this));
	
	m_ui.rankBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.PnlPowerRank,"Btn_Rank"));
	m_ui.rankMyRank = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlPowerRank,"Lab_MyRank"));
	m_ui.rankMyPower = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlPowerRank,"Lab_MyPower"));
	m_ui.rankTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlPowerRank,"Lab_Tips"));
	m_ui.rankTimeLab->setFontName(FONT_FZZHENGHEI);
	m_ui.rankMyRank->setFontName(FONT_FZZHENGHEI);
	m_ui.rankMyPower->setFontName(FONT_FZZHENGHEI);
	m_ui.rankTips->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.rankTips,Color3B(0xac,0x03,0x03),2);

	//#4 Vip礼包
	m_ui.PnlShowSuperFriend = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Show"));
	m_ui.PnlShowSuperFriendPos = m_ui.PnlShowSuperFriend->getPosition();
	m_ui.imgSuperFriend = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Image_hero"));
	m_ui.pBtnCall = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Btn_Vip10"));

	//#5 理财通
	m_ui.PnlGrown = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Grown"));
	m_ui.PnlGrownPos = m_ui.PnlGrown->getPosition();
	ImageView *Img_growndi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlGrown,"Img_di"));
	m_ui.grownButBuy = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.PnlGrown,"Btn_Buy"));
	m_ui.grownSVGrown = dynamic_cast<ui::ScrollView*>(Helper::seekWidgetByName(m_ui.PnlGrown,"SV_Grown"));
	m_ui.growState = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlGrown,"state"));
	ImageView *Growicon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlGrown,"icon"));
	Growicon->loadTexture("Image/UIProActivity/Image/10006.png");
	ImageView *GrowImgTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlGrown,"Img_Title"));
	GrowImgTitle->loadTexture("Image/UIProActivity/Image/img_di5.png");

	m_OpListLicaiBar = CustomScrollBar::create();
	m_OpListLicaiBar->initScrollBar(m_ui.grownSVGrown->getContentSize().height);
	m_OpListLicaiBar->setPosition(m_ui.grownSVGrown->getPosition() + Point(-12, 0));
	Img_growndi->addChild(m_OpListLicaiBar);
	m_OpListLicaiBar->setScrollBarLengthPercent(1.0f);
	m_OpListLicaiBar->setScrollBarPercentY(0.0f);
	m_ui.grownSVGrown->addEventListener(CC_CALLBACK_2(OperateActLayer::onScrollViewEvent, this));
	

	//#6 全民福利
	m_ui.PnlBenefit = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Benefit"));
	m_ui.PnlBenefitPos = m_ui.PnlBenefit->getPosition();
	m_ui.bebefitCount  = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlBenefit,"Lab_Count"));
	m_ui.benefitSVBenefit = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.PnlBenefit,"SV_Benefit"));
	m_ui.bebefitCount->setFontName(FONT_FZZHENGHEI);
	ImageView *Img_Benefitdi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlBenefit,"Img_di2"));
	m_OpNationWellBar = CustomScrollBar::create();
	m_OpNationWellBar->initScrollBar(m_ui.benefitSVBenefit->getContentSize().height);
	m_OpNationWellBar->setPosition(m_ui.benefitSVBenefit->getPosition() + Point(-12, 0));
	Img_Benefitdi->addChild(m_OpNationWellBar);
	m_OpNationWellBar->setScrollBarLengthPercent(1.0f);
	m_OpNationWellBar->setScrollBarPercentY(0.0f);
	dynamic_cast<ui::ScrollView*>(m_ui.benefitSVBenefit)->addEventListener(CC_CALLBACK_2(OperateActLayer::onScrollViewEvent, this));

	//#通用
	m_ui.PnlCommon = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Generic"));
	m_ui.PnlCommonPos = m_ui.PnlCommon->getPosition();
	m_ui.PnlTbView = (Layout*)m_ui.PnlCommon->getChildByName("Pnl_tbView");
	m_ui.imgTile = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlCommon,"title"));
	ImageView *Img_Title = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlCommon,"Img_Title"));
	Img_Title->loadTexture("Image/UIProActivity/Image/img_di5.png");
	m_ui.Img_TimeA = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlCommon,"Img_TimeA"));
	m_ui.labTime1A = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlCommon,"Lab_TimeA"));
	m_ui.imgTimeB = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlCommon,"Img_TimeB"));
	m_ui.labTimeB = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlCommon,"Lab_TimeB"));
	m_ui.CommonTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlCommon,"Lab_Tips"));
	m_ui.labTime1A->setFontName(FONT_FZZHENGHEI);
	m_ui.labTimeB->setFontName(FONT_FZZHENGHEI);
	m_ui.CommonTips->setFontName(FONT_FZZHENGHEI);
	ImageView *Img_PnlCommondi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlCommon,"Img_di2"));
	m_OpComomABar = CustomScrollBar::create();

	//#12 双倍掉落
	m_ui.Pnl_Double = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Double"));
	m_ui.PnlDoublePos = m_ui.Pnl_Double->getPosition();
	m_ui.doubleImgTips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Pnl_Double,"Img_Tips1"));
	m_ui.doubleTexTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Pnl_Double,"Lab_Tips3"));
	m_ui.doubleTexTime = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Pnl_Double,"Lab_Time"));
	m_ui.doubleBtnGo = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Pnl_Double,"Btn_Go"));
	m_ui.doubleTexTips->setFontName(FONT_FZZHENGHEI);
	m_ui.doubleTexTime->setFontName(FONT_FZZHENGHEI);

	//#13限时抢购
	m_ui.Pnl_Purchase = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Purchase"));
	m_ui.PnlPurchasePos = m_ui.Pnl_Purchase->getPosition();
	m_ui.purTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Pnl_Purchase,"title"));
	m_ui.purImgTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Pnl_Purchase,"Img_Title"));
	m_ui.purTime = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Pnl_Purchase,"Lab_Time"));
	m_ui.PurTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Pnl_Purchase,"Lab_Tips"));
	m_ui.purTime->setFontName(FONT_FZZHENGHEI);
	m_ui.PurTips->setFontName(FONT_FZZHENGHEI);
	m_ui.pruImgItem[0] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Pnl_Purchase,"Img_itemA"));
	m_ui.pruImgItem[1] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Pnl_Purchase,"Img_itemB"));
	m_ui.pruImgItem[2] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Pnl_Purchase,"Img_itemC"));

	//#14 超值礼包
	m_ui.PnlVipGift = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_VipGift"));
	m_ui.PnlVipGiftPos = m_ui.PnlVipGift->getPosition();
	m_ui.imgVipGiftTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"title"));
	m_ui.imgVipGiftBigTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"Img_Title"));
	m_ui.vipGiftTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"Lab_Tips"));
	m_ui.vipGiftTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"Lab_Tips2"));
	m_ui.vipGiftLabTime = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"Lab_Time"));
	m_ui.vipGiftLabPrice = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"Lab_Price"));
	m_ui.vipGiftLabGiftName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"Lab_GiftName"));
	m_ui.vipGiftLabVipPrice = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"Lab_VipPrice"));
	m_ui.vipGiftLabVip = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"Lab_Vip"));
	m_ui.vipHead = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"head"));
	m_ui.vipGiftTips->setFontName(FONT_FZZHENGHEI);
	m_ui.vipGiftTips2->setFontName(FONT_FZZHENGHEI);
	m_ui.vipGiftLabTime->setFontName(FONT_FZZHENGHEI);
	m_ui.vipGiftLabPrice->setFontName(FONT_FZZHENGHEI);
	m_ui.vipGiftLabGiftName->setFontName(FONT_FZZHENGHEI);
	m_ui.vipGiftLabVipPrice->setFontName(FONT_FZZHENGHEI);
	m_ui.vipGiftLabVip->setFontName(FONT_FZZHENGHEI);
	m_ui.vipGiftBuy = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"Btn_Buy"));
	m_ui.vipGiftHaveBuy = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"haveBuy"));
	m_ui.vipGiftMyVip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"Img_MyVip"));
	m_ui.ListView_VIP = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"ListView_VIP"));
	m_ui.VipCheck = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.PnlVipGift,"CheckBox"));
	m_ui.VipCheck->setVisible(false);

	operatSort();

	m_ui.PnlCommonItem = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIProActivity/UIItem.ExportJson"));
	m_ui.PnlCommonItem->setVisible(false);
	this->addChild(m_ui.PnlCommonItem, -1);

	//tableview
	m_pCommonTabView = extension::TableView::create(this, m_ui.PnlTbView->getContentSize(), nullptr);
	m_pCommonTabView->setDirection(extension::ScrollView::Direction::VERTICAL);
	m_pCommonTabView->setVerticalFillOrder(extension::TableView::VerticalFillOrder::TOP_DOWN);
	m_pCommonTabView->reloadData();
	m_pCommonTabView->setDelegate(this);
	m_pCommonTabView->setBounceable(true);
	m_pCommonTabView->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pCommonTabView->setPosition(Point(10, 0));
	m_ui.PnlTbView->addChild(m_pCommonTabView);


	//设置通用列表滚动条
	ImageView* CommonImgSevendi2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlCommon,"Img_di2"));
	m_OpComomABar->initScrollBar(m_pCommonTabView->getViewSize().height);
	m_OpComomABar->setPosition(m_ui.PnlTbView->getPosition() + Point(-12, 0));
	m_OpComomABar->setScrollBarLengthPercent(1.0f);
	m_OpComomABar->setScrollBarPercentY(0.0f);

	CommonImgSevendi2->addChild(m_OpComomABar, 99);

	m_OpComomABar->setScrollBarLengthPercent(m_pCommonTabView->getViewSize().height / m_pCommonTabView->getContentSize().height);
	m_OpComomABar->setScrollBarPercentY(0.0f);
}

void OperateActLayer::onEnter()
{
	Layer::onEnter();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(OperateActLayer::updateNextFram),this,0,false);

	//是否购买月卡或者尊卡
	if ((OperateActModel::getInstance()->getIsBuyMonthCard() || OperateActModel::getInstance()->getIsBuyExtremeCard()) 
		&& m_isChooseRemaday)
	{
		OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_MONTH_CARD);
		item->isInit = false;

		updateRemadayInfo();
	}

	//左边滑动按钮复原
	if (m_ui.OpListView->getItems().size() > 0)
	{
		m_ui.OpListView->getInnerContainer()->setPosition(m_OpListViewPos);
	}

	//如果有最强伙伴活动且这次切换到VIP界面有买到了那个英雄就要重置界面
	if (m_isHavaSuperFriend && !isSuperFriendExhibition())
	{
		m_isHavaSuperFriend = false;
		reSetOperate();
	}
}

void OperateActLayer::updateNextFram(float ft)
{
	OperateActController::getInstance()->getOprateSuppleMentTable();
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(OperateActLayer::updateNextFram), this);
}

void OperateActLayer::onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	switch(type)
	{
	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			if (pSender == m_ui.grownSVGrown)
			{
				Size size = m_ui.grownSVGrown->getInnerContainerSize();
				Vec2 vec = m_ui.grownSVGrown->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.grownSVGrown->getContentSize().height);

				m_OpListLicaiBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			else if (pSender == m_ui.SVSevenList)
			{
				Size size = m_ui.SVSevenList->getInnerContainerSize();
				Vec2 vec = m_ui.SVSevenList->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.SVSevenList->getContentSize().height);

				m_OpSevenBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			else if (pSender == m_ui.benefitSVBenefit)
			{
				Size size = m_ui.benefitSVBenefit->getInnerContainerSize();
				Vec2 vec = m_ui.benefitSVBenefit->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.benefitSVBenefit->getContentSize().height);

				m_OpNationWellBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			else if (pSender == m_ui.rankListView)
			{
				Size size = m_ui.rankListView->getInnerContainerSize();
				Vec2 vec = m_ui.rankListView->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.rankListView->getContentSize().height);

				m_OpRankABar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			else if (pSender == m_ui.OpListView)
			{
				m_OpListViewPos = m_ui.OpListView->getInnerContainer()->getPosition();
			}
		}
		break;
	default:
		break;
	}
}

void OperateActLayer::onExit()
{
	cocostudio::GUIReader::destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIProActivity");

	Layer::onExit();
}

OPERATE_ACT_SORT_ITEM* OperateActLayer::getAllOperatActivtyItemById(int id)
{
	for (auto iter = m_operatSort.begin(); iter != m_operatSort.end(); iter++)
	{
		if (iter->id == id)
		{
			return &(*iter);
		}
	}

	return nullptr;
}

OPERATE_ACT_SORT_ITEM* OperateActLayer::getAllOperatActivtyItemByTaskId(int taskId)
{
	for (auto iter = m_operatSort.begin(); iter != m_operatSort.end(); iter++)
	{
		for (auto _iter = iter->info.begin(); _iter != iter->info.end(); _iter++)
		{
			if (_iter->id == taskId)
			{
				return &(*iter);
			}
		}
	}

	return nullptr;
}

void OperateActLayer::setOperatListEndTime()
{
	int openTime = OperateActModel::getInstance()->getOpenTime();//开启
	int createTime = OperateActModel::getInstance()->getCreateTime();//创建
	int localTime = LocalTimer::getInstance()->getCurServerTime();

	for (auto iter = m_operatSort.begin(); iter != m_operatSort.end(); iter++)
	{
		OPERATE_ACT_SORT_ITEM& item = *iter;
		int trueOpenTime = 0;
		if (item.timeType == OPERATE_TIME_TYPE_SEVER)
		{
			trueOpenTime = openTime + item.openTime*24*3600;
		}
		else if (item.timeType == OPERATE_TIME_TYPE_CREATE)
		{
			trueOpenTime = createTime + item.openTime*24*3600;
		}

		if (item.timeType == OPERATE_TIME_TYPE_SEVER || item.timeType == OPERATE_TIME_TYPE_CREATE) //开服时间
		{
			if (item.endTime <= -1)
			{
				item.daojiTime = 0;
			}
			else
			{
				item.daojiTime = trueOpenTime + item.endTime - localTime;
			}
		}
		else if (item.timeType == OPERATE_TIME_TYPE_LIMIT)
		{
			item.daojiTime = item.endTime - localTime;
		}
	}
}

bool OperateActLayer::isNeedStopTime(int &maxDaojishi)
{
	bool ret = true;
	for (auto iter = m_operatSort.begin(); iter != m_operatSort.end(); iter++)
	{
		if (iter->daojiTime > 0)
		{
			maxDaojishi = maxDaojishi > iter->daojiTime ? maxDaojishi:iter->daojiTime ;
			ret = false;
		}
	}

	return ret;
}

void OperateActLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	
	switch (observerParam->id)
	{
	case OPRATEACT_EVENT_TYPE_GETTABLE:
		{
			//本地服务器合并
			m_operatSort.clear();
			for (auto iter = m_operatLocalSort.begin(); iter != m_operatLocalSort.end(); iter++)
			{
				m_operatSort.push_back(*iter);
			}

			VEV_OPERATE_ACT_SORT_ITEM OperatActivty = OperateActModel::getInstance()->getOperatActivty();
			for (int i = 0; i < OperatActivty.size(); i++)
			{
				m_operatSort.push_back(OperatActivty.at(i));
			}

			sort(m_operatSort.begin(),m_operatSort.end(),[](OPERATE_ACT_SORT_ITEM a,OPERATE_ACT_SORT_ITEM b)
			{
				return a.tagIdx < b.tagIdx;
			});

			OperateActController::getInstance()->getOprateActInfo();
		}
		break;
	case OPRATEACT_EVENT_TYPE_INFO:
		{
			setOperatListEndTime();
			setOperatListView(false,true);

			int maxDaojishi = 0;
			if (!isNeedStopTime(maxDaojishi))
			{
				setEndTimeStamp(LocalTimer::getInstance()->getCurServerTime() + maxDaojishi);
				LocalTimer::getInstance()->addLocalTimerListener(this);
			}
			else
			{
				//停止监听器
				setEndTimeStamp(0);
			}
		}
		break;
	case OPRATEACT_EVENT_TYPE_GET_PRIZE:
		{
			showPopReward(OperateActModel::getInstance()->getRewardShow());
			int taskId = OperateActModel::getInstance()->getCurTaskId();
			OPERATE_ACT_SORT_ITEM* item = getAllOperatActivtyItemByTaskId(taskId);
			if (item)
			{
				switch (item->type)
				{
				case OPERATE_ACT_TYPE_SEVEN_DAY: //七日礼包
					{
						vector<int>& ActTyp1 = OperateActModel::getInstance()->getActTyp1();
						ActTyp1.push_back(taskId);

						if (isSevenExhibition())
						{
							OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_SEVEN_DAY);
							item->isInit = false;
							updateSevenInfo();
						}
						else
						{
							reSetOperate();
						}
					}
					break;
				case OPERATE_ACT_TYPE_SUPER_FRIEND://强力伙伴
					{

					}
					break;
				case OPERATE_ACT_TYPE_FINANCIAL: //理财通
					{
						vector<int>& ActTyp5 = OperateActModel::getInstance()->getActTyp5();
						ActTyp5.push_back(taskId);

						if (isFinacialExhibition())
						{
							OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_FINANCIAL);
							item->isInit = false;

							updateFinacialInfo(true);
						}
						else
						{
							reSetOperate();
						}
					}
					break;
				case OPERATE_ACT_TYPE_NATIONAL_WELL_BEING://全民福利
					{
						vector<int>& ActTyp6 = OperateActModel::getInstance()->getActTyp6();
						ActTyp6.push_back(taskId);

						if (isNationWellExhibition())
						{
							OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_NATIONAL_WELL_BEING);
							item->isInit = false;

							updateNationWellInfo();
						}
						else
						{
							reSetOperate();
						}
					}
					break;
				case OPERATE_ACT_TYPE_COMMOMA://7- 通用模版A（每日5点重置）
					{
						COMMONA* common  = OperateActModel::getInstance()->getComon7ById(chooseCommonOprateId);
						common->actTyp.push_back(taskId);

						if (isCommomAExhibition(chooseCommonOprateId))
						{
							updateCommomAInfo(chooseCommonOprateId);
						}
						else
						{
							reSetOperate();
						}
					}
					break;
				case OPERATE_ACT_TYPE_COMMOMA_://8- 通用模版A_
					{
						COMMONA* common  = OperateActModel::getInstance()->getComon8ById(chooseCommonOprateId);
						common->actTyp.push_back(taskId);

						if (_isCommomAExhibition(chooseCommonOprateId))
						{
							_updateCommomAInfo(chooseCommonOprateId);
						}
						else
						{
							reSetOperate();
						}
					}
					break;
				case OPERATE_ACT_TYPE_COMMOMB://9- 通用模版B（每日5点重置）
					{
						COMMONB* common  = OperateActModel::getInstance()->getComon9ById(chooseCommonOprateId);
						common->actTyp.push_back(taskId);
						if (isCommomBExhibition(chooseCommonOprateId))
						{
							updateCommomBInfo(chooseCommonOprateId);
						}
						else
						{
							reSetOperate();
						}
					}
					break;
				case OPERATE_ACT_TYPE_COMMOMB_://10- 通用模版B_
					{
						COMMONB* common  = OperateActModel::getInstance()->getComon10ById(chooseCommonOprateId);
						common->actTyp.push_back(taskId);
						if (_isCommomBExhibition(chooseCommonOprateId))
						{
							_updateCommomBInfo(chooseCommonOprateId);
						}
						else
						{
							reSetOperate();
						}
					}
					break;
				case OPERATE_ACT_TYPE_SINGLE_RECHARGE: //11- 单笔充值
					{
						SINGLE_RECHARGE* ActInfo11  = OperateActModel::getInstance()->getActInfo11();
						ActInfo11->actTyp.push_back(taskId);
						if (isSingleRechargeExhibition())
						{
							updateSingleRechargeInfo();
						}
						else
						{
							reSetOperate();
						}
					}
					break;
				default:
					break;
				}
			}

			//红点
			setOperateTishi();
		}
		break;
	case OPRATEACT_EVENT_TYPE_FINACIAL:
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20234));
			OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_FINANCIAL);
			item->isInit = false;

			updateFinacialInfo();
			setOperateTishi();
		}
		break;
	case OPRATEACT_EVENT_TYPE_VIPGIFT:
		{
			showPopReward(OperateActModel::getInstance()->getRewardShow());

			if (isVipGiftExhibition())
			{
				OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_VIPBAG);
				item->isInit = false;

				updateVipGiftInfo();
			}
			else
			{
				reSetOperate();
			}

			//红点
			setOperateTishi();
		}
		break;
	default:
		break;
	}
}

void OperateActLayer::timeTick(int dt)
{
	int openTime = OperateActModel::getInstance()->getOpenTime();//开启
	int createTime = OperateActModel::getInstance()->getCreateTime();//创建
	int localTime = LocalTimer::getInstance()->getCurServerTime();

	for (auto iter = m_operatSort.begin(); iter != m_operatSort.end(); iter++)
	{
		if (iter->daojiTime >= dt)
		{
			iter->daojiTime -= dt;
			
			if (iter->daojiTime <= 0)
			{
				reSetOperate(true); //有为零的重置界面
			}

			switch (iter->type)
			{
			case OPERATE_ACT_TYPE_FIGHT_RANK:
				{
					m_ui.rankTimeLab->setString(StringFormat::convertSecToFormatTime(iter->daojiTime));
					setTextAddStroke(m_ui.rankTimeLab,Color3B(0x09,0x4e,0x51),2);
				}
				break;
			case OPERATE_ACT_TYPE_COMMOMA://7- 通用模版A（每日5点重置）
				{
					if (chooseCommonOprateId == iter->id)
					{
						m_ui.labTime1A->setString(StringFormat::convertSecToFormatTime(iter->daojiTime) + "("+
							DataManager::getInstance()->searchCommonTexdtById(20235)+ "）");
						setTextAddStroke(m_ui.labTime1A,Color3B(0x09,0x4e,0x51),2);
					}
				}
				break;
			case OPERATE_ACT_TYPE_COMMOMA_://8- 通用模版A_
				{
					if (chooseCommonOprateId == iter->id)
					{
						m_ui.labTimeB->setString(StringFormat::convertSecToFormatTime(iter->daojiTime));
						setTextAddStroke(m_ui.labTimeB,Color3B(0x09,0x4e,0x51),2);
					}
				}
				break;
			case OPERATE_ACT_TYPE_COMMOMB://9- 通用模版B（每日5点重置）
				{
					if (chooseCommonOprateId == iter->id)
					{
						m_ui.labTime1A->setString(StringFormat::convertSecToFormatTime(iter->daojiTime) +  "("+
						DataManager::getInstance()->searchCommonTexdtById(20235) + "）");
						setTextAddStroke(m_ui.labTime1A,Color3B(0x09,0x4e,0x51),2);
					}
				}
				break;
			case OPERATE_ACT_TYPE_COMMOMB_://10- 通用模版B_
				{
					if (chooseCommonOprateId == iter->id)
					{
						m_ui.labTimeB->setString(StringFormat::convertSecToFormatTime(iter->daojiTime));
						setTextAddStroke(m_ui.labTimeB,Color3B(0x09,0x4e,0x51),2);
					}
				}
				break;
			case OPERATE_ACT_TYPE_VIPBAG://超值礼包
				{
					m_ui.vipGiftLabTime->setString(StringFormat::convertSecToFormatTime(iter->daojiTime));
					setTextAddStroke(m_ui.vipGiftLabTime,Color3B(0x09,0x4e,0x51),2);
				}
				break;
			default:
				break;
			}
		}
	}

	int maxDaojishi = 0;
	if (isNeedStopTime(maxDaojishi))
	{
		//停止监听器
		setEndTimeStamp(0);
	}
}

void OperateActLayer::operatSort()
{
	//信息表
	multimap<int, RowData>& DToperatMap = DataManager::getInstance()->getDTOperateActivitySort()->getTableData();

	//遍历数据表
	m_operatLocalSort.clear();
	for(auto iter = DToperatMap.begin(); iter != DToperatMap.end(); iter++)
	{
		RowData* rowData = &(iter->second);

		OPERATE_ACT_TYPE type = (OPERATE_ACT_TYPE)rowData->getIntData("actTyp");
		if ((type == OPERATE_ACT_TYPE_MONTH_CARD &&	!MainModel::getInstance()->getMonthCardIsOpen()) ||
			rowData->getIntData("data") == 1)  //苹果审核 超值月卡暂不出现
		{
			continue;
		}

		OPERATE_ACT_SORT_ITEM operateItem;
		operateItem.id = rowData->getIntData("id");
		operateItem.tips = rowData->getStringData("tips");
		operateItem.type = rowData->getIntData("actTyp");
		operateItem.tagIdx = rowData->getIntData("tag");
		operateItem.timeType = rowData->getIntData("timeTyp");
		operateItem.serverTime = rowData->getIntData("serverTime");
		operateItem.roleTime = rowData->getIntData("roleTime");
		operateItem.openTime = rowData->getIntData("openTime");
		operateItem.endTime = rowData->getIntData("endTime");
		operateItem.sign = rowData->getIntData("sign");
		operateItem.res = rowData->getIntData("res");
		operateItem.disappear = rowData->getIntData("disappear");
		operateItem.tips2 = rowData->getStringData("tips2");
		operateItem.daojiTime = 0;
		operateItem.isInit = false;

		multimap<int, RowData>& operateTable = DataManager::getInstance()->getDTOperateActivity()->getTableData(); 
		//遍历数据表
		for(auto iter = operateTable.begin(); iter != operateTable.end(); iter++)
		{
			RowData* rowData = &(iter->second);
			if (rowData->getIntData("actTyp") == operateItem.id)
			{
				OPERATE_ACT_INFO_ITEM item;
				item.id = rowData->getIntData("id"); //奖励id
				item.tips = rowData->getStringData("tips");
				item.actTyp = rowData->getIntData("actTyp");
				item.refTyp = rowData->getIntData("refTyp");
				item.amount1 = rowData->getIntData("amount1");
				item.amount2 = rowData->getIntData("amount2");
				item.price = rowData->getStringData("price");
				item.actInfo = rowData->getStringData("actInfo");

				vector<string> vecDst;
				StringFormat::parseCsvStringVec(rowData->getStringData("reward"), vecDst);
				for (int k = 0; k < vecDst.size(); k++ )
				{
					vector<int> _vecDst;
					StringFormat::parseCsvStringVecBySemi(vecDst[k], _vecDst);

					OPERATEA_PRIZE_ITEM prize;
					prize.id = _vecDst[0];
					prize.amount = _vecDst[1];
					item.reward.push_back(prize);
				}

				operateItem.info.push_back(item);
			}
		}

		m_operatLocalSort.push_back(operateItem);
	}
}

void OperateActLayer::setOperatListView(bool isEndTimeOut,bool isPnlIn )
{
	if (isFirstOperateType != OPERATE_ACT_TYPE_NONE)
	{
		return;
	}

	//找出右边应该显示那种面板
	for(int i = 0; i < m_operatSort.size(); i++)
	{
		OPERATE_ACT_SORT_ITEM item = m_operatSort.at(i);
		if (isFirstOperateType == OPERATE_ACT_TYPE_NONE)
		{
			switch (item.type)
			{
			case OPERATE_ACT_TYPE_SEVEN_DAY: //七日礼包
				{
					if (isSevenExhibition(isEndTimeOut)) 
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_SEVEN_DAY;
						break;
					}
				}
				break;
			case OPERATE_ACT_TYPE_MONTH_CARD: //超值月卡
				{
					chooseCommonOprateId = item.id;
					isFirstOperateType = OPERATE_ACT_TYPE_MONTH_CARD;
					break;
				}
				break;
			case OPERATE_ACT_TYPE_FIGHT_RANK: //战力排行
				{
					if (isFightRankExhibition(isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_FIGHT_RANK;
					}
					break;
				}
				break;
			case OPERATE_ACT_TYPE_SUPER_FRIEND://强力伙伴
				{
					if (isSuperFriendExhibition(isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_SUPER_FRIEND;
					}
					break;
				}
				break;
			case OPERATE_ACT_TYPE_FINANCIAL: //理财通
				{
					if (isFinacialExhibition(isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_FINANCIAL;
						break;
					}
				}
				break;
			case OPERATE_ACT_TYPE_NATIONAL_WELL_BEING://全民福利
				{
					if (isNationWellExhibition(isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_NATIONAL_WELL_BEING;
						break;
					}
				}
				break;
			case OPERATE_ACT_TYPE_COMMOMA://通用模版A（每日5点重置）
				{
					if (isCommomAExhibition(item.id,isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_COMMOMA;
						break;
					}
				}
				break;
			case OPERATE_ACT_TYPE_COMMOMA_://通用模版A_
				{
					if (_isCommomAExhibition(item.id,isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_COMMOMA_;
						break;
					}
				}
				break;
			case OPERATE_ACT_TYPE_COMMOMB://通用模版B（每日5点重置）
				{
					if (isCommomBExhibition(item.id,isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_COMMOMB;
						break;
					}
				}
				break;
			case OPERATE_ACT_TYPE_COMMOMB_://通用模版B_
				{
					if (_isCommomBExhibition(item.id,isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_COMMOMB_;
						break;
					}
				}
				break;
			case OPERATE_ACT_TYPE_DOUBLE_FALL: //双倍掉落
				{
					if (isDoubleFallExhibition(item.id,isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_DOUBLE_FALL;
						break;
					}
				}
				break;
			case OPERATE_ACT_TYPE_SINGLE_RECHARGE://单笔充值C
				{
					if (isSingleRechargeExhibition(isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_SINGLE_RECHARGE;
						break;
					}
				}
				break;
			case OPERATE_ACT_TYPE_VIPBAG://超值礼包
				{
					if (isVipGiftExhibition(isEndTimeOut))
					{
						chooseCommonOprateId = item.id;
						isFirstOperateType = OPERATE_ACT_TYPE_VIPBAG;
					}
					break;
				}
				break;
			default:
				break;
			}
		}
	}

	//左边节点
	m_ui.OpListView->removeAllChildren();
	for(int i = 0; i < m_operatSort.size(); i++)
	{
		OPERATE_ACT_SORT_ITEM item = m_operatSort.at(i);

		string strFileName;
		strFileName.assign("Image/UIProActivity/UIProActivity_Item.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		switch (item.type)
		{
		case OPERATE_ACT_TYPE_SEVEN_DAY: //七日礼包
			{
				if (!isSevenExhibition(isEndTimeOut)) 
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_FIGHT_RANK://最强战力
			{
				if (!isFightRankExhibition(isEndTimeOut))
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_SUPER_FRIEND://强力伙伴
			{
				if (!isSuperFriendExhibition(isEndTimeOut))
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_FINANCIAL: //理财通
			{
				if (!isFinacialExhibition(isEndTimeOut))
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_NATIONAL_WELL_BEING://全民福利
			{
				if (!isNationWellExhibition(isEndTimeOut))
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_COMMOMA://通用模版A（每日5点重置）
			{
				if (!isCommomAExhibition(item.id,isEndTimeOut))
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_COMMOMA_://通用模版A_
			{
				if (!_isCommomAExhibition(item.id,isEndTimeOut))
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_COMMOMB://通用模版B（每日5点重置）
			{
				if (!isCommomBExhibition(item.id,isEndTimeOut))
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_COMMOMB_://通用模版B_
			{
				if (!_isCommomBExhibition(item.id,isEndTimeOut))
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_DOUBLE_FALL: //双倍掉落
			{
				if (!isDoubleFallExhibition(item.id,isEndTimeOut))
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_SINGLE_RECHARGE://单笔充值
			{
				if (!isSingleRechargeExhibition(isEndTimeOut))
				{
					continue;
				}
			}
			break;
		case OPERATE_ACT_TYPE_VIPBAG://超值礼包
			{
				if (!isVipGiftExhibition(isEndTimeOut))
				{
					continue;
				}
			}
			break;
		default:
			break;
		}
		
		CheckBox *check = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(layer,"proItme"));
		ImageView *dian = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"dian"));
		Text *name = dynamic_cast<Text*>(Helper::seekWidgetByName(check,"name"));
		name->setFontName(FONT_FZZHENGHEI);
		name->setColor(Color3B(0xea,0xc1,0x71));
		name->setString(item.tips);
		setTextAddStroke(name,Color3B(0x77,0x00,0x00),2);
		auto callCheck = [this,check,dian,item](Ref* sender,CheckBox::EventType type)->void
		{
			if (type == CheckBox::EventType::SELECTED)
			{
				chooseCommonOprateId = item.id;

				SoundUtils::playSoundEffect("dianji");
				check->setTouchEnabled(false);

				Vector<Widget*>& Items = m_ui.OpListView->getItems();
				for (auto a:Items)
				{
					CheckBox *_check = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(a,"proItme"));
					if (_check != check)
					{
						_check->setTouchEnabled(true);
						_check->setSelectedState(false);

						Text *name = dynamic_cast<Text*>(Helper::seekWidgetByName(_check,"name"));
						name->setColor(Color3B(0xea,0xc1,0x71));
						setTextAddStroke(name,Color3B(0x77,0x00,0x00),2);
					}
					else
					{
						Text *name = dynamic_cast<Text*>(Helper::seekWidgetByName(_check,"name"));
						name->setColor(Color3B(0xf0,0xf5,0x64));
						setTextAddStroke(name,Color3B(0x9a,0x09,0x09),2);
					}
				}
				m_isChooseRemaday = false;
				switch (item.type)
				{
				case OPERATE_ACT_TYPE_SEVEN_DAY: //七日礼包
					{
						updateSevenInfo();
					}
					break;
				case OPERATE_ACT_TYPE_MONTH_CARD: //超值月卡
					{
						m_isChooseRemaday = true;
						updateRemadayInfo();
					}
					break;
				case OPERATE_ACT_TYPE_FIGHT_RANK: //战力排行
					{
						updateFightRankInfo();
					}
					break;
				case OPERATE_ACT_TYPE_SUPER_FRIEND://强力伙伴
					{
						m_isShowSuperFriendTiShi = false;
						dian->setVisible(false);

						updateSuperFriendInfo();
					}
					break;
				case OPERATE_ACT_TYPE_FINANCIAL: //理财通
					{
						updateFinacialInfo();
					}
					break;
				case OPERATE_ACT_TYPE_NATIONAL_WELL_BEING://全民福利
					{
						updateNationWellInfo();
					}
					break;
				case OPERATE_ACT_TYPE_COMMOMA://通用模版A（每日5点重置）
					{
						updateCommomAInfo(item.id);
					}
					break;
				case OPERATE_ACT_TYPE_COMMOMA_://通用模版A_
					{			
						_updateCommomAInfo(item.id);
					}
					break;
				case OPERATE_ACT_TYPE_COMMOMB://通用模版B（每日5点重置）
					{
						updateCommomBInfo(item.id);
					}
					break;
				case OPERATE_ACT_TYPE_COMMOMB_://通用模版B_
					{			
						_updateCommomBInfo(item.id);
					}
					break;
				case OPERATE_ACT_TYPE_DOUBLE_FALL: //双倍掉落
					{
						updateDoubleFallInfo(item.id);
					}
					break;
				case OPERATE_ACT_TYPE_SINGLE_RECHARGE://单笔充值
					{
						updateSingleRechargeInfo();
					}
					break;
				case OPERATE_ACT_TYPE_LIMIT_BUY:
					{
						updateLimitBuyInfo();
					}
					break;
				case OPERATE_ACT_TYPE_VIPBAG://超值礼包
					{
						updateVipGiftInfo();
					}
					break;
				default:
					break;
				}
			}
		};
		check->addEventListener(callCheck);
		if (isFirstCheckBox) //默认显示第一个
		{
			isFirstCheckBox = false;
			check->setSelectedState(true);
			check->setTouchEnabled(false);
			name->setColor(Color3B(0xf0,0xf5,0x64));
			name->setString(item.tips);
			setTextAddStroke(name,Color3B(0x9a,0x09,0x09),2);
		}

		layer->setName(TO_STR(item.type) + "|" + TO_STR(item.id) + "|" + TO_STR(item.sign)); //需要保存type 和 id 没办法
		m_ui.OpListView->pushBackCustomItem(layer);
	}

	if (isPnlIn)
	{
		setAllPnlIn();
	}
	//红点
	setOperateTishi();
	setOperateActive();

	m_ui.OpListView->refreshView();
	m_ui.OpListView->scrollToTop(0.01f,false);
}

void OperateActLayer::setOperateTishi()
{
	for(int i = 0; i < m_ui.OpListView->getItems().size(); i++)
	{
		Widget *layer = m_ui.OpListView->getItems().at(i);
		ImageView *dian = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"dian"));
		ImageView *doublefall = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"doublefall"));
		dian->setVisible(false);
		doublefall->setVisible(false);

		string name = layer->getName();
		vector<int> vecName;
		StringFormat::parseCsvStringVec(name, vecName);

		switch (vecName.at(0))
		{
		case OPERATE_ACT_TYPE_SEVEN_DAY: //七日礼包
			{
				if (isSevenExhibition() && isSevenHave()) 
				{
					dian->setVisible(true);
				}
			}
			break;
		case OPERATE_ACT_TYPE_SUPER_FRIEND://强力伙伴
			{
				if (isSuperFriendExhibition() && m_isShowSuperFriendTiShi)
				{
					dian->setVisible(true);
					m_isHavaSuperFriend = true;
				}
			}
			break;
		case OPERATE_ACT_TYPE_FINANCIAL: //理财通
			{
				if (isFinacialExhibition() && isFinacialHave())
				{
					dian->setVisible(true);
				}
			}
			break;
		case OPERATE_ACT_TYPE_NATIONAL_WELL_BEING://全民福利
			{
				if (isNationWellExhibition() && isNationWellHave())
				{
					dian->setVisible(true);
				}
			}
			break;
		case OPERATE_ACT_TYPE_COMMOMA://通用模版A（每日5点重置）
			{
				if (isCommomAExhibition(vecName.at(1)) && isCommomAHave(vecName.at(1)))
				{
					dian->setVisible(true);
				}
			}
			break;
		case OPERATE_ACT_TYPE_COMMOMA_://通用模版A_
			{
				if (_isCommomAExhibition(vecName.at(1)) && _isCommomAHave(vecName.at(1)))
				{
					dian->setVisible(true);
				}
			}
			break;
		case OPERATE_ACT_TYPE_COMMOMB://通用模版B（每日5点重置）
			{
				if (isCommomBExhibition(vecName.at(1)) && isCommomBHave(vecName.at(1)))
				{
					dian->setVisible(true);
				}
			}
			break;
		case OPERATE_ACT_TYPE_COMMOMB_://通用模版B_
			{
				if (_isCommomBExhibition(vecName.at(1)) && _isCommomBHave(vecName.at(1)))
				{
					dian->setVisible(true);
				}
			}
			break;
		case OPERATE_ACT_TYPE_SINGLE_RECHARGE: //单笔充值
			{
				if (isSingleRechargeExhibition() && isSingleRechargeHave())
				{
					dian->setVisible(true);
				}
			}
			break;
		case OPERATE_ACT_TYPE_DOUBLE_FALL:
			{
				doublefall->setVisible(true);
				doublefall->loadTexture("Image/UIProActivity/Image/" + TO_STR(vecName.at(2)) + ".png");
			}
			break;
		case OPERATE_ACT_TYPE_VIPBAG://超值礼包
			{
				if (isVipGiftExhibition() && isVipGiftHave())
				{
					dian->setVisible(true);
				}
			}
			break;
		default:
			break;
		}
	}
}
void OperateActLayer::setOperateActive()
{
	switch (isFirstOperateType)
	{
	case OPERATE_ACT_TYPE_SEVEN_DAY: //七日礼包
		{
			updateSevenInfo();
		}
		break;
	case OPERATE_ACT_TYPE_MONTH_CARD: //超值月卡
		{
			updateRemadayInfo();
		}
		break;
	case OPERATE_ACT_TYPE_FIGHT_RANK: //战力排行
		{
			updateFightRankInfo();
		}
		break;
	case OPERATE_ACT_TYPE_SUPER_FRIEND://强力伙伴
		{
			updateSuperFriendInfo();
		}
		break;
	case OPERATE_ACT_TYPE_FINANCIAL: //理财通
		{
			updateFinacialInfo();
		}
		break;
	case OPERATE_ACT_TYPE_NATIONAL_WELL_BEING://全民福利
		{
			updateNationWellInfo();
		}
		break;
	case OPERATE_ACT_TYPE_COMMOMA://通用模版A（每日5点重置）
		{
			updateCommomAInfo(chooseCommonOprateId);
		}
		break;
	case OPERATE_ACT_TYPE_COMMOMA_://通用模版A_
		{			
			_updateCommomAInfo(chooseCommonOprateId);
		}
		break;
	case OPERATE_ACT_TYPE_COMMOMB://通用模版B（每日5点重置）
		{
			updateCommomBInfo(chooseCommonOprateId);
		}
		break;
	case OPERATE_ACT_TYPE_COMMOMB_://通用模版B_
		{			
			_updateCommomBInfo(chooseCommonOprateId);
		}
		break;
	case OPERATE_ACT_TYPE_DOUBLE_FALL: //双倍掉落
		{
			updateDoubleFallInfo(chooseCommonOprateId);
		}
		break;
	case OPERATE_ACT_TYPE_SINGLE_RECHARGE://单笔充值
		{
			updateSingleRechargeInfo();
		}
		break;
	case OPERATE_ACT_TYPE_LIMIT_BUY:
		{
			updateLimitBuyInfo();
		}
		break;
	case OPERATE_ACT_TYPE_VIPBAG://超值礼包
		{
			updateVipGiftInfo();
		}
		break;
	default:
		break;
	}
}

void OperateActLayer::reSetOperate(bool isEndTimeOut)
{
	isFirstCheckBox = true;
	chooseCommonOprateId = -1;
	isFirstOperateType = OPERATE_ACT_TYPE_NONE;
	setOperatListView(isEndTimeOut);
}

void OperateActLayer::closeAllPanel()
{
	m_ui.PnlSeven->setVisible(false);
	m_ui.PnlCard->setVisible(false);
	m_ui.PnlPowerRank->setVisible(false);
	m_ui.PnlShowSuperFriend->setVisible(false);
	m_ui.PnlGrown->setVisible(false);
	m_ui.PnlBenefit->setVisible(false);
	m_ui.PnlCommon->setVisible(false);
	m_ui.Pnl_Double->setVisible(false);
	m_ui.Pnl_Purchase->setVisible(false);
	m_ui.PnlVipGift->setVisible(false);
}

void OperateActLayer::setAllPnlOut()
{
	Size size = Director::getInstance()->getVisibleSize();
	m_ui.Img_diList->setPosition(Vec2(-m_ui.Img_diList->getContentSize().width/2,m_ui.Img_diListPos.y));
	m_ui.PnlSeven->setPosition(Vec2(m_ui.PnlSevenPos.x + size.width,m_ui.PnlSevenPos.y));
	m_ui.PnlCard->setPosition(Vec2(m_ui.PnlCardPos.x + size.width,m_ui.PnlCardPos.y));
	m_ui.PnlPowerRank->setPosition(Vec2(m_ui.PnlPowerRankPos.x + size.width,m_ui.PnlPowerRankPos.y));
	m_ui.PnlShowSuperFriend->setPosition(Vec2(m_ui.PnlShowSuperFriendPos.x + size.width,m_ui.PnlShowSuperFriendPos.y));
	m_ui.PnlGrown->setPosition(Vec2(m_ui.PnlGrownPos.x + size.width,m_ui.PnlGrownPos.y));
	m_ui.PnlBenefit->setPosition(Vec2(m_ui.PnlBenefitPos.x + size.width,m_ui.PnlBenefitPos.y));
	m_ui.PnlCommon->setPosition(Vec2(m_ui.PnlCommonPos.x + size.width,m_ui.PnlCommonPos.y));
	m_ui.PnlVipGift->setPosition(Vec2(m_ui.PnlVipGiftPos.x + size.width,m_ui.PnlVipGiftPos.y));
}

void OperateActLayer::setAllPnlIn()
{
	float speedTime = 0.15f;
	m_ui.Img_diList->runAction(CCMoveTo::create(speedTime,m_ui.Img_diListPos));
	switch (isFirstOperateType)
	{
	case OPERATE_ACT_TYPE_SEVEN_DAY: //七日礼包
		{
			m_ui.PnlSeven->runAction(CCMoveTo::create(speedTime,m_ui.PnlSevenPos));

			m_ui.PnlCard->setPosition(m_ui.PnlCardPos);
			m_ui.PnlPowerRank->setPosition(m_ui.PnlPowerRankPos);
			m_ui.PnlShowSuperFriend->setPosition(m_ui.PnlShowSuperFriendPos);
			m_ui.PnlGrown->setPosition(m_ui.PnlGrownPos);
			m_ui.PnlBenefit->setPosition(m_ui.PnlBenefitPos);
			m_ui.PnlCommon->setPosition(m_ui.PnlCommonPos);
			m_ui.Pnl_Double->setPosition(m_ui.PnlDoublePos);
			m_ui.Pnl_Purchase->setPosition(m_ui.PnlPurchasePos);
			m_ui.PnlVipGift->setPosition(m_ui.PnlVipGiftPos);
		}
		break;
	case OPERATE_ACT_TYPE_MONTH_CARD: //超值月卡
		{
			m_ui.PnlSeven->setPosition(m_ui.PnlSevenPos);
			m_ui.PnlCard->runAction(CCMoveTo::create(speedTime,m_ui.PnlCardPos));
			m_ui.PnlPowerRank->setPosition(m_ui.PnlPowerRankPos);
			m_ui.PnlShowSuperFriend->setPosition(m_ui.PnlShowSuperFriendPos);
			m_ui.PnlGrown->setPosition(m_ui.PnlGrownPos);
			m_ui.PnlBenefit->setPosition(m_ui.PnlBenefitPos);
			m_ui.PnlCommon->setPosition(m_ui.PnlCommonPos);
			m_ui.Pnl_Double->setPosition(m_ui.PnlDoublePos);
			m_ui.Pnl_Purchase->setPosition(m_ui.PnlPurchasePos);
			m_ui.PnlVipGift->setPosition(m_ui.PnlVipGiftPos);
		}
		break;
	case OPERATE_ACT_TYPE_FIGHT_RANK: //战力排行
		{
			m_ui.PnlSeven->setPosition(m_ui.PnlSevenPos);
			m_ui.PnlCard->setPosition(m_ui.PnlCardPos);
			m_ui.PnlPowerRank->runAction(CCMoveTo::create(speedTime,m_ui.PnlPowerRankPos));
			m_ui.PnlShowSuperFriend->setPosition(m_ui.PnlShowSuperFriendPos);
			m_ui.PnlGrown->setPosition(m_ui.PnlGrownPos);
			m_ui.PnlBenefit->setPosition(m_ui.PnlBenefitPos);
			m_ui.PnlCommon->setPosition(m_ui.PnlCommonPos);
			m_ui.Pnl_Double->setPosition(m_ui.PnlDoublePos);
			m_ui.Pnl_Purchase->setPosition(m_ui.PnlPurchasePos);
			m_ui.PnlVipGift->setPosition(m_ui.PnlVipGiftPos);
		}
		break;
	case OPERATE_ACT_TYPE_SUPER_FRIEND://强力伙伴
		{
			m_ui.PnlSeven->setPosition(m_ui.PnlSevenPos);
			m_ui.PnlCard->setPosition(m_ui.PnlCardPos);
			m_ui.PnlPowerRank->setPosition(m_ui.PnlPowerRankPos);
			m_ui.PnlShowSuperFriend->runAction(CCMoveTo::create(speedTime,m_ui.PnlShowSuperFriendPos));
			m_ui.PnlGrown->setPosition(m_ui.PnlGrownPos);
			m_ui.PnlBenefit->setPosition(m_ui.PnlBenefitPos);
			m_ui.PnlCommon->setPosition(m_ui.PnlCommonPos);
			m_ui.Pnl_Double->setPosition(m_ui.PnlDoublePos);
			m_ui.Pnl_Purchase->setPosition(m_ui.PnlPurchasePos);
			m_ui.PnlVipGift->setPosition(m_ui.PnlVipGiftPos);
		}
		break;
	case OPERATE_ACT_TYPE_FINANCIAL: //理财通
		{
			m_ui.PnlSeven->setPosition(m_ui.PnlSevenPos);
			m_ui.PnlCard->setPosition(m_ui.PnlCardPos);
			m_ui.PnlPowerRank->setPosition(m_ui.PnlPowerRankPos);
			m_ui.PnlShowSuperFriend->setPosition(m_ui.PnlShowSuperFriendPos);
			m_ui.PnlGrown->runAction(CCMoveTo::create(speedTime,m_ui.PnlGrownPos));
			m_ui.PnlBenefit->setPosition(m_ui.PnlBenefitPos);
			m_ui.PnlCommon->setPosition(m_ui.PnlCommonPos);
			m_ui.Pnl_Double->setPosition(m_ui.PnlDoublePos);
			m_ui.Pnl_Purchase->setPosition(m_ui.PnlPurchasePos);
			m_ui.PnlVipGift->setPosition(m_ui.PnlVipGiftPos);
		}
		break;
	case OPERATE_ACT_TYPE_NATIONAL_WELL_BEING://全民福利
		{
			m_ui.PnlSeven->setPosition(m_ui.PnlSevenPos);
			m_ui.PnlCard->setPosition(m_ui.PnlCardPos);
			m_ui.PnlPowerRank->setPosition(m_ui.PnlPowerRankPos);
			m_ui.PnlShowSuperFriend->setPosition(m_ui.PnlShowSuperFriendPos);
			m_ui.PnlGrown->setPosition(m_ui.PnlGrownPos);
			m_ui.PnlBenefit->runAction(CCMoveTo::create(speedTime,m_ui.PnlBenefitPos));
			m_ui.PnlCommon->setPosition(m_ui.PnlCommonPos);
			m_ui.Pnl_Double->setPosition(m_ui.PnlDoublePos);
			m_ui.Pnl_Purchase->setPosition(m_ui.PnlPurchasePos);
			m_ui.PnlVipGift->setPosition(m_ui.PnlVipGiftPos);
		}
		break;
	case OPERATE_ACT_TYPE_COMMOMA://通用模版A（每日5点重置）
	case OPERATE_ACT_TYPE_COMMOMA_://通用模版A_
	case OPERATE_ACT_TYPE_COMMOMB://通用模版B（每日5点重置）
	case OPERATE_ACT_TYPE_COMMOMB_://通用模版B_
		{			
			m_ui.PnlSeven->setPosition(m_ui.PnlSevenPos);
			m_ui.PnlCard->setPosition(m_ui.PnlCardPos);
			m_ui.PnlPowerRank->setPosition(m_ui.PnlPowerRankPos);
			m_ui.PnlShowSuperFriend->setPosition(m_ui.PnlShowSuperFriendPos);
			m_ui.PnlGrown->setPosition(m_ui.PnlGrownPos);
			m_ui.PnlBenefit->setPosition(m_ui.PnlBenefitPos);
			m_ui.PnlCommon->runAction(CCMoveTo::create(speedTime,m_ui.PnlCommonPos));
			m_ui.Pnl_Double->setPosition(m_ui.PnlDoublePos);
			m_ui.Pnl_Purchase->setPosition(m_ui.PnlPurchasePos);
			m_ui.PnlVipGift->setPosition(m_ui.PnlVipGiftPos);
		}
		break;
	case OPERATE_ACT_TYPE_DOUBLE_FALL: //双倍掉落
		{
			m_ui.PnlSeven->setPosition(m_ui.PnlSevenPos);
			m_ui.PnlCard->setPosition(m_ui.PnlCardPos);
			m_ui.PnlPowerRank->setPosition(m_ui.PnlPowerRankPos);
			m_ui.PnlShowSuperFriend->setPosition(m_ui.PnlShowSuperFriendPos);
			m_ui.PnlGrown->setPosition(m_ui.PnlGrownPos);
			m_ui.PnlBenefit->setPosition(m_ui.PnlBenefitPos);
			m_ui.PnlCommon->setPosition(m_ui.PnlCommonPos);
			m_ui.Pnl_Double->runAction(CCMoveTo::create(speedTime,m_ui.PnlDoublePos));
			m_ui.Pnl_Purchase->setPosition(m_ui.PnlPurchasePos);
			m_ui.PnlVipGift->setPosition(m_ui.PnlVipGiftPos);
		}
		break;
	case OPERATE_ACT_TYPE_LIMIT_BUY:
		{
			m_ui.PnlSeven->setPosition(m_ui.PnlSevenPos);
			m_ui.PnlCard->setPosition(m_ui.PnlCardPos);
			m_ui.PnlPowerRank->setPosition(m_ui.PnlPowerRankPos);
			m_ui.PnlShowSuperFriend->setPosition(m_ui.PnlShowSuperFriendPos);
			m_ui.PnlGrown->setPosition(m_ui.PnlGrownPos);
			m_ui.PnlBenefit->setPosition(m_ui.PnlBenefitPos);
			m_ui.PnlCommon->setPosition(m_ui.PnlCommonPos);
			m_ui.Pnl_Double->setPosition(m_ui.PnlDoublePos);
			m_ui.Pnl_Purchase->runAction(CCMoveTo::create(speedTime,m_ui.PnlPurchasePos));
			m_ui.PnlVipGift->setPosition(m_ui.PnlVipGiftPos);
		}
		break;
	case OPERATE_ACT_TYPE_VIPBAG://超值礼包
		{
			m_ui.PnlSeven->setPosition(m_ui.PnlSevenPos);
			m_ui.PnlCard->setPosition(m_ui.PnlCardPos);
			m_ui.PnlPowerRank->setPosition(m_ui.PnlPowerRankPos);
			m_ui.PnlShowSuperFriend->setPosition(m_ui.PnlShowSuperFriendPos);
			m_ui.PnlGrown->setPosition(m_ui.PnlGrownPos);
			m_ui.PnlBenefit->setPosition(m_ui.PnlBenefitPos);
			m_ui.PnlCommon->setPosition(m_ui.PnlCommonPos);
			m_ui.Pnl_Double->setPosition(m_ui.PnlDoublePos);
			m_ui.Pnl_Purchase->setPosition(m_ui.PnlPurchasePos);
			m_ui.PnlVipGift->runAction(CCMoveTo::create(speedTime,m_ui.PnlVipGiftPos));
		}
		break;
	default:
		break;
	}
}

void showGetCard()
{
	vector<int>&  petList = OperateActModel::getInstance()->getPetIdList();
	if (petList.size() > 0)
	{
		int petId = petList[0];
		petList.erase(petList.begin());

		LayerGetCard *_layerGetCard = LayerGetCard::create(petId,false,1,showGetCard);
		Director::getInstance()->getRunningScene()->addChild(_layerGetCard);
	}
}

void OperateActLayer::showPopReward(vector<CHABGE_PRIZE_ITEM> reward)
{
	vector<PopRewardItem*> vecRewardItem;
	//道具
	for (size_t i = 0; i < reward.size(); i++)
	{
		CHABGE_PRIZE_ITEM& signRewardInfo = reward.at(i);

		switch (signRewardInfo.id)
		{
		case OPRETTE_GOLD_ITEM_ID://金币
			{
				PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(signRewardInfo.amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case OPRETTE_COIN_ITEM_ID://银币
			{
				PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(signRewardInfo.amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case OPRETTE_PVP_ITEM_ID: //竞技场积分
			{
				PopRewardItem_PvpScore* rewardItem = new PopRewardItem_PvpScore(signRewardInfo.amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case OPRETTE_SHILIAN_ITEM_ID: //试炼积分
			{
				PopRewardItem_TrialScore* rewardItem = new PopRewardItem_TrialScore(signRewardInfo.amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case OPRETTE_GROUP_ITEM_ID: //帮派积分
			{
				PopRewardItem_BanGong* rewardItem = new PopRewardItem_BanGong(signRewardInfo.amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case OPRETTE_HUNTIAN_ITEM_ID: //浑天丹
			{
				PopRewardItem_HunTian* rewardItem = new PopRewardItem_HunTian(signRewardInfo.amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case OPRETTE_JIUZHUAN_ITEM_ID: //九转丹
			{
				PopRewardItem_JiuZhuan* rewardItem = new PopRewardItem_JiuZhuan(signRewardInfo.amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case OPRETTE_VIPEXP_ITEM_ID: //VIP经验
			{
				PopRewardItem_VipExp* rewardItem = new PopRewardItem_VipExp(signRewardInfo.amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case OPRETTE_CUILIAN_ITEM_ID: //淬炼碎片
			{
				PopRewardItem_CuiLian* rewardItem = new PopRewardItem_CuiLian(signRewardInfo.amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case OPRETTE_EXP_ITEM_ID: //主角经验
			{
				PopRewardItem_Exp* rewardItem = new PopRewardItem_Exp(signRewardInfo.amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		default:
			PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(signRewardInfo.id, signRewardInfo.amount);
			vecRewardItem.push_back(rewardItem);
			break;
		}
	}


	auto call = [](Ref* sender, PopRewardEventType type)->void
	{
		//获得整卡
		if (type == EVENT_TYPE_END)
		{
			vector<int>&  petList = OperateActModel::getInstance()->getPetIdList();
			if (petList.size() > 0)
			{
				int petId = petList[0];
				petList.erase(petList.begin());

				LayerGetCard *_layerGetCard = LayerGetCard::create(petId,false,1,showGetCard);
				Director::getInstance()->getRunningScene()->addChild(_layerGetCard);
			}
		}
	};

	PopReward::show(vecRewardItem,call);

}

void OperateActLayer::updateSevenInfo()
{
	closeAllPanel();
	m_ui.PnlSeven->setVisible(true);

	//防止反复切换都要重新更新界面
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_SEVEN_DAY);
	if (item->isInit)
	{
		return;
	}

	item->isInit = true;
	
	//防止重复设置
	int loginTime = OperateActModel::getInstance()->getLoginTimes();
	vector<int>& ActTyp1 = OperateActModel::getInstance()->getActTyp1();  //#记录已领取的7天奖励ID集合
	m_ui.SevenCount->setString(TO_STR(loginTime) + DataManager::getInstance()->searchCommonTexdtById(10150));
	setTextAddStroke(m_ui.SevenCount,Color3B(0x09,0x4e,0x51),2);

	sevenListSort(item);
	m_ui.SVSevenList->removeAllItems();
	for(int i = 0; i < item->info.size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIProActivity/UIItem.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		Text* Lab_Tips = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Tips"));
		Text* Lab_Tips_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Tips_0"));
		Lab_Tips->setFontName(FONT_FZZHENGHEI);
		Lab_Tips_0->setFontName(FONT_FZZHENGHEI);
		Lab_Tips_0->setVisible(true);

		//奖励
		OPERATE_ACT_INFO_ITEM info = item->info.at(i);  
		VEV_OPERATEA_PRIZE_ITEM sevenItem = info.reward;
		for (int j = 0; j < sevenItem.size(); j++)
		{
			Layout *itemPrize = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_item" + TO_STR(j+1)));

			UIToolHeader *icon = UIToolHeader::create(sevenItem[j].id);
			icon->setNumEx(sevenItem[j].amount);
			icon->setAnchorPoint(Vec2(0,0));
			icon->setScale(0.8f);
			icon->setToolId(sevenItem[j].id);
			icon->showLightEff();
			icon->setTipsEnabled(true);
			itemPrize->addChild(icon);
		}
		Lab_Tips->setString(DataManager::getInstance()->searchCommonTexdtById(20236) + TO_STR(info.amount1) +
			DataManager::getInstance()->searchCommonTexdtById(10150));
		setTextAddStroke(Lab_Tips,Color3B(0x9e,0x0f,0x0f),2);

		Button* Btn_Pay = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Pay"));
		Button* Btn_Get = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Get"));
		ImageView *getEd = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"getEd"));
		Button* chongzhi = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"chongzhi"));
		Btn_Pay->setVisible(false);
		Btn_Get->setVisible(false);
		getEd->setVisible(false);
		chongzhi->setVisible(false);

		if (info.amount1 <= loginTime)
		{
			vector<int>::iterator result = find( ActTyp1.begin( ), ActTyp1.end( ), info.id ); //查找3
			if ( result == ActTyp1.end( ) ) //没找到 能领取
			{
				Btn_Get->setVisible(true);
				Btn_Get->setBright(true);

				auto call = [this,info](Ref* sender, Widget::TouchEventType type)->void
				{
					if (type == Widget::TouchEventType::BEGAN)
					{
						SoundUtils::playSoundEffect("dianji");
					}
					else if (type == Widget::TouchEventType::ENDED)
					{
						OperateActController::getInstance()->getOprateActtask(info.id);
					}
				};

				Btn_Get->addTouchEventListener(call);
			}
			else //找到 已领取
			{
				getEd->setVisible(true);
				Btn_Get->setVisible(false);
				Lab_Tips_0->setVisible(false);
			}
		}
		else
		{
			Btn_Get->setVisible(true);
			Btn_Get->setBright(false);
			Btn_Get->addTouchEventListener(nullptr);
		}

		//设置进度
		if(Lab_Tips_0->isVisible())
		{
			Lab_Tips_0->setString("(" + TO_STR(loginTime) + "/" + TO_STR(info.amount1) + ")");
			if(loginTime < info.amount1)
			{
				Lab_Tips_0->setColor(Color3B::WHITE);
			}
			else
			{
				Lab_Tips_0->setColor(Color3B(0x54, 0xdd, 0x51));
			}
		}
		
		m_ui.SVSevenList->pushBackCustomItem(layer);
	}

	m_ui.SVSevenList->refreshView();
	m_ui.SVSevenList->scrollToTop(0.01f,false);

	//初始化完毕再设置滚动条比例
	m_OpSevenBar->setScrollBarLengthPercent(
		m_ui.SVSevenList->getContentSize().height / m_ui.SVSevenList->getInnerContainerSize().height);
	m_OpSevenBar->setScrollBarPercentY(0.0f);
}

bool OperateActLayer::isSevenHave()
{
	bool ret = false;
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_SEVEN_DAY);
	int loginTime = OperateActModel::getInstance()->getLoginTimes();
	vector<int>& ActTyp1 = OperateActModel::getInstance()->getActTyp1();  //#记录已领取的7天奖励ID集合
	for(int i = 0; i < item->info.size(); i++)
	{
		//奖励
		OPERATE_ACT_INFO_ITEM info = item->info.at(i);  
		if (info.amount1 <= loginTime)
		{
			vector<int>::iterator result = find( ActTyp1.begin( ), ActTyp1.end( ), info.id ); //查找3
			if ( result == ActTyp1.end( ) ) //没找到 能领取
			{
				ret = true;
				return ret;
			}
		}
	}

	return ret;
}

bool OperateActLayer::isSevenExhibition(bool isEndTimeOut)
{
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_SEVEN_DAY);
	if (isOpen(*item,isEndTimeOut))
	{
		if (item->disappear == -1) //奖励领取消失
		{
			vector<int>& ActTyp1 = OperateActModel::getInstance()->getActTyp1();  //#记录已领取的7天奖励ID集合
			for(int i = 0; i < item->info.size(); i++)
			{
				//奖励
				OPERATE_ACT_INFO_ITEM info = item->info.at(i);  
				vector<int>::iterator result = find( ActTyp1.begin( ), ActTyp1.end( ), info.id ); //查找3
				if ( result == ActTyp1.end( ) ) //没找到 能领取
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void OperateActLayer::sevenListSort(OPERATE_ACT_SORT_ITEM* item)
{
	int loginTime = OperateActModel::getInstance()->getLoginTimes();
	vector<int>& ActTyp1 = OperateActModel::getInstance()->getActTyp1();
	
	sort(item->info.begin(),item->info.end(),[=](OPERATE_ACT_INFO_ITEM a,OPERATE_ACT_INFO_ITEM b)
	{
		bool canA = false;
		bool canB = false;
		//判断A是否能领取
		if (a.amount1 <= loginTime)
		{
			auto result = find( ActTyp1.begin( ), ActTyp1.end( ), a.id ); //查找3
			if ( result == ActTyp1.end( ) ) //没找到 能领取
			{
				canA = true;
			}
		}

		//判断B是否能领取
		if(b.amount1 <= loginTime)
		{
			auto result = find( ActTyp1.begin( ), ActTyp1.end( ), b.id ); //查找3
			if ( result == ActTyp1.end( ) ) //没找到 能领取
			{
				canB = true;
			}
		}
		//A能领取
		if (canA && !canB)
		{
			return true;
		}
		//B能领取
		else if (!canA && canB)
		{
			return false;
		}
		else
		{
			bool HaveA = false;
			bool HaveB = false;
			//判断A是已经领取
			auto resultA = find( ActTyp1.begin( ), ActTyp1.end( ), a.id ); //查找3
			if ( resultA != ActTyp1.end( ) ) //找到 领取
			{
				HaveA = true;
			}

			//判断B是已经领取
			auto resultB = find( ActTyp1.begin( ), ActTyp1.end( ), b.id ); //查找3
			if ( resultB != ActTyp1.end( ) ) //找到 领取
			{
				HaveB = true;
			}

			if(!HaveA && HaveB)
			{
				return true;
			}
			else if(HaveA && !HaveB)
			{
				return false;
			}
			else
			{
				return a.id < b.id;
			}
		}
	});
}

void OperateActLayer::updateRemadayInfo()
{
	closeAllPanel();
	m_ui.PnlCard->setVisible(true);

	//防止反复切换都要重新更新界面
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_MONTH_CARD);
	if (item->isInit)
	{
		return;
	}

	item->isInit = true;

	//购买月卡或者尊卡记得还原
	OperateActModel::getInstance()->getIsBuyMonthCard()  = false;
	OperateActModel::getInstance()->getIsBuyExtremeCard() = false;

	int remaday = OperateActModel::getInstance()->getRemaday(); //获取月卡剩余天数
	bool card2Status = OperateActModel::getInstance()->getCard2Status(); //是否购买至尊月卡

	//月卡
	if (remaday <= -1)
	{
		m_ui.LabCardTime->setString("");
		m_ui.BtnCardBuy1->setVisible(true);
		m_ui.ImgState_1->setVisible(false);
		setTextAddStroke(m_ui.LabCardTime,Color3B(0x09,0x4e,0x51),2);
	}
	else if (remaday == 0)
	{
		m_ui.LabCardTime->setString(DataManager::getInstance()->searchCommonTexdtById(20237));
		m_ui.BtnCardBuy1->setVisible(true);
		m_ui.ImgState_1->setVisible(false);
		setTextAddStroke(m_ui.LabCardTime,Color3B(0x09,0x4e,0x51),2);
	}
	else if (remaday <= 3)
	{
		m_ui.LabCardTime->setString(DataManager::getInstance()->searchCommonTexdtById(20238) + TO_STR(remaday) + 
		DataManager::getInstance()->searchCommonTexdtById(20231));
		m_ui.BtnCardBuy1->setVisible(true);
		m_ui.ImgState_1->setVisible(false);
		setTextAddStroke(m_ui.LabCardTime,Color3B(0x09,0x4e,0x51),2);
	}
	else
	{
		m_ui.LabCardTime->setString(DataManager::getInstance()->searchCommonTexdtById(20238) + TO_STR(remaday) + 
		DataManager::getInstance()->searchCommonTexdtById(20231));
		m_ui.BtnCardBuy1->setVisible(false);
		m_ui.ImgState_1->setVisible(true);
		setTextAddStroke(m_ui.LabCardTime,Color3B(0x09,0x4e,0x51),2);
	}
	auto call1 = [](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
		}
	};
	m_ui.BtnCardBuy1->addTouchEventListener(call1);

	//至尊
	if (card2Status)
	{
		m_ui.BtnCardBuy2->setVisible(false);
		m_ui.ImgState_2->setVisible(true);
	}
	else
	{
		m_ui.BtnCardBuy2->setVisible(true);
		m_ui.ImgState_2->setVisible(false);
	}
	auto call2 = [](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
		}
	};
	m_ui.BtnCardBuy2->addTouchEventListener(call2);
}

void OperateActLayer::updateFightRankInfo()
{
	closeAllPanel();
	m_ui.PnlPowerRank->setVisible(true);
	//防止反复切换都要重新更新界面
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_FIGHT_RANK);
	if (item->isInit)
	{
		return;
	}
	item->isInit = true;

	m_ui.rankTimeLab->setString(StringFormat::convertSecToFormatTime(item->daojiTime));
	setTextAddStroke(m_ui.rankTimeLab,Color3B(0x09,0x4e,0x51),2);

	m_ui.rankListView->removeAllItems();
	vector<OPERATE_PLAYER_RANK> PlayerRank = OperateActModel::getInstance()->getPlayerRank();
	for(int i = 0; i < item->info.size(); i++)
	{
		//奖励
		OPERATE_ACT_INFO_ITEM info = item->info.at(i);  

		string strFileName;
		strFileName.assign("Image/UIProActivity/UIPlayer.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
		ImageView* Image_rank = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Image_rank"));
		Text* Lab_Rank = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Rank"));
		Lab_Rank->setFontName(FONT_FZZHENGHEI);
		if (i < 3)
		{
			Lab_Rank->setVisible(false);
			Image_rank->loadTexture("Image/Icon/global/img_" + TO_STR(i+1) + "ming.png");
		}
		else
		{
			Image_rank->loadTexture("Image/Icon/global/img_4ming.png");

			Lab_Rank->setVisible(true);
			Lab_Rank->setString(TO_STR(info.amount1) + "~" + TO_STR(info.amount2));
			setTextAddStroke(Lab_Rank,Color3B(0x00,0x00,0x00),2);
		}

		Text* Lab_Name = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Name"));
		ImageView* Image_vip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Image_vip"));
		Text* Lab_Power = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Power"));
		Layout *imgHero = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_HeadIcon"));
		Lab_Name->setFontName(FONT_FZZHENGHEI);
		Lab_Power->setFontName(FONT_FZZHENGHEI);
		Lab_Name->setVisible(false);
		Image_vip->setVisible(false);
		Lab_Power->setVisible(false);
		imgHero->setVisible(false);
		
		for (int j = 0; j < PlayerRank.size(); j++)
		{
			if (j+1 == info.amount1)
			{
				Lab_Name->setVisible(true);
				Image_vip->setVisible(true);
				Lab_Power->setVisible(true);
				imgHero->setVisible(true);

				Lab_Name->setString(PlayerRank.at(j).name);
				setTextAddStroke(Lab_Name,Color3B(0x7d,0x3f,0x1c),2);
				Image_vip->loadTexture("Image/Icon/vip/btn_VIP" + TO_STR(PlayerRank.at(j).vip) + ".png");
				Lab_Power->setString(TO_STR(PlayerRank.at(j).fight));
				setTextAddStroke(Lab_Power,Color3B(0x00,0x00,0x00),2);
				UIToolHeader *icon = UIToolHeader::create(PlayerRank.at(j).res);
				icon->setScale(0.6f);
				icon->setAnchorPoint(Vec2(0,0));
				icon->showLightEff();
				imgHero->addChild(icon,0,"imgHero");
				break;
			}
		}

		VEV_OPERATEA_PRIZE_ITEM sevenItem = info.reward;
		for (int k = 0; k < sevenItem.size(); k++)
		{
			Layout *itemPrize = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_Item" + TO_STR(k+1)));

			UIToolHeader *icon = UIToolHeader::create(sevenItem[k].id);
			icon->setNumEx(sevenItem[k].amount);
			icon->setAnchorPoint(Vec2(0,0));
			icon->setScale(0.6f);
			icon->setToolId(sevenItem[k].id);
			icon->setTipsEnabled(true);
			itemPrize->addChild(icon);
		}
		m_ui.rankListView->pushBackCustomItem(layer);
	}

	m_ui.rankListView->refreshView();
	m_ui.rankListView->scrollToTop(0.01f,false);

	m_OpRankABar->setScrollBarLengthPercent(
		m_ui.rankListView->getContentSize().height / m_ui.rankListView->getInnerContainerSize().height);
	m_OpRankABar->setScrollBarPercentY(0.0f);
	
	auto call = [](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			RankTopModel::getInstance()->setCurRankTopType(POWER_RANK_TOP);
			Director::getInstance()->pushScene(RankTopScene::createScene(true));
		}
	};
	m_ui.rankBtn->addTouchEventListener(call);

	m_ui.rankMyRank->setString(TO_STR(OperateActModel::getInstance()->getMyRankId()));
	setTextAddStroke(m_ui.rankMyRank,Color3B(0x00,0x00,0x00),2);
	m_ui.rankMyPower->setString(TO_STR(OperateActModel::getInstance()->getMyTotalAtk()));
	setTextAddStroke(m_ui.rankMyPower,Color3B(0x00,0x00,0x00),2);
}

bool OperateActLayer::isFightRankExhibition(bool isEndTimeOut)
{
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_FIGHT_RANK);
	if (isOpen(*item,isEndTimeOut))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void OperateActLayer::updateSuperFriendInfo()
{
	closeAllPanel();
	m_ui.PnlShowSuperFriend->setVisible(true);
	//防止反复切换都要重新更新界面
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_SUPER_FRIEND);
	if (item->isInit)
	{
		return;
	}
	item->isInit = true;

	m_ui.imgSuperFriend->loadTexture("Image/UIProActivity/Image/img_bg1.png");
	auto call = [](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			Director::getInstance()->pushScene(VipScene::createScene(VIP_VIEW,11));
		}
	};
	m_ui.pBtnCall->addTouchEventListener(call);
}

bool OperateActLayer::isSuperFriendExhibition(bool isEndTimeOut /*= false*/)
{
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_SUPER_FRIEND);
	if (isOpen(*item,isEndTimeOut))
	{
		if (HeroModel::getInstance()->searchHeroByTemplateId(item->info.at(0).amount1)) //有这个英雄
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void OperateActLayer::updateFinacialInfo(bool updateByIdx)
{
	closeAllPanel();
	m_ui.PnlGrown->setVisible(true);
	//防止反复切换都要重新更新界面
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_FINANCIAL);
	if (item->isInit)
	{
		return;
	}

	item->isInit = true;
	
	bool buyType = OperateActModel::getInstance()->getBuyTyp5();		// # 是否购买理财通 0:false
	int  myLevel = OperateActModel::getInstance()->getMyLevel();		// # 我的等级
	vector<int>& ActTyp5 = OperateActModel::getInstance()->getActTyp5(); // # 已领取的领柴通奖励ID

	if (buyType)
	{
		m_ui.growState->setVisible(true);
		m_ui.grownButBuy->setVisible(false);
	}
	else
	{
		m_ui.growState->setVisible(false);
		m_ui.grownButBuy->setVisible(true);

		auto call = [this](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (type == Widget::TouchEventType::ENDED)
			{
				MainParam *Mainparam = MainModel::getInstance()->getMainParam();
				if (Mainparam->mVipLevel < DataManager::getInstance()->searchToolSystemById(FINACIAL_VIP_LEVEL)->getIntData("value"))
				{
					CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20239), [this](Ref* pSender, CustomRetType type)->void
					{
						switch (type)
						{
						case RET_TYPE_OK:
							{
								Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
							}
							break;
						default:
							break;
						}

					}, CUSTOM_YES_NO);
				}
				else if (Mainparam->mGold < DataManager::getInstance()->searchToolSystemById(FINACIAL_GOLD_COUNT)->getIntData("value"))
				{
					CustomGoldTips *_tips = CustomGoldTips::create();
					if (_tips)
					{
						Director::getInstance()->getRunningScene()->addChild(_tips);
					}
				}
				else
				{
					OperateActController::getInstance()->BuyOprateFinancial();
				}
			}
		};
		m_ui.grownButBuy->addTouchEventListener(call);
	}

	if (updateByIdx)
	{
		for (int i = 0; i < item->info.size(); i++)
		{
			//奖励
			OPERATE_ACT_INFO_ITEM info = item->info.at(i);
			if (info.id == OperateActModel::getInstance()->getCurTaskId())
			{
				Widget * item = (Widget*)m_ui.grownSVGrown->getChildByName("layer"+TO_STR(i));
				Layout*Pnl_Item = dynamic_cast<Layout*>(Helper::seekWidgetByName(item,"Pnl_Item"));
				vector<int>::iterator result = find( ActTyp5.begin( ), ActTyp5.end( ), info.id ); //查找3
				if ( result == ActTyp5.end( ) ) //没找到 能领取
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20240));
				}
				else //找到 已领取
				{
					UIToolHeader *icon = (UIToolHeader*)Pnl_Item->getChildByName("icon");
					if (icon->getChildByName("AniZhuangbeiEffe"))
					{
						icon->removeChildByName("AniZhuangbeiEffe");
					}
					icon->setMaskFlag(true);
					icon->setTipsEnabled(true);
					icon->addTouchEventListener(nullptr);
				}
				break;
			}
		}
	}
	else
	{
		int row = ceil(item->info.size()/3.0f);
		int scrollViewHeight = row * 140+10;
		scrollViewHeight = scrollViewHeight >  m_ui.grownSVGrown->getContentSize().height ? scrollViewHeight : 
			m_ui.grownSVGrown->getContentSize().height;
		int scrollViewWidth = m_ui.grownSVGrown->getInnerContainerSize().width;
		m_ui.grownSVGrown->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

		m_ui.grownSVGrown->removeAllChildren();
		int siIdx = 0;
		for (int i = 0; i < item->info.size(); i++)
		{
			int rowNumber = i/3;
			int colNumber = i%3;

			string strFileName;
			strFileName.assign("Image/UIProActivity/UIProGrown.ExportJson");
			Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
			Layout*Pnl_Item = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_Item"));
			Text*Lab_Level = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Level"));

			//奖励
			OPERATE_ACT_INFO_ITEM info = item->info.at(i);
			VEV_OPERATEA_PRIZE_ITEM sevenItem = info.reward;
			UIToolHeader *icon = UIToolHeader::create(sevenItem[0].id);
			icon->setNumEx(sevenItem[0].amount);
			icon->setAnchorPoint(Vec2(0,0));
			icon->setScale(0.8f);
			icon->setToolId(sevenItem[0].id);
			icon->showLightEff();
			icon->setTouchEnabled(true);
			Pnl_Item->addChild(icon,0,"icon");
			Lab_Level->setString(TO_STR(info.amount1) + DataManager::getInstance()->searchCommonTexdtById(20101));
			Lab_Level->setFontName(FONT_FZZHENGHEI);
			setTextAddStroke(Lab_Level,Color3B(0x9e,0x0f,0x0f),2);
			//没购买
			if (buyType)
			{
				//如果已经领取
				if (myLevel >= info.amount1)
				{
					vector<int>::iterator result = find( ActTyp5.begin( ), ActTyp5.end( ), info.id ); //查找3
					if ( result == ActTyp5.end( ) ) //没找到 能领取
					{
						ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniZhuangbeiEffe/AniZhuangbeiEffe.ExportJson");
						Armature* pArmature = Armature::create("AniZhuangbeiEffe");//名字需要和 .ExportJson一致

						pArmature->getAnimation()->playByIndex(0,-1,1);
						//设置动画精灵位置 
						pArmature->setPosition(Vec2(icon->getContentSize().width/2,icon->getContentSize().height/2));
						icon->addChild(pArmature,14, "AniZhuangbeiEffe");

						auto call = [this,info](Ref* ref, Widget::TouchEventType type)->void
						{
							if (type == Widget::TouchEventType::BEGAN)
							{
								SoundUtils::playSoundEffect("dianji");
							}
							else if (type == Widget::TouchEventType::ENDED)
							{
								OperateActController::getInstance()->getOprateActtask(info.id);
							}
						};
						icon->addTouchEventListener(call);
					}
					else //找到 已领取
					{
						icon->setMaskFlag(true);
						icon->setTipsEnabled(true);
					}
				}
				else
				{
					icon->setTipsEnabled(true);
				}
			}
			else
			{
				icon->setTipsEnabled(true);
			}

			layer->setPosition(Vec2(colNumber*120 + 25, scrollViewHeight-(rowNumber+1)*140+10 ));
			m_ui.grownSVGrown->addChild(layer,0,"layer"+TO_STR(i));
		}

		//初始化完毕再设置滚动条比例
		m_OpListLicaiBar->setScrollBarLengthPercent(
			m_ui.grownSVGrown->getContentSize().height / m_ui.grownSVGrown->getInnerContainerSize().height);
		m_OpListLicaiBar->setScrollBarPercentY(0.0f);
	}
}

bool OperateActLayer::isFinacialHave()
{
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_FINANCIAL);

	bool buyType = OperateActModel::getInstance()->getBuyTyp5();		// # 是否购买理财通 0:false
	MainParam *Mainparam = MainModel::getInstance()->getMainParam();
	if (Mainparam->mVipLevel < DataManager::getInstance()->searchToolSystemById(FINACIAL_VIP_LEVEL)->getIntData("value"))
	{
		return false;
	}
	else
	{
		if (!buyType && Mainparam->mVipLevel >= DataManager::getInstance()->searchToolSystemById(FINACIAL_VIP_LEVEL)->getIntData("value"))
		{
			return true;
		}
		else
		{
			int  myLevel = OperateActModel::getInstance()->getMyLevel();		// # 我的等级
			vector<int>& ActTyp5 = OperateActModel::getInstance()->getActTyp5(); // # 已领取的领柴通奖励ID
			for (int i = 0; i < item->info.size(); i++)
			{
				OPERATE_ACT_INFO_ITEM info = item->info.at(i);
				//如果已经领取
				if (myLevel >= info.amount1)
				{
					vector<int>::iterator result = find( ActTyp5.begin( ), ActTyp5.end( ), info.id ); //查找3
					if ( result == ActTyp5.end( ) ) //没找到 能领取
					{
						return true;
					}
				}
			}

			return false;
		}
	}
}

bool OperateActLayer::isFinacialExhibition(bool isEndTimeOut)
{
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_FINANCIAL);
	if (isOpen(*item,isEndTimeOut))
	{
		if (item->disappear == -1) //奖励领取消失
		{
			vector<int>& ActTyp5 = OperateActModel::getInstance()->getActTyp5(); // # 已领取的领柴通奖励ID
			for (int i = 0; i < item->info.size(); i++)
			{
				OPERATE_ACT_INFO_ITEM info = item->info.at(i);
				//如果已经领取
				vector<int>::iterator result = find( ActTyp5.begin( ), ActTyp5.end( ), info.id ); //查找3
				if ( result == ActTyp5.end( ) ) //没找到 能领取
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void OperateActLayer::updateNationWellInfo()
{
	closeAllPanel();
	m_ui.PnlBenefit->setVisible(true);
	//防止反复切换都要重新更新界面
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_NATIONAL_WELL_BEING);
	if (item->isInit)
	{
		return;
	}

	item->isInit = true;
	
	int AllBuyCount = OperateActModel::getInstance()->getAllBuyCount();   // #全民福利购买次数
	vector<int>& ActTyp6 = OperateActModel::getInstance()->getActTyp6();  //#已领取的全民福利ID
	
	m_ui.bebefitCount->setString(TO_STR(AllBuyCount));
	setTextAddStroke(m_ui.bebefitCount,Color3B(0x00,0x00,0x00),2);

	NationWellSort(item);
	m_ui.benefitSVBenefit->removeAllItems();
	for(int i = 0; i < item->info.size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIProActivity/UIItem.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		OPERATE_ACT_INFO_ITEM info = item->info.at(i);
		Text* Lab_Tips = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Tips"));
		Text* Lab_Tips_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Tips_0"));
		Lab_Tips->setFontName(FONT_FZZHENGHEI);
		Lab_Tips_0->setFontName(FONT_FZZHENGHEI);
		Lab_Tips->setString(DataManager::getInstance()->searchCommonTexdtById(20241) + TO_STR(info.amount1));
		setTextAddStroke(Lab_Tips,Color3B(0x9e,0x0f,0x0f),2);
		Lab_Tips_0->setVisible(true);

		//奖励
		VEV_OPERATEA_PRIZE_ITEM sevenItem = info.reward;
		for (int j = 0; j < sevenItem.size(); j++)
		{
			Layout *itemPrize = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_item" + TO_STR(j+1)));

			UIToolHeader *icon = UIToolHeader::create(sevenItem[j].id);
			icon->setNumEx(sevenItem[j].amount);
			icon->setAnchorPoint(Vec2(0,0));
			icon->setScale(0.8f);
			icon->setToolId(sevenItem[j].id);
			icon->showLightEff();
			icon->setTipsEnabled(true);
			itemPrize->addChild(icon);
		}

		Button* Btn_Pay = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Pay"));
		Button* Btn_Get = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_Get"));
		ImageView *getEd = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"getEd"));
		Button* chongzhi = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"chongzhi"));
		Btn_Pay->setVisible(false);
		Btn_Get->setVisible(false);
		getEd->setVisible(false);
		chongzhi->setVisible(false);

		if (info.amount1 <= AllBuyCount)
		{
			vector<int>::iterator result = find( ActTyp6.begin( ), ActTyp6.end( ), info.id ); //查找3
			if ( result == ActTyp6.end( ) ) //没找到 能领取
			{
				Btn_Get->setVisible(true);

				auto call = [this,info](Ref* sender, Widget::TouchEventType type)->void
				{
					if (type == Widget::TouchEventType::BEGAN)
					{
						SoundUtils::playSoundEffect("dianji");
					}
					else if (type == Widget::TouchEventType::ENDED)
					{
						OperateActController::getInstance()->getOprateActtask(info.id);
					}
				};

				Btn_Get->addTouchEventListener(call);
			}
			else //找到 已领取
			{
				getEd->setVisible(true);
				Lab_Tips_0->setVisible(false);
			}
		}
		else
		{
			Btn_Get->setVisible(true);
			Btn_Get->setBright(false);
			Btn_Get->addTouchEventListener(nullptr);
		}

		//设置进度
		if(Lab_Tips_0->isVisible())
		{
			Lab_Tips_0->setString("(" + TO_STR(info.amount1) + "/" + TO_STR(AllBuyCount) + ")");
			if(info.amount1 < AllBuyCount)
			{
				Lab_Tips_0->setColor(Color3B::WHITE);
			}
			else
			{
				Lab_Tips_0->setColor(Color3B(0x54, 0xdd, 0x51));
			}
		}

		m_ui.benefitSVBenefit->pushBackCustomItem(layer);
	}

	m_ui.benefitSVBenefit->refreshView();
	m_ui.benefitSVBenefit->scrollToTop(0.01f,false);

	m_OpNationWellBar->setScrollBarLengthPercent(
		m_ui.benefitSVBenefit->getContentSize().height / m_ui.benefitSVBenefit->getInnerContainerSize().height);
	m_OpNationWellBar->setScrollBarPercentY(0.0f);
}

bool OperateActLayer::isNationWellHave()
{
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_NATIONAL_WELL_BEING);

	bool ret = false;
	int AllBuyCount = OperateActModel::getInstance()->getAllBuyCount();   // #全民福利购买次数
	vector<int>& ActTyp6 = OperateActModel::getInstance()->getActTyp6();  //#已领取的全民福利ID
	for(int i = 0; i < item->info.size(); i++)
	{
		OPERATE_ACT_INFO_ITEM info = item->info.at(i);
		if (info.amount1 <= AllBuyCount)
		{
			vector<int>::iterator result = find( ActTyp6.begin( ), ActTyp6.end( ), info.id ); //查找3
			if ( result == ActTyp6.end( ) ) //没找到 能领取
			{
				ret = true;
				return ret;
			}
		}
	}

	return ret;
}

bool OperateActLayer::isNationWellExhibition(bool isEndTimeOut)
{
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_NATIONAL_WELL_BEING);
	if (isOpen(*item,isEndTimeOut))
	{
		if (item->disappear == -1) //奖励领取消失
		{
			vector<int>& ActTyp6 = OperateActModel::getInstance()->getActTyp6();  //#已领取的全民福利ID
			for(int i = 0; i < item->info.size(); i++)
			{
				OPERATE_ACT_INFO_ITEM info = item->info.at(i);
				vector<int>::iterator result = find( ActTyp6.begin( ), ActTyp6.end( ), info.id ); //查找3
				if ( result == ActTyp6.end( ) ) //没找到 能领取
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void OperateActLayer::NationWellSort(OPERATE_ACT_SORT_ITEM* item)
{
	int AllBuyCount = OperateActModel::getInstance()->getAllBuyCount();   // #全民福利购买次数
	vector<int>& ActTyp6 = OperateActModel::getInstance()->getActTyp6();  //#已领取的全民福利ID

	sort(item->info.begin(),item->info.end(),[=](OPERATE_ACT_INFO_ITEM a,OPERATE_ACT_INFO_ITEM b)
	{
		bool canA = false;
		bool canB = false;
		//判断A是否能领取
		if (a.amount1 <= AllBuyCount)
		{
			auto result = find( ActTyp6.begin( ), ActTyp6.end( ), a.id ); //查找3
			if ( result == ActTyp6.end( ) ) //没找到 能领取
			{
				canA = true;
			}
		}

		//判断B是否能领取
		if(b.amount1 <= AllBuyCount)
		{
			auto result = find( ActTyp6.begin( ), ActTyp6.end( ), b.id ); //查找3
			if ( result == ActTyp6.end( ) ) //没找到 能领取
			{
				canB = true;
			}
		}

		if (canA && !canB)
		{
			return true;
		}
		else if (!canA && canB)
		{
			return false;
		}
		else
		{
			bool HaveA = false;
			bool HaveB = false;
			//判断A是已经领取
			auto resultA = find( ActTyp6.begin( ), ActTyp6.end( ), a.id ); //查找3
			if ( resultA != ActTyp6.end( ) ) //找到 领取
			{
				HaveA = true;
			}

			//判断B是已经领取
			auto resultB = find( ActTyp6.begin( ), ActTyp6.end( ), b.id ); //查找3
			if ( resultB != ActTyp6.end( ) ) //找到 领取
			{
				HaveB = true;
			}

			if(!HaveA && HaveB)
			{
				return true;
			}
			else if(HaveA && !HaveB)
			{
				return false;
			}
			else
			{
				return a.id < b.id;
			}
		}
	});
}

void OperateActLayer::updateCommomAInfo(int id)
{
	closeAllPanel();
	m_ui.PnlCommon->setVisible(true);
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	m_sOprateItemData = oprateItem;

	m_ui.imgTile->loadTexture("Image/UIProActivity/Image/" + TO_STR(oprateItem->res) + ".png");
	if (oprateItem->endTime <= -1)
	{
		m_ui.Img_TimeA->setVisible(false);
		m_ui.labTime1A->setVisible(false);

		m_ui.imgTimeB->setVisible(false);
		m_ui.labTimeB->setVisible(false);
	}
	else
	{	m_ui.Img_TimeA->setVisible(true);
		m_ui.labTime1A->setVisible(true);

		m_ui.imgTimeB->setVisible(false);
		m_ui.labTimeB->setVisible(false);
	}
	m_ui.CommonTips->setString(oprateItem->tips2);
	setTextAddStroke(m_ui.CommonTips,Color3B(0xac,0x03,0x03),2);
	m_ui.labTime1A->setString(StringFormat::convertSecToFormatTime(oprateItem->daojiTime) + DataManager::getInstance()->searchCommonTexdtById(20326));
	setTextAddStroke(m_ui.labTime1A,Color3B(0x09,0x4e,0x51),2);

	CommomASort(id);
	m_eCurActType = OPERATE_ACT_TYPE_COMMOMA;

	m_pCommonTabView->reloadData();

	m_OpComomABar->setScrollBarLengthPercent(m_pCommonTabView->getViewSize().height / m_pCommonTabView->getContentSize().height);
	m_OpComomABar->setScrollBarPercentY(0.0f);
}

bool OperateActLayer::isCommomAHave(int id)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	if (!oprateItem)
	{
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20242));
	}
	vector<int> ActTyp = OperateActModel::getInstance()->getComon7ById(id)->actTyp;

	for(int i = 0; i < oprateItem->info.size(); i++)
	{
		if (comonARefType(oprateItem->info.at(i).refTyp,oprateItem->info.at(i))) //达到条件
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), oprateItem->info.at(i).id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				return true;
			}
		}
	}

	return false;
}

bool OperateActLayer::isCommomAExhibition(int id,bool isEndTimeOut)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	if (isOpen(*oprateItem,isEndTimeOut))
	{
		if (oprateItem->disappear == -1) //奖励领取消失
		{
			vector<int> ActTyp = OperateActModel::getInstance()->getComon7ById(id)->actTyp;

			for(int i = 0; i < oprateItem->info.size(); i++)
			{
				auto result = find( ActTyp.begin( ), ActTyp.end( ), oprateItem->info.at(i).id ); //查找3
				if ( result == ActTyp.end( ) ) //没找到 能领取
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void OperateActLayer::CommomASort(int id)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	vector<int> ActTyp = OperateActModel::getInstance()->getComon7ById(id)->actTyp;

	sort(oprateItem->info.begin(),oprateItem->info.end(),[=](OPERATE_ACT_INFO_ITEM a,OPERATE_ACT_INFO_ITEM b)
	{
		bool canA = false;
		bool canB = false;
		//判断A是否能领取
		if (comonARefType(a.refTyp,a))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), a.id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				canA = true;
			}
		}

		//判断B是否能领取
		if (comonARefType(b.refTyp,b))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), b.id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				canB = true;
			}
		}

		if (canA && !canB)
		{
			return true;
		}
		else if (!canA && canB)
		{
			return false;
		}
		else
		{
			bool HaveA = false;
			bool HaveB = false;
			//判断A是已经领取
			auto resultA = find( ActTyp.begin( ), ActTyp.end( ), a.id ); //查找3
			if ( resultA != ActTyp.end( ) ) //找到 领取
			{
				HaveA = true;
			}

			//判断B是已经领取
			auto resultB = find( ActTyp.begin( ), ActTyp.end( ), b.id ); //查找3
			if ( resultB != ActTyp.end( ) ) //找到 领取
			{
				HaveB = true;
			}

			if(!HaveA && HaveB)
			{
				return true;
			}
			else if(HaveA && !HaveB)
			{
				return false;
			}
			else
			{
				return a.id < b.id;
			}
		}
	});
}

void OperateActLayer::_updateCommomAInfo(int id)
{
	closeAllPanel();
	m_ui.PnlCommon->setVisible(true);
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	m_sOprateItemData = oprateItem;

	m_ui.imgTile->loadTexture("Image/UIProActivity/Image/" + TO_STR(oprateItem->res) + ".png");
	if (oprateItem->endTime <= -1)
	{
		m_ui.Img_TimeA->setVisible(false);
		m_ui.labTime1A->setVisible(false);

		m_ui.imgTimeB->setVisible(false);
		m_ui.labTimeB->setVisible(false);
	}
	else
	{
		m_ui.Img_TimeA->setVisible(false);
		m_ui.labTime1A->setVisible(false);

		m_ui.imgTimeB->setVisible(true);
		m_ui.labTimeB->setVisible(true);
	}

	m_ui.CommonTips->setString(oprateItem->tips2);
	setTextAddStroke(m_ui.CommonTips,Color3B(0xac,0x03,0x03),2);
	m_ui.labTimeB->setString(StringFormat::convertSecToFormatTime(oprateItem->daojiTime));
	setTextAddStroke(m_ui.labTimeB,Color3B(0x09,0x4e,0x51),2);

	_CommomASort(id);
	m_eCurActType = OPERATE_ACT_TYPE_COMMOMA_;

	m_pCommonTabView->reloadData();

	m_OpComomABar->setScrollBarLengthPercent(m_pCommonTabView->getViewSize().height / m_pCommonTabView->getContentSize().height);
	m_OpComomABar->setScrollBarPercentY(0.0f);
}

bool OperateActLayer::_isCommomAHave(int id)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	vector<int> ActTyp = OperateActModel::getInstance()->getComon8ById(id)->actTyp;

	for(int i = 0; i < oprateItem->info.size(); i++)
	{
		if (comonARefType(oprateItem->info.at(i).refTyp,oprateItem->info.at(i)))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), oprateItem->info.at(i).id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				return true;
			}
		}
	}

	return false;
}

bool OperateActLayer::_isCommomAExhibition(int id,bool isEndTimeOut)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	if (isOpen(*oprateItem,isEndTimeOut))
	{
		if (oprateItem->disappear == -1) //奖励领取消失
		{
			vector<int> ActTyp = OperateActModel::getInstance()->getComon8ById(id)->actTyp;

			for(int i = 0; i < oprateItem->info.size(); i++)
			{
				auto result = find( ActTyp.begin( ), ActTyp.end( ), oprateItem->info.at(i).id ); //查找3
				if ( result == ActTyp.end( ) ) //没找到 能领取
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void OperateActLayer::_CommomASort(int id)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	vector<int> ActTyp = OperateActModel::getInstance()->getComon8ById(id)->actTyp;

	sort(oprateItem->info.begin(),oprateItem->info.end(),[=](OPERATE_ACT_INFO_ITEM a,OPERATE_ACT_INFO_ITEM b)
	{
		bool canA = false;
		bool canB = false;
		//判断A是否能领取
		if (comonARefType(a.refTyp,a))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), a.id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				canA = true;
			}
		}

		//判断B是否能领取
		if (comonARefType(b.refTyp,b))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), b.id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				canB = true;
			}
		}

		if (canA && !canB)
		{
			return true;
		}
		else if (!canA && canB)
		{
			return false;
		}
		else
		{
			bool HaveA = false;
			bool HaveB = false;
			//判断A是已经领取
			auto resultA = find( ActTyp.begin( ), ActTyp.end( ), a.id ); //查找3
			if ( resultA != ActTyp.end( ) ) //找到 领取
			{
				HaveA = true;
			}

			//判断B是已经领取
			auto resultB = find( ActTyp.begin( ), ActTyp.end( ), b.id ); //查找3
			if ( resultB != ActTyp.end( ) ) //找到 领取
			{
				HaveB = true;
			}

			if(!HaveA && HaveB)
			{
				return true;
			}
			else if(HaveA && !HaveB)
			{
				return false;
			}
			else
			{
				return a.id < b.id;
			}
		}
	});
}

void OperateActLayer::updateCommomBInfo(int id)
{
	closeAllPanel();
	m_ui.PnlCommon->setVisible(true);

	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	m_sOprateItemData = oprateItem;
	
	m_ui.imgTile->loadTexture("Image/UIProActivity/Image/" + TO_STR(oprateItem->res) + ".png");
	if (oprateItem->endTime <= -1)
	{
		m_ui.Img_TimeA->setVisible(false);
		m_ui.labTime1A->setVisible(false);

		m_ui.imgTimeB->setVisible(false);
		m_ui.labTimeB->setVisible(false);
	}
	else
	{
		m_ui.Img_TimeA->setVisible(true);
		m_ui.labTime1A->setVisible(true);

		m_ui.imgTimeB->setVisible(false);
		m_ui.labTimeB->setVisible(false);
	}
	m_ui.CommonTips->setString(oprateItem->tips2);
	setTextAddStroke(m_ui.CommonTips,Color3B(0xac,0x03,0x03),2);
	m_ui.labTime1A->setString(StringFormat::convertSecToFormatTime(oprateItem->daojiTime) + DataManager::getInstance()->searchCommonTexdtById(20326));
	setTextAddStroke(m_ui.labTime1A,Color3B(0x09,0x4e,0x51),2);

	CommomBSort(id);
	m_eCurActType = OPERATE_ACT_TYPE_COMMOMB;

	m_pCommonTabView->reloadData();

	m_OpComomABar->setScrollBarLengthPercent(m_pCommonTabView->getViewSize().height / m_pCommonTabView->getContentSize().height);
	m_OpComomABar->setScrollBarPercentY(0.0f);
}

bool OperateActLayer::isCommomBHave(int id)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	vector<int> ActTyp = OperateActModel::getInstance()->getComon9ById(id)->actTyp;
	COMMONB* common  = OperateActModel::getInstance()->getComon9ById(id);
	for(int i = 0; i < oprateItem->info.size(); i++)
	{
		if (comonBRefType( oprateItem->info.at(i),*common ))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), oprateItem->info.at(i).id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				return true;
			}
		}
	}

	return false;
}

bool OperateActLayer::isCommomBExhibition(int id,bool isEndTimeOut)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	if (isOpen(*oprateItem,isEndTimeOut))
	{
		if (oprateItem->disappear == -1) //奖励领取消失
		{
			vector<int> ActTyp = OperateActModel::getInstance()->getComon9ById(id)->actTyp;
			for(int i = 0; i < oprateItem->info.size(); i++)
			{
				auto result = find( ActTyp.begin( ), ActTyp.end( ), oprateItem->info.at(i).id ); //查找3
				if ( result == ActTyp.end( ) ) //没找到 能领取
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void OperateActLayer::CommomBSort(int id)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	vector<int> ActTyp = OperateActModel::getInstance()->getComon9ById(id)->actTyp;
	COMMONB* common  = OperateActModel::getInstance()->getComon9ById(id);

	sort(oprateItem->info.begin(),oprateItem->info.end(),[=](OPERATE_ACT_INFO_ITEM a,OPERATE_ACT_INFO_ITEM b)
	{
		bool canA = false;
		bool canB = false;
		//判断A是否能领取
		if (comonBRefType(a,*common))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), a.id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				canA = true;
			}
		}

		//判断B是否能领取
		if (comonBRefType(b,*common))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), b.id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				canB = true;
			}
		}

		if (canA && !canB)
		{
			return true;
		}
		else if (!canA && canB)
		{
			return false;
		}
		else
		{
			bool HaveA = false;
			bool HaveB = false;
			//判断A是已经领取
			auto resultA = find( ActTyp.begin( ), ActTyp.end( ), a.id ); //查找3
			if ( resultA != ActTyp.end( ) ) //找到 领取
			{
				HaveA = true;
			}

			//判断B是已经领取
			auto resultB = find( ActTyp.begin( ), ActTyp.end( ), b.id ); //查找3
			if ( resultB != ActTyp.end( ) ) //找到 领取
			{
				HaveB = true;
			}

			if(!HaveA && HaveB)
			{
				return true;
			}
			else if(HaveA && !HaveB)
			{
				return false;
			}
			else
			{
				return a.id < b.id;
			}
		}
	});
}

void OperateActLayer::_updateCommomBInfo(int id)
{
	closeAllPanel();
	m_ui.PnlCommon->setVisible(true);
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	m_sOprateItemData = oprateItem;

	m_ui.imgTile->loadTexture("Image/UIProActivity/Image/" + TO_STR(oprateItem->res) + ".png");
	if (oprateItem->endTime <= -1)
	{
		m_ui.Img_TimeA->setVisible(false);
		m_ui.labTime1A->setVisible(false);

		m_ui.imgTimeB->setVisible(false);
		m_ui.labTimeB->setVisible(false);
	}
	else
	{
		m_ui.Img_TimeA->setVisible(false);
		m_ui.labTime1A->setVisible(false);

		m_ui.imgTimeB->setVisible(true);
		m_ui.labTimeB->setVisible(true);
	}
	m_ui.CommonTips->setVisible(true);
	m_ui.CommonTips->setString(oprateItem->tips2);
	setTextAddStroke(m_ui.CommonTips,Color3B(0xac,0x03,0x03),2);
	m_ui.labTimeB->setString(StringFormat::convertSecToFormatTime(oprateItem->daojiTime));
	setTextAddStroke(m_ui.labTimeB,Color3B(0x09,0x4e,0x51),2);

	_CommomBSort(id);
	m_eCurActType = OPERATE_ACT_TYPE_COMMOMB_;

	m_pCommonTabView->reloadData();

	m_OpComomABar->setScrollBarLengthPercent(m_pCommonTabView->getViewSize().height / m_pCommonTabView->getContentSize().height);
	m_OpComomABar->setScrollBarPercentY(0.0f);
}

bool OperateActLayer::_isCommomBHave(int id)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	vector<int> ActTyp = OperateActModel::getInstance()->getComon10ById(id)->actTyp;
	COMMONB* common  = OperateActModel::getInstance()->getComon10ById(id);

	for(int i = 0; i < oprateItem->info.size(); i++)
	{
		if (comonBRefType( oprateItem->info.at(i),*common))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), oprateItem->info.at(i).id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				return true;
			}
		}
	}

	return false;
}

bool OperateActLayer::_isCommomBExhibition(int id,bool isEndTimeOut)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	if (isOpen(*oprateItem,isEndTimeOut))
	{
		if (oprateItem->disappear == -1) //奖励领取消失
		{
			vector<int> ActTyp = OperateActModel::getInstance()->getComon10ById(id)->actTyp;

			for(int i = 0; i < oprateItem->info.size(); i++)
			{
				auto result = find( ActTyp.begin( ), ActTyp.end( ), oprateItem->info.at(i).id ); //查找3
				if ( result == ActTyp.end( ) ) //没找到 能领取
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void OperateActLayer::_CommomBSort(int id)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	vector<int> ActTyp = OperateActModel::getInstance()->getComon10ById(id)->actTyp;
	COMMONB* common  = OperateActModel::getInstance()->getComon10ById(id);

	sort(oprateItem->info.begin(),oprateItem->info.end(),[=](OPERATE_ACT_INFO_ITEM a,OPERATE_ACT_INFO_ITEM b)
	{
		bool canA = false;
		bool canB = false;
		//判断A是否能领取
		if (comonBRefType(a,*common))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), a.id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				canA = true;
			}
		}

		//判断B是否能领取
		if (comonBRefType(a,*common))
		{
			auto result = find( ActTyp.begin( ), ActTyp.end( ), b.id ); //查找3
			if ( result == ActTyp.end( ) ) //没找到 能领取
			{
				canB = true;
			}
		}

		if (canA && !canB)
		{
			return true;
		}
		else if (!canA && canB)
		{
			return false;
		}
		else
		{
			bool HaveA = false;
			bool HaveB = false;
			//判断A是已经领取
			auto resultA = find( ActTyp.begin( ), ActTyp.end( ), a.id ); //查找3
			if ( resultA != ActTyp.end( ) ) //找到 领取
			{
				HaveA = true;
			}

			//判断B是已经领取
			auto resultB = find( ActTyp.begin( ), ActTyp.end( ), b.id ); //查找3
			if ( resultB != ActTyp.end( ) ) //找到 领取
			{
				HaveB = true;
			}

			if(!HaveA && HaveB)
			{
				return true;
			}
			else if(HaveA && !HaveB)
			{
				return false;
			}
			else
			{
				return a.id < b.id;
			}
		}
	});
}

void OperateActLayer::updateSingleRechargeInfo()
{
	closeAllPanel();
	m_ui.PnlCommon->setVisible(true);
	OPERATE_ACT_SORT_ITEM* oprateItem = searchActivyByType(OPERATE_ACT_TYPE_SINGLE_RECHARGE);
	m_sOprateItemData = oprateItem;

	m_ui.imgTile->loadTexture("Image/UIProActivity/Image/" + TO_STR(oprateItem->res) + ".png");
	if (oprateItem->endTime <= -1)
	{
		m_ui.Img_TimeA->setVisible(false);
		m_ui.labTime1A->setVisible(false);

		m_ui.imgTimeB->setVisible(false);
		m_ui.labTimeB->setVisible(false);
	}
	else
	{
		m_ui.Img_TimeA->setVisible(false);
		m_ui.labTime1A->setVisible(false);

		m_ui.imgTimeB->setVisible(true);
		m_ui.labTimeB->setVisible(true);
	}
	m_ui.CommonTips->setVisible(true);
	m_ui.CommonTips->setString(oprateItem->tips2);
	setTextAddStroke(m_ui.CommonTips,Color3B(0xac,0x03,0x03),2);
	m_ui.labTimeB->setString(StringFormat::convertSecToFormatTime(oprateItem->daojiTime));
	setTextAddStroke(m_ui.labTimeB,Color3B(0x09,0x4e,0x51),2);

	SingleRechargeSort();
	m_eCurActType = OPERATE_ACT_TYPE_SINGLE_RECHARGE;

	m_pCommonTabView->reloadData();

	m_OpComomABar->setScrollBarLengthPercent(m_pCommonTabView->getViewSize().height / m_pCommonTabView->getContentSize().height);
	m_OpComomABar->setScrollBarPercentY(0.0f);
}

bool OperateActLayer::isSingleRechargeHave()
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchActivyByType(OPERATE_ACT_TYPE_SINGLE_RECHARGE);
	SINGLE_RECHARGE* ActInfo11 = OperateActModel::getInstance()->getActInfo11();

	for(int i = 0; i < oprateItem->info.size(); i++)
	{
		if (singleRechargeRefType( oprateItem->info.at(i),ActInfo11->condition))
		{
			auto result = find( ActInfo11->actTyp.begin( ), ActInfo11->actTyp.end( ), oprateItem->info.at(i).id ); //查找3
			if ( result == ActInfo11->actTyp.end( ) ) //没找到 能领取
			{
				return true;
			}
		}
	}

	return false;
}

bool OperateActLayer::isSingleRechargeExhibition(bool isEndTimeOut /*= false*/)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchActivyByType(OPERATE_ACT_TYPE_SINGLE_RECHARGE);
	if (isOpen(*oprateItem,isEndTimeOut))
	{
		if (oprateItem->disappear == -1) //奖励领取消失
		{
			SINGLE_RECHARGE* ActInfo11 = OperateActModel::getInstance()->getActInfo11();

			for(int i = 0; i < oprateItem->info.size(); i++)
			{
				auto result = find( ActInfo11->actTyp.begin( ), ActInfo11->actTyp.end( ), oprateItem->info.at(i).id ); //查找3
				if ( result == ActInfo11->actTyp.end( ) ) //没找到 能领取
				{
					return true;
				}
			}

			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

void OperateActLayer::SingleRechargeSort()
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchActivyByType(OPERATE_ACT_TYPE_SINGLE_RECHARGE);
	SINGLE_RECHARGE* ActInfo11 = OperateActModel::getInstance()->getActInfo11();

	sort(oprateItem->info.begin(),oprateItem->info.end(),[=](OPERATE_ACT_INFO_ITEM a,OPERATE_ACT_INFO_ITEM b)
	{
		bool canA = false;
		bool canB = false;
		//判断A是否能领取
		if (singleRechargeRefType(a,ActInfo11->condition))
		{
			auto result = find( ActInfo11->actTyp.begin( ), ActInfo11->actTyp.end( ), a.id ); //查找3
			if ( result == ActInfo11->actTyp.end( ) ) //没找到 能领取
			{
				canA = true;
			}
		}

		//判断B是否能领取
		if (singleRechargeRefType(a,ActInfo11->condition))
		{
			auto result = find( ActInfo11->actTyp.begin( ), ActInfo11->actTyp.end( ), b.id ); //查找3
			if ( result == ActInfo11->actTyp.end( ) ) //没找到 能领取
			{
				canB = true;
			}
		}

		if (canA && !canB)
		{
			return true;
		}
		else if (!canA && canB)
		{
			return false;
		}
		else
		{
			bool HaveA = false;
			bool HaveB = false;
			//判断A是已经领取
			auto resultA = find( ActInfo11->actTyp.begin( ), ActInfo11->actTyp.end( ), a.id ); //查找3
			if ( resultA != ActInfo11->actTyp.end( ) ) //找到 领取
			{
				HaveA = true;
			}

			//判断B是已经领取
			auto resultB = find( ActInfo11->actTyp.begin( ), ActInfo11->actTyp.end( ), b.id ); //查找3
			if ( resultB != ActInfo11->actTyp.end( ) ) //找到 领取
			{
				HaveB = true;
			}

			if(!HaveA && HaveB)
			{
				return true;
			}
			else if(HaveA && !HaveB)
			{
				return false;
			}
			else
			{
				return a.id < b.id;
			}
		}
	});
}

bool OperateActLayer::comonARefType(int refType,OPERATE_ACT_INFO_ITEM info)
{
	if (refType == 9) //拥有英雄
	{
		if (HeroModel::getInstance()->searchHeroByTemplateId(info.amount1))
		{
			return true;
		}
	}
	else if (refType == 5) //主角等级
	{
		if (HeroModel::getInstance()->getMainHeroLevel() >= info.amount1)
		{
			return true;
		}
	}

	return false;
}

bool OperateActLayer::comonBRefType(OPERATE_ACT_INFO_ITEM info,COMMONB comB)
{
	if (info.refTyp == 4 || info.refTyp == 21 || info.refTyp == 22 || info.refTyp == 1 ||
		info.refTyp == 7 || info.refTyp == 14 || info.refTyp == 10 || info.refTyp == 11 ||
		info.refTyp == 12 || info.refTyp == 13 || info.refTyp == 17 || info.refTyp == 18 ||
		info.refTyp == 19 || info.refTyp == 23 || info.refTyp == 24 || info.refTyp == 26 ||
		info.refTyp == 28 || info.refTyp == 27 || info.refTyp == 29) 
		//4累计充值多少天,21天罡封印,22淬炼宝匣,1登录天数,7累计充值,10 普通关卡,11精英关卡
		//12金币消耗,13银币消耗,17竞技挑战,18爬塔挑战,19试炼挑战,23情缘送礼,24神器洗练,26掠夺狂人
		//28活动挑战,14精力充沛,27幻境 29历练任务
	{
		if (comB.condition >= info.amount1)
		{
			return true;
		}
	}
	else if (info.refTyp == 5) //主角等级
	{
		if (HeroModel::getInstance()->getMainHeroLevel() >= info.amount1)
		{
			return true;
		}
	}

	return false;
}

void OperateActLayer::comoneDoubleFallRefType(OPERATE_ACT_INFO_ITEM info)
{
	if (info.refTyp == 30 || info.refTyp == 31) //30普通关卡 31精英关卡
	{
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE))
		{
			Director::getInstance()->replaceScene(SceneChapterLoading::create());
		}
	}
	else if (info.refTyp == 32 || info.refTyp == 33 || info.refTyp == 34) //32梁山密藏 33久经考验 34江湖传闻
	{
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE_EVENT))
		{
			Director::getInstance()->replaceScene(SceneActivity::create());
		}
	}
	else if (info.refTyp == 35) //35体力购买
	{
		LayerCommonBuy* buyPower = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
		Director::getInstance()->getRunningScene()->addChild(buyPower);
	}
	else if (info.refTyp == 36) //36银币购买
	{
		LayerCommonBuy* buyCoin = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
		Director::getInstance()->getRunningScene()->addChild(buyCoin);
	}
}

bool OperateActLayer::singleRechargeRefType(OPERATE_ACT_INFO_ITEM info,vector<int> condition)
{
	if (info.refTyp == 8 ) 
		//8单笔充值
	{
		for (int i = 0; i < condition.size(); i++)
		{
			if (condition.at(i) == info.id)
			{
				return true;
			}
		}
	}

	return false;
}


bool OperateActLayer::isOpen(OPERATE_ACT_SORT_ITEM info,bool isEndTimeOut)
{
	if (isEndTimeOut && info.daojiTime <= 0)
	{
		return false;
	}

	int openTime = OperateActModel::getInstance()->getOpenTime();//开启
	int createTime = OperateActModel::getInstance()->getCreateTime();//创建
	int localTime = LocalTimer::getInstance()->getCurServerTime();

	int severTime = 0;  //实际服务器开启
	int roleTime = 0;   //实际角色开启
	int trueOpenTime = 0;   //开启时间

	if (info.timeType == OPERATE_TIME_TYPE_SEVER)
	{
		trueOpenTime = openTime + info.openTime*24*3600;
	}
	else if (info.timeType == OPERATE_TIME_TYPE_CREATE)
	{
		trueOpenTime = createTime + info.openTime*24*3600;
	}

	if (info.timeType == OPERATE_TIME_TYPE_SEVER || info.timeType == OPERATE_TIME_TYPE_CREATE) //开服时间
	{
		if (info.roleTime <= 0 && info.serverTime <= 0)
		{
			if (info.endTime <= -1)
			{
				if (localTime < trueOpenTime )
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				if (localTime < trueOpenTime || localTime > trueOpenTime+info.endTime )
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
		else if (info.roleTime > 0 && info.serverTime <= 0)
		{
			roleTime = createTime + info.roleTime*24*3600;
			if (roleTime <= trueOpenTime)
			{
				if (info.endTime <= -1)
				{
					if (localTime < trueOpenTime )
					{
						return false;
					}
					else
					{
						return true;
					}
				}
				else
				{
					if (localTime < trueOpenTime || localTime > trueOpenTime+info.endTime )
					{
						return false;
					}
					else
					{
						return true;
					}
				}
			}
			else
			{
				return false;
			}
		}
		else if (info.roleTime <= 0 && info.serverTime > 0)
		{
			severTime = openTime + info.serverTime*24*3600;
			if (severTime <= trueOpenTime)
			{
				if (info.endTime <= -1)
				{
					if (localTime < trueOpenTime )
					{
						return false;
					}
					else
					{
						return true;
					}
				}
				else
				{
					if (localTime < trueOpenTime || localTime > trueOpenTime+info.endTime )
					{
						return false;
					}
					else
					{
						return true;
					}
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			severTime = openTime + info.serverTime*24*3600;
			roleTime = createTime + info.roleTime*24*3600;
			if (severTime <= trueOpenTime && roleTime <= trueOpenTime)
			{
				if (info.endTime <= -1)
				{
					if (localTime < trueOpenTime )
					{
						return false;
					}
					else
					{
						return true;
					}
				}
				else
				{
					if (localTime < trueOpenTime || localTime > trueOpenTime+info.endTime )
					{
						return false;
					}
					else
					{
						return true;
					}
				}
			}
			else
			{
				return false;
			}
		}
	}
	else if (info.timeType == OPERATE_TIME_TYPE_LIMIT)//限时时间
	{
		if (info.roleTime <= 0 && info.serverTime <= 0)
		{
			if (localTime < info.openTime || localTime > info.endTime )
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else if (info.roleTime > 0 && info.serverTime <= 0)
		{
			roleTime = createTime + info.roleTime*24*3600;
			if (roleTime <= info.openTime)
			{
				if (localTime < info.openTime || localTime > info.endTime )
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		else if (info.roleTime <= 0 && info.serverTime > 0)
		{
			severTime = openTime + info.serverTime*24*3600;
			if (severTime <= info.openTime)
			{
				if (localTime < info.openTime || localTime > info.endTime )
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			severTime = openTime + info.serverTime*24*3600;
			roleTime = createTime + info.roleTime*24*3600;
			if (severTime <= info.openTime && roleTime <= info.openTime)
			{
				if (localTime < info.openTime || localTime > info.endTime )
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				return false;
			}
		}
	}
}

OPERATE_ACT_SORT_ITEM* OperateActLayer::searchActivyByType(OPERATE_ACT_TYPE type)
{
	for (auto iter = m_operatSort.begin(); iter != m_operatSort.end(); iter++)
	{
		if (iter->type == type)
		{
			return &(*iter);
		}
	}
	return nullptr;
}

OPERATE_ACT_SORT_ITEM* OperateActLayer::searchBuChongActivyById(int id)
{
	for (auto iter = m_operatSort.begin(); iter != m_operatSort.end(); iter++)
	{
		if (iter->id == id)
		{
			return &(*iter);
		}
	}
	return nullptr;
}

void OperateActLayer::tableCellTouched( extension::TableView* table, extension::TableViewCell* cell )
{

}

Size OperateActLayer::cellSizeForTable( extension::TableView *table )
{
	return m_ui.PnlCommonItem->getContentSize();
}

extension::TableViewCell* OperateActLayer::tableCellAtIndex( extension::TableView *table, ssize_t idx )
{
	extension::TableViewCell* cell = table->dequeueCell();
	if(!cell)
	{
		cell = extension::TableViewCell::create();
	}

	cell->removeAllChildren();
	Layout* pnlItem = (Layout*)m_ui.PnlCommonItem->clone();
	pnlItem->setPosition(cell->getContentSize()/2);
	pnlItem->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	pnlItem->setVisible(true);
	cell->addChild(pnlItem);
	pnlItem->setName("pnlItem");

	//设置cell
	setCellContent(cell, idx);

	return cell;
}

ssize_t OperateActLayer::numberOfCellsInTableView( extension::TableView *table )
{
	if(m_sOprateItemData == nullptr)
	{
		return 0;
	}

	return m_sOprateItemData->info.size();
}

void OperateActLayer::scrollViewDidScroll( extension::ScrollView* view )
{
	Size size = view->getContentSize();
	Vec2 vec = view->getContentOffset();

	//滚动条重设进度
	float percentY = -vec.y / (size.height - view->getViewSize().height);
	m_OpComomABar->setScrollBarPercentY((1.0f - percentY)*100.0f);	
}

void OperateActLayer::setCellContent( extension::TableViewCell* cell, ssize_t idx )
{
	Layout* pnlItem = (Layout*)cell->getChildByName("pnlItem");
	if(pnlItem == nullptr)
	{
		return;
	}
	//int idx = cell->getIdx();
	OPERATE_ACT_INFO_ITEM info = m_sOprateItemData->info.at(idx);
	
	Text* Lab_Tips = dynamic_cast<Text*>(Helper::seekWidgetByName(pnlItem,"Lab_Tips"));
	Lab_Tips->setFontName(FONT_FZZHENGHEI);
	Lab_Tips->setString(info.actInfo);
	setTextAddStroke(Lab_Tips,Color3B(0x9e,0x0f,0x0f),2);
	

	//奖励
	VEV_OPERATEA_PRIZE_ITEM sevenItem = info.reward;
	for (int j = 0; j < sevenItem.size(); j++)
	{
		Layout *itemPrize = dynamic_cast<Layout*>(Helper::seekWidgetByName(pnlItem,"Pnl_item" + TO_STR(j+1)));
		itemPrize->removeAllChildren();

		UIToolHeader *icon = UIToolHeader::create(sevenItem[j].id);
		icon->setNumEx(sevenItem[j].amount);
		icon->setAnchorPoint(Vec2(0,0));
		icon->setScale(0.8f);
		icon->setToolId(sevenItem[j].id);
		icon->showLightEff();
		icon->setTipsEnabled(true);
		icon->setName("UIToolHeader");
		onBtnTouchEvent((Button*)icon);
		itemPrize->addChild(icon);
	}

	setCommonListState(pnlItem, idx);
}

void OperateActLayer::setCommonListState(Layout* pnlItem, int idx)
{
	//UI
	Button* Btn_Pay = dynamic_cast<Button*>(Helper::seekWidgetByName(pnlItem,"Btn_Pay"));
	Button* Btn_Get = dynamic_cast<Button*>(Helper::seekWidgetByName(pnlItem,"Btn_Get"));
	ImageView *getEd = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pnlItem,"getEd"));
	Button* chongzhi = dynamic_cast<Button*>(Helper::seekWidgetByName(pnlItem,"chongzhi"));
	Text* Lab_Tips_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(pnlItem,"Lab_Tips_0"));	
	Lab_Tips_0->setFontName(FONT_FZZHENGHEI);Lab_Tips_0->setVisible(true);
	Btn_Pay->setVisible(false);
	Btn_Get->setVisible(false);
	getEd->setVisible(false);
	chongzhi->setVisible(false);
	Btn_Get->setBright(false);
	Btn_Get->loadTextureNormal("Image/UIProActivity/Image/btn_lingqu_1.png");

	//状态数据
	bool isCanGet = false;
	OPERATE_ACT_INFO_ITEM info = m_sOprateItemData->info.at(idx);
	COMMONA *commonA = nullptr;
	COMMONB* commonB = nullptr;
	SINGLE_RECHARGE* ActInfo11;
	if(m_eCurActType == OPERATE_ACT_TYPE_COMMOMA || m_eCurActType == OPERATE_ACT_TYPE_COMMOMA_)
	{
		if(m_eCurActType == OPERATE_ACT_TYPE_COMMOMA)
		{
			commonA  = OperateActModel::getInstance()->getComon7ById(m_sOprateItemData->id);
		}
		else
		{
			commonA  = OperateActModel::getInstance()->getComon8ById(m_sOprateItemData->id);
		}
		isCanGet = comonARefType(info.refTyp,info);
	}
	else if(m_eCurActType == OPERATE_ACT_TYPE_COMMOMB || m_eCurActType == OPERATE_ACT_TYPE_COMMOMB_)
	{
		if(m_eCurActType == OPERATE_ACT_TYPE_COMMOMB)
		{
			commonB  = OperateActModel::getInstance()->getComon9ById(m_sOprateItemData->id);
		}
		else
		{
			commonB  = OperateActModel::getInstance()->getComon10ById(m_sOprateItemData->id);
		}
		isCanGet = comonBRefType(info,*commonB);
	}
	else if(m_eCurActType == OPERATE_ACT_TYPE_SINGLE_RECHARGE)
	{
		ActInfo11 = OperateActModel::getInstance()->getActInfo11();
		
		isCanGet = singleRechargeRefType(info,ActInfo11->condition);
	}

	if(isCanGet)
	{
		vector<int>::iterator result;
		if(m_eCurActType == OPERATE_ACT_TYPE_COMMOMA || m_eCurActType == OPERATE_ACT_TYPE_COMMOMA_)
		{
			result = find( commonA->actTyp.begin( ), commonA->actTyp.end( ), info.id );
			//查找
			if ( result == commonA->actTyp.end()) //没找到 能领取
			{
				Btn_Get->setVisible(true);
				Btn_Get->setBright(true);
			}
			else //找到 已领取
			{
				getEd->setVisible(true);
				Lab_Tips_0->setVisible(false);
			}
		}
		else if(m_eCurActType == OPERATE_ACT_TYPE_COMMOMB || m_eCurActType == OPERATE_ACT_TYPE_COMMOMB_)
		{
			result = find( commonB->actTyp.begin( ), commonB->actTyp.end( ), info.id );
			//查找
			if ( result == commonB->actTyp.end()) //没找到 能领取
			{
				Btn_Get->setVisible(true);
				Btn_Get->setBright(true);
			}
			else //找到 已领取
			{
				getEd->setVisible(true);
				Lab_Tips_0->setVisible(false);
			}
		}
		else if(m_eCurActType == OPERATE_ACT_TYPE_SINGLE_RECHARGE)
		{
			result = find( ActInfo11->actTyp.begin( ), ActInfo11->actTyp.end( ), info.id );
			//查找
			if ( result == ActInfo11->actTyp.end()) //没找到 能领取
			{
				Btn_Get->setVisible(true);
				Btn_Get->setBright(true);
			}
			else //找到 已领取
			{
				getEd->setVisible(true);
				Lab_Tips_0->setVisible(false);
			}
		}
	}
	else
	{
		Btn_Get->setVisible(true);
		Btn_Get->setBright(false);
	}

	onBtnTouchEvent(Btn_Get);

	//设置进度
	if(Lab_Tips_0->isVisible())
	{
		if(m_eCurActType == OPERATE_ACT_TYPE_COMMOMA || m_eCurActType == OPERATE_ACT_TYPE_COMMOMA_)
		{
			int num = 0;		//进度值
			int goal = 0;		//目标值
			if(info.refTyp == 9)
			{
				//拥有指定英雄
				if (HeroModel::getInstance()->searchHeroByTemplateId(info.amount1))
				{
					num = 1;
				}
				goal = 1;
			}
			else if(info.refTyp == 5)
			{
				//主角等级
				num = HeroModel::getInstance()->getMainHeroLevel();
				goal = info.amount1;
			}
			Lab_Tips_0->setString("(" + TO_STR(num) + "/" + TO_STR(goal) + ")");
			if(num < goal)
			{
				Lab_Tips_0->setColor(Color3B::WHITE);
			}
			else
			{
				Lab_Tips_0->setColor(Color3B(0x54, 0xdd, 0x51));
			}
		}
		else if(m_eCurActType == OPERATE_ACT_TYPE_COMMOMB)
		{
			Lab_Tips_0->setString("(" + TO_STR(commonB->condition) + "/" + TO_STR(info.amount1) + ")");
			if(commonB->condition < info.amount1)
			{
				Lab_Tips_0->setColor(Color3B::WHITE);
			}
			else
			{
				Lab_Tips_0->setColor(Color3B(0x54, 0xdd, 0x51));
			}
		}
		else if(m_eCurActType == OPERATE_ACT_TYPE_COMMOMB_)
		{
			int num = 0;
			int goal = info.amount1;
			if (info.refTyp == 5) //主角等级
			{
				num = HeroModel::getInstance()->getMainHeroLevel();
			}
			else
			{
				num = commonB->condition;
			}

			Lab_Tips_0->setString("(" + TO_STR(num) + "/" + TO_STR(goal) + ")");
			if(num < goal)
			{
				Lab_Tips_0->setColor(Color3B::WHITE);
			}
			else
			{
				Lab_Tips_0->setColor(Color3B(0x54, 0xdd, 0x51));
			}
		}
		else if (m_eCurActType == OPERATE_ACT_TYPE_SINGLE_RECHARGE)
		{
			int num = 0;
			int goal = info.amount1;
			for (int i = 0; i < ActInfo11->condition.size(); i++ )
			{
				if (ActInfo11->condition.at(i) == info.id)
				{
					num = info.amount1;
					break;
				}
			}

			Lab_Tips_0->setString("(" + TO_STR(num) + "/" + TO_STR(goal) + ")");
			if(num < goal)
			{
				Lab_Tips_0->setColor(Color3B::WHITE);
			}
			else
			{
				Lab_Tips_0->setColor(Color3B(0x54, 0xdd, 0x51));
			}
		}
	}
}

void OperateActLayer::onBtnTouchEvent( Button *btn )
{
	btn->setTouchEnabled(false);
	auto _touchListener = EventListenerTouchOneByOne::create();
	_touchListener->setSwallowTouches(false);
	_touchListener->onTouchBegan = [=](Touch* touch, Event* event)->bool {
		Point locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
		Size size = event->getCurrentTarget()->getContentSize();
		Rect rect = Rect(0.0f, 0.0f, size.width, size.height);

		Point locationInParent = m_ui.PnlTbView->convertToNodeSpace(touch->getLocation());
		Rect rectParent = Rect(0, 0, m_ui.PnlTbView->getContentSize().width,
			m_ui.PnlTbView->getContentSize().height);

		if (rect.containsPoint(locationInNode) && rectParent.containsPoint(locationInParent) && m_ui.PnlCommon->isVisible())
		{
			if(event->getCurrentTarget()->getName() == "UIToolHeader")
			{
				UIToolHeader* toolBtn = (UIToolHeader*)event->getCurrentTarget();
			}
			else if(event->getCurrentTarget()->getName() == "Btn_Get")
			{
				Button* touchBtn = (Button*)event->getCurrentTarget();
				if(touchBtn->isBright())
					touchBtn->loadTextureNormal("Image/UIProActivity/Image/btn_lingqu_2.png");
			}
			
			m_bIsSelect = false;
			return true;
		}
		return false;
	};
	_touchListener->onTouchEnded = [=](Touch* touch, Event* event)->void {
		if(event->getCurrentTarget()->getName() == "UIToolHeader")
		{
			UIToolHeader* toolBtn = (UIToolHeader*)event->getCurrentTarget();

			float offset = (touch->getStartLocation() - touch->getLocation()).getLength();
			toolBtn->showTipsNow(true, offset);
		}
		else if(event->getCurrentTarget()->getName() == "Btn_Get")
		{
			Button* touchBtn = (Button*)event->getCurrentTarget();
			if(touchBtn->isBright())
				touchBtn->loadTextureNormal("Image/UIProActivity/Image/btn_lingqu_1.png");
		}

		if(m_bIsSelect)
		{
			return;
		}
		else
		{
			if(event->getCurrentTarget()->getName() == "UIToolHeader")
			{
				UIToolHeader* toolBtn = (UIToolHeader*)event->getCurrentTarget();
				float offset = (touch->getStartLocation() - touch->getLocation()).getLength();
				toolBtn->showTipsNow(true, offset);
			}
			else if(event->getCurrentTarget()->getName() == "Btn_Get")
			{
				Button* touchBtn = (Button*)event->getCurrentTarget();
				if(touchBtn->isBright())
				{
					SoundUtils::playSoundEffect("dianji");

					Layout* itemRoot = (Layout*)(touchBtn->getParent()->getParent());
					extension::TableViewCell* cell = (extension::TableViewCell*)(itemRoot->getParent());
					OPERATE_ACT_INFO_ITEM info = m_sOprateItemData->info.at(cell->getIdx());
					OperateActController::getInstance()->getOprateActtask(info.id);
				}
			}
		}
	};
	_touchListener->onTouchMoved = [=](Touch* touch, Event* event)->void {
		if (abs(touch->getDelta().x) > 30.0f || abs(touch->getDelta().y) > 30.0f)
		{
			m_bIsSelect = true;
		}
		else
		{
			if(event->getCurrentTarget()->getName() == "UIToolHeader")
			{
				UIToolHeader* toolBtn = (UIToolHeader*)event->getCurrentTarget();
			}
		}
	};
	_touchListener->onTouchCancelled = [=](Touch* touch, Event* event)->void {
		if(event->getCurrentTarget()->getName() == "UIToolHeader")
		{
			UIToolHeader* toolBtn = (UIToolHeader*)event->getCurrentTarget();
		}
		else if(event->getCurrentTarget()->getName() == "Btn_Get")
		{
			Button* touchBtn = (Button*)event->getCurrentTarget();
			if(touchBtn->isBright())
				touchBtn->loadTextureNormal("Image/UIProActivity/Image/btn_lingqu_1.png");
		}
	};
	btn->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, btn);
}

void OperateActLayer::updateVipGiftInfo()
{
	closeAllPanel();
	m_ui.PnlVipGift->setVisible(true);
	//防止反复切换都要重新更新界面
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_VIPBAG);
	if (item->isInit)
	{
		return;
	}
	item->isInit = true;

	m_ui.imgVipGiftTitle->loadTexture("Image/UIProActivity/Image/" + TO_STR(item->res) + ".png");
	m_ui.imgVipGiftBigTitle->loadTexture("Image/UIProActivity/Image/img_di5.png");
	m_ui.vipGiftTips->setString(item->tips2);
	setTextAddStroke(m_ui.vipGiftTips,Color3B(0xac,0x03,0x03),2);
	setTextAddStroke(m_ui.vipGiftTips2,Color3B(0xac,0x03,0x03),2);
	m_ui.vipGiftLabTime->setString(StringFormat::convertSecToFormatTime(item->daojiTime));
	setTextAddStroke(m_ui.vipGiftLabTime,Color3B(0x09,0x4e,0x51),2);
	m_ui.vipGiftMyVip->loadTexture("Image/Icon/vip/btn_VIP" + TO_STR(MainModel::getInstance()->getMainParam()->mVipLevel) + ".png");
	
	auto call = [this,item](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			vector<int> Vecprize;
			StringFormat::parseCsvStringVec(item->info.at(m_vipCheckBoxIdx).price, Vecprize);
			MainParam* mainParam = MainModel::getInstance()->getMainParam();
			if (mainParam->mVipLevel < item->info.at(m_vipCheckBoxIdx).amount1)
			{
				CustomTips::showUtf8("VIP" + TO_STR(item->info.at(m_vipCheckBoxIdx).amount1) +
					DataManager::getInstance()->searchCommonTexdtById(20243));
			}
			else if (mainParam->mGold < Vecprize.at(1))
			{
				CustomGoldTips *_tips = CustomGoldTips::create();
				if (_tips)
				{
					Director::getInstance()->getRunningScene()->addChild(_tips);
				}
			}
			else
			{
				OperateActController::getInstance()->BuyVipGift(item->info.at(m_vipCheckBoxIdx).id,Vecprize.at(1));
			}
		}
	};
	m_ui.vipGiftBuy->addTouchEventListener(call);

	vector<int>& ActTyp14 = OperateActModel::getInstance()->getActTyp14();  //#已领取的全民福利ID
	MainParam *Mainparam = MainModel::getInstance()->getMainParam();
	m_ui.ListView_VIP->removeAllChildren();
	for(int i = 0; i < item->info.size(); i++)
	{
		CheckBox *check = dynamic_cast<CheckBox *>(m_ui.VipCheck->clone());
		check->setVisible(true);
		Text *vipText = dynamic_cast<Text *>(Helper::seekWidgetByName(check,"Label_23"));
		vipText->setFontName(FONT_FZZHENGHEI);
		vipText->setString("VIP" + TO_STR(item->info.at(i).amount1));
		ImageView *tishi = dynamic_cast<ImageView *>(Helper::seekWidgetByName(check,"hongdiantishi"));
		tishi->setVisible(false);
		if (Mainparam->mVipLevel >= item->info.at(i).amount1)
		{
			vector<int>::iterator result = find( ActTyp14.begin( ), ActTyp14.end( ), item->info.at(i).id ); //查找3
			if ( result == ActTyp14.end() ) //没找到 可领取
			{
				tishi->setVisible(true);
			}
		}

		if (i == m_vipCheckBoxIdx)
		{
			check->setSelectedState(true);
			check->setTouchEnabled(false);
			vipText->setColor(Color3B(0xd7,0xd2,0x90));
			setTextAddStroke(vipText,Color3B(0xac,0x03,0x03),2);
		}
		else
		{
			check->setSelectedState(false);
			check->setTouchEnabled(true);
			vipText->setColor(Color3B(0xcc,0xbf,0xaa));
			setTextAddStroke(vipText,Color3B(0x3f,0x2a,0x2a),2);
		}

		auto callCheck = [this,check,i](Ref* sender,CheckBox::EventType type)->void
		{
			if (type == CheckBox::EventType::SELECTED)
			{
				m_vipCheckBoxIdx = i;

				SoundUtils::playSoundEffect("dianji");
				check->setTouchEnabled(false);
				Text *vipText = dynamic_cast<Text *>(Helper::seekWidgetByName(check,"Label_23"));
				vipText->setColor(Color3B(0xd7,0xd2,0x90));
				setTextAddStroke(vipText,Color3B(0xac,0x03,0x03),2);

				Vector<Widget*>& Items = m_ui.ListView_VIP->getItems();
				for (auto item:Items)
				{
					CheckBox *_check = dynamic_cast<CheckBox*>(item);
					if (_check != check)
					{
						_check->setTouchEnabled(true);
						_check->setSelectedState(false);
						Text *vipText = dynamic_cast<Text *>(Helper::seekWidgetByName(_check,"Label_23"));
						vipText->setColor(Color3B(0xcc,0xbf,0xaa));
						setTextAddStroke(vipText,Color3B(0x3f,0x2a,0x2a),2);
					}
					else
					{
						setVipGiftCHoose(m_vipCheckBoxIdx);
					}
				}
			}
		};
		check->addEventListener(callCheck);
		
		m_ui.ListView_VIP->pushBackCustomItem(check);
	}

	setVipGiftCHoose(m_vipCheckBoxIdx);
}

bool OperateActLayer::isVipGiftHave()
{
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_VIPBAG);

	bool ret = false;
	MainParam *Mainparam = MainModel::getInstance()->getMainParam();
	vector<int>& ActTyp14 = OperateActModel::getInstance()->getActTyp14();  //#已领取的全民福利ID
	for(int i = 0; i < item->info.size(); i++)
	{
		OPERATE_ACT_INFO_ITEM info = item->info.at(i);
		if (Mainparam->mVipLevel >= info.amount1)
		{
			vector<int>::iterator result = find( ActTyp14.begin( ), ActTyp14.end( ), info.id ); //查找3
			if ( result == ActTyp14.end( ) ) //没找到 能领取
			{
				ret = true;
				return ret;
			}
		}
	}

	return ret;
}

bool OperateActLayer::isVipGiftExhibition(bool isEndTimeOut /*= false*/)
{
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_VIPBAG);
	if (isOpen(*item,isEndTimeOut))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void OperateActLayer::setVipGiftCHoose(int idx)
{
	OPERATE_ACT_SORT_ITEM* item = searchActivyByType(OPERATE_ACT_TYPE_VIPBAG);
	vector<int>& ActTyp14 = OperateActModel::getInstance()->getActTyp14();  //#已领取的全民福利ID
	bool isfind = false;
	for(int i = 0; i < item->info.size(); i++)
	{
		OPERATE_ACT_INFO_ITEM info = item->info.at(i);
		if (info.amount1 == idx)
		{
			vector<int>::iterator result = find( ActTyp14.begin( ), ActTyp14.end( ), info.id ); //查找3
			if ( result != ActTyp14.end( ) ) //找到 已领取
			{
				isfind = true;
				break;
			}
		}
	}
	if (isfind)
	{
		m_ui.vipGiftBuy->setVisible(false);
		m_ui.vipGiftHaveBuy->setVisible(true);
	}
	else
	{
		m_ui.vipGiftBuy->setVisible(true);
		m_ui.vipGiftHaveBuy->setVisible(false);
	}
	vector<int> Vecprize;
	StringFormat::parseCsvStringVec(item->info.at(idx).price, Vecprize);
	m_ui.vipGiftLabPrice->setString(TO_STR(Vecprize.at(0)));
	setTextAddStroke(m_ui.vipGiftLabPrice,Color3B(0x00,0x00,0x00),2);
	m_ui.vipGiftLabVipPrice->setString(TO_STR(Vecprize.at(1)));
	setTextAddStroke(m_ui.vipGiftLabVipPrice,Color3B(0x00,0x00,0x00),2);
	m_ui.vipGiftLabGiftName->setString(item->info.at(idx).actInfo);
	setTextAddStroke(m_ui.vipGiftLabGiftName,Color3B(0xac,0x03,0x03),2);
	m_ui.vipGiftLabVip->setString("VIP" + TO_STR(item->info.at(idx).amount1));
	setTextAddStroke(m_ui.vipGiftLabVip,Color3B(0xac,0x03,0x03),2);
	m_ui.vipGiftTips2->setPosition(Vec2(m_ui.vipGiftLabVip->getPosition().x + m_ui.vipGiftLabVip->getContentSize().width + 2,
		m_ui.vipGiftTips2->getPosition().y));

	if (m_ui.vipHead->getChildByName("imgHero"))
	{
		m_ui.vipHead->removeChildByName("imgHero");
	}

	UIToolHeader *head = UIToolHeader::create(item->info.at(idx).reward.at(0).id);
	head->setNumEx(item->info.at(idx).reward.at(0).amount);
	head->setToolId(item->info.at(idx).reward.at(0).id);
	head->setTipsEnabled(true);
	head->setAnchorPoint(Vec2(0,0));
	m_ui.vipHead->addChild(head,0,"imgHero");
}

void OperateActLayer::updateDoubleFallInfo(int id)
{
	closeAllPanel();
	m_ui.Pnl_Double->setVisible(true);
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	m_ui.doubleImgTips->loadTexture("Image/UIProActivity/Image/" + TO_STR(oprateItem->res) + ".png");
	if (m_ui.doubleTexTips->getChildByName("upfontText"))
	{
		m_ui.doubleTexTips->removeChildByName("upfontText");
	}
	bool ignoreSize = m_ui.doubleTexTips->isIgnoreContentAdaptWithSize();
	int fontSize = m_ui.doubleTexTips->getFontSize();
	Size ContentSize = m_ui.doubleTexTips->getContentSize();
	Text* up = Text::create(oprateItem->tips2, FONT_FZZHENGHEI, fontSize); 
	up->setContentSize(ContentSize);
	up->ignoreContentAdaptWithSize(ignoreSize);
	up->setColor(m_ui.doubleTexTips->getColor());  
	up->setPosition(Vec2(0,100));
	up->setAnchorPoint(Vec2(0,1.0f));
	m_ui.doubleTexTips->addChild(up,0,"upfontText");
	setTextAddStroke(up,Color3B(0xac,0x03,0x03),2);

	m_ui.doubleTexTime->setString(convertCurrDateBycSec(oprateItem->openTime) + "--" + convertCurrDateBycSec(oprateItem->endTime));
	setTextAddStroke(m_ui.doubleTexTime,Color3B(0x09,0x4e,0x51),2);

	auto call = [this,oprateItem](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			if (oprateItem->info.size() > 0)
			{
				comoneDoubleFallRefType(oprateItem->info.at(0));
			}
			else
			{
				//CustomTips::show("老司机，配表有问题");
			}
		}
	};
	m_ui.doubleBtnGo->addTouchEventListener(call);
}

bool OperateActLayer::isDoubleFallExhibition(int id,bool isEndTimeOut /*= false*/)
{
	OPERATE_ACT_SORT_ITEM* oprateItem = searchBuChongActivyById(id);
	if (isOpen(*oprateItem,isEndTimeOut))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void OperateActLayer::updateLimitBuyInfo()
{

}

bool OperateActLayer::isLimitBuyHave()
{
	return true;
}

bool OperateActLayer::isLimitBuyExhibition(bool isEndTimeOut /*= false*/)
{
	return true;
}