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
		HARM_NONE,			//���˺�
		HARM_NORMAL,		//��ͨ
		HARM_DODGE,			//����
		HARM_CRITICAL,		//����
		HARM_RESTRICTION,	//�������

		HARM_BLEED,			//��Ѫ(��ѪbuffЧ��)
		HARM_BLOOD,			//��Ѫ(��ѪbuffЧ��)
		HARM_POISON,		//�ж�
		HARM_CURE,			//�ظ�
		HARM_INVINCIBLE		//�޵�
	};

public:

	//�˺�Ŀ������
	int mTargetIndex;

	//�˺�Ŀ�귽
	FightStateMachine::E_SIDE mTargetSide;

	//�˺�����
	int mHarmType;

	//�˺�ֵ
	int mValue;

	//��������
	int mComboCount;
};

class SkillResultParam : public Ref
{

public:

	SkillResultParam();
	~SkillResultParam();

	static SkillResultParam* create();

public:

	//�������
	void clear();

public:

	//����id
	int mSkillId;

	//��������
	int mSkillType;

	//���ܻ�����
	int mComboCount;

	//������
	FightStateMachine::E_SIDE mAttackSide;

	//��������
	FightStateMachine::E_SIDE mTargetSide;

	//��ǰ������id
	int mAttackerIndex;

	//��ǰ��������id����
	vector<int> mVecTargetIndex;

	//��������Ч��
	vector<HarmParam> mVecTargetHarm;

	//��ѪЧ��
	HarmParam mBloodHarm;

	//�ҷ�buff���²���
	map<int, BuffUpdateParam> mMapMyBuffUpdateParam;

	//�з�buff���²���
	map<int, BuffUpdateParam> mMapEnemyBuffUpdateParam;

	//������
	int mChangeSkin;
};



class BuffInfo
{

public:

	//buffId
	int mBuffId;

	//buff����
	int mBuffType;

	//һ����buff��Чid
	int mOnceResId;

	//������buff��Чid
	int mCircleResId;

	//������buff�Ƿ���Ҫ����
	bool mCircleFlip;

	//һ����buff�Ƿ���Ҫ����
	bool mOnceFlip;

};

//buff���²���
class BuffUpdateParam
{

public:

	//�ҷ����buff
	vector<BuffInfo> mVecAddBuff;

	//�ҷ��Ƴ�buff
	vector<BuffInfo> mVecRemoveBuff;
};

//�غϸ��²���
class RoundParam
{

public:

	//�ҷ��غϽ����˺�����
	map<int, vector<HarmParam> > mMyRoundHarm;

	//�з��غϽ����˺�����
	map<int, vector<HarmParam> > mEnemyRoundHarm;

	//�ҷ�buff���²���
	map<int, BuffUpdateParam> mMapMyBuffUpdateParam;

	//�з�buff���²���
	map<int, BuffUpdateParam> mMapEnemyBuffUpdateParam;
};

#endif