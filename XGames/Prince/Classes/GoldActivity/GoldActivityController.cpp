#include "GoldActivityController.h"

GoldActivityController* GoldActivityController::mInstance = nullptr;

GoldActivityController::GoldActivityController(void)
{
	SocketCommunicator::getInstance()->addListener(this);
}

GoldActivityController* GoldActivityController::getInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new GoldActivityController();
	}
	return mInstance;
}

void GoldActivityController::destroyInstance()
{
	if (mInstance)
	{
		SAFE_DELETE_ELEMENT(mInstance);
	}
}

GoldActivityController::~GoldActivityController(void)
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void GoldActivityController::onRecvMessage(int nCmdID, const char* szData)
{  
	switch (nCmdID)
	{
	case nMAIN_CMD_GOLD_ACTIVITY_INFO:
		{
			processTableInfo(szData);
		}
		break;
	case nMAIN_CMD_GOLD_ACTIVITY_CHANGE:
		{
			processchangeGold(szData);
		}
		break;
	case nMAIN_CMD_GOLD_ACTIVITY_CHANGEINFO:
		{
			processotherChangeGoldInfo(szData);
		}
		break;
	default:
		break;
	}
	return;
}

void GoldActivityController::getTableInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GOLD_ACTIVITY_INFO, "");
}

void GoldActivityController::changeGold()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GOLD_ACTIVITY_CHANGE, "");
}

void GoldActivityController::otherChangeGoldInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GOLD_ACTIVITY_CHANGEINFO, "");
}

void GoldActivityController::processTableInfo(const char* szData)
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

	GoldActivityModel::getInstance()->parseTableInfo(data);
}

void GoldActivityController::processchangeGold(const char* szData)
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

	GoldActivityModel::getInstance()->parsechangeGold(data);
}

void GoldActivityController::processotherChangeGoldInfo(const char* szData)
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

	GoldActivityModel::getInstance()->parseotherChangeGoldInfo(data);
}
