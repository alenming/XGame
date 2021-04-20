#include "FirstRechargeController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Utils/Observer.h"
#include "FirstRechargeModel.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"


FirstRechargeController::FirstRechargeController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

FirstRechargeController::~FirstRechargeController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

//发送获取随机名称指令
void FirstRechargeController::sendGetRewardMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FIRST_RECHARGE, "");
}

//解析随机名称数据
void FirstRechargeController::parseGetRewardMsg(const char* szData)
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

		Json::Value changeInfo = data["changeInfo"];

		MainModel::getInstance()->updateData(changeInfo);

		//英雄数据
		Json::Value petLists = changeInfo["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = changeInfo["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);	

		FirstRechargeModel::getInstance()->mFirstRechargeState = 2;

		//通知UI
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_FIRST_RECHARGE;
		FirstRechargeModel::getInstance()->notifyObservers(&obParam);
	}
}

void FirstRechargeController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_FIRST_RECHARGE:
		parseGetRewardMsg(szData);
		break;
	default:
		break;
	}
}