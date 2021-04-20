#include "RefineryController.h"

RefineryController::RefineryController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

RefineryController::~RefineryController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void RefineryController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_REFINE_CARD_SKILL:
		{
			processRefineryData(szData);
		}
		break;
	default:
		break;
	}
	return;
}

void RefineryController::processRefineryData(const char* szData)
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

	RefineryModel::getInstance()->parseRefineryData(data);
}

void RefineryController::refiney(REFINERY_NODE_VEC materilVec)
{
	Json::FastWriter writer;
	Json::Value data;
	for (int i = 0; i < materilVec.size(); i++)
	{
		Json::Value item;
		item["itemId"] = materilVec.at(i).id;
		item["amount"] = materilVec.at(i).amount;
		data["itemList"].append(item);
	}
	
	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_REFINE_CARD_SKILL, json_file.c_str());
}
