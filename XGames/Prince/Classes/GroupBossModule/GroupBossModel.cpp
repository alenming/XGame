#include "GroupBossModel.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "MainModule/MainModel.h"
#include "Common/Common.h"
#include "Communication/Command.h"
#include "SDK_TalkingData/TalkingData.h"
#include "ToolModule/ToolModel.h"
#define JINNANGID	10071

GroupBossModel* GroupBossModel::ms_pInstance = nullptr;

GroupBossModel::GroupBossModel()
{
	m_iMyTotalDam = 0;
	m_iInspireTimes = 0;
	m_iRestAtkTimes = 0;
	m_iMyTotalDam = 0;		
	m_iRestTime = 0;
	m_iRewardBossId = 0;
	m_fDamAdded = 0.0f;
	
	m_bIsActOpen = false;
	m_bIsCanAtk = false;
	m_bIsNeedUpdateCoolTime = true;

	m_vGroupDamRank.clear();
	m_vPlayerDamRank.clear();
	m_vMemberDamRank.clear();
	m_vBoxReward.clear();
	m_vDamReward.clear();
	m_vInspireCost.clear();
	m_vMainGroupRank.clear();
	m_vMainPlayerRank.clear();

	initFromLocal();
}

GroupBossModel::~GroupBossModel()
{

}

GroupBossModel* GroupBossModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new GroupBossModel();
	}
	return ms_pInstance;
}

void GroupBossModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void GroupBossModel::initFromLocal()
{
	//初始化鼓舞金币消耗
	m_vInspireCost.clear();
	RowData* inspireCostData = DataManager::getInstance()->getDTToolSystem()->searchDataById(INSPIRE_COST_DATA_ID);
	if(inspireCostData != nullptr)
	{
		string inspireCost = inspireCostData->getStringData("value");
		StringFormat::parseCsvStringVec(inspireCost, m_vInspireCost);
	}

	//初始化伤害奖励阈值
	m_vDamReward.clear();
	multimap<int, RowData>& groupBossMap = DataManager::getInstance()->getDTGroupBoss()->getTableData();
	for(auto iter = groupBossMap.begin(); iter != groupBossMap.end(); iter++)
	{
		RowData* rowData = &(iter->second);
		if(rowData)
		{
			if(rowData->getIntData("type") == eTYPE_DAM_EXCHANGE)
			{
				sDamReward damRewardItem;
				damRewardItem.boxId = iter->first;
				damRewardItem.damLimit = rowData->getIntData("value");
				damRewardItem.isCanGet = false;		
				damRewardItem.isGetted = false;
				
				//解析配表奖励
				string boxInfo = rowData->getStringData("item");

				if(!boxInfo.empty())
				{
					vector<string> vecStr;
					StringFormat::parseCsvStringVec(boxInfo, vecStr);
					damRewardItem.vDamRewardInfo.clear();
					for(string strTmp : vecStr)
					{
						int nIndex1 = strTmp.find_first_of(',', 0);
						string strTmp1 = strTmp.substr(0, nIndex1);
						string strTmp2 = strTmp.erase(0, nIndex1 + 1);
	
						sRewardItemData rewardInfoItem;
						rewardInfoItem.rewardId = atoi(strTmp1.c_str());
						rewardInfoItem.rewardCnt = atoi(strTmp2.c_str());

						damRewardItem.vDamRewardInfo.push_back(rewardInfoItem);
					}
				}
				m_vDamReward.push_back(damRewardItem);
			}
		}
	}
}

void GroupBossModel::initFromSvr( const Json::Value& data )
{
	Json::Value groupRank = data["bangpaiPM"];
	Json::Value bossData = data["bossdata"];
	Json::Value playerRank = data["gerenPM"];
	Json::Value bossReward = data["bossreward"];
	Json::Value exchangeList = data["exchangeList"];
	Json::Value myrewardedList = data["myrewardedList"];
	
	//updateGroupRankData(groupRank);
	//updatePlayerRankData(playerRank, eTYPE_ALL);
	updateMainGroupRankData(groupRank);
	updateMainPlayerRankData(playerRank, eTYPE_ALL);
	updateBossInfo(bossData);
	updateKilledBossRewardData(bossReward);
	updateBossCanGetInfo(myrewardedList);
	updateExchangeCanInfo(exchangeList);
	//通知UI, 刷新主界面
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_GROUP_BOSS_MAIN;
	notifyObservers(&obParam);
}

void GroupBossModel::updateBossCanGetInfo( const Json::Value& data )
{
	//已领取过的
	for(int i=0; i<data.size(); i++)
	{
		for(int j=0; j<m_vDamReward.size(); j++)
		{
			if(data[i].asInt() == m_vDamReward.at(j).boxId)
			{
				m_vDamReward.at(j).isGetted = true;
				break;
			}
		}
		
	}

	//可领取的
	for(int i=0; i<m_vDamReward.size(); i++)
	{
		if(m_iMyTotalDam >= m_vDamReward.at(i).damLimit && !m_vDamReward.at(i).isGetted)
		{
			m_vDamReward.at(i).isCanGet = true;
		}
	}

	sortDamRewardData();
}

void GroupBossModel::sortDamRewardData()
{
	//排序，已领取的放后面
	std::sort(m_vDamReward.begin(), m_vDamReward.end(), 
		[](const sDamReward& p1, const sDamReward& p2)->bool
	{
		//可领取的在前面
		if(p1.isCanGet && !p2.isCanGet)
		{
			return true;
		}
		else if(!p1.isCanGet && p2.isCanGet)
		{
			return false;
		}
		else if(p1.isCanGet && p2.isCanGet)
		{
			return p1.boxId < p2.boxId;
		}
		else
		{
			//都不可领取时
			if(p1.isGetted && !p2.isGetted)
			{
				return false;
			}
			else if(!p1.isGetted && p2.isGetted)
			{
				return true;
			}
			else
			{
				return p1.boxId < p2.boxId;
			}
		}
	});
}

void GroupBossModel::updateExchangeCanInfo( const Json::Value& data )
{
	m_vExchangeJn.clear();

	for (int i = 0; i < data.size(); i++)
	{
		auto oneItem = data[i];
		sExchangeJn son;

		son.boxId = oneItem["firstId"].asInt();
		son.firstId = JINNANGID;
		son.firstCount = oneItem["price"].asInt();
		son.secondId = oneItem["id"].asInt();
		son.secondCount = oneItem["amount"].asInt();

		RowData* _row = DataManager::getInstance()->searchToolById(son.secondId);	
		
		son.name =  (_row == nullptr) ? "" : _row->getStringData("itemName");
		
		m_vExchangeJn.push_back(son);
	}

	//按照兑换价格排序
	std::sort(m_vExchangeJn.begin(), m_vExchangeJn.end(), 
		[](const sExchangeJn& p1, const sExchangeJn& p2)->bool
	{
		return p1.firstCount < p2.firstCount;
	});
}

void GroupBossModel::updateMainGroupRankData( const Json::Value& data )
{
	Json::Value rankList = data["PMlist"];
	Json::Value myGroup = data["mylist"];

	m_vMainGroupRank.clear();
	for(int i=0; i<rankList.size(); i++)
	{
		sGroupDamRank groupDamItem;
		groupDamItem.rank = i+1;
		groupDamItem.groupName = rankList[i]["name"].asString();
		groupDamItem.killedBossNum = rankList[i]["deadcount"].asInt();
		groupDamItem.damageVal = rankList[i]["totalDamcount"].asString();
		m_vMainGroupRank.push_back(groupDamItem);
	}

	updateMyGroupRankData(myGroup);
}

void GroupBossModel::updateMainPlayerRankData( const Json::Value& data, eRankType type )
{
	Json::Value rankList = data["PMlist"];
	Json::Value myDam = data["mylist"];

	m_vMainPlayerRank.clear();

	for(int i=0; i<rankList.size(); i++)
	{
		sPlayerDamRank playerDamItem;
		playerDamItem.rank = i+1;
		playerDamItem.playerName = rankList[i]["name"].asString();
		playerDamItem.damageVal = rankList[i]["totalDamcount"].asString();
		if(!rankList[i]["viplevel"].isNull())
		{
			playerDamItem.vipLv = rankList[i]["viplevel"].asInt();
		}
		if(!rankList[i]["level"].isNull())
		{
			playerDamItem.level = rankList[i]["level"].asInt();
		}
		if(!rankList[i]["sexTyp"].isNull())
		{
			playerDamItem.headerResId = rankList[i]["sexTyp"].asInt() == 0 ? 41099 : 41098;
		}
		m_vMainPlayerRank.push_back(playerDamItem);
	}

	updateMyRankData(myDam, type);
}

void GroupBossModel::updateGroupRankData( const Json::Value& data )
{
	Json::Value rankList = data["PMlist"];
	Json::Value myGroup = data["mylist"];

	m_vGroupDamRank.clear();
	for(int i=0; i<rankList.size(); i++)
	{
		sGroupDamRank groupDamItem;
		groupDamItem.rank = i+1;
		groupDamItem.groupName = rankList[i]["name"].asString();
		groupDamItem.killedBossNum = rankList[i]["deadcount"].asInt();
		groupDamItem.damageVal = rankList[i]["totalDamcount"].asString();
		m_vGroupDamRank.push_back(groupDamItem);
	}

	updateMyGroupRankData(myGroup);
}

void GroupBossModel::updateMyGroupRankData( const Json::Value& data )
{
	m_sMyGroupDamRank.rank = data["rank"].asInt();
	m_sMyGroupDamRank.groupName = data["name"].asString();
	m_sMyGroupDamRank.killedBossNum = data["deadcount"].asInt();
	m_sMyGroupDamRank.damageVal = data["totalDamcount"].asString();
}

void GroupBossModel::updatePlayerRankData( const Json::Value& data, eRankType type )
{
	Json::Value rankList = data["PMlist"];
	Json::Value myDam = data["mylist"];

	m_vMainPlayerRank.clear();
	if(type == eTYPE_ALL)
	{
		m_vPlayerDamRank.clear();
	}
	else if(type == eTYPE_IN_GROUP)
	{
		m_vMemberDamRank.clear();
	}
	for(int i=0; i<rankList.size(); i++)
	{
		sPlayerDamRank playerDamItem;
		playerDamItem.rank = i+1;
		playerDamItem.playerName = rankList[i]["name"].asString();
		playerDamItem.damageVal = rankList[i]["totalDamcount"].asString();
		if(!rankList[i]["viplevel"].isNull())
		{
			playerDamItem.vipLv = rankList[i]["viplevel"].asInt();
		}
		if(!rankList[i]["level"].isNull())
		{
			playerDamItem.level = rankList[i]["level"].asInt();
		}
		if(!rankList[i]["sexTyp"].isNull())
		{
			playerDamItem.headerResId = rankList[i]["sexTyp"].asInt() == 0 ? 41099 : 41098;
		}
		m_vMainPlayerRank.push_back(playerDamItem);
		if(type == eTYPE_ALL)
		{
			m_vPlayerDamRank.push_back(playerDamItem);
		}
		else if(type == eTYPE_IN_GROUP)
		{
			m_vMemberDamRank.push_back(playerDamItem);
		}
	}

	updateMyRankData(myDam, type);
}

void GroupBossModel::updateMyRankData( const Json::Value& data, eRankType type )
{
	if(type == eTYPE_ALL)
	{
		m_sMyDamRank.rank = data["rank"].asInt();
		m_sMyDamRank.playerName = data["name"].asString();
		m_sMyDamRank.damageVal = data["totalDamcount"].asString();
	}
	else if(type == eTYPE_IN_GROUP)
	{
		m_sMyInGroupDamRank.rank = data["rank"].asInt();
		m_sMyInGroupDamRank.playerName = data["name"].asString();
		m_sMyInGroupDamRank.damageVal = data["totalDamcount"].asString();
	}
}

void GroupBossModel::updateKilledBossRewardData( const Json::Value& data )
{
	m_vBoxReward.clear();
	m_iRewardBossId = data["bossId"].asInt();
	RowData* rowData = DataManager::getInstance()->searchMonsterById(m_iRewardBossId);
	CCASSERT(rowData != nullptr, "bossId no valid!");
	string bossName = rowData->getStringData("name");

	//已领取
	Json::Value gettedList = data["rewardedList"];
	for(int i=0; i<gettedList.size(); i++)
	{
		sBoxReward boxRewardData;
		boxRewardData.boxId = gettedList[i].asInt();
		boxRewardData.isCanGet = false;
		boxRewardData.isGetted = true;
		boxRewardData.bossName = bossName;
		parseBossRewardData(boxRewardData.boxId, boxRewardData.vBoxRewardInfo);
		m_vBoxReward.push_back(boxRewardData);
	}

	//可领取
	Json::Value canGetList = data["rewardList"];
	for(int i=0; i<canGetList.size(); i++)
	{
		sBoxReward boxRewardData;
		boxRewardData.boxId = canGetList[i].asInt();
		boxRewardData.isCanGet = true;
		boxRewardData.isGetted = false;
		boxRewardData.bossName = bossName;
		parseBossRewardData(boxRewardData.boxId, boxRewardData.vBoxRewardInfo);
		m_vBoxReward.push_back(boxRewardData);
	}

	//未领取同时不可领取
	int bossOrder = m_sBossInfo.bossOrder;
	int bossRewardMaxId = bossOrder*5 + BOSS_REWARD_START_ID - 1;
	int bossRewardMinId = bossRewardMaxId - 4;
	for(int id = bossRewardMinId; id < bossRewardMaxId+1; id++)
	{
		if(isRewardCanNotGetState(id))
		{
			sBoxReward boxRewardData;
			boxRewardData.boxId = id;
			boxRewardData.isCanGet = false;
			boxRewardData.isGetted = false;
			boxRewardData.bossName = bossName;
			parseBossRewardData(boxRewardData.boxId, boxRewardData.vBoxRewardInfo);
			m_vBoxReward.push_back(boxRewardData);
		}
	}
	
	//按照ID进行排序
	std::sort(m_vBoxReward.begin(), m_vBoxReward.end(), 
		[](const sBoxReward& p1, const sBoxReward& p2)->bool
	{
		return p1.boxId < p2.boxId;
	});

	if(!data["changeInfo"].isNull())
	{
		//更新changeInfo
		Json::Value changeInfo = data["changeInfo"];

		//英雄数据
		Json::Value petLists = changeInfo["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = changeInfo["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

		//金币，银币
		int goldAdd = 0;
		int coinAdd = 0;
		int oldGoldNum = MainModel::getInstance()->getMainParam()->mGold;
		int oldCoinNum = MainModel::getInstance()->getMainParam()->mCoin;

		MainModel::getInstance()->updateData(changeInfo);

		int newGoldNum = MainModel::getInstance()->getMainParam()->mGold;
		int newCoinNum = MainModel::getInstance()->getMainParam()->mCoin;

		if(newGoldNum > oldGoldNum)
		{
			goldAdd = newGoldNum - oldGoldNum;
			TOOL_NEW_INFO goldItem;
			goldItem.ntemplateId = GOLD_SYSTEM_ID;
			goldItem.nstack = goldAdd;
			vecNewTools.push_back(goldItem);
		}
		if(newCoinNum > oldCoinNum)
		{
			coinAdd = newCoinNum - oldCoinNum;
			TOOL_NEW_INFO coinItem;
			coinItem.ntemplateId = COIN_SYSTEM_ID;
			coinItem.nstack = coinAdd;
			vecNewTools.push_back(coinItem);
		}

		//通知UI, 领取了boss宝箱奖励
		notifyUiLayer(nMAIN_CMD_GROUP_BOSS_GET_BOSS_REWARD, &vecNewTools);
	}
}

void GroupBossModel::parseBossRewardData( int boxId, vector<sRewardItemData>& vecReward )
{
	RowData* boxData = DataManager::getInstance()->searchGroupBossById(boxId);
	if(boxData != nullptr)
	{
		string boxInfo = boxData->getStringData("item");
		if(!boxInfo.empty())
		{
			vector<string> vecStr;
			StringFormat::parseCsvStringVec(boxInfo, vecStr);

			for(string strTmp : vecStr)
			{
				int nIndex1 = strTmp.find_first_of(',', 0);
				string strTmp1 = strTmp.substr(0, nIndex1);
				string strTmp2 = strTmp.erase(0, nIndex1 + 1);

				sRewardItemData rewardInfoItem;
				rewardInfoItem.rewardId = atoi(strTmp1.c_str());
				rewardInfoItem.rewardCnt = atoi(strTmp2.c_str());

				vecReward.push_back(rewardInfoItem);
			}
		}
	}
}


void GroupBossModel::updateBossInfo( const Json::Value& data )
{
	m_iRestTime = data["remaining_time"].asInt();
	m_bIsActOpen = m_iRestTime > 0 ? true : false;
	m_iRestAtkTimes = data["remaining_challenge"].asInt();
	m_iInspireTimes = data["guwuTimes"].asInt();
	m_iMyTotalDam = data["totalDamcount"].asUInt();

	m_sBossInfo.bossOrder = data["bossNum"].asInt();
	m_sBossInfo.totalBossNum = data["totalBossNum"].asInt();
	m_sBossInfo.bossId = data["bossId"].asInt();
	m_sBossInfo.bossIdx = data["bossIdx"].asInt();

	RowData* rowData = DataManager::getInstance()->searchMonsterById(m_sBossInfo.bossId);
	CCASSERT(rowData != nullptr, "bossId no valid!");
	m_sBossInfo.bossName = rowData->getStringData("name");
	m_sBossInfo.bossLv = rowData->getIntData("lvl");
	m_sBossInfo.totalHp = rowData->getLongData("hp");
	m_sBossInfo.curHp = m_sBossInfo.totalHp - data["bossTotalDam"].asUInt();
	m_sBossInfo.totalReviveTimes = BOSS_MAX_LIFE;
	m_sBossInfo.restReviveTimes = m_sBossInfo.totalReviveTimes - data["bossDeadNum"].asInt();
	m_sBossInfo.bossNickResId = rowData->getIntData("resID");
	m_sBossInfo.bossResId = m_sBossInfo.bossNickResId;
	if(m_bIsNeedUpdateCoolTime)
	{
		m_sBossInfo.reviveCoolTime = data["cooltime"].asInt() + 1;		//延时1s，防止前后端时间误差
	}

	//冷却定时器
	if(m_sBossInfo.reviveCoolTime > 0)
	{
		m_bIsCanAtk = false;
		setTimeStamp(LocalTimer::getInstance()->getCurServerTime() + m_sBossInfo.reviveCoolTime);
		LocalTimer::getInstance()->addTimeStampListener(this);
	}
	else
	{
		m_bIsCanAtk = true;
	}
}

void GroupBossModel::updateInspireInfo( const Json::Value& data )
{
	//上报TalkingData
	if(m_iInspireTimes < m_vInspireCost.size())
	{
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10165), 1, m_vInspireCost.at(m_iInspireTimes));
	}
	
	m_fDamAdded = data["atkInc"].asDouble();
	m_iInspireTimes = data["guwuTimes"].asInt();

	//更新金币
	MainModel::getInstance()->updateData(data);

	//通知UI, 进行了鼓舞
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_GROUP_BOSS_INSPIRE;
	notifyObservers(&obParam);
}

void GroupBossModel::updateRankData( const Json::Value& data )
{
	if(m_rankTemp == eTYPE_GROUP)
	{
		updateGroupRankData(data);
	}
	else if(m_rankTemp == eTYPE_ALL)
	{
		updatePlayerRankData(data, eTYPE_ALL);
	}
	else if(m_rankTemp == eTYPE_IN_GROUP)
	{
		updatePlayerRankData(data, eTYPE_IN_GROUP);
	}
	notifyUiLayer(nMAIN_CMD_GROUP_BOSS_RANK);
}

void GroupBossModel::updateGetRewardInfo( const Json::Value& data )
{
	//更新已领取的奖励ID
	Json::Value rewardList = data["reward"];
	for(int i=0; i<rewardList.size(); i++)
	{
		for(int j=0; j<m_vDamReward.size(); j++)
		{
			if(rewardList[i].asInt() == m_vDamReward.at(j).boxId)
			{
				m_vDamReward.at(j).isGetted = true;
				m_vDamReward.at(j).isCanGet = false;
				break;
			}
		}
	}

	Json::Value changeInfo = data["changeInfo"];

	//道具数据
	vector<TOOL_NEW_INFO> vecNewTools;
	Json::Value items = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

	//更新changeInfo

	MainModel::getInstance()->updateData(changeInfo);

	//英雄数据
	Json::Value petLists = changeInfo["petList"];
	HeroModel::getInstance()->updateData(petLists);


	notifyUiLayer(nMAIN_CMD_GROUP_BOSS_GET_REWARD, &vecNewTools);
}

void GroupBossModel::updateExchangeJnang( const Json::Value& data )
{
	//更新changeInfo
	Json::Value changeInfo = data["changeInfo"];

	//英雄数据
	Json::Value petLists = changeInfo["petList"];
	HeroModel::getInstance()->updateData(petLists);

	//道具数据
	vector<TOOL_NEW_INFO> vecNewTools;
	Json::Value items = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

	MainModel::getInstance()->updateData(changeInfo);

	notifyUiLayer(nMAIN_CMD_GROUP_BOSS_EXCHANGE, &vecNewTools);
}

sDamReward* GroupBossModel::getDamRewardByID( int boxId )
{
	for(int i = 0; i != m_vDamReward.size(); ++i)
	{
		if(m_vDamReward.at(i).boxId == boxId)
		{
			return &(m_vDamReward.at(i));
		}
	}

	return nullptr;
}

sBoxReward* GroupBossModel::getBoxRewardByID( int boxId )
{
	for(int i = 0; i != m_vBoxReward.size(); ++i)
	{
		if(m_vBoxReward.at(i).boxId == boxId)
		{
			return &(m_vBoxReward.at(i));
		}
	}

	return nullptr;
}

vector<sGroupDamRank>* GroupBossModel::getGroupRankData()
{
	return &m_vGroupDamRank;
}

sGroupDamRank* GroupBossModel::getMyGroupRankData()
{
	return &m_sMyGroupDamRank;
}

vector<sPlayerDamRank>* GroupBossModel::getPlayerRankData( eRankType type )
{
	return &(type == eTYPE_ALL ? m_vPlayerDamRank : m_vMemberDamRank);
}

sPlayerDamRank* GroupBossModel::getMyRankData( eRankType type )
{
	return type == eTYPE_ALL ? &m_sMyDamRank : &m_sMyInGroupDamRank;
}

int GroupBossModel::getRestAtkCount()
{
	return m_iRestAtkTimes;
}

int GroupBossModel::getInspireCount()
{
	return m_iInspireTimes;
}

sGroupBossInfo* GroupBossModel::getBossInfo()
{
	return &m_sBossInfo;
}

time_t GroupBossModel::getMyTotalDam()
{
	return m_iMyTotalDam;
}

bool GroupBossModel::isActOpen()
{
	return m_bIsActOpen;
}

bool GroupBossModel::isCanGetBossReward()
{
	for(auto iter = m_vBoxReward.begin(); iter != m_vBoxReward.end(); ++iter)
	{
		if(iter->isCanGet)
		{
			return true;
		}
	}

	return false;
}

bool GroupBossModel::isCanGetDamReward()
{
	for(int i=0; i<m_vDamReward.size(); i++)
	{
		if(m_vDamReward.at(i).isCanGet)
		{
			return true;
		}
	}

	return false;
}

bool GroupBossModel::isCanExchange()
{
	//获取道具数量
	const TOOL_INFO* info2 = ToolModel::getInstance()->GetToolInfoBytemplateId(JIN_NANG_TOOL_ID);
	if(info2 != nullptr && !m_vExchangeJn.empty())
	{
		return info2->nstack >= m_vExchangeJn.front().firstCount;
	}

	return false;
}

bool GroupBossModel::isCanExchangeByID( int boxId )
{
	for(int i=0; i<m_vExchangeJn.size(); i++)
	{
		if(m_vExchangeJn.at(i).boxId == boxId)
		{
			//获取道具数量
			const TOOL_INFO* info2 = ToolModel::getInstance()->GetToolInfoBytemplateId(JIN_NANG_TOOL_ID);
			if(info2 != nullptr)
			{
				return info2->nstack >= m_vExchangeJn.at(i).firstCount;
			}

			return false;
		}
	}

	return false;
}

sExchangeJn* GroupBossModel::getExchangeJnByID( int boxId )
{
	for(int i=0; i<m_vExchangeJn.size(); i++)
	{
		if(boxId == m_vExchangeJn.at(i).boxId)
		{
			return &m_vExchangeJn.at(i);
		}
	}

	return nullptr;
}

int GroupBossModel::getNextCost()
{
	if(m_iInspireTimes < m_vInspireCost.size())
	{
		return m_vInspireCost.at(m_iInspireTimes);
	}
	
	return -1;
}

int GroupBossModel::getMaxInspireCount()
{
	return m_vInspireCost.size();
}

vector<sBoxReward>* GroupBossModel::getBoxReward()
{
	return &m_vBoxReward;
}

std::vector<sDamReward>* GroupBossModel::getDamReward()
{
	return &m_vDamReward;
}

std::vector<sExchangeJn>* GroupBossModel::getExchangeJn()
{
	return &m_vExchangeJn;
}

int GroupBossModel::getCurRewardBossId()
{
	return m_iRewardBossId;
}

float GroupBossModel::getDamAdded()
{
	//鼓舞伤害加成
	RowData* inspireBuffAdd = DataManager::getInstance()->getDTToolSystem()->searchDataById(INSPIRE_BUFF_ADD_ID);
	if(inspireBuffAdd != nullptr)
	{
		float singleAdd = inspireBuffAdd->getFloatData("value");
		return m_iInspireTimes*(singleAdd*100);
	}

	return 0;
}

bool GroupBossModel::isBossCanAtk()
{
	return m_bIsCanAtk;
}

int& GroupBossModel::getRestTime()
{
	return m_iRestTime;
}

void GroupBossModel::timeOut()
{
	//boss可攻击
	m_bIsCanAtk = true;
}

void GroupBossModel::setIsNeedUpdateCoolTime( bool isNeed )
{
	m_bIsNeedUpdateCoolTime = isNeed;
}

bool GroupBossModel::isRewardCanNotGetState( int boxId )
{
	for(int i=0; i<m_vBoxReward.size(); i++)
	{
		if(boxId == m_vBoxReward.at(i).boxId)
		{
			return false;
		}
	}

	return true;
}

bool GroupBossModel::getIsNeedUpdateCoolTime()
{
	return m_bIsNeedUpdateCoolTime;
}

vector<sGroupDamRank>* GroupBossModel::getMainGroupRankData()
{
	return &m_vMainGroupRank;
}

vector<sPlayerDamRank>* GroupBossModel::getMainPlayerRankData()
{
	return &m_vMainPlayerRank;
}

void GroupBossModel::notifyUiLayer(int type, void* data )
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}



















