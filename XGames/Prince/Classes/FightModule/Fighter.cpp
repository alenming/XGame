#include "Fighter.h"
#include "FightMacros.h"
#include "Buff/AbstractBuff.h"
#include "Utils/ValueCrypt.h"
#include <algorithm>

Fighter::Fighter()
	: mHeroId(-1)
	, mResId(-1)
	, mName("")
	, mPosIndex(-1)
	, mMaxHP(0)
	, mCurrHP(0)
	, mInitialHp(0)
	, mState(AbstractSkill::RESULT_STATE::NONE_STATE)
	, mNormalSkill(nullptr)
	, mActiveSkill(nullptr)
	, mRushSkill(nullptr)
	, mBuffSkill(nullptr)
	, mTriggerSkill(nullptr)
	, mSide(FightStateMachine::E_SIDE::MY_SIDE)
	, mTagTyp(-1)
	, mAtkTyp(-1)
	, mAtk(0)
	, mPdd(0)
	, mMdd(0)
	, mCriPrb(0.0f)
	, mCriCoe(0.0f)
	, mCtrPrb(0.0f)
	, mDodgePrb(0.0f)
	, mHitPrb(0.0f)
	, mCriRes(0.0f)
	, mJobTyp(0)
	, mSexTyp(0)
{

}

Fighter::~Fighter()
{
	mVecBuff.clear();
	CC_SAFE_RELEASE(mNormalSkill);
	CC_SAFE_RELEASE(mActiveSkill);
	CC_SAFE_RELEASE(mRushSkill);
	CC_SAFE_RELEASE(mBuffSkill);
	CC_SAFE_RELEASE(mTriggerSkill);
}

Fighter* Fighter::create()
{
	Fighter* fighter = new Fighter();
	fighter->autorelease();
	return fighter;
}

int Fighter::getHeroID() const
{
	return mHeroId;
}

void Fighter::setHeroID( int n )
{
	mHeroId = n;
}

int Fighter::getResID() const
{
	return mResId;
}

void Fighter::setResID( int n )
{
	mResId = n;
}

std::string Fighter::getName() const
{
	return mName;
}

void Fighter::setName( const char* szName )
{
	mName = szName;
}

//获取角色所在方
FightStateMachine::E_SIDE Fighter::getSide()
{
	return mSide;
}

//设置角色所在方
void Fighter::setSide(FightStateMachine::E_SIDE side)
{
	mSide = side;
}

bool Fighter::isAlive()
{
	return getCurrHP() > 0;
}

void Fighter::onHarm(int harmValue)
{
	long long curHp = getCurrHP();
	long long maxHp = getMaxHP();

	curHp += harmValue;
	curHp = curHp < 0 ? 0 : curHp;
	curHp = curHp > maxHp ? maxHp : curHp;

	setCurrHP(curHp);
	
	//角色死亡则删除全部buff
	if (!isAlive())
	{
		mVecBuff.clear();
	}
}

int Fighter::onBlood(int harmValue)
{
	int bloodValue = 0;
	for (int i = 0; i < mVecBuff.size(); i++)
	{
		AbstractBuff* buff = mVecBuff.at(i);

		bloodValue = buff->doBlood(harmValue);
		if (bloodValue > 0)
		{
			onHarm(bloodValue); //执行吸血数值效果
			return bloodValue;
		}
	}
	return bloodValue;
}

AbstractSkill::RESULT_STATE Fighter::getState()
{
	return mState;
}

void Fighter::setState(AbstractSkill::RESULT_STATE state)
{
	mState = state;
}

void Fighter::clearState()
{
	mState = AbstractSkill::RESULT_STATE::NONE_STATE;
}

void Fighter::setNormalSkill(int skillId)
{
	if (!mNormalSkill)
	{
		mNormalSkill = NormalSkill::create(skillId);
		if (mNormalSkill)
		{
			mNormalSkill->setAttacker(this);
			mNormalSkill->retain();
		}
	}
	else
	{
		mNormalSkill->init(skillId);
	}
}

void Fighter::setActiveSkill(int skillId)
{
	if (!mActiveSkill)
	{
		mActiveSkill = ActiveSkill::create(skillId);
		if (mActiveSkill)
		{
			mActiveSkill->setAttacker(this);
			mActiveSkill->retain();
		}
	}
	else
	{
		mActiveSkill->init(skillId);
	}
}

void Fighter::setRushSkill(int skillId)
{
	if (!mRushSkill)
	{
		mRushSkill = RushSkill::create(skillId);
		if (mRushSkill)
		{
			mRushSkill->setAttacker(this);
			mRushSkill->retain();
		}
	}
	else
	{
		mRushSkill->init(skillId);
	}
}

void Fighter::setBuffSkill(int skillId)
{
	if (!mBuffSkill)
	{
		mBuffSkill = BuffSkill::create(skillId);
		if (mBuffSkill)
		{
			mBuffSkill->setAttacker(this);
			mBuffSkill->retain();
		}
	}
	else
	{
		mBuffSkill->init(skillId);
	}
}

void Fighter::setTriggerSkill(int skillId)
{
	if (!mTriggerSkill)
	{
		mTriggerSkill = TriggerSkill::create(skillId);
		if (mTriggerSkill)
		{
			mTriggerSkill->setAttacker(this);
			mTriggerSkill->retain();
		}
	}
	else
	{
		mTriggerSkill->init(skillId);
	}
}

NormalSkill* Fighter::getNormalSkill()
{
	return mNormalSkill;
}

ActiveSkill* Fighter::getActiveSkill()
{
	return mActiveSkill;
}

//获取被动1技能
RushSkill* Fighter::getRushSkill()
{
	return mRushSkill;
}

BuffSkill* Fighter::getBuffSkill()
{
	return mBuffSkill;
}

TriggerSkill* Fighter::getTriggerSkill()
{
	return mTriggerSkill;
}

float Fighter::getPercentHP()
{
	return 100.0f * getCurrHP() / (double)getMaxHP();
}

//添加Buff
BuffInfo Fighter::addBuff(AbstractBuff* buff)
{
	BuffInfo buffInfo;
	buffInfo.mBuffId = buff->getBuffID();
	buffInfo.mBuffType = buff->getBuffType();
	buffInfo.mOnceResId = buff->getOnceEffectResId();
	buffInfo.mCircleResId = buff->getConstancyEffectResId();
	buffInfo.mCircleFlip = buff->isCircleFlip();
	buffInfo.mOnceFlip = buff->isOnceFlip();
	mVecBuff.pushBack(buff);
	return buffInfo;
}

//移除Buff
BuffInfo Fighter::removeBuff(AbstractBuff* buff)
{
	BuffInfo buffInfo;
	buffInfo.mBuffId = buff->getBuffID();
	buffInfo.mBuffType = buff->getBuffType();
	buffInfo.mOnceResId = buff->getOnceEffectResId();
	buffInfo.mCircleResId = buff->getConstancyEffectResId();
	buffInfo.mCircleFlip = buff->isCircleFlip();
	buffInfo.mOnceFlip = buff->isOnceFlip();
	mVecBuff.eraseObject(buff);
	return buffInfo;
}

Vector<AbstractBuff*>* Fighter::getVecBuff()
{
	return &mVecBuff;
}

//是否禁止全部攻击
bool Fighter::isBanAll()
{
	for (auto buff : mVecBuff)
	{
		if (buff && buff->isBanAll())
		{
			return true;
		}
	}
	return false;
}

//是否禁止普通攻击
bool Fighter::isBanNormal()
{
	for (auto buff : mVecBuff)
	{
		if (buff && buff->isBanNormal())
		{
			return true;
		}
	}
	return false;
}

//是否禁止追击
bool Fighter::isBanRush()
{
	for (auto buff : mVecBuff)
	{
		if (buff && buff->isBanRush())
		{
			return true;
		}
	}
	return false;
}

//是否禁止主动技能
bool Fighter::isBanActive()
{
	for (auto buff : mVecBuff)
	{
		if (buff && buff->isBanActive())
		{
			return true;
		}
	}
	return false;
}

bool Fighter::hasTyrants()
{
	for (auto buff : mVecBuff)
	{
		if (buff && buff->isTyrants())
		{
			return true;
		}
	}
	return false;
}

bool Fighter::isBuffResist()
{
	for (auto buff : mVecBuff)
	{
		if (buff && buff->isBuffResist())
		{
			return true;
		}
	}
	return false;
}

bool Fighter::isInvincible()
{
	for (auto buff : mVecBuff)
	{
		if (buff && buff->isInvincible())
		{
			return true;
		}
	}
	return false;
}


//是否是物理攻击免疫
bool Fighter::isPhyResist()
{
	for (auto buff : mVecBuff)
	{
		if (buff && buff->isPhyResist())
		{
			return true;
		}
	}
	return false;
}

//是否是魔法攻击免疫
bool Fighter::isMagResist()
{
	for (auto buff : mVecBuff)
	{
		if (buff && buff->isMagResist())
		{
			return true;
		}
	}
	return false;
}

//获取角色攻击力
float Fighter::getFinalAtk()
{
	float atk = getAtk();
	for (auto buff : mVecBuff)
	{
		if (buff)
		{
			atk += buff->atkAddition();
		}
	}

	//攻击需加入边界判定，最小为1
	if (atk < 1)
	{
		atk = 1.0f;
	}

	return atk;
}

//获取角色物理防御力
float Fighter::getFinalPDef()
{
	float pdef = getPdd();
	for (auto buff : mVecBuff)
	{
		if (buff)
		{
			pdef += buff->defAddition();
		}
	}
	return pdef;
}

//获取角色魔法防御力
float Fighter::getFinalMDef()
{
	float mdef = getMdd();
	for (auto buff : mVecBuff)
	{
		if (buff)
		{
			mdef += buff->mdefAddition();
		}
	}
	return mdef;
}

//获取角色暴击率
float Fighter::getFinalCrit()
{
	float crit = getCriPrb();
	for (auto buff : mVecBuff)
	{
		if (buff)
		{
			crit += buff->critAddition();
		}
	}
	return crit;
}

//获取角色闪避率
float Fighter::getFinalDodge()
{
	//暂不处理
	float dodge = getDodgePrb();
	for (auto buff : mVecBuff)
	{
		if (buff)
		{
			dodge += buff->dodgeAddition();
		}
	}
	return dodge;
}

//获取角色命中率
float Fighter::getFinalHitPrb()
{
	float hit = getHitPrb();
	for (auto buff : mVecBuff)
	{
		if (buff)
		{
			hit += buff->hitAddition();
		}
	}
	return hit;
}

//获取角色暴击抗性
float Fighter::getFinalCritRes()
{
	float criRes = getCriRes();
	for (auto buff : mVecBuff)
	{
		if (buff)
		{
			criRes += buff->critResAddition();
		}
	}
	return criRes;
}

//获取角色暴击伤害
float Fighter::getFinalCritCoe()
{
	float criCoe = getCriCoe();
	for (auto buff : mVecBuff)
	{
		if (buff)
		{
			criCoe += buff->critCoeAddition();
		}
	}
	return criCoe;
}

//获取角色伤害加成
float Fighter::getFinalDamCoe()
{
	float damCoe = getDamCoe();
	for (auto buff : mVecBuff)
	{
		if (buff)
		{
			damCoe += buff->damageUpAddition();
		}
	}
	return damCoe;
}

//获取角色伤害减免
float Fighter::getFinalDamRes()
{
	float damRes = getDamRes();
	for (auto buff : mVecBuff)
	{
		if (buff)
		{
			damRes += buff->damageDownAddition();
		}
	}
	return damRes;
}

//扣减buff持续回合
void Fighter::decreaseBuffRound(map<int, BuffUpdateParam>& mapBuffUpdateParam)
{
	vector<AbstractBuff*> removeBuffList;
	for (auto buff : mVecBuff)
	{
		if (buff && buff->decreaseRound())
		{
			removeBuffList.push_back(buff);
		}
	}

	//统计被删除的buff信息
	BuffUpdateParam buffUpdateParam;
	for (auto buff : removeBuffList)
	{
		buffUpdateParam.mVecRemoveBuff.push_back(removeBuff(buff));
	}
	mapBuffUpdateParam.insert(pair<int, BuffUpdateParam>(getIndex(), buffUpdateParam));
}

void Fighter::onChange(int monsterId)
{
	RowData* data = DataManager::getInstance()->searchMonsterById(monsterId);

	if(!data) return;

	setTagType(data->getIntData("tagTyp"));
	setAtkType(data->getIntData("atkTyp"));
	setPdd(data->getIntData("pdd"));
	setAtk(data->getIntData("atk"));
	int hp = data->getLongData("hp");
	setMaxHP(hp);
	setCurrHP(hp);
	setInitialHp(hp);
	setMdd(data->getIntData("mdd"));
	setCriPrb(data->getFloatData("criPrb"));
	setCriCoe(data->getFloatData("criCoe"));
	setCtrPrb(data->getFloatData("ctrPrb"));
	setDodgePrb(data->getFloatData("miss"));
	setHitPrb(data->getFloatData("hit"));
	setCriRes(data->getFloatData("criRes"));
	setDamCoe(0);
	setDamRes(0);

	int normalSkillId = data->getIntData("atkID");
	int activeSkillId = data->getIntData("a_sklID");
	int rushSkillId = data->getIntData("p1_sklID");
	int triggerSkillId = data->getIntData("p3_sklID");

	CCLOG("onChange:%d, normalSkillId:%d, activeSkill:%d, rushSkill:%d, triggerSkillId:%d",
		monsterId, normalSkillId, activeSkillId, rushSkillId, triggerSkillId);

	setNormalSkill(normalSkillId);
	setActiveSkill(activeSkillId);
	setRushSkill(rushSkillId);
	//setBuffSkill(data->getIntData("p2_sklID"));
	setTriggerSkill(triggerSkillId);
}