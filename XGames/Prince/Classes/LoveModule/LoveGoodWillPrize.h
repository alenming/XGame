#ifndef LoveGoodWillPrize_h__
#define LoveGoodWillPrize_h__


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

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;


const int PrizeCount = 3;
const float fadeTime = 0.4f;
typedef std::function<void()> GoodWillPrizeCallback;

 
struct  UI_LOVE_GOOD_WILL_PRIZE
{
	// UI根节点层容器
	Layout		*pRoot;

	//信息面板
	ImageView   *pPnlInformation;
	Text        *LabTips;
	Layout      *PnlHero;
	Text        *LabName;
	
	ImageView   *ImgLove;
	ImageView   *ImgUp1;

	Text        *LabLevel;
	ImageView   *ImgUp2;


	//奖励面板
	Layout      *pPnlReward;
	Layout      *pPrizeItem[PrizeCount];
};

class LoveGoodWillPrize:public ModalLayer 
{
public:
	~LoveGoodWillPrize(void);
	static LoveGoodWillPrize* create(LOVE_HERO_INFO heroInfo,SEND_GIFT_REWARD_VEC* reward,GoodWillPrizeCallback call = nullptr);

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi(LOVE_HERO_INFO heroInfo,SEND_GIFT_REWARD_VEC* reward,GoodWillPrizeCallback call);  //初始化主界面

	void setPnlInformation(LOVE_HERO_INFO heroInfo);//设置英雄面板
	void setPnlPrize(SEND_GIFT_REWARD_VEC* reward);//设置奖励面板

	void setAnimationLoveFeelUp(); //设置情缘升级动画

private:
	LoveGoodWillPrize(void);

	UI_LOVE_GOOD_WILL_PRIZE  m_ui;
};

#endif
