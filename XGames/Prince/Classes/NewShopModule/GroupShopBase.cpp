#include "GroupShopBase.h"
#include "MainModule/MainModel.h"
#include "GroupModule/MyGroupModel.h"


GroupShopBase::GroupShopBase(void)
{
	m_shopType = ST_SEHGNWANG;
}


GroupShopBase::~GroupShopBase(void)
{
}

int GroupShopBase::getMoney()
{
	return MainModel::getInstance()->getMainParam()->mGroupMoney;
}



int GroupShopBase::getSysId()
{
	return 10061;
}

std::string GroupShopBase::getMaxRefreshTimesFieldByVip()
{
	return "factionshop";
}

void GroupShopBase::updateRefreshTimes()
{
	MainModel::getInstance()->getTradingStatus()->mFactionShopRefreshNum++;
	MainModel::getInstance()->notifyObservers();
}

int GroupShopBase::getRefreshTimes()
{
	return MainModel::getInstance()->getTradingStatus()->mFactionShopRefreshNum;
}
