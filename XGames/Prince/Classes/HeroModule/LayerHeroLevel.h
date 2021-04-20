#ifndef LayerHeroLevel_h__
#define LayerHeroLevel_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "HeroModel.h"
#include "ToolModule/UIToolHeader.h"
#include "UITagHero.h"
#include "Utils/Observer.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

struct UITagHeroLevel
{
	//升级面板
	Layout* mRoot;
	ImageView* mLevelUpPnl;
	Text* mCurLvlAndMaxLvl;
	LoadingBar* mExpBar;
	TextAtlas* mCurExpText;
	TextAtlas* mMaxExpText;
	UIToolHeader* mExpHeader[EXP_ITEM_COUNT];
};

class LayerHeroLevel : public ModalLayer, public Observer
{

public:

	LayerHeroLevel();
	virtual ~LayerHeroLevel();

	static LayerHeroLevel* create(HeroParam* heroParam);

public:

	virtual void updateSelf(void* data);
	virtual bool init(HeroParam* heroParam);

	virtual void onEnter();
	virtual void onExit();

private:

	void updateUI();

	//点击响应
	void onExpItemClicked(Ref* ref, Widget::TouchEventType type, int itemId);

	//判定长按
	void judgeLongPressed(float delta);

	//执行长按
	void onLongPressed(float delta);

	//递归计算升级经验
	void calExp(int curExp, int maxExp);

private:

	//英雄参数
	HeroParam* mHeroParam;
	
	//UI
	UITagHeroLevel mUI;

	//当前选中的道具图标
	UIToolHeader* mCurIcon;

	//当前选中道具加多少经验
	int mExpAdd;
	//当前选中的道具最大数量
	int mMaxNum;
	//道具使用数量
	int mUseNum;
	//当前等级
	int mHeroLevel;
	//当前经验
	int mCurExp;

	//是否进入长按状态
	bool isLongPressed;
	
};

#endif //LayerHeroLevel_h__