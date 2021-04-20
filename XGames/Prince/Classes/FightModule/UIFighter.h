#ifndef UIFighter_h__
#define UIFighter_h__

#include "cocos2d.h"
#include "spine/spine-cocos2dx.h"
#include "CocoStudio.h"
#include "FightStateMachine.h"
#include "UIFighterDef.h"
#include "Skill/AbstractSkill.h"
#include "Widget/SpineAnimation.h"
#include "CCArmature.h"

using namespace cocostudio;

extern const int nFIHGTER_WIDTH;
extern const int nFIHGTER_HEIGHT;

const float fARMATURE_SCALE_SIZE = 0.8f;

const char szANI_WAIT[]			= "wait";
const char szANI_BEATN[]		= "hit";
const char szANI_DEAD[]			= "dead";
const char szANI_WIN[]			= "victory";

const char FIGHTER_SKIN_1[]		= "skin1";
const char FIGHTER_SKIN_2[]		= "skin2";

class SkillResultParam;
class LayerFighter;
class UIFighter;
class UIFighterHp;
class UISkillEffect;
class UIBuffDelegate;
class HarmParam;

typedef struct tagSKILL_PARAM
{
	// 技能ID
	int nSklID;

	// 技能类型,对应enum SKILL_TYPE
	int nSklType;

	// 技能击打数
	int nComboMaxCount;

	// 技能当前击打数
	int nComboCount;

	//攻击者
	UIFighter* pAttacker;

	//切换皮肤
	int nChangeSkin;

	// 目标阵营
	FightStateMachine::E_SIDE targetSide;

	// 攻击目标,可能是多个
	std::vector<UIFighter*> vecpTarget;

	// 攻击伤害,与攻击目标对应
	std::vector<HarmParam> vecDamage;

	//吸血效果
	HarmParam bloodHarm;

	//获取伤害类型
	HarmParam* getHarmParamForIndex(int index)
	{
		for (int i = 0; i < vecDamage.size(); i++)
		{
			HarmParam& harmParam = vecDamage.at(i);
			if (harmParam.mTargetIndex == index)
			{
				return &harmParam;
			}
		}
		return nullptr;
	}

	bool isFirstCombo()
	{
		return nComboCount == 1;
	}

	//是否是最后一击
	bool isFinalCombo()
	{
		return nComboCount >= nComboMaxCount;
	}

	tagSKILL_PARAM()
	{
		vecpTarget.clear();
		vecDamage.clear();
	}
	
}SKILL_PARAM;


class UIFighter: public cocos2d::Node
{

public:

	enum ZORDER
	{
		ZORDER_BUFF_BOTTOM		= 1,
		ZORDER_EFFECT_BOTTOM,
		ZORDER_ARMATURE,
		ZORDER_HP,
		ZORDER_BUFF_TOP,
		ZORDER_EFFECT_TOP,
		ZORDER_DAMAGE
	};

	enum MOVE_SPEED_TYPE
	{
		MOVE_SPEED_UP,
		MOVE_SPEED_DOWN,
		MOVE_SPEED_NO,
	};

public:

	UIFighter();

	~UIFighter();

	static UIFighter* create( int nResID, FightStateMachine::E_SIDE side, int nIndex, bool isAssist = false );

public:

	virtual bool init( int nResID, FightStateMachine::E_SIDE side, int nIndex, bool isAssist);

	virtual void onEnter();

	virtual void onExit();

	// 从IEffectUI继承的接口
	virtual void onAttackStart(SkillResultParam* pSkill, SKILL_EFFECT* effect);

	virtual void onAttackFinish();

	//开始被击动作，nDamge大于0为加血
	virtual void startActionHit(HarmParam* harmParam, int sklTyp = -1);

	//本次攻击最后一击回调
	virtual void startActionHitFinal(UIFighter* pSponsor, int sklTyp);

	// 每回合结束加buff作用
	virtual void startBuffRoundEffect(vector<HarmParam>* vecHarm);

	//buff效果回调
	void buffRoundEffect(HarmParam* harmParam, bool isFinal);

	virtual FightStateMachine::E_SIDE getSide(){return m_enSide;}

	virtual int	getIndex(){return m_nIndex;}

	//获取骨骼动画
	spine::SkeletonAnimation* getSkeletonAnimation();

	//获取父结点
	virtual LayerFighter* getLayerFighter();

	//前置技能特效播放结束
	virtual void onSkillPreEffectEnd();

	//后置技能特效播放结束
	virtual void onSkillActEffectEnd();

	//执行闪避效果
	void startMotionDodge();

	//获取对应英雄的数据
	Fighter* getFighterData(bool isAssist = false);

	//移动效果
	void startActionJump(SKILL_EFFECT::MOVE_TYPE moveType, MOVE_SPEED_TYPE moveSpeedType, 
		bool hasShadow, bool hasEndAction, Point endPoint, float speed, int jumpHeight, 
		const function<void(void)>& callBack, bool isEndActionSpeedDown = false);

	//播放胜利动作
	void playActionWin();

	//更新buff
	void updateBuff(BuffUpdateParam* buffUpdateParam);

	//移除所有buff
	void removeAllUIBuff();

	void startMotionBack();

	//更新角色血条
	void updateHP(bool isAssist = false);

	void startActionDead();

protected:

	//移动前的暴光效果
	void startSkillFlash();

	//主动技能闪光效果
	void startActiveSkillFlash();

	//追打技能闪光效果
	void startRushSkillFlash();

	//被动3技能闪光效果
	void startP3SkillFlash();

	//星灵技能
	void startStarSpriteSkill();

	//星灵动画播放完毕回调
	void onStarSpriteSkillFlashEnd(Armature *armature, MovementEventType eventType, const std::string& strName);

	// 出手前移动
	virtual void startMotionFront();

	// 出手前移动完成的回调
	virtual void onMotionFrontEnd(Point p = Point(0, 0));

	// 出手释放技能
	virtual void startActionSkill();

	// 出手释放技能的事件帧回调
	virtual void onActionSkillEvent(int trackIndex, spEvent* event);

	// 出手释放技能的动作end回调
	virtual void onActionSkillEnd();

	//开始播放技能特效
	virtual void startSkillEffect();

	// 设置战士位置坐标，side-所属的敌我阵营，nIndex-阵营位置索引，0-8
	virtual void setPositionByIndex(FightStateMachine::E_SIDE side, int nIndex);

	// 设置战士朝向，side-所属的敌我阵营
	virtual void setDirection(FightStateMachine::E_SIDE side);

	// 更新战士移动中层次
	virtual void setMoveZorder(Point targetPos);

	// 更新战士攻击时层次
	virtual void setFightZorder();

	// 重置战士初始位置层
	virtual void setOriginZorder();

	// 重置战士初始位置
	virtual void setOriginPosition();

	// 获取初始位置
	Point getOriginPosition();

	//无伤害技能(只加buff的技能)
	virtual bool isNoHarmSkill();

	Point getDistancePosition();

	//void startMotionBack();

	void onMotionBackEnd();

	virtual void onActionDeadEnd(int trackIndex);

	virtual void playActionWait();

	// 添加伤害，一般伤害添加到LayerFighter层，反伤的伤害添加位置应和伤害添加对象位置一致，所以添加到UIFighter自身
	virtual void addDamage(int nDamage, int damageType);

	//添加吸血伤害效果
	void addBloodDamage();

	// 被击状态后回位
	virtual void onMotionReturnEnd();

	virtual void waitMoveBack();

	//最后一击执行慢动作
	void slowMotion();

	//执行慢动作回调
	void onSlowMotionReturn(float dt);

	//创建追打特效动画
	void showTargetRushFlag(AbstractSkill::RESULT_STATE state);

	Color3B getShadowColor();

	//播放追打开始叹号动画并暂停除动画外的全部动画
	void startRushAniAndPause();

	//判断位置是否相同
	bool isPointEqual(const Point& p1, const Point& p2);

protected:

	// 敌我阵营
	FightStateMachine::E_SIDE m_enSide;		

	// 位置索引0-8
	int	m_nIndex;

	// 资源ID，用于初始化UI
	int	m_nResID;

	//技能动作播放结束
	bool m_bIsSkillEnd;

	//技能前置特效播放结束
	bool m_bIsPreEffectEnd;

	//技能后置特效播放结束
	bool m_bIsActEffectEnd;

	// 英雄骨骼动画
	SpineAnimation* m_pSkelFighter;

	// HP条
	UIFighterHp* m_pBarHP;		

	// 父节点
	LayerFighter* m_pParent;

	//buff更新代理
	UIBuffDelegate* mBuffDelegate;

	// 阴影
	Sprite* mShade;

	//技能参数
	SKILL_PARAM sklParam;

	//特效参数
	SKILL_EFFECT effect;

	//追打动画
	Armature* mRushStateAni;


};

#endif // UIFighter_h__
