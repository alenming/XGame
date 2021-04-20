#ifndef BuffSkill_h__
#define BuffSkill_h__

#include "AbstractSkill.h"

//该技能已废弃
class BuffSkill : public AbstractSkill
{

public:

	BuffSkill();
	virtual ~BuffSkill();

	//创建技能
	static BuffSkill* create(int skillId);

public:

	//通过技能id初始化技能
	virtual bool init(int skillId);

	//攻击者触发的攻击事件
	void onAttack(SkillResultParam* resultParam, Fighter* attacker, Fighter* target);

	//被攻击者触发的被击事件
	void onBeaten(SkillResultParam* resultParam, Fighter* attacker, Fighter* target);

	//加密属性
	ENCRYPT_INT_GETTER_SETTER(BuffSourceType, mBuffSourceType);

public:

	enum BUFF_SOURCE_TYPE
	{
		SOURCE_ATTACK = 1,	//攻击触发
		SOURCE_BEATEN		//被击触发
	};

private:

	//被动2buff来源
	int mBuffSourceType;

};

#endif //BuffSkill_h__