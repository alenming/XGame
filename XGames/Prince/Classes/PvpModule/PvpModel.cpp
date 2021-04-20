#include "PvpModel.h"

PvpModel* PvpModel::mInstance = nullptr;

PvpModel::PvpModel()
{
	m_rewardPvp.clear();
	m_rewardWIN.clear();
	m_pvpCurOrder = 0;
	m_pvpBestOrder = 0;
	m_pvpCurOrderInc = 0;
	m_pvpBestOrderInc = 0;
	m_goldCount = 0;
	m_pvpScore = 0;
	m_changeFlag = 0;
	m_pvpWinCount = 0;
	m_pvpCount = 0;
	m_win = false;
	m_buyCount = 0;
	parsePvpTable();
}

PvpModel::~PvpModel()
{

}

void PvpModel::parsePvpTable()
{
	for (int i = 0; ; i++)
	{
		RowData *pvpData = DataManager::getInstance()->searchArenaRewordById(i+1);
		if (pvpData)
		{
			if (pvpData->getIntData("rewardType") == PVP_RANK_TYPE_PVP)
			{
				PVP_REWARD_ITEM iteminfo;
				iteminfo.top = pvpData->getIntData("top");
				iteminfo.bot = pvpData->getIntData("bot");

				vector<string> vecDst;
				StringFormat::parseCsvStringVec(pvpData->getStringData("itemList"), vecDst);
				for (int k = 0; k < vecDst.size(); k++)
				{
					vector<int> _vecDst;
					StringFormat::parseCsvStringVecByDu(vecDst[k], _vecDst);

					PVP_REWARD_NODE node;
					node.templateId = _vecDst[0];
					node.amount = _vecDst[1];
					iteminfo.prize.push_back(node);
				}


				m_rewardPvp.push_back(iteminfo);
			}
			else if (pvpData->getIntData("rewardType") == PVP_RANK_TYPE_WIN)
			{
				PVP_REWARD_ITEM iteminfo;
				iteminfo.top = pvpData->getIntData("top");
				iteminfo.bot = pvpData->getIntData("bot");

				vector<string> vecDst;
				StringFormat::parseCsvStringVec(pvpData->getStringData("itemList"), vecDst);
				for (int k = 0; k < vecDst.size(); k++)
				{
					vector<int> _vecDst;
					StringFormat::parseCsvStringVecByDu(vecDst[k], _vecDst);

					PVP_REWARD_NODE node;
					node.templateId = _vecDst[0];
					node.amount = _vecDst[1];
					iteminfo.prize.push_back(node);
				}

				m_rewardWIN.push_back(iteminfo);
			}
		}
		else
		{
			break;
		}
	}
}

PvpModel* PvpModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new PvpModel();
	}
	return mInstance;
}

void PvpModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void PvpModel::notify(PVP_EVENT_TYPE eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void PvpModel::parsePvpInfoData(const Json::Value &data)
{
	//玩家信息
	m_pvpScore = data["pvpscore"].asInt();
	m_pvpCurOrder = data["pvporder"].asInt();
	m_pvpBestOrder = data["bestorder"].asInt();
	m_changeFlag = data["changeFlag"].asInt();
	m_pvpCount = data["lessCount"].asInt();
	m_buyCount = data["buyCount"].asInt();

	MainModel::getInstance()->setEnergy(data["energy"].asInt());
	MainModel::getInstance()->getMainParam()->mPvpScore = data["pvpscore"].asInt();
	MainModel::getInstance()->notifyObservers();

	Json::Value pvpList = data["pvpList"];

	//对手英雄信息
	m_player.clear();
	for (int i = 0; i < pvpList.size(); i++)
	{
		PVP_PLAYER_DATA player;
		player.resID = pvpList[i]["resID"].asInt();
		player.name = pvpList[i]["name"].asString();
		player.level = pvpList[i]["level"].asInt();
		player.atk = pvpList[i]["atk"].asInt();
		player.pvporder = pvpList[i]["pvporder"].asInt();
		player.vipLv = pvpList[i]["vipLevel"].asInt();
		player.groupName = pvpList[i]["group"].asString();

		for (int k = 0; k < pvpList[i]["matrixIds"].size(); k++ )
		{
			matrixData singleMatrix;
			singleMatrix.id = pvpList[i]["matrixIds"][k]["id"].asInt();
			singleMatrix.stars = pvpList[i]["matrixIds"][k]["stars"].asInt();
			player.matrixIds.push_back(singleMatrix);
		}
		player.id = pvpList[i]["id"].asInt();

		m_player.push_back(player);
	}

	m_pvpWinCount = data["pvpdaycount"].asInt();
	m_rewardFlag.clear();
	for (int i = 0; i < data["rewardflag"].size(); i++)
	{
		m_rewardFlag.push_back(data["rewardflag"][i].asInt());
	}


	notify(PVP_EVENT_TYPE_GET_PVP_INFO);
}

void PvpModel::parsepvpSetMentAccountsData(const Json::Value &data)
{
	m_pvpCurOrder = data["curOrderNum"].asInt();
	m_pvpBestOrder = data["histOrderNum"].asInt();
	m_pvpCurOrderInc = data["curOrderInc"].asInt();
	m_pvpBestOrderInc = data["bestOrderInc"].asInt();
	m_goldCount = data["rewardGold"].asInt();
	m_pvpWinCount = data["pvpdaycount"].asInt();

	m_rewardFlag.clear();
	for (int i = 0; i < data["rewardflag"].size(); i++)
	{
		m_rewardFlag.push_back(data["rewardflag"][i].asInt());
	}
}

void PvpModel::parsepvpRankData(const Json::Value &data)
{
	//排名信息
	m_rank.clear();
	for (int i = 0; i < data.size(); i++)
	{
		PVP_PLAYER_DATA rankNode;
		rankNode.resID = data[i]["resID"].asInt();
		rankNode.name = data[i]["name"].asString();
		rankNode.level = data[i]["level"].asInt();
		rankNode.atk = data[i]["atk"].asInt();
		rankNode.pvporder = data[i]["orderNum"].asInt();

		for (int k = 0; k < data[i]["matrixIds"].size(); k++)
		{
			matrixData singleMatrix;
			singleMatrix.id = data[i]["matrixIds"][k]["id"].asInt();
			singleMatrix.stars = data[i]["matrixIds"][k]["stars"].asInt();
			rankNode.matrixIds.push_back(singleMatrix);
		}
		rankNode.id = data[i]["id"].asInt();

		m_rank.push_back(rankNode);
	}

	notify(PVP_EVENT_TYPE_RANK);
}

void PvpModel::parsePvpPrizeData(const Json::Value &data)
{
	Json::Value changeInfo = data["changeInfo"];

	m_rewardShow.mCoin = changeInfo["coin"].asInt() - MainModel::getInstance()->getMainParam()->mCoin;
	m_rewardShow.mGold = changeInfo["gold"].asInt() - MainModel::getInstance()->getMainParam()->mGold;

	//道具数据
	vector<TOOL_NEW_INFO> vecNewTools;
	Json::Value items = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);
	m_rewardShow.mVecRewardInfo.clear();

	for (int i = 0; i < vecNewTools.size(); i++)
	{
		PVP_SignRewardInfo itemIfo;
		itemIfo.mItemId = vecNewTools[i].ntemplateId;
		itemIfo.mItemCount = vecNewTools[i].nstack;

		m_rewardShow.mVecRewardInfo.push_back(itemIfo);
	}

	//积分
	PVP_SignRewardInfo itemIfo;
	itemIfo.mItemId = 11003; 
	itemIfo.mItemCount = changeInfo["score"].asInt() - MainModel::getInstance()->getMainParam()->mPvpScore;

	m_rewardShow.mVecRewardInfo.push_back(itemIfo);

	//主数据
	MainModel::getInstance()->updateData(changeInfo);

	//英雄数据
	Json::Value petLists = changeInfo["petList"];
	HeroModel::getInstance()->updateData(petLists);

	//通知初始化
	notify(PVP_EVENT_TYPE_WIN_PRIZE);
}

void PvpModel::parsePvpCount(const Json::Value &data)
{
	m_pvpCount = data["lessCount"].asInt();
	m_buyCount = data["buyCount"].asInt();

	//主数据
	MainModel::getInstance()->updateData(data);

	//通知初始化
	notify(PVP_EVENT_TYPE_BUY_COUNT);
}
