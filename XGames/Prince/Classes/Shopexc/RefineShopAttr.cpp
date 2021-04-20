#include "RefineShopAttr.h"
#include "MainModule/MainModel.h"

RefineShopAttr::RefineShopAttr(void)
{
}


RefineShopAttr::~RefineShopAttr(void)
{
}
void RefineShopAttr::setMoney(int nMoney)
{
	//TowerModel::getInstance()->setScore(nMoney);
}

int RefineShopAttr::getMoney()
{
	//return TowerModel::getInstance()->getScore();
	return 0;
}

bool RefineShopAttr::isItemValid(const ShopexcItemParam& param)
{
	return true;
}

int RefineShopAttr::getSysId()
{
	return 10060;
}

std::string RefineShopAttr::getMaxRefreshTimesFieldByVip()
{
	return "trialshop";
}

void RefineShopAttr::updateRefreshTimes()
{
	MainModel::getInstance()->getTradingStatus()->mTrailShopRefreshNum++;
	MainModel::getInstance()->notifyObservers();
}

int RefineShopAttr::getRefreshTimes()
{
	return MainModel::getInstance()->getTradingStatus()->mTrailShopRefreshNum;
}
