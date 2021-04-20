#include "AILogic.h"
#include "Skill/ActiveSkill.h"
#include <algorithm>


AILogic::AILogic(FightStateMachine::E_SIDE side)
	: mPause(true)
{
	mSide = side;
	mFightModel = FightModel::getInstance();
}

AILogic::~AILogic()
{

}

AILogic* AILogic::create(FightStateMachine::E_SIDE side)
{
	AILogic* aiLogic = new AILogic(side);
	aiLogic->autorelease();
	return aiLogic;
}

//执行自动战斗
void AILogic::start()
{
	mPause = true;
	execute();
}

//暂停自动战斗
void AILogic::pause()
{
	mPause = false;
}

void AILogic::execute()
{
	//暂停状态不执行逻辑
	if (!mPause)
	{
		return;
	}

	vector<ActiveSkill*> vecActiveSkill;

	//统计所有可释放的主动技能
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* fighter = mFightModel->getAliveFighter(mSide, i);
		if (fighter)
		{
			ActiveSkill* activeSkill = fighter->getActiveSkill();
			if (activeSkill && activeSkill->isReady())
			{
				vecActiveSkill.push_back(activeSkill);
			}
		}
	}

	//对技能的权重值进行排序, 降序排列
	sort(vecActiveSkill.begin(), vecActiveSkill.end(), 
		[this](ActiveSkill* skillA, ActiveSkill* skillB)
		{
			//改为按照阵位序号进行排列，序号小的先释放
			int index1 = skillA->getAttacker()->getIndex();
			int index2 = skillB->getAttacker()->getIndex();
			return index1 < index2;
		}
	);

	//释放技能
	for (ActiveSkill* skill : vecActiveSkill)
	{
		if (skill->isReady())
		{
			skill->launchSkill();
		}
	}

	vecActiveSkill.clear();
}