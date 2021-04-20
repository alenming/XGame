#include "HeroController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainModel.h"


HeroController::HeroController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

HeroController::~HeroController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void HeroController::sendHeroAdvanceMsg(int heroId)
{
	Json::FastWriter writer;
	Json::Value val;
	val["petId"] = heroId;
	std::string json_file = writer.write(val);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_HERO_ADVANCE_INFO, json_file.c_str());
}

void HeroController::sendHeroStarUpMsg(int heroId)
{
	Json::FastWriter writer;
	Json::Value val;
	val["petId"] = heroId;
	std::string json_file = writer.write(val);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_HERO_STAR_UP_INFO, json_file.c_str());
}

void HeroController::sendEquipPutOnMsg(int heroId, int toolId)
{
	Json::FastWriter writer;
	Json::Value val;
	val["petId"]	= heroId;
	val["itemId"]	= toolId;
	std::string json_file = writer.write(val);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOOL_PUTON, json_file.c_str());
}

void HeroController::sendUseExpItemMsg(int heroId, int templetId, int num)
{
	Json::FastWriter writer;
	Json::Value root;
	root["petId"] = heroId;
	root["itemList"] = Json::Value();
	root["itemList"]["id"] = templetId;
	root["itemList"]["amount"] = num;
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAINING_ADD_EXP, json_file.c_str());
}

void HeroController::sendExchangeChipsMsg(int heroId, int num)
{
	Json::FastWriter writer;
	Json::Value root;
	root["petId"] = heroId;
	root["count"] = num;
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_EXCHANGE_CHIPS, json_file.c_str());
}

void HeroController::parseHeroAdvanceMsg(const char* szData)
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

		//更新主角数据
		MainModel::getInstance()->updateData(data);
		
		//道具数据更新
		Json::Value itemList = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(itemList);

		//英雄数据更新
		Json::Value heroList = data["petList"];
		HeroModel::getInstance()->updateData(heroList);

		//通知UI数据更新
		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_HERO_ADVANCE_INFO;
		HeroModel::getInstance()->notifyObservers(&observerParam);
	}
}

void HeroController::parseHeroStarUpMsg(const char* szData)
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

		//更新主角数据
		MainModel::getInstance()->updateData(data);

		//道具数据更新
		Json::Value itemList = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(itemList);

		//英雄数据更新
		Json::Value heroList = data["petList"];
		HeroModel::getInstance()->updateData(heroList);

		//通知UI数据更新
		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_HERO_STAR_UP_INFO;
		HeroModel::getInstance()->notifyObservers(&observerParam);
	}
}

void HeroController::parseEquipPutOnMsg(const char* szData)
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
		//更新主角数据
		MainModel::getInstance()->updateData(data);
		//道具数据更新
		Json::Value itemList = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(itemList);

		//英雄数据更新
		Json::Value heroList = data["petList"];
		HeroModel::getInstance()->updateData(heroList);

		int i = 0;
		int equipId = itemList.size() > 0 ? itemList[i]["id"].asInt() : -1;
		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_TOOL_PUTON;
		observerParam.updateData = &equipId;
		HeroModel::getInstance()->notifyObservers(&observerParam);
	}
}

void HeroController::parseUseExpItemMsg(const char* szData)
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
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items);

		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_TRAINING_ADD_EXP;
		HeroModel::getInstance()->notifyObservers(&observerParam);
	}
}

void HeroController::parseExchangeChipsMsg(const char* szData)
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
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items);

		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_EXCHANGE_CHIPS;
		HeroModel::getInstance()->notifyObservers(&observerParam);
	}
}

void HeroController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_HERO_ADVANCE_INFO:
		parseHeroAdvanceMsg(szData);
		break;
	case nMAIN_CMD_HERO_STAR_UP_INFO:
		parseHeroStarUpMsg(szData);
		break;
	case nMAIN_CMD_TOOL_PUTON:
		parseEquipPutOnMsg(szData);
		break;
	case nMAIN_CMD_TRAINING_ADD_EXP:
		parseUseExpItemMsg(szData);
		break;
	case nMAIN_CMD_EXCHANGE_CHIPS:
		parseExchangeChipsMsg(szData);
		break;
	default:
		break;
	}
	return;
}