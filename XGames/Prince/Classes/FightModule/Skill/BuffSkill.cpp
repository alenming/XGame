#include "BuffSkill.h"

//�ü����ѷ���
BuffSkill::BuffSkill()
	: AbstractSkill()
	, mBuffSourceType(-1)
{

}

BuffSkill::~BuffSkill()
{

}

BuffSkill* BuffSkill::create(int skillId)
{
	BuffSkill* buffSkill = new BuffSkill();
	if (buffSkill && buffSkill->init(skillId))
	{
		buffSkill->autorelease();
		return buffSkill;
	}
	else
	{
		delete buffSkill;
		return nullptr;
	}
}

bool BuffSkill::init(int skillId)
{
	if (!AbstractSkill::init(skillId))
	{
		return false;
	}

	if (getSkillType() != SKILL_TYPE::BUFF_SKILL)
	{
		return false;
	}

	if (mSkillData)
	{
		setBuffSourceType(mSkillData->getIntData("orgTyp"));
	}
	return true;
}

//�����ߴ����Ĺ����¼�
void BuffSkill::onAttack(SkillResultParam* resultParam, Fighter* attacker, Fighter* target)
{
	if (getBuffSourceType() == BUFF_SOURCE_TYPE::SOURCE_ATTACK)
	{

		Fighter* buffTarget = nullptr;
		switch (getTargetType())
		{
		case TARGET_TYPE::ENEMY:
			buffTarget = target;
			break;
		case TARGET_TYPE::MINE:
			buffTarget = attacker;
			break;

		default:
			break;
		}

		//buff�ͷ���Ϊ������
		if (buffTarget && attacker)
		{
			doBuff(getMBuffId(), resultParam, buffTarget, attacker);
		}
	}

}

//�������ߴ����ı����¼�
void BuffSkill::onBeaten(SkillResultParam* resultParam, Fighter* attacker, Fighter* target)
{
	if (getBuffSourceType() == BUFF_SOURCE_TYPE::SOURCE_BEATEN)
	{

		Fighter* buffTarget = nullptr;
		switch (getTargetType())
		{
		case TARGET_TYPE::ENEMY:
			buffTarget = attacker;
			break;
		case TARGET_TYPE::MINE:
			buffTarget = target;
			break;

		default:
			break;
		}

		//buff�ͷ���Ϊ��������
		if (buffTarget && target)
		{
			doBuff(getMBuffId(), resultParam, buffTarget, target);
		}
	}
}

