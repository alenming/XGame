#include "UIAbstractSkillEffect.h"
#include "SceneFight.h"
#include "BlinkEffect.h"
#include "Utils/SchedulerController.h"
#include "Fighter.h"
#include "UIFighter.h"
#include "LayerFightInfo.h"
#include "Widget/SpineAnimation.h"
#include <ostream>


UIAbstractSkillEffect::UIAbstractSkillEffect()
	: m_pSkeAni(nullptr)
	, m_pParent(nullptr)
	, mTargetFighter(nullptr)
	, mIsSkillEnd(false)
{

}


UIAbstractSkillEffect::~UIAbstractSkillEffect()
{

}

void UIAbstractSkillEffect::onEnter()
{
	Node::onEnter();
	m_pParent = dynamic_cast<LayerFighter*>(this->getParent());
}

void UIAbstractSkillEffect::initDirection()
{
	//float scale = getScaleValue(m_effect.nPreResID);

	//攻击者的方向是敌方，则特效翻转
	if (FightStateMachine::E_SIDE::ENEMY_SIDE == m_effParam.pAttacker->getSide())
	{
		m_pSkeAni->setScale(-1.0f, 1.0f);
	}
	else
	{
		m_pSkeAni->setScale(1.0f);
	}
}

void UIAbstractSkillEffect::setEffectZorder(int orderType)
{
	int nZorder;

	switch (orderType)
	{
	case SKILL_EFFECT::ORDER_TYPE::ACT_OVER_ALL:
		nZorder = LayerFighter::ZORDER_EFFECT_TOP;
		break;
	case SKILL_EFFECT::ORDER_TYPE::ACT_UNDER_ALL:
		nZorder = LayerFighter::ZORDER_EFFECT_BOTTOM;
		break;
	case SKILL_EFFECT::ORDER_TYPE::ACT_OVER_FIGHTER:
		nZorder = m_pParent->getZorderByIndex(mTargetFighter->getSide(), mTargetFighter->getIndex()) + 10;
		break;
	case SKILL_EFFECT::ORDER_TYPE::ACT_UNDER_FIGHTER:
		nZorder = m_pParent->getZorderByIndex(mTargetFighter->getSide(), mTargetFighter->getIndex()) - 1;
		break;
	case SKILL_EFFECT::ORDER_TYPE::ACT_OVER_ATTACKER:
		nZorder = m_effParam.pAttacker->getLocalZOrder() + 10;
		break;
	default:
		CCLOG("!!error nActType in setActZorder: %d", m_effect.nActOrderType);
		return;
	}

	this->setLocalZOrder(nZorder);
}


void UIAbstractSkillEffect::setEffectPosition(int orderType)
{
	Point pos = getEffectPosition(orderType);
	this->setPosition(pos);
}

Point UIAbstractSkillEffect::getEffectPosition(int orderType)
{
	Point pos;
	switch (orderType)
	{
	case SKILL_EFFECT::ORDER_TYPE::ACT_OVER_ALL:
	case SKILL_EFFECT::ORDER_TYPE::ACT_UNDER_ALL:
		if (m_effParam.nSklType == AbstractSkill::SKILL_TYPE::ACTIVE_SKILL)
		{
			pos = m_pParent->get3dPositionByIndex(m_effParam.targetSide, 4)+Size(0, nFIHGTER_HEIGHT/2);
		}
		else
		{
			pos = m_pParent->getPositionByIndex(m_effParam.targetSide, 4)+Size(0, nFIHGTER_HEIGHT/2);
		}
		break;

	case SKILL_EFFECT::ORDER_TYPE::ACT_OVER_FIGHTER:
	case SKILL_EFFECT::ORDER_TYPE::ACT_UNDER_FIGHTER:
		if (m_effParam.nSklType == AbstractSkill::SKILL_TYPE::ACTIVE_SKILL)
		{
			pos = m_pParent->get3dPositionByIndex(mTargetFighter->getSide(),
				mTargetFighter->getIndex());
		}
		else
		{
			pos = m_pParent->getPositionByIndex(mTargetFighter->getSide(),
				mTargetFighter->getIndex());
		}
		break;
	case SKILL_EFFECT::ORDER_TYPE::ACT_OVER_ATTACKER:
		pos = m_effParam.pAttacker->getPosition();
		break;
	default:
		CCLOG("!!error nActType in setActPosition: %d", m_effect.nActOrderType);
		break;
	}
	return pos;
}

void UIAbstractSkillEffect::callBackRemove(float dt)
{
	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
	//this->removeAllChildren();
	this->removeFromParent();
}

void UIAbstractSkillEffect::removeEffect()
{
	//在下一帧移除该组件，防止崩溃
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(UIAbstractSkillEffect::callBackRemove), this, 0, 0, 0, false);
}


////////////////////////////////昏割线//////////////////////////////////////////


UIPreSkillEffect* UIPreSkillEffect::create(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd)
{
	UIPreSkillEffect* pRet = new UIPreSkillEffect();
	if (pRet != nullptr && pRet->init(effParam, effect, target, isSkillEnd))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool UIPreSkillEffect::init(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd)
{
	m_effParam = effParam;
	m_effect = effect;
	mTargetFighter = target;
	mIsSkillEnd = isSkillEnd;

	m_pSkeAni = SpineAnimation::createSkillWithResId(m_effect.nPreResID);
	this->addChild(m_pSkeAni);
	initDirection();

	return true;
}

void UIPreSkillEffect::onEnter()
{
	UIAbstractSkillEffect::onEnter();

	setEffectZorder(m_effect.nPreOrderType);

	if (SKILL_EFFECT::PRE_TYPE::PRE_MOVE_LINE == m_effect.nPreType)
	{
		//前置特效位移
		spTrackEntry* pEntry = m_pSkeAni->setAnimation(0, m_effect.preName, true);
		this->setPosition(m_pParent->getEffectStartPostion(m_effParam.pAttacker));
		FiniteTimeAction * pMotion = MoveTo::create(fTIME_MOVE, getEffectPosition(m_effect.nPreOrderType));
		CallFunc* pCallBack = CallFunc::create(CC_CALLBACK_0(UIPreSkillEffect::onEffectMoveEnd, this));
		this->runAction(Sequence::create(pMotion, pCallBack, NULL));
	}
	else if (SKILL_EFFECT::PRE_TYPE::PRE_NO_MOVE == m_effect.nPreType)
	{
		setEffectPosition(m_effect.nPreOrderType);
		spTrackEntry* pEntry = m_pSkeAni->setAnimation(0, m_effect.preName, false);
		m_pSkeAni->setTrackEventListener(pEntry, CC_CALLBACK_2(UIPreSkillEffect::onEffectEvent, this));
		m_pSkeAni->setTrackCompleteListener(pEntry, CC_CALLBACK_0(UIPreSkillEffect::onEffectEnd, this));
	}
}

void UIPreSkillEffect::onEffectMoveEnd()
{
	//触发后置特效并移除该特效
	startActEffect();
	onEffectEnd();
}

void UIPreSkillEffect::onEffectEnd()
{
	stopAllActions();
	removeEffect();

	if (mIsSkillEnd)
	{
		m_effParam.pAttacker->onSkillPreEffectEnd();
	}
}

void UIPreSkillEffect::onEffectEvent(int trackIndex, spEvent* event)
{
	if (strcmp(event->data->name, "cb_shake") == 0)
	{
		m_pParent->getSceneFight()->onShakeScreen();
	}
	else if(strcmp(event->data->name, "cb_atk") == 0)
	{
		startActEffect();
	}
	else
	{
		FightSoundEffectPlayer::playFightEffect(event->data->name);
	}
}

void UIPreSkillEffect::startActEffect()
{
	m_effParam.nComboCount += 1;
	//判断是否有后置特效
	if (m_effect.isHaveActEffect())
	{
		//判断前置特效是单个还是多个
		if (m_effect.nPreTagTyp == SKILL_EFFECT::PRE_TAG_TYPE::SINGLE)
		{
			//单个前置特效释放多个后置特效
			for (size_t i = 0; i < m_effParam.vecpTarget.size(); i++)
			{
				UIFighter* fighter = m_effParam.vecpTarget.at(i);
				bool isSkillEnd = (i == m_effParam.vecpTarget.size() - 1) && m_effParam.isFinalCombo() && mIsSkillEnd;
				UIActSkillEffect* ackEffect = UIActSkillEffect::create(m_effParam, m_effect, fighter, isSkillEnd);
				m_pParent->addChild(ackEffect);
			}
		}
		else
		{
			//多个前置特效每个特效单独释放后置特效
			UIActSkillEffect* ackEffect = UIActSkillEffect::create(m_effParam, m_effect, mTargetFighter, mIsSkillEnd);
			m_pParent->addChild(ackEffect);
		}
	}
}

////////////////////////////////昏割线//////////////////////////////////////////


UIActSkillEffect* UIActSkillEffect::create( const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkilEnd )
{
	UIActSkillEffect* pRet = new UIActSkillEffect();
	if (pRet != nullptr && pRet->init(effParam, effect, target, isSkilEnd))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool UIActSkillEffect::init(const SKILL_PARAM& effParam, SKILL_EFFECT& effect, UIFighter* target, bool isSkillEnd)
{
	m_effParam = effParam;
	m_effect = effect;
	mTargetFighter = target;
	mIsSkillEnd = isSkillEnd;

	//闪避不触发后置特效，直接显示闪避动作并显示闪避文字
	HarmParam* harmParam = m_effParam.getHarmParamForIndex(mTargetFighter->getIndex());
	if (harmParam->mHarmType == HarmParam::HARM_TYPE::HARM_DODGE)
	{
		mTargetFighter->startMotionDodge();
		mTargetFighter->startActionHit(harmParam);
		doFinal();
		removeEffect();
	}
	else
	{
		m_pSkeAni = SpineAnimation::createSkillWithResId(m_effect.nActResID);
		this->addChild(m_pSkeAni); 
		initDirection();
	}

	return true;
}

void UIActSkillEffect::onEnter()
{
	UIAbstractSkillEffect::onEnter();

	if (m_pSkeAni)
	{
		setEffectZorder(m_effect.nActOrderType);
		setEffectPosition(m_effect.nActOrderType);
		spTrackEntry* pEntry = m_pSkeAni->setAnimation(0, m_effect.actName, false);
		m_pSkeAni->setTrackEventListener(pEntry, CC_CALLBACK_2(UIActSkillEffect::onEffectEvent, this));
		m_pSkeAni->setTrackCompleteListener(pEntry, CC_CALLBACK_0(UIActSkillEffect::onEffectEnd, this));
	}
}

void UIActSkillEffect::onEffectEvent(int trackIndex, spEvent* event)
{
	if (strcmp(event->data->name, "cb_shake") == 0)
	{
		m_pParent->getSceneFight()->onShakeScreen();
	}
	else if(strcmp(event->data->name, "cb_atk") == 0)
	{
		//攻击事件
		HarmParam* harmParam = m_effParam.getHarmParamForIndex(mTargetFighter->getIndex());
		mTargetFighter->startActionHit(harmParam, m_effParam.nSklType);
		doFinal();

		if (m_effParam.nSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL)
		{
			auto blinkEffect = BlinkEffect::create();
			m_pParent->getSceneFight()->addChild(BlinkEffect::create());
			SoundUtils::playSoundEffect("kan");
		}

	}
}

void UIActSkillEffect::onEffectEnd()
{
	removeEffect();
}

void UIActSkillEffect::doFinal()
{
	//对目标单位攻击结束、处理状态附加、死亡等动作
	if (m_effParam.isFinalCombo())
	{
		mTargetFighter->startActionHitFinal(m_effParam.pAttacker, m_effParam.nSklType);
	}

	//技能特效播放结束，唤醒状态机
	if (mIsSkillEnd)
	{
		m_effParam.pAttacker->onSkillActEffectEnd();
	}
}
