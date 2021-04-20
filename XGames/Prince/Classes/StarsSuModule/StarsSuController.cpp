#include "StarsSuController.h"

StarsSuController::StarsSuController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

StarsSuController::~StarsSuController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void StarsSuController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_GET_STAR_INFO:
		{
			processGetStarInfoData(szData);
		}
		break;
	case nMAIN_CMD_LIGHT_STAR:
		{
			processonAstrologyAndBreakOut(szData);
		}
		break;
	default:
		break;
	}
	return;
}

void StarsSuController::onGetStarsInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GET_STAR_INFO, "");
}

void StarsSuController::onAstrologyAndBreakOut(ASTROLOGY_TYPE type)
{
	Json::FastWriter writer;
	Json::Value val;
	val["operType"] = (int)type;

	std::string json_file = writer.write(val);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LIGHT_STAR, json_file.c_str());
}

void StarsSuController::processGetStarInfoData(const char* szData)
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

	StarsSuModel::getInstance()->parseGetStarInfoData(data);
}

void StarsSuController::processonAstrologyAndBreakOut(const char* szData)
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

	StarsSuModel::getInstance()->parseAstrologyAndBreakOut(data);
}

