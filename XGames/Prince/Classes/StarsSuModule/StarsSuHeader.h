#ifndef StarsSuHeader_h__
#define StarsSuHeader_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "Communication/ICommListener.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Socket/Util.h"
#include "HeroModule/HeroModel.h"
#include "MainModule/MainModel.h"
#include "Common/Common.h"
#include "FormationModule/CustomScrollBar.h"
#include "GuideCenter/GuideManager.h"
#include "ChapterModule/SceneChapterLoading.h"
#include "2d/CCActionInterval.h"
#include "Widget/LayerCommHeadInfo.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const int STAR_TYPE_COUNT = 3;
const int STAR_SU_COUNT = 7;
const int HERO_STAR_COUNT = 6;


enum STARSSU_EVENT_TYPE
{
	STARSSU_EVENT_TYPE_NONE,
	STARSSU_EVENT_TYPE_GET_INFO,
	STARSSU_EVENT_TYPE_ASTROLOGY_BREAK_OUT,
};

enum ASTROLOGY_TYPE
{
	ASTROLOGY_TYPE_NONE = 0,
	ASTROLOGY_TYPE_FREE,
	ASTROLOGY_TYPE_COIN,
	ASTROLOGY_TYPE_BREAK_OUT,
	ASTROLOGY_TYPE_ONE_KEY
};

enum class ASTROLOGY_TYPE_BUTTON
{
	ASTROLOGY_TYPE_BUTTON_ZHAN, //占星
	ASTROLOGY_TYPE_BUTTON_ONE_KEY_ZHAN, //一键占星
	ASTROLOGY_TYPE_BUTTON_TUPO  //突破
};

struct STARSTSU_INFO
{
	STARSTSU_INFO()
	{
		memset(this,0,sizeof(STARSTSU_INFO));
	}
	int lastsId; //星宿ID
	int CursId; //星宿ID
	int lastPoint; //上次星点数
	int curPoint; //当前星点数
	int coinCost; //占星金币、银币消耗
	int freeCount; //免费次数
	int criPrb;//暴击
};

struct STARSSY_UI
{
	Layout*  pRoot;  //根节点
	Button*  pAstrology;//占星
	Button*  pOneKeyAstrology; //一键占星
	Button*  pBreakOut; //突破
	Text*    pBreakOutLv; //几重天
	ListView* pAttributeList;//属性列表
	ImageView* pHeroStar[HERO_STAR_COUNT];
	Text*    pAstrologySpend;//占星花费
	ImageView*pAstrologySpendImg;
	Text*    pBreakOutSpend;//突破花费
	ImageView* pBreakOutSpendImg;
	Text*    freeCount;
	Text*    starCountTips;
	vector<Layout*> pStarPanel;
	vector<ImageView*> pStar[STAR_TYPE_COUNT];
	ImageView*  pStatTips;

	ImageView*   pAstrologyBall;
	ImageView*   pbreakOutBall;
	LoadingBar* pLoadingBar;//占星进度
	Text*      pLoadingBarCount;
	Text*      pAstrologyBallDesc;
	Text*      pAstrologyBallDescCount;

	Text*      pbreakOutBallDesc1;
	Text*      pbreakOutBallDesc1count;
	Text*      pbreakOutBallDesc2;
	Text*      pbreakOutBallDesc2count;

	Text*    Pbaoji;
	Vec2     baojiPos;

	Layout*  pNoStarTips;  //根节点

};

#endif