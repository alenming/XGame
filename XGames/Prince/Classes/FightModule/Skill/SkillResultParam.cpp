#include "SkillResultParam.h"


SkillResultParam::SkillResultParam()
	: mAttackSide(FightStateMachine::E_SIDE::MY_SIDE)
	, mTargetSide(FightStateMachine::E_SIDE::MY_SIDE)
	, mAttackerIndex(-1)
	, mSkillId(-1)
	, mSkillType(-1)
	, mChangeSkin(-1)
{

}

SkillResultParam::~SkillResultParam()
{
	clear();
}

SkillResultParam* SkillResultParam::create()
{
	SkillResultParam* resultParam = new SkillResultParam();
	resultParam->autorelease();
	return resultParam;
}

void SkillResultParam::clear()
{
	mAttackSide = FightStateMachine::E_SIDE::MY_SIDE;
	mTargetSide = FightStateMachine::E_SIDE::MY_SIDE;
	mAttackerIndex = -1;
	mSkillId = -1;
	mSkillType = -1;
	mComboCount = 0;
	mChangeSkin = -1;
	mVecTargetIndex.clear();
	mVecTargetHarm.clear();
	mMapMyBuffUpdateParam.clear();
	mMapEnemyBuffUpdateParam.clear();
}