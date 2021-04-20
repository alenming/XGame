#ifndef MainTaskLayer_h__
#define MainTaskLayer_h__

#include "cocos2d.h"
#include "UI/ModalLayer.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "FormationModule/CustomScrollBar.h"
#include "Utils/Observer.h"
#include "MainTaskModel.h"

using namespace cocostudio;
using namespace ui;

USING_NS_CC;


class MainTaskNode;

class MainTaskLayer : public ModalLayer, public Observer
{

public:

	enum TAB_TAG
	{
		TAB_NONE,
		TAB_DALY,		//日常任务
		TAB_ACHIEVEMENT	//成就任务
	};

	enum BTN_TAG
	{
		CLOSE,			//关闭按钮
		DALIY,			//日常任务按钮
		ACHIEVEMENT		//成就任务按钮
	};

	MainTaskLayer();
	virtual ~MainTaskLayer();

	CREATE_FUNC(MainTaskLayer);

public:

	//初始化
	virtual bool init();

	virtual void onEnter();
	virtual void onExit(); 

	//更新
	virtual void updateSelf(void* data);

private:

	//初始化UI
	void initUI();

	//初始化列表
	void initList();

	//更新小红点标记
	void updateTaskFlag();

	//按钮点击
	void onBtnClicked(Ref* target, Widget::TouchEventType eventType, BTN_TAG btnTag);

	//前往按钮点击
	void onGotoClicked(Ref* target, Widget::TouchEventType eventType, int jumpScene);

	//领取按钮点击
	void onReceiveClicked(Ref* target, Widget::TouchEventType eventType, int taskId);

	//滚动事件响应
	void onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	//切换tab
	void switchTab(TAB_TAG tabTag, bool isInitList = true);

	//添加奖励信息
	void addRewardInfo(MainTaskNode* taskNode, MainTaskParam* taskParam);

	//创建奖励领取弹窗
	void showPopReward(MainTaskParam* taskParam);

private:

	//日常任务按钮
	ImageView* mDailyBrightImg;
	ImageView* mDailyGrayImg;
	ImageView* mDailyFlag;

	//成就任务按钮
	ImageView* mAchievementBrightImg;
	ImageView* mAchievementGrayImg;
	ImageView* mAchievementFlag;
	
	//关闭按钮
	Button* mCloseBtn;

	//滚动面板
	ScrollView* mScrollPanel;

	//滚动条
	CustomScrollBar* mScrollBar;

	//当前标签页
	TAB_TAG mTabTag;

	//列表是否需要播放动画
	bool m_bIsNeedListAct;
};

class MainTaskNode : public Button
{

public:

	MainTaskNode();
	virtual ~MainTaskNode();

	static MainTaskNode* create(const std::string& normalImage, const std::string& selectedImage = "");

public:

	virtual bool init(const std::string& normalImage, const std::string& selectedImage = "");

private:

	void initUI();


public:

	//任务id
	int mTaskId;

	//任务类型
	int mTaskType;

	//任务名称
	Text* mTaskName;

	//任务完成进度
	Text* mTaskNum;

	//任务描述
	Text* mTaskDes;

	//领体力
	ImageView* mReceivePower;

	//领奖励
	ImageView* mReceiveReward;

	//时间未到
	ImageView* mTimeNot;

	//前往按钮
	Button* mGotoBtn;

	//图标位置
	Layout* mIconPanel;

	//奖励位置
	Layout* mRewardPanel;

};

#endif //MainTaskLayer_h__