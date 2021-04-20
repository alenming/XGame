#include "RefineShop8Attr.h"
#include "MainModule/MainModel.h"
#include "ToolModule/ToolModel.h"

RefineShop8Attr::RefineShop8Attr(void)
{
}


RefineShop8Attr::~RefineShop8Attr(void)
{
}

void RefineShop8Attr::setMoney(int nMoney)
{
	MainModel::getInstance()->getMainParam()->mHunTiandan = nMoney;
	MainModel::getInstance()->notifyObservers();
}

int RefineShop8Attr::getMoney()
{
	return MainModel::getInstance()->getMainParam()->mHunTiandan;
}

bool RefineShop8Attr::isItemValid(const ShopexcItemParam& param)
{
	return true;
}

int RefineShop8Attr::getSysId()
{
	return 0;
}

std::string RefineShop8Attr::getMaxRefreshTimesFieldByVip()
{
	return "hunTiandan";
}

void RefineShop8Attr::updateRefreshTimes()
{
	MainModel::getInstance()->getTradingStatus()->mRefineShop8RefreshNum++;
	MainModel::getInstance()->notifyObservers();
}

int RefineShop8Attr::getRefreshTimes()
{
	return MainModel::getInstance()->getTradingStatus()->mRefineShop8RefreshNum;
}

int RefineShop8Attr::getRefreshCost()
{
	int nCost = 0;

	// 刷新令和金币各自有配置，优先使用刷新令，然后使用金币;
	int nId = (ToolModel::getInstance()->searchNumByTemplateId(10005) > 0) ? 10100 : 10101;
	RowData* _data = DataManager::getInstance()->getDTToolSystem()->searchDataById(nId);
	assert(_data != nullptr);
	nCost = _data->getIntData("value");

	return nCost;
}

bool RefineShop8Attr::checkRefreshCost()
{
	int nRefreshTokenCount = ToolModel::getInstance()->searchNumByTemplateId(10005);
	if (nRefreshTokenCount > 0)
	{
		CCLOG("%d--------------------%d",getRefreshCost(),nRefreshTokenCount);
		if (getRefreshCost() <= nRefreshTokenCount)
		{
			return true;
		}
		else
		{
			// 10/30/2015 跟小强确认过，刷新令单次消耗1个，不会出现多个的情况;
			// system表是有刷新令消耗数量配置的，目前是配置的1个，如果配置成多个，有可能此处出错;
			assert(false);
		}
	}
	else
	{
		return getRefreshCost() <= MainModel::getInstance()->getMainParam()->mGold;
	}
}
