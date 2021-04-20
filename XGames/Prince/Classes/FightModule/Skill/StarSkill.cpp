#include "StarSkill.h"
#include "../StarSprite.h"
#include "../FightModel.h"

//最大精灵怒气
const int MAX_SPRITE_AG = 100;

//每次普通攻击增加的怒气值
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

		//先给目标附加Buff
		doBuff(getMBuffId(), getSkillResultParam(), target, nullptr);

		HarmParam harmParam;
		harmParam.mTargetIndex = targetId;
		harmParam.mTargetSide = mResultParam.mTargetSide;

		//基础伤害（计算公式：(基础伤害 * 暴击系数 * 技能系数  - 敌方防御 * coe) * 浮动系数 * 相克系数）
		float harm = mStarSprite->getBasicDamage();

		if (getDamageType() == DAMAGE_TYPE::HARM)
		{
			int accuracy = 10000; //精确度
			int dodge = target->getFinalDodge() * accuracy;

			//生成随机数
			int randNum = CCRANDOM_0_1() * accuracy;

			//判定闪避、暴击、普通
			if (randNum < dodge)
			{
				//闪避
				harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_DODGE;
				harmParam.mValue = 0;
				harmParam.mComboCount = getComboCount(); //需要判定combo有值
				target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);
			}
			else
			{
				//星灵攻击无暴击，普通攻击
				harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_NORMAL;
				harmParam.mComboCount = getComboCount();
				
				//星灵伤害系数随等级增加
				harm *= getDamageCoe() + mStarSprite->getAddtionDamageCoe();

				//伤害 * 浮动系数
				float floatCoe = FLOAT_COE_MIN + CCRANDOM_0_1() * (FLOAT_COE_MAX - FLOAT_COE_MIN);
				harm *= floatCoe;

				//伤害值修正，最低伤害为1
				harm = (harm <= 0) ? 1.0 : harm;
				harm = -harm;
				harmParam.mValue = harm;

				//执行伤害
				target->onHarm(harmParam.mValue);
			}
		}
		else if (getDamageType() == DAMAGE_TYPE::CURE) 
		{
			//乘以技能系数
			harm *= getDamageCoe();

			//伤害 * 浮动系数
			float floatCoe = FLOAT_COE_MIN + CCRANDOM_0_1() * (FLOAT_COE_MAX - FLOAT_COE_MIN);
			harm *= floatCoe;

			//加血不计算暴击和闪避，数值为人物自身攻击值
			harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_CURE;
			harmParam.mValue = harm;
			harmParam.mComboCount = 1;
			target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);

			target->onHarm(harmParam.mValue);
		}
		if (getDamageType() == DAMAGE_TYPE::NONE_DAMAGE)
		{
			//未直接造成伤害，不做伤害数值计算
			harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_NONE;
			harmParam.mValue = 0;
			harmParam.mComboCount = 1;
			target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);
		}

		//统计伤害计算结果
		mResultParam.mVecTargetHarm.push_back(harmParam);
	}

	//攻击后重置精灵怒气值
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


