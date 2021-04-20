#include "TacticsController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "ModelTactics.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"


TacticsController* TacticsController::ms_pInstance = nullptr;

TacticsController* TacticsController::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new TacticsController();
	}
	return ms_pInstance;
}

void TacticsController::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

TacticsController::TacticsController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

TacticsController::~TacticsController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}


//获取阵法信息
void TacticsController::sendGetTacticsListMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TACTICS_MAIN_LIST, "");
}

//使用阵法
void TacticsController::sendUseTacticsMsg(int matrixId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["matrixId"] = matrixId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TACTICS_USE_TACTICS, json_file.c_str());
}

//阵法精炼
void TacticsController::sendUpgradeTacticsMsg(int matrixId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["matrixId"] = matrixId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TACTICS_UPGRADE, json_file.c_str());
}

//排强化
void TacticsController::sendUpgradeRowMsg(vector<sRowUpMaterial> vecItemList, int rowId)
{
	Json::FastWriter writer;
	Json::Value data;

	for(int i=0; i<vecItemList.size(); i++)
	{
		Json::Value unit;
		unit["amount"] = vecItemList.at(i).amount;
		unit["id"] = vecItemList.at(i).id;
		data["itemList"].append(unit);
	}
	data["rowId"] = rowId+1;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TACTICS_ROW_UPGRADE, json_file.c_str());
}

//解析获取的阵法信息
void TacticsController::parseTacticsMsg(const char* szData)
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

		ModelTactics::getInstance()->initFromSvr(data);
	}
}

//解析获取的使用阵法信息
void TacticsController::parseUseTacticsMsg(const char* szData)
{
	Json::Reader read;
	Json::Value  root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		//英雄数据
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//阵法数据
		ModelTactics::getInstance()->updateUsedTactics(data);
	}
}

//解析获取的阵法精炼数据
void TacticsController::parseUpgradeTacticsMsg(const char* szData)
{
	Json::Reader read;
	Json::Value  root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		Json::Value retData = data["retData"];
		Json::Value matrixList = data["matrixList"];
		Json::Value matrixId = data["matrixId"];

		//主数据
		MainModel::getInstance()->updateData(retData);

		//如果是正在使用的则，更新英雄数据
		if(ModelTactics::getInstance()->getCurUsingTacticsData().mRowId/100 == matrixId.asInt()/100)
		{
			Json::Value petLists = retData["petList"];
			HeroModel::getInstance()->updateData(petLists);
		}		

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = retData["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

		//阵法数据
		ModelTactics::getInstance()->updateTacticsList(matrixList);
		ModelTactics::getInstance()->reflushUIAfterUpTactics();
	}
}

//解析排强化
void TacticsController::parseUpgradeRowMsg(const char* szData)
{
	Json::Reader read;
	Json::Value  root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		Json::Value retData = data["retData"];

		//主数据
		MainModel::getInstance()->updateData(retData);

		//英雄数据
		Json::Value petLists = retData["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = retData["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

		//阵法数据
		ModelTactics::getInstance()->updateRowData(data);
	}
}

void TacticsController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_TACTICS_MAIN_LIST:
		parseTacticsMsg(szData);
		break;

	case nMAIN_CMD_TACTICS_USE_TACTICS:
		parseUseTacticsMsg(szData);
		break;

	case nMAIN_CMD_TACTICS_ROW_UPGRADE:
		parseUpgradeRowMsg(szData);
		break;

	case nMAIN_CMD_TACTICS_UPGRADE:
		parseUpgradeTacticsMsg(szData);
		break;

	default:
		break;
	}
}