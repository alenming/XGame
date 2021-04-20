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
		//��ͨ��������������Ч����
		mNormalEffectParam = mEffectParam;
		mExtraEffectParam = mEffectParam;

		//��ͨ��������
		setResultStateType(mSkillData->getIntData("staTyp"));
		setMutiComboCoe(mSkillData->getFloatData("cDmgCoe"));
		setMutiComboCount(mSkillData->getFloatData("cCmb"));

		//��ʼ����ι�����Ч
		int effectId = mSkillData->getIntData("effId");
		if (effectId > 0)
		{
			RowData* effectData = DataManager::getInstance()->searchSkillEffectById(effectId);
			if (effectData)
			{
				//������Ч����
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
		//������ι����������������仯����������������״̬������Ч�������仯
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

//�����ߴ����Ĺ����¼�
void NormalSkill::onAttack(Fighter* attacker, Fighter* target, int harm)
{
	int bloodValue = attacker->onBlood(harm);

	if(bloodValue > 0) //˵������ѪЧ����ע��ֻ����ͨ��������ѪЧ��
	{
		mResultParam.mBloodHarm.mTargetIndex = attacker->getIndex();
		mResultParam.mBloodHarm.mTargetSide = mResultParam.mAttackSide;
		mResultParam.mBloodHarm.mHarmType = HarmParam::HARM_TYPE::HARM_BLOOD;
		mResultParam.mBloodHarm.mComboCount = 0;
		mResultParam.mBloodHarm.mValue = bloodValue;
	}
	else
	{
		//bugfix:���û����ѪЧ������������գ���ֹ�������ݲ���
		mResultParam.mBloodHarm.mTargetIndex = -1;
		mResultParam.mBloodHarm.mTargetSide = mResultParam.mAttackSide;
		mResultParam.mBloodHarm.mHarmType = HarmParam::HARM_TYPE::HARM_BLOOD;
		mResultParam.mBloodHarm.mComboCount = 0;
		mResultParam.mBloodHarm.mValue = 0;
	}

// 	//���������ߵı���2����;����2���ٴ�������Ϊ�����ԣ��ͻ���������� by bran:20160413
// 	BuffSkill* buffSkill = attacker->getBuffSkill();
// 	if (buffSkill)
// 	{
// 		buffSkill->onAttack(getSkillResultParam(), attacker, target);
// 	}
}

//�������ߴ����ı���ʱ��
void NormalSkill::onBeaten(Fighter* attacker, Fighter* target, int harm)
{
// 	//���������ߵı���2����;����2���ٴ�������Ϊ�����ԣ��ͻ���������� by bran:20160413
// 	BuffSkill* buffSkill = target->getBuffSkill();
// 	if (buffSkill)
// 	{
// 		buffSkill->onBeaten(getSkillResultParam(), attacker, target);
// 	}
}

