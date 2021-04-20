#include "TalentModel.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "HeroModule/HeroModel.h"

TalentModel* TalentModel::ms_pInstance = nullptr;
TalentModel::TalentModel()
{
	mSex = HeroModel::getInstance()->getMainHero()->mSex;
	
	mainHeroLevel = 1;
	mVecSkillData.clear();
	
	parseTalentData(TALENT_SKILL_TYPE::TALENT_MANU_SKILL);
	parseTalentData(TALENT_SKILL_TYPE::TALENT_NORMAL_SKILL);
	parseTalentData(TALENT_SKILL_TYPE::TALENT_RUSH_SKILL);
	parseTalentData(TALENT_SKILL_TYPE::TALENT_BUFF_SKILL);
	parseTalentData(TALENT_SKILL_TYPE::TALENT_TRIGGER_SKILL);
}

TalentModel::~TalentModel()
{

}

TalentModel* TalentModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new TalentModel();
	}
	return ms_pInstance;
}

void TalentModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void TalentModel::setRecordMainHeroLv(int lv)
{
	mainHeroLevel = lv;
}

int TalentModel::getRecordMainHeroLv()
{
	return mainHeroLevel;
}

int TalentModel::getSkillTypeId(TALENT_SKILL_TYPE tag)
{
	if (mSex)
	{
		switch (tag)
		{
		case TALENT_SKILL_TYPE::TALENT_MANU_SKILL:
			return 2002;
			break;
		case TALENT_SKILL_TYPE::TALENT_NORMAL_SKILL:
			return 2001;
			break;
		case TALENT_SKILL_TYPE::TALENT_RUSH_SKILL:
			return 2003;
			break;
		case TALENT_SKILL_TYPE::TALENT_BUFF_SKILL:
			return 2004;
			break;
		case TALENT_SKILL_TYPE::TALENT_TRIGGER_SKILL:
			return 2005;
			break;
		default:
			break;
		}
	}
	else
	{
		switch (tag)
		{
		case TALENT_SKILL_TYPE::TALENT_MANU_SKILL:
			return 1002;
			break;
		case TALENT_SKILL_TYPE::TALENT_NORMAL_SKILL:
			return 1001;
			break;
		case TALENT_SKILL_TYPE::TALENT_RUSH_SKILL:
			return 1003;
			break;
		case TALENT_SKILL_TYPE::TALENT_BUFF_SKILL:
			return 1004;
			break;
		case TALENT_SKILL_TYPE::TALENT_TRIGGER_SKILL:
			return 1005;
			break;
		default:
			break;
		}
	}
}

void TalentModel::parseTalentData(TALENT_SKILL_TYPE skillType)
{
	int skillTypeId = getSkillTypeId(skillType);
	RowData* skillRowData = DataManager::getInstance()->searchTalentSkillById(skillTypeId);
	if(skillRowData)
	{
		string skillStr = skillRowData->getStringData("skillId");
		vector<int> vecSkillId;
		StringFormat::parseCsvStringVec(skillStr, vecSkillId);

		string levelStr = skillRowData->getStringData("openLevel");
		vector<int> vecLevelId;
		StringFormat::parseCsvStringVec(levelStr, vecLevelId);

		for(int i=0; i<TALENT_SKILL_COUNT; i++)
		{
			TalentSkillData skillData;
			skillData.skillTypeId = skillTypeId;
			skillData.unlockLvl = vecLevelId.at(i);
			skillData.skillId = vecSkillId.at(i);
			skillData.skillUIindex = i;
			skillData.isNeedNewOpenFlag = (mainHeroLevel < skillData.unlockLvl);

			RowData* skillItem = DataManager::getInstance()->searchSkillById(skillData.skillId);
			if(skillItem)
			{
				skillData.skillIconResId = skillItem->getIntData("resId");
				skillData.skillName = skillItem->getStringData("name");
				skillData.skillDes = skillItem->getStringData("capTxt");
				skillData.skillCD = skillItem->getIntData("skl_cd");
				skillData.skillInCD = skillItem->getIntData("ini_cd");
				skillData.skillSP = skillItem->getIntData("sp");
			}
			mVecSkillData.push_back(skillData);
		}
	}
}

vector<TalentSkillData>& TalentModel::getSkillData()
{
	return mVecSkillData;
}

void TalentModel::setSkillNewOpenState(int index)
{
	if(index < mVecSkillData.size())
	{
		mVecSkillData.at(index).isNeedNewOpenFlag = false;
	}
}