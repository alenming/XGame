#include "AbstractBuff.h"
#include "../Fighter.h"
#include "../Skill/SkillResultParam.h"

AbstractBuff::AbstractBuff()
	: mBuffData(nullptr)
	, mBuffId(-1)
	, mMainBuffType(-1)
	, mBuffType(-1)
	, mOnceEffectResId(-1)
	, mConstancyEffectResId(-1)
	, mChance(0.0f)
	, mMaxRound(-1)
	, mQuoteValueType(-1)
	, mQuoteAttrType(-1)
	, mFactorA(0.0f)
	, mFactorB(0.0f)
	, mAttacker(nullptr)
	, mTarget(nullptr)
	, mCurrentRound(-1)
	, mHarm(0)
	, mGainType(-1)
	, mReplacePriority(-1)
	, mCircleFlip(false)
	, mOnceFlip(false)
{

}

AbstractBuff::~AbstractBuff()
{

}

AbstractBuff* AbstractBuff::create(int buffId)
{
	AbstractBuff* buff = new AbstractBuff();
	if (buff && buff->init(buffId))
	{
		buff->autorelease();
		return buff;
	}
	else
	{
		delete buff;
		return nullptr;
	}
}

bool AbstractBuff::init(int buffId)
{
	mBuffId = buffId;

	DataManager* dataManager = DataManager::getInstance();
	mBuffData = dataManager->searchBuffById(mBuffId);

	if (mBuffData)
	{
		//buffͨ������
		setMainBuffType(mBuffData->getIntData("mbufTyp"));
		setBuffType(mBuffData->getIntData("bufTyp"));
		setChance(mBuffData->getFloatData("bufPrb"));
		setMaxRound(mBuffData->getIntData("rnd"));
		setGainType(mBuffData->getIntData("gainTpy"));
		setReplacePriority(mBuffData->getIntData("priority"));
		setCurrentRound(getMaxRound());

		//��ֵ��buff����
		setQuoteValueType(mBuffData->getIntData("quoTarTyp"));
		setQuoteAttrType(mBuffData->getIntData("quoValType"));
		setFactorA(mBuffData->getFloatData("factorA"));
		setFactorB(mBuffData->getFloatData("factorB"));

		//��ЧId
		mOnceEffectResId = mBuffData->getIntData("onceBuff");
		mConstancyEffectResId = mBuffData->getIntData("serialBuff");
		mCircleFlip = mBuffData->getIntData("rotate") == 1;
		mOnceFlip = mBuffData->getIntData("onceRotate") == 1;

		return true;
	}
	return false;
}

double AbstractBuff::getFighterAttr(Fighter* fighter, int quoteAttrType)
{
	double value = 0;
	if (fighter)
	{
		switch(quoteAttrType)
		{
		case QUOTE_ATTR_TYPE::QUATE_ATK:
			value = fighter->getAtk();
			break;
		case QUOTE_ATTR_TYPE::QUATE_HP:
			value = fighter->getCurrHP();
			break;
		case QUOTE_ATTR_TYPE::QUATE_MAX_HP:
			value = fighter->getMaxHP();
			break;
		case QUOTE_ATTR_TYPE::QUATE_PDD:
			value = fighter->getPdd();
			break;
		case QUOTE_ATTR_TYPE::QUATE_MDD:
			value = fighter->getMdd();
			break;
		case QUOTE_ATTR_TYPE::QUATE_CRIT:
			value = fighter->getCriPrb();
			break;
		case QUOTE_ATTR_TYPE::QUATE_HARM:
			value = abs(getHarm());
			break;
		case QUOTE_ATTR_TYPE::QUATE_DAM_COE:
			value = fighter->getDamCoe();
			break;
		case QUOTE_ATTR_TYPE::QUATE_DAM_RES:
			value = fighter->getDamRes();
			break;
		case QUOTE_ATTR_TYPE::QUATE_HIT:
			value = fighter->getHitPrb();
			break;
		case QUOTE_ATTR_TYPE::QUATE_CRIT_RES:
			value = fighter->getCriRes();
			break;
		case QUOTE_ATTR_TYPE::QUATE_CRIT_COE:
			value = fighter->getCriCoe();
			break;
		}
	}
	return value;
}

double AbstractBuff::doAttrAddition(int quoteAttrType)
{
	double value = 0;

	switch (getQuoteValueType())
	{
	case QUOTE_VALUE_TYPE::TARGET_ATTR:
		{
			double attr = getFighterAttr(mTarget, quoteAttrType);
			value = attr * getFactorA() + getFactorB();
		}
		break;
	case QUOTE_VALUE_TYPE::ATTACKER_ATTR:
		{
			double attr = getFighterAttr(mAttacker,quoteAttrType);
			value = attr * getFactorA() + getFactorB();
		}
		break;
	default:
		break;
	}
	return value;
}

double AbstractBuff::doAttrAddition()
{
	return doAttrAddition(getQuoteAttrType());
}

//����ֵ����
double AbstractBuff::atkAddition()
{
	if (getBuffType() == BUFF_TYPE::ATK_ADDITION)
	{
		double v = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}
	return 0;
}

//�������ֵ����
double AbstractBuff::defAddition()
{
	if (getBuffType() == BUFF_TYPE::DEF_ADDITION)
	{
		double v = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}

	if (getBuffType() == BUFF_TYPE::MDD_PDD_ADDITION)
	{
		double v = doAttrAddition(QUOTE_ATTR_TYPE::QUATE_PDD);
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}

	return 0;
}

//ħ����������
double AbstractBuff::mdefAddition()
{
	if (getBuffType() == BUFF_TYPE::MDEF_ADDITION)
	{
		double v = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}

	if (getBuffType() == BUFF_TYPE::MDD_PDD_ADDITION)
	{
		double v = doAttrAddition(QUOTE_ATTR_TYPE::QUATE_MDD);
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}

	return 0;
}

//��������
double AbstractBuff::critAddition()
{
	if (getBuffType() == BUFF_TYPE::CRIT_ADDITION)
	{
		double v = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}
	return 0;
}

//��������
double AbstractBuff::dodgeAddition()
{
	if (getBuffType() == BUFF_TYPE::DODGE_ADDITION)
	{
		double v = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}
	return 0;
}

//�˺����Ӽӳ�
double AbstractBuff::damageUpAddition()
{
	if (getBuffType() == BUFF_TYPE::DAMAGE_UP)
	{
		double v = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}
	return 0;
}

//�˺����ټӳ�
double AbstractBuff::damageDownAddition()
{
	if (getBuffType() == BUFF_TYPE::DAMAGE_DOWN)
	{
		double v = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}
	return 0;
}

//���мӳ�
double AbstractBuff::hitAddition()
{
	if (getBuffType() == BUFF_TYPE::HIT_ADDITION)
	{
		double v = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}
	return 0;
}

//�������Լӳ�
double AbstractBuff::critResAddition()
{
	if (getBuffType() == BUFF_TYPE::CRIT_RES_ADDITION)
	{
		double v = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}
	return 0;
}

//�����˺��ӳ�
double AbstractBuff::critCoeAddition()
{
	if (getBuffType() == BUFF_TYPE::CRIT_COE_ADDITION)
	{
		double v = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}
	return 0;
}

//����ʱ����BuffЧ������Ѫ��
int AbstractBuff::doBlood(int harm)
{
	setHarm(harm);

	if (getBuffType() == BUFF_TYPE::BLOOD)
	{
		//ע����Ѫ���������˺����㣬���˺�Ϊ���������Դ˴���Ҫ������ֵת��Ϊ����
		double v = abs(doAttrAddition());
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}

	return 0;
}

//�����˺�����
int AbstractBuff::doBeaten(int harm)
{
	setHarm(harm);

// 	if (mBuffType == BUFF_TYPE::COUNTER)
// 	{
// 		return doAttrAddition();
// 	}
	return 0;
}

//ÿ�غϽ�������buffЧ��
int AbstractBuff::doRoundTick()
{
	int buffType = getBuffType();
	if (buffType == BUFF_TYPE::POISON || buffType == BUFF_TYPE::REGON || buffType == BUFF_TYPE::BLEED)
	{
		int addition = doAttrAddition();
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), addition);
		mTarget->onHarm(addition);
		return addition;
	}
	return 0;
}

//�Ƿ��ֹȫ������
bool AbstractBuff::isBanAll()
{
	return getBuffType() == BUFF_TYPE::DIZZY;
}

//�Ƿ��ֹ��ͨ����
bool AbstractBuff::isBanNormal()
{
	return getBuffType() == BUFF_TYPE::BAN_NOMAL;
}

//�Ƿ��ֹ׷��
bool AbstractBuff::isBanRush()
{
	return getBuffType() == BUFF_TYPE::BAN_RUSH;
}

//�Ƿ��ֹ��������
bool AbstractBuff::isBanActive()
{
	return getBuffType() == BUFF_TYPE::SILENCE;
}

bool AbstractBuff::decreaseRound()
{
	int currentRound = getCurrentRound();
	currentRound--;
	setCurrentRound(currentRound);
	if (currentRound <= 0)
	{
		return true;
	}
	return false;
}

//���buff
BuffUpdateParam AbstractBuff::addToTarget(Fighter* target)
{
	mTarget = target;
	BuffUpdateParam buffUpdateParam;

	//��ɫ���������κ�buff����
	if (!mTarget->isAlive())
	{
		return buffUpdateParam;
	}

	//�����ӵ�buff�����߻����޵У������Ŀ���������и���buff
	if (isBuffResist() || isInvincible())
	{
		vector<AbstractBuff*> vecRemoveBuff;
		Vector<AbstractBuff*>* vecBuff = mTarget->getVecBuff();
		for (int i = 0; i < vecBuff->size(); i++)
		{
			AbstractBuff* tempBuff = vecBuff->at(i);
			if (tempBuff && tempBuff->isDebuff())
			{
				vecRemoveBuff.push_back(tempBuff);
			}
		}

		for (int i = 0; i < vecRemoveBuff.size(); i++)
		{
			AbstractBuff* tempBuff = vecRemoveBuff.at(i);
			buffUpdateParam.mVecRemoveBuff.push_back(mTarget->removeBuff(tempBuff));
		}
	}


	//���Ŀ�����������߻����޵�Ч���������и���buff���޷����
	if ((mTarget->isBuffResist() || mTarget->isInvincible()) && isDebuff())
	{
		return buffUpdateParam;
	}

	//20160601: ����������ֵ��buff���Ե���
	AbstractBuff* replaceBuff = nullptr;
	Vector<AbstractBuff*>* vecBuff = mTarget->getVecBuff();
	for (int i = 0; i < vecBuff->size(); i++)
	{
		//Buff�滻�߼�,�滻������ͬ�����滻
		AbstractBuff* tempBuff = vecBuff->at(i);

		//������buff���滻���ȼ�
		if (tempBuff->getMainBuffType() == MAIN_BUFF_TYPE::CONTROL_BUFF &&
			getMainBuffType() == MAIN_BUFF_TYPE::CONTROL_BUFF)
		{
			//���ȼ�
			if (getReplacePriority() >= tempBuff->getReplacePriority())
			{
				replaceBuff = tempBuff;
				break;
			}
		}
	}

	//���buff
	buffUpdateParam.mVecAddBuff.push_back(mTarget->addBuff(this));
	
	//�Ƴ����滻��buff
	if (replaceBuff)
	{
		buffUpdateParam.mVecRemoveBuff.push_back(mTarget->removeBuff(replaceBuff));
	}

	return buffUpdateParam;
}

//���ù�����
void AbstractBuff::setAttacker(Fighter* attacker)
{
	mAttacker = attacker;
}

int AbstractBuff::getBuffID()
{
	return mBuffId;
}

bool AbstractBuff::isDebuff()
{
	return (getGainType() == BUFF_GAIN_TYPE::DEBUFF);
}

//��ȡһ����buff��Чid
int AbstractBuff::getOnceEffectResId()
{
	return mOnceEffectResId;
}

//��ȡ������buff��Чid
int AbstractBuff::getConstancyEffectResId()
{
	return mConstancyEffectResId;
}

bool AbstractBuff::isTyrants()
{
	return getBuffType() == TYRANTS;
}

bool AbstractBuff::isBuffResist()
{
	return getBuffType() == BUFF_RESIST;
}

bool AbstractBuff::isInvincible()
{
	return getBuffType() == INVINCIBLE;
}

//�Ƿ�������������
bool AbstractBuff::isPhyResist()
{
	return getBuffType() == PHYSICAL_RESIST;
}

//�Ƿ��Ƿ�����������
bool AbstractBuff::isMagResist()
{
	return getBuffType() == MAGIC_RESIST;
}

bool AbstractBuff::isCircleFlip()
{
	return mCircleFlip;
}

bool AbstractBuff::isOnceFlip()
{
	return mOnceFlip;
}
