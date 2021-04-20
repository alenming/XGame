/***************英雄缘分的计算规则****************
1. 英雄缘分数据不包括主角
2. 后端不记录英雄缘分数据，全部由客户端自行查表获取
3. 前端的计算规则：
   a. 每个英雄目前会对其它英雄总共产生三条缘分										petBase表中fateHero2字段
   b. 每个英雄目前会受到其它英雄总共赋予三条缘分									petBase表中fateHero字段
   c. 每个英雄会与兵书/马书产生缘分，部分会与武器(天命)产生缘分
   d. 综abc，每个英雄会有5条或者6条缘分属性，俱在HERO_RELATION表格中查找
   e. 概念-缘分类型：1.2.3表示英雄赋予的缘分；
      4.5.6依次对应马书，兵书，武器赋予的缘分
   f. 概念-缘分激活条件：当缘分类型是1.2.3时，读取system表ID=10138的数据
	  当缘分类型是4.5.6时，不差表，默认为1
   g. 缘分激活条件，实际上是根据缘分英雄的觉醒等级来比较判定
   h. 缘分ID = heroTempId*100 + 缘分类型*100 + 缘分激活条件
   i. 根据缘分ID查找HERO_RELATION表，获得缘分相关数据

****************英雄缘分的计算规则***************/


#include "HeroRelationModel.h"
#include "Utils/StringFormat.h"
#include "HeroModule/HeroModel.h"
#include "SmithyModule/SmithyDef.h"
#include "SmithyModule/SmithyModel.h"
#include "Common/Common.h"

HeroRelationModel* HeroRelationModel::ms_pInstance = nullptr;

HeroRelationModel::HeroRelationModel()
{
	m_vFateHeroId.clear();
	m_vFateId.clear();
	m_vFateLimit.clear();
	m_vFateAttri.clear();
}

HeroRelationModel::~HeroRelationModel()
{

}

HeroRelationModel* HeroRelationModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new HeroRelationModel();

		ms_pInstance->m_vFateHeroId.clear();
		ms_pInstance->m_vFateId.clear();
		ms_pInstance->m_vFateLimit.clear();
		ms_pInstance->m_vFateAttri.clear();
	}
	return ms_pInstance;
}

void HeroRelationModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void HeroRelationModel::initData(int tempLateId)
{
	//关联的英雄ID
	RowData* _rowDataPet = DataManager::getInstance()->searchCharacterById(tempLateId);
	if(_rowDataPet != nullptr)
	{
		string _fateHero = _rowDataPet->getStringData("fateHero2");
		m_vFateHeroId.clear();
		vector<int> vecFateHeroData;
		StringFormat::parseCsvStringVec(_fateHero, vecFateHeroData);
		for(int i=0; i<vecFateHeroData.size(); i++)
		{
			sFateHeroOrder fateHeroData;
			fateHeroData.valueData = vecFateHeroData.at(i);
			fateHeroData.order = i+1;
			m_vFateHeroId.push_back(fateHeroData);
		}
	}

	//激活条件
	RowData* _rowData = DataManager::getInstance()->getDTToolSystem()->searchDataById(HERO_RELATION_LIMIT_ID);
	if(_rowData != nullptr)
	{
		string fateLimit = _rowData->getStringData("value");
		m_vFateLimit.clear();
		vector<int> vecFateLimitData;
		StringFormat::parseCsvStringVec(fateLimit, vecFateLimitData);
		for(int i=0; i<vecFateLimitData.size(); i++)
		{
			sFateHeroOrder fateLimitData;
			fateLimitData.valueData = vecFateLimitData.at(i);
			fateLimitData.order = i+1;
			m_vFateLimit.push_back(fateLimitData);
		}
	}

	//计算缘分ID
	calculateFateId(tempLateId);

	//计算缘分属性
	parseFateAttri(tempLateId);
}

void HeroRelationModel::calculateFateId( int tempLateId )
{
	m_vFateId.clear();

	//英雄赋予的缘分
	for(int i=0; i<m_vFateHeroId.size(); i++)
	{
		//获取英雄数据
		HeroParam* fateHero = HeroModel::getInstance()->searchHeroByTemplateId(m_vFateHeroId.at(i).valueData);
		if(fateHero == nullptr)
		{
			//缘分类型 = i+1;
			int fateId = tempLateId*1000 + (i+1)*100 + 1;
			m_vFateId.push_back(fateId);
		}
		else
		{
			//觉醒等级
			int fateLv = 0;
			if(fateHero->mAdvLvl >= m_vFateLimit.back().valueData)
			{
				int fateId = tempLateId*1000 + (i+1)*100 + m_vFateLimit.back().valueData;
				m_vFateId.push_back(fateId);
			}
			else
			{
				for(int j=0; j<m_vFateLimit.size(); j++)
				{
					//查询觉醒等级对应的缘分等级
					if(j == 0)
					{
						if(fateHero->mAdvLvl <= m_vFateLimit.at(j).valueData)
						{
							fateLv = m_vFateLimit.at(j).order;
							int fateId = tempLateId*1000 + (i+1)*100 + m_vFateLimit.at(j).valueData;
							m_vFateId.push_back(fateId);
							break;
						}
					}
					else 
					{
						if(fateHero->mAdvLvl < m_vFateLimit.at(j).valueData)
						{
							fateLv = m_vFateLimit.at(j-1).order;
							int fateId = tempLateId*1000 + (i+1)*100 + m_vFateLimit.at(j-1).valueData;
							m_vFateId.push_back(fateId);
							break;
						}
					}
				}	
			}
		}
	}

	//马书赋予的缘分
	int horseFate = tempLateId*1000 + 4*100 + 1;
	m_vFateId.push_back(horseFate);

	//兵书赋予的缘分
	int bookFate = tempLateId*1000 + 5*100 + 1;
	m_vFateId.push_back(bookFate);

	//武器赋予的缘分
	int weaponFate = tempLateId*1000 + 6*100 + 1;
	RowData* _rowData = DataManager::getInstance()->searchHeroRelationById(weaponFate);
	if(_rowData != nullptr)
	{
		if(_rowData->getIntData("value") != -1)
		{
			m_vFateId.push_back(weaponFate);
		}
	}
}

void HeroRelationModel::parseFateAttri(int templateId)
{
	m_vFateAttri.clear();

	for(int i=0; i<m_vFateId.size(); i++)
	{
		sFateAttri fateAttriData;
		RowData* _rowData = DataManager::getInstance()->searchHeroRelationById(m_vFateId.at(i));
		if(_rowData != nullptr)
		{
			fateAttriData.fateId = m_vFateId.at(i);
			fateAttriData.fateType = _rowData->getIntData("type");
			fateAttriData.fateLevel = _rowData->getIntData("level");
			fateAttriData.fateHeroId = _rowData->getIntData("fateHero");
			fateAttriData.attriDes = _rowData->getStringData("dec");
			fateAttriData.attriDes2 = _rowData->getStringData("nextDec");
			fateAttriData.fateName = _rowData->getStringData("name");
			
			//缘分是否已激活
			HeroParam* hero = HeroModel::getInstance()->searchHeroByTemplateId(templateId);
			if(hero == nullptr)
			{
				fateAttriData.isFateActed = false;
			}
			else
			{
				if(fateAttriData.fateType == 4)
				{
					//马书
					CL_Param cl_param;
					SmithyModel::getInstance()->getCLParam(hero->mHorseId, EQUIP_ITEM_TYPE_HORSE, cl_param);
					if(cl_param.bIsPreFate && cl_param.nEquCLLevel > 0)
					{
						fateAttriData.isFateActed = true;
						fateAttriData.isFateMaxLv = true;
					}
					else
					{
						fateAttriData.isFateActed = false;
						fateAttriData.isFateMaxLv = false;
					}

					int equipIconId = templateId*100 + HORSE_ICON_ID_BASE;
					//查淬炼表
					RowData* _row = DataManager::getInstance()->searchEquipmentQuenchById(equipIconId);
					if(_row != nullptr)
					{
						fateAttriData.fateObjName = _row->getStringData("name");
					}
				}
				else if(fateAttriData.fateType == 5)
				{
					//兵书
					CL_Param cl_param;
					SmithyModel::getInstance()->getCLParam(hero->mBookId, EQUIP_ITEM_TYPE_BOOK, cl_param);
					if(cl_param.bIsPreFate && cl_param.nEquCLLevel > 0)
					{
						fateAttriData.isFateActed = true;
						fateAttriData.isFateMaxLv = true;
					}
					else
					{
						fateAttriData.isFateActed = false;
						fateAttriData.isFateMaxLv = false;
					}

					int equipIconId = templateId*100 + BOOK_ICON_ID_BASE;
					//查淬炼表
					RowData* _row = DataManager::getInstance()->searchEquipmentQuenchById(equipIconId);
					if(_row != nullptr)
					{
						fateAttriData.fateObjName = _row->getStringData("name");
					}
				}
				else if(fateAttriData.fateType == 6)
				{
					if(_rowData->getIntData("value") != -1)
					{
						//武器天命
						CL_Param cl_param;
						SmithyModel::getInstance()->getCLParam(hero->mWeaponId, EQUIP_ITEM_TYPE_WEAPON, cl_param);
						if(cl_param.bIsPreFate && cl_param.nEquCLLevel > 0)
						{
							fateAttriData.isFateActed = true;
							fateAttriData.isFateMaxLv = true;
						}
						else
						{
							fateAttriData.isFateActed = false;
							fateAttriData.isFateMaxLv = false;
						}

						int equipIconId = templateId*100 + WEAPON_ICON_ID_BASE;
						//查淬炼表
						RowData* _row = DataManager::getInstance()->searchEquipmentQuenchById(equipIconId);
						if(_row != nullptr)
						{
							fateAttriData.fateObjName = _row->getStringData("name");
						}
					}
					else
					{
						fateAttriData.isFateActed = false;
					}
				}
				else
				{
					//发起缘分的英雄名称
					RowData* _rowDataPet = DataManager::getInstance()->searchCharacterById(fateAttriData.fateHeroId);
					if(_rowDataPet != nullptr)
					{
						fateAttriData.fateObjName = _rowDataPet->getStringData("name");
					}

					fateAttriData.isFateActed = HeroModel::getInstance()->searchHeroByTemplateId(fateAttriData.fateHeroId) != nullptr;
					//判断缘分是否到了最高级			
					fateAttriData.isFateMaxLv = fateAttriData.fateLevel == m_vFateLimit.back().order;
				}
			}

			fateAttriData.leftSkillName = "";
			fateAttriData.rightSkillName = "";
			if(_rowData->getStringData("skill") != "-1")
			{
				vector<string> vecSkillName;
				StringFormat::parseCsvStringVec(_rowData->getStringData("skill"), vecSkillName);
				fateAttriData.leftSkillName = vecSkillName.front();
				fateAttriData.rightSkillName = vecSkillName.back();
			}
			if(_rowData->getFloatData("hpCoe") != 0)
			{
				fateAttriData.attriEnName = "hpCoe";
				fateAttriData.attriValue = _rowData->getFloatData("hpCoe");
				fateAttriData.attriChName = DataManager::getInstance()->searchCommonTexdtById(10008);
			}
			else if(_rowData->getFloatData("atkCoe") != 0)
			{
				fateAttriData.attriEnName = "atkCoe";
				fateAttriData.attriValue = _rowData->getFloatData("atkCoe");
				fateAttriData.attriChName = DataManager::getInstance()->searchCommonTexdtById(10009);
			}
			else if(_rowData->getFloatData("pddCoe") != 0)
			{
				fateAttriData.attriEnName = "pddCoe";
				fateAttriData.attriValue = _rowData->getFloatData("pddCoe");
				fateAttriData.attriChName = DataManager::getInstance()->searchCommonTexdtById(10244);
			}

			m_vFateAttri.push_back(fateAttriData);
		}
	}
}

void HeroRelationModel::initFatedData(int templateId)
{
	initData(templateId);
}

void HeroRelationModel::getRelationHeroId( vector<sFateHeroOrder>& vFateHeroId )
{
	vFateHeroId.clear();
	vFateHeroId = m_vFateHeroId;
}

void HeroRelationModel::getRelationActLimit( vector<sFateHeroOrder>& vFateActLimit )
{
	vFateActLimit.clear();
	vFateActLimit = m_vFateLimit;
}
	

void HeroRelationModel::getRelationIDList( vector<int>& vFateID )
{
	vFateID.clear();
	vFateID = m_vFateId;
}

void HeroRelationModel::getRelationAttriList( vector<sFateAttri>& vFateAttri )
{
	vFateAttri.clear();
	vFateAttri = m_vFateAttri;
}

void HeroRelationModel::getFatedHeroId( int heroId, vector<sFateHeroOrder>& vFatedHeroID )
{
	vFatedHeroID.clear();
	//主英雄觉醒等级
	int heroAdv = 0;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(heroId);
	if(heroParam == nullptr)
	{
		heroAdv = 1;
	}
	else
	{
		heroAdv = heroParam->mAdvLvl;
	}

	//先查找主缘英雄相对于受缘英雄所属的缘分类型
	RowData* _rowDataPet = DataManager::getInstance()->searchCharacterById(heroId);
	if(_rowDataPet != nullptr)
	{
		m_vFatedHeroId.clear();
		m_vFatedId.clear();
		string _fateHero = _rowDataPet->getStringData("fateHero");
		vector<int> vecFateHeroData;
		StringFormat::parseCsvStringVec(_fateHero, vecFateHeroData);
		for(int i=0; i<vecFateHeroData.size(); i++)
		{
			sFateHeroOrder fateHeroData;
			fateHeroData.valueData = vecFateHeroData.at(i);
			fateHeroData.order = i+1;
			//关联英雄ID
			m_vFatedHeroId.push_back(fateHeroData);

			//如果影响到的英雄不存在则不计入缘分ID，此处理应在调用弹窗前就被过滤掉了
			if(HeroModel::getInstance()->searchHeroByTemplateId(fateHeroData.valueData) == nullptr)
			{
				continue;
			}

			//计算缘分ID
			RowData* _rowDataPet1 = DataManager::getInstance()->searchCharacterById(fateHeroData.valueData);
			if(_rowDataPet1 != nullptr)
			{
				string _fateHero = _rowDataPet1->getStringData("fateHero2");
				vector<int> vecFateHeroData1;
				StringFormat::parseCsvStringVec(_fateHero, vecFateHeroData1);
				for(int j=0; j<vecFateHeroData1.size(); j++)
				{
					if(heroId == vecFateHeroData1.at(j))
					{
						//缘分类型
						int fateType = j+1;

						//构建缘分ID
						int fateId = fateHeroData.valueData*1000 + fateType*100 + heroAdv;
						m_vFatedId.push_back(fateId);
						break;
					}
				}
			}
		}
	}

	//解析缘分属性
	parseFatedAttri(heroId);

	vFatedHeroID = m_vFatedHeroId;
}

void HeroRelationModel::getFatedIdList(vector<int>& vFatedId)
{
	vFatedId.clear();
	vFatedId = m_vFatedId;
}

void HeroRelationModel::parseFatedAttri( int templateId )
{
	m_vFatedAttri.clear();
	for(int i=0; i<m_vFatedId.size(); i++)
	{
		sFateAttri fateAttriData;
		RowData* _rowData = DataManager::getInstance()->searchHeroRelationById(m_vFatedId.at(i));
		if(_rowData != nullptr)
		{
			fateAttriData.fateId = m_vFatedId.at(i);
			fateAttriData.fateType = _rowData->getIntData("type");
			fateAttriData.fateLevel = _rowData->getIntData("level");
			fateAttriData.fateHeroId = _rowData->getIntData("hero");
			fateAttriData.attriDes = _rowData->getStringData("dec");
			fateAttriData.attriDes2 = _rowData->getStringData("dec2");
			fateAttriData.fateName = _rowData->getStringData("name");

			//缘分是否已激活
			HeroParam* hero = HeroModel::getInstance()->searchHeroByTemplateId(templateId);
			if(hero == nullptr)
			{
				fateAttriData.isFateActed = false;
			}
			else
			{
				if(fateAttriData.fateType == 1 || fateAttriData.fateType == 2 || fateAttriData.fateType == 3)
				{
					fateAttriData.isFateActed = HeroModel::getInstance()->searchHeroByTemplateId(templateId) != nullptr;
				}
			}
		
			if(_rowData->getFloatData("hpCoe") != 0)
			{
				fateAttriData.attriEnName = "hpCoe";
				fateAttriData.attriValue = _rowData->getFloatData("hpCoe");
				fateAttriData.attriChName = DataManager::getInstance()->searchCommonTexdtById(10008);
			}
			else if(_rowData->getFloatData("atkCoe") != 0)
			{
				fateAttriData.attriEnName = "atkCoe";
				fateAttriData.attriValue = _rowData->getFloatData("atkCoe");
				fateAttriData.attriChName = DataManager::getInstance()->searchCommonTexdtById(10009);
			}
			else if(_rowData->getFloatData("pddCoe") != 0)
			{
				fateAttriData.attriEnName = "pddCoe";
				fateAttriData.attriValue = _rowData->getFloatData("pddCoe");
				fateAttriData.attriChName = DataManager::getInstance()->searchCommonTexdtById(10244);
			}

			m_vFatedAttri.push_back(fateAttriData);
		}
	}
}

void HeroRelationModel::getFatedAttri( vector<sFateAttri>& vFatedAttri )
{
	vFatedAttri.clear();
	vFatedAttri = m_vFatedAttri;
}





