#include "PvpController.h"

PvpController* PvpController::mInstance = nullptr;
PvpController::PvpController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

PvpController::~PvpController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

PvpController* PvpController::getInstance()
{
	if(!mInstance)
	{
		mInstance = new PvpController();
	}
	return mInstance;
}

void PvpController::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void PvpController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_ARENA_MAIN:
		{
			processPvpInfoData(szData);
		}
		break;
	case nMAIN_CMD_ARENA_RANK:
		{
			processpvpRankData(szData);
		}
		break;
	case nMAIN_CMD_ARENA_GET_PRIZE:
		{
			processPvpPrizeData(szData);
		}
		break;
	case nMAIN_CMD_ARENA_BUY_COUNT:
		{
			processPvpCount(szData);
		}
	default:
		break;
	}
	return;
}

void PvpController::getPvpInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ARENA_MAIN, "");
}

void PvpController::getRank()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ARENA_RANK, "");
}

void PvpController::getPvpPrize(int idx)
{
	Json::FastWriter writer;
	Json::Value data;
	data["rewardType"] = idx;

	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ARENA_GET_PRIZE, json_file.c_str());
}

void PvpController::buyPvpCount(int times)
{
	Json::FastWriter writer;
	Json::Value data;
	data["times"] = times;

	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ARENA_BUY_COUNT, json_file.c_str());
}

void PvpController::processPvpInfoData(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData,root))
	{
		if (throwException(root))
		{
			return;
		}
	}

	Json::Value data = root["data"];

	PvpModel::getInstance()->parsePvpInfoData(data);
}

void PvpController::processpvpRankData(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData,root))
	{
		if (throwException(root))
		{
			return;
		}
	}

	Json::Value data = root["data"];

	PvpModel::getInstance()->parsepvpRankData(data);
}

void PvpController::processPvpPrizeData(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}
	}

	Json::Value data = root["data"];

	PvpModel::getInstance()->parsePvpPrizeData(data);
}

void PvpController::processPvpCount(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}
	}

	Json::Value data = root["data"];

	PvpModel::getInstance()->parsePvpCount(data);
}
