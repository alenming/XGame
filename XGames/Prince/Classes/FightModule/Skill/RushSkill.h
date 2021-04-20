#ifndef RushSkill_h__
#define RushSkill_h__

#include "AbstractSkill.h"
#include "../Fighter.h"

class RushSkill : public AbstractSkill
{

public:

	RushSkill();
	virtual ~RushSkill();

	//创建技能
	static RushSkill* create(int skillId);

public:

	//初始化技能
	virtual bool init(int skillId);

	//判定是否可释放技能
	virtual bool onJudgeStart();

	//目标选定
	virtual void onTargetSelect();

	//伤害修正
	virtual void onHarmModification(Fighter* target, int& harm);

	//加密属性
	ENCRYPT_INT_GETTER_SETTER(PreStateType, mPreStateType);
	ENCRYPT_INT_GETTER_SETTER(PstStateType, mPstStateType);

protected:

	virtual void onSkillEffect(Fighter* target, HarmParam& harmParam, float& harm);

private:

	//攻击附加状态目标
	void selectStateTarget();

private:

	//被动1前置状态
	int mPreStateType;

	//被动1后置状态
	int mPstStateType;


};


#endif //RushSkill_h__