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

	//������Ч
	virtual void onEnter();

protected:

	// ��ʼ�����򣬵з��������ҷ�����Ч��Ҫˮƽ��ת
	void initDirection();

	//���ü��ܲ��
	void setEffectZorder(int orderType);

	//��ȡ����λ��
	Point getEffectPosition(int orderType);

	//���ü���λ��
	void setEffectPosition(int orderType);

	//���ٸ���Ч���
	void callBackRemove(float dt);

	//�Ƴ���Ч
	void removeEffect();

protected:

	//����Ч��
	spine::SkeletonAnimation* m_pSkeAni;

	//���ڵ�
	LayerFighter* m_pParent;

	//���ܸ���Ŀ��
	UIFighter* mTargetFighter;

	//���ܲ���
	SKILL_PARAM m_effParam;

	//��Ч����
	SKILL_EFFECT m_effect;

	//�����Ƿ����
	bool mIsSkillEnd;

};

class UIPreSkillEffect : public UIAbstractSkillEffect
{

public:

	//����
	static UIPreSkillEffect* create(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd = false);

public:

	//������Ч
	virtual void onEnter();

	//��ʼ��
	virtual bool init(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd);

private:

	//��Ч�ƶ������ص�
	void onEffectMoveEnd();

	//��Ч���Ž���
	void onEffectEnd();

	//�¼�֡�ص�
	void onEffectEvent(int trackIndex, spEvent* event);

	//���ź��ü�����Ч
	void startActEffect();

};

class UIActSkillEffect : public UIAbstractSkillEffect
{

public:

	//����
	static UIActSkillEffect* create(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd = false);

public:

	//������Ч
	virtual void onEnter();

	//��ʼ��
	virtual bool init(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd);

private:

	//��Ч���Ž���
	void onEffectEnd();

	//�¼�֡�ص�
	void onEffectEvent(int trackIndex, spEvent* event);

	//�ж����һ��
	void doFinal();

};


#endif //UIAbstractSkillEffect_h__