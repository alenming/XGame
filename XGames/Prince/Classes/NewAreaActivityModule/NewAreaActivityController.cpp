#include "NewAreaActivityController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "NewAreaActivityModel.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "SDK_TalkingData/TalkingData.h"

NewAreaActivityController* NewAreaActivityController::mInstance = nullptr;

NewAreaActivityController::NewAreaActivityController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

NewAreaActivityController::~NewAreaActivityController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void NewAreaActivityController::sendGetMainInfoMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GET_TASK_NEW_INFO, "");
}

NewAreaActivityController* NewAreaActivityController::getInstance()
{
	if (nullptr == mInstance)
	{
		mInstance = new NewAreaActivityController;
	}
	return mInstance;
}

void NewAreaActivityController::destroyInstance()
{
	if (nullptr != mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void NewAreaActivityController::sendGetRewardMsg(int rewardId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["taskId"] = rewardId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GET_TASK_NEW_REWARD, json_file.c_str());
}

void NewAreaActivityController::parseGetMainInfoMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		
		NewAreaActivityModel::getInstance()->initFromSvr(data);

		//通知UI
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_GET_TASK_NEW_INFO;
		NewAreaActivityModel::getInstance()->notifyObservers(&obParam);
	}
}

void NewAreaActivityController::parseGetRewardMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		//记录购买特价礼包付费点
		sTeJiaBox tjBox = NewAreaActivityModel::getInstance()->getCurBuyBox();
		if(tjBox.isBox)
		{
			string des = DataManager::getInstance()->searchCommonTexdtById(20259) + _TO_STR(tjBox.days) +
				DataManager::getInstance()->searchCommonTexdtById(20260);
			TalkingData::onPurchase(des, 1, tjBox.price);
			NewAreaActivityModel::getInstance()->clearCurBuyBox();
		}	

		Json::Value data = root["data"];
		Json::Value changeInfo = data["changeInfo"];

		MainModel::getInstance()->updateData(changeInfo);

		//英雄数据
		Json::Value petLists = changeInfo["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = changeInfo["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);	

		for(int i=0; i<NewAreaActivityModel::getInstance()->m_vecNewAreaActData.size(); i++)
		{
			if(NewAreaActivityModel::getInstance()->m_vecNewAreaActData.at(i).actId == data["taskId"].asInt())
			{
				NewAreaActivityModel::getInstance()->m_vecNewAreaActData.at(i).isCanGet = false;
				NewAreaActivityModel::getInstance()->m_vecNewAreaActData.at(i).isGetted = true;
				NewAreaActivityModel::getInstance()->m_vecNewAreaActData.at(i).amount3 = data["buycount"].asInt();
			}
		}

		//通知UI
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_GET_TASK_NEW_REWARD;
		NewAreaActivityModel::getInstance()->notifyObservers(&obParam);
	}
}

void NewAreaActivityController::parseNewAreaActPushMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	NewAreaActivityModel* pInstance = NewAreaActivityModel::getInstance();
	if (read.parse(szData, root)) 
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		Json::Value taskList = data["taskList"];
		NewAreaActivityModel::getInstance()->updateFromSvr(taskList);
	}
}

void NewAreaActivityController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_GET_TASK_NEW_INFO:
		parseGetMainInfoMsg(szData);
		break;
	case nMAIN_CMD_GET_TASK_NEW_REWARD:
		parseGetRewardMsg(szData);
		break;
	case nMAIN_CMD_NEWAREA_ACT_PUSH_INFO:
		parseNewAreaActPushMsg(szData);
		break;
	default:
		break;
	}
}