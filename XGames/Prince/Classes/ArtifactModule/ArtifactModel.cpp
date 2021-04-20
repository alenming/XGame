#include "ArtifactModel.h"
#include "Common/Common.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "Communication/Command.h"
#include "HeroModule/HeroModel.h"
#include "ChapterModule/ChapterModel.h"


ArtifactParam* ArtifactParam::create()
{
	ArtifactParam* artifactParam = new ArtifactParam();
	artifactParam->autorelease();
	return artifactParam;
}

ArtifactParam::~ArtifactParam()
{

}

ArtifactParam::ArtifactParam()
{
	m_vActiveCondition.clear();
	m_vFightAttri.clear();
}

bool ArtifactParam::isActived()
{
	return m_sCommonAttri.m_bIsActived;
}

bool ArtifactParam::isRefineAll()
{
	return m_sCommonAttri.m_bIsRefineAll;
}


/******************************* 分 隔 线 **********************************/

ArtifactModel* ArtifactModel::ms_pInstance = nullptr;

ArtifactModel::ArtifactModel()
{
	m_vArtifact.clear();
	m_iRefType = 0;
	initLocalData();
}

ArtifactModel::~ArtifactModel()
{

}

/************************************************************************/
/*  神器模块单例比较特殊，param: 是否需要更新本地数据，默认不需要
	在取神器模块数据使用时，可能需要更新本地数据。一般destroy时不需要更新数据                                                       
/************************************************************************/

ArtifactModel* ArtifactModel::getInstance(bool isInit)
{
	if(!ms_pInstance)
	{
		ms_pInstance = new ArtifactModel();
	}

	if(isInit)
	{
		ms_pInstance->initLocalData();
	}

	return ms_pInstance;
}

void ArtifactModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void ArtifactModel::initLocalData()
{
	//神器表格
	multimap<int, RowData>& ArtifactDT = DataManager::getInstance()->getDTArtifact()->getTableData();

	m_vArtifact.clear();
	//遍历神器数据表
	int order = 1;
	for(auto iter = ArtifactDT.begin(); iter != ArtifactDT.end(); iter++)
	{
		RowData* rowData = &(iter->second);
		if(rowData == nullptr)
		{
			CCLOG("读取神器表格出错!");
			return;
		}

		ArtifactParam* artifact = ArtifactParam::create();

		//条件
		vector<sCondition> vecConditions;
		vecConditions.clear();	
		initActiveCondition(rowData, &vecConditions);
		artifact->setActiveCondition(vecConditions);

		//属性
		vector<sFightAttri> vecAttributes;
		vecAttributes.clear();
		initFightAttri(rowData, &vecAttributes);
		artifact->setFightAttribute(vecAttributes);

		//神器通用属性
		sCommonAttri commonAttri;
		commonAttri.m_bIsActived = false;
		commonAttri.m_bIsRefineAll = false;
		commonAttri.m_iOrderNum = order;
		artifact->setCommonAttribute(commonAttri);

		m_vArtifact.pushBack(artifact);
		order++;
	}

	initRefineCost();
}

void ArtifactModel::initActiveCondition( RowData* rowData, vector<sCondition>* vecCondition )
{
	int iType = 1;
	string conditionDes = rowData->getStringData("des");
	vector<string> vecStr;

	if(!conditionDes.empty())
	{	
		StringFormat::parseCsvStringVec(conditionDes, vecStr);
	}

	//排除描述为-1的情况
	if(!vecStr.empty())
	{
		vecStr.erase(
			std::remove_if(vecStr.begin(), vecStr.end(), 
			[this](string item)
		{
			if (item.compare("-1") == 0)
			{
				return true;
			}
			return false;
		}	
		), vecStr.end());
	}	

	int level = rowData->getIntData("level");
	if(level != -1)
	{
		sCondition condition;
		condition.m_iDoneNum = HeroModel::getInstance()->getMainHeroLevel();
		condition.isExtra = false;
		condition.m_iGoalNum = level;
		condition.m_bIsDone = condition.m_iDoneNum >= condition.m_iGoalNum ? true : false;
		condition.m_iType = iType;
		if(iType <= vecStr.size())
		{
			condition.m_strDes = vecStr.at(iType-1);
		}
		condition.m_bIsStarSu = false;
		vecCondition->push_back(condition);
		iType++;
	}
	
	int heroNum = rowData->getIntData("heroNum");
	if(heroNum != -1)
	{
		sCondition condition;
		condition.m_iDoneNum = HeroModel::getInstance()->getHeroCount()-1;			//主角不算
		condition.isExtra = false;
		condition.m_iGoalNum = heroNum;
		condition.m_bIsDone = condition.m_iDoneNum >= condition.m_iGoalNum ? true : false;
		condition.m_iType = iType;
		if(iType <= vecStr.size())
		{
			condition.m_strDes = vecStr.at(iType-1);
		}
		condition.m_bIsStarSu = false;
		vecCondition->push_back(condition);
		iType++;
	}
	
	string heroStar = rowData->getStringData("heroStar");
	if(heroStar != "-1")
	{
		sCondition condition;
		//拆分条件
		if(!heroStar.empty())
		{
				int nIndex1 = heroStar.find_first_of('|', 0);
				string strTmp1 = heroStar.substr(0, nIndex1);
				string strTmp2 = heroStar.erase(0, nIndex1 + 1);
				condition.m_iGoalNum = atoi(strTmp1.c_str());
				condition.m_iExtraGoalNum = atoi(strTmp2.c_str());
		}

		condition.m_iDoneNum = 0;
		Vector<HeroParam*>* vecHero = HeroModel::getInstance()->getHeroList();
		for(auto item : *vecHero)
		{
			if(item->mHeroStars >= condition.m_iExtraGoalNum)
			{
				condition.m_iDoneNum++;
			}
		}
		condition.m_bIsDone = condition.m_iDoneNum >= condition.m_iGoalNum ? true : false;
		condition.isExtra = true;
		condition.m_iType = iType;
		if(iType <= vecStr.size())
		{
			condition.m_strDes = vecStr.at(iType-1);
		}
		condition.m_bIsStarSu = false;
		vecCondition->push_back(condition);
		iType++;
	}

	string heroAD = rowData->getStringData("heroAD");
	if(heroAD != "-1")
	{
		sCondition condition;
		//拆分条件
		if(!heroAD.empty())
		{
			int nIndex1 = heroAD.find_first_of('|', 0);
			string strTmp1 = heroAD.substr(0, nIndex1);
			string strTmp2 = heroAD.erase(0, nIndex1 + 1);
			condition.m_iGoalNum = atoi(strTmp1.c_str());
			condition.m_iExtraGoalNum = atoi(strTmp2.c_str());
		}

		condition.m_iDoneNum = 0;
		Vector<HeroParam*>* vecHero = HeroModel::getInstance()->getHeroList();
		for(auto item : *vecHero)
		{
			if(item->mAdvLvl >= condition.m_iExtraGoalNum)
			{
				condition.m_iDoneNum++;
			}
		}
		condition.m_bIsDone = condition.m_iDoneNum >= condition.m_iGoalNum ? true : false;
		condition.isExtra = true;
		condition.m_iType = iType;
		if(iType <= vecStr.size())
		{
			condition.m_strDes = vecStr.at(iType-1);
		}
		condition.m_bIsStarSu = false;
		vecCondition->push_back(condition);
		iType++;
	}

	int barrier = rowData->getIntData("barrier");
	if(barrier != -1)
	{
		FR_ChapterStory sChapterInfo;
		ChapterModel::getInstance()->getFightRecordData(sChapterInfo);
		sCondition condition;
		condition.m_iDoneNum = sChapterInfo.nCurBarrierId;			//需要获取当前的关卡章节
		condition.isExtra = false;
		condition.m_iGoalNum = barrier;
		condition.m_bIsDone = condition.m_iDoneNum >= condition.m_iGoalNum ? true : false;
		condition.m_iType = iType;
		condition.m_bIsShowProgress = false;
		if(iType <= vecStr.size())
		{
			condition.m_strDes = vecStr.at(iType-1);
		}
		condition.m_bIsStarSu = false;
		vecCondition->push_back(condition);
		iType++;
	}

	int star = rowData->getIntData("star");
	if(star != -1)
	{
		sCondition condition;
		condition.m_iDoneNum = 0;				//需要获取星宿等级
		condition.isExtra = false;
		condition.m_iGoalNum = star;
		condition.m_bIsDone = condition.m_iDoneNum >= condition.m_iGoalNum ? true : false;
		condition.m_iType = iType;
		if(iType <= vecStr.size())
		{
			condition.m_strDes = vecStr.at(iType-1);
		}
		condition.m_bIsStarSu = true;
		vecCondition->push_back(condition);
		iType++;
	}
}

void ArtifactModel::initFightAttri( RowData* rowData, vector<sFightAttri>* vecAttri )
{
	float hp = rowData->getFloatData("hp");
	if(hp != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_HP;
		attri.fValue = 0;
		attri.fMaxValue = hp;
		//attri.strChName = STR_UTF8("血量");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(10008);
		//英文名称填对应的和服务器商定好的发过来的字段，方便后面解析服务器数据, 以下同
		attri.strEnName = "hp";
		vecAttri->push_back(attri);
	}

	float atk = rowData->getFloatData("atk");
	if(atk != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_ATK;
		attri.fValue = 0;
		attri.fMaxValue = atk;
		//attri.strChName = STR_UTF8("攻击");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(10009);
		attri.strEnName = "atk";
		vecAttri->push_back(attri);
	}

	float pdd = rowData->getFloatData("pdd");
	if(pdd != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_PHYDEFENSE;
		attri.fValue = 0;
		attri.fMaxValue = pdd;
		//attri.strChName = STR_UTF8("物防");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(10010);
		attri.strEnName = "pdd";
		vecAttri->push_back(attri);
	}

	float mdd = rowData->getFloatData("mdd");
	if(mdd != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_MGCDEFENSE;
		attri.fValue = 0;
		attri.fMaxValue = mdd;
		//attri.strChName = STR_UTF8("法防");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(10011);
		attri.strEnName = "mdd";
		vecAttri->push_back(attri);
	}

	float hit = rowData->getFloatData("hit");
	if(hit != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_HIT;
		attri.fValue = 0;
		attri.fMaxValue = hit;
		//attri.strChName = STR_UTF8("命中");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(10012);
		attri.strEnName = "hit";
		vecAttri->push_back(attri);
	}

	float miss = rowData->getFloatData("miss");
	if(miss != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_HIT;
		attri.fValue = 0;
		attri.fMaxValue = miss;
		//attri.strChName = STR_UTF8("闪避");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(10013);
		attri.strEnName = "miss";
		vecAttri->push_back(attri);
	}

	float criPrb = rowData->getFloatData("criPrb");
	if(criPrb != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_CRITPROB;
		attri.fValue = 0;
		attri.fMaxValue = criPrb;
		//attri.strChName = STR_UTF8("暴击");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(10014);
		attri.strEnName = "criPrb";
		vecAttri->push_back(attri);
	}

	float criRes = rowData->getFloatData("criRes");
	if(criRes != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_UNCRITPROB;
		attri.fValue = 0;
		attri.fMaxValue = criRes;
		//attri.strChName = STR_UTF8("抗暴");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(10015);
		attri.strEnName = "criRes";
		vecAttri->push_back(attri);
	}

	float criCoe = rowData->getFloatData("criCoe");
	if(criCoe != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_CRITDAMAGE;
		attri.fValue = 0;
		attri.fMaxValue = criCoe;
		//attri.strChName = STR_UTF8("暴伤");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(10016);
		attri.strEnName = "criCoe";
		vecAttri->push_back(attri);
	}

	float coeRes = rowData->getFloatData("coeRes");
	if(coeRes != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_UNCRITDAMAGE;
		attri.fValue = 0;
		attri.fMaxValue = coeRes;
		//attri.strChName = STR_UTF8("暴伤减免");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(10017);
		attri.strEnName = "coeRes";
		vecAttri->push_back(attri);
	}

	float ctrPrb = rowData->getFloatData("ctrPrb");
	if(ctrPrb != 0)
	{
		sFightAttri attri;
		attri.eType = eTYPE_COMBOPROB;
		attri.fValue = 0;
		attri.fMaxValue = ctrPrb;
		//attri.strChName = STR_UTF8("连击率");
		attri.strChName = DataManager::getInstance()->searchCommonTexdtById(20152);
		attri.strEnName = "ctrPrb";
		vecAttri->push_back(attri);
	}

	std::sort(vecAttri->begin(), vecAttri->end(), 
		[](const sFightAttri p1, const sFightAttri p2)->bool
	{
		//按类型排序
		return p1.eType < p2.eType;
	});
}


void ArtifactModel::checkRefineAll()
{
	for(auto item : m_vArtifact)
	{
		vector<sFightAttri> fightAttri = item->getFightAttribute();
		sCommonAttri attri = item->getCommonAttribute();

		attri.m_bIsRefineAll = true;
		for(auto iter=fightAttri.begin(); iter!=fightAttri.end(); iter++)
		{
			if(iter->fValue < iter->fMaxValue)
			{
				attri.m_bIsRefineAll = false;
				break;
			}
		}
		item->setCommonAttribute(attri);
	}
}


void ArtifactModel::updateDataFromSvr(Json::Value& data)
{
	Json::Value artiList = data["artList"];

	for(auto item : m_vArtifact)
	{
		vector<sFightAttri> fightAttri = item->getFightAttribute();
		sCommonAttri attri = item->getCommonAttribute();
		vector<sCondition> condition = item->getActiveCondition();

		//神器属性列表解析
		for(int i=0; i<artiList.size(); i++)
		{
			Json::Value single = artiList[i];
			if(attri.m_iOrderNum + 10 == single["itemId"].asInt())
			{
				attri.m_bIsActived = true;
				for(auto iter=fightAttri.begin(); iter!=fightAttri.end(); iter++)
				{
					iter->fValue = single[iter->strEnName].asDouble();		
				}
				item->setFightAttribute(fightAttri);
				item->setCommonAttribute(attri);
			}
		}
		

		//星宿等级条件
		if(!attri.m_bIsActived)
		{
			for(auto iter=condition.begin(); iter!=condition.end(); iter++)
			{
				if(iter->m_bIsStarSu)
				{
					iter->m_iDoneNum = data["starLvl"].asInt(); //服务器星宿等级字段解析
					iter->m_bIsDone = iter->m_iDoneNum >= iter->m_iGoalNum ? true : false;
					item->setActiveCondition(condition);
					break;
				}
			}
		}
	}

	//检测洗练已满的道具
	checkRefineAll();

	//通知UI, 刷新帮派主界面
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_ARTIFACT_MAIN;
	notifyObservers(&obParam);
}

void ArtifactModel::updateActiveData()
{
	for(auto item : m_vArtifact)
	{
		sCommonAttri attri = item->getCommonAttribute();
		if(attri.m_iOrderNum == m_iCurArtifactID)
		{
			attri.m_bIsActived = true;
			item->setCommonAttribute(attri);
			break;
		}
	}

	//通知UI, 刷新帮派主界面
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_ARTIFACT_ACTIVE;
	notifyObservers(&obParam);
}

void ArtifactModel::updateRefineData(Json::Value& data)
{
	Json::Value props = data["props"];
	for(auto item : m_vArtifact)
	{
		vector<sFightAttri> fightAttri = item->getFightAttribute();
		sCommonAttri attri = item->getCommonAttribute();

		if(attri.m_iOrderNum == m_iCurArtifactID)
		{
			for(auto iter=fightAttri.begin(); iter!=fightAttri.end(); iter++)
			{
				iter->fTempValue = props[iter->strEnName].asDouble();		
			}
			item->setFightAttribute(fightAttri);
			break;
		}	
	}

	//通知UI, 刷新帮派主界面
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_ARTIFACT_REFINE;
	notifyObservers(&obParam);
}

void ArtifactModel::updateSaveData()
{
	for(auto item : m_vArtifact)
	{
		vector<sFightAttri> fightAttri = item->getFightAttribute();
		sCommonAttri attri = item->getCommonAttribute();

		if(attri.m_iOrderNum == m_iCurArtifactID)
		{
			for(auto iter=fightAttri.begin(); iter!=fightAttri.end(); iter++)
			{
				iter->fValue += iter->fTempValue;
				iter->fTempValue = 0;
			}
			item->setFightAttribute(fightAttri);
			break;
		}
	}

	//检测洗练已满
	checkRefineAll();

	//通知UI, 刷新帮派主界面
	ObserverParam obParam;
	obParam.id = nMAIN_CMD_ARTIFACT_SAVE;
	notifyObservers(&obParam);
}

void ArtifactModel::resetCancelRefine()
{
	for(auto item : m_vArtifact)
	{
		vector<sFightAttri> fightAttri = item->getFightAttribute();
		sCommonAttri attri = item->getCommonAttribute();

		if(attri.m_iOrderNum == m_iCurArtifactID)
		{
			for(auto iter=fightAttri.begin(); iter!=fightAttri.end(); iter++)
			{
				iter->fTempValue = 0;
			}
			item->setFightAttribute(fightAttri);
			break;
		}	
	}
}

ArtifactParam* ArtifactModel::getArtifactByOrder( int orderNum )
{
	for(auto item : m_vArtifact)
	{
		if(item->getCommonAttribute().m_iOrderNum == orderNum)
		{
			return item;
			break;
		}
	}

	return nullptr;
}

Vector<ArtifactParam*> ArtifactModel::getAllArtifact()
{
	return m_vArtifact;
}

sRefineCost ArtifactModel::getRefineCost()
{
	return m_sCost;
}

string ArtifactModel::getAttriIconPath(eAttriType eType)
{
	stringstream ss;	

	switch (eType)
	{
	case eTYPE_ATK:
		ss << "Image/Icon/global/img_gongji.png";
		break;
	case eTYPE_HP:
		ss << "Image/Icon/global/img_xueliang.png";
		break;
	case eTYPE_PHYDEFENSE:
		ss << "Image/Icon/global/img_wufang.png";
		break;
	case eTYPE_MGCDEFENSE:
		ss << "Image/Icon/global/img_mofang.png";
		break;
	case eTYPE_CRITPROB:
		ss << "Image/Icon/global/img_shuxing_baoji.png";
		break;
	case eTYPE_UNCRITPROB:
		ss << "Image/Icon/global/img_shuxing_baokang.png";
		break;
	case eTYPE_CRITDAMAGE:
		ss << "Image/Icon/global/img_shuxing_baoshang.png";
		break;
	case eTYPE_UNCRITDAMAGE:			//暴伤减免没有图标
		ss << "Image/Icon/global/img_gongji.png";
		break;
	case eTYPE_MISS:
		ss << "Image/Icon/global/img_shuxing_shanbi.png";
		break;
	case eTYPE_HIT:
		ss << "Image/Icon/global/img_shuxing_mingzhong.png";
		break;
	case eTYPE_COMBOPROB:
		ss << "Image/Icon/global/img_shuxing_lianji.png";
		break;

	default:
		return "";
		break;
	}

	return ss.str();	
}

void ArtifactModel::initRefineCost()
{
	RowData* rowData1 = DataManager::getInstance()->searchToolSystemById(RefineCostStore);
	if(rowData1)
	{
		m_sCost.m_iCostStore = rowData1->getIntData("value");
	}

	RowData* rowData2 = DataManager::getInstance()->searchToolSystemById(RefineCostCoin);
	if(rowData2 != nullptr)
	{
		m_sCost.m_iCostCoin = rowData2->getIntData("value");
	}

	RowData* rowData3 = DataManager::getInstance()->searchToolSystemById(RefineCostGold);
	if(rowData3 != nullptr)
	{
		m_sCost.m_iCostGold = rowData3->getIntData("value");
	}
}

void ArtifactModel::setRefineType(int times)
{
	m_iRefType = times;
}

int ArtifactModel::getRefineType()
{
	return m_iRefType;
}














