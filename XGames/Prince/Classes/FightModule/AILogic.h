#ifndef AILogic_h__
#define AILogic_h__

#include "cocos2d.h"
#include "FightModel.h"
#include "Skill/ActiveSkill.h"

USING_NS_CC;

class FightStateMachine;

class AILogic : public Ref 
{

public:

	AILogic(FightStateMachine::E_SIDE side);
	virtual ~AILogic();

	//����AI�߼���
	static AILogic* create(FightStateMachine::E_SIDE side);

public:

	//ִ���Զ�ս��
	void start();

	//��ͣ�Զ�ս��
	void pause();

	//ִ���߼�
	void execute();

private:

	//�ķ���AI
	FightStateMachine::E_SIDE mSide;

	//ս����ɫ����ģ��
	FightModel* mFightModel;

	//�Ƿ�ֹͣ
	bool mPause;

};

#endif //AILogic