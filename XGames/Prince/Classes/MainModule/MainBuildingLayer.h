#ifndef PerspectLayer_h__
#define PerspectLayer_h__

#include "UI/UIDef.h"
#include "Utils/Observer.h"

USING_NS_CC;
using namespace cocos2d::ui;

const int FLOOR_COUNT = 7;
const int BUILDING_COUNT = 25;

typedef function<void(void)> MainCityScrollCallBack;

typedef struct tagUIBuilding
{
	//建筑物实体
	ImageView* buildingEntity;

	//建筑名称
	ImageView* buildingTitle;

	//建筑物id
	int buildingId;

	//建筑物名称
	string buildingName;

}UIBuilding;

typedef struct tagUIPerspect
{
	//根结点
	Layout* root;

	//第一层
	cocos2d::ui::ScrollView* firstFloor;

	//层列表
	vector<cocos2d::ui::ScrollView*> vecFloor;

	//建筑物列表
	vector<UIBuilding> vecBuilding;

}UIPerspect;

// enum MAIN_BUILDING_TAG
// {
// 	BUILDING_PVP = 2,				//竞技场
// 	BUILDING_SHOP = 4,				//商城
// 	BUILDING_LOVE = 6,				//情缘
// 	BUILDING_FACTION = 7,			//帮派
// 	BUILDING_TOWER = 9,				//爬塔
// 	BUILDING_REFINE = 13,		    //炼化炉子
// 	BUILDING_STAR_CAMP = 14,		//迷宫
// 	BUILDING_PVE = 15,				//关卡
// };

enum MAIN_TAG
{
	TAG_MAZE = 5,			//幻境
	TAG_SEAL = 6,			//封印
	TAG_PVE = 7,			//冒险
	TAG_TOWER = 11,			//试炼塔
	TAG_DAILY_PVE = 12,		//每日挑战
	TAG_REFINE = 13,		//炼化炉
	TAG_FACTION = 17,		//帮派
	TAG_LOVE = 18,			//情缘
	TAG_SHOP = 19,			//集市
	TAG_BOSS = 20,			//世界BOSS
	TAG_PVP = 23,			//竞技场
};

class MainBuildingLayer : public Layer,public Observer
{

public:

	MainBuildingLayer();
	virtual ~MainBuildingLayer();

	CREATE_FUNC(MainBuildingLayer);

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);
public:

	virtual bool init() override;

	virtual void onEnter() override;

	//建筑物点击响应
	void onBuildingClick(int buildingTagId);

	//跳转至某场景
	void jumpToScene(int buildingTagId);

	//跳转到当前记录位置
	void jumpToCurrent();

	//跳转到百分比
	void jumpToPercent(float percent);

	//滚动到百分比
	void scrollToPercent(float percent, const MainCityScrollCallBack& callback);

	//根据屏幕分辨率计算初始滚动比例
	static float getInitialPercent();

	//设置初始滚动比例
	static void setInitialPercent();

	//获取建筑图标上的小红点
	Node* getBuildingRetFlag(int buildingTagId);

	//统一设置建筑标签显隐
	void setTileVisible();

	//主界面动画与建筑关联
	//string getBuildingAniName(MAIN_BUILDING_TAG tag);

private:

	//初始化触摸事件
	void initTouchEvent();

	//滚动事件
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//滚动触摸修正
	void onTouchScrollEvent(Ref* pSender, Widget::TouchEventType type);

	//触摸事件
	virtual bool onTouchBegan(Touch* touch, Event* event);
	virtual void onTouchMoved(Touch* touch, Event* event);
	virtual void onTouchEnded(Touch* touch, Event* event);
	virtual void onTouchCancelled(Touch* touch, Event* event);

	//alpha检测
	bool alphaTouchCheck(ImageView* image, Touch *touch);

	//显示建筑物选中效果
	void showSelectedEffect(UIBuilding* building);

	//隐藏建筑物选中效果
	void removeSelectedEffect(UIBuilding* building);

private:

	UIPerspect mUI;

	//当前点击的建筑物
	UIBuilding* mTouchBuilding;

	//记录下当前滚动百分比
	static float mScrollPercent;
	static bool mIsInitScrollPercent;

	//阳光图片
	Sprite* mSunShineSprite;

};




#endif //PerspectLayer_h__