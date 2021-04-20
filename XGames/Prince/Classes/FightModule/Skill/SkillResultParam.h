#ifndef SkillResultParam_h__
#define SkillResultParam_h__

#include "../FightStateMachine.h"

class BuffUpdateParam;

class HarmParam
{

public:

	HarmParam()
		: mHarmType(HARM_NORMAL)
		, mValue(0)
		, mComboCount(0)
		, mTargetIndex(-1)
		, mTargetSide(FightStateMachine::E_SIDE::MY_SIDE)
	{}

	~HarmParam(){}

	enum HARM_TYPE
	{
		HARM_NONE,			//无伤害
		HARM_NORMAL,		//普通
		HARM_DODGE,			//闪避
		HARM_CRITICAL,		//暴击
		HARM_RESTRICTION,	//属性相克

		HARM_BLEED,			//流血(流血buff效果)
		HARM_BLOOD,			//吸血(吸血buff效果)
		HARM_POISON,		//中毒
		HARM_CURE,			//回复
		HARM_INVINCIBLE		//无敌
	};

public:

	//伤害目标索引
	int mTargetIndex;

	//伤害目标方
	FightStateMachine::E_SIDE mTargetSide;

	//伤害类型
	int mHarmType;

	//伤害值
	int mValue;

	//击打数量
	int mComboCount;
};

class SkillResultParam : public Ref
{

public:

	SkillResultParam();
	~SkillResultParam();

	static SkillResultParam* create();

public:

	//清空数据
	void clear();

public:

	//技能id
	int mSkillId;

	//技能类型
	int mSkillType;

	//技能击打数
	int mComboCount;

	//攻击方
	FightStateMachine::E_SIDE mAttackSide;

	//被攻击方
	FightStateMachine::E_SIDE mTargetSide;

	//当前攻击者id
	int mAttackerIndex;

	//当前被攻击者id序列
	vector<int> mVecTargetIndex;

	//被攻击者效果
	vector<HarmParam> mVecTargetHarm;

	//吸血效果
	HarmParam mBloodHarm;

	//我方buff更新参数
	map<int, BuffUpdateParam> mMapMyBuffUpdateParam;

	//敌方buff更新参数
	map<int, BuffUpdateParam> mMapEnemyBuffUpdateParam;

	//变身标记
	int mChangeSkin;
};



class BuffInfo
{

public:

	//buffId
	int mBuffId;

	//buff类型
	int mBuffType;

	//一次性buff特效id
	int mOnceResId;

	//持续性buff特效id
	int mCircleResId;

	//持续性buff是否需要翻面
	bool mCircleFlip;

	//一次性buff是否需要翻面
	bool mOnceFlip;

};

//buff更新参数
class BuffUpdateParam
{

public:

	//我方添加buff
	vector<BuffInfo> mVecAddBuff;

	//我方移除buff
	vector<BuffInfo> mVecRemoveBuff;
};

//回合更新参数
class RoundParam
{

public:

	//我方回合结束伤害参数
	map<int, vector<HarmParam> > mMyRoundHarm;

	//敌方回合结束伤害参数
	map<int, vector<HarmParam> > mEnemyRoundHarm;

	//我方buff更新参数
	map<int, BuffUpdateParam> mMapMyBuffUpdateParam;

	//敌方buff更新参数
	map<int, BuffUpdateParam> mMapEnemyBuffUpdateParam;
};

#endif