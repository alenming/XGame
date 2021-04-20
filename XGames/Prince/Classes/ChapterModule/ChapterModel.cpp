#include "ChapterModel.h"
#include "MainModule/MainModel.h"
#include "BarrierModel.h"
#include "HeroModule/HeroModel.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "VIPModule/VipModel.h"
#include "CommonBuy/LayerCommonBuy.h"
#include "DoubleBonus/DoubleBonusModel.h"

ChapterModel* ChapterModel::mInstance = nullptr;

ChapterModel::ChapterModel(void)
	: m_bIsInit(false)
	, m_jumpType(CH_JUMP_TYPE_FIGHT)
{
	m_mapChapterInfoList.clear();
	m_mapChapterPos.clear();
	m_vcSweepSettlement.clear();
	m_mapStarRewardRecord.clear();
}


ChapterModel::~ChapterModel(void)
{
	m_mapChapterInfoList.clear();
	m_vcSweepSettlement.clear();
}

ChapterModel* ChapterModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new ChapterModel();
	}
	return mInstance;
}

void ChapterModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void ChapterModel::updateFightRecord(FR_ChapterStory fightRecord)
{
	// 记录进度;
	m_sFightRecord.nCurChapterId  = fightRecord.nCurChapterId;
	m_sFightRecord.nCurBarrierId  = fightRecord.nCurBarrierId;
	m_sFightRecord.mapPrevBarrier = fightRecord.mapPrevBarrier;
	m_sFightRecord.mapStarRewardCount = fightRecord.mapStarRewardCount;

	// 新玩家无记录，默认第一章节第一关卡，构造数据;
	if (0 == m_sFightRecord.mapPrevBarrier.size())
	{
		openBarrier(DEFAULT_BARRIER_ID);
	}

	/* 20150305 新需求，关卡开启的规则修改;
	// 遍历所有战斗记录，打开每一层结构的下一个关卡;
	for (auto iter = fightRecord.mapPrevBarrier.begin(); iter != fightRecord.mapPrevBarrier.end(); iter++)
	{
		// [重要]修正战斗记录(1.所有关卡的下一个难度 2.当前关卡位置);
		// 遍历过程中，size的值可能会变化(新增)，此处使用原始size遍历，才不会有问题;
		// 因为使用原始size，不仅保证遍历过程稳定，也避免了新增的关卡不会再做重复判断;
		unsigned int nCountBarrier = (unsigned int)fightRecord.mapPrevBarrier.find(iter->first)->second.size();
		for (unsigned int k = 0; k < nCountBarrier; k++ )
		{
			// 先开启关卡的下一个难度(若有的话);
			// openBarrier函数本身会先检查自身，然后判断下一个难度;
			FR_Barrier frBarrier = iter->second.at(k);
			openBarrier(iter->first, frBarrier.nBarrierId);

			// 战斗记录中的当前关卡有星级(至少一颗星)，则当前关卡移至下一关卡;
			// 此处必须用原始战斗记录中的fightRecord.nCurBarrierId来做判断，因为在开启当前关卡下一难度的时候，当前关卡的Id可能会发生变化;
			// 此处主要是保证当前关卡处如果是章节内最后一关时，切换到下一个章节;
			if (frBarrier.nBarrierId == fightRecord.nCurBarrierId)
			{
				// 然后开启下一关卡的简单难度;
				if (frBarrier.nStar != 0)
				{
					int nTmpChapter = 0;
					int nTmpBarrier = 0;
					if (getNextBarrier(fightRecord.nCurChapterId, fightRecord.nCurBarrierId, nTmpChapter, nTmpBarrier))
					{
						openBarrier(nTmpChapter, nTmpBarrier);
					}
				}

				// 因为战斗记录中的数据可能是逻辑无序的，比如10002排在10101前面;
				// 当前关卡不一定是最后一个数据，所以继续遍历而不是break跳出;
				continue;
			}
		}
	}
	*/

	// 遍历所有战斗记录，打开每一层结构的下一个关卡;
	for (auto iter = fightRecord.mapPrevBarrier.begin(); iter != fightRecord.mapPrevBarrier.end(); iter++)
	{
		// 新规则：普通->精英;
		// 获取当前最大关卡的下一关即可;
		int nCurBigOrderBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;
		int nNextBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;
		bool bPassed = false;
		//auto it = m_sFightRecord.mapPrevBarrier.find(iter->first);
		//if (it != m_sFightRecord.mapPrevBarrier.end())
		//{
			auto it = iter->second.rbegin();
			if (it != iter->second.rend())
			{
				nCurBigOrderBarrierId = it->first;
				bPassed = (it->second.nStar > 0) ? true : false;
			}
		//}

		// 过关了才有开启下一关/章的可能性;
		if (bPassed)
		{
			// 开启下一关;
			if (getNextBarrier(nCurBigOrderBarrierId, nNextBarrierId))
			{
				openBarrier(nNextBarrierId);
			}

			// 是否开启下一章节(用原始值判断);
			if ((getBarrierLv(nCurBigOrderBarrierId) == BARRIER_LEVEL_0 && getBarrierLv(nNextBarrierId) == BARRIER_LEVEL_1)
				|| getBarrierLv(nCurBigOrderBarrierId) > BARRIER_LEVEL_0)
			{
				int nNextChapterId = constructChapterId(nCurBigOrderBarrierId) + 1;
				if (nNextChapterId > m_sFightRecord.nCurChapterId)
				{
					auto iter = m_mapChapterInfoList.find(nNextChapterId);
					if (iter != m_mapChapterInfoList.end())
					{
						openBarrier(getFirstBarrierInChapter(nNextChapterId));
					}
					else
					{
						// 通关全部章节;
						m_sFightRecord.bIsGameOver = true;
					}
				}
			}
		}
	}

	// 此处做一个时间差，在主城拉取战斗记录时，m_bIsInit还未标记为true;
	// 正好避开更新UI;
	if (m_bIsInit)
	{
		// 更新至UI;
		updateChapterStoryBase();
		updateChapterStoryState();
	}

	m_bIsInit = true;
}


void ChapterModel::updateFightRecordFromBarrier( int nBarrierId, int nStarCount, int nTimes /*= 1*/ )
{
	// 匹配当前的进度，并更新;
	int nChapterId = constructChapterId(nBarrierId);
	auto iter = m_sFightRecord.mapPrevBarrier.find(nChapterId);

	// 定位当前章节/关卡;
	bool bNextBarrier = false;
	if (iter != m_sFightRecord.mapPrevBarrier.end())
	{
		// 查找当前关卡;
		auto it = iter->second.find(nBarrierId);
		if (it != iter->second.end())
		{
			it->second.nDayCount += (nStarCount > 0) ? nTimes : 0;
			it->second.nStar = (it->second.nStar >= nStarCount) ? it->second.nStar : nStarCount;
			bNextBarrier = (nStarCount > 0);
		}
	}
	else
	{
		return;
	}

	// 开启下一关卡;
	if (bNextBarrier)
	{
		bool bIsFirstTime = false;
		int nNextBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;
		if (getNextBarrier(nBarrierId, nNextBarrierId))
		{
			bIsFirstTime = openBarrier(nNextBarrierId);
		}

		// 是否开启下一章节;
		if (getBarrierLv(nBarrierId) == BARRIER_LEVEL_0
			&& getBarrierLv(nNextBarrierId) == BARRIER_LEVEL_1)
		{
			int nNextChapterId = constructChapterId(nBarrierId) + 1;
			if (nNextChapterId > m_sFightRecord.nCurChapterId)
			{
				auto iter = m_mapChapterInfoList.find(nNextChapterId);
				if (iter != m_mapChapterInfoList.end())
				{
					bIsFirstTime = openBarrier(getFirstBarrierInChapter(nNextChapterId));
				}
				else
				{
					// 通关全部章节;
					m_sFightRecord.bIsGameOver = true;
				}
			}
		}

		// 下一关第一次被开启;
		if (bIsFirstTime && (getBarrierLv(nNextBarrierId) > getBarrierLv(nBarrierId)))
		{
			auto itLv = m_mapChapternewLv.find(constructChapterId(nNextBarrierId));
			if (itLv == m_mapChapternewLv.end())
			{
				m_mapChapternewLv.insert(pair<int, bool>(constructChapterId(nNextBarrierId), true));
			}
			else
			{
				// 可能是下一个难度，所以已经存在;
				// 可能打开后被覆盖，所以要重新置为true;
				itLv->second = true;
			}
		}
	}

}


bool ChapterModel::isBarrierAllowed(int nChapterId, int nBarrierId, int nCount, string& strReason, bool bFuzzy)
{
	bool bRet = false;

	// 条件;
	// 1.查FightRecord中是否有记录;
	// 若有;
	//	a.等级是否满足;
	//	b.体力是否足够;
	//  c.当日挑战次数;
	// 若无返回false;
	auto iter = m_sFightRecord.mapPrevBarrier.find(nChapterId);
	if (iter != m_sFightRecord.mapPrevBarrier.end())
	{
		auto it = iter->second.find(nBarrierId);
		if (it != iter->second.end())
		{
			// 是否模糊查询;
			if (bFuzzy)
			{
				return true;
			}

			// 后续判断;
			bool  bCondition1 = false;		// 条件1: 满足等级限制;
			bool  bCondition2 = false;		// 条件2: 满足体力限制;
			bool  bCondition3 = false;		// 条件3: 当日挑战次数;

			// 条件1判断;
			int nHeroLv = HeroModel::getInstance()->getMainHeroLevel();
			int nEnterLv = BarrierModel::getInstance()->getBarrierStoryById(nBarrierId)->nEnterLevel;
			if (nHeroLv >= nEnterLv)
			{
				bCondition1 = true;
			}
			else
			{
				strReason.assign(DataManager::getInstance()->searchCommonTexdtById(10062));
				strReason.append(StringFormat::intToStr(nEnterLv));
				strReason.append(DataManager::getInstance()->searchCommonTexdtById(10001));
				return false;
			}

			// 条件2判断;
			int nHeroStrength = MainModel::getInstance()->getMainParam()->mPower;
			if (nHeroStrength >= BarrierModel::getInstance()->getBarrierStoryById(nBarrierId)->nCost * nCount)
			{
				bCondition2 = true;
			}
			else
			{
				LayerCommonBuy *buyPower = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
				Director::getInstance()->getRunningScene()->addChild(buyPower);
				return false;
			}

			// 条件3判断;
			//if (BarrierModel::getInstance()->getBarrierStoryById(nBarrierId)->nTime - it->second.nDayCount >= nCount)
			if (ChapterModel::getInstance()->getBarrierLeftTimes(nBarrierId) >= nCount)
			{
				bCondition3 = true;
			}
			else
			{
				strReason.assign(DataManager::getInstance()->searchCommonTexdtById(STR_BARRIER_FORBIDDEN_DAY_COUNT));
				return false;
			}

			if (bCondition1 && bCondition2 && bCondition3)
			{
				strReason.assign("");
				return true;
			}
		}
		else
		{
			strReason.assign(DataManager::getInstance()->searchCommonTexdtById(STR_BARRIER_FORBIDDEN_NOT_ARRIVED));
			return false;
		}
	}
	else
	{
		strReason.assign(DataManager::getInstance()->searchCommonTexdtById(STR_BARRIER_FORBIDDEN_FR_UNVALIBLE));
	}

	return bRet;
}


bool ChapterModel::getNextBarrier( int nCurBarrierId, int& nNextBarrierId )
{
	if (nCurBarrierId == INVALID_CHAPTER_OR_BARRIER_ID)
	{
		return false;
	}

	// 初始化输出参数;
	nNextBarrierId = nCurBarrierId;

	// 1. 判断关卡序号，是否本章内最后一关;
	int nBarrierCount = 0;
	int nCurChapterId = constructChapterId(nCurBarrierId);
	auto iter = m_mapChapterInfoList.find(nCurChapterId);
	if (iter != m_mapChapterInfoList.end())
	{
		nBarrierCount = iter->second.vcBarriers.size();
	}
	else
	{
		return false;
	}

	// 若是最后一关，获取下一章的第一关;
	if (getBarrierOrder(nCurBarrierId) >= nBarrierCount)
	{
		auto it = m_mapChapterInfoList.find(nCurChapterId + 1);
		if (it != m_mapChapterInfoList.end())
		{
			nNextBarrierId = getFirstBarrierInChapter(nCurChapterId + 1);
			return true;
		}
		else
		{
			return false;
		}
	}
	// 若不是最后一关，+1返回;
	else
	{
		nNextBarrierId = nCurBarrierId + 1;
		return true;
	}

	return false;
}


bool ChapterModel::openBarrier( int nBarrierId )
{
	bool bIsFirstTime = false;
	int nChapterId = constructChapterId(nBarrierId);

	// 1.已完成章节;
	if (nChapterId < m_sFightRecord.nCurChapterId)
	{
		// 已记录的历史章节，可能只完成了一部分关卡，所以继续判断;
		// 将新关卡写入战斗记录，不影响当前进行的值;
		auto iter = m_sFightRecord.mapPrevBarrier.find(nChapterId);
		if (iter != m_sFightRecord.mapPrevBarrier.end())
		{
			auto it = iter->second.find(nBarrierId);

			// 若已有，跳过;
			if (it == iter->second.end())
			{
				FR_Barrier frBarrier(nBarrierId);
				iter->second.insert(pair<int, FR_Barrier>(nBarrierId, frBarrier));
				bIsFirstTime = true;
			}
		}
	}
	// 2.当前章节;
	else if (nChapterId == m_sFightRecord.nCurChapterId)
	{
		if (nBarrierId <= m_sFightRecord.nCurBarrierId)
		{
			// 当前章节中的历史关卡，已打开;
			// 因为关卡Id是按照从小到大的顺序排列了简单->普通->困难;
			// 所以只判断Id值的大小就可以严格区分是否已打过;
			return bIsFirstTime;
		}
		else
		{
			// 当前章节中的未开启关卡;
			auto iter = m_sFightRecord.mapPrevBarrier.find(nChapterId);
			if (iter != m_sFightRecord.mapPrevBarrier.end())
			{
				FR_Barrier frBarrier(nBarrierId);
				iter->second.insert(pair<int, FR_Barrier>(nBarrierId, frBarrier));
				bIsFirstTime = true;
			}

			// 修正当前进行;
			checkCurChapterAndBarrier(nBarrierId);
		}

	}
	// 3.新章节，将第一个关卡添加到战斗记录中去;
	else if (nChapterId > m_sFightRecord.nCurChapterId)
	{
		map<int, FR_Barrier> mapBarriers;
		FR_Barrier frBarrier(nBarrierId);
		mapBarriers.insert(pair<int, FR_Barrier>(nBarrierId, frBarrier));
		m_sFightRecord.mapPrevBarrier.insert(pair<int, map<int, FR_Barrier> >(nChapterId, mapBarriers));

		// 星级奖励领取记录也要构造出来;
		map<int, bool> mapStarRewardRec;
		for (int i = 1; i <= 3; ++i)
			mapStarRewardRec.insert(make_pair(i, false));
		m_sFightRecord.mapStarRewardCount.insert(make_pair(nChapterId, mapStarRewardRec));

		bIsFirstTime = true;

		// 修正当前进行;
		checkCurChapterAndBarrier(nBarrierId);
	}
	return bIsFirstTime;
}


void ChapterModel::checkCurChapterAndBarrier( int nBarrierId )
{
	int nChapterId = constructChapterId(nBarrierId);
	if (nChapterId > m_sFightRecord.nCurChapterId)
	{
		m_sFightRecord.nCurChapterId = nChapterId;
		m_sFightRecord.nCurBarrierId = nBarrierId;
	}
	else if (nChapterId == m_sFightRecord.nCurChapterId)
	{
		// 比较关卡序号,修正当前关卡;
		if (getBarrierOrder(nBarrierId) > getBarrierOrder(m_sFightRecord.nCurBarrierId))
		{
			m_sFightRecord.nCurBarrierId = nBarrierId;
		}
	}
}


void ChapterModel::updateChapterInfo(map<int, ChapterInfo>  mapInfo)
{
	m_mapChapterInfoList.clear();
	m_mapChapterInfoList = mapInfo;

	// 星级奖励数据结构初始化;
	for (auto iter = m_mapChapterInfoList.begin(); iter != m_mapChapterInfoList.end(); ++iter)
	{
		UI_STAR_REWARD_PARAM  starReward;
		starReward.nChapterId = iter->first;
		/*
		for (auto it = iter->second.vcStarReward.begin(); it != iter->second.vcStarReward.end(); ++it)
		{
			starReward.vcStarLv.push_back(it->first);
		}
		*/
		m_mapStarRewardRecord.insert(pair<int, UI_STAR_REWARD_PARAM>(iter->first, starReward));
	}
}

void ChapterModel::updateChapterPosInfo(map<int, ChapterPos> mapPosInfo)
{
	m_mapChapterPos = mapPosInfo;
}


void ChapterModel::updateUI(OBS_PARAM_TYPE_CM type, void* data)
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}


void ChapterModel::getFightRecordData(FR_ChapterStory& fightRecord)
{
	fightRecord.nCurChapterId = m_sFightRecord.nCurChapterId;
	fightRecord.nCurBarrierId = m_sFightRecord.nCurBarrierId;
	fightRecord.mapPrevBarrier = m_sFightRecord.mapPrevBarrier;
}

void ChapterModel::updateChapterStoryBase()
{
	// 1. 先更新整体章节数据;
	UI_CHAPTER_STORY_PARAM paramAll;
	paramAll.nCurChapterId = INVALID_CHAPTER_OR_BARRIER_ID;
	for (auto it = m_mapChapterInfoList.begin(); it != m_mapChapterInfoList.end(); ++it)
	{
		vector<int>  vcStar;
		vcStar.push_back(0);
		vcStar.push_back(it->second.vcBarriers.size());
		paramAll.mapChapterState.insert(pair<int, vector<int> >(it->first, vcStar));
	}
	updateUI(OBS_PARAM_TYPE_CHAPTER_STATE, (void*)(&paramAll));
}

void ChapterModel::updateChapterStoryState()
{
	// 2. 再更新进度;
	UI_CHAPTER_STORY_PARAM param;

	// 当前章节Id;
	param.nCurChapterId = m_sFightRecord.nCurChapterId;

	// 计算星数;
	for (auto iter = m_sFightRecord.mapPrevBarrier.begin(); iter != m_sFightRecord.mapPrevBarrier.end(); iter++)
	{
		// 已获取星数;
		int nStarCount = 0;
		for (auto it = iter->second.begin(); it != iter->second.end(); it++)
		{
			nStarCount += it->second.nStar;
		}
		vector<int>  vcStar;
		vcStar.push_back(nStarCount);

		// 总星数;
		auto it = m_mapChapterInfoList.find(iter->first);
		if (it != m_mapChapterInfoList.end())
		{
			vcStar.push_back(it->second.vcBarriers.size() * MAX_BARRIER_STAR_COUNT);
			param.mapChapterState.insert(pair<int, vector<int> >(iter->first, vcStar));
		}

		// 3. 星级奖励状态;
		bool bNext = false;
		auto ittt = m_sFightRecord.mapStarRewardCount.find(iter->first);
		if (ittt != m_sFightRecord.mapStarRewardCount.end())
		{
			map<int, bool>  maptmp = ittt->second;
			int count = 0;
			for (auto itstate : maptmp)
			{
				if (itstate.second)
					count++;
			}
			map<int, int>  mapTmpContent;
			int nNeedCount = getStarRewardsByLv(iter->first, count+1, mapTmpContent);
			bNext = (nStarCount >= nNeedCount && nNeedCount != -1) ? true : false;

			/*
			int nLv = ittt->second + 1;
			map<int, int>  mapTmp;
			int nNeedCount = getStarRewardsByLv(iter->first, nLv, mapTmp);
			bNext = (nStarCount >= nNeedCount && nNeedCount != -1) ? true : false;
			*/
		}
		param.mapStarRewardState.insert(make_pair(iter->first, bNext));
	}

	updateUI(OBS_PARAM_TYPE_CHAPTER_STATE, (void*)(&param));
}


void ChapterModel::updateChapterDetail(int nChapterId)
{
	UI_CHAPTER_DETAIL_PARAM  uiParam;

	// 从<配置的关卡数据>和<战斗记录>来匹配进度信息;
	auto iter = m_mapChapterInfoList.find(nChapterId);
	auto iterSR = m_mapStarRewardRecord.find(nChapterId);
	if (iter != m_mapChapterInfoList.end() && iterSR != m_mapStarRewardRecord.end())
	{
		uiParam.nChapterId = iter->second.nChapterId;
		uiParam.strChapterName = iter->second.strChapterName;
		uiParam.nTotalCoin = MainModel::getInstance()->getMainParam()->mCoin;
		uiParam.nTotalStrength = MainModel::getInstance()->getMainParam()->mPower;

		// 最高难度;
		auto rit = iter->second.vcBarriers.rbegin();
		assert(rit != iter->second.vcBarriers.rend());
		uiParam.nMaxLv = getBarrierLv(*rit);
		uiParam.bNewLv = getChapterNewLv(nChapterId);

		// 遍历战斗记录，找到对应的章节记录;
		auto it = m_sFightRecord.mapPrevBarrier.find(uiParam.nChapterId);
		if (it != m_sFightRecord.mapPrevBarrier.end())
		{
			// 战斗记录中的最后一关的难度即为已开启的最大难度;
			auto ittt = it->second.rbegin();
			assert(ittt != it->second.rend());

			uiParam.nNewLv = getBarrierLv(ittt->second.nBarrierId);

			// 星级奖励数据计算;
			iterSR->second.nStarCount = 0;
			for (auto itpre : it->second)
			{
				iterSR->second.nStarCount += itpre.second.nStar;
			}
			//iterSR->second.nStarCount = (ittt->second.nStar > 0) ? it->second.size() : (it->second.size() - 1);
			iterSR->second.nTotalStarCount = (int)(iter->second.vcBarriers.size() * MAX_BARRIER_STAR_COUNT);
		}

		// 更新至UI;
		updateUI(OBS_PARAM_TYPE_CHAPTER_INFO, (void*)(&uiParam));

		// 更新星级奖励;
		updateStarReward(nChapterId);
	}
}


void ChapterModel::selectBarrierLevel(BARRIER_LEVEL nLv)
{
	// 选中难度;
	updateUI(OBS_PARAM_TYPE_BARRIER_LV, (void*)(&nLv));
}


void ChapterModel::updateBarrierList(UI_BARRIER_LIST_PARAM& uiParam)
{
	vector<UI_BARRIER_PARAM> vcBarrierList;

	// 从<配置的关卡数据>和<战斗记录>来匹配进度信息;
	auto iter = m_mapChapterInfoList.find(uiParam.nChapterId);
	if (iter != m_mapChapterInfoList.end())
	{
		// 遍历本章全部关卡;
		for (unsigned int i = 0; i < iter->second.vcBarriers.size(); i++)
		{
			// 获取指定难度的关卡数据;
			BarrierInfo* _barrier = BarrierModel::getInstance()->getBarrierStoryById(iter->second.vcBarriers.at(i));
			assert(_barrier != nullptr);
			if (_barrier->nLevel != uiParam.nLv)
				continue;

			// 1)构建需要显示的关卡UI数据结构;
			UI_BARRIER_PARAM  param;
			param.nChapterId = iter->first;
			param.nBarrierId = _barrier->nBarrierId;
			param.nLv = getBarrierLv(param.nBarrierId);
			param.nResId = _barrier->nResId;
			param.strBarrierName = _barrier->strBarrierName;
			param.nBossShowId = _barrier->nBossShowId;
			param.bIsBoss = (param.nBossShowId == -1) ? false : true;
			param.mapFirstReward = _barrier->mapFirstReward;

			// 遍历战斗记录，匹配对应的章节&关卡;
			auto it = m_sFightRecord.mapPrevBarrier.find(param.nChapterId);
			assert(it != m_sFightRecord.mapPrevBarrier.end());
			auto ittt = it->second.find(param.nBarrierId);
			if (ittt != it->second.end())
			{
				param.nStarCount = ittt->second.nStar;
				param.bIsEnabled = true;
			}
			vcBarrierList.push_back(param);
		} // end for

		uiParam.vcBarrierList = vcBarrierList;

		// 更新至UI;
		updateUI(OBS_PARAM_TYPE_BARRIER_LIST, (void*)(&uiParam));
	}
}


void ChapterModel::updateSweepState(SweepState sweepData)
{
	// 更新Model数据，给外部读取;
	m_sSweepState = sweepData;

	// 更新至UI;
	updateUI(OBS_PARAM_TYPE_SWEEP_STATE, (void*)(&m_sSweepState));
}


void ChapterModel::updateSweepSettlement(vector<SweepSettlement> vcSettlement)
{
	// 保存扫荡结果;
	m_vcSweepSettlement = vcSettlement;
	for (int i = 0; i < m_vcSweepSettlement.size(); ++i)
	{
		//fix by rain: 加空指针判定，防止出现崩溃
		BarrierInfo* barrierInfo = BarrierModel::getInstance()->getBarrierStoryById(m_vcSweepSettlement.at(i).nBarrierId);
		if (barrierInfo != nullptr)
		{
			m_vcSweepSettlement.at(i).nPower = barrierInfo->nCost * -1;
		}
	}

	// 更新至UI;
	//log("===== sweep settlement, updateUI =====");
	updateUI(OBS_PARAM_TYPE_SWEEP_SETTLEMENT, (void*)(&m_vcSweepSettlement));
}

/*
int ChapterModel::getNextLvInBarrier( int nBarrierId, int N )
{
	// 解析当前难度;
	BARRIER_LEVEL nCurLv = getBarrierLv(nBarrierId);
	if (nCurLv >= BARRIER_LEVEL_2 || N <= 0)
	{
		return nBarrierId;
	}

	// 下一个相邻的难度;
	int nNextLvWithId = constructBarrierIdWithLv(nBarrierId, (BARRIER_LEVEL)(nCurLv+1));
	return getNextLvInBarrier(nNextLvWithId, N-1);
}
*/

int ChapterModel::getBarrierCount( int nChapterId )
{
	auto iter = m_mapChapterInfoList.find(nChapterId);
	if (iter != m_mapChapterInfoList.end())
	{
		return iter->second.vcBarriers.size();
	}
	return 0;
}

int ChapterModel::getBarrierCountWithLv(int nChapterId, BARRIER_LEVEL nLv)
{
	auto iter = m_mapChapterInfoList.find(nChapterId);
	if (iter != m_mapChapterInfoList.end())
	{
		int nCount = 0;
		BarrierInfo* _barrier = nullptr;

		for_each(iter->second.vcBarriers.begin(), iter->second.vcBarriers.end(),
			[&](const int& nId){
				_barrier = BarrierModel::getInstance()->getBarrierStoryById(nId);
				if (_barrier != nullptr && _barrier->nLevel == nLv)
				{
					nCount++;
				}
		});

		/*
		for (auto Id : iter->second.vcBarriers)
		{
			_barrier = BarrierModel::getInstance()->getBarrierStoryById(nId);
			if (_barrier != nullptr && _barrier->nLevel == nLv)
			{
				nCount++;
			}
		}
		*/

		return nCount;
	}
	return 0;
}

void ChapterModel::updateGetStarRewardResult(int nChapterId, int nStarIndex)
{
	// 增加已领取次数(统一入口，从战斗记录中修正);
	auto it = m_sFightRecord.mapStarRewardCount.find(nChapterId);
	if (it != m_sFightRecord.mapStarRewardCount.end())
	{
		it->second.find(nStarIndex)->second = true;;
	}
	else
	{
		map<int, bool>  mapState;
		mapState.insert(make_pair(1, false));
		mapState.insert(make_pair(2, false));
		mapState.insert(make_pair(3, false));
		mapState.find(nStarIndex)->second = true;
		m_sFightRecord.mapStarRewardCount.insert(make_pair(nChapterId, mapState));
	}

	// 更新UI;
	updateUI(OBS_PARAM_TYPE_STAR_REWARD_RESULT, (void*)(&(nStarIndex)));
}

void ChapterModel::updateStarReward( int nChapterId )
{
	auto iter = m_mapStarRewardRecord.find(nChapterId);
	if (iter != m_mapStarRewardRecord.end())
	{
		/*
		// 已领取次数;
		auto it = m_sFightRecord.mapStarRewardCount.find(nChapterId);
		if (it != m_sFightRecord.mapStarRewardCount.end())
		{
			//iter->second.nGetRewardTimes = it->second;
			map<int, bool>  maptmp = it->second;
			int count = 0;
			for (auto itstate : maptmp)
			{
				if (itstate.second)
					count++;
			}
			iter->second.nGetRewardTimes = count;
		}

		// 展示可能掉落;
		int nLv = iter->second.nGetRewardTimes + 1;
		if (nLv > MAX_STAR_REWARD_TYPE)
		{
			nLv = MAX_STAR_REWARD_TYPE;
		}
		iter->second.mapRewards.clear();
		int nNeedCount = getStarRewardsByLv(nChapterId, nLv, iter->second.mapRewards);
		iter->second.bNext = (iter->second.nStarCount >= nNeedCount && nNeedCount != -1) ? true : false;
		*/

		// 更新至UI;
		updateUI(OBS_PARAM_TYPE_STAR_REWARD, (void*)(&(iter->second)));
	}
}

int ChapterModel::getStarRewardsByLv( int nChapterId, int nLv, map<int, int>& mapRewards )
{
	if (nLv <= 0 || nLv > MAX_STAR_REWARD_TYPE)
	{
		return -1;
	}

	auto iter = m_mapChapterInfoList.find(nChapterId);
	if (iter != m_mapChapterInfoList.end())
	{
		size_t i = 0;
		for (auto it = iter->second.vcStarReward.begin(); it != iter->second.vcStarReward.end(); ++it, ++i)
		{
			if (i+1 == nLv)
			{
				for (auto ittt = it->second.begin(); ittt != it->second.end(); ++ittt)
				{
					mapRewards.insert(pair<int, int>(ittt->first, ittt->second));
					/*
					RowData* data = DataManager::getInstance()->searchToolById(ittt->first);
					if (nullptr != data)
					{
						//mapRewards.insert(pair<int, int>(data->getIntData("resId"), ittt->second));
						mapRewards.insert(pair<int, int>(ittt->first, ittt->second));
						//mapRewardsQua.insert(pair<int, int>(data->getIntData("itemQua"));
					}
					*/
				}
				return it->first;
			}
		}
	}
	else
	{
		return -1;
	}

	// 异常情况;
	return -1;//getBarrierCount(nChapterId);
}

/************************************************************************/
/* 查询关卡难度，会先获取到关卡数据本身，随机查询关卡难度时可使用;      */
/* 如果在循环中顺序查询若干关卡的难度，可以直接获取到关卡数据本身来使用;*/
/* 避免多次重复获取关卡数据，降低效率;									*/
/* Phil 03/10/2015														*/
/************************************************************************/
BARRIER_LEVEL ChapterModel::getBarrierLv( int nBarrierId )
{
	BarrierInfo* _barrier = BarrierModel::getInstance()->getBarrierStoryById(nBarrierId);
	if (nullptr != _barrier)
	{
		return _barrier->nLevel;
	}

	assert(false);
	return BARRIER_LEVEL_0;
}

int ChapterModel::getBarrierLogicOrder( int nBarrierId )
{
	int nLogicOrder = 1;
	BARRIER_LEVEL nLv = BARRIER_LEVEL_0;
	auto iter = m_mapChapterInfoList.find(constructChapterId(nBarrierId));
	assert(iter != m_mapChapterInfoList.end());
	for (unsigned int i = 0; i < iter->second.vcBarriers.size(); ++i)
	{
		BARRIER_LEVEL nTmpLv = getBarrierLv(iter->second.vcBarriers.at(i));
		if (nLv != nTmpLv)
		{
			nLv = nTmpLv;
			nLogicOrder = 1;
		}
		if (nBarrierId == iter->second.vcBarriers.at(i))
		{
			break;
		}
		else
		{
			nLogicOrder++;
		}
	}

	return nLogicOrder;
}

bool ChapterModel::getChapterNewLv( int nChapterId )
{
	auto iter = m_mapChapternewLv.find(nChapterId);
	if (iter != m_mapChapternewLv.end())
	{
		return iter->second;
	}
	return false;
}

void ChapterModel::resetChapterNewLv( int nChapterId )
{
	auto iter = m_mapChapternewLv.find(nChapterId);
	if (iter != m_mapChapternewLv.end())
	{
		iter->second = false;
	}
}

bool ChapterModel::isBarrierPassed( int nBarrierId )
{
	bool bRet = false;
	int nChapterId = constructChapterId(nBarrierId);
	auto iter = m_sFightRecord.mapPrevBarrier.find(nChapterId);
	if (iter != m_sFightRecord.mapPrevBarrier.end())
	{
		auto it = iter->second.find(nBarrierId);
		if (it != iter->second.end() && it->second.nStar > 0)
		{
			bRet = true;
		}
	}

	return bRet;
}

cocos2d::Vec2 ChapterModel::getChapterPos( int nChapterId )
{
	Vec2 pos;
	auto iter = m_mapChapterPos.find(nChapterId);
	if (iter != m_mapChapterPos.end())
	{
		pos.x = iter->second.fPosX;
		pos.y = iter->second.fPosY;
	}
	return pos;
}

bool ChapterModel::isGetStarReward( int nChapterId, int nStarIndex )
{
	bool bRet = false;

	auto ittt = m_sFightRecord.mapStarRewardCount.find(nChapterId);
	if (ittt != m_sFightRecord.mapStarRewardCount.end())
	{
		return ittt->second.find(nStarIndex)->second;
	}

	return bRet;
}

int ChapterModel::getVipLvBySweepFunc()
{
	int nRet = 0;

	DataTable* dtVip = DataManager::getInstance()->getDTVip();
	for (auto iter = dtVip->getTableData().begin(); iter != dtVip->getTableData().end(); ++iter)
	{
		if (iter->second.getIntData("sweep") > 0)
		{
			nRet = iter->second.getIntData("id");
			break;
		}
	}

	return nRet;
}

int ChapterModel::getMusicNameIndex( int nChapterId )
{
	string strName = "chapter_";
	int nChapterOrder = ChapterModel::getInstance()->getChapterOrderFromChapterId(nChapterId);

	int a[9] = {1, 7, 13, 14, 19, 23, 27, 31, 36};
	vector<int>  vc1(a, a+9);
	auto it = find(vc1.begin(), vc1.end(), nChapterOrder);
	if (it != vc1.end())
		return 1;

	int b[5] = {2, 4, 10, 12, 16};
	vector<int>  vc2(b, b+5);
	it = find(vc2.begin(), vc2.end(), nChapterOrder);
	if (it != vc2.end())
		return 2;

	int c[6] = {6, 17, 22, 26, 33, 38};
	vector<int>  vc3(c, c+6);
	it = find(vc3.begin(), vc3.end(), nChapterOrder);
	if (it != vc3.end())
		return 3;

	int d[7] = {8, 15, 21, 28, 30, 35, 40};
	vector<int>  vc4(d, d+7);
	it = find(vc4.begin(), vc4.end(), nChapterOrder);
	if (it != vc4.end())
		return 4;

	int e[8] = {3, 5, 9, 18, 25, 29, 34, 39};
	vector<int>  vc5(e, e+8);
	it = find(vc5.begin(), vc5.end(), nChapterOrder);
	if (it != vc5.end())
		return 5;

	int f[5] = {11, 20, 24, 32, 37};
	vector<int>  vc6(f, f+5);
	it = find(vc6.begin(), vc6.end(), nChapterOrder);
	if (it != vc6.end())
		return 6;

	// error
	return 0;
}

int ChapterModel::getCurMaxTimes()
{
	DataTable* _dt = DataManager::getInstance()->getDTVip();
	assert(_dt != nullptr);
	int nCurVipLv = VipModel::getInstance()->getCurVipLv();
	int nCurValue = 0;
	RowData* _row = _dt->searchDataById(nCurVipLv);
	if (_row != nullptr)
	{
		nCurValue = _row->getIntData("barrier");
	}
	return nCurValue;
}

int ChapterModel::getNextValidVip( int& nNextValue )
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
		nNextValue = _rowNext->getIntData("barrier");
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

int ChapterModel::getBarrierTotalTimes( int nBarrierId )
{
	BarrierInfo* barrier = BarrierModel::getInstance()->getBarrierStoryById(nBarrierId);
	int nTotalTimes = (barrier != nullptr) ? barrier->nTime : 0;
	return nTotalTimes;
}

int ChapterModel::getBarrierLeftTimes( int nBarrierId )
{
	auto iterChapter = m_sFightRecord.mapPrevBarrier.find(constructChapterId(nBarrierId));
	assert(iterChapter != m_sFightRecord.mapPrevBarrier.end());
	auto iterBarrier = iterChapter->second.find(nBarrierId);
	int nDayCount = (iterBarrier != iterChapter->second.end()) ? iterBarrier->second.nDayCount : 0;

	// 取出双倍奖励中额外的次数加成;
	DOUBLE_BONUS_TYPE type = (getBarrierLv(nBarrierId) == BARRIER_LEVEL_0) ? DOUBLE_BONUS_TYPE_STORY : DOUBLE_BONUS_TYPE_ELITE;
	const vector<DoubleBonusData*>* vcDoubleBonusData = DoubleBonusModel::getInstance()->getDataByType(type);
	int nAddCount = 0;
	if (vcDoubleBonusData != nullptr)
	{
		nAddCount = max(0, vcDoubleBonusData->at(0)->nReservedParam1);
	}

	// 计算总的剩余次数;
	int nLeftTimes = getBarrierTotalTimes(nBarrierId) - nDayCount + nAddCount;
	nLeftTimes = max(0, nLeftTimes);
	return nLeftTimes;
}

void ChapterModel::updateBarrierResetCount( int nBarrierId )
{
	auto iterChapter = m_sFightRecord.mapPrevBarrier.find(constructChapterId(nBarrierId));
	assert(iterChapter != m_sFightRecord.mapPrevBarrier.end());
	auto iterBarrier = iterChapter->second.find(nBarrierId);
	assert(iterBarrier != iterChapter->second.end());
	iterBarrier->second.nResetDayCount++;

	// 重置;
	iterBarrier->second.nDayCount = 0;
	BarrierModel::getInstance()->updateBarrierStoryShow(nBarrierId, &(iterBarrier->second));
}

void ChapterModel::setJumpType( CHAPTER_JUMP_TYPE jumpType )
{
	m_jumpType = jumpType;
}
