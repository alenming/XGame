#include "RankTopModel.h"
#include "RankTopLayer.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "MainModule/MainModel.h"
#include "Common/Common.h"
#include "Communication/Command.h"

RankTopModel* RankTopModel::ms_pInstance = nullptr;

RankTopModel::RankTopModel()
{
	m_vLevelRankTop.clear();
	m_vPowerRankTop.clear();
	m_vPvpRankTop.clear();
	m_vGroupRankTop.clear();
	mIsGetLevelRank = false;
	mIsGetPowerRank = false;
	m_curRankTopType = LEVEL_RANK_TOP;		//默认第一项，等级榜
}

RankTopModel::~RankTopModel()
{

}

RankTopModel* RankTopModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new RankTopModel();
	}
	return ms_pInstance;
}

void RankTopModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void RankTopModel::initFromSvr(const Json::Value& rankTop)
{
	int rankType = rankTop["type"].asInt();
	Json::Value rankTopData = rankTop["topInfo"];

	if(rankType == LEVEL_RANK_TOP)
	{
		m_vLevelRankTop.clear();
		mIsGetLevelRank = true;

		m_myRankInfo.myLevel = rankTop["myTotalAtk"].asInt();
		m_myRankInfo.myLevelRank = rankTop["myTopId"].asInt();	
	}
	else if(rankType == POWER_RANK_TOP)
	{
		m_vPowerRankTop.clear();
		mIsGetPowerRank = true;

		m_myRankInfo.myPower = rankTop["myTotalAtk"].asInt();
		m_myRankInfo.myPowerRank = rankTop["myTopId"].asInt();
	}
	else if(rankType == PVP_RANK_TOP)
	{
		m_vPvpRankTop.clear();

		m_myRankInfo.myPower = rankTop["myTotalAtk"].asInt();
		m_myRankInfo.myPvpRank = rankTop["myTopId"].asInt();
	}
	else if(rankType == GROUP_RANK_TOP)
	{
		m_vGroupRankTop.clear();

		m_myRankInfo.myGroupRank = rankTop["orderNum"].asInt();
		m_myRankInfo.myGroupPower = rankTop["totalAtk"].asInt();
	}

	for(int i=0; i<rankTopData.size(); i++)
	{
		
		sRankTopInfo rankInfo;
		rankInfo.rank = i+1;
		if(rankType != GROUP_RANK_TOP)
		{
			rankInfo.iconId = rankTopData[i]["petResId"].asInt();
			rankInfo.name = rankTopData[i]["nickname"].asString();	
			rankInfo.group = rankTopData[i]["group"].asString();
			rankInfo.level = rankTopData[i]["level"].asInt();
			rankInfo.power = rankTopData[i]["totalAtk"].asInt();
			rankInfo.uId = rankTopData[i]["characterId"].asInt();
			rankInfo.vipLv = rankTopData[i]["vipLevel"].asInt();
			Json::Value& matrixIds = rankTopData[i]["matrixIds"];
			for(int i=0; i<matrixIds.size(); i++)
			{
				matrixData singleMatrix;
				singleMatrix.id = matrixIds[i]["id"].asInt();
				singleMatrix.stars = matrixIds[i]["stars"].asInt();
				rankInfo.matrix.push_back(singleMatrix);
			}
		}
		else
		{
			//帮派时字段不一样......!!!!
			rankInfo.iconId = rankTopData[i]["resId"].asInt();
			rankInfo.name = rankTopData[i]["leader"].asString();	
			rankInfo.group = rankTopData[i]["name"].asString();
			rankInfo.level = rankTopData[i]["level"].asInt();
			rankInfo.power = rankTopData[i]["totalAtk"].asInt();
			rankInfo.uId = rankTopData[i]["id"].asInt();
			rankInfo.curNum = rankTopData[i]["curNum"].asInt();
			rankInfo.maxNum = rankTopData[i]["maxNum"].asInt();
			rankInfo.notice = rankTopData[i]["remark2"].asString();
		}
		
		if(rankType == LEVEL_RANK_TOP)
		{
			rankInfo.rankType = LEVEL_RANK_TOP;
			m_vLevelRankTop.push_back(rankInfo);			
		}
		else if(rankType == POWER_RANK_TOP)
		{
			rankInfo.rankType = POWER_RANK_TOP;
			m_vPowerRankTop.push_back(rankInfo);
		}
		else if(rankType == PVP_RANK_TOP)
		{
			rankInfo.rankType = PVP_RANK_TOP;
			m_vPvpRankTop.push_back(rankInfo);
		}
		else if(rankType == GROUP_RANK_TOP)
		{
			rankInfo.rankType = GROUP_RANK_TOP;
			m_vGroupRankTop.push_back(rankInfo);
		}
	}

	if(rankType == LEVEL_RANK_TOP || mIsGetLevelRank)
	{
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_RANK_TOP_LIST;
		notifyObservers(&obParam);
	}
}

vector<sRankTopInfo>& RankTopModel::getRankTopData(rankTopType rankType)
{
	if(rankType == LEVEL_RANK_TOP)
	{
		return m_vLevelRankTop;
	}
	else if(rankType == POWER_RANK_TOP)
	{
		return m_vPowerRankTop;
	}
	else if(rankType == PVP_RANK_TOP)
	{
		return m_vPvpRankTop;
	}
	else
	{
		return m_vGroupRankTop;
	}
}

sMyRankTopInfo& RankTopModel::getMyRankInfo()
{
	return m_myRankInfo;
}

void RankTopModel::setCurRankTopType(rankTopType type)
{
	m_curRankTopType = type;
}

rankTopType RankTopModel::getCurRankTopType()
{
	return m_curRankTopType;
}