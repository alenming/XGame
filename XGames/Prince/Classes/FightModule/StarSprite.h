#ifndef StarSprite_h__
#define StarSprite_h__

#include "cocos2d.h"
#include "Skill/StarSkill.h"
#include "FightStateMachine.h"

USING_NS_CC;
using namespace std;

class StarSprite : public Ref
{

public:

	StarSprite();
	virtual ~StarSprite();

	static StarSprite* create(int id, int level, FightStateMachine::E_SIDE side);

public:

	bool init(int id, int level, FightStateMachine::E_SIDE side);

	//获取星灵方位
	FightStateMachine::E_SIDE getSide();

	//获取星灵基础伤害值
	int getBasicDamage();

	//获取星灵随等级增加的伤害系数加成
	float getAddtionDamageCoe();

	//获取星灵攻击类型
	int getAttackType();

	//获取星灵技能
	StarSkill* getStarSkill();

private:

	//星灵id
	int mId;

	//星灵等级
	int mLevel;

	//星灵方位
	FightStateMachine::E_SIDE mSide;

	//攻击类型(物理攻击、法术攻击)
	int mAttackType;

	//每一级增加的伤害系数
	float mGrowDamageCoe;

	//伤害计算基础值
	int mBasicDamage;

	//星灵主动技能
	StarSkill* mStarSkill;

	//星灵主动技能Resid
	int mStarSkillResId;
};


#endif //StarSprite_h__