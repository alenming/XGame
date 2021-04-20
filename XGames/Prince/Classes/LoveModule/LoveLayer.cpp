#include "LoveLayer.h"
#include "Widget/LayerGameRules.h"
#include "Temp/CustomGoldTips.h"
#include "SDK_TalkingData/TalkingData.h"
#include "VIPModule/VipModel.h"
#include "ToolModule/AniButton.h"

LoveLayer::LoveLayer()
{
	m_state = LOVE_LAYER_STATE_NONE;
	m_customScrollBarHero = nullptr;
	m_customScrollBarGift = nullptr;

	m_GooddGiftAdd = 0;
	m_OrdinaryGiftAdd = 0;
	m_BadGiftAdd = 0;
	m_canGuess = true;
	m_canVisit = true;
	m_canBack = true;
	m_isFirstAddHero = true;
	m_playMapIdx = -1;
	m_isChangeMing = false;
	m_canBuyWin = true;
}

LoveLayer::~LoveLayer()
{
}

LoveLayer* LoveLayer::create(bool pushScene)
{
	LoveLayer *pRet = new LoveLayer();
	if (pRet && pRet->init(pushScene))
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

bool LoveLayer::init(bool pushScene)
{
	m_isPushScene = pushScene;
	if (!Layer::init())
	{
		return false;
	}

	m_chooseHeroPetId = -1;
	initUi();
	initVipUI_1();
	initVipUI_2();

	return true;
}

void LoveLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIqingyuan/UIqingyuan.ExportJson"));
	this->addChild(m_ui.pRoot);	
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_love_bg.jpg");

	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED && m_canBack)
		{
			switch (m_state)
			{
			case LOVE_LAYER_STATE_NONE:
				break;
			case LOVE_LAYER_STATE_ENTER:
				if (m_isPushScene)
				{
					Director::getInstance()->popScene();
				}
				else
				{
					Director::getInstance()->replaceScene(MainCityScene::create());
				}
				break;
			case LOVE_LAYER_STATE_HERO:
				{
					m_canBack = false;

					m_state = LOVE_LAYER_STATE_ENTER;
					switchState();
					exitHero();
				}
				break;
			case LOVE_LAYER_STATE_PLAY:
				{
					m_canBack = false;

					m_state = LOVE_LAYER_STATE_HERO;
					switchState();
					exitPlay();
					setHeroUpdtaInfo();
				}
				break;
			case LOVE_LAYER_STATE_GIFT:
				{
					m_canBack = false;

					m_state = LOVE_LAYER_STATE_HERO;
					switchState();
					exitGift();
					setHeroUpdtaInfo();
				}
				break;
			case LOVE_LAYER_STATE_GUESS:
				break;
			case LOVE_LAYER_STATE_DRINK:
				{
					m_canBack = false;

					m_state =  LOVE_LAYER_STATE_HERO;
					switchState();
					initHero();
					exitDrink();
				}
				break;
			default:
				break;
			}
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_LOVE);
	_layerCommHeadInfo->setPosition(Vec2(-pos.x,pos.y));
	m_ui.pRoot->addChild(_layerCommHeadInfo);

	m_ui.phelp = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Btn_help"));
	m_ui.phelp->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify, this));	
	//对白
	m_ui.pPnlDuiDai = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"duibai"));
	m_ui.pImgDuiDai = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_duibai"));
	m_ui.pImgDuiDaiPos = m_ui.pImgDuiDai->getPosition();
	m_ui.pImgDuiDai->setVisible(true);
	m_ui.pImgDuiDaiNpc = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_name"));
	m_ui.pImgDuiDaiHero = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_name_hero"));
	m_ui.pTextNpcName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pImgDuiDaiNpc,"npc_name"));
	m_ui.pTextHeroName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pImgDuiDaiHero,"hero_name"));
	m_ui.pTextDuiBaiContext = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pImgDuiDai,"Lab_duibai"));
	m_ui.BtnextDuibai = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pImgDuiDai,"Btn_next"));
	m_ui.BtnextDuibai->setVisible(false);
	m_ui.pTextNpcName->setFontName(FONT_FZZHENGHEI);
	m_ui.pTextHeroName->setFontName(FONT_FZZHENGHEI);
	m_ui.pTextDuiBaiContext->setFontName(FONT_FZZHENGHEI);
	//主界面
	m_ui.pPnlMain = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_main"));
	m_ui.pPnlHeroItem = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlMain,"Pnl_heroItem"));
	m_ui.pPnlHeroItemPos = m_ui.pPnlHeroItem->getPosition();
	m_ui.pPnlHeroItem->setPosition(Vec2(m_ui.pPnlMain->getContentSize().width,m_ui.pPnlHeroItemPos.y));

	m_ui.pPnlHeroItem->runAction(CCSequence::create(
		CCEaseSineOut::create(CCMoveTo::create(enterMain_time,m_ui.pPnlHeroItemPos)), nullptr));

	m_ui.pImgWaiter = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlMain,"Img_laobanniang"));
	m_ui.pImgWaiterPos = m_ui.pImgWaiter->getPosition();

	//主界面英雄列表
	m_ui.pHeroList = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pPnlMain,"List_hero"));
	ImageView* pHeroListDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlMain,"Img_di2"));
	m_customScrollBarHero = CustomScrollBar::create();
	m_customScrollBarHero->initScrollBar(m_ui.pHeroList->getContentSize().height);
	m_customScrollBarHero->setPosition(m_ui.pHeroList->getPosition() + Point(-12, 0));
	pHeroListDi->addChild(m_customScrollBarHero);
	m_customScrollBarHero->setScrollBarLengthPercent(1.0f);
	m_customScrollBarHero->setScrollBarPercentY(0.0f);
	m_ui.pHeroList->addEventListener(CC_CALLBACK_2(LoveLayer::onScrollViewEvent, this));
	m_ui.pHeroLockCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlMain,"Lab_unlock"));
	m_ui.pHeroLockCount->setFontName(FONT_FZZHENGHEI);
	m_ui.pHeroGuessCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlMain,"Lab_count1"));
	m_ui.pHeroGuessCount->setFontName(FONT_FZZHENGHEI);
	m_ui.pHeroPlayCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlMain,"Lab_count2"));
	m_ui.pHeroPlayCount->setFontName(FONT_FZZHENGHEI);
	m_ui.pHeroGuessCount->setVisible(false);
	m_ui.pHeroPlayCount->setVisible(false);

	//英雄界面
	m_ui.pPnlHero = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_hero"));
	m_ui.pImgHero = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlHero,"Img_hero"));  
	m_ui.pImgHeroPos = m_ui.pImgHero->getPosition();

	m_ui.pBtnGift = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pPnlHero,"Btn_gift")); 
	m_ui.pBtnGiftPos = m_ui.pBtnGift->getPosition();
	m_ui.pBtnPlay = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pPnlHero,"Btn_play"));  
	m_ui.pBtnPlayPos = m_ui.pBtnPlay->getPosition();
	m_ui.pBtnDrink = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pPnlHero,"Btn_drink"));  
	m_ui.pBtnDrinkPos = m_ui.pBtnDrink->getPosition();
	m_ui.pPnlFavour = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlHero,"Pnl_favour"));
	m_ui.pPnlHeroPro = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlHero,"Pnl_shuxing"));
	m_ui.pPnlHeroProPos = m_ui.pPnlHeroPro->getPosition();
	m_ui.pPnlHeroATK = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Lab_ATK"));
	Text *pPnlHeroHP = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Lab_HP"));
	Text *pPnlHeroDEF1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Lab_DEF1"));
	Text *pPnlHeroDEF2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Lab_DEF2"));
	m_ui.pPnlHeroProAtk = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Lab_ATKAdd"));
	m_ui.pPnlHeroProBloodUp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Lab_HPAdd"));
	m_ui.pPnlHeroProPhysicsDef = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Lab_DEF1Add"));
	m_ui.pPnlHeroProSpellDef = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Lab_DEF2Add"));
	m_ui.pPnlHeroProFriendShip = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Lab_friendship"));
	m_ui.pPnlHeroATK->setFontName(FONT_FZZHENGHEI);
	pPnlHeroHP->setFontName(FONT_FZZHENGHEI);
	pPnlHeroDEF1->setFontName(FONT_FZZHENGHEI);
	pPnlHeroDEF2->setFontName(FONT_FZZHENGHEI);
	m_ui.pPnlHeroProAtk->setFontName(FONT_FZZHENGHEI);
	m_ui.pPnlHeroProBloodUp->setFontName(FONT_FZZHENGHEI);
	m_ui.pPnlHeroProPhysicsDef->setFontName(FONT_FZZHENGHEI);
	m_ui.pPnlHeroProSpellDef->setFontName(FONT_FZZHENGHEI);
	m_ui.pPnlHeroProFriendShip->setFontName(FONT_FZZHENGHEI);

	m_ui.pPnlHeroProAdd = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Btn_Add"));
	m_ui.pPnlHeroProAdd->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify,this));
	m_ui.pPnlHeroProTips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlHeroPro,"Img_Tips"));


	//游玩界面
	m_ui.pPnlplay = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_play"));
	m_ui.pImgMapDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlplay,"Img_di"));
	m_ui.Panel_mapInfo = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlplay,"Panel_mapInfo"));
	m_ui.Panel_mapInfo->setVisible(false);
	m_ui.imgMapDiPos = m_ui.pImgMapDi->getPosition();
	for (int i = 0; i < CHOOSE_MAP_COUNT; i++)
	{
		m_ui.imgChooseMap[i] = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pImgMapDi,"map" + TO_STR(i+1)));
	}

	
	//送礼界面
	m_ui.pPnlGift = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_gift"));
	m_ui.giftListView = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlGift,"Img_bg"));
	m_ui.giftListPos = m_ui.giftListView->getPosition();
	m_ui.giftList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.giftListView,"ListView_23"));
	ImageView *giftListdI = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.giftListView,"Img_di"));
	m_customScrollBarGift = CustomScrollBar::create();
	m_customScrollBarGift->initScrollBar(m_ui.giftList->getContentSize().height);
	m_customScrollBarGift->setPosition(m_ui.giftList->getPosition() + Point(-12, 0));
	giftListdI->addChild(m_customScrollBarGift);
	m_customScrollBarGift->setScrollBarLengthPercent(1.0f);
	m_customScrollBarGift->setScrollBarPercentY(0.0f);
	dynamic_cast<ScrollView*>(m_ui.giftList)->addEventListener(CC_CALLBACK_2(LoveLayer::onScrollViewEvent, this));
	Text *Lab_tips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.giftListView,"Lab_tips"));
	Lab_tips->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Lab_tips,Color3B(0x00,0x00,0x00),2);

	//猜迷界面
	m_ui.pPnlGuess = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_play"));

	//猜拳界面
	m_ui.pPnlDrink = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_drink"));
	m_ui.pPnlDrinkInfo = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlDrink,"Img_di1"));
	m_ui.pPnlDrinkInfoPrize = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlDrink,"Img_di2Prize"));
	m_ui.pPnlDrinkInfoPrizePneal =  dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlDrink,"Panel_5prize"));
	m_ui.pPnlDrinkInfoPrizePneal->setVisible(false);
	auto callpPnlDrinkInfoPrizePneal = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			m_ui.pPnlDrinkInfoPrizePneal->setVisible(false);
		}
	};
	m_ui.pPnlDrinkInfoPrizePneal->addTouchEventListener(callpPnlDrinkInfoPrizePneal);
	m_ui.pnlTime = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfo,"Pnl_time"));        //猜拳次数
	m_ui.fingerCountDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pnlTime,"Lab_tips")); 
	m_ui.fingerCountDesc->setFontName(FONT_FZZHENGHEI);
	m_ui.fingerRCountDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pnlTime,"Lab_Times"));
	m_ui.fingerRCountDesc->setFontName(FONT_FZZHENGHEI);
	m_ui.DrinkHero = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlDrink,"Img_hero"));      //本英雄
	m_ui.DrinkHeroPos = m_ui.DrinkHero->getPosition();
	m_ui.DrinkMin = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlDrink,"Img_lead"));     //敌方英雄
	m_ui.DrinkMinPos = m_ui.DrinkMin->getPosition();
	m_ui.PnlDrinkReward = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfo,"Pnl_reward")); //结算
	for (int i = 0; i < DRINK_PRIZE_COUNT; i++)
	{
		m_ui.BtnDrinkReward[i] = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.PnlDrinkReward,"Btn_win" + TO_STR(i+1)));
		m_ui.BtnDrinkReward[i]->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onViewGuessPrize,this,i));
		m_ui.TextDrinkReward[i] = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlDrinkReward,"Lab_win" + TO_STR(i+1)));
		m_ui.TextDrinkReward[i]->setFontName(FONT_FZZHENGHEI);
	}
	

	m_ui.pnlDrinkBegin = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfo,"Pnl_began"));  //猜拳

	m_ui.BtnfingerStone = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pnlDrinkBegin,"Btn_quantou"));
	m_ui.BtnfingerScissors = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pnlDrinkBegin,"Btn_jiandao"));
	m_ui.BtnfingerCloth = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pnlDrinkBegin,"Btn_bu"));
	m_ui.BtnfingerStone->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify,this));      
	m_ui.BtnfingerScissors->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify,this));   
	m_ui.BtnfingerCloth->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify,this));      


	m_ui.pnlFinger = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfo,"Pnl_finger"));        //拳头
	m_ui.fingerHero = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pnlFinger,"Img_go1"));
	m_ui.fingerMin = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pnlFinger,"Img_go2"));
	m_ui.addGuessBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfo,"addGuessBtn"));
	m_ui.ImgResult = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfo,"Img_result"));     //输赢
	m_ui.addGuessBtn->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify,this));      
	m_ui.changeMing = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfo,"changeMing"));
	m_ui.changeMingAgain = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.changeMing,"Btn_again"));
	m_ui.changeMingWin = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.changeMing,"Btn_win"));
	m_ui.changeMingCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.changeMing,"Lab_cost"));
	m_ui.changeMingCost->setFontName(FONT_FZZHENGHEI);
	m_ui.changeMingAgain->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify,this));
	m_ui.changeMingWin->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify,this));

	m_ui.pPnlDuiDai->setVisible(true);
	m_ui.pPnlMain->setVisible(true);
	m_ui.pPnlHero->setVisible(false);
	m_ui.pPnlplay->setVisible(false);
	m_ui.pPnlGift->setVisible(false);
	m_ui.pPnlGuess->setVisible(false);
	m_ui.pPnlDrink->setVisible(false);
	m_ui.pImgDuiDaiNpc->setVisible(false);
	m_ui.pImgDuiDaiHero->setVisible(false); 

	m_state = LOVE_LAYER_STATE_ENTER;
	switchState();
}

void LoveLayer::initVipUI_1()
{
	m_ui.pVipStateRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UiNewbuy/UIVip_State.ExportJson"));
	this->addChild(m_ui.pVipStateRoot, 3);
	m_ui.pVipStateRoot->setVisible(false);
	m_ui.imgVipBg1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Img_Bg"));
	m_ui.btnVipStateOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_OK"));
	m_ui.btnVipStateCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_Cancel"));
	m_ui.btnVipStateClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_Close"));

	m_ui.txtVipStateTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi1"));
	m_ui.txtVipStateTips1->setFontName(FONT_FZZHENGHEI);
	string str(DataManager::getInstance()->searchCommonTexdtById(20284));
	m_ui.txtVipStateTips1->setString((str));
	m_ui.txtVipStateTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi2"));
	m_ui.txtVipStateTips2->setFontName(FONT_FZZHENGHEI);
	str.clear();
	str.assign(DataManager::getInstance()->searchCommonTexdtById(20285));
	m_ui.txtVipStateTips2->setString((str));
	m_ui.txtVipStateTips3 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi4"));
	m_ui.txtVipStateTips3->setFontName(FONT_FZZHENGHEI);
	str.clear();
	str.assign(DataManager::getInstance()->searchCommonTexdtById(20286));
	m_ui.txtVipStateTips3->setString((str));

	// 购买消耗;
	m_ui.imgCurVip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Img_Vip"));
	m_ui.imgCurVip->loadTexture(getVipIcon(VipModel::getInstance()->getCurVipLv()));
	m_ui.txtCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Lab_Cost"));
	m_ui.txtCost->setFontName(FONT_FZZHENGHEI);

	// 剩余次数;
	m_ui.txtCurCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Lab_Left_Times"));
	m_ui.txtCurCount->setFontName(FONT_FZZHENGHEI);

	m_ui.btnVipStateOK->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify, this));
	m_ui.btnVipStateCancel->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify, this));
	m_ui.btnVipStateClose->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify, this));
}

std::string LoveLayer::getVipIcon(int nVipLv)
{
	string strPath = "Image/UIVip/VipLv/img_vip_";
	strPath.append(_TO_STR(nVipLv));
	strPath.append(".png");
	return strPath;
}

int LoveLayer::getNextValidVip(int& nNextValue)
{
	// 当前VIP属性;
	int nCurVipLv = VipModel::getInstance()->getCurVipLv();
	RowData* vipInfo = DataManager::getInstance()->searchVipDescriptionById(nCurVipLv);
	int buyDrinkNum = vipInfo->getIntData("buyDrinkNum");

	DataTable* _dt = DataManager::getInstance()->getDTVip();
	int nLoopLv = nCurVipLv;
	RowData* _rowNext = _dt->searchDataById(++nLoopLv);
	while (_rowNext != nullptr)
	{
		// 下一级的值;
		nNextValue = _rowNext->getIntData("buyDrinkNum");
		if (nNextValue > buyDrinkNum)
		{
			return nLoopLv;
			break;
		}
		else
		{
			_rowNext = _dt->searchDataById(++nLoopLv);
		}
	}
	
	return --nLoopLv;
}

void LoveLayer::initVipUI_2()
{
	m_ui.pVipRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UiNewbuy/UiVipUp.ExportJson"));
	this->addChild(m_ui.pVipRoot, 3);
	m_ui.pVipRoot->setVisible(false);
	m_ui.btnVipOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_OK)));
	m_ui.btnVipCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_Cancel)));
	m_ui.btnVipClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_Close)));

	m_ui.imgVipBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Img_di"));
	m_ui.txtVipTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Tips1"));
	m_ui.txtVipTips1->setFontName(FONT_FZZHENGHEI);
	string str(DataManager::getInstance()->searchCommonTexdtById(20287));
	m_ui.txtVipTips1->setString((str));
	m_ui.txtVipTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Tips2"));
	m_ui.txtVipTips2->setFontName(FONT_FZZHENGHEI);
	m_ui.imgVipNext = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Img_Vip"));
	m_ui.txtNextCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Count"));
	m_ui.txtNextCount->setFontName(FONT_FZZHENGHEI);

	m_ui.btnVipOK->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify, this));
	m_ui.btnVipCancel->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify, this));
	m_ui.btnVipClose->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify, this));
}

void LoveLayer::initHero()
{
	m_ui.pBtnGift->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify,this));       //送礼
	m_ui.pBtnPlay->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify,this));       //游玩
	m_ui.pBtnDrink->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnNotify,this));      //喝酒

	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
	RowData *heroData = DataManager::getInstance()->searchToolById(heroInfo->petId);
	m_ui.pImgHero->loadTexture(ResourceUtils::getHeroCompleteCardPath(heroData->getIntData("resId")));
	RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(heroInfo->petId);
	vector<float> pointVect;
	StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
	m_ui.pImgHero->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));

	Size size = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	m_ui.pBtnGift->setPosition(m_ui.pImgHeroPos);
	m_ui.pBtnGift->setVisible(false);
	m_ui.pBtnPlay->setPosition(m_ui.pImgHeroPos);
	m_ui.pBtnPlay->setVisible(false);
	m_ui.pBtnDrink->setPosition(m_ui.pImgHeroPos);
	m_ui.pBtnDrink->setVisible(false);
	m_ui.pImgHero->setVisible(false);
	m_ui.pPnlFavour->setVisible(false);

	//英雄属性信息
	m_ui.pPnlHeroPro->setPosition(Vec2(size.width+origin.x,m_ui.pPnlHeroProPos.y));  


	setHeroUpdtaInfo();
}

void LoveLayer::initHeroAttribute()
{
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);

	RowData *petData = DataManager::getInstance()->searchCharacterById(m_chooseHeroPetId);
	int atkType = petData->getIntData("atkTyp");
	if (atkType == 1)
	{
		m_ui.pPnlHeroATK->setString(DataManager::getInstance()->searchCommonTexdtById(20156));
	}
	else if (atkType == 2)
	{
		m_ui.pPnlHeroATK->setString(DataManager::getInstance()->searchCommonTexdtById(20289));
	}

	if (heroInfo->expType > HERO_GOOD_LV_STRANGE)
	{
		m_ui.pPnlHeroProTips->setVisible(false);
		m_ui.pPnlHeroProFriendShip->setVisible(true);
		m_ui.pPnlHeroProFriendShip->setString(TO_STR(heroInfo->friendShip));
	}
	else
	{
		m_ui.pPnlHeroProTips->setVisible(true);
		m_ui.pPnlHeroProFriendShip->setVisible(false);
	}

	//攻击
	RowData *atkData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeAtk);
	if (atkData)
	{
		m_ui.pPnlHeroProAtk->setString("+" + TO_STR(atkData->getIntData("atk"))); //物理攻击
	}

	//血量
	RowData *hpData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeHp);
	if (hpData)
	{
		m_ui.pPnlHeroProBloodUp->setString("+" + TO_STR(hpData->getIntData("hp"))); //物理攻击
	}

	//物防
	RowData *pddData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typePdd);
	if (pddData)
	{
		m_ui.pPnlHeroProPhysicsDef->setString("+" + TO_STR(pddData->getFloatData("pddCoe")*100.0f) + "%"); //物理攻击
	}

	//法防
	RowData *mddData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeMdd);
	if (mddData)
	{
		m_ui.pPnlHeroProSpellDef->setString("+" + TO_STR(mddData->getFloatData("mddCoe")*100.0f) + "%"); //物理攻击
	}
}

void LoveLayer::initGiftList()
{
	Size size = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	m_ui.giftListView->setPosition(Vec2(size.width + origin.x + m_ui.giftListView->getContentSize().width/2,m_ui.giftListView->getPosition().y));
	m_ui.pImgHero->runAction(CCMoveBy::create(enterGift_time/2,Vec2(-125,0)));

	addGiftList();
}

void LoveLayer::initDrink()
{
	Size size = Director::getInstance()->getVisibleSize();
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
	RowData *heroData = DataManager::getInstance()->searchToolById(heroInfo->petId);
	m_ui.DrinkHero->loadTexture(ResourceUtils::getHeroCompleteCardPath(heroData->getIntData("resId")));
	m_ui.DrinkHero->setPosition(Vec2(-m_ui.DrinkHero->getContentSize().width/2,m_ui.DrinkHeroPos.y));
	RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(heroInfo->petId);
	vector<float> pointVect;
	StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
	m_ui.pImgHero->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));

	//这里分性别 玩家玩的一张 女的一张
	if (HeroModel::getInstance()->getMainHero()->mSex == HERO_SEX_MALE)
	{
		m_ui.DrinkMin->loadTexture("Image/Icon/integrityCard/41099.png");	
	}
	else
	{
		m_ui.DrinkMin->loadTexture("Image/Icon/integrityCard/41098.png");
	}
	m_ui.DrinkMin->setPosition(Vec2(size.width+m_ui.DrinkMin->getContentSize().width/2,m_ui.DrinkMinPos.y));

	setFingerGuseePanelInfo();

	m_ui.pnlFinger->setVisible(false);
	m_ui.ImgResult->setVisible(false);
	m_ui.changeMing->setVisible(false);
	m_ui.pPnlDrinkInfoPrizePneal->setVisible(false);
	m_ui.pnlDrinkBegin->setVisible(true);
	m_ui.PnlDrinkReward->setVisible(true);
	m_ui.pnlDrinkBegin->setOpacity(255);
}

void LoveLayer::intiPlay(bool move)
{
	if (move)
	{
		Size size = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();
		m_ui.pImgMapDi->setPosition(Vec2(size.width + origin.x + m_ui.pImgMapDi->getContentSize().width/2,m_ui.pImgMapDi->getPosition().y));
		m_ui.pImgHero->setPosition(m_ui.pImgHeroPos);
		m_ui.pImgHero->runAction(CCSpawn::create(CCMoveBy::create(enterGift_time/2,Vec2(-400,-15)),CCScaleTo::create(enterGift_time/2,0.7f),nullptr));
	}
	
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
	PLAY_MAP_VEC* playMap = LoveModel::getInstance()->getPlayMap();
	int timeIdx = 0;
	for (int i = 0; i < CHOOSE_MAP_COUNT; i++)
	{
		ImageView *success = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.imgChooseMap[i],"Img_Result"));
		success->setVisible(false);

		switch (playMap->at(i).flg)
		{
		case PLAY_MAP_TYPE_RESULT_IMCOMPLETE:
			break;
		case PLAY_MAP_TYPE_RESULT_COMPLETE:
			{
				success->loadTexture("Image/Icon/global/img_tanbao_chenggong.png");
			}
			break;
		case PLAY_MAP_TYPE_RESULT_FAIL:
			{
				success->loadTexture("Image/Icon/global/img_tanbao_shibai.png");
			}
			break;
		default:
			break;
		}

		//砸
		if (move && playMap->at(i).flg != PLAY_MAP_TYPE_RESULT_IMCOMPLETE)
		{
			success->setVisible(true);
			success->setScale(0.6f);
			success->runAction(CCSequence::create(CCDelayTime::create(0.8f+timeIdx*0.2),CCScaleTo::create(0.2f,0.4f),nullptr));
			timeIdx++;
		}
		else if(!move&& playMap->at(i).flg != PLAY_MAP_TYPE_RESULT_IMCOMPLETE && m_playMapIdx != i)
		{
			success->setVisible(true);
		}
		else if( !move&& playMap->at(i).flg != PLAY_MAP_TYPE_RESULT_IMCOMPLETE && m_playMapIdx == i)
		{
			success->setVisible(true);
			success->setScale(0.6f);
			success->runAction(CCSequence::create(CCDelayTime::create(0.8f),CCScaleTo::create(0.2f,0.4f),nullptr));
		}

		RowData *lovePlay = DataManager::getInstance()->searchLoveTreasureByid(playMap->at(i).id);
		RowData *toolDate = DataManager::getInstance()->searchToolById(lovePlay->getIntData("treasureId"));

		Text *name = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.imgChooseMap[i],"Lab_Name"));
		name->setFontName(FONT_FZZHENGHEI);
		name->setString(toolDate->getStringData("itemName"));
		setTextAddStroke(name,Color3B(0x00,0x00,0x00),2);

		ImageView *head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.imgChooseMap[i],"Img_Item"));
		head->loadTexture(ResourceUtils::getSmallIconPath(toolDate->getIntData("resId")));

		ImageView *Img_Difficult1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.imgChooseMap[i],"Img_Difficult1"));
		ImageView *Img_Difficult2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.imgChooseMap[i],"Img_Difficult2"));
		ImageView *Img_Difficult3 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.imgChooseMap[i],"Img_Difficult3"));
		ImageView *Img_Difficult4 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.imgChooseMap[i],"Img_Difficult4"));
		ImageView *Img_Difficult5 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.imgChooseMap[i],"Img_Difficult5"));

		Img_Difficult1->setVisible(false);
		Img_Difficult2->setVisible(false);
		Img_Difficult3->setVisible(false);
		Img_Difficult4->setVisible(false);
		Img_Difficult5->setVisible(false);
		switch (lovePlay->getIntData("difficult"))
		{
		case 1:
			Img_Difficult1->setVisible(true);
			break;
		case 2:
			Img_Difficult2->setVisible(true);
			break;
		case 3:
			Img_Difficult3->setVisible(true);
			break;
		case 4:
			Img_Difficult4->setVisible(true);
			break;
		case 5:
			Img_Difficult5->setVisible(true);
			break;
		default:
			break;
		}

		m_ui.imgChooseMap[i]->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onChoosePlayInfo,this,heroInfo->petId,i));
	}
}

void LoveLayer::setHeroUpdtaInfo()
{
	//英雄属性信息
	initHeroAttribute();

	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
	RowData *heroData = DataManager::getInstance()->searchToolById(heroInfo->petId);
	m_ui.pTextHeroName->setString(heroData->getStringData("itemName"));
	m_npcTalkCount = NPC_TALK_COUNT;
	m_ui.pTextDuiBaiContext->setString("");

	//设置游玩提示
	ImageView *imgMarkPlay = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pBtnPlay,"Img_mark"));
	ImageView *imgMarkDrink = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pBtnDrink,"Img_mark"));
	if (heroInfo->expType > HERO_GOOD_LV_STRANGE)
	{
		m_ui.pBtnGift->setBright(true);
		m_ui.pBtnPlay->setBright(true);
		m_ui.pBtnDrink->setBright(true);

		imgMarkPlay->setVisible(true);
		imgMarkDrink->setVisible(true);

		//情缘游玩开启条件
		RowData* rowDataPlay = DataManager::getInstance()->searchToolSystemById(FUNCTION_LOVE_PLAY);
		if (rowDataPlay)
		{
			int functionLevel = rowDataPlay->getIntData("value");
			if (heroInfo->curLevel >= functionLevel)
			{
				int playAllCount = LoveModel::getInstance()->getAllPlayCount();
				if (heroInfo->playCount <= 0 || playAllCount <= 0) //公用游玩次数大于0 且没有玩过
				{
					imgMarkPlay->setVisible(false);
				}
			}
			else
			{
				m_ui.pBtnPlay->setBright(false);
				imgMarkPlay->setVisible(false);
			}
		}

		//情缘喝酒开启条件
		RowData* rowDataDrink = DataManager::getInstance()->searchToolSystemById(FUNCTION_LOVE_DRINK);
		if (rowDataDrink)
		{
			int functionLevel = rowDataDrink->getIntData("value");
			if (heroInfo->curLevel >= functionLevel)
			{
				int guessAllCount = LoveModel::getInstance()->getGuessCount();
				if (heroInfo->guessCount <= 0 || (!heroInfo->isGuess && guessAllCount <= 0)) //公用c猜拳次数大于0 且没有玩过
				{
					imgMarkDrink->setVisible(false);
				}
			}
			else
			{
				m_ui.pBtnDrink->setBright(false);
				imgMarkDrink->setVisible(false);
			}
		}
	}
	else
	{
		m_ui.pBtnGift->setBright(false);
		m_ui.pBtnPlay->setBright(false);
		m_ui.pBtnDrink->setBright(false);
		imgMarkPlay->setVisible(false);
		imgMarkDrink->setVisible(false);
	}

	setHerofavour();
}

void LoveLayer::setHerofavour()
{
	LOVE_HERO_INFO_VEC* heroVec = LoveModel::getInstance()->getLoveHeroInfoVec();
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);

	Text* Lablevel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlFavour,"Lab_level"));
	Lablevel->setFontName(FONT_FZZHENGHEI);
	ImageView* imgMax = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlFavour,"imgMax"));
	imgMax->setVisible(false);
	TextAtlas *textCur = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(m_ui.pPnlFavour,"AtlasLabel_24"));
	ImageView *gang = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlFavour,"Img_gang"));
	TextAtlas *textMax = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(m_ui.pPnlFavour,"AtlasLabel_24_0"));
	LoadingBar *bar = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(m_ui.pPnlFavour,"ProgressBar_27"));

	RowData *loveLevelData = DataManager::getInstance()->searchLoveLevelById(heroInfo->curLevel);
	ImageView *Img_relation = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlFavour,"Img_relation"));
	ImageView *Img_relation_Mid = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlFavour,"Img_relation_0"));
	if (loveLevelData)
	{
		Img_relation->setVisible(true);
		Img_relation_Mid->setVisible(false);

		Img_relation->loadTexture("Image/Icon/global/img_haogan2_" + TO_STR(loveLevelData->getIntData("loveType")) + ".png");
		if (loveLevelData->getIntData("exp") <= 0)//最高等级
		{
			textCur->setVisible(false);
			gang->setVisible(false);
			textMax->setVisible(false);
			bar->setPercent(100);

			Lablevel->setVisible(true);
			Lablevel->setString(DataManager::getInstance()->searchCommonTexdtById(20290));
			setTextAddStroke(Lablevel,Color3B(0x8e,0x00,0x03),2);
			imgMax->setVisible(true);
		}
		else
		{
			textCur->setVisible(true);
			gang->setVisible(true);
			textMax->setVisible(true);
			textCur->setString(TO_STR(heroInfo->goodWill));
			textMax->setString(TO_STR(loveLevelData->getIntData("exp")));
			bar->setPercent(heroInfo->goodWill*100/loveLevelData->getIntData("exp"));

			Lablevel->setVisible(true);
			Lablevel->setString("LV" + TO_STR(loveLevelData->getIntData("level")));
			setTextAddStroke(Lablevel,Color3B(0x8e,0x00,0x03),2);
		}
	}
	else
	{
		Img_relation->setVisible(false);
		Img_relation_Mid->setVisible(true);
		Img_relation_Mid->loadTexture("Image/Icon/global/img_haogan2_" + TO_STR(0) + ".png");
		Lablevel->setVisible(false);

		textCur->setVisible(true);
		gang->setVisible(true);
		textMax->setVisible(true);
		textCur->setString(TO_STR(0));

		RowData *_loveLevelData = DataManager::getInstance()->searchLoveLevelById(1);
		textMax->setString(TO_STR(_loveLevelData->getIntData("exp")));
		bar->setPercent(heroInfo->goodWill*100/_loveLevelData->getIntData("exp"));
	}
}

void LoveLayer::setAddlove()
{
	Size size = Director::getInstance()->getVisibleSize();
	string strFileName;
	strFileName.assign("Image/UIqingyuan/UIqingyuanAdd.ExportJson");
	Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

	ImageView *Img_addlove = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_addlove_0"));
	Text *Lab_value = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_addlove,"Lab_value"));
	vector<int>* addWillVec =  LoveModel::getInstance()->getGoodWillAddVec();
	//说话
	if (addWillVec->at(0)/m_giftSendCount == m_BadGiftAdd)
	{
		setHeroRandSay("giftAnswer3");
	}
	else if (addWillVec->at(0)/m_giftSendCount == m_OrdinaryGiftAdd)
	{
		setHeroRandSay("giftAnswer2");
	}
	else if (addWillVec->at(0)/m_giftSendCount == m_GooddGiftAdd)
	{
		setHeroRandSay("giftAnswer1");
	}

	Lab_value->setString("+" + TO_STR(addWillVec->at(0)));
	Lab_value->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Lab_value,Color3B(0x10,0x3b,0x19),2);
	addWillVec->erase(addWillVec->begin());
	m_ui.pPnlGift->addChild(layer);

	auto call = [layer]()->void
	{
		layer->removeFromParent();
	};

	Img_addlove->runAction(CCSequence::create(MoveBy::create(0.5f,Vec2(0,150.0f)),CCFadeOut::create(0.2f),CCCallFunc::create(call),nullptr));
}

void LoveLayer::setFingerAddlove()
{
	vector<int>* addWillVec =  LoveModel::getInstance()->getFingerGoodWillAddVec();
	if (addWillVec->size() <= 0) //满级有可能没有好感度
	{
		return;
	}

	string strFileName;
	strFileName.assign("Image/UIqingyuan/UIqingyuanAdd.ExportJson");
	Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

	ImageView *Img_addlove = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_addlove_0"));
	Text *Lab_value = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_addlove,"Lab_value"));

	Lab_value->setString("+" + TO_STR(addWillVec->at(0)));
	Lab_value->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Lab_value,Color3B(0x10,0x3b,0x19),2);
	addWillVec->erase(addWillVec->begin());
	m_ui.pPnlDrink->addChild(layer);

	auto call = [layer]()->void
	{
		layer->removeFromParent();
	};

	Img_addlove->runAction(CCSequence::create(MoveBy::create(0.5f,Vec2(0,150.0f)),CCFadeOut::create(0.2f),CCCallFunc::create(call),nullptr));
}

void LoveLayer::setUpGradePrize()
{
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
	SEND_GIFT_REWARD_VEC* rewardVec = LoveModel::getInstance()->getLoveGiftReeardVec();
	
	//升级数据改变
	if (heroInfo->curLevel > heroInfo->lastLevel)
	{
		auto call = [this]()->void
		{
			
		};
		//升级会有友情点奖励
		RowData *friend_ship = DataManager::getInstance()->searchToolSystemById(FIREND_SHIP_ID);
		int friend_ship_count = 0;
		for (int i = heroInfo->lastLevel; i < heroInfo->curLevel; i++)
		{
			RowData *loveLevelData = DataManager::getInstance()->searchLoveLevelById(i);
			if (loveLevelData && loveLevelData->getIntData("friendShip") > 0)
			{
				friend_ship_count = friend_ship_count + loveLevelData->getIntData("friendShip");
			}
		}

		if (friend_ship)
		{
			SEND_GIFT_REWARD reward;
			reward.templateId = friend_ship->getIntData("value");
			reward.amount = friend_ship_count;
			rewardVec->push_back(reward);
		}

		LoveGoodWillPrize *Prize = LoveGoodWillPrize::create(*heroInfo,rewardVec,call);
		addChild(Prize,1);

		LoveController::getHeroListInfo();
		initHeroAttribute();
	}
}

void LoveLayer::showPopReward()
{
	SEND_GIFT_REWARD_VEC* extraReward = LoveModel::getInstance()->getExtraReward();
	vector<PopRewardItem*> vecRewardItem;

	if (extraReward->size() > 0)
	{
		//道具
		for (size_t i = 0; i < extraReward->size(); i++)
		{
			PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(extraReward->at(i).templateId, extraReward->at(i).amount);
			vecRewardItem.push_back(rewardItem);
		}
		extraReward->clear();
		auto call = [this](Ref* ref, PopRewardEventType type)->void
		{
			if (type == EVENT_TYPE_END)
			{
				setUpGradePrize();
			}
		};

		PopReward::show(vecRewardItem,call);

	}
	else
	{
		setUpGradePrize();
	}
}

void LoveLayer::setFingerGuseeInfo(FINGER_ENTER_TYPE_RESULT result)
{
	switch (result)
	{
	case FINGER_ENTER_TYPE_RESULT_WIN: //赢
		{
			setFingerFailWinTips(FINGER_ENTER_TYPE_RESULT_WIN);
			showPopReward();

			setFingerGuseePanelInfo();
			setHeroRandFingerSay("winAnswer");
			

			switch (m_MyfingetGuseeType)
			{
			case FINGER_ENTER_TYPE_STONE:
				{
					m_ui.fingerHero->loadTexture("Image/Icon/global/img_jiandao.png");
					m_ui.fingerMin->loadTexture("Image/Icon/global/img_shitou.png");
				}
				break;
			case FINGER_ENTER_TYPE_SCISS:
				{
					m_ui.fingerHero->loadTexture("Image/Icon/global/img_bu.png");
					m_ui.fingerMin->loadTexture("Image/Icon/global/img_jiandao.png");
				}
				break;
			case FINGER_ENTER_TYPE_CLOTH:
				{
					m_ui.fingerHero->loadTexture("Image/Icon/global/img_shitou.png");
					m_ui.fingerMin->loadTexture("Image/Icon/global/img_bu.png");
				}
				break;
			default:
				break;
			}
		}
		break;
	case FINGER_ENTER_TYPE_RESULT_FAILD: //输
		{
			setFingerFailWinTips(FINGER_ENTER_TYPE_RESULT_FAILD);
			setFingerGuseePanelInfo();
			setHeroRandFingerSay("loseAnswer");

			switch (m_MyfingetGuseeType)
			{
			case FINGER_ENTER_TYPE_STONE:
				{
					m_ui.fingerHero->loadTexture("Image/Icon/global/img_bu.png");
					m_ui.fingerMin->loadTexture("Image/Icon/global/img_shitou.png");
				}
				break;
			case FINGER_ENTER_TYPE_SCISS:
				{
					m_ui.fingerHero->loadTexture("Image/Icon/global/img_shitou.png");
					m_ui.fingerMin->loadTexture("Image/Icon/global/img_jiandao.png");
				}
				break;
			case FINGER_ENTER_TYPE_CLOTH:
				{
					m_ui.fingerHero->loadTexture("Image/Icon/global/img_jiandao.png");
					m_ui.fingerMin->loadTexture("Image/Icon/global/img_bu.png");
				}
				break;
			default:
				break;
			}
		}
		break;
	case FINGER_ENTER_TYPE_RESULT_PIN://平
		{
			setFingerFailWinTips(FINGER_ENTER_TYPE_RESULT_PIN);
			setFingerGuseePanelInfo();
			setHeroRandFingerSay("dogFallAnswer");

			switch (m_MyfingetGuseeType)
			{
			case FINGER_ENTER_TYPE_STONE:
				{
					m_ui.fingerHero->loadTexture("Image/Icon/global/img_shitou.png");
					m_ui.fingerMin->loadTexture("Image/Icon/global/img_shitou.png");
				}
				break;
			case FINGER_ENTER_TYPE_SCISS:
				{
					m_ui.fingerHero->loadTexture("Image/Icon/global/img_jiandao.png");
					m_ui.fingerMin->loadTexture("Image/Icon/global/img_jiandao.png");
				}
				break;
			case FINGER_ENTER_TYPE_CLOTH:
				{
					m_ui.fingerHero->loadTexture("Image/Icon/global/img_bu.png");
					m_ui.fingerMin->loadTexture("Image/Icon/global/img_bu.png");
				}
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}
}

void LoveLayer::setFingerGuseePanelInfo()
{
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);

	RowData *loveFinger = DataManager::getInstance()->searchLoveFingerById(heroInfo->petId);
	vector<int> vecWill;
	StringFormat::parseCsvStringVec(loveFinger->getStringData("win"),vecWill);
	for (int i = 0; i < DRINK_PRIZE_COUNT; i++)
	{
		m_ui.TextDrinkReward[i]->setString(TO_STR(vecWill[i]) + DataManager::getInstance()->searchCommonTexdtById(20291));
		setTextAddStroke(m_ui.TextDrinkReward[i],Color3B(0x7d,0x19,0x0e),2);

		vector<int>::iterator result = find( heroInfo->guessReward.begin( ), heroInfo->guessReward.end( ), i); //查找3
		if ( result == heroInfo->guessReward.end()) //没找到 能领取
		{
			m_ui.BtnDrinkReward[i]->setBright(true);
			if ( heroInfo->guessWinCount >= vecWill[i] )
			{
				m_ui.BtnDrinkReward[i]->runAction(CCRepeatForever::create(CCSequence::create(CCScaleTo::create(0.1f,1.1f),
					CCDelayTime::create(0.2f),CCScaleTo::create(0.3f,1.0f),nullptr)));
			}
			else
			{
				m_ui.BtnDrinkReward[i]->stopAllActions();
			}
		}
		else
		{
			m_ui.BtnDrinkReward[i]->stopAllActions();
			m_ui.BtnDrinkReward[i]->setBright(false);
		}
	}
	

	m_ui.fingerCountDesc->setString(DataManager::getInstance()->searchCommonTexdtById(20292)
		+ TO_STR(heroInfo->guessWinCount) +
		DataManager::getInstance()->searchCommonTexdtById(10049));
	setTextAddStroke(m_ui.fingerCountDesc,Color3B(0x7d,0x19,0x0e),2);

	m_ui.fingerRCountDesc->setString(STR_UTF8("(")+
		DataManager::getInstance()->searchCommonTexdtById(20067) + TO_STR(heroInfo->guessCount) + STR_UTF8(")"));
}

void LoveLayer::setFingerFailWinTips(FINGER_ENTER_TYPE_RESULT result)
{
	switch (result)
	{
	case FINGER_ENTER_TYPE_RESULT_WIN:
		{
			m_ui.ImgResult->setVisible(true);
			m_ui.ImgResult->setOpacity(0);
			m_ui.ImgResult->loadTexture("Image/Icon/global/img_shengli.png");
			m_ui.ImgResult->setScale(0.1f);

			auto call1 = [this]()->void
			{
				m_ui.pnlDrinkBegin->setVisible(true);
				m_ui.pnlDrinkBegin->setOpacity(255);
				m_ui.PnlDrinkReward->setVisible(true);
				m_ui.pnlFinger->setVisible(false);
			};

			auto call2 = [this]()->void
			{
				setFingerAddlove();
				m_canGuess = true;
			};

			m_ui.ImgResult->runAction(CCScaleTo::create(WIN_IMAGE_TIME,1.0f));
			m_ui.ImgResult->runAction(CCSequence::create(CCFadeIn::create(WIN_IMAGE_TIME),
				CCDelayTime::create(1.0f),CCCallFunc::create(call2),CCDelayTime::create(1.0f),
				CCFadeOut::create(WIN_IMAGE_TIME),CCCallFunc::create(call1),nullptr));
		}
		break;
	case FINGER_ENTER_TYPE_RESULT_FAILD:
		{
			m_ui.ImgResult->setVisible(true);
			m_ui.ImgResult->setOpacity(0);
			m_ui.ImgResult->loadTexture("Image/Icon/global/img_shibai.png");
			m_ui.ImgResult->setScale(0.1f);

			auto call = [this]()->void
			{
				m_ui.PnlDrinkReward->setVisible(false);
				m_ui.changeMing->setVisible(true);
				m_ui.changeMingCost->setString(TO_STR(LoveModel::getInstance()->getChangeGuessPrice()));
				m_canGuess = true;
			};
			m_ui.ImgResult->runAction(CCScaleTo::create(WIN_IMAGE_TIME,1.0f));
			m_ui.ImgResult->runAction(CCSequence::create(CCFadeIn::create(WIN_IMAGE_TIME),CCDelayTime::create(0.5f),CCCallFunc::create(call),nullptr));
		}
		break;
	case FINGER_ENTER_TYPE_RESULT_PIN:
		{
			m_ui.ImgResult->setVisible(true);
			m_ui.ImgResult->setOpacity(0);
			m_ui.ImgResult->loadTexture("Image/Icon/global/img_pingju.png");
			m_ui.ImgResult->setScale(0.1f);

			auto call = [this]()->void
			{
				m_ui.PnlDrinkReward->setVisible(false);
				m_ui.changeMing->setVisible(true);
				m_ui.changeMingCost->setString(TO_STR(LoveModel::getInstance()->getChangeGuessPrice()));
				m_canGuess = true;
			};
			m_ui.ImgResult->runAction(CCScaleTo::create(WIN_IMAGE_TIME,1.0f));
			m_ui.ImgResult->runAction(CCSequence::create(CCFadeIn::create(WIN_IMAGE_TIME),CCDelayTime::create(0.5f),CCCallFunc::create(call),nullptr));
		}
		break;
	default:
		break;
	}
}

void LoveLayer::switchState()
{
	switch (m_state)
	{
	case LOVE_LAYER_STATE_NONE:
		break;
	case LOVE_LAYER_STATE_ENTER:
		{
			m_ui.phelp->setVisible(true);
			m_ui.pPnlMain->setVisible(true);
			m_ui.pImgDuiDaiNpc->setVisible(true);
			m_ui.pImgDuiDaiHero->setVisible(false); 
			m_npcTalkCount = 0;
			showNpcTalk();

			m_ui.pPnlHero->setVisible(false);

			m_ui.pBtnGift->stopAllActions();
			m_ui.pBtnPlay->stopAllActions();
			m_ui.pBtnDrink->stopAllActions();
			m_ui.pImgHero->stopAllActions();
		}
		break;
	case LOVE_LAYER_STATE_HERO:
		{
			m_ui.phelp->setVisible(true);
			m_ui.phelp->setVisible(true);
			m_ui.pImgDuiDaiNpc->setVisible(false);
			m_ui.pImgDuiDaiHero->setVisible(true); 

			m_ui.pPnlHero->setVisible(true);
		}
		break;
	case LOVE_LAYER_STATE_PLAY:
		{
			m_ui.phelp->setVisible(false);

			m_ui.pImgDuiDaiNpc->setVisible(false);
			m_ui.pImgDuiDaiHero->setVisible(true); 

			m_ui.pPnlplay->setVisible(true);

			m_ui.pBtnGift->stopAllActions();
			m_ui.pBtnGift->setVisible(false);
			m_ui.pBtnPlay->stopAllActions();
			m_ui.pBtnPlay->setVisible(false);
			m_ui.pBtnDrink->stopAllActions();
			m_ui.pBtnDrink->setVisible(false);
			m_ui.pPnlFavour->setVisible(false);

			m_ui.pPnlHeroPro->stopAllActions();
			m_ui.pPnlHeroPro->setVisible(false);
		}
		break;
	case LOVE_LAYER_STATE_GIFT:
		{
			m_ui.phelp->setVisible(false);

			m_ui.pImgDuiDaiNpc->setVisible(false);
			m_ui.pImgDuiDaiHero->setVisible(true); 

			m_ui.pPnlGift->setVisible(true);

			m_ui.pBtnGift->stopAllActions();
			m_ui.pBtnGift->setVisible(false);
			m_ui.pBtnPlay->stopAllActions();
			m_ui.pBtnPlay->setVisible(false);
			m_ui.pBtnDrink->stopAllActions();
			m_ui.pBtnDrink->setVisible(false);

			m_ui.pPnlHeroPro->stopAllActions();
			m_ui.pPnlHeroPro->setVisible(false);
		}
		break;
	case LOVE_LAYER_STATE_GUESS:
		{
			m_ui.phelp->setVisible(false);

			m_ui.pImgDuiDaiNpc->setVisible(false);
			m_ui.pImgDuiDaiHero->setVisible(true); 
		}
		break;
	case LOVE_LAYER_STATE_DRINK:
		{
			m_ui.phelp->setVisible(false);

			m_ui.pImgDuiDaiNpc->setVisible(false);
			m_ui.pImgDuiDaiHero->setVisible(true); 

			m_ui.pPnlHero->setVisible(false);
			m_ui.pPnlDrink->setVisible(true);
			m_ui.pPnlDrinkInfo->setVisible(true);
		}
		break;
	default:
		break;
	}
}

void LoveLayer::onEnter()
{
	LoveModel::getInstance()->addObserver(this);
	Layer::onEnter();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(LoveLayer::updateNextFram), this, 0, false);
}

void LoveLayer::updateNextFram(float ft)
{
	LoveController::getHeroListInfo();
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(LoveLayer::updateNextFram), this);
}

void LoveLayer::onExit()
{
	LoveModel::getInstance()->removeObserver(this);
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIqingyuan");
	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(LoveLayer::callBackTalkOut), this);
	this->unschedule(schedule_selector(LoveLayer::onCallGiftSend));
	this->unschedule(schedule_selector(LoveLayer::onAddGiftSendCount));
	Layer::onExit();
}

void LoveLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
	case LOVE_EVENT_TYPE_GET_INFO:
		{
			//第一次添加情缘列表再显示新手引导
			if (m_isFirstAddHero)
			{
				GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_LOVE);
			}

			
			LOVE_GIFT_INFO_VEC* giftVec = LoveModel::getInstance()->getLoveGiftInfoVec();

			sort(giftVec->begin(),giftVec->end(),[this](LOVE_GIFT_INFO a, LOVE_GIFT_INFO b)->bool{
				int goodWillA = getGiftToHeroGoodWill(a.templateId);
				int goodWillB = getGiftToHeroGoodWill(b.templateId);

				if (goodWillA > goodWillB)
				{
					return true;
				}
				else
				{
					return false;
				}
			});//排序
			
			addHeroList();
		}
		break;
	case LOVE_EVENT_TYPE_SEND_GIFT:
		{
			updataGiftListItemById();
			setHerofavour();
			setAddlove();
			showPopReward();
		}
		break;
	case LOVE_EVENT_TYPE_BUY_GIFT:
		{
			updataGiftListItemById();

			RowData *giftRow = DataManager::getInstance()->searchToolById(m_giftTemplateId);
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20293)
				+ TO_STR(m_giftBuyCount) + DataManager::getInstance()->searchCommonTexdtById(20077) 
				+ giftRow->getStringData("itemName"),
				NORMAL_TIPS_BACKGROUND_IMG, 35, cocos2d::Color3B(0x54,0xff,0x26));

			LOVE_GIFT_INFO_VEC* giftVec = LoveModel::getInstance()->getLoveGiftInfoVec();
			LOVE_GIFT_INFO giftInfo = giftVec->at(m_giftListItemIdx);
			TalkingData::onPurchase(giftRow->getStringData("itemName"),m_giftBuyCount,giftInfo.price);
		}
		break;
	case LOVE_EVENT_TYPE_FINGER_GUESS:
		{
			setFingerGuseeInfo(FINGER_ENTER_TYPE_RESULT(LoveModel::getInstance()->getFingerOutCome()));
		}
		break;
	case LOVE_EVENT_TYPE_PLAY_MAP:
		{
			if (!LoveModel::getInstance()->getPlayMapBack())
			{
				m_state = LOVE_LAYER_STATE_PLAY;
				switchState();
				exitHero();
				intiPlay();
			}
			else
			{
				LoveModel::getInstance()->getPlayMapBack() = false;
					
				if (m_state == LOVE_LAYER_STATE_PLAY)
				{
					intiPlay(false);
				}
			}

			int& flag = LoveModel::getInstance()->getPlayFlag();
			if (flag == 1)
			{
				flag = 0;

				LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
				//ImageView* heroLoveLv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pImgDuiDaiHero,"hero_love_lv"));
				//heroLoveLv->loadTexture("Image/Icon/global/img_haogan2_" + TO_STR(heroInfo->expType) + ".png");

				LoveController::getHeroListInfo();
				initHeroAttribute();
			}
		}
		break;
	case LOVE_EVENT_TYPE_PLAY_MAP_INFO:
		{
			LoveTreasureScene *TreasureScene = LoveTreasureScene::create(m_playMapIdx,m_chooseHeroPetId);
			Director::getInstance()->pushScene(TreasureScene);
		}
		break;
	case LOVE_EVENT_TYPE_BUY_GUESS_INFO:
		{
			if (LoveModel::getInstance()->getBuyGuessMaxCount() > 0 && 
				LoveModel::getInstance()->getBuyGuessCurCount() < LoveModel::getInstance()->getBuyGuessMaxCount())
			{
				m_ui.pVipStateRoot->setVisible(true);
				SoundUtils::playSoundEffect("tanchuang");
				ActionCreator::runCommDlgAction(m_ui.imgVipBg1);
				m_ui.txtCost->setString(TO_STR(LoveModel::getInstance()->getBuyGuessPrice()));
				m_ui.txtCurCount->setString(TO_STR(LoveModel::getInstance()->getBuyGuessMaxCount()-
					LoveModel::getInstance()->getBuyGuessCurCount()) + "/" +
					TO_STR(LoveModel::getInstance()->getBuyGuessMaxCount()));
			}
			else
			{
				DataTable* _dt = DataManager::getInstance()->getDTVip();
				int nCurVipLv = VipModel::getInstance()->getCurVipLv();
				int nNextValue = 0;
				int nNextVipLv = getNextValidVip(nNextValue);
				if (nCurVipLv == nNextVipLv)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20287));
				}
				else
				{
					ostringstream oss;
					oss << DataManager::getInstance()->searchCommonTexdtById(20294) << nNextValue << 
						DataManager::getInstance()->searchCommonTexdtById(10049);
					m_ui.txtNextCount->setString(oss.str());
					oss.str("");
					m_ui.imgVipNext->loadTexture(getVipIcon(nNextVipLv));
					m_ui.pVipRoot->setVisible(true);
					SoundUtils::playSoundEffect("tanchuang");
					ActionCreator::runCommDlgAction(m_ui.imgVipBg);
				}
			}
		}
		break;
	case LOVE_EVENT_TYPE_BUY_GUESS:
		{
			setFingerGuseePanelInfo();
		}
		break;
	case LOVE_EVENT_TYPE_ADD_ATTRIBUTE:
		{
			initHeroAttribute();
		}
		break;
	case LOVE_EVENT_TYPE_RESET_ATTRIBUTE:
		{
			initHeroAttribute();
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20295));
		}
		break;
	case LOVE_EVENT_TYPE_CHANGE_GUESS:
		{
			m_canBuyWin = true;
			if (m_isChangeMing)
			{
				setFingerGuseeInfo(FINGER_ENTER_TYPE_RESULT_WIN);
			}
			else
			{
				setFingerAddlove();
				showPopReward();

				setFingerGuseePanelInfo();

				m_ui.changeMing->setVisible(false);
				m_ui.ImgResult->setVisible(false);
				m_ui.pnlFinger->setVisible(false);

				m_ui.pnlDrinkBegin->setVisible(true);
				m_ui.PnlDrinkReward->setVisible(true);
				m_ui.pnlDrinkBegin->setOpacity(255);
				m_canGuess = true;
			}
		}
		break;
	case LOVE_EVENT_TYPE_GET_GUESS_PRIZE:
		{
			setFingerGuseePanelInfo();

			SEND_GIFT_REWARD_VEC* Reward = LoveModel::getInstance()->getLoveGiftReeardVec();
			vector<PopRewardItem*> vecRewardItem;

			if (Reward->size() > 0)
			{
				//道具
				RowData *friend_ship = DataManager::getInstance()->searchToolSystemById(FIREND_SHIP_ID);
				for (size_t i = 0; i < Reward->size(); i++)
				{
					if (friend_ship && Reward->at(i).templateId == friend_ship->getIntData("value"))
					{
						PopRewardItem_FriendShip* rewardItem = new PopRewardItem_FriendShip(Reward->at(i).amount);
						vecRewardItem.push_back(rewardItem);
					}
					else
					{
						PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(Reward->at(i).templateId, Reward->at(i).amount);
						vecRewardItem.push_back(rewardItem);
					}
				}
				PopReward::show(vecRewardItem);
			}
		}
		break;
	default:
	break;
	}
}

void LoveLayer::onBtnNotify(Ref* sender, Widget::TouchEventType type)
{
	Size size = Director::getInstance()->getVisibleSize();
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (m_state)
		{
		case LOVE_LAYER_STATE_NONE:
			break;
		case LOVE_LAYER_STATE_ENTER:
			{
				if (sender == m_ui.phelp) //帮助
				{
					LayerGameRules* loveRule = LayerGameRules::create(LOVE_RULE_ID);
					this->addChild(loveRule, 1);

					ActionCreator::runCommDlgAction(loveRule);
				}
			}
			break;
		case LOVE_LAYER_STATE_HERO:
			{
				if (sender == m_ui.phelp) //帮助
				{
					LayerGameRules* loveRule = LayerGameRules::create(LOVE_RULE_ID);
					this->addChild(loveRule, 1);

					ActionCreator::runCommDlgAction(loveRule);
				}
				else if (sender == m_ui.pBtnGift)
				{
					LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
					if (heroInfo->expType > HERO_GOOD_LV_STRANGE)
					{
						m_state = LOVE_LAYER_STATE_GIFT;
						switchState();
						exitHero();
						initGiftList();
					}
					else
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20296));
						setHeroRandSay("unlockAnswer");
					}
				}
				else if (sender == m_ui.pBtnPlay)
				{
					LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
					
					if (heroInfo->expType > HERO_GOOD_LV_STRANGE)
					{
						//情缘游玩开启条件
						RowData* rowDataPlay = DataManager::getInstance()->searchToolSystemById(FUNCTION_LOVE_PLAY);
						if (rowDataPlay)
						{
							int functionLevel = rowDataPlay->getIntData("value");
							if (heroInfo->curLevel >= functionLevel)
							{
								if (heroInfo->playCount > 0 && LoveModel::getInstance()->getAllPlayCount() > 0) //公用游玩次数大于0 且没有玩过
								{
									LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
									LoveController::playMap(heroInfo->petId);
								}
								else if (heroInfo->playCount <= 0)
								{
									CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20297));
									setHeroRandSay("playAnswer"); // 次数不足
								}
								else if(LoveModel::getInstance()->getAllPlayCount() <= 0)
								{
									CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20298));
									setHeroRandSay("playAnswer"); // 次数不足
								}
							}
							else
							{
								RowData *loveLevelData = DataManager::getInstance()->searchLoveLevelById(functionLevel);
								CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20299)
									+ loveLevelData->getStringData("name") 
									+ "LV"+ TO_STR(loveLevelData->getIntData("level")),NORMAL_TIPS_BACKGROUND_IMG, 35,
									cocos2d::Color3B(0xf9, 0xee, 0x82));
							}
						}
					}
					else
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20296));
						setHeroRandSay("unlockAnswer");
					}
				}
				else if (sender == m_ui.pBtnDrink)
				{
					LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
					if (heroInfo->expType > HERO_GOOD_LV_STRANGE)
					{
						//情缘喝酒开启条件
						RowData* rowData = DataManager::getInstance()->searchToolSystemById(FUNCTION_LOVE_DRINK);
						if (rowData)
						{
							int functionLevel = rowData->getIntData("value");
							if (heroInfo->curLevel >= functionLevel)
							{
								int guessAllCount = LoveModel::getInstance()->getGuessCount();
								if (!heroInfo->isGuess && guessAllCount <= 0)
								{
									CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20300));
								}
								else
								{
									m_state = LOVE_LAYER_STATE_DRINK;
									switchState();
									exitHero();
									initDrink();
								}
							}
							else
							{
								RowData *loveLevelData = DataManager::getInstance()->searchLoveLevelById(functionLevel);
								CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20299) +
									loveLevelData->getStringData("name") 
									+ "LV"+ TO_STR(loveLevelData->getIntData("level")),NORMAL_TIPS_BACKGROUND_IMG, 35, 
									cocos2d::Color3B(0xf9, 0xee, 0x82));
							}
						}
					}
					else
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20296));
						setHeroRandSay("unlockAnswer");
					}
				}
				else if(sender == m_ui.pPnlHeroProAdd)
				{
					LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
					if (heroInfo->expType > HERO_GOOD_LV_STRANGE)
					{
						LoveProAdd   *loveAdd = LoveProAdd::create(m_chooseHeroPetId);
						addChild(loveAdd,2);
					}
					else
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20296));
						setHeroRandSay("unlockAnswer");
					}
				}
			}
			break;
		case LOVE_LAYER_STATE_PLAY:
			break;
		case LOVE_LAYER_STATE_GIFT:
			{
				if (sender == m_ui.phelp) //帮助
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20301));
				}
			}
			break;
		case LOVE_LAYER_STATE_GUESS:
			break;
		case LOVE_LAYER_STATE_DRINK:
			{
				LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);

				if (sender == m_ui.phelp) //帮助
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20301));
				}
				else if (sender == m_ui.BtnfingerStone)//石头
				{
					if (heroInfo->guessCount <= 0)
					{
						LoveController::buyGuessCountInfo(m_chooseHeroPetId);
					}
					else if(m_canGuess)
					{
						m_canGuess = false;
						onFingerEnter(FINGER_ENTER_TYPE_STONE);
					}
				}
				else if (sender == m_ui.BtnfingerScissors) //剪刀
				{
					if (heroInfo->guessCount <= 0)
					{
						LoveController::buyGuessCountInfo(m_chooseHeroPetId);
					}
					else if(m_canGuess)
					{
						m_canGuess = false;
						onFingerEnter(FINGER_ENTER_TYPE_SCISS);
					}
				}
				else if (sender == m_ui.BtnfingerCloth) //布
				{
					if (heroInfo->guessCount <= 0)
					{
						LoveController::buyGuessCountInfo(m_chooseHeroPetId);
					}
					else if(m_canGuess)
					{
						m_canGuess = false;
						onFingerEnter(FINGER_ENTER_TYPE_CLOTH);
					}
				}
				else if (sender == m_ui.addGuessBtn) //购买次数
				{
					LoveController::buyGuessCountInfo(m_chooseHeroPetId);
				}
				else if (sender == m_ui.btnVipStateOK) //购买提示确认
				{
					m_ui.pVipStateRoot->setVisible(false);
					MainParam* param = MainModel::getInstance()->getMainParam();
					if (param->mGold < LoveModel::getInstance()->getBuyGuessPrice())
					{
						CustomGoldTips *_tips = CustomGoldTips::create();
						if (_tips)
						{
							Director::getInstance()->getRunningScene()->addChild(_tips);
						}
					}
					else
					{
						LoveController::buyGuessCount(m_chooseHeroPetId);
					}
				}
				else if (sender == m_ui.btnVipStateCancel) //购买提示取消
				{
					m_ui.pVipStateRoot->setVisible(false);
				}
				else if (sender == m_ui.btnVipStateClose) //购买提示关闭
				{
					m_ui.pVipStateRoot->setVisible(false);
				}
				else if (sender == m_ui.btnVipOK) //购买提升vip确认
				{
					m_ui.pVipRoot->setVisible(false);
					Director::getInstance()->pushScene(VipScene::createScene());
				}
				else if (sender == m_ui.btnVipCancel) //购买提升vip取消
				{
					m_ui.pVipRoot->setVisible(false);
				}
				else if (sender == m_ui.btnVipClose) //购买提升vip关闭
				{
					m_ui.pVipRoot->setVisible(false);
				}
				else if (sender == m_ui.changeMingAgain) //继续
				{
					m_isChangeMing = false;
					LoveController::changeGuess(m_chooseHeroPetId,0);
				}
				else if (sender == m_ui.changeMingWin) //赢
				{
					int ChangeGuessPrice = LoveModel::getInstance()->getChangeGuessPrice();
					MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
					
					if (m_canBuyWin)
					{
						m_canBuyWin = false;
						if (pMainParam->mGold < ChangeGuessPrice)
						{
							CustomGoldTips *_tips = CustomGoldTips::create();
							if (_tips)
							{
								Director::getInstance()->getRunningScene()->addChild(_tips);
							}
							//充值界面
							return;
						}
						onChangeMingEnter();
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

void LoveLayer::onBtnVisitFriend(Ref* sender, Widget::TouchEventType type,int petId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED && m_canVisit)
	{
		m_canVisit = false;
		m_chooseHeroPetId = petId;
		LoveModel::getInstance()->removeNewPetId(petId);

		m_state =  LOVE_LAYER_STATE_HERO;
		switchState();
		exitMain();
		initHero();
	}
}

void LoveLayer::onGiftSend(Ref* sender, Widget::TouchEventType type,int petId,LOVE_GIFT_INFO *giftInfo,int idx)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");

		m_giftLongPress= false;
		m_giftSendCount = 0;
		this->schedule(schedule_selector(LoveLayer::onCallGiftSend), 0.5f);

		m_giftListItemIdx = idx;
		m_giftPetId = petId;
		m_giftInfo = giftInfo;

		LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
		heroInfo->lastLevel = heroInfo->curLevel;	//上次好感度等级	
	}
	else if (type == Widget::TouchEventType::MOVED)
	{
		// 移动只解除长按判定;
		//this->unschedule(schedule_selector(LoveLayer::onCallGiftSend));
	}
	else
	{
		this->unschedule(schedule_selector(LoveLayer::onCallGiftSend));
		this->unschedule(schedule_selector(LoveLayer::onAddGiftSendCount));

		if (m_giftLongPress)
		{
			if (m_giftSendCount > 0)
			{
				LoveController::sendGift(m_giftPetId,m_giftInfo->templateId,m_giftSendCount);
			}
		}
		else
		{
			LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
			RowData *loveLevelData = DataManager::getInstance()->searchLoveLevelById(heroInfo->curLevel);

			if (loveLevelData && loveLevelData->getIntData("exp") <= 0)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20302));
				return;
			}

			m_giftSendCount = 1;
			LoveController::sendGift(m_giftPetId,m_giftInfo->templateId,m_giftSendCount);
		}
	}
}

void LoveLayer::onChoosePlayInfo(Ref* sender, Widget::TouchEventType type,int petId,int idx)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		m_playMapIdx = idx;
		PLAY_MAP playMap = LoveModel::getInstance()->getPlayMap()->at(idx);
		switch (playMap.flg)
		{
		case PLAY_MAP_TYPE_RESULT_IMCOMPLETE:
			{
				Layout* pImgMapInfo = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.Panel_mapInfo,"Pnl_Task"));

				string strFileName;
				strFileName.assign("Image/UIqingyuan/UITask2.ExportJson");
				Widget* layer = dynamic_cast<Widget*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
				if (pImgMapInfo->getChildByName("PlayInfo"))
				{
					pImgMapInfo->removeChildByName("PlayInfo");
				}
				pImgMapInfo->addChild(layer,1,"PlayInfo");

				ImageView* Img_tanbaoling = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_tanbaoling"));
				ActionCreator::runCommDlgAction(Img_tanbaoling);

				LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);

				RowData *lovePlay = DataManager::getInstance()->searchLoveTreasureByid(playMap.id);
				RowData *toolDate = DataManager::getInstance()->searchToolById(lovePlay->getIntData("treasureId"));

				ImageView *result = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Img_tanbaoling,"result"));
				result->setVisible(false);

				Text *name = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_tanbaoling,"Lab_Name"));
				name->setFontName(FONT_FZZHENGHEI);
				name->setString(toolDate->getStringData("itemName"));
				setTextAddStroke(name,Color3B(0x00,0x00,0x00),2);

				ImageView *head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Img_tanbaoling,"Img_Item"));
				head->loadTexture(ResourceUtils::getSmallIconPath(toolDate->getIntData("resId")));
				for (int i = 0; i < DifficultCount; i++)
				{
					ImageView *Img_Difficult = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Img_tanbaoling,"Img_Difficult_" + TO_STR(i)));
					Img_Difficult->setVisible(false);

					if (i < lovePlay->getIntData("difficult"))
					{
						Img_Difficult->setVisible(true);
					}
				}

				vector<string> rewardItem;
				StringFormat::parseCsvStringVec(lovePlay->getStringData("rewardItem"), rewardItem);
				for (int i = 0; i < 3; i++)
				{
					Layout *PnlReward = dynamic_cast<Layout*>(Helper::seekWidgetByName(Img_tanbaoling,"Pal_Reward_" + TO_STR(i)));

					if (rewardItem.size() > i)
					{
						PnlReward->setVisible(true);
						vector<int> reward;
						StringFormat::parseCsvStringVecByDu(rewardItem[i], reward);

						UIToolHeader *icon = UIToolHeader::create(reward[0]);
						icon->setNumEx(reward[1]);
						icon->setAnchorPoint(Vec2(0,0));
						icon->setScale(0.8f);
						icon->setToolId(reward[0]);
						icon->setTipsEnabled(true);
						PnlReward->addChild(icon);
					}
					else
					{
						PnlReward->setVisible(false);
					}
				}

				//开始玩
				Button *BtnGo = dynamic_cast<Button*>(Helper::seekWidgetByName(Img_tanbaoling,"Btn_Go"));
				auto goCall = [this,layer,petId,playMap,heroInfo](Ref* sender, Widget::TouchEventType type)->void
				{
					if (type == Widget::TouchEventType::BEGAN )
					{
						SoundUtils::playSoundEffect("dianji");
					}
					else if (type == Widget::TouchEventType::ENDED)
					{
						if (heroInfo->playCount > 0)
						{
							m_ui.Panel_mapInfo->setVisible(false);
							LoveController::chooseMap(petId,playMap.id);
						}
						else
						{
							CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20303));
						}
					}
				};
				BtnGo->addTouchEventListener(goCall);

				//退出
				m_ui.Panel_mapInfo->setVisible(true);
				auto call = [this,layer](Ref* sender, Widget::TouchEventType type)->void
				{
					if (type == Widget::TouchEventType::BEGAN)
					{
						SoundUtils::playSoundEffect("dianji");
					}
					else if (type == Widget::TouchEventType::ENDED)
					{
						layer->removeFromParentAndCleanup(true);
						m_ui.Panel_mapInfo->setVisible(false);
					}
				};
				m_ui.Panel_mapInfo->addTouchEventListener(call);
			}
			break;
		case PLAY_MAP_TYPE_RESULT_COMPLETE:
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20304));
			}
			break;
		case PLAY_MAP_TYPE_RESULT_FAIL:
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20305));
			}
			break;
		default:
			break;
		}
	}
}

void LoveLayer::onViewGuessPrize(Ref* sender, Widget::TouchEventType type,int idx)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
		RowData *loveFinger = DataManager::getInstance()->searchLoveFingerById(heroInfo->petId);
		vector<int> vecWill;
		StringFormat::parseCsvStringVec(loveFinger->getStringData("win"),vecWill);

		vector<int>::iterator result = find( heroInfo->guessReward.begin( ), heroInfo->guessReward.end( ), idx); //查找3
		if ( result == heroInfo->guessReward.end() && heroInfo->guessWinCount >= vecWill[idx] ) //没找到 能领取
		{
			LoveController::getGuseePrize(m_chooseHeroPetId,idx);
		}
		else
		{
			m_ui.pPnlDrinkInfoPrizePneal->setVisible(true);
			ActionCreator::runCommDlgAction(m_ui.pPnlDrinkInfoPrize);

			LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
			RowData *loveFinger = DataManager::getInstance()->searchLoveFingerById(heroInfo->petId);
			vector<string> rewardStr;
			StringFormat::parseCsvStringVec(loveFinger->getStringData("reward"),rewardStr);
			vector<string> reward;
			StringFormat::parseCsvStringVec(rewardStr.at(idx),reward,';');
			for (int i = 0; i < reward.size(); i++) //道具
			{
				vector<int> _reward;
				StringFormat::parseCsvStringVecByDu(reward.at(i),_reward);

				UIToolHeader* item = UIToolHeader::create(_reward[0]);
				item->setAnchorPoint(Vec2(0,0));
				item->setNumEx(_reward[1]);
				item->setToolId(_reward[0]);
				item->setTipsEnabled(true);
				item->setScale(0.8f);

				Layout *Pnl_Item = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfoPrize,"Pnl_Item" + TO_STR(i+1)));
				Pnl_Item->addChild(item);

				RowData* rowData = DataManager::getInstance()->searchToolById(_reward[0]);
				Text *Lab_ItemName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfoPrize,"Lab_ItemName" + TO_STR(i+1)));
				Lab_ItemName->setFontName(FONT_FZZHENGHEI);
				Lab_ItemName->setString(rowData->getStringData("itemName"));
				setTextAddStroke(Lab_ItemName,Color3B(0x8e,0x00,0x03),2);
			}

			//友情点
			RowData *friend_ship = DataManager::getInstance()->searchToolSystemById(FIREND_SHIP_ID);
			vector<int> friendShip;
			StringFormat::parseCsvStringVec(loveFinger->getStringData("friendShip"),friendShip);
			if (friendShip.at(idx) > 0)
			{
				UIToolHeader* item = UIToolHeader::create(friend_ship->getIntData("value"));
				item->setAnchorPoint(Vec2(0,0));
				item->setNumEx(friendShip.at(idx));
				item->setToolId(friend_ship->getIntData("value"));
				item->setTipsEnabled(true);
				item->setScale(0.8f);

				Layout *Pnl_Item = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfoPrize,"Pnl_Item3"));
				Pnl_Item->addChild(item);
				RowData* rowData = DataManager::getInstance()->searchToolById(friend_ship->getIntData("value"));
				Text *Lab_ItemName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfoPrize,"Lab_ItemName3"));
				Lab_ItemName->setFontName(FONT_FZZHENGHEI);
				Lab_ItemName->setString(rowData->getStringData("itemName"));
				setTextAddStroke(Lab_ItemName,Color3B(0x8e,0x00,0x03),2);

				Pnl_Item->setVisible(true);
				Lab_ItemName->setVisible(true);
			}
			else
			{
				Layout *Pnl_Item = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfoPrize,"Pnl_Item3"));
				Text *Lab_ItemName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfoPrize,"Lab_ItemName3"));
				Pnl_Item->setVisible(false);
				Lab_ItemName->setVisible(false);
			}


			vector<int> vecWill;
			StringFormat::parseCsvStringVec(loveFinger->getStringData("win"),vecWill);
			Text *Lab_Count = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlDrinkInfoPrize,"Lab_Count"));
			Lab_Count->setFontName(FONT_FZZHENGHEI);
			Lab_Count->setString(TO_STR(vecWill.at(idx)));
		}
	}
}


void LoveLayer::onCallGiftSend(float dt)
{
	m_giftLongPress = true;
	this->unschedule(schedule_selector(LoveLayer::onCallGiftSend));
	this->schedule(schedule_selector(LoveLayer::onAddGiftSendCount), 0.1f, kRepeatForever, 0.0f);
}

void LoveLayer::onAddGiftSendCount(float dt)
{
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
	RowData *loveLevelData = DataManager::getInstance()->searchLoveLevelById(heroInfo->curLevel);

	if (loveLevelData && loveLevelData->getIntData("exp") <= 0)
	{
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20302));
		this->unschedule(schedule_selector(LoveLayer::onAddGiftSendCount));
		return;
	}
	else
	{
		int goodWillCount = getGiftToHeroGoodWill(m_giftInfo->templateId);
		LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);

		if (m_giftInfo->amount > 0 &&  (!loveLevelData || (loveLevelData && loveLevelData->getIntData("exp") > 0)))
		{
			if (!loveLevelData)
			{
				RowData *_loveLevelData = DataManager::getInstance()->searchLoveLevelById(1);
				if (heroInfo->goodWill + goodWillCount >= _loveLevelData->getIntData("exp"))
				{
					heroInfo->goodWill = heroInfo->goodWill + goodWillCount - _loveLevelData->getIntData("exp");
					heroInfo->curLevel++;
				}
				else
				{
					heroInfo->goodWill  = heroInfo->goodWill + goodWillCount;
				}
			}
			else
			{
				if (heroInfo->goodWill + goodWillCount >= loveLevelData->getIntData("exp"))
				{
					heroInfo->goodWill = heroInfo->goodWill + goodWillCount - loveLevelData->getIntData("exp");
					heroInfo->curLevel++;
				}
				else
				{
					heroInfo->goodWill  = heroInfo->goodWill + goodWillCount;
				}
			}

			m_giftSendCount++;
			m_giftInfo->amount--;

			setHerofavour();
			updataGiftListItemById();
		}
		else if (m_giftInfo->amount <= 0)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10236));
			this->unschedule(schedule_selector(LoveLayer::onAddGiftSendCount));
		}
		else
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20307));
			this->unschedule(schedule_selector(LoveLayer::onAddGiftSendCount));
		}
	}
}

void LoveLayer::onGiftBuyBtn(Ref* sender, Widget::TouchEventType type,LOVE_GIFT_INFO *info,int idx)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		m_giftTemplateId = info->templateId;
		m_giftListItemIdx = idx;

		GeneralBuy *buy = GeneralBuy::create(info->resCount,info->price,CC_CALLBACK_1(LoveLayer::onGiftBuy,this));
		addChild(buy,1);
	}
}

void LoveLayer::onGiftBuy(int amount)
{
	m_giftBuyCount = amount;
	LoveController::buyGift(m_giftTemplateId,m_giftBuyCount);
}

void LoveLayer::onFingerEnter(FINGER_ENTER_TYPE type)
{
	m_ui.PnlDrinkReward->setVisible(false);

	m_MyfingetGuseeType = type;
	auto call = [this]()->void
	{
		m_ui.pnlFinger->setVisible(true);
		m_ui.pnlDrinkBegin->setVisible(false);
		m_ui.fingerHero->loadTexture("Image/Icon/global/img_shitou.png");
		m_ui.fingerMin->loadTexture("Image/Icon/global/img_shitou.png");

		auto _call = [this]()->void
		{
			LOVE_HERO_INFO_VEC* heroVec = LoveModel::getInstance()->getLoveHeroInfoVec();
			LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);

			LoveController::fingerGuess(heroInfo->petId,m_MyfingetGuseeType);
		};

		m_ui.fingerMin->runAction(CCSequence::create(CCRepeat::create(CCSequence::create(CCRotateBy::create(0.25f,30),
			CCRotateBy::create(0.25f,-30),nullptr),2),CCRotateBy::create(0.25f,60),CCRotateBy::create(0.1f,-60),
			CCDelayTime::create(0.3f),CCCallFunc::create(_call),nullptr));

		m_ui.fingerHero->runAction(CCSequence::create(CCRepeat::create(CCSequence::create(CCRotateBy::create(0.25f,-30),
			CCRotateBy::create(0.25f,30),nullptr),2),CCRotateBy::create(0.25f,-60),CCRotateBy::create(0.1f,60),nullptr));
	};
	m_ui.pnlDrinkBegin->runAction(CCSequence::create(CCFadeOut::create(0.3f),CCCallFunc::create(call),nullptr));
}

void LoveLayer::onChangeMingEnter()
{
	auto call = [this]()->void
	{
		m_ui.ImgResult->setVisible(false);
		m_ui.changeMing->setVisible(false);
		m_ui.fingerHero->loadTexture("Image/Icon/global/img_shitou.png");
		m_ui.fingerMin->loadTexture("Image/Icon/global/img_shitou.png");

		auto _call = [this]()->void
		{
			LOVE_HERO_INFO_VEC* heroVec = LoveModel::getInstance()->getLoveHeroInfoVec();
			LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);

			m_isChangeMing = true;
			LoveController::changeGuess(m_chooseHeroPetId,1);
		};

		m_ui.fingerMin->runAction(CCSequence::create(CCRepeat::create(CCSequence::create(CCRotateBy::create(0.25f,30),
			CCRotateBy::create(0.25f,-30),nullptr),2),CCRotateBy::create(0.25f,60),CCRotateBy::create(0.1f,-60),
			CCDelayTime::create(0.3f),CCCallFunc::create(_call),nullptr));

		m_ui.fingerHero->runAction(CCSequence::create(CCRepeat::create(CCSequence::create(CCRotateBy::create(0.25f,-30),
			CCRotateBy::create(0.25f,30),nullptr),2),CCRotateBy::create(0.25f,-60),CCRotateBy::create(0.1f,60),nullptr));
	};
	m_ui.pnlDrinkBegin->runAction(CCSequence::create(CCFadeOut::create(0.3f),CCCallFunc::create(call),nullptr));
}

void LoveLayer::addHeroList()
{
	//显示剩余猜拳和探宝英雄
	m_ui.pHeroGuessCount->setVisible(true);
	m_ui.pHeroPlayCount->setVisible(true);
	m_ui.pHeroGuessCount->setString(TO_STR(LoveModel::getInstance()->getGuessCount()));
	m_ui.pHeroPlayCount->setString(TO_STR(LoveModel::getInstance()->getAllPlayCount()));
	setTextAddStroke(m_ui.pHeroGuessCount,Color3B(0x8e,0x00,0x03),2);
	setTextAddStroke(m_ui.pHeroPlayCount,Color3B(0x8e,0x00,0x03),2);

	LOVE_HERO_INFO_VEC* heroVec = LoveModel::getInstance()->getLoveHeroInfoVec();
	int row = ceil(heroVec->size()/5.0f);
	int scrollViewHeight = row * 148+10;
	scrollViewHeight = scrollViewHeight >  m_ui.pHeroList->getContentSize().height ? scrollViewHeight : m_ui.pHeroList->getContentSize().height;
	int scrollViewWidth = m_ui.pHeroList->getInnerContainerSize().width;
	m_ui.pHeroList->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	//添加奖励面板控件
	m_ui.pHeroList->removeAllChildren();
	m_unLockCount = 0;
	for (int i = 0; i < heroVec->size(); i++)
	{
		int rowNumber = i/5;
		int colNumber = i%5;
		UIToolHeader* item = UIToolHeader::create(heroVec->at(i).petId);
		item->setAnchorPoint(Vec2(0,0));
		//情缘等级
		ImageView* lv = ImageView::create("Image/Icon/global/img_haogan1_" + TO_STR(heroVec->at(i).expType) + ".png");
		lv->setPosition(Vec2(12,item->getContentSize().height-20));
		item->addChild(lv,10);
		//心情
		ImageView* mood; 
		if (LoveModel::getInstance()->findNewPetId(heroVec->at(i).petId))
		{
			mood = ImageView::create("Image/Icon/global/img_new.png");
			mood->setPosition(Vec2(item->getContentSize().width-12,item->getContentSize().height-12));
			item->addChild(mood,10);
			mood->setOpacity(0);

			mood->runAction(CCRepeatForever::create(CCSequence::create(CCFadeIn::create(1.0f),CCDelayTime::create(0.2),
				CCFadeOut::create(1.0f),CCDelayTime::create(0.2),nullptr)));
		}
		else if(heroVec->at(i).isGuess)
		{
			mood = ImageView::create("Image/Icon/global/img_icon_caiquan.png");
			mood->setPosition(Vec2(item->getContentSize().width-12,item->getContentSize().height-12));
			item->addChild(mood,10);
		}

		RowData* rowData = DataManager::getInstance()->searchToolById(heroVec->at(i).petId);
		Text *name = Text::create(rowData->getStringData("itemName"),FONT_FZZHENGHEI,24);
		name->setColor(Color3B(0xff,0xc0,0x46));
		name->setPosition(Vec2(item->getContentSize().width/2,-20));
		item->addChild(name);

		item->setPosition(Vec2(colNumber*140 + 10, scrollViewHeight-(rowNumber+1)*148+30 ));
		m_ui.pHeroList->addChild(item,10);

		item->setTouchEnabled(true);
		item->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onBtnVisitFriend,this,heroVec->at(i).petId));

		if (heroVec->at(i).expType > HERO_GOOD_LV_STRANGE) //解锁
		{
			m_unLockCount++;
			item->setMaskFlag(false);
		}
		else
		{
			item->setMaskFlag(true,false);
		}
	}

	//第一次添加英雄列表
	if (m_isFirstAddHero)
	{
		m_isFirstAddHero = false;
	}

	m_ui.pHeroLockCount->setString(DataManager::getInstance()->searchCommonTexdtById(20308) +
		TO_STR(m_unLockCount) + "/" + TO_STR(heroVec->size()));
	//初始化完毕再设置滚动条比例
	m_customScrollBarHero->setScrollBarLengthPercent(
		m_ui.pHeroList->getContentSize().height / m_ui.pHeroList->getInnerContainerSize().height);
	m_customScrollBarHero->setScrollBarPercentY(0.0f);
	m_ui.pHeroList->jumpToTop();
}

void LoveLayer::addGiftList()
{
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
	LOVE_GIFT_INFO_VEC* giftVec = LoveModel::getInstance()->getLoveGiftInfoVec();
	sort(giftVec->begin(),giftVec->end(),[this](LOVE_GIFT_INFO a, LOVE_GIFT_INFO b)->bool{
		int goodWillA = getGiftToHeroGoodWill(a.templateId);
		int goodWillB = getGiftToHeroGoodWill(b.templateId);

		if (goodWillA > goodWillB)
		{
			return true;
		}
		else
		{
			return false;
		}
	});//排序
	
	m_ui.giftList->removeAllItems();
	for (int i = 0; i < giftVec->size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIqingyuan/UIGiftItem.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		LOVE_GIFT_INFO *giftInfo = &giftVec->at(i);
		RowData *giftRowData = DataManager::getInstance()->searchToolById(giftInfo->templateId);

		//名字
		Text *name = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_name"));
		name->setFontName(FONT_FZZHENGHEI);
		name->setString(giftRowData->getStringData("itemName"));

		Text *goodWill = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_num"));
		goodWill->setFontName(FONT_FZZHENGHEI);

		int goodWillCount = getGiftToHeroGoodWill(giftInfo->templateId);
		goodWill->setString("+" + TO_STR(goodWillCount));
		setTextAddStroke(goodWill,Color3B(0x10,0x3b,0x19),2);

		ImageView *Imagelike =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Image_96"));
		if (goodWillCount == m_GooddGiftAdd) //最好
		{
			Imagelike->setVisible(true);
		}
		else 
		{
			Imagelike->setVisible(false);
		}

		Text *price = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_price"));
		price->setFontName(FONT_FZZHENGHEI);
		price->setString(TO_STR(giftInfo->price));

		Button *buy = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_buy"));
		buy->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onGiftBuyBtn,this,giftInfo,i));

		Button *give = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_give"));
		give->addTouchEventListener(CC_CALLBACK_2(LoveLayer::onGiftSend,this,heroInfo->petId,giftInfo,i));

		if (giftInfo->amount > 0)
		{
			buy->setVisible(false);
			give->setVisible(true);
		}
		else
		{
			buy->setVisible(true);
			give->setVisible(false);
		}
		Layout *Pnl_icon = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_icon"));

		UIToolHeader* icon = NULL;
		icon = UIToolHeader::create(giftInfo->templateId);
		icon->setNumEx(giftInfo->amount);
		icon->setScale(0.8f);
		icon->setToolId(giftInfo->templateId);
		icon->setTipsEnabled(true);
		icon->setPosition(Vec2(Pnl_icon->getContentSize().width/2,Pnl_icon->getContentSize().height/2));
		Pnl_icon->addChild(icon,1,"GIFT_ITEM_IMG");

		m_ui.giftList->pushBackCustomItem(layer);
	}

	m_ui.giftList->refreshView();
	m_customScrollBarGift->setScrollBarLengthPercent(
		m_ui.giftList->getContentSize().height / m_ui.giftList->getInnerContainerSize().height);
	m_customScrollBarGift->setScrollBarPercentY(0.0f);
	m_ui.giftList->jumpToTop();
}

int LoveLayer::getGiftToHeroGoodWill(int gitfId)
{
	if (m_chooseHeroPetId < 0)
	{
		return 0;
	}
	LOVE_HERO_INFO_VEC* heroVec = LoveModel::getInstance()->getLoveHeroInfoVec();
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
	RowData *LoveHero = DataManager::getInstance()->searchLoveHeroById(heroInfo->petId);

	//好礼物
	vector<string> goodVecStr;
	StringFormat::parseCsvStringVec(LoveHero->getStringData("goodGift"), goodVecStr);
	for (int i = 0; i < goodVecStr.size(); i++)
	{
		vector<int> goodVec;
		StringFormat::parseCsvStringVecByDu(goodVecStr[i], goodVec);

		if (goodVec[0] == gitfId)
		{
			m_GooddGiftAdd = goodVec[1];
		
			return goodVec[1];
		}
	}

	//普通礼物
	vector<string> ordinaryVecStr;
	StringFormat::parseCsvStringVec(LoveHero->getStringData("ordinaryGift"), ordinaryVecStr);
	for (int i = 0; i < ordinaryVecStr.size(); i++)
	{
		vector<int> ordinaryVec;
		StringFormat::parseCsvStringVecByDu(ordinaryVecStr[i], ordinaryVec);

		if (ordinaryVec[0] == gitfId)
		{
			m_OrdinaryGiftAdd = ordinaryVec[1];

			return ordinaryVec[1];
		}
	}

	//差礼物
	vector<string> badVecStr;
	StringFormat::parseCsvStringVec(LoveHero->getStringData("badGift"), badVecStr);
	for (int i = 0; i < badVecStr.size(); i++)
	{
		vector<int> badVec;
		StringFormat::parseCsvStringVecByDu(badVecStr[i], badVec);

		if (badVec[0] == gitfId)
		{
			m_BadGiftAdd = badVec[1];

			return badVec[1];
		}
	}

	return 0;
}

void LoveLayer::updataGiftListItemById()
{
	Widget * item = m_ui.giftList->getItem(m_giftListItemIdx);

	LOVE_GIFT_INFO_VEC* giftVec = LoveModel::getInstance()->getLoveGiftInfoVec();

	LOVE_GIFT_INFO giftInfo = giftVec->at(m_giftListItemIdx);

	Button *buy = dynamic_cast<Button*>(Helper::seekWidgetByName(item,"Btn_buy"));
	Button *give = dynamic_cast<Button*>(Helper::seekWidgetByName(item,"Btn_give"));
	if (giftInfo.amount > 0)
	{
		buy->setVisible(false);
		give->setVisible(true);
	}
	else
	{
		buy->setVisible(true);
		give->setVisible(false);
	}

	Layout *Pnl_icon = dynamic_cast<Layout*>(Helper::seekWidgetByName(item,"Pnl_icon"));
	if (Pnl_icon->getChildByName("GIFT_ITEM_IMG"))
	{
		Pnl_icon->removeChildByName("GIFT_ITEM_IMG");
	}

	UIToolHeader* icon = NULL;
	icon = UIToolHeader::create(giftInfo.templateId);
	icon->setNumEx(giftInfo.amount);
	icon->setScale(0.8f);
	icon->setPosition(Vec2(Pnl_icon->getContentSize().width/2,Pnl_icon->getContentSize().height/2));
	Pnl_icon->addChild(icon,1,"GIFT_ITEM_IMG");
}

void LoveLayer::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	switch(type)
	{
	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			if (pSender == m_ui.pHeroList)
			{
				Size size = m_ui.pHeroList->getInnerContainerSize();
				Vec2 vec = m_ui.pHeroList->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.pHeroList->getContentSize().height);

				m_customScrollBarHero->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			else if (pSender == m_ui.giftList)
			{
				Size size = m_ui.giftList->getInnerContainerSize();
				Vec2 vec = m_ui.giftList->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.giftList->getContentSize().height);

				m_customScrollBarGift->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
		}

		break;
	default:
		break;
	}
}

//对话动态显示
void LoveLayer::animationTalkOut(string strText, bool isL2R)
{
	m_strTalkContent = strText;
	m_nTalkContentLength = 0;
	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(LoveLayer::callBackTalkOut), this);
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(LoveLayer::callBackTalkOut), this, 0.03, kRepeatForever, 0, false);
}

void LoveLayer::callBackTalkOut(float dt)
{
	if (m_nTalkContentLength > m_strTalkContent.length())
	{
		Director::getInstance()->getScheduler()->unschedule(
			schedule_selector(LoveLayer::callBackTalkOut), this);
		m_nTalkContentLength = 0;

		auto call = [this]()->void
		{
			showNpcTalk();
		};
		m_ui.pTextNpcName->stopAllActions();
		m_ui.pTextNpcName->runAction(CCSequence::create(CCDelayTime::create(5.0f),CCCallFunc::create(call),nullptr));
	}
	else if (m_nTalkContentLength == m_strTalkContent.length())
	{
		m_nTalkContentLength ++;
		m_ui.pTextDuiBaiContext->setText(m_strTalkContent.substr(0, m_nTalkContentLength));
	}
	else
	{
		char cValue = m_strTalkContent.at(m_nTalkContentLength);
		if (cValue>0 && cValue<127 )
		{
			m_nTalkContentLength ++;
		}
		else//utf8中文占3个字符
		{
			m_nTalkContentLength += 3;
			if (m_nTalkContentLength > m_strTalkContent.length()+1)
				m_nTalkContentLength = m_strTalkContent.length()+1;
		}
		m_ui.pTextDuiBaiContext->setText(m_strTalkContent.substr(0, m_nTalkContentLength));
	}
}

void LoveLayer::setHeroRandSay(const string str)
{
	//英雄随机说句话
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
	
	RowData *LoveHero = DataManager::getInstance()->searchLoveHeroById(heroInfo->petId);
	vector<int> sayVec;
	StringFormat::parseCsvStringVec(LoveHero->getStringData(str), sayVec);
	srand((unsigned)time(NULL));
	int sayId = rand()%(sayVec.size());
	RowData *talkData = DataManager::getInstance()->searchLoveTalkById(sayVec[sayId]);
	animationTalkOut(talkData->getStringData("answer"));
}

void LoveLayer::setHeroRandFingerSay(const string str)
{
	//英雄随机说句话
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);

	RowData *LoveHero = DataManager::getInstance()->searchLoveFingerById(heroInfo->petId);
	vector<int> sayVec;
	StringFormat::parseCsvStringVec(LoveHero->getStringData(str), sayVec);
	srand((unsigned)time(NULL));
	int sayId = rand()%(sayVec.size());
	RowData *talkData = DataManager::getInstance()->searchLoveTalkById(sayVec[sayId]);
	animationTalkOut(talkData->getStringData("answer"));
}

void LoveLayer::showNpcTalk()
{
	m_npcTalkCount++;
	if (m_npcTalkCount > NPC_TALK_COUNT)
	{
		return;
	}
	int talkId = 10000 + m_npcTalkCount;
	RowData *talkData = DataManager::getInstance()->searchLoveTalkById(talkId);
	animationTalkOut(talkData->getStringData("answer"));
}

void LoveLayer::exitMain()
{
	Size size = Director::getInstance()->getVisibleSize();
	//英雄列表右移
	m_ui.pPnlHeroItem->runAction(CCMoveTo::create(exitMain_time,Vec2(m_ui.pPnlMain->getContentSize().width,m_ui.pPnlHeroItemPos.y)));

	//服务员
	m_ui.pImgWaiter->runAction(CCMoveTo::create(exitMain_time,Vec2(-m_ui.pImgWaiter->getContentSize().width/2,
		m_ui.pImgWaiterPos.y)));

	////对白
	auto call = [this]()->void
	{
		m_ui.pPnlMain->setVisible(false);
		enterHero(true);
		m_canVisit = true;
	};
	m_ui.pImgDuiDai->runAction(CCSequence::create(CCMoveTo::create(exitMain_time,Vec2(m_ui.pImgDuiDaiPos.x,
		-m_ui.pImgDuiDai->getContentSize().height/2)),CCCallFunc::create(call),nullptr));
}

void LoveLayer::enterMain()
{
	//英雄列表返回
	m_ui.pPnlHeroItem->runAction(CCMoveTo::create(enterMain_time,m_ui.pPnlHeroItemPos));

	//服务员返回
	m_ui.pImgWaiter->runAction(CCMoveTo::create(enterMain_time,m_ui.pImgWaiterPos));

	////对白返回
	m_ui.pImgDuiDai->runAction(CCMoveTo::create(enterMain_time,m_ui.pImgDuiDaiPos));
}

void LoveLayer::enterHero(bool isFromMain)
{
	auto call = [this,isFromMain]()->void
	{
		if (isFromMain)
		{
			auto moveCall = [this]()->void
			{
				GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_LOVE_FUNC);
			};
			m_ui.pBtnGift->setVisible(true);
			m_ui.pBtnGift->runAction(CCSequence::create(CCSpawn::create(CCMoveTo::create(enterHero_time/2,m_ui.pBtnGiftPos),
				CCFadeIn::create(enterHero_time/2),nullptr),CCScaleTo::create(0.1f,1.2f),CCScaleTo::create(0.1f,1.0f),
				CCCallFunc::create(moveCall),nullptr));
		}
		else
		{
			m_ui.pBtnGift->setVisible(true);
			m_ui.pBtnGift->runAction(CCSequence::create(CCSpawn::create(CCMoveTo::create(enterHero_time/2,m_ui.pBtnGiftPos),
				CCFadeIn::create(enterHero_time/2),nullptr),CCScaleTo::create(0.1f,1.2f),CCScaleTo::create(0.1f,1.0f),nullptr));
		}

		m_ui.pBtnPlay->setVisible(true);
		m_ui.pBtnPlay->runAction(CCSequence::create(CCSpawn::create(CCMoveTo::create(enterHero_time/2,m_ui.pBtnPlayPos),
			CCFadeIn::create(enterHero_time/2),nullptr),CCScaleTo::create(0.1f,1.2f),CCScaleTo::create(0.1f,1.0f),nullptr));

		m_ui.pBtnDrink->setVisible(true);
		m_ui.pBtnDrink->runAction(CCSequence::create(CCSpawn::create(CCMoveTo::create(enterHero_time/2,m_ui.pBtnDrinkPos),
			CCFadeIn::create(enterHero_time/2),nullptr),CCScaleTo::create(0.1f,1.2f),CCScaleTo::create(0.1f,1.0f),nullptr));

		m_ui.pPnlHeroPro->setVisible(true);
		m_ui.pPnlHeroPro->runAction(CCMoveTo::create(enterHero_time/2,m_ui.pPnlHeroProPos));

		m_ui.pPnlFavour->setVisible(true);

		LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_chooseHeroPetId);
		if (heroInfo->expType > HERO_GOOD_LV_STRANGE)
		{
			setHeroRandSay("opening");
		}
		else
		{
			setHeroRandSay("unlockAnswer");
		}
	};
	m_ui.pImgHero->setVisible(true);
	m_ui.pImgHero->setOpacity(0);
	m_ui.pImgHero->setPosition(m_ui.pImgHeroPos);
	m_ui.pImgHero->runAction(CCSequence::create(CCFadeIn::create(enterHero_time),CCCallFunc::create(call),nullptr));
	m_ui.pImgDuiDai->runAction(CCMoveTo::create(enterHero_time,m_ui.pImgDuiDaiPos));
}

void LoveLayer::exitHero()
{
	auto call = [this]()->void
	{
		m_canBack = true;
		if (m_state == LOVE_LAYER_STATE_HERO || m_state == LOVE_LAYER_STATE_ENTER)
		{
			enterMain();
			addHeroList();
		}
		else if (m_state == LOVE_LAYER_STATE_GIFT)
		{
			enterGift();
		}
		else if (m_state == LOVE_LAYER_STATE_DRINK)
		{
			enterDrink();
		}
		else if (m_state == LOVE_LAYER_STATE_PLAY)
		{
			enterPlay();
		}
	};
	m_ui.pImgDuiDai->runAction(CCSequence::create(CCMoveTo::create(exitMain_time,Vec2(m_ui.pImgDuiDaiPos.x,
		-m_ui.pImgDuiDai->getContentSize().height/2)),CCCallFunc::create(call),nullptr));
}

void LoveLayer::enterGift()
{
	m_ui.giftListView->runAction(CCMoveTo::create(enterGift_time,m_ui.giftListPos));

	auto call = [this]()->void
	{
		GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_LOVE_GIFT);

		setHeroRandSay("giftAnswer");
	};
	////对白返回
	m_ui.pImgDuiDai->runAction(CCSequence::create(CCMoveTo::create(enterGift_time,m_ui.pImgDuiDaiPos),CCCallFunc::create(call),nullptr));
}

void LoveLayer::exitGift()
{
	auto call = [this]()->void
	{
		m_canBack = true;

		m_ui.pImgHero->runAction(CCMoveBy::create(enterHero_time/2,Vec2(125,0)));
		m_ui.pPnlHeroPro->setVisible(true);

		m_ui.pBtnGift->setVisible(true);
		m_ui.pBtnPlay->setVisible(true);
		m_ui.pBtnDrink->setVisible(true);
		m_ui.pPnlGift->setVisible(false);

		auto _call = [this]()->void
		{
			setHeroRandSay("opening");
		};

		m_ui.pImgDuiDai->runAction(CCSequence::create(CCMoveTo::create(enterHero_time,m_ui.pImgDuiDaiPos),CCCallFunc::create(_call),nullptr));
	};
	Size size = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	m_ui.giftListView->runAction(CCMoveTo::create(exitGift_time,
		Vec2(size.width + origin.x + m_ui.giftListView->getContentSize().width/2,m_ui.giftListView->getPosition().y)));

	m_ui.pImgDuiDai->runAction(CCSequence::create(CCMoveTo::create(exitGift_time,Vec2(m_ui.pImgDuiDaiPos.x,
		-m_ui.pImgDuiDai->getContentSize().height/2)),CCCallFunc::create(call),nullptr));
}

void LoveLayer::enterDrink()
{
	m_ui.DrinkHero->runAction(CCMoveTo::create(enterDrink_time,m_ui.DrinkHeroPos));
	m_ui.DrinkMin->runAction(CCMoveTo::create(enterDrink_time,m_ui.DrinkMinPos));
	m_canBuyWin = true;
	auto call = [this]()->void
	{
		setHeroRandFingerSay("begainAnswer");
	};
	//对白返回
	m_ui.pImgDuiDai->runAction(CCSequence::create(CCMoveTo::create(enterDrink_time,m_ui.pImgDuiDaiPos),CCCallFunc::create(call),nullptr));
}

void LoveLayer::exitDrink()
{
	Size size = Director::getInstance()->getVisibleSize();
	//英雄左移
	m_ui.DrinkHero->runAction(CCMoveTo::create(exitDrink_time,Vec2(-m_ui.DrinkHero->getContentSize().width/2,m_ui.DrinkHeroPos.y)));
	//右移
	m_ui.DrinkMin->runAction(CCMoveTo::create(exitDrink_time,Vec2(size.width+m_ui.DrinkMin->getContentSize().width/2,
		m_ui.DrinkMinPos.y)));

	m_ui.pPnlDrinkInfo->setVisible(false);
	auto call = [this]()->void
	{
		m_canBack = true;

		m_ui.pPnlDrink->setVisible(false);
		enterHero();
	};
	//对白
	m_ui.pImgDuiDai->runAction(CCSequence::create(CCMoveTo::create(exitDrink_time,Vec2(m_ui.pImgDuiDaiPos.x,
		-m_ui.pImgDuiDai->getContentSize().height/2)),CCCallFunc::create(call),nullptr));
}

void LoveLayer::enterPlay()
{
	m_ui.pImgMapDi->runAction(CCMoveTo::create(enterPlay_time,m_ui.imgMapDiPos));

	auto call = [this]()->void
	{
		setHeroRandSay("taskAnswer");
	};
	////对白返回
	m_ui.pImgDuiDai->runAction(CCSequence::create(CCMoveTo::create(enterPlay_time,m_ui.pImgDuiDaiPos),CCCallFunc::create(call),nullptr));
}

void LoveLayer::exitPlay()
{
	auto call = [this]()->void
	{
		m_canBack = true;

		m_ui.pImgHero->runAction(CCSpawn::create(CCMoveBy::create(enterHero_time/2,Vec2(400,15)),CCScaleTo::create(enterHero_time/2,1.0f),nullptr));

		m_ui.pBtnGift->setVisible(true);
		m_ui.pBtnPlay->setVisible(true);
		m_ui.pBtnDrink->setVisible(true);
		m_ui.pPnlFavour->setVisible(true);
		m_ui.pPnlHeroPro->setVisible(true);

		m_ui.pPnlplay->setVisible(false);

		auto _call = [this]()->void
		{
			setHeroRandSay("opening");
		};

		m_ui.pImgDuiDai->runAction(CCSequence::create(CCMoveTo::create(enterHero_time,m_ui.pImgDuiDaiPos),CCCallFunc::create(_call),nullptr));
	};
	Size size = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	m_ui.pImgMapDi->runAction(CCMoveTo::create(exitPlay_time,
		Vec2(size.width + origin.x + m_ui.pImgMapDi->getContentSize().width/2,m_ui.pImgMapDi->getPosition().y)));

	m_ui.pImgDuiDai->runAction(CCSequence::create(CCMoveTo::create(exitPlay_time,Vec2(m_ui.pImgDuiDaiPos.x,
		-m_ui.pImgDuiDai->getContentSize().height/2)),CCCallFunc::create(call),nullptr));
}
