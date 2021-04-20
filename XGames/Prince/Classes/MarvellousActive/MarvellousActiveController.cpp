#include "MarvellousActiveController.h"

MarvellousActiveController::MarvellousActiveController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

MarvellousActiveController::~MarvellousActiveController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void MarvellousActiveController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_MAR_ACTIVE:
		{
			processMaInfoData(szData);
		}
		break;
	default:
		break;
	}
	return;
}

void MarvellousActiveController::getMaInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAR_ACTIVE, "");
}

void MarvellousActiveController::processMaInfoData(const char* szData)
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

	MarvellousActiveModel::getInstance()->parseMaInfoData(data);
}
