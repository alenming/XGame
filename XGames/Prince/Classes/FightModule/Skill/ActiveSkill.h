#ifndef ActiveSkill_h__
#define ActiveSkill_h__

#include "AbstractSkill.h"
#include "Utils/ValueCrypt.h"

class ActiveSkill : public AbstractSkill
{

public:

	ActiveSkill();
	virtual ~ActiveSkill();

	//��������
	static ActiveSkill* create(int skillId);

public:

	//ͨ������id��ʼ������
	virtual bool init(int skillId);

	//�ж��Ƿ���ͷż���
	virtual bool onJudgeStart();

	//�����׶�
	virtual void onAttack();

	//���������Ƿ����ͷ�
	bool isActived();

	//���������Ƿ�ɷ���
	bool isReady();

	//����CD
	void decreaseCD();

	//������������
	void launchSkill();

	//ŭ��ֵ�Ƿ��㹻�ͷ�
	bool isSpEnough();

	ENCRYPT_INT_GETTER_SETTER(SpCost, mSpCost);
	ENCRYPT_INT_GETTER_SETTER(CurrentCD, mCurrentCD);
	ENCRYPT_INT_GETTER_SETTER(MaxCD, mMaxCD);
	ENCRYPT_INT_GETTER_SETTER(EnterCD, mEnterCD);
	ENCRYPT_INT_GETTER_SETTER(ResultStateType, mResultStateType);
	ENCRYPT_INT_GETTER_SETTER(ExtraTargetType, mExtraTargetType);
	ENCRYPT_INT_GETTER_SETTER(ExtraRangeType, mExtraRangeType);
	ENCRYPT_INT_GETTER_SETTER(ExtraBuffId, mExtraBuffId);

private:

	//ŭ������*
	int mSpCost;

	//�������CD*
	int mMaxCD;

	//���ܽ���CD*
	int mEnterCD;

	//���״̬*
	int mResultStateType;

	//���ܵ�ǰCD*
	int mCurrentCD;

	//���������Ƿ񱻼���*
	bool mIsActive;

	//�������ܸ���Ŀ������*
	int mExtraTargetType;

	//�������ܸ�����Χ����*
	int mExtraRangeType;

	//�������ܸ�������buff*
	int mExtraBuffId;

};



#endif //ActiveSkill_h__



