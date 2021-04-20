#include "MailModel.h"
#include "Common/Common.h"
#include "MailDef.h"

MailModel* MailModel::mInstance = nullptr;

MailModel::MailModel()
{
	m_isLoaded = false;
	m_mailCountMax = 0;
}

MailModel::~MailModel()
{

}

MailModel* MailModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new MailModel();
	}
	return mInstance;
}

void MailModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void MailModel::notify(MAIL_TYPE eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void MailModel::parseGetMailsData(const Json::Value &data)
{
	setLoaded(true);
	m_mailCountMax = max(m_mailCountMax,data["mailCountMax"].asInt());
	Json::Value maillist = data["mailList"];
	struct tm* nowtime = getNowTime();
	for(int i = 0; i < maillist.size(); i++)
	{
		MAIL_INFOS mailInfos;
		mailInfos.id = maillist[i]["id"].asInt();			
		mailInfos.type = maillist[i]["type"].asInt();
		mailInfos.stat = maillist[i]["stat"].asInt();
		mailInfos.sender = maillist[i]["sender"].asString();
		mailInfos.strTitle = maillist[i]["title"].asString();
		mailInfos.strTime = maillist[i]["date"].asString();
		//计算保留时间
		struct tm mailtime;
		bool res = FriendModel::getInstance()->getCurTime(mailInfos.strTime, mailtime.tm_year, mailtime.tm_mon, mailtime.tm_mday);
		if (!res)
		{
			continue;
		}
		mailInfos.nSaveDates = FriendModel::getInstance()->cmpTime(&mailtime, nowtime);
		//
		mailInfos.strContent = maillist[i]["content"].asString();	
		Json::Value AccessoryList	= maillist[i]["itemList"];
		MAIL_INFO_ACCESSORY itemAccessory;
		for (int j = 0; j < AccessoryList.size(); j++)
		{
			itemAccessory.id		= AccessoryList[j]["id"].asInt();
			itemAccessory.amount	= AccessoryList[j]["amount"].asInt();
			mailInfos.vAccessory.push_back(itemAccessory);
		}
		m_vmails.push_front(mailInfos);
	}

	notify(MAIL_TYPE_GET_MAIL);
}

void MailModel::parseOpenMailData(const Json::Value &data)
{
	int id = data["mailId"].asInt();
	updateMailStat(id, MAIL_STAT_READED);
}

void MailModel::parseGetAccessoryData(const Json::Value &data)
{
	MainModel::getInstance()->updateData(data);		
	//英雄数据
	Json::Value petLists = data["petList"];
	HeroModel::getInstance()->updateData(petLists);		
	//
	Json::Value items = data["itemList"];
	ToolModel::getInstance()->updateToolJsonData(items);

	int id = data["mailId"].asInt();
	updateMailStat(id, MAIL_STAT_GOT);

	m_Reawrd.clear();
	for (int i = 0; i < data["showItem"].size(); i++)
	{
		MAIL_INFO_ACCESSORY itemAccessory;
		itemAccessory.id		= data["showItem"][i]["id"].asInt();
		itemAccessory.amount	= data["showItem"][i]["amount"].asInt();
		m_Reawrd.push_back(itemAccessory);
	}

	notify(MAIL_TYPE_GET_ONE_PRIZE,&id);
}

void MailModel::parseDeleteMailData(const Json::Value &data)
{
	int id = data["mailId"].asInt();
	if (id <= -1)
	{
		deleteAllMail();
		notify(MAIL_TYPE_DEL_ALL_MAIL);
	}
	else
	{
		int i = deleteMail(id);
		notify(MAIL_TYPE_DEL_ONE_MAIL, &i);
	}
}

int MailModel::deleteMail(const int& id)
{
	int idx = 0;
	for (auto iter = m_vmails.begin(); iter != m_vmails.end(); )
	{
		if (id == iter->id)
		{
			m_vmails.erase(iter);
			return idx;
		}
		else
		{
			iter++;
			idx++;
		}
	}
}

void MailModel::deleteAllMail()
{
	for (auto iter = m_vmails.begin(); iter != m_vmails.end(); )
	{
		if (iter->stat >= MAIL_STAT_GOT)
		{
			iter = m_vmails.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

void MailModel::updateMailStat(const int& id, const int& stat)
{
	if (id == -1)
	{
		for (auto iter = m_vmails.begin(); iter != m_vmails.end(); iter++)
		{
			iter->stat = stat;
		}
	}
	else
	{
		for (auto iter = m_vmails.begin(); iter != m_vmails.end(); iter++)
		{
			if (id == iter->id)
			{
				iter->stat = stat;
				break;
			}
		}
	}	
}

MAIL_INFOS_DEQUE& MailModel::getMails()
{
	return m_vmails;
}

bool MailModel::getMail(const int& id, MAIL_INFOS& mail)
{
	for (auto iter = m_vmails.begin(); iter != m_vmails.end(); iter++)
	{
		if (id == iter->id)
		{
			mail = *iter;
			return true;
		}
	}
	return false;
}

//
void MailModel::setLoaded(bool isLoaded)
{
	m_isLoaded = isLoaded;
}
//是否已经从服务器加载过
bool MailModel::isLoaded()
{
	return m_isLoaded;
}

