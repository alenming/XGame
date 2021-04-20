#ifndef UIFighterDef_h__
#define UIFighterDef_h__

#include "Utils/SoundUtils.h"

using namespace std;

typedef struct tagSKILL_EFFECT
{
	//前置特效资源id
	int nPreResID;

	//后置特效资源id
	int nActResID;

	//攻击位置类型
	int nDisType;

	//攻击距离
	int nDistance;

	//技能前置类型
	int nPreType;

	//技能前置层次类型
	int nPreOrderType;

	//技能后置层次类型
	int nActOrderType;

	//前置技能特效附加单个目标或者多个目标
	int nPreTagTyp;

	//技能使用的人物动作名称
	string actionName;

	//技能前置特效动作名称
	string preName;

	//技能后置特效动作名称
	string actName;

	//技能移动方式、分跳跃、冲刺两种
	int nMoveType;

	enum DIS_TYPE
	{
		DIS_LONG = 1,		// 远程
		DIS_MID_ABSOLUTE,	// XY都绝对固定的中程位置
		DIS_MID_REL_ELEM,	// 相对于个体的中程位置
		DIS_ENEMY_CENTER	// 相对与敌方阵位中心位置
	};

	enum PRE_TAG_TYPE
	{
		SINGLE = 1,			//单个目标
		MULTI			//多个目标
	};

	enum PRE_TYPE
	{
		PRE_MOVE_LINE = 1,	// 平移
		PRE_MOVE_PARAB,		// 抛物线
		PRE_NO_MOVE			// 无移动
	};

	enum ORDER_TYPE
	{
		ACT_UNDER_FIGHTER = 1,	// 目标单位之上
		ACT_OVER_FIGHTER,		// 目标单位下
		ACT_OVER_ALL,			// 所有单位之上
		ACT_UNDER_ALL,			// 所有单位之下
		ACT_OVER_ATTACKER		// 发起者之上
	};

	enum MOVE_TYPE
	{
		MOVE_RUSH = 1,	//冲刺
		MOVE_JUMP		//跳跃
	};

	bool isHavePreEffect()
	{
		return nPreResID != -1;
	}

	bool isHaveActEffect()
	{
		return nActResID != -1;
	}

}SKILL_EFFECT;


class FightSoundEffectPlayer
{

public:

	static void playFightEffect(const char* eventName)
	{
		string strEvent = string(eventName);
		string patten = "cb_";
		std::string result = strEvent.substr(patten.size());
		SoundUtils::playSoundEffect(result);
	}
};

#endif // UIFighterDef_h__
