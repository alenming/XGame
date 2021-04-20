#ifndef AbstractSkill_h__
#define AbstractSkill_h__

#include "cocos2d.h"
#include "../../DataManager/DataManager.h"
#include "../FightStateMachine.h"
#include "../UIFighterDef.h"
#include "Utils/ValueCrypt.h"
#include "SkillResultParam.h"
#include <map>

#define SKILL_ACCURACY		10000	//精确度
#define FLOAT_COE_MAX		1.05f	//浮动伤害上限系数
#define FLOAT_COE_MIN		0.95f	//浮动伤害下限系数
#define RESTRICTION_COE		1.15f	//属性相克系数
#define DEFEND_MIN_COE		0.5f	//防御系数，参与修正计算
#define DEFEND_MAX_COE		2.0f	//
#define DODGE_MAX			0.38f	//最高闪避率

//比较宏
#define USER_MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define USER_MIN(a, b)	(((a) < (b)) ? (a) : (b))

USING_NS_CC;

class FightModel;
class Fighter;

class AbstractSkill : public Ref 
{

public:

	AbstractSkill();
	virtual ~AbstractSkill();

	//通过技能id初始化技能
	virtual bool init(int skillId);

public:

	//开始判定阶段
	virtual bool onJudgeStart();

	//目标选定阶段
	virtual void onTargetSelect();

	//攻击阶段
	virtual void onAttack();

	//判定阶段
	virtual void onJudgeEnd();

	//通用目标选择
	vector<int> onTargetSelect(FightStateMachine::E_SIDE targetSide, int rangeType);

	//获取技能特效参数
	SKILL_EFFECT* getSkillEffectParam();

	//获取技能数值运算结果
	SkillResultParam* getSkillResultParam();

	//清空数值运算结果
	void clearResultParam();

	//设置技能所有者
	void setAttacker(Fighter* fighter);

	//获取技能所有者
	Fighter* getAttacker();

	//获取攻击目标方向
	FightStateMachine::E_SIDE getTargetSide(int targetType);

	//是否是普通技能
	bool isNormalSkill();

	//是否是主动技能
	bool isActiveSkill();

	//是否是被动1技能
	bool isRushSkill();

	//是否是被动2技能
	bool isBuffSkill();

	//是否是被动3技能
	bool isTriggerSkill();

	//是否是星灵技能
	bool isStarSkill();

	//获取技能资源id
	int getSkillResId();

	//获取技能ID
	int getSkillId(){ return mSkillId; }

	//附加给被攻击方的Buff
	void doBuff(int buffId, SkillResultParam* resultParam, Fighter* target, Fighter* attacker);

	//加密属性
	ENCRYPT_INT_GETTER_SETTER(RangeType, mRangeType);
	ENCRYPT_INT_GETTER_SETTER(TargetType, mTargetType);
	ENCRYPT_FLOAT_GETTER_SETTER(DamageCoe, mDamageCoe);
	ENCRYPT_INT_GETTER_SETTER(SkillType, mSkillType);
	ENCRYPT_INT_GETTER_SETTER(DamageType, mDamageType);
	ENCRYPT_INT_GETTER_SETTER(ComboCount, mComboCount);
	ENCRYPT_INT_GETTER_SETTER(MBuffId, mMBuffId);
	ENCRYPT_INT_GETTER_SETTER(ClearBuff, mClearBuff);
	ENCRYPT_INT_GETTER_SETTER(ExtraDamage, mExtraDamage);
	ENCRYPT_INT_GETTER_SETTER(BaseValueType, mBaseValueType);

protected:

	//攻击者触发的攻击事件
	virtual void onAttack(Fighter* attacker, Fighter* target, int harm);

	//被攻击者触发的被击事件
	virtual void onBeaten(Fighter* attacker, Fighter* target, int harm);

	//伤害修正
	virtual void onHarmModification(Fighter* target, int& harm);

	//根据不同的技能调整数值计算，以及附加状态
	virtual void onSkillEffect(Fighter* target, HarmParam& harmParam, float& harm);

	//技能清除buff效果
	void clearFighterBuff(Fighter* target, bool clearDebuff = true);

	//单体目标
	vector<int> selectSingleTarget(FightStateMachine::E_SIDE targetSide);

	//随机目标
	vector<int> selectRandomTarget(FightStateMachine::E_SIDE targetSide, int count);

	//全体攻击
	vector<int> selectAllTarget(FightStateMachine::E_SIDE targetSide, int rangeType);

	//自身
	vector<int> selectMySelfTarget();

	//逻辑横排攻击
	vector<int> selectRowTarget(FightStateMachine::E_SIDE targetSide, int rangeType);

	//逻辑纵排攻击
	vector<int> selectColTarget(FightStateMachine::E_SIDE targetSide);

	//最高血量攻击
	vector<int> selectMaxHpTarget(FightStateMachine::E_SIDE targetSide);

	//最低血量攻击
	vector<int> selectMinHpTarget(FightStateMachine::E_SIDE targetSide, int count = 1);

	//最低血量百分比攻击
	vector<int> selectMinPercentTarget(FightStateMachine::E_SIDE targetSide, int count = 1);

	//判定是否属性相克
	bool isRestriction(int attackerTag, int targetTag);

	//添加buff更新信息
	void addBuffUpdateInfo(SkillResultParam* resultParam, Fighter* target, BuffInfo& buffInfo, bool isAdd);

	//添加buff更新信息
	void addBuffUpdateInfo(SkillResultParam* resultParam, Fighter* target, BuffUpdateParam& buffUpdateParam);

public:

	//技能类型
	enum SKILL_TYPE
	{
		NORMAL_SKILL = 1,	//普通攻击
		ACTIVE_SKILL,		//主动技能
		RUSH_SKILL,			//被动1，追打技能
		BUFF_SKILL,			//被动2，释放buff
		TRIGGER_SKILL,		//被动3，连击技能
		STAR_SKILL			//精灵技能
	};

	//技能作用目标
	enum TARGET_TYPE
	{
		ENEMY = 1,	//敌方
		MINE		//我方
	};

	//技能作用范围
	enum RANGE
	{
		SINGLE = 0,			//单体
		RANDOM_1,			//随机1个
		RANDOM_2,			//随机2个
		RANDOM_3,			//随机3个
		RANDOM_4,			//随机4个
		HORIZONTAL,			//逻辑横排攻击
		VERTICAL,			//逻辑垂直攻击
		MIN_HP,				//最低血量
		MAX_HP,				//最高血量
		ALL,				//全体
		STATE_TARGET,		//附带状态表
		MYSELF,				//自身
		HORIZONTAL_2,		//中排
		HORIZONTAL_3,		//后排
		MIN_HP_PERCENT,		//最低血量百分比
		MIN_HP_2,			//两个最低血量
		MIN_HP_PERCENT_2,	//两个最低血量百分比
		MIN_HP_3,			//三个最低血量
		MIN_HP_PERCENT_3,	//三个最低血量百分比
		TAGET_HERO_FIRST,	//所有先手英雄
		TAGET_HERO_ATTACK,	//所有攻击英雄
		TAGET_HERO_DEFENSE,	//所有防御英雄
		TAGET_HERO_SUPPORT,	//所有辅助英雄
		TAGET_HERO_MALE,	//所有男性英雄
		TAGET_HERO_FEMALE	//所有女性英雄
	};

	enum DAMAGE_TYPE
	{
		NONE_DAMAGE = 0,
		HARM,	//伤害类
		CURE	//治愈类
	};

	enum RESULT_STATE
	{
		NONE_STATE = 0,		//无状态
		FELL,				//倒地 雷 人
		BACK,				//击退 火 地
		FLOAT,				//浮空 风 天
	};

	enum SKILL_BASE_VALUE_TYPE
	{
		BASE_VALUE_TYPE_ATK = 1,			//引用攻击者攻击
		BASE_VALUE_TYPE_TARGET_HP = 2,		//引用目标最大血量
	};

protected:

	//数据表原始数据
	RowData* mSkillData;

	////////////////////////技能通用属性////////////////////////////////////

	//技能id
	int mSkillId;

	//技能资源id
	int mSkillResId;

	//技能类型*
	int mSkillType;

	//技能攻击目标*
	int mTargetType;

	//攻击目标范围*
	int mRangeType;

	//伤害类型*
	int mDamageType;

	//连击数量*
	int mComboCount;

	//伤害系数*
	float mDamageCoe;

	//技能施加的buff*
	int mMBuffId;

	//是否清除buff*
	int mClearBuff;

	//固定伤害
	int mExtraDamage;

	//伤害计算引用的基础值
	int mBaseValueType;

	////////////////////////技能战斗数据计算结果////////////////////////////////////
	
	//技能释放者
	Fighter* mAttacker;

	//战斗角色数据模型
	FightModel* mFightModel;

	//技能的结果运算数据
	SkillResultParam mResultParam;

	//技能特效参数
	SKILL_EFFECT mEffectParam;

};



#endif