#include "CommonBuyModel.h"
#include "MainModule/MainModel.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "VIPModule/VipModel.h"
#include "DoubleBonus/DoubleBonusModel.h"

CommonBuyModel* CommonBuyModel::m_pInstance = nullptr;

CommonBuyModel::CommonBuyModel(void)
	: m_type(COMMON_BUY_TYPE::COMMON_BUY_TYPE_GOLD_COIN)
{
}


CommonBuyModel::~CommonBuyModel(void)
{
}

CommonBuyModel* CommonBuyModel::getInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CommonBuyModel;
	}
	return m_pInstance;
}

void CommonBuyModel::destroyInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

int CommonBuyModel::getTimes()
{
	int nTimes = 0;
	switch (m_type)
	{
	case COMMON_BUY_TYPE_GOLD_POWER:
		nTimes = MainModel::getInstance()->getTradingStatus()->mPowerNum;
		break;
	case COMMON_BUY_TYPE_GOLD_ENERGY:
		nTimes = MainModel::getInstance()->getTradingStatus()->mEnergyNum;
		break;
	case COMMON_BUY_TYPE_GOLD_MAZEPOWER:
		nTimes = MainModel::getInstance()->getTradingStatus()->mActionNum;
		break;
	case COMMON_BUY_TYPE_GOLD_COIN:
		nTimes = MainModel::getInstance()->getTradingStatus()->mCoinNum;
		break;
	default:
		break;
	}

	return nTimes;
}

void CommonBuyModel::setTimes( int nTimes )
{
	switch (m_type)
	{
	case COMMON_BUY_TYPE_GOLD_POWER:
		MainModel::getInstance()->getTradingStatus()->mPowerNum = nTimes;
		break;
	case COMMON_BUY_TYPE_GOLD_ENERGY:
		MainModel::getInstance()->getTradingStatus()->mEnergyNum = nTimes;
		break;
	case COMMON_BUY_TYPE_GOLD_MAZEPOWER:
		MainModel::getInstance()->getTradingStatus()->mActionNum = nTimes;
		break;
	case COMMON_BUY_TYPE_GOLD_COIN:
		MainModel::getInstance()->getTradingStatus()->mCoinNum = nTimes;
		break;
	default:
		break;
	}
	MainModel::getInstance()->notifyObservers();
}

int CommonBuyModel::getCurMaxTimes()
{
	DataTable* _dt = DataManager::getInstance()->getDTVip();
	assert(_dt != nullptr);
	int nCurVipLv = VipModel::getInstance()->getCurVipLv();
	int nCurValue = 0;
	RowData* _row = _dt->searchDataById(nCurVipLv);
	if (_row != nullptr)
	{
		nCurValue = _row->getIntData(getTypeField());
	}
	return nCurValue;
}

std::string CommonBuyModel::getTypeField()
{
	string strField = "";
	switch (m_type)
	{
	case COMMON_BUY_TYPE_GOLD_POWER:
		strField.assign("power");
		break;
	case COMMON_BUY_TYPE_GOLD_ENERGY:
		strField.assign("energy");
		break;
	case COMMON_BUY_TYPE_GOLD_MAZEPOWER:
		strField.assign("action");
		break;
	case COMMON_BUY_TYPE_GOLD_COIN:
		strField.assign("coinNum");
		break;
	default:
		break;
	}
	return strField;
}

int CommonBuyModel::getNextValidVip(int& nNextValue)
{
	// 当前VIP属性;
	int nCurVipLv = VipModel::getInstance()->getCurVipLv();
	int nCurValue = getCurMaxTimes();
	int nNextVipLv = nCurVipLv;
	nNextValue = nCurValue;

	// 下一级;
	int nLoopLv = nNextVipLv;
	DataTable* _dt = DataManager::getInstance()->getDTVip();
	assert(_dt != nullptr);
	RowData* _rowNext = _dt->searchDataById(++nLoopLv);
	while (_rowNext != nullptr)
	{
		// 下一级的值;
		nNextValue = _rowNext->getIntData(getTypeField());
		if (nNextValue > nCurValue)
		{
			nNextVipLv = nLoopLv;
			break;
		}
		else
			_rowNext = _dt->searchDataById(++nLoopLv);
	}

	return nNextVipLv;
}

void CommonBuyModel::updateData( COMMON_BUY_TYPE type )
{
	m_type = type;

	CommonBuyData  data;
	data.type = m_type;
	data.nNum = getCurMaxTimes() - getTimes();
	data.nTotalNum = getCurMaxTimes();
	data.nCostGold = getPrice();
	data.nAddValue = getBuyAmount();

	// 多次;
	data.nTimes = getMultiTimes();

	// 更新UI;
	ObserverParam param;
	param.id = 0;
	param.updateData = (void*)(&data);
	this->notifyObservers((void*)&param);
}

int CommonBuyModel::getPrice()
{
	int nNextTime = getTimes() + 1;
	int nPriceId = 0;
	int nAmountId = 0;
	getSysId(nPriceId, nAmountId);
	RowData* _row = DataManager::getInstance()->searchToolSystemById(nPriceId);
	assert(_row != nullptr);
	vector<int>  vcPrice;
	StringFormat::parseCsvStringVec(_row->getStringData("value"), vcPrice);
	assert(!vcPrice.empty());
	int nIndex = (nNextTime > vcPrice.size() ? vcPrice.size()-1 : nNextTime-1);
	return vcPrice.at(nIndex);
}

int CommonBuyModel::getBuyAmount()
{
	int nPriceId = 0;
	int nAmountId = 0;
	getSysId(nPriceId, nAmountId);
	RowData* _row = DataManager::getInstance()->searchToolSystemById(nAmountId);
	assert(_row != nullptr);
	return _row->getIntData("value");
}

void CommonBuyModel::getSysId( int& nPriceId, int& nAmountId )
{
	switch (m_type)
	{
	case COMMON_BUY_TYPE_GOLD_POWER:
		{
			nPriceId = 10066;
			nAmountId = 10068;
		}
		break;
	case COMMON_BUY_TYPE_GOLD_ENERGY:
		{
			nPriceId = 10067;
			nAmountId = 10069;
		}
		break;
	case COMMON_BUY_TYPE_GOLD_MAZEPOWER:
		{
			nPriceId = 10072;
			nAmountId = 10073;
		}
		break;
	case COMMON_BUY_TYPE_GOLD_COIN:
		{
			nPriceId = 10064;
			nAmountId = 10065;
		}
		break;
	default:
		assert(false);
		break;
	}
}

int CommonBuyModel::getMultiTimes()
{
	if (m_type != COMMON_BUY_TYPE_GOLD_COIN)
	{
		return 1;
	}
	int nLeftTimes = getCurMaxTimes() - getTimes();
	return (nLeftTimes > 5 ? 5 : nLeftTimes);
}

bool CommonBuyModel::isDoubleValidByBuyTimes()
{
	// 不仅有活动开启/关闭时间限制，通用购买这里还有<前xx次购买才双倍>的限制;
	// 本方法只检查<前xx次购买才双倍>的条件;
	const vector<DoubleBonusData*>* vcDoubleBonusData = DoubleBonusModel::getInstance()->getDataByType(checkTypeByBuyType(m_type));
	int nCount = 0;
	if (vcDoubleBonusData != nullptr)
	{
		// 从服务器下发的数据中读出"前xx次"的值;
		nCount = max(0, vcDoubleBonusData->at(0)->nReservedParam1);
	}

	return (getTimes() < nCount);
}

COMMON_BUY_TYPE CommonBuyModel::checkTypeByDoubleBonusType( DOUBLE_BONUS_TYPE type )
{
	COMMON_BUY_TYPE buyType = COMMON_BUY_TYPE_GOLD_POWER;
	switch (type)
	{
	case DOUBLE_BONUS_TYPE_BUY_COIN:
		buyType = COMMON_BUY_TYPE_GOLD_COIN;
		break;
	case DOUBLE_BONUS_TYPE_BUY_POWER:
		buyType = COMMON_BUY_TYPE_GOLD_POWER;
		break;
	default:
		buyType = COMMON_BUY_TYPE_GOLD_POWER;
		break;
	}
	return buyType;
}

DOUBLE_BONUS_TYPE CommonBuyModel::checkTypeByBuyType(COMMON_BUY_TYPE buyType)
{
	DOUBLE_BONUS_TYPE type = DOUBLE_BONUS_TYPE_NONE;
	switch (buyType)
	{
	case COMMON_BUY_TYPE_GOLD_POWER:
		type = DOUBLE_BONUS_TYPE_BUY_POWER;
		break;
	case COMMON_BUY_TYPE_GOLD_COIN:
		type = DOUBLE_BONUS_TYPE_BUY_COIN;
		break;
	default:
		break;
	}
	return type;
}
