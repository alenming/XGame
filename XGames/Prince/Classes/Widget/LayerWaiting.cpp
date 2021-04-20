#include "LayerWaiting.h"

LayerWaiting* LayerWaiting::mLayerWaiting = nullptr;

LayerWaiting::LayerWaiting(void)
{

}

LayerWaiting::~LayerWaiting(void)
{
	Director::getInstance()->getScheduler()->unscheduleAllForTarget(this);
	m_ui.imgWaiting->stopAllActions();
	this->stopAllActions();
}

bool LayerWaiting::init()
{
	if (!LayerColor::initWithColor(Color4B(0x00, 0x00, 0x00, 0x55)))
	{
		return false;
	}

	initUI();

	return true;
}

void LayerWaiting::initUI()
{
	//Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	m_ui.pRoot = Layout::create();
	if (nullptr != m_ui.pRoot)
	{
		m_ui.pRoot->setContentSize(visibleSize);
		m_ui.pRoot->setPosition(pos);
		this->addChild(m_ui.pRoot);
	}
	else
	{
		return;
	}

	m_ui.imgWaiting = ImageView::create("Image/Icon/global/quan.png");
	if (nullptr != m_ui.imgWaiting)
	{
		m_ui.imgWaiting->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
		m_ui.pRoot->addChild(m_ui.imgWaiting);
		m_ui.imgWaiting->setScale(0.15f);
	}

	// 屏蔽触摸事件;
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [=]( Touch *touch, Event *event )
	{
		return true;
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,this);

	//15秒干掉菊花
	getScheduler()->schedule(schedule_selector(LayerWaiting::timerKillWaiting), this, WAITING_AUTO_DELETE_SECONDS, false);

	this->setZOrder(INT_MAX);
	
}

void LayerWaiting::timerKillWaiting(float dt)
{
	getScheduler()->unschedule(schedule_selector(LayerWaiting::timerKillWaiting), this);
	hideWaiting();
}

void LayerWaiting::onEnter()
{
	Layer::onEnter();

	ActionInterval* action = RotateBy::create(1.0f, 360);
	m_ui.imgWaiting->runAction(RepeatForever::create(action));

	this->setOpacity(0);
	this->runAction(FadeTo::create(1.5f, 0x55));
	m_ui.imgWaiting->setOpacity(0);
	m_ui.imgWaiting->runAction(FadeIn::create(1.5f));
}

void LayerWaiting::onExit()
{
	Layer::onExit();
}

void LayerWaiting::showWaiting()
{
	// 菊花出现;
	if (mLayerWaiting == nullptr)
	{
		mLayerWaiting = LayerWaiting::create();
		if (nullptr != mLayerWaiting)
		{
			Scene* runningScene = Director::getInstance()->getRunningScene();
			runningScene->addChild(mLayerWaiting,INT_MAX);
			mLayerWaiting->retain();
		}
	}
}

void LayerWaiting::hideWaiting()
{
	// 菊花消失;
	if (nullptr != mLayerWaiting)
	{
		if (1 == mLayerWaiting->getReferenceCount())
		{
			// 已被销毁;
			mLayerWaiting->release();
		}
		else
		{
			mLayerWaiting->removeFromParentAndCleanup(true);
			mLayerWaiting->release();
		}

		mLayerWaiting = nullptr;
	}
}
