#ifndef RefineryHeader_h__
#define RefineryHeader_h__

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
#include "MainModule/MainCityScene.h"
#include "Utils/ResourceUtils.h"
#include "Temp/PopReward.h"
#include "FormationModule/LayerFormation.h"
#include "FightModule/SceneFightLoading.h"
#include "FightModule/FightController.h"
#include "HeroModule/HeroModel.h"
#include "CommonBuy/LayerCommonBuy.h"
#include "Shopexc/LayerShopexc.h"
#include "Widget/LayerCommHeadInfo.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const int MaterialCount = 6;

//PVP主界面控件
struct  UI_REFINERY_ENTER
{
	// UI根节点层容器
	Layout		*pRoot;

	Layout      *PanelLeft; //炉子

	CheckBox   *checkBaGua;
	CheckBox   *checkJiuGong;
	ImageView  *imgBg; //背景
	ImageView  *materialItem[MaterialCount];

	Button     *baGuaShop;
	Button     *jiuGongShop;
	Button     *refining;

	ImageView  *imgShowBagua;
	ImageView  *imgShowJiuGong;

	ImageView  *imgGetBagua;
	ImageView  *imgGetJiuGong;
	Text       *getAmount;

	Text       *tips;


	Layout      *PanelRight; //材料
	ScrollView  *materialList;
	Text        *meTips;
};

enum REFINERY_STATE
{
	REFINERY_STATE_BAGUA,
	REFINERY_STATE_JIUGONG
};

enum REFINERY_EVENT_TYPE
{
	REFINERY_EVENT_TYPE_SEND,
};

struct REFINERY_NODE
{
	int id;  //物品id
	int amount; //物品个数
	int idx; //对应炼化炉索引

	REFINERY_NODE():
	id(0),
	amount(0),
	idx(-1)
	{

	}
};

typedef vector<REFINERY_NODE> REFINERY_NODE_VEC;

#endif