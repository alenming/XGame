#include "BossModel.h"
#include "DataManager/DataManager.h"
#include "Communication/Command.h"

BossModel* BossModel::ms_pInstance = nullptr;

BossModel::BossModel()
{
	for(int i=0; i<3; i++)
	{
		m_sTop[i].rankId = 0;
		m_sTop[i].damage = "";
		m_sTop[i].name = "";
	}
	m_bIsOpen = false;
	m_iSeconds = 0;
	m_vBossInfoFromSvr.clear();
	m_iDamageAdd = 0;
	m_iInspireTimes = 0;
	m_iAtkBossId = 0;
	m_myTotalDamage = "";
	m_iMyRank = 0;
	m_iTotalDamage = 0;
	m_iCurSeledBoss = 0;
	m_iCurInspireCost = 0; 
	m_iCurClearCoolCost = 0;
	m_bIsFightSettement = false;
	m_iIsEndActive = false;
	m_vLastRankList.clear();
	m_vRewardIDList.clear();
	m_vRewardIDInfo.clear();
}

BossModel::~BossModel()
{

}

void BossModel::setRewardId()
{
	m_vRewardIDInfo.clear();
	for(int i=BOSS_REWARD_ID_BEGIN; i<=BOSS_REWARD_ID_END; i++)
	{	
		RowData* rowData = DataManager::getInstance()->searchBossRewardById(i);
		sRewardIDInfo single;
		single.rewardID = i;
		single.isCanGetted = false;
		if(m_iTotalDamage >= rowData->getIntData("damage"))
		{
			single.isCanGetted = true;
		}
		m_vRewardIDInfo.push_back(single);
	}
}

void BossModel::resetRewardId()
{
	for(int i=0; i<m_vRewardIDInfo.size(); i++)
	{
		for(int j=0; j<m_vRewardIDList.size(); j++)
		{
			if(m_vRewardIDList.at(j) == m_vRewardIDInfo.at(i).rewardID)
			{
				m_vRewardIDInfo.at(i).isCanGetted = false;
			}
		}
	}
}

void BossModel::parseEndLastFightToScene(const Json::Value& data)
{
	Json::Value info = data["info"];
	Json::Value topList = info["top3"];
	Json::Value bossList = info["bossInfo"];
	Json::Value rewardList = info["reward"];

	//BossModel::getInstance()->m_bIsOpen = data["openflag"].asInt();

	for(int i=0; i<topList.size(); i++)
	{
		//伤害前三名
		Json::Value damList = topList[i];
		BossModel::getInstance()->m_sTop[i].rankId = i+1;
		BossModel::getInstance()->m_sTop[i].name = damList["name"].asString();
		BossModel::getInstance()->m_sTop[i].damage = damList["totalDam"].asString();
	}	

	BossModel::getInstance()->m_iDamageAdd = info["atkInc"].asDouble();
	BossModel::getInstance()->m_iFightCoolSeconds = info["CDSeconds"].asInt();
	BossModel::getInstance()->m_iInspireTimes = info["guwuTimes"].asInt();
	BossModel::getInstance()->m_iMyRank = info["myOrderNum"].asInt();
	BossModel::getInstance()->m_myTotalDamage = info["myTotalDam"].asString();
	//BossModel::getInstance()->m_iTotalDamage = data["totalDam"].asInt();

	BossModel::getInstance()->m_vBossInfoFromSvr.clear();
	for(int i=0; i<bossList.size(); i++)
	{
		sBossInfoFromSvr bossDetails;
		Json::Value bossInfomation = bossList[i];
		bossDetails.bossHp = bossInfomation["hp"].asInt();
		bossDetails.bossId = bossInfomation["id"].asInt();
		bossDetails.needLv = bossInfomation["needLvl"].asInt();
		bossDetails.rewardCoin = bossInfomation["rewardCoin"].asInt();
		bossDetails.state = bossInfomation["stat"].asInt();
		bossDetails.bossName = bossInfomation["name"].asString();
		bossDetails.killer = bossInfomation["killer"].asString();
		bossDetails.attribute = bossInfomation["prop"].asInt();
		BossModel::getInstance()->m_vBossInfoFromSvr.push_back(bossDetails);
	}

	//已经领取过的奖励列表
	BossModel::getInstance()->m_vRewardIDList.clear();
	for(int i=0; i<rewardList.size(); i++)
	{
		BossModel::getInstance()->m_vRewardIDList.push_back(rewardList[i].asInt());
	}

	BossModel::getInstance()->setRewardId();	

	//通知UI, boss战斗结算
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_BOSS_SETTLEMENT;
	BossModel::getInstance()->notifyObservers(&obParam);
	BossModel::getInstance()->m_bIsFightSettement = false;
}

BossModel* BossModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new BossModel();
	}
	return ms_pInstance;
}

void BossModel::destroyInstance()
{
	if(ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}