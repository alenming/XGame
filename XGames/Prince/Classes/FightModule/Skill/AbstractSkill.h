#ifndef AbstractSkill_h__
#define AbstractSkill_h__

#include "cocos2d.h"
#include "../../DataManager/DataManager.h"
#include "../FightStateMachine.h"
#include "../UIFighterDef.h"
#include "Utils/ValueCrypt.h"
#include "SkillResultParam.h"
#include <map>

#define SKILL_ACCURACY		10000	//��ȷ��
#define FLOAT_COE_MAX		1.05f	//�����˺�����ϵ��
#define FLOAT_COE_MIN		0.95f	//�����˺�����ϵ��
#define RESTRICTION_COE		1.15f	//�������ϵ��
#define DEFEND_MIN_COE		0.5f	//����ϵ����������������
#define DEFEND_MAX_COE		2.0f	//
#define DODGE_MAX			0.38f	//���������

//�ȽϺ�
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

	//ͨ������id��ʼ������
	virtual bool init(int skillId);

public:

	//��ʼ�ж��׶�
	virtual bool onJudgeStart();

	//Ŀ��ѡ���׶�
	virtual void onTargetSelect();

	//�����׶�
	virtual void onAttack();

	//�ж��׶�
	virtual void onJudgeEnd();

	//ͨ��Ŀ��ѡ��
	vector<int> onTargetSelect(FightStateMachine::E_SIDE targetSide, int rangeType);

	//��ȡ������Ч����
	SKILL_EFFECT* getSkillEffectParam();

	//��ȡ������ֵ������
	SkillResultParam* getSkillResultParam();

	//�����ֵ������
	void clearResultParam();

	//���ü���������
	void setAttacker(Fighter* fighter);

	//��ȡ����������
	Fighter* getAttacker();

	//��ȡ����Ŀ�귽��
	FightStateMachine::E_SIDE getTargetSide(int targetType);

	//�Ƿ�����ͨ����
	bool isNormalSkill();

	//�Ƿ�����������
	bool isActiveSkill();

	//�Ƿ��Ǳ���1����
	bool isRushSkill();

	//�Ƿ��Ǳ���2����
	bool isBuffSkill();

	//�Ƿ��Ǳ���3����
	bool isTriggerSkill();

	//�Ƿ������鼼��
	bool isStarSkill();

	//��ȡ������Դid
	int getSkillResId();

	//��ȡ����ID
	int getSkillId(){ return mSkillId; }

	//���Ӹ�����������Buff
	void doBuff(int buffId, SkillResultParam* resultParam, Fighter* target, Fighter* attacker);

	//��������
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

	//�����ߴ����Ĺ����¼�
	virtual void onAttack(Fighter* attacker, Fighter* target, int harm);

	//�������ߴ����ı����¼�
	virtual void onBeaten(Fighter* attacker, Fighter* target, int harm);

	//�˺�����
	virtual void onHarmModification(Fighter* target, int& harm);

	//���ݲ�ͬ�ļ��ܵ�����ֵ���㣬�Լ�����״̬
	virtual void onSkillEffect(Fighter* target, HarmParam& harmParam, float& harm);

	//�������buffЧ��
	void clearFighterBuff(Fighter* target, bool clearDebuff = true);

	//����Ŀ��
	vector<int> selectSingleTarget(FightStateMachine::E_SIDE targetSide);

	//���Ŀ��
	vector<int> selectRandomTarget(FightStateMachine::E_SIDE targetSide, int count);

	//ȫ�幥��
	vector<int> selectAllTarget(FightStateMachine::E_SIDE targetSide, int rangeType);

	//����
	vector<int> selectMySelfTarget();

	//�߼����Ź���
	vector<int> selectRowTarget(FightStateMachine::E_SIDE targetSide, int rangeType);

	//�߼����Ź���
	vector<int> selectColTarget(FightStateMachine::E_SIDE targetSide);

	//���Ѫ������
	vector<int> selectMaxHpTarget(FightStateMachine::E_SIDE targetSide);

	//���Ѫ������
	vector<int> selectMinHpTarget(FightStateMachine::E_SIDE targetSide, int count = 1);

	//���Ѫ���ٷֱȹ���
	vector<int> selectMinPercentTarget(FightStateMachine::E_SIDE targetSide, int count = 1);

	//�ж��Ƿ��������
	bool isRestriction(int attackerTag, int targetTag);

	//���buff������Ϣ
	void addBuffUpdateInfo(SkillResultParam* resultParam, Fighter* target, BuffInfo& buffInfo, bool isAdd);

	//���buff������Ϣ
	void addBuffUpdateInfo(SkillResultParam* resultParam, Fighter* target, BuffUpdateParam& buffUpdateParam);

public:

	//��������
	enum SKILL_TYPE
	{
		NORMAL_SKILL = 1,	//��ͨ����
		ACTIVE_SKILL,		//��������
		RUSH_SKILL,			//����1��׷����
		BUFF_SKILL,			//����2���ͷ�buff
		TRIGGER_SKILL,		//����3����������
		STAR_SKILL			//���鼼��
	};

	//��������Ŀ��
	enum TARGET_TYPE
	{
		ENEMY = 1,	//�з�
		MINE		//�ҷ�
	};

	//�������÷�Χ
	enum RANGE
	{
		SINGLE = 0,			//����
		RANDOM_1,			//���1��
		RANDOM_2,			//���2��
		RANDOM_3,			//���3��
		RANDOM_4,			//���4��
		HORIZONTAL,			//�߼����Ź���
		VERTICAL,			//�߼���ֱ����
		MIN_HP,				//���Ѫ��
		MAX_HP,				//���Ѫ��
		ALL,				//ȫ��
		STATE_TARGET,		//����״̬��
		MYSELF,				//����
		HORIZONTAL_2,		//����
		HORIZONTAL_3,		//����
		MIN_HP_PERCENT,		//���Ѫ���ٷֱ�
		MIN_HP_2,			//�������Ѫ��
		MIN_HP_PERCENT_2,	//�������Ѫ���ٷֱ�
		MIN_HP_3,			//�������Ѫ��
		MIN_HP_PERCENT_3,	//�������Ѫ���ٷֱ�
		TAGET_HERO_FIRST,	//��������Ӣ��
		TAGET_HERO_ATTACK,	//���й���Ӣ��
		TAGET_HERO_DEFENSE,	//���з���Ӣ��
		TAGET_HERO_SUPPORT,	//���и���Ӣ��
		TAGET_HERO_MALE,	//��������Ӣ��
		TAGET_HERO_FEMALE	//����Ů��Ӣ��
	};

	enum DAMAGE_TYPE
	{
		NONE_DAMAGE = 0,
		HARM,	//�˺���
		CURE	//������
	};

	enum RESULT_STATE
	{
		NONE_STATE = 0,		//��״̬
		FELL,				//���� �� ��
		BACK,				//���� �� ��
		FLOAT,				//���� �� ��
	};

	enum SKILL_BASE_VALUE_TYPE
	{
		BASE_VALUE_TYPE_ATK = 1,			//���ù����߹���
		BASE_VALUE_TYPE_TARGET_HP = 2,		//����Ŀ�����Ѫ��
	};

protected:

	//���ݱ�ԭʼ����
	RowData* mSkillData;

	////////////////////////����ͨ������////////////////////////////////////

	//����id
	int mSkillId;

	//������Դid
	int mSkillResId;

	//��������*
	int mSkillType;

	//���ܹ���Ŀ��*
	int mTargetType;

	//����Ŀ�귶Χ*
	int mRangeType;

	//�˺�����*
	int mDamageType;

	//��������*
	int mComboCount;

	//�˺�ϵ��*
	float mDamageCoe;

	//����ʩ�ӵ�buff*
	int mMBuffId;

	//�Ƿ����buff*
	int mClearBuff;

	//�̶��˺�
	int mExtraDamage;

	//�˺��������õĻ���ֵ
	int mBaseValueType;

	////////////////////////����ս�����ݼ�����////////////////////////////////////
	
	//�����ͷ���
	Fighter* mAttacker;

	//ս����ɫ����ģ��
	FightModel* mFightModel;

	//���ܵĽ����������
	SkillResultParam mResultParam;

	//������Ч����
	SKILL_EFFECT mEffectParam;

};



#endif