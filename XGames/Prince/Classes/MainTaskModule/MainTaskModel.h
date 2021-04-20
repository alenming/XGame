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

	//����ID
	int mItemId;

	//��������
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

	//�Ƿ���δ�������ע����������Ҳ��δ�������
	bool isUndoneTask();

	//�Ƿ��Ǽ�������
	bool isActivedTask();

	//�Ƿ���ʾ����
	bool hasProgress();

	//�Ƿ�������������
	bool isGetPowerTask();

	//�Ƿ���ǰ����ť
	bool hasGoto();

	//�Ƿ���ʾʱ�䣬ע��ֻ��TASK_COND_GET_TIMER_POWER���͵�������Ч
	bool isReachShowTime();

public:

	//����id
	int mTaskId;

	//ǰ������id
	int mPriorTaskId;

	//��ʾ�ȼ�
	int mShowLevel;

	//����������
	int mTaskType;

	//������������
	int mTaskCondType;

	//��������
	int mTaskOrder;

	//����������������ֵ
	int mTaskCondMaxAmount;

	//����������������ֵ2
	int mTaskCondMaxAmount2;

	//������ʾʱ�䣬������������ȡ����
	int mTaskShowTime;

	//������ɵ�ǰֵ
	int mTaskCondCurAmount;

	//����״̬
	TASK_STATE mTaskState;

	//��������
	string mTaskName;

	//��������
	string mTaskDes;

	//������Դid
	int mTaskResId;

	//�������ҽ���
	int mCoin;

	//�����ҽ���
	int mGold;

	//�����齱��
	int mExp;

	//������������
	int mPower;

	//�Ƿ���ʾ����
	bool mShowProcess;

	//��ת����
	int mJumpScene;

	//���߽����б�
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

	//�������ݳ�ʼ��
	void initFromLocal();

	//��ʼ������������
	void initTaskRewardData(MainTaskParam* taskParam, string strItem);

	//��������ݳ�ʼ��
	void initFromSvr(const Json::Value& taskListData);

	//��������
	void updateFromSvr(const Json::Value& updateData);

	//���ָ������
	void doneTask(int taskId);

	//����ָ��������
	MainTaskParam* searchTaskParam(int taskId);

	//�����б����Ƿ��м�������
	bool hasActivedTask(TASK_TYPE taskType = TASK_ALL);

	//��ȡ�����б�
	Vector<MainTaskParam*>* getTaskList();

	//��ȡ�ճ������б�
	vector<MainTaskParam*> getTaskList(TASK_TYPE taskType);
	
	//ʱ�����
	virtual void hourTick(int hour);

	//֪ͨ������
	void notify(TASK_EVENT_TYPE eventType, void* data = nullptr);

private:

	static MainTaskModel* mInstance;

	//�����б�
	Vector<MainTaskParam*> mVecTaskParam;

};



#endif //MainTaskModel_h__