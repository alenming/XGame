#include "LayerTakeCard.h"

#include "CocoStudio.h"
#include "TakeCardModel.h"
#include "TakeCardController.h"
#include "Common/Common.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "ResourceManager/ResourceLoader.h"
#include "UI/UIDef.h"
#include "Widget/LayerLoading.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/SceneTool.h"
#include "../ChapterModule/ChapterModel.h"
#include "Temp/CustomPop.h"
#include "Temp/CustomTips.h"
#include "HeroModule/HeroModel.h"
#include "SmithyModule/SmithyModel.h"
#include "Utils/StringFormat.h"
#include "../MainModule/MainModel.h"
#include "../Utils/ResourceUtils.h"
#include "MainModule/MainCityScene.h"

#include "VIPModule/VipLayer.h"
#include "../FormationModule/CustomScrollBar.h"
#include "CCArmature.h"
#include "GuideCenter/GuideManager.h"

#include "../RobModule/UIToolRob.h"
#include "ToolModule/UICardPrize.h"
#include "ToolModule/UICardHead.h"
#include "Common/Common.h"
#include "Utils/StringFormat.h"
#include "Widget/SimpleRichText.h"
#include "Common/Common.h"
#include "Temp/CustomGoldTips.h"
#include "SDK_TalkingData/TalkingData.h"
#include "LayerTakeCardView.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;  


const int MAX_TOOL_ITEM_WIDTH		= 310;
const int MAX_TOOL_ITEM_HEIGHT		= 222;

const int TAG_TAKECARD_BG = 10;
const int TAG_TAKECARD_FRONT = 11;
const int TAG_TAKECARD_TOOL = 12;

const float TEN_CARD_SCALE = 0.5f;
const float ONE_CARD_SCALE = 1.0f;

const float MOVE_TIME = 0.2f;

const float CARD_TURN_ANIMATION_DURATION = 0.1f;
const float CARD_TURN_ANIMATION_ROTATION = 360.0f;
const float CARD_ROTATION_ANIMATION_ANGLE = 135.0f;


LayerTakeCard::LayerTakeCard()
{
	m_isFreeTime		= false;
	m_isFreeTime2		= false;
	
	m_nTakeCardResultAnimationTime = 0;
	m_nTakeCardResultAnimationIndex		= 0;
	m_bTakeCardResultAnimationIsRun		= false;
	m_pTakeCardResultArmature			= nullptr;
	m_nTakeCardType = TakeCardTypeCoin;

	COIN_ONE_SPEND = DataManager::getInstance()->searchToolSystemById(COIN_ONE_SPEND_ID)->getIntData("value");
	COIN_TEN_SPEND = DataManager::getInstance()->searchToolSystemById(COIN_TEN_SPEND_ID)->getIntData("value");
	GOLD_ONE_SPEND = DataManager::getInstance()->searchToolSystemById(GOLD_ONE_SPEND_ID)->getIntData("value");
	GOLD_TEN_SPEND = DataManager::getInstance()->searchToolSystemById(GOLD_TEN_SPEND_ID)->getIntData("value");

	TakeCardModel::getInstance()->addObserver(this);
}

LayerTakeCard::~LayerTakeCard()
{
	CCLOG("LayerTakeCard destroy");
	TakeCardModel::getInstance()->removeObserver(this);
}

void LayerTakeCard::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;

	switch (observerParam->id)
	{
	case TakeCardNotify_GetInfo:
		{
			onGetTakeCardInfo(TakeCardModel::getInstance()->getTakeCardInfo());
		}
		break;
	case TakeCardNotify_GetCard:
		{
			onTakeCardResult();
		}
		break;
	case TakeCardNotify_Timer:
		{
			setCoinCountDownTime();
			
			setGoldCountDownTime();
		}
		break;
	default:
		break;
	}
}

LayerTakeCard* LayerTakeCard::create(bool pushScene)
{
	LayerTakeCard *pRet = new LayerTakeCard();
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

bool LayerTakeCard::init(bool pushScene)
{
	m_isPushScene = pushScene;
	if(!Layer::init())
	{
		return false;
	}
	
	initUI();
	initTakeCardUI();
	
	//初始状态隐藏
	m_ui.layerTakeCard.pLayTakeCard->setVisible(true);
	m_ui.layerTakeCard.pLayResult->setVisible(false);

	return true;
}


void LayerTakeCard::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// UI处理
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UImall/UImall_main.ExportJson"));
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);

	// 将UILayer层加入到当前的场景
	this->addChild(m_ui.pRoot,0);
	m_state = TakeCardState_zhu;
	auto _backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			switch (m_state)
			{
			case TakeCardState_zhu:
				if (m_isPushScene)
				{
					Director::getInstance()->popScene();
				}
				else
				{
					Director::getInstance()->replaceScene(MainCityScene::create());
				}
				break;
			case TakeCardState_show:
				{
					if (m_bTakeCardResultAnimationIsRun)
					{
						return;
					}	

					m_ui.layerTakeCard.pLayResult->setVisible(false);
					m_ui.layerTakeCard.pLayTakeCard->setVisible(true);

					m_ui.layerTakeCard.pLayTakeCardFront1->setVisible(true);
					m_ui.layerTakeCard.pLayTakeCardOther1->setVisible(false);
					m_ui.layerTakeCard.pLayTakeCardFront2->setVisible(true);
					m_ui.layerTakeCard.pLayTakeCardOther2->setVisible(false);

					m_state = TakeCardState_zhu;
				}
				break;
			default:
				break;
			}
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(_backCall,HEAD_INFO_TYPE_MALL);
	this->addChild(_layerCommHeadInfo,3,"layerCommHeadInfo_takeCard");

	m_ui.pRoot->setBackGroundImage("Image/Bg/img_shop_bg.jpg");
	//	

	ImageView *laobang = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"laobang"));
	laobang->setOpacity(0);
	laobang->runAction(CCFadeIn::create(0.5f));

	Layout *canKao = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_cankao"));
	Vec2 canKaoPos = canKao->getPosition();
	canKao->setPosition(Vec2(canKao->getContentSize().width,canKaoPos.y));

	auto moveCall = [this]()->void
	{
		GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_SHOP_1);
	};
	canKao->runAction(CCSequence::create(CCEaseSineOut::create(CCMoveTo::create(0.5f,canKaoPos)),
		CCCallFunc::create(moveCall),nullptr));

}

void LayerTakeCard::initTakeCardUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Vec2 pos = Director::getInstance()->getVisibleOrigin();
	Layout *canKao = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_cankao"));

	//抽卡
	m_ui.layerTakeCard.pLayTakeCard = dynamic_cast<Layout*>(Helper::seekWidgetByName(canKao,"Panel_chouka"));

	//地煞
	m_ui.layerTakeCard.pImgTakeCard1 = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pLayTakeCard->getChildByName("img_disha"));
	m_ui.layerTakeCard.pImgTakeCard1->addTouchEventListener(CC_CALLBACK_2(LayerTakeCard::onTakeCardClicked, this, 1));
	m_ui.layerTakeCard.DiBtnPreview = dynamic_cast<Button*>(m_ui.layerTakeCard.pLayTakeCard->getChildByName("Btn_PreviewCoin"));
	m_ui.layerTakeCard.DiBtnPreview->addTouchEventListener(CC_CALLBACK_2(LayerTakeCard::onBtnTakeCardView, this));
	//正面
	m_ui.layerTakeCard.pLayTakeCardFront1 = dynamic_cast<Layout*>(m_ui.layerTakeCard.pImgTakeCard1->getChildByName("Panel_zhengmian"));
	m_ui.layerTakeCard.pLayTakeCardFront1->setVisible(true);

	m_ui.layerTakeCard.pLayTakeCardFrontCountdown1 = dynamic_cast<Layout*>(m_ui.layerTakeCard.pLayTakeCardFront1->getChildByName("Panel_daojishi"));

	//倒计时
	m_ui.layerTakeCard.pDiShaZhengImgDaoJiShi = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pLayTakeCardFrontCountdown1->getChildByName("Img_daojishi"));
	m_ui.layerTakeCard.pDiShaZhengTextTime1 = dynamic_cast<Text*>(m_ui.layerTakeCard.pDiShaZhengImgDaoJiShi->getChildByName("lab_daojishi"));
	m_ui.layerTakeCard.pDiShaZhengTextTime1->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.pDiShaZhengTextTime1->setVisible(false);
	//剩余次数
	m_ui.layerTakeCard.pDiShaZhengImgDelCount = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pLayTakeCardFrontCountdown1->getChildByName("Img_cishu"));
	m_ui.layerTakeCard.pDiShaZhengDelCount = dynamic_cast<Text*>(m_ui.layerTakeCard.pDiShaZhengImgDelCount->getChildByName("Lab_cishu"));
	m_ui.layerTakeCard.pDiShaZhengDelCount->setFontName(FONT_FZZHENGHEI);
	//通用信息
	m_ui.layerTakeCard.pDiShaZhengImgCostBg = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pLayTakeCardFrontCountdown1->getChildByName("Img_cost_bg"));
	m_ui.layerTakeCard.pImgDiShaZhengYinbi = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pDiShaZhengImgCostBg->getChildByName("Img_yinbi"));
	m_ui.layerTakeCard.pImgDiShaZhengOrderCard = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pDiShaZhengImgCostBg->getChildByName("Img_dishaling"));
	m_ui.layerTakeCard.pDiShaZhengCommomCount = dynamic_cast<Text*>(m_ui.layerTakeCard.pDiShaZhengImgCostBg->getChildByName("Label_cost"));
	m_ui.layerTakeCard.pDiShaZhengCommomCount->setFontName(FONT_FZZHENGHEI);

	//反面
	m_ui.layerTakeCard.pLayTakeCardOther1  = dynamic_cast<Layout*>(m_ui.layerTakeCard.pImgTakeCard1->getChildByName("Panel_fanmian"));
	m_ui.layerTakeCard.pLayTakeCardOther1->setVisible(false);
	m_ui.layerTakeCard.pBtnTakeCardOne1 = dynamic_cast<Button*>(m_ui.layerTakeCard.pLayTakeCardOther1->getChildByName("btn_kaiyici"));
	m_ui.layerTakeCard.pBtnTakeCardTen1 = dynamic_cast<Button*>(m_ui.layerTakeCard.pLayTakeCardOther1->getChildByName("btn_kaishici"));
	m_ui.layerTakeCard.pBtnTakeCardOne1->addTouchEventListener(CC_CALLBACK_2(LayerTakeCard::onBtnTakeCard1, this, 1));
	m_ui.layerTakeCard.pBtnTakeCardTen1->addTouchEventListener(CC_CALLBACK_2(LayerTakeCard::onBtnTakeCard1, this, 10));

	m_ui.layerTakeCard.pTextTakeCardIntro1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardOther1,"Img_cishu1"));
	m_ui.layerTakeCard.pTextTakeCardIntro1Panel = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardOther1,"Img_buy1"));
	m_ui.layerTakeCard.pTextTakeCardIntro1Bichu = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardOther1,"img_bichu"));


	m_ui.layerTakeCard.DiImgCostYiciBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardOther1,"Img_cost_yici_bg"));
	m_ui.layerTakeCard.DiImgCostYiciYinbi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.DiImgCostYiciBg,"Img_yinbi"));
	m_ui.layerTakeCard.DiImgCostYiciOrderCard = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.DiImgCostYiciBg,"Img_dishaling"));
	m_ui.layerTakeCard.DiImgCostYiciCommomCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.layerTakeCard.DiImgCostYiciBg,"Label_cost"));
	m_ui.layerTakeCard.DiImgCostYiciCommomCount->setFontName(FONT_FZZHENGHEI);

	m_ui.layerTakeCard.DiImgCostShiciBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardOther1,"Img_cost_shici_bg"));
	m_ui.layerTakeCard.DiImgCostShiciYinbi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.DiImgCostShiciBg,"Img_yinbi"));
	m_ui.layerTakeCard.DiImgCostShiciOrderCard = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.DiImgCostShiciBg,"Img_dishaling"));
	m_ui.layerTakeCard.DiImgCostShiciCommomCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.layerTakeCard.DiImgCostShiciBg,"Label_cost"));
	m_ui.layerTakeCard.DiImgCostShiciCommomCount->setFontName(FONT_FZZHENGHEI);

	//天罡
	m_ui.layerTakeCard.pImgTakeCard2 = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pLayTakeCard->getChildByName("img_tiangang"));
	m_ui.layerTakeCard.pImgTakeCard2->addTouchEventListener(CC_CALLBACK_2(LayerTakeCard::onTakeCardClicked, this, 2));
	m_ui.layerTakeCard.TianBtnPreview = dynamic_cast<Button*>(m_ui.layerTakeCard.pLayTakeCard->getChildByName("Btn_PreviewGold"));
	m_ui.layerTakeCard.TianBtnPreview->addTouchEventListener(CC_CALLBACK_2(LayerTakeCard::onBtnTakeCardView, this));
	//正面
	m_ui.layerTakeCard.pLayTakeCardFront2 = dynamic_cast<Layout*>(m_ui.layerTakeCard.pImgTakeCard2->getChildByName("Panel_zhengmian"));
	m_ui.layerTakeCard.pLayTakeCardFront2->setVisible(true);
	m_ui.layerTakeCard.pLayTakeCardFrontCountdown2 = dynamic_cast<Layout*>(m_ui.layerTakeCard.pLayTakeCardFront2->getChildByName("Panel_daojishi"));

	m_ui.layerTakeCard.pTianGangZhengImgDaoJiShi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardFrontCountdown2,"Img_daojishi")); 
	m_ui.layerTakeCard.pTianGangZhengTextTime1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pTianGangZhengImgDaoJiShi,"lab_daojishi"));
	m_ui.layerTakeCard.pTianGangZhengTextTime1->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.pTianGangZhengTextTime1->setVisible(false);

	m_ui.layerTakeCard.pTianGangZhengImgDelCount = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardFrontCountdown2,"Img_cishu"));
	m_ui.layerTakeCard.pTianGangZhengDelCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pTianGangZhengImgDelCount,"Lab_cishu"));
	m_ui.layerTakeCard.pTianGangZhengDelCount->setFontName(FONT_FZZHENGHEI);

	m_ui.layerTakeCard.pTianGangZhengImgCostBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardFrontCountdown2,"Img_cost_bg"));
	m_ui.layerTakeCard.pTianGangZhengGold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pTianGangZhengImgCostBg,"Img_jinbi"));
	m_ui.layerTakeCard.pTianGangZhengOrderCard = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pTianGangZhengImgCostBg,"Img_tiangangling"));
	m_ui.layerTakeCard.pTianGangZhengCommomCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pTianGangZhengImgCostBg,"Label_cost"));
	m_ui.layerTakeCard.pTianGangZhengCommomCount->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.firstGetCard = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardFront2,"Img_shouchou"));
	m_ui.layerTakeCard.firstGetCard->setVisible(false);
	//反面
	m_ui.layerTakeCard.pLayTakeCardOther2  = dynamic_cast<Layout*>(m_ui.layerTakeCard.pImgTakeCard2->getChildByName("Panel_fanmian"));
	m_ui.layerTakeCard.pLayTakeCardOther2->setVisible(false);
	m_ui.layerTakeCard.pBtnTakeCardOne2 = dynamic_cast<Button*>(m_ui.layerTakeCard.pLayTakeCardOther2->getChildByName("btn_kaiyici"));
	m_ui.layerTakeCard.pBtnTakeCardTen2 = dynamic_cast<Button*>(m_ui.layerTakeCard.pLayTakeCardOther2->getChildByName("btn_kaishici"));
	m_ui.layerTakeCard.pBtnTakeCardOne2->addTouchEventListener(CC_CALLBACK_2(LayerTakeCard::onBtnTakeCard2, this, 1));
	m_ui.layerTakeCard.pBtnTakeCardTen2->addTouchEventListener(CC_CALLBACK_2(LayerTakeCard::onBtnTakeCard2, this, 10));

	m_ui.layerTakeCard.pTextTakeCardIntro2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardOther2,"Img_cishu1"));
	m_ui.layerTakeCard.pTextTakeCardIntro2Panel = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardOther2,"Img_buy1"));
	m_ui.layerTakeCard.pTextTakeCardIntro2Bichu = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardOther2,"img_bichu"));

	m_ui.layerTakeCard.TianImgCostYiciBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardOther2,"Img_cost_yici_bg"));
	m_ui.layerTakeCard.TianImgCostYiciGold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.TianImgCostYiciBg,"Img_jinbi"));
	m_ui.layerTakeCard.TianImgCostYiciOrderCard = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.TianImgCostYiciBg,"Img_tiangangling"));
	m_ui.layerTakeCard.TianImgCostYiciCommomCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.layerTakeCard.TianImgCostYiciBg,"Label_cost"));
	m_ui.layerTakeCard.TianImgCostYiciCommomCount->setFontName(FONT_FZZHENGHEI);

	m_ui.layerTakeCard.TianImgCostShiciBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pLayTakeCardOther2,"Img_cost_shici_bg"));
	m_ui.layerTakeCard.TianImgCostShiciGold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.TianImgCostShiciBg,"Img_jinbi"));
	m_ui.layerTakeCard.TianImgCostShiciOrderCard = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.TianImgCostShiciBg,"Img_tiangangling"));
	m_ui.layerTakeCard.TianImgCostShiciCommomCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.layerTakeCard.TianImgCostShiciBg,"Label_cost"));
	m_ui.layerTakeCard.TianImgCostShiciCommomCount->setFontName(FONT_FZZHENGHEI);

	//抽卡结果
	m_ui.layerTakeCard.pLayResult = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UImall/UImall_getcard.ExportJson"));
	m_ui.layerTakeCard.pLayResult ->setBackGroundImage("Image/Bg/img_shangcheng.jpg");

	this->addChild(m_ui.layerTakeCard.pLayResult,2);
	m_ui.layerTakeCard.pLayResult->setSize(size);
	m_ui.layerTakeCard.pLayResult->setPosition(pos);

	m_ui.layerTakeCard.PnlChouka = dynamic_cast<Layout*>(m_ui.layerTakeCard.pLayResult->getChildByName("Pnl_chouka"));
	m_ui.layerTakeCard.pBtnAgainOne = dynamic_cast<Button*>(m_ui.layerTakeCard.PnlChouka->getChildByName("btn_kaiyici"));
	m_ui.layerTakeCard.pBtnAgainOne->addTouchEventListener(CC_CALLBACK_2(LayerTakeCard::onBtnAgain, this, 1));
	m_ui.layerTakeCard.pBtnAgainTen = dynamic_cast<Button*>(m_ui.layerTakeCard.PnlChouka->getChildByName("btn_kaishici"));
	m_ui.layerTakeCard.pBtnAgainTen->addTouchEventListener(CC_CALLBACK_2(LayerTakeCard::onBtnAgain, this, 10));

	m_ui.layerTakeCard.pCoinPanel = dynamic_cast<Layout*>(m_ui.layerTakeCard.PnlChouka->getChildByName("coin"));
	m_ui.layerTakeCard.pCoinCardOrder1 = dynamic_cast<Text*>(m_ui.layerTakeCard.pCoinPanel->getChildByName("cardCount1"));
	m_ui.layerTakeCard.pCoinCardOrder10 = dynamic_cast<Text*>(m_ui.layerTakeCard.pCoinPanel->getChildByName("cardCount10"));
	m_ui.layerTakeCard.pCoinCardOrder1->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.pCoinCardOrder10->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.pCoinYibi1 = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pCoinPanel->getChildByName("img_yinbi1"));
	m_ui.layerTakeCard.pCoinYibi10 = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pCoinPanel->getChildByName("img_yinbi2"));
	m_ui.layerTakeCard.pYinbiyici = dynamic_cast<Text*>(m_ui.layerTakeCard.pCoinYibi1->getChildByName("yinbiyici"));
	m_ui.layerTakeCard.pYinbishici = dynamic_cast<Text*>(m_ui.layerTakeCard.pCoinYibi10->getChildByName("yinbishici"));
	m_ui.layerTakeCard.pYinbiyici->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.pYinbishici->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.pYinbiyici->setString(TO_STR(COIN_ONE_SPEND));
	m_ui.layerTakeCard.pYinbishici->setString(TO_STR(COIN_TEN_SPEND));
	setTextAddStroke(m_ui.layerTakeCard.pYinbiyici,Color3B(0x00,0x00,0x00),2);
	setTextAddStroke(m_ui.layerTakeCard.pYinbishici,Color3B(0x00,0x00,0x00),2);

	m_ui.layerTakeCard.pGoldPanel = dynamic_cast<Layout*>(m_ui.layerTakeCard.PnlChouka->getChildByName("gold"));
	m_ui.layerTakeCard.pGoldCardOrder1 = dynamic_cast<Text*>(m_ui.layerTakeCard.pGoldPanel->getChildByName("cardCount1"));
	m_ui.layerTakeCard.pGoldCardOrder10 = dynamic_cast<Text*>(m_ui.layerTakeCard.pGoldPanel->getChildByName("cardCount10"));
	m_ui.layerTakeCard.pGoldCardOrder1->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.pGoldCardOrder10->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.pGoldYibi1 = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pGoldPanel->getChildByName("img_jinbi1"));
	m_ui.layerTakeCard.pGoldYibi10 = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pGoldPanel->getChildByName("img_jinbi2"));
	m_ui.layerTakeCard.pJinbiyici = dynamic_cast<Text*>(m_ui.layerTakeCard.pGoldYibi1->getChildByName("jinbiyici"));
	m_ui.layerTakeCard.pJinbishici = dynamic_cast<Text*>(m_ui.layerTakeCard.pGoldYibi10->getChildByName("jinbishici"));
	m_ui.layerTakeCard.pJinbiyici->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.pJinbishici->setFontName(FONT_FZZHENGHEI);
	m_ui.layerTakeCard.pJinbiyici->setString(TO_STR(GOLD_ONE_SPEND));
	m_ui.layerTakeCard.pJinbishici->setString(TO_STR(GOLD_TEN_SPEND));
	setTextAddStroke(m_ui.layerTakeCard.pJinbiyici,Color3B(0x00,0x00,0x00),2);
	setTextAddStroke(m_ui.layerTakeCard.pJinbishici,Color3B(0x00,0x00,0x00),2);
	m_ui.layerTakeCard.imgNowFree = dynamic_cast<ImageView*>(m_ui.layerTakeCard.PnlChouka->getChildByName("imgNowFree"));

	//单张卡牌
	m_ui.layerTakeCard.pLayHeroCardItemOne = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.PnlChouka,"imgCard"));
	m_ui.layerTakeCard.pLayHeroCardItemOne->setTouchEnabled(false);
	
	ImageView* pHeroCardItem = m_ui.layerTakeCard.pLayHeroCardItemOne;
	pHeroCardItem->setVisible(false);
	ImageView* pImgBg = ImageView::create("Image/Icon/global/img_kapai_beimian.png");
	pImgBg->setPosition(cocos2d::Vec2(pHeroCardItem->getContentSize().width/2, pHeroCardItem->getContentSize().height/2));
	pImgBg->setTag(TAG_TAKECARD_BG);
	pHeroCardItem->addChild(pImgBg);

	ImageView* pImgFront = ImageView::create();
	pImgFront->setTag(TAG_TAKECARD_FRONT);
	pHeroCardItem->addChild(pImgFront);

	//十张卡
	for (int i = 0; i < TEN_SPEND; i++)
	{
		string cardName = "img_"+TO_STR(i+1);
		m_ui.layerTakeCard.pLayHeroCardItem[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.PnlChouka,cardName));

		ImageView* pHeroCardItem = m_ui.layerTakeCard.pLayHeroCardItem[i];
		ImageView* pImgBg = ImageView::create("Image/Icon/global/img_kapai_beimian.png");
		pImgBg->setPosition(cocos2d::Vec2(pHeroCardItem->getContentSize().width/2, pHeroCardItem->getContentSize().height/2));
		pImgBg->setTag(TAG_TAKECARD_BG);
		pHeroCardItem->addChild(pImgBg);

		ImageView* pImgFront = ImageView::create();
		pImgFront->setTag(TAG_TAKECARD_FRONT);
		pHeroCardItem->addChild(pImgFront);
	}
	
	m_ui.layerTakeCard.pBuyView = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.layerTakeCard.PnlChouka,"buyView"));
	m_ui.layerTakeCard.pImgNeedCount = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pBuyView,"Img_ci1"));
	m_ui.layerTakeCard.pImgTipsChip1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pBuyView,"Img_tips1"));
	m_ui.layerTakeCard.pImgTipsHero1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pBuyView,"Img_tips2"));
	m_ui.layerTakeCard.Image_xiao = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pBuyView,"Image_xiao"));
	m_ui.layerTakeCard.Image_zhong = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pBuyView,"Image_zhong"));

	m_ui.layerTakeCard.pBuyBichu = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.layerTakeCard.PnlChouka,"BuyBichu"));
	m_ui.layerTakeCard.pImgTipsChip2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pBuyBichu,"Img_tips1"));
	m_ui.layerTakeCard.pImgTipsHero2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pBuyBichu,"Img_tips2"));
	Text *buyBichuTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.layerTakeCard.pBuyBichu,"Lab_tips"));
	buyBichuTips->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(buyBichuTips,Color3B(0x82,0x2b,0x2b),2);
}

void LayerTakeCard::onEnter()
{
	Layer::onEnter();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(LayerTakeCard::updateNextFram), this, 0, false);
}

void LayerTakeCard::updateNextFram(float ft)
{
	//拉取商城数据
	TakeCardController::getInstance()->onGetCardInfo();
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(LayerTakeCard::updateNextFram), this);
}

void LayerTakeCard::onExit()
{
	CCLOG("LayerTakeCard onExit");
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UImall");
	Layer::onExit();
}

void LayerTakeCard::onGetTakeCardInfo(const TAKE_CARD_INFO& cardInfos)
{
	if (TakeCardModel::getInstance()->getcoinCardSeconds() == 0 && cardInfos.coinItem.CurCount > 0)
	{
		m_isFreeTime = true;			
	}
	else
	{
		m_isFreeTime = false;
	}

	if (TakeCardModel::getInstance()->getgoldCardSeconds() == 0)
	{
		m_isFreeTime2 = true;			
	}
	else
	{
		m_isFreeTime2 = false;
	}
	setTakeCardInfo();
	//第几次必得5星
	if (m_nTakeCardType == TakeCardTypeCoin)
	{
		setRewardText(cardInfos.goldItem.times, TakeCardTypeGold);
		setRewardText(cardInfos.coinItem.times, TakeCardTypeCoin);
	}
	else if (m_nTakeCardType == TakeCardTypeGold)
	{
		setRewardText(cardInfos.coinItem.times, TakeCardTypeCoin);
		setRewardText(cardInfos.goldItem.times, TakeCardTypeGold);
	}
}

void LayerTakeCard::onTakeCardResult()
{
	m_state = TakeCardState_show;
	m_ui.layerTakeCard.pLayHeroCardItemOne->setVisible(false);
	for (int i = 0; i < TEN_SPEND; i++)
	{		
		m_ui.layerTakeCard.pLayHeroCardItem[i]->setVisible(false);
	}
	vector<NEW_CARD_INFO>& vecHeros= TakeCardModel::getInstance()->gettakeCardResulHeros();
	//动画显示出现卡牌背景
	startTakeCardResultAnimation(vecHeros.size());
	//动画显示英雄
	m_ui.layerTakeCard.pLayResult->setVisible(true);

	m_ui.layerTakeCard.pLayTakeCard->setVisible(false);
	//第几次必得5星
	const TAKE_CARD_INFO& _cardInfo = TakeCardModel::getInstance()->getTakeCardInfo();

	if (m_nTakeCardType == TakeCardTypeCoin)
	{		
		if (_cardInfo.coinItem.CurCount <= 0)
		{
			m_isFreeTime = false;
		}
		else
		{
			if (TakeCardModel::getInstance()->getcoinCardSeconds() == 0)
			{
				m_isFreeTime = true;
			}
			else
			{				
				m_isFreeTime = false;
			}		
		}
		setRewardText(_cardInfo.coinItem.times, m_nTakeCardType);
	}
	else if (m_nTakeCardType == TakeCardTypeGold)
	{
		if (TakeCardModel::getInstance()->getgoldCardSeconds() == 0)
		{
			m_isFreeTime2 = true;
		}
		else
		{				
			m_isFreeTime2 = false;
		}
		setRewardText(_cardInfo.goldItem.times, m_nTakeCardType);

		if (m_mTakeCardTypeWhat == TakeCardType_Spend)
		{
			vector<NEW_CARD_INFO>& vecHeros= TakeCardModel::getInstance()->gettakeCardResulHeros();
			if (vecHeros.size() == ONE_SPEND)//一个
			{
				TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10248),1,_cardInfo.goldItem.onecost);
			}
			else if(vecHeros.size() == TEN_SPEND)
			{
				TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10249),1,_cardInfo.goldItem.tencost);
			}
		}
	}
	setTakeCardInfo();
}

void LayerTakeCard::onTakeCardClicked(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int index)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		//点击抽卡中的整张卡牌均为按钮，点击后，卡牌有旋转效果，旋转到背面如图。旋转后，可点击除按钮部分，再次旋转回来
		animationCardRotation(index);	
	}
}

//
void LayerTakeCard::onBtnTakeCard1(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int times)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (m_bTakeCardResultAnimationIsRun)
		{
			return;
		}
		m_nTakeCardType = TakeCardTypeCoin;
		//是：	判断玩家是否背包有≥1个空位
		//	有：进入抽卡动画（后文详解）
		//	没有：弹出提示：你背包满了，整理下再来吧
		if (!ToolModel::getInstance()->backpackIsUsable())
		{
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20124));
			return;
		}

		takeCardCoin(times);
	}
}

void LayerTakeCard::onBtnTakeCard2(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int times)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (m_bTakeCardResultAnimationIsRun)
		{
			return;
		}
		m_nTakeCardType = TakeCardTypeGold;
		/*玩家是否有免费抽取的机会							
			有：进入抽卡选择界面（下文详解）						
			无：	判断玩家拥有≥280元宝					
			是：	判断玩家是否背包有≥1个空位				
			有：进入抽卡选择界面（下文详解）			
			没有：弹出提示：你背包满了，整理下再来吧			

			*/	
		if (!ToolModel::getInstance()->backpackIsUsable())
		{
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20124));
			return;
		}
	
		takeCardGold(times);
	}
}

void LayerTakeCard::onBtnTakeCardView(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (sender == m_ui.layerTakeCard.DiBtnPreview)
		{
			LayerTakeCardView* _layer = LayerTakeCardView::create(CardViewType_Coin);
			if (nullptr != _layer)
			{
				Director::getInstance()->getRunningScene()->addChild(_layer);
			}
		}
		else if (sender == m_ui.layerTakeCard.TianBtnPreview)
		{
			LayerTakeCardView* _layer = LayerTakeCardView::create(CardViewType_Gold);
			if (nullptr != _layer)
			{
				Director::getInstance()->getRunningScene()->addChild(_layer);
			}
		}
	}
}

void LayerTakeCard::takeCardCoin(int times)
{
	RowData* coinData = DataManager::getInstance()->getDTToolSystem()->searchDataById(COIN_SYSTEM_ID);
	int coinCardOrderCount = 0;
	if (ToolModel::getInstance()->GetToolInfoBytemplateId(coinData->getIntData("value")))
	{
		coinCardOrderCount = ToolModel::getInstance()->GetToolInfoBytemplateId(coinData->getIntData("value"))->nstack;
	}

	MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
	if (times <= ONE_SPEND)
	{
		if (m_isFreeTime)//免费抽取
		{
			m_mTakeCardTypeWhat = TakeCardType_Free;
		}
		else if (coinCardOrderCount >= ONE_SPEND)
		{
			m_mTakeCardTypeWhat = TakeCardType_Card;
		}
		else if (pMainParam->mCoin < TakeCardModel::getInstance()->getTakeCardInfo().coinItem.onecost)//玩家拥有＜280元宝则弹出充值界面（统一的余额不足充值界面）
		{
			LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
			Director::getInstance()->getRunningScene()->addChild(buyEnergy);
			return;
		}
	}
	else
	{
		if (coinCardOrderCount >= TEN_SPEND)
		{
			m_mTakeCardTypeWhat = TakeCardType_Card;
		}
		else if (pMainParam->mCoin < TakeCardModel::getInstance()->getTakeCardInfo().coinItem.tencost)//玩家拥有＜280元宝则弹出充值界面（统一的余额不足充值界面）
		{
			LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
			Director::getInstance()->getRunningScene()->addChild(buyEnergy);
			
			//充值界面
			return;
		}
	}
	m_mTakeCardTypeWhat = TakeCardType_Spend;
	TakeCardController::getInstance()->onTakeCard(m_nTakeCardType,times);
}

void LayerTakeCard::takeCardGold(int times)
{
	//金币抽取
	RowData* goldData = DataManager::getInstance()->getDTToolSystem()->searchDataById(GOLG_SYSTEM_ID);
	int goldCardOrderCount = 0;
	if (ToolModel::getInstance()->GetToolInfoBytemplateId(goldData->getIntData("value")))
	{
		goldCardOrderCount = ToolModel::getInstance()->GetToolInfoBytemplateId(goldData->getIntData("value"))->nstack;
	}

	MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
	if (times <= ONE_SPEND)
	{
		if (m_isFreeTime2)//免费抽取
		{
			m_mTakeCardTypeWhat = TakeCardType_Free;
		}
		else if (goldCardOrderCount >= ONE_SPEND)
		{
			m_mTakeCardTypeWhat = TakeCardType_Card;
		}
		else if (pMainParam->mGold < TakeCardModel::getInstance()->getTakeCardInfo().goldItem.onecost)//玩家拥有＜280元宝则弹出充值界面（统一的余额不足充值界面）
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
			}
			//充值界面
			return;
		}
	}
	else
	{
		if (goldCardOrderCount >= TEN_SPEND)
		{
			m_mTakeCardTypeWhat = TakeCardType_Card;
		}
		else if (pMainParam->mGold < TakeCardModel::getInstance()->getTakeCardInfo().goldItem.tencost)//玩家拥有＜280元宝则弹出充值界面（统一的余额不足充值界面）
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
			}
			//充值界面
			return;
		}
	}

	m_mTakeCardTypeWhat = TakeCardType_Spend;
	TakeCardController::getInstance()->onTakeCard(m_nTakeCardType,times);	
}

void LayerTakeCard::onBtnAgain(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int times)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (m_bTakeCardResultAnimationIsRun)
		{
			return;
		}
		//玩家点击再抽一次，则重复他选择的颜色卡牌抽取方式再次播放动画抽取，如果玩家未达到抽取条件，则做出相应金币不足的提示并转入充值界面
		const TakeCardParam& takeCardParam = TakeCardController::getInstance()->getTakeCardParam();
		if (takeCardParam.buyType == TakeCardTypeCoin)//蓝色抽卡， 使用道具
		{
			takeCardCoin(times);
		}
		else if (takeCardParam.buyType == TakeCardTypeGold)//紫色抽卡使用金币
		{
			takeCardGold(times);
		}	
	}
}

void LayerTakeCard::onBtnTakeCardBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (m_bTakeCardResultAnimationIsRun)
		{
			return;
		}	

		m_ui.layerTakeCard.pLayResult->setVisible(false);
		m_ui.layerTakeCard.pLayTakeCard->setVisible(true);

		m_ui.layerTakeCard.pLayTakeCardFront1->setVisible(true);
		m_ui.layerTakeCard.pLayTakeCardOther1->setVisible(false);
		m_ui.layerTakeCard.pLayTakeCardFront2->setVisible(true);
		m_ui.layerTakeCard.pLayTakeCardOther2->setVisible(false);
	}
}

void LayerTakeCard::setTakeCardInfo()
{
	if (!TakeCardModel::getInstance()->getFirstGet())
	{
		m_ui.layerTakeCard.firstGetCard->setVisible(true);
	}
	else
	{
		m_ui.layerTakeCard.firstGetCard->setVisible(false);
	}
	
	const TAKE_CARD_INFO &takeCardInfo = TakeCardModel::getInstance()->getTakeCardInfo();
	RowData* coinData = DataManager::getInstance()->getDTToolSystem()->searchDataById(COIN_SYSTEM_ID);
	RowData* goldData = DataManager::getInstance()->getDTToolSystem()->searchDataById(GOLG_SYSTEM_ID);

	int coinCardOrderCount = 0;
	int goldCardOrderCount = 0;
	if (ToolModel::getInstance()->GetToolInfoBytemplateId(coinData->getIntData("value")))
	{
		coinCardOrderCount = ToolModel::getInstance()->GetToolInfoBytemplateId(coinData->getIntData("value"))->nstack;
	}
	if (ToolModel::getInstance()->GetToolInfoBytemplateId(goldData->getIntData("value")))
	{
		goldCardOrderCount = ToolModel::getInstance()->GetToolInfoBytemplateId(goldData->getIntData("value"))->nstack;
	}
	

	if (m_isFreeTime) //银币免费
	{
		//正面
		m_ui.layerTakeCard.pDiShaZhengImgDaoJiShi->setVisible(false);
		m_ui.layerTakeCard.pDiShaZhengImgDelCount->setVisible(true);
		m_ui.layerTakeCard.pImgDiShaZhengYinbi->setVisible(true);
		m_ui.layerTakeCard.pImgDiShaZhengOrderCard->setVisible(false);
		m_ui.layerTakeCard.pDiShaZhengCommomCount->setString(DataManager::getInstance()->searchCommonTexdtById(20125));
		

		//反面上部
		m_ui.layerTakeCard.DiImgCostYiciYinbi->setVisible(true); //一次银币
		m_ui.layerTakeCard.DiImgCostYiciOrderCard->setVisible(false); //一次抽卡令
		m_ui.layerTakeCard.DiImgCostYiciCommomCount->setString(DataManager::getInstance()->searchCommonTexdtById(20125)); //通用个数

		//反面下部
		if (coinCardOrderCount >= TEN_SPEND)
		{
			m_ui.layerTakeCard.DiImgCostShiciYinbi->setVisible(false); //一次银币
			m_ui.layerTakeCard.DiImgCostShiciOrderCard->setVisible(true); //一次抽卡令
			m_ui.layerTakeCard.DiImgCostShiciCommomCount->setString("(" + TO_STR(coinCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")"); //通用个数
		}
		else
		{
			m_ui.layerTakeCard.DiImgCostShiciYinbi->setVisible(true); //一次银币
			m_ui.layerTakeCard.DiImgCostShiciOrderCard->setVisible(false); //一次抽卡令
			m_ui.layerTakeCard.DiImgCostShiciCommomCount->setString(TO_STR(COIN_TEN_SPEND)); //通用个数
		}
	}
	else
	{
		//剩余免费次数
		if (takeCardInfo.coinItem.CurCount > 0)
		{
			//正面
			m_ui.layerTakeCard.pDiShaZhengImgDaoJiShi->setVisible(true);
			m_ui.layerTakeCard.pDiShaZhengImgDelCount->setVisible(false);
		}
		else
		{
			m_ui.layerTakeCard.pDiShaZhengImgDaoJiShi->setVisible(false);
			m_ui.layerTakeCard.pDiShaZhengImgDelCount->setVisible(true);
		}

		if (coinCardOrderCount > 0)
		{
			//正面
			m_ui.layerTakeCard.pImgDiShaZhengYinbi->setVisible(false); //地煞正面银币
			m_ui.layerTakeCard.pImgDiShaZhengOrderCard->setVisible(true); //地煞正面抽卡令
			m_ui.layerTakeCard.pDiShaZhengCommomCount->setString("x " + TO_STR(coinCardOrderCount));

			//反面上
			m_ui.layerTakeCard.DiImgCostYiciYinbi->setVisible(false); //一次银币
			m_ui.layerTakeCard.DiImgCostYiciOrderCard->setVisible(true); //一次抽卡令
			m_ui.layerTakeCard.DiImgCostYiciCommomCount->setString("(" + TO_STR(coinCardOrderCount) + "/" + TO_STR(ONE_SPEND) + ")"); //通用个数

			//反面下
			if (coinCardOrderCount >= TEN_SPEND)
			{
				m_ui.layerTakeCard.DiImgCostShiciYinbi->setVisible(false); //一次银币
				m_ui.layerTakeCard.DiImgCostShiciOrderCard->setVisible(true); //一次抽卡令
				m_ui.layerTakeCard.DiImgCostShiciCommomCount->setString("(" + TO_STR(coinCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")");
			}
			else
			{
				m_ui.layerTakeCard.DiImgCostShiciYinbi->setVisible(true); //一次银币
				m_ui.layerTakeCard.DiImgCostShiciOrderCard->setVisible(false); //一次抽卡令
				m_ui.layerTakeCard.DiImgCostShiciCommomCount->setString(TO_STR(COIN_TEN_SPEND));
			}
		}
		else
		{
			//正面
			m_ui.layerTakeCard.pImgDiShaZhengYinbi->setVisible(true); //地煞正面银币
			m_ui.layerTakeCard.pImgDiShaZhengOrderCard->setVisible(false); //地煞正面抽卡令
			m_ui.layerTakeCard.pDiShaZhengCommomCount->setString(TO_STR(COIN_ONE_SPEND));

			//反面上
			m_ui.layerTakeCard.DiImgCostYiciYinbi->setVisible(true); //一次银币
			m_ui.layerTakeCard.DiImgCostYiciOrderCard->setVisible(false); //一次抽卡令
			m_ui.layerTakeCard.DiImgCostYiciCommomCount->setString(TO_STR(COIN_ONE_SPEND)); //通用个数

			//反面下
			m_ui.layerTakeCard.DiImgCostShiciYinbi->setVisible(true); //一次银币
			m_ui.layerTakeCard.DiImgCostShiciOrderCard->setVisible(false); //一次抽卡令
			m_ui.layerTakeCard.DiImgCostShiciCommomCount->setString(TO_STR(COIN_TEN_SPEND));
		}
	}
	//
	if (m_isFreeTime2) //金币免费
	{
		//正面
		m_ui.layerTakeCard.pTianGangZhengImgDaoJiShi->setVisible(false);
		m_ui.layerTakeCard.pTianGangZhengImgDelCount->setVisible(true);
		m_ui.layerTakeCard.pTianGangZhengDelCount->setString("(" + TO_STR(ONE_SPEND) + "/" + TO_STR(ONE_SPEND) + ")");

		m_ui.layerTakeCard.pTianGangZhengGold->setVisible(true);
		m_ui.layerTakeCard.pTianGangZhengOrderCard->setVisible(false);
		m_ui.layerTakeCard.pTianGangZhengCommomCount->setString(DataManager::getInstance()->searchCommonTexdtById(20125));

		//反面上部
		m_ui.layerTakeCard.TianImgCostYiciGold->setVisible(true); //一次金币
		m_ui.layerTakeCard.TianImgCostYiciOrderCard->setVisible(false); //一次抽卡令
		m_ui.layerTakeCard.TianImgCostYiciCommomCount->setString(DataManager::getInstance()->searchCommonTexdtById(20125)); //通用个数

		//反面下部
		if (goldCardOrderCount >= TEN_SPEND)
		{
			m_ui.layerTakeCard.TianImgCostShiciGold->setVisible(false); 
			m_ui.layerTakeCard.TianImgCostShiciOrderCard->setVisible(true); //一次抽卡令
			m_ui.layerTakeCard.TianImgCostShiciCommomCount->setString("(" + TO_STR(goldCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")"); //通用个数
		}
		else
		{
			m_ui.layerTakeCard.TianImgCostShiciGold->setVisible(true);
			m_ui.layerTakeCard.TianImgCostShiciOrderCard->setVisible(false); //抽卡令
			m_ui.layerTakeCard.TianImgCostShiciCommomCount->setString(TO_STR(GOLD_TEN_SPEND)); //通用个数
		}
	}
	else
	{
		//正面
		m_ui.layerTakeCard.pTianGangZhengImgDaoJiShi->setVisible(true);
		m_ui.layerTakeCard.pTianGangZhengImgDelCount->setVisible(false);

		if (goldCardOrderCount > 0)
		{
			//正面
			m_ui.layerTakeCard.pTianGangZhengGold->setVisible(false); 
			m_ui.layerTakeCard.pTianGangZhengOrderCard->setVisible(true); 
			m_ui.layerTakeCard.pTianGangZhengCommomCount->setString("x " + TO_STR(goldCardOrderCount));

			//反面上
			m_ui.layerTakeCard.TianImgCostYiciGold->setVisible(false);
			m_ui.layerTakeCard.TianImgCostYiciOrderCard->setVisible(true);
			m_ui.layerTakeCard.TianImgCostYiciCommomCount->setString("(" + TO_STR(goldCardOrderCount) + "/" + TO_STR(ONE_SPEND) + ")"); //通用个数

			//反面下
			if (goldCardOrderCount >= TEN_SPEND)
			{
				m_ui.layerTakeCard.TianImgCostShiciGold->setVisible(false);
				m_ui.layerTakeCard.TianImgCostShiciOrderCard->setVisible(true);
				m_ui.layerTakeCard.TianImgCostShiciCommomCount->setString("(" + TO_STR(goldCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")");
			}
			else
			{
				m_ui.layerTakeCard.TianImgCostShiciGold->setVisible(true);
				m_ui.layerTakeCard.TianImgCostShiciOrderCard->setVisible(false);
				m_ui.layerTakeCard.TianImgCostShiciCommomCount->setString(TO_STR(GOLD_TEN_SPEND));
			}
		}
		else
		{
			//正面
			m_ui.layerTakeCard.pTianGangZhengGold->setVisible(true);
			m_ui.layerTakeCard.pTianGangZhengOrderCard->setVisible(false); //正面抽卡令
			m_ui.layerTakeCard.pTianGangZhengCommomCount->setString(TO_STR(GOLD_ONE_SPEND));

			//反面上
			m_ui.layerTakeCard.TianImgCostYiciGold->setVisible(true); //一次银币
			m_ui.layerTakeCard.TianImgCostYiciOrderCard->setVisible(false); //一次抽卡令
			m_ui.layerTakeCard.TianImgCostYiciCommomCount->setString(TO_STR(GOLD_ONE_SPEND)); //通用个数

			//反面下
			m_ui.layerTakeCard.TianImgCostShiciGold->setVisible(true); //一次银币
			m_ui.layerTakeCard.TianImgCostShiciOrderCard->setVisible(false); //一次抽卡令
			m_ui.layerTakeCard.TianImgCostShiciCommomCount->setString(TO_STR(GOLD_TEN_SPEND));
		}
	}
}

void LayerTakeCard::showResultCardsItem(cocos2d::ui::ImageView* pItem, const NEW_CARD_INFO& info,bool isChip)
{
	if (isChip && info.countChips > 0)
	{
		RowData* pRowData = DataManager::getInstance()->searchToolById(info.id);

		if (pRowData != nullptr)
		{
			ImageView* pImgFront = dynamic_cast<ImageView*>(pItem->getChildByTag(TAG_TAKECARD_FRONT));

			UICardPrize *cardPrize = UICardPrize::create(pRowData->getIntData("transNeed"));
			if (pImgFront->getChildByName("UICardPrize"))
			{
				pImgFront->removeChildByName("UICardPrize");
			}
			pImgFront->addChild(cardPrize,1,"UICardPrize");
			cardPrize->setCardCount(info.countChips);
			cardPrize->setCardName(pRowData->getStringData("itemName"));
		}
	}
	else
	{
		RowData* pRowData = DataManager::getInstance()->searchToolById(info.id);
		if (pRowData != nullptr)
		{
			ImageView* pImgFront = dynamic_cast<ImageView*>(pItem->getChildByTag(TAG_TAKECARD_FRONT));

			UICardPrize *cardPrize = UICardPrize::create(info.id);
			if (pImgFront->getChildByName("UICardPrize"))
			{
				pImgFront->removeChildByName("UICardPrize");
			}

			pImgFront->addChild(cardPrize,1,"UICardPrize");
			cardPrize->setCardCount(info.amount);
			cardPrize->setCardName(pRowData->getStringData("itemName"));
		}
	}
}

void LayerTakeCard::showResultCards()
{
	vector<NEW_CARD_INFO>& vecHeros= TakeCardModel::getInstance()->gettakeCardResulHeros();
	if (vecHeros.size() == 1)//一个
	{
		m_ui.layerTakeCard.pLayHeroCardItemOne->setVisible(true);
		for (int i = 0; i < TEN_SPEND; i++)
		{
			m_ui.layerTakeCard.pLayHeroCardItem[i]->setVisible(false);
		}

		showResultCardsItem(m_ui.layerTakeCard.pLayHeroCardItemOne, *vecHeros.begin());//
		
		ImageView* pImgBg = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pLayHeroCardItemOne->getChildByTag(TAG_TAKECARD_BG));
		ImageView* pImgFront = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pLayHeroCardItemOne->getChildByTag(TAG_TAKECARD_FRONT));
		pImgBg->setVisible(true);
		pImgFront->setVisible(false);

		m_isChangeChip = false;
		if (((NEW_CARD_INFO)*vecHeros.begin()).countChips > 0)
		{
			m_nTakeCardResultAnimationTime = secondNow();
			m_isChangeChip = true;
			m_ChangChipCount = ((NEW_CARD_INFO)*vecHeros.begin()).countChips;
		}

		int isShow = 0;
		m_CurShowHeroId = 0;
		RowData* pRowData = DataManager::getInstance()->searchToolById(vecHeros.at(0).id);
		if (pRowData != nullptr)
		{
			if (pRowData->getIntData("itemTyp") == TOOL_ITEM_TYPE_HERO )  //-英雄
			{
				//需要展示
				isShow = 1;
				m_CurShowHeroId = vecHeros.at(0).id;
			}			
		}

		animationShowCard(m_ui.layerTakeCard.pLayHeroCardItemOne,ONE_CARD_SCALE,isShow);
		
		return;
	}
	//10个
	int index = 0;
	m_ui.layerTakeCard.pLayHeroCardItemOne->setVisible(false);
	for (int i = 0; i < TEN_SPEND; i++)
	{		
		m_ui.layerTakeCard.pLayHeroCardItem[i]->setVisible(true);
	}
	for (auto iter = vecHeros.begin(); iter != vecHeros.end(); iter++)
	{
		//展示英雄卡牌
		showResultCardsItem(m_ui.layerTakeCard.pLayHeroCardItem[index], *iter);

		ImageView* pImgBg = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pLayHeroCardItem[index]->getChildByTag(TAG_TAKECARD_BG));
		ImageView* pImgFront = dynamic_cast<ImageView*>(m_ui.layerTakeCard.pLayHeroCardItem[index]->getChildByTag(TAG_TAKECARD_FRONT));
		pImgBg->setVisible(true);
		pImgFront->setVisible(false);

		index ++;
		if (index >= TEN_SPEND)
		{
			break;
		}		
	}
	//
	m_isChangeChip = false;
	if (((NEW_CARD_INFO)*vecHeros.begin()).countChips > 0)
	{
		m_nTakeCardResultAnimationTime = secondNow();
		m_isChangeChip = true;
		m_ChangChipCount = ((NEW_CARD_INFO)*vecHeros.begin()).countChips;
	}

	int isshow = 0;
	m_CurShowHeroId = 0;
	m_nTakeCardResultAnimationIndex = 0;
	RowData* pRowData = DataManager::getInstance()->searchToolById(vecHeros.at(m_nTakeCardResultAnimationIndex).id);
	if (pRowData != nullptr)
	{
		if (pRowData->getIntData("itemTyp") == TOOL_ITEM_TYPE_HERO)  //4-英雄
		{
			//旋转移动到中间放大到正常 翻卡
			isshow = 1;
			m_CurShowHeroId = vecHeros.at(m_nTakeCardResultAnimationIndex).id;
		}			
	}
	//
	animationShowCard(m_ui.layerTakeCard.pLayHeroCardItem[0], TEN_CARD_SCALE, isshow);
}

void LayerTakeCard::setRewardText(const int& amount, const TakeCardType& index)
{
	const TAKE_CARD_INFO  &TakeCardInfo = TakeCardModel::getInstance()->getTakeCardInfo();
	RowData* coinData = DataManager::getInstance()->getDTToolSystem()->searchDataById(COIN_SYSTEM_ID);
	RowData* goldData = DataManager::getInstance()->getDTToolSystem()->searchDataById(GOLG_SYSTEM_ID);

	int coinCardOrderCount = 0;
	int goldCardOrderCount = 0;
	if (ToolModel::getInstance()->GetToolInfoBytemplateId(coinData->getIntData("value")))
	{
		coinCardOrderCount = ToolModel::getInstance()->GetToolInfoBytemplateId(coinData->getIntData("value"))->nstack;
	}
	if (ToolModel::getInstance()->GetToolInfoBytemplateId(goldData->getIntData("value")))
	{
		goldCardOrderCount = ToolModel::getInstance()->GetToolInfoBytemplateId(goldData->getIntData("value"))->nstack;
	}


	if (amount > 1)
	{
		//再开N次必得英雄碎片
		m_ui.layerTakeCard.pBuyView->setVisible(true);
		m_ui.layerTakeCard.pBuyBichu->setVisible(false);
		if (index == 1 )
		{
			m_ui.layerTakeCard.pGoldPanel->setVisible(false);

			if (m_isFreeTime)
			{
				m_ui.layerTakeCard.imgNowFree->setVisible(true);
				m_ui.layerTakeCard.pCoinPanel->setVisible(true);

				m_ui.layerTakeCard.pCoinYibi1->setVisible(false);
				m_ui.layerTakeCard.pCoinCardOrder1->setVisible(false);

				if (coinCardOrderCount >= TEN_SPEND)
				{
					m_ui.layerTakeCard.pCoinYibi10->setVisible(false);
					m_ui.layerTakeCard.pCoinCardOrder10->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder10->setString("(" + TO_STR(coinCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")");
					setTextAddStroke(m_ui.layerTakeCard.pCoinCardOrder10,Color3B(0x00,0x00,0x00),3);
				}
				else
				{
					m_ui.layerTakeCard.pCoinYibi10->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder10->setVisible(false);
				}
			}
			else
			{
				m_ui.layerTakeCard.imgNowFree->setVisible(false);
				m_ui.layerTakeCard.pCoinPanel->setVisible(true);
				if (coinCardOrderCount > 0)
				{
					m_ui.layerTakeCard.pCoinYibi1->setVisible(false);
					m_ui.layerTakeCard.pCoinCardOrder1->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder1->setString("(" + TO_STR(coinCardOrderCount) + "/" + TO_STR(ONE_SPEND) + ")");
					setTextAddStroke(m_ui.layerTakeCard.pCoinCardOrder1,Color3B(0x00,0x00,0x00),3);
				}
				else
				{
					m_ui.layerTakeCard.pCoinYibi1->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder1->setVisible(false);
				}

				if (coinCardOrderCount >= TEN_SPEND)
				{
					m_ui.layerTakeCard.pCoinYibi10->setVisible(false);
					m_ui.layerTakeCard.pCoinCardOrder10->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder10->setString("(" + TO_STR(coinCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")");
					setTextAddStroke(m_ui.layerTakeCard.pCoinCardOrder10,Color3B(0x00,0x00,0x00),3);
				}
				else
				{
					m_ui.layerTakeCard.pCoinYibi10->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder10->setVisible(false);
				}
			}

			m_ui.layerTakeCard.pTextTakeCardIntro1Panel->setVisible(true);
			m_ui.layerTakeCard.pTextTakeCardIntro1Bichu->setVisible(false);
			m_ui.layerTakeCard.pTextTakeCardIntro1->loadTexture("Image/Icon/global/img_cishu1_"+ TO_STR(amount-1) +".png");
			m_ui.layerTakeCard.pDiShaZhengDelCount->setText("("+TO_STR(TakeCardInfo.coinItem.CurCount)+"/"+TO_STR(TakeCardInfo.coinItem.limitCount)+")");
			
			m_ui.layerTakeCard.pImgNeedCount->loadTexture("Image/Icon/global/img_cishu2_"+ TO_STR(amount-1) +".png");
			m_ui.layerTakeCard.pImgTipsChip1->setVisible(true);
			m_ui.layerTakeCard.pImgTipsHero1->setVisible(false);
			m_ui.layerTakeCard.Image_xiao->setVisible(true);
			m_ui.layerTakeCard.Image_zhong->setVisible(false);
		}
		else
		{
			m_ui.layerTakeCard.pCoinPanel->setVisible(false);
			if (m_isFreeTime2)
			{
				m_ui.layerTakeCard.imgNowFree->setVisible(true);
				m_ui.layerTakeCard.pGoldPanel->setVisible(true);

				m_ui.layerTakeCard.pGoldCardOrder1->setVisible(false);
				m_ui.layerTakeCard.pGoldYibi1->setVisible(false);

				if (goldCardOrderCount >= TEN_SPEND)
				{
					m_ui.layerTakeCard.pGoldCardOrder10->setVisible(true);
					m_ui.layerTakeCard.pGoldCardOrder10->setString("(" + TO_STR(goldCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")");
					m_ui.layerTakeCard.pGoldYibi10->setVisible(false);
				}
				else
				{
					m_ui.layerTakeCard.pGoldCardOrder10->setVisible(false);
					m_ui.layerTakeCard.pGoldYibi10->setVisible(true);
				}
			}
			else
			{
				m_ui.layerTakeCard.imgNowFree->setVisible(false);
				m_ui.layerTakeCard.pGoldPanel->setVisible(true);

				if (goldCardOrderCount > 0)
				{
					m_ui.layerTakeCard.pGoldYibi1->setVisible(false);
					m_ui.layerTakeCard.pGoldCardOrder1->setVisible(true);
					m_ui.layerTakeCard.pGoldCardOrder1->setString("(" + TO_STR(goldCardOrderCount) + "/" + TO_STR(ONE_SPEND) + ")");
					setTextAddStroke(m_ui.layerTakeCard.pGoldCardOrder1,Color3B(0x00,0x00,0x00),3);
				}
				else
				{
					m_ui.layerTakeCard.pGoldYibi1->setVisible(true);
					m_ui.layerTakeCard.pGoldCardOrder1->setVisible(false);
				}

				if (goldCardOrderCount >= TEN_SPEND)
				{
					m_ui.layerTakeCard.pGoldYibi10->setVisible(false);
					m_ui.layerTakeCard.pGoldCardOrder10->setVisible(true);
					m_ui.layerTakeCard.pGoldCardOrder10->setString("(" + TO_STR(goldCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")");
					setTextAddStroke(m_ui.layerTakeCard.pGoldCardOrder10,Color3B(0x00,0x00,0x00),3);
				}
				else
				{
					m_ui.layerTakeCard.pGoldYibi10->setVisible(true);
					m_ui.layerTakeCard.pGoldCardOrder10->setVisible(false);
				}
			}

			m_ui.layerTakeCard.pTextTakeCardIntro2Panel->setVisible(true);
			m_ui.layerTakeCard.pTextTakeCardIntro2Bichu->setVisible(false);
			m_ui.layerTakeCard.pTextTakeCardIntro2->loadTexture("Image/Icon/global/img_cishu1_"+ TO_STR(amount-1) +".png");

			m_ui.layerTakeCard.pImgNeedCount->loadTexture("Image/Icon/global/img_cishu2_"+ TO_STR(amount-1) +".png");
			m_ui.layerTakeCard.pImgTipsChip1->setVisible(false);
			m_ui.layerTakeCard.pImgTipsHero1->setVisible(true);
			m_ui.layerTakeCard.Image_xiao->setVisible(false);
			m_ui.layerTakeCard.Image_zhong->setVisible(true);
		}		
	}
	else
	{
		m_ui.layerTakeCard.pBuyView->setVisible(false);
		m_ui.layerTakeCard.pBuyBichu->setVisible(true);
		if (index == 1 )
		{
			m_ui.layerTakeCard.pGoldPanel->setVisible(false);
			if (m_isFreeTime)
			{
				m_ui.layerTakeCard.imgNowFree->setVisible(true);
				m_ui.layerTakeCard.pCoinPanel->setVisible(true);

				m_ui.layerTakeCard.pCoinYibi1->setVisible(false);
				m_ui.layerTakeCard.pCoinCardOrder1->setVisible(false);

				if (coinCardOrderCount >= TEN_SPEND)
				{
					m_ui.layerTakeCard.pCoinYibi10->setVisible(false);
					m_ui.layerTakeCard.pCoinCardOrder10->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder10->setString("(" + TO_STR(coinCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")");
					setTextAddStroke(m_ui.layerTakeCard.pCoinCardOrder10,Color3B(0x00,0x00,0x00),3);
				}
				else
				{
					m_ui.layerTakeCard.pCoinYibi10->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder10->setVisible(false);
				}
			}
			else
			{
				m_ui.layerTakeCard.imgNowFree->setVisible(false);
				m_ui.layerTakeCard.pCoinPanel->setVisible(true);
				if (coinCardOrderCount > 0)
				{
					m_ui.layerTakeCard.pCoinYibi1->setVisible(false);
					m_ui.layerTakeCard.pCoinCardOrder1->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder1->setString("(" + TO_STR(coinCardOrderCount) + "/" + TO_STR(ONE_SPEND) + ")");
					setTextAddStroke(m_ui.layerTakeCard.pCoinCardOrder1,Color3B(0x00,0x00,0x00),3);
				}
				else
				{
					m_ui.layerTakeCard.pCoinYibi1->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder1->setVisible(false);
				}

				if (coinCardOrderCount >= TEN_SPEND)
				{
					m_ui.layerTakeCard.pCoinYibi10->setVisible(false);
					m_ui.layerTakeCard.pCoinCardOrder10->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder10->setString("(" + TO_STR(coinCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")");
					setTextAddStroke(m_ui.layerTakeCard.pCoinCardOrder10,Color3B(0x00,0x00,0x00),3);
				}
				else
				{
					m_ui.layerTakeCard.pCoinYibi10->setVisible(true);
					m_ui.layerTakeCard.pCoinCardOrder10->setVisible(false);
				}
			}

			m_ui.layerTakeCard.pTextTakeCardIntro1Panel->setVisible(false);
			m_ui.layerTakeCard.pTextTakeCardIntro1Bichu->setVisible(true);
			m_ui.layerTakeCard.pDiShaZhengDelCount->setText(TO_STR(TakeCardInfo.coinItem.CurCount)+"/"+TO_STR(TakeCardInfo.coinItem.limitCount));

			m_ui.layerTakeCard.pImgTipsChip2->setVisible(true);
			m_ui.layerTakeCard.pImgTipsHero2->setVisible(false);
			m_ui.layerTakeCard.Image_xiao->setVisible(true);
			m_ui.layerTakeCard.Image_zhong->setVisible(false);
		}
		else
		{
			m_ui.layerTakeCard.pCoinPanel->setVisible(false);
			if (m_isFreeTime2)
			{
				m_ui.layerTakeCard.imgNowFree->setVisible(true);
				m_ui.layerTakeCard.pGoldPanel->setVisible(true);

				m_ui.layerTakeCard.pGoldCardOrder1->setVisible(false);
				m_ui.layerTakeCard.pGoldYibi1->setVisible(false);

				if (goldCardOrderCount >= TEN_SPEND)
				{
					m_ui.layerTakeCard.pGoldCardOrder10->setVisible(true);
					m_ui.layerTakeCard.pGoldCardOrder10->setString("(" + TO_STR(goldCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")");
					m_ui.layerTakeCard.pGoldYibi10->setVisible(false);
				}
				else
				{
					m_ui.layerTakeCard.pGoldCardOrder10->setVisible(false);
					m_ui.layerTakeCard.pGoldYibi10->setVisible(true);
				}
			}
			else
			{
				m_ui.layerTakeCard.imgNowFree->setVisible(false);
				m_ui.layerTakeCard.pGoldPanel->setVisible(true);

				if (goldCardOrderCount > 0)
				{
					m_ui.layerTakeCard.pGoldYibi1->setVisible(false);
					m_ui.layerTakeCard.pGoldCardOrder1->setVisible(true);
					m_ui.layerTakeCard.pGoldCardOrder1->setString("(" + TO_STR(goldCardOrderCount) + "/" + TO_STR(ONE_SPEND) + ")");
					setTextAddStroke(m_ui.layerTakeCard.pGoldCardOrder1,Color3B(0x00,0x00,0x00),3);
				}
				else
				{
					m_ui.layerTakeCard.pGoldYibi1->setVisible(true);
					m_ui.layerTakeCard.pGoldCardOrder1->setVisible(false);
				}

				if (goldCardOrderCount >= TEN_SPEND)
				{
					m_ui.layerTakeCard.pGoldYibi10->setVisible(false);
					m_ui.layerTakeCard.pGoldCardOrder10->setVisible(true);
					m_ui.layerTakeCard.pGoldCardOrder10->setString("(" + TO_STR(goldCardOrderCount) + "/" + TO_STR(TEN_SPEND) + ")");
					setTextAddStroke(m_ui.layerTakeCard.pGoldCardOrder10,Color3B(0x00,0x00,0x00),3);
				}
				else
				{
					m_ui.layerTakeCard.pGoldYibi10->setVisible(true);
					m_ui.layerTakeCard.pGoldCardOrder10->setVisible(false);
				}
			}

			m_ui.layerTakeCard.pTextTakeCardIntro2Panel->setVisible(false);
			m_ui.layerTakeCard.pTextTakeCardIntro2Bichu->setVisible(true);

			m_ui.layerTakeCard.pImgTipsChip2->setVisible(false);
			m_ui.layerTakeCard.pImgTipsHero2->setVisible(true);
			m_ui.layerTakeCard.Image_xiao->setVisible(false);
			m_ui.layerTakeCard.Image_zhong->setVisible(true);
		}
	}	
}

void LayerTakeCard::setCoinCountDownTime()
{
	int lremainderSec = TakeCardModel::getInstance()->getcoinCardSeconds();
	if (lremainderSec <= 0)
	{
		const TAKE_CARD_INFO& _cardInfo = TakeCardModel::getInstance()->getTakeCardInfo();
		if (_cardInfo.coinItem.CurCount > 0)
		{
			m_isFreeTime = true;			
		}
		else
		{
			m_isFreeTime = false;
		}
		setTakeCardInfo();

		m_ui.layerTakeCard.pDiShaZhengTextTime1->setVisible(false);
	}
	else
	{
		int nHour			= lremainderSec/3600;
		int nMinute			= (lremainderSec%3600) / 60;
		int nSecond			= (lremainderSec%3600) % 60;
		char szRemTime[100];
		sprintf(szRemTime, "%02d:%02d:%02d", nHour, nMinute, nSecond);
		m_ui.layerTakeCard.pDiShaZhengTextTime1->setText(szRemTime);
		m_ui.layerTakeCard.pDiShaZhengTextTime1->setVisible(true);
	}
}

void LayerTakeCard::setGoldCountDownTime()
{
	int lremainderSec	= TakeCardModel::getInstance()->getgoldCardSeconds();
	if (lremainderSec <= 0)
	{
		m_isFreeTime2 = true;
		setTakeCardInfo();
		m_ui.layerTakeCard.pTianGangZhengTextTime1->setVisible(false);
	}
	else
	{
		int nHour			= lremainderSec/3600;
		int nMinute			= (lremainderSec%3600) / 60;
		int nSecond			= (lremainderSec%3600) % 60;
		char szRemTime[100];
		sprintf(szRemTime, "%02d:%02d:%02d", nHour, nMinute, nSecond);

		m_ui.layerTakeCard.pTianGangZhengTextTime1->setText(szRemTime);
		m_ui.layerTakeCard.pTianGangZhengTextTime1->setVisible(true);
	}
}

Armature* LayerTakeCard::creatTakeCardResultAnimation(const int& loopTimes)
{	
	string strName = "cardone";
	float scale = 1.124f;
	if (loopTimes == 1)//单抽
	{
	}
	else  //10连抽
	{
		strName = "cards";
		scale = 1.028f;
	}	
	
	ArmatureDataManager::getInstance()->addArmatureFileInfo( "Image/AniTakeCard/" + strName +"/" + strName +"0.png", "Image/AniTakeCard/" + strName +"/" + strName +"0.plist", "Image/AniTakeCard/" + strName +"/" + strName + ".ExportJson");
	m_pTakeCardResultArmature = Armature::create(strName );

	m_pTakeCardResultArmature->setScale(scale);
	//设置帧事件；播放完动画
	
	m_pTakeCardResultArmature->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerTakeCard::onResultAnimationCallFunc,this));
	//设置动画精灵位置 
	m_pTakeCardResultArmature->setPosition(m_ui.layerTakeCard.pLayResult->getContentSize().width/2
		, m_ui.layerTakeCard.pLayResult->getContentSize().height/2+40);
	//添加到当前页面 
	m_ui.layerTakeCard.pLayResult->addChild(m_pTakeCardResultArmature,99);
	return m_pTakeCardResultArmature;
}
void LayerTakeCard::startTakeCardResultAnimation(const int& loopTimes)
{	
	if (loopTimes <= 0)
	{
		return;
	}
	if (m_bTakeCardResultAnimationIsRun)
	{
		return;
	}
	m_nTakeCardNum = loopTimes;
	if (m_pTakeCardResultArmature != nullptr)
	{
		m_ui.layerTakeCard.pLayResult->removeChild(m_pTakeCardResultArmature);
		m_pTakeCardResultArmature = nullptr;
	}
	m_bTakeCardResultAnimationIsRun = true;

	creatTakeCardResultAnimation(loopTimes);
	m_pTakeCardResultArmature->getAnimation()->playByIndex(0, 1, 0);
			
	m_pTakeCardResultArmature->setVisible(true);	
}

void LayerTakeCard::onResultAnimationCallFunc(Armature *armature, MovementEventType eventType, const std::string& strName)
{
	if (eventType == MovementEventType::COMPLETE)
	{
		m_bTakeCardResultAnimationIsRun = false;
		if (m_pTakeCardResultArmature != nullptr)
		{
			m_pTakeCardResultArmature->pause();
			m_pTakeCardResultArmature->setVisible(false);
		}
		showResultCards();
	}
} 

//抽卡按钮点击旋转动画
void LayerTakeCard::animationCardRotation(int index)
{
	OrbitCamera* camera = OrbitCamera::create(0.2f, 0.5f, 0.0f, 0.0f, CARD_ROTATION_ANIMATION_ANGLE, 0, 0);
	Vec3 vec = camera->getCenter();
	CCCallFuncN *pCallFuncN = CCCallFuncN::create(CC_CALLBACK_1(LayerTakeCard::animitionCardRotationCallFunND, this, index));
	Action* pSequence = Sequence::create(camera, pCallFuncN, nullptr); 

	if (index == 1)
	{
		m_ui.layerTakeCard.pImgTakeCard1->stopAllActions();
		m_ui.layerTakeCard.pImgTakeCard1->runAction(pSequence);
	}
	else
	{
		m_ui.layerTakeCard.pImgTakeCard2->stopAllActions();
		m_ui.layerTakeCard.pImgTakeCard2->runAction(pSequence);
	}
}
void LayerTakeCard::animitionCardRotationCallFunND(cocos2d::Node* pNode, int pValue)
{
	int index = (int)pValue;
	// 继续翻转;
	OrbitCamera* camera = OrbitCamera::create(0.2f, 0.5f, 0.0f, CARD_ROTATION_ANIMATION_ANGLE+180.0f, 180.0f-CARD_ROTATION_ANIMATION_ANGLE, 0, 0);
	Action* act = CCSequence::create(camera, nullptr); 

	if (index == 1)
	{
		if (m_ui.layerTakeCard.pLayTakeCardFront1->isVisible())
		{
			m_ui.layerTakeCard.pLayTakeCardFront1->setVisible(false);
			m_ui.layerTakeCard.pLayTakeCardOther1->setVisible(true);
		}
		else
		{
			m_ui.layerTakeCard.pLayTakeCardFront1->setVisible(true);
			m_ui.layerTakeCard.pLayTakeCardOther1->setVisible(false);
		}
		m_ui.layerTakeCard.pImgTakeCard1->stopAllActions();
		m_ui.layerTakeCard.pImgTakeCard1->runAction(act);
	}
	else
	{
		if (m_ui.layerTakeCard.pLayTakeCardFront2->isVisible())
		{
			m_ui.layerTakeCard.pLayTakeCardFront2->setVisible(false);
			m_ui.layerTakeCard.pLayTakeCardOther2->setVisible(true);
		}
		else
		{
			m_ui.layerTakeCard.pLayTakeCardFront2->setVisible(true);
			m_ui.layerTakeCard.pLayTakeCardOther2->setVisible(false);
		}
		m_ui.layerTakeCard.pImgTakeCard2->stopAllActions();
		m_ui.layerTakeCard.pImgTakeCard2->runAction(act);
	}
}

//抽卡翻卡动画
void LayerTakeCard::animationShowCard(cocos2d::ui::ImageView *pLayHeroCard, float fScale, int show)
{
	ImageView* pImgBg = dynamic_cast<ImageView*>(pLayHeroCard->getChildByTag(TAG_TAKECARD_BG));
	ImageView* pImgFront = dynamic_cast<ImageView*>(pLayHeroCard->getChildByTag(TAG_TAKECARD_FRONT));
	
	pLayHeroCard->setZOrder(5);
	m_fTakeCardResultAnimationScale = fScale;
	m_bTakeCardResultAnimationIsRun = true;
	
	Sequence* pSequence = nullptr;
	//翻转
	ScaleTo *pScaleTo = ScaleTo::create(CARD_TURN_ANIMATION_DURATION, 0.0f, fScale);	
	CCCallFuncN *pCallFuncN1 = CCCallFuncN::create(CC_CALLBACK_1(LayerTakeCard::animitionShowCardCallFunND, this, show));
	CCCallFuncN *pCallFuncN2 = CCCallFuncN::create(CC_CALLBACK_1(LayerTakeCard::animitionShowCardEndCallFunND, this, show));
	pSequence = Sequence::create( pScaleTo,pCallFuncN2,pCallFuncN1,nullptr);
	
	pLayHeroCard->stopAllActions();
	pLayHeroCard->runAction(pSequence);	
}

void LayerTakeCard::animitionShowCardCallFunND(cocos2d::Node* pNode, int show)
{
	ImageView* pImgFront = dynamic_cast<ImageView*>(pNode->getChildByTag(TAG_TAKECARD_FRONT));
	ImageView* pImgBg = dynamic_cast<ImageView*>(pNode->getChildByTag(TAG_TAKECARD_BG));
	pImgBg->setVisible(false);
	pImgFront->setVisible(true);

	ScaleTo *pScaleTo = ScaleTo::create(CARD_TURN_ANIMATION_DURATION, m_fTakeCardResultAnimationScale, m_fTakeCardResultAnimationScale);
	Sequence* pSequence = Sequence::create( pScaleTo, nullptr);
	pNode->runAction(pSequence);
}

void LayerTakeCard::animitionShowCardEndCallFunND(cocos2d::Node* pNode, int show)
{
	pNode->setZOrder(0);

	if (show == 1)
	{
		auto call = [this]()->void
		{
			m_bTakeCardResultAnimationIsRun = false;

			vector<NEW_CARD_INFO>& vecHeros= TakeCardModel::getInstance()->gettakeCardResulHeros();
			if (vecHeros.size() == 1)//一个
			{
				showResultCardsItem(m_ui.layerTakeCard.pLayHeroCardItemOne, *vecHeros.begin(),true);
			}
			else
			{
				showResultCardsItem(m_ui.layerTakeCard.pLayHeroCardItem[m_nTakeCardResultAnimationIndex], 
					vecHeros.at(m_nTakeCardResultAnimationIndex),true);
			}
			
			animationShowCardContinue();
		};
		LayerGetCard *_layerGetCard = LayerGetCard::create(m_CurShowHeroId,m_isChangeChip,m_ChangChipCount,call);
		this->addChild(_layerGetCard,4);
	}
	else
	{
		animationShowCardContinue();
	}
}

void LayerTakeCard::animationShowCardContinue()
{	
	m_nTakeCardResultAnimationIndex ++;
	vector<NEW_CARD_INFO>& vecHeros= TakeCardModel::getInstance()->gettakeCardResulHeros();
	if (m_nTakeCardResultAnimationIndex < vecHeros.size())
	{
		m_isChangeChip = false;
		if (vecHeros.at(m_nTakeCardResultAnimationIndex).countChips > 0)
		{
			m_nTakeCardResultAnimationTime = secondNow();
			m_isChangeChip = true;
			m_ChangChipCount = vecHeros.at(m_nTakeCardResultAnimationIndex).countChips;
		}

		int isshow = 0;
		m_CurShowHeroId = 0;
		RowData* pRowData = DataManager::getInstance()->searchToolById(vecHeros.at(m_nTakeCardResultAnimationIndex).id);
		if (pRowData != nullptr)
		{			 
			if (pRowData->getIntData("itemTyp") == TOOL_ITEM_TYPE_HERO)  //4-英雄
			{
				//旋转移动到中间放大到正常 翻卡
				isshow = 1;
				m_CurShowHeroId = vecHeros.at(m_nTakeCardResultAnimationIndex).id;
			}			
		}
		animationShowCard(m_ui.layerTakeCard.pLayHeroCardItem[m_nTakeCardResultAnimationIndex], m_fTakeCardResultAnimationScale, isshow);
	}
	else
	{
		m_bTakeCardResultAnimationIsRun = false;

		GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_SHOP_DONE);
	}
}