#include "MainTaskModel.h"
#include "HeroModule/HeroModel.h"
#include "Utils/StringFormat.h"
#include "MainModule/MainModel.h"

MainTaskModel* MainTaskModel::mInstance = nullptr;


MainTaskModel::MainTaskModel()
{
	initFromLocal();
	LocalTimer::getInstance()->addHourListener(this);
}

MainTaskModel::~MainTaskModel()
{
	LocalTimer::getInstance()->removeHourListener(this);
}

MainTaskModel* MainTaskModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new MainTaskModel();
	}
	return mInstance;
}

void MainTaskModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void MainTaskModel::initFromLocal()
{
	multimap<int, RowData>& taskMap = DataManager::getInstance()->getDTTask()->getTableData();

	//���������
	for (auto iter = taskMap.begin(); iter != taskMap.end(); iter++)
	{
		RowData* rowData = &(iter->second);

		//������¿�(28)����(45)���� ���� δ�����¿����ܣ������񲻷����б�
		if(!MainModel::getInstance()->getMonthCardIsOpen())
		{
			int refType = rowData->getIntData("refTyp");
			if(refType == 28 || refType == 45)
			{
				continue;
			}
		}

		MainTaskParam* taskParam = MainTaskParam::create();

		taskParam->mTaskId = rowData->getIntData("id");
		taskParam->mTaskType = rowData->getIntData("missTyp");
		taskParam->mTaskCondType = rowData->getIntData("refTyp");
		taskParam->mTaskOrder = rowData->getIntData("disPlay");
		taskParam->mTaskCondMaxAmount = rowData->getIntData("amount1");
		taskParam->mTaskCondMaxAmount2 = rowData->getIntData("amount2");
		taskParam->mTaskShowTime = rowData->getIntData("avTime");
		taskParam->mTaskName = rowData->getStringData("missName");
		taskParam->mTaskDes = rowData->getStringData("missInfo");
		taskParam->mTaskResId = rowData->getIntData("resId");
		taskParam->mCoin = rowData->getIntData("coin");
		taskParam->mGold = rowData->getIntData("gold");
		taskParam->mExp = rowData->getIntData("exp");
		taskParam->mPower = rowData->getIntData("strength");
		taskParam->mPriorTaskId = rowData->getIntData("exId");
		taskParam->mShowLevel = rowData->getIntData("avLevel");
		taskParam->mShowProcess = rowData->getIntData("finish");
		taskParam->mJumpScene = rowData->getIntData("interface");

		string itemInfo = rowData->getStringData("item");
		initTaskRewardData(taskParam, itemInfo);

		mVecTaskParam.pushBack(taskParam);
	}
}

void MainTaskModel::initTaskRewardData(MainTaskParam* taskParam, string strItem)
{
	taskParam->mVecRewardInfo.clear();
	if (!strItem.empty())
	{
		vector<string> vecStr;
		StringFormat::parseCsvStringVec(strItem, vecStr);

		for (string strTmp : vecStr)
		{
			int nIndex1 = strTmp.find_first_of(',', 0);
			string strTmp1 = strTmp.substr(0, nIndex1);
			string strTmp2 = strTmp.erase(0, nIndex1 + 1);
			int itemId = atoi(strTmp1.c_str());
			int itemCount = atoi(strTmp2.c_str());
			if (itemId > 0 && itemCount > 0)
			{
				TaskRewardInfo taskRewardInfo;
				taskRewardInfo.mItemId = itemId;
				taskRewardInfo.mItemCount = itemCount;

				taskParam->mVecRewardInfo.push_back(taskRewardInfo);
			}
		}
	}
}

void MainTaskModel::initFromSvr(const Json::Value& taskListData)
{
	Json::Value finishTaskList = taskListData["finishList"];
	Json::Value activeTaskList = taskListData["activeList"];
	Json::Value doinTaskList = taskListData["doinList"];

	for (int i = 0; i < finishTaskList.size();i++)
	{
		int taskId = finishTaskList[i].asInt();
		MainTaskParam* taskParam = searchTaskParam(taskId);
		if (taskParam)
		{
			taskParam->mTaskState = TASK_STATE_DONE;
			taskParam->mTaskCondCurAmount = taskParam->mTaskCondMaxAmount;
		}
	}

	for (int i = 0; i < activeTaskList.size();i++)
	{
		int taskId = activeTaskList[i].asInt();
		MainTaskParam* taskParam = searchTaskParam(taskId);
		if (taskParam)
		{
			taskParam->mTaskState = TASK_STATE_ACTIVE;
			taskParam->mTaskCondCurAmount = taskParam->mTaskCondMaxAmount;
		}
	}

	for (int i = 0; i < doinTaskList.size();i++)
	{
		int taskId = doinTaskList[i]["id"].asInt();
		int amount = doinTaskList[i]["amount"].asInt();
		MainTaskParam* taskParam = searchTaskParam(taskId);
		if (taskParam)
		{
			taskParam->mTaskState = TASK_STATE_NONE;
			taskParam->mTaskCondCurAmount = amount;
		}
	}

	//��vip�ȼ����ܷ����仯�������ڴ˴����ʼ��vip����������
	for (MainTaskParam* taskParam : mVecTaskParam)
	{
		if (taskParam && taskParam->mTaskCondType == TASK_COND_VIP)
		{
			int vipLvl = MainModel::getInstance()->getMainParam()->mVipLevel;
			vipLvl = vipLvl == 0 ? 1 : vipLvl; //vip0��ʾvip1�Ľ���
			RowData* rowData = DataManager::getInstance()->searchVipDescriptionById(vipLvl);
			string itemInfo = rowData->getStringData("taskitem");
			initTaskRewardData(taskParam, itemInfo);
		}
	}

	//֪ͨ��ʼ��
	notify(TASK_EVENT_INIT);
}


void MainTaskModel::updateFromSvr(const Json::Value& updateData)
{
	for (int i = 0; i < updateData.size();i++)
	{
		int taskId = updateData[i].asInt();
		MainTaskParam* taskParam = searchTaskParam(taskId);
		if(taskParam != nullptr)		//�¿�������ܱ�����
		{
			taskParam->mTaskState = TASK_STATE_ACTIVE;
			taskParam->mTaskCondCurAmount = taskParam->mTaskCondMaxAmount;
		}
	}

	//֪ͨ����
	notify(TASK_EVENT_UPDATE);
}

void MainTaskModel::doneTask(int taskId)
{
	auto taskParam = searchTaskParam(taskId);
	taskParam->mTaskState = TASK_STATE_DONE;

	//֪ͨ����
	notify(TASK_EVENT_RECEIVE, taskParam);
}

MainTaskParam* MainTaskModel::searchTaskParam(int taskId)
{
	for (MainTaskParam* taskParam : mVecTaskParam)
	{
		if (taskParam->mTaskId == taskId)
		{
			return taskParam;
		}
	}
	return nullptr;
}

bool MainTaskModel::hasActivedTask(TASK_TYPE taskType)
{
	for (MainTaskParam* taskParam : mVecTaskParam)
	{
		//�ж��Ƿ���ָ������
		bool isTaskTypeSuit = false;
		switch (taskType)
		{
		case TASK_ALL:
			isTaskTypeSuit = true;
			break;
		case TASK_TYPE_ACHIEVEMENT:
		case TASK_TYPE_DAILY:
			isTaskTypeSuit = (taskParam->mTaskType == taskType);
			break;
		default:
			break;
		}

		//�ж��Ƿ�ﵽ��ʾ�ȼ�
		int currentLevel = HeroModel::getInstance()->getMainHero()->mLevel;
		bool isReachLevel = (currentLevel >= taskParam->mShowLevel);

		//�ж�ǰ�������Ƿ����
		MainTaskParam* priorTask = searchTaskParam(taskParam->mPriorTaskId);
		bool priorTaskDone = (priorTask == nullptr || priorTask->mTaskState == TASK_STATE_DONE);

		//���������������������񼤻�״̬����ǰ��������ɡ��Ҵﵽ��ʾ�ȼ��������ж�����
		if (isTaskTypeSuit && taskParam->mTaskState == TASK_STATE_ACTIVE && priorTaskDone && isReachLevel)
		{
			return true;
		}
	}
	return false;
}

Vector<MainTaskParam*>* MainTaskModel::getTaskList()
{
	return &mVecTaskParam;
}

vector<MainTaskParam*> MainTaskModel::getTaskList(TASK_TYPE taskType)
{
	vector<MainTaskParam*> tmpTaskList;

	for (MainTaskParam* taskParam : mVecTaskParam)
	{
		//�ж�ǰ�������Ƿ����
		MainTaskParam* priorTask = searchTaskParam(taskParam->mPriorTaskId);
		bool priorTaskDone = (priorTask == nullptr || priorTask->mTaskState == TASK_STATE_DONE);

		//�ж��Ƿ�ﵽ��ʾ�ȼ�
		int currentLevel = HeroModel::getInstance()->getMainHero()->mLevel;
		bool isReachLevel = (currentLevel >= taskParam->mShowLevel);

		//��������ƥ�� �� ����δ��� �� ����ǰ��������� �� �ﵽ��ʾ�ȼ��������������ʾ
		if (taskParam->mTaskType == taskType && taskParam->isUndoneTask() &&
			priorTaskDone && isReachLevel && taskParam->isReachShowTime())
		{
			tmpTaskList.push_back(taskParam);
		}
	}

	std::sort(tmpTaskList.begin(), tmpTaskList.end(), 
		[](MainTaskParam* p1, MainTaskParam* p2)->bool
	{
		//�Ȱ�����״̬�����ٰ����ȼ�����
		if (p1->mTaskState != p2->mTaskState)
		{
			return p1->mTaskState > p2->mTaskState;
		}
		else
		{
			return p1->mTaskOrder < p2->mTaskOrder;
		}

	});

	return tmpTaskList;
}

void MainTaskModel::hourTick(int hour)
{
	for (MainTaskParam* taskParam : mVecTaskParam)
	{
		if(taskParam->isGetPowerTask())
		{
			//����ʾʱ�䵫��δ������ʱ��
			if (LocalTimer::getInstance()->isTimeOut(taskParam->mTaskShowTime) &&
				!LocalTimer::getInstance()->isTimeOut(taskParam->mTaskCondMaxAmount))
			{
				//����UI
				notify(TASK_EVENT_UPDATE);
			}
			//������ʱ���
			else if (LocalTimer::getInstance()->isTimeOut(taskParam->mTaskCondMaxAmount) &&
				!LocalTimer::getInstance()->isTimeOut(taskParam->mTaskCondMaxAmount2))
			{
				if (taskParam->mTaskState == TASK_STATE_NONE)
				{
					taskParam->mTaskState = TASK_STATE_ACTIVE;
					notify(TASK_EVENT_UPDATE);
				}
			}
			//����ʱ���
			else
			{
				if (taskParam->mTaskState == TASK_STATE_ACTIVE)
				{
					taskParam->mTaskState = TASK_STATE_NONE;
					notify(TASK_EVENT_UPDATE);
				}
			}
			
		}
	}
}

void MainTaskModel::notify(TASK_EVENT_TYPE eventType, void* data)
{
	ObserverParam observerParam;
	observerParam.id = eventType;
	observerParam.updateData = data;
	notifyObservers(&observerParam);
}

///////////////////////////////�����///////////////////////////////////////////////


MainTaskParam* MainTaskParam::create()
{
	MainTaskParam* taskParam = new MainTaskParam();
	taskParam->autorelease();
	return taskParam;
}


bool MainTaskParam::isUndoneTask()
{
	return mTaskState != TASK_STATE_DONE;
}

bool MainTaskParam::isActivedTask()
{
	return mTaskState == TASK_STATE_ACTIVE;
}

bool MainTaskParam::hasProgress()
{
	return mShowProcess;
}

bool MainTaskParam::isGetPowerTask()
{
	return mTaskCondType == TASK_COND_GET_TIMER_POWER;
}

bool MainTaskParam::hasGoto()
{
	return mJumpScene > 0;
}

bool MainTaskParam::isReachShowTime()
{
	if (isGetPowerTask())
	{
		//��ǰ��Сʱ��ʾ
		return LocalTimer::getInstance()->isTimeOut(mTaskShowTime) && 
			!LocalTimer::getInstance()->isTimeOut(mTaskCondMaxAmount2);
	}
	else
	{
		return true;
	}
}

