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
		//���ܻ�������
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
			//������Ч����
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

//����ǰ�ж�
bool AbstractSkill::onJudgeStart()
{
	if (mAttacker && mAttacker->isBanAll())
	{
		return false;
	}
	return true;
}

//Ŀ��ѡ���׶�
void AbstractSkill::onTargetSelect()
{
	//���Ŀ�����
	clearResultParam();

	//���ù���Ŀ�귽
	mResultParam.mTargetSide = getTargetSide(getTargetType());

	//����Ŀ��ѡ��
	mResultParam.mVecTargetIndex = onTargetSelect(mResultParam.mTargetSide, getRangeType());
}

vector<int> AbstractSkill::onTargetSelect(FightStateMachine::E_SIDE targetSide, int rangeType)
{
	vector<int> vecTargetIndex;

	//Ŀ���ж�
	switch (rangeType)
	{
	case SINGLE:		//���幥��
		vecTargetIndex = selectSingleTarget(targetSide);
		break;
	case RANDOM_1:		//���1��
		vecTargetIndex = selectRandomTarget(targetSide, 1);
		break;
	case RANDOM_2:		//���2��
		vecTargetIndex = selectRandomTarget(targetSide, 2);
		break;
	case RANDOM_3:		//���3��
		vecTargetIndex = selectRandomTarget(targetSide, 3);
		break;
	case RANDOM_4:		//���4��
		vecTargetIndex = selectRandomTarget(targetSide, 4);
		break;
	case HORIZONTAL:	//�߼����Ź���
		vecTargetIndex = selectRowTarget(targetSide, rangeType);
		break;
	case HORIZONTAL_2:	//�ڶ���
		vecTargetIndex = selectRowTarget(targetSide, rangeType);
		break;
	case HORIZONTAL_3:	//������
		vecTargetIndex = selectRowTarget(targetSide, rangeType);
		break;
	case VERTICAL:		//�߼���ֱ����
		vecTargetIndex = selectColTarget(targetSide);
		break;
	case MAX_HP:		//���Ѫ������
		vecTargetIndex = selectMaxHpTarget(targetSide);
		break;
	case MIN_HP:		//���Ѫ������
		vecTargetIndex = selectMinHpTarget(targetSide);
		break;
	case ALL:					//ȫ�幥��
	case TAGET_HERO_FIRST:		//��������Ӣ��
	case TAGET_HERO_ATTACK:		//���й���Ӣ��
	case TAGET_HERO_DEFENSE:	//���з���Ӣ��
	case TAGET_HERO_SUPPORT:	//���и���Ӣ��
	case TAGET_HERO_MALE:		//��������Ӣ��
	case TAGET_HERO_FEMALE:		//����Ů��Ӣ��
		vecTargetIndex = selectAllTarget(targetSide, rangeType);
		break;
	case MYSELF:		//����
		vecTargetIndex = selectMySelfTarget();
		break;
	case MIN_HP_PERCENT:	//���Ѫ���ٷֱ�
		vecTargetIndex = selectMinPercentTarget(targetSide);
		break;
	case MIN_HP_2:			//�������Ѫ��
		vecTargetIndex = selectMinHpTarget(targetSide, 2);
		break;
	case MIN_HP_PERCENT_2:	//�������Ѫ���ٷֱ�
		vecTargetIndex = selectMinPercentTarget(targetSide, 2);
		break;
	case MIN_HP_3:			//�������Ѫ��
		vecTargetIndex = selectMinHpTarget(targetSide, 3);
		break;
	case MIN_HP_PERCENT_3:	//�������Ѫ���ٷֱ�
		vecTargetIndex = selectMinPercentTarget(targetSide, 3);
		break;
	default:
		break;
	}

	return vecTargetIndex;
}

void AbstractSkill::onAttack()
{
	//��ȡ�����߽�ɫ����
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
			//���������� = MIN( 0.38��1 + ���ط������� - ������������)
			int dodge = (int)USER_MAX(0, (USER_MIN(1.0f + target->getFinalDodge() - attacker->getFinalHitPrb(), DODGE_MAX) * SKILL_ACCURACY + 0.5f)); // ��������ȡ������ֹ�������
			//���ձ�����  = MAX( 0�� ������������ - ���ط�������)
			int crit = (int)(USER_MAX(attacker->getFinalCrit() - target->getFinalCritRes(), 0) * SKILL_ACCURACY + 0.5f); //��������ȡ������ֹ�������
			//�����ܼ����뱩�����ʼӺʹ���100%ʱ��������������
			if (dodge + crit > SKILL_ACCURACY)
			{
				crit = SKILL_ACCURACY - dodge;
			}
			
			//���������
			int randNum = CCRANDOM_0_1() * SKILL_ACCURACY;

			CCLOG("=====================================");
			CCLOG("targetDodge=%f, attackerHit=%f", target->getFinalDodge(), attacker->getFinalHitPrb());
			CCLOG("finalDodge=%d, finalCrit=%d, randNum=%d", dodge, crit, randNum);
			CCLOG("=====================================");

			//�ж����ܡ���������ͨ
			if (randNum < dodge)
			{
				//����
				harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_DODGE;
				harmParam.mValue = 0;
				harmParam.mComboCount = getComboCount(); //��Ҫ�ж�combo��ֵ
				target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);
			}
			else
			{
				//�����˺� = IF(���������� - �������� > 0������������ - �������� / 2 ������������ * ����������/��������  * 0.5)
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

				//�˺� = �������˺� * MAX (0����1+�˺��ӳ� -�˺����⣩��) * �����˺�ϵ�� *Ӣ�������˺�ϵ��*�����˺�ϵ�� * RANDBETWEEN(0.9��1.1)��+ ���̶ܹ��˺�+1
				float finalHarm = baseHarm * USER_MAX(0, 1 + attacker->getFinalDamCoe() - target->getFinalDamRes());
				
				//�����˺�ϵ��
				onSkillEffect(target, harmParam, finalHarm);

				//Ӣ�������˺�ϵ��
				if (isRestriction(attacker->getTagType(), target->getTagType()))
				{
					harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_RESTRICTION;
					finalHarm *= RESTRICTION_COE; //�����������ϵ��
				}

				//����ϵ��
				finalHarm *= FLOAT_COE_MIN + CCRANDOM_0_1() * (FLOAT_COE_MAX - FLOAT_COE_MIN);

				//���ϼ��ܶ����˺�
				finalHarm += getExtraDamage();
				
				if (randNum >= dodge && randNum < crit + dodge)
				{
					//���������Ա����˺�ϵ��
					harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_CRITICAL;
					finalHarm *= attacker->getFinalCritCoe();
				}
				
				int intFinalHarm = (int) finalHarm;

				//�˺�����������ȣ�
				onHarmModification(target, intFinalHarm);

				//�˺�ֵ����������˺�Ϊcombo������ȡ����ֵ�ĸ���
				intFinalHarm = -USER_MAX(intFinalHarm, harmParam.mComboCount);
				harmParam.mValue = intFinalHarm;

				//����Ŀ��Ϊ�޵У����˺�Ϊ0
				if (target->isInvincible())
				{
					harmParam.mValue = 0;
					harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_INVINCIBLE;
				}
				else
				{
					//�Ƿ�����������
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

				//�����������߱����¼�
				onAttack(attacker, target, harmParam.mValue);
				onBeaten(attacker, target, harmParam.mValue);

				//ִ���˺�
				target->onHarm(harmParam.mValue);

				//��Ŀ�긽��Buff
				doBuff(getMBuffId(), getSkillResultParam(), target, attacker);
			}

		}
		else if (getDamageType() == DAMAGE_TYPE::CURE) 
		{
			float harm = getBaseValueType() == BASE_VALUE_TYPE_ATK ? attacker->getFinalAtk() : target->getMaxHP();

			//���Լ���ϵ��
			harm *= getDamageCoe();

			//�˺� * ����ϵ��
			float floatCoe = FLOAT_COE_MIN + CCRANDOM_0_1() * (FLOAT_COE_MAX - FLOAT_COE_MIN);
			harm *= floatCoe;

			//����ӳ�
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
			//δֱ������˺��������˺���ֵ����
			harmParam.mHarmType = HarmParam::HARM_TYPE::HARM_NONE;
			harmParam.mValue = 0;
			harmParam.mComboCount = 1;
			target->setState(AbstractSkill::RESULT_STATE::NONE_STATE);

			doBuff(getMBuffId(), getSkillResultParam(), target, attacker);
		}

		//ͳ���˺�������
		mResultParam.mVecTargetHarm.push_back(harmParam);
	}
}

void AbstractSkill::doBuff(int buffId, SkillResultParam* resultParam, Fighter* target, Fighter* attacker)
{
	if (buffId >= 0)
	{
		AbstractBuff* buff = AbstractBuff::create(buffId);
		//buff�д�������
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
	//����ϵ���Լ�������
	harmParam.mComboCount = getComboCount();
	harm *= getDamageCoe();
}

//�����ߴ����Ĺ����¼�
void AbstractSkill::onAttack(Fighter* attacker, Fighter* target, int harm)
{
	//Ĭ�ϲ����д���
}

//�������ߴ����ı���ʱ��
void AbstractSkill::onBeaten(Fighter* attacker, Fighter* target, int harm)
{
	//Ĭ�ϲ����д���
}

void AbstractSkill::onHarmModification(Fighter* target, int& harm)
{
	//Ĭ�ϲ����д���
}

void AbstractSkill::onJudgeEnd()
{

}

FightStateMachine::E_SIDE AbstractSkill::getTargetSide(int targetType)
{
	FightStateMachine::E_SIDE side;

	if (targetType == TARGET_TYPE::MINE)	  //����Ŀ��Ϊ�ҷ�����
	{
		side = mResultParam.mAttackSide;
	}
	else if(targetType == TARGET_TYPE::ENEMY) //����Ŀ��Ϊ�з�
	{
		side = (mResultParam.mAttackSide == FightStateMachine::E_SIDE::MY_SIDE)
			? FightStateMachine::E_SIDE::ENEMY_SIDE
			: FightStateMachine::E_SIDE::MY_SIDE;
	}
	return side;
}

//ѡ����Ŀ��
vector<int> AbstractSkill::selectSingleTarget(FightStateMachine::E_SIDE targetSide)
{
	vector<int> vecTargetIndex;
	if (mResultParam.mAttackerIndex >= 0)
	{
		//�ҷ�վλ
		int row = TO_POS_ROW(mResultParam.mAttackerIndex);
		int col = TO_POS_COL(mResultParam.mAttackerIndex);

		//�����ж���ÿ�����ж��Ƿ��к͹�����ͬ�Ž�ɫ�����û�У������С������
		for (int c = 0; c < COL_COUNT; c++)
		{
			//���ж��Ƿ��к͹�����ͬ�Ž�ɫ
			int cIndex = TO_INDEX(row, c);
			if (mFightModel->getAliveFighter(targetSide, cIndex))
			{
				vecTargetIndex.push_back(cIndex);
				return vecTargetIndex;
			}

			//���û�У������С������
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

//���Ŀ��
vector<int> AbstractSkill::selectRandomTarget(FightStateMachine::E_SIDE targetSide, int count)
{
	vector<int> vecTargetIndex;

	//ͳ�ƴ��Ŀ��
	vector<int> vecAliveTarget;
	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* fighter = mFightModel->getAliveFighter(targetSide, i);
		if (fighter)
		{
			vecAliveTarget.push_back(i);
		}
	}

	//���Ŀ������С�����������ֱ�ӷ���
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

//ȫ�幥��
vector<int> AbstractSkill::selectAllTarget(FightStateMachine::E_SIDE targetSide, int rangeType)
{
	vector<int> vecTargetIndex;

	for (int i = 0; i < INDEX_COUNT; i++)
	{
		Fighter* fighter = FightModel::getInstance()->getAliveFighter(targetSide, i);
		if (fighter)
		{
			//ɸѡ����������Ӣ��
			bool cond = true;
			switch (rangeType)
			{
			case ALL:					//����Ӣ��
				break;
			case TAGET_HERO_FIRST:		//��������Ӣ��
				cond = fighter->getJobTyp() == Fighter::FIGHTER_JOB_TYPE::FIGHTER_JOB_FIRST;
				break;
			case TAGET_HERO_ATTACK:		//���й���Ӣ��
				cond = fighter->getJobTyp() == Fighter::FIGHTER_JOB_TYPE::FIGHTER_JOB_ATK;
				break;
			case TAGET_HERO_DEFENSE:	//���з���Ӣ��
				cond = fighter->getJobTyp() == Fighter::FIGHTER_JOB_TYPE::FIGHTER_JOB_DEFENSE;
				break;
			case TAGET_HERO_SUPPORT:	//���и���Ӣ��
				cond = fighter->getJobTyp() == Fighter::FIGHTER_JOB_TYPE::FIGHTER_JOB_HELP;
				break;
			case TAGET_HERO_MALE:		//��������Ӣ��
				cond = fighter->getSexTyp() == Fighter::FIGHTER_SEX_TYPE::FIGHTER_SEX_MALE;
				break;
			case TAGET_HERO_FEMALE:		//����Ů��Ӣ��
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

//�߼����Ź���
vector<int> AbstractSkill::selectRowTarget(FightStateMachine::E_SIDE targetSide, int rangeType)
{
	vector<int> vecTargetIndex;

	//���Ź����߼���Ϊ����һ�У���������������Ƿ�Ϊ�գ���Ϊ���򷵻ظ���Ŀ��
	for (int i = 0; i < COL_COUNT; i++)
	{
		int col = i;
		if (rangeType == HORIZONTAL_2) //�������ţ��������û�пɹ�����ɫ��������
		{
			col = (i + 1) % COL_COUNT;
		}
		else if(rangeType == HORIZONTAL_3) //�������ţ��������û�пɹ�����ɫ���������
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

//�߼����Ź���
vector<int> AbstractSkill::selectColTarget(FightStateMachine::E_SIDE targetSide)
{
	vector<int> vecTargetIndex;

	if (mAttacker)
	{
		//�ڲ���������ѯĳһ�е�ȫ��Ŀ��
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

		//�ҷ�վλ
		int row = TO_POS_ROW(mResultParam.mAttackerIndex);
		int col = TO_POS_COL(mResultParam.mAttackerIndex);

		//�����ж���ÿ�����ж��Ƿ��к͹�����ͬ�Ž�ɫ�����û�У������С������
		for (int c = 0; c < COL_COUNT; c++)
		{
			//���ж��Ƿ��к͹�����ͬ�Ž�ɫ
			int cIndex = TO_INDEX(row, c);
			if (mFightModel->getAliveFighter(targetSide, cIndex))
			{
				return getColTarget(row);
			}

			//���û�У������С������
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

//���Ѫ���ٷֱȹ���
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

//���Ѫ���ٷֱȹ���
vector<int> AbstractSkill::selectMinHpTarget(FightStateMachine::E_SIDE targetSide, int count)
{
	vector<int> vecTargetIndex;

	vector<Fighter*> allAliveFighter;
	mFightModel->getAllAliveFighter(targetSide, allAliveFighter);

	if (!allAliveFighter.empty())
	{
		//��������ȡѪ�����ٵļ���
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
		//��������ȡѪ�����ٵļ���
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
	//������
	bool ret1 = attackerTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_POW && 
		targetTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_AGI;

	//�ǿ���
	bool ret2 = attackerTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_INT && 
		targetTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_POW;

	//������
	bool ret3 = attackerTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_AGI && 
		targetTag == Fighter::ATTACK_PROPERTY_TYPE::ATTACK_PROPERTY_INT;

	//��������һ�����ж��������
	return ret1 || ret2 || ret3;

}

