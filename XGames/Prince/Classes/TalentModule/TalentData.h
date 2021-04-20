#ifndef TalentSkillData_h__
#define TalentSkillData_h__

#include <iostream>
using namespace std;


const int TALENT_SKILL_COUNT = 3;

enum class TALENT_SKILL_TYPE
{
	TALENT_MANU_SKILL,
	TALENT_NORMAL_SKILL,
	TALENT_RUSH_SKILL,
	TALENT_BUFF_SKILL,
	TALENT_TRIGGER_SKILL
};

struct TalentSkillData
{
	//数据
	int skillTypeId;			//技能类型id
	int skillId;				//技能id
	string skillName;			//技能名称
	int skillIconResId;			//技能资源Id
	string skillDes;			//技能描述
	int unlockLvl;				//解锁等级

	int skillCD;				//CD	
	int skillInCD;				//进场CD
	int skillSP;				//怒气消耗

	int skillUIindex;			//UI关联数据的索引

	bool isNeedNewOpenFlag;		//是否需要展示新开启UI

	TalentSkillData()
		: skillTypeId(0)
		, skillId(0)
		, skillName("")
		, skillIconResId(0)
		, skillDes("")
		, unlockLvl(0)
		, skillCD(0)
		, skillInCD(0)
		, skillSP(0)
		, skillUIindex(0)
		, isNeedNewOpenFlag(true) {}
};

#endif