#include "StarsSuModel.h"

StarsSuModel* StarsSuModel::mInstance = nullptr;

StarsSuModel::StarsSuModel()
{
	m_getStatSuInfoCount = 0;
}

StarsSuModel::~StarsSuModel()
{

}

StarsSuModel* StarsSuModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new StarsSuModel();
	}
	return mInstance;
}

void StarsSuModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void StarsSuModel::notify(STARSSU_EVENT_TYPE eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void StarsSuModel::parseGetStarInfoData(const Json::Value &val)
{
	//英雄数据
	Json::Value petLists = val["petList"];
	HeroModel::getInstance()->updateData(petLists);

	MainModel::getInstance()->getMainParam()->mStarNum = val["starNum"].asInt(); //星星数
	MainModel::getInstance()->setSilver(val["coin"].asInt());//银币
	MainModel::getInstance()->notifyObservers();

	//解析星宿信息
	m_StarsSuInfo.lastsId = val["sId"].asInt();
	m_StarsSuInfo.CursId = m_StarsSuInfo.lastsId; //星宿ID
	m_StarsSuInfo.lastPoint = val["curPoint"].asInt();
	m_StarsSuInfo.curPoint = val["curPoint"].asInt(); //当前星点数
	m_StarsSuInfo.coinCost =val["coinCost"].asInt(); //、银币消耗
	m_StarsSuInfo.freeCount = val["freeCount"].asInt(); //免费次数
	m_StarsSuInfo.criPrb = val["criPrb"].asInt();

	notify(STARSSU_EVENT_TYPE_GET_INFO);

	m_getStatSuInfoCount++;
}

void StarsSuModel::parseAstrologyAndBreakOut(const Json::Value &val)
{
	//英雄数据
	Json::Value petLists = val["petList"];
	HeroModel::getInstance()->updateData(petLists);
	MainModel::getInstance()->getMainParam()->mStarNum = val["starNum"].asInt(); //星星数
	MainModel::getInstance()->setSilver(val["coin"].asInt());//金币银币
	MainModel::getInstance()->notifyObservers();

	//解析星宿信息
	m_StarsSuInfo.lastsId = m_StarsSuInfo.CursId;
	m_StarsSuInfo.CursId = val["sId"].asInt(); //星宿ID
	m_StarsSuInfo.lastPoint = m_StarsSuInfo.curPoint;
	m_StarsSuInfo.curPoint = val["curPoint"].asInt(); //当前星点数
	m_StarsSuInfo.coinCost =val["coinCost"].asInt(); //、银币消耗
	m_StarsSuInfo.freeCount = val["freeCount"].asInt(); //免费次数
	m_StarsSuInfo.criPrb = val["criPrb"].asInt();

	notify(STARSSU_EVENT_TYPE_ASTROLOGY_BREAK_OUT);
}


const STARSTSU_INFO& StarsSuModel::getStarssSuInfo()
{
	return m_StarsSuInfo;
}