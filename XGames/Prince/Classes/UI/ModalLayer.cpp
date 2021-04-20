#include "ModalLayer.h"


ModalLayer::ModalLayer()
	: mClickEmptyDispose(false)
{

}

ModalLayer::~ModalLayer()
{
	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
}

bool ModalLayer::init()
{
	if (!LayerColor::init()) {
		return false;
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(ModalLayer::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(ModalLayer::onTouchEnded, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,this);

	this->setColor(Color3B::BLACK);
	this->setOpacity(156);
	return true;
}

void ModalLayer::show( Node* parent,int zoder )
{
	parent->addChild(this,zoder);
}

void ModalLayer::dispose()
{
	//启动定时器，在下一帧执行销毁操作，防止崩溃
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(ModalLayer::callBackDispose), this, 0, 0, 0, false);
}

void ModalLayer::callBackDispose(float dt)
{
	removeFromParent();
}

void ModalLayer::setClickEmptyDispose(bool clickEmptyDispose)
{
	mClickEmptyDispose = clickEmptyDispose;
}

bool ModalLayer::onTouchBegan( Touch *touch, Event *event )
{
	if (isVisible())
	{
		return true;
	}
	return false;
}

void ModalLayer::onTouchEnded(Touch *touch, Event *event)
{
	if (mClickEmptyDispose)
	{
		SoundUtils::playSoundEffect("dianji");

		dispose();
	}
}





