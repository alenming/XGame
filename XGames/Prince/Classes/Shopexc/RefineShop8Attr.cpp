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

	// ˢ����ͽ�Ҹ��������ã�����ʹ��ˢ���Ȼ��ʹ�ý��;
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
			// 10/30/2015 ��Сǿȷ�Ϲ���ˢ���������1����������ֶ�������;
			// system������ˢ���������������õģ�Ŀǰ�����õ�1����������óɶ�����п��ܴ˴�����;
			assert(false);
		}
	}
	else
	{
		return getRefreshCost() <= MainModel::getInstance()->getMainParam()->mGold;
	}
}
