#include "DoubleBonusModel.h"

DoubleBonusModel* DoubleBonusModel::m_pInstance = nullptr;

DoubleBonusModel::DoubleBonusModel(void)
{
	m_Data.clear();
}


DoubleBonusModel::~DoubleBonusModel(void)
{
	for_each(m_Data.begin(), m_Data.end(), [=](map<DOUBLE_BONUS_TYPE, vector<DoubleBonusData*> >::value_type _pair){
		for_each(_pair.second.begin(), _pair.second.end(), [=](DoubleBonusData* data){
			delete data;
			data = nullptr;
		});
	});
	m_Data.clear();
}

DoubleBonusModel* DoubleBonusModel::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new DoubleBonusModel();
	}
	return m_pInstance;
}

void DoubleBonusModel::destoryInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void DoubleBonusModel::onTimeOut(DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE type, void* data)
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	DoubleBonusModel::getInstance()->notifyObservers((void*)&param);
}

bool DoubleBonusModel::isDoubleBonusValid( DOUBLE_BONUS_TYPE type )
{
	bool bIsValid = false;
	auto iter = m_Data.find(type);
	if (iter != m_Data.end())
	{
		auto it = find_if(iter->second.begin(), iter->second.end(), [=](DoubleBonusData* data)->bool {
			return data->isValid();
		});
		bIsValid = (it != iter->second.end());
	}
	return bIsValid;
}

void DoubleBonusModel::parseDoubleBonusData( const Json::Value &data )
{
	if (!m_Data.empty())
		return;

	// 先解析时间和类型，再根据id取出这一组中的条件;
	Json::Value infoList = data["DoubleInfoList"];
	Json::Value timeList = data["DoubleTimeList"];
	for (int i = 0; i < timeList.size(); ++i)
	{
		Json::Value single = timeList[i];
		DoubleBonusData* dbData = new DoubleBonusData();
		dbData->nModuleId = single["id"].asInt();
		dbData->nType = getTypeByInt(single["dataTyp"].asInt());
		dbData->nStartStamp = single["openTime"].asInt();
		dbData->nEndStamp = single["endTime"].asInt();

		// 查找条件;
		Json::Value info = infoList[_TO_STR(dbData->nModuleId)];

		// 目前只有一个字段，如果再有，可以重新扩展reserve字段;
		int index = 0;
		dbData->nReservedParam1 = info[index].asInt();

		// 是否有同类型的多个活动(数据结构以type来区分并索引，不能用id);
		// 考虑极限情况，例如配置两个普通关卡的活动，开启时间很近但不同，是应该放在同一组的;
		auto it = m_Data.find(dbData->nType);
		if (it == m_Data.end())
		{
			vector<DoubleBonusData*>  vcSameTypeData;
			vcSameTypeData.push_back(dbData);
			m_Data.insert(make_pair(dbData->nType, vcSameTypeData));
		}
		else
		{
			it->second.push_back(dbData);
		}
	}

	// 
	checkAndRun();
}

DOUBLE_BONUS_TYPE DoubleBonusModel::getTypeByInt( int nVal )
{
	// 跟配表的字段类型做一下转换;
	// 没有把枚举值直接定义为配表值是因为这里方便default容错;
	DOUBLE_BONUS_TYPE type;
	switch (nVal)
	{
	case 30:	type = DOUBLE_BONUS_TYPE_STORY;				break;
	case 31:	type = DOUBLE_BONUS_TYPE_ELITE;				break;
	case 32:	type = DOUBLE_BONUS_TYPE_ACTIVITY_COIN;		break;
	case 33:	type = DOUBLE_BONUS_TYPE_ACTIVITY_EXP;		break;
	case 34:	type = DOUBLE_BONUS_TYPE_ACTIVITY_CHIP;		break;
	case 35:	type = DOUBLE_BONUS_TYPE_BUY_POWER;			break;
	case 36:	type = DOUBLE_BONUS_TYPE_BUY_COIN;			break;
	default:	type = DOUBLE_BONUS_TYPE_NONE;				break;
	}
	return type;
}

void DoubleBonusModel::checkAndRun()
{
	auto callBack = [=](float dt)->void
	{
		Director::getInstance()->getScheduler()->unschedule("DoubleBonusModel::checkAndRun", this);

		// 遍历所有活动数据，使生效;
		for_each(m_Data.begin(), m_Data.end(), [=](map<DOUBLE_BONUS_TYPE, vector<DoubleBonusData*> >::value_type _pair){
			for_each(_pair.second.begin(), _pair.second.end(), [=](DoubleBonusData* data){
				data->startListener();
			});
		});
	};
	Director::getInstance()->getScheduler()->schedule(callBack, this, 0, 0, 0, false, "DoubleBonusModel::checkAndRun");
	
}

const vector<DoubleBonusData*>* DoubleBonusModel::getDataByType( DOUBLE_BONUS_TYPE type )
{
	vector<DoubleBonusData*>* vcData = nullptr;
	auto iter = m_Data.find(type);
	if (iter != m_Data.end())
	{
		vcData = &(iter->second);
	}
	return vcData;
}
