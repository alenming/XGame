#include "BlinkEffect.h"

const float BLINK_INTERVAL = 0.1f;

BlinkEffect::BlinkEffect()
	: mBlack(nullptr)
	, mWhite(nullptr)
{

}

BlinkEffect::~BlinkEffect()
{

}

void BlinkEffect::onEnter()
{
	Layer::onEnter();

	mBlack = Sprite::create("Image/Fight/line_black.png");
	mBlack->setAnchorPoint(Point::ANCHOR_MIDDLE);
	mBlack->setScale(getContentSize().width / mBlack->getContentSize().width,
		getContentSize().height / mBlack->getContentSize().height);
	mBlack->setPosition(getContentSize() / 2);
	addChild(mBlack);
	
	auto seq = Sequence::create(
		Blink::create(BLINK_INTERVAL, 1),
		CallFunc::create(CC_CALLBACK_0(BlinkEffect::firstEffectCallBack, this)),
		DelayTime::create(BLINK_INTERVAL),
		nullptr);
	mBlack->runAction(seq);
}

void BlinkEffect::firstEffectCallBack()
{
	mBlack->removeFromParent();
	mBlack = nullptr;

	mWhite = Sprite::create("Image/Fight/line_white.png");
	mWhite->setAnchorPoint(Point::ANCHOR_MIDDLE);
	mWhite->setScale(getContentSize().width / mWhite->getContentSize().width,
		getContentSize().height / mWhite->getContentSize().height);
	mWhite->setPosition(getContentSize() / 2);
	addChild(mWhite);

	auto seq = Sequence::create(
		Blink::create(BLINK_INTERVAL, 1),
		CallFunc::create(CC_CALLBACK_0(BlinkEffect::secondEffectCallBack, this)),
		nullptr);
	mWhite->runAction(seq);
}

void BlinkEffect::secondEffectCallBack()
{
	this->removeFromParent();
}

vector<Ref*> BlinkEffect::getAllTargets()
{
	vector<Ref*> targets;

	if (mBlack)
	{
		targets.push_back(mBlack);
	}

	if (mWhite)
	{
		targets.push_back(mWhite);
	}

	return targets;
}

