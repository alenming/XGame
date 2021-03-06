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

	//????????????????????
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

	//????????????????
	int totalSp = mRoundCount * 20 >= 100 ? 100 : mRoundCount * 20;
	mFightModel->setMyTotalSp(totalSp);
	mFightModel->setEnemyTotalSp(totalSp);

	//??????????????
	mCurrentSide = mFightModel->isMySideFirst() ? E_SIDE::MY_SIDE : E_SIDE::ENEMY_SIDE;

	//??????????????????
	for (int i = 0; i < mMyTurns.size(); i++)
	{
		mMyTurns[i] = false;
	}

	//??????????????????
	for (int i = 0; i < mEnemyTurns.size(); i++)
	{
		mEnemyTurns[i] = false;
	}

	//????????????????????????????
	for (int i = 0; i < mMyRushTurns.size(); i++)
	{
		mMyRushTurns[i] = false;
	}

	//????????????????????????????
	for (int i = 0; i < mEnemyRushTurns.size(); i++)
	{
		mEnemyRushTurns[i] = false;
	}

	//??????????
	vector<AssistFighterInfo> vecAssistInfo;
	mFightModel->addAssistFighter(mRoundCount, vecAssistInfo);

	//????AI????
	mEnemyAILogic->execute();
	mMyAILogic->execute();

	//????UI????????????????
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
		//??????????????????????????
		setCurrentState(STATE::TURN_START);
	}

	fightProcess();
}

void FightStateMachine::onTurnStart()
{
	CCLOG("Turn Start!");
	setCurrentState(STATE::JUDGE_START);

	//??????????????????(????????????????)
	mFightModel->clearAllFighterState();
	
	mAttackerIndex = -1;
	mCurrentSkill = nullptr;
	bool launchActiveSkill = false;

	for (int i = 0; i < INDEX_COUNT; i++)
	{
		//??????????????????????????????????????????????????
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
	
	//??????????????????????????????????????????
	if (!launchActiveSkill)
	{
		if (mCurrentSide == E_SIDE::MY_SIDE)
		{
			//??????????????????
			for (int i = 0; i < mMyTurns.size(); i++)
			{
				//????????????????????
				if (mMyTurns[i])
				{
					continue;
				}
				else
				{
					mMyTurns[i] = true;

					//????????????????????????
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
			//??????????????????
			for (int i = 0; i < mEnemyTurns.size(); i++)
			{
				//????????????????????
				if (mEnemyTurns[i])
				{
					continue;
				}
				else
				{
					mEnemyTurns[i] = true;

					//????????????????????????
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

		//????????????
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

	//??????????????????????????????????????
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
		//??????????????????3??????????????
		if (!launchBeforeAttack())
		{
			//????????????????????????????????????UI
			mCurrentSkill->onAttack();
			notifyAttack(mCurrentSkill);
		}
	}
	fightProcess();
}

void FightStateMachine::onJudgeEnd()
{
	CCLOG("Judge End");

	//????????????????????
	bool isSuccessCombo = false;

	//??????????????????????????????????????????
	if (launchDeadAttack())
	{
		isSuccessCombo = true;
	}
	else
	{
		//??????????????????1????????
		if (launchRushAttack())
		{
			isSuccessCombo = true;
		}
		else
		{
			//??????????????????????????
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

	//????????????????????????????????
	if (!isSuccessCombo)
	{
		setCurrentState(STATE::TURN_END);
	}

	fightProcess();
}

void FightStateMachine::onTurnEnd()
{
	CCLOG("Turn End!");

	//??????????????????(????????????????)
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

	//??????????????????,????????
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

		//????????????????????????????????
		if (allMyTurnsDone && allEnemyTurnsDone)
		{
			setCurrentState(STATE::ROUND_END);
		}
		else
		{
			setCurrentState(STATE::TURN_START);

			if (!mIsPVP)	//pve????????????
			{
				//???????????????????????????????? ???? ?????????????????????????????????? ????????????
				if (allMyTurnsDone && allEnemyTunrsUnDone || allEnemyTurnsDone && allMyTurnsUnDone)
				{
					mCurrentSide = (mCurrentSide == E_SIDE::MY_SIDE) ? E_SIDE::ENEMY_SIDE : E_SIDE::MY_SIDE;

					setCurrentState(STATE::TURN_CHANGE);

					//????UI??????????????????????????
					if (!mIsChangeSide)
					{
						mIsChangeSide = true;
						mFightUI->onChangeSide(mCurrentSide, false);
					}
				}
			}
			else //pvp????????????
			{
				//????????????????????????
				E_SIDE targetSide = (mCurrentSide == E_SIDE::MY_SIDE) ? E_SIDE::ENEMY_SIDE : E_SIDE::MY_SIDE;
				//??????????????????????????????
				if (!isTurnsDone(targetSide))
				{
					mCurrentSide = targetSide;
					//????UI????????
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
		//??????????????????????????????CD
		if (mineFighter && mineFighter->getActiveSkill())
		{
			mineFighter->getActiveSkill()->decreaseCD();
		}
		//????????????????3??????????
		if (mineFighter && mineFighter->getTriggerSkill())
		{
			mineFighter->getTriggerSkill()->onRoundEnd();
		}

		Fighter* enemyFighter = mFightModel->getAliveFighter(E_SIDE::ENEMY_SIDE, i);
		//??????????????????????????????CD
		if (enemyFighter && enemyFighter->getActiveSkill())
		{
			enemyFighter->getActiveSkill()->decreaseCD();
		}
		//????????????????3??????????
		if (enemyFighter && enemyFighter->getTriggerSkill())
		{
			enemyFighter->getTriggerSkill()->onRoundEnd();
		}
	}

	//????Combo????
	clearHitCount();

	//????????????????Buff
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

	//??????????????????,????????
	if (isAllMineDead || isAllEnemyDead)
	{
		setCurrentState(STATE::FIGHT_END);
	}

	//????????????????????????????
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

	//????????????
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

	//??????????????
	int starCount = 3;
	if (isWin)
	{
		//??????????????????????1
		starCount -=  FightModel::getInstance()->getMyDeadFighterCount();
		starCount = starCount < 1 ? 1 : starCount;
	}
	else
	{
		//????????????????0
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

	//????buff??????????????????????????????buff????????
	mFightModel->onBuffRoundTick(roundParam);

	//??????????????buff????????????????buff????????
	mFightModel->decreaseAllBuffRound(roundParam);

	//????????????UI
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
	//????????3????????
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
	//????????????????????????
	Fighter* target = nullptr;
	E_SIDE targetSide = (mCurrentSide == E_SIDE::MY_SIDE) ? E_SIDE::ENEMY_SIDE : E_SIDE::MY_SIDE;

	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* tmpFighter = mFightModel->getAliveFighter(targetSide, i);
		if (tmpFighter != nullptr)
		{
			//????????????????1????????????????
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
		//??????????????????????????????
		if (!launchedRushTurn(mCurrentSide, mAttackerIndex))
		{
			//????????????????????1??????????????
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
					//??????????????
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

//??????????
void FightStateMachine::timerWait(float dt)
{
	wait();
	//????????????1s????????????
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
	//??????????
	int hitCount = 0;

	//??????????????combo??
	if (resultParam->mSkillType != AbstractSkill::SKILL_TYPE::STAR_SKILL)
	{
		for (HarmParam harmParam : resultParam->mVecTargetHarm)
		{
			//????????????????????????
			if (harmParam.mHarmType == HarmParam::HARM_TYPE::HARM_CRITICAL || 
				harmParam.mHarmType == HarmParam::HARM_TYPE::HARM_NORMAL ||
				harmParam.mHarmType == HarmParam::HARM_TYPE::HARM_RESTRICTION)
			{
				hitCount += harmParam.mComboCount;
			}
		}
	}

	//?????????? mark:??????????????bug..
	if (resultParam->mAttackSide == E_SIDE::MY_SIDE)
	{
		mFightModel->addMyHitCount(hitCount);
	}
	else
	{
		mFightModel->addEnemyHitCount(hitCount);
	}

	//????????????????????
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