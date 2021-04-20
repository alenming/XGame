#ifndef FightModel_h__
#define FightModel_h__

#include "cocos2d.h"
#include "FightStateMachine.h"
#include "base/CCVector.h"
#include "Utils/Observer.h"
#include "Utils/ValueCrypt.h"
#include "Fighter.h"
#include <map>
#include <vector>

USING_NS_CC;

enum SKIP_FLAG 
{
	CANNOT_SKIP = 0,
	CAN_SKIP
};

enum FIGHT_WIN_COND_TYPE
{
	DEFEAT_ALL_ENEMY = 1,	//击败全部敌人
	WIN_AND_ALIVE_N,		//胜利且存活N人
	WIN_IN_ROUND_N,			//N回合内取胜
};

enum BG_COND
{
	BG_COND_ROUND,					//回合前场景变换
	BG_COND_ROUND_BEFORE_ENEMY		//回合敌方出手前
};

//助战角色进入战场时传递的角色信息
struct AssistFighterInfo
{
	FightStateMachine::E_SIDE side;
	int index;
};

class StarSprite;

class FightModel : public Observable
{

private:

	FightModel();
	virtual ~FightModel();

public:

	//获取单实例
	static FightModel* getInstance();

	//销毁单实例
	static void destroyInstance();

	//添加我方角色
 	void addHero(Fighter* hero);

	//添加敌方角色
	void addMonster(Fighter* monster);

	//添加助战角色
	void addAssist(Fighter* fighter);

	//清空我方角色
	void clearHero();

	//清空敌方角色
	void clearMonster();

	//清空助战角色
	void clearAssist();

	//获取我方英雄
	Vector<Fighter*>* getHeroList();
	Fighter* getHeroByIndex(int n);

	//获取敌方英雄
	Vector<Fighter*>* getMonsterList();
	Fighter* getMonsterByIndex(int n);

	//获取助战列表
	Vector<Fighter*>* getAssistList();

	//获取战斗角色
	Fighter* getFighter(FightStateMachine::E_SIDE side, int nIndex);

	//获取助战角色
	Fighter* getAssistFighter(FightStateMachine::E_SIDE side, int nIndex);

	//获取存活的战斗角色
	Fighter* getAliveFighter(FightStateMachine::E_SIDE side, int nIndex);

	//获取所有存活的战斗角色
	void getAllAliveFighter(FightStateMachine::E_SIDE side, vector<Fighter*>& vecAliveFighter);

	//获取我方存活角色数量
	int getMyAliveFighterCount();

	//获取我方死亡角色数量
	int getMyDeadFighterCount();

	//清空所有角色的附加状态(浮空、击退、倒地)
	void clearAllFighterState();

	//我方或者敌方角色是否全部死亡
	bool isAllFighterDead(FightStateMachine::E_SIDE side);

	//我方或敌方角色是否全部存活
	bool isAllFighterAlive(FightStateMachine::E_SIDE side);

	//清空所有成员的OnRush标记
	void clearOnRushFlag();

	//获取当前总HP
	long long getTotalHp(FightStateMachine::E_SIDE side);

	//获取最大总HP
	long long getTotalMaxHp(FightStateMachine::E_SIDE side);

	//获取最大总SP
	int getTotalMaxSp();

	//扣减所有角色的buff持续回合
	void decreaseAllBuffRound(RoundParam& roundParam);

	//清空我方连击数
	void clearMyHitCount();

	//增加我方连击数
	void addMyHitCount(int hitCount);

	//清空敌方连击数
	void clearEnemyHitCount();

	//增加敌方连击数
	void addEnemyHitCount(int hitCount);

	//设置是否可跳过战斗
	void setSkipFlag(int skipFlag);

	//是否可跳过战斗
	bool canSkip();

	//获取当前星灵
	StarSprite* getStarSprite(FightStateMachine::E_SIDE side);

	//设置精灵
	void setStarSprite(StarSprite* starSprite, FightStateMachine::E_SIDE side);

	//buff每回合的执行效果
	void onBuffRoundTick(RoundParam& roundParam);

	//设置战斗胜利条件
	void setWinCondAndCount(int winCond, int condCount);

	//设置战斗背景资源id
	void setBgResId(int resId);

	//获取战斗背景资源id
	int getBgResId();

	//获取该场战斗我方对敌方造成的总伤害
	int getTotalDamage();

	//获取战斗中boss受到的伤害(boss必须在阵位中间)
	int getBossDamage();

	// 获取该场战斗完成度百分比(0-100);
	float getCompletionPercent();

	//添加助战角色
	void addAssistFighter(int round, vector<AssistFighterInfo>& vecAssistInfo);

	//设置/获取场景切换资源id
	void setChangeBgResId(int bgResId) { mChangeBgResId = bgResId; }
	int getChangeBgResId() { return mChangeBgResId; }

	//设置/获取场景切换条件
	void setChangeBgCond(int cond) { mChangeBgCond = cond; }
	int getChangeBgCond() { return mChangeBgCond; }

	//设置/获取场景切换回合数
	void setChangeBgRound(int round) { mChangeBgRound = round; }
	int getChangeBgRound() { return mChangeBgRound; }

	//判定我方是否先出手
	bool isMySideFirst();


	//加密属性
	ENCRYPT_INT_GETTER_SETTER(MyTotalSp, mMyTotalSp);
	ENCRYPT_INT_GETTER_SETTER(EnemyTotalSp, mEnemyTotalSp);
	ENCRYPT_INT_GETTER_SETTER(MyHitCount, mMyHitCount);
	ENCRYPT_INT_GETTER_SETTER(EnemyHitCount, mEnemyHitCount);
	ENCRYPT_INT_GETTER_SETTER(RoundCount, mRoundCount);
	ENCRYPT_INT_GETTER_SETTER(WinCond, mWinCond);
	ENCRYPT_INT_GETTER_SETTER(WinCondCount, mWinCondCount);
	ENCRYPT_INT_GETTER_SETTER(AssistEnterRound, mAssistEnterRound);
	ENCRYPT_INT_GETTER_SETTER(AssistSide, mAssistSide);
	ENCRYPT_INT_GETTER_SETTER(MyFirstValue, mMyFirstValue);
	ENCRYPT_INT_GETTER_SETTER(EnemyFirstValue, mEnemyFirstValue);
	
private:

	//单实例
	static FightModel* ms_pInstance;

	//我方英雄
	Vector<Fighter*> mVecHero;

	//敌方英雄
	Vector<Fighter*> mVecMonster;

	//数据是否更新完毕
	bool m_bIsUpdateDone;

	//我方总怒气值*
	int mMyTotalSp;

	//敌方总怒气值*
	int mEnemyTotalSp;

	//我方连击数*
	int mMyHitCount;

	//敌方连击数*
	int mEnemyHitCount;

	//当前战斗回合数*
	int mRoundCount;

	//是否可跳过
	int mSkipFlag;

	//双方星灵信息
	StarSprite* mMyStarSprite;
	StarSprite* mEnemyStarSprite;

	//胜利条件*
	int mWinCond;

	//胜利条件附带数量*
	int mWinCondCount;


	//我方先攻值
	int mMyFirstValue;
	//敌方先攻值
	int mEnemyFirstValue;

	//////////////////////////////////助战者////////////////////////////////////////

	//助战者是我方还是敌方*
	int mAssistSide;

	//助战者入场回合数*
	int mAssistEnterRound;

	//助战者
	Vector<Fighter*> mVecAssistFighter;

	//////////////////////////////////场景相关////////////////////////////////////////
	//背景资源id
	int mBgResId;

	//切换背景资源id
	int mChangeBgResId;

	//场景切换条件
	int mChangeBgCond;

	//场景切换回合数
	int mChangeBgRound;
};



#endif // FightModel_h__
