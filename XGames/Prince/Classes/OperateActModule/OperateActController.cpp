#include "OperateActController.h"
#include "SDK_TalkingData/TalkingData.h"
#include "DoubleBonus/DoubleBonusModel.h"

OperateActController* OperateActController::mInstance = nullptr;

OperateActController::OperateActController()
{
	SocketCommunicator::getInstance()->addListener(this);
	m_buyVipGiftPrice = 0;
}

OperateActController::~OperateActController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

OperateActController* OperateActController::getInstance()
{
	if (nullptr == mInstance)
	{
		mInstance = new OperateActController();	
	}
	return mInstance;
}

void OperateActController::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void OperateActController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_OPERATE_SUPPLEMENT:
		{
			processOprateSuppleMentTable(szData);
		}
		break;
	case nMAIN_CMD_OPERATE_ACT_INFO:
		{
			processOprateActInfo(szData);
		}
		break;
	case nMAIN_CMD_OPERATE_ACT_TASK:
		{
			processOprateActPrize(szData);
		}
		break;
	case nMAIN_CMD_OPERATE_ACT_FINANCIAL:
		{
			processOprateFinancial(szData);
		}
		break;
	case nMAIN_CMD_OPERATE_VIPGIFT:
		{
			processVipGift(szData);
		}
		break;
	default:
		break;
	}
	return;
}

void OperateActController::getOprateSuppleMentTable()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_OPERATE_SUPPLEMENT, "");
}

void OperateActController::getOprateActInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_OPERATE_ACT_INFO, "");
}

void OperateActController::getOprateActtask(int taskId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["taskId"] = taskId;

	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_OPERATE_ACT_TASK, json_file.c_str());
}

void OperateActController::BuyOprateFinancial()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_OPERATE_ACT_FINANCIAL, "");
}

void OperateActController::BuyVipGift(int taskId,int price)
{
	m_buyVipGiftPrice = price;
	Json::FastWriter writer;
	Json::Value data;
	data["taskId"] = taskId;

	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_OPERATE_VIPGIFT, json_file.c_str());
}

void OperateActController::processOprateSuppleMentTable(const char* szData)
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

	OperateActModel::getInstance()->parseOprateSuppleMentTable(data);

	// 双倍活动数据;
	DoubleBonusModel::getInstance()->parseDoubleBonusData(data["Double"]);
}

void OperateActController::processOprateActInfo(const char* szData)
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

	OperateActModel::getInstance()->parseOprateActInfo(data);
}

void OperateActController::processOprateActPrize(const char* szData)
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

	OperateActModel::getInstance()->parseOprateActPrize(data);
}

void OperateActController::processOprateFinancial(const char* szData)
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

	TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20232), 1, DataManager::getInstance()->searchToolSystemById(FINACIAL_GOLD_COUNT)->getIntData("value"));

	Json::Value data = root["data"];
	OperateActModel::getInstance()->parseOprateActFinancial(data);
}

void OperateActController::processVipGift(const char* szData)
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

	TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20233), 1, m_buyVipGiftPrice);

	Json::Value data = root["data"];
	OperateActModel::getInstance()->parseVipGift(data);
}
