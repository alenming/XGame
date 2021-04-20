#include "OperateActModel.h"
#include "OperateActLayer.h"

OperateActModel* OperateActModel::mInstance = nullptr;

OperateActModel::OperateActModel()
{
	m_Remaday = 0;
	m_LoginTimes = 0;
	m_MyRankId = 0;
	m_MyTotalAtk = 0;
	m_BuyTyp5 = false;
	m_Level = 0;
	m_AllBuyCount = 0;
	m_CurTaskId = -1;
	isLoad = false;
	m_isBuyMonthCard = false;
	m_isBuyExtremeCard = false;
}

OperateActModel::~OperateActModel()
{

}

OperateActModel* OperateActModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new OperateActModel();
	}
	return mInstance;
}

void OperateActModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void OperateActModel::notify(OPRATEACT_EVENT_TYPE eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void OperateActModel::parseOprateSuppleMentTable(const Json::Value &data)
{
	m_operatActivty.clear();
	for (int i = 0; i < data["ActiveTimeList"].size(); i++)
	{
		OPERATE_ACT_SORT_ITEM operateItem;
		operateItem.id = data["ActiveTimeList"][i]["id"].asInt();
		operateItem.tips = data["ActiveTimeList"][i]["tips"].asString();
		operateItem.type = data["ActiveTimeList"][i]["actTyp"].asInt();
		operateItem.tagIdx = data["ActiveTimeList"][i]["tag"].asInt();
		operateItem.timeType = data["ActiveTimeList"][i]["timeTyp"].asInt();
		operateItem.serverTime = data["ActiveTimeList"][i]["serverTime"].asInt();
		operateItem.roleTime = data["ActiveTimeList"][i]["roleTime"].asInt();
		operateItem.openTime = data["ActiveTimeList"][i]["openTime"].asInt();
		operateItem.endTime = data["ActiveTimeList"][i]["endTime"].asInt();
		operateItem.sign = data["ActiveTimeList"][i]["sign"].asInt();
		operateItem.res = data["ActiveTimeList"][i]["res"].asInt();
		operateItem.disappear = data["ActiveTimeList"][i]["disappear"].asInt();
		operateItem.tips2 = data["ActiveTimeList"][i]["tips2"].asString();
		operateItem.daojiTime = 0;
		operateItem.isInit = false;

		for (int j = 0; j < data["ActiveInfoList"].size(); j++)
		{
			if (data["ActiveInfoList"][j]["actTyp"].asInt() == operateItem.id) //活动id
			{
				OPERATE_ACT_INFO_ITEM item;
				item.id = data["ActiveInfoList"][j]["id"].asInt(); //奖励id
				item.tips = data["ActiveInfoList"][j]["tips"].asString();
				item.actTyp = data["ActiveInfoList"][j]["actTyp"].asInt();
				item.refTyp = data["ActiveInfoList"][j]["refTyp"].asInt();
				item.amount1 = atoi(data["ActiveInfoList"][j]["amount1"].asString().c_str());
				item.amount2 = atoi(data["ActiveInfoList"][j]["amount2"].asString().c_str());
				item.price = data["ActiveInfoList"][j]["price"].asString();
				item.actInfo = data["ActiveInfoList"][j]["actInfo"].asString();

				vector<string> vecDst;
				StringFormat::parseCsvStringVec(data["ActiveInfoList"][j]["reward"].asString(), vecDst);
				for (int k = 0; k < vecDst.size(); k++ )
				{
					vector<int> _vecDst;
					StringFormat::parseCsvStringVecBySemi(vecDst[k], _vecDst);

					OPERATEA_PRIZE_ITEM prize;
					prize.id = _vecDst[0];
					prize.amount = _vecDst[1];
					item.reward.push_back(prize);
				}

				operateItem.info.push_back(item);
			}
		}

		m_operatActivty.push_back(operateItem);
	}


	notify(OPRATEACT_EVENT_TYPE_GETTABLE);
}

void OperateActModel::parseOprateActInfo(const Json::Value &data)
{
	isLoad = true;
	//#1 七日礼包
	m_LoginTimes = data["loginTimes"].asInt();  //# 登录天数
	m_ActTyp1.clear();
	for (int i = 0; i < data["actTyp1"].size(); i++)
	{
		m_ActTyp1.push_back(data["actTyp1"][i].asInt());  //#记录已领取的7天奖励ID集合
	}

	//#2 月卡
	m_Remaday = data["remaday"].asInt();  //#月卡剩余天数 <=0:false
    m_Card2Status = data["card2Status"].asBool(); // #是否已购买 至尊月卡 0:false

	//#3 战力排行
	m_MyRankId = data["myTopId"].asInt();	 //我的排名
	m_MyTotalAtk = data["myTotalAtk"].asInt();	 //我的战斗力
	m_playerRank.clear();
	for (int i = 0; i < data["topInfo"].size(); i++)
	{
		OPERATE_PLAYER_RANK item;
		item.fight = data["topInfo"][i]["totalAtk"].asInt();
		item.name = data["topInfo"][i]["nickname"].asString();
		item.vip = data["topInfo"][i]["vipLevel"].asInt();
		item.res = data["topInfo"][i]["petResId"].asInt();

		m_playerRank.push_back(item);
	}

	//#4 强力伙伴


	//#5 理财通
	m_BuyTyp5 = data["buyTyp5"].asBool();	// # 是否购买理财通 0:false
	m_Level = data["level"].asInt();		// # 我的等级
	m_ActTyp5.clear();
	for (int i = 0; i < data["actTyp5"].size(); i++)
	{
		m_ActTyp5.push_back(data["actTyp5"][i].asInt());  // # 已领取的领柴通奖励ID
	}

	//#6 全民福利
	m_AllBuyCount = data["allBuyCount"].asInt();   // #全民福利购买次数
	m_ActTyp6.clear();
	for (int i = 0; i < data["actTyp6"].size(); i++)
	{
		m_ActTyp6.push_back(data["actTyp6"][i].asInt());  //#已领取的全民福利ID
	}

	m_createTime = time_t(data["createTime"].asInt());	//活动结束剩余时间（角色创建时间）时间戳
	m_openTime = time_t(data["openTime"].asInt());	//活动结束剩余时间（开服时间） 时间戳

	//通用模板
	//#7  通用模板A
	m_comon7.clear();
	for (int i = 0; i < data["commontyp7"].size(); i++)
	{
		COMMONA item;
		item.id = data["commontyp7"][i]["actTyp"].asInt();
		for (int j = 0; j < data["commontyp7"][i]["actTyp7"].size(); j++)
		{
			item.actTyp.push_back(data["commontyp7"][i]["actTyp7"][j].asInt());
		}
		
		m_comon7.push_back(item);
	}

	//#8  通用模板A_
	m_comon8.clear();
	for (int i = 0; i < data["commontyp8"].size(); i++)
	{
		COMMONA item;
		item.id = data["commontyp8"][i]["actTyp"].asInt();
		for (int j = 0; j < data["commontyp8"][i]["actTyp8"].size(); j++)
		{
			item.actTyp.push_back(data["commontyp8"][i]["actTyp8"][j].asInt());
		}

		m_comon8.push_back(item);
	}

	//#9  通用模板B
	m_comon9.clear();
	for (int i = 0; i < data["commontyp9"].size(); i++)
	{
		COMMONB item;
		item.id = data["commontyp9"][i]["actTyp"].asInt();
		for (int j = 0; j < data["commontyp9"][i]["actTyp9"].size(); j++)
		{
			item.actTyp.push_back(data["commontyp9"][i]["actTyp9"][j].asInt());
		}
		item.condition = data["commontyp9"][i]["condition"].asInt();
		
		m_comon9.push_back(item);
	}

	//#10 通用模板B_
	m_comon10.clear();
	for (int i = 0; i < data["commontyp10"].size(); i++)
	{
		COMMONB item;
		item.id = data["commontyp10"][i]["actTyp"].asInt();
		for (int j = 0; j < data["commontyp10"][i]["actTyp10"].size(); j++)
		{
			item.actTyp.push_back(data["commontyp10"][i]["actTyp10"][j].asInt());
		}
		item.condition = data["commontyp10"][i]["condition"].asInt();

		m_comon10.push_back(item);
	}

	//#11 单笔充值
	m_ActInfo11.actTyp.clear();
	m_ActInfo11.condition.clear();
	for (int i = 0; i < data["actTyp11"].size(); i++)
	{
		m_ActInfo11.actTyp.push_back(data["actTyp11"][i].asInt());  // # 已领取的单笔充值ID
	}
	for (int i = 0; i < data["condition11"].size(); i++)
	{
		m_ActInfo11.condition.push_back(data["condition11"][i].asInt());  // # 已领取的单笔充值ID
	}

	//#14 超值礼包
	m_ActTyp14.clear();
	for (int i = 0; i < data["actTyp14"].size(); i++)
	{
		m_ActTyp14.push_back(data["actTyp14"][i].asInt());  // # 已领取的超值礼包奖励ID
	}

	notify(OPRATEACT_EVENT_TYPE_INFO);
}

void OperateActModel::parseOprateActPrize(const Json::Value &data)
{
	m_CurTaskId = data["taskId"].asInt();
	//整卡id
	m_petIdList.clear();
	for (int i = 0; i < data["petIdList"].size(); i++)
	{
		m_petIdList.push_back(data["petIdList"][i].asInt());
	}

	Json::Value changeInfo = data["changeInfo"];
	
	//道具数据
	vector<TOOL_NEW_INFO> vecNewTools;
	Json::Value items = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);
	rewardShow.clear();
	for (int i = 0; i < vecNewTools.size(); i++)
	{
		CHABGE_PRIZE_ITEM itemIfo;
		itemIfo.id = vecNewTools[i].ntemplateId;
		itemIfo.amount = vecNewTools[i].nstack;

		rewardShow.push_back(itemIfo);
	}

	Json::Value _petList = changeInfo["petList"];
	// 更新Hero;
	HeroModel::getInstance()->updateData(_petList);

	MainModel::getInstance()->updateData(changeInfo,&rewardShow);

	notify(OPRATEACT_EVENT_TYPE_GET_PRIZE);
}

void OperateActModel::parseOprateActFinancial(const Json::Value &data)
{
	m_BuyTyp5 = data["buyTyp5"].asBool();	// # 是否购买理财通 0:false
	m_AllBuyCount = data["allBuyCount"].asInt();

	MainModel::getInstance()->setGold(data["getGold"].asInt());
	MainModel::getInstance()->notifyObservers();

	notify(OPRATEACT_EVENT_TYPE_FINACIAL);
}

void OperateActModel::parseVipGift(const Json::Value &data)
{
	//整卡id
	m_petIdList.clear();
	for (int i = 0; i < data["petIdList"].size(); i++)
	{
		m_petIdList.push_back(data["petIdList"][i].asInt());
	}

	Json::Value changeInfo = data["changeInfo"];

	//道具数据
	vector<TOOL_NEW_INFO> vecNewTools;
	Json::Value items = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);
	rewardShow.clear();
	for (int i = 0; i < vecNewTools.size(); i++)
	{
		CHABGE_PRIZE_ITEM itemIfo;
		itemIfo.id = vecNewTools[i].ntemplateId;
		itemIfo.amount = vecNewTools[i].nstack;

		rewardShow.push_back(itemIfo);
	}

	if (data["getGoldTyp14"].asInt() > 0) //金币换金币bug
	{
		CHABGE_PRIZE_ITEM itemIfo;
		itemIfo.id = OPRETTE_GOLD_ITEM_ID;
		itemIfo.amount = data["getGoldTyp14"].asInt();

		rewardShow.push_back(itemIfo);
	}

	Json::Value _petList = changeInfo["petList"];
	// 更新Hero;
	HeroModel::getInstance()->updateData(_petList);

	MainModel::getInstance()->updateData(changeInfo,&rewardShow);

	m_ActTyp14.clear();
	for (int i = 0; i < data["actTyp14"].size(); i++)
	{
		m_ActTyp14.push_back(data["actTyp14"][i].asInt());  // # 已领取的超值礼包奖励ID
	}

	notify(OPRATEACT_EVENT_TYPE_VIPGIFT);
}

bool OperateActModel::isHaveGet()
{
	/////////////////////////////////////////////////////////////////////////////////////
	////////////////////////本地数据
	/////////////////////////////////////////////////////////////////////////////////////

	multimap<int, RowData>& operateTable = DataManager::getInstance()->getDTOperateActivitySort()->getTableData(); 
	//遍历数据表
	for(auto iter = operateTable.begin(); iter != operateTable.end(); iter++)
	{
		RowData* rowData = &(iter->second);
		OPERATE_ACT_TYPE type = (OPERATE_ACT_TYPE)rowData->getIntData("actTyp");
		if ((type == OPERATE_ACT_TYPE_MONTH_CARD &&	!MainModel::getInstance()->getMonthCardIsOpen()) ||
			rowData->getIntData("data") == 1)  //苹果审核 超值月卡暂不出现
		{
			continue;
		}

		OPERATE_ACT_SORT_ITEM item;
		item.id = rowData->getIntData("id");
		item.tips = rowData->getStringData("tips");
		item.type = rowData->getIntData("actTyp");
		item.tagIdx = rowData->getIntData("tag");
		item.timeType = rowData->getIntData("timeTyp");
		item.serverTime = rowData->getIntData("serverTime");
		item.roleTime = rowData->getIntData("roleTime");
		item.openTime = rowData->getIntData("openTime");
		item.endTime = rowData->getIntData("endTime");
		item.sign = rowData->getIntData("sign");
		item.res = rowData->getIntData("res");
		item.disappear = rowData->getIntData("disappear");
		item.tips2 = rowData->getStringData("tips2");
		item.daojiTime = 0;
		item.isInit = false;

		if (!OperateActLayer::isOpen(item))
		{
			continue;
		}

		multimap<int, RowData>& OperateActivityData = DataManager::getInstance()->getDTOperateActivity()->getTableData(); 
		if (item.type == OPERATE_ACT_TYPE_SEVEN_DAY)
		{
			for(auto _iter = OperateActivityData.begin(); _iter != OperateActivityData.end(); _iter++)
			{
				RowData* _rowData = &(_iter->second);
				if (_rowData->getIntData("actTyp") == item.id)
				{
					if (_rowData->getIntData("amount1") <= m_LoginTimes)
					{
						vector<int>::iterator result = find( m_ActTyp1.begin( ), m_ActTyp1.end( ), _rowData->getIntData("id")); //查找3
						if ( result == m_ActTyp1.end( ) ) //没找到 能领取
						{
							return true;
						}
					}
				}
			}
		}
		else if (item.type == OPERATE_ACT_TYPE_FINANCIAL)
		{
			MainParam *Mainparam = MainModel::getInstance()->getMainParam();
			m_Level = HeroModel::getInstance()->getMainHeroLevel();
			if (!m_BuyTyp5 && Mainparam->mVipLevel >= DataManager::getInstance()->searchToolSystemById(FINACIAL_VIP_LEVEL)->getIntData("value"))
			{
				return true;
			}
			else
			{
				for(auto _iter = OperateActivityData.begin(); _iter != OperateActivityData.end(); _iter++)
				{
					RowData* _rowData = &(_iter->second);
					if (_rowData->getIntData("actTyp") == item.id)
					{
						if (_rowData->getIntData("amount1") <= m_Level && m_BuyTyp5)
						{
							vector<int>::iterator result = find( m_ActTyp5.begin( ), m_ActTyp5.end( ), _rowData->getIntData("id") ); //查找3
							if ( result == m_ActTyp5.end( ) ) //没找到 能领取
							{
								return true;
							}
						}
					}
				}
			}
		}
		else if (item.type == OPERATE_ACT_TYPE_NATIONAL_WELL_BEING)
		{
			for(auto _iter = OperateActivityData.begin(); _iter != OperateActivityData.end(); _iter++)
			{
				RowData* _rowData = &(_iter->second);
				if (_rowData->getIntData("actTyp") == item.id)
				{
					if (_rowData->getIntData("amount1") <= m_AllBuyCount)
					{
						vector<int>::iterator result = find( m_ActTyp6.begin( ), m_ActTyp6.end( ), _rowData->getIntData("id") ); //查找3
						if ( result == m_ActTyp6.end( ) ) //没找到 能领取
						{
							return true;
						}
					}
				}
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	////////////////////////补充数据
	/////////////////////////////////////////////////////////////////////////////////////
	for (auto iter = m_operatActivty.begin(); iter != m_operatActivty.end(); iter++)
	{
		if (OperateActLayer::isOpen(*iter))
		{
			if (iter->type == OPERATE_ACT_TYPE_COMMOMA)
			{
				COMMONA* common = getComon7ById(iter->id);
				if (common)
				{
					vector<int> ActTyp = common->actTyp;
					for(int i = 0; i < iter->info.size(); i++)
					{
						if (OperateActLayer::comonARefType(iter->info.at(i).refTyp,iter->info.at(i)))
						{
							auto result = find( ActTyp.begin( ), ActTyp.end( ), iter->info.at(i).id ); //查找3
							if ( result == ActTyp.end( ) ) //没找到 能领取
							{
								return true;
							}
						}
					}
				}
			}
			else if (iter->type == OPERATE_ACT_TYPE_COMMOMA_)
			{
				COMMONA* common = getComon8ById(iter->id);
				if (common)
				{
					vector<int> ActTyp = common->actTyp;
					for(int i = 0; i < iter->info.size(); i++)
					{
						if (OperateActLayer::comonARefType(iter->info.at(i).refTyp,iter->info.at(i)))
						{
							auto result = find( ActTyp.begin( ), ActTyp.end( ), iter->info.at(i).id ); //查找3
							if ( result == ActTyp.end( ) ) //没找到 能领取
							{
								return true;
							}
						}
					}
				}
			}
			else if (iter->type == OPERATE_ACT_TYPE_COMMOMB)
			{
				COMMONB* common  = getComon9ById(iter->id);
				if (common)
				{
					vector<int> ActTyp = common->actTyp;
					for(int i = 0; i < iter->info.size(); i++)
					{
						if (OperateActLayer::comonBRefType(iter->info.at(i),*common))
						{
							auto result = find( ActTyp.begin( ), ActTyp.end( ), iter->info.at(i).id ); //查找3
							if ( result == ActTyp.end( ) ) //没找到 能领取
							{
								return true;
							}
						}
					}
				}
			}
			else if (iter->type == OPERATE_ACT_TYPE_COMMOMB_)
			{
				COMMONB* common  = getComon10ById(iter->id);
				if (common)
				{
					vector<int> ActTyp = common->actTyp;
					for(int i = 0; i < iter->info.size(); i++)
					{
						if (OperateActLayer::comonBRefType(iter->info.at(i),*common))
						{
							auto result = find( ActTyp.begin( ), ActTyp.end( ), iter->info.at(i).id ); //查找3
							if ( result == ActTyp.end( ) ) //没找到 能领取
							{
								return true;
							}
						}
					}
				}
			}
			else if (iter->type == OPERATE_ACT_TYPE_SINGLE_RECHARGE)
			{
				for(int i = 0; i < iter->info.size(); i++)
				{
					if (OperateActLayer::singleRechargeRefType(iter->info.at(i),m_ActInfo11.condition))
					{
						auto result = find( m_ActInfo11.actTyp.begin( ), m_ActInfo11.actTyp.end( ), iter->info.at(i).id ); //查找3
						if ( result == m_ActInfo11.actTyp.end( ) ) //没找到 能领取
						{
							return true;
						}
					}
				}
			}
			else if (iter->type == OPERATE_ACT_TYPE_VIPBAG)
			{
				MainParam *Mainparam = MainModel::getInstance()->getMainParam();
				for(int i = 0; i < iter->info.size(); i++)
				{
					if (Mainparam->mVipLevel > iter->info.at(i).amount1)
					{
						auto result = find( m_ActTyp14.begin( ), m_ActTyp14.end( ), iter->info.at(i).id ); //查找3
						if ( result == m_ActTyp14.end( ) ) //没找到 能领取
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

void OperateActModel::setBuyMonthCard()
{
	m_isBuyMonthCard = true;
	m_Remaday += DataManager::getInstance()->searchToolSystemById(MONTH_CARD_DAYS)->getIntData("value");
}

void OperateActModel::setBuyExtremeCard()
{
	m_isBuyExtremeCard = true;
	m_Card2Status = true;
}

COMMONA* OperateActModel::getComon7ById(int id)
{
	for (auto iter = m_comon7.begin(); iter != m_comon7.end(); iter++)
	{
		if (iter->id == id)
		{
			return &(*iter);
		}
	}

	return nullptr;
}

COMMONA* OperateActModel::getComon8ById(int id)
{
	for (auto iter = m_comon8.begin(); iter != m_comon8.end(); iter++)
	{
		if (iter->id == id)
		{
			return &(*iter);
		}
	}

	return nullptr;
}

COMMONB* OperateActModel::getComon9ById(int id)
{
	for (auto iter = m_comon9.begin(); iter != m_comon9.end(); iter++)
	{
		if (iter->id == id)
		{
			return &(*iter);
		}
	}

	return nullptr;
}

COMMONB* OperateActModel::getComon10ById(int id)
{
	for (auto iter = m_comon10.begin(); iter != m_comon10.end(); iter++)
	{
		if (iter->id == id)
		{
			return &(*iter);
		}
	}

	return nullptr;
}

