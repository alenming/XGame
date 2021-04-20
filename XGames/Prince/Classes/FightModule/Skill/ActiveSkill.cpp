#include "ActiveSkill.h"
#include "../FightModel.h"


ActiveSkill::ActiveSkill()
	: AbstractSkill()
	, mSpCost(-1)
	, mMaxCD(-1)
	, mEnterCD(-1)
	, mCurrentCD(-1)
	, mIsActive(false)
	, mResultStateType(-1)
	, mExtraTargetType(-1)
	, mExtraRangeType(-1)
	, mExtraBuffId(-1)
{

}

ActiveSkill::~ActiveSkill()
{

}

ActiveSkill* ActiveSkill::create(int skillId)
{
	ActiveSkill* activeSkill = new ActiveSkill();
	if (activeSkill && activeSkill->init(skillId))
	{
		activeSkill->autorelease();
		return activeSkill;
	}
	else
	{
		delete activeSkill;
		return nullptr;
	}
}

bool ActiveSkill::init(int skillId)
{
	if (!AbstractSkill::init(skillId))
	{
		return false;
	}

	if (getSkillType() != SKILL_TYPE::ACTIVE_SKILL)
	{
		return false;
	}

	if (mSkillData)
	{
		//主动技能属性
		setSpCost(mSkillData->getIntData("sp"));
		setMaxCD(mSkillData->getIntData("skl_cd"));
		setEnterCD(mSkillData->getIntData("ini_cd"));
		setResultStateType(mSkillData->getIntData("addTpy"));
		//进场CD有配置则主动技能当前CD为进场CD
		setCurrentCD(getEnterCD() >= 0 ? getEnterCD() : getMaxCD());
		
		setExtraTargetType(mSkillData->getIntData("sTarTyp"));
		setExtraRangeType(mSkillData->getIntData("sRngmTyp"));
		setExtraBuffId(mSkillData->getIntData("sBufID"));

	}

	mIsActive = false;

	return true;
}

bool ActiveSkill::onJudgeStart()
{
	if (!AbstractSkill::onJudgeStart())
	{
		return false;
	}

	if (mAttacker && mAttacker->isBanActive())
	{
		return false;
	}

	return true;
}

void ActiveSkill::onAttack()
{
	//攻击前给血量最少的攻击目标附加状态
	if (getResultStateType() > 0)
	{
		Fighter* fighter = nullptr;
		for (size_t i = 0; i < mResultParam.mVecTargetIndex.size(); i++)
		{
			int targetId = mResultParam.mVecTargetIndex[i];
			Fighter* tempFighter = mFightModel->getAliveFighter(mResultParam.mTargetSide, targetId);
			if (tempFighter)
			{
				if (!fighter)
				{
					fighter = tempFighter;
				}
				else
				{
					if (tempFighter->getCurrHP() < fighter->getCurrHP())
					{
						fighter = tempFighter;
					}
				}
			}
		}
		if (fighter)
		{
			fighter->setState((AbstractSkill::RESULT_STATE)getResultStateType());
		}
	}

	//攻击
	AbstractSkill::onAttack();

	//辅攻击, 给辅攻目标加buff
	if (getExtraRangeType() > 0)
	{
		FightStateMachine::E_SIDE extraTargetSide = getTargetSide(getExtraTargetType());
		vector<int> vecExtraTargetId = onTargetSelect(extraTargetSide, getExtraRangeType());
		for (size_t i = 0; i < vecExtraTargetId.size(); i++)
		{
			int extraTargetId = vecExtraTargetId[i];
			Fighter* target = mFightModel->getAliveFighter(extraTargetSide, extraTargetId);
			if (target)
			{
				doBuff(getExtraBuffId(), getSkillResultParam(), target, mAttacker);
			}
		}
	}
	
	//主动技能需在释放时还原CD
	setCurrentCD(getMaxCD());
	mIsActive = false;
}

bool ActiveSkill::isActived()
{
	return mIsActive;
}

bool ActiveSkill::isReady()
{
	if (getCurrentCD() == 0 && isSpEnough() && !isActived())
	{
		return true;
	}
	return false;
}

bool ActiveSkill::isSpEnough()
{
	int totalSp = 0;

	if (mResultParam.mAttackSide == FightStateMachine::E_SIDE::MY_SIDE)
	{
		totalSp = mFightModel->getMyTotalSp();
	}
	else if (mResultParam.mAttackSide == FightStateMachine::E_SIDE::ENEMY_SIDE)
	{
		totalSp = mFightModel->getEnemyTotalSp();
	}

	return getSpCost() <= totalSp;
}

void ActiveSkill::decreaseCD()
{
	int currentCD = getCurrentCD();
	currentCD = currentCD > 0 ? --currentCD : 0;
	setCurrentCD(currentCD);
}

void ActiveSkill::launchSkill()
{
	if (isReady())
	{
		mIsActive = true;
		//激活主动技能并扣减消耗怒气
		if (mResultParam.mAttackSide == FightStateMachine::E_SIDE::MY_SIDE)
		{
			int leftSp = mFightModel->getMyTotalSp() - getSpCost();
			mFightModel->setMyTotalSp(leftSp);
		}
		else if (mResultParam.mAttackSide == FightStateMachine::E_SIDE::ENEMY_SIDE)
		{
			int leftSp = mFightModel->getEnemyTotalSp() - getSpCost();
			mFightModel->setEnemyTotalSp(leftSp);
		}
	}
}

