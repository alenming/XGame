#include "FightStateMachine.h"
#include "IFightUI.h"
#include "FightModel.h"
#include "Skill/AbstractSkill.h"
#include "Skill/SkillResultParam.h"
#include "AILogic.h"
#include "StarSprite.h"


FightStateMachine::FightStateMachine()
	: mRoundCount(0)
	, mTurnCount(0)
	, mMyTurns(INDEX_COUNT)
	, mEnemyTurns(INDEX_COUNT)
	, mMyRushTurns(INDEX_COUNT)
	, mEnemyRushTurns(INDEX_COUNT)
	, mAttackerIndex(-1)
	, mFightUI(nullptr)
	, mCurrentSkill(nullptr)
	, mCurrentState(STATE::WAIT)
	, mPreviousState(STATE::WAIT)
	, mEnemyAILogic(nullptr)
	, mMyAILogic(nullptr)
	, mIsPVP(false)
	, mIsChangeSide(false)
	, mMaxRoundCount(MAX_ROUND_COUNT)
{
	mFightModel = FightModel::getInstance();
	mEnemyAILogic = AILogic::create(E_SIDE::ENEMY_SIDE);
	mEnemyAILogic->retain();
	mMyAILogic = AILogic::create(E_SIDE::MY_SIDE);
	mMyAILogic->pause();
	mMyAILogic->retain();
}

FightStateMachine::~FightStateMachine()
{
	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
	CC_SAFE_RELEASE(mEnemyAILogic);
	CC_SAFE_RELEASE(mMyAILogic);
}

FightStateMachine* FightStateMachine::create()
{
	FightStateMachine *fightStateMachine = new FightStateMachine();
	fightStateMachine->autorelease();
	return fightStateMachine;
}

void FightStateMachine::start()
{
	setCurrentState(STATE::FIGHT_START);
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(FightStateMachine::execute), this, 0, false);
}

void FightStateMachine::wait()
{
	if (mCurrentState != STATE::WAIT)
	{
		setCurrentState(STATE::WAIT);
	}
}

void FightStateMachine::resume()
{
	if (mCurrentState == STATE::WAIT)
	{
		mCurrentState = mPreviousState;
	}
}

void FightStateMachine::setCurrentState(FightStateMachine::STATE state)
{
	mPreviousState = mCurrentState;
	mCurrentState = state;
}

FightStateMachine::STATE FightStateMachine::getCurrentState()
{
	return mCurrentState;
}

void FightStateMachine::setFightUI(IFightUI* fightUI)
{
	this->mFightUI = fightUI;
}

void FightStateMachine::execute(float dt)
{
	fightProcess();
}

void FightStateMachine::fightProcess()
{
	if (mFightUI == nullptr) return;
	
	switch(mCurrentState)
	{
	case STATE::FIGHT_START:
		onFightStart();
		break;
	case STATE::ROUND_BEFORE:
		onRoundBefore();
		break;
	case STATE::ROUND_START:
		onRoundStart();
		break;
	case STATE::TURN_START:
		onTurnStart();
		break;
	case STATE::JUDGE_START:
		onJudgeStart();
		break;
	case STATE::TARGET_SELECT:
		onTargetSelect();
		break;
	case STATE::ATTACK:
		onAttack();
		break;
	case STATE::JUDGE_END:
		onJudgeEnd();
		break;
	case STATE::TURN_END:
		onTurnEnd();
		break;
	case STATE::TURN_CHANGE:
		onTurnChange();
		break;
	case STATE::ROUND_END:
		onRoundEnd();
		break;
	case STATE::FIGHT_END:
		onFightEnd();
		break;
	case STATE::WAIT:
		break;

	default:
		break;
	}
}

void FightStateMachine::onFightStart()
{
	CCLOG("Fight Start!");
	setCurrentState(STATE::ROUND_BEFORE);

	//ս����ʼ�׶η�������
	launchFightStartAttack();

	fightProcess();
}

void FightStateMachine::onRoundBefore()
{
	CCLOG("Round Before!");
	setCurrentState(STATE::ROUND_START);

	mIsChangeSide = false;

	mRoundCount++;
	mFightModel->setRoundCount(mRoundCount);

	mTurnCount = 0;

	//�غϿ�ʼ����ŭ��
	int totalSp = mRoundCount * 20 >= 100 ? 100 : mRoundCount * 20;
	mFightModel->setMyTotalSp(totalSp);
	mFightModel->setEnemyTotalSp(totalSp);

	//�ж��ķ��ȳ���
	mCurrentSide = mFightModel->isMySideFirst() ? E_SIDE::MY_SIDE : E_SIDE::ENEMY_SIDE;

	//��ʼ���ҷ����ּ�¼
	for (int i = 0; i < mMyTurns.size(); i++)
	{
		mMyTurns[i] = false;
	}

	//��ʼ���з����ּ�¼
	for (int i = 0; i < mEnemyTurns.size(); i++)
	{
		mEnemyTurns[i] = false;
	}

	//��ʼ�����غ��ҷ��������ּ�¼
	for (int i = 0; i < mMyRushTurns.size(); i++)
	{
		mMyRushTurns[i] = false;
	}

	//��ʼ���з����غ��������ּ�¼
	for (int i = 0; i < mEnemyRushTurns.size(); i++)
	{
		mEnemyRushTurns[i] = false;
	}

	//�����ս��
	vector<AssistFighterInfo> vecAssistInfo;
	mFightModel->addAssistFighter(mRoundCount, vecAssistInfo);

	//ִ��AI�߼�
	mEnemyAILogic->execute();
	mMyAILogic->execute();

	//֪ͨUI�����غϿ�ʼ�¼�
	if (mFightUI)
	{
		mFightUI->onRoundStart(vecAssistInfo);
	}
	
	fightProcess();
}

void FightStateMachine::onRoundStart()
{
	CCLOG("Round Start!");
	
	if (!(launchRoundAttack() || launchRoundStartAttack()))
	{
		//��δ������ӣ�������һ�׶�
		setCurrentState(STATE::TURN_START);
	}

	fightProcess();
}

void FightStateMachine::onTurnStart()
{
	CCLOG("Turn Start!");
	setCurrentState(STATE::JUDGE_START);

	//������н�ɫ��״̬(���ա����ء�����)
	mFightModel->clearAllFighterState();
	
	mAttackerIndex = -1;
	mCurrentSkill = nullptr;
	bool launchActiveSkill = false;

	for (int i = 0; i < INDEX_COUNT; i++)
	{
		//�����ǰ���ַ��пɷ������������ܣ������ִ�ֱ�ӷ���
		Fighter* fighter = fighter = mFightModel->getAliveFighter(mCurrentSide, i);
		if (fighter)
		{
			ActiveSkill* activeSkill = fighter->getActiveSkill();
			if (activeSkill && activeSkill->isActived() && activeSkill->onJudgeStart())
			{
				mCurrentSkill = activeSkill;
				mAttackerIndex = i;
				launchActiveSkill = true;
				break;
			}
		}
	}
	
	//û�пɷ������������ܣ����ִη�����ͨ����
	if (!launchActiveSkill)
	{
		if (mCurrentSide == E_SIDE::MY_SIDE)
		{
			//�ж��ҷ��ֵ�˭����
			for (int i = 0; i < mMyTurns.size(); i++)
			{
				//���ֹ��Ľ�ɫ���ٳ���
				if (mMyTurns[i])
				{
					continue;
				}
				else
				{
					mMyTurns[i] = true;

					//�жϵ�ǰ��λ�Ƿ���ڽ�ɫ
					if (mFightModel->getAliveFighter(mCurrentSide, i))
					{
						mTurnCount++;
						mAttackerIndex = i;
						break;
					}
					else
					{
						continue;
					}
				}
			}
		}
		else if (mCurrentSide == E_SIDE::ENEMY_SIDE)
		{
			//�жϵз��ֵ�˭����
			for (int i = 0; i < mEnemyTurns.size(); i++)
			{
				//���ֹ��Ľ�ɫ���ٳ���
				if (mEnemyTurns[i])
				{
					continue;
				}
				else
				{
					mEnemyTurns[i] = true;

					//�жϵ�ǰ��λ�Ƿ���ڽ�ɫ
					if (mFightModel->getAliveFighter(mCurrentSide, i))
					{
						mTurnCount++;
						mAttackerIndex = i;
						break;
					}
					else
					{
						continue;
					}
				}
			}
		}

		//������ͨ����
		Fighter* attacker = mFightModel->getAliveFighter(mCurrentSide, mAttackerIndex);
		if (attacker)
		{
			NormalSkill* normalSkill = attacker->getNormalSkill();
			mCurrentSkill = normalSkill;
		}
	}

	fightProcess();
}

void FightStateMachine::onJudgeStart()
{
	CCLOG("Judge Start!");
	setCurrentState(STATE::TARGET_SELECT);

	//����ü��ܲ��ܷ�������ת���ִο�ʼ�׶�
	if (mCurrentSkill && !mCurrentSkill->onJudgeStart())
	{
		setCurrentState(STATE::TURN_START);
	}

	fightProcess();
}

void FightStateMachine::onTargetSelect()
{
	CCLOG("Target Select!");
	setCurrentState(STATE::ATTACK);

	if (mCurrentSkill)
	{
		mCurrentSkill->onTargetSelect();
	}
	
	fightProcess();
}

void FightStateMachine::onAttack()
{
	CCLOG("Attack!");
	setCurrentState(STATE::JUDGE_END);

	if (mCurrentSkill)
	{
		//�ж��Ƿ�ɷ�������3����ǰ��������
		if (!launchBeforeAttack())
		{
			//������ֵ���㣬������ֵ���������ݵ�UI
			mCurrentSkill->onAttack();
			notifyAttack(mCurrentSkill);
		}
	}
	fightProcess();
}

void FightStateMachine::onJudgeEnd()
{
	CCLOG("Judge End");

	//����Ƿ�ɹ��������
	bool isSuccessCombo = false;

	//�ж��������Ƿ����������������Ƿ�ɼ�������
	if (launchDeadAttack())
	{
		isSuccessCombo = true;
	}
	else
	{
		//�ж��Ƿ��з�������1��׷����
		if (launchRushAttack())
		{
			isSuccessCombo = true;
		}
		else
		{
			//�ж��Ƿ����˷�������������
			if (launchHitCountAttack())
			{
				isSuccessCombo = true;
			}
			else
			{
				if (launchStarSpriteAttack())
				{
					isSuccessCombo = true;
				}
			}
		}
	}

	//��δ�����������ֱ�ӽ�����һ�׶�
	if (!isSuccessCombo)
	{
		setCurrentState(STATE::TURN_END);
	}

	fightProcess();
}

void FightStateMachine::onTurnEnd()
{
	CCLOG("Turn End!");

	//������н�ɫ��״̬(���ա����ء�����)
	mFightModel->clearAllFighterState();

	bool isAllMineDead = mFightModel->isAllFighterDead(E_SIDE::MY_SIDE);
	bool isAllEnemyDead = mFightModel->isAllFighterDead(E_SIDE::ENEMY_SIDE);

	if (isAllEnemyDead)
	{
		mWinSide = E_SIDE::MY_SIDE;
	}

	if (isAllMineDead)
	{
		mWinSide = E_SIDE::ENEMY_SIDE;
	}

	//��һ����ɫȫ������,ս������
	if (isAllMineDead || isAllEnemyDead)
	{
		setCurrentState(STATE::FIGHT_END);
	}
	else
	{
		bool allMyTurnsDone = isAllMyTurnsDone();
		bool allEnemyTurnsDone = isAllEnemyTurnsDone();
		bool allMyTurnsUnDone = isAllMyTurnsUnDone();
		bool allEnemyTunrsUnDone = isAllEnemyTurnsUnDone();

		//�ҷ��͵з�ȫ��������ϣ��غϽ���
		if (allMyTurnsDone && allEnemyTurnsDone)
		{
			setCurrentState(STATE::ROUND_END);
		}
		else
		{
			setCurrentState(STATE::TURN_START);

			if (!mIsPVP)	//pve�ִν����߼�
			{
				//�ҷ�ȫ���ж������ҵз�ȫ��δ�ж� ���� �ҷ�ȫ��δ�ж��ҵз�ȫ���ж������� ���������¼�
				if (allMyTurnsDone && allEnemyTunrsUnDone || allEnemyTurnsDone && allMyTurnsUnDone)
				{
					mCurrentSide = (mCurrentSide == E_SIDE::MY_SIDE) ? E_SIDE::ENEMY_SIDE : E_SIDE::MY_SIDE;

					setCurrentState(STATE::TURN_CHANGE);

					//֪ͨUI���߳��֣�ÿ�غ�ֻ֪ͨһ��
					if (!mIsChangeSide)
					{
						mIsChangeSide = true;
						mFightUI->onChangeSide(mCurrentSide, false);
					}
				}
			}
			else //pvp�ִν����߼�
			{
				//��ͨ����������ϻ��߳���
				E_SIDE targetSide = (mCurrentSide == E_SIDE::MY_SIDE) ? E_SIDE::ENEMY_SIDE : E_SIDE::MY_SIDE;
				//����Է�δ������ϣ��򻻱߳���
				if (!isTurnsDone(targetSide))
				{
					mCurrentSide = targetSide;
					//֪ͨUI���߳���
					mFightUI->onChangeSide(mCurrentSide, true);
				}
			}
		}
	}

	fightProcess();
}

void FightStateMachine::onTurnChange()
{
	setCurrentState(STATE::TURN_START);

	launchRoundBeforeEnemyAttack();

	fightProcess();
}

void FightStateMachine::onRoundEnd()
{
	CCLOG("Round End!");

	setCurrentState(STATE::ROUND_BEFORE);

	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* mineFighter = mFightModel->getAliveFighter(E_SIDE::MY_SIDE, i);
		//�غϽ����ۼ����������������ܵ�CD
		if (mineFighter && mineFighter->getActiveSkill())
		{
			mineFighter->getActiveSkill()->decreaseCD();
		}
		//�غϽ������ñ���3�Ѵ������
		if (mineFighter && mineFighter->getTriggerSkill())
		{
			mineFighter->getTriggerSkill()->onRoundEnd();
		}

		Fighter* enemyFighter = mFightModel->getAliveFighter(E_SIDE::ENEMY_SIDE, i);
		//�غϽ����ۼ����������������ܵ�CD
		if (enemyFighter && enemyFighter->getActiveSkill())
		{
			enemyFighter->getActiveSkill()->decreaseCD();
		}
		//�غϽ������ñ���3�Ѵ������
		if (enemyFighter && enemyFighter->getTriggerSkill())
		{
			enemyFighter->getTriggerSkill()->onRoundEnd();
		}
	}

	//���Combo����
	clearHitCount();

	//�غϽ���ͳһ����Buff
	onBuffRoundTick();

	bool isAllMineDead = mFightModel->isAllFighterDead(E_SIDE::MY_SIDE);
	bool isAllEnemyDead = mFightModel->isAllFighterDead(E_SIDE::ENEMY_SIDE);

	if (isAllEnemyDead)
	{
		mWinSide = E_SIDE::MY_SIDE;
	}
	if (isAllMineDead)
	{
		mWinSide = E_SIDE::ENEMY_SIDE;
	}

	//��һ����ɫȫ������,ս������
	if (isAllMineDead || isAllEnemyDead)
	{
		setCurrentState(STATE::FIGHT_END);
	}

	//�غ�������������ޣ�ս������
	if (mRoundCount >= mMaxRoundCount)
	{
		mWinSide = E_SIDE::ENEMY_SIDE;
		setCurrentState(STATE::FIGHT_END);
	}

	fightProcess();
}

void FightStateMachine::onFightEnd()
{
	CCLOG("FightEnd");
	setCurrentState(STATE::WAIT);

	//�ж�ʤ������
	int winCond = FightModel::getInstance()->getWinCond();
	int winCondCount = FightModel::getInstance()->getWinCondCount();
	bool isWin = (mWinSide == E_SIDE::MY_SIDE);
	if (isWin && winCondCount > 0)
	{
		switch (winCond)
		{
		case WIN_AND_ALIVE_N:
			isWin = FightModel::getInstance()->getMyAliveFighterCount() >= winCondCount;
			break;
		case WIN_IN_ROUND_N:
			isWin = mRoundCount <= winCondCount;
			break;
		default:
			break;
		}
	}

	//����Ǽ�Ϊ����
	int starCount = 3;
	if (isWin)
	{
		//���ʤ��������С�Ǽ�Ϊ1
		starCount -=  FightModel::getInstance()->getMyDeadFighterCount();
		starCount = starCount < 1 ? 1 : starCount;
	}
	else
	{
		//���ʧ�ܣ��Ǽ�Ϊ0
		starCount = 0;
	}

	if (mFightUI)
	{
		mFightUI->onFightEnd(isWin, starCount);
	}

	fightProcess();
}

void FightStateMachine::onBuffRoundTick()
{
	
	RoundParam roundParam;

	//����buff�ڻغϽ���ʱ������Ч����������buff�˺�����
	mFightModel->onBuffRoundTick(roundParam);

	//�ۼ����������buff�����غϣ�������buff��������
	mFightModel->decreaseAllBuffRound(roundParam);

	//��Ч�����ݸ�UI
	if (mFightUI)
	{
		mFightUI->onRoundEnd(roundParam);
	}
}

bool FightStateMachine::isTurnsDone(E_SIDE side)
{
	if (side == E_SIDE::MY_SIDE)
	{
		return isAllMyTurnsDone();
	}
	else
	{
		return isAllEnemyTurnsDone();
	}
}

bool FightStateMachine::isAllMyTurnsDone()
{
	for (int i = 0; i < mMyTurns.size(); i++)
	{
		Fighter* fighter = mFightModel->getAliveFighter(E_SIDE::MY_SIDE, i);
		if (!mMyTurns[i] && fighter)
		{
			return false;
		}
	}
	return true;
}

bool FightStateMachine::isAllMyTurnsUnDone()
{
	for (int i = 0; i < mMyTurns.size(); i++)
	{
		Fighter* fighter = mFightModel->getAliveFighter(E_SIDE::MY_SIDE, i);
		if (mMyTurns[i] && fighter)
		{
			return false;
		}
	}
	return true;
}

bool FightStateMachine::isAllEnemyTurnsDone()
{
	for (int i = 0; i < mEnemyTurns.size(); i++)
	{
		Fighter* fighter = mFightModel->getAliveFighter(E_SIDE::ENEMY_SIDE, i);
		if (!mEnemyTurns[i] && fighter)
		{
			return false;
		}
	}
	return true;
}

bool FightStateMachine::isAllEnemyTurnsUnDone()
{
	for (int i = 0; i < mEnemyTurns.size(); i++)
	{
		Fighter* fighter = mFightModel->getAliveFighter(E_SIDE::ENEMY_SIDE, i);
		if (mEnemyTurns[i] && fighter)
		{
			return false;
		}
	}
	return true;
}

bool FightStateMachine::launchedRushTurn(E_SIDE side, int index)
{
	bool ret = false;
	if (side == E_SIDE::MY_SIDE)
	{
		ret = mMyRushTurns[index];
		//mMyRushTurns[index] = true;	
	}
	else
	{
		ret = mEnemyRushTurns[index];
		//mEnemyRushTurns[index] = true;
	}
	return ret;
}

void FightStateMachine::setRushTurn(E_SIDE side, int index)
{
	if (side == E_SIDE::MY_SIDE)
	{
		mMyRushTurns[index] = true;	
	}
	else
	{
		mEnemyRushTurns[index] = true;	
	}
}

bool FightStateMachine::launchBeforeAttack()
{
	//�жϱ���3�Ƿ񷢶�
	if (mCurrentSkill)
	{
		int attackerIndex = mCurrentSkill->getSkillResultParam()->mAttackerIndex;
		E_SIDE attackSide = mCurrentSkill->getSkillResultParam()->mAttackSide;
		Fighter* attacker = mFightModel->getFighter(attackSide, attackerIndex);

		if (attacker)
		{
			TriggerSkill* triggerSkill = attacker->getTriggerSkill();
			if (triggerSkill && triggerSkill->triggerBeforeAttack(mCurrentSkill))
			{
				setCurrentState(STATE::JUDGE_START);
				notifyAttack(triggerSkill);
				return true;
			}
		}
	}
	return false;
}

bool FightStateMachine::launchDeadAttack()
{
	if (mCurrentSkill)
	{
		int attackerIndex = mCurrentSkill->getSkillResultParam()->mAttackerIndex;
		E_SIDE attackSide = mCurrentSkill->getSkillResultParam()->mAttackSide;
		Fighter* attacker = mFightModel->getFighter(attackSide, attackerIndex);

		if (attacker)
		{
			TriggerSkill* triggerSkill = attacker->getTriggerSkill();
			if (triggerSkill && triggerSkill->triggerTargetDead(mCurrentSkill))
			{
				setCurrentState(STATE::JUDGE_START);
				setCurrentSkill(triggerSkill);
				return true;
			}
		}
	}
	return false;
}

bool FightStateMachine::launchRushAttack()
{
	//�ж��з��Ƿ��пɱ�׷����
	Fighter* target = nullptr;
	E_SIDE targetSide = (mCurrentSide == E_SIDE::MY_SIDE) ? E_SIDE::ENEMY_SIDE : E_SIDE::MY_SIDE;

	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* tmpFighter = mFightModel->getAliveFighter(targetSide, i);
		if (tmpFighter != nullptr)
		{
			//���ҷ��Ϸ�������1��ǰ��״̬��Ŀ��
			if (tmpFighter->getState() == AbstractSkill::RESULT_STATE::BACK
				|| tmpFighter->getState() == AbstractSkill::RESULT_STATE::FELL
				|| tmpFighter->getState() == AbstractSkill::RESULT_STATE::FLOAT)
			{
				target = tmpFighter;
				break;
			}
		}
	}

	if (target != nullptr)
	{
		//�ж������߱��ִ��Ƿ񷢶�������
		if (!launchedRushTurn(mCurrentSide, mAttackerIndex))
		{
			//�ж������ߵı�������1�Ƿ�ɷ�������
			Fighter* attacker = mFightModel->getAliveFighter(mCurrentSide, mAttackerIndex);
			if (attacker != nullptr && attacker->getRushSkill() &&
				attacker->getRushSkill()->getPreStateType() == target->getState())
			{
				setRushTurn(mCurrentSide, mAttackerIndex);
				mCurrentSkill = attacker->getRushSkill();
				setCurrentState(STATE::JUDGE_START);
				return true;
			}
		}

		for (int i = 0; i < INDEX_COUNT; i++)
		{
			if (!launchedRushTurn(mCurrentSide, i))
			{
				Fighter* attacker = mFightModel->getAliveFighter(mCurrentSide, i);
				if (attacker != nullptr && attacker->getRushSkill() && 
					attacker->getRushSkill()->getPreStateType() == target->getState())
				{
					//�����߷����仯
					mAttackerIndex = i;
					setRushTurn(mCurrentSide, mAttackerIndex);
					mCurrentSkill = attacker->getRushSkill();
					setCurrentState(STATE::JUDGE_START);
					return true;
				}
			}
		}
	}
	return false;
}

bool FightStateMachine::launchFightStartAttack()
{
	auto func = [this](E_SIDE side)->bool
	{
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			Fighter* fighter = mFightModel->getAliveFighter(side, i);
			if (fighter)
			{
				TriggerSkill* triggerSkill = fighter->getTriggerSkill();
				if (triggerSkill && fighter->getTriggerSkill()->triggerFightStart())
				{
					setCurrentState(STATE::FIGHT_START);
					notifyAttack(triggerSkill);
					return true;
				}
			}
		}
		return false;
	};

	if (mFightModel->isMySideFirst())
	{
		if (func(E_SIDE::MY_SIDE) || func(E_SIDE::ENEMY_SIDE))
		{
			return true;
		}
	}
	else
	{
		if (func(E_SIDE::ENEMY_SIDE) || func(E_SIDE::MY_SIDE))
		{
			return true;
		}
	}
	return false;
}

bool FightStateMachine::launchRoundStartAttack()
{
	auto func = [this](E_SIDE side)->bool
	{
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			Fighter* fighter = mFightModel->getAliveFighter(side, i);
			if (fighter)
			{
				TriggerSkill* triggerSkill = fighter->getTriggerSkill();
				if (triggerSkill && fighter->getTriggerSkill()->triggerRoundStart())
				{
					setCurrentState(STATE::ROUND_START);
					notifyAttack(triggerSkill);
					return true;
				}
			}
		}
		return false;
	};

	if (mFightModel->isMySideFirst())
	{
		if (func(E_SIDE::MY_SIDE) || func(E_SIDE::ENEMY_SIDE))
		{
			return true;
		}
	}
	else
	{
		if (func(E_SIDE::ENEMY_SIDE) || func(E_SIDE::MY_SIDE))
		{
			return true;
		}
	}
	return false;
}

bool FightStateMachine::launchRoundAttack()
{
	auto func = [this](E_SIDE side)->bool
	{
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			Fighter* fighter = mFightModel->getAliveFighter(side, i);
			if (fighter)
			{
				TriggerSkill* triggerSkill = fighter->getTriggerSkill();
				if (triggerSkill && fighter->getTriggerSkill()->triggerRound())
				{
					setCurrentState(STATE::ROUND_START);
					notifyAttack(triggerSkill);
					return true;
				}
			}
		}
		return false;
	};

	if (mFightModel->isMySideFirst())
	{
		if (func(E_SIDE::MY_SIDE) || func(E_SIDE::ENEMY_SIDE))
		{
			return true;
		}
	}
	else
	{
		if (func(E_SIDE::ENEMY_SIDE) || func(E_SIDE::MY_SIDE))
		{
			return true;
		}
	}
	return false;
}

bool FightStateMachine::launchRoundBeforeEnemyAttack()
{
	auto func = [this](E_SIDE side)->bool
	{
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			Fighter* fighter = mFightModel->getAliveFighter(side, i);
			if (fighter)
			{
				TriggerSkill* triggerSkill = fighter->getTriggerSkill();
				if (triggerSkill && fighter->getTriggerSkill()->triggerRoundBeforeEnemy())
				{
					setCurrentState(STATE::JUDGE_START);
					setCurrentSkill(triggerSkill);
					return true;
				}
			}
		}
		return false;
	};

	if (mFightModel->isMySideFirst())
	{
		if (func(E_SIDE::MY_SIDE) || func(E_SIDE::ENEMY_SIDE))
		{
			return true;
		}
	}
	else
	{
		if (func(E_SIDE::ENEMY_SIDE) || func(E_SIDE::MY_SIDE))
		{
			return true;
		}
	}
	return false;
}

bool FightStateMachine::launchHitCountAttack()
{
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* fighter = mFightModel->getAliveFighter(mCurrentSide, i);
		if (fighter)
		{
			TriggerSkill* triggerSkill = fighter->getTriggerSkill();
			if (triggerSkill && fighter->getTriggerSkill()->triggerHitCount())
			{
				setCurrentState(STATE::JUDGE_START);
				setCurrentSkill(triggerSkill);
				return true;
			}
		}
	}
	return false;
}

bool FightStateMachine::launchStarSpriteAttack()
{
	StarSprite* starSprite = FightModel::getInstance()->getStarSprite(mCurrentSide);
	if (starSprite)
	{
		StarSkill* starSkill = starSprite->getStarSkill();
		if (starSkill && starSkill->isReady())
		{
			setCurrentState(STATE::JUDGE_START);
			setCurrentSkill(starSkill);
			return true;
		}
	}
	return false;
}

//״̬���ȴ�
void FightStateMachine::timerWait(float dt)
{
	wait();
	//������ʱ����1s����״̬��
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(FightStateMachine::timerResume), this, 0, 0, dt, false);
}

void FightStateMachine::timerResume(float dt)
{
	resume();
	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(FightStateMachine::timerResume), this);
}

AbstractSkill* FightStateMachine::getCurrentSkill()
{
	return mCurrentSkill;
}

void FightStateMachine::setCurrentSkill(AbstractSkill* skill)
{
	mCurrentSkill = skill;
}

void FightStateMachine::addHitCount(SkillResultParam* resultParam)
{
	//ͳ��������
	int hitCount = 0;

	//���鼼�ܲ�����combo��
	if (resultParam->mSkillType != AbstractSkill::SKILL_TYPE::STAR_SKILL)
	{
		for (HarmParam harmParam : resultParam->mVecTargetHarm)
		{
			//����˺����ܼ�����������
			if (harmParam.mHarmType == HarmParam::HARM_TYPE::HARM_CRITICAL || 
				harmParam.mHarmType == HarmParam::HARM_TYPE::HARM_NORMAL ||
				harmParam.mHarmType == HarmParam::HARM_TYPE::HARM_RESTRICTION)
			{
				hitCount += harmParam.mComboCount;
			}
		}
	}

	//���������� mark:�˴����������bug..
	if (resultParam->mAttackSide == E_SIDE::MY_SIDE)
	{
		mFightModel->addMyHitCount(hitCount);
	}
	else
	{
		mFightModel->addEnemyHitCount(hitCount);
	}

	//��ͨ������������ŭ��
	if (resultParam->mSkillType == AbstractSkill::SKILL_TYPE::NORMAL_SKILL)
	{
		StarSprite* starSprite = mFightModel->getStarSprite(resultParam->mAttackSide);
		if (starSprite)
		{
			starSprite->getStarSkill()->addSpriteAG();
		}
	}
}

void FightStateMachine::clearHitCount()
{
	mFightModel->clearMyHitCount();
	mFightModel->clearEnemyHitCount();
}

void FightStateMachine::notifyAttack(AbstractSkill* skill)
{
	addHitCount(skill->getSkillResultParam());
	if (mFightUI)
	{
		mFightUI->onAttack(skill->getSkillResultParam(), skill->getSkillEffectParam());
	}
	
}

void FightStateMachine::setAutoFight(bool autoFight)
{
	if (autoFight)
	{
		mMyAILogic->start();
	}
	else
	{
		mMyAILogic->pause();
	}
}

void FightStateMachine::setPVP(bool isPVP)
{
	mIsPVP = isPVP;
}

void FightStateMachine::setMaxRoundCount(int maxRound)
{
	mMaxRoundCount = maxRound;
}