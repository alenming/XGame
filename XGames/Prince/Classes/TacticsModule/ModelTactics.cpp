#include "ModelTactics.h"
#include "TacticsDataDef.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "Communication/Command.h"
#include "Common/Common.h"

ModelTactics* ModelTactics::ms_pInstance = nullptr;

ModelTactics::ModelTactics()
{
	m_bIsUsedTactics = false;
	m_bIsInit = false;
	m_vRowMaterialCostCoin.clear();
	parseRowMaterialCost();
}

ModelTactics::~ModelTactics()
{

}

ModelTactics* ModelTactics::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new ModelTactics();
	}
	return ms_pInstance;
}

void ModelTactics::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void ModelTactics::initRowData()
{
	for(int i=0; i<m_vCurRowData.size(); i++)
	{
		RowData* row = DataManager::getInstance()->searchTacticsById(m_vCurRowData.at(i).mRowID);
		if(row != nullptr)
		{
			int limit = row->getIntData("limit");
			if(limit != -1)
			{
				RowData* limitRow = DataManager::getInstance()->searchTacticsById(limit);
				m_vCurRowData.at(i).mRowLvLimit = limitRow->getIntData("level");
			}
			else
			{
				m_vCurRowData.at(i).mRowLvLimit = row->getIntData("limit");
			}

			m_vCurRowData.at(i).mRowLevel = row->getIntData("level");

			//排属性加成
			m_vCurRowData.at(i).mRowRangeHp = row->getIntData("hp");						//排血量增量
			m_vCurRowData.at(i).mRowRangeAtk = row->getIntData("atk");						//排攻击增量
			m_vCurRowData.at(i).mRowRangePhyDef = row->getIntData("pdd");					//排物防增量
			m_vCurRowData.at(i).mRowRangeMagicDef = row->getIntData("mdd");					//排法防增量

			m_vCurRowData.at(i).mRowLvUpExp = row->getIntData("exp");						//升级所需经验
		}	
	}
}

void ModelTactics::initTacticsListData()
{
	for(int i=0; i<NUM_TACTICS_LIST; i++)
	{
		sTacticsData tacticsRowData;

		tacticsRowData.mRowId = (4+i)*100+1;

		RowData* rowData = DataManager::getInstance()->searchTacticsById(tacticsRowData.mRowId);

		tacticsRowData.mName = rowData->getStringData("name");
		tacticsRowData.mIconId = rowData->getIntData("resId");
		tacticsRowData.mLevel = rowData->getIntData("level");
		//升级条件
		string strLimit = rowData->getStringData("limit");
		if(!strLimit.empty())
		{
			vector<string> vecStr;
			StringFormat::parseCsvStringVec(strLimit, vecStr);
			tacticsRowData.mLvUpLimit.clear();
			for(int i=0; i<vecStr.size(); i++)
			{
				RowData* row = DataManager::getInstance()->searchTacticsById(atoi(vecStr.at(i).c_str()));
				tacticsRowData.mLvUpLimit.push_back(row->getIntData("level"));
			}
		}

		tacticsRowData.mAttrType = (ATTR_TYPE)i;
		setTypeStrKey(tacticsRowData.mAttrType, tacticsRowData.mAttriName);
		tacticsRowData.mAttrValue = rowData->getFloatData(tacticsRowData.mAttriName.en_keyName);

		//下一级属性增加
		if(tacticsRowData.mLevel < TACTICS_MAX_LEVEL)
		{
			RowData* rowNextVal = DataManager::getInstance()->searchTacticsById(tacticsRowData.mRowId+1);
			if(rowNextVal != nullptr)
			{
				float nextLvAtt = rowNextVal->getFloatData(tacticsRowData.mAttriName.en_keyName);
				tacticsRowData.mNextAttrVal = nextLvAtt - tacticsRowData.mAttrValue;
			}
		}
		else
		{
			tacticsRowData.mNextAttrVal = 0.0f;
		}
		
		//特殊材料
		tacticsRowData.mLvUpMaterialId1 = rowData->getIntData("mat1Id");
		tacticsRowData.mLvUpMaterialCnt1 = rowData->getIntData("amount1");

		//公用材料
		RowData* sysData = DataManager::getInstance()->getDTToolSystem()->searchDataById(TACTICS_UPGRADE_MATERIAL_ID);
		tacticsRowData.mLvUpMaterialId2 = sysData->getIntData("value");

		tacticsRowData.mLvUpCoin = rowData->getIntData("costCoin");

		tacticsRowData.mIsActivated = false;
		tacticsRowData.mIsUsing = false;

		m_vTacticsDataList.push_back(tacticsRowData);
	}
}

void ModelTactics::reSetRowData(int rowId, int rowIndex)
{
	RowData* row = DataManager::getInstance()->searchTacticsById(rowId);

	int limit = row->getIntData("limit");
	if(limit != -1)
	{
		RowData* limitRow = DataManager::getInstance()->searchTacticsById(limit);
		m_vCurRowData.at(rowIndex).mRowLvLimit = limitRow->getIntData("level");
	}

	m_vCurRowData.at(rowIndex).mRowLevel = row->getIntData("level");

	//排属性加成
	m_vCurRowData.at(rowIndex).mRowRangeHp = row->getIntData("hp");						//排血量增量
	m_vCurRowData.at(rowIndex).mRowRangeAtk = row->getIntData("atk");						//排攻击增量
	m_vCurRowData.at(rowIndex).mRowRangePhyDef = row->getIntData("pdd");					//排物防增量
	m_vCurRowData.at(rowIndex).mRowRangeMagicDef = row->getIntData("mdd");					//排法防增量

	m_vCurRowData.at(rowIndex).mRowLvUpExp = row->getIntData("exp");						//升级所需经验
}

void ModelTactics::initFromSvr(const Json::Value& tacticsDataList)
{
	Json::Value matrix = tacticsDataList["matrix"];
	Json::Value matrixList = matrix["matrixList"];

	//前排数据
	m_vCurRowData.clear();
	for(int i=1; i<4; i++)
	{
		sCurRowData rowData;
		rowData.mRowCurExp = matrix[string("exp")+_TO_STR(i)].asInt();
		rowData.mRowID = matrix[string("row")+_TO_STR(i)+string("Id")].asInt();
		rowData.mRowType = i;
		m_vCurRowData.push_back(rowData);
	}
	initRowData();


	//初始化阵法数据
	m_vTacticsDataList.clear();
	int matrixId = matrix["matrixId"].asInt();
	initTacticsListData();
	updateTacticsList(matrixList);

	m_vCurTacticsDataList.clear();
	for(int i=0; i<m_vTacticsDataList.size(); i++)
	{
		if(m_vTacticsDataList.at(i).mIsActivated)
		{
			m_vCurTacticsDataList.push_back(m_vTacticsDataList.at(i));
			if(m_vTacticsDataList.at(i).mRowId/100 == matrixId/100)
			{
				m_vTacticsDataList.at(i).mIsUsing = true;
				m_bIsUsedTactics = true;
				m_curUsingTacticsData = m_vTacticsDataList.at(i);
			}
		}
	}

	m_bIsInit = true;

	//通知UI显示
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_TACTICS_MAIN_LIST;
	notifyObservers(&obParam);
}

void ModelTactics::updateTacticsList(const Json::Value& tacData)
{
	for(int i=0; i<tacData.size(); i++)
	{
		for(int j=0; j<m_vTacticsDataList.size(); j++)
		{
			if(m_vTacticsDataList.at(j).mRowId/100 == tacData[i].asInt()/100)
			{
				m_vTacticsDataList.at(j).mRowId = tacData[i].asInt();
				RowData* rowData = DataManager::getInstance()->searchTacticsById(m_vTacticsDataList.at(j).mRowId);

				m_vTacticsDataList.at(j).mName = rowData->getStringData("name");
				m_vTacticsDataList.at(j).mIconId = rowData->getIntData("resId");
				m_vTacticsDataList.at(j).mLevel = rowData->getIntData("level");
				//升级条件
				string strLimit = rowData->getStringData("limit");
				if(!strLimit.empty())
				{
					vector<string> vecStr;
					StringFormat::parseCsvStringVec(strLimit, vecStr);
					m_vTacticsDataList.at(j).mLvUpLimit.clear();
					for(string strTmp : vecStr)
					{
						RowData* row = DataManager::getInstance()->searchTacticsById(atoi(strTmp.c_str()));
						m_vTacticsDataList.at(j).mLvUpLimit.push_back(row->getIntData("level"));
					}
				}

				m_vTacticsDataList.at(j).mAttrValue = rowData->getFloatData(m_vTacticsDataList.at(j).mAttriName.en_keyName);

				if(m_vTacticsDataList.at(j).mLevel < TACTICS_MAX_LEVEL)
				{
					RowData* rowNextVal = DataManager::getInstance()->searchTacticsById(m_vTacticsDataList.at(j).mRowId+1);
					if(rowNextVal != nullptr)
					{
						float nextLvAtt = rowNextVal->getFloatData(m_vTacticsDataList.at(j).mAttriName.en_keyName);
						m_vTacticsDataList.at(j).mNextAttrVal = nextLvAtt - m_vTacticsDataList.at(j).mAttrValue;
					}
				}
				else
				{
					m_vTacticsDataList.at(j).mNextAttrVal = 0.0f;
				}

				//特殊材料
				m_vTacticsDataList.at(j).mLvUpMaterialId1 = rowData->getIntData("mat1Id");
				m_vTacticsDataList.at(j).mLvUpMaterialCnt1 = rowData->getIntData("amount1");

				//公用材料
				RowData* sysData = DataManager::getInstance()->getDTToolSystem()->searchDataById(TACTICS_UPGRADE_MATERIAL_ID);
				m_vTacticsDataList.at(j).mLvUpMaterialId2 = sysData->getIntData("value");

				m_vTacticsDataList.at(j).mLvUpCoin = rowData->getIntData("costCoin");

				m_vTacticsDataList.at(j).mIsActivated = true;
			}
		}
		//更新已激活的阵法列表和正在使用的阵法
		m_vCurTacticsDataList.clear();
		for(int i=0; i<m_vTacticsDataList.size(); i++)
		{		
			if(m_vTacticsDataList.at(i).mIsActivated)
			{
				m_vCurTacticsDataList.push_back(m_vTacticsDataList.at(i));
				if(m_vTacticsDataList.at(i).mIsUsing && isUsedTactics())
				{
					m_curUsingTacticsData = m_vTacticsDataList.at(i);
				}
			}
		}
	}
}

void ModelTactics::reflushUIAfterUpTactics()
{
	//通知UI刷新显示
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_TACTICS_UPGRADE;
	notifyObservers(&obParam);
}

void ModelTactics::updateUsedTactics(const Json::Value& tacData)
{
	int matrixId = tacData["matrixId"].asInt();

	m_bIsUsedTactics = true;
	int index = 0;
	for(int i=0; i<m_vTacticsDataList.size(); i++)
	{
		m_vTacticsDataList.at(i).mIsUsing = false;
		if(m_vTacticsDataList.at(i).mRowId == matrixId)
		{
			index = i;
		}
	}
	m_vTacticsDataList.at(index).mIsUsing = true;
	m_curUsingTacticsData = m_vTacticsDataList.at(index);

	//通知UI刷新显示
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_TACTICS_USE_TACTICS;
	notifyObservers(&obParam);
}

void ModelTactics::updateActivedNewTactics()
{
	sCurRowData curFrontRowData = m_vCurRowData.at(FRONT_ROW_TYPE);
	sCurRowData curMidRowData = m_vCurRowData.at(MIDDLE_ROW_TYPE);
	sCurRowData curBackRowData = m_vCurRowData.at(BACK_ROW_TYPE);

	for(int i=0; i<m_vTacticsDataList.size(); i++)
	{
		if(!m_vTacticsDataList.at(i).mIsActivated)
		{
			if(curFrontRowData.mRowLevel >= m_vTacticsDataList.at(i).mLvUpLimit.at(0) &&
				curMidRowData.mRowLevel >= m_vTacticsDataList.at(i).mLvUpLimit.at(1) &&
				curBackRowData.mRowLevel >= m_vTacticsDataList.at(i).mLvUpLimit.at(2))
			{
				m_vTacticsDataList.at(i).mIsActivated = true;
			}
		}
	}
}

//服务器回馈的排强化
void ModelTactics::updateRowData(const Json::Value& rowData)
{
	int rowId = rowData["rowId"].asInt();
	int exp = rowData["exp"].asInt();

	for(int i=0; i<m_vCurRowData.size(); i++)
	{
		if(rowId == m_vCurRowData.at(i).mRowID)
		{
			m_vCurRowData.at(i).mRowCurExp = exp;
			break;
		}
		else if(rowId/100 == m_vCurRowData.at(i).mRowID/100)		//排升级了
		{			
			//int level = rowData["level"].asInt();
			m_vCurRowData.at(i).mRowID = rowId;
			m_vCurRowData.at(i).mRowCurExp = exp;
			reSetRowData(rowId, i);
			break;
		}
	}

	//重置激活的阵法
	updateActivedNewTactics();

	//通知UI刷新显示
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_TACTICS_ROW_UPGRADE;
	notifyObservers(&obParam);
}

void ModelTactics::parseRowMaterialCost()
{
	RowData* rowData = DataManager::getInstance()->getDTToolSystem()->searchDataById(ROW_UPGRADE_MATERIAL_LIST_ID1);

	string materialInfo = rowData->getStringData("value");

	if(!materialInfo.empty())
	{
		vector<string> vecStr;
		StringFormat::parseCsvStringVec(materialInfo, vecStr);

		for(string strTmp : vecStr)
		{
			int nIndex1 = strTmp.find_first_of(',', 0);
			string strTmp1 = strTmp.substr(0, nIndex1);
			string strTmp2 = strTmp.erase(0, nIndex1 + 1);
			int matId = atoi(strTmp1.c_str());
			int matCost = atoi(strTmp2.c_str());

			sRowUpMaterialPrice matterialInfo;
			matterialInfo.id = matId;
			matterialInfo.costCoin = matCost;

			m_vRowMaterialCostCoin.push_back(matterialInfo);
		}
	}
}

sCurRowData& ModelTactics::getCurRowData(eRowType type)
{
	return m_vCurRowData.at(type);
}

vector<sTacticsData>& ModelTactics::getTacticsDataList()
{
	return m_vTacticsDataList;
}

vector<sTacticsData>& ModelTactics::getActedTacticsDataList()
{
	return m_vCurTacticsDataList;
}

sTacticsData& ModelTactics::getCurUsingTacticsData()
{
	return m_curUsingTacticsData;
}

bool ModelTactics::isUsedTactics()
{
	return m_bIsUsedTactics;
}

bool ModelTactics::isInittedFromSvr()
{
	return m_bIsInit;
}

int ModelTactics::getRowUpMaterialPrice(int id)
{
	for(int i=0; i<m_vRowMaterialCostCoin.size(); i++)
	{
		if(id == m_vRowMaterialCostCoin.at(i).id)
		{
			return m_vRowMaterialCostCoin.at(i).costCoin;
		}
	}
	return 0;
}

string ModelTactics::getCurUsingTacticsAttribute()
{
	if(!m_bIsUsedTactics)
	{
		//没有正在使用的阵法
		return DataManager::getInstance()->searchCommonTexdtById(20148);
	}
	else
	{
		string attriValue = _TO_STR((int)(m_curUsingTacticsData.mAttrValue*100));
		string attriDes = m_curUsingTacticsData.mAttriName.ch_name;
		string curUsingTacticsDes =  attriValue + "%" + attriDes;
		return curUsingTacticsDes;
	}
}

void ModelTactics::setTypeStrKey( ATTR_TYPE type, sAttriName& attName)
{
	switch (type)
	{
	case ATTR_KIND_QI_XING:
		{
			attName.en_keyName = "hpCoe";
			attName.ch_name = DataManager::getInstance()->searchCommonTexdtById(10008);
			attName.icon_path = "Image/Icon/global/img_xueliang.png";
		}
		break;
	case ATTR_KIND_TIAN_GANG:
		{
			attName.en_keyName = "pddCoe";
			attName.ch_name = DataManager::getInstance()->searchCommonTexdtById(10244);
			attName.icon_path = "Image/Icon/global/img_wufang.png";
		}
		break;
	case ATTR_KIND_CI_HUN:
		{
			attName.en_keyName = "atkCoe";
			attName.ch_name =  DataManager::getInstance()->searchCommonTexdtById(10009);
			attName.icon_path = "Image/Icon/global/img_gongji.png";
		}
		break;
	case ATTR_KIND_HE_YI:
		{
			attName.en_keyName = "criPrb";
			attName.ch_name = DataManager::getInstance()->searchCommonTexdtById(10014);
			attName.icon_path = "Image/Icon/global/img_shuxing_baoji.png";
		}
		break;
	case ATTR_KIND_FEI_YU:
		{
			attName.en_keyName = "hit";
			attName.ch_name = DataManager::getInstance()->searchCommonTexdtById(10012);
			attName.icon_path = "Image/Icon/global/img_shuxing_mingzhong.png";
		}
		break;
	case ATTR_KIND_JIAN_XING:
		{
			attName.en_keyName = "miss";
			attName.ch_name = DataManager::getInstance()->searchCommonTexdtById(10013);
			attName.icon_path = "Image/Icon/global/img_shuxing_shanbi.png";
		}
		break;
	case ATTR_KIND_TIAN_MEN:
		{
			attName.en_keyName = "ctrPrb";
			attName.ch_name = DataManager::getInstance()->searchCommonTexdtById(20152);
			attName.icon_path = "Image/Icon/global/img_shuxing_lianji.png";
		}
		break;
	default:
		break;
	}
}


