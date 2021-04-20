#include "GuideLayer.h"
#include "Utils/ResourceUtils.h"
#include "GuideManager.h"
#include "UI/UIDef.h"


GuideLayer::GuideLayer()
	: mGuideArea(nullptr)
	, mIsForceGuide(true)
	, mIsKeyNode(false)
	, mIsHitGuideArea(false)
	, mGuideCircle(nullptr)
	, mGuidePointer(nullptr)
	, mGuideText(nullptr)
	, mStencil(nullptr)
	, mStencilSp(nullptr)
{

}

GuideLayer::~GuideLayer()
{

}

GuideLayer* GuideLayer::create(const string& resName, bool isForceGuide, bool isKeyGuide)
{
	GuideLayer* guideLayer = new GuideLayer();
	if (guideLayer && guideLayer->init(resName, isForceGuide, isKeyGuide))
	{
		guideLayer->autorelease();
		return guideLayer;
	}
	else
	{
		delete guideLayer;
		return nullptr;
	}
}

bool GuideLayer::init(const string& resName, bool isForceGuide, bool isKeyGuide)
{
	mLayoutName = resName;
	mIsForceGuide = isForceGuide;
	mIsKeyNode = isKeyGuide;


	//初始化引导画布
	string name = ResourceUtils::getGuideResPath(resName);
	auto root = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile(name.c_str()));
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	root->setSize(size);
	root->setPosition(pos);
	root->setTouchEnabled(false);
	

	//初始化引导区域
	mGuideArea = dynamic_cast<Layout*>(ui::Helper::seekWidgetByName(root, "Panel_yindaoqu"));
// 	LayerColor* layerColor = LayerColor::create(Color4B::RED);
// 	layerColor->setContentSize(mGuideArea->getContentSize());
// 	layerColor->setOpacity(200);
// 	mGuideArea->addChild(layerColor);
	
	//引导光圈
	mGuideCircle = dynamic_cast<ImageView*>(ui::Helper::seekWidgetByName(root, "img_quan"));
	
	//引导指针
	mGuidePointer = dynamic_cast<ImageView*>(ui::Helper::seekWidgetByName(root, "img_jiantou"));

	//引导文字
	mGuideText = dynamic_cast<Text*>(ui::Helper::seekWidgetByName(root, "lab_tishi"));
	if (mGuideText)
	{
		mGuideText->setFontName(FONT_FZZHENGHEI);
// 		mGuideText->enableOutline(Color4B(0x7c, 0x08, 0x08, 0xff), 1);
// 		mGuideText->setColor(Color3B::WHITE);//(0xf9, 0xf2, 0x7e));
		mGuideText->setTouchEnabled(false);
	}

	//如果光圈不存在，则不需要蒙板
	if (mGuideCircle && mGuideCircle->isVisible())
	{
		ClippingNode* clipper = ClippingNode::create();
		clipper->setInverted(true);
		clipper->setAlphaThreshold(1);

		LayerColor* layerColor = LayerColor::create(Color4B(0, 0, 0, 160));
		clipper->addChild(layerColor);

		mStencil = Sprite::create(ResourceUtils::getIconGlobalPath("img_quanquandi.png"));
		mStencil->setScale(mGuideCircle->getScale());

		Point worldPoint = mGuideCircle->convertToWorldSpaceAR(this->getPosition());
		mStencil->setPosition(worldPoint);
		clipper->setStencil(mStencil);
		this->addChild(clipper);

		mStencilSp = Sprite::create(ResourceUtils::getIconGlobalPath("img_quanquandi.png"));
		mStencilSp->setPosition(worldPoint);
		mStencilSp->setOpacity(160);
		mStencilSp->setScale(mGuideCircle->getScale());
		this->addChild(mStencilSp);
	}

	addChild(root);

	initTouch();
	playAnimation();

	return true;
}

void GuideLayer::playAnimation()
{
	//光圈动画
	if (mGuideCircle)
	{
		float scale = mGuideCircle->getScale();
		auto circleAction = RepeatForever::create(
			Sequence::create(
			ScaleTo::create(0.7f, scale * 0.88),
			ScaleTo::create(0.7f, scale * 1.12),
			nullptr
			)
		);
		mGuideCircle->runAction(circleAction);
	}

	if (mGuidePointer && mGuideCircle)
	{
		//箭头动画，箭头动画指向圆心
		Point p1 = mGuidePointer->getPosition();
		Point p2 = mGuideCircle->getPosition();
		auto pointerAction = RepeatForever::create(
			Sequence::create(
			MoveTo::create(0.7f, p1 + 0.2f * (p2 - p1)),
			MoveTo::create(0.7f, p1),
			nullptr
			)	
			);
		mGuidePointer->runAction(pointerAction);
	}

	if (mStencil && mStencilSp && mGuideCircle)
	{
		mStencil->setScale(15.0f);
		mStencil->runAction(EaseSineOut::create(ScaleTo::create(0.4f, mGuideCircle->getScale())));

		mStencilSp->setScale(15.0f);
		mStencilSp->runAction(EaseSineOut::create(ScaleTo::create(0.4f, mGuideCircle->getScale())));
	}
}

void GuideLayer::initTouch()
{
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(GuideLayer::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(GuideLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(GuideLayer::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(GuideLayer::onTouchCancelled, this);

	_touchListener = listener;
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool GuideLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	auto listener = ((EventListenerTouchOneByOne*)_touchListener);

	bool isHitGuideArea = false;
	if (mGuideArea)
	{
		//强制引导：引导区域可穿透响应，并关闭引导；非引导区域不可响应不可穿透
		//非强制引导：引导区域可穿透响应，并关闭引导；非引导区域不可穿透，但可响应并结束该引导
		Rect rect = Rect(0, 0, mGuideArea->getContentSize().width, mGuideArea->getContentSize().height);
		isHitGuideArea = rect.containsPoint(mGuideArea->convertTouchToNodeSpace(touch));
	}
	
	if (listener)
	{
		listener->setSwallowTouches(!isHitGuideArea);
	}

	return true;
}

void GuideLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
	auto listener = ((EventListenerTouchOneByOne*)_touchListener);

	bool isHitGuideArea = false;
	if (mGuideArea)
	{
		Rect rect = Rect(0, 0, mGuideArea->getContentSize().width, mGuideArea->getContentSize().height);
		isHitGuideArea = rect.containsPoint(mGuideArea->convertTouchToNodeSpace(touch));
	}

	if (listener)
	{
		listener->setSwallowTouches(!isHitGuideArea);
	}
}

void GuideLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
	bool isHitGuideArea = true;

	if (mGuideArea)
	{
		Rect rect = Rect(0, 0, mGuideArea->getContentSize().width, mGuideArea->getContentSize().height);
		isHitGuideArea = rect.containsPoint(mGuideArea->convertTouchToNodeSpace(touch));
	}

	if (isHitGuideArea || !mIsForceGuide)
	{
		removeFromParent();
		GuideManager::getInstance()->next();
	}
}

void GuideLayer::onTouchCancelled( Touch *touch, Event *unused_event )
{


}

