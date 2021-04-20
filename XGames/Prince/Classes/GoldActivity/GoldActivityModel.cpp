#include "GoldActivityModel.h"
#include "MainModule/MainModel.h"

GoldActivityModel* GoldActivityModel::mInstance = nullptr;


GoldActivityModel::GoldActivityModel(void)
{
	m_actOverSec = 0;
	m_openTimeSec = 0;
	m_changeCount = 0;
	m_goldNum = 0;
	m_OtherChange.clear();
	m_myChangeInfo.ChangeInfo.clear();
	m_isLoad = false;
	b_isNotifyMainInfo = false;
}


GoldActivityModel::~GoldActivityModel(void)
{
	LocalTimer::getInstance()->removeLocalTimerListener(this);
}

GoldActivityModel* GoldActivityModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new GoldActivityModel();
	}
	return mInstance;
}

void GoldActivityModel::destroyInstance()
{
	if(mInstance)
	{
		SAFE_DELETE_ELEMENT(mInstance);
	}
}


void GoldActivityModel::notify(GoldActivityNotify_Type eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void GoldActivityModel::timeTick(int dt)
{
	if (m_actOverSec <= 0 && m_openTimeSec <= 0)
	{
		m_actOverSec = 0;
		//停止监听器
		setEndTimeStamp(0);
		return;
	}
	//倒计时
	if (m_actOverSec >= dt)
	{
		m_actOverSec -= dt;
	}

	if (m_openTimeSec >= 0 && !b_isNotifyMainInfo) ///有资格开启
	{
		m_openTimeSec -= dt;
		if (m_openTimeSec <= 0)
		{
			m_openTimeSec = 0;
			b_isNotifyMainInfo = true;
			notify(GoldActivityNotify_Type_Tips);
		}
	}
	
	notify(GoldActivityNotify_Type_Timer);
}

void GoldActivityModel::setLocalTime()
{
	if (m_openTimeSec == 0 && !b_isNotifyMainInfo) ///有资格开启
	{
		b_isNotifyMainInfo = true;
		notify(GoldActivityNotify_Type_Tips);
	}

	if (m_actOverSec > 0 || m_openTimeSec > 0)
	{
		int localTime = m_actOverSec > m_openTimeSec ? m_actOverSec : m_openTimeSec;
		setEndTimeStamp(localTime  + LocalTimer::getInstance()->getCurServerTime());

		LocalTimer::getInstance()->addLocalTimerListener(this);
	}
}

void GoldActivityModel::parseTableInfo(const Json::Value &data)
{
	m_isLoad = true;

	m_actOverSec = data["remaining_time"].asInt();
	m_openTimeSec = data["opentime"].asInt();
	m_changeCount = data["drawcount"].asInt();
	m_goldNum = data["changgold"].asInt(); //上次记录

	m_GoldTbale.vipVec.clear();
	m_GoldTbale.costVec.clear();
	m_GoldTbale.showVec.clear();
	StringFormat::parseCsvStringVec(data["vip"].asString(), m_GoldTbale.vipVec);
	StringFormat::parseCsvStringVec(data["cost"].asString(), m_GoldTbale.costVec);
	StringFormat::parseCsvStringVec(data["show"].asString(), m_GoldTbale.showVec);

	m_GoldTbale.tips = data["tips"].asString();
	m_GoldTbale.info = data["info"].asString();

	m_OtherChange.clear();
	for (int i = 0; i < data["historylist"].size(); i++)
	{
		OtherPlayerChange info;
		info.ChangeInfo.push_back(data["historylist"][i]["name"].asString());
		info.ChangeInfo.push_back(TO_STR(data["historylist"][i]["gold"].asInt()));

		m_OtherChange.push_front(info);
	}

	setLocalTime();
	notify(GoldActivityNotify_Type_Info);
}

void GoldActivityModel::parsechangeGold(const Json::Value &data)
{
	MainModel::getInstance()->setGold(data["curGold"].asInt());
	MainModel::getInstance()->notifyObservers();

	m_goldNum = data["changegold"].asInt();
	m_changeCount = data["drawcount"].asInt();

	notify(GoldActivityNotify_Type_ChangeGold);
}

void GoldActivityModel::parseotherChangeGoldInfo(const Json::Value &data)
{
	if (data["id"].asInt() == MainModel::getInstance()->getMainParam()->mRoleId)
	{
		m_myChangeInfo.ChangeInfo.clear();
		m_myChangeInfo.ChangeInfo.push_back(data["name"].asString());
		m_myChangeInfo.ChangeInfo.push_back(TO_STR(data["gold"].asInt()));
	}
	else
	{
		if (m_OtherChange.size() >= PlayerChangeInfoNum)
		{
			m_OtherChange.pop_front();
		}
		OtherPlayerChange info;
		info.ChangeInfo.push_back(data["name"].asString());
		info.ChangeInfo.push_back(TO_STR(data["gold"].asInt()));

		m_OtherChange.push_back(info);
	}
	
	notify(GoldActivityNotify_Type_OtherChangeGold);
}

void GoldActivityModel::getMoney()
{
	if (m_OtherChange.size() >= PlayerChangeInfoNum)
	{
		m_OtherChange.pop_front();
	}
	OtherPlayerChange info;
	info.ChangeInfo.push_back(m_myChangeInfo.ChangeInfo.at(0));
	info.ChangeInfo.push_back(m_myChangeInfo.ChangeInfo.at(1));

	m_OtherChange.push_back(info);

	m_myChangeInfo.ChangeInfo.clear();
	notify(GoldActivityNotify_Type_OtherChangeGold);
}

bool GoldActivityModel::isHaveChange()
{
	if (m_actOverSec <= 0 || m_changeCount >= m_GoldTbale.vipVec.size())
	{
		return false;
	}
	else 
	{
		return true;
	}
}
