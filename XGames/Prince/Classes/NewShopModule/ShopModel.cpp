#include "ShopModel.h"
#include "PvpModule/PvpModel.h"
#include "TowerModule/TowerModel.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "ToolModule/ToolModel.h"
#include "PvpShopBase.h"
#include "TrailShopBase.h"
#include "GroupShopBase.h"
#include "ZahuoShopBase.h"
#include "CuilianShopBase.h"

#include "VIPModule/VipModel.h"
#include "ShopController.h"

ShopModel* ShopModel::m_pInstance = nullptr;

ShopModel::ShopModel(void)
	: m_nMaxRefreshCount(0)
	, m_nRefreshCount(0)
	, m_pAttr(nullptr)
	, m_freshCount(5)
	, m_isFree(false)
	, m_oneChou(0)
	, m_tenChou(0)
	, m_isPushScene(false)
	, m_root(nullptr)
	, m_SuccessFul(false)
	, m_freshSuc(false)
	, m_firstChou(false)
	, m_isShowFlage(false)
{
	m_vcAllItems.clear();
	m_canGetPram.clear();
	m_ChouPram.clear();
}


ShopModel::~ShopModel(void)
{
}

ShopModel* ShopModel::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new ShopModel;
	}

	return m_pInstance;
}

void ShopModel::destroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void ShopModel::setShopType(SHOP_TYPE type)
{
	m_shopType = type;
	initAttr(m_shopType);
}

SHOP_TYPE ShopModel::getShopType()
{
	return m_shopType;
}

void ShopModel::initAttr(SHOP_TYPE shopType)
{
	switch (shopType)
	{
	case ST_CUILIAN:
		m_pAttr = new CuilianShopBase;
		break;
	case ST_RONGYU:
		m_pAttr = new PvpShopBase;
		break;
	case ST_JIFENG:
		m_pAttr = new TrailShopBase;
		break;
	case ST_NONE:
		m_pAttr = nullptr;//new NoneShopBase;
	case ST_ZAHUO:
		m_pAttr = new ZahuoShopBase;
		break;
	case ST_SEHGNWANG:
		m_pAttr = new GroupShopBase;
		break;
	default:
		break;
	}
}

void ShopModel::updateUI( SHOP_EVENT_TYEP type, void* data )
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}

void ShopModel::updateAllItems( vector<ShopItemParam> vcAllItems )
{
	m_vcAllItems.clear();
	m_vcAllItems = vcAllItems;

	for (auto iter = m_vcAllItems.begin(); iter != m_vcAllItems.end(); iter++)
	{
		RowData* _row = DataManager::getInstance()->searchToolById(iter->nId);
		iter->strName = (_row == nullptr) ? "" : _row->getStringData("itemName");
	}

	// 更新至UI;
	//updateUI(SHOP_EVENT_TYEP_ALL_ITEM, (&m_vcAllItems));
	//bugly上这一部有崩溃问题 偿试着不传参,直接从model上拿数据
	updateUI(SHOP_EVENT_TYEP_ALL_ITEM,nullptr);

}

ShopItemParam* ShopModel::getItem(int index)
{
	int nSrvPos = m_vcAllItems.at(index).nSrvPos;

	auto iter = find(m_vcAllItems.begin(), m_vcAllItems.end(), nSrvPos);
	if (iter != m_vcAllItems.end())
	{
		return &(*iter);
	}
	return nullptr;
}

int ShopModel::isItemValid( int nIndex )
{
	if (m_shopType == ST_CUILIAN)
	{
		if (nIndex >= 0 && nIndex <= vcGoods.size() - 1)
		{
			// 3.验证货币数量和状态;
			switch (vcGoods.at(nIndex).moneyType)
			{
			case MONEY_GOLD:
				{
					if (MainModel::getInstance()->getMainParam()->mGold < vcGoods.at(nIndex).nPrice )
					{
						return -5;
					}
				}
				break;
			case MONEY_COIN:
				{
					if (MainModel::getInstance()->getMainParam()->mCoin < vcGoods.at(nIndex).nPrice)
					{
						return -6;
					}
				}
				break;
			case MONEY_COMMON:
				{
					if (getMoney() < vcGoods.at(nIndex).nPrice)
					{
						return -3;
					}
				}
			default:
				break;
			}
			// 可购买，返回Pos用来回传给服务器;
			return vcGoods.at(nIndex).nSrvPos;
		}
	}
	else
	{
		if (nIndex >= 0 && nIndex <= m_vcAllItems.size() - 1)
		{
			// 3.验证货币数量和状态;
			switch (m_vcAllItems.at(nIndex).moneyType)
			{
			case MONEY_GOLD:
				{
					if (MainModel::getInstance()->getMainParam()->mGold < m_vcAllItems.at(nIndex).nPrice )
					{
						return -5;
					}
				}
				break;
			case MONEY_COIN:
				{
					if (MainModel::getInstance()->getMainParam()->mCoin < m_vcAllItems.at(nIndex).nPrice)
					{
						return -6;
					}
				}
				break;
			case MONEY_COMMON:
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
	}
	
	return -1;
}

void ShopModel::onItemSold( int nSrvPos, int flag )
{
	// 更新货物状态;
	auto iter = std::find(m_vcAllItems.begin(), m_vcAllItems.end(), nSrvPos);
	if (iter != m_vcAllItems.end())
	{
		if ((*iter).buyLimit != -1 && (*iter).buyLimit>flag)
		{
			(*iter).bEnabled = true;
		}
		else if ((*iter).buyLimit != -1 && (*iter).buyLimit<=flag)
		{
			// 已售;
			(*iter).bEnabled = false;
		}
		else if ((*iter).buyLimit == -1)
		{
			(*iter).bEnabled = true;
		}
		// 更新至UI;
		updateUI(SHOP_EVENT_TYEP_SINGLE_ITEM, (&(*iter)));
	}

	// 更新UI公共信息;
	updatePublicInfo();

}

void ShopModel::updatePublicInfo()
{
	// 0-货币  1-次数;
	vector<int>  vcInfo;

	// 1. 获取货币总量;
	vcInfo.push_back(getMoney());

	// 2. 次数;
	vcInfo.push_back(getLeftRefreshCount());

	// 更新至UI;
	updateUI(SHOP_EVENT_TYEP_PUBLIC, (&vcInfo));
}

void ShopModel::updateFirstShopInfo()
{
	updateUI(SHOP_EVENT_TYEP_FIRST_SHOP,nullptr);
}

void ShopModel::updateChouInfo(CHOU_TYPE type)
{
	if (type ==ONE_CHOU)
	{
		updateUI(SHOP_EVENT_TYEP_ONE_CHOU,nullptr);
	}
	else if (type ==TEN_CHOU)
	{
		updateUI(SHOP_EVENT_TYEP_TEN_CHOU,nullptr);
	}
	
}

bool ShopModel::checkPrice( int nItemIndex )
{
	bool bRet = false;

	ShopItemParam itemParam;
	if (m_shopType == ST_CUILIAN)
	{
		itemParam = vcGoods.at(nItemIndex);
	}
	else
	{
		itemParam = m_vcAllItems.at(nItemIndex);
	}
	
	switch (itemParam.moneyType)
	{
		case MONEY_GOLD:
			{
				return itemParam.nPrice > MainModel::getInstance()->getMainParam()->mGold;
			}
			break;
		case MONEY_COIN:
			{
				return itemParam.nPrice > MainModel::getInstance()->getMainParam()->mCoin;
			}
			break;
		case MONEY_COMMON:
			{
				return itemParam.nPrice > getMoney();
			}
			break;
		default:
			break;
	}
	return bRet;
}

int ShopModel::getMaxRefreshCount()
{
	DataTable* _dt = DataManager::getInstance()->getDTVip();
	assert(_dt != nullptr);
	int nCurVipLv = VipModel::getInstance()->getCurVipLv();
	int nCurValue = 0;
	RowData* _row = _dt->searchDataById(nCurVipLv);
	if (_row != nullptr)
	{
		if (m_pAttr)
		{
			nCurValue = _row->getIntData(m_pAttr->getMaxRefreshTimesFieldByVip());
		}
		
	}
	return nCurValue;
}

void ShopModel::updateRefreshCount()
{
	if (m_pAttr)
	{
		m_pAttr->updateRefreshTimes();
	}
}

int ShopModel::getLeftRefreshCount()
{
	if (m_pAttr)
	{
		return getMaxRefreshCount() - m_pAttr->getRefreshTimes();
	}
	return 0;
}

int ShopModel::getRefreshCost()
{
	if (m_pAttr)
	{
		return m_pAttr->getRefreshCost();
	}
	return 0;
}

int ShopModel::getMoney()
{
	if (m_pAttr)
	{
		return m_pAttr->getMoney();
	}
	return 0;
}

bool ShopModel::checkRefreshCost()
{
	if (m_pAttr)
	{
		return m_pAttr->checkRefreshCost();
	}
	return false;
}

void ShopModel::updateCuilian()
{
	updateUI(SHOP_EVENT_TYEP_CUILIAN_PUB,nullptr);
}

void ShopModel::initCuiLian()
{
	for (auto iter= vcGoods.begin(); iter != vcGoods.end(); ++iter)
	{
		RowData* _row = DataManager::getInstance()->searchToolById(iter->nId);
		iter->strName = (_row == nullptr) ? "" : _row->getStringData("itemName");
	}

	updateUI(SHOP_EVENT_TYEP_CUILIAN,nullptr);
}

void ShopModel::timeOut(  )
{
	//通知主界面是否有提示

	ShopModel::getInstance()->setIsFree(true);
	ShopModel::getInstance()->setIsShowFlag(true);
	//通知主城
	ObserverParam param;
	param.id = FREECHOUTIME;
	param.updateData = nullptr;
	this->notifyObservers((void*)&param);
}

void ShopModel::timeTick(int dt)
{
	//通知刷新倒计时
	m_nStamp = m_nStamp - dt;
	updateUI(SHOP_EVENT_TYEP_COUNT_DOWN, nullptr);

}

void ShopModel::setLocalTime()
{
	//时间不等于0
	if (m_nStamp>0)
	{
		setEndTimeStamp(m_nStamp + LocalTimer::getInstance()->getCurServerTime());

		LocalTimer::getInstance()->addLocalTimerListener(this);
	}

	if (m_nStamp <= 0)
	{
		ShopController::getInstance()->handleShopInfo(ShopModel::getInstance()->getShopType());
	}
}

void ShopModel::setEndTime(int timestate)
{

	//时间不等于0
	if (timestate>0)
	{
		setTimeStamp(timestate + LocalTimer::getInstance()->getCurServerTime());

		LocalTimer::getInstance()->addTimeStampListener(this);
	}
	int count = ShopModel::getInstance()->getFreshCount();
	bool isFree = ShopModel::getInstance()->getIsFree();
	if (count>0 || isFree)
	{
		//通知主城
		ObserverParam param;
		param.id = FREECHOUTIME;
		param.updateData = nullptr;
		this->notifyObservers((void*)&param);
	}
}


