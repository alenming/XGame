#include "AbstractSkill.h"
#include "../FightModel.h"
#include "../FightMacros.h"
#include "../Buff/AbstractBuff.h"
#include "../Fighter.h"
#include "Utils/ValueCrypt.h"
#include <cmath>


AbstractSkill::AbstractSkill()
	: mSkillData(nullptr)
	, mSkillId(-1)
	, mSkillResId(-1)
	, mSkillType(-1)
	, mRangeType(-1)
	, mTargetType(-1)
	, mDamageType(-1)
	, mComboCount(-1)
	, mDamageCoe(-1)
	, mMBuffId(-1)
	, mClearBuff(0)
	, mExtraDamage(0)
	, mBaseValueType(-1)
	, mAttacker(nullptr)
	
{
	mFightModel = FightModel::getInstance();
}

AbstractSkill::~AbstractSkill()
{

}

bool AbstractSkill::init(int skillId)
{
	if (skillId < 0)
	{
		return false;
	}

	mSkillId = skillId;

	DataManager* dataManager = DataManager::getInstance();
	mSkillData = dataManager->searchSkillById(mSkillId);
	
	if (mSkillData)
	{
		//技能基础属性
		setSkillType(mSkillData->getIntData("sklTyp"));
		setTargetType(mSkillData->getIntData("mTarTyp"));
		setRangeType(mSkillData->getIntData("mRngTyp"));
		setDamageType(mSkillData->getIntData("dmgTyp"));
		setComboCount(mSkillData->getIntData("cmb"));
		setDamageCoe(mSkillData->getFloatData("dmgCoe"));
		setMBuffId(mSkillData->getIntData("mBufID"));
		setClearBuff(mSkillData->getIntData("clearBuff"));
		setExtraDamage(mSkillData->getIntData("dmg"));
		setBaseValueType(mSkillData->getIntData("attrType"));
		mSkillResId = mSkillData->getIntData("resId");

		int skillEffectId = mSkillData->getIntData("actionId");
		RowData* effectData = dataManager->searchSkillEffectById(skillEffectId);
		if (effectData)
		{
			//技能特效属性
			mEffectParam.nDisType = effectData->getIntData("disTyp");
			mEffectParam.nDistance = effectData->getIntData("distance");
			mEffectParam.nPreResID = effectData->getIntData("preResId");
			mEffectParam.nPreType = effectData->getIntData("preTyp");
			mEffectParam.nPreOrderType = effectData->getIntData("preOrdTyp");
			mEffectParam.nActResID = effectData->getIntData("actResId");
			mEffectParam.nActOrderType = effectData->getIntData("actTyp");
			mEffectParam.nPreTagTyp = effectData->getIntData("preTagTyp");
			mEffectParam.actionName = effectData->getStringData("aniId");
			mEffectParam.nMoveType = effectData->getIntData("moveTyp");
			mEffectParam.preName = effectData->getStringData("preAni");
			mEffectParam.actName = effectData->getStringData("actAni");
		}
		else
		{
			CCLOG("AbstractSkill::init() Not Found SkillEffectId: skillId=%d, effectId=%d", skillId, skillEffectId);
			return false;
		}

		return true;
	}
	else
	{
		CCLOG("AbstractSkill::init() Not Found SkillId: id=%d", mSkillId);
		return false;
	}
}

SKILL_EFFECT* AbstractSkill::getSkillEffectParam()
{
	return &mEffectParam;
}

SkillResultParam* AbstractSkill::getSkillResultParam()
{
	return &mResultParam;
}

//攻击前判定
bool AbstractSkill::onJudgeStart()
{
	if (mAttacker && mAttacker->isBanAll())
	{
		return false;
	}
	return true;
}

//目标选定阶段
void AbstractSkill::onTargetSelect()
{
	//清空目标队列
	clearResultParam();

	//设置攻击目标方
	mResultParam.mTargetSide = getTargetSide(getTargetType());

	//设置目标选择方
	mResultParam.mVecTargetIndex = onTargetSelect(mResultParam.mTargetSide, getRangeType());
}

vector<int> AbstractSkill::onTargetSelect(FightStateMachine::E_SIDE targetSide, int rangeType)
{
	vector<int> vecTargetIndex;

	//目标判定
	switch (rangeType)
	{
	case SINGLE:		//单体攻击
		vecTargetIndex = selectSingleTarget(targetSide);
		break;
	case RANDOM_1:		//随机1个
		vecTargetIndex = selectRandomTarget(targetSide, 1);
		break;
	case RANDOM_2:		//随机2个
		vecTargetIndex = selectRandomTarget(targetSide, 2);
		break;
	case RANDOM_3:		//随机3个
		vecTargetIndex = selectRandomTarget(targetSide, 3);
		break;
	case RANDOM_4:		//随机4个
		vecTargetIndex = selectRandomTarget(targetSide, 4);
		break;
	case HORIZONTAL:	//逻辑横排攻击
		vecTargetIndex = selectRowTarget(targetSide, rangeType);
		break;
	case HORIZONTAL_2:	//第二排
		vecTargetIndex = selectRowTarget(targetSide, rangeType);
		break;
	case HORIZONTAL_3:	//第三排
		vecTargetIndex = selectRowTarget(targetSide, rangeType);
		break;
	case VERTICAL:		//逻辑垂直攻击
		vecTargetIndex = selectColTarget(targetSide);
		break;
	case MAX_HP:		//最高血量攻击
		vecTargetIndex = selectMaxHpTarget(targetSide);
		break;
	case MIN_HP:		//最低血量攻击
		vecTargetIndex = selectMinHpTarget(targetSide);
		break;
	case ALL:					//全体攻击
	case TAGET_HERO_FIRST:		//所有先手英雄
	case TAGET_HERO_ATTACK:		//所有攻击英雄
	case TAGET_HERO_DEFENSE:	//所有防御英雄
	case TAGET_HERO_SUPPORT:	//所有辅助英雄
	case TAGET_HERO_MALE:		//所有男性英雄
	case TAGET_HERO_FEMALE:		//所有女性英雄
		vecTargetIndex = selectAllTarget(targetSide, rangeType);
		break;
	case MYSELF:		//自身
		vecTargetIndex = selectMySelfTarget();
		break;
	case MIN_HP_PERCENT:	//最低血量百分比
		vecTargetIndex = selectMinPercentTarget(targetSide);
		break;
	case MIN_HP_2:			//两个最低血量
		vecTargetIndex = selectMinHpTarget(targetSide, 2);
		break;
	case MIN_HP_PERCENT_2:	//两个最低血量百分比
		vecTargetIndex = selectMinPercentTarget(targetSide, 2);
		break;
	case MIN_HP_3:			//三个最低血量
		vecTargetIndex = selectMinHpTarget(targetSide, 3);
		break;
	case MIN_HP_PERCENT_3:	//三个最低血量百分比
		vecTargetIndex = selectMinPercentTarget(targetSide, 3);
		break;
	default:
		break;
	}

	return vecTargetIndex;
}

void AbstractSkill::onAttack()
{
	//获取攻击者角色数据
	Fighter* attacker = mAttacker;
	if (!attacker) return;

	for (int i = 0; i < mResultParam.mVecTargetIndex.size(); i++)
	{
		int targetId = mResultParam.mVecTargetIndex[i];
		Fighter* target = mFightModel->getAliveFighter(mResultParam.mTargetSide, targetId);

		HarmParam harmParam;
		harmParam.mTargetIndex = targetId;
		harmParam.mTargetSide = mResultParam.mTargetSide;

		clearFighterBuff(target);

		if (getDamageType() == DAMAGE_TYPE::HARM)
		{
			//最终闪避率 = MIN( 0.38，1 + 防守方闪避率 - 攻击方命中率)
			int dodge = (int)USER_MAX(0, (USER_MIN(1.0f + target->getFinalDodge() - attacker->getFinalHitPrb(), DODGE_MAX) * SKILL_ACCURACY + 0.5f)); // 四舍五入取整，防止精度误差
			//最终暴击率  = MAX( 0， 攻击方暴击率 - 防守方抗爆率)
			int crit = (int)(USER_MAX(attacker->getFinalCrit() - target->getFinalCritRes(), 0) * SKILL_ACCURACY + 0.5f); //四舍五入取整，防止精度误差
			//当闪避几率与暴击几率加和大于100%时，削减暴击概率
			if (dodge + crit > SKILL_ACCURACY)
			{
				crit = SKILL_ACCURACY - dodge;
			}
			
			//生成随机数
			int randNum = CCRANDOM_0_1() * SKILL_ACCURACY;

			CCLOG("=====================================");
			CCLOG("targetDodge=%f, attackerHit=%f", target->getFinalDodge(), attacker->getFinalHitPrb());
			CCLOG("finalDodge=%d, finalCrit=%d, randNum=%d", dodge, crit, randNum);
			CCLOG("=====================================");

			//判定闪避、暴击、普通
			if (randNum < dodge)
			{
				//闪避
				harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_DODGE;
				harmParam.mValue = 0;
				harmParam.mComboCount = getComboCount(); //需要判定combo有值
				target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);
			}
			else
			{
				//基础伤害 = IF(攻方攻击力 - 防方防御 > 0，攻方攻击力 - 防方防御 / 2 ，攻方攻击力 * 攻方攻击力/防方防御  * 0.5)
				float baseHarm = 0.0f;
				int attackerAtk = attacker->getFinalAtk();
				bool isPhyAtk = attacker->getAtkType() == Fighter::ATTACK_TYPE::ATTACK_PHYSICAL;
				int targetDef = isPhyAtk ? target->getFinalPDef() : target->getFinalMDef();
				if (attackerAtk - targetDef > 0)
				{
					baseHarm = attackerAtk - targetDef / DEFEND_MAX_COE;
				}
				else
				{
					baseHarm = attackerAtk * (attackerAtk / (float) targetDef) * DEFEND_MIN_COE;
				}

				//伤害 = （基础伤害 * MAX (0，（1+伤害加成 -伤害减免））) * 技能伤害系数 *英雄类型伤害系数*阵型伤害系数 * RANDBETWEEN(0.9，1.1)）+ 技能固定伤害+1
				float finalHarm = baseHarm * USER_MAX(0, 1 + attacker->getFinalDamCoe() - target->getFinalDamRes());
				
				//技能伤害系数
				onSkillEffect(target, harmParam, finalHarm);

				//英雄类型伤害系数
				if (isRestriction(attacker->getTagType(), target->getTagType()))
				{
					harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_RESTRICTION;
					finalHarm *= RESTRICTION_COE; //乘以属性相克系数
				}

				//浮动系数
				finalHarm *= FLOAT_COE_MIN + CCRANDOM_0_1() * (FLOAT_COE_MAX - FLOAT_COE_MIN);

				//加上技能额外伤害
				finalHarm += getExtraDamage();
				
				if (randNum >= dodge && randNum < crit + dodge)
				{
					//暴击，乘以暴击伤害系数
					harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_CRITICAL;
					finalHarm *= attacker->getFinalCritCoe();
				}
				
				int intFinalHarm = (int) finalHarm;

				//伤害修正（霸体等）
				onHarmModification(target, intFinalHarm);

				//伤害值修正，最低伤害为combo数，并取最终值的负数
				intFinalHarm = -USER_MAX(intFinalHarm, harmParam.mComboCount);
				harmParam.mValue = intFinalHarm;

				//攻击目标为无敌，则伤害为0
				if (target->isInvincible())
				{
					harmParam.mValue = 0;
					harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_INVINCIBLE;
				}
				else
				{
					//是否免疫物理攻击
					if (target->isPhyResist() && isPhyAtk)
					{
						harmParam.mValue = 0;
						harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_INVINCIBLE;
					}
					else if (target->isMagResist() && !isPhyAtk)
					{
						harmParam.mValue = 0;
						harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_INVINCIBLE;
					}
				}

				//触发攻击或者被击事件
				onAttack(attacker, target, harmParam.mValue);
				onBeaten(attacker, target, harmParam.mValue);

				//执行伤害
				target->onHarm(harmParam.mValue);

				//给目标附加Buff
				doBuff(getMBuffId(), getSkillResultParam(), target, attacker);
			}

		}
		else if (getDamageType() == DAMAGE_TYPE::CURE) 
		{
			float harm = getBaseValueType() == BASE_VALUE_TYPE_ATK ? attacker->getFinalAtk() : target->getMaxHP();

			//乘以技能系数
			harm *= getDamageCoe();

			//伤害 * 浮动系数
			float floatCoe = FLOAT_COE_MIN + CCRANDOM_0_1() * (FLOAT_COE_MAX - FLOAT_COE_MIN);
			harm *= floatCoe;

			//额外加成
			harm += getExtraDamage();

			harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_CURE;
			harmParam.mValue = harm;
			harmParam.mComboCount = getComboCount();
			target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);

			target->onHarm(harmParam.mValue);

			doBuff(getMBuffId(), getSkillResultParam(), target, attacker);

		}
		if (getDamageType() == DAMAGE_TYPE::NONE_DAMAGE)
		{
			//未直接造成伤害，不做伤害数值计算
			harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_NONE;
			harmParam.mValue = 0;
			harmParam.mComboCount = 1;
			target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);

			doBuff(getMBuffId(), getSkillResultParam(), target, attacker);
		}

		//统计伤害计算结果
		mResultParam.mVecTargetHarm.push_back(harmParam);
	}
}

void AbstractSkill::doBuff(int buffId, SkillResultParam* resultParam, Fighter* target, Fighter* attacker)
{
	if (buffId >= 0)
	{
		AbstractBuff* buff = AbstractBuff::create(buffId);
		//buff有触发概率
		if (buff && IS_HIT_CHANCE(buff->getChance()))
		{
			BuffUpdateParam param = buff->addToTarget(target);
			addBuffUpdateInfo(resultParam, target, param);
			buff->setAttacker(attacker);
		}
	}
}

void AbstractSkill::clearFighterBuff(Fighter* target, bool clearDebuff)
{
	if (getClearBuff() == 1)
	{
		Vector<AbstractBuff*>* vecBuff = target->getVecBuff();

		for (size_t i = 0; i < vecBuff->size(); i++)
		{
			AbstractBuff* buff = vecBuff->at(i);
			if (buff)
			{
				if (clearDebuff)
				{
					if (buff->isDebuff())
					{
						BuffInfo info = target->removeBuff(buff);
						addBuffUpdateInfo(getSkillResultParam(), target, info, false);
					}
				}
				else
				{
					BuffInfo info = target->removeBuff(buff);
					addBuffUpdateInfo(getSkillResultParam(), target, info, false);
				}
			}
		}
	}
}

void AbstractSkill::addBuffUpdateInfo(SkillResultParam* resultParam, Fighter* target, BuffInfo& buffInfo, bool isAdd)
{
	map<int, BuffUpdateParam>& mapBuffUpdateParam = (target->getSide() == FightStateMachine::E_SIDE::MY_SIDE) 
		? resultParam->mMapMyBuffUpdateParam : resultParam->mMapEnemyBuffUpdateParam;

	auto iter = mapBuffUpdateParam.find(target->getIndex());
	if(iter == mapBuffUpdateParam.end())
	{
		BuffUpdateParam buffUpdateParam;
		if (isAdd)
		{
			buffUpdateParam.mVecAddBuff.push_back(buffInfo);
		}
		else
		{
			buffUpdateParam.mVecRemoveBuff.push_back(buffInfo);
		}
		mapBuffUpdateParam.insert(pair<int, BuffUpdateParam>(target->getIndex(), buffUpdateParam));
	}
	else
	{
		BuffUpdateParam& buffUpdateParam = iter->second;

		if (isAdd)
		{
			buffUpdateParam.mVecAddBuff.push_back(buffInfo);
		}
		else
		{
			buffUpdateParam.mVecRemoveBuff.push_back(buffInfo);
		}
	}
}

void AbstractSkill::addBuffUpdateInfo(SkillResultParam* resultParam, Fighter* target, BuffUpdateParam& buffUpdateParam)
{
	for (size_t i = 0; i < buffUpdateParam.mVecAddBuff.size(); i++)
	{
		BuffInfo& buffInfo = buffUpdateParam.mVecAddBuff.at(i);
		addBuffUpdateInfo(resultParam, target, buffInfo, true);
	}

	for (size_t i = 0; i < buffUpdateParam.mVecRemoveBuff.size(); i++)
	{
		BuffInfo& buffInfo = buffUpdateParam.mVecRemoveBuff.at(i);
		addBuffUpdateInfo(resultParam, target, buffInfo, false);
	}
}

void AbstractSkill::onSkillEffect(Fighter* target, HarmParam& harmParam, float& harm)
{
	//技能系数以及连击数
	harmParam.mComboCount = getComboCount();
	harm *= getDamageCoe();
}

//攻击者触发的攻击事件
void AbstractSkill::onAttack(Fighter* attacker, Fighter* target, int harm)
{
	//默认不进行处理
}

//被攻击者触发的被击时间
void AbstractSkill::onBeaten(Fighter* attacker, Fighter* target, int harm)
{
	//默认不进行处理
}

void AbstractSkill::onHarmModification(Fighter* target, int& harm)
{
	//默认不进行处理
}

void AbstractSkill::onJudgeEnd()
{

}

FightStateMachine::E_SIDE AbstractSkill::getTargetSide(int targetType)
{
	FightStateMachine::E_SIDE side;

	if (targetType == TARGET_TYPE::MINE)	  //作用目标为我方队友
	{
		side = mResultParam.mAttackSide;
	}
	else if(targetType == TARGET_TYPE::ENEMY) //作用目标为敌方
	{
		side = (mResultParam.mAttackSide == FightStateMachine::E_SIDE::MY_SIDE)
			? FightStateMachine::E_SIDE::ENEMY_SIDE
			: FightStateMachine::E_SIDE::MY_SIDE;
	}
	return side;
}

//选择单体目标
vector<int> AbstractSkill::selectSingleTarget(FightStateMachine::E_SIDE targetSide)
{
	vector<int> vecTargetIndex;
	if (mResultParam.mAttackerIndex >= 0)
	{
		//我方站位
		int row = TO_POS_ROW(mResultParam.mAttackerIndex);
		int col = TO_POS_COL(mResultParam.mAttackerIndex);

		//逐列判定，每列先判定是否有和攻击者同排角色，如果没有，序号最小的优先
		for (int c = 0; c < COL_COUNT; c++)
		{
			//先判定是否有和攻击者同排角色
			int cIndex = TO_INDEX(row, c);
			if (mFightModel->getAliveFighter(targetSide, cIndex))
			{
				vecTargetIndex.push_back(cIndex);
				return vecTargetIndex;
			}

			//如果没有，序号最小的优先
			for (int r = 0; r < ROW_COUNT; r++)
			{
				int rIndex = TO_INDEX(r, c);
				if (r != row && mFightModel->getAliveFighter(targetSide, rIndex))
				{
					vecTargetIndex.push_back(rIndex);
					return vecTargetIndex;
				}
			}
		}
	}
	return vecTargetIndex;
}

//随机目标
vector<int> AbstractSkill::selectRandomTarget(FightStateMachine::E_SIDE targetSide, int count)
{
	vector<int> vecTargetIndex;

	//统计存活目标
	vector<int> vecAliveTarget;
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* fighter = mFightModel->getAliveFighter(targetSide, i);
		if (fighter)
		{
			vecAliveTarget.push_back(i);
		}
	}

	//存活目标数量小于随机数量，直接返回
	if (vecAliveTarget.size() <= count)
	{
		vecTargetIndex = vecAliveTarget;
	}
	else
	{
		while (vecTargetIndex.size() < count)
		{
			int randIndex = rand() % vecAliveTarget.size();
			vecTargetIndex.push_back(vecAliveTarget.at(randIndex));
			vecAliveTarget.erase(vecAliveTarget.begin() + randIndex);
		}
	}

	return vecTargetIndex;
}

//全体攻击
vector<int> AbstractSkill::selectAllTarget(FightStateMachine::E_SIDE targetSide, int rangeType)
{
	vector<int> vecTargetIndex;

	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* fighter = FightModel::getInstance()->getAliveFighter(targetSide, i);
		if (fighter)
		{
			//筛选符合条件的英雄
			bool cond = true;
			switch (rangeType)
			{
			case ALL:					//所有英雄
				break;
			case TAGET_HERO_FIRST:		//所有先手英雄
				cond = fighter->getJobTyp() == Fighter::FIGHTER_JOB_TYPE::FIGHTER_JOB_FIRST;
				break;
			case TAGET_HERO_ATTACK:		//所有攻击英雄
				cond = fighter->getJobTyp() == Fighter::FIGHTER_JOB_TYPE::FIGHTER_JOB_ATK;
				break;
			case TAGET_HERO_DEFENSE:	//所有防御英雄
				cond = fighter->getJobTyp() == Fighter::FIGHTER_JOB_TYPE::FIGHTER_JOB_DEFENSE;
				break;
			case TAGET_HERO_SUPPORT:	//所有辅助英雄
				cond = fighter->getJobTyp() == Fighter::FIGHTER_JOB_TYPE::FIGHTER_JOB_HELP;
				break;
			case TAGET_HERO_MALE:		//所有男性英雄
				cond = fighter->getSexTyp() == Fighter::FIGHTER_SEX_TYPE::FIGHTER_SEX_MALE;
				break;
			case TAGET_HERO_FEMALE:		//所有女性英雄
				cond = fighter->getSexTyp() == Fighter::FIGHTER_SEX_TYPE::FIGHTER_SEX_FEMALE;
				break;
			default:
				break;
			}

			if (cond)
			{
				vecTargetIndex.push_back(i);
			}
		}
	}
	
	return vecTargetIndex;
}

vector<int> AbstractSkill::selectMySelfTarget()
{
	vector<int> vecTargetIndex;
	vecTargetIndex.push_back(mResultParam.mAttackerIndex);
	return vecTargetIndex;
}

//逻辑横排攻击
vector<int> AbstractSkill::selectRowTarget(FightStateMachine::E_SIDE targetSide, int rangeType)
{
	vector<int> vecTargetIndex;

	//横排攻击逻辑上为攻击一列，按列序号搜索列是否为空，不为空则返回该列目标
	for (int i = 0; i < COL_COUNT; i++)
	{
		int col = i;
		if (rangeType == HORIZONTAL_2) //攻击中排，如果中排没有可攻击角色，检查后排
		{
			col = (i + 1) % COL_COUNT;
		}
		else if(rangeType == HORIZONTAL_3) //攻击后排，如果后排没有可攻击角色，检查中排
		{
			col = 2 - i;
		}

		vector<int> tempVecTargetIndex;

		for (int j = 0; j < ROW_COUNT; j++)
		{
			int index = TO_INDEX(j, col);
			if (FightModel::getInstance()->getAliveFighter(targetSide, index))
			{
				tempVecTargetIndex.push_back(index);
			}
		}

		if (!tempVecTargetIndex.empty())
		{
			vecTargetIndex = tempVecTargetIndex;
			break;
		}
	}

	return vecTargetIndex;
}

//逻辑纵排攻击
vector<int> AbstractSkill::selectColTarget(FightStateMachine::E_SIDE targetSide)
{
	vector<int> vecTargetIndex;

	if (mAttacker)
	{
		//内部函数，查询某一行的全部目标
		auto getColTarget = [targetSide](int row)->vector<int>
		{
			vector<int> tempVecTargetIndex;
			for (int j = 0; j < COL_COUNT; j++)
			{
				int index = TO_INDEX(row, j);
				if (FightModel::getInstance()->getAliveFighter(targetSide, index))
				{
					tempVecTargetIndex.push_back(index);
				}
			}
			return tempVecTargetIndex;
		};

		//我方站位
		int row = TO_POS_ROW(mResultParam.mAttackerIndex);
		int col = TO_POS_COL(mResultParam.mAttackerIndex);

		//逐列判定，每列先判定是否有和攻击者同排角色，如果没有，序号最小的优先
		for (int c = 0; c < COL_COUNT; c++)
		{
			//先判定是否有和攻击者同排角色
			int cIndex = TO_INDEX(row, c);
			if (mFightModel->getAliveFighter(targetSide, cIndex))
			{
				return getColTarget(row);
			}

			//如果没有，序号最小的优先
			for (int r = 0; r < ROW_COUNT; r++)
			{
				int rIndex = TO_INDEX(r, c);
				if (r != row && mFightModel->getAliveFighter(targetSide, rIndex))
				{
					return getColTarget(r);
				}
			}
		}
	}

	return vecTargetIndex;
}

//最高血量百分比攻击
vector<int> AbstractSkill::selectMaxHpTarget(FightStateMachine::E_SIDE targetSide)
{
	vector<int> vecTargetIndex;

	Fighter* fighter = nullptr;
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* tempFighter = mFightModel->getAliveFighter(targetSide, i);
		if (tempFighter)
		{
			if (!fighter)
			{
				fighter = tempFighter;
			}
			else
			{
				if (tempFighter->getCurrHP() > fighter->getCurrHP())
				{
					fighter = tempFighter;
				}
			}
		}
	}

	if (fighter)
	{
		vecTargetIndex.push_back(fighter->getIndex());
	}
	
	return vecTargetIndex;
}

//最低血量百分比攻击
vector<int> AbstractSkill::selectMinHpTarget(FightStateMachine::E_SIDE targetSide, int count)
{
	vector<int> vecTargetIndex;

	vector<Fighter*> allAliveFighter;
	mFightModel->getAllAliveFighter(targetSide, allAliveFighter);

	if (!allAliveFighter.empty())
	{
		//先排序，再取血量最少的几个
		std::sort(allAliveFighter.begin(), allAliveFighter.end(), 
			[](Fighter* f1, Fighter* f2)
			{
				return f1->getCurrHP() < f2->getCurrHP();
			}
		);

		for (int i = 0; i < allAliveFighter.size() && i < count; i++)
		{
			int index = allAliveFighter.at(i)->getIndex();
			vecTargetIndex.push_back(index);
		}
	}

	return vecTargetIndex;

}

vector<int> AbstractSkill::selectMinPercentTarget(FightStateMachine::E_SIDE targetSide, int count)
{
	vector<int> vecTargetIndex;

	vector<Fighter*> allAliveFighter;
	mFightModel->getAllAliveFighter(targetSide, allAliveFighter);

	if (!allAliveFighter.empty())
	{
		//先排序，再取血量最少的几个
		std::sort(allAliveFighter.begin(), allAliveFighter.end(), 
			[](Fighter* f1, Fighter* f2)
			{
				return f1->getPercentHP() < f2->getPercentHP();
			}
		);

		for (int i = 0; i < allAliveFighter.size() && i < count; i++)
		{
			int index = allAliveFighter.at(i)->getIndex();
			vecTargetIndex.push_back(index);
		}
	}

	return vecTargetIndex;
}

bool AbstractSkill::isNormalSkill()
{
	return getSkillType() == SKILL_TYPE::NORMAL_SKILL;
}

bool AbstractSkill::isActiveSkill()
{
	return getSkillType() == SKILL_TYPE::ACTIVE_SKILL;
}

bool AbstractSkill::isRushSkill()
{
	return getSkillType() == SKILL_TYPE::RUSH_SKILL;
}

bool AbstractSkill::isBuffSkill()
{
	return getSkillType() == SKILL_TYPE::BUFF_SKILL;
}

bool AbstractSkill::isTriggerSkill()
{
	return getSkillType() == SKILL_TYPE::TRIGGER_SKILL;
}

bool AbstractSkill::isStarSkill()
{
	return getSkillType() == SKILL_TYPE::STAR_SKILL;
}

void AbstractSkill::clearResultParam()
{
	mResultParam.mVecTargetIndex.clear();
	mResultParam.mVecTargetHarm.clear();
	mResultParam.mMapMyBuffUpdateParam.clear();
	mResultParam.mMapEnemyBuffUpdateParam.clear();
}

void AbstractSkill::setAttacker(Fighter* fighter)
{
	mAttacker = fighter;
	mResultParam.mAttackerIndex = mAttacker->getIndex();
	mResultParam.mAttackSide = mAttacker->getSide();
	mResultParam.mSkillId = mSkillId;
	mResultParam.mSkillType = getSkillType();
	mResultParam.mComboCount = getComboCount();
	mResultParam.mChangeSkin = -1;
}

Fighter* AbstractSkill::getAttacker()
{
	return mAttacker;
}

int AbstractSkill::getSkillResId()
{
	return mSkillResId;
}

bool AbstractSkill::isRestriction(int attackerTag, int targetTag)
{
	//力克敏
	bool ret1 = attackerTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_POW && 
		targetTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_AGI;

	//智克力
	bool ret2 = attackerTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_INT && 
		targetTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_POW;

	//敏克智
	bool ret3 = attackerTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_AGI && 
		targetTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_INT;

	//满足其中一条即判定属性相克
	return ret1 || ret2 || ret3;

}

