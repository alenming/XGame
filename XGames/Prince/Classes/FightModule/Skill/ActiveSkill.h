#ifndef ActiveSkill_h__
#define ActiveSkill_h__

#include "AbstractSkill.h"
#include "Utils/ValueCrypt.h"

class ActiveSkill : public AbstractSkill
{

public:

	ActiveSkill();
	virtual ~ActiveSkill();

	//创建技能
	static ActiveSkill* create(int skillId);

public:

	//通过技能id初始化技能
	virtual bool init(int skillId);

	//判定是否可释放技能
	virtual bool onJudgeStart();

	//攻击阶段
	virtual void onAttack();

	//主动技能是否已释放
	bool isActived();

	//主动技能是否可发动
	bool isReady();

	//减少CD
	void decreaseCD();

	//发动主动技能
	void launchSkill();

	//怒气值是否足够释放
	bool isSpEnough();

	ENCRYPT_INT_GETTER_SETTER(SpCost, mSpCost);
	ENCRYPT_INT_GETTER_SETTER(CurrentCD, mCurrentCD);
	ENCRYPT_INT_GETTER_SETTER(MaxCD, mMaxCD);
	ENCRYPT_INT_GETTER_SETTER(EnterCD, mEnterCD);
	ENCRYPT_INT_GETTER_SETTER(ResultStateType, mResultStateType);
	ENCRYPT_INT_GETTER_SETTER(ExtraTargetType, mExtraTargetType);
	ENCRYPT_INT_GETTER_SETTER(ExtraRangeType, mExtraRangeType);
	ENCRYPT_INT_GETTER_SETTER(ExtraBuffId, mExtraBuffId);

private:

	//怒气消耗*
	int mSpCost;

	//技能最大CD*
	int mMaxCD;

	//技能进场CD*
	int mEnterCD;

	//造成状态*
	int mResultStateType;

	//技能当前CD*
	int mCurrentCD;

	//主动技能是否被激活*
	bool mIsActive;

	//主动技能辅攻目标类型*
	int mExtraTargetType;

	//主动技能辅攻范围类型*
	int mExtraRangeType;

	//主动技能辅攻附加buff*
	int mExtraBuffId;

};



#endif //ActiveSkill_h__



