#include "LoveModel.h"

bool sortHero(LOVE_HERO_INFO a, LOVE_HERO_INFO b)
{
	//是否结识
	if (a.expType != HERO_GOOD_LV_STRANGE &&  b.expType == HERO_GOOD_LV_STRANGE)
	{
		return true;
	}
	else if (a.expType == HERO_GOOD_LV_STRANGE &&  b.expType != HERO_GOOD_LV_STRANGE)
	{
		return false;
	}
	else if (a.expType == HERO_GOOD_LV_STRANGE &&  b.expType == HERO_GOOD_LV_STRANGE)
	{
		if (a.sortIdx < b.sortIdx)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		//是否新开启
		if (LoveModel::getInstance()->findNewPetId(a.petId) && !LoveModel::getInstance()->findNewPetId(b.petId))
		{
			return true;
		}
		else if (!LoveModel::getInstance()->findNewPetId(a.petId) && LoveModel::getInstance()->findNewPetId(b.petId))
		{
			return false;
		}
		else
		{
			//好感度大在前
			if (a.curLevel > b.curLevel)
			{
				return true;
			}
			else if (a.curLevel < b.curLevel)
			{
				return false;
			}
			else
			{
				//字段排序
				if (a.sortIdx < b.sortIdx)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
}

LoveModel* LoveModel::mInstance = nullptr;

LoveModel::LoveModel()
{
	m_fingerGoodWillAdd.clear();
	m_goodWillAdd.clear();
	m_curOpenLoveHeroPetId = 0;
	isPlayMapBack = false;
	m_isBarrier = false;
}

LoveModel::~LoveModel()
{

}

LoveModel* LoveModel::getInstance()
{
	if (nullptr == mInstance)
	{
		mInstance = new LoveModel;
	}
	return mInstance;
}

void LoveModel::destroyInstance()
{
	if (nullptr != mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void LoveModel::parseHeroListInfoData(const Json::Value &data)
{
	//英雄数据
	Json::Value heroList = data["heroList"];
	Json::Value giftList = data["giftList"];
	m_playCount = data["playNum"].asInt();
	m_guessCount = data["drinkHero"].asInt();

	//解析英雄信息
	m_heroInfoVec.clear();
	for (int i = 0; i < heroList.size(); i++)
	{
		LOVE_HERO_INFO heroInfo;

		heroInfo.petId = heroList[i]["petId"].asInt(); //英雄ID(查表)
		RowData *LoveHeroData = DataManager::getInstance()->searchLoveHeroById(heroInfo.petId);
		if (LoveHeroData)
		{
			heroInfo.sortIdx = LoveHeroData->getIntData("sort");
		}
		heroInfo.lastLevel = heroList[i]["level"].asInt();	//上次好感度等级	
		heroInfo.curLevel = heroList[i]["level"].asInt();	//当前好感度等级					

		heroInfo.goodWill = heroList[i]["goodWill"].asInt();//好感度	
		heroInfo.friendShip = heroList[i]["friendShip"].asInt(); //友情点
		RowData *loveLevel = DataManager::getInstance()->searchLoveLevelById(heroInfo.curLevel);
		if (loveLevel)
		{
			heroInfo.expType = loveLevel->getIntData("loveType");//好感度类型（）
			heroInfo.lastExpType = heroInfo.expType;
		}
		else
		{
			heroInfo.expType = HERO_GOOD_LV_STRANGE;//好感度类型（）
			heroInfo.lastExpType = heroInfo.expType;
		}

		heroInfo.playCount = heroList[i]["playCount"].asInt();//每天游玩次数		
		heroInfo.isGuess = heroList[i]["guessTyp"].asBool(); //是否猜拳
		heroInfo.guessCount = heroList[i]["guessCount"].asInt(); //猜拳次数，每天3次
		heroInfo.guessWinCount = heroList[i]["guessRcount"].asInt();//猜拳胜的次数
		heroInfo.guessReward.clear();
		for (int j = 0; j < heroList[i]["guessReward"].size(); j++)
		{
			heroInfo.guessReward.push_back(heroList[i]["guessReward"][j].asInt());
		}
		heroInfo.typeAtk = heroList[i]["type1"].asInt();
		heroInfo.typeHp = heroList[i]["type2"].asInt();
		heroInfo.typePdd = heroList[i]["type3"].asInt();
		heroInfo.typeMdd = heroList[i]["type4"].asInt();

		m_heroInfoVec.push_back(heroInfo); 
	}
	//新解锁英雄
	for (int i = 0; i < data["newPet"].size(); i++)
	{
		addNewPetId(data["newPet"][i].asInt());
	}
	sort(m_heroInfoVec.begin(),m_heroInfoVec.end(),sortHero);//排序

	//解析礼物信息u
	m_giftInfoVec.clear();
	for (int i = 0; i < giftList.size(); i++)
	{
		LOVE_GIFT_INFO giftInfo;

		giftInfo.templateId = giftList[i]["templateId"].asInt();
		giftInfo.amount = giftList[i]["amount"].asInt();
		giftInfo.resCount = giftList[i]["resCount"].asInt();
		giftInfo.price = giftList[i]["price"].asInt();

		m_giftInfoVec.push_back(giftInfo);
	}

	notify(LOVE_EVENT_TYPE_GET_INFO);
}

void LoveModel::parseHeroSendGiftData(const Json::Value &data)
{
	Json::Value _itemList = data["changeInfo"]["itemList"];
	Json::Value _petList = data["changeInfo"]["petList"];
	// 更新背包;
	ToolModel::getInstance()->updateToolJsonData(_itemList);
	// 更新Hero;
	HeroModel::getInstance()->updateData(_petList);

	MainModel::getInstance()->updateData(data["changeInfo"]);

	Json::Value qingyuan = data["qingyuan"];
	Json::Value giftInfo = data["giftList"];
	m_goodWillAdd.push_back(data["groGoodwill"].asInt());

	//解析英雄信息
	for (int i = 0; i < m_heroInfoVec.size(); i++)
	{
		if ( m_heroInfoVec[i].petId == qingyuan["petId"].asInt())
		{
			m_heroInfoVec[i].curLevel = qingyuan["level"].asInt();	//当前好感度等级					
			m_heroInfoVec[i].goodWill = qingyuan["goodWill"].asInt();//好感度
			m_heroInfoVec[i].friendShip = qingyuan["friendShip"].asInt(); //友情点
			m_heroInfoVec[i].lastExpType = m_heroInfoVec[i].expType;
			RowData *loveLevel = DataManager::getInstance()->searchLoveLevelById(m_heroInfoVec[i].curLevel);
			if (loveLevel)
			{
				m_heroInfoVec[i].expType = loveLevel->getIntData("loveType");//好感度类型（）
			}
			else
			{
				m_heroInfoVec[i].expType = HERO_GOOD_LV_STRANGE;//好感度类型（）
			}
	
			break;
		}
	}
	//新解锁英雄
	for (int i = 0; i < data["newPet"].size(); i++)
	{
		addNewPetId(data["newPet"][i].asInt());
	}
	sort(m_heroInfoVec.begin(),m_heroInfoVec.end(),sortHero);//排序

	//解析礼物信息
	for (int i = 0; i < m_giftInfoVec.size(); i++)
	{
		if (m_giftInfoVec[i].templateId == giftInfo["templateId"].asInt())
		{
			m_giftInfoVec[i].amount = giftInfo["amount"].asInt();
			m_giftInfoVec[i].resCount = giftInfo["resCount"].asInt();
			m_giftInfoVec[i].price = giftInfo["price"].asInt();

			break;
		}
	}

	//奖励
	m_reward.clear();
	for (int i = 0; i < data["loveItem"].size(); i++)
	{
		SEND_GIFT_REWARD reward;
		reward.templateId = data["loveItem"][i]["patchItemId"].asInt();
		reward.amount = data["loveItem"][i]["patchAmount"].asInt();

		m_reward.push_back(reward);
	}

	notify(LOVE_EVENT_TYPE_SEND_GIFT);
}

void LoveModel::parseHeroBuyGiftData(const Json::Value &data)
{
	MainModel::getInstance()->updateData(data["changeInfo"]);

	//解析礼物信息
	for (int i = 0; i < m_giftInfoVec.size(); i++)
	{
		if (m_giftInfoVec[i].templateId == data["templateId"].asInt())
		{
			m_giftInfoVec[i].amount = data["amount"].asInt();
			m_giftInfoVec[i].resCount = data["resCount"].asInt();
			break;
		}
	}

	notify(LOVE_EVENT_TYPE_BUY_GIFT);
}

void LoveModel::parseHeroFingerGuessData(const Json::Value &data)
{
	Json::Value _itemList = data["changeInfo"]["itemList"];
	Json::Value _petList = data["changeInfo"]["petList"];
	// 更新背包;
	ToolModel::getInstance()->updateToolJsonData(_itemList);
	// 更新Hero;
	HeroModel::getInstance()->updateData(_petList);

	MainModel::getInstance()->updateData(data["changeInfo"]);

	Json::Value qingyuan = data["qingyuan"];
	if (data["groGoodwill"].asInt() > 0)
	{
		m_fingerGoodWillAdd.push_back(data["groGoodwill"].asInt());
	}
	m_FingerOutCome = data["outcome"].asInt();
	m_changeGuessPrice = data["price"].asInt();

	//解析英雄信息
	for (int i = 0; i < m_heroInfoVec.size(); i++)
	{
		if ( m_heroInfoVec[i].petId == qingyuan["petId"].asInt())
		{
			m_heroInfoVec[i].lastLevel = m_heroInfoVec[i].curLevel;	//上次好感度等级	
			m_heroInfoVec[i].curLevel = qingyuan["level"].asInt();	//当前好感度等级					
			m_heroInfoVec[i].goodWill = qingyuan["goodWill"].asInt();//好感度
			m_heroInfoVec[i].friendShip = qingyuan["friendShip"].asInt(); //友情点

			RowData *loveLevel = DataManager::getInstance()->searchLoveLevelById(m_heroInfoVec[i].curLevel);
			if (loveLevel)
			{
				m_heroInfoVec[i].lastExpType = m_heroInfoVec[i].expType;
				m_heroInfoVec[i].expType = loveLevel->getIntData("loveType");//好感度类型（）
			}
			else
			{
				m_heroInfoVec[i].lastExpType = m_heroInfoVec[i].expType;
				m_heroInfoVec[i].expType = HERO_GOOD_LV_STRANGE;//好感度类型（）
			}

			m_heroInfoVec[i].isGuess = qingyuan["guessTyp"].asBool(); //是否猜拳
			m_heroInfoVec[i].guessCount = qingyuan["guessCount"].asInt(); //猜拳次数，每天3次
			m_heroInfoVec[i].guessWinCount =qingyuan["guessRcount"].asInt();//猜拳胜的次数
			m_heroInfoVec[i].guessReward.clear();
			for (int j = 0; j < qingyuan["guessReward"].size(); j++)
			{
				m_heroInfoVec[i].guessReward.push_back(qingyuan["guessReward"][j].asInt());
			}

			break;
		}
	}
	//新解锁英雄
	for (int i = 0; i < data["newPet"].size(); i++)
	{
		addNewPetId(data["newPet"][i].asInt());
	}
	sort(m_heroInfoVec.begin(),m_heroInfoVec.end(),sortHero);//排序

	//奖励
	m_reward.clear();
	for (int i = 0; i < data["loveItem"].size(); i++)
	{
		SEND_GIFT_REWARD reward;
		reward.templateId = data["loveItem"][i]["patchItemId"].asInt();
		reward.amount = data["loveItem"][i]["patchAmount"].asInt();

		m_reward.push_back(reward);
	}

	//额外奖励
	m_extraReward.clear();
	for (int i = 0; i < data["extraReward"].size(); i++)
	{
		SEND_GIFT_REWARD reward;
		reward.templateId = data["extraReward"][i]["patchItemId"].asInt();
		reward.amount = data["extraReward"][i]["patchAmount"].asInt();

		m_extraReward.push_back(reward);
	}

	notify(LOVE_EVENT_TYPE_FINGER_GUESS);
}

void LoveModel::parseHeroPlayMapData(const Json::Value &data)
{
	m_playCount = data["allPlayCount"].asInt();

	m_playMap.clear();
	for (int i = 0; i < data["mapIdList"].size(); i++)
	{
		PLAY_MAP playMap;
		playMap.id = data["mapIdList"][i]["id"].asInt();
		playMap.flg = data["mapIdList"][i]["flg"].asInt();

		m_playMap.push_back(playMap);
	}
	notify(LOVE_EVENT_TYPE_PLAY_MAP);
}

void LoveModel::parseHeroChooseMapData(const Json::Value &data)
{
	m_playMapNode.clear();
	m_playMapStepCount = data["playStep"].asInt();
	for (int i = 0; i < data["mapCfg"].size(); i++)
	{
		PLAY_MAP_NODE_INFO playMapNode;
		playMapNode._event = (PLAY_MAP_EVENT_TYPE)data["mapCfg"][i]["event"].asInt();
		playMapNode.flg = data["mapCfg"][i]["flg"].asInt();

		m_playMapNode.push_back(playMapNode);
	}
	notify(LOVE_EVENT_TYPE_PLAY_MAP_INFO);
}

void LoveModel::parseHeroChooseMapEvent(const Json::Value &data)
{
	Json::Value changeInfo = data["changeInfo"];
	Json::Value _itemList = changeInfo["itemList"];
	Json::Value _petList = changeInfo["petList"];
	// 更新背包;
	ToolModel::getInstance()->updateToolJsonData(_itemList);
	// 更新Hero;
	HeroModel::getInstance()->updateData(_petList);

	MainModel::getInstance()->updateData(changeInfo);

	Json::Value qingyuan = data["qingyuan"];
	

	//解析英雄信息
	for (int i = 0; i < m_heroInfoVec.size(); i++)
	{
		if ( m_heroInfoVec[i].petId == qingyuan["petId"].asInt())
		{
			m_heroInfoVec[i].lastLevel = m_heroInfoVec[i].curLevel;	//上次好感度等级	
			m_heroInfoVec[i].curLevel = qingyuan["level"].asInt();	//当前好感度等级	
			m_heroInfoVec[i].goodWill = qingyuan["goodWill"].asInt();//好感度
			RowData *loveLevel = DataManager::getInstance()->searchLoveLevelById(m_heroInfoVec[i].curLevel);
			if (loveLevel)
			{
				m_heroInfoVec[i].lastExpType = m_heroInfoVec[i].expType;
				m_heroInfoVec[i].expType = loveLevel->getIntData("loveType");//好感度类型（）
			}
			else
			{
				m_heroInfoVec[i].lastExpType = m_heroInfoVec[i].expType;
				m_heroInfoVec[i].expType = HERO_GOOD_LV_STRANGE;//好感度类型（）
			}
			m_heroInfoVec[i].playCount = qingyuan["playCount"].asInt();//每天游玩次数				
			m_heroInfoVec[i].friendShip = qingyuan["friendShip"].asInt(); //友情点

			break;
		}
	}
	//新解锁英雄
	for (int i = 0; i < data["newPet"].size(); i++)
	{
		addNewPetId(data["newPet"][i].asInt());
	}

	sort(m_heroInfoVec.begin(),m_heroInfoVec.end(),sortHero);//排序

	m_playMapStepCount = data["playStep"].asInt();

	if (data["groGoodwill"].asInt() > 0)
	{
		m_goodWillAdd.push_back(data["groGoodwill"].asInt());
	}
	if (data["stepChange"].asInt() != 0)
	{
		m_playStepChange.push_back(data["stepChange"].asInt());
	}

	//游玩宝箱
	m_boxItem.clear();
	for (int i = 0; i < data["boxItem"].size(); i++)
	{
		SEND_GIFT_REWARD reward;
		reward.templateId = data["boxItem"][i]["itemId"].asInt();
		reward.amount = data["boxItem"][i]["amount"].asInt();
		m_boxItem.push_back(reward);
	}

	m_playFightId = data["fightId"].asInt();
	m_PlayFlag = data["playFlg"].asInt();

	m_playRewardItem.clear();
	for (int i = 0; i < data["rewardItem"].size(); i++)
	{
		SEND_GIFT_REWARD reward;
		reward.templateId = data["rewardItem"][i]["itemId"].asInt();
		reward.amount = data["rewardItem"][i]["amount"].asInt();
		m_playRewardItem.push_back(reward);
	}

	//奖励
	m_reward.clear();
	for (int i = 0; i < data["loveItem"].size(); i++)
	{
		SEND_GIFT_REWARD reward;
		reward.templateId = data["loveItem"][i]["patchItemId"].asInt();
		reward.amount = data["loveItem"][i]["patchAmount"].asInt();

		m_reward.push_back(reward);
	}

	curCHooseXy = data["eventChange"]["coords"].asInt();
	for (int i = 0; i < m_playMapNode.size(); i++)
	{
		if ( i == curCHooseXy)
		{
			m_playMapNode[i]._event = (PLAY_MAP_EVENT_TYPE)data["eventChange"]["event"].asInt();
			m_playMapNode[i].flg = data["eventChange"]["flg"].asInt();
			break;
		}
	}

	notify(LOVE_EVENT_TYPE_PLAY_MAP_EVENT);
}

void LoveModel::parseGuessCountInfo(const Json::Value &data)
{
	m_buyGuessPrice = data["price"].asInt(); //购买猜拳价格
	m_buyGuessCurCount = data["num"].asInt(); //购买猜拳当前次数
	m_buyGuessMaxCount = data["totalNum"].asInt(); //购买猜拳总次数

	notify(LOVE_EVENT_TYPE_BUY_GUESS_INFO);
}

void LoveModel::parseGuessCount(const Json::Value &data)
{
	MainModel::getInstance()->setGold(data["gold"].asInt());
	MainModel::getInstance()->notifyObservers();

	Json::Value qingyuan = data["qingyuan"];

	//解析英雄信息
	for (int i = 0; i < m_heroInfoVec.size(); i++)
	{
		if ( m_heroInfoVec[i].petId == qingyuan["petId"].asInt())
		{
			m_heroInfoVec[i].guessCount = qingyuan["guessCount"].asInt(); //猜拳次数，每天3次
			break;
		}
	}
	sort(m_heroInfoVec.begin(),m_heroInfoVec.end(),sortHero);//排序

	notify(LOVE_EVENT_TYPE_BUY_GUESS);
}

void LoveModel::parseAddAttribute(const Json::Value &data)
{
	// 更新Hero;
	HeroModel::getInstance()->updateData(data["petList"]);

	//英雄数据
	Json::Value qingyuan = data["qingyuan"];

	//解析英雄信息
	for (int i = 0; i < m_heroInfoVec.size(); i++)
	{
		if ( m_heroInfoVec[i].petId == qingyuan["petId"].asInt())
		{
			m_heroInfoVec[i].friendShip = qingyuan["friendShip"].asInt(); //友情点
			m_heroInfoVec[i].typeAtk = qingyuan["type1"].asInt();
			m_heroInfoVec[i].typeHp = qingyuan["type2"].asInt();
			m_heroInfoVec[i].typePdd = qingyuan["type3"].asInt();
			m_heroInfoVec[i].typeMdd = qingyuan["type4"].asInt();
			break;
		}
	}

	notify(LOVE_EVENT_TYPE_ADD_ATTRIBUTE);
}

void LoveModel::parseResetAttribute(const Json::Value &data)
{
	// 更新Hero;
	HeroModel::getInstance()->updateData(data["petList"]);
	//英雄数据
	Json::Value qingyuan = data["qingyuan"];

	//解析英雄信息
	for (int i = 0; i < m_heroInfoVec.size(); i++)
	{
		if ( m_heroInfoVec[i].petId == qingyuan["petId"].asInt())
		{
			m_heroInfoVec[i].friendShip = qingyuan["friendShip"].asInt(); //友情点
			m_heroInfoVec[i].typeAtk = qingyuan["type1"].asInt();
			m_heroInfoVec[i].typeHp = qingyuan["type2"].asInt();
			m_heroInfoVec[i].typePdd = qingyuan["type3"].asInt();
			m_heroInfoVec[i].typeMdd = qingyuan["type4"].asInt();
			break;
		}
	}

	notify(LOVE_EVENT_TYPE_RESET_ATTRIBUTE);
}

void LoveModel::parseChangeGuess(const Json::Value &data)
{
	Json::Value _itemList = data["changeInfo"]["itemList"];
	Json::Value _petList = data["changeInfo"]["petList"];
	// 更新背包;
	ToolModel::getInstance()->updateToolJsonData(_itemList);
	// 更新Hero;
	HeroModel::getInstance()->updateData(_petList);

	MainModel::getInstance()->updateData(data["changeInfo"]);

	Json::Value qingyuan = data["qingyuan"];
	if (data["groGoodwill"].asInt() > 0)
	{
		m_fingerGoodWillAdd.push_back(data["groGoodwill"].asInt());
	}

	//解析英雄信息
	for (int i = 0; i < m_heroInfoVec.size(); i++)
	{
		if ( m_heroInfoVec[i].petId == qingyuan["petId"].asInt())
		{
			m_heroInfoVec[i].lastLevel = m_heroInfoVec[i].curLevel;	//上次好感度等级	
			m_heroInfoVec[i].curLevel = qingyuan["level"].asInt();	//当前好感度等级					
			m_heroInfoVec[i].goodWill = qingyuan["goodWill"].asInt();//好感度
			m_heroInfoVec[i].friendShip = qingyuan["friendShip"].asInt(); //友情点

			RowData *loveLevel = DataManager::getInstance()->searchLoveLevelById(m_heroInfoVec[i].curLevel);
			if (loveLevel)
			{
				m_heroInfoVec[i].lastExpType = m_heroInfoVec[i].expType;
				m_heroInfoVec[i].expType = loveLevel->getIntData("loveType");//好感度类型（）
			}
			else
			{
				m_heroInfoVec[i].lastExpType = m_heroInfoVec[i].expType;
				m_heroInfoVec[i].expType = HERO_GOOD_LV_STRANGE;//好感度类型（）
			}

			m_heroInfoVec[i].isGuess = qingyuan["guessTyp"].asBool(); //是否猜拳
			m_heroInfoVec[i].guessCount = qingyuan["guessCount"].asInt(); //猜拳次数，每天3次
			m_heroInfoVec[i].guessWinCount =qingyuan["guessRcount"].asInt();//猜拳胜的次数
			m_heroInfoVec[i].guessReward.clear();
			for (int j = 0; j < qingyuan["guessReward"].size(); j++)
			{
				m_heroInfoVec[i].guessReward.push_back(qingyuan["guessReward"][j].asInt());
			}

			break;
		}
	}
	//新解锁英雄
	for (int i = 0; i < data["newPet"].size(); i++)
	{
		addNewPetId(data["newPet"][i].asInt());
	}
	sort(m_heroInfoVec.begin(),m_heroInfoVec.end(),sortHero);//排序

	//奖励
	m_reward.clear();
	for (int i = 0; i < data["loveItem"].size(); i++)
	{
		SEND_GIFT_REWARD reward;
		reward.templateId = data["loveItem"][i]["patchItemId"].asInt();
		reward.amount = data["loveItem"][i]["patchAmount"].asInt();

		m_reward.push_back(reward);
	}

	//额外奖励
	m_extraReward.clear();
	for (int i = 0; i < data["extraReward"].size(); i++)
	{
		SEND_GIFT_REWARD reward;
		reward.templateId = data["extraReward"][i]["patchItemId"].asInt();
		reward.amount = data["extraReward"][i]["patchAmount"].asInt();

		m_extraReward.push_back(reward);
	}

	notify(LOVE_EVENT_TYPE_CHANGE_GUESS);
}

void LoveModel::parseGetGuessPrize(const Json::Value &data)
{
	Json::Value _itemList = data["changeInfo"]["itemList"];
	Json::Value _petList = data["changeInfo"]["petList"];
	// 更新背包;
	ToolModel::getInstance()->updateToolJsonData(_itemList);
	// 更新Hero;
	HeroModel::getInstance()->updateData(_petList);

	MainModel::getInstance()->updateData(data["changeInfo"]);

	Json::Value qingyuan = data["qingyuan"];
	//解析英雄信息
	for (int i = 0; i < m_heroInfoVec.size(); i++)
	{
		if ( m_heroInfoVec[i].petId == qingyuan["petId"].asInt())
		{
			m_heroInfoVec[i].friendShip = qingyuan["friendShip"].asInt(); //友情点
			m_heroInfoVec[i].guessReward.clear();
			for (int j = 0; j < qingyuan["guessReward"].size(); j++)
			{
				m_heroInfoVec[i].guessReward.push_back(qingyuan["guessReward"][j].asInt());
			}

			break;
		}
	}

	//奖励
	m_reward.clear();
	for (int i = 0; i < data["showItem"].size(); i++)
	{
		SEND_GIFT_REWARD reward;
		reward.templateId = data["showItem"][i]["id"].asInt();
		reward.amount = data["showItem"][i]["amount"].asInt();

		m_reward.push_back(reward);
	}

	if (data["friendShip"].asInt() > 0)
	{
		RowData *friend_ship = DataManager::getInstance()->searchToolSystemById(FIREND_SHIP_ID);
		if (friend_ship)
		{
			SEND_GIFT_REWARD reward;
			reward.templateId = friend_ship->getIntData("value");
			reward.amount = data["friendShip"].asInt();
			m_reward.push_back(reward);
		}
	}

	notify(LOVE_EVENT_TYPE_GET_GUESS_PRIZE);
}

void LoveModel::notify(LOVE_EVENT_TYPE eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void LoveModel::addNewPetId(int petId,bool isBarrier /*= false*/)
{
	for (auto iter = m_newPetId.begin(); iter != m_newPetId.end(); iter++)
	{
		if (*iter == petId)
		{
			return;
		}
	}

	if (petId > 0)
	{
		m_newPetId.push_back(petId);
	}

	if (petId > 0 && petId != LUZHISHEN)
	{
		m_curOpenLoveHeroPetId = petId;
	}

	m_isBarrier = isBarrier;
}

void LoveModel::removeNewPetId(int petId)
{
	for (auto iter = m_newPetId.begin(); iter != m_newPetId.end(); iter++)
	{
		if (*iter == petId)
		{
			m_newPetId.erase(iter);
			return;
		}
	}
}

bool LoveModel::findNewPetId(int petId)
{
	for (auto iter = m_newPetId.begin(); iter != m_newPetId.end(); iter++)
	{
		if (*iter == petId)
		{
			return true;
		}
	}

	return false;
}

LOVE_HERO_INFO* LoveModel::getHeroByHeroId(int petId)
{
	//如果是送礼后 有可能排序了 数据会改变
	for (int i = 0; i < m_heroInfoVec.size(); i++)
	{
		if (m_heroInfoVec.at(i).petId == petId)
		{
			return &m_heroInfoVec.at(i);
		}
	}

	return nullptr;
}
