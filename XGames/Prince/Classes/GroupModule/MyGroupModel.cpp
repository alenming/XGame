#include "MyGroupModel.h"
#include "Utils/LocalTimer.h"
#include "DataManager/DataManager.h"
#include "SDK_TalkingData/TalkingData.h"

MyGroupModel* MyGroupModel::ms_pInstance = nullptr;

//高级捐献 文本
//const string strDonateType1 = "帮派高级捐献";

//土豪捐献 文本
//const string strDonateType2 = "帮派土豪捐献";

MyGroupModel::MyGroupModel()
{
	m_iFlagNum = 0;
	m_iFlagTempID = 0;
	priceOld = 0;
	m_vPlayerInfo.clear();
	m_vProposerInfo.clear();
	m_vGroupEvent.clear();
	m_vBuildInfo.clear();
	m_vDonateDT.clear();
	initDonateLocalData();
	initDynEventLocalData();
	initFlagNum();
}

MyGroupModel::~MyGroupModel()
{

}

MyGroupModel* MyGroupModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new MyGroupModel();
	}
	return ms_pInstance;
}

void MyGroupModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void MyGroupModel::updateGroupInfoFromSvr(const Json::Value& data)
{
	Json::Value info = data["bangPai"];
	Json::Value groupInfo = info["bangpai"];

	int curViceRecord = 0;
	updateAllPlayerInfoFromSvr(info, &curViceRecord);

	//更新加入条件
	eJoinType type = eJOIN_DEFAULT_TYPE;
	type = groupInfo["checkFlag"].asBool() ? eJOIN_CHECK_TYPE : eJOIN_FREE_TYPE;
	int levelLimit = groupInfo["needLvl"].asInt();
	sJoinLimit limit;
	limit.type = type;
	limit.levelLimit = levelLimit;
	updateJoinLimit(limit);

	//更新帮派建筑信息
	updateGroupBuildInfo(groupInfo);

	m_sMyGroupInfo.id = data["bId"].asInt();
	m_sMyGroupInfo.name = groupInfo["name"].asString();
	m_sMyGroupInfo.level = groupInfo["level"].asInt();
	m_sMyGroupInfo.resId = groupInfo["resId"].asInt();
	m_sMyGroupInfo.leaderName = groupInfo["leader"].asString();
	m_sMyGroupInfo.curMemberNum = groupInfo["curNum"].asInt();
	m_sMyGroupInfo.totalMemberNum = groupInfo["maxNum"].asInt();
	m_sMyGroupInfo.rank = groupInfo["orderNum"].asInt();
	m_sMyGroupInfo.curViceNum = curViceRecord;				//当前副帮主的数量

	/*********以下四个字段暂时不添加***********/
	/*
	int exp;										//帮派经验
	int todayExp;									//今日帮派经验值
	string viceName1;								//副帮主名
	string viceName2;								//副帮主名
	*/
	//入口活动建筑信息
	initGroupActivityBuildInfo();
	//活动信息
	auto actInfo = data["activity"];
	updateGroupActivity(actInfo);

	//通知UI, 刷新帮派主界面
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_GROUP_MAIN_INFO;
	notifyObservers(&obParam);
}

void MyGroupModel::updateAllPlayerInfoFromSvr(const Json::Value& data, int* viceRecord)
{
	Json::Value memberInfo = data["member"];
	m_vPlayerInfo.clear();

	int selfUId = MainModel::getInstance()->getMainParam()->mRoleId;

	for(int i=0; i<memberInfo.size(); i++)
	{
		sPlayerInfo playerData;
		playerData.uId = memberInfo[i]["id"].asInt();
		playerData.iconResId = memberInfo[i]["petResId"].asInt();
		playerData.name = memberInfo[i]["nickname"].asString();
		playerData.vipLevel = memberInfo[i]["viplevel"].asInt();
		playerData.level = memberInfo[i]["level"].asInt();
		playerData.contributeVal = memberInfo[i]["giftTotal"].asInt();
		playerData.fightPower = memberInfo[i]["totalAtk"].asInt();
		playerData.isOnLine = memberInfo[i]["onlineflag"].asBool();
		playerData.lastLoginTime = memberInfo[i]["lastLoginTime"].asUInt();
		//职位
		switch (memberInfo[i]["position"].asInt())
		{
		case 1:
			playerData.power = ePOWERLEADER;
			break;
		case 2:
			{
				playerData.power = ePOWERVICE;
				viceRecord += 1;
			}
			break;
		case 3:
			playerData.power = ePOWERNOPOWER;
			break;
		default:
			break;
		}
	
		//阵型
		Json::Value& matrixIds = memberInfo[i]["matrixIds"];
		for(int i=0; i<matrixIds.size(); i++)
		{
			matrixData singleMatrix;
			singleMatrix.id = matrixIds[i]["id"].asInt();
			singleMatrix.stars = matrixIds[i]["stars"].asInt();
			playerData.matrix.push_back(singleMatrix);
		}

		//如果是自己
		if(playerData.uId == selfUId)
		{
			m_sMyInfo.myPrestige = MainModel::getInstance()->getMainParam()->mGroupMoney;
			m_sMyInfo.info = playerData;
		}

		m_vPlayerInfo.push_back(playerData);
	}

	sortAllPlayer();
}

void MyGroupModel::sortAllPlayer()
{
	//排序
	std::sort(m_vPlayerInfo.begin(), m_vPlayerInfo.end(), 
		[](sPlayerInfo p1, sPlayerInfo p2)->bool
	{
		//排序规则1.在线排前面，离线排后面
		//在线规则1.按照职位高低排序, 2.按照贡献排序, 3.按照主角等级排序
		//离线规则1.按照职位高低排序, 2.按照贡献排序, 3.按照主角等级排序, 4.按照离线时长排序,离线时间长的排后面

		if(p1.isOnLine && !p2.isOnLine)
		{
			return true;
		}
		else if(!p1.isOnLine && p2.isOnLine)
		{
			return false;
		}
		else
		{
			if(p1.power != p2.power)
			{
				return p1.power < p2.power;
			}
			else
			{
				if(p1.contributeVal != p2.contributeVal)
				{
					return p1.contributeVal > p2.contributeVal;
				}
				else
				{
					if(p1.level != p2.level)
					{
						return p1.level > p2.level;
					}
					else
					{
						if(p1.isOnLine && p2.isOnLine)
						{
							//都在线, 此时随便一个排前面
							return false;
						}
						else
						{
							//都离线, 按最后登录时间
							if(p1.lastLoginTime != p2.lastLoginTime)
							{
								return p1.lastLoginTime > p2.lastLoginTime;
							}
							else
							{
								//此时随便一个排前面
								return false;
							}
						}
					}
				}
			}
		}
	});
}


void MyGroupModel::updateMyInfo(const Json::Value& data)
{

}

void MyGroupModel::updateGroupDeclaration()
{
	m_sDeclaration.insideDec = getTempInNotice();
	m_sDeclaration.outsideDec = getTempOutNotice();

	setTempInNotice(m_sDeclaration.insideDec);
	setTempOutNotice(m_sDeclaration.outsideDec);

	//通知UI, 刷新帮派主界面
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_GROUP_MODIFY_NOTICE;
	notifyObservers(&obParam);
}
	
void MyGroupModel::updateGroupEvent(const Json::Value& data)
{
	m_sDeclaration.insideDec = data["remark1"].asString();
	m_sDeclaration.outsideDec = data["remark2"].asString();

	Json::Value eventList = data["trendsList"];
	
	m_vEventDate.clear();
	m_vGroupEvent.clear();
	for(int i=0; i<eventList.size(); i++)
	{
		string content = eventList[i]["content"].asString();
		time_t createDate = eventList[i]["createDate"].asUInt();
		int type = eventList[i]["type"].asInt();

		struct tm* tm = localtime(&createDate);
		
		sEventDate eventDate;
		eventDate.year = tm->tm_year + 1900;
		eventDate.month = tm->tm_mon + 1;
		eventDate.day = tm->tm_mday;

		bool isAdded = false;						//存放不同日期
		for(int j=0; j<m_vEventDate.size(); ++j)
		{
			if(eventDate.year == m_vEventDate.at(j).year && 
				eventDate.month == m_vEventDate.at(j).month &&
				eventDate.day == m_vEventDate.at(j).day)
			{
				isAdded = true;
				break;
			}
		}	

		if(!isAdded)
		{
			m_vEventDate.push_back(eventDate);
		}

		sDynamicEvent dynEvent;
		dynEvent.eventDate = eventDate;
		dynEvent.eventContent = formatEventByType(content, type);
		dynEvent.eventTime = formatTime(tm->tm_hour, tm->tm_min);
		m_vGroupEvent.push_back(dynEvent);
	}

	//通知UI, 刷新帮派动态事件
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_GROUP_DYN_EVENT;
	notifyObservers(&obParam);
}

void MyGroupModel::updateJoinLimit(sJoinLimit limit)
{
	m_sJoinLimit.type = limit.type;
	m_sJoinLimit.levelLimit = limit.levelLimit;
}

void MyGroupModel::updateGroupBuildInfo(const Json::Value& data)
{
	//暂时只有两个帮派建筑，目前协议写死，如果要扩展这里要改
	//服务端没有构建建筑ID，根据表格和建筑等级，前端自己构建建筑ID，建筑升级时，发送建筑标识
	//flag = 1--聚义厅，flag = 2--声望商店; 捐赠类型 itemType = 1--普通捐献，itemType = 2--高级捐献，itemType = 3--土豪捐献
	
	m_vBuildInfo.clear();
	//聚义厅数据
	sBuildInfo buildInfo;
	buildInfo.level = data["level"].asInt();
	buildInfo.type = eBUILD_JUYITING_TYPE;
	buildInfo.id = buildInfo.level + 100;
	m_vBuildInfo.push_back(buildInfo);
	
	//声望商店数据
	sBuildInfo buildStoreInfo;
	buildStoreInfo.level = data["storeLvl"].asInt();
	buildStoreInfo.type = eBUILD_DOSNATE_SHOP_TYPE;
	buildStoreInfo.id = buildStoreInfo.level + 200;
	m_vBuildInfo.push_back(buildStoreInfo);			//商店先不放进来

	//寻龙阁数据
	sBuildInfo buildXunlongInfo;
	buildXunlongInfo.level = data["storeLvl"].asInt();
	buildXunlongInfo.type = eBUILD_DOSNATE_XUNLONG;
	buildXunlongInfo.id = buildXunlongInfo.level + 300;
	m_vBuildInfo.push_back(buildXunlongInfo);			//商店先不放进来

	initBuildInfo();
}

void MyGroupModel::initBuildInfo(eBuildType type, int buildID)
{
	if(type == eBUILD_DEFAULT_TYPE && buildID == 0)
	{
		for(auto &info : m_vBuildInfo)
		{
			RowData* rowData = DataManager::getInstance()->searchBuildingById(info.id);
			if(!rowData)
			{
				return;
			}

			info.resId = rowData->getIntData("resId");
			info.nextExp = rowData->getIntData("levelExp");
			info.name= rowData->getStringData("buildingName");
			info.curEffect = rowData->getStringData("current");
			info.nextEffect = rowData->getStringData("next");
			info.reset = rowData->getIntData("reset");
			//建筑当前经验值和玩家捐献次数需要在每次点击建筑弹窗时实时拉取
		}
	}
	else
	{
		RowData* rowData = DataManager::getInstance()->searchBuildingById(buildID);
		if(!rowData)
		{
			return;
		}
		
		for(auto &info : m_vBuildInfo)
		{
			if(info.type == type)
			{
				info.resId = rowData->getIntData("resId");
				info.nextExp = rowData->getIntData("levelExp");
				info.name= rowData->getStringData("buildingName");
				info.curEffect = rowData->getStringData("current");
				info.nextEffect = rowData->getStringData("next");

				//建筑当前经验值和玩家捐献次数需要在每次点击建筑弹窗时实时拉取
				return;
			}
		}
	}
}

void MyGroupModel::initGroupActivityBuildInfo()
{
	m_vActivityBuildInfo.clear();

	DataTable* alldate = DataManager::getInstance()->getGroupActivityInfo();
	if(!alldate)
	{
		return;
	}
	auto aaa = alldate->getTableData();

	for (auto iterrater = aaa.begin(); iterrater!=aaa.end(); iterrater++)
	{
		auto oneDate = (RowData)iterrater->second;

		sActivityBuildInfo info;
		//因为目前就一个活动所以你懂的
		info.type = eActivityType::ActivityType_XunLong;
		info.resId = oneDate.getIntData("res");
		info.name = oneDate.getStringData("name");
		info.tips = oneDate.getStringData("tips"); 
		info._sort = oneDate.getIntData("sort");	

		StringFormat::parseCsvStringVec(oneDate.getStringData("show"), info.showId);

		m_vActivityBuildInfo.push_back(info);
	}

	for (int i = 0; i < m_vActivityBuildInfo.size()-1; i++)
	{
		for (int j = 1; j < m_vActivityBuildInfo.size(); j++)
		{
			auto a = m_vActivityBuildInfo.at(i);
			auto b = m_vActivityBuildInfo.at(j);
			if (a._sort<b._sort)
			{
				int infoid = a.id;
				int inforesId = a.resId;
				string infoname = a.name;
				string infotips = a.tips;
				int info_sort = a._sort;
				std::vector<int>infoshow = a.showId;
				
				a.resId = b.resId;
				a.name = b.name;
				a.id = b.id;
				a.tips = b.tips;
				a._sort = b._sort;
				a.showId = b.showId;

				b.resId = inforesId;
				b.name = infoname;
				b.id = infoid;
				b.tips = infotips;
				b._sort = info_sort;
				b.showId = infoshow;
			}
		}
	}
}

void MyGroupModel::updateBuildInfoByType(const Json::Value& data, eBuildType type, int cmd)
{
	MainModel::getInstance()->setGold(data["gold"].asInt());
	MainModel::getInstance()->setSilver(data["coin"].asInt());
	MainModel::getInstance()->setGroupMoney(data["credit"].asInt());

	m_sMyInfo.myPrestige = data["credit"].asInt();
	m_sMyInfo.myMaxDonateTimes = data["maxTimes"].asInt();
	m_sMyInfo.myCurDonateTimes = data["curTimes"].asInt();

	m_sMyGroupInfo.todayMaxExp = data["maxDonate"].asInt();
	m_sMyGroupInfo.todayExp = data["curDonate"].asInt();

	for(auto &info : m_vBuildInfo)
	{
		if(info.type == type)
		{
			info.curExp = data["curExp"].asInt();
			//如果该建筑发生了等级变化，则建筑信息需要重新读表，否则信息不更新
			if(info.level != data["level"].asInt())
			{
				info.level = data["level"].asInt();
				if(info.type == eBUILD_JUYITING_TYPE)
				{
					info.id = 100 + info.level;
					m_sMyGroupInfo.level = info.level;			//更新帮派等级=聚义厅等级
				}
				else if(info.type == eBUILD_DOSNATE_SHOP_TYPE)
				{
					info.id = 200 + info.level;
				}
				else
				{
					break;
				}

				initBuildInfo(type, info.id);
				break;
			}
		}
	}

	if(cmd == nMAIN_CMD_GROUP_BUILD_DONATE)
	{
		MainModel::getInstance()->notifyObservers();

		//talkingData统计
		sDonateLocalDT donateData;
		getDonateDataByType(getTempDonateType(), &donateData);
		switch (getTempDonateType())
		{
		case eDONATE_ADVANCE_TYPE:	
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10196), 1, donateData.costGold);
			break;
		case eDONATE_RICH_TYPE:
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10197), 1, donateData.costGold);
			break;
		default:
			break;
		}
	}

	//通知UI, 通知捐赠弹窗/刷新捐赠事件
	ObserverParam obParam;
	obParam.id = cmd;
	notifyObservers(&obParam);
}

void MyGroupModel::updateMemberPower(int cmd)
{
	switch (cmd)
	{
	case nMAIN_CMD_GROUP_ABDICATE:
		{
			m_sMyGroupInfo.id = getNewLeaderID();
			m_sMyGroupInfo.leaderName = getPlayerInfoByUid(m_sMyGroupInfo.id)->name;
			int isUpdateEnd = 0;
			for(auto &info : m_vPlayerInfo)
			{
				if(info.uId == m_sMyGroupInfo.id)
				{
					info.power = ePOWERLEADER;
					isUpdateEnd++;
				}
				else if(info.uId == getOldLeaderID())
				{
					info.power = ePOWERNOPOWER;
					if(info.uId == m_sMyInfo.info.uId)
					{
						info.power = ePOWERNOPOWER;
					}
					isUpdateEnd++;
				}
				if(isUpdateEnd == 2)
				{
					break;
				}
			}
		}
		break;
	case nMAIN_CMD_GROUP_APPOINT:
		{
			for(auto &info : m_vPlayerInfo)
			{
				if(info.uId == getApplicantID())
				{
					if(info.power == ePOWERVICE)
					{
						info.power = ePOWERNOPOWER;
						if(info.uId == m_sMyInfo.info.uId)
						{
							m_sMyInfo.info.power = ePOWERNOPOWER;
						}
					}
					else if(info.power == ePOWERNOPOWER)
					{
						info.power = ePOWERVICE;
						if(info.uId == m_sMyInfo.info.uId)
						{
							m_sMyInfo.info.power = ePOWERVICE;
						}
					}
					break;
				}
			}
		}
		break;
	default:
		break;
	}
	
	sortAllPlayer();

	//通知UI, 刷新帮派主界面
	ObserverParam obParam;
	obParam.id = cmd;
	notifyObservers(&obParam);
}

void MyGroupModel::updateProposerList(const Json::Value& data)
{
	m_vProposerInfo.clear();

	Json::Value applicantList = data["applyList"];

	for(int i = 0; i<applicantList.size(); i++)
	{
		sProposerInfo applicant;
		applicant.uId = applicantList[i]["id"].asInt();
		applicant.name = applicantList[i]["nickname"].asString();
		applicant.level = applicantList[i]["level"].asInt();
		applicant.vipLv = applicantList[i]["viplevel"].asInt();
		applicant.iconResId = applicantList[i]["petResId"].asInt();
		applicant.fightPower = applicantList[i]["totalAtk"].asInt();

		m_vProposerInfo.push_back(applicant);
	}

	//通知UI, 刷新申请人列表
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_GROUP_APPLICANT_LIST;
	notifyObservers(&obParam);
}

void MyGroupModel::updateProposerInfo()
{
	for(auto iter = m_vProposerInfo.begin(); iter != m_vProposerInfo.end(); ++iter)
	{
		if(iter->uId == getApplicantID())
		{
			m_vProposerInfo.erase(iter);
			break;
		}
	}

	if(getTempIsAllow())
	{
		m_sMyGroupInfo.curMemberNum += 1;
	}
	//sortAllPlayer();
}

void MyGroupModel::updateExitGroup(int cmd)
{
	if(cmd != nMAIN_CMD_GROUP_EXPEL)
	{
		//清空相关数据
		m_vProposerInfo.clear();
		m_vBuildInfo.clear();
		m_vEventDate.clear();
		m_vGroupEvent.clear();
		m_vPlayerInfo.clear();

		sPlayerInfo info;
		m_sMyInfo.info = info;
		m_sMyInfo.myCurDonateTimes = 0;
	}

	//通知UI
	ObserverParam obParam;
	obParam.id = cmd;
	MyGroupModel::getInstance()->notifyObservers(&obParam);
}

void MyGroupModel::updateGroupActivity(const Json::Value& data)
{
	//活动数据
	m_sActivityInfo.goldCost = data["price"].asInt();
	priceOld = m_sActivityInfo.goldCost;
	m_sActivityInfo.clickCount = data["actCount"].asInt();
	m_sActivityInfo.searchID = data["dropoutId"].asInt();
	m_sActivityInfo.isXunlong = data["type"].asInt();

	m_sActivityInfo.showedIdflag.clear();
	auto item9 = data["actList"];
	for (int i = 0; i< item9.size(); ++i)
	{
		m_sActivityInfo.showedIdflag.push_back(item9[i].asInt());
	}

	auto showInfo = DataManager::getInstance()->searchGroupActivityshowInfoById(m_sActivityInfo.searchID);
	if (!showInfo)
	{
		return;
	}
	m_sActivityInfo.showHero = showInfo->getIntData("heroShow");
	m_sActivityInfo.showItem.clear();
	StringFormat::parseCsvStringVec(showInfo->getStringData("itemShow"),m_sActivityInfo.showItem);
	m_sActivityInfo.nineList.clear();
	StringFormat::parseCsvStringVec(showInfo->getStringData("itemBox"),m_sActivityInfo.nineList);

	random_shuffle(m_sActivityInfo.nineList.begin(),m_sActivityInfo.nineList.end());

	m_sActivityInfo.importantId = showInfo->getIntData("specialitem");

}


void MyGroupModel::updateGroupActivityRefresh( const Json::Value& data )
{

	auto actInfo = data["activity"];
	updateGroupActivity(actInfo);
	//通知UI
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_GROUP_ACTIVITY_REFRESH;
	MyGroupModel::getInstance()->notifyObservers(&obParam);
}

void MyGroupModel::updateGroupActivityClick( const Json::Value& data )
{

	auto showItem = data["showItem"];
	if (!showItem.isNull())
	{
		for (int i = 0; i < showItem.size(); i++)
		{
			auto temp = showItem[i];
			m_sActivityInfo.actFind.id = temp["itemId"].asInt();
			m_sActivityInfo.actFind.count = temp["amount"].asInt();
		}
	}

	//金币消耗
	TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10198),1,priceOld);
	
	auto actInfo = data["activity"];

	updateGroupActivity(actInfo);

	//通知UI
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_GROUP_ACTIVITY_CLICK;
	MyGroupModel::getInstance()->notifyObservers(&obParam);
}

void MyGroupModel::updateGroupActivityHistory( const Json::Value& data )
{
	m_sActivityInfo.clickedHistory.clear();
	auto itemhistory = data["itemList"];
	for (int i = 0; i< itemhistory.size(); ++i)
	{
		std::vector<NineList> oneHist;
		auto oneItem = itemhistory[i];
		for (int j = 0; j < oneItem.size(); j++)
		{
			auto oneItem1 = oneItem[j];
			NineList  onelist;
			onelist.resId = oneItem1["itemId"].asInt();
			onelist.count = oneItem1["amount"].asInt();
			oneHist.push_back(onelist);
		}
		m_sActivityInfo.clickedHistory.push_back(oneHist);
	}

	//通知UI
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_GROUP_ACTIVITY_HISTORY;
	MyGroupModel::getInstance()->notifyObservers(&obParam);
}

void MyGroupModel::initDynEventLocalData()
{

}

void MyGroupModel::initDonateLocalData()
{
	m_vDonateDT.clear();
	multimap<int, RowData>& powerTable = DataManager::getInstance()->getTableDonate()->getTableData();
	for(auto iter = powerTable.begin(); iter != powerTable.end(); iter++)
	{
		RowData* rowData = &(iter->second);

		if(rowData == nullptr)
		{
			continue;
		}

		sDonateLocalDT donateData;

		donateData.id = rowData->getIntData("id");
		donateData.name = rowData->getStringData("name");
		donateData.costCoin = rowData->getIntData("costCoin");
		donateData.costGold = rowData->getIntData("costGold");
		donateData.addExp = rowData->getIntData("addExp");
		donateData.prestige = rowData->getIntData("prestige");
		donateData.contriVal = rowData->getIntData("contribution");

		m_vDonateDT.push_back(donateData);
	}
}

void MyGroupModel::initFlagNum()
{
	DataTable* flagTable = DataManager::getInstance()->getTableEmblem();
	m_iFlagNum = flagTable->getTableData().size();
}

string MyGroupModel::formatTime(int hour, int min)
{
	if(hour >= 24 || hour <0 || min >= 60 || min < 0)
	{
		return "";
	}

	string _hour = hour < 10 ? (_TO_STR(0) + _TO_STR(hour)) : _TO_STR(hour);
	string _min = min < 10 ? (_TO_STR(0) + _TO_STR(min)) : _TO_STR(min);

	return _hour + ":" + _min;
}

string MyGroupModel::formatEventByType(string keyField, int type)
{
	RowData* eventData = DataManager::getInstance()->searchEventById(type);
	if(eventData == nullptr)
	{
		return "";
	}

	//先把keyField解析
	vector<string> keyTemp;
	keyTemp.clear();
	if(!keyField.empty())
	{	
		StringFormat::parseCsvStringVec(keyField, keyTemp);
	}

	//keyField字段插入表格中的描述中
	string eventDes = eventData->getStringData("description");
	vector<string> desTemp;
	desTemp.clear();
	if(!eventDes.empty())
	{
		StringFormat::parseCsvStringVec(eventDes, desTemp, '@');
	}

	string resultStr = "";
	for(int i=0; i<desTemp.size(); i++)
	{
		resultStr += desTemp.at(i);
		if(i < keyTemp.size())
		{
			resultStr += keyTemp.at(i);
		}
	}
	
	return resultStr;
}

sMyGroupData* MyGroupModel::getMyGroupInfo()
{
	return &m_sMyGroupInfo;
}


vector<sPlayerInfo>* MyGroupModel::getAllPlayerInfo()
{
	return &m_vPlayerInfo;
}


sPlayerInfo* MyGroupModel::getPlayerInfoByUid(int uId)
{
	for(auto &info : m_vPlayerInfo)
	{
		if(info.uId == uId)
		{
			return &info;
		}
	}

	return nullptr;
}


sMyInfo* MyGroupModel::getMyInfo()
{
	return &m_sMyInfo;
}


sDeclaration* MyGroupModel::getGroupDeclaration()
{
	return &m_sDeclaration;
}

vector<sEventDate>* MyGroupModel::getGroupEventDate()
{
	return &m_vEventDate;
}

void MyGroupModel::getGroupEventByDate(sEventDate date, vector<sDynamicEvent>* vecEvent)
{
	vecEvent->clear();
	
	for(auto &eve : m_vGroupEvent)
	{
		if(date.year == eve.eventDate.year && date.month == eve.eventDate.month && date.day == eve.eventDate.day)
		{
			vecEvent->push_back(eve);
		}
	}
}

sJoinLimit* MyGroupModel::getJoinLimit()
{
	return &m_sJoinLimit;
}

vector<sBuildInfo>* MyGroupModel::getGroupBuildInfo()
{
	return &m_vBuildInfo;
}

sBuildInfo* MyGroupModel::getBuildInfoByID(eBuildType type)
{
	for(auto &info : m_vBuildInfo)
	{
		if(info.type == type)
		{
			return &info;
		}
	}

	return nullptr;
}

vector<sActivityBuildInfo>* MyGroupModel::getGroupActivityBuildInfo()
{
	return &m_vActivityBuildInfo;
}

sActivityBuildInfo* MyGroupModel::getGroupActivityBuildInfoByID(eActivityType type)
{
	for(auto &info : m_vActivityBuildInfo)
	{
		if(info.type == type)
		{
			return &info;
		}
	}

	return nullptr;
}

sActivityInfo* MyGroupModel::getGroupActivityInfo()
{
	return &m_sActivityInfo;
}

vector<sProposerInfo>* MyGroupModel::getAllProposerInfo()
{
	return &m_vProposerInfo;
}

string MyGroupModel::getPostPowerName(ePostPower post)
{
	switch (post)
	{
	case ePOWERLEADER:
		//return STR_UTF8("帮主");
		return DataManager::getInstance()->searchCommonTexdtById(10199);
		break;
	case ePOWERVICE:
		//return STR_UTF8("副帮主");
		return DataManager::getInstance()->searchCommonTexdtById(10200);
		break;
	case ePOWERNOPOWER:
		//return STR_UTF8("帮众");
		return DataManager::getInstance()->searchCommonTexdtById(10201);
		break;
	
	default:
		break;
	}

	return "";
}

string MyGroupModel::getGroupIconPath(int iconId)
{
	stringstream ss;

	RowData* flagResData = DataManager::getInstance()->searchEmblemById(iconId);
	if(flagResData)
	{
		string name = flagResData->getStringData("resId");
		ss << "Image/Icon/Flag/" << name << ".png";
		return ss.str();
	}

	return "";
}

string MyGroupModel::getBuildIconPath(int buildId)
{
	stringstream ss;

	ss << "Image/Icon/global/" << buildId << ".png";
	return ss.str();
}

void MyGroupModel::getDonateDataById(int id, sDonateLocalDT* donateData)
{
	for(auto &info : m_vDonateDT)
	{
		if(info.id == id)
		{
			*donateData = info;
			break;
		}
	}
}

void MyGroupModel::getDonateDataByType(eDonateType type, sDonateLocalDT* donateData)
{
	getDonateDataById(type, donateData);
}

string MyGroupModel::getLastLoginTimeDes(int uId)
{
	time_t lastLoginTime  = 0;

	for(auto &info : m_vPlayerInfo)
	{
		if(info.uId == uId)
		{
			if(info.isOnLine)
			{
				//return STR_UTF8("在线");
				return DataManager::getInstance()->searchCommonTexdtById(10202);
			}
			lastLoginTime = info.lastLoginTime;
			break;
		}
	}
	
	if(lastLoginTime == 0)
	{
		return "";
	}

	time_t curSvrTime = LocalTimer::getInstance()->getCurServerTime();
	time_t outLineTime = curSvrTime - lastLoginTime;

	time_t min = outLineTime/60;
	if(min <= 59)
	{
		//return _TO_STR(min) + STR_UTF8("分钟前");
		return _TO_STR(min) + DataManager::getInstance()->searchCommonTexdtById(10203);
	}
	else
	{
		time_t hour = outLineTime/3600;
		if(hour <= 23)
		{
			//return _TO_STR(hour) + STR_UTF8("小时前");
			return _TO_STR(hour) + DataManager::getInstance()->searchCommonTexdtById(10204);
		}
		else
		{
			time_t day = outLineTime/3600/24;
			//return _TO_STR(day) + STR_UTF8("天前");
			return _TO_STR(day) + DataManager::getInstance()->searchCommonTexdtById(10205);
		}
	}
}









