#ifndef FightStateMachine_h__
#define FightStateMachine_h__

#include "cocos2d.h"
#include <vector>

#define TO_POS_ROW(n)				(n % 3)
#define TO_POS_COL(n)				(n / 3)
#define TO_INDEX(row, col)		((col)*3+(row))

#define ROW_COUNT 3
#define COL_COUNT 3
#define INDEX_COUNT 9
#define MAX_ROUND_COUNT 20

USING_NS_CC;
using namespace std;

class IFightUI;
class AbstractSkill;
class FightModel;
class SkillResultParam;
class AILogic;
class StarSprite;

class FightStateMachine : public Ref
{

public:

	FightStateMachine();
	~FightStateMachine();

	//创建
	static FightStateMachine* create();

public:

	//战斗状态机标识
	enum STATE
	{
		FIGHT_START = 0,//战斗开始
		ROUND_BEFORE,	//战斗回合之前，在该阶段调用UI的onRoundStart,防止与回合前攻击发生冲突
		ROUND_START,	//回合开始
		TURN_START,		//轮次开始
		JUDGE_START,	//攻击前状态判定（攻击目标）
		TARGET_SELECT,	//目标选定
		ATTACK,			//攻击目标
		JUDGE_END,		//攻击后状态判定（攻击目标，被攻击目标）
		TURN_END,		//轮次结束
		TURN_CHANGE,	//换边出手
		ROUND_END,		//回合结束
		FIGHT_END,		//战斗结束
		WAIT			//等待
	};

	//我方和敌方标识
	enum E_SIDE
	{
		MY_SIDE,
		ENEMY_SIDE
	};

	enum WIN_CONDITION
	{
		ALL_ENEMY_DEAD = 1,
		ALL_MY_ALIVE_AND_WIN,
		ROUND_WIN_3,
		WIN_AND_MY_DEAD_LESS_THAN_OR_EQUAL_TO_1,
	};

	//状态机启动
	void start();

	//状态机暂停
	void wait();

	//状态机恢复
	void resume();

	//设置战斗UI
	void setFightUI(IFightUI* fightUI);

	//获取当前正在执行的技能
	AbstractSkill* getCurrentSkill();

	//设置当前执行的技能
	void setCurrentSkill(AbstractSkill* skill);

	//设置我方为自动战斗
	void setAutoFight(bool autoFight);

	//设置pvp战斗，pvp战斗出手逻辑与pve不同
	void setPVP(bool isPVP);

	//设置最大回合数
	void setMaxRoundCount(int maxRound);

private:

	//状态机执行
	void execute(float dt);

	//状态机执行逻辑
	void fightProcess();
	void onFightStart();
	void onRoundBefore();
	void onRoundStart();
	void onTurnStart();
	void onJudgeStart();
	void onTargetSelect();
	void onAttack();
	void onJudgeEnd();
	void onTurnEnd();
	void onTurnChange();
	void onRoundEnd();
	void onFightEnd();

	//是否全部出手
	bool isTurnsDone(E_SIDE side);

	//我方是否全部出手过
	bool isAllMyTurnsDone();

	//我方是否全部未出手过
	bool isAllMyTurnsUnDone();

	//敌方是否全部出手过
	bool isAllEnemyTurnsDone();

	//敌方是否全部未出手过
	bool isAllEnemyTurnsUnDone();

	//设置状态机当前状态
	void setCurrentState(STATE state);

	//获取状态机当前状态
	STATE getCurrentState();

	//判断阵位是否在当前回合追击过
	bool launchedRushTurn(E_SIDE side, int index);

	//标记阵位已出手
	void setRushTurn(E_SIDE side, int index);

	//判断是否发动被动1追击效果
	bool launchRushAttack();

	//判断是否发动被动3死亡连击效果
	bool launchDeadAttack();

	//判断是否发动被动3攻击前攻击效果
	bool launchBeforeAttack();

	//判断是否发动被动3战斗前攻击效果
	bool launchFightStartAttack();

	//判断是否发动被动3回合前攻击效果
	bool launchRoundStartAttack();

	//判断是否发动被动3指定回合攻击效果
	bool launchRoundAttack();

	//判断是否发动被动3回合数达到N且敌方出手前攻击
	bool launchRoundBeforeEnemyAttack();

	//判断是否发动被动3连击数技能
	bool launchHitCountAttack();

	//判断是否发动星灵攻击
	bool launchStarSpriteAttack();

	//状态机等待
	void timerWait(float dt = 1.0f);

	//定时唤醒状态机
	void timerResume(float dt);

	//回合结束统一处理Buff
	void onBuffRoundTick();

	//增加连击数
	void addHitCount(SkillResultParam* resultParam);

	//清空连击数
	void clearHitCount();

	//通知UI发起攻击
	void notifyAttack(AbstractSkill* skill);

private:

	//状态机当前状态
	STATE mCurrentState;

	//上一个状态，用于恢复状态机执行
	STATE mPreviousState;

	//胜利方
	E_SIDE mWinSide;

	//当前出手方
	E_SIDE mCurrentSide;

	//当前攻击者id
	int mAttackerIndex;

	//回合数统计
	int mRoundCount;

	//轮次数统计
	int mTurnCount;

	//我方阵位是否出手
	vector<bool> mMyTurns;

	//敌方阵位是否出手
	vector<bool> mEnemyTurns;

	//我方在该回合内是否发动过连击
	vector<bool> mMyRushTurns;

	//敌方在该回合内是否发动过连击
	vector<bool> mEnemyRushTurns;

	//战斗UI
	IFightUI* mFightUI;

	//当前出手技能
	AbstractSkill* mCurrentSkill;

	//战斗角色数据模型
	FightModel* mFightModel;

	//敌方自动战斗AI
	AILogic* mEnemyAILogic;

	//我方自动战斗AI
	AILogic* mMyAILogic;

	//是否是pvp战斗，pvp战斗轮流出手
	bool mIsPVP;

	//PVE战斗，本回合是否换边出手标记，已出手过则标记为true
	bool mIsChangeSide;

	//最大战斗回合，如果在最大回合内没有结束战斗，则强制结束战斗
	int mMaxRoundCount;
};






#endif