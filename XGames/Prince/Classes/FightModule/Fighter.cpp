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

//��ȡ��ɫ���ڷ�
FightStateMachine::E_SIDE Fighter::getSide()
{
	return mSide;
}

//���ý�ɫ���ڷ�
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
	
	//��ɫ������ɾ��ȫ��buff
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
			onHarm(bloodValue); //ִ����Ѫ��ֵЧ��
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

//��ȡ����1����
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

//���Buff
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

//�Ƴ�Buff
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

//�Ƿ��ֹȫ������
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

//�Ƿ��ֹ��ͨ����
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

//�Ƿ��ֹ׷��
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

//�Ƿ��ֹ��������
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


//�Ƿ�������������
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

//�Ƿ���ħ����������
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

//��ȡ��ɫ������
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

	//���������߽��ж�����СΪ1
	if (atk < 1)
	{
		atk = 1.0f;
	}

	return atk;
}

//��ȡ��ɫ���������
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

//��ȡ��ɫħ��������
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

//��ȡ��ɫ������
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

//��ȡ��ɫ������
float Fighter::getFinalDodge()
{
	//�ݲ�����
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

//��ȡ��ɫ������
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

//��ȡ��ɫ��������
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

//��ȡ��ɫ�����˺�
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

//��ȡ��ɫ�˺��ӳ�
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

//��ȡ��ɫ�˺�����
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

//�ۼ�buff�����غ�
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

	//ͳ�Ʊ�ɾ����buff��Ϣ
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