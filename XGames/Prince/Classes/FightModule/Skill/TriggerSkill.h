#ifndef TriggerSkill_h__
#define TriggerSkill_h__

#include "AbstractSkill.h"
#include "../Fighter.h"

class TriggerSkill : public AbstractSkill
{

public:

	enum TRIGGER_TYPE
	{
		EVERY_ROUND = 1, //ÿ�غϴ�����ÿ�غϴ���һ�Σ�
		TARGET_DEAD,	 //��ͨ����Ŀ����������
		ENTER_FIGHT,	 //����ս����ÿ��ս��ֻ����һ�Σ�
		COMBO_N,		 //�ҷ����ֻغ��������ﵽN��ÿ�غϴ���һ�Σ�
		BEFORE_ATTACK,	 //��ͨ����ǰ������ÿ�غϴ���һ�Σ�
		ROUND_N_ENEMY,	 //�غ�N�з�����ǰ(ÿ�غ�ֻ����һ��)
		ROUND_N			 //�غ�N(ÿ�غ�ֻ����һ��)
	};

	TriggerSkill();
	virtual ~TriggerSkill();

	//��������
	static TriggerSkill* create(int skillId);

public:

	//�����׶�
	virtual void onAttack();

	//��ʼ������
	virtual bool init(int skillId);

	//�Ƿ��Ѿ�������
	bool isTrigger();

	//����ս��(ֻ����һ��)
	bool triggerFightStart();

	//ÿ�غϴ���
	bool triggerRoundStart();

	//��ͨ������ɱ
	bool triggerTargetDead(AbstractSkill* currentSkill);

	//ÿ����ͨ����ǰ����
	bool triggerBeforeAttack(AbstractSkill* currentSkill);

	//Combo���ﵽNʱ����
	bool triggerHitCount();

	//�غ����ﵽNʱ�з�����ǰ
	bool triggerRoundBeforeEnemy();

	//�غ����ﵽNʱ����
	bool triggerRound();

	//��������
	void onRoundEnd();

	//��������
	ENCRYPT_INT_GETTER_SETTER(TriggerType, mTriggerType);
	ENCRYPT_INT_GETTER_SETTER(TriggerCount, mTriggerCount);
	ENCRYPT_INT_GETTER_SETTER(ChangeId, mChangeId);
	ENCRYPT_INT_GETTER_SETTER(ChangeSkin, mChangeSkin);

private:

	//��������
	int mTriggerType;

	//����������������/�غ���
	int mTriggerCount;

	//�Ƿ��Ѿ�������
	bool mIsTrigger;

	//����id
	int mChangeId;

	//������
	int mChangeSkin;

	

};


#endif //TriggerSkill_h__