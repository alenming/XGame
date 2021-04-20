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
	//调用该方法创建对象时需注意，此方法并不会调用autorelase，由于该动画播放完毕即被析构，
	//但是此时影子动画并未播放完毕，影子动画回调会崩溃，所以需等待所有影子动画播放完毕方可删除
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

	//重置所有影子的位置，保持世界坐标不变
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
	
	//每个影子播放渐隐动画
	auto fadeOut = FadeOut::create(mShadowFadeDuration);
	auto func = CallFuncN::create(CC_CALLBACK_1(ShadowAction::onRemoveCallBack, this));
	auto seq = Sequence::create(fadeOut, func, nullptr);
	shadow->runAction(seq);
}

void ShadowAction::onRemoveCallBack(Node* target)
{
	target->removeFromParent();
	mMapShadow.erase((Sprite*)target);

	//所有影子播放完毕，将自身加入回收队列
	if (mMapShadow.empty())
	{
		this->autorelease();
	}
}


