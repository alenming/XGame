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

//清空我方连击数
void FightModel::clearMyHitCount()
{
	setMyHitCount(0);
}

//增加我方连击数
void FightModel::addMyHitCount(int hitCount)
{
	int myHitCount = getMyHitCount();
	myHitCount += hitCount;
	setMyHitCount(myHitCount);
}

//清空敌方连击数
void FightModel::clearEnemyHitCount()
{
	setEnemyHitCount(0);
}

//增加敌方连击数
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

//设置战斗背景资源id
void FightModel::setBgResId(int resId)
{
	mBgResId = resId;
}

//获取战斗背景资源id
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
			//此处做容错，防止配置相同阵位的角色
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

//buff每回合的执行效果
void FightModel::onBuffRoundTick(RoundParam& roundParam)
{
	//内部函数
	auto buffRoundTick = [this](FightStateMachine::E_SIDE side, map<int, vector<HarmParam> >& mapHarm)
	{
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			Fighter* fighter = getAliveFighter(side, i);
			if (!fighter) continue;

			Vector<AbstractBuff*>* vecBuff = fighter->getVecBuff();
			if (vecBuff->empty()) continue;

			//每个人身上的所有伤害/治疗效果
			vector<HarmParam> vecHarm;

			//判定是否有流血buff
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

			//如果有流血buff，则治疗效果无效
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

			//流血
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

			//中毒
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

			//将每个人身上的统计效果插入到map
			mapHarm.insert(pair<int, vector<HarmParam> >(i, vecHarm));
		}

	};

	//执行内部函数
	buffRoundTick(FightStateMachine::E_SIDE::MY_SIDE, roundParam.mMyRoundHarm);
	buffRoundTick(FightStateMachine::E_SIDE::ENEMY_SIDE, roundParam.mEnemyRoundHarm);
}

bool FightModel::isMySideFirst()
{
	return getMyFirstValue() >= getEnemyFirstValue();
}