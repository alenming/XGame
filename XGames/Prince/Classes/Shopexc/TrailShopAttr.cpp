#include "TrailShopAttr.h"
#include "MainModule/MainModel.h"
#include "TowerModule/TowerModel.h"

TrailShopAttr::TrailShopAttr(void)
{
	m_shopType = SHOP_TYPE_TRAIL;
}


TrailShopAttr::~TrailShopAttr(void)
{
}

void TrailShopAttr::setMoney(int nMoney)
{
	TowerModel::getInstance()->setScore(nMoney);
}

int TrailShopAttr::getMoney()
{
	return TowerModel::getInstance()->getScore();
}

bool TrailShopAttr::isItemValid(const ShopexcItemParam& param)
{
	return true;
}

int TrailShopAttr::getSysId()
{
	return 10060;
}

std::string TrailShopAttr::getMaxRefreshTimesFieldByVip()
{
	return "trialshop";
}

void TrailShopAttr::updateRefreshTimes()
{
	MainModel::getInstance()->getTradingStatus()->mTrailShopRefreshNum++;
	MainModel::getInstance()->notifyObservers();
}

int TrailShopAttr::getRefreshTimes()
{
	return MainModel::getInstance()->getTradingStatus()->mTrailShopRefreshNum;
}
