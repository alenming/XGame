#include "LayerGameBoard.h"
#include "LoginModule/LoginModel.h"
#include "Utils/SystemUtils.h"

LayerGameBoard::LayerGameBoard(void)
{

}

LayerGameBoard::~LayerGameBoard(void)
{

}

bool LayerGameBoard::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void LayerGameBoard::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UILoginnew/UILoginnew_gonggao.ExportJson"));
	this->addChild(m_root);	
	m_root->setSize(size);
	m_root->setPosition(pos);
	Button *close = dynamic_cast<Button*>(Helper::seekWidgetByName(m_root,"Button_guanbi"));
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
	Layout *webViewP = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_root,"webViewP"));
	ostringstream urlStr;
	urlStr << SystemUtils::getGAME_BOARD_URL().c_str();
	urlStr << "?sid=" << SystemUtils::getChannelId();
	string completeUrl = urlStr.str();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		cocos2d::ui::WebView *_webView = cocos2d::ui::WebView::create();
		_webView->setPosition(Vec2(webViewP->getContentSize().width/2,webViewP->getContentSize().height/2));
		_webView->setContentSize(webViewP->getContentSize());
		_webView->loadURL(completeUrl);
		_webView->setScalesPageToFit(true);
		webViewP->addChild(_webView);
	}
#endif
}

void LayerGameBoard::onEnter()
{
	ModalLayer::onEnter();
}

void LayerGameBoard::onExit()
{
	ModalLayer::onExit();
}