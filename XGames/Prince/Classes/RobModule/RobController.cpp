#include "RobController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Json/json.h"
#include "RobModel.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "LevelUp/LevelUpController.h"
#include "Temp/CustomGoldTips.h"
#include "SDK_TalkingData/TalkingData.h"

RobController* RobController::m_pInstance = nullptr;

RobController::RobController()
	: m_nRestTime(0)
	, m_restType(0)
{
	SocketCommunicator::getInstance()->addListener(this);
}
RobController::~RobController()
{
	SocketCommunicator::getInstance()->removeListener(this);
	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}

RobController* RobController::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new RobController();
	}

	return m_pInstance;
}

void RobController::destroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void RobController::onRecvMessage(int nCmdID, const char* szData)
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	switch (nCmdID)
	{	
	case nMAIN_CMD_ROB_GET_CHIPS:
		procRobMainInfo(szData);
		break;
	case nMAIN_CMD_ROB_AVOID_WAR:
		procRest(szData);
		break;
	case nMAIN_CMD_ROB_COMPOSE:
		procCompose(szData);
		break;
	case nMAIN_CMD_ROB_REFRESH_ENEMY:	// 掠夺刷新对手
		procRefreshEnemy(szData);
		break;
	case nMAIN_CMD_ROB_GET_ROB_RECORD:	// 掠夺获取掠夺记录
		procRobRecord(szData);
		break;
	case nMAIN_CMD_ROB_GET_ROB_TEN_INFO:
		procRobTenResult(szData);
		break;
	case nMAIN_CMD_ROB_GET_ROB_TEN_REWARD:
		procRobTenResult(szData);
		break;
	case nMAIN_CMD_ROB_PUSH_UPDATE:
		procRobPushUpdate(szData);
		break;
	default:
		break;
	}
}
void RobController::procRobMainInfo(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		// 免战时间;
		Rob_Main_Info  mainInfo;
		mainInfo.nRestTime = data["avoidtime"].asInt();
		if (mainInfo.nRestTime > 0)
		{
			// 重新计时;
			startRestTimer(data["avoidtime"].asInt());
		}

		// 免战牌数量;
		mainInfo.nSignAmount = data["avoidamount"].asInt();
		RobModel::getInstance()->setAvoidamount(mainInfo.nSignAmount);

		// 更新精力值;
		MainModel::getInstance()->setEnergy(data["energy"].asInt());
		MainModel::getInstance()->notifyObservers();

		// ;
		RobModel::getInstance()->updateMainInfo(&mainInfo);
	}
}
void RobController::procRest(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data	= root["data"];
		MainModel::getInstance()->updateData(data);

		// 
		if (m_restType == 2)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20211), 1, RobModel::getInstance()->getRestCost());
		}

		// 英雄数据;
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		// 背包数据;
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items);

		// 免战牌;
		RobModel::getInstance()->setAvoidamount(data["avoidamount"].asInt());

		// 重新计时;
		startRestTimer(data["avoidtime"].asInt());
	}
}

void RobController::refreshCountDown( float delta )
{
	RobModel::getInstance()->onCountDown(m_nRestTime);
	if (m_nRestTime <= 0)
	{
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(RobController::refreshCountDown), this);
		return;
	}
	m_nRestTime--;
}

void RobController::procCompose(const char* szData)
{	
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items);

		RobModel::getInstance()->onComposeResult();
	}
}

void RobController::procRefreshEnemy(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		Json::Value playerList = data["playerList"];
		vector<ROB_PLAYER_INFO>  vcEnemy;
		for (int i = 0; i < playerList.size(); i++)
		{
			Json::Value playerItem = playerList[i];

			ROB_PLAYER_INFO playerInfos;
			playerInfos.strPlayerName = playerItem["name"].asString();
			playerInfos.nPlayerId = playerItem["enemyId"].asInt();
			playerInfos.nPlayerType = (playerInfos.nPlayerId <= 0) ? -1 : 1;
			playerInfos.nTotalAtk = playerItem["atk"].asInt();
			playerInfos.nPro = playerItem["pro"].asInt();
			playerInfos.petResId = playerItem["petResId"].asInt();
			playerInfos.nLv = playerItem["level"].asInt();
			Json::Value matrixIds = playerItem["matrixIds"];
			for (int i = 0; i < matrixIds.size(); i++)
			{
				matrixData singleMatrix;
				singleMatrix.id = matrixIds[i]["id"].asInt();
				singleMatrix.stars = matrixIds[i]["stars"].asInt();
				playerInfos.vecMatrixIds.push_back(singleMatrix);
			}
			vcEnemy.push_back(playerInfos);
		}
		RobModel::getInstance()->setRobFightId(data["fightId"].asInt());

		RobModel::getInstance()->updateEnemy(&vcEnemy);
	}
}

void RobController::procRobRecord(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		vector<ROB_RECORDS_INFO> vcRecords;
		Json::Value recordList = data["recordList"];
		for (int i = 0; i < recordList.size(); i++)
		{
			ROB_RECORDS_INFO recordInfo;
			recordInfo.id = recordList[i]["id"].asInt();
			recordInfo.amount = recordList[i]["amount"].asInt();
			recordInfo.strPlayerName = recordList[i]["nickname"].asString();
			recordInfo.petResId = recordList[i]["petResId"].asInt();
			recordInfo.level = recordList[i]["level"].asInt();
			recordInfo.id = recordList[i]["id"].asInt();
			recordInfo.itemId = recordList[i]["itemId"].asInt();
			recordInfo.amount = recordList[i]["amount"].asInt();
			recordInfo.succflag = recordList[i]["succflag"].asInt();
			recordInfo.strDate = recordList[i]["datetime"].asString();
			recordInfo.direction = recordList[i]["flag"].asInt();

			vcRecords.push_back(recordInfo);
		}
		RobModel::getInstance()->updateRobRecord(&vcRecords);
	}
}

void RobController::procRobTenResult(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);
		MainModel::getInstance()->updateData(data);

		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items);

		ROB_TEN_RESULT robResult;
		robResult.succFlag	= data["flag"].asInt();
		robResult.avoidtime	= data["avoidtime"].asInt();
		robResult.times = data["times"].asInt();
		robResult.getTimes = data["getTimes"].asInt();
		robResult.robFlag = data["robflag"].asInt();
		robResult.strTip = data["tip"].asString();
		robResult.costEnergy = data["costEnergy"].asInt();
		robResult.fragId = data["fragId"].asInt();
		robResult.fragAmount = data["fragAmount"].asInt();

		// 掠夺掉落;
		Json::Value getItemList = data["getItemList"];
		for (int i = 0; i < getItemList.size(); ++i)
		{
			ROB_RESULT_INFO  _singleInfo;
			_singleInfo.nOrder = i+1;

			Json::Value single = getItemList[i];
			for (int k = 0; k < single.size(); ++k)
			{
				_singleInfo.nCoin = data["getCoin"].asInt();
				_singleInfo.nExp = data["getExp"].asInt();
				_singleInfo.nEnergy = data["costEnergy"].asInt();
				_singleInfo.mapDropItems.insert(make_pair(single[k]["id"].asInt(), single[k]["amount"].asInt()));
			}
			robResult.mapResult.insert(make_pair(i, _singleInfo));
		}

		RobModel::getInstance()->updateRobTenResult(&robResult);

		// 检测升级;
		LevelUpController::getInstance()->checkLevelUp();
	}
}

void RobController::procRobPushUpdate(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		// 仅更新物品，协议里只有itemList;
		Json::Value data = root["data"];
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items);
		RobModel::getInstance()->updateRobToolsAndChips();
	}
}


void RobController::handleRobMainInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ROB_GET_CHIPS, "");
}

void RobController::handleCompose(const int& id, const int& flag)
{
	//flag = 0 :合成 1 : 一键合成 
	Json::FastWriter writer;
	Json::Value root;
	root["id"] = id;
	root["flag"] = flag;
	std::string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ROB_COMPOSE, json_file.c_str());
}

void RobController::handleRobRecord()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ROB_GET_ROB_RECORD, "");
}

void RobController::handleRest(int flag)
{
	// 金币数量判定;
	if (flag == 2)
	{
		RowData* _row = DataManager::getInstance()->searchToolSystemById(10099);
		if (nullptr != _row)
		{
			int per = _row->getIntData("value");
			if (MainModel::getInstance()->getMainParam()->mGold < per)
			{
				CustomGoldTips *_tips = CustomGoldTips::create();
				if (_tips)
				{
					Director::getInstance()->getRunningScene()->addChild(_tips);
				}
				return;
			}
		}
	}

	// @param flag: 1-道具免战，2-金币免战;
	Json::FastWriter writer;
	Json::Value root;
	m_restType = flag;
	root["flag"] = m_restType;
	std::string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ROB_AVOID_WAR, json_file.c_str());
}

void RobController::handleRefreshEnemy(int nChipId)
{
	// 刷新对手 @param fragId: 碎片Id 
	Json::FastWriter writer;
	Json::Value root;
	root["fragId"] = nChipId;
	std::string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ROB_REFRESH_ENEMY, json_file.c_str());
}

void RobController::handleRobTen()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ROB_GET_ROB_TEN_INFO, "");
}

void RobController::startRestTimer( int nRestTime )
{
	if (nRestTime < 0)
		return;

	m_nRestTime = nRestTime;
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(RobController::refreshCountDown), this);
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(RobController::refreshCountDown), this, 1.0f, kRepeatForever, 0.1f, false);
}
