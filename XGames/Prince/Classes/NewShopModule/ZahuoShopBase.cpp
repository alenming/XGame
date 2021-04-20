#include "ZahuoShopBase.h"
#include "MainModule/MainModel.h"
#include "GroupModule/MyGroupModel.h"


ZahuoShopBase::ZahuoShopBase(void)
{
	m_shopType = ST_ZAHUO;
}


ZahuoShopBase::~ZahuoShopBase(void)
{
}

int ZahuoShopBase::getMoney()
{
	return MainModel::getInstance()->getMainParam()->mGold;
}


int ZahuoShopBase::getSysId()
{
	return 10126;
}

std::string ZahuoShopBase::getMaxRefreshTimesFieldByVip()
{
	return "goldshop";
}

void ZahuoShopBase::updateRefreshTimes()
{
	MainModel::getInstance()->getTradingStatus()->mZahuoRefrashNum++;
	MainModel::getInstance()->notifyObservers();
}

int ZahuoShopBase::getRefreshTimes()
{
	return MainModel::getInstance()->getTradingStatus()->mZahuoRefrashNum;
}
