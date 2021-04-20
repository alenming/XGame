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
		//buff通用属性
		setMainBuffType(mBuffData->getIntData("mbufTyp"));
		setBuffType(mBuffData->getIntData("bufTyp"));
		setChance(mBuffData->getFloatData("bufPrb"));
		setMaxRound(mBuffData->getIntData("rnd"));
		setGainType(mBuffData->getIntData("gainTpy"));
		setReplacePriority(mBuffData->getIntData("priority"));
		setCurrentRound(getMaxRound());

		//数值类buff属性
		setQuoteValueType(mBuffData->getIntData("quoTarTyp"));
		setQuoteAttrType(mBuffData->getIntData("quoValType"));
		setFactorA(mBuffData->getFloatData("factorA"));
		setFactorB(mBuffData->getFloatData("factorB"));

		//特效Id
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

//攻击值修正
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

//物理防御值修正
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

//魔法防御修正
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

//暴击修正
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

//闪避修正
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

//伤害增加加成
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

//伤害减少加成
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

//命中加成
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

//暴击抗性加成
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

//暴击伤害加成
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

//攻击时处理Buff效果（吸血）
int AbstractBuff::doBlood(int harm)
{
	setHarm(harm);

	if (getBuffType() == BUFF_TYPE::BLOOD)
	{
		//注：吸血可能引用伤害计算，因伤害为负数，所以此处需要将计算值转化为正数
		double v = abs(doAttrAddition());
		CCLOG("buff add target[%d][%d] : targetId=%d, buffType=%d, value=%f", mTarget->getSide(), mTarget->getIndex(), mTarget->getHeroID(), getBuffType(), v);
		return v;
	}

	return 0;
}

//被击伤害处理
int AbstractBuff::doBeaten(int harm)
{
	setHarm(harm);

// 	if (mBuffType == BUFF_TYPE::COUNTER)
// 	{
// 		return doAttrAddition();
// 	}
	return 0;
}

//每回合结束触发buff效果
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

//是否禁止全部攻击
bool AbstractBuff::isBanAll()
{
	return getBuffType() == BUFF_TYPE::DIZZY;
}

//是否禁止普通攻击
bool AbstractBuff::isBanNormal()
{
	return getBuffType() == BUFF_TYPE::BAN_NOMAL;
}

//是否禁止追击
bool AbstractBuff::isBanRush()
{
	return getBuffType() == BUFF_TYPE::BAN_RUSH;
}

//是否禁止主动技能
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

//添加buff
BuffUpdateParam AbstractBuff::addToTarget(Fighter* target)
{
	mTarget = target;
	BuffUpdateParam buffUpdateParam;

	//角色死亡不做任何buff操作
	if (!mTarget->isAlive())
	{
		return buffUpdateParam;
	}

	//如果添加的buff是免疫或者无敌，则清除目标身上所有负面buff
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


	//如果目标身上有免疫或者无敌效果，则所有负面buff均无法添加
	if ((mTarget->isBuffResist() || mTarget->isInvincible()) && isDebuff())
	{
		return buffUpdateParam;
	}

	//20160601: 需求变更，数值类buff可以叠加
	AbstractBuff* replaceBuff = nullptr;
	Vector<AbstractBuff*>* vecBuff = mTarget->getVecBuff();
	for (int i = 0; i < vecBuff->size(); i++)
	{
		//Buff替换逻辑,替换类型相同则发生替换
		AbstractBuff* tempBuff = vecBuff->at(i);

		//控制类buff有替换优先级
		if (tempBuff->getMainBuffType() == MAIN_BUFF_TYPE::CONTROL_BUFF &&
			getMainBuffType() == MAIN_BUFF_TYPE::CONTROL_BUFF)
		{
			//优先级
			if (getReplacePriority() >= tempBuff->getReplacePriority())
			{
				replaceBuff = tempBuff;
				break;
			}
		}
	}

	//添加buff
	buffUpdateParam.mVecAddBuff.push_back(mTarget->addBuff(this));
	
	//移除被替换的buff
	if (replaceBuff)
	{
		buffUpdateParam.mVecRemoveBuff.push_back(mTarget->removeBuff(replaceBuff));
	}

	return buffUpdateParam;
}

//设置攻击者
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

//获取一次性buff特效id
int AbstractBuff::getOnceEffectResId()
{
	return mOnceEffectResId;
}

//获取持续性buff特效id
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

//是否是物理攻击免疫
bool AbstractBuff::isPhyResist()
{
	return getBuffType() == PHYSICAL_RESIST;
}

//是否是法术攻击免疫
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
