#include "FightModel.h"
#include <algorithm>
#include "StarSprite.h"
#include "cocos2d.h"
#include "Utils/StringFormat.h"

using namespace std;

#define nMAX_SP 100

FightModel*	FightModel::ms_pInstance	= nullptr;

FightModel::FightModel()
	: m_bIsUpdateDone(false)
	, mMyTotalSp(0)
	, mEnemyTotalSp(0)
	, mMyHitCount(0)
	, mEnemyHitCount(0)
	, mRoundCount(0)
	, mSkipFlag(SKIP_FLAG::CANNOT_SKIP)
	, mMyStarSprite(nullptr)
	, mEnemyStarSprite(nullptr)
	, mWinCond(DEFEAT_ALL_ENEMY)
	, mWinCondCount(-1)
	, mBgResId(-1)
	, mChangeBgResId(-1)
	, mChangeBgCond(-1)
	, mChangeBgRound(0)
	, mAssistSide(FightStateMachine::E_SIDE::MY_SIDE)
	, mAssistEnterRound(-1)
	, mMyFirstValue(0)
	, mEnemyFirstValue(0)
{
	clearMyHitCount();
	clearEnemyHitCount();
}

 FightModel::~FightModel()
 {
	 mVecHero.clear();
	 mVecMonster.clear();

	 CC_SAFE_RELEASE(mMyStarSprite);
	 CC_SAFE_RELEASE(mEnemyStarSprite);
 }

FightModel* FightModel::getInstance()
{
	if (!ms_pInstance)
	{
		ms_pInstance = new FightModel();
	}
	return ms_pInstance;
}

void FightModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void FightModel::clearHero()
{
	mVecHero.clear();
}

void FightModel::addHero(Fighter* hero)
{
	mVecHero.pushBack(hero);
}

void FightModel::clearMonster()
{
	mVecMonster.clear();
}

void FightModel::clearAssist()
{
	mVecAssistFighter.clear();
}

void FightModel::addMonster(Fighter* monster)
{
	mVecMonster.pushBack(monster);
}

void FightModel::addAssist(Fighter* fighter)
{
	mVecAssistFighter.pushBack(fighter);
}

Vector<Fighter*>* FightModel::getAssistList()
{
	return &mVecAssistFighter;
}

Fighter* FightModel::getHeroByIndex( int n )
{
	for (auto fighter : mVecHero)
	{
		if (fighter->getIndex() == n)
		{
			return fighter;
		}
	}

	return nullptr;
}

Fighter* FightModel::getMonsterByIndex(int n)
{
	for (auto fighter : mVecMonster)
	{
		if (fighter->getIndex() == n)
		{
			return fighter;
		}
	}
	return nullptr;
}

Vector<Fighter*>* FightModel::getHeroList()
{
	return &mVecHero;
}

Vector<Fighter*>* FightModel::getMonsterList()
{
	return &mVecMonster;
}

Fighter* FightModel::getFighter(FightStateMachine::E_SIDE side, int nIndex)
{
	if (side == FightStateMachine::E_SIDE::MY_SIDE)
	{
		return getHeroByIndex(nIndex);
	}
	else
	{
		return getMonsterByIndex(nIndex);
	}
}

Fighter* FightModel::getAssistFighter(FightStateMachine::E_SIDE side, int nIndex)
{
	for (auto fighter : mVecAssistFighter)
	{
		if (fighter->getIndex() == nIndex && fighter->getSide() == side)
		{
			return fighter;
		}
	}
	return nullptr;
}

Fighter* FightModel::getAliveFighter(FightStateMachine::E_SIDE side, int nIndex)
{
	Fighter* fighter = nullptr;

	if (side == FightStateMachine::E_SIDE::MY_SIDE)
	{
		fighter = getHeroByIndex(nIndex);
	}
	else
	{
		fighter = getMonsterByIndex(nIndex);
	}

	if (fighter == nullptr || !fighter->isAlive())
	{
		return nullptr;
	}
	else
	{
		return fighter;
	}
}

void FightModel::getAllAliveFighter(FightStateMachine::E_SIDE side, vector<Fighter*>& vecAliveFighter)
{
	if (side == FightStateMachine::E_SIDE::MY_SIDE)
	{
		for (auto fighter : mVecHero)
		{
			if (fighter && fighter->isAlive())
			{
				vecAliveFighter.push_back(fighter);
			}
		}
	}
	else
	{
		for (auto fighter : mVecMonster)
		{
			if (fighter && fighter->isAlive())
			{
				vecAliveFighter.push_back(fighter);
			}
		}
	}
}

int FightModel::getMyAliveFighterCount()
{
	int count = 0;
	for (auto fighter : mVecHero)
	{
		if (fighter->isAlive())
		{
			count++;
		}
	}
	return count;
}

int FightModel::getMyDeadFighterCount()
{
	int count = 0;
	for (auto fighter : mVecHero)
	{
		if (!fighter->isAlive())
		{
			count++;
		}
	}
	return count;
}

void FightModel::clearAllFighterState()
{
	for (auto fighter : mVecHero)
	{
		fighter->clearState();
	}

	for (auto fighter : mVecMonster)
	{
		fighter->clearState();
	}
}

bool FightModel::isAllFighterDead(FightStateMachine::E_SIDE side)
{
	bool ret = true;

	if (side == FightStateMachine::E_SIDE::MY_SIDE)
	{
		for (auto fighter : mVecHero)
		{
			if (fighter->isAlive())
			{
				ret = false;
				break;
			}
		}
	}
	else if(side == FightStateMachine::E_SIDE::ENEMY_SIDE)
	{
		for (auto fighter : mVecMonster)
		{
			if (fighter->isAlive())
			{
				ret = false;
				break;
			}
		}
	}
	return ret;
}

bool FightModel::isAllFighterAlive(FightStateMachine::E_SIDE side)
{
	bool ret = true;

	if (side == FightStateMachine::E_SIDE::MY_SIDE)
	{
		for (auto fighter : mVecHero)
		{
			if (!fighter->isAlive())
			{
				ret = false;
				break;
			}
		}
	}
	else if(side == FightStateMachine::E_SIDE::ENEMY_SIDE)
	{
		for (auto fighter : mVecMonster)
		{
			if (!fighter->isAlive())
			{
				ret = false;
				break;
			}
		}
	}
	return ret;
}


long long FightModel::getTotalHp( FightStateMachine::E_SIDE side )
{
	long long nSum = 0;

	if (FightStateMachine::E_SIDE::MY_SIDE == side)
	{
		for (auto fighter : mVecHero)
		{
			nSum += fighter->getCurrHP();
		}
	}
	else
	{
		for (auto fighter : mVecMonster)
		{
			nSum += fighter->getCurrHP();
		}
	}

	return nSum;
}


long long FightModel::getTotalMaxHp( FightStateMachine::E_SIDE side )
{
	long long totalMaxHp = 0;
	if (FightStateMachine::E_SIDE::MY_SIDE == side)
	{
		for (auto fighter : mVecHero)
		{
			totalMaxHp += fighter->getMaxHP();
		}
	}
	else
	{
		for (auto fighter : mVecMonster)
		{
			totalMaxHp += fighter->getMaxHP();
		}
	}
	return totalMaxHp;
}


int FightModel::getTotalMaxSp()
{
	return nMAX_SP;
}

void FightModel::decreaseAllBuffRound(RoundParam& roundParam)
{
	for (auto fighter : mVecHero)
	{
		fighter->decreaseBuffRound(roundParam.mMapMyBuffUpdateParam);
	}

	for (auto fighter : mVecMonster)
	{
		fighter->decreaseBuffRound(roundParam.mMapEnemyBuffUpdateParam);
	}
}

//����ҷ�������
void FightModel::clearMyHitCount()
{
	setMyHitCount(0);
}

//�����ҷ�������
void FightModel::addMyHitCount(int hitCount)
{
	int myHitCount = getMyHitCount();
	myHitCount += hitCount;
	setMyHitCount(myHitCount);
}

//��յз�������
void FightModel::clearEnemyHitCount()
{
	setEnemyHitCount(0);
}

//���ӵз�������
void FightModel::addEnemyHitCount(int hitCount)
{
	int enemyHitCount = getEnemyHitCount();
	enemyHitCount += hitCount;
	setEnemyHitCount(enemyHitCount);
}

void FightModel::setSkipFlag(int skipFlag)
{
	mSkipFlag = skipFlag;
}

bool FightModel::canSkip()
{
	return mSkipFlag == SKIP_FLAG::CAN_SKIP;
}

void FightModel::setWinCondAndCount(int winCond, int condCount)
{
	setWinCond(winCond);
	setWinCondCount(condCount);
}

//����ս��������Դid
void FightModel::setBgResId(int resId)
{
	mBgResId = resId;
}

//��ȡս��������Դid
int FightModel::getBgResId()
{
	return mBgResId;
}

int FightModel::getTotalDamage()
{
	int totalDamage = 0;
	for (auto fighter : mVecMonster)
	{
		if (fighter)
		{
			totalDamage += (fighter->getInitialHp() - fighter->getCurrHP());
		}
	}
	return totalDamage;
}

int FightModel::getBossDamage()
{
	int totalDamage = 0;
	Fighter* fighter = getFighter(FightStateMachine::ENEMY_SIDE, 4);
	if (fighter)
	{
		totalDamage += (fighter->getInitialHp() - fighter->getCurrHP());
	}
	return totalDamage;
}

float FightModel::getCompletionPercent()
{
	float fPercent = 0.0;
	int totalDamage = 0;
	int totalHp = 0;
	for (auto fighter : mVecMonster)
	{
		if (fighter)
		{
			totalHp += fighter->getInitialHp();
			totalDamage += (fighter->getInitialHp() - fighter->getCurrHP());
		}
	}
	fPercent = (((float)totalDamage)/((float)totalHp))*100.0f;
	fPercent = min(fPercent, 100.0f);
	fPercent = max(0.0f, fPercent);
	return fPercent;
}

void FightModel::addAssistFighter(int round, vector<AssistFighterInfo>& vecAssistInfo)
{
	int enterRound = getAssistEnterRound();
	int assistSide = getAssistSide();

	if (round == enterRound && enterRound > 0 && assistSide >= 0)
	{
		for (auto assist : mVecAssistFighter)
		{
			//�˴����ݴ���ֹ������ͬ��λ�Ľ�ɫ
			if (assist && !getFighter((FightStateMachine::E_SIDE)assistSide, assist->getIndex()))
			{
				if (assistSide == FightStateMachine::E_SIDE::MY_SIDE)
				{
					addHero(assist);
				}
				else
				{
					addMonster(assist);
				}
				
				AssistFighterInfo fighterInfo;
				fighterInfo.side = assist->getSide();
				fighterInfo.index = assist->getIndex();
				vecAssistInfo.push_back(fighterInfo);
			}
		}
		clearAssist();
	}
}

StarSprite* FightModel::getStarSprite(FightStateMachine::E_SIDE side)
{
	if (side == FightStateMachine::E_SIDE::MY_SIDE)
	{
		return mMyStarSprite;
	}
	else
	{
		return mEnemyStarSprite;
	}
}

void FightModel::setStarSprite(StarSprite* starSprite, FightStateMachine::E_SIDE side)
{
	if (side == FightStateMachine::E_SIDE::MY_SIDE)
	{
		mMyStarSprite = starSprite;
		mMyStarSprite->retain();
	}
	else
	{
		mEnemyStarSprite = starSprite;
		mEnemyStarSprite->retain();
	}
}

//buffÿ�غϵ�ִ��Ч��
void FightModel::onBuffRoundTick(RoundParam& roundParam)
{
	//�ڲ�����
	auto buffRoundTick = [this](FightStateMachine::E_SIDE side, map<int, vector<HarmParam> >& mapHarm)
	{
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			Fighter* fighter = getAliveFighter(side, i);
			if (!fighter) continue;

			Vector<AbstractBuff*>* vecBuff = fighter->getVecBuff();
			if (vecBuff->empty()) continue;

			//ÿ�������ϵ������˺�/����Ч��
			vector<HarmParam> vecHarm;

			//�ж��Ƿ�����Ѫbuff
			bool hasBleedBuff = false;
			for (int j = 0; j < vecBuff->size(); j++)
			{
				AbstractBuff* buff = vecBuff->at(j);
				if (buff->getBuffType() == AbstractBuff::BUFF_TYPE::BLEED)
				{
					hasBleedBuff = true;
					break;
				}
			}

			//�������Ѫbuff��������Ч����Ч
			if (!hasBleedBuff)
			{
				for (int j = 0; j < vecBuff->size(); j++)
				{
					AbstractBuff* buff = vecBuff->at(j);

					if (buff->getBuffType() == AbstractBuff::BUFF_TYPE::REGON)
					{
						HarmParam harmParam;
						harmParam.mTargetIndex = i;
						harmParam.mTargetSide = side;
						harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_CURE;
						harmParam.mComboCount = 0;
						harmParam.mValue = buff->doRoundTick();
						fighter->onHarm(harmParam.mValue);
						vecHarm.push_back(harmParam);
					}
				}
			}

			//��Ѫ
			for (int j = 0; j < vecBuff->size(); j++)
			{
				AbstractBuff* buff = vecBuff->at(j);

				if (buff->getBuffType() == AbstractBuff::BUFF_TYPE::BLEED)
				{
					HarmParam harmParam;
					harmParam.mTargetIndex = i;
					harmParam.mTargetSide = side;
					harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_BLEED;
					harmParam.mComboCount = 0;
					harmParam.mValue = buff->doRoundTick();
					fighter->onHarm(harmParam.mValue);
					vecHarm.push_back(harmParam);
				}
			}

			//�ж�
			for (int j = 0; j < vecBuff->size(); j++)
			{
				AbstractBuff* buff = vecBuff->at(j);

				if (buff->getBuffType() == AbstractBuff::BUFF_TYPE::POISON)
				{
					HarmParam harmParam;
					harmParam.mTargetIndex = i;
					harmParam.mTargetSide = side;
					harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_POISON;
					harmParam.mComboCount = 0;
					harmParam.mValue = buff->doRoundTick();
					fighter->onHarm(harmParam.mValue);
					vecHarm.push_back(harmParam);
				}
			}

			//��ÿ�������ϵ�ͳ��Ч�����뵽map
			mapHarm.insert(pair<int, vector<HarmParam> >(i, vecHarm));
		}

	};

	//ִ���ڲ�����
	buffRoundTick(FightStateMachine::E_SIDE::MY_SIDE, roundParam.mMyRoundHarm);
	buffRoundTick(FightStateMachine::E_SIDE::ENEMY_SIDE, roundParam.mEnemyRoundHarm);
}

bool FightModel::isMySideFirst()
{
	return getMyFirstValue() >= getEnemyFirstValue();
}