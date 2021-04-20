#include "StarSkill.h"
#include "../StarSprite.h"
#include "../FightModel.h"

//�����ŭ��
const int MAX_SPRITE_AG = 100;

//ÿ����ͨ�������ӵ�ŭ��ֵ
const int ADD_SPRITE_AG = 15;

StarSkill::StarSkill()
	: mStarSprite(nullptr)
	, mMaxSpriteAg(MAX_SPRITE_AG)
	, mCurSpriteAg(0)
{

}

StarSkill::~StarSkill()
{

}

StarSkill* StarSkill::create(int skillId)
{
	StarSkill* starSkill = new StarSkill();
	if (starSkill && starSkill->init(skillId))
	{
		starSkill->autorelease();
		return starSkill;
	}
	else
	{
		delete starSkill;
		return nullptr;
	}
}

bool StarSkill::init(int skillId)
{
	if (!AbstractSkill::init(skillId))
	{
		return false;
	}

	if (getSkillType() != SKILL_TYPE::STAR_SKILL)
	{
		return false;
	}

	return true;
}

void StarSkill::setStarSprite(StarSprite* starSprite)
{
	mStarSprite = starSprite;
	mAttacker = nullptr;
	mResultParam.mAttackerIndex = -1;
	mResultParam.mAttackSide = starSprite->getSide();
	mResultParam.mSkillId = mSkillId;
	mResultParam.mSkillType = getSkillType();
	mResultParam.mComboCount = getComboCount();
}

void StarSkill::onAttack()
{
	for (int i = 0; i < mResultParam.mVecTargetIndex.size(); i++)
	{
		int targetId = mResultParam.mVecTargetIndex[i];
		Fighter* target = mFightModel->getAliveFighter(mResultParam.mTargetSide, targetId);

		//�ȸ�Ŀ�긽��Buff
		doBuff(getMBuffId(), getSkillResultParam(), target, nullptr);

		HarmParam harmParam;
		harmParam.mTargetIndex = targetId;
		harmParam.mTargetSide = mResultParam.mTargetSide;

		//�����˺������㹫ʽ��(�����˺� * ����ϵ�� * ����ϵ��  - �з����� * coe) * ����ϵ�� * ���ϵ����
		float harm = mStarSprite->getBasicDamage();

		if (getDamageType() == DAMAGE_TYPE::HARM)
		{
			int accuracy = 10000; //��ȷ��
			int dodge = target->getFinalDodge() * accuracy;

			//���������
			int randNum = CCRANDOM_0_1() * accuracy;

			//�ж����ܡ���������ͨ
			if (randNum < dodge)
			{
				//����
				harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_DODGE;
				harmParam.mValue = 0;
				harmParam.mComboCount = getComboCount(); //��Ҫ�ж�combo��ֵ
				target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);
			}
			else
			{
				//���鹥���ޱ�������ͨ����
				harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_NORMAL;
				harmParam.mComboCount = getComboCount();
				
				//�����˺�ϵ����ȼ�����
				harm *= getDamageCoe() + mStarSprite->getAddtionDamageCoe();

				//�˺� * ����ϵ��
				float floatCoe = FLOAT_COE_MIN + CCRANDOM_0_1() * (FLOAT_COE_MAX - FLOAT_COE_MIN);
				harm *= floatCoe;

				//�˺�ֵ����������˺�Ϊ1
				harm = (harm <= 0) ? 1.0 : harm;
				harm = -harm;
				harmParam.mValue = harm;

				//ִ���˺�
				target->onHarm(harmParam.mValue);
			}
		}
		else if (getDamageType() == DAMAGE_TYPE::CURE) 
		{
			//���Լ���ϵ��
			harm *= getDamageCoe();

			//�˺� * ����ϵ��
			float floatCoe = FLOAT_COE_MIN + CCRANDOM_0_1() * (FLOAT_COE_MAX - FLOAT_COE_MIN);
			harm *= floatCoe;

			//��Ѫ�����㱩�������ܣ���ֵΪ����������ֵ
			harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_CURE;
			harmParam.mValue = harm;
			harmParam.mComboCount = 1;
			target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);

			target->onHarm(harmParam.mValue);
		}
		if (getDamageType() == DAMAGE_TYPE::NONE_DAMAGE)
		{
			//δֱ������˺��������˺���ֵ����
			harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_NONE;
			harmParam.mValue = 0;
			harmParam.mComboCount = 1;
			target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);
		}

		//ͳ���˺�������
		mResultParam.mVecTargetHarm.push_back(harmParam);
	}

	//���������þ���ŭ��ֵ
	mCurSpriteAg = 0;
}

void StarSkill::addSpriteAG()
{
	mCurSpriteAg += ADD_SPRITE_AG;
	if (mCurSpriteAg > MAX_SPRITE_AG)
	{
		mCurSpriteAg = MAX_SPRITE_AG;
	}
}

bool StarSkill::isReady()
{
	if (mCurSpriteAg >= MAX_SPRITE_AG)
	{
		return true;
	}
	return false;
}


