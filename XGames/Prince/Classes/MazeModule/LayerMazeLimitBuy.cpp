#include "LayerMazeLimitBuy.h"

#include "CocoStudio.h"
#include "MazeModel.h"
#include "MazeLimitController.h"
#include "Common/Common.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "ResourceManager/ResourceLoader.h"
#include "UI/UIDef.h"
#include "Widget/LayerLoading.h"
#include "../ToolModule/ToolModel.h"
#include "Temp/CustomPop.h"
#include "Temp/CustomTips.h"
#include "HeroModule/HeroModel.h"
#include "Utils/ResourceUtils.h"
#include "Utils/StringFormat.h"
#include "../MainModule/MainModel.h"
#include "../FightModule/SceneFightLoading.h"
#include "../FightModule/FightController.h"
#include "MainModule/MainCityScene.h"
#include "MazeLimitController.h"
#include "Temp/CustomGoldTips.h"

using namespace cocos2d;
using namespace cocos2d::ui;

const float F_DISPLAY_HERO_SCALE = 0.6;


LayerMazeLimitBuy::LayerMazeLimitBuy()
{	
	
	mMazeLimitController.onSetView(this);
	m_pObserver = nullptr;
	m_head = nullptr;
	m_cardPrize = nullptr;
}

LayerMazeLimitBuy::~LayerMazeLimitBuy()
{
	CCLOG("LayerMazeLimitBuy destroy");
	mMazeLimitController.onRemoveView();
}

bool LayerMazeLimitBuy::init()
{
	if(!ModalLayer::init())
	{
		return false;
	}
	initUI();

	return true;
}

void LayerMazeLimitBuy::onLimitBuyResult(const int& nResult, const vector<TOOL_NEW_INFO>& vecNewTools)
{
	//获得道具
	std::string strNewTools="";
	for (auto iter = vecNewTools.begin(); iter < vecNewTools.end(); iter++)
	{
		RowData* pToolData = DataManager::getInstance()->searchToolById(iter->ntemplateId);
		if (pToolData != nullptr)
		{
			if (pToolData->getIntData("itemTyp") == TOOL_ITEM_TYPE_CONSUME)
			{
				strNewTools+=pToolData->getStringData("itemName");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20271)
					+ (": ") + _TO_STR(iter->nstack) +
					DataManager::getInstance()->searchCommonTexdtById(20077)+ strNewTools, 
					NORMAL_TIPS_BACKGROUND_IMG, 30, cocos2d::Color3B(0x54,0xff,0x26));
			}
			else if (pToolData->getIntData("itemTyp") == TOOL_ITEM_TYPE_HERO)
			{
				LayerGetCard *_layerGetCard = LayerGetCard::create(iter->ntemplateId,false,iter->nstack);
				Director::getInstance()->getRunningScene()->addChild(_layerGetCard);
			}
			else if (pToolData->getIntData("itemTyp") == TOOL_ITEM_TYPE_HERO_CHIP)
			{
				LayerGetCard *_layerGetCard = LayerGetCard::create(pToolData->getIntData("transItem"),true,iter->nstack);
				Director::getInstance()->getRunningScene()->addChild(_layerGetCard);
			}			
			break;
		}							
	}	

	if (m_pObserver != nullptr)
	{
		m_pObserver->onMazeLimitBuyEnd();
	}
	dispose();
	if (m_head)
	{
		m_head->removeFromParent();
	}
	else if(m_cardPrize)
	{
		m_cardPrize->removeFromParent();
	}
}

void LayerMazeLimitBuy::initUI()
{
	Size size = Director::getInstance()->getWinSize();
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIxianshi/UIxianshi.ExportJson"));
	this->addChild(m_ui.pRoot, 9);
	m_ui.pRoot->addTouchEventListener(CC_CALLBACK_2(LayerMazeLimitBuy::onClickPass, this));

	//时间
	ImageView* pImgBg = dynamic_cast<ImageView*>(m_ui.pRoot->getChildByName("Img_xianshi"));
	m_ui.mTextTime  = dynamic_cast<Text*>(pImgBg->getChildByName("Lab_time"));
	m_ui.mTextTime->setFontName(FONT_FZZHENGHEI);
	//
	m_ui.pBtnBuy = dynamic_cast<Button*>(m_ui.pRoot->getChildByName("Btn_buy"));
	m_ui.pBtnWait = dynamic_cast<Button*>(m_ui.pRoot->getChildByName("Btn_wait"));
	m_ui.pBtnBuy->addTouchEventListener(CC_CALLBACK_2(LayerMazeLimitBuy::onBtnBuy, this));
	m_ui.pBtnWait->addTouchEventListener(CC_CALLBACK_2(LayerMazeLimitBuy::onBtnWait, this));

	m_ui.mBuyText = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBtnBuy, "Lab_text"));
	Text* mCoinText = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBtnBuy, "Lab_coinNum"));
	Text* mWaitText = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBtnWait, "Lab_text"));
	m_ui.mBuyText->setFontName(FONT_FZZHENGHEI);
	mCoinText->setFontName(FONT_FZZHENGHEI);
	mWaitText->setFontName(FONT_FZZHENGHEI);

	//m_ui.Item = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Image_5"));
	m_ui.mvecHeroPanel = Vec2(500,200);
}


void LayerMazeLimitBuy::updateInfo(const int& heroTmpId, const int& limtSecond, const int& heroPrice, cocos2d::Vec2 vecStartPostion,Layout* layout,int buyCount)
{
	RowData* itemData = DataManager::getInstance()->searchToolById(heroTmpId);
	RowData* heroData = DataManager::getInstance()->searchCharacterById(heroTmpId);
	if (heroData == nullptr && itemData == nullptr)
	{
		return;		
	}

	if (heroData)//卡牌
	{
		m_head = UICardHead::create(heroTmpId,false,true);
		layout->addChild(m_head,12);
	}
	else
	{
		m_cardPrize = UICardPrize::create(heroTmpId);
		layout->addChild(m_cardPrize,12);
		m_cardPrize->setCardCount(buyCount);
		m_cardPrize->setCardName(itemData->getStringData("itemName"));
	}

	m_vecHeroMoveStartPostion = vecStartPostion;
	m_price = heroPrice;
	Text* mCoinText = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBtnBuy, "Lab_coinNum"));
	mCoinText->setText(_TO_STR(heroPrice));

	//倒计时
	startCountDown(limtSecond);
	//
	animationHeroMove(1, m_vecHeroMoveStartPostion);
}

void LayerMazeLimitBuy::onClickPass(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{

	}

}

void LayerMazeLimitBuy::onBtnBuy(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		// 迷宫限时购买
		MainParam* pMainParam = MainModel::getInstance()->getMainParam();
		if (pMainParam != nullptr && pMainParam->mGold < m_price)
		{
			//金币不够
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
			}
			return;
		}

		mMazeLimitController.onLimitBuy();
	}
}

void LayerMazeLimitBuy::onBtnWait(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		animationHeroMove(0, m_vecHeroMoveStartPostion);
		m_pObserver->onMazeLimitBuyWait();
		//dispose();
	}
}

//
void LayerMazeLimitBuy::startCountDown(const long ltotalSec)
{
	if (ltotalSec <= 0)
	{
		return;
	}

	m_ltotalSec			= ltotalSec;
	m_time_start_sec	= secondNow();

	long lremainderSec	= m_ltotalSec;
	int nHour			= lremainderSec/3600;
	int nMinute			= (lremainderSec%3600) / 60;
	int nSecond			= (lremainderSec%3600) % 60;
	char szRemTime[100];
	sprintf(szRemTime, "%02d:%02d:%02d", nHour, nMinute, nSecond);

	m_ui.mTextTime->setText(szRemTime);

	Director::getInstance()->getScheduler()->schedule(schedule_selector(LayerMazeLimitBuy::updateCountDown), this, 1, false);
}

void LayerMazeLimitBuy::stopCountDown()
{
	m_time_start_sec	= 0;
	m_ltotalSec			= 0;
	m_isFreeTime		= true;
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(LayerMazeLimitBuy::updateCountDown), this);
}

void LayerMazeLimitBuy::updateCountDown(float ft)
{
	long ldisTime		=  secondNow()-m_time_start_sec;
	long lremainderSec	= m_ltotalSec - ldisTime;

	int nHour			= lremainderSec/3600;
	int nMinute			= (lremainderSec%3600) / 60;
	int nSecond			= (lremainderSec%3600) % 60;

	char szRemTime[100];
	sprintf(szRemTime, "%02d:%02d:%02d", nHour, nMinute, nSecond);

	m_ui.mTextTime->setText(szRemTime);
	if (lremainderSec <= 0)
	{
		stopCountDown();
		dispose();
		if (m_head)
		{
			m_head->removeFromParent();
		}
		else if(m_cardPrize)
		{
			m_cardPrize->removeFromParent();
		}
	}	
}

//卡牌飞出动画
void LayerMazeLimitBuy::animationHeroMove(int isOut, cocos2d::Vec2 vecStartPostion)
{
	Size size = Director::getInstance()->getWinSize();

	float fD = 0.5;
	MoveTo* pMoveTo = nullptr;
	ScaleTo* pScaleTo = nullptr;
	if (isOut == 0)
	{
		//卡牌渐渐变小飞向屏幕左上角消失		
		pMoveTo = MoveTo::create(fD, vecStartPostion);// cocos2d::Vec2(0, size.height)
		pScaleTo = ScaleTo::create(fD, 0.0);
	}
	else
	{
		//卡牌渐渐变大飞向屏幕
		if (m_head)
		{
			m_head->setScale(0.0);
			m_head->setPosition(vecStartPostion);
		}
		else if(m_cardPrize)
		{
			m_cardPrize->setScale(0.0);
			m_cardPrize->setPosition(vecStartPostion);
		}
		
		pMoveTo = MoveTo::create(fD, m_ui.mvecHeroPanel);

		if (m_head)
		{
			pScaleTo = ScaleTo::create(fD, 0.65f);
		}
		else if(m_cardPrize)
		{
			pScaleTo = ScaleTo::create(fD, 1.0);
		}
	}
	
	EaseBackOut* pEaseOut = EaseBackOut::create(pMoveTo);
	Spawn*	pSpawn = Spawn::create(pScaleTo, pEaseOut, nullptr);

	CCCallFunc *pCallFuncN = CCCallFunc::create(CC_CALLBACK_0(LayerMazeLimitBuy::animationHeroMoveCallFunND, this, isOut));
	Sequence* pSequence = Sequence::create( pSpawn, pCallFuncN, nullptr);
	if (m_head)
	{
		m_head->stopAllActions();
		m_head->runAction(pSequence);
	}
	else if(m_cardPrize)
	{
		m_cardPrize->stopAllActions();
		m_cardPrize->runAction(pSequence);
	}
	
}

void LayerMazeLimitBuy::animationHeroMoveCallFunND( int isOut)
{
	if (isOut == 0)
	{
		dispose();
		if (m_head)
		{
			m_head->removeFromParent();
		}
		else if(m_cardPrize)
		{
			m_cardPrize->removeFromParent();
		}
	}	
}


