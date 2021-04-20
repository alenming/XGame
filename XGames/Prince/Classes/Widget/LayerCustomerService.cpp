#include "LayerCustomerService.h"
#include "Communication/HttpPublisher.h"

LayerCustomerService::LayerCustomerService(void)
{

}

LayerCustomerService::~LayerCustomerService(void)
{

}

bool LayerCustomerService::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void LayerCustomerService::initUI()
{
	//Size winSize = Director::getInstance()->getWinSize();
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	string str = HttpPublisher::getInstance()->getCustomerServiceSystemUrl();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		cocos2d::ui::WebView *_webView = cocos2d::ui::WebView::create();
		_webView->setPosition(pos+size*0.5);
		_webView->setContentSize(size * 0.8);
		_webView->loadURL(str);
		_webView->setScalesPageToFit(true);
		addChild(_webView);

		Button *close = Button::create("Image/Icon/global/btn_close.png");
		Vec2 _pos = Vec2(pos.x+size.width*0.5+size.width*0.4,pos.y+size.height*0.5+size.height*0.4+close->getContentSize().height/2 );
		//Vec2 _pos = pos+size*0.5+size*0.4;

		close->setPosition(_pos);
		addChild(close);

		auto closeCall = [this](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (type == Widget::TouchEventType::ENDED)
			{
				dispose();
			}
		};
		close->addTouchEventListener(closeCall);
	}
#endif
}

void LayerCustomerService::onEnter()
{
	ModalLayer::onEnter();
}

void LayerCustomerService::onExit()
{
	ModalLayer::onExit();
}