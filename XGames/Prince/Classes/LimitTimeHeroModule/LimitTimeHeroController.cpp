#include "LimitTimeHeroController.h"
#include "LimitTimeHeroModel.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "ToolModule/ToolModel.h"
#include "SDK_TalkingData/TalkingData.h"

LimitTimeHeroController::LimitTimeHeroController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

LimitTimeHeroController::~LimitTimeHeroController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void LimitTimeHeroController::sendGetTableDataMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LIMITTIMEHERO_CONFIG,"");
}

void LimitTimeHeroController::sendGetMainInfoMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LIMITTIMEHERO_INFO,"");
}

void LimitTimeHeroController::sendTakeCardMsg( eActType actType, eChouKaType type )
{
	Json::FastWriter  writer;
	Json::Value vParam;
	if (type == eChouKaType::eTYPE_ONE)
	{
		vParam["times"] = iItemNumOne;
	}
	if (type == eChouKaType::eTYPE_TEN)
	{
		vParam["times"] = iItemNumTen;
	}

	std::string  json_file=writer.write(vParam);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LIMITTIMEHERO_CARD, json_file.c_str());
}

void LimitTimeHeroController::sendGetScoreRewardMsg( int boxId )
{
	Json::FastWriter  writer;
	Json::Value vParam;
	vParam["index"] = boxId;
	std::string  json_file=writer.write(vParam);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LIMITTIMEHERO_GETBOX, json_file.c_str());
}

void LimitTimeHeroController::parseTableDataMsg( const char* szData )
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

		LimitTimeHeroModel::getInstance()->initActInfoFromSvr(data);
	}
}

void LimitTimeHeroController::parseMainInfoMsg( const char* szData )
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

		LimitTimeHeroModel::getInstance()->initMainInfoFromSvr(data);
	}
}

void LimitTimeHeroController::parseTakeCardMsg( const char* szData )
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


		if (LimitTimeHeroModel::getInstance()->getActData()->actType == eMETHOD_TAKECARD)
		{
			LimitTimeHeroModel::getInstance()->setTakeCardData(data);
		}
		else if (LimitTimeHeroModel::getInstance()->getActData()->actType == eMETHOD_OPENBOX)
		{
			LimitTimeHeroModel::getInstance()->setOpenBoxData(data);
		}
	}
}

void LimitTimeHeroController::parseScoreRewardMsg( const char* szData )
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

		LimitTimeHeroModel::getInstance()->getGetScoreBox(data);
	}
}

void LimitTimeHeroController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_LIMITTIMEHERO_CONFIG:
		parseTableDataMsg(szData);
		break;
	case nMAIN_CMD_LIMITTIMEHERO_GETBOX:
		parseScoreRewardMsg(szData);
		break;
	case nMAIN_CMD_LIMITTIMEHERO_INFO:
		parseMainInfoMsg(szData);
		break;
	case nMAIN_CMD_LIMITTIMEHERO_CARD:
		parseTakeCardMsg(szData);
		break;
	default:
		break;
	}
	return;
}