#include "ToolController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/HttpCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Json/json.h"
#include "../HeroModule/HeroModel.h"
#include "Temp/CustomTips.h"
#include "Common/Common.h"
#include "ToolDef.h"
#include "ToolModel.h"
#include "../MainModule/MainModel.h"
#include "cocos2d.h"
using namespace cocos2d;

ToolController::ToolController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

ToolController::~ToolController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

//使用道具
void ToolController::sendUseToolsMsg(const int id, int num)
{
	Json::FastWriter  writer;
	Json::Value toolId;
	toolId["itemId"] = id;
	toolId["amount"] = num;
	std::string  json_file = writer.write(toolId);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOOL_USE, json_file.c_str());
}

//开宝箱
void ToolController::sendOpenChestMsg(const int id)
{		
	Json::FastWriter  writer;
	Json::Value toolId;
	toolId["itemId"] = id;
	std::string  json_file = writer.write(toolId);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_OPEN_CHEST, json_file.c_str());
}

void ToolController::sendSellToolsMsg(const int& id, const int& amount)
{
	Json::FastWriter  writer;
	Json::Value vParam;
	vParam["itemId"]			= id;
	vParam["amount"]		= amount;
	std::string  json_file=writer.write(vParam);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_RECYCLE_SHOP_TOOLS, json_file.c_str());
}

//使用道具
void ToolController::parseUseToolsMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		int nResult = root["result"].asInt();
		string strMsg = root["msg"].asString();
		Json::Value data = root["data"];
		//
		MainModel::getInstance()->updateData(data);
		//英雄数据
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);
		
		//itemList
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items);

		int useCount = data["amount"].asInt();
		
		//如果使用的是宝箱
		if(data["itemTyp"].asInt() == TOOL_ITEM_TYPE_CHEST)
		{
			ToolModel::getInstance()->setIsUsedBox(true);
			Json::Value showItems = data["showItem"];
			ToolModel::getInstance()->setUsedBoxInfo(showItems);
		}		

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_TOOL_USE;
		obParam.updateData = &useCount;
		ToolModel::getInstance()->notifyObservers(&obParam);
	}
}

void ToolController::parseSellToolsMsg(const char* szData)
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
		//
		MainModel::getInstance()->updateData(data);

		//英雄数据
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);		
		//
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items);

		int sellCount = data["amount"].asInt();

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_RECYCLE_SHOP_TOOLS;
		obParam.updateData = &sellCount;
		ToolModel::getInstance()->notifyObservers(&obParam);
	}
}

//打开宝箱
void ToolController::parseOpenChestMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data	= root["data"];
		//
		MainModel::getInstance()->updateData(data);

		//英雄数据
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);		
		//
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items);

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_OPEN_CHEST;
		ToolModel::getInstance()->notifyObservers(&obParam);
	}
}

void ToolController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_TOOL_USE://使用道具
		parseUseToolsMsg(szData);
		break;
	case nMAIN_CMD_RECYCLE_SHOP_TOOLS:  //回收道具
		parseSellToolsMsg(szData);
		break;
	case nMAIN_CMD_OPEN_CHEST:
		parseOpenChestMsg(szData);
		break;
	default:
		break;
	}
}

