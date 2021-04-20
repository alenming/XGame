#include "PvpShopBase.h"
#include "MainModule/MainModel.h"
#include "PvpModule/PvpModel.h"


PvpShopBase::PvpShopBase(void)
{
	m_shopType = ST_RONGYU;
}


PvpShopBase::~PvpShopBase(void)
{
}

int PvpShopBase::getMoney()
{
	return MainModel::getInstance()->getMainParam()->mPvpScore;
}


int PvpShopBase::getSysId()
{
	return 10059;
}

std::string PvpShopBase::getMaxRefreshTimesFieldByVip()
{
	return "arenashop";
}

void PvpShopBase::updateRefreshTimes()
{
	MainModel::getInstance()->getTradingStatus()->mArenaShopRefreshNum++;
	MainModel::getInstance()->notifyObservers();
}

int PvpShopBase::getRefreshTimes()
{
	return MainModel::getInstance()->getTradingStatus()->mArenaShopRefreshNum;
}
