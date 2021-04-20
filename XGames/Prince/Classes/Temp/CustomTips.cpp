#include "CustomTips.h"
#include "ResourceManager/ResourceLoader.h"
#include "Widget/ActionCreator.h"

CustomTips::CustomTips()
{	
}

CustomTips::~CustomTips()
{
	//log("custom pop destroy");
}

void CustomTips::onEnter()
{
	Layer::onEnter();

	ActionCreator::runCommDlgAction(m_pImgBg);
}

void CustomTips::onExit()
{
	Layer::onExit();
}

void CustomTips::show(const string& strContent, const string& strBgName, const int& fontSize, const cocos2d::Color3B& fontColor)
{
	CustomTips::create(STR_UTF8(strContent.c_str()), strBgName, fontSize, fontColor);
}

void CustomTips::showUtf8(const string& strContent, const string& strBgName, const int& fontSize, const cocos2d::Color3B& fontColor)
{	
	CustomTips::create(strContent, strBgName, fontSize, fontColor);
}

CustomTips* CustomTips::create(const string& strContent, const string& strBgName, const int& fontSize, const cocos2d::Color3B& fontColor)
{
	CustomTips* pop = new CustomTips;
	if (nullptr != pop && pop->init(strContent, strBgName, fontSize, fontColor))
	{
		pop->autorelease();
		return pop;
	}
	CC_SAFE_DELETE(pop);
	return nullptr;
}

bool CustomTips::init(const string& strContent, const string& strBgName, const int& fontSize, const cocos2d::Color3B& fontColor)
{
	if (!Layer::init())
	{
		return false;
	}
	// 加载UI;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//this->setContentSize(visibleSize);
	this->setColor(Color3B(0x0, 0x0, 0x0));
	this->setOpacity(200);
	//this->setTouchEnabled(true);
	if (!initUI(strContent, strBgName, fontSize, fontColor))
	{
		return false;
	}	
    return true;
}

bool CustomTips::initUI(const string& strContent, const string& strBgName, const int& fontSize, const cocos2d::Color3B& fontColor)
{	
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();	
	
	m_pImgBg = ImageView::create(strBgName);
	float widthSize = m_pImgBg->getContentSize().width;
	m_pImgBg->setScale9Enabled(true);
	this->addChild(m_pImgBg);
	m_pImgBg->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));

	m_txtContent = Text::create(strContent, FONT_FZZHENGHEI, fontSize);
	m_txtContent->setColor(fontColor);
	if(m_txtContent->getContentSize().width > widthSize)
	{
		m_pImgBg->setContentSize(Size(m_txtContent->getContentSize().width+50, m_pImgBg->getContentSize().height+10));
	}
	m_pImgBg->addChild(m_txtContent);
	m_txtContent->setPosition(cocos2d::Vec2(m_pImgBg->getContentSize().width/2, m_pImgBg->getContentSize().height/2));
	Director::getInstance()->getRunningScene()->addChild(this, 999);
	this->setPosition(Director::getInstance()->getRunningScene()->getPosition());
	//自动关闭
	dispose();
	return true;
}

void CustomTips::dispose()
{
	//启动定时器，在下一帧执行销毁操作，防止崩溃
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(CustomTips::callBackDispose), this, 0, 0, 2, false);
}
void CustomTips::callBackDispose(float dt)
{
	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(CustomTips::callBackDispose), this);
	removeFromParent();
}
