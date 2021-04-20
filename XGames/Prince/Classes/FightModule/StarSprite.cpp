#include "StarSprite.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"

StarSprite::StarSprite()
	: mId(0)
	, mLevel(0)
	, mAttackType(0)
	, mGrowDamageCoe(0.0f)
	, mBasicDamage(0)
	, mStarSkill(nullptr)
	, mStarSkillResId(0)
{

}

StarSprite::~StarSprite()
{

}

StarSprite* StarSprite::create( int id, int level, FightStateMachine::E_SIDE side )
{
	StarSprite* ret = new StarSprite();
	if (ret && ret->init(id, level, side))
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		delete ret;
		ret = nullptr;
		return ret;
	}
}

bool StarSprite::init(int id, int level, FightStateMachine::E_SIDE side)
{
	mId = id;
	mLevel = level;
	mSide = side;

	RowData* rowData = DataManager::getInstance()->searchStarSpriteAttriById(id);

	if (rowData && level > 0)
	{
		mAttackType = rowData->getIntData("atkTyp");
		mBasicDamage = rowData->getIntData("sklNum");
		mGrowDamageCoe = rowData->getFloatData("growDmg");
		string skillStr = rowData->getStringData("a_sklID");
		vector<int> vecSkillId;
		StringFormat::parseCsvStringVec(skillStr, vecSkillId);
		int skillId = -1;
		if (mLevel % 10 < vecSkillId.size())
		{
			skillId = vecSkillId.at(mLevel % 10);
		}
		mStarSkill = StarSkill::create(skillId);
		mStarSkill->setStarSprite(this);
		return true;
	}

	return false;
	
}

FightStateMachine::E_SIDE StarSprite::getSide()
{
	return mSide;
}

int StarSprite::getBasicDamage()
{
	return mBasicDamage;
}

float StarSprite::getAddtionDamageCoe()
{
	return (1 + mGrowDamageCoe) * mLevel;
}

int StarSprite::getAttackType()
{
	return mAttackType;
}

StarSkill* StarSprite::getStarSkill()
{
	return mStarSkill;
}

