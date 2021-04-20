#include "FriendController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/HttpCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Json/json.h"
#include "../HeroModule/HeroModel.h"
#include "../ToolModule/ToolModel.h"
#include "Common/Common.h"
#include "FriendDef.h"
#include "FriendModel.h"
#include "../MainModule/MainModel.h"
#include "cocos2d.h"
using namespace cocos2d;

FriendController* FriendController::mInstance = nullptr;

FriendController::FriendController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

FriendController::~FriendController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

FriendController* FriendController::getInstance()
{
	if (nullptr == mInstance)
	{
		mInstance = new FriendController();	
	}
	return mInstance;
}

void FriendController::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void FriendController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{	
	case nMAIN_CMD_FRIEND_GET_FRIEND_LIST:			// 获取好友列表
		processGetFriendsInfoData(szData);
		break;
	case nMAIN_CMD_FRIEND_SEND_POWER:				// 送体力
		processSendPowerData(szData);
		break;	
	case nMAIN_CMD_FRIEND_GET_POWER:               //获得体力
		processGetRewardsData(szData);
		break;	
	case nMAIN_CMD_FRIEND_ONE_KEY_SEND:					//一键赠送
		processOneKeySendData(szData);
		break;
	case nMAIN_CMD_FRIEND_ONE_KEY_GET_POWER:        //一键领取
		processOneKeyGetData(szData);
		break;
	case nMAIN_CMD_FRIEND_ADD_FRIEND:				// 添加好友
		processAddFriendData(szData);
		break;	
	case nMAIN_CMD_FRIEND_DELETE_FRIEND:			// 删除好友
		processDeleteFriendData(szData);
		break;
	case nMAIN_CMD_FRIEND_RECOMMEND_LIST:
		processAddFriendListData(szData);
		break;
	case nMAIN_CMD_FRIEND_SEARCH_FRIEND:			
		processSearchFriendData(szData);
		break;	
	case nMAIN_CMD_FRIEND_APPLY:
		processApplyFriendListData(szData);
		break;
	case nMAIN_CMD_FRIEND_APPLY_FRIEND:
		processApplyFriendData(szData);
		break;
	case nMAIN_CMD_FRIEND_ONESKY_APPLY_FRIEND:
		processOneKeypplyFriendData(szData);
		break;
	case nMAIN_CMD_FRIEND_TUI_SONG:
		processTuiSongData(szData);
		break;
	default:
		break;
	}
	return;
}
void FriendController::processGetFriendsInfoData(const char* szData)
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

	FriendModel::getInstance()->parseGetFriendsInfoData(data);
}

void FriendController::processSendPowerData(const char* szData)
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

	FriendModel::getInstance()->parseSendPowerData(data);
}

void FriendController::processGetRewardsData(const char* szData)
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

	FriendModel::getInstance()->parseGetPowerData(data);
}

void FriendController::processOneKeySendData(const char* szData)
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

	FriendModel::getInstance()->parseOneKeySendData(data);
}

void FriendController::processOneKeyGetData(const char* szData)
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

	FriendModel::getInstance()->parseOneKeyGetData(data);
}

void FriendController::processAddFriendListData(const char* szData)
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

	FriendModel::getInstance()->parseAddFriendListData(data);
}

void FriendController::processSearchFriendData(const char* szData)
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

	FriendModel::getInstance()->parseSearchFriendData(data);
}

void FriendController::processApplyFriendListData(const char* szData)
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

	FriendModel::getInstance()->parseApplyFriendListData(data);
}

void FriendController::processApplyFriendData(const char* szData)
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

	FriendModel::getInstance()->parseApplyFriendData(data);
}

void FriendController::processOneKeypplyFriendData(const char* szData)
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

	FriendModel::getInstance()->parseOnekeyApplyFriendData(data);
}

void FriendController::processAddFriendData(const char* szData)
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

	FriendModel::getInstance()->parseAddFriendData(data);
}
void FriendController::processDeleteFriendData(const char* szData)
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

	FriendModel::getInstance()->parseDeleteFriendData(data);
}

void FriendController::processTuiSongData(const char* szData)
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

	FriendModel::getInstance()->parsTuiSongData(data);
}

//获取好友列表信息
void FriendController::onGetFriendsList()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_GET_FRIEND_LIST, "");
}

//获取添加好友列表
void FriendController::onGetAddFriendList()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_RECOMMEND_LIST, "");
}

//送体力
void FriendController::onSendPower(const int& friendId)
{
	Json::FastWriter  writer;
	Json::Value jsonValue;
	jsonValue["friendId"] = friendId;
	std::string  json_file=writer.write(jsonValue);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_SEND_POWER, json_file.c_str());
}

void FriendController::onOneKeySendPower()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_ONE_KEY_SEND, "");
}

void FriendController::onGetPower(const int& friendId)
{
	Json::FastWriter  writer;
	Json::Value jsonValue;
	jsonValue["friendId"] = friendId;
	std::string  json_file=writer.write(jsonValue);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_GET_POWER, json_file.c_str());
}

void FriendController::onOneKeyGetPower()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_ONE_KEY_GET_POWER, "");
}

//搜索好友
void FriendController::onSearchFriend(const string& key)
{
	Json::FastWriter  writer;
	Json::Value jsonValue;
	jsonValue["key"] = key;
	std::string  json_file=writer.write(jsonValue);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_SEARCH_FRIEND, json_file.c_str());
}

//加好友
void FriendController::onAddFriend(const int& friendId)
{
	Json::FastWriter  writer;
	Json::Value jsonValue;
	jsonValue["friendId"] = friendId;
	std::string  json_file=writer.write(jsonValue);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_ADD_FRIEND, json_file.c_str());
}
//删除好友
void FriendController::onDeleteFriend(const int& friendId)
{
	Json::FastWriter  writer;
	Json::Value jsonValue;
	jsonValue["friendId"] = friendId;
	std::string  json_file=writer.write(jsonValue);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_DELETE_FRIEND, json_file.c_str());
}

void FriendController::onRequestFriendList()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_APPLY, "");
}

void FriendController::onApplyFriend(int isRefuse,const int& friendId)
{
	Json::FastWriter  writer;
	Json::Value jsonValue;
	jsonValue["fResult"] = isRefuse;
	jsonValue["friendId"] = friendId;
	std::string  json_file=writer.write(jsonValue);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_APPLY_FRIEND, json_file.c_str());
}

void FriendController::onOneKeyApplyFriend(int isRefuse)
{
	Json::FastWriter  writer;
	Json::Value jsonValue;
	jsonValue["type"] = isRefuse;
	std::string  json_file=writer.write(jsonValue);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FRIEND_ONESKY_APPLY_FRIEND, json_file.c_str());
}



