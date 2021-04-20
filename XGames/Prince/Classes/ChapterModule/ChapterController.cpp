#include "ChapterController.h"
#include "Communication/Command.h"
#include "Communication/SocketCommunicator.h"
#include "HeroModule/HeroModel.h"
#include "ChapterModel.h"
#include "BarrierModel.h"
#include "Activity/ActivityModel.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "Common/Common.h"
#include "SceneChapter.h"
#include "../MainModule/MainModel.h"
#include "ToolModule/ToolModel.h"
#include "Utils/StringFormat.h"
#include "SDK_TalkingData/TalkingData.h"
#include "DoubleBonus/DoubleBonusModel.h"
#include "MysteryShop/MysteryShopModel.h"

ChapterController* ChapterController::m_pInstance = nullptr;

ChapterController::ChapterController(void)
{
	m_nDstChapterId = INVALID_CHAPTER_OR_BARRIER_ID;
	m_nDstBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;
	m_nCurSelChapterId = INVALID_CHAPTER_OR_BARRIER_ID;
	m_nCurSelBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;
	m_pChapterFrameView = nullptr;
	m_pChapterView = nullptr;
	m_pBarrierView = nullptr;
	m_nCurStarIndex = -1;
	m_nSweepType = 0;
	m_nSweepResultTimes = 0;
	SocketCommunicator::getInstance()->addListener(this);
}


ChapterController::~ChapterController(void)
{
	SocketCommunicator::getInstance()->removeListener(this);
}

ChapterController* ChapterController::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new ChapterController;
	}

	return m_pInstance;
}

void ChapterController::destroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void ChapterController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_FIGHT_RECORD:
		{
			procFightRecordData(szData);
		}
		break;

	case nMAIN_CMD_SWEEP_START:
		{
			procStartSweep(szData);
		}
		break;

	case nMAIN_CMD_SWEEP_SETTLEMENT:
		{
			procSweepSettlement(szData);
		}
		break;

	case nMAIN_CMD_RESET_BARRIER:
		procResetBarrier(szData);
		break;

	case nMAIN_CMD_STAR_REWARD:
		{
			procStarReward(szData);
		}
		break;

	default:
		break;
	}
}


void ChapterController::handlePveData()
{
	if (m_pChapterFrameView != nullptr)
	{
		m_pChapterFrameView->switchLayerChapter(CHAPTER_TYPE_STORY);
	}

	if (!ChapterModel::getInstance()->isInit())
	{
		// 1..先解析本地csv数据;
		initLocalData();

		// 2.. 然后再获取进度;
		handleFightRecordData();
	}
	//FR_ChapterStory fightRecord;
	//ChapterModel::getInstance()->getFightRecordData(fightRecord);
	//if (fightRecord.mapPrevBarrier.size() == 0)
	//{
	//}
	else
	{
		// 更新章节状态;
		ChapterModel::getInstance()->updateChapterStoryBase();
		ChapterModel::getInstance()->updateChapterStoryState();

		// 跳转指定章节或关卡;
		jump2ChapterBarrier();
	}
}

void ChapterController::jump2ChapterBarrier()
{
	////////// 无预设的情况下，定位到当前进行章节，相当于默认预设;
	// 07/29/2015  Phil @zcjoy
	assert(m_pChapterView != nullptr);
	if (m_nDstChapterId == INVALID_CHAPTER_OR_BARRIER_ID
		&& m_nDstBarrierId == INVALID_CHAPTER_OR_BARRIER_ID)
	{
		FR_ChapterStory fightRecord;
		ChapterModel::getInstance()->getFightRecordData(fightRecord);
		m_nDstChapterId = fightRecord.nCurChapterId;
	}

	////////// 如果有预设目的章节，进入;
	// 剧情章节和关卡;
	if (m_nDstChapterId != INVALID_CHAPTER_OR_BARRIER_ID
		&& m_nDstBarrierId != INVALID_CHAPTER_OR_BARRIER_ID)
	{
		m_pChapterView->onChapterSelected(m_nDstChapterId);
		m_nDstChapterId = INVALID_CHAPTER_OR_BARRIER_ID;
	}
	// 只定位到剧情章节，不进入关卡;
	else if (m_nDstChapterId != INVALID_CHAPTER_OR_BARRIER_ID
		&& m_nDstBarrierId == INVALID_CHAPTER_OR_BARRIER_ID)
	{
		m_pChapterView->switch2Chapter(ChapterModel::getInstance()->getChapterOrderFromChapterId(m_nDstChapterId), CHAPTER_NODE_POS_MIDDLE);
	}
	/*******************  活动章节移到主城了;
	// 活动章节;
	// 暂时把【无章节Id有关卡Id】的情况先默认是活动章节，有特殊需求再从SceneChapter参数入手修改，区分战斗类型;
	else if (nullptr != m_pChapterFrameView && m_nDstChapterId == INVALID_CHAPTER_OR_BARRIER_ID
		&& m_nDstBarrierId != INVALID_CHAPTER_OR_BARRIER_ID)
	{
		m_pChapterFrameView->switchLayerChapter(CHAPTER_TYPE_EVENT);
		m_nDstBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;
		handleEventData(getWeekday());
	}
	*/
}

void ChapterController::initLocalData()
{
	if (!BarrierModel::getInstance()->isLocalDataReady())
	{
		procChapterData();
		procBarrierData();
		BarrierModel::getInstance()->setLocalDataReady();
	}
}


void ChapterController::procChapterData()
{
	DataTable* dtChapter = DataManager::getInstance()->getDTChapter();

	map<int, ChapterInfo>  mapInfo;
	mapInfo.clear();

	for (auto iter = dtChapter->getTableData().begin(); iter != dtChapter->getTableData().end(); iter++)
	{
		// 解析本地表;
		ChapterInfo info;
		info.nChapterId = iter->second.getIntData("id");
		info.strChapterName = STR_ANSI(iter->second.getStringData("name"));

		int nNum = 0;
		map<int, int> mapStarRewardSingle;
		mapStarRewardSingle.clear();

		for (int i = 0; i < MAX_STAR_REWARD_TYPE; i++)
		{
			ostringstream strNum, strReward;
			strNum << "starNum" << i+1;
			strReward << "reward" << i+1;
			nNum = iter->second.getIntData(strNum.str());
			string reward = iter->second.getStringData(strReward.str());
			mapStarRewardSingle.clear();
			StringFormat::parseCsvStringMap(reward, mapStarRewardSingle);
			info.vcStarReward.push_back(pair<int, map<int, int> >(nNum, mapStarRewardSingle));
		}

		info.nHighRaid = iter->second.getIntData("high_raid");
		info.nMaze     = iter->second.getIntData("maze");
		info.nLevel    = iter->second.getIntData("level");
		info.strRes    = STR_ANSI(iter->second.getStringData("res"));
		info.strDescription = STR_ANSI(iter->second.getStringData("des"));
		info.nRollReward = iter->second.getIntData("roll_reward");
		string strRaids  = iter->second.getStringData("raids");
		StringFormat::parseCsvStringVec(strRaids, info.vcBarriers);

		mapInfo.insert(pair<int, ChapterInfo>(info.nChapterId, info));
	}

	// 更新数据;
	ChapterModel::getInstance()->updateChapterInfo(mapInfo);

	// 章节坐标数据;
	map<int, ChapterPos>  mapChapterPos;
	DataTable* dtChapterPos = DataManager::getInstance()->getDTChapterPos();
	for (auto it = dtChapterPos->getTableData().begin(); it != dtChapterPos->getTableData().end(); it++)
	{
		int nGroupId = it->second.getIntData("id");
		string strPos = it->second.getStringData("location");
		vector<vector<int> >  vcPos;
		StringFormat::parseCsvStringVec(strPos, vcPos);
		for (unsigned j = 0; j < vcPos.size(); ++j)
		{
			ChapterPos  posInfo;
			posInfo.nGroupId = nGroupId;
			posInfo.nChapterId = vcPos.at(j).at(0);
			posInfo.nTitleResId = vcPos.at(j).at(1);
			posInfo.fPosX = (float)(vcPos.at(j).at(2)) + (nGroupId - 1) * 2938.0f;
			posInfo.fPosY = (float)(vcPos.at(j).at(3));
			mapChapterPos.insert(pair<int, ChapterPos>(posInfo.nChapterId, posInfo));
		}
	}
	ChapterModel::getInstance()->updateChapterPosInfo(mapChapterPos);
}


void ChapterController::procBarrierData()
{
	DataTable* dtBarrier = DataManager::getInstance()->getDTBarrier();

	// 清掉当前关卡配置信息;
	BarrierModel::getInstance()->clearBarrierStoryInfo();
	ActivityModel::getInstance()->clearActivityBarrierInfo();

	for (auto iter = dtBarrier->getTableData().begin(); iter != dtBarrier->getTableData().end(); iter++)
	{
		BarrierInfo info;
		info.nBarrierId = iter->second.getIntData("id");
		info.strBarrierName = iter->second.getStringData("name");
		info.nType = (CHAPTER_TYPE)(iter->second.getIntData("type"));
		info.nLevel = (BARRIER_LEVEL)(iter->second.getIntData("level")-1);
		info.nEnterLevel = iter->second.getIntData("enterLv");
		info.nCost = iter->second.getIntData("cost");
		string strMonsterId = iter->second.getStringData("monsterId");
		StringFormat::parseCsvStringVec(strMonsterId, info.vcMonsterId);
		string strReward = iter->second.getStringData("reward");
		StringFormat::parseCsvStringVec(strReward, info.vcReward);
		info.nExp   = iter->second.getIntData("exp");
		info.nCoin  = iter->second.getIntData("coin");
		info.nTime  = iter->second.getIntData("time");
		info.nResId = iter->second.getIntData("resId");
		info.strDescription = iter->second.getStringData("des");
		string strRewardShow = iter->second.getStringData("rewardShow");
		StringFormat::parseCsvStringVec(strRewardShow, info.vcRewardShow);
		for (unsigned int i = 0; i < info.vcRewardShow.size(); i++)
		{
			RowData* data = DataManager::getInstance()->searchToolById(info.vcRewardShow.at(i));
			if (nullptr != data)
			{
				info.vcRewardShowRes.push_back(data->getIntData("resId"));
				info.vcRewardShowQua.push_back(data->getIntData("itemQua"));
			}
		}
		/*
		string strMonsterShow = iter->second.getStringData("monsterShow");
		StringFormat::parseCsvStringVec(strMonsterShow, info.vcMonsterShow);
		for (unsigned int k = 0; k < info.vcMonsterShow.size(); k++)
		{
			RowData* data = DataManager::getInstance()->searchToolById(info.vcMonsterShow.at(k));
			int nResId = ((nullptr != data) ? data->getIntData("resId") : -1);
			info.vcMonsterShowRes.push_back(nResId);
		}
		*/
		string strFirstReward = iter->second.getStringData("rewardFirst");
		if (strFirstReward.compare("-1") != 0)
		{
			StringFormat::parseCsvStringMap(strFirstReward, info.mapFirstReward);
		}
		info.nBossShowId = iter->second.getIntData("bossShow");
		info.nMaxRound = iter->second.getIntData("maxRound");
		info.nMaxTime = iter->second.getIntData("time");

		// 更新数据;
		if (info.nType == CHAPTER_TYPE_STORY)
		{
			BarrierModel::getInstance()->updateBarrierStoryInfo(info);
			continue;
		}

		// 更新活动关卡数据;
		if (info.nType == CHAPTER_TYPE_EVENT)
		{
			// 继续解析活动关卡的数据;
			info.nStartDate = iter->second.getIntData("startdate");
			info.nEndDate = iter->second.getIntData("enddate");
			bitset<7> bitDays(iter->second.getStringData("openday"));
			info.btOpenDay = bitDays;
			info.nOrder = iter->second.getIntData("orderNum");
			info.nTitleId = iter->second.getIntData("titleId");

			ActivityModel::getInstance()->updateActivityBarrierInfo(info);
		}
	}
}


void ChapterController::handleFightRecordData()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FIGHT_RECORD, "");
}


void ChapterController::procFightRecordData(const char* szData)
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		bool bResult = root["result"].asBool();
		if (bResult)
		{
			Json::Value data = root["data"];
			Json::Value fightRecord = data["fightRecord"];
			procChapterListData(fightRecord);

			// 跳转指定章节或关卡;
			if (m_pChapterView != nullptr)
			{
				jump2ChapterBarrier();
			}
		}
		else
		{
			CustomPop::show(STR_ANSI(root["msg"].asString()));;
		}
	}
}


void ChapterController::procChapterListData(const Json::Value& data)
{
	// 空值也要更新至UI，可能是新玩家;

	FR_ChapterStory fightRecord;

	// 遍历Json文件中的关卡节点;
	for (auto iter = data.begin(); iter != data.end(); iter++)
	{
		// 遍历到每一章节节点，获取章节Id;
		string strId = iter.key().asString();

		// 容错;
		int nTmpId = atoi(strId.c_str());
		if (nTmpId <= 0)
			continue;

		// 继续读取有效值;
		Json::Value chapter = data[strId];

		// 星级奖励次数;
		//fightRecord.mapStarRewardCount.insert(pair<int, int>(nTmpId, chapter["rewards"].asInt()));
		Json::Value rewardRecord = chapter["rewards"];
		map<int, bool>  mapIndex;
		mapIndex.insert(make_pair(1, false));
		mapIndex.insert(make_pair(2, false));
		mapIndex.insert(make_pair(3, false));
		if (!rewardRecord.empty())
		{
			for (int i = 0; i < rewardRecord.size(); ++i)
			{
				// 领了第几次的;
				int nIndex = rewardRecord[i].asInt();
				mapIndex.find(nIndex)->second = true;
			}
		}
		fightRecord.mapStarRewardCount.insert(make_pair(nTmpId, mapIndex));

		// 关卡记录;
		Json::Value gkRecord = chapter["gkrecord"];
		if (!gkRecord.empty())
		{
			// 记录当前最大值节点为当前进行章节，直至遍历结束;
			if (atoi(strId.c_str()) > fightRecord.nCurChapterId)
			{
				fightRecord.nCurChapterId = atoi(strId.c_str());
				fightRecord.nCurBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;
			}

			map<int, FR_Barrier> mapChapter;

			// 遍历章节节点内的所有关卡记录;
			for (int i = 0; i < gkRecord.size(); i++)
			{
				FR_Barrier record;
				procBarrierListData(gkRecord[i], record);
				mapChapter.insert(pair<int, FR_Barrier>(record.nBarrierId, record));

				// 记录当前节点为当前进行关卡，直至遍历结束;
				if (fightRecord.nCurChapterId == atoi(strId.c_str())
					&& (ChapterModel::getInstance()->getBarrierOrder(record.nBarrierId) > ChapterModel::getInstance()->getBarrierOrder(fightRecord.nCurBarrierId)))
				{
					fightRecord.nCurBarrierId = record.nBarrierId;
				}
			}

			// 添加记录;
			fightRecord.mapPrevBarrier.insert(pair<int, map<int, FR_Barrier> >(atoi(strId.c_str()), mapChapter));

		} // end if
	} // end for

	// 读取Json配置结束，更新进度数据;
	ChapterModel::getInstance()->updateFightRecord(fightRecord);
}


void ChapterController::procBarrierListData(const Json::Value& data, FR_Barrier& record)
{
	if (data.empty())
	{
		return;
	}

	record.nBarrierId = data["id"].asInt();
	record.nDayCount = data["daycount"].asInt();
	record.nResetDayCount = data["buycount"].asInt();
	record.nStar = data["star"].asInt();
}


bool ChapterController::handleChapterDetail()
{
	if (m_nCurSelChapterId == INVALID_CHAPTER_OR_BARRIER_ID)
	{
		return false;
	}

	// 更新章节关卡公共信息;
	ChapterModel::getInstance()->updateChapterDetail(m_nCurSelChapterId);
	BARRIER_LEVEL nCurLv = BARRIER_LEVEL_0;

	FR_ChapterStory record;
	ChapterModel::getInstance()->getFightRecordData(record);
	// 首先判断有无预设关卡，不要重复执行选关卡的操作;
	if (m_nDstBarrierId == INVALID_CHAPTER_OR_BARRIER_ID)
	{
		// 如果是当前进行的章节，默认选中当前进行的关卡;
		if (m_nCurSelChapterId == record.nCurChapterId)
		{
			// 当前进行的一定是LEVEL_0，否则当前是下一章;
		}
		// 如果是历史章节;
		else
		{
			// 判断本章内所有关卡是否打完;
			auto it = record.mapPrevBarrier.find(m_nCurSelChapterId);
			assert(it != record.mapPrevBarrier.end());
			int nCount1 = it->second.size();
			auto it1 = it->second.rbegin();
			assert(it1 != it->second.rend());

			// 全部难度都打完;
			if (nCount1 >= ChapterModel::getInstance()->getBarrierCount(m_nCurSelChapterId)
				&& it1->second.nStar > 0)
			{
				// 默认选中第一关的难度;
				int nFirstBarrierId = ChapterModel::getInstance()->getFirstBarrierInChapter(m_nCurSelChapterId);
				nCurLv = ChapterModel::getInstance()->getBarrierLv(nFirstBarrierId);
			}
			// 未全部打完;
			else
			{
				// 选中本章当前进行到的关卡的难度;
				nCurLv = ChapterModel::getInstance()->getBarrierLv(it1->second.nBarrierId);
			}
		}
	}
	// 若有预设关卡，选中所在难度;
	else if (m_nDstBarrierId != INVALID_CHAPTER_OR_BARRIER_ID)
	{
		nCurLv = ChapterModel::getInstance()->getBarrierLv(m_nDstBarrierId);
	}

	// 选中当前难度，进而更新对应的关卡列表;
	ChapterModel::getInstance()->selectBarrierLevel(nCurLv);

	return true;
}


void ChapterController::setDstChapter(int nChapterId)
{
	m_nDstChapterId = nChapterId;
}


void ChapterController::setDstBarrier(int nBarrierId)
{
	m_nDstBarrierId = nBarrierId;
}


void ChapterController::onBarrierSelected(int nBarrierId)
{
	// 校验是否是本章关卡; 1000001
	if (ChapterModel::getInstance()->constructChapterId(nBarrierId) != m_nCurSelChapterId)
	{
		return;
	}

	// 保存选中的关卡Id;
	m_nCurSelBarrierId = nBarrierId;

	// 获取关卡战斗记录;
	FR_ChapterStory  fightRecord;
	ChapterModel::getInstance()->getFightRecordData(fightRecord);

	// 更新细节;
	bool bFound = false;
	auto iter = fightRecord.mapPrevBarrier.find(m_nCurSelChapterId);
	if (iter != fightRecord.mapPrevBarrier.end())
	{
		auto it = iter->second.begin();
		for (; it != iter->second.end(); ++it)
		{
			if (m_nCurSelBarrierId == it->second.nBarrierId)
			{
				bFound = true;
				BarrierModel::getInstance()->updateBarrierStoryShow(m_nCurSelBarrierId, &(it->second));
				break;
			}
		}
	}

	// 未开启的关卡;
	if (!bFound)
	{
		BarrierModel::getInstance()->updateBarrierStoryShow(m_nCurSelBarrierId, nullptr);
	}

}

bool ChapterController::onBarrierLevelSelected(BARRIER_LEVEL nLv)
{
	// 判断当前级别是否可选(之前的难度是否已通过);
	FR_ChapterStory record;
	ChapterModel::getInstance()->getFightRecordData(record);
	auto iter = record.mapPrevBarrier.find(m_nCurSelChapterId);
	assert(iter != record.mapPrevBarrier.end());
	int nCount1 = iter->second.size();
	auto it = iter->second.rbegin();
	assert(it != iter->second.rend());
	BARRIER_LEVEL nCurMaxLv = ChapterModel::getInstance()->getBarrierLv(it->second.nBarrierId);
	if (nCurMaxLv < nLv)
	{
		return false;
	}

	// 初始参数;
	UI_BARRIER_LIST_PARAM  uiParam;
	uiParam.nChapterId = m_nCurSelChapterId;
	uiParam.nLv = nLv;
	uiParam.nMaxLv = nCurMaxLv;

	if (nLv == nCurMaxLv)
	{
		ChapterModel::getInstance()->resetChapterNewLv(m_nCurSelChapterId);
	}

	// 以下是为了实现关卡定位;
	if (m_nDstBarrierId == INVALID_CHAPTER_OR_BARRIER_ID)
	{
		// 如果是当前进行的章节，默认选中当前进行的关卡;
		if (m_nCurSelChapterId == record.nCurChapterId)
		{
			// ;
			uiParam.nCurBarrierId = record.nCurBarrierId;
			uiParam.nSelectedBarrierId = record.nCurBarrierId;
		}
		// 如果是历史章节;
		else
		{
			// 本章内全部难度都打完，默认选中第一关;
			if (nCount1 >= ChapterModel::getInstance()->getBarrierCount(m_nCurSelChapterId)
				&& it->second.nStar > 0)
			{
				int nFirstBarrierId = ChapterModel::getInstance()->getFirstBarrierInChapter(m_nCurSelChapterId);
				uiParam.nCurBarrierId = nFirstBarrierId;
				uiParam.nSelectedBarrierId = nFirstBarrierId;
			}
			// 未全部打完，选中当前;
			else
			{
				uiParam.nCurBarrierId = it->second.nBarrierId;
				uiParam.nSelectedBarrierId = it->second.nBarrierId;
			}
		}
	}
	// 若有预设关卡，选中;
	else if (m_nDstBarrierId != INVALID_CHAPTER_OR_BARRIER_ID)
	{
		// 选中的仍然是预设关卡;
		uiParam.nSelectedBarrierId = m_nDstBarrierId;
		uiParam.nCurBarrierId = m_nDstBarrierId;

		// 只有从战斗中跳转，才修正新开启关卡的位置;
		if (ChapterModel::getInstance()->getJumpType() == CH_JUMP_TYPE_FIGHT)
		{
			// 修正新开启关卡;
			if (/*m_nCurSelChapterId == record.nCurChapterId && */ChapterModel::getInstance()->isBarrierPassed(m_nDstBarrierId))
			{
				// 下一关;
				ChapterModel::getInstance()->getNextBarrier(m_nDstBarrierId, uiParam.nCurBarrierId);

				// 如果下一关未通过，则选中下一关(新开启)，如果下一关已通过，则仍选中当前打的那一关;
				// 并且在同一章节;
				if (!ChapterModel::getInstance()->isBarrierPassed(uiParam.nCurBarrierId)
					&& ChapterModel::getInstance()->constructChapterId(m_nDstBarrierId) == ChapterModel::getInstance()->constructChapterId(uiParam.nCurBarrierId))
				{
					uiParam.nSelectedBarrierId = uiParam.nCurBarrierId;

					// 这个显示新开启的逻辑只有在打完关卡回来时才有，就是说只有在有预设关卡时才会有;
					uiParam.nNewBarrier = uiParam.nCurBarrierId;
				}
				else
				{
					uiParam.nCurBarrierId = m_nDstBarrierId;
				}
			}
		}
		m_nDstBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;
	}

	// 更新关卡列表;
	ChapterModel::getInstance()->updateBarrierList(uiParam);
	return true;
}


int ChapterController::handleFight(string& strReason)
{
	int nRet = INVALID_CHAPTER_OR_BARRIER_ID;
	if (ChapterModel::getInstance()->isBarrierAllowed(m_nCurSelChapterId, m_nCurSelBarrierId, 1, strReason))
	{
		nRet = m_nCurSelBarrierId;
	}
	return nRet;
}


int ChapterController::handlePreStartSweep( int nCount, string& strReason, int& nSweepFinalCount )
{
	// 取最低次数(不包含扫荡券数量判断);
	int nFinalCount = getMaxSweepCount(nCount, strReason);
	if (nFinalCount <= 0)
	{
		return nFinalCount;
	}

	// 扫荡券数量判断;
	int nCount3 = ToolModel::getInstance()->searchNumByTemplateId(10010);
	if (nCount3 < nFinalCount)
	{
		nSweepFinalCount = nFinalCount;
		return -2;
	}

	// 体力;
	if (MainModel::getInstance()->getMainParam()->mPower < BarrierModel::getInstance()->getBarrierStoryById(m_nCurSelBarrierId)->nCost * nFinalCount)
	{
		return -3;
	}

	nSweepFinalCount = nFinalCount;
	return nFinalCount;
}

int ChapterController::getMaxSweepCount( int OriCount, string& strReason )
{
	multimap<int, int>  mapTmp;

	// 1.期望次数 OriCount;
	int nCount1 = OriCount;
	mapTmp.insert(make_pair(nCount1, nCount1));

	// 2.体力允许次数;
	int nCount2 = MainModel::getInstance()->getMainParam()->mPower / BarrierModel::getInstance()->getBarrierStoryById(m_nCurSelBarrierId)->nCost;
	mapTmp.insert(make_pair(nCount2, nCount2));
	if (nCount2 <= 0)
	{
		strReason.assign(DataManager::getInstance()->searchCommonTexdtById(STR_BARRIER_FORBIDDEN_STRENGTH));
		return -3;
	}

	// 3.扫荡券剩余次数;
	/*
	int nCount3 = ToolModel::getInstance()->searchNumByTemplateId(10010);
	mapTmp.insert(make_pair(nCount3, nCount3));
	if (nCount3 <= 0)
	{
		strReason.assign(STR_BARRIER_FORBIDDEN_SWEEP_TICKET);
		return 0;
	}
	*/

	// 4.关卡剩余次数;
	int nCount4 = ChapterModel::getInstance()->getBarrierLeftTimes(m_nCurSelBarrierId);
	if (nCount4 <= 0)
	{
		strReason.assign(DataManager::getInstance()->searchCommonTexdtById(STR_BARRIER_FORBIDDEN_DAY_COUNT));
		return -4;
	}
	mapTmp.insert(make_pair(nCount4, nCount4));

	// 取最小值;
	return mapTmp.begin()->second;
}


bool ChapterController::handleStartSweep(int nCount, string& strReason, int nFlag /*= 0*/)
{
	if (ChapterModel::getInstance()->isBarrierAllowed(m_nCurSelChapterId, m_nCurSelBarrierId, nCount, strReason))
	{
		// 准备扫荡数据;
		m_sweepState.nChapterId = m_nCurSelChapterId;
		m_sweepState.nBarrierId = m_nCurSelBarrierId;
		m_sweepState.nLv = ChapterModel::getInstance()->getBarrierLv(m_nCurSelBarrierId);
		m_sweepState.nTotalCount = nCount;
		m_sweepState.nCycStrength = BarrierModel::getInstance()->getBarrierStoryById(m_nCurSelBarrierId)->nCost;
		m_sweepState.nCycTime = BarrierModel::getInstance()->getSweepTime();
		m_sweepState.nLeftTime = m_sweepState.nCycTime * m_sweepState.nTotalCount;
		m_sweepState.nFinishTimes = 0;

		// 发起扫荡请求;
		Json::FastWriter writer;
		Json::Value root;
		root["guankaId"] = m_nCurSelBarrierId;
		root["times"] = nCount;
		m_nSweepType = nFlag;
		root["flag"] = m_nSweepType;
		string json_file = writer.write(root);
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_SWEEP_START, json_file.c_str());
		
		return true;
	}

	return false;
}


void ChapterController::procStartSweep(const char* szData)
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 处理协议，根据返回值来决定是否可以开始扫荡;
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		// 更新扫荡状态;
		m_sweepState.bSweeping = true;
		ChapterModel::getInstance()->updateSweepState(m_sweepState);

		// 统计付费扫荡;
		if (m_nSweepType == 1)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10054), 
				m_sweepState.nTotalCount, BarrierModel::getInstance()->getSweepPrice());
		}

		// 直接拿到扫荡结果;
		procSweepSettlement(szData);

		// 开始扫荡;
		//float fInterval = BarrierModel::getInstance()->getSweepTime();
		//Director::getInstance()->getScheduler()->schedule(
		//	schedule_selector(ChapterController::sweepCountDown), this, fInterval, kRepeatForever, 0.0f, false);
	}
}


void ChapterController::sweepCountDown(float delta)
{
	// 每秒更新一次扫荡状态;
	m_sweepState.nLeftTime -= m_sweepState.nCycTime;
	ChapterModel::getInstance()->updateSweepState(m_sweepState);

	// 周期;
	//if (m_sweepState.nLeftTime % m_sweepState.nCycTime == 0)
	//{
		// 单次结算;
		//log("===== sweep start settlement, send msg =====");
		handleSweepSettlement(m_sweepState.nFinishTimes+1);
	//}

	// 停止条件;
	if (m_sweepState.nLeftTime < 0.000001f || m_sweepState.bSweeping == false)
	{
		//log("===== sweep start settlement, stop timer =====");
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(ChapterController::sweepCountDown), this);
	}
	
}


void ChapterController::handleSweepSettlement(int nIndex)
{
	// 发起结算;
	Json::FastWriter writer;
	Json::Value root;
	root["times"] = nIndex;
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_SWEEP_SETTLEMENT, json_file.c_str());
}


void ChapterController::procSweepSettlement(const char* szData)
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 处理协议;
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		//log("===== sweep settlement, rcv msg =====");
		Json::Value data = root["data"];
		Json::Value dropItems = data["cleanUpItemList"];

		vector<SweepSettlement>  vcSettlement;
		int nTimes = data["times"].asInt();

		// 扫荡结果;
		for (unsigned int i = 0; i < nTimes; i++)
		{
			SweepSettlement settlement;
			Json::Value single = dropItems[i];

			// 关卡Id;
			settlement.nBarrierId = m_nCurSelBarrierId;

			// 序号;
			settlement.nOrder = i+1;

			// 掉落;
			Json::Value singleItemList = single["itemlist"];
			for (auto it = singleItemList.begin(); it != singleItemList.end(); it++)
			{
				settlement.vcDropItems.push_back(pair<int, int>(atoi(it.memberName()), singleItemList[it.memberName()].asInt()));
			}

			// 货币;
			settlement.nCoin = single["coin"].asInt();

			// 经验;
			settlement.nExp = single["exp"].asInt();

			vcSettlement.push_back(settlement);
		}

		//神秘商店状态
		auto timeM = data["guankaStoreTime"].asInt();
		MysteryShopModel::getInstance()->setStamp(timeM);
		if (timeM >LocalTimer::getInstance()->getCurServerTime())//大于服务器时间
		{
			MysteryShopModel::getInstance()->setIsMainShow(true);//主城显示
			MysteryShopModel::getInstance()->setIsInterShow(true); //关卡场景显示
		}

		// 更新角色信息;
		HeroModel::getInstance()->updateData(data["petList"]);

		// 更新体力/能量/货币;
		MainModel::getInstance()->updateData(data);

		// 更新本地关卡页面的信息(主要是货币和体力，以及挑战次数);
		ChapterModel::getInstance()->updateChapterDetail(m_nCurSelChapterId);
		ChapterModel::getInstance()->updateFightRecordFromBarrier(m_nCurSelBarrierId, 3, nTimes - m_sweepState.nFinishTimes);
		m_sweepState.nFinishTimes = nTimes;

		// 更新背包大小;
		int nPkgUseSize = data["pkgUsesize"].asInt();
		int nPkgSize = data["pkgsize"].asInt();
		ToolModel::getInstance()->updateBackpackUsedCount(nPkgUseSize, nPkgSize);

		// 更新道具;
		ToolModel::getInstance()->updateToolJsonData(data["itemList"]);

		// 更新扫荡结果(要在更新道具之后，因为扫荡券数量可能有变);
		//ChapterModel::getInstance()->updateSweepSettlement(vcSettlement);

		m_nSweepResultTimes = 1;
		float fInterval = BarrierModel::getInstance()->getSweepTime();
		Director::getInstance()->getScheduler()->schedule([=](float dt)->void {

			vector<SweepSettlement>  vcSingle;
			for (int k = 0; k < m_nSweepResultTimes && k < vcSettlement.size(); ++k)
				vcSingle.push_back(vcSettlement.at(k));

			ChapterModel::getInstance()->updateSweepSettlement(vcSingle);
			m_nSweepResultTimes++;
			log("+++++++++++++++++ sweep %d, size = %d", m_nSweepResultTimes, vcSingle.size());

			if (m_nSweepResultTimes > vcSettlement.size())
			{
				m_sweepState.bSweeping = false;
				m_sweepState.nFinishTimes = 0;
				ChapterModel::getInstance()->updateSweepState(m_sweepState);
			}
		},
			this, fInterval, max(0, nTimes-1), 0.001f, false, "ChapterController::procSweepSettlement");

		// 结算奖励数量等于总数量，停止扫荡;
		//if (m_sweepState.nTotalCount == nRewardTimes)
		//{
		//	//log("===== sweep settlement, stop sweep =====");
		//	m_sweepState.bSweeping = false;
		//	m_sweepState.nFinishTimes = 0;
		//	Director::getInstance()->getScheduler()->unschedule(schedule_selector(ChapterController::sweepCountDown), this);
		//}
		//else
		//{
		//	//log("***** continue sweep: curTimes = %d *****", nRewardTimes);
		//	int fuck = 0;
		//}
		//ChapterModel::getInstance()->updateSweepState(m_sweepState);
	}
}

void ChapterController::setBarrierView(LayerBarrier* pView)
{
	m_pBarrierView = pView;
	if (pView == nullptr)
	{
		// 重置选中的关卡;
		m_nCurSelBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;
	}
}

void ChapterController::handleStarReward( int nStarIndex )
{
	m_nCurStarIndex = nStarIndex;

	Json::FastWriter writer;
	Json::Value root;
	root["zhangjieId"] = m_nCurSelChapterId;
	root["starIndex"] = nStarIndex;
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_STAR_REWARD, json_file.c_str());
}

void ChapterController::procStarReward( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		int nChapterId = data["zhangjieId"].asInt();
		Json::Value _itemList = data["changeInfo"]["itemList"];
		Json::Value _petList = data["changeInfo"]["petList"];

		// 货币更新;
		MainModel::getInstance()->setGold(data["changeInfo"]["gold"].asInt());
		MainModel::getInstance()->setSilver(data["changeInfo"]["coin"].asInt());
		MainModel::getInstance()->notifyObservers();

		// 更新背包;
		ToolModel::getInstance()->updateToolJsonData(_itemList);

		// 更新Hero;
		HeroModel::getInstance()->updateData(_petList);

		// 容错，防止未返回结果时跳转到其他章节;
		if (m_nCurSelChapterId == nChapterId && m_nCurStarIndex != -1)
		{
			ChapterModel::getInstance()->updateGetStarRewardResult(nChapterId, m_nCurStarIndex);
			ChapterModel::getInstance()->updateStarReward(nChapterId);
			ChapterModel::getInstance()->updateChapterStoryState();
			m_nCurStarIndex = -1;
		}
	}
}

void ChapterController::handleResetBarrier()
{
	/*
	debuger(蒙雄发) 2015-09-30 16:14:31
	14008
	参数：guankaId
	返回 gold,guankaId
	debuger(蒙雄发) 2015-09-30 17:49:43
	buycount
	*/

	// 发起重置;
	Json::FastWriter writer;
	Json::Value root;
	root["guankaId"] = m_nCurSelBarrierId;
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_RESET_BARRIER, json_file.c_str());
}

int ChapterController::handlePreResetBarrier()
{
	// 判断剩余金币;
	return MainModel::getInstance()->getMainParam()->mGold - BarrierModel::getInstance()->getBarrierResetCost(m_nCurSelBarrierId);
}

void ChapterController::procResetBarrier( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		// 更新剩余金币;
		int nGold = data["gold"].asInt();
		assert(nGold < MainModel::getInstance()->getMainParam()->mGold);
		MainModel::getInstance()->setGold(nGold);
		MainModel::getInstance()->notifyObservers();

		// 更新重置次数;
		int nBarrierId = data["guankaId"].asInt();
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10055), 
			1, BarrierModel::getInstance()->getBarrierResetCost(nBarrierId));
		ChapterModel::getInstance()->updateBarrierResetCount(nBarrierId);
	}
}
