#include "JoinGroupController.h"
#include "JoinGroupModel.h"
#include "SDK_TalkingData/TalkingData.h"

JoinGroupController::JoinGroupController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

JoinGroupController::~JoinGroupController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void JoinGroupController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_GROUP_MAIN_INFO:
		{
			procesGetGroupInfoData(szData);
		}
		break;
	case nMAIN_CMD_GROUP_CREATE:
		{
			processCreateGroupData(szData);
		}
		break;
	case nMAIN_CMD_GROUP_SEARCH:
		{
			processGroupFindData(szData);
		}
		break;
	case nMAIN_CMD_GROUP_APPLY_JOIN:
		{
			processApplyJoinGroupData(szData);
		}
		break;
	case nMAIN_CMD_GROUP_CANCE_APLLY:
		{
			processCancleApplyGroupData(szData);
		}
		break;
	default:
		break;
	}
	return;
}

void JoinGroupController::getGroupInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_MAIN_INFO,"");
}

void JoinGroupController::createGroup(string name,int resId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["name"] = name;
	data["resId"] = resId;

	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_CREATE, json_file.c_str());
}

void JoinGroupController::groupFind(SEARCH_TYPE type, int id, int pageNum)
{
	Json::FastWriter writer;
	Json::Value data;
	data["flag"] = type;
	data["bpId"] = id;
	data["pageNum"] = pageNum;

	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_SEARCH, json_file.c_str());
}

void JoinGroupController::applyJoinGroup(int groupId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["id"] = groupId;

	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_APPLY_JOIN, json_file.c_str());
}

void JoinGroupController::cancleApplyGroup(int groupId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["id"] = groupId;

	std::string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_CANCE_APLLY, json_file.c_str());
}

void JoinGroupController::procesGetGroupInfoData(const char* szData)
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

	JoinGroupModel::getInstance()->parseGetGroupInfoData(data);
}

void JoinGroupController::processCreateGroupData(const char* szData)
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

	//记录创建帮派花费的金币
	TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10166), 1, JoinGroupModel::getInstance()->getCreateSpend());

	JoinGroupModel::getInstance()->parseCreateGroupData(data);
}

void JoinGroupController::processGroupFindData(const char* szData)
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

	JoinGroupModel::getInstance()->parseGroupFindData(data);
}

void JoinGroupController::processApplyJoinGroupData(const char* szData)
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

	JoinGroupModel::getInstance()->parseApplyJoinGroupData(data);
}

void JoinGroupController::processCancleApplyGroupData(const char* szData)
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

	JoinGroupModel::getInstance()->parseCancleApplyGroupData(data);
}
