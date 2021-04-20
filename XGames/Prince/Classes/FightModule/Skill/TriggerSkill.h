#ifndef TriggerSkill_h__
#define TriggerSkill_h__

#include "AbstractSkill.h"
#include "../Fighter.h"

class TriggerSkill : public AbstractSkill
{

public:

	enum TRIGGER_TYPE
	{
		EVERY_ROUND = 1, //每回合触发（每回合触发一次）
		TARGET_DEAD,	 //普通攻击目标死亡触发
		ENTER_FIGHT,	 //进入战斗（每场战斗只触发一次）
		COMBO_N,		 //我方出手回合连击数达到N（每回合触发一次）
		BEFORE_ATTACK,	 //普通攻击前触发（每回合触发一次）
		ROUND_N_ENEMY,	 //回合N敌方出手前(每回合只触发一次)
		ROUND_N			 //回合N(每回合只触发一次)
	};

	TriggerSkill();
	virtual ~TriggerSkill();

	//创建技能
	static TriggerSkill* create(int skillId);

public:

	//攻击阶段
	virtual void onAttack();

	//初始化技能
	virtual bool init(int skillId);

	//是否已经被触发
	bool isTrigger();

	//进入战斗(只触发一次)
	bool triggerFightStart();

	//每回合触发
	bool triggerRoundStart();

	//普通攻击击杀
	bool triggerTargetDead(AbstractSkill* currentSkill);

	//每次普通攻击前触发
	bool triggerBeforeAttack(AbstractSkill* currentSkill);

	//Combo数达到N时触发
	bool triggerHitCount();

	//回合数达到N时敌方出手前
	bool triggerRoundBeforeEnemy();

	//回合数达到N时触发
	bool triggerRound();

	//攻击结束
	void onRoundEnd();

	//加密属性
	ENCRYPT_INT_GETTER_SETTER(TriggerType, mTriggerType);
	ENCRYPT_INT_GETTER_SETTER(TriggerCount, mTriggerCount);
	ENCRYPT_INT_GETTER_SETTER(ChangeId, mChangeId);
	ENCRYPT_INT_GETTER_SETTER(ChangeSkin, mChangeSkin);

private:

	//触发类型
	int mTriggerType;

	//触发条件：连击数/回合数
	int mTriggerCount;

	//是否已经被触发
	bool mIsTrigger;

	//变身id
	int mChangeId;

	//变身标记
	int mChangeSkin;

	

};


#endif //TriggerSkill_h__