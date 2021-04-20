#ifndef LoveTreasureLayer_h__
#define LoveTreasureLayer_h__


#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "FormationModule/CustomScrollBar.h"
#include "Common/Common.h"
#include "ui/UIDef.h"
#include "Temp/CustomTips.h"
#include "Temp/CustomPop.h"
#include "MainModule/MainModel.h"
#include "ui/ModalLayer.h"
#include "LoveHeader.h"
#include "LoveModule/LoveModel.h"
#include "Widget/LayerCommHeadInfo.h"
#include "LoveController.h"
#include "LoveGoodWillPrize.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const int _DifficultCount = 5;
const float MapMoveTime = 0.4f;
const int LatticeCount = 49;
const int MapColCount = 7;

const float enterDu_time = 0.4f;
const float exitDu_time = 0.2f;

struct  Ui_Love_Treasure
{
	// UI根节点层容器
	Layout		*pRoot;

	Layout      *PnlTask;
	Layout      *PnlTaskResult;
	Layout      *TaskResult;
	Layout      *PnlDuiBai;
	ImageView   *heroImg;
	Vec2        heroImgPos;

	ImageView   *duibai;
	Vec2        duibaiPos;
	Text        *content;

	Vec2        PnlTaskPos;

	ImageView   *ImgDi;
	Vec2        ImgDiPos;

	Text       *stepCount;
	Text       *boxCount;

	ImageView  *Lattice[LatticeCount];
};

const int PLAY_GOLD_ITEM_ID = 11001;
const int PLAY_COIN_ITEM_ID = 11002;

class LoveTreasureLayer:public Layer, public Observer
{
public:
	~LoveTreasureLayer(void);

	static LoveTreasureLayer* create(int idx,int petId);

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi(int idx,int petId);  //初始化主界面
	void addInfoPanel(Widget *node,PLAY_MAP_RESULT_TYPE type);

	void CalculationAdjacent(); //计算相邻
	void moveStep(int idx);
	void setAddlove(); //设置增加的好感
	void setUpGradePrize();//好感升级奖励

	void setAddStepChange(); //设置增加的步骤
	void showItemReward(); //宝箱
	void showplayReward(); //宝藏


	void playResult();
	void setDuiBai(string str);
	void animationTalkOut(string strText);
	void callBackTalkOut(float dt);

	void setLatticeInfo(bool view = false);
	void setLatticeItemByData(int idx,bool view = false);
	void setStepCount();

	LOVE_HERO_INFO* getHeroByHeroId(int petId);
private:
	LoveTreasureLayer(void);

	Ui_Love_Treasure m_ui;

	int m_mapIdx;  //地图索引
	int m_heroPetId; //游玩英雄
	int m_initIdx; //地图当前位置

	bool  m_Iscomplete;
	bool  m_canBack;

	//对白
	std::string		m_strTalkContent;
	int				m_nTalkContentLength;
};

#endif
