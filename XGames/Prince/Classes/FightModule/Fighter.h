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

	//��ɫ�����˺�����
	enum ATTACK_TYPE
	{
		ATTACK_PHYSICAL = 1,
		ATTACK_MAGIC
	};

	//��ɫ��������
	enum ATTACK_PROPERTY_TYPE
	{
		ATTACK_PROPERTY_POW = 1,
		ATTACK_PROPERTY_INT,
		ATTACK_PROPERTY_AGI
	};

	enum FIGHTER_JOB_TYPE
	{
		FIGHTER_JOB_DEFENSE = 1,		//������
		FIGHTER_JOB_ATK,				//������
		FIGHTER_JOB_HELP,				//������
		FIGHTER_JOB_FIRST				//������
	};

	enum FIGHTER_SEX_TYPE
	{
		FIGHTER_SEX_MALE,				//����
		FIGHTER_SEX_FEMALE,				//Ů��
	};

public:

	int getHeroID() const;
	void setHeroID(int n);

	int getResID() const;
	void setResID(int n);

	string getName() const;
	void setName(const char* szName);

	float getPercentHP();


	//ֱ������˺�(����Ϊ��Ѫ������Ϊ��Ѫ)
	void onHarm(int harmValue);

	//��������, ������ѪЧ��ֵ��������ѪЧ�����򷵻�0
	int onBlood(int harmValue);

	//�Ƿ���
	bool isAlive();

	//��ȡ���︽��״̬
	AbstractSkill::RESULT_STATE getState();

	//�������︽��״̬
	void setState(AbstractSkill::RESULT_STATE state);

	//������︽��״̬
	void clearState();

	//������ͨ����
	void setNormalSkill(int skillId);

	//������������
	void setActiveSkill(int skillId);

	//���ñ���1����
	void setRushSkill(int skillId);

	//���ñ�������2
	void setBuffSkill(int skillId);

	//���ñ�������3
	void setTriggerSkill(int skillId);

	//��ȡ��ͨ����
	NormalSkill* getNormalSkill();

	//��ȡ��������
	ActiveSkill* getActiveSkill();

	//��ȡ����1����
	RushSkill* getRushSkill();

	//��ȡ����2����
	BuffSkill* getBuffSkill();

	//��ȡ����3����
	TriggerSkill* getTriggerSkill();

	//���Buff,��������ӵ�buffId
	BuffInfo addBuff(AbstractBuff* buff);

	//�Ƴ�Buff,�������Ƴ���buffId
	BuffInfo removeBuff(AbstractBuff* buff);

	//��ȡ����Buff
	Vector<AbstractBuff*>* getVecBuff();

	//�Ƿ��ֹȫ������
	bool isBanAll();

	//�Ƿ��ֹ��ͨ����
	bool isBanNormal();

	//�Ƿ��ֹ׷��
	bool isBanRush();

	//�Ƿ��ֹ��������
	bool isBanActive();

	//�Ƿ����
	bool hasTyrants();

	//�Ƿ����߸���buff
	bool isBuffResist();

	//�Ƿ�������������
	bool isPhyResist();

	//�Ƿ���ħ����������
	bool isMagResist();

	//�Ƿ��޵�
	bool isInvincible();

	//��ȡ��ɫ������
	float getFinalAtk();

	//��ȡ��ɫ���������
	float getFinalPDef();

	//��ȡ��ɫħ��������
	float getFinalMDef();

	//��ȡ��ɫ������
	float getFinalCrit();

	//��ȡ��ɫ������
	float getFinalDodge();

	//��ȡ��ɫ������
	float getFinalHitPrb();

	//��ȡ��ɫ��������
	float getFinalCritRes();

	//��ȡ��ɫ�����˺�
	float getFinalCritCoe();

	//��ȡ��ɫ�˺��ӳ�
	float getFinalDamCoe();

	//��ȡ��ɫ�˺�����
	float getFinalDamRes();

	//�ۼ�Buff�����غ�
	void decreaseBuffRound(map<int, BuffUpdateParam>& mapBuffUpdateParam);

	//��ȡ��ɫ���ڷ�
	FightStateMachine::E_SIDE getSide();

	//���ý�ɫ���ڷ�
	void setSide(FightStateMachine::E_SIDE side);

	//����ӿ�
	void onChange(int monsterId);

	//��������
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

	int mHeroId;		//����ID
	int mResId;			//��ԴID
	string mName;		//����

	int mPosIndex;		//ս��λ��0-8*(��*��ʾ��������)

	long long mMaxHP;			//��ֵHP*
	long long mCurrHP;			//��ǰHP*
	long long mInitialHp;		//��ʼHP*

	int mTagTyp;		//��ɫ�������ͣ���ˮ��ľ
	int	mAtkTyp;		//�������ͣ���������ħ������*

	int mJobTyp;		//��ɫְҵ���͡��ȡ�����������
	int mSexTyp;		//�Ա�

	int	mAtk;			//������*
	int	mPdd;			//�������*
	int	mMdd;			//��������*

	float mCriPrb;		//������*
	float mCriCoe;		//�����˺�*
	float mCtrPrb;		//��������*
	float mDodgePrb;	//���ܸ���*
	float mHitPrb;		//���и���*
	float mCriRes;		//��������*

	float mDamCoe;		//�˺��ӳ�*
	float mDamRes;		//�˺�����*


	//��ǰ�����ɫ�����ķ�
	FightStateMachine::E_SIDE mSide;

	//������ɵ�״̬�����գ�����, ���أ��ޣ� 
	AbstractSkill::RESULT_STATE mState;

	//��ͨ����
	NormalSkill* mNormalSkill;

	//��������
	ActiveSkill* mActiveSkill;

	//����1���ܣ�׷��
	RushSkill* mRushSkill;

	//����2���ܣ��չ�����Buff
	BuffSkill* mBuffSkill;

	//����3���ܣ��չ�����׷��
	TriggerSkill* mTriggerSkill;

	//���︽��buff
	Vector<AbstractBuff*> mVecBuff;

};

#endif // Fighter_h__
