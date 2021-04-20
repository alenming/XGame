#include "NewAreaActivityModel.h"
#include "DataManager/DataManager.h"
#include "Communication/Command.h"
#include "Utils/StringFormat.h"
#include "ChapterModule/ChapterModel.h"
#include "ChapterModule/ChapterDef.h"
#include "HeroModule/HeroModel.h"
#include "MainModule/MainModel.h"


NewAreaActivityModel* NewAreaActivityModel::ms_pInstance = nullptr;

NewAreaActivityModel::NewAreaActivityModel()
{
	m_vecNewAreaActData.clear();
	m_iRemainTime = 0;
	m_iCurActDay = 0;
	m_iActEndedTime = 0;
	m_bIsActOpen = false;
	m_bIsLoaded = false;
	m_bIsFirstLoad = false;
	initFromLocal();
}

NewAreaActivityModel::~NewAreaActivityModel()
{
	//LocalTimer::getInstance()->removeLocalTimerListener(this);
}

void NewAreaActivityModel::initFromLocal()
{
	//新区活动数据表
	multimap<int, RowData>& newAreaActMap = DataManager::getInstance()->getDTNewAreaActivity()->getTableData();

	//遍历新区活动数据表
	for(auto iter = newAreaActMap.begin(); iter != newAreaActMap.end(); iter++)
	{
		RowData* rowData = &(iter->second);

		sNewAreaActivityData actInfo;

		actInfo.actId = rowData->getIntData("id");
		actInfo.actName = rowData->getStringData("labName");
		actInfo.actShow = rowData->getIntData("labShow");
		actInfo.actOpenTime = rowData->getIntData("show");
		actInfo.taskType = rowData->getIntData("refTyp");
		actInfo.actDes = rowData->getStringData("actInfo");
		actInfo.skipType = rowData->getIntData("skip");
		actInfo.amount1.done = 0;
		actInfo.amount3 = 0;

		//数量1解析
		string cnt = rowData->getStringData("amount1");
		if(!cnt.empty())
		{
			int nIndex = cnt.find_first_of(';', 0);
			if(nIndex == string::npos)
			{
				if(actInfo.taskType == 5)				//关卡任务，完成显示1，未完成显示0，不显示进度了
				{
					actInfo.amount1.id = atoi(cnt.c_str());
					actInfo.amount1.done = 0;
					actInfo.amount1.cnt = 1;
				}
				else
				{
					actInfo.amount1.id = -1;
					actInfo.amount1.cnt = atoi(cnt.c_str());
				}
				if(actInfo.taskType == 2)			//任务类型是主角等级
				{
					actInfo.amount1.done = HeroModel::getInstance()->getMainHeroLevel();
				}
			}
			else
			{
				string strTmp1 = cnt.substr(0, nIndex);					
				string strTmp2 = cnt.erase(0, nIndex + 1);
				actInfo.amount1.id = atoi(strTmp1.c_str());
				actInfo.amount1.cnt = atoi(strTmp2.c_str());
			}
		}

		//价格解析
		if(actInfo.taskType != 4)				//只有特价抢购 有礼包个数，原价和特价
		{
			actInfo.oldPrice = -1;
			actInfo.newPrice = -1;
			actInfo.amount2 = -1;
		}
		else
		{
			string price = rowData->getStringData("price");
			if(!price.empty())
			{
				int nIndex = price.find_first_of('|', 0);
				string strTmp1 = price.substr(0, nIndex);					
				string strTmp2 = price.erase(0, nIndex + 1);
				actInfo.oldPrice = atoi(strTmp1.c_str());
				actInfo.newPrice = atoi(strTmp2.c_str());
			}

			actInfo.amount2 = rowData->getIntData("amount2");
			actInfo.amount3 = actInfo.amount2;
		}

		//道具解析
		string propItems = rowData->getStringData("item");
		actInfo.vecPropInfo.clear();
		if(!propItems.empty())
		{
			vector<string> vecStr;
			StringFormat::parseCsvStringVec(propItems, vecStr);

			for(string strTmp : vecStr)
			{
				int nIndex = strTmp.find_first_of(';', 0);
				string strTmp1 = strTmp.substr(0, nIndex);
				string strTmp2 = strTmp.erase(0, nIndex + 1);
				sPropInfo propItem;
				propItem.propId = atoi(strTmp1.c_str());
				propItem.propCnt = atoi(strTmp2.c_str());
				actInfo.vecPropInfo.push_back(propItem);
			}
		}
		actInfo.isCanGet = false;
		actInfo.isGetted = false;
		m_vecNewAreaActData.push_back(actInfo);
	}
}

void NewAreaActivityModel::initFromSvr(const Json::Value& actData)
{
	m_bIsLoaded = true;
	//时间倒计时
	m_iRemainTime = actData["countdown"].asInt();
	//当前开服第几天
	m_iCurActDay = actData["curday"].asInt();
	//倒计时不为0，活动状态为开启
	if(m_iCurActDay == 8)
	{
		m_iCurActDay = 7;			//开服狂欢活动结束临界点处理
	}
	//活动结束时间戳
	m_iActEndedTime = actData["closeDate"].asInt();

	for(int i=0; i<m_vecNewAreaActData.size(); i++)
	{
		if(m_vecNewAreaActData.at(i).taskType == 1)			//开服第几天福利
		{
			m_vecNewAreaActData.at(i).amount1.done = m_iCurActDay;
		}
		m_vecNewAreaActData.at(i).isCanGet = false;
	}
	
	Json::Value buycountList = actData["buycount"];
	Json::Value doinList = actData["doinList"];
	Json::Value active3List = actData["active3List"];
	Json::Value finish3List = actData["finish3List"];

	for(int i=0; i<doinList.size(); i++)
	{
		for(int j=0; j<m_vecNewAreaActData.size(); j++)
		{
			Json::Value single = doinList[i];
			if(m_vecNewAreaActData.at(j).actId == single["id"].asInt())
			{
				m_vecNewAreaActData.at(j).amount1.done = single["amount"].asInt();
				break;
			}
		}
	}

	for(int i=0; i<active3List.size(); i++)
	{
		for(int j=0; j<m_vecNewAreaActData.size(); j++)
		{
			if(m_vecNewAreaActData.at(j).actId == active3List[i].asInt())
			{
				m_vecNewAreaActData.at(j).isCanGet = true;
				m_vecNewAreaActData.at(j).amount1.done = m_vecNewAreaActData.at(j).amount1.cnt;
				break;
			}
		}
	}

	for(int i=0; i<finish3List.size(); i++)
	{
		for(int j=0; j<m_vecNewAreaActData.size(); j++)
		{
			if(m_vecNewAreaActData.at(j).actId == finish3List[i].asInt())
			{
				m_vecNewAreaActData.at(j).isCanGet = false;
				m_vecNewAreaActData.at(j).isGetted = true;
				m_vecNewAreaActData.at(j).amount1.done = m_vecNewAreaActData.at(j).amount1.cnt;
				break;
			}
		}
	}

	//放在三个列表后面是在拿到相关数据后再过滤一次特价礼包是否可购买
	//剩余可购买数量
	for(int i=0; i<buycountList.size(); i++)
	{
		for(int j=0; j<m_vecNewAreaActData.size(); j++)
		{
			Json::Value single = buycountList[i];
			if(m_vecNewAreaActData.at(j).actId == single["id"].asInt())
			{
				int resAmount = m_vecNewAreaActData.at(j).amount2 - single["count"].asInt();
				m_vecNewAreaActData.at(j).amount3 = resAmount;

				//特价礼包如果还没买，同时剩余数量为0了，则也置为不可领取
				if(!m_vecNewAreaActData.at(j).isGetted)
				{
					m_vecNewAreaActData.at(j).isCanGet = resAmount > 0 ? true : false;
				}
				break;
			}
		}
	}

	//英雄等级
	for(int i=0; i<m_vecNewAreaActData.size(); i++)
	{
		if(m_vecNewAreaActData.at(i).taskType == 2)				//任务类型是主角等级
		{
			m_vecNewAreaActData.at(i).amount1.done = HeroModel::getInstance()->getMainHeroLevel();
		}
	}

	if(m_iCurActDay != 0 && m_iRemainTime > 0)
	{
		m_bIsActOpen = true;
		m_bIsFirstLoad = false;
	}
	else
	{
		m_bIsActOpen = false;
	}
	ObserverParam observerParam;
	notifyObservers(&observerParam);
}

void NewAreaActivityModel::updateFromSvr(const Json::Value& actData)
{
	for(int i=0; i<actData.size(); i++)
	{
		for(int j=0; j<m_vecNewAreaActData.size(); j++)
		{
			if(m_vecNewAreaActData.at(j).actId == actData[i].asInt())
			{
				m_vecNewAreaActData.at(j).isCanGet = true;
				if(m_vecNewAreaActData.at(j).taskType == 5)
				{
					m_vecNewAreaActData.at(j).amount1.done = 1;
				}
			}
		}
	}

	//通知UI
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_NEWAREA_ACT_PUSH_INFO;
	NewAreaActivityModel::getInstance()->notifyObservers(&obParam);
}

bool NewAreaActivityModel::isHaveCanGetReward()
{
	for(int i=0; i<m_vecNewAreaActData.size(); i++)
	{
		if(m_vecNewAreaActData.at(i).isCanGet == true && m_vecNewAreaActData.at(i).actOpenTime <= m_iCurActDay)
		{
			return true;
		}
	}
	return false;
}

bool NewAreaActivityModel::getIsLoaded()
{
	return m_bIsLoaded;
}

bool NewAreaActivityModel::getActStates()
{
	return m_bIsActOpen;
}

void NewAreaActivityModel::setActStates(bool states)
{
	m_bIsActOpen = states;
}

void NewAreaActivityModel::setIsFristLoad(bool isFirstLoad)
{
	m_bIsFirstLoad = isFirstLoad;
}

void NewAreaActivityModel::timeOut()
{
	m_bIsActOpen = false;
	//通知主界面，活动结束
	ObserverParam observerParam;
	notifyObservers(&observerParam);
}

NewAreaActivityModel* NewAreaActivityModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new NewAreaActivityModel();
	}
	return ms_pInstance;
}

void NewAreaActivityModel::destroyInstance()
{
	if(ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

//记录当前购买的特价礼包信息
//获取当前购买的特价礼包信息
//清空当前购买的特价礼包信息(因为和其他任务奖励是同一条ID，使用完必须清空)
void NewAreaActivityModel::setCurBuyBox(sTeJiaBox tjBox)
{
	m_sTeJiaBox = tjBox;
}

sTeJiaBox NewAreaActivityModel::getCurBuyBox()
{
	return m_sTeJiaBox;
}

void NewAreaActivityModel::clearCurBuyBox()
{
	m_sTeJiaBox.isBox = false;
	m_sTeJiaBox.days = 0;
	m_sTeJiaBox.price = 0;
}


