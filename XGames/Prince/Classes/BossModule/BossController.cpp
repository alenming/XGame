#include "BossController.h"
#include "BossModel.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "Widget/LayerWaiting.h"
#include "SDK_TalkingData/TalkingData.h"

BossController::BossController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

BossController::~BossController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void BossController::sendGetBossMainMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_BOSS_MAIN_LIST, "");
	LayerWaiting::hideWaiting();
}

//发送鼓舞
void BossController::sendInspireMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_BOSS_INSPIRE, "");
}

//发送清除冷却
void BossController::sendClearCoolTimeMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_BOSS_CLEAR_COOLTIME, "");
}

//发送获取奖励
void BossController::sendGetRewardMsg(int rewardId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["rewardId"] = rewardId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_BOSS_GET_REWARD, json_file.c_str());
}

//发送获取伤害排名
void BossController::sendGetDamageRankMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_BOSS_CUR_DAMAGE_RANK, "");
}

//解析鼓舞
void BossController::parseInspireMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		//记录付费点
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10019), 1, BossModel::getInstance()->m_iCurInspireCost);

		Json::Value data = root["data"];

		BossModel::getInstance()->m_iDamageAdd = data["atkInc"].asDouble();
		BossModel::getInstance()->m_iInspireTimes = data["guwuTimes"].asInt();
		MainModel::getInstance()->setGold(data["gold"].asInt());
		MainModel::getInstance()->notifyObservers();

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_BOSS_INSPIRE;
		BossModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析清除冷却
void BossController::parseClearCoolTimeMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		//记录付费点
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10020), 1, BossModel::getInstance()->m_iCurClearCoolCost);

		Json::Value data = root["data"];

		MainModel::getInstance()->setGold(data["gold"].asInt());
		BossModel::getInstance()->m_iFightCoolSeconds = 0;
		MainModel::getInstance()->notifyObservers();

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_BOSS_CLEAR_COOLTIME;
		BossModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析获取奖励
void BossController::parseGetRewardMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		Json::Value rewardId = data["reward"];
		Json::Value changeInfo = data["changeInfo"];
		BossModel::getInstance()->m_vRewardIDList.clear();
		for(int i=0; i<rewardId.size(); i++)
		{
			BossModel::getInstance()->m_vRewardIDList.push_back(rewardId[i].asInt());
		}
		BossModel::getInstance()->resetRewardId();

		MainModel::getInstance()->updateData(changeInfo);

		//英雄数据
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = changeInfo["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_BOSS_GET_REWARD;
		BossModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析获取伤害排名
void BossController::parseGetDamageRankMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		Json::Value top50 = data["top50"];

		BossModel::getInstance()->m_myLastTotalDam = data["myTotalDam"].asString();
		BossModel::getInstance()->m_iMyLastRank = data["myOrderNum"].asInt();

		BossModel::getInstance()->m_vLastRankList.clear();
		for(int i=0; i<top50.size(); i++)
		{
			Json::Value single = top50[i];

			sLastDamRank damRankItem;
			damRankItem.resID = single["resID"].asInt();
			damRankItem.name = single["name"].asString();
			damRankItem.level = single["level"].asInt();
			damRankItem.rank = single["orderNum"].asInt();
			damRankItem.damage = single["atk"].asInt();
			damRankItem.totalDam = single["totalDam"].asString();
			BossModel::getInstance()->m_vLastRankList.push_back(damRankItem);
		}

		std::sort(BossModel::getInstance()->m_vLastRankList.begin(), BossModel::getInstance()->m_vLastRankList.end(), 
			[](const sLastDamRank& Dam1, const sLastDamRank& Dam2)->bool
		{
			//按vip等级从小到大排序
			return Dam1.rank < Dam2.rank;
		});

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_BOSS_CUR_DAMAGE_RANK;
		BossModel::getInstance()->notifyObservers(&obParam);
	}
}

void BossController::parseBossMainMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		Json::Value topList = data["top3"];
		Json::Value bossList = data["bossInfo"];
		Json::Value rewardList = data["reward"];

		BossModel::getInstance()->m_bIsOpen = data["openflag"].asInt();
		//由于倒计时结束时，服务器不会主动下发活动开启，只能是客户端通过定时器判定的时间去拉取开启信息
		//未防止客户端时间与服务段时间误差导致拉取不到活动已开启的信息，所以这里做时间压缩1s
		BossModel::getInstance()->m_iSeconds = (int)data["seconds"].asInt() + 1;
		for(int i=0; i<3; i++)
		{
			BossModel::getInstance()->m_sTop[i].rankId = 0;
			BossModel::getInstance()->m_sTop[i].damage = "";
			BossModel::getInstance()->m_sTop[i].name = "";
		}
		switch (data["openflag"].asInt())
		{
			//未开启
		case 0:
			{
				for(int i=0; i<topList.size(); i++)
				{
					//伤害前三名
					BossModel::getInstance()->m_sTop[i].rankId = i+1;
					BossModel::getInstance()->m_sTop[i].name = topList[i].asString();
				}	
			}
			break;
			//已开启
		case 1:
			{
				for(int i=0; i<topList.size(); i++)
				{
					//伤害前三名
					Json::Value damList = topList[i];
					BossModel::getInstance()->m_sTop[i].rankId = i+1;
					BossModel::getInstance()->m_sTop[i].name = damList["name"].asString();
					BossModel::getInstance()->m_sTop[i].damage = damList["totalDam"].asString();
				}	
			}
			break;
		default:
			break;
		}
		
		BossModel::getInstance()->m_iDamageAdd = data["atkInc"].asDouble();
		BossModel::getInstance()->m_iFightCoolSeconds = data["CDSeconds"].asInt();
		BossModel::getInstance()->m_iInspireTimes = data["guwuTimes"].asInt();
		BossModel::getInstance()->m_iMyRank = data["myOrderNum"].asInt();
		BossModel::getInstance()->m_myTotalDamage = data["myTotalDam"].asString();
		BossModel::getInstance()->m_iTotalDamage = data["totalDam"].asInt();

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

		//计算
		BossModel::getInstance()->setRewardId();

		//已经领取过的奖励列表
		BossModel::getInstance()->m_vRewardIDList.clear();
		for(int i=0; i<rewardList.size(); i++)
		{
			BossModel::getInstance()->m_vRewardIDList.push_back(rewardList[i].asInt());
		}

		BossModel::getInstance()->resetRewardId();

		//通知UI, 刷新boss主界面信息
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_BOSS_MAIN_LIST;
		BossModel::getInstance()->notifyObservers(&obParam);	

		if(BossModel::getInstance()->m_bIsFightSettement)
		{
			//通知UI, boss战斗结算
			ObserverParam obParam;
			obParam.id = nMAIN_CMD_BOSS_SETTLEMENT;
			BossModel::getInstance()->notifyObservers(&obParam);
			BossModel::getInstance()->m_bIsFightSettement = false;
		}
	}
}

void BossController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_BOSS_MAIN_LIST:
		{
			parseBossMainMsg(szData);
		}
		break;

	case nMAIN_CMD_BOSS_INSPIRE:
		{
			parseInspireMsg(szData);
		}
		break;

	case nMAIN_CMD_BOSS_CLEAR_COOLTIME:
		{
			parseClearCoolTimeMsg(szData);
		}
		break;

	case nMAIN_CMD_BOSS_GET_REWARD:
		{
			parseGetRewardMsg(szData);
		}
		break;

	case nMAIN_CMD_BOSS_CUR_DAMAGE_RANK:
		{
			parseGetDamageRankMsg(szData);
		}
		break;
	default:
		break;
	}
}