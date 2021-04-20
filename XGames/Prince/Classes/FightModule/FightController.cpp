#include "FightController.h"
#include "FightModel.h"
#include "../Communication/SocketCommunicator.h"
#include "../Communication/Command.h"
#include "../Common/Common.h"
#include "../DataManager/DataManager.h"
#include "FightStateMachine.h"
#include "ChapterModule/ChapterDef.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainModel.h"
#include "StarSprite.h"
#include "TowerModule/TowerModel.h"
#include "MainModule/MainCityLoadingScene.h"
#include "PvpModule/PvpModel.h"
#include "PvpModule/PvpScene.h"
#include "LoveModule/LoveModel.h"
#include "RobModule/RobModel.h"
#include "RobModule/RobController.h"
#include "BossModule/BossModel.h"
#include "BossModule/BossScene.h"
#include "GroupBossModule/GroupBossScene.h"
#include "MysteryShop/MysteryShopModel.h"

FightController* FightController::ms_pInstance = nullptr;

FightController::FightController()
	: m_pSceneFight(nullptr)
{
	SocketCommunicator::getInstance()->addListener(this);
}

FightController::~FightController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

FightController* FightController::getInstance()
{
	if (nullptr == ms_pInstance)
	{
		ms_pInstance = new FightController();
	}
	return ms_pInstance;
}

void FightController::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void FightController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_PVP_FIGHT_INFO:
	case nMAIN_CMD_FIGHT_HERO_INFO:
	case nMAIN_CMD_ROB_GET_FIGHT_INFO:
	case nMAIN_CMD_TOWER_FIGHT_INFO:
	case nMAIN_CMD_TRAIL_FIGHT_INFO:
	case nMAIN_CMD_FIGHT_MAZE:
	case nMAIN_CMD_BOSS_FIGHT:
	case nMAIN_CMD_GROUP_BOSS_FIGHT:
	case nMAIN_CMD_ACTIVITY_FIGHT:
		parseFightData(szData);
		break;

	case nMAIN_CMD_SETTLEMENT:
	case nMAIN_CMD_ACTIVITY_SETTLEMENT:
		parseSettlementConfirm(szData);
		break;

	case nMAIN_CMD_PVP_FIGHT_SETTLEMENT:
		parsePvpSettlementConfirm(szData);
		break;
	case nMAIN_CMD_TOWER_SETTLEMENT:
		parseTowerSettlement(szData);
		break;

	case nMAIN_CMD_TRAIL_SETTLEMENT:
		parseTrailSettlement(szData);
		break;

	case nMAIN_CMD_ROB_SAVE_FIGHT_INFO:
		parseRobSettlement(szData);
		break;

	case nMAIN_CMD_BOSS_SETTLEMENT:
		parseBossSettlement(szData);
		break;
	case nMAIN_CMD_GROUP_BOSS_SETTLEMENT:
		parseGroupBossSettlement(szData);
		break;

	default:
		break;
	}
	return;
}

void FightController::sendPVEFightMsg(int nBarrierID)
{
	Json::FastWriter writer;
	Json::Value data;
	data["guankaId"] = nBarrierID;

	////////// 加入阵型数据;
	data["flag"] = FormationModel::getInstance()->isFormationChanged() ? 1 : 0;
	vector<int>  vcFormation;
	FormationModel::getInstance()->getFormation(vcFormation);
	for (unsigned int i = 0; i < FORMATION_MAX_POS_COUNT; i++)
	{
		data["petIdList"].append(vcFormation.at(i));
	}
	Combo_Skill_Param comboParam;
	data["maxCombo"] = FormationModel::getInstance()->calcMaxCombo(comboParam);


	std::string  json_file=writer.write(data);
	//log("%s",json_file.c_str());

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FIGHT_HERO_INFO, json_file.c_str());
}

void FightController::sendActivityFightMsg(int nBarrierId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["guankaId"] = nBarrierId;

	////////// 加入阵型数据;
	data["flag"] = FormationModel::getInstance()->isFormationChanged() ? 1 : 0;
	vector<int>  vcFormation;
	FormationModel::getInstance()->getFormation(vcFormation);
	for (unsigned int i = 0; i < FORMATION_MAX_POS_COUNT; i++)
	{
		data["petIdList"].append(vcFormation.at(i));
	}
	Combo_Skill_Param comboParam;
	data["maxCombo"] = FormationModel::getInstance()->calcMaxCombo(comboParam);
	std::string  json_file=writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ACTIVITY_FIGHT, json_file.c_str());
}

void FightController::sendCommonFightMsg(int nFightID)
{
	Json::FastWriter writer;
	Json::Value data;
	data["fightId"] = nFightID;
	std::string  json_file=writer.write(data);
	CCLOG("%s",json_file.c_str());

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_FIGHT_MAZE, json_file.c_str());
}

void FightController::sendRobFightMsg(int index)
{
	Json::FastWriter writer;
	Json::Value data;
	data["enemyId"] = index;
	std::string  json_file=writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_ROB_GET_FIGHT_INFO, json_file.c_str());
}

void FightController::sendPVPFightMsg(int enemyOrderNum,int myOrderNum,int enemyId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["enemyOrderNum"] = enemyOrderNum;
	data["myOrderNum"] = myOrderNum;
	data["enemyId"] = enemyId;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_PVP_FIGHT_INFO, json_file.c_str());
}

void FightController::sendBossFightMsg(int bossId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["bossId"] = bossId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_BOSS_FIGHT, json_file.c_str());
}

void FightController::sendGroupBossFightMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_BOSS_FIGHT, "");
}

void FightController::parseFightData( const char* szData )
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root)) 
	{
		if (throwException(root))
		{
			Director::getInstance()->popToRootScene();
			Director::getInstance()->replaceScene(MainCityLoadingScene::create());
			return;
		}

		Json::Value data = root["data"];
		Json::Value fightInfo = data["fightInfo"];
		Json::Value petList = data["petList"];

		HeroModel::getInstance()->updateData(petList);

		Json::Value heroInfo = fightInfo["petsInfo"];
		parseHeroList(heroInfo);

		Json::Value monsterList = fightInfo["monstersInfo"];
		praseMonsterList(monsterList);

		//解析助战者
		Json::Value assistList = fightInfo["assitsInfo"];
		parseAssistList(assistList);

		int skipFlag = fightInfo["skipFlag"].asInt();
		FightModel::getInstance()->setSkipFlag(skipFlag);

		PvpModel::getInstance()->getPvpCount() = data["lessCount"].asInt(); //设置pvp次数

		//通知UI通信完毕，可以进行异步加载
		FightModel::getInstance()->notifyObservers();
	}
}

bool FightController::parseHeroList( const Json::Value& data )
{
	//初始化怒气
	int curSp = data["nuqi"].asInt();
	FightModel::getInstance()->setMyTotalSp(curSp);

	//初始化速度
	int speed = data["totalSpeed"].asInt();
	FightModel::getInstance()->setMyFirstValue(speed);

	CCLOG("===============init HeroList: curSp=%d  speed:%d===============", curSp, speed);

	//初始化英雄
	FightModel::getInstance()->clearHero();
	Json::Value heroList = data["petList"];
	for (size_t i = 0; i < heroList.size(); ++i)
	{
		Fighter* hero = Fighter::create();
		hero->setSide(FightStateMachine::E_SIDE::MY_SIDE);
		if (parseFighter(heroList[i], hero))
		{
			FightModel::getInstance()->addHero(hero);
		}
	}

	//初始化星灵
	// 	Json::Value spriteInfo = data["spriteInfo"];
	// 	int spriteLevel = spriteInfo["starLv"].asInt();
	// 	int spriteId = spriteInfo["id"].asInt();
	// 	if (spriteId > 0 && spriteLevel > 0)
	// 	{
	// 		StarSprite* starSprite = StarSprite::create(spriteId, spriteLevel, FightStateMachine::E_SIDE::MY_SIDE);
	// 		FightModel::getInstance()->setStarSprite(starSprite, FightStateMachine::E_SIDE::MY_SIDE);
	// 	}

	return true;
}

bool FightController::praseMonsterList( const Json::Value& data )
{
	//初始化怒气
	int curSp = data["nuqi"].asInt();
	FightModel::getInstance()->setEnemyTotalSp(curSp);

	//初始化速度
	int speed = data["totalSpeed"].asInt();
	FightModel::getInstance()->setEnemyFirstValue(speed);

	CCLOG("===============init MonsterList: curSp=%d  speed:%d===============", curSp, speed);

	//初始化英雄
	FightModel::getInstance()->clearMonster();
	Json::Value monsterList = data["monsterList"];
	for (size_t i = 0; i < monsterList.size(); i++)
	{
		Fighter* monster = Fighter::create();
		monster->setSide(FightStateMachine::E_SIDE::ENEMY_SIDE);
		if (parseFighter(monsterList[i], monster))
		{
			FightModel::getInstance()->addMonster(monster);
		}
	}

	//初始化星灵
	// 	Json::Value spriteInfo = data["spriteInfo"];
	// 	int spriteLevel = spriteInfo["starLv"].asInt();
	// 	int spriteId = spriteInfo["id"].asInt();
	// 	if (spriteId > 0 && spriteLevel > 0)
	// 	{
	// 		StarSprite* starSprite = StarSprite::create(spriteId, spriteLevel, FightStateMachine::E_SIDE::ENEMY_SIDE);
	// 		FightModel::getInstance()->setStarSprite(starSprite, FightStateMachine::E_SIDE::ENEMY_SIDE);
	// 	}

	return true;
}

bool FightController::parseAssistList(const Json::Value& data)
{
	//初始化助战角色 mark
	FightModel::getInstance()->clearAssist();

	int assistSide = data["iff"].asInt();
	int assistRound = data["monsterRound"].asInt();
	FightModel::getInstance()->setAssistSide(assistSide);
	FightModel::getInstance()->setAssistEnterRound(assistRound);

	if (assistSide >= 0 && assistRound > 0)
	{
		Json::Value assistList = data["assitList"];
		for (size_t i = 0; i < assistList.size(); i++)
		{
			Fighter* fighter = Fighter::create();
			fighter->setSide((FightStateMachine::E_SIDE)assistSide);
			if (parseFighter(assistList[i], fighter))
			{
				FightModel::getInstance()->addAssist(fighter);
			}
		}
	}
	return true;
}

bool FightController::parseFighter(const Json::Value& data, Fighter* fighter)
{
	int heroId = data["id"].asInt();
	int resId = data["resID"].asInt();
	//const char* name = STR_ANSI(data["name"].asString()).c_str();
	int pos = data["pos"].asInt();

	fighter->setHeroID(heroId);
	fighter->setResID(resId);
	fighter->setName(STR_ANSI(data["name"].asString()).c_str());
	fighter->setIndex(pos);

	CCLOG("===========heroId=%d, resId=%d, pos=%d============", heroId, resId, pos);

	int nTagTyp = data["tagTyp"].asInt();
	int nAtkTyp = data["atkTyp"].asInt();
	int nJobTyp = data["jobTyp"].asInt();
	int nSexTyp = data["sexTyp"].asInt();
	int nPdd = data["pdd"].asInt();
	int nAtk = data["atk"].asInt();
	int nMaxHP = data["hp"].asInt();
	int currHp = data["curHp"].asInt();
	int nMdd = data["mdd"].asInt();
	float fCriPrb = data["criPrb"].asDouble();
	float fCriCoe = data["criCoe"].asDouble();
	float fCtrPrb = data["ctrPrb"].asDouble();
	float fDodgePrb = data["miss"].asDouble();
	float fHitPrb = data["hit"].asDouble();
	float fCriRes = data["criRes"].asDouble();
	float fDamCoe = data["damCoe"].asDouble();
	float fDamRes = data["damRes"].asDouble();

	CCLOG("nTagTyp=%d, nAtkTyp=%d, nPdd=%d, nAtk=%d, nMaxHP=%d, currHp=%d, nMdd=%d, fCriPrb=%f, fCriCoe=%f, fCtrPrb=%f, fDodgePrb=%f, fHitPrb=%f, fCriRes=%f, fDamCoe=%f, fDamRes= %f", 
		nTagTyp, nAtkTyp, nPdd, nAtk, nMaxHP, currHp, nMdd, fCriPrb, fCriCoe, fCtrPrb, fDodgePrb, fHitPrb, fCriRes, fDamCoe, fDamRes);

	fighter->setTagType(nTagTyp);
	fighter->setAtkType(nAtkTyp);
	fighter->setJobTyp(nJobTyp);
	fighter->setSexTyp(nSexTyp);
	fighter->setPdd(nPdd);
	fighter->setAtk(nAtk);
	fighter->setMaxHP(nMaxHP);
	fighter->setCurrHP(currHp);
	fighter->setInitialHp(currHp);
	fighter->setMdd(nMdd);
	fighter->setCriPrb(fCriPrb);
	fighter->setCriCoe(fCriCoe);
	fighter->setCtrPrb(fCtrPrb);
	fighter->setDodgePrb(fDodgePrb);
	fighter->setHitPrb(fHitPrb);
	fighter->setCriRes(fCriRes);
	fighter->setDamCoe(fDamCoe);
	fighter->setDamRes(fDamRes);

	int normalSkillId = data["atkID"].asInt();
	int activeSkillId = data["a_sklID"].asInt();
	int rushSkillId = data["p1_sklID"].asInt();
	//int buffSkillId = data["p2_sklID"].asInt();
	int triggerSkillId = data["p3_sklID"].asInt();
	

	CCLOG("normalSkillId=%d, activeSkillId=%d, rushSkillId=%d, triggerSkillId=%d",
		normalSkillId, activeSkillId, rushSkillId, triggerSkillId);

	fighter->setNormalSkill(normalSkillId);
	fighter->setActiveSkill(activeSkillId);
	fighter->setRushSkill(rushSkillId);
	//fighter->setBuffSkill(buffSkillId);
	fighter->setTriggerSkill(triggerSkillId);

	return true;
}

void FightController::handleSettlement(FIGHT_TYPE nFightType, int nBarrierId, int nFlag, SceneFight* pScene)
{
	m_pSceneFight = pScene;

	// 战斗结算;
	Json::FastWriter _writer;
	Json::Value _value;
	int nCmdId;

	// 以具体战斗类型来区分，构造不同内容的上行协议;
	if (nFightType == FIGHT_TYPE_PVE
		|| nFightType == FIGHT_TYPE_FIND)
	{
		_value["guankaId"] = nBarrierId;
		_value["flag"] = nFlag;
		nCmdId = nMAIN_CMD_SETTLEMENT;
	}
	else if (nFightType == FIGHT_TYPE_PVE_ACTIVITY)
	{
		_value["guankaId"] = nBarrierId;
		_value["flag"] = nFlag;
		_value["totalDam"] = FightModel::getInstance()->getTotalDamage();
		nCmdId = nMAIN_CMD_ACTIVITY_SETTLEMENT;
	}
	else if (nFightType == FIGHT_TYPE_PVP_ARENA)
	{
		_value["flag"] = nFlag;
		nCmdId = nMAIN_CMD_PVP_FIGHT_SETTLEMENT;
	}
	else if (nFightType == FIGHT_TYPE_TRAIL)
	{
		_value["succFlag"] = nFlag;
		_value["nuqi"] = FightModel::getInstance()->getMyTotalSp();

		// 英雄列表;
		Vector<Fighter*>* vcHeroList = FightModel::getInstance()->getHeroList();
		for (int i = 0; i < vcHeroList->size(); ++i)
		{
			Json::Value single;
			single["petId"] = vcHeroList->at(i)->getHeroID();
			single["hp"] = (double)(vcHeroList->at(i)->getCurrHP());
			_value["petList"].append(single);
		}

		// 怪物列表;
		Vector<Fighter*>* vcMonsterList = FightModel::getInstance()->getMonsterList();
		for (int k = 0; k < vcMonsterList->size(); ++k)
		{
			Json::Value single;
			single["petId"] = vcMonsterList->at(k)->getHeroID();
			single["hp"] = (double)(vcMonsterList->at(k)->getCurrHP());
			_value["enemyList"].append(single);
		}
		nCmdId = nMAIN_CMD_TRAIL_SETTLEMENT;
	}
	else if (nFightType == FIGHT_TYPE_TOWER)
	{
		_value["guankaId"] = nBarrierId;
		_value["flag"] = nFlag;
		nCmdId = nMAIN_CMD_TOWER_SETTLEMENT;
	}
	else if (nFightType == FIGHT_TYPE_ROB_FIGHT)
	{
		_value["fightId"] = RobModel::getInstance()->getRobFightId();
		_value["succflag"] = nFlag;
		nCmdId = nMAIN_CMD_ROB_SAVE_FIGHT_INFO;
	}
	else
	{
		return;
	}

	string json_file = _writer.write(_value);
	SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());
}

void FightController::parseSettlementConfirm(const char* szData)
{
	if (nullptr != m_pSceneFight)
	{
		SettlementInfo info;
		Json::Reader read;
		Json::Value root;
		if (read.parse(szData, root))
		{
			if (throwException(root))
			{
				return;
			}

			Json::Value data = root["data"];
			info.nPower = data["power"].asInt();
			info.nEnergy = data["energy"].asInt();
			info.nTotalGold = data["gold"].asInt();
			info.nTotalCoin = data["coin"].asInt();
			info.nCoin = data["getCoin"].asInt();
			if (info.nCoin < 0)	info.nCoin = 0;
			info.nExp = data["getExp"].asInt();
			if (info.nExp < 0)	info.nExp = 0;
			info.nPkgSize = data["pkgsize"].asInt();
			info.nPkgUseSize = data["pkgUsesize"].asInt();
			int nStarNum = data["starNum"].asInt();
			info.fCompletionPercent = FightModel::getInstance()->getCompletionPercent();


			//神秘商店状态
			auto timeM = data["guankaStoreTime"].asInt();
			MysteryShopModel::getInstance()->setStamp(timeM);
			if (timeM >LocalTimer::getInstance()->getCurServerTime())//大于服务器时间
			{
				MysteryShopModel::getInstance()->setIsMainShow(true);//主城显示
				MysteryShopModel::getInstance()->setIsInterShow(true); //关卡场景显示
			}

			LoveModel::getInstance()->addNewPetId(data["qingyuanPetId"].asInt(),true);

			Json::Value itemList = data["itemList"];

			// 掉落物品<templateId, count>;
			Json::Value getItemList = data["getItemList"];
			for (int k = 0; k < getItemList.size(); k++)
			{
				info.mapReward.insert(pair<int, int>(getItemList[k]["id"].asInt(), getItemList[k]["amount"].asInt()));
			}

			Json::Value starReward = root["rewards"];
			for (int i = 0; i < starReward.size(); i++)
			{
				info.nStarReward[i] = starReward[i].asInt();
			}
			Json::Value petList = data["petList"];

			// 更新道具;
			ToolModel::getInstance()->updateToolJsonData(itemList);
			
			// 更新Hero;
			HeroModel::getInstance()->updateData(petList);

			// 更新主角属性;
			MainModel::getInstance()->setPower(info.nPower);
			MainModel::getInstance()->setEnergy(info.nEnergy);
			MainModel::getInstance()->setGold(info.nTotalGold);
			MainModel::getInstance()->setSilver(info.nTotalCoin);
			MainModel::getInstance()->getMainParam()->mStarNum = nStarNum;
			MainModel::getInstance()->notifyObservers();
		}
		m_pSceneFight->onSettlementConfirmed(info);
		m_pSceneFight = nullptr;
	}
}

void FightController::parsePvpSettlementConfirm( const char* szData )
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData,root))
	{
		if (throwException(root))
		{
			return;
		}
	}

	Json::Value data = root["data"];

	if (nullptr != m_pSceneFight)
	{
		SettlementInfo info;
		info.nCoin = data["getCoin"].asInt();
		info.nExp = data["getExp"].asInt();
		Json::Value changeInfo = data["changeInfo"];

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		ToolModel::getInstance()->updateToolJsonData(changeInfo["itemList"],nullptr,&vecNewTools);
		for (int i = 0; i < vecNewTools.size(); i++)
		{
			info.mapReward.insert(pair<int, int>(vecNewTools[i].ntemplateId,vecNewTools[i].nstack));
		}

		//主数据
		MainModel::getInstance()->updateData(changeInfo);

		//英雄数据
		HeroModel::getInstance()->updateData(changeInfo["petList"]);

		m_pSceneFight->onSettlementConfirmed(info);
		m_pSceneFight = nullptr;
	}

	PvpModel::getInstance()->parsepvpSetMentAccountsData(data);
}

void FightController::sendTrailFightMsg( int nOpLv )
{
	Json::FastWriter writer;
	Json::Value data;
	data["hardLevel"] = nOpLv;
	string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TRAIL_FIGHT_INFO, json_file.c_str());
}

void FightController::parseTrailSettlement( const char* szData )
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
		SettlementInfo info;
		info.nCoin = data["coin"].asInt() - MainModel::getInstance()->getMainParam()->mCoin;
		info.nScore = data["getScore"].asInt();

		// 更新货币;
		MainModel::getInstance()->updateData(data);

		m_pSceneFight->onSettlementConfirmed(info);
		m_pSceneFight = nullptr;

		// 是否触发buff;
		TowerModel::getInstance()->setBuffTriggered(data["eventTyp"].asInt() != -1);

		// bufflist;
		vector<pair<int, int> >  vcBuff;
		Json::Value buffList = data["buffList"];
		for (int i = 0; i < buffList.size(); ++i)
		{
			vcBuff.push_back(make_pair(buffList[i]["id"].asInt(), buffList[i]["cost"].asInt()));
		}
		TowerModel::getInstance()->updateBuffContent(vcBuff);
	}
}

void FightController::sendTowerFightMsg( int nBarrierId )
{
	Json::FastWriter writer;
	Json::Value data;
	data["guankaId"] = nBarrierId;

	////////// 加入阵型数据;
	data["flag"] = FormationModel::getInstance()->isFormationChanged() ? 1 : 0;
	vector<int>  vcFormation;
	FormationModel::getInstance()->getFormation(vcFormation);
	for (unsigned int i = 0; i < FORMATION_MAX_POS_COUNT; i++)
	{
		data["petIdList"].append(vcFormation.at(i));
	}
	Combo_Skill_Param comboParam;
	data["maxCombo"] = FormationModel::getInstance()->calcMaxCombo(comboParam);

	string json_file = writer.write(data);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOWER_FIGHT_INFO, json_file.c_str());
}

void FightController::parseTowerSettlement( const char* szData )
{
	// 与普通关卡战斗结算协议一致;
	parseSettlementConfirm(szData);
}

void FightController::parseRobSettlement( const char* szData )
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

		// 更新免战时间;
		RobController::getInstance()->startRestTimer(data["avoidtime"].asInt());

		SettlementInfo info;
		if (data["flag"].asInt() > 0)
		{
			MainModel::getInstance()->updateData(data);

			// 英雄数据;
			Json::Value petLists = data["petList"];
			HeroModel::getInstance()->updateData(petLists);
			Json::Value items = data["itemList"];
			ToolModel::getInstance()->updateToolJsonData(items);

			// 结算信息;
			info.nCoin = data["getCoin"].asInt();
			info.nExp = data["getExp"].asInt();
			info.bRobDone = (data["robflag"].asInt() > 0);

			int fragId = data["fragId"].asInt();
			if (fragId > 0)
			{
				info.mapReward.insert(make_pair(fragId, data["fragAmount"].asInt()));
			}
		}
		else
		{
			// 输了，回包只有精力值，所以只更新精力，其他不管;
			MainModel::getInstance()->setEnergy(data["energy"].asInt());
			MainModel::getInstance()->notifyObservers();
			info.bRobDone = false;
		}

		m_pSceneFight->onSettlementConfirmed(info);
	}
}

void FightController::setBossFightSettlement(int startCount, int bossId, int totalDam, FIGHT_TYPE type, SceneFight* scene)
{
	Json::FastWriter writer;
	Json::Value data;
	data["succFlag"] = startCount;
	data["bossId"] = bossId;
	data["totalDam"] = totalDam;
	std::string json_file = writer.write(data);

	if(type == FIGHT_TYPE_BOSS)
	{
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_BOSS_SETTLEMENT, json_file.c_str());
	}
	else if(type == FIGHT_TYPE_GROUP_BOSS)
	{
		m_pSceneFight = scene;
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GROUP_BOSS_SETTLEMENT, json_file.c_str());
	}
}

void FightController::parseBossSettlement(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	CCLOG("%s", szData);
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		//此处解析数据，解析数据之后跳转至世界boss场景
		Json::Value data = root["data"];
		BossModel::getInstance()->m_iIsEndActive = data["endflag"].asInt();
		if(data["endflag"].asInt() == 1)
		{
			Director::getInstance()->replaceScene(BossScene::create());
			BossModel::getInstance()->parseEndLastFightToScene(data);
		}
		else
		{
			int coin = data["coin"].asInt();
			if(data["winflag"].asInt())
			{
				BossModel::getInstance()->m_vBossInfoFromSvr.at(BossModel::getInstance()->m_iCurSeledBoss).state = 2;
			}
			BossModel::getInstance()->m_iFightCoolSeconds = data["CDSeconds"].asInt();
			BossModel::getInstance()->m_bIsFightSettement = true;
			Director::getInstance()->replaceScene(BossScene::create());
		}
	}
}

void FightController::parseGroupBossSettlement( const char* szData )
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData,root))
	{
		if (throwException(root))
		{
			return;
		}
	}

	Json::Value data = root["data"];

	if (nullptr != m_pSceneFight)
	{
		SettlementInfo info;
		info.nCoin = data["getCoin"].asInt();
		info.nExp = data["getExp"].asInt();

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		ToolModel::getInstance()->updateToolJsonData(data["itemList"],nullptr,&vecNewTools);
		for (int i = 0; i < vecNewTools.size(); i++)
		{
			info.mapReward.insert(pair<int, int>(vecNewTools[i].ntemplateId,vecNewTools[i].nstack));
		}

		//主数据
		MainModel::getInstance()->updateData(data);

		//英雄数据
		HeroModel::getInstance()->updateData(data["petList"]);

		m_pSceneFight->onSettlementConfirmed(info);
		m_pSceneFight = nullptr;
	}
}
