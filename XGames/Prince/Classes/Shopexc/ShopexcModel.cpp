#include "ShopexcModel.h"
#include "PvpModule/PvpModel.h"
#include "TowerModule/TowerModel.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "ToolModule/ToolModel.h"
#include "PvpShopAttr.h"
#include "TrailShopAttr.h"
#include "RefineShop8Attr.h"
#include "RefineShop9Attr.h"
#include "VIPModule/VipModel.h"

ShopexcModel* ShopexcModel::m_pInstance = nullptr;

ShopexcModel::ShopexcModel(void)
	: m_nMaxRefreshCount(0)
	, m_nRefreshCount(0)
	, m_pAttr(nullptr)
	, m_SuccessFul(false)
{
	m_vcAllItems.clear();
}


ShopexcModel::~ShopexcModel(void)
{
}

ShopexcModel* ShopexcModel::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new ShopexcModel;
	}

	return m_pInstance;
}

void ShopexcModel::destroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void ShopexcModel::setShopType(ShopType type)
{
	m_shopType = type;
	initAttr(m_shopType);
}

ShopType ShopexcModel::getShopType()
{
	return m_shopType;
}

void ShopexcModel::initAttr(ShopType shopType)
{
	switch (shopType)
	{
	case SHOP_TYPE_NONE:
		break;
	case SHOP_TYPE_ARENA:
		m_pAttr = new PvpShopAttr;
		break;
	case SHOP_TYPE_TRAIL:
		m_pAttr = new TrailShopAttr;
		break;
	case SHOP_TYPE_FACTION:
		break;

	case SHOP_TYPE_REFINE_8:
		m_pAttr = new RefineShop8Attr;
		break;

	case SHOP_TYPE_REFINE_9:
		m_pAttr = new RefineShop9Attr;
		break;

	default:
		break;
	}
}

void ShopexcModel::updateUI( OBS_PARAM_TYPE_SHOPESC type, void* data )
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}

void ShopexcModel::updateAllItems( vector<ShopexcItemParam> vcAllItems )
{
	m_vcAllItems.clear();
	m_vcAllItems = vcAllItems;

	for (auto &it : m_vcAllItems)
	{
		RowData* _row = DataManager::getInstance()->searchToolById(it.nId);
		it.strName = (_row == nullptr) ? "" : _row->getStringData("itemName");
	}

	// 更新至UI;
	updateUI(OBS_PARAM_TYPE_SHOPESC_ALL_ITEM, (void*)(&m_vcAllItems));
}

ShopexcItemParam* ShopexcModel::getItem(int nSrvPos)
{
	auto iter = find(m_vcAllItems.begin(), m_vcAllItems.end(), nSrvPos);
	if (iter != m_vcAllItems.end())
	{
		return &(*iter);
	}
	return nullptr;
}

int ShopexcModel::isItemValid( int nIndex )
{
	if (nIndex >= 0 && nIndex <= m_vcAllItems.size() - 1)
	{
		// 1.验证物品状态;
		if (!m_vcAllItems.at(nIndex).bEnabled)
		{
			return -2;
		}

		// 2.各类型商店限制条件;
		if (!m_pAttr->isItemValid(m_vcAllItems.at(nIndex)))
		{
			return -4;
		}

		// 3.验证货币数量和状态;
		switch (m_vcAllItems.at(nIndex).costType)
		{
		case SHOP_COST_TYPE_GOLD:
			{
				if (MainModel::getInstance()->getMainParam()->mGold < m_vcAllItems.at(nIndex).nPrice )
				{
					return -5;
				}
			}
			break;
		case SHOP_COST_TYPE_COIN:
			{
				if (MainModel::getInstance()->getMainParam()->mCoin < m_vcAllItems.at(nIndex).nPrice)
				{
					return -6;
				}
			}
			break;
		case SHOP_COST_TYPE_COMMON:
			{
				if (getMoney() < m_vcAllItems.at(nIndex).nPrice)
				{
					return -3;
				}
			}
		default:
			break;
		}

		// 可购买，返回Pos用来回传给服务器;
		return m_vcAllItems.at(nIndex).nSrvPos;
	}

	return -1;
}

void ShopexcModel::onItemSold( int nSrvPos, int nLeftMoney )
{
	// 更新货物状态;
	auto iter = std::find(m_vcAllItems.begin(), m_vcAllItems.end(), nSrvPos);
	if (iter != m_vcAllItems.end())
	{
		// 已售;
		(*iter).bEnabled = false;

		// 更新至UI;
		updateUI(OBS_PARAM_TYPE_SHOPESC_SINGLE_ITEM, (void*)(&(*iter)));
	}

	// 更新剩余货币;
	setMoney(nLeftMoney);

	// 更新UI公共信息;
	updatePublicInfo();
}

void ShopexcModel::updatePublicInfo()
{
	// 0-货币  1-次数;
	vector<int>  vcInfo;

	// 1. 获取货币总量;
	vcInfo.push_back(getMoney());

	// 2. 次数;
	vcInfo.push_back(getLeftRefreshCount());

	// 更新至UI;
	updateUI(OBS_PARAM_TYPE_SHOPESC_PUBLIC, (void*)(&vcInfo));
}

void ShopexcModel::onCountDown( int nSec )
{
	updateUI(OBS_PARAM_TYPE_SHOPESC_COUNT_DOWN, (void*)(&nSec));
}

bool ShopexcModel::checkPrice( int nItemIndex )
{
	bool bRet = false;

	if (nItemIndex >= 0 && nItemIndex < m_vcAllItems.size())
	{
		ShopexcItemParam itemParam = m_vcAllItems.at(nItemIndex);
		switch (itemParam.costType)
		{
		case SHOP_COST_TYPE_GOLD:
			{
				return itemParam.nPrice > MainModel::getInstance()->getMainParam()->mGold;
			}
			break;
		case SHOP_COST_TYPE_COIN:
			{
				return itemParam.nPrice > MainModel::getInstance()->getMainParam()->mCoin;
			}
			break;
		case SHOP_COST_TYPE_COMMON:
			{
				return itemParam.nPrice > getMoney();
			}
			break;
		default:
			break;
		}
	}
	return bRet;
}

int ShopexcModel::getMaxRefreshCount()
{
	DataTable* _dt = DataManager::getInstance()->getDTVip();
	assert(_dt != nullptr);
	int nCurVipLv = VipModel::getInstance()->getCurVipLv();
	int nCurValue = 0;
	RowData* _row = _dt->searchDataById(nCurVipLv);
	if (_row != nullptr)
	{
		nCurValue = _row->getIntData(m_pAttr->getMaxRefreshTimesFieldByVip());
	}
	return nCurValue;
}

void ShopexcModel::updateRefreshCount()
{
	m_pAttr->updateRefreshTimes();
}

int ShopexcModel::getLeftRefreshCount()
{
	return getMaxRefreshCount() - m_pAttr->getRefreshTimes();
}

int ShopexcModel::getRefreshCost()
{
	return m_pAttr->getRefreshCost();
}

int ShopexcModel::getMoney()
{
	return m_pAttr->getMoney();
}

void ShopexcModel::setMoney(int nMoney)
{
	m_pAttr->setMoney(nMoney);
}

bool ShopexcModel::checkRefreshCost()
{
	return m_pAttr->checkRefreshCost();
}
