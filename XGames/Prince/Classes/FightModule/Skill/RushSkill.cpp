#include "RushSkill.h"
#include "../FightModel.h"

RushSkill::RushSkill()
	: AbstractSkill()
	, mPreStateType(-1)
	, mPstStateType(-1)
{

}

RushSkill::~RushSkill()
{

}

RushSkill* RushSkill::create(int skillId)
{
	RushSkill* rushSkill = new RushSkill();
	if (rushSkill && rushSkill->init(skillId))
	{
		rushSkill->autorelease();
		return rushSkill;
	}
	else
	{
		delete rushSkill;
		return nullptr;
	}
}

bool RushSkill::init(int skillId)
{
	if (!AbstractSkill::init(skillId))
	{
		return false;
	}

	if (getSkillType() != SKILL_TYPE::RUSH_SKILL)
	{
		return false;
	}

	if (mSkillData)
	{
		//����1����
		setPreStateType(mSkillData->getIntData("preTyp"));
		setPstStateType(mSkillData->getIntData("pstTyp"));
	}
	return true;
}

bool RushSkill::onJudgeStart()
{
	if (!AbstractSkill::onJudgeStart())
	{
		return false;
	}

	if (mAttacker && mAttacker->isBanRush())
	{
		return false;
	}

	return true;
}

void RushSkill::onTargetSelect()
{
	//���Ŀ�����
	clearResultParam();

	//����1���ܵ�����Ŀ��һ��Ϊ�Է�
	mResultParam.mTargetSide = (mResultParam.mAttackSide == FightStateMachine::E_SIDE::MY_SIDE)
		? FightStateMachine::E_SIDE::ENEMY_SIDE
		: FightStateMachine::E_SIDE::MY_SIDE;

	selectStateTarget();
}

void RushSkill::selectStateTarget()
{
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* target = mFightModel->getAliveFighter(mResultParam.mTargetSide, i);
		if (target != nullptr)
		{
			//���ҷ��Ϸ�������1��ǰ��״̬��Ŀ��
			if (target->getState() == getPreStateType())
			{
				//target->setOnRush(true);
				mResultParam.mVecTargetIndex.push_back(i);
				break;
			}
		}
	}
}

void RushSkill::onSkillEffect(Fighter* target, HarmParam& harmParam, float& harm)
{
	harmParam.mComboCount = getComboCount();
	target->setState((RESULT_STATE)getPstStateType());
}

void RushSkill::onHarmModification(Fighter* target, int& harm)
{
	if (target->hasTyrants())
	{
		//�����˺�ǿ��Ϊ1
		harm = 1;
	}
}
