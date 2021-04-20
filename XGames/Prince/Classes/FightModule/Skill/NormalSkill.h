#ifndef NormalSkill_h__
#define NormalSkill_h__

#include "AbstractSkill.h"

class NormalSkill : public AbstractSkill
{

public:

	NormalSkill();
	virtual ~NormalSkill();

	//��������
	static NormalSkill* create(int skillId);

public:

	virtual bool init(int skillId);

	//�ж��Ƿ���ͷż���
	virtual bool onJudgeStart();

protected:

	//���ݲ�ͬ�ļ��ܵ�����ֵ���㣬�Լ�����״̬
	virtual void onSkillEffect(Fighter* target, HarmParam& harmParam, float& harm);

	//�����ߴ����Ĺ����¼�
	virtual void onAttack(Fighter* attacker, Fighter* target, int harm);

	//�������ߴ����ı����¼�
	virtual void onBeaten(Fighter* attacker, Fighter* target, int harm);

	//��������
	ENCRYPT_INT_GETTER_SETTER(ResultStateType, mResultStateType);
	ENCRYPT_FLOAT_GETTER_SETTER(MutiComboCoe, mMutiComboCoe);
	ENCRYPT_INT_GETTER_SETTER(MutiComboCount, mMutiComboCount);

private:

	//��ͨ������ɵ�״̬
	int mResultStateType;

	//��ι������˺�ϵ��
	float mMutiComboCoe;

	//��ι���ʱ�Ĺ�������
	int mMutiComboCount;

	//��ι���������Ч����
	SKILL_EFFECT mExtraEffectParam;
	
	//���ι���������Ч����
	SKILL_EFFECT mNormalEffectParam;

};

#endif //NormalSkill_h__