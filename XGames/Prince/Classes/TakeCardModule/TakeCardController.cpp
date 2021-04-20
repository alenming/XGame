#include "TakeCardController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/HttpCommunicator.h"
#include "Communication/Json/json.h"
#include "../HeroModule/HeroModel.h"
#include "../ToolModule/ToolModel.h"
#include "Common/Common.h"
#include "TakeCardDef.h"
#include "TakeCardModel.h"
#include "../MainModule/MainModel.h"
#include "Temp/CustomTips.h"

#include "cocos2d.h"
using namespace cocos2d;

TakeCardController*	TakeCardController::ms_pInstance	= nullptr;

TakeCardController::TakeCardController()
{
	SocketCommunicator::getInstance()->addListener(this);
	memset(&m_lastTakeCardParam, 0, sizeof(TakeCardParam));
}
TakeCardController::~TakeCardController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

TakeCardController* TakeCardController::getInstance()
{
	if (nullptr == ms_pInstance)
	{
		ms_pInstance = new TakeCardController();		
	}
	return ms_pInstance;
}

void TakeCardController::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void TakeCardController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{	
	case nMAIN_CMD_TAKE_CARD_INFO:
		processGetTakeCardInfoData(szData);
		break;
	case nMAIN_CMD_TAKE_CARD:
		processTakeCardResultData(szData);
		break;
	default:
		break;
	}
	return;
}

void TakeCardController::processGetTakeCardInfoData(const char* szData)
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

	TakeCardModel::getInstance()->parseGetTakeCardInfoData(data);
}

void TakeCardController::processTakeCardResultData(const char* szData)
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

	TakeCardModel::getInstance()->parseTakeCardResultData(data);
}

void TakeCardController::onGetCardInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TAKE_CARD_INFO, "");
}

void TakeCardController::onTakeCard(const int& buyType, const int& times)
{
	Json::FastWriter  writer;
	Json::Value vParam;
	vParam["buyType"]			= buyType;
	vParam["times"]			= times;
	std::string  json_file=writer.write(vParam);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TAKE_CARD, json_file.c_str());

	m_lastTakeCardParam.buyType		= buyType;
	m_lastTakeCardParam.times		= times;
}

void TakeCardController::onTakeCardAgain(const int& times)
{
	Json::FastWriter  writer;
	Json::Value vParam;
	vParam["buyType"]			= m_lastTakeCardParam.buyType;
	vParam["times"]			= times;
	std::string  json_file=writer.write(vParam);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TAKE_CARD, json_file.c_str());
}
