#include "RankTopController.h"
#include "Communication/Command.h"
#include "Communication/SocketCommunicator.h"
#include "RankTopModel.h"

RankTopController::RankTopController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

RankTopController::~RankTopController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void RankTopController::sendGetRankTopListMsg(int rankType)
{
	Json::FastWriter writer;
	Json::Value data;
	data["type"] = rankType;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_RANK_TOP_LIST, json_file.c_str());
}

void RankTopController::parseRankTopListMsg(const char* szData)
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

		RankTopModel::getInstance()->initFromSvr(data);
	}
}

void RankTopController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_RANK_TOP_LIST:
		parseRankTopListMsg(szData);
		break;
	
	default:
		break;
	}
}
