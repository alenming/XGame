#include "TriggerSkill.h"
#include "../FightModel.h"

TriggerSkill::TriggerSkill()
	: AbstractSkill()
	, mTriggerType(-1)
	, mIsTrigger(false)
	, mTriggerCount(-1)
	, mChangeId(-1)
	, mChangeSkin(-1)
{

}

TriggerSkill::~TriggerSkill()
{

}

TriggerSkill* TriggerSkill::create(int skillId)
{
	TriggerSkill* triggerSkill = new TriggerSkill();
	if (triggerSkill && triggerSkill->init(skillId))
	{
		triggerSkill->autorelease();
		return triggerSkill;
	}
	else
	{
		delete triggerSkill;
		return nullptr;
	}
}

bool TriggerSkill::init(int skillId)
{
	if (!AbstractSkill::init(skillId))
	{
		return false;
	}

	if (getSkillType() != SKILL_TYPE::TRIGGER_SKILL)
	{
		return false;
	}

	if (mSkillData)
	{
		//����1����
		setTriggerType(mSkillData->getIntData("triTyp"));
		setTriggerCount(mSkillData->getIntData("triAmn"));
		setChangeId(mSkillData->getIntData("moster"));
		setChangeSkin(mSkillData->getIntData("skin"));
	}

	mIsTrigger = false;

	return true;
}

void TriggerSkill::onAttack()
{
	AbstractSkill::onAttack();

	//����Ϊ�ѱ�����
	mIsTrigger = true;

	//�ж��Ƿ񴥷�����
	int changeId = getChangeId();
	if (changeId > 0)
	{
		mAttacker->onChange(changeId);
		mResultParam.mChangeSkin = getChangeSkin();
		clearFighterBuff(mAttacker); //���������������buff
	}
	
}

bool TriggerSkill::isTrigger()
{
	return mIsTrigger;
}

//����ս��(ֻ����һ��)
bool TriggerSkill::triggerFightStart()
{
	if (getTriggerType() == TRIGGER_TYPE::ENTER_FIGHT)
	{
		if (!isTrigger() && onJudgeStart())
		{
			onTargetSelect();
			onAttack();
			return true;
		}
	}
	return false;
}

//ÿ�غϴ���
bool TriggerSkill::triggerRoundStart()
{
	if (getTriggerType() == TRIGGER_TYPE::EVERY_ROUND)
	{
		if (!isTrigger() && onJudgeStart())
		{
			onTargetSelect();
			onAttack();
			return true;
		}
	}
	return false;
}

//��ͨ������ɱ
bool TriggerSkill::triggerTargetDead(AbstractSkill* currentSkill)
{
	if (getTriggerType() == TRIGGER_TYPE::TARGET_DEAD)
	{
		if (currentSkill && !currentSkill->getSkillResultParam()->mVecTargetIndex.empty())
		{
			//��ǰ���������ļ��ܱ�������ͨ�������߱���3����
			if (currentSkill->isNormalSkill() || currentSkill->isTriggerSkill())
			{
				int targetIndex = currentSkill->getSkillResultParam()->mVecTargetIndex.at(0);
				FightStateMachine::E_SIDE targetSide = currentSkill->getSkillResultParam()->mTargetSide;

				int attackerIndex = currentSkill->getSkillResultParam()->mAttackerIndex;
				FightStateMachine::E_SIDE attackSide = currentSkill->getSkillResultParam()->mAttackSide;
				Fighter* target = mFightModel->getFighter(targetSide, targetIndex);

				//�жϱ��������Ƿ�����
				if (target && !target->isAlive() && !mFightModel->isAllFighterDead(targetSide) && onJudgeStart())
				{
					return true;
				}
			}
		}
	}
	return false;
}

//ÿ����ͨ����ǰ����
bool TriggerSkill::triggerBeforeAttack(AbstractSkill* currentSkill)
{
	if (getTriggerType() == TRIGGER_TYPE::BEFORE_ATTACK)
	{
		if (currentSkill && currentSkill->isNormalSkill())
		{
			int attackerIndex = currentSkill->getSkillResultParam()->mAttackerIndex;
			FightStateMachine::E_SIDE attackSide = currentSkill->getSkillResultParam()->mAttackSide;

			//����3δ�������Ҵ�������Ϊ����ǰ
			if (!isTrigger() && onJudgeStart())
			{
				onTargetSelect();
				onAttack();
				return true;
			}
		}
	}
	return false;
}

bool TriggerSkill::triggerHitCount()
{
	if (getTriggerType() == TRIGGER_TYPE::COMBO_N)
	{
		FightStateMachine::E_SIDE attackSide = mResultParam.mAttackSide;
		int currentHitCount = 0;
		if (attackSide == FightStateMachine::E_SIDE::MY_SIDE)
		{
			currentHitCount = mFightModel->getMyHitCount();
		}
		else
		{
			currentHitCount = mFightModel->getEnemyHitCount();
		}

		if (currentHitCount >= getTriggerCount() && onJudgeStart() && !isTrigger())
		{
			//onTargetSelect();
			//onAttack();
			return true;
		}
	}
	return false;
}

bool TriggerSkill::triggerRoundBeforeEnemy()
{
	if (getTriggerType() == TRIGGER_TYPE::ROUND_N_ENEMY)
	{
		int roundCount = FightModel::getInstance()->getRoundCount();
		if (!isTrigger() && getTriggerCount() == roundCount)
		{
			return true;
		}
	}
	return false;
}

bool TriggerSkill::triggerRound()
{
	if (getTriggerType() == TRIGGER_TYPE::ROUND_N)
	{
		int roundCount = FightModel::getInstance()->getRoundCount();
		if (!isTrigger() && getTriggerCount() == roundCount && onJudgeStart())
		{
			onTargetSelect();
			onAttack();
			return true;
		}
	}
	return false;
}

void TriggerSkill::onRoundEnd()
{
	switch (getTriggerType())
	{
	case EVERY_ROUND:
	case COMBO_N:
	case BEFORE_ATTACK:
	case ROUND_N_ENEMY:
		mIsTrigger = false;
		break;
	default:
		break;
	}
}