#include "ActivityModel.h"
#include "DataManager/DataManager.h"
#include "HeroModule/HeroModel.h"
#include "MainModule/MainModel.h"
#include "VIPModule/VipModel.h"

ActivityModel* ActivityModel::m_pInstance = nullptr;

ActivityModel::ActivityModel(void)
	: m_nCurSelActivityId(-1)
	, m_nCurSelActivityBarrierId(-1)
	, m_nCurSelActivityType(ACT_TYPE_CHIP_JiangHuChuanWen)
{
	m_mapActivityBarrierInfo.clear();
	m_mapActivityState.clear();
}


ActivityModel::~ActivityModel(void)
{
}

ActivityModel* ActivityModel::getInstance()
{
	if(nullptr == m_pInstance)
	{
		m_pInstance = new ActivityModel;
	}
	return m_pInstance;
}

void ActivityModel::destroyInstance()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void ActivityModel::updateActivityBarrierInfo(ActivityBarrierInfo info)
{
	auto iter = m_mapActivityBarrierInfo.find(info.nBarrierId);
	if (iter != m_mapActivityBarrierInfo.end())
	{
		iter->second = info;
	}
	else
	{
		m_mapActivityBarrierInfo.insert(pair<int, ActivityBarrierInfo>(info.nBarrierId, info));
	}
}

void ActivityModel::updateActivityState(ActivityState state)
{
	auto iter = m_mapActivityState.find(state.nActivityId);
	if (iter != m_mapActivityState.end())
	{
		iter->second = state;

		// 更新至UI;
		updateUI(OBS_PARAM_TYPE_ACTIVITY_NODE, (void*)&(iter->second));
	}
}

void ActivityModel::getActivityState(int nActivityId, ActivityState& state)
{
	auto iter = m_mapActivityState.find(nActivityId);
	if (iter != m_mapActivityState.end())
	{
		state = iter->second;
	}
}

void ActivityModel::updateUI(OBS_PARAM_TYPE_CM type, void* data)
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}


int ActivityModel::getPowerCost( int nActivityBarrierId )
{
	auto iter = m_mapActivityBarrierInfo.find(nActivityBarrierId);
	if (iter != m_mapActivityBarrierInfo.end())
	{
		return iter->second.nCost;
	}
	return 0;
}

void ActivityModel::updateActivityList( map<int, ActivityState> mapState )
{
	m_mapActivityState = mapState;
	vector<UI_ACTIVITY_NODE_PARAM>  vcUIParam;
	for (auto it : m_mapActivityState)
	{
		UI_ACTIVITY_NODE_PARAM  param;
		param.nActivityId = it.first;
		RowData* row = DataManager::getInstance()->searchActivityShowById(param.nActivityId);
		assert(row != nullptr);
		param.nType = (ACTIVITY_TYPE)(row->getIntData("type"));
		StringFormat::parseCsvStringVec(row->getStringData("rewardShow"), param.vcDrops);
		StringFormat::parseCsvStringVec(row->getStringData("monsterShow"), param.vcEnemy);
		param.nValidCount = it.second.nValidCount;
		param.nTotalCount = it.second.nTotalCount;
		vcUIParam.push_back(param);
	}

	// 更新至UI;
	updateUI(OBS_PARAM_TYPE_ACTIVITY_LIST, (void*)(&vcUIParam));
}

int ActivityModel::getNextValidVip( int& nNextValue )
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
		nNextValue = _rowNext->getIntData("activity");
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

int ActivityModel::getCurVipLeftTimes( int nActivityId )
{
	ActivityState  state;
	getActivityState(nActivityId, state);
	return getCurMaxTimes() - state.nBuyCount;
}

int ActivityModel::getCurMaxTimes()
{
	DataTable* _dt = DataManager::getInstance()->getDTVip();
	assert(_dt != nullptr);
	int nCurVipLv = VipModel::getInstance()->getCurVipLv();
	int nCurValue = 0;
	RowData* _row = _dt->searchDataById(nCurVipLv);
	if (_row != nullptr)
	{
		nCurValue = _row->getIntData("activity");
	}
	return nCurValue;
}

int ActivityModel::getBuyActivityCost( int nActivityId )
{
	ActivityState  state;
	getActivityState(nActivityId, state);
	int nNextTime = state.nBuyCount + 1;
	RowData* _row = DataManager::getInstance()->searchToolSystemById(10070);
	assert(_row != nullptr);
	vector<int>  vcPrice;
	StringFormat::parseCsvStringVec(_row->getStringData("value"), vcPrice);
	assert(!vcPrice.empty());
	int nIndex = (nNextTime > vcPrice.size() ? vcPrice.size()-1 : nNextTime-1);
	return vcPrice.at(nIndex);
}

void ActivityModel::getActivityBarrierByActivityId( int nActivityId, vector<ActivityBarrierInfo*>& vcBarrierInfo )
{
	vcBarrierInfo.clear();
	RowData* row = DataManager::getInstance()->searchActivityShowById(nActivityId);
	assert(row != nullptr);
	vector<int>  vcTmp;
	StringFormat::parseCsvStringVec(row->getStringData("fightId"), vcTmp);
	for (int i = 0; i < vcTmp.size(); ++i)
	{
		auto it = m_mapActivityBarrierInfo.find(vcTmp.at(i));
		if (it != m_mapActivityBarrierInfo.end())
		{
			vcBarrierInfo.push_back(&(it->second));
		}
	};
}

void ActivityModel::setCurActivityBarrierId( int nActivityId, int nActivityBarrierId )
{
	m_nCurSelActivityId = nActivityId;
	m_nCurSelActivityBarrierId = nActivityBarrierId;
}

void ActivityModel::getActivityHeroList( vector<pair<int, string> >& vcActivityHeroList )
{
	if (m_nCurSelActivityId == -1 || m_nCurSelActivityBarrierId == -1)
		return;

	RowData* row = DataManager::getInstance()->searchActivityShowById(m_nCurSelActivityId);
	assert(row != nullptr);
	vector<int>  vcTmp;
	StringFormat::parseCsvStringVec(row->getStringData("monsterShow"), vcTmp);
	if (vcTmp.size() > 0)
	{
		vcActivityHeroList.clear();
		for (int i = 0; i < vcTmp.size(); ++i)
		{
			RowData* _hero = DataManager::getInstance()->searchToolById(vcTmp.at(i));
			if (nullptr != _hero)
			{
				vcActivityHeroList.push_back(make_pair(vcTmp.at(i), _hero->getStringData("itemName")));
			}
		}
	}
}

bool ActivityModel::isActivityBarrierPassed( int nActivityId, int nActivityBarrierId )
{
	auto it = m_mapActivityState.find(nActivityId);
	assert(it != m_mapActivityState.end());
	auto it2 = it->second.mapBarrierRecord.find(nActivityBarrierId);
	if (it2 != it->second.mapBarrierRecord.end())
	{
		return (it2->second > 0);
	}
	return false;
}


int ActivityModel::isActivityBarrierValid( int nActivityId, int nActivityBarrierId )
{
	auto iter = m_mapActivityBarrierInfo.find(nActivityBarrierId);
	if (iter != m_mapActivityBarrierInfo.end())
	{
		return isActivityBarrierValid(nActivityId, iter->second);
	}
	return -9;
}


int ActivityModel::isActivityBarrierValid( int nActivityId, ActivityBarrierInfo activityInfo )
{
	// 体力判断;
	if (MainModel::getInstance()->getMainParam()->mPower < activityInfo.nCost)
	{
		return -1;
	}

	// 挑战次数判断;
	auto iter = m_mapActivityState.find(nActivityId);
	if (iter != m_mapActivityState.end())
	{
		if (iter->second.nValidCount <= 0)
		{
			return -2;
		}
	}

	return 0;
}

void ActivityModel::onBuySuccess()
{
	updateUI(OBS_PARAM_TYPE_ACTIVITY_BUY, (void*)(""));
}

void ActivityModel::updateSweepResult( map<int, int>* mapReward )
{
	updateUI(OBS_PARAM_TYPE_ACTIVITY_SWEEP, mapReward);
}

/*
string ActivityModel::getDateStr(ActivityInfo activityInfo)
{
	ostringstream strDate;
	// 若星期数据全满，则显示日期数据;
	if (activityInfo.btOpenDay.count() == activityInfo.btOpenDay.size())
	{
		strDate << (activityInfo.nStartDate % 10000)/100 << "月" << activityInfo.nStartDate % 100 << "日";
		strDate << "～";
		strDate << (activityInfo.nEndDate % 10000)/100 << "月" << activityInfo.nEndDate % 100 << "日";
	}
	// 否则，显示星期数据;
	else
	{
		int nCount = 0;
		strDate << "每周";
		for (unsigned int i = 0; i < activityInfo.btOpenDay.size(); i++)
		{
			if (0 == activityInfo.btOpenDay[i])
				continue;

			nCount++;

			switch (i)
			{
			case 0:	strDate << "一";	break;
			case 1:	strDate << "二";	break;
			case 2:	strDate << "三";	break;
			case 3:	strDate << "四";	break;
			case 4:	strDate << "五";	break;
			case 5:	strDate << "六";	break;
			case 6:	strDate << "日";	break;
			default:
				break;
			}

			if (nCount != activityInfo.btOpenDay.count())
			{
				strDate << ",";
			}
		}
	}
	return strDate.str();
}
*/

/*
int ActivityModel::updateActivityState( int nWeekday )
{
	// 现在是周几;
	int nTodayNow = getWeekday();

	// 判断上次刷新至现在是否已经相隔一天;
	if (nTodayNow == m_nToday)
	{
		// 仍是当日;
		// 再判断是否需要显示的是不是当天的活动;
		if (nWeekday == m_nToday)
		{
			// 是今天直接更新;
			updateActivityState(m_mapActivityState, true);
		}
		else
		{
			// 筛选出所选那天的活动，构造状态;
			for (auto it : m_mapActivityBarrierInfo)
			{
				if (it.second.btOpenDay[7-nWeekday] == 1)
				{
					ActivityState  state;
					state.nActivityId = it.first;
					state.nBuyCount = 0;
					state.nValidCount = 3;
					state.nTotalCount = 3;

					// 更新至UI;
					updateUI(OBS_PARAM_TYPE_ACTIVITY_NODE, (void*)&(state));
				}
			}
		}
	}
	else
	{
		// 已经隔天，再次请求数据;
		m_nToday = nTodayNow;
		return 1;
	}
	return 0;
}
*/
