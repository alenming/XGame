#ifndef AbstractBuff_h__
#define AbstractBuff_h__

#include "cocos2d.h"
#include "../../DataManager/DataManager.h"
#include "../Skill/SkillResultParam.h"
#include "Utils/ValueCrypt.h"

USING_NS_CC;

class Fighter;

class AbstractBuff : public Ref
{

public:

	AbstractBuff();
	~AbstractBuff();
	
	//创建buff
	static AbstractBuff* create(int buffId);

	//初始化Buff
	virtual bool init(int buffId);

public:

	//buff主类型
	enum MAIN_BUFF_TYPE
	{
		CONTROL_BUFF = 1,	//控制类buff、控制类buff有优先级替换
		VALUE_BUFF			//数值类buff
	};

	enum BUFF_GAIN_TYPE
	{
		GAIN_BUFF = 1,		//增益buff
		DEBUFF				//减益buff
	};

	//buff子类型, mark:需对照数据表
	enum BUFF_TYPE
	{
		DIZZY = 1,			//眩晕
		SILENCE,			//沉默
		BAN_NOMAL,			//不能普攻
		BAN_RUSH,			//不能追打
		ATK_ADDITION,		//加/减攻击
		DEF_ADDITION,		//加/减物理防御
		MDEF_ADDITION,		//加/减法术防御
		CRIT_ADDITION,		//暴击概率
		DODGE_ADDITION,		//闪避概率
		BLOOD,				//吸血
		POISON,				//中毒
		REGON,				//回复
		BLEED,				//流血
		DAMAGE_UP,			//伤害增加
		DAMAGE_DOWN,		//伤害减少
		TYRANTS,			//霸体，追打伤害为1
		MDD_PDD_ADDITION,	//加减防御（物防/法防 都加）
		CRIT_RES_ADDITION,	//加减暴击抗性
		HIT_ADDITION,		//加减命中率
		CRIT_COE_ADDITION,	//加减暴击伤害
		BUFF_RESIST,		//免疫，免疫所有负面buff
		INVINCIBLE,			//无敌，免疫所有伤害和清除所有负面buff
		PHYSICAL_RESIST,	//物理攻击免疫
		MAGIC_RESIST		//法术攻击免疫
	};

	//引用值类型
	enum QUOTE_VALUE_TYPE
	{
		ATTACKER_ATTR = 1,		//引用攻击方数值，引用BUFF附加者
		TARGET_ATTR,			//引用被击方数值，引用BUFF佩带者
	};

	//引用值属性类型 mark:需对照数据表
	enum QUOTE_ATTR_TYPE
	{
		QUATE_ATK = 1,		//引用攻击
		QUATE_HP,			//引用当前血量
		QUATE_MAX_HP,		//引用最大血量
		QUATE_PDD,			//引用防御
		QUATE_CRIT,			//引用暴击
		QUATE_HARM,			//引用伤害
		QUATE_DAM_COE,		//引用伤害加成
		QUATE_DAM_RES,		//引用伤害减免
		QUATE_MDD,			//引用法术防御
		QUATE_CRIT_RES,		//引用暴击抗性
		QUATE_HIT,			//引用命中
		QUATE_CRIT_COE,		//引用暴击伤害
		QUATE_PDD_MDD,		//引用物理防御、魔法防御	
	};

public:

	/////////////数值类Buff触发接口//////////////////

	//攻击值修正值
	double atkAddition();

	//物理防御修正
	double defAddition();

	//魔法防御修正
	double mdefAddition();

	//暴击修正
	double critAddition();

	//闪避修正
	double dodgeAddition();

	//伤害增加加成
	double damageUpAddition();

	//伤害减少加成
	double damageDownAddition();

	//命中加成
	double hitAddition();

	//暴击抗性加成
	double critResAddition();

	//暴击伤害加成
	double critCoeAddition();

	//攻击时处理Buff效果(吸血)，返回吸血值
	int doBlood(int harm);

	//被击伤害处理(反击)
	int doBeaten(int harm);

	//每回合结束触发buff效果(中毒、回血)
	int doRoundTick();

	/////////////控制类Buff触发接口//////////////////

	//是否眩晕buff
	bool isBanAll();

	//是否禁止普通攻击buff
	bool isBanNormal();

	//是否禁止追击buff
	bool isBanRush();

	//是否禁止沉默buff
	bool isBanActive();

	/////////////通用Buff接口//////////////////

	//是否是霸体buff（追打伤害为1）
	bool isTyrants();

	//是否是免疫buff
	bool isBuffResist();

	//是否是无敌buff
	bool isInvincible();

	//是否是物理攻击免疫
	bool isPhyResist();

	//是否是法术攻击免疫
	bool isMagResist();

	//扣减buff持续回合
	bool decreaseRound();

	//添加buff
	BuffUpdateParam addToTarget(Fighter* target);

	//设置攻击者
	void setAttacker(Fighter* attacker);

	//获取buff的id
	int getBuffID();

	//是否是减益buff
	bool isDebuff();

	//获取一次性buff特效id
	int getOnceEffectResId();

	//获取持续性buff特效id
	int getConstancyEffectResId();

	//buff持续性效果是否需要翻面
	bool isCircleFlip();

	//buff一次性效果是否需要翻面
	bool isOnceFlip();

	//加密属性
	ENCRYPT_INT_GETTER_SETTER(MainBuffType, mMainBuffType);
	ENCRYPT_INT_GETTER_SETTER(BuffType, mBuffType);
	ENCRYPT_FLOAT_GETTER_SETTER(Chance, mChance);
	ENCRYPT_INT_GETTER_SETTER(MaxRound, mMaxRound);
	ENCRYPT_INT_GETTER_SETTER(CurrentRound, mCurrentRound);
	ENCRYPT_INT_GETTER_SETTER(GainType, mGainType);
	ENCRYPT_FLOAT_GETTER_SETTER(Harm, mHarm);
	ENCRYPT_INT_GETTER_SETTER(QuoteValueType, mQuoteValueType);
	ENCRYPT_INT_GETTER_SETTER(QuoteAttrType, mQuoteAttrType);
	ENCRYPT_FLOAT_GETTER_SETTER(FactorA, mFactorA);
	ENCRYPT_FLOAT_GETTER_SETTER(FactorB, mFactorB);
	ENCRYPT_INT_GETTER_SETTER(ReplacePriority, mReplacePriority);

private:

	//计算buff加成值
	double doAttrAddition();

	//计算buff加成值
	double doAttrAddition(int quoteAttrType);

	//获取角色属性
	double getFighterAttr(Fighter* fighter, int quoteAttrType);

protected:

	//buff施加者
	Fighter* mAttacker;

	//buff佩戴者
	Fighter* mTarget;

	//数据表原始数据
	RowData* mBuffData;

	//一次性buff特效id
	int mOnceEffectResId;

	//持续性buff特效id
	int mConstancyEffectResId;

	//buff持续效果是否翻面
	bool mCircleFlip;

	//buff一次性效果是否翻面
	bool mOnceFlip;

	/////////////////////////////////Buff通用属性/////////////////////////////////////////

	//Buff的id标识
	int mBuffId;

	//buff主类型*
	int mMainBuffType;

	//buff子类型*
	int mBuffType;

	//buff触发概率*
	float mChance;

	//buff持续回合*
	int mMaxRound;

	//buff当前剩余持续回合*
	int mCurrentRound;

	//buff增益减益类型*
	int mGainType;

	//被引用的伤害数值*
	float mHarm;

	//////////////////////////////////数值类buff属性////////////////////////////////////////

	//引用数值类型*
	int mQuoteValueType;

	//引用属性类型*
	int mQuoteAttrType;

	//数值*
	float mFactorA; //百分比
	float mFactorB;	//固定值

	//////////////////////////////////控制类buff属性////////////////////////////////////////

	//控制类buff替换优先级*
	int mReplacePriority;

};




#endif //AbstractBuff_h__