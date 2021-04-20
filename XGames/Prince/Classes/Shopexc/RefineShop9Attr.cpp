#include "RefineShop9Attr.h"
#include "MainModule/MainModel.h"
#include "ToolModule/ToolModel.h"

RefineShop9Attr::RefineShop9Attr(void)
{
}


RefineShop9Attr::~RefineShop9Attr(void)
{
}

void RefineShop9Attr::setMoney(int nMoney)
{
	MainModel::getInstance()->getMainParam()->mJiuZhuandan = nMoney;
	MainModel::getInstance()->notifyObservers();
}

int RefineShop9Attr::getMoney()
{
	return MainModel::getInstance()->getMainParam()->mJiuZhuandan;
}

bool RefineShop9Attr::isItemValid(const ShopexcItemParam& param)
{
	return true;
}

int RefineShop9Attr::getSysId()
{
	return 0;
}

std::string RefineShop9Attr::getMaxRefreshTimesFieldByVip()
{
	return "jiuZhuandan";
}

void RefineShop9Attr::updateRefreshTimes()
{
	MainModel::getInstance()->getTradingStatus()->mRefineShop9RefreshNum++;
	MainModel::getInstance()->notifyObservers();
}

int RefineShop9Attr::getRefreshTimes()
{
	return MainModel::getInstance()->getTradingStatus()->mRefineShop9RefreshNum;
}

int RefineShop9Attr::getRefreshCost()
{
	int nCost = 0;

	// ˢ����ͽ�Ҹ��������ã�����ʹ��ˢ���Ȼ��ʹ�ý��;
	int nId = (ToolModel::getInstance()->searchNumByTemplateId(10005) > 0) ? 10102 : 10103;
	RowData* _data = DataManager::getInstance()->getDTToolSystem()->searchDataById(nId);
	assert(_data != nullptr);
	nCost = _data->getIntData("value");

	return nCost;
}

bool RefineShop9Attr::checkRefreshCost()
{
	int nRefreshTokenCount = ToolModel::getInstance()->searchNumByTemplateId(10005);
	if (nRefreshTokenCount > 0)
	{
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
