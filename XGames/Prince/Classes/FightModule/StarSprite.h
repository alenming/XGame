#ifndef StarSprite_h__
#define StarSprite_h__

#include "cocos2d.h"
#include "Skill/StarSkill.h"
#include "FightStateMachine.h"

USING_NS_CC;
using namespace std;

class StarSprite : public Ref
{

public:

	StarSprite();
	virtual ~StarSprite();

	static StarSprite* create(int id, int level, FightStateMachine::E_SIDE side);

public:

	bool init(int id, int level, FightStateMachine::E_SIDE side);

	//��ȡ���鷽λ
	FightStateMachine::E_SIDE getSide();

	//��ȡ��������˺�ֵ
	int getBasicDamage();

	//��ȡ������ȼ����ӵ��˺�ϵ���ӳ�
	float getAddtionDamageCoe();

	//��ȡ���鹥������
	int getAttackType();

	//��ȡ���鼼��
	StarSkill* getStarSkill();

private:

	//����id
	int mId;

	//����ȼ�
	int mLevel;

	//���鷽λ
	FightStateMachine::E_SIDE mSide;

	//��������(����������������)
	int mAttackType;

	//ÿһ�����ӵ��˺�ϵ��
	float mGrowDamageCoe;

	//�˺��������ֵ
	int mBasicDamage;

	//������������
	StarSkill* mStarSkill;

	//������������Resid
	int mStarSkillResId;
};


#endif //StarSprite_h__