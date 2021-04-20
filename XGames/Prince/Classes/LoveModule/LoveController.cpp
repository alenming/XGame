#include "LoveController.h"
#include "SDK_TalkingData/TalkingData.h"
LoveController::LoveController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

LoveController::~LoveController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void LoveController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_LOVE_GET_INFO:
		{
			processHeroListInfoData(szData);
		}
		break;
	case nMAIN_CMD_LOVE_GIVE_GIFT:
		{
			processHeroSendGiftData(szData);
		}
		break;
	case nMAIN_CMD_LOVE_GET_BUY:
		{
			processHeroBuyGiftData(szData);
		}
		break;
	case nMAIN_CMD_LOVE_FINGER_GUESS:
		{
			processHeroFingerGuessData(szData);
		}
		break;
	case nMAIN_CMD_LOVE_PLAY:
		{
			processHeroPlayMapData(szData);
		}
		break;
	case nMAIN_CMD_LOVE_PLAY_MAP_CHOOSE:
		{
			processChooseMapData(szData);
		}
		break;
	case nMAIN_CMD_LOVE_PLAY_MAP_EVENT:
		{
			processChooseMapEvent(szData);
		}
		break;
	case nMAIN_CMD_LOVE_GET_BUY_GUESS_INFO:
		{
			processGuessCountInfo(szData);
		}
		break;
	case nMAIN_CMD_LOVE_GET_BUY_GUESS:
		{
			processGuessCount(szData);
		}
		break;
	case nMAIN_CMD_LOVE_ADD_ATTRIBUTE:
		{
			processAddAttribute(szData);
		}
		break;
	case nMAIN_CMD_LOVE_RESET_ATTRIBUTE:
		{
			processResetAttribute(szData);
		}
		break;
	case nMAIN_CMD_LOVE_CHANGE_GUESS:
		{
			processChangeGuess(szData);
		}
		break;
	case nMAIN_CMD_LOVE_GET_GUESS_PRIZE:
		{
			processGetGuessPrize(szData);
		}
		break;
	default:
		break;
	}
	return;
}

void LoveController::getHeroListInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_GET_INFO, "");
}

void LoveController::sendGift(int petId,int itemId,int count)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = petId;
	data["itemId"] = itemId;
	data["count"] = count;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_GIVE_GIFT, json_file.c_str());
}

void LoveController::buyGift(int itemId,int amount)
{
	Json::FastWriter writer;
	Json::Value data;
	data["itemId"] = itemId;
	data["amount"] = amount;
	data["module"] = 2;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_GET_BUY, json_file.c_str());
}

void LoveController::fingerGuess(int id,FINGER_ENTER_TYPE type)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = id;
	data["outcome"] = type;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_FINGER_GUESS, json_file.c_str());
}

void LoveController::playMap(int petId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = petId;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_PLAY, json_file.c_str());
}

void LoveController::chooseMap(int petId,int mapId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = petId;
	data["mapId"] = mapId;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_PLAY_MAP_CHOOSE, json_file.c_str());
}

void LoveController::mapEvent(int petId,int mapId,int curCoords,int tarCoords,int _event)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = petId;
	data["mapId"] = mapId;
	data["curCoords"] = curCoords;
	data["tarCoords"] = tarCoords;
	data["event"] = _event;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_PLAY_MAP_EVENT, json_file.c_str());
}

void LoveController::buyGuessCountInfo(int petId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = petId;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_GET_BUY_GUESS_INFO, json_file.c_str());
}

void LoveController::buyGuessCount(int petId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = petId;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_GET_BUY_GUESS, json_file.c_str());
}

void LoveController::changeGuess(int petId,int type)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = petId;
	data["type"] = type;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_CHANGE_GUESS, json_file.c_str());
}

void LoveController::getGuseePrize(int petId,int idx)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = petId;
	data["index"] = idx;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_GET_GUESS_PRIZE, json_file.c_str());
}

void LoveController::addAttribute(int petId,int type)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = petId;
	data["type"] = type;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_ADD_ATTRIBUTE, json_file.c_str());
}

void LoveController::resetAttribute(int petId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["petId"] = petId;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_LOVE_RESET_ATTRIBUTE, json_file.c_str());
}

void LoveController::processHeroListInfoData(const char* szData)
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

	LoveModel::getInstance()->parseHeroListInfoData(data);
}

void LoveController::processHeroSendGiftData(const char* szData)
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

	LoveModel::getInstance()->parseHeroSendGiftData(data);
}

void LoveController::processHeroBuyGiftData(const char* szData)
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

	LoveModel::getInstance()->parseHeroBuyGiftData(data);
}

void LoveController::processHeroFingerGuessData(const char* szData)
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

	LoveModel::getInstance()->parseHeroFingerGuessData(data);
}

void LoveController::processHeroPlayMapData(const char* szData)
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

	LoveModel::getInstance()->parseHeroPlayMapData(data);
}

void LoveController::processChooseMapData(const char* szData)
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

	LoveModel::getInstance()->parseHeroChooseMapData(data);
}

void LoveController::processChooseMapEvent(const char* szData)
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

	LoveModel::getInstance()->parseHeroChooseMapEvent(data);
}

void LoveController::processGuessCountInfo(const char* szData)
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

	LoveModel::getInstance()->parseGuessCountInfo(data);
}

void LoveController::processGuessCount(const char* szData)
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
	TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20281), 1, LoveModel::getInstance()->getBuyGuessPrice());

	Json::Value data = root["data"];
	LoveModel::getInstance()->parseGuessCount(data);
}

void LoveController::processAddAttribute(const char* szData)
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

	LoveModel::getInstance()->parseAddAttribute(data);
}

void LoveController::processResetAttribute(const char* szData)
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

	LoveModel::getInstance()->parseResetAttribute(data);
}

void LoveController::processChangeGuess(const char* szData)
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

	TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20282), 1, LoveModel::getInstance()->getChangeGuessPrice());

	Json::Value data = root["data"];
	LoveModel::getInstance()->parseChangeGuess(data);
}

void LoveController::processGetGuessPrize(const char* szData)
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

	LoveModel::getInstance()->parseGetGuessPrize(data);
}
