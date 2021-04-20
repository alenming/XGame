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
		//������������
		setSpCost(mSkillData->getIntData("sp"));
		setMaxCD(mSkillData->getIntData("skl_cd"));
		setEnterCD(mSkillData->getIntData("ini_cd"));
		setResultStateType(mSkillData->getIntData("addTpy"));
		//����CD���������������ܵ�ǰCDΪ����CD
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
	//����ǰ��Ѫ�����ٵĹ���Ŀ�긽��״̬
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

	//����
	AbstractSkill::onAttack();

	//������, ������Ŀ���buff
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
	
	//�������������ͷ�ʱ��ԭCD
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
		//�����������ܲ��ۼ�����ŭ��
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

