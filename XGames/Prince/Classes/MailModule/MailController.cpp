#include "MailController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/HttpCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Json/json.h"
#include "../HeroModule/HeroModel.h"
#include "../ToolModule/ToolModel.h"
#include "Common/Common.h"
#include "MailDef.h"
#include "MailModel.h"
#include "../MainModule/MainModel.h"
#include "Temp/CustomTips.h"
#include "cocos2d.h"
using namespace cocos2d;

MailController* MailController::mInstance;

MailController::MailController()
{
	SocketCommunicator::getInstance()->addListener(this);
}
MailController::~MailController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

MailController* MailController::getInstance()
{
	if (nullptr == mInstance)
	{
		mInstance = new MailController;
	}
	return mInstance;
}

void MailController::destroyInstance()
{
	if (nullptr != mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void MailController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{	
	case nMAIN_CMD_MAIL_GET_MAILS:			//获取邮件
		processGetMailsData(szData);
		break;
	case nMAIN_CMD_MAIL_OPEN_MAIL:			//打开邮件
		processOpenMailData(szData);
		break;
	case nMAIN_CMD_MAIL_GET_ACCESSORY:		//领取附件
		processGetAccessoryData(szData);
		break;
	case nMAIN_CMD_MAIL_DELETE_MAIL:		//删除邮件
		processDeleteMailData(szData);
		break;
	default:
		break;
	}
	return;
}

void MailController::processGetMailsData(const char* szData)
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
	MailModel::getInstance()->parseGetMailsData(data);
}
void MailController::processOpenMailData(const char* szData)
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
	MailModel::getInstance()->parseOpenMailData(data);
}
void MailController::processGetAccessoryData(const char* szData)
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
	MailModel::getInstance()->parseGetAccessoryData(data);
}
void MailController::processDeleteMailData(const char* szData)
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

	MailModel::getInstance()->parseDeleteMailData(root);
}


//获取邮件
void MailController::onGetMails()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAIL_GET_MAILS, "");
}
//打开邮件
void MailController::onOpenMail(const int& id)
{
	Json::FastWriter  writer;
	Json::Value mailId;
	mailId["mailId"] = id;
	std::string  json_file=writer.write(mailId);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAIL_OPEN_MAIL, json_file.c_str());
}
//领取附件
void MailController::onGetAccessory(const int& id)
{
	Json::FastWriter  writer;
	Json::Value mailId;
	mailId["mailId"] = id;
	std::string  json_file=writer.write(mailId);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAIL_GET_ACCESSORY, json_file.c_str());
}
//删除邮件
void MailController::onDeleteMail(const int& id)
{
	Json::FastWriter  writer;
	Json::Value mailId;
	mailId["mailId"] = id;
	std::string  json_file=writer.write(mailId);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAIL_DELETE_MAIL, json_file.c_str());
}