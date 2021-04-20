#include "MyGroupController.h"
#include "MyGroupModel.h"
#include "JoinGroupModel.h"
#include "SDK_TalkingData/TalkingData.h"

MyGroupController* MyGroupController::ms_pInstance = nullptr;

MyGroupController* MyGroupController::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new MyGroupController();
	}
	return ms_pInstance;
}

void MyGroupController::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

MyGroupController::MyGroupController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

MyGroupController::~MyGroupController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

//发送请求帮派主界面
void MyGroupController::sendGetGroupMainMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_MAIN_INFO, "");
}

//发送请求帮派动态和宣言
void MyGroupController::sendGetDynamicEventMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_DYN_EVENT, "");
}

//发送请求退出帮派
void MyGroupController::sendExitGroupMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_EXIT, "");
}

//发送请求解散帮派
void MyGroupController::sendDisbandGroupMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_DISBAND, "");
}

//发送请求修改宣言, 参数1.宣言内容, 参数2.宣言类型
void MyGroupController::sendModifyDeclarationMsg(string content1, string content2)
{
	Json::FastWriter writer;
	Json::Value data;
	data["remark1"] = content1;
	data["remark2"] = content2;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_MODIFY_NOTICE, json_file.c_str());
}

//发送修改头像
void MyGroupController::sendModifyHeadIconMsg(int iconId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["resId"] = iconId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_CHANGE_ICON, json_file.c_str());
}

//发送请求设置帮派加入条件
void MyGroupController::sendSetJoinLimitMsg(sJoinLimit limit)
{
	Json::FastWriter writer;
	Json::Value data;
	if(limit.type == eJOIN_CHECK_TYPE)
	{
		data["checkFlag"] = 1;
	}
	else if(limit.type == eJOIN_FREE_TYPE)
	{
		data["checkFlag"] = 0;
	}
	else
	{
		return;
	}
	data["level"] = limit.levelLimit;
	std::string json_file = writer.write(data);

	MyGroupModel::getInstance()->setTempJoinLimit(limit);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_SET, json_file.c_str());
}

//发送请求申请人列表
void MyGroupController::sendGetApplicantMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_APPLICANT_LIST, "");
}

//发送请求新人审批, 参数1.玩家ID，参数2.是否同意
void MyGroupController::sendCheckJoinMsg(int uId, bool isAllow)
{
	Json::FastWriter writer;
	Json::Value data;
	data["id"] = uId;
	data["status"] = isAllow ? 1 : 2;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_APPLY_CHECK, json_file.c_str());
}

//发送请求逐出帮派
void MyGroupController::sendExpelMsg(int uId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["id"] = uId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_EXPEL, json_file.c_str());
}

//发送请求职位任命
void MyGroupController::sendAppointMsg(int uId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["id"] = uId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_APPOINT, json_file.c_str());
}

//发送请求转让帮主
void MyGroupController::sendAbdicateMsg(int uId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["id"] = uId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_ABDICATE, json_file.c_str());
}

//发送请求建设界面
void MyGroupController::sendGetBuildInfoMsg(eBuildType type)
{
	Json::FastWriter writer;
	Json::Value data;

	data["flag"] = (int)type;


	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_BUILD_INFO, json_file.c_str());
}

//发送请求捐赠建筑
void MyGroupController::sendDonateBuildMsg(eBuildType type, eDonateType itemType)
{
	Json::FastWriter writer;
	Json::Value data;



	data["flag"] = (int)type;

	data["itemType"] = (int)itemType;

	std::string json_file = writer.write(data);

	//记录下捐献的类型，talkingData用到
	MyGroupModel::getInstance()->setTempDonateType(itemType);
	
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_BUILD_DONATE, json_file.c_str());
}

//发送请求商店界面
void MyGroupController::sendGetPrestigeShopMsg()
{

}

//发送请求该买商品
void MyGroupController::sendBuyProductMsg(int productId)
{

}

void MyGroupController::sendClieckNineList( int listId )
{
	Json::FastWriter writer;
	Json::Value data;

	data["flag"] = listId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_ACTIVITY_CLICK,json_file.c_str());
}

void MyGroupController::sendRefreshList()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_ACTIVITY_REFRESH, "");
}

void MyGroupController::sendXunlongHistory()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_ACTIVITY_HISTORY, "");
}

//解析帮派主界面信息
void MyGroupController::parseGroupMainMsg(const char* szData)
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

		MyGroupModel::getInstance()->updateGroupInfoFromSvr(data);
	}
}

//解析修改头像
void MyGroupController::parseModifyHeadIconMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		sMyGroupData* groupInfo = MyGroupModel::getInstance()->getMyGroupInfo();
		groupInfo->resId = MyGroupModel::getInstance()->getFlagTempID();

		//通知UI, 刷新帮派旗帜
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_GROUP_CHANGE_ICON;
		MyGroupModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析帮派动态和宣言
void MyGroupController::parseDynamicEventMsg(const char* szData)
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

		MyGroupModel::getInstance()->updateGroupEvent(data);
	}
}

//解析解散、退出、逐出、推送解散帮派
void MyGroupController::parseExitGroupMsg(const char* szData, int cmd)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}
		if(cmd == nMAIN_CMD_GROUP_EXIT || cmd == nMAIN_CMD_GROUP_DISBAND)
		{
			MainModel::getInstance()->setGroupInfo(0, "暂无帮派");
		}
		if(cmd == nMAIN_CMD_GROUP_PUSH)
		{
			Json::Value data = root["data"];
			
			int groupId = data["bangpaiId"].asInt();
			string groupName = data["bangpaiName"].asString();
			int flag = data["flag"].asInt();
			MainModel::getInstance()->setGroupInfo(groupId, groupName);

			MyGroupModel::getInstance()->setFlagPushMsg(flag);

			//flag = 6是新人审核推送，若审核通过并且处在帮派列表界面，则需要切换到帮派主界面去
			if(flag == 6 && groupId > 0)
			{
				//通知UI, 审核通过
				ObserverParam obParam;
				obParam.id = nMAIN_CMD_GROUP_PUSH;
				JoinGroupModel::getInstance()->notifyObservers(&obParam);
			}
		}

		MyGroupModel::getInstance()->updateExitGroup(cmd);
	}
}

//解析修改宣言
void MyGroupController::parseModifyDeclarationMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		MyGroupModel::getInstance()->updateGroupDeclaration();
	}
}

//解析设置帮派加入条件
void MyGroupController::parseSetJoinLimitMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		MyGroupModel::getInstance()->updateJoinLimit(MyGroupModel::getInstance()->getTempJoinLimit());

		//通知UI, 设置成功
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_GROUP_SET;
		MyGroupModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析申请人列表
void MyGroupController::parseApplicantMsg(const char* szData)
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
		MyGroupModel::getInstance()->updateProposerList(data);
	}
}

//解析新人审批
void MyGroupController::parseCheckJoinMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		MyGroupModel::getInstance()->updateProposerInfo();

		//通知UI, 审批结果
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_GROUP_APPLY_CHECK;
		MyGroupModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析职位任命
void MyGroupController::parseAppointMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		//MyGroupModel::getInstance()->updateMemberPower(nMAIN_CMD_GROUP_APPOINT);

		//通知UI, 刷新帮派主界面
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_GROUP_APPOINT;
		MyGroupModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析转让帮主
void MyGroupController::parseAbdicateMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		//MyGroupModel::getInstance()->updateMemberPower(nMAIN_CMD_GROUP_ABDICATE);

		//通知UI, 刷新帮派主界面
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_GROUP_ABDICATE;
		MyGroupModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析建设界面
void MyGroupController::parseBuildInfoMsg(const char* szData, int cmd)
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
		MyGroupModel::getInstance()->updateBuildInfoByType(data, MyGroupModel::getInstance()->getTempBuildType(), cmd);
	}
}

//解析商店界面
void MyGroupController::parsePrestigeShopMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}


	}
}

//解析该买商品
void MyGroupController::parseBuyProductMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}


	}
}

//推送新人审批结果
void MyGroupController::parseCheckJoinPushMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}


	}
}

void MyGroupController::parseClickNineList( const char* szData )
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

		auto changeInfo = data["changeInfo"];

		// 更新道具;
		if (!changeInfo.isNull())
		{	
			ToolModel::getInstance()->updateToolJsonData(changeInfo["itemList"]);
			//更新金币/银币
			MainModel::getInstance()->updateData(changeInfo);
		}

		
		MyGroupModel::getInstance()->updateGroupActivityClick(data);
	}
}

void MyGroupController::parseRefreshList( const char* szData )
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
		MyGroupModel::getInstance()->updateGroupActivityRefresh(data);
	}
}

void MyGroupController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_GROUP_MAIN_INFO:
		parseGroupMainMsg(szData);
		break;
	case nMAIN_CMD_GROUP_DYN_EVENT:
		parseDynamicEventMsg(szData);
		break;
	case nMAIN_CMD_GROUP_CHANGE_ICON:
		parseModifyHeadIconMsg(szData);
		break;
	case nMAIN_CMD_GROUP_SET:
		parseSetJoinLimitMsg(szData);
		break;
	case nMAIN_CMD_GROUP_EXPEL:
	case nMAIN_CMD_GROUP_PUSH:
	case nMAIN_CMD_GROUP_EXIT:
	case nMAIN_CMD_GROUP_DISBAND:
		parseExitGroupMsg(szData, nCmdID);
		break;
	case nMAIN_CMD_GROUP_ABDICATE:
		parseAbdicateMsg(szData);
		break;
	case nMAIN_CMD_GROUP_MODIFY_NOTICE:
		parseModifyDeclarationMsg(szData);
		break;
	case nMAIN_CMD_GROUP_APPOINT:
		parseAppointMsg(szData);
		break;
	case nMAIN_CMD_GROUP_BUILD_INFO:
	case nMAIN_CMD_GROUP_BUILD_DONATE:
		parseBuildInfoMsg(szData, nCmdID);
		break;
	case nMAIN_CMD_GROUP_APPLICANT_LIST:
		parseApplicantMsg(szData);
		break;
	case nMAIN_CMD_GROUP_APPLY_CHECK:
		parseCheckJoinMsg(szData);
		break;
	case nMAIN_CMD_GROUP_ACTIVITY_CLICK:
		parseClickNineList(szData);
		break;
	case nMAIN_CMD_GROUP_ACTIVITY_REFRESH:
		parseRefreshList(szData);
		break;
	case nMAIN_CMD_GROUP_ACTIVITY_HISTORY:
		parseXunLongHistory(szData);
		break;
	default:
		break;
	}
}



void MyGroupController::parseXunLongHistory( const char* szData )
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

		MyGroupModel::getInstance()->updateGroupActivityHistory(data);
	}
}





