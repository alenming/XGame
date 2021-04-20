#include "TrailShopBase.h"
#include "MainModule/MainModel.h"
#include "TowerModule/TowerModel.h"

TrailShopBase::TrailShopBase(void)
{
	m_shopType = ST_JIFENG;
}


TrailShopBase::~TrailShopBase(void)
{
}

int TrailShopBase::getMoney()
{
	return MainModel::getInstance()->getMainParam()->mTrialScore;
}

int TrailShopBase::getSysId()
{
	return 10060;
}

std::string TrailShopBase::getMaxRefreshTimesFieldByVip()
{
	return "trialshop";
}

void TrailShopBase::updateRefreshTimes()
{
	MainModel::getInstance()->getTradingStatus()->mTrailShopRefreshNum++;
	MainModel::getInstance()->notifyObservers();
}

int TrailShopBase::getRefreshTimes()
{
	return MainModel::getInstance()->getTradingStatus()->mTrailShopRefreshNum;
}
