#ifndef NormalSkill_h__
#define NormalSkill_h__

#include "AbstractSkill.h"

class NormalSkill : public AbstractSkill
{

public:

	NormalSkill();
	virtual ~NormalSkill();

	//创建技能
	static NormalSkill* create(int skillId);

public:

	virtual bool init(int skillId);

	//判定是否可释放技能
	virtual bool onJudgeStart();

protected:

	//根据不同的技能调整数值计算，以及附加状态
	virtual void onSkillEffect(Fighter* target, HarmParam& harmParam, float& harm);

	//攻击者触发的攻击事件
	virtual void onAttack(Fighter* attacker, Fighter* target, int harm);

	//被攻击者触发的被击事件
	virtual void onBeaten(Fighter* attacker, Fighter* target, int harm);

	//加密属性
	ENCRYPT_INT_GETTER_SETTER(ResultStateType, mResultStateType);
	ENCRYPT_FLOAT_GETTER_SETTER(MutiComboCoe, mMutiComboCoe);
	ENCRYPT_INT_GETTER_SETTER(MutiComboCount, mMutiComboCount);

private:

	//普通攻击造成的状态
	int mResultStateType;

	//多次攻击总伤害系数
	float mMutiComboCoe;

	//多次攻击时的攻击次数
	int mMutiComboCount;

	//多次攻击技能特效参数
	SKILL_EFFECT mExtraEffectParam;
	
	//单次攻击技能特效参数
	SKILL_EFFECT mNormalEffectParam;

};

#endif //NormalSkill_h__