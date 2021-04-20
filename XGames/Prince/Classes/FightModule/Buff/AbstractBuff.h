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
	
	//����buff
	static AbstractBuff* create(int buffId);

	//��ʼ��Buff
	virtual bool init(int buffId);

public:

	//buff������
	enum MAIN_BUFF_TYPE
	{
		CONTROL_BUFF = 1,	//������buff��������buff�����ȼ��滻
		VALUE_BUFF			//��ֵ��buff
	};

	enum BUFF_GAIN_TYPE
	{
		GAIN_BUFF = 1,		//����buff
		DEBUFF				//����buff
	};

	//buff������, mark:��������ݱ�
	enum BUFF_TYPE
	{
		DIZZY = 1,			//ѣ��
		SILENCE,			//��Ĭ
		BAN_NOMAL,			//�����չ�
		BAN_RUSH,			//����׷��
		ATK_ADDITION,		//��/������
		DEF_ADDITION,		//��/���������
		MDEF_ADDITION,		//��/����������
		CRIT_ADDITION,		//��������
		DODGE_ADDITION,		//���ܸ���
		BLOOD,				//��Ѫ
		POISON,				//�ж�
		REGON,				//�ظ�
		BLEED,				//��Ѫ
		DAMAGE_UP,			//�˺�����
		DAMAGE_DOWN,		//�˺�����
		TYRANTS,			//���壬׷���˺�Ϊ1
		MDD_PDD_ADDITION,	//�Ӽ����������/���� ���ӣ�
		CRIT_RES_ADDITION,	//�Ӽ���������
		HIT_ADDITION,		//�Ӽ�������
		CRIT_COE_ADDITION,	//�Ӽ������˺�
		BUFF_RESIST,		//���ߣ��������и���buff
		INVINCIBLE,			//�޵У����������˺���������и���buff
		PHYSICAL_RESIST,	//����������
		MAGIC_RESIST		//������������
	};

	//����ֵ����
	enum QUOTE_VALUE_TYPE
	{
		ATTACKER_ATTR = 1,		//���ù�������ֵ������BUFF������
		TARGET_ATTR,			//���ñ�������ֵ������BUFF�����
	};

	//����ֵ�������� mark:��������ݱ�
	enum QUOTE_ATTR_TYPE
	{
		QUATE_ATK = 1,		//���ù���
		QUATE_HP,			//���õ�ǰѪ��
		QUATE_MAX_HP,		//�������Ѫ��
		QUATE_PDD,			//���÷���
		QUATE_CRIT,			//���ñ���
		QUATE_HARM,			//�����˺�
		QUATE_DAM_COE,		//�����˺��ӳ�
		QUATE_DAM_RES,		//�����˺�����
		QUATE_MDD,			//���÷�������
		QUATE_CRIT_RES,		//���ñ�������
		QUATE_HIT,			//��������
		QUATE_CRIT_COE,		//���ñ����˺�
		QUATE_PDD_MDD,		//�������������ħ������	
	};

public:

	/////////////��ֵ��Buff�����ӿ�//////////////////

	//����ֵ����ֵ
	double atkAddition();

	//�����������
	double defAddition();

	//ħ����������
	double mdefAddition();

	//��������
	double critAddition();

	//��������
	double dodgeAddition();

	//�˺����Ӽӳ�
	double damageUpAddition();

	//�˺����ټӳ�
	double damageDownAddition();

	//���мӳ�
	double hitAddition();

	//�������Լӳ�
	double critResAddition();

	//�����˺��ӳ�
	double critCoeAddition();

	//����ʱ����BuffЧ��(��Ѫ)��������Ѫֵ
	int doBlood(int harm);

	//�����˺�����(����)
	int doBeaten(int harm);

	//ÿ�غϽ�������buffЧ��(�ж�����Ѫ)
	int doRoundTick();

	/////////////������Buff�����ӿ�//////////////////

	//�Ƿ�ѣ��buff
	bool isBanAll();

	//�Ƿ��ֹ��ͨ����buff
	bool isBanNormal();

	//�Ƿ��ֹ׷��buff
	bool isBanRush();

	//�Ƿ��ֹ��Ĭbuff
	bool isBanActive();

	/////////////ͨ��Buff�ӿ�//////////////////

	//�Ƿ��ǰ���buff��׷���˺�Ϊ1��
	bool isTyrants();

	//�Ƿ�������buff
	bool isBuffResist();

	//�Ƿ����޵�buff
	bool isInvincible();

	//�Ƿ�������������
	bool isPhyResist();

	//�Ƿ��Ƿ�����������
	bool isMagResist();

	//�ۼ�buff�����غ�
	bool decreaseRound();

	//���buff
	BuffUpdateParam addToTarget(Fighter* target);

	//���ù�����
	void setAttacker(Fighter* attacker);

	//��ȡbuff��id
	int getBuffID();

	//�Ƿ��Ǽ���buff
	bool isDebuff();

	//��ȡһ����buff��Чid
	int getOnceEffectResId();

	//��ȡ������buff��Чid
	int getConstancyEffectResId();

	//buff������Ч���Ƿ���Ҫ����
	bool isCircleFlip();

	//buffһ����Ч���Ƿ���Ҫ����
	bool isOnceFlip();

	//��������
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

	//����buff�ӳ�ֵ
	double doAttrAddition();

	//����buff�ӳ�ֵ
	double doAttrAddition(int quoteAttrType);

	//��ȡ��ɫ����
	double getFighterAttr(Fighter* fighter, int quoteAttrType);

protected:

	//buffʩ����
	Fighter* mAttacker;

	//buff�����
	Fighter* mTarget;

	//���ݱ�ԭʼ����
	RowData* mBuffData;

	//һ����buff��Чid
	int mOnceEffectResId;

	//������buff��Чid
	int mConstancyEffectResId;

	//buff����Ч���Ƿ���
	bool mCircleFlip;

	//buffһ����Ч���Ƿ���
	bool mOnceFlip;

	/////////////////////////////////Buffͨ������/////////////////////////////////////////

	//Buff��id��ʶ
	int mBuffId;

	//buff������*
	int mMainBuffType;

	//buff������*
	int mBuffType;

	//buff��������*
	float mChance;

	//buff�����غ�*
	int mMaxRound;

	//buff��ǰʣ������غ�*
	int mCurrentRound;

	//buff�����������*
	int mGainType;

	//�����õ��˺���ֵ*
	float mHarm;

	//////////////////////////////////��ֵ��buff����////////////////////////////////////////

	//������ֵ����*
	int mQuoteValueType;

	//������������*
	int mQuoteAttrType;

	//��ֵ*
	float mFactorA; //�ٷֱ�
	float mFactorB;	//�̶�ֵ

	//////////////////////////////////������buff����////////////////////////////////////////

	//������buff�滻���ȼ�*
	int mReplacePriority;

};




#endif //AbstractBuff_h__