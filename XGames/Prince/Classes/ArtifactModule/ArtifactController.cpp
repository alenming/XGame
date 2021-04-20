#include "ArtifactController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "ArtifactModel.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "SDK_TalkingData/TalkingData.h"


ArtifactController::ArtifactController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

ArtifactController::~ArtifactController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void ArtifactController::sendGetArtifactMainMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ARTIFACT_MAIN, "");
}

void ArtifactController::sendActiveArtifactMsg(int ArtifactID)
{
	Json::FastWriter writer;
	Json::Value data;
	data["artId"] = ArtifactID;
	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ARTIFACT_ACTIVE, json_file.c_str());
}

void ArtifactController::sendRefineArtifactMsg(int ArtifactID, int times, int refType)
{
	Json::FastWriter writer;
	Json::Value data;
	data["artId"] = ArtifactID;
	data["times"] = times;
	data["refType"] = refType;
	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ARTIFACT_REFINE, json_file.c_str());
}

void ArtifactController::sendSaveRefineMsg(int ArtifactID)
{
	Json::FastWriter writer;
	Json::Value data;
	data["artId"] = ArtifactID;
	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ARTIFACT_SAVE, json_file.c_str());
}

void ArtifactController::parseArtifactMainMsg( const char* szData )
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

		ArtifactModel::getInstance()->updateDataFromSvr(data);
	}
}

void ArtifactController::parseActiveMsg( const char* szData )
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

		ArtifactModel::getInstance()->updateActiveData();
	}
}

void ArtifactController::parseRefineMsg( const char* szData )
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

		MainModel::getInstance()->updateData(data);

		//英雄数据
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

		//上报TalkingData
		if(ArtifactModel::getInstance()->getRefineType() == 1)
		{
			//string des = "洗炼一次";
			string des = DataManager::getInstance()->searchCommonTexdtById(10002);
			TalkingData::onPurchase(des, 1, ArtifactModel::getInstance()->getRefineCost().m_iCostGold);
		}
		else if(ArtifactModel::getInstance()->getRefineType() == 10)
		{
			//string des = "洗炼十次";
			string des = DataManager::getInstance()->searchCommonTexdtById(10003);
			TalkingData::onPurchase(des, 1, ArtifactModel::getInstance()->getRefineCost().m_iCostGold*10);
		}

		ArtifactModel::getInstance()->updateRefineData(data);

		//通知到标题栏通用控件
		ArtifactModel::getInstance()->notifyObservers();
	}
}

void ArtifactController::parseSaveRefineMsg( const char* szData )
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

		MainModel::getInstance()->updateData(data);

		//英雄数据
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

		ArtifactModel::getInstance()->updateSaveData();
	}
}

void ArtifactController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_ARTIFACT_MAIN:
		parseArtifactMainMsg(szData);
		break;
	case nMAIN_CMD_ARTIFACT_ACTIVE:
		parseActiveMsg(szData);
		break;
	case nMAIN_CMD_ARTIFACT_REFINE:
		parseRefineMsg(szData);
		break;
	case nMAIN_CMD_ARTIFACT_SAVE:
		parseSaveRefineMsg(szData);
		break;

	default:
		break;
	}
	return;
}

