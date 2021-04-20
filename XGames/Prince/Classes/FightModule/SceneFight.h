#ifndef SceneFight_h__
#define SceneFight_h__

#include "cocos2d.h"
#include "CocosGUI.h"

#include "IFightUI.h"
#include "./Skill/AbstractSkill.h"
#include "ChapterModule/ChapterDef.h"
#include "LayerFightBack.h"
#include "../MazeModule/MazeDef.h"
#include "ChapterModule/Settlement/LayerSettlement.h"
#include "CCArmature.h"

class LayerFighter;
class LayerFightInfo;
class LayerManualSkill;
class LayerLoading;
class LayerSettlement;

class SceneFight: public cocos2d::Scene, public IFightUI
{

public:

	SceneFight();
	~SceneFight();

	static SceneFight* create(FIGHT_TYPE nFightType, int nBarrierId);

public:

	virtual void onEnter();
	virtual void onExit();

	//战斗开始回调
	virtual void onFightStart();

	//回合开始回调
	virtual void onRoundStart(vector<AssistFighterInfo>& vecAssistInfo);

	//战斗攻击回调
	virtual void onAttack(SkillResultParam* resultParam, SKILL_EFFECT* effectParam);

	//换边回调
	virtual void onChangeSide(FightStateMachine::E_SIDE currSide, bool isPvp);

	//战斗回合结束回调
	virtual void onRoundEnd(RoundParam buffRoundHarm);

	//战斗结束回调
	virtual void onFightEnd(bool isWin, int starCount);

	//战斗攻击UI结束回调
	virtual void onAttackFinish();

	//进入战场
	void onFightEnter(float dt);

	//战斗结算
	void onSettlementConfirmed(SettlementInfo info);

	//发送战斗结算消息
	void sendFightResult(bool isWin);
	//震屏事件
	void onShakeScreen();

	//获取战斗状态机
	FightStateMachine* getFSM();

	//获取战斗信息界面
	LayerFightInfo* getLayerFightInfo();

	//获取战斗背景
	LayerFightBack* getLayerFightBack();

	//获取主动技能栏
	LayerManualSkill* getLayerManualSkill();

	//获取战斗角色层
	LayerFighter* getLayerFighter();

	//设置蒙板层是否隐藏
	void setLayerMaskVisible(bool visible);

	//启动战斗状态机
	void startFSM();

	//启动战斗引导
	void startGuide();

	//暂停状态机
	void pauseFSM(float time, const function<void(void)>& callBack = nullptr);

	//场景跳转
	void jumpScene();

	//播放战斗背景音乐
	void playFightMusic();

	//上报数据
	void onBeginTalkingData();
	void onResultTalkingData(bool isEscape = false);

	//设置/获取战斗跳过标记
	void setSkip(bool skip);
	bool getSkip();

protected:

	virtual bool init(FIGHT_TYPE nFightType, int nBarrierId);

	//初始化随机数种子
	void initRandomSeed();

	//初始化战斗状态机
	void initFSM();

	//根据战斗类型不同差异化
	void initFightType();

	//处理战斗结算
	void doFightEnd();

private:

	//战斗状态机
	FightStateMachine* m_pFSM;

	//战斗角色
	LayerFighter* m_pLayerFighter;

	//战斗信息
	LayerFightInfo* m_pLayerFightInfo;

	//主动技能栏
	LayerManualSkill* m_pLayerSkill;

	//背景层
	LayerFightBack* mLayerFightBack;

	//蒙板层
	LayerColor* m_pLayerMask;

	// 战斗类型(结算用到);
	FIGHT_TYPE  m_nFightType;

	// 关卡Id(结算用到);
	int m_nBarrierId;

	// 结算页面;
	LayerSettlement* m_layerSettlement;

	//是否胜利
	bool mIsWin;
	int mStarCount;

	//追打动画
	Armature* mRushBackAni;

	//当前试练层数，数据上报时使用
	int mTrailCurrFloor;

	//战斗是否跳过
	bool mIsSkip;
};

#endif // SceneFight_h__
