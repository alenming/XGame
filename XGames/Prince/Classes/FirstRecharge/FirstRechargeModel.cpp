#include "FirstRechargeModel.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"


#define FIRST_RECHARGE_REWARD_ID	10107

FirstRechargeModel* FirstRechargeModel::ms_pInstance = nullptr;
FirstRechargeModel::FirstRechargeModel()
{
	mFirstRechargeState = -1;
	initDataFromLocal();
}

FirstRechargeModel::~FirstRechargeModel()
{

}

FirstRechargeModel* FirstRechargeModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new FirstRechargeModel();
	}
	return ms_pInstance;
}

void FirstRechargeModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void FirstRechargeModel::initDataFromLocal()
{
	RowData* rowData = DataManager::getInstance()->searchToolSystemById(FIRST_RECHARGE_REWARD_ID);
	if(rowData)
	{
		string rewardInfo = rowData->getStringData("value");

		if(!rewardInfo.empty())
		{
			vector<string> vecStr;
			StringFormat::parseCsvStringVec(rewardInfo, vecStr);

			for(string strTmp : vecStr)
			{
				int nIndex1 = strTmp.find_first_of(',', 0);
				string strTmp1 = strTmp.substr(0, nIndex1);
				string strTmp2 = strTmp.erase(0, nIndex1 + 1);				

				sReward rewardItem;
				rewardItem.id = atoi(strTmp1.c_str());
				rewardItem.count = atoi(strTmp2.c_str());

				m_vRewardList.push_back(rewardItem);
			}
		}
	}
}

vector<sReward>& FirstRechargeModel::getFirstRechargeReward()
{
	return m_vRewardList;
}