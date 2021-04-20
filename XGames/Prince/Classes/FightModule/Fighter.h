#ifndef Fighter_h__
#define Fighter_h__

#include "cocos2d.h"
#include "Skill/NormalSkill.h"
#include "Skill/ActiveSkill.h"
#include "Skill/RushSkill.h"
#include "Skill/TriggerSkill.h"
#include "Skill/BuffSkill.h"
#include "Buff/AbstractBuff.h"
#include "FightStateMachine.h"

USING_NS_CC;
using namespace std;

class NormalSkill;
class ActiveSkill;
class RushSkill;
class TriggerSkill;
class BuffSkill;

class Fighter : public Ref
{

public:

	Fighter();
	virtual ~Fighter();

	static Fighter* create();

public:

	//角色攻击伤害类型
	enum ATTACK_TYPE
	{
		ATTACK_PHYSICAL = 1,
		ATTACK_MAGIC
	};

	//角色属性类型
	enum ATTACK_PROPERTY_TYPE
	{
		ATTACK_PROPERTY_POW = 1,
		ATTACK_PROPERTY_INT,
		ATTACK_PROPERTY_AGI
	};

	enum FIGHTER_JOB_TYPE
	{
		FIGHTER_JOB_DEFENSE = 1,		//防御性
		FIGHTER_JOB_ATK,				//攻击性
		FIGHTER_JOB_HELP,				//辅助性
		FIGHTER_JOB_FIRST				//先手性
	};

	enum FIGHTER_SEX_TYPE
	{
		FIGHTER_SEX_MALE,				//男性
		FIGHTER_SEX_FEMALE,				//女性
	};

public:

	int getHeroID() const;
	void setHeroID(int n);

	int getResID() const;
	void setResID(int n);

	string getName() const;
	void setName(const char* szName);

	float getPercentHP();


	//直接造成伤害(正数为加血，负数为减血)
	void onHarm(int harmValue);

	//攻击触发, 返回吸血效果值，如无吸血效果，则返回0
	int onBlood(int harmValue);

	//是否存活
	bool isAlive();

	//获取人物附加状态
	AbstractSkill::RESULT_STATE getState();

	//设置人物附加状态
	void setState(AbstractSkill::RESULT_STATE state);

	//清空人物附加状态
	void clearState();

	//设置普通攻击
	void setNormalSkill(int skillId);

	//设置主动技能
	void setActiveSkill(int skillId);

	//设置被动1技能
	void setRushSkill(int skillId);

	//设置被动技能2
	void setBuffSkill(int skillId);

	//设置被动技能3
	void setTriggerSkill(int skillId);

	//获取普通技能
	NormalSkill* getNormalSkill();

	//获取主动技能
	ActiveSkill* getActiveSkill();

	//获取被动1技能
	RushSkill* getRushSkill();

	//获取被动2技能
	BuffSkill* getBuffSkill();

	//获取被动3技能
	TriggerSkill* getTriggerSkill();

	//添加Buff,并返回添加的buffId
	BuffInfo addBuff(AbstractBuff* buff);

	//移除Buff,并返回移除的buffId
	BuffInfo removeBuff(AbstractBuff* buff);

	//获取所有Buff
	Vector<AbstractBuff*>* getVecBuff();

	//是否禁止全部攻击
	bool isBanAll();

	//是否禁止普通攻击
	bool isBanNormal();

	//是否禁止追击
	bool isBanRush();

	//是否禁止主动技能
	bool isBanActive();

	//是否霸体
	bool hasTyrants();

	//是否免疫负面buff
	bool isBuffResist();

	//是否是物理攻击免疫
	bool isPhyResist();

	//是否是魔法攻击免疫
	bool isMagResist();

	//是否无敌
	bool isInvincible();

	//获取角色攻击力
	float getFinalAtk();

	//获取角色物理防御力
	float getFinalPDef();

	//获取角色魔法防御力
	float getFinalMDef();

	//获取角色暴击率
	float getFinalCrit();

	//获取角色闪避率
	float getFinalDodge();

	//获取角色命中率
	float getFinalHitPrb();

	//获取角色暴击抗性
	float getFinalCritRes();

	//获取角色暴击伤害
	float getFinalCritCoe();

	//获取角色伤害加成
	float getFinalDamCoe();

	//获取角色伤害减免
	float getFinalDamRes();

	//扣减Buff持续回合
	void decreaseBuffRound(map<int, BuffUpdateParam>& mapBuffUpdateParam);

	//获取角色所在方
	FightStateMachine::E_SIDE getSide();

	//设置角色所在方
	void setSide(FightStateMachine::E_SIDE side);

	//变身接口
	void onChange(int monsterId);

	//加密属性
	ENCRYPT_LONG_GETTER_SETTER(CurrHP, mCurrHP);
	ENCRYPT_LONG_GETTER_SETTER(MaxHP, mMaxHP);
	ENCRYPT_LONG_GETTER_SETTER(InitialHp, mInitialHp);
	ENCRYPT_INT_GETTER_SETTER(TagType, mTagTyp);
	ENCRYPT_INT_GETTER_SETTER(AtkType, mAtkTyp);
	ENCRYPT_INT_GETTER_SETTER(JobTyp, mJobTyp);
	ENCRYPT_INT_GETTER_SETTER(SexTyp, mSexTyp);
	ENCRYPT_INT_GETTER_SETTER(Atk, mAtk);
	ENCRYPT_INT_GETTER_SETTER(Pdd, mPdd);
	ENCRYPT_INT_GETTER_SETTER(Mdd, mMdd);
	ENCRYPT_INT_GETTER_SETTER(Index, mPosIndex);
	ENCRYPT_FLOAT_GETTER_SETTER(CriPrb, mCriPrb);
	ENCRYPT_FLOAT_GETTER_SETTER(CriCoe, mCriCoe);
	ENCRYPT_FLOAT_GETTER_SETTER(CtrPrb, mCtrPrb);
	ENCRYPT_FLOAT_GETTER_SETTER(DodgePrb, mDodgePrb);
	ENCRYPT_FLOAT_GETTER_SETTER(HitPrb, mHitPrb);
	ENCRYPT_FLOAT_GETTER_SETTER(CriRes, mCriRes);
	ENCRYPT_FLOAT_GETTER_SETTER(DamCoe, mDamCoe);
	ENCRYPT_FLOAT_GETTER_SETTER(DamRes, mDamRes);


private:

	int mHeroId;		//人物ID
	int mResId;			//资源ID
	string mName;		//名称

	int mPosIndex;		//战斗位置0-8*(带*表示加密属性)

	long long mMaxHP;			//满值HP*
	long long mCurrHP;			//当前HP*
	long long mInitialHp;		//初始HP*

	int mTagTyp;		//角色属性类型，火、水、木
	int	mAtkTyp;		//攻击类型，物理攻击、魔法攻击*

	int mJobTyp;		//角色职业类型、先、攻、防、辅
	int mSexTyp;		//性别

	int	mAtk;			//物理攻击*
	int	mPdd;			//物理防御*
	int	mMdd;			//法术防御*

	float mCriPrb;		//暴击率*
	float mCriCoe;		//暴击伤害*
	float mCtrPrb;		//连击概率*
	float mDodgePrb;	//闪避概率*
	float mHitPrb;		//命中概率*
	float mCriRes;		//暴击抗性*

	float mDamCoe;		//伤害加成*
	float mDamRes;		//伤害减少*


	//当前人物角色属于哪方
	FightStateMachine::E_SIDE mSide;

	//技能造成的状态（浮空，击退, 倒地，无） 
	AbstractSkill::RESULT_STATE mState;

	//普通攻击
	NormalSkill* mNormalSkill;

	//主动技能
	ActiveSkill* mActiveSkill;

	//被动1技能，追击
	RushSkill* mRushSkill;

	//被动2技能，普攻附带Buff
	BuffSkill* mBuffSkill;

	//被动3技能，普攻附带追击
	TriggerSkill* mTriggerSkill;

	//人物附加buff
	Vector<AbstractBuff*> mVecBuff;

};

#endif // Fighter_h__
