#include "CustomGoldTips.h"
#include "ResourceManager/ResourceLoader.h"
#include "Widget/ActionCreator.h"
#include "VIPModule/VipScene.h"

CustomGoldTips::CustomGoldTips()
{	
}

CustomGoldTips::~CustomGoldTips()
{

}

void CustomGoldTips::onEnter()
{
	ModalLayer::onEnter();
}

void CustomGoldTips::onExit()
{
	ModalLayer::onExit();
}

bool CustomGoldTips::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}
	
	initUI();
	
    return true;
}

void CustomGoldTips::initUI()
{	
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UImall/UImall_notenough.ExportJson"));
	this->addChild(m_root);	
	m_root->setSize(size);
	m_root->setPosition(pos);
	ImageView *img_di = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_root,"img_di"));
	ActionCreator::runCommDlgAction(img_di);

	Text *lab_chongzhitishi = dynamic_cast<Text*>(Helper::seekWidgetByName(m_root,"lab_chongzhitishi"));
	lab_chongzhitishi->setFontName(FONT_FZZHENGHEI);

	Button *btn_kaolv = dynamic_cast<Button*>(Helper::seekWidgetByName(m_root,"btn_kaolv"));
	auto kaolvCall = [this](Ref* sender, Widget::TouchEventType type)->void
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
	btn_kaolv->addTouchEventListener(kaolvCall);


	Button *btn_chongzhi = dynamic_cast<Button*>(Helper::seekWidgetByName(m_root,"btn_chongzhi"));
	auto chongzhiCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			this->setVisible(false); //防止vip界面重复创建 4/18/2016 by sonic
			dispose();
			Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
		}
	};
	btn_chongzhi->addTouchEventListener(chongzhiCall);
}