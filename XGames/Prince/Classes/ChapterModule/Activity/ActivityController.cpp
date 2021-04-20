#include "ActivityController.h"
#include "Communication/Command.h"
#include "Communication/SocketCommunicator.h"
#include "ActivityModel.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "SDK_TalkingData/TalkingData.h"

ActivityController* ActivityController::m_pInstance = nullptr;

ActivityController::ActivityController(void)
{
	SocketCommunicator::getInstance()->addListener(this);
}

ActivityController::~ActivityController(void)
{
	SocketCommunicator::getInstance()->removeListener(this);
}

ActivityController* ActivityController::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new ActivityController;
	}
	return m_pInstance;
}

void ActivityController::destroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void ActivityController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_ACTIVITY:
		{
			procActivityData(szData);
		}
		break;

	case nMAIN_CMD_ACTIVITY_BUY:
		{
			procBuyActivityTimesResult(szData);
		}
		break;

	case nMAIN_CMD_ACTIVITY_SWEEP:
		{
			procActivitySweepResult(szData);
		}
		break;

	default:
		break;
	}
}

void ActivityController::handleActivityState()
{
	// 请求活动关卡状态数据;
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ACTIVITY, "");
}

void ActivityController::procActivityData(const char* szData)
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		Json::Value data = root["data"];
		unsigned int nPrice = data["unitPrice"].asInt();

		// 活动列表;
		Json::Value activitys = data["guankaList"];
		map<int, ActivityState> mapActivityState;
		for (int i = 0; i < activitys.size(); i++)
		{
			Json::Value activityData = activitys[i];
			ActivityState state;
			state.nActivityId = activityData["id"].asInt();
			state.nBuyCount = activityData["saleTimes"].asInt();	// 已购买次数;
			state.nValidCount = activityData["useableTimes"].asInt();
			state.nTotalCount = activityData["totalTimes"].asInt();

			// 关卡列表;
			Json::Value barriers = activityData["gkList"];
			for (int k = 0; k < barriers.size(); k++)
			{
				Json::Value barrier = barriers[k];
				state.mapBarrierRecord.insert(make_pair(barrier["id"].asInt(), barrier["star"].asInt()));
			}
			mapActivityState.insert(pair<int, ActivityState>(state.nActivityId, state));
		}
		// 更新数据;
		ActivityModel::getInstance()->updateActivityList(mapActivityState);
	}
}


int ActivityController::handleActivityChallenge( int nActivityId, int nActivityBarrierId )
{
	// 查询指定的活动关卡是否可用;
	return ActivityModel::getInstance()->isActivityBarrierValid(nActivityId, nActivityBarrierId);
}


void ActivityController::handleBuyActivityTimes( int nActivityId, int nCount /*= 1*/ )
{
	Json::FastWriter writer;
	Json::Value root;
	root["guankaId"] = nActivityId;
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ACTIVITY_BUY, json_file.c_str());
}


void ActivityController::procBuyActivityTimesResult(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		// 购买成功扣金币;
		int nActivityId = data["guankaId"].asInt();
		MainModel::getInstance()->setGold(data["gold"].asInt());
		MainModel::getInstance()->notifyObservers();
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10038), 1, ActivityModel::getInstance()->getBuyActivityCost(nActivityId));

		// 更新活动关卡状态;
		ActivityState state;
		ActivityModel::getInstance()->getActivityState(nActivityId, state);
		state.nValidCount++;
		state.nBuyCount++;
		ActivityModel::getInstance()->updateActivityState(state);

		// 购买成功;
		ActivityModel::getInstance()->onBuySuccess();
	}
}

void ActivityController::handleActivityBarrierSweep( int nActivityId, int nActivityBarrierId )
{
	Json::FastWriter writer;
	Json::Value root;
	root["guankaId"] = nActivityBarrierId;
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ACTIVITY_SWEEP, json_file.c_str());
}

void ActivityController::procActivitySweepResult( const char* szData )
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		// 更新活动关卡状态;
		int nActivityId = data["guankaId"].asInt();
		ActivityState state;
		ActivityModel::getInstance()->getActivityState(nActivityId, state);
		state.nValidCount--;
		ActivityModel::getInstance()->updateActivityState(state);

		// 更新道具;
		ToolModel::getInstance()->updateToolJsonData(data["itemList"]);

		// 更新Hero;
		HeroModel::getInstance()->updateData(data["petList"]);

		// 更新体力;
		MainModel::getInstance()->updateData(data);

		// 掉落物品<templateId, count>;
		map<int, int>  mapReward;
		Json::Value getItemList = data["getItemList"];
		for (int k = 0; k < getItemList.size(); k++)
		{
			mapReward.insert(pair<int, int>(getItemList[k]["id"].asInt(), getItemList[k]["amount"].asInt()));
		}
		ActivityModel::getInstance()->updateSweepResult(&mapReward);
	}
}
