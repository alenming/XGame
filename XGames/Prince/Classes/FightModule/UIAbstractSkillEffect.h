#ifndef UIAbstractSkillEffect_h__
#define UIAbstractSkillEffect_h__

#include "cocos2d.h"
#include "spine/spine-cocos2dx.h"
#include "UIFighter.h"
#include "LayerFighter.h"
#include <vector>

using namespace cocos2d;
using namespace spine;

const char szANI_PRE[] = "pre";
const char szANI_ACT[] = "act";

const float fTIME_MOVE = 0.5f;
const float fSCALE = 0.85f;

USING_NS_CC;

class UIAbstractSkillEffect : public Node
{

public:

	UIAbstractSkillEffect();
	~UIAbstractSkillEffect();

public:

	//启动特效
	virtual void onEnter();

protected:

	// 初始化方向，敌方作用在我方的特效需要水平翻转
	void initDirection();

	//设置技能层次
	void setEffectZorder(int orderType);

	//获取技能位置
	Point getEffectPosition(int orderType);

	//设置技能位置
	void setEffectPosition(int orderType);

	//销毁该特效组件
	void callBackRemove(float dt);

	//移除特效
	void removeEffect();

protected:

	//动画效果
	spine::SkeletonAnimation* m_pSkeAni;

	//父节点
	LayerFighter* m_pParent;

	//技能附加目标
	UIFighter* mTargetFighter;

	//技能参数
	SKILL_PARAM m_effParam;

	//特效参数
	SKILL_EFFECT m_effect;

	//技能是否结束
	bool mIsSkillEnd;

};

class UIPreSkillEffect : public UIAbstractSkillEffect
{

public:

	//创建
	static UIPreSkillEffect* create(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd = false);

public:

	//播放特效
	virtual void onEnter();

	//初始化
	virtual bool init(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd);

private:

	//特效移动结束回调
	void onEffectMoveEnd();

	//特效播放结束
	void onEffectEnd();

	//事件帧回调
	void onEffectEvent(int trackIndex, spEvent* event);

	//播放后置技能特效
	void startActEffect();

};

class UIActSkillEffect : public UIAbstractSkillEffect
{

public:

	//创建
	static UIActSkillEffect* create(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd = false);

public:

	//播放特效
	virtual void onEnter();

	//初始化
	virtual bool init(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd);

private:

	//特效播放结束
	void onEffectEnd();

	//事件帧回调
	void onEffectEvent(int trackIndex, spEvent* event);

	//判定最后一击
	void doFinal();

};


#endif //UIAbstractSkillEffect_h__