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

//ִ���Զ�ս��
void AILogic::start()
{
	mPause = true;
	execute();
}

//��ͣ�Զ�ս��
void AILogic::pause()
{
	mPause = false;
}

void AILogic::execute()
{
	//��ͣ״̬��ִ���߼�
	if (!mPause)
	{
		return;
	}

	vector<ActiveSkill*> vecActiveSkill;

	//ͳ�����п��ͷŵ���������
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

	//�Լ��ܵ�Ȩ��ֵ��������, ��������
	sort(vecActiveSkill.begin(), vecActiveSkill.end(), 
		[this](ActiveSkill* skillA, ActiveSkill* skillB)
		{
			//��Ϊ������λ��Ž������У����С�����ͷ�
			int index1 = skillA->getAttacker()->getIndex();
			int index2 = skillB->getAttacker()->getIndex();
			return index1 < index2;
		}
	);

	//�ͷż���
	for (ActiveSkill* skill : vecActiveSkill)
	{
		if (skill->isReady())
		{
			skill->launchSkill();
		}
	}

	vecActiveSkill.clear();
}