#ifndef RushSkill_h__
#define RushSkill_h__

#include "AbstractSkill.h"
#include "../Fighter.h"

class RushSkill : public AbstractSkill
{

public:

	RushSkill();
	virtual ~RushSkill();

	//��������
	static RushSkill* create(int skillId);

public:

	//��ʼ������
	virtual bool init(int skillId);

	//�ж��Ƿ���ͷż���
	virtual bool onJudgeStart();

	//Ŀ��ѡ��
	virtual void onTargetSelect();

	//�˺�����
	virtual void onHarmModification(Fighter* target, int& harm);

	//��������
	ENCRYPT_INT_GETTER_SETTER(PreStateType, mPreStateType);
	ENCRYPT_INT_GETTER_SETTER(PstStateType, mPstStateType);

protected:

	virtual void onSkillEffect(Fighter* target, HarmParam& harmParam, float& harm);

private:

	//��������״̬Ŀ��
	void selectStateTarget();

private:

	//����1ǰ��״̬
	int mPreStateType;

	//����1����״̬
	int mPstStateType;


};


#endif //RushSkill_h__