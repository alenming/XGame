#include "LayerSignController.h"

LayerSignController* LayerSignController::mInstance = nullptr;

LayerSignController::LayerSignController(void)
{
	SocketCommunicator::getInstance()->addListener(this);
}

LayerSignController* LayerSignController::getInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new LayerSignController;
	}
	return mInstance;
}

void LayerSignController::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

LayerSignController::~LayerSignController(void)
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void LayerSignController::onRecvMessage(int nCmdID, const char* szData)
{  
	switch (nCmdID)
	{
	case nMAIN_CMD_SIGN_GET_LIST:
		parseSignList(szData);
		break;
	case nMAIN_CMD_SIGN_RECEIVE:
		parseSignRecvice(szData);
		break;
	default:
		break;
	}
	return;
}

void LayerSignController::sendGetSignList()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_SIGN_GET_LIST, "");
}


void LayerSignController::sendSignRecvice(int num)
{
	Json::FastWriter writer;
	Json::Value data;
	data["num"] = num;

	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_SIGN_RECEIVE,json_file.c_str());
}


void LayerSignController::parseSignList(const char* szData)
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

	LayerSignModel::getInstance()->parseSignData(data);
}

void LayerSignController::parseSignRecvice(const char* szData)
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

	LayerSignModel::getInstance()->parseSignGetData(data);
}
