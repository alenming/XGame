#include "LimitTimeHeroModel.h"
#include "SDK_TalkingData//TalkingData.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "Utils/LocalTimer.h"
#include "Utils/StringFormat.h"
#include "MainModule/MainModel.h"
#include "Communication/Command.h"

//抽卡、宝匣金币配置表id
const int takeCardOneCostId = 10129;
const int takeCardTenCostId = 10130;
const int takeBoxOneCostId = 10131;
const int takeBoxTenCostId = 10132;

LimitTimeHeroModel* LimitTimeHeroModel::ms_pInstance = nullptr;

LimitTimeHeroModel::LimitTimeHeroModel()
{
	m_tFreeChou = 0;
	m_iOneCost = 0;
	m_iTenCost = 0;

	m_bIsFree = false;
	m_bIsAddBuyLayer = false;
	m_bIsGettedTable = false;

	m_vScoreRank.clear();
	m_vItemParam.clear();
}

LimitTimeHeroModel::~LimitTimeHeroModel()
{

}

LimitTimeHeroModel* LimitTimeHeroModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new LimitTimeHeroModel();
	}

	return ms_pInstance;
}

void LimitTimeHeroModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

bool LimitTimeHeroModel::isCanBuyFree()
{
	return m_bIsFree;
}


bool LimitTimeHeroModel::isGettedTableData()
{
	return m_bIsGettedTable;
}

bool LimitTimeHeroModel::isCanGetScoreBox()
{
	m_sActData = getActData();
	if(m_sActData == nullptr)
	{
		return false;
	}

	for(auto box : m_sActData->vecBoxRewardData)
	{
		if(box.isCanGet)
		{
			return true;
		}
	}

	return false;
}

sActData* LimitTimeHeroModel::getActData()
{
	if(!m_vActList.empty())
	{
		m_sActData = &(m_vActList.front());
		return m_sActData;
	}
	
	return nullptr;
}

vector<sScoreRank>* LimitTimeHeroModel::getScoreRankData()
{
	return &m_vScoreRank;
}

vector<sRankRewardData>* LimitTimeHeroModel::getRankRewardData()
{
	m_sActData = getActData();
	return &(m_sActData->vecRankRewardData);;
}

sRewardBox* LimitTimeHeroModel::getRewardBoxById( int boxId )
{
	vector<sRewardBox>* vecBox = &(m_sActData->vecBoxRewardData);
	for(int i=0; i<vecBox->size(); ++i)
	{
		if(vecBox->at(i).boxId == boxId)
		{
			return &(vecBox->at(i));
		}
	}

	return nullptr;
}

vector<sGetItemParam>& LimitTimeHeroModel::getBuyItemParam()
{
	return m_vItemParam;
}

sScoreRank* LimitTimeHeroModel::getMyScoreData()
{
	return &m_sMyScoreRank;
}

int LimitTimeHeroModel::getCost( eChouKaType type )
{
	return type == eTYPE_ONE ? m_iOneCost : m_iTenCost;
}

bool LimitTimeHeroModel::getIsAddBuyLayer()
{
	return m_bIsAddBuyLayer;
}

vector<sActData>* LimitTimeHeroModel::getActList()
{
	return &m_vActList;
}

void LimitTimeHeroModel::setIsAddBuyLayer( bool isAdd )
{
	m_bIsAddBuyLayer = isAdd;
}

void LimitTimeHeroModel::openTimer()
{
	m_sActData = getActData();
	if(m_sActData != nullptr)
	{
		if(m_sActData->actState == eSTATE_WAIT)
		{
			//开启活动等待开启的计时
			setTimeStamp(m_sActData->actTime.actOpenTime);
			LocalTimer::getInstance()->addTimeStampListener(this);
		}
		else if(m_sActData->actState == eSTATE_SHOW)
		{
			//开启展示定时器
			setTimeStamp(m_sActData->actTime.actShowTime);
			LocalTimer::getInstance()->addTimeStampListener(this);
		}
	}
}

void LimitTimeHeroModel::initMainInfoFromSvr( Json::Value& data )
{
	m_bIsFree = data["isFree"].asInt() == 0 ? false : true;

	//我的积分
	m_sMyScoreRank.rank = data["myTopId"].asInt();
	m_sMyScoreRank.score = data["myScore"].asInt();
	m_sMyScoreRank.name = "";

	setBoxOpenState(data);
	setScoreRankData(data);
	
	//抽奖价格
	sActData* actData = getActData();
	if(actData != nullptr)
	{
		if (actData->actType ==	eActType::eMETHOD_OPENBOX)
		{
			auto data1 =  DataManager::getInstance()->searchToolSystemById(takeBoxOneCostId);
			if (data1)
			{
				m_iOneCost = data1->getIntData("value");
			}
			auto data2 =  DataManager::getInstance()->searchToolSystemById(takeBoxTenCostId);
			if (data2)
			{
				m_iTenCost = data2->getIntData("value");
			}
		}
		else if (actData->actType ==	eActType::eMETHOD_TAKECARD)
		{
			auto data1 =  DataManager::getInstance()->searchToolSystemById(takeCardOneCostId);
			if (data1)
			{
				m_iOneCost = data1->getIntData("value");
			}
			auto data2 =  DataManager::getInstance()->searchToolSystemById(takeCardTenCostId);
			if (data2)
			{
				m_iTenCost = data2->getIntData("value");
			}
		}
	}

	//通知活动UI
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_LIMITTIMEHERO_INFO;
	notifyObservers(&obParam);
}

void LimitTimeHeroModel::setBoxOpenState( Json::Value& data )
{
	//已经开启的宝箱
	Json::Value openBoxList = data["rewardList"];

	m_sActData = getActData();

	if(m_sActData == nullptr)
	{
		return;
	}

	for(int j=0; j<m_sActData->vecBoxRewardData.size(); j++)
	{
		for(int i=0; i<openBoxList.size(); ++i)
		{
			int boxId = openBoxList[i].asInt();
			if(boxId == m_sActData->vecBoxRewardData.at(j).boxId)
			{
				m_sActData->vecBoxRewardData.at(j).isGetted = true;
				break;
			}
		}

		if(m_sMyScoreRank.score >= m_sActData->vecBoxRewardData.at(j).scorePoint
			&& !m_sActData->vecBoxRewardData.at(j).isGetted)
		{
			m_sActData->vecBoxRewardData.at(j).isCanGet = true;
		}
		else
		{
			m_sActData->vecBoxRewardData.at(j).isCanGet = false;
		}
	}
}

void LimitTimeHeroModel::setScoreRankData( Json::Value& data )
{
	//积分排名数据
	Json::Value scoreRankList = data["topInfo"];
	m_vScoreRank.clear();

	for(int i=0; i<scoreRankList.size(); i++)
	{
		sScoreRank scoreRank;
		scoreRank.name = scoreRankList[i]["nickName"].asString();
		scoreRank.score = scoreRankList[i]["score"].asInt();
		scoreRank.rank = scoreRankList[i]["topId"].asInt();

		m_vScoreRank.push_back(scoreRank);
	}
	//排序
	std::sort(m_vScoreRank.begin(), m_vScoreRank.end(), 
		[](const sScoreRank& p1, const sScoreRank& p2)->bool
	{
		//按排名从小到大排序
		return p1.rank < p2.rank;
	});
}

void LimitTimeHeroModel::setTakeCardData( Json::Value& data )
{
	Json::Value changeInfo = data["changeInfo"];
	m_bIsFree = data["isFree"].asInt() == 0 ? false : true;

	m_sActData = getActData();
	//新增的英雄数据	
	m_vItemParam.clear();
	Json::Value newpetLists = data["newItemList"];//#抽到的道具列表 id:xx,count:xx
	for (size_t i = 0; i < newpetLists.size(); i++)
	{
		sGetItemParam newCard;
		newCard.nId = newpetLists[i]["id"].asInt();
		newCard.nCount = newpetLists[i]["amount"].asInt();
		newCard.countChips = newpetLists[i]["count2"].asInt();
		m_vItemParam.push_back(newCard);
	}

	//随机打乱顺序
	random_shuffle(m_vItemParam.begin(),m_vItemParam.end());

	if(m_sActData != nullptr)
	{
		if(m_sActData->actType  ==eTYPE_ONE)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10248),1,getCost(eTYPE_ONE));
		}
		else if (m_sActData->actType  ==eTYPE_TEN)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10249),1,getCost(eTYPE_TEN));
		}
	}
	
	//玩家数据
	MainModel::getInstance()->updateData(changeInfo);
	//英雄数据
	HeroModel::getInstance()->updateData(changeInfo["petList"]);
	// 更新道具;
	ToolModel::getInstance()->updateToolJsonData(changeInfo["itemList"]);

	updateUI(nMAIN_CMD_LIMITTIMEHERO_CARD);
}

void LimitTimeHeroModel::setOpenBoxData( Json::Value& data )
{
	m_bIsFree = data["isFree"].asInt() == 0 ? false : true;

	if(m_sActData->actType ==eTYPE_ONE)
	{
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10250),1,getCost(eTYPE_ONE));
	}
	else if (m_sActData->actType  ==eTYPE_TEN)
	{
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10251),1,getCost(eTYPE_TEN));
	}

	//抽到的东西
	auto allchouParam = data["newItemList"];

	m_vItemParam.clear();
	for (int i = 0; i < allchouParam.size(); i++)
	{
		sGetItemParam item;
		Json::Value single = allchouParam[i];
		item.nId = single["id"].asInt();
		item.nCount = single["amount"].asInt();

		RowData* _row = DataManager::getInstance()->searchToolById(item.nId);
		item.strName = (_row == nullptr) ? "" : _row->getStringData("itemName");

		m_vItemParam.push_back(item);
	}

	random_shuffle(m_vItemParam.begin(), m_vItemParam.end()); 

	// 更新道具;
	ToolModel::getInstance()->updateToolJsonData(data["itemList"]);
	//更新金币/银币
	MainModel::getInstance()->updateData(data["changeInfo"]);

	updateUI(nMAIN_CMD_LIMITTIMEHERO_CARD);
}

void LimitTimeHeroModel::initActInfoFromSvr( Json::Value& data )
{
	Json::Value actInfoList = data["ActiveInfoList"];

	m_vActList.clear();
	m_sActData = nullptr;

	//活动列表
	for(int i=0; i<actInfoList.size(); ++i)
	{
		sActData actData;
		actData.actId = actInfoList[i]["id"].asInt();
		actData.actName = actInfoList[i]["name"].asString();

		actData.actType = (eActType)actInfoList[i]["type"].asInt();
		actData.heroId = actInfoList[i]["heroId"].asInt();

		actData.actTime.actOpenTime = actInfoList[i]["openTime"].asUInt();
		actData.actTime.actOverTime = actInfoList[i]["overTime"].asUInt();
		actData.actTime.actShowTime = actData.actTime.actOverTime + actInfoList[i]["showTime"].asInt();
		actData.actTime.svrOpenTime = actInfoList[i]["serverTime"].asInt();
		actData.actTime.timeType = (eActOpenTimeType)actInfoList[i]["timeTyp"].asInt();

		//活动开启状态
		setActState(&actData);

		//积分宝箱奖励
		setScoreBoxRewardData(actInfoList[i]["point"], actInfoList[i]["pointReward"], &actData);

		//排名奖励列表
		
		setRankRewardData(actInfoList[i]["mailInfoList"], &actData);

		m_vActList.push_back(actData);

		if(i == 0)
		{
			m_sActData = &actData;		//默认当前活动为第一个，当第一个活动结束时在定时器中会重置为下一期活动
		}
	}

	m_bIsGettedTable = true;

	//开启定时器
	openTimer();

	//进游戏后先手动通知主场景UI，是否显示活动图标
	ObserverParam obParam;
	obParam.id = -1;
	if(m_sActData == nullptr || m_sActData->actState == eSTATE_WAIT || m_sActData->actState == eSTATE_OVER)
	{
		//说明当前无活动
		//通知主城UI，关闭图标显示
		obParam.id = LIMIT_TIME_HERO_ACT_SHOW_NO;	
	}
	else if(m_sActData->actState == eSTATE_OPEN || m_sActData->actState == eSTATE_SHOW)
	{
		//说明当前有活动
		//通知主城UI，显示图标
		obParam.id = LIMIT_TIME_HERO_ACT_SHOW_YES;	
	}

	notifyObservers(&obParam);
}


void LimitTimeHeroModel::getGetScoreBox( Json::Value& data )
{
	setBoxOpenState(data);

	Json::Value changeInfo = data["changeInfo"];

	MainModel::getInstance()->updateData(changeInfo);

	//英雄数据
	Json::Value petLists = changeInfo["petList"];
	HeroModel::getInstance()->updateData(petLists);

	Json::Value itemList = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(itemList);

	//通知活动UI
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_LIMITTIMEHERO_GETBOX;
	notifyObservers(&obParam);
}

void LimitTimeHeroModel::setActState(sActData* actData)
{
	//服务器当前时间
	time_t curSvrTime = LocalTimer::getInstance()->getCurServerTime();

	if(curSvrTime < actData->actTime.actOpenTime)
	{
		actData->actState = eSTATE_WAIT;	
	}
	else if(curSvrTime < actData->actTime.actOverTime)
	{
		actData->actState = eSTATE_OPEN;
	}
	else if(curSvrTime < actData->actTime.actShowTime)
	{
		actData->actState = eSTATE_SHOW;
	}
	else
	{
		actData->actState = eSTATE_OVER;
	}

	if(actData->actState == eSTATE_WAIT)
	{
		actData->actTime.actOverSec = actData->actTime.actOverTime - actData->actTime.actOpenTime;
	}
	else if(actData->actState == eSTATE_OPEN)
	{
		actData->actTime.actOverSec = actData->actTime.actOverTime - curSvrTime;
	}
	else
	{
		actData->actTime.actOverSec = 0;
	}
}

void LimitTimeHeroModel::setScoreBoxRewardData( Json::Value& scoreData, Json::Value& rewardData, sActData* actData )
{
	actData->vecBoxRewardData.clear();

	string strScoreList = scoreData.asString();
	string strRewardList = rewardData.asString();

	if(!strScoreList.empty())
	{
		vector<int> vecScoreStr;
		vector<string> vecRewardStr;
		StringFormat::parseCsvStringVec(strScoreList, vecScoreStr);
		StringFormat::parseCsvStringVec(strRewardList, vecRewardStr);			//解析"|"分割符

		/*理论上积分点的size是和积分奖励的size大小相等的，否则配置表出错*/

		for(int i=0; i<vecScoreStr.size() && i<vecRewardStr.size(); i++)
		{
			sRewardBox boxReward;

			boxReward.boxId = i;
			boxReward.isCanGet = false;
			boxReward.isGetted = false;
			boxReward.scorePoint = vecScoreStr.at(i);
			boxReward.vecRewardData.clear();

			vector<string> vecRewardItems;
			StringFormat::parseCsvStringVec(vecRewardStr.at(i), vecRewardItems, ';');		//解析";"分割符

			for(string strTmp : vecRewardItems)
			{
				sRankRewardItem rewardItem;

				int nIndex1 = strTmp.find_first_of(',', 0);
				string strTmp1 = strTmp.substr(0, nIndex1);
				string strTmp2 = strTmp.erase(0, nIndex1 + 1);
				rewardItem.rewardId = atoi(strTmp1.c_str());
				rewardItem.rewardCnt = atoi(strTmp2.c_str());

				boxReward.vecRewardData.push_back(rewardItem);
			}

			actData->vecBoxRewardData.push_back(boxReward);
		}
	}
}


void LimitTimeHeroModel::setRankRewardData( Json::Value& rankRewardList, sActData* actData )
{
	for(int i=0; i<rankRewardList.size(); ++i)
	{
		sRankRewardData rewardData;
		rewardData.id = rankRewardList[i]["id"].asInt();
		rewardData.actId = rankRewardList[i]["activity"].asInt();
		rewardData.rewardTips = rankRewardList[i]["tips"].asString();
		rewardData.limit = rankRewardList[i]["limit"].asInt();

		string rank = rankRewardList[i]["rank"].asString();
		int nIndex1 = rank.find_first_of('|', 0);
		string strTmp1 = rank.substr(0, nIndex1);
		string strTmp2 = rank.erase(0, nIndex1 + 1);
		rewardData.rankSection.beginRank = atoi(strTmp1.c_str());
		rewardData.rankSection.endRank = atoi(strTmp2.c_str());

		rewardData.rankRewardItem.clear();
		string strReward = rankRewardList[i]["rankReward"].asString();
		vector<string> vecRewardStr;
		StringFormat::parseCsvStringVec(strReward, vecRewardStr);

		for(string strTmp : vecRewardStr)
		{
			sRankRewardItem rewardItem;

			int nIdx = strTmp.find_first_of(';', 0);
			string strReTmp1 = strTmp.substr(0, nIdx);
			string strReTmp2 = strTmp.erase(0, nIdx + 1);
			rewardItem.rewardId = atoi(strReTmp1.c_str());
			rewardItem.rewardCnt = atoi(strReTmp2.c_str());

			rewardData.rankRewardItem.push_back(rewardItem);
		}

		actData->vecRankRewardData.push_back(rewardData);
	}
}

void LimitTimeHeroModel::timeOut()
{
	m_sActData = LimitTimeHeroModel::getInstance()->getActData();
	if(m_sActData == nullptr)
	{
		return;
	}

	if(m_sActData->actState == eSTATE_WAIT)
	{
		m_sActData->actState = eSTATE_OPEN;
	
		//开启UI刷新定时器
		//openTimer();

		//通知主城UI，开放图标显示
		ObserverParam obParam;
		obParam.id = LIMIT_TIME_HERO_ACT_SHOW_YES;
		notifyObservers(&obParam);
	}
	else if(m_sActData->actState == eSTATE_SHOW)
	{
		m_sActData->actState = eSTATE_OVER;

		//移除该活动
		m_vActList.erase(
			std::remove_if(m_vActList.begin(), m_vActList.end(), 
			[this](sActData item)
		{
			if (item.actId == m_sActData->actId)
			{
				return true;
			}
			return false;
		}	
		), m_vActList.end());

		//通知主城UI，关闭图标显示
		ObserverParam obParam;
		obParam.id = LIMIT_TIME_HERO_ACT_SHOW_NO;
		notifyObservers(&obParam);
	}
}

void LimitTimeHeroModel::updateUI(int id )
{
	ObserverParam param;
	param.id = id;
	param.updateData = nullptr;
	this->notifyObservers((void*)&param);
}





