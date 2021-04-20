#include "CocoStudio.h"

#include "LayerFighter.h"
#include "SceneFight.h"
#include "UIFighterDamage.h"
#include "FightModel.h"
#include "UIAbstractSkillEffect.h"
#include "LayerFightBack.h"
#include "Utils/SoundUtils.h"
#include "Common/Common.h"

using namespace cocos2d;
using namespace cocostudio;

const int nHOR_SPACE_FIGHTER_2_FIGHTER		= 125;
const int nHOR_SPACE_FIGHTER_2_WINDOW		= 20;

const int nVER_SPACE_FIGHTER_2_FIGHTER		= 100;
const int nVER_SPACE_FIGHTER_2_BOTTOM		= 150;

LayerFighter::LayerFighter()
	: m_pParent(nullptr)
	, m_nLastIndex(-1)
	, m_nLastSklType(-1)
	, m_lastSide(FightStateMachine::E_SIDE::MY_SIDE)
	, m_nCurrIndex(-1)
	, m_nCurrSklType(-1)
	, m_currSide(FightStateMachine::E_SIDE::MY_SIDE)
	, mLastState(AbstractSkill::RESULT_STATE::NONE_STATE)
	, mCurrentState(AbstractSkill::RESULT_STATE::NONE_STATE)
	, mLastStateFighter(nullptr)
	, mCurrentStateFighter(nullptr)
{
	memset(m_pUIFighter, 0, sizeof(m_pUIFighter));
}


LayerFighter::~LayerFighter()
{

}


bool LayerFighter::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUIFighter();

	return true;
}


void LayerFighter::onEnter()
{
	Layer::onEnter();
	m_pParent = dynamic_cast<SceneFight*>(this->getParent());
}

void LayerFighter::onExit()
{
	unscheduleAllSelectors();
	m_pParent = nullptr;
	Layer::onExit();
}

void LayerFighter::initUIFighter()
{
	int offset = 1280 / 2;

	initUIPosition();
	initUIZorder();

	CCLOG("!--initUIFighter");

	memset(m_pUIFighter, 0, sizeof(m_pUIFighter));
	memset(mAssistFighter, 0, sizeof(m_pUIFighter));

	Vector<Fighter*>* pHeroList = FightModel::getInstance()->getHeroList();

	//我方战斗角色初始化
	for (ssize_t i=0; i<pHeroList->size(); ++i)
	{
		Fighter* pFighter = pHeroList->at(i);
		int nIndex = pFighter->getIndex();
		int nResID = pFighter->getResID();

		UIFighter* fighter = UIFighter::create(nResID, FightStateMachine::E_SIDE::MY_SIDE, nIndex);
		int order = m_nUIZorder[0][nIndex];
		//需加初始偏移量
		fighter->setPosition(
			getPositionByIndex(FightStateMachine::E_SIDE::MY_SIDE, nIndex) + Point(-offset, 0));
		this->addChild(fighter, order);
		m_pUIFighter[0][nIndex] = fighter;
	}

	//敌方战斗角色初始化
	Vector<Fighter*>* pMonsterList = FightModel::getInstance()->getMonsterList();
	for (ssize_t i=0; i<pMonsterList->size(); ++i)
	{
		Fighter* pFighter = pMonsterList->at(i);
		int nIndex = pFighter->getIndex();
		int nResID = pFighter->getResID();

		UIFighter* fighter = UIFighter::create(nResID, FightStateMachine::E_SIDE::ENEMY_SIDE, nIndex);
		int order = m_nUIZorder[1][nIndex];
		//需加初始偏移量
		fighter->setPosition(
			getPositionByIndex(FightStateMachine::E_SIDE::ENEMY_SIDE, nIndex) + Point(offset, 0));
		this->addChild(fighter, order);
		m_pUIFighter[1][nIndex] = fighter;
	}

	//助战角色初始化
	Vector<Fighter*>* pAssistList = FightModel::getInstance()->getAssistList();
	for (ssize_t i = 0; i < pAssistList->size(); ++i)
	{
		Fighter* pFighter = pAssistList->at(i);
		int nIndex = pFighter->getIndex();
		int nResID = pFighter->getResID();
		FightStateMachine::E_SIDE side = pFighter->getSide();

		UIFighter* fighter = UIFighter::create(nResID, side, nIndex, true);
		int order = m_nUIZorder[side][nIndex];
		//需加初始偏移量
		int assistOffset = (side == FightStateMachine::E_SIDE::MY_SIDE) ? -offset : offset;
		fighter->setPosition(getPositionByIndex(side, nIndex) + Point(assistOffset, 0));
		this->addChild(fighter, order);
		mAssistFighter[side][nIndex] = fighter;
	}

}

void LayerFighter::onAttackStart( SkillResultParam* pSkill, SKILL_EFFECT* skillEffect )
{
	int nAttackerSide = ENUM_TO_INT(pSkill->mAttackSide);
	int nTargetSide = ENUM_TO_INT(pSkill->mTargetSide);

	if (nullptr==m_pUIFighter[nAttackerSide][pSkill->mAttackerIndex])
	{
		CCLOG("!!error, m_pUIFighter[%d][%d] null", nAttackerSide, pSkill->mAttackerIndex);
		return;
	}

	if (pSkill->mVecTargetIndex.empty())
	{
		CCLOG("!!error, Skill target empty");
		onAttackFinish();
		return;
	}

	m_nLastIndex = m_nCurrIndex;
	m_nLastSklType = m_nCurrSklType;
	m_lastSide = m_currSide;

	m_nCurrIndex = pSkill->mAttackerIndex;
	m_nCurrSklType = pSkill->mSkillType;
	m_currSide = pSkill->mAttackSide;

	mMapMyBuffUpdateParam = pSkill->mMapMyBuffUpdateParam;
	mMapEnemyBuffUpdateParam = pSkill->mMapEnemyBuffUpdateParam;

	//统计当前追打状态以及记录上一次追打状态
	mLastState = mCurrentState;
	mCurrentState = AbstractSkill::RESULT_STATE::NONE_STATE;
	mLastStateFighter = mCurrentStateFighter;
	mCurrentStateFighter = nullptr;
	for (size_t i=0; i<pSkill->mVecTargetHarm.size(); ++i)
	{
		HarmParam& harmParam = pSkill->mVecTargetHarm.at(i);
		Fighter* pTarget = FightModel::getInstance()->getFighter(harmParam.mTargetSide, harmParam.mTargetIndex);
		if (pTarget && pTarget->getState() != AbstractSkill::RESULT_STATE::NONE_STATE)
		{
			mCurrentStateFighter = getFighterByIndex(harmParam.mTargetSide, harmParam.mTargetIndex);
			mCurrentState = pTarget->getState();
			break;
		}
	}

	m_pUIFighter[nAttackerSide][pSkill->mAttackerIndex]->onAttackStart(pSkill, skillEffect);
}

void LayerFighter::onAttackFinish()
{
	updateBuff(mMapMyBuffUpdateParam, mMapEnemyBuffUpdateParam);
	m_pParent->onAttackFinish();
}


void LayerFighter::initUIPosition()
{
	Size minSize(1080, 720);
	Size viewSize = Director::getInstance()->getVisibleSize();
	Size winSize = Director::getInstance()->getWinSize();

	Point pos = Director::getInstance()->getVisibleOrigin();

	float fLeftMargin = nHOR_SPACE_FIGHTER_2_WINDOW + nFIHGTER_WIDTH/2 + (viewSize.width-minSize.width)/2;
	float fBottomMargin = nVER_SPACE_FIGHTER_2_BOTTOM;
	float fRowSpace = nHOR_SPACE_FIGHTER_2_FIGHTER;
	float fColSpace = nVER_SPACE_FIGHTER_2_FIGHTER;
	float fRowOffset = 20;

	for (int nSide=0; nSide<2; ++nSide)
	{
		for (int nIndex = 0; nIndex < INDEX_COUNT; ++nIndex)
		{
			int nRow = TO_POS_ROW(nIndex);
			int nCol = TO_POS_COL(nIndex);

			if (0 == nSide)
			{// MY_SIDE
				m_pUIPosition[nSide][nIndex] = Point(pos.x+fLeftMargin+fRowSpace*(2-nCol) + (ROW_COUNT-1-nRow)*fRowOffset, 
					fBottomMargin+fColSpace*(2-nRow) - 25);
			}
			else
			{// ENEMY_SIDE
				m_pUIPosition[nSide][nIndex] = Point(pos.x+viewSize.width-fLeftMargin-fRowSpace*(2-nCol) - (ROW_COUNT-1-nRow)*fRowOffset, 
					fBottomMargin+fColSpace*(2-nRow) - 25);
			}
		}
	}

	//格子缩放系数
	float gridScale = 0.7f;

	//初始化透视视角坐标
	for (int nSide=0; nSide<2; ++nSide)
	{
		for (int nIndex = 0; nIndex < INDEX_COUNT; ++nIndex)
		{
			int nRow = TO_POS_ROW(nIndex);
			int nCol = TO_POS_COL(nIndex);

			if (0 == nSide)
			{// MY_SIDE

				float xx = pos.x+fLeftMargin+fRowSpace*(2-nCol)*gridScale + (ROW_COUNT-1-nRow)*fRowOffset*gridScale +
					(fRowSpace*COL_COUNT + fRowOffset*(COL_COUNT-1))*(1-gridScale)/2.0f;
				float yy = fBottomMargin+fColSpace*(2-nRow)*gridScale - 25 + fColSpace*ROW_COUNT*(1-gridScale)/2.0f;

				m3dPosition[nSide][nIndex] = Point(xx, yy);
			}
			else
			{// ENEMY_SIDE
				float xx = pos.x+viewSize.width-fLeftMargin-fRowSpace*(2-nCol)*gridScale - (ROW_COUNT-1-nRow)*fRowOffset*gridScale - 
					(fRowSpace*COL_COUNT + fRowOffset*(COL_COUNT-1))*(1-gridScale)/2.0f;
				float yy = fBottomMargin+fColSpace*(2-nRow)*gridScale - 25 + fColSpace*ROW_COUNT*(1-gridScale)/2.0f;

				m3dPosition[nSide][nIndex] = Point(xx, yy);
			}
		}
	}
}


void LayerFighter::initUIZorder()
{
	for (int nSide=0; nSide<2; ++nSide)
	{
		for (int nIndex = 0; nIndex < INDEX_COUNT; ++nIndex)
		{
			int nRow = TO_POS_ROW(nIndex);
			int nCol = TO_POS_COL(nIndex);

			// 双方阵营同一index位置的英雄位于同一层级
			// 同一行的英雄，高1列的英雄位于高1个层级
			// 不同行的英雄，高1行的英雄位于高10个层级
			m_nUIZorder[nSide][nIndex] = 10*(ZORDER_FIGHTER_ROW_1+TO_POS_ROW(nIndex))+TO_POS_COL(nIndex);
		}
	}
}


cocos2d::Point LayerFighter::getPositionByIndex( FightStateMachine::E_SIDE side, int nIndex )
{
	int nSide = ENUM_TO_INT(side);
	return m_pUIPosition[nSide][nIndex];
}

Point LayerFighter::get3dPositionByIndex(FightStateMachine::E_SIDE side, int nIndex)
{
	int nSide = ENUM_TO_INT(side);
	return m3dPosition[nSide][nIndex];
}

UIFighter* LayerFighter::getFighterByIndex( FightStateMachine::E_SIDE side, int nIndex )
{
	int nSide = ENUM_TO_INT(side);
	return m_pUIFighter[nSide][nIndex];
}


void LayerFighter::onFighterDead(FightStateMachine::E_SIDE side, int nIndex)
{
	m_pUIFighter[ENUM_TO_INT(side)][nIndex]->setVisible(false);
	m_pUIFighter[ENUM_TO_INT(side)][nIndex] = nullptr;
}


int LayerFighter::getZorderByIndex( FightStateMachine::E_SIDE side, int nIndex )
{
	return m_nUIZorder[ENUM_TO_INT(side)][nIndex];
}


cocos2d::Point LayerFighter::getEffectStartPostion( cocos2d::Node* pNode )
{
	//return this->convertToNodeSpace(pNode->getPosition());
	return pNode->getPosition();
}


cocos2d::Point LayerFighter::getEffectActPosition(FightStateMachine::E_SIDE side)
{
	return getPositionByIndex(side, 4)+Size(0, nFIHGTER_HEIGHT/2);
}


bool LayerFighter::isConsequentAttack()
{
	return m_nCurrIndex == m_nLastIndex && m_lastSide == m_currSide;
}

bool LayerFighter::isStartRushAttack()
{
	return m_lastSide == m_currSide
		&& m_nLastSklType != AbstractSkill::SKILL_TYPE::RUSH_SKILL
		&& mLastStateFighter;
}

bool LayerFighter::isEndRushAttack()
{
	return //m_nLastSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL && 
		m_nCurrSklType != AbstractSkill::SKILL_TYPE::RUSH_SKILL;
}

void LayerFighter::onRoundStart()
{
	//回合开始阶段更新buff
	updateBuff(mBuffRoundHarm.mMapMyBuffUpdateParam, mBuffRoundHarm.mMapEnemyBuffUpdateParam);
}

void LayerFighter::onRoundEnd(RoundParam buffRoundHarm)
{
	mBuffRoundHarm = buffRoundHarm;
	auto updateOnRoundEnd = [this](float dt)
	{
		if (isAllFighterBack())
		{
			getScheduler()->unschedule("updateOnRoundEnd", this);

			//将伤害向每个战斗角色派发
			for (auto iter = mBuffRoundHarm.mMyRoundHarm.begin(); iter != mBuffRoundHarm.mMyRoundHarm.end(); iter++)
			{
				int index = iter->first;
				vector<HarmParam>* vecHarm = &(iter->second);
				m_pUIFighter[0][index]->startBuffRoundEffect(vecHarm);
			}

			for (auto iter = mBuffRoundHarm.mEnemyRoundHarm.begin(); iter != mBuffRoundHarm.mEnemyRoundHarm.end(); iter++)
			{
				int index = iter->first;
				vector<HarmParam>* vecHarm = &(iter->second);
				m_pUIFighter[1][index]->startBuffRoundEffect(vecHarm);
			}
		}
	};

	getScheduler()->schedule(updateOnRoundEnd, this, 0, false, "updateOnRoundEnd");
}

bool LayerFighter::isAllFighterBack()
{
	for (int side=0; side<2; ++side)
	{
		for (int i=0; i<INDEX_COUNT; ++i)
		{
			if (m_pUIFighter[side][i]!=nullptr)
			{
				if (m_pUIFighter[side][i]->getPosition() != this->getPositionByIndex(FightStateMachine::E_SIDE(side), i))
				{
					return false;
				}
			}
		}
	}

	return true;
}

SceneFight* LayerFighter::getSceneFight()
{
	return m_pParent;
}

void LayerFighter::startMoveOutAnimation(SKILL_PARAM& skillParam)
{
	Size winSize = Director::getInstance()->getWinSize();

	for (int side = 0; side < 2; side++)
	{
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			UIFighter* fighter = m_pUIFighter[side][i];
			if (!fighter) continue;

			fighter->stopAllActions();
			//fighter->runAction(EaseInOut::create(ScaleTo::create(0.3f, fighter->getScale()*1.15f), 2.0f));

			//筛选是否是被击对象
			bool isTarget = false;
			for (size_t j = 0; j < skillParam.vecpTarget.size(); j++)
			{
				UIFighter* target = skillParam.vecpTarget.at(j);
				if (target == fighter)
				{
					isTarget = true;
					break;
				}
			}

			if (fighter == skillParam.pAttacker || isTarget)
			{
				//攻击者和被击者移动到3d视角位置
				auto action =  EaseInOut::create(
					MoveTo::create(0.3f, get3dPositionByIndex(fighter->getSide(),
					fighter->getIndex())), 2.0f);
				fighter->runAction(action);
			}
			else
			{
				//其他角色移出屏幕
				float offsetX = (side == FightStateMachine::E_SIDE::MY_SIDE) ? -winSize.width / 2 : winSize.width / 2;
				auto action = MoveBy::create(0.2f,  Point(offsetX, 0));
				fighter->runAction(action);
			}
		}
	}
}

void LayerFighter::startMoveInAnimation(SKILL_PARAM& skillParam)
{
	for (int side = 0; side < 2; side++)
	{
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			UIFighter* fighter = m_pUIFighter[side][i];
			if (!fighter) continue;
			//fighter->runAction(EaseInOut::create(ScaleTo::create(0.3f, fighter->getScale()/1.15f), 2.0f));
			if (fighter != skillParam.pAttacker)
			{
				//非攻击者移回屏幕
				auto action = MoveTo::create(0.2f,  getPositionByIndex(fighter->getSide(), fighter->getIndex()));
				fighter->runAction(action);
			}
		}
	}
}

UIFighter* LayerFighter::getLastAttacker()
{
	return getFighterByIndex(m_lastSide, m_nLastIndex);
}

bool LayerFighter::isLastSkillActiveOrRush()
{
	return m_lastSide == m_currSide 
		&& (m_nLastSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL 
		|| m_nCurrSklType == AbstractSkill::SKILL_TYPE::ACTIVE_SKILL);
}

//判定上一个技能是否是主动技能
bool LayerFighter::isLastSkillActive()
{
	return m_lastSide == m_currSide && m_nCurrSklType == AbstractSkill::SKILL_TYPE::ACTIVE_SKILL;
}

//判定上一个技能是否是追打技能
bool LayerFighter::isLastSkillRush()
{
	return m_lastSide == m_currSide && m_nLastSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL;
}

bool LayerFighter::isLastCurSkillNormal()
{
	return m_lastSide == m_currSide 
		&& m_nLastSklType == AbstractSkill::SKILL_TYPE::NORMAL_SKILL
		&& m_nCurrSklType == AbstractSkill::SKILL_TYPE::NORMAL_SKILL;
}

void LayerFighter::startJump(float dt, UIFighter* fighter, int count)
{
	getScheduler()->unschedule("startEntranceAnimation" + TO_STR(count), this);

	auto jumpEndFunc = [this, fighter, count]()->void
	{
		SoundUtils::playSoundEffect("luodi");
		fighter->getSkeletonAnimation()->addAnimation(0, szANI_WAIT, true);
		fighter->getSkeletonAnimation()->setTimeScale(1.0f);
	};

	Point targetPos = getPositionByIndex(fighter->getSide(), fighter->getIndex());
	fighter->startActionJump(SKILL_EFFECT::MOVE_TYPE::MOVE_JUMP, UIFighter::MOVE_SPEED_TYPE::MOVE_SPEED_DOWN,
		false, true, targetPos, 2200, 150, jumpEndFunc, true);
}

void LayerFighter::startEntranceAnimation(const function<void(void)>& callBack)
{
	auto accumulateTime = 0.0f;
	auto intervalTime = 0.15f;
	int count = 0;

	auto callBackFunc = [this, callBack](float dt)
		{
			getScheduler()->unschedule("startEntranceAnimationCallBack", this);
			callBack();
		};
	
	//我方出场
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		auto fighter = getFighterByIndex(FightStateMachine::MY_SIDE, i);
		if (fighter)
		{
			count++;
			float time = accumulateTime + (count * intervalTime);
			getScheduler()->schedule(CC_CALLBACK_1(LayerFighter::startJump, this, fighter, count),
				this, 0, 0, time , false, "startEntranceAnimation" + TO_STR(count));
		}
	}

	//敌方出场前延迟一段时间
	accumulateTime += 0.2f;

	//敌方出场
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		auto fighter = getFighterByIndex(FightStateMachine::ENEMY_SIDE, i);
		if (fighter)
		{
			count++;
			float time = accumulateTime + (count * intervalTime);
			getScheduler()->schedule(CC_CALLBACK_1(LayerFighter::startJump, this, fighter, count), 
				this, 0, 0, time , false, "startEntranceAnimation" + TO_STR(count));
		}
	}

	//最后执行回调
	accumulateTime += count * intervalTime + 0.5f;
	getScheduler()->schedule(callBackFunc, this, 0, 0, accumulateTime , false, "startEntranceAnimationCallBack");
}

void LayerFighter::startAssistEntrance(vector<AssistFighterInfo> vecAssistInfo, const function<void(void)>& callBack)
{
	auto accumulateTime = 0.0f;
	auto intervalTime = 0.15f;
	int count = 0;

	for(AssistFighterInfo info : vecAssistInfo)
	{
		UIFighter* fighter = mAssistFighter[info.side][info.index];
		if (fighter)
		{
			count++;
			float time = accumulateTime + (count * intervalTime);
			getScheduler()->schedule(CC_CALLBACK_1(LayerFighter::startJump, this, fighter, count),
				this, 0, 0, time , false, "startAssistEntrance" + TO_STR(count));

			//将助战角色添加到战斗角色数组中
			m_pUIFighter[info.side][info.index] = fighter;
		}
	}

	//最后执行回调
	accumulateTime += count * intervalTime + 0.5f;
	auto callBackFunc = [this, callBack](float dt)
	{
		getScheduler()->unschedule("startAssistEntranceCallBack", this);
		callBack();
	};
	getScheduler()->schedule(callBackFunc, this, 0, 0, accumulateTime , false, "startAssistEntranceCallBack");
}

void LayerFighter::startWinAnimation(FightStateMachine::E_SIDE winSide)
{
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		auto fighter = getFighterByIndex(winSide, i);
		if (fighter && fighter->getFighterData() && fighter->getFighterData()->isAlive())
		{
			fighter->playActionWin();
		}
	}
}

void LayerFighter::updateBuff(map<int, BuffUpdateParam>& myMapBuffUpdate, map<int, BuffUpdateParam>& enemyMapBuffUpdate)
{
	for (auto iter = myMapBuffUpdate.begin(); iter != myMapBuffUpdate.end(); iter++)
	{
		BuffUpdateParam* buffUpdateParam = &(iter->second);
		int index = iter->first;
		UIFighter* fighter = getFighterByIndex(FightStateMachine::E_SIDE::MY_SIDE, index);
		if (fighter)
		{
			fighter->updateBuff(buffUpdateParam);
		}
	}

	for (auto iter = enemyMapBuffUpdate.begin(); iter != enemyMapBuffUpdate.end(); iter++)
	{
		BuffUpdateParam* buffUpdateParam = &(iter->second);
		int index = iter->first;
		UIFighter* fighter = getFighterByIndex(FightStateMachine::E_SIDE::ENEMY_SIDE, index);
		if (fighter)
		{
			fighter->updateBuff(buffUpdateParam);
		}
	}
}

void LayerFighter::removeAllUIBuff()
{
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		UIFighter* fighter = getFighterByIndex(FightStateMachine::E_SIDE::MY_SIDE, i);
		if (fighter)
		{
			fighter->removeAllUIBuff();
		}

		fighter = getFighterByIndex(FightStateMachine::E_SIDE::ENEMY_SIDE, i);
		if (fighter)
		{
			fighter->removeAllUIBuff();
		}
	}
}

AbstractSkill::RESULT_STATE LayerFighter::getLastState()
{
	return mLastState;
}

UIFighter* LayerFighter::getLastStateFighter()
{
	return mLastStateFighter;
}

void LayerFighter::getAllAliveFighterIndex( FightStateMachine::E_SIDE side, vector<pair<int, Point> >& vcAliveFighterIndex )
{
	vcAliveFighterIndex.clear();
	for (int i = 0; i < INDEX_COUNT; ++i)
	{
		UIFighter* fighter = m_pUIFighter[ENUM_TO_INT(side)][i];
		if (nullptr != fighter)
		{
			vcAliveFighterIndex.push_back(make_pair(i, fighter->getPosition()));
		}
	}
}

//隐藏所有死亡英雄
void LayerFighter::setDeadFighterUnvisible()
{
	for (int side = 0; side < 2; side++)
	{
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			UIFighter* uiFighter = m_pUIFighter[side][i];
			if (!uiFighter) continue;

			Fighter* fighter = uiFighter->getFighterData();
			if(!fighter) continue;

			if (!fighter->isAlive())
			{
				uiFighter->startActionDead();
			}
			
		}
	}
}

void LayerFighter::updateAllFighterHpBar()
{
	for (int side = 0; side < 2; side++)
	{
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			UIFighter* uiFighter = m_pUIFighter[side][i];
			if (!uiFighter) continue;

			Fighter* fighter = uiFighter->getFighterData();
			if(!fighter) continue;

			if (fighter->isAlive())
			{
				uiFighter->updateHP();
			}
		}
	}
}
