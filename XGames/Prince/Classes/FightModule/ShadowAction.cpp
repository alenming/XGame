#include "ShadowAction.h"

ShadowAction::ShadowAction()
	: mCurrentTick(0.0f)
{

}

ShadowAction::~ShadowAction()
{
	
}

ShadowAction* ShadowAction::create(float duration, float shadowFadeDuration, float scale, bool isFlip, Point shadowAnchorPoint, Color3B color, string shadowPath)
{
	//���ø÷�����������ʱ��ע�⣬�˷������������autorelase�����ڸö���������ϼ���������
	//���Ǵ�ʱӰ�Ӷ�����δ������ϣ�Ӱ�Ӷ����ص��������������ȴ�����Ӱ�Ӷ���������Ϸ���ɾ��
	ShadowAction *ret = new ShadowAction();
	ret->initWithDuration(duration, shadowFadeDuration, scale, isFlip, shadowAnchorPoint, color, shadowPath);
	return ret;
}

bool ShadowAction::initWithDuration(float duration, float shadowFadeDuration, float scale, bool isFlip, Point shadowAnchorPoint, Color3B color, string shadowPath)
{
	if (ActionInterval::initWithDuration(duration))
	{
		mShadowPath = shadowPath;
		mShadowColor = color;
		mShadowFadeDuration = shadowFadeDuration;
		mShadowAnchorPoint = shadowAnchorPoint;
		mScale = scale;
		mFlip = isFlip;
		return true;
	}

	return false;
}

ShadowAction* ShadowAction::clone() const
{
	return ShadowAction::create(_duration, mShadowFadeDuration, mScale, mFlip, mShadowAnchorPoint, mShadowColor, mShadowPath);
}

ShadowAction* ShadowAction::reverse(void) const
{
	return ShadowAction::create(_duration, mShadowFadeDuration, mScale, mFlip, mShadowAnchorPoint, mShadowColor, mShadowPath);
}

void ShadowAction::startWithTarget(Node* target)
{
	ActionInterval::startWithTarget(target);
	mMapShadow.clear();
}

void ShadowAction::update(float time)
{
// 	if (mCurrentTick < SHADOW_TICK)
// 	{
// 		mCurrentTick += time;
// 		return;
// 	}
// 	else
// 	{
// 		mCurrentTick = 0;
// 	}

	//��������Ӱ�ӵ�λ�ã������������겻��
	for (auto iter = mMapShadow.begin(); iter != mMapShadow.end(); iter++)
	{
		Sprite* shadow = iter->first;
		Point originPoint = iter->second;
		shadow->setPosition(originPoint - _target->getPosition());
	}

	Sprite* shadow = Sprite::create(mShadowPath);
	shadow->setColor(mShadowColor);
	BlendFunc blendFunc = {GL_SRC_ALPHA, GL_ONE};
	shadow->setBlendFunc(blendFunc);
	shadow->setScale(mScale);
	shadow->setFlippedX(mFlip);
	shadow->setAnchorPoint(mShadowAnchorPoint);

	mMapShadow.insert(pair<Sprite*, Point>(shadow, _target->getPosition()));
	_target->addChild(shadow, -999);
	
	//ÿ��Ӱ�Ӳ��Ž�������
	auto fadeOut = FadeOut::create(mShadowFadeDuration);
	auto func = CallFuncN::create(CC_CALLBACK_1(ShadowAction::onRemoveCallBack, this));
	auto seq = Sequence::create(fadeOut, func, nullptr);
	shadow->runAction(seq);
}

void ShadowAction::onRemoveCallBack(Node* target)
{
	target->removeFromParent();
	mMapShadow.erase((Sprite*)target);

	//����Ӱ�Ӳ�����ϣ������������ն���
	if (mMapShadow.empty())
	{
		this->autorelease();
	}
}


