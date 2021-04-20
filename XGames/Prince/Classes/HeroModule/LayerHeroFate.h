#ifndef LayerHeroFate_h__
#define LayerHeroFate_h__

#include "UI/ModalLayer.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolDef.h"
#include "HeroModel.h"
#include "HeroFateController.h"
#include "Utils/Observer.h"

//
const int MAX_HERO_FATE_COUNT = 8;
struct  UI_HeroFate
{
	// UI根节点层容器
	cocos2d::ui::Layout		*pRoot;

	cocos2d::ui::Layout		*pLayHeros;
	cocos2d::ui::Layout		*pLayTiedFates;

	//英雄
	cocos2d::ui::ScrollView		*pSvHeros;
	//
	cocos2d::ui::Button			*pBtnLeft;
	cocos2d::ui::Button			*pBtnRight;
	//结缘
	//缘分
	cocos2d::ui::Layout			*pLayFates[MAX_HERO_FATE_COUNT];
	UIToolHeader				*pHeroFates[MAX_HERO_FATE_COUNT];
	//缘分位置是否为空
	bool						posFatesIsSpace[MAX_HERO_FATE_COUNT];
	//缘分说明
	cocos2d::ui::ImageView		*pImgDes[MAX_HERO_FATE_COUNT];
	//主角
	cocos2d::ui::ImageView		*pImgMainRole;
	//提示
	cocos2d::ui::Text			*pTextTips;
	//刷新
	cocos2d::ui::Text			*pTextRefresh;
	cocos2d::ui::Text			*pTextRefreshCost;

	//结识、刷新、替换三个按钮的位置容器(按钮们居无定所，惨啊~)
	cocos2d::ui::Layout			*pBtnPnl1;
	cocos2d::ui::Layout			*pBtnPnl2;
	cocos2d::ui::Layout			*pBtnPnl3;

	//结识缘分按钮
	cocos2d::ui::Button			*pBtnTiedFates;
	//刷新缘分按钮
	cocos2d::ui::Button			*pBtnRefreshFates;
	//替换缘分按钮
	cocos2d::ui::Button			*pBtnReplaceFates;
};

class LayerHeroFate : public ModalLayer, public Observer
{

public:

	LayerHeroFate();
	virtual ~LayerHeroFate();

	CREATE_FUNC(LayerHeroFate);

public:

	virtual void updateSelf(void* data);

	//初始化
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

	//设置当前显示英雄的id
	void setCurrentHeroId(int heroId);

private:

	//初始化UI
	void initUI();

	//清空按钮容器
	void clearBtnPanel();

	void onBtnBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onBtnLeft(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onBtnRight(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	//
	void	onBtnCancel(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnTiedFates(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnRemoveFates(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int index);
	void	onBtnRefreshFates(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//上方英雄图标
	void	onBtnHeroClicked(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int heroId);
	//缘分英雄图标
	void	onBtnFateClicked(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int index, int heroId);

	//弹出框按钮事件
	void	onBtnReplaceBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnRefreshBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnRefreshRefresh(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnRefreshReplace(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void onScrollViewEvent(Ref* sender, ScrollviewEventType type);

	void  onCustomRetRemoveFates(Ref* pSender, CustomRetType type);
	//初始化英雄列表
	void initHeroList();	
	//显示缘分界面
	void showFates();

	//归还已上架但未结缘的英雄图标至上排滚动列表
	void cancelSelectedHeroIcon();

	//结识缘分结果处理
	void tiedFatesResult(ObserverParam* obparam);
	//刷新缘分结果处理
	void refreshFatesResult(ObserverParam* obparam);
	//替换缘分结果处理
	void replaceFatesResult(ObserverParam* obparam);
	//取消缘分结果处理
	void cancelFatesResult();

	//结缘成功后心形动画
	void playTiedFatesHeartAni();

	//已结缘的线形动画
	void playTiedFatesLineAni(int aniTag);

	//移除已结缘的线形动画
	void removeTiedFatesLineAni(int index);

	//根据属性类型获取属性名称
	string getFateAttriName(const string& attriType);

	//获取可结识缘分数:lvl 主角等级或者VIP等级
	int getCanTiedFatesCount(int lvl, int sysId);

	//获取缘分第一个空位
	int getFatesSpacepos(int nomalCount, int vipCount);
	//英雄增加为缘分
	void startHeroToFates(int heroId);
	void callBackHeroToFates(float dt);

	//判断当前选中的缘分是否刷新到了更高级数据同时还没替换的情况
	bool checkNeedReplaceFate();

private:

	HeroFateController mHeroFateController;

	//角色界面UI成员
	UI_HeroFate m_ui;

	//下方缘分选中的英雄ID
	int m_nCurHeroId;
	//当前选中位置
	int m_nCurIndex;

	//
	int m_nHeroToFatesHeroId;

	//显示英雄，中间3个放大显示
	int m_nDisHeroTotleCount;
	//开始显示位置
	int m_nDisHeroBeginIndex;
	//
	int m_nDisHeroBigBeginIndex;

	//滑动计数
	int m_nMoveCount;
};


#endif