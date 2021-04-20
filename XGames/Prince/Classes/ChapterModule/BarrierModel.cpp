#include "BarrierModel.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "MainModule/MainModel.h"
#include "VIPModule/VipModel.h"

BarrierModel* BarrierModel::mInstance = nullptr;

BarrierModel::BarrierModel(void)
{
	m_mapBarrierStoryList.clear();
	m_bIsLocalDataReady = false;
	m_nSweepTime = 0.0f;
}


BarrierModel::~BarrierModel(void)
{
	m_mapBarrierStoryList.clear();
}

BarrierModel* BarrierModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new BarrierModel();
	}
	return mInstance;
}

void BarrierModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void BarrierModel::updateBarrierStoryInfo(BarrierInfo info)
{
	int nChapterId = ChapterModel::getInstance()->constructChapterId(info.nBarrierId);
	auto iter = m_mapBarrierStoryList.find(nChapterId);
	if (iter != m_mapBarrierStoryList.end())
	{
		iter->second.insert(pair<int, BarrierInfo>(info.nBarrierId, info));
	}
	else
	{
		map<int, BarrierInfo>  mapBarrier;
		mapBarrier.insert(pair<int, BarrierInfo>(info.nBarrierId, info));
		m_mapBarrierStoryList.insert(pair<int, map<int, BarrierInfo> >(nChapterId, mapBarrier));
	}
}


void BarrierModel::clearBarrierStoryInfo()
{
	m_mapBarrierStoryList.clear();
}


void BarrierModel::updateBarrierStoryShow(int nBarrierId, FR_Barrier* fightRecord)
{
	//if (nullptr == fightRecord)
	//{
	//	return;
	//}

	// 定位章节;
	int nChapterId = ChapterModel::getInstance()->constructChapterId(nBarrierId);
	auto it = m_mapBarrierStoryList.find(nChapterId);
	if (it == m_mapBarrierStoryList.end())
		return;

	// 定位关卡;
	UI_BARRIER_INFO_PARAM  param;
	auto iter = it->second.find(nBarrierId);
	if (iter != it->second.end())
	{
		param.bIsAllowed = (fightRecord == nullptr) ? false : true;
		param.nBarrierId = iter->second.nBarrierId;
		param.nPower = MainModel::getInstance()->getMainParam()->mPower;
		param.nConsume = iter->second.nCost;
		param.nCoin = iter->second.nCoin;
		param.nExp = iter->second.nExp;
		param.nDayCount = (fightRecord == nullptr) ? 0 : fightRecord->nDayCount;
		param.nTotalCount = iter->second.nTime;
		param.nType = iter->second.nType;
		ostringstream oss;
		int nChapterOrder = ChapterModel::getInstance()->getChapterOrderFromBarrierId(param.nBarrierId);
		int nBarrierOrder = ChapterModel::getInstance()->getBarrierOrder(param.nBarrierId);
		oss << nChapterOrder << "-" << nBarrierOrder << " " << iter->second.strBarrierName;
		param.strBarrierName.assign(oss.str());
		oss.str("");
		param.nStar = (fightRecord == nullptr) ? 0 : fightRecord->nStar;
		param.strDescription = iter->second.strDescription;
		param.vcMonsterShowRes = iter->second.vcMonsterShowRes;
		for (int i = 0 ; i < iter->second.vcRewardShowRes.size(); ++i)
		{
			vector<int>  vcAttri;
			vcAttri.push_back(iter->second.vcRewardShowRes.at(i));
			vcAttri.push_back(iter->second.vcRewardShowQua.at(i));
			param.mapRewardShow.insert(pair<int, vector<int> >(iter->second.vcRewardShow.at(i), vcAttri));
		}
		param.nBossShowResId = iter->second.nBossShowId;
	}

	// 更新至UI;
	updateUI(OBS_PARAM_TYPE_BARRIER_INFO, (void*)(&param));
}


BarrierInfo* BarrierModel::getBarrierStoryById(int nBarrierId)
{
	// 定位章节;
	int nChapterId = ChapterModel::getInstance()->constructChapterId(nBarrierId);
	auto it = m_mapBarrierStoryList.find(nChapterId);
	if (it == m_mapBarrierStoryList.end())
		return nullptr;

	auto iter = it->second.find(nBarrierId);
	if (iter != it->second.end())
	{
		return &(iter->second);
	}
	return nullptr;
}


void BarrierModel::updateUI(OBS_PARAM_TYPE_CM type, void* data)
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}


float BarrierModel::getSweepTime()
{
	// 策划要求写死;
	//m_nSweepTime = 0.5f;
	//return m_nSweepTime;

	// 读系统表，拿到单独配置的扫荡时间;
	if (m_nSweepTime > -0.000001f && m_nSweepTime < 0.000001f)
	{
		DataTable* dtSystem = DataManager::getInstance()->getDTToolSystem();
		RowData* data = dtSystem->searchDataById(10010);
		if (nullptr != data)
		{
			m_nSweepTime = data->getIntData("value");
		}
		else
		{
			// 以防万一，自己给个默认值;
			m_nSweepTime = 2.0f;
		}
	}

	return m_nSweepTime;
}

int BarrierModel::getBarrierResetCost( int nBarrierId )
{
	if (nBarrierId < 0)
	{
		return 0;
	}

	FR_ChapterStory record;
	ChapterModel::getInstance()->getFightRecordData(record);
	auto it = record.mapPrevBarrier.find(ChapterModel::getInstance()->constructChapterId(nBarrierId));
	assert(it != record.mapPrevBarrier.end());
	auto iter = it->second.find(nBarrierId);
	assert(iter != it->second.end());
	int nNextTime = iter->second.nResetDayCount + 1;
	RowData* _row = DataManager::getInstance()->searchToolSystemById(10071);
	assert(_row != nullptr);
	vector<int>  vcPrice;
	StringFormat::parseCsvStringVec(_row->getStringData("value"), vcPrice);
	assert(!vcPrice.empty());
	int nIndex = (nNextTime > vcPrice.size() ? vcPrice.size()-1 : nNextTime-1);
	return vcPrice.at(nIndex);
}

int BarrierModel::getCurVipLeftTimes( int nBarrierId )
{
	if (nBarrierId < 0)
	{
		return 0;
	}

	FR_ChapterStory record;
	ChapterModel::getInstance()->getFightRecordData(record);
	auto it = record.mapPrevBarrier.find(ChapterModel::getInstance()->constructChapterId(nBarrierId));
	assert(it != record.mapPrevBarrier.end());
	auto iter = it->second.find(nBarrierId);
	assert(iter != it->second.end());
	return ChapterModel::getInstance()->getCurMaxTimes() - iter->second.nResetDayCount;
}

int BarrierModel::getSweepPrice()
{
	RowData* row = DataManager::getInstance()->searchToolSystemById(10106);
	assert(row != nullptr);
	int nPrice = row->getIntData("value");
	return nPrice;
}
