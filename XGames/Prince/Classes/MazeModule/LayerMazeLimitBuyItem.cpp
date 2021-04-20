#include "LayerMazeLimitBuyItem.h"

#include "CocoStudio.h"
#include "MazeModel.h"
#include "MazeController.h"
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

using namespace cocos2d;
using namespace cocos2d::ui;


LayerMazeLimitBuyItem::LayerMazeLimitBuyItem()
{	
	m_MazeLimitObserver = nullptr;
}

LayerMazeLimitBuyItem::~LayerMazeLimitBuyItem()
{
	CCLOG("LayerMazeLimitBuyItem destroy");
}

bool LayerMazeLimitBuyItem::init()
{
	if(!Layer::init())
	{
		return false;
	}
	initUI();

	return true;
}


void LayerMazeLimitBuyItem::initUI()
{
	Size size = Director::getInstance()->getWinSize();
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIxianshi/UIxianshiIcon.ExportJson"));
	this->addChild(m_ui.pRoot);
	m_ui.pRoot->addTouchEventListener(CC_CALLBACK_2(LayerMazeLimitBuyItem::onClickPass, this));
	m_ui.pRoot->setTouchEnabled(true);
	//
	m_ui.pLayHero = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_hero"));
	m_ui.mImgBg = dynamic_cast<ImageView*>(m_ui.pRoot->getChildByName("Img_xianshi"));
	m_ui.mTextTime  = dynamic_cast<Text*>(m_ui.mImgBg->getChildByName("Lab_time"));
	m_ui.mTextTime->setFontName(FONT_FZZHENGHEI);
}


void LayerMazeLimitBuyItem::updateInfo(const int& heroTmpId, const int& limtSecond,const int& buyCount)
{
	RowData* itemData = DataManager::getInstance()->searchToolById(heroTmpId);
	if (itemData == nullptr)
	{
		return;
	}
	m_ui.pLayHero->removeAllChildrenWithCleanup(true);
	UIToolHeader* pTool = UIToolHeader::create(ResourceUtils::getSmallIconPath(itemData->getIntData("resId")), "Image/Icon/global/btn_kuang_juese_1.png", "Image/Icon/global/btn_kuang_juese_2.png");
	m_ui.pLayHero->addChild(pTool);
	pTool->setTouchEnabled(false);
	if (buyCount > 1)
	{
		pTool->setNumEx(buyCount);
	}
	pTool->setPosition(Vec2(m_ui.pLayHero->getContentSize().width/2, m_ui.pLayHero->getContentSize().height/2));
	
	startCountDown(limtSecond);
}

void LayerMazeLimitBuyItem::onClickPass(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_MazeLimitObserver != nullptr)
		{
			m_MazeLimitObserver->onMazeLimitClicked();
		}	
	}
}

long LayerMazeLimitBuyItem::getLastCountDown()
{
	long ldisTime		=  secondNow()-m_time_start_sec;
	return m_ltotalSec - ldisTime;
}

//
cocos2d::Vec2 LayerMazeLimitBuyItem::getHeroPostion()
{	
	return cocos2d::Vec2(this->getPosition().x + m_ui.pLayHero->getContentSize().width/2
							, this->getPosition().y + m_ui.pLayHero->getPosition().y + m_ui.pLayHero->getContentSize().height/2);
}

//
void LayerMazeLimitBuyItem::startCountDown(const long ltotalSec)
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
	sprintf(szRemTime, "%d:%02d:%02d", nHour, nMinute, nSecond);

	m_ui.mTextTime->setString(szRemTime);

	Director::getInstance()->getScheduler()->schedule(schedule_selector(LayerMazeLimitBuyItem::updateCountDown), this, 1, false);
}

void LayerMazeLimitBuyItem::stopCountDown()
{
	m_time_start_sec	= 0;
	m_ltotalSec			= 0;
	m_isFreeTime		= true;
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(LayerMazeLimitBuyItem::updateCountDown), this);
}

void LayerMazeLimitBuyItem::updateCountDown(float ft)
{
	long ldisTime		=  secondNow()-m_time_start_sec;
	long lremainderSec	= m_ltotalSec - ldisTime;

	int nHour			= lremainderSec/3600;
	int nMinute			= (lremainderSec%3600) / 60;
	int nSecond			= (lremainderSec%3600) % 60;

	char szRemTime[100];
	sprintf(szRemTime, "%d:%02d:%02d", nHour, nMinute, nSecond);

	m_ui.mTextTime->setString(szRemTime);
	if (lremainderSec <= 0)
	{
		stopCountDown();
		this->removeFromParent();
	}	
}


