#ifndef LayerHeroStarsUp_h__
#define LayerHeroStarsUp_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "UITagHero.h"
#include "DataManager/DataManager.h"
#include "LayerHero.h"


USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

struct UITagLayerHeroStarsUp
{
	//根结点
	Layout* mRoot;
	//卡牌面板
	Layout* mCardPanel;
	//星星
	ImageView* mStarImg1[STAR_COUNT];
	Point mStarPos1[STAR_COUNT];
	ImageView* mStarImg2[STAR_COUNT];
	Point mStarPos2[STAR_COUNT];
	
	//属性面板
	Layout* mStarsUpPropertyPnl;

	//点击提示
	ImageView* mClickImg;

	//箭头
	ImageView* mArrowImg;

	//升星数值
	Text* mAtkGrow1;
	Text* mHpGrow1;
	Text* mPddGrow1;
	Text* mMddGrow1;
	Text* mSpeed1;
	Text* mAtkGrow2;
	Text* mHpGrow2;
	Text* mPddGrow2;
	Text* mMddGrow2;
	Text* mSpeed2;

	Text* mAtkAdd;
	Text* mHpAdd;
	Text* mPddAdd;
	Text* mMddAdd;

	//战力提升
	Text* mFightPower1;
	Text* mFightPower2;
};

class LayerHeroStarsUp : public ModalLayer
{

public:

	LayerHeroStarsUp();
	virtual ~LayerHeroStarsUp();

	static LayerHeroStarsUp* create(int templetId);

public:

	virtual bool init(int templetId);
	virtual void onEnter();
	virtual void onExit();

private:

	//初始化UI
	void initUI();

	//更新UI
	void updateUI();

	//初始化动画
	void initAnimation();

	//动画回调
	void aniAniCallBack(Armature *armature, MovementEventType movementType, const std::string& movementID, const string& nextAniName);

	//升星动画
	void startUpStarAni();
	//启动星星动画
	void startStarAni();

private:

	UITagLayerHeroStarsUp mUI;

	//英雄模版ID
	int mHeroId;

	//星星数量
	int mStarCount1;
	int mStarCount2;
};

#endif //LayerHeroStarsUp_h__