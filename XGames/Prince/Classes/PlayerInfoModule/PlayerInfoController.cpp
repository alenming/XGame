#include "PlayerInfoController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Utils/Observer.h"
#include "PlayerInfoModule/PlayerInfoModel.h"
#include "MainModule/MainModel.h"
#include "ToolModule/ToolModel.h"
#include "SDK_TalkingData/TalkingData.h"


PlayerInfoController::PlayerInfoController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

PlayerInfoController::~PlayerInfoController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

//发送获取随机名称指令
void PlayerInfoController::sendGetRandomNameMsg(int sex)
{
	Json::FastWriter writer;
	Json::Value data;
	data["sex"] = sex;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_RANDOM_NAMES, json_file.c_str());
}

//发送修改昵称指令
void PlayerInfoController::sendModifyNickNameMsg(string str)
{
	Json::FastWriter writer;
	Json::Value data;
	data["nickname"] = str.c_str();
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_PLAYER_INFO_MODIFY_NAME, json_file.c_str());
}

//发送兑换指令
void PlayerInfoController::sendExchangeMsg(string key)
{
	Json::FastWriter writer;
	Json::Value data;
	data["id"] = key.c_str();
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_PLAYER_INFO_CDKEY, json_file.c_str());
}

//解析随机名称数据
void PlayerInfoController::parseGetRandomNameMsg(const char* szData)
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

		vecRandomNames.clear();
		Json::Value randomNames = data["nameList"];
		for (size_t i = 0; i < randomNames.size(); i++)
		{
			vecRandomNames.push_back(randomNames[i].asString());
		}
		
		//通知UI更新名字列表
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_RANDOM_NAMES;
		PlayerInfoModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析服务器修改昵称返回的数据(花费)
void PlayerInfoController::parseModifyNickNameMsg(const char* szData)
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
		Json::Value goldNum = data["getGold"];

		int gold = goldNum.asInt();
		MainModel::getInstance()->setGold(gold);

		//上报TalkingData，修改昵称消耗的金币
		RowData* priceData = DataManager::getInstance()->getDTToolSystem()->searchDataById(10090);
		MainParam* pMainParam = MainModel::getInstance()->getMainParam();
		if (pMainParam != nullptr)
		{
			string des = DataManager::getInstance()->searchCommonTexdtById(20223);
			int price = priceData->getIntData("value");
			TalkingData::onPurchase(des, 1, price);
		}

		//通知UI, 修改昵称需要花费的金币
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_PLAYER_INFO_MODIFY_NAME;
		PlayerInfoModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析兑换结果
void PlayerInfoController::parseExchangeMsg(const char* szData)
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

		vector<TOOL_NEW_INFO>* vecNewTools = PlayerInfoModel::getInstance()->getNewTools();
		vecNewTools->clear();

		//记录更新前金币银币
		int goldNum = MainModel::getInstance()->getMainParam()->mGold;
		int coinNum = MainModel::getInstance()->getMainParam()->mCoin;

		MainModel::getInstance()->updateData(changeInfo);

		//金币银币奖励
		if(MainModel::getInstance()->getMainParam()->mGold > goldNum)
		{
			TOOL_NEW_INFO goldItem;
			goldItem.nid = -1;
			goldItem.ntemplateId = 11001;
			goldItem.nstack = MainModel::getInstance()->getMainParam()->mGold - goldNum;
			vecNewTools->push_back(goldItem);
		}
		if(MainModel::getInstance()->getMainParam()->mCoin > coinNum)
		{
			TOOL_NEW_INFO coinItem;
			coinItem.nid = -1;
			coinItem.ntemplateId = 11002;
			coinItem.nstack = MainModel::getInstance()->getMainParam()->mCoin - coinNum;
			vecNewTools->push_back(coinItem);
		}

		//英雄数据
		Json::Value petLists = changeInfo["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//道具数据
		Json::Value items = changeInfo["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, vecNewTools);

		//通知UI, 兑换结果
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_PLAYER_INFO_CDKEY;
		PlayerInfoModel::getInstance()->notifyObservers(&obParam);
	}
}

void PlayerInfoController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_PLAYER_INFO_MODIFY_NAME:
		parseModifyNickNameMsg(szData);
		break;
	case nMAIN_CMD_RANDOM_NAMES:
		{
			parseGetRandomNameMsg(szData);
		}
		break;
	case nMAIN_CMD_PLAYER_INFO_CDKEY:
		parseExchangeMsg(szData);
		break;
	default:
		break;
	}
}