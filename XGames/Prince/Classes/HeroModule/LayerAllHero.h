#ifndef LayerAllHero_h__
#define LayerAllHero_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UITagHero.h"
#include "UIHeroCard.h"
#include "FormationModule/CustomScrollBar.h"
#include "Utils/Observer.h"
#include "HeroRecruitController.h"

USING_NS_CC;
using namespace cocostudio;
using namespace ui;
using namespace std;

const int ENTER_CARD_COUNT = 8;

struct UITagLayerAllHero
{
	//根结点
	Layout* mRoot;

	//滚动条父结点
	Layout* mScrollBarPanel;

	//滚动面板
	ScrollView* mHeroScrollView;

	//关闭按钮
	Button* mCloseBtn;

	//页卡父结点
	Layout* mTabPanel;

	//Tab页
	CheckBox* mTabAllHero;			//全部
	CheckBox* mTabAtkHero;			//攻击
	CheckBox* mTabDefHero;			//防御
	CheckBox* mTabHelpHero;			//辅助
	CheckBox* mTabFirstHero;		//先手

	//Tab响应
	Layout* mClickAllHero;
	Layout* mClickAtkHero;
	Layout* mClickDefHero;
	Layout* mClickHelpHero;
	Layout* mClickFirstHero;

	//滚动条
	CustomScrollBar* mScrollBar;

};

class LayerAllHero : public Layer, public Observer
{

public:

	LayerAllHero();
	virtual ~LayerAllHero();

	CREATE_FUNC(LayerAllHero);

public:

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	void update(float dt);

private:

	//数据更新
	virtual void updateSelf(void* data);

	//更新UI
	virtual void updateUI();

	//初始化UI
	void initUI();

	//按键响应
	void onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId);

	//切换标签页
	void switchTab(int heroJob);

	//显示英雄列表，param1. -1表示显示全部
	void showHeroList(int heroJob);

	//初始化英雄列表, param.是否全部初始化
	void initHeroList();

	//滚动事件响应
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//加载卡牌之前先对ID排序
	void sortTempId();

	//获取指定id英雄碎片数量
	int getDebrisNumById(int tempId);

	//获取指定id英雄是否可招募
	bool isRecruitableById(int tempId);

	//获取指定职业的英雄数量
	int getHeroCountByJob(int heroJob);

	//显示卡牌入场动画
	void showCardAnimation();

	//触摸事件
	virtual bool onTouchBegan(Touch* touch, Event* event, UIHeroCard* heroCard);
	virtual void onTouchMoved(Touch* touch, Event* event);
	virtual void onTouchEnded(Touch* touch, Event* event);
	virtual void onTouchCancelled(Touch* touch, Event* event);

private:

	//UI类
	UITagLayerAllHero mUI;

	//当前显示的英雄类型
	int mHeroShowType;

	//当前显示的英雄职业
	int mHeroShowJob;

	//英雄卡牌组件列表
	Vector<UIHeroCard*> mVecHeroCard;

	vector<UIHeroCard*> mVecEnterCard;

	//当前点击的卡牌
	UIHeroCard* mCurrentCard;

	//通信
	HeroRecruitController heroRecruitController;

	//是否首次进入该界面
	bool mIsFirstEnter;

	//是否已经点击卡牌，防止重复点击
	bool mIsCardClicked;
	
	//卡牌动入场动画是否播放完毕
	bool mIsCardAniEnd;

	//卡牌tempId集合
	vector<int> mVecCardTempId;

	//当前创建的卡牌数量
	int mCurCreateNum;
};


#endif //LayerAllHero_h__