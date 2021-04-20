#include "CuilianShopBase.h"
#include "MainModule/MainModel.h"
#include "GroupModule/MyGroupModel.h"


CuilianShopBase::CuilianShopBase(void)
{
	m_shopType = ST_NONE;
}


CuilianShopBase::~CuilianShopBase(void)
{
}

int CuilianShopBase::getMoney()
{
	return MainModel::getInstance()->getMainParam()->mCuiLianMoney;
}


int CuilianShopBase::getSysId()
{
	return 10059;
}

std::string CuilianShopBase::getMaxRefreshTimesFieldByVip()
{
	return "arenashop";
}

void CuilianShopBase::updateRefreshTimes()
{
}

int CuilianShopBase::getRefreshTimes()
{
	return 0;
}
