#include "NormalSkill.h"
#include "../FightMacros.h"
#include "../Fighter.h"
#include "SkillResultParam.h"

NormalSkill::NormalSkill()
	: AbstractSkill()
	, mResultStateType(-1)
	, mMutiComboCoe(0.0f)
	, mMutiComboCount(-1)
{

}

NormalSkill::~NormalSkill()
{

}

NormalSkill* NormalSkill::create(int skillId)
{
	NormalSkill* normalSkill = new NormalSkill();
	if (normalSkill && normalSkill->init(skillId))
	{
		normalSkill->autorelease();
		return normalSkill;
	}
	else
	{
		delete normalSkill;
		return nullptr;
	}
}

bool NormalSkill::init(int skillId)
{
	if (!AbstractSkill::init(skillId))
	{
		return false;
	}

	if (getSkillType() != SKILL_TYPE::NORMAL_SKILL)
	{
		return false;
	}

	if (mSkillData)
	{
		//普通攻击单击技能特效备份
		mNormalEffectParam = mEffectParam;
		mExtraEffectParam = mEffectParam;

		//普通攻击属性
		setResultStateType(mSkillData->getIntData("staTyp"));
		setMutiComboCoe(mSkillData->getFloatData("cDmgCoe"));
		setMutiComboCount(mSkillData->getFloatData("cCmb"));

		//初始化多段攻击特效
		int effectId = mSkillData->getIntData("effId");
		if (effectId > 0)
		{
			RowData* effectData = DataManager::getInstance()->searchSkillEffectById(effectId);
			if (effectData)
			{
				//技能特效属性
				mExtraEffectParam.nDisType = effectData->getIntData("disTyp");
				mExtraEffectParam.nDistance = effectData->getIntData("distance");
				mExtraEffectParam.nPreResID = effectData->getIntData("preResId");
				mExtraEffectParam.nPreType = effectData->getIntData("preTyp");
				mExtraEffectParam.nPreOrderType = effectData->getIntData("preOrdTyp");
				mExtraEffectParam.nActResID = effectData->getIntData("actResId");
				mExtraEffectParam.nActOrderType = effectData->getIntData("actTyp");
				mExtraEffectParam.nPreTagTyp = effectData->getIntData("preTagTyp");
				mExtraEffectParam.actionName = effectData->getStringData("aniId");
				mExtraEffectParam.nMoveType = effectData->getIntData("moveTyp");
				mExtraEffectParam.preName = effectData->getStringData("preAni");
				mExtraEffectParam.actName = effectData->getStringData("actAni");
			}
			else
			{
				CCLOG("NormalSkill::init false: skillId=%d, effectId=%d", skillId, effectId);
				return false;
			}
		}

		return true;
	}
	else
	{
		CCLOG("NormalSkill::init false: skillId=%d", skillId);
		return false;
	}
	
}

bool NormalSkill::onJudgeStart()
{
	if (!AbstractSkill::onJudgeStart())
	{
		return false;
	}

	if (mAttacker && mAttacker->isBanNormal())
	{
		return false;
	}

	return true;
}

void NormalSkill::onSkillEffect(Fighter* target, HarmParam& harmParam, float& harm)
{
	if (IS_HIT_CHANCE(mAttacker->getCtrPrb()))
	{
		//触发多次攻击，连击数发生变化，并触发被攻击者状态，技能效果发生变化
		harmParam.mComboCount = getMutiComboCount();
		mResultParam.mComboCount = getMutiComboCount();
		harm *= getMutiComboCoe();
		mEffectParam = mExtraEffectParam;
		target->setState((AbstractSkill::RESULT_STATE)getResultStateType());
	}
	else
	{
		harmParam.mComboCount = getComboCount();
		mResultParam.mComboCount = getComboCount();
		harm *= getDamageCoe();
		mEffectParam = mNormalEffectParam;
		target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);
	}
}

//攻击者触发的攻击事件
void NormalSkill::onAttack(Fighter* attacker, Fighter* target, int harm)
{
	int bloodValue = attacker->onBlood(harm);

	if(bloodValue > 0) //说明有吸血效果，注：只有普通攻击有吸血效果
	{
		mResultParam.mBloodHarm.mTargetIndex = attacker->getIndex();
		mResultParam.mBloodHarm.mTargetSide = mResultParam.mAttackSide;
		mResultParam.mBloodHarm.mHarmType = HarmParam::HARM_TYPE::HARM_BLOOD;
		mResultParam.mBloodHarm.mComboCount = 0;
		mResultParam.mBloodHarm.mValue = bloodValue;
	}
	else
	{
		//bugfix:如果没有吸血效果，则数据清空，防止出现数据残留
		mResultParam.mBloodHarm.mTargetIndex = -1;
		mResultParam.mBloodHarm.mTargetSide = mResultParam.mAttackSide;
		mResultParam.mBloodHarm.mHarmType = HarmParam::HARM_TYPE::HARM_BLOOD;
		mResultParam.mBloodHarm.mComboCount = 0;
		mResultParam.mBloodHarm.mValue = 0;
	}

// 	//触发攻击者的被动2技能;被动2不再触发，改为加属性，客户端无需计算 by bran:20160413
// 	BuffSkill* buffSkill = attacker->getBuffSkill();
// 	if (buffSkill)
// 	{
// 		buffSkill->onAttack(getSkillResultParam(), attacker, target);
// 	}
}

//被攻击者触发的被击时间
void NormalSkill::onBeaten(Fighter* attacker, Fighter* target, int harm)
{
// 	//触发被击者的被动2技能;被动2不再触发，改为加属性，客户端无需计算 by bran:20160413
// 	BuffSkill* buffSkill = target->getBuffSkill();
// 	if (buffSkill)
// 	{
// 		buffSkill->onBeaten(getSkillResultParam(), attacker, target);
// 	}
}

