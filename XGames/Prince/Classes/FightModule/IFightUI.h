#ifndef __IFIGHT_UI__
#define __IFIGHT_UI__

#include "FightStateMachine.h"
#include "Skill/SkillResultParam.h"
#include "UIFighterDef.h"
#include "FightModel.h"

class IFightUI
{

public:

	//ս����ʼ�ص�
	virtual void onFightStart() = 0;

	//�غϿ�ʼ�ص�
	virtual void onRoundStart(vector<AssistFighterInfo>& vecAssistInfo) = 0;

	//ս�������ص�
	virtual void onAttack(SkillResultParam* resultParam, SKILL_EFFECT* effectParam) = 0;

	//���߳���
	virtual void onChangeSide(FightStateMachine::E_SIDE currSide, bool isPvp) = 0;

	//ս���غϽ����ص�
	virtual void onRoundEnd(RoundParam buffRoundHarm) = 0;

	//ս�������ص�
	virtual void onFightEnd(bool isWin, int starCount) = 0;

};


#endif