#include "GroupBossController.h"
#include "GroupBossModel.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"

GroupBossController* GroupBossController::ms_pInstance = nullptr;

GroupBossController::GroupBossController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

GroupBossController::~GroupBossController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

GroupBossController* GroupBossController::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new GroupBossController();
	}
	return ms_pInstance;
}

void GroupBossController::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void GroupBossController::sendGetMainInfoMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_BOSS_MAIN, "");
	LayerWaiting::hideWaiting();
}

void GroupBossController::sendGetRankMsg( eRankType type )
{
	Json::FastWriter writer;
	Json::Value data;
	data["type"] = (int)type;
	std::string json_file = writer.write(data);

	GroupBossModel::getInstance()->setRankTemp(type);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_BOSS_RANK, json_file.c_str());
}

void GroupBossController::sendInspireMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_BOSS_INSPIRE, "");
}

void GroupBossController::sendGetBossRewardMsg( eGetRewardMethod method, int boxId /*= 0*/ )
{
	Json::FastWriter writer;
	Json::Value data;
	data["flag"] = method == eMETHOD_ALL ? 0 : 1;
	data["bossId"] = GroupBossModel::getInstance()->getCurRewardBossId();
	data["rewardId"] = method == eMETHOD_ALL ? 0 : boxId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_BOSS_GET_BOSS_REWARD, json_file.c_str());
}

void GroupBossController::sendGetDamRewardMsg( int boxId )
{
	Json::FastWriter writer;
	Json::Value data;
	data["rewardId"] = boxId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_BOSS_GET_REWARD, json_file.c_str());
}

void GroupBossController::sendGetJinNangRewardMsg( int jinNangId )
{
	Json::FastWriter writer;
	Json::Value data;
	data["rewardId"] = jinNangId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_BOSS_EXCHANGE, json_file.c_str());
}

void GroupBossController::parseMainInfoMsg( const char* szData )
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

		GroupBossModel::getInstance()->initFromSvr(data);
	}
}

void GroupBossController::parseRankMsg( const char* szData )
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

		GroupBossModel::getInstance()->updateRankData(data);
	}
}

void GroupBossController::parseInspireMsg( const char* szData )
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

		GroupBossModel::getInstance()->updateInspireInfo(data);
	}
}

void GroupBossController::parseBossRewardMsg( const char* szData )
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

		GroupBossModel::getInstance()->updateKilledBossRewardData(data);
	}
}

void GroupBossController::parseDamRewardMsg( const char* szData )
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

		GroupBossModel::getInstance()->updateGetRewardInfo(data);
	}
}

void GroupBossController::parseOpenStatePushMsg( const char* szData )
{

}

void GroupBossController::parseExchangeJnang( const char* szData )
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

		GroupBossModel::getInstance()->updateExchangeJnang(data);
	}
}


void GroupBossController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_GROUP_BOSS_MAIN:
		parseMainInfoMsg(szData);
		break;
	case nMAIN_CMD_GROUP_BOSS_INSPIRE:
		parseInspireMsg(szData);
		break;
	case nMAIN_CMD_GROUP_BOSS_RANK:
		parseRankMsg(szData);
		break;
	case nMAIN_CMD_GROUP_BOSS_GET_REWARD:
		parseDamRewardMsg(szData);
		break;
	case nMAIN_CMD_GROUP_BOSS_EXCHANGE:
		parseExchangeJnang(szData);
		break;
	case nMAIN_CMD_GROUP_BOSS_GET_BOSS_REWARD:
		parseBossRewardMsg(szData);
		break;
	default:
		break;
	}
	return;
}

