#include "MysteryShopModel.h"
#include "VIPModule/VipModel.h"
#include "MysteryShopController.h"


#define MYSTERY_COST_ID		10139
MysteryShopModel* MysteryShopModel::m_pInstance = nullptr;

MysteryShopModel::MysteryShopModel(void)
{
	m_vcAllItems.clear();
	m_SuccessFul = false;
	m_freshSuc = false;
	m_nStamp = 0;
	m_nDaoTime = 0;
	m_ismainShow = false;
	m_isiterLayerShow = false;
}

MysteryShopModel::~MysteryShopModel(void)
{
}

MysteryShopModel* MysteryShopModel::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new MysteryShopModel;
	}

	return m_pInstance;
}

void MysteryShopModel::destroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}


void MysteryShopModel::updateUI( SHOP_EVENT_TYEP type, void* data )
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}


void MysteryShopModel::updatePublicInfo()
{
	// 0-货币  1-次数;
	vector<int>  vcInfo;

	// 2. 次数;
	vcInfo.push_back(this->getLeftRefreshCount());
	//价格
	vcInfo.push_back(this->getRefreshCost());
	// 更新至UI;
	updateUI(SHOP_EVENT_TYEP_PUBLIC, (&vcInfo));
}



ShopItemParam* MysteryShopModel::getItem(int index)
{
	int nSrvPos = m_vcAllItems.at(index).nSrvPos;

	auto iter = find(m_vcAllItems.begin(), m_vcAllItems.end(), nSrvPos);
	if (iter != m_vcAllItems.end())
	{
		return &(*iter);
	}
	return nullptr;
}


void MysteryShopModel::timeOut(  )
{
	//通知主界面是否有提示
	//时间到把状态还原成初始状态
	m_nStamp = 0;
	m_nDaoTime = 0;
	this->setIsInterShow(false);
	this->setIsMainShow(false);
	ObserverParam param;
	param.id = MYSTERY_SHOW;
	vector<bool> m_vec;
	m_vec.push_back(true);
	param.updateData = &m_vec;
	this->notifyObservers((void*)&param);
}

void MysteryShopModel::timeTick(int dt)
{
	//通知刷新倒计时
	m_nDaoTime = m_nDaoTime - dt;
	updateUI(SHOP_EVENT_TYEP_COUNT_DOWN, nullptr);

}

void MysteryShopModel::updateAllItems( vector<ShopItemParam> vcAllItems )
{
	m_vcAllItems.clear();
	m_vcAllItems = vcAllItems;

	for (auto iter = m_vcAllItems.begin(); iter != m_vcAllItems.end(); iter++)
	{
		RowData* _row = DataManager::getInstance()->searchToolById(iter->nId);
		iter->strName = (_row == nullptr) ? "" : _row->getStringData("itemName");
	}

	updateUI(SHOP_EVENT_TYEP_ALL_ITEM,nullptr);
}


int MysteryShopModel::isItemValid( int nIndex )
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
				//if (getMoney() < m_vcAllItems.at(nIndex).nPrice)
				//{
				//	return -3;
				//}
			}
		default:
			break;
		}
		// 可购买，返回Pos用来回传给服务器;
		return m_vcAllItems.at(nIndex).nSrvPos;

	}
	return -1;
}


void MysteryShopModel::onItemSold( int nSrvPos, int flag )
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

}

int  MysteryShopModel::getRefreshCost()
{
	int nCost = 0;

	// 从系统表读取每次价格;
	int nId = MYSTERY_COST_ID;

	if (-1 != nId)
	{
		RowData* _data = DataManager::getInstance()->getDTToolSystem()->searchDataById(nId);
		assert(_data != nullptr);
		string str = _data->getStringData("value");
		vector<int>  vcPrice;
		StringFormat::parseCsvStringVec(str, vcPrice);
		assert(vcPrice.size() > 0);
		int nIndex = getRefreshTimes() + 1;

		if (nIndex > vcPrice.size())
			nIndex = vcPrice.size();

		nCost = vcPrice.at(nIndex-1);
	}

	return nCost;
}

int MysteryShopModel::getRefreshTimes()
{
	return MainModel::getInstance()->getTradingStatus()->mMysteryRefrashNum;
}

int MysteryShopModel::getMaxRefreshCount()
{
	DataTable* _dt = DataManager::getInstance()->getDTVip();
	assert(_dt != nullptr);
	int nCurVipLv = VipModel::getInstance()->getCurVipLv();
	int nCurValue = 0;
	RowData* _row = _dt->searchDataById(nCurVipLv);
	if (_row != nullptr)
	{
		nCurValue = _row->getIntData("mysticalshop");
	}
	return nCurValue;
}

void MysteryShopModel::setStamp( int ty )
{
	if (ty > 0)
	{
		m_nStamp = ty;
		m_nDaoTime = m_nStamp - LocalTimer::getInstance()->getCurServerTime();

		if (m_nDaoTime > 0)
		{
			//this->setIsInterShow(false);
			//this->setIsMainShow(true);
			//开始定时
			setLocalTime();
			setEndTime();
		}

	}
}


void MysteryShopModel::setLocalTime()
{
	//时间不等于0
	if (m_nStamp>0)
	{
		setEndTimeStamp(m_nStamp);// + LocalTimer::getInstance()->getCurServerTime());

		LocalTimer::getInstance()->addLocalTimerListener(this);
	}

}

void MysteryShopModel::setEndTime( )
{

	//时间不等于0
	if (m_nDaoTime>0)
	{
		setTimeStamp(m_nStamp);// + LocalTimer::getInstance()->getCurServerTime());

		LocalTimer::getInstance()->addTimeStampListener(this);
	}

	//bool  vcInfo = false;
	//if (m_nDaoTime > 0)
	//{
	//	//通知主城打开显示
	//	ObserverParam param;
	//	param.id = MYSTERY_SHOW;
	//	vcInfo = (false);
	//
	//	param.updateData = &vcInfo;
	//	this->notifyObservers((void*)&param);
	//}
	//else
	//{
	//	//通知主城关闭显示
	//	ObserverParam param;
	//	param.id = MYSTERY_SHOW;
	//	vcInfo = (true);
	//	param.updateData = &vcInfo;
	//	this->notifyObservers((void*)&param);
	//}
}



int MysteryShopModel::getStamp()
{
	return m_nDaoTime;
}

void MysteryShopModel::updateRefreshCount(int dayCount)
{
	MainModel::getInstance()->getTradingStatus()->mMysteryRefrashNum = dayCount;
}

int MysteryShopModel::getLeftRefreshCount()
{
	return (this->getMaxRefreshCount()) - (this->getRefreshTimes());
}

bool MysteryShopModel::checkRefreshCost()
{
	return (getRefreshCost() <= MainModel::getInstance()->getMainParam()->mGold)?true:false;
}