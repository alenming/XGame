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

	//创建AI逻辑类
	static AILogic* create(FightStateMachine::E_SIDE side);

public:

	//执行自动战斗
	void start();

	//暂停自动战斗
	void pause();

	//执行逻辑
	void execute();

private:

	//哪方的AI
	FightStateMachine::E_SIDE mSide;

	//战斗角色数据模型
	FightModel* mFightModel;

	//是否停止
	bool mPause;

};

#endif //AILogic