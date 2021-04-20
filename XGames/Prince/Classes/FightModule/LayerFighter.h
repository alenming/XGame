#ifndef LayerFighter_h__
#define LayerFighter_h__

#include "cocos2d.h"
#include "spine/spine-cocos2dx.h"
#include "UIFighter.h"
#include "Skill/SkillResultParam.h"
#include "FightModel.h"

#ifndef ENUM_TO_INT
	#define ENUM_TO_INT(_enmu_) (static_cast<int>(_enmu_))
#endif

using namespace spine;

class SceneFight;

class LayerFighter: public cocos2d::Layer
{

public:

	LayerFighter();
	virtual ~LayerFighter();

	CREATE_FUNC(LayerFighter);

public:

	virtual bool init();

	virtual void onEnter();

	virtual void onExit();

	Point getEffectActPosition(FightStateMachine::E_SIDE side);

	Point getEffectStartPostion(cocos2d::Node* pNode);

	void onAttackStart(SkillResultParam* pSkill, SKILL_EFFECT* skillEffect);

	void onAttackFinish();

	//回合开始
	void onRoundStart();

	//战斗回合结束回调
	void onRoundEnd(RoundParam buffRoundHarm);

	// 获取相应index位置在层中的坐标
	Point getPositionByIndex(FightStateMachine::E_SIDE side, int nIndex);

	// 获取3d透视效果角色的位置
	Point	get3dPositionByIndex(FightStateMachine::E_SIDE side, int nIndex);

	// 获取相应index位置的UIFighter
	UIFighter* getFighterByIndex(FightStateMachine::E_SIDE side, int nIndex);

	void onFighterDead(FightStateMachine::E_SIDE side, int nIndex);

	int	getZorderByIndex(FightStateMachine::E_SIDE side, int nIndex);

	//是否是连续攻击
	bool isConsequentAttack();

	//是否开始追打
	bool isStartRushAttack();

	//是否追打结束
	bool isEndRushAttack();

	//获取父结点
	SceneFight* getSceneFight();

	//播放主动技能非相关角色移除动画
	void startMoveOutAnimation(SKILL_PARAM& skillParam);

	//主动技能播放完毕，所有角色归位
	void startMoveInAnimation(SKILL_PARAM& skillParam);

	//获取上一个攻击者
	UIFighter* getLastAttacker();

	//判定上一个技能是否是主动技或被动1
	bool isLastSkillActiveOrRush();

	//判定上一个技能是否是主动技能
	bool isLastSkillActive();

	//判定上一个技能是否是追打技能
	bool isLastSkillRush();

	//判定上一个技能和本次技能是否都是普通攻击
	bool isLastCurSkillNormal();

	//播放开场动画，战斗双方跳入动画
	void startEntranceAnimation(const function<void(void)>& callBack);

	//播放助战角色入场动画
	void startAssistEntrance(vector<AssistFighterInfo> vecAssistInfo, const function<void(void)>& callBack);

	//跳跃进场
	void startJump(float dt, UIFighter* fighter, int count);

	//战斗结束播放胜利动画
	void startWinAnimation(FightStateMachine::E_SIDE winSide);

	//更新buff
	void updateBuff(map<int, BuffUpdateParam>& myMapBuffUpdate, map<int, BuffUpdateParam>& enemyMapBuffUpdate);

	//移除所有buff效果
	void removeAllUIBuff();

	//获取上一次的追打状态
	AbstractSkill::RESULT_STATE getLastState();

	//获取上一次获得追打状态的角色
	UIFighter* getLastStateFighter();

	//获取所有存活英雄的坐标
	void getAllAliveFighterIndex(FightStateMachine::E_SIDE side, vector<pair<int, Point> >& vcAliveFighterIndex);

	//隐藏所有死亡英雄
	void setDeadFighterUnvisible();

	//更新所有角色的血条
	void updateAllFighterHpBar();

protected:

	// 初始化UIFighter
	void initUIFighter();

	// 初始化所有index位置的坐标
	void initUIPosition();

	// 初始化所有index位置的层级
	void initUIZorder();

	bool isAllFighterBack();

protected:

	SceneFight* m_pParent;
	
	// 双方阵营UIFighter数组
	UIFighter* m_pUIFighter[2][INDEX_COUNT];

	// 助阵角色UIFighter数组
	UIFighter* mAssistFighter[2][INDEX_COUNT];

	// 双方阵营所有index的坐标
	cocos2d::Point	m_pUIPosition[2][INDEX_COUNT];

	// 角色透视效果的位置坐标
	Point m3dPosition[2][INDEX_COUNT];

	// 双方阵营所有index的层级
	int	m_nUIZorder[2][INDEX_COUNT];

	// 上一次攻击者
	int m_nLastIndex;

	// 上一次攻击类型
	int m_nLastSklType;

	//上一次攻击方
	FightStateMachine::E_SIDE m_lastSide;

	// 本次攻击者
	int m_nCurrIndex;

	// 本次攻击类型
	int m_nCurrSklType;

	//本次攻击方
	FightStateMachine::E_SIDE m_currSide;

	//Buff回合效果参数
	RoundParam mBuffRoundHarm;

	//我方buff更新参数
	map<int, BuffUpdateParam> mMapMyBuffUpdateParam;

	//敌方buff更新参数
	map<int, BuffUpdateParam> mMapEnemyBuffUpdateParam;

	//记录上一个追打状态
	AbstractSkill::RESULT_STATE mLastState;

	//记录当前的追打状态
	AbstractSkill::RESULT_STATE mCurrentState;

	//带追打状态的角色
	UIFighter* mLastStateFighter;
	UIFighter* mCurrentStateFighter;

public:
	enum ZORDER
	{
		ZORDER_MASK	=0,
		ZORDER_EFFECT_BOTTOM	= 1,
		ZORDER_FIGHTER_ROW_1	= 2,
		ZORDER_FIGHTER_ROW_2	= 3,
		ZORDER_FIGHTER_ROW_3	= 4,
		ZORDER_EFFECT_TOP		= 50,
		ZORDER_DAMAGE
	};
};

#endif // LayerFighter_h__
