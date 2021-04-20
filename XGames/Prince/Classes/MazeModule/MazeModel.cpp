#include "MazeModel.h"


#include "Common/Common.h"
#include "MazeDef.h"
#include "MainModule/MainModel.h"
#include "MazeController.h"

#include "cocos2d.h"
using namespace cocos2d;

MazeModel*	MazeModel::ms_pInstance	= nullptr;

MazeModel::MazeModel()
{
	m_nCurEventId = 0;
	m_MazeInfos.mazeId = 0;
	clearMazEvent();
}

MazeModel* MazeModel::getInstance()
{
	if (nullptr == ms_pInstance)
	{
		ms_pInstance = new MazeModel();		
	}
	return ms_pInstance;
}

void MazeModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void MazeModel::clearMazeInfo()
{
	//m_MazeInfos.clear();
	m_MazeInfos.mazeId = 0;
}

MAZE_INFOS& MazeModel::getMazeInfos()
{
	return m_MazeInfos;
}

void MazeModel::setTotleStep(const int& step)
{
	//m_MazeInfos.totalStep = step;
}

void MazeModel::setCurPower(const int& power)
{
	m_MazeInfos.curPower = power;
}

void MazeModel::setTimes(const int& times)
{
	m_MazeInfos.times = times;
}

void MazeModel::clearMazEvent()
{
	m_EventId = 0;
}
void MazeModel::setEventId(const int& eventId)
{
	m_EventId = eventId;
}
int MazeModel::getEventId()
{
	return m_EventId;
}

void MazeModel::clearMazeDropInfo()
{
	m_mazeDropInfo.getCoin = 0;
	m_mazeDropInfo.getExp = 0;
	m_mazeDropInfo.vecNewTools.clear();
}
MAZE_DROP_INFO& MazeModel::getMazeDropInfo()
{
	return m_mazeDropInfo;
}


void MazeModel::processMazesData(Json::Value data)
{
	Maze_ReWard_Show rewardShowNew;

	Json::Value changeInfo = data["changeInfo"];

	rewardShowNew.mCoin = data["getCoin"].asInt();
	rewardShowNew.mGold = data["getGold"].asInt();
	rewardShowNew.mExp = data["getExp"].asInt();
	//道具奖励
	for (int i = 0; i < data["getItemList"].size(); i++)
	{
		MazeRewardInfo itemIfo;
		itemIfo.mItemId = data["getItemList"][i]["id"].asInt();
		itemIfo.mItemCount = data["getItemList"][i]["amount"].asInt();

		rewardShowNew.mVecRewardInfo.push_back(itemIfo);
	}

	//主数据
	MainModel::getInstance()->updateData(changeInfo);

	//英雄数据
	Json::Value petLists = changeInfo["petList"];
	HeroModel::getInstance()->updateData(petLists);

	//道具数据
	vector<TOOL_NEW_INFO> vecNewTools;
	Json::Value items = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);


	m_MazeInfos.curPower = data["movePower"].asInt();
	MainModel::getInstance()->getMainParam()->mMazePower = m_MazeInfos.curPower;
	MainModel::getInstance()->notifyObservers();

	for (int i = 0; i < m_MazeInfos.vecMaps.size(); i++)
	{
		for (int k = 0 ; k < data["maplist"].size(); k++)
		{
			if (m_MazeInfos.vecMaps.at(i).id == data["maplist"][i]["id"].asInt())
			{
				if (data["maplist"][i]["count"].asInt()-m_MazeInfos.vecMaps.at(i).num > 0)
				{
					//是否有藏宝图
					MazeRewardInfo itemIfo;
					itemIfo.mItemId = data["maplist"][i]["id"].asInt();
					itemIfo.mItemCount = data["maplist"][i]["count"].asInt()-m_MazeInfos.vecMaps.at(i).num;

					rewardShowNew.mVecRewardMap.push_back(itemIfo);
				}

				m_MazeInfos.vecMaps.at(i).num = data["maplist"][i]["count"].asInt();
			}
		}
	}

	MazeController::getInstance()->onGetView()->onRewardResult(rewardShowNew);
}

void MazeModel::processMazesOpenCardData(Json::Value data)
{
	Json::Value changeInfo = data["changeInfo"];

	//主数据
	MainModel::getInstance()->updateData(changeInfo);

	//英雄数据
	Json::Value petLists = changeInfo["petList"];
	HeroModel::getInstance()->updateData(petLists);

	//道具数据
	vector<TOOL_NEW_INFO> vecNewTools;
	Json::Value items = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

	CHOOSEED_ITEM_REWARD reward;
	reward.openCount = data["openCount"].asInt();
	reward.drawItemId = data["drawItemId"].asInt();
	reward.drawItemCount = data["drawItemCount"].asInt();
	reward.pos = data["pos"].asInt();

	MazeController::getInstance()->onGetView()->onRewardOpenCardResult(reward);
}

void MazeModel::setCurEventId(const int& id)
{
	m_nCurEventId = id;
}
int MazeModel::getCurEventId()
{
	return m_nCurEventId;
}

