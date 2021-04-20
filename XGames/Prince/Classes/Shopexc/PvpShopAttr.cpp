#include "PvpShopAttr.h"
#include "MainModule/MainModel.h"
#include "PvpModule/PvpModel.h"


PvpShopAttr::PvpShopAttr(void)
{
	m_shopType = SHOP_TYPE_ARENA;
}


PvpShopAttr::~PvpShopAttr(void)
{
}

void PvpShopAttr::setMoney(int nMoney)
{
	MainModel::getInstance()->getMainParam()->mPvpScore = nMoney;
	MainModel::getInstance()->notifyObservers();
}

int PvpShopAttr::getMoney()
{
	return MainModel::getInstance()->getMainParam()->mPvpScore;
}

bool PvpShopAttr::isItemValid(const ShopexcItemParam& param)
{
	bool bRet = false;

	// 1.竞技场商店限购条件 - 排名;
	if (param.nRank == -1 || PvpModel::getInstance()->getpvpOrder() <= param.nRank)
	{
		bRet = true;
	}
	else
	{
		CustomTips::show("未达到排名条件");
	}

	return bRet;
}

int PvpShopAttr::getSysId()
{
	return 10059;
}

std::string PvpShopAttr::getMaxRefreshTimesFieldByVip()
{
	return "arenashop";
}

void PvpShopAttr::updateRefreshTimes()
{
	MainModel::getInstance()->getTradingStatus()->mArenaShopRefreshNum++;
	MainModel::getInstance()->notifyObservers();
}

int PvpShopAttr::getRefreshTimes()
{
	return MainModel::getInstance()->getTradingStatus()->mArenaShopRefreshNum;
}
