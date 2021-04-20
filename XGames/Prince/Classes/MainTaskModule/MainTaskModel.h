#ifndef MainTaskModel_h__
#define MainTaskModel_h__

#include "Utils/Observer.h"
#include "DataManager/DataManager.h"
#include "Communication/Json/json.h"
#include "cocos2d.h"
#include "MainTaskDef.h"
#include "Utils/LocalTimer.h"

struct TaskRewardInfo
{

	//道具ID
	int mItemId;

	//道具数量
	int mItemCount;

};

class MainTaskParam : public Ref
{

public:

	MainTaskParam() 
		: mTaskId(-1)
		, mPriorTaskId(-1)
		, mShowLevel(-1)
		, mTaskType(-1)
		, mTaskCondType(-1)
		, mTaskOrder(-1)
		, mTaskCondMaxAmount(-1)
		, mTaskCondCurAmount(0)
		, mTaskShowTime(0)
		, mTaskState(TASK_STATE_NONE)
		, mTaskName("")
		, mTaskDes("")
		, mTaskResId(-1)
		, mCoin(0)
		, mGold(0)
		, mExp(0)
		, mPower(0)
		, mShowProcess(false)
		, mJumpScene(-1)
	{

	}
		
	virtual ~MainTaskParam(){ };

	static MainTaskParam* create();

public:

	//是否是未完成任务，注：激活任务也算未完成任务
	bool isUndoneTask();

	//是否是激活任务
	bool isActivedTask();

	//是否显示进度
	bool hasProgress();

	//是否是领体力任务
	bool isGetPowerTask();

	//是否有前往按钮
	bool hasGoto();

	//是否到显示时间，注：只有TASK_COND_GET_TIMER_POWER类型的任务有效
	bool isReachShowTime();

public:

	//任务id
	int mTaskId;

	//前置任务id
	int mPriorTaskId;

	//显示等级
	int mShowLevel;

	//任务主类型
	int mTaskType;

	//任务条件类型
	int mTaskCondType;

	//任务序列
	int mTaskOrder;

	//任务完成条件最大数值
	int mTaskCondMaxAmount;

	//任务完成条件最大数值2
	int mTaskCondMaxAmount2;

	//任务显示时间，仅限于体力领取任务
	int mTaskShowTime;

	//任务完成当前值
	int mTaskCondCurAmount;

	//任务状态
	TASK_STATE mTaskState;

	//任务名称
	string mTaskName;

	//任务描述
	string mTaskDes;

	//任务资源id
	int mTaskResId;

	//任务银币奖励
	int mCoin;

	//任务金币奖励
	int mGold;

	//任务经验奖励
	int mExp;

	//任务体力奖励
	int mPower;

	//是否显示进度
	bool mShowProcess;

	//跳转界面
	int mJumpScene;

	//道具奖励列表
	vector<TaskRewardInfo> mVecRewardInfo;

};

class MainTaskModel : public Observable, public HourListener
{

public:

	MainTaskModel();
	virtual ~MainTaskModel();

public:

	static MainTaskModel* getInstance();
	static void destroyInstance();

public:

	//本地数据初始化
	void initFromLocal();

	//初始化任务奖励数据
	void initTaskRewardData(MainTaskParam* taskParam, string strItem);

	//服务端数据初始化
	void initFromSvr(const Json::Value& taskListData);

	//更新数据
	void updateFromSvr(const Json::Value& updateData);

	//完成指定任务
	void doneTask(int taskId);

	//查找指定的任务
	MainTaskParam* searchTaskParam(int taskId);

	//任务列表中是否有激活任务
	bool hasActivedTask(TASK_TYPE taskType = TASK_ALL);

	//获取任务列表
	Vector<MainTaskParam*>* getTaskList();

	//获取日常任务列表
	vector<MainTaskParam*> getTaskList(TASK_TYPE taskType);
	
	//时间更新
	virtual void hourTick(int hour);

	//通知监听者
	void notify(TASK_EVENT_TYPE eventType, void* data = nullptr);

private:

	static MainTaskModel* mInstance;

	//任务列表
	Vector<MainTaskParam*> mVecTaskParam;

};



#endif //MainTaskModel_h__