#include "BossActStateListener.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "MainModule/MainModel.h"


BossActStateListener* BossActStateListener::ms_pInstance = nullptr;
BossActStateListener::BossActStateListener()
{
	SocketCommunicator::getInstance()->addListener(this);
}

BossActStateListener::~BossActStateListener()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

BossActStateListener* BossActStateListener::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new BossActStateListener();
	}
	return ms_pInstance;
}

void BossActStateListener::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void BossActStateListener::parseBossActStatePushMsg(const char* szData)
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
		int state = data["openFlag"].asInt();
		MainModel::getInstance()->mBossActOpenState = data["openFlag"].asInt();

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_BOSS_ACT_STATE;
		MainModel::getInstance()->notifyObservers(&obParam);
	}
}

void BossActStateListener::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_BOSS_ACT_STATE:
		parseBossActStatePushMsg(szData);
		break;
	default:
		break;
	}
}