#ifndef __IFIGHT_UI__
#define __IFIGHT_UI__

#include "FightStateMachine.h"
#include "Skill/SkillResultParam.h"
#include "UIFighterDef.h"
#include "FightModel.h"

class IFightUI
{

public:

	//战斗开始回调
	virtual void onFightStart() = 0;

	//回合开始回调
	virtual void onRoundStart(vector<AssistFighterInfo>& vecAssistInfo) = 0;

	//战斗攻击回调
	virtual void onAttack(SkillResultParam* resultParam, SKILL_EFFECT* effectParam) = 0;

	//换边出手
	virtual void onChangeSide(FightStateMachine::E_SIDE currSide, bool isPvp) = 0;

	//战斗回合结束回调
	virtual void onRoundEnd(RoundParam buffRoundHarm) = 0;

	//战斗结束回调
	virtual void onFightEnd(bool isWin, int starCount) = 0;

};


#endif