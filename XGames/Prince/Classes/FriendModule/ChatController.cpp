#include "ChatController.h"
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

ChatController*	ChatController::ms_pInstance	= nullptr;

ChatController* ChatController::getInstance()
{
	if (nullptr == ms_pInstance)
	{
		ms_pInstance = new ChatController();	
	}
	return ms_pInstance;
}

void ChatController::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

ChatController::ChatController()
{
	m_pView = nullptr;
	m_pNoticeView = nullptr;
	SocketCommunicator::getInstance()->addListener(this);
}
ChatController::~ChatController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

//设置UI接口
void ChatController::onSetView(IChatView* pView)
{
	m_pView = pView;
}

void ChatController::onSetNoticeView(INoticeView* pView)
{
	m_pNoticeView = pView;
}

void ChatController::onRemoveNoticeView()
{
	m_pNoticeView = nullptr;
}

void ChatController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{		
	case nMAIN_CMD_CHAT_SYS_NOTICE:// 系统跑马灯公告消息
		{
			processNoticeMessageData(szData);
		}
		break;
	case nMAIN_CMD_CHAT_PLAYER_BEHAVIOR_MESSAGE://玩家行为消息
		{
			processPlayerBehaviorMessageData(szData);
		}
		break;
	case nMAIN_CMD_CHAT_PUSH_MESSAGE: //服务器推送的系统消息
		{
			processSCPushMessageData(szData);
		}
		break;
	case nMAIN_CMD_CHAT_SEND_MESSAGE://玩家发送聊天消息
		{
			processCSChatMessageData(szData);
		}
		break;
	default:
		break;
	}
	return;
}
void ChatController::processNoticeMessageData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data	= root["data"];
		Json::Value noticeList	= data["noticeList"];
		for (int i = 0; i < noticeList.size(); i++)
		{
			Json::Value item		= noticeList[i];
			int id					= item["id"].asInt();
			int repMinute			= item["repMinute"].asInt();
			string strContent		= item["content"].asString();
			string strTitle			= item["title"].asString();
			int strStartTime		= item["starttime"].asInt();
			int strEndTime		= item["endtime"].asInt();

			//跑马灯公告显示循环播放
			FriendModel::getInstance()->addSysNotice(id, FRIEND_CHAT_SYSTEM2, repMinute, strStartTime, strEndTime, strTitle, strContent);
			
			//跑马灯公告在消息面板显示
			tm *nowtime = getNowTime();
			char szTime[100];
			sprintf(szTime, "%02d:%02d", nowtime->tm_hour, nowtime->tm_min);
			string strGetTime = _TO_STR(nowtime->tm_year) + "-" + _TO_STR(nowtime->tm_mon) + "-" + _TO_STR(nowtime->tm_mday)
				+ " " + szTime;
			FriendModel::getInstance()->addPublicChatMessage(id, FRIEND_CHAT_SYSTEM2, strContent, strTitle, strGetTime, 0, 0, 0);
		}

		if (m_pNoticeView != nullptr)
		{
			m_pNoticeView->onRecvNoticeMessage();
		}
	}
}

void ChatController::processPlayerBehaviorMessageData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data	= root["data"];
		int repMinute = data["repMinute"].asInt();
		string strContent = "";
		Json::Value msgstr = data["msgstr"];
		int msgType = data["msgtype"].asInt();
		if (msgType == 1)
		{
			//恭喜XXX成为尊贵的VIP4！
			string nickname = msgstr["nickname"].asString();
			int viplevel = msgstr["viplevel"].asInt();
			/*
			strContent = STR_UTF8("恭喜|") + 
				"0xf5fe00::" + nickname + "|" + 
				STR_UTF8("成为尊贵的|") +
				"0xf5fe00::" + "VIP" + TO_STR(viplevel) + "!";
				*/

			strContent = DataManager::getInstance()->searchCommonTexdtById(10107) + 
				"0xf5fe00::" + nickname + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(10108) +
				"0xf5fe00::" + "VIP" + TO_STR(viplevel) + "!";
		}
		else if(msgType == 2)
		{
			//XXX真乃神将降世,过关斩将,武艺高强,占据竞技场第X名
			string nickname = msgstr["nickname"].asString();
			int pvporder = msgstr["pvporder"].asInt();
			strContent = "0xf5fe00::" + nickname + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(10109) +
				"0xf5fe00::" + DataManager::getInstance()->searchCommonTexdtById(10110) + TO_STR(pvporder) 
				+ DataManager::getInstance()->searchCommonTexdtById(10111);
		}
		else if (msgType == 3)
		{
			//神将降世！XXX于登陆礼包获得★★★英雄李师师.
			string nickname = msgstr["nickname"].asString();
			string gift = msgstr["gift"].asString();
			int star = msgstr["star"].asInt();
			string name = msgstr["name"].asString();

			strContent = DataManager::getInstance()->searchCommonTexdtById(10112) + 
				"0xf5fe00::" + nickname + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(10113) +
				"0x61ff3s::" + gift + "|" +
				DataManager::getInstance()->searchCommonTexdtById(10114) + 
				"Image/Icon/global/img_star1.png;;" + TO_STR(star) + "|" +
				DataManager::getInstance()->searchCommonTexdtById(10115) + 
				"0xf5fe00::" + name;
		}
		else if (msgType == 4)
		{
			//神将降世！XXX获得★★★英雄李师师.
			string nickname = msgstr["nickname"].asString();
			int star = msgstr["star"].asInt();
			string name = msgstr["name"].asString();

			strContent = DataManager::getInstance()->searchCommonTexdtById(10112) + 
				"0xf5fe00::" + nickname + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(10114) + 
				"Image/Icon/global/img_star1.png;;" + TO_STR(star) + "|" +
				DataManager::getInstance()->searchCommonTexdtById(10115) + 
				"0xf5fe00::" + name;
		}
		else if (msgType == 5)
		{
			//XXX把英雄鲁智深提升到★★★★，阵容更加强大了
			string nickname = msgstr["nickname"].asString();
			int star = msgstr["star"].asInt();
			string name = msgstr["name"].asString();

			strContent = "0xf5fe00::" + nickname + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(10116) + 
				"0xf5fe00::" + name + "|" +
				DataManager::getInstance()->searchCommonTexdtById(10117) +
				"Image/Icon/global/img_star1.png;;" + TO_STR(star) + "|" +
				DataManager::getInstance()->searchCommonTexdtById(10118);
		}
		else if (msgType == 6)
		{
			//人品爆发，XXX玩家开启淬炼宝匣获得冷月拂尘
			string nickname = msgstr["nickname"].asString();
			int itemId = msgstr["itemId"].asInt();
			RowData* rowData = DataManager::getInstance()->searchToolById(itemId);
			strContent = DataManager::getInstance()->searchCommonTexdtById(10119) + 
				"0xf5fe00::" + nickname + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(10120) + 
				"0x61ff3s::" + DataManager::getInstance()->searchCommonTexdtById(10121) + 
				DataManager::getInstance()->searchCommonTexdtById(10114) + 
				"0xee9309::" + rowData->getStringData("itemName");
		}
		else if (msgType == 7)
		{
			//恭喜XXX玩家于淬炼宝匣中兑换获得冷月拂尘
			string nickname = msgstr["nickname"].asString();
			int itemId = msgstr["itemId"].asInt();
			RowData* rowData = DataManager::getInstance()->searchToolById(itemId);
			strContent = DataManager::getInstance()->searchCommonTexdtById(10107) + 
				"0xf5fe00::" + nickname + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(10122) + 
				"0x61ff3s::" + DataManager::getInstance()->searchCommonTexdtById(10121) + 
				DataManager::getInstance()->searchCommonTexdtById(10123) + 
				"0xee9309::" + rowData->getStringData("itemName");
		}

		//跑马灯公告显示循环播放
		FriendModel::getInstance()->addSysNotice(0, FRIEND_CHAT_PLAYER_DO, repMinute, 0, 0, "", strContent);

		if (m_pNoticeView != nullptr)
		{
			m_pNoticeView->onRecvNoticeMessage();
		}
	}
}

void ChatController::processCSChatMessageData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data	= root["data"];
		Json::Value chatList	= data["msgList"];
		for (int i = 0; i < chatList.size(); i++)
		{
			Json::Value item		= chatList[i];
			int msgtype				= item["msgtype"].asInt();
			string strChatMsg		= item["msgstr"].asString();
			string strName			= item["nickName"].asString();
			int useId			= item["characterId"].asInt();
			int vipLvl				= item["vipLvl"].asInt();
			string strTime			= item["msgtime"].asString();
			int level				= item["level"].asInt();
			int resId				= item["resId"].asInt();
			//		
			if (msgtype == FRIEND_CHAT_PRIVATE) //私聊
			{
				FriendModel::getInstance()->addPrivateChatMessage(useId, msgtype, strChatMsg, strName, strTime, vipLvl, level, resId);
			}
			else if (msgtype == FRIEND_CHAT_GROUP)//帮派
			{
				FriendModel::getInstance()->addGroupChatMessage(useId, msgtype, strChatMsg, strName, strTime, vipLvl, level, resId);
			}
			
			FriendModel::getInstance()->addPublicChatMessage(useId, msgtype, strChatMsg, strName, strTime, vipLvl, level, resId);
		}
		if (m_pView != nullptr)
		{
			m_pView->onRecvChatMessage();		
		}
		else
		{
			notifyObservers();
		}
	}
}

void ChatController::processSCPushMessageData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data	= root["data"];

		int msgtype				= data["msgtype"].asInt();
		string strChatMsg		= data["msgstr"].asString();
		string strTime			= data["msgtime"].asString();

		FriendModel::getInstance()->addPublicChatMessage(0, FRIEND_CHAT_SYSTEM, strChatMsg, "", strTime, 0, 0, 0);

		if (m_pView != nullptr)
		{
			m_pView->onRecvChatMessage();
		}
	}
}

void ChatController::onRemoveView()
{
	m_pView = nullptr;
}

void ChatController::onSendMessage(const int nuserId, const int ntype, const string strMessage)
{
	//msgtype:消息类型1-世界,2-帮派,3-好友,4-私聊
	//friendId:私聊玩家角色id或好友角色id, 综合聊天填0
	Json::FastWriter  writer;
	Json::Value jsonValue;
	jsonValue["friendId"]	= nuserId;
	jsonValue["msgtype"]	= ntype;
	jsonValue["msgstr"]		= strMessage;
	std::string  json_file=writer.write(jsonValue);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_CHAT_SEND_MESSAGE, json_file.c_str());
}
