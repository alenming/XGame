#include "CommonBuyController.h"
#include "Communication/Command.h"
#include "Communication/SocketCommunicator.h"
#include "MainModule/MainModel.h"
#include "CommonBuyModel.h"
#include "SDK_TalkingData/TalkingData.h"
#include "DataManager/DataManager.h"

CommonBuyController::CommonBuyController(void)
	: m_type(COMMON_BUY_TYPE_GOLD_COIN)
	, m_nTimes(1)
{
	SocketCommunicator::getInstance()->addListener(this);
}


CommonBuyController::~CommonBuyController(void)
{
	SocketCommunicator::getInstance()->removeListener(this);
	CommonBuyModel::destroyInstance();
}

void CommonBuyController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_COMMON_BUY_GOLD_POWER:
	case nMAIN_CMD_COMMON_BUY_GOLD_ENERGY:
	case nMAIN_CMD_COMMON_BUY_GOLD_COIN:
	case nMAIN_CMD_COMMON_BUY_GOLD_MAZEPOWER:
		procBuyResult(szData);
		break;

	default:
		break;
	}
}

void CommonBuyController::handleInitData( COMMON_BUY_TYPE type )
{
	m_type = type;
	CommonBuyModel::getInstance()->updateData(type);
}

void CommonBuyController::procBuyResult( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		switch (m_type)
		{
		case COMMON_BUY_TYPE_GOLD_POWER:
			{
				int nPower = data["power"].asInt();
				MainModel::getInstance()->setPower(nPower);
				TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10075), 
					m_nTimes, CommonBuyModel::getInstance()->getPrice());
			}
			break;
		case COMMON_BUY_TYPE_GOLD_ENERGY:
			{
				int nEnergy = data["energy"].asInt();
				MainModel::getInstance()->setEnergy(nEnergy);
				TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10076), 
					m_nTimes, CommonBuyModel::getInstance()->getPrice());
			}
			break;
		case COMMON_BUY_TYPE_GOLD_MAZEPOWER:
			{
				int nMazePower = data["movePower"].asInt();
				MainModel::getInstance()->getMainParam()->mMazePower = nMazePower;
				TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10077), 
					m_nTimes, CommonBuyModel::getInstance()->getPrice());
			}
			break;
		case COMMON_BUY_TYPE_GOLD_COIN:
			{
				int nCoin = data["coin"].asInt();
				MainModel::getInstance()->setSilver(nCoin);
				TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10078), 
					m_nTimes, CommonBuyModel::getInstance()->getPrice());
			}
			break;
		default:
			break;
		}
		MainModel::getInstance()->setGold(data["gold"].asInt());
		MainModel::getInstance()->notifyObservers();

		// 更新已购买次数;
		CommonBuyModel::getInstance()->setTimes(CommonBuyModel::getInstance()->getTimes() + m_nTimes);
		m_nTimes = 1;

		// 更新UI;
		CommonBuyModel::getInstance()->updateData(m_type);
	}
}

void CommonBuyController::handleBuy()
{
	int nCmdId = 0;
	switch (m_type)
	{
	case COMMON_BUY_TYPE_GOLD_POWER:
		nCmdId = nMAIN_CMD_COMMON_BUY_GOLD_POWER;
		break;
	case COMMON_BUY_TYPE_GOLD_ENERGY:
		nCmdId = nMAIN_CMD_COMMON_BUY_GOLD_ENERGY;
		break;
	case COMMON_BUY_TYPE_GOLD_MAZEPOWER:
		nCmdId = nMAIN_CMD_COMMON_BUY_GOLD_MAZEPOWER;
		break;
	default:
		break;
	}
	m_nTimes = 1;
	SocketCommunicator::getInstance()->sendMessage(nCmdId, "");
}

void CommonBuyController::handleMultiBuy( bool bMulti/* = false*/ )
{
	Json::FastWriter writer;
	Json::Value data;
	m_nTimes = bMulti ? CommonBuyModel::getInstance()->getMultiTimes() : 1;
	data["times"] = m_nTimes;
	string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_COMMON_BUY_GOLD_COIN, json_file.c_str());
}
