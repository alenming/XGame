#include "TowerController.h"
#include "Communication/Command.h"
#include "Communication/SocketCommunicator.h"
#include "TowerModel.h"
#include "FormationModule/FormationModel.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainModel.h"
#include "LevelUp/LevelUpController.h"
#include "SDK_TalkingData/TalkingData.h"


TowerController* TowerController::m_pInstance = nullptr;

TowerController::TowerController(void)
{
	SocketCommunicator::getInstance()->addListener(this);
	m_nStamp = 0;
}


TowerController::~TowerController(void)
{
	SocketCommunicator::getInstance()->removeListener(this);
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(TowerController::refreshCountDown), this);
}

TowerController* TowerController::getInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new TowerController;
	}
	return m_pInstance;
}

void TowerController::destroyInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void TowerController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_TRAIL_MAIN_INFO:
		procTrailMainInfo(szData);
		break;

	case nMAIN_CMD_TRAIL_GET_TRAIL_OP:
		procGetTrailOp(szData);
		break;

	case nMAIN_CMD_TRAIL_REWARD:
		procTrailReward(szData);
		break;

	case nMAIN_CMD_TRAIL_BUY_BUFF:
		procBuyBuff(szData);
		break;

	case nMAIN_CMD_TRAIL_OPEN_TREASURE:
		procOpenTreasure(szData);
		break;

	case nMAIN_CMD_TRAIL_REFRESH_TREASURE:
		procRefreshTreasure(szData);
		break;

	case nMAIN_CMD_TRAIL_RANKING:
	case nMAIN_CMD_TOWER_RANKING:
		procRanking(nCmdID, szData);
		break;

	case nMAIN_CMD_TOWER_MAIN_INFO:
		procTowerMainInfo(szData);
		break;

	case nMAIN_CMD_TOWER_RESET:
		procResetTower(szData);
		break;

	case nMAIN_CMD_TOWER_SWEEP:
		procTowerSweep(szData);
		break;

	case nMAIN_CMD_TOWER_SWEEP_SETTLEMENT:
		procTowerSweepSettlement(szData);
		break;

	default:
		break;
	}
}

void TowerController::handleTailMainInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAIL_MAIN_INFO, "");
}

bool TowerController::handleTrailReward()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAIL_REWARD, "");
	return true;
}

void TowerController::procTrailMainInfo(const char* szData)
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
		TrailMainInfo info;
		info.nCurFloor = data["curId"].asInt();
		info.nMaxFloor = data["topId"].asInt();
		info.nScore = data["score"].asInt();
		info.nStar = data["starNum"].asInt();
		info.nGold = data["gold"].asInt();
		info.bEnd = (data["topFlag"].asInt() == 1);

		Json::Value buffList = data["buffList"];
		for (auto it = buffList.begin(); it != buffList.end(); it++)
		{
			info.mapBuffEffect.insert(pair<string, float>(it.memberName(), buffList[it.memberName()].asDouble()));
		}

		// 更新数据;
		TowerModel::getInstance()->updateTrailMainInfo(info);

		// 更新楼层;
		checkCurFloor(TOWER_TYPE_TRAIL);
	}
}

void TowerController::procTrailFormation( const char* szData )
{
	
}

void TowerController::checkCurFloor( TOWER_TYPE nType )
{
	if (nType == TOWER_TYPE_TRAIL)
	{
		// 若有触发buff，则当前层-1，因为服务器已经行进至下一层;
		// 直至buff触发完成，恢复实际层数;
		if (TowerModel::getInstance()->isBuffTriggered())
		{
			//nFloor--;
			TowerModel::getInstance()->updateTrailCurFloorNum(-1);
		}
		TowerModel::getInstance()->updateTrailFloor();
	}
	else if (nType == TOWER_TYPE_TOWER)
	{
		TowerModel::getInstance()->updateTowerFloor();
	}
}

void TowerController::handleBuffContent()
{
	int nCurFloor = TowerModel::getInstance()->getTrailCurFloor();
	TowerModel::getInstance()->updateBuff(nCurFloor);
}

void TowerController::handleBuyBuff( int nBuffId, int nHeroId /*= -1*/ )
{
	if (nBuffId == -1)
	{
		return;
	}

	if (TowerModel::getInstance()->getBuffPrice(nBuffId) > TowerModel::getInstance()->getTrailMainInfo()->nStar)
	{
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20069));
		return;
	}

	Json::FastWriter writer;
	Json::Value data;
	data["buffId"] = nBuffId;
	data["petId"] = nHeroId;	// -1:全体;
	string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAIL_BUY_BUFF, json_file.c_str());

	// 即时刷新buff状态;
	TowerModel::getInstance()->updateBuffState(nBuffId);
}

void TowerController::procBuyBuff( const char* szData )
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
		int nStar = data["starNum"].asInt();
		Json::Value buffList = data["buffList"];
		map<string, float> mapBuff;
		for (auto it = buffList.begin(); it != buffList.end(); it++)
		{
			//string str(it.memberName());
			mapBuff.insert(make_pair(it.memberName(), buffList[it.memberName()].asDouble()));
		}

		// 更新星星数量和新的buff列表;
		TowerModel::getInstance()->updateStarCount(nStar, mapBuff);
	}
}

bool TowerController::handleCloseBuff()
{
	bool bAllApplied = TowerModel::getInstance()->isBuffAllApplied();
	if (bAllApplied)
	{
		TowerModel::getInstance()->setBuffTriggered(false);

		// 更新楼层(递增至下一层);
		TowerModel::getInstance()->updateTrailCurFloorNum();
	}
	return bAllApplied;
}

bool TowerController::handleRewardClose()
{
	// 更新楼层(递增至下一层);
	TowerModel::getInstance()->updateTrailCurFloorNum();

	// 有无触发宝藏;
	return TowerModel::getInstance()->isTreasureTriggered();
}

bool TowerController::handleOpenTreasure()
{
	// 检查金币数量;
	int nNextGold = TowerModel::getInstance()->getNextTreasureCost();
	int nTotalGold = MainModel::getInstance()->getMainParam()->mGold;
	if (nNextGold > nTotalGold)
	{
		return false;
	}

	// 发送请求;
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAIL_OPEN_TREASURE, "");
	return true;
}

int TowerController::handleRefreshTreasure()
{
	// 检查剩余刷新次数;
	if (TowerModel::getInstance()->m_queTreasureRefreshData.empty())
		return -1;

	// 检查金币数量;
	int nRefreshGold = TowerModel::getInstance()->getRefreshTreasureCost();
	int nTotalGold = MainModel::getInstance()->getMainParam()->mGold;
	if (nRefreshGold > nTotalGold)
	{
		return -2;
	}

	// 刷新;
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAIL_REFRESH_TREASURE, "");
	return 0;
}

/*
void TowerController::handleResetTrail()
{
	// 2. 发起重置;
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAIL_RESET, "");
}
*/

/*
void TowerController::procResetTrail( const char* szData )
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
		int nId = data["curId"].asInt();
		int nDate = data["resetdate"].asInt();

		// 重置;
		TowerModel::getInstance()->resetTrail(nDate);
	}
}
*/

void TowerController::handleGetTrailOp()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAIL_GET_TRAIL_OP, "");
}

void TowerController::procGetTrailOp(const char* szData)
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
		Json::Value enemyInfo = data["enemyInfo"];
		ostringstream oss;
		map<int, TrailOp>  mapTrailOp;
		for (unsigned int i = 0; i < 3; ++i)
		{
			oss.str("");
			oss << i+1;
			Json::Value single = enemyInfo[oss.str()];
			TrailOp op;
			op.nType = single["id"].asInt();
			op.strName = single["name"].asString();
			op.nLv = single["level"].asInt();
			op.nAtk = single["totalAtk"].asInt();
			Json::Value pets = single["matrixIds"];
			for (unsigned int k = 0; k < pets.size(); k++)
			{
				matrixData singleMatrix;
				singleMatrix.id = pets[k]["id"].asInt();
				singleMatrix.stars = pets[k]["stars"].asInt();
				op.vcFormation.push_back(singleMatrix);
			}
			mapTrailOp.insert(pair<int, TrailOp>(i+1, op));
		}

		// 更新数据;
		TowerModel::getInstance()->updateTrailOp(mapTrailOp);

		// 试炼对手状态;
		int nValidLv = data["hardLevel"].asInt();
		if (nValidLv > 0 && nValidLv <= 3)
		{
			TowerModel::getInstance()->updateTrailOpState(nValidLv);
		}
	}
}

void TowerController::procTrailReward( const char* szData )
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
		Json::Value reward = data["itemList"];

		// 更新货币;
		int nTotalCoin = data["coin"].asInt();
		MainModel::getInstance()->setSilver(nTotalCoin);
		int nTotalGold = data["gold"].asInt();
		MainModel::getInstance()->setGold(nTotalGold);

		// 奖励内容<templateId, count>;
		map<int, int>  mapReward;
		for (unsigned int i = 0; i < reward.size(); ++i)
		{
			Json::Value single = reward[i];
			int nTemplateId = single["templateId"].asInt();
			mapReward.insert(pair<int, int>(nTemplateId, single["stack"].asInt() - ToolModel::getInstance()->searchNumByTemplateId(nTemplateId)));
		}

		// 更新背包;
		ToolModel::getInstance()->updateToolJsonData(reward);

		// 更新背包大小;
		int nPkgSize = data["pkgsize"].asInt();
		int nPkgUseSize = data["pkgUsesize"].asInt();
		ToolModel::getInstance()->updateBackpackUsedCount(nPkgUseSize, nPkgSize);

		// 更新数据;
		TowerModel::getInstance()->updateTrailReward(mapReward);

		// 是否触发宝藏;
		bool bIsTreasureTriggered = (data["eventTyp"].asInt() != -1) ? true : false;
		if (bIsTreasureTriggered)
		{
			TowerModel::getInstance()->resetTreasureData();
			int nTreasureBoxId = data["boxId"].asInt();
			TowerModel::getInstance()->setTreasureTriggered(nTreasureBoxId);

			// 宝藏相关数据;
			Json::Value refreshData = data["refreshCostList"];
			for (int k = 0; k < refreshData.size(); ++k)
				TowerModel::getInstance()->m_queTreasureRefreshData.push(atoi(refreshData[k].asString().c_str()));

			Json::Value openData = data["costList"];
			for (int k = 0; k < openData.size(); ++k)
			{
				TowerModel::getInstance()->m_queTreasureOpenData.push(atoi(openData[k].asString().c_str()));
				TowerModel::getInstance()->m_queTreasureOpenData_backup.push(atoi(openData[k].asString().c_str()));
			}
		}
	}
}

void TowerController::procOpenTreasure( const char* szData )
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
		Json::Value treasure = data["itemList"];

		// 更新货币;
		int nTotalCoin = data["coin"].asInt();
		MainModel::getInstance()->setSilver(nTotalCoin);
		int nTotalGold = data["gold"].asInt();
		MainModel::getInstance()->setGold(nTotalGold);
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20070), 1, TowerModel::getInstance()->getNextTreasureCost());

		// 更新背包;
		ToolModel::getInstance()->updateToolJsonData(treasure);

		// 更新背包大小;
		int nPkgSize = data["pkgsize"].asInt();
		int nPkgUseSize = data["pkgUsesize"].asInt();
		ToolModel::getInstance()->updateBackpackUsedCount(nPkgUseSize, nPkgSize);

		// 宝藏内容<templateId, count>;
		// 其实只有一个，不过要按这个格式解析;
		map<int, int>  mapTreasure;
		/*for (int i = 0; i < treasure.size(); ++i)
		{
			Json::Value single = treasure[i];
			mapTreasure.insert(pair<int, int>(single["templateId"].asInt(), single["stack"].asInt()));
		}*/
		// 改协议，物品展示单独加了showItem字段，以防止itemList为空的情况;
		Json::Value showItem = data["showItem"];
		for (int i = 0; i < showItem.size(); ++i)
		{
			Json::Value single = showItem[i];
			mapTreasure.insert(pair<int, int>(single["id"].asInt(), single["amount"].asInt()));
		}
		assert(!mapTreasure.empty());

		// 更新数据;
		TowerModel::getInstance()->onTreasureOpend(mapTreasure.begin()->first, mapTreasure.begin()->second);
		TowerModel::getInstance()->updateGold(nTotalGold);
	}
}

void TowerController::procRefreshTreasure( const char* szData )
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

		// 更新货币;
		int nTotalGold = data["gold"].asInt();
		MainModel::getInstance()->setGold(nTotalGold);
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20071), 1, TowerModel::getInstance()->getRefreshTreasureCost());

		// 宝藏Id;
		int nTreasureId = data["boxId"].asInt();

		// 更新数据;
		TowerModel::getInstance()->refreshTreasure(nTreasureId);
		TowerModel::getInstance()->updateGold(nTotalGold);
	}
}

void TowerController::handleTreasureClose()
{
	// 重置宝藏数据;
	TowerModel::getInstance()->resetTreasureData();

	// 更新楼层(递增至下一层);
	//TowerModel::getInstance()->updateTrailCurFloorNum();
}

void TowerController::handleRanking( TOWER_TYPE nType )
{
	int nCmd = (nType == TOWER_TYPE_TOWER) ? nMAIN_CMD_TOWER_RANKING : nMAIN_CMD_TRAIL_RANKING;
	SocketCommunicator::getInstance()->sendMessage(nCmd, "");
}

void TowerController::procRanking( int nCmd, const char* szData )
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
		Json::Value rankList = data["orderList"];

		UI_TRAIL_RANKING_PARAM  uiParam;
		uiParam.nRank = data["orderNum"].asInt();
		if (uiParam.nRank < 0)
		{
			uiParam.nRank = 0;
		}
		uiParam.nMaxFloor = data["topId"].asInt();
		if (uiParam.nMaxFloor < 0)
		{
			uiParam.nMaxFloor = 0;
		}

		vector<RankingData>  vcRankingParam;
		for (unsigned int i = 0; i < rankList.size(); ++i)
		{
			Json::Value single = rankList[i];
			RankingData  param;
			param.nResId = single["petResId"].asInt();
			param.nFc = single["totalAtk"].asInt();
			param.strPlayerName = single["nickname"].asString();
			param.nMaxFloor = single["topId"].asInt();
			param.nVipLv = single["vipLevel"].asInt();
			param.strGroupName = single["group"].asString();
			if (param.nMaxFloor < 0)
			{
				param.nMaxFloor = 0;
			}
			param.nRankingIndex = i;
			param.bShowRankingIcon = (param.nRankingIndex < 3);
			Json::Value matrix = single["matrixIds"];
			for (unsigned int k = 0; k < matrix.size(); ++k)
			{
				matrixData singleMatrix;
				singleMatrix.id = matrix[k]["id"].asInt();
				singleMatrix.stars = matrix[k]["stars"].asInt();
				param.vcFormation.push_back(singleMatrix);
			}
			assert(param.vcFormation.size() == 9);
			vcRankingParam.push_back(param);
		}
		uiParam.vcRankingData = vcRankingParam;

		TOWER_TYPE nType = (nCmd == nMAIN_CMD_TOWER_RANKING) ? TOWER_TYPE_TOWER : TOWER_TYPE_TRAIL;
		TowerModel::getInstance()->updateRanking(nType, uiParam);
	}
}

void TowerController::handleTowerMainInfo()
{
	// 1. 本地信息;
	if (TowerModel::getInstance()->getTowerFloorInfo()->empty())
	{
		parseTowerLocalInfo();
	}

	// 2. 主界面信息;
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOWER_MAIN_INFO, "");
}

void TowerController::procTowerMainInfo( const char* szData )
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
		TowerMainInfo info;
		info.nCurFloor = TowerModel::getInstance()->towerId2Floor(data["curId"].asInt());
		info.nMaxFloor = TowerModel::getInstance()->towerId2Floor(data["topId"].asInt());
		info.nLeftResetCount = data["resetcount"].asInt();
		info.bEnd = (data["topFlag"].asInt() == 1);

		// 判断扫荡剩余时间，是否在扫荡中;
		int nLeftTime = data["needSeconds"].asInt();
		info.nLeftSec = nLeftTime;

		// 扫荡时间已经结束;
		info.bIsSweep = nLeftTime <= 0 ? false : true;

		// 是否有扫荡结果未取(服务器将未取结果的都当做是扫荡中);
		int nFlag = data["cleanUpFlag"].asInt();
		info.bSweepResult = (nFlag == 0 ? false : true);

		// 更新数据;
		TowerModel::getInstance()->updateTowerMainInfo(info);

		// 更新楼层;
		checkCurFloor(TOWER_TYPE_TOWER);

		// 仍在扫荡计时中;
		if (nLeftTime > 0)
		{
			// 更新时间戳;
			m_nStamp = nLeftTime;

			// 重新计时;
			Director::getInstance()->getScheduler()->unschedule(schedule_selector(TowerController::refreshCountDown), this);
			Director::getInstance()->getScheduler()->schedule(
				schedule_selector(TowerController::refreshCountDown), this, 1.0f, kRepeatForever, 0.000001f, false);
		}
	}
}

bool TowerController::handlePreResetTower()
{
	// 判定剩余重置次数;
	if (TowerModel::getInstance()->getTowerMainInfo()->nLeftResetCount > 0)
	{
		return true;
	}

	return false;
}

bool TowerController::handlePreResetTower2()
{
	// 判定金币数量;
	if (MainModel::getInstance()->getMainParam()->mGold >= TowerModel::getInstance()->getTowerResetCost())
	{
		return true;
	}

	return false;
}

void TowerController::handleResetTower()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOWER_RESET, "");
}

void TowerController::procResetTower( const char* szData )
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
		int nId = data["curId"].asInt();

		// 更新金币;
		int nGold = data["gold"].asInt();
		MainModel::getInstance()->setGold(nGold);
		MainModel::getInstance()->notifyObservers();

		//
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20072), 1, TowerModel::getInstance()->getTowerResetCost());

		// 数据重置;
		TowerModel::getInstance()->resetTower();
	}
}

void TowerController::parseTowerLocalInfo()
{
	DataTable* dtBarrier = DataManager::getInstance()->getDTBarrier();

	map<int, BarrierInfo>  mapInfo;
	multimap<int, RowData>& tableData = dtBarrier->getTableData();
	for (auto iter = tableData.begin(); iter != tableData.end(); iter++)
	{
		BarrierInfo info;
		info.nType = (CHAPTER_TYPE)(iter->second.getIntData("type"));
		if (info.nType != CHAPTER_TYPE_TOWER)
		{
			continue;
		}

		info.nBarrierId = iter->second.getIntData("id");
		info.strBarrierName = (iter->second.getStringData("name"));
		info.nEnterLevel = iter->second.getIntData("enterLv");
		info.strDescription = (iter->second.getStringData("des"));
		string strRewardShow = (iter->second.getStringData("rewardShow"));
		if (strRewardShow.compare("-1") != 0)
		{
			StringFormat::parseCsvStringMap(strRewardShow, info.mapRewardShow);
		}
		info.nBossShowId = iter->second.getIntData("bossShow");
		info.nCoin = iter->second.getIntData("coin");
		info.nExp = iter->second.getIntData("exp");

		mapInfo.insert(pair<int, BarrierInfo>(info.nBarrierId, info));
	}

	// 更新数据;
	TowerModel::getInstance()->updateTowerFloorInfo(mapInfo);
}

/*
int TowerController::handleTowerFight()
{
	// 1. 剩余挑战次数;
	//if (TowerModel::getInstance()->getLeftTimes() <= 0)
	//	return -1;

	// 2. 背包;
	if(!ToolModel::getInstance()->backpackIsUsable())
		return -2;

	return 0;
}
*/

/*
int TowerController::handleBuyChallengeTimes()
{
	// 1. 检查剩余购买次数;
	if (TowerModel::getInstance()->getLeftBuyTimes() <= 0)
	{
		return -1;
	}

	// 2. 检查元宝数量;
	if (MainModel::getInstance()->getMainParam()->mGold < TowerModel::getInstance()->getPrice())
	{
		return -2;
	}

	// 2. 购买;
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOWER_BUY_CHALLENGE_TIMES, "");
	return 0;
}
*/

/*
void TowerController::procBuyChallengeTimes( const char* szData )
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

	}
}*/

void TowerController::handleTowerReward()
{
	TowerModel::getInstance()->updateTowerRewardPreview();
}

void TowerController::handleTowerSweep()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOWER_SWEEP, "");
}

void TowerController::procTowerSweep( const char* szData )
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

		// 记录扫荡状态;
		TowerModel::getInstance()->setTowerSweepState(true);

		// 记录时间戳;
		m_nStamp = data["needSeconds"].asInt();

		// 开始计时;
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(TowerController::refreshCountDown), this);
		Director::getInstance()->getScheduler()->schedule(
			schedule_selector(TowerController::refreshCountDown), this, 1.0f, kRepeatForever, 0.000001f, false);
	}
}

void TowerController::handleTowerSweepSettlement()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOWER_SWEEP_SETTLEMENT, "");
}

void TowerController::procTowerSweepSettlement( const char* szData )
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
			
		// 解析扫荡结果;
		m_towerSweepSettlement.nCoin = data["getCoin"].asInt();
		m_towerSweepSettlement.nExp = data["getExp"].asInt();
		Json::Value itemList = data["getItemList"];
		for (auto it = itemList.begin(); it != itemList.end(); ++it)
		{
			m_towerSweepSettlement.mapItem.insert(make_pair(atoi(it.memberName()), itemList[it.memberName()].asInt()));
		}

		// 数据变化;
		procChangeInfo(data["changeInfo"]);

		// 更新结果;
		TowerModel::getInstance()->updateSweepSettlement(&m_towerSweepSettlement);

		// 升级检测;
		LevelUpController::getInstance()->checkLevelUp();
	}
}

void TowerController::procChangeInfo( const Json::Value& changeInfo )
{
	////////// 结算协议中的固有数据结构解析;

	// 背包已使用/总容量;
	int nPkgUseSize = changeInfo["pkgUsesize"].asInt();
	int nPkgSize = changeInfo["pkgsize"].asInt();
	ToolModel::getInstance()->updateBackpackUsedCount(nPkgUseSize, nPkgSize);

	// 英雄列表;
	Json::Value pets = changeInfo["petList"];
	HeroModel::getInstance()->updateData(pets);

	// 背包物品;
	Json::Value itemList = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(itemList);

	// 更新数据;
	MainModel::getInstance()->updateData(changeInfo);

	/*// 积分(区分具体类型来解析使用，用不到就不解);
	int nScore = changeInfo["score"].asInt();

	// 体力;
	int nPower = changeInfo["power"].asInt();
	MainModel::getInstance()->setPower(nPower);

	// 精力;
	int nEnergy = changeInfo["energy"].asInt();
	MainModel::getInstance()->setEnergy(nEnergy);

	// 金币;
	int nGold = changeInfo["gold"].asInt();
	MainModel::getInstance()->setGold(nGold);

	// 银币;
	int nCoin = changeInfo["coin"].asInt();
	MainModel::getInstance()->setSilver(nCoin);*/

}

void TowerController::refreshCountDown( float delta )
{
	TowerModel::getInstance()->onCountDown(m_nStamp);
	if (m_nStamp <= 0)
	{
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(TowerController::refreshCountDown), this);

		// 取扫荡结果;
		handleTowerSweepSettlement();
		return;
	}
	m_nStamp--;
}

void TowerController::checkUISweepSettlement( bool bShown /*= false*/ )
{
	// 检查扫荡结果是否已显示;
	if (!bShown)
	{
		if (!m_towerSweepSettlement.empty())
		{
			TowerModel::getInstance()->updateSweepSettlement(&m_towerSweepSettlement);
		}
	}
	// 确认已显示;
	else
	{
		m_towerSweepSettlement.clear();
		TowerModel::getInstance()->checkUISweepSettlement();
	}
}

void TowerController::onTowerViewExit()
{
	// 以爬塔试炼主界面为准，因为是在主界面请求TowerMainInfo;
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(TowerController::refreshCountDown), this);
}

