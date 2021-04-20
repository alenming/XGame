#include "CustomPop.h"
#include "ResourceManager/ResourceLoader.h"

CustomPop::CustomPop()
	: m_type(CUSTOM_ID_YES)
	, m_pRoot(nullptr)
	, m_pnlPop(nullptr)
	, m_txtContent(nullptr)
	, m_btnOK(nullptr)
	, m_btnCancel(nullptr)
	, m_btnClose(nullptr)
	, m_btnRecharge(nullptr)
	, _callbackFunc(nullptr)
	, mShowPopAnimation(true)
{
}

CustomPop::~CustomPop()
{
	//log("custom pop destroy");
}

void CustomPop::onEnter()
{
	LayerColor::onEnter();

	// 动态效果;
	Action* seq = Sequence::create(/*FadeTo::create(0.05f, 200.0f),*/
								ScaleTo::create(0.05f, 1.15f),
								ScaleTo::create(0.05f, 1.1f), nullptr);
	this->runAction(seq);
}

void CustomPop::onExit()
{
	// 释放资源;
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UICustomPop");

	LayerColor::onExit();
}

void CustomPop::show(const string strContent, CustomPopCallback callbackFunc, CustomPopType type)
{
	CustomPop::create(STR_UTF8(strContent.c_str()), callbackFunc, type);
}

void CustomPop::showUtf8(const string strContent, CustomPopCallback callbackFunc, CustomPopType type)
{	
	CustomPop::create(strContent, callbackFunc, type);
}

CustomPop* CustomPop::create(const string strContent, CustomPopCallback callbackFunc, CustomPopType type)
{
	CustomPop* pop = new CustomPop;
	if (nullptr != pop && pop->init(strContent, callbackFunc, type))
	{
		pop->autorelease();
		return pop;
	}

	CC_SAFE_DELETE(pop);
	return nullptr;
}

bool CustomPop::init(const string strContent, CustomPopCallback callbackFunc, CustomPopType type)
{
	if (!LayerColor::init())
	{
		return false;
	}

	_callbackFunc = callbackFunc;
	m_type = type;

	this->setColor(Color3B(0x0, 0x0, 0x0));
	this->setOpacity(200);

	if (!initUI(strContent))
	{
		return false;
	}
	
    return true;
}

bool CustomPop::initUI(const string strContent)
{
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 加载UI;
	string strFileName;
	strFileName.assign("Image/UICustomPop/UICustomPop.ExportJson");
	m_pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_pRoot->setContentSize(winSize);
	m_pnlPop = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_pRoot, STRING(Pnl_Custom_Pop)));
	m_pnlPop->setSize(visibleSize);
	m_txtContent = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot, STRING(Lab_Text)));
	m_txtContent->setFontName(SystemUtils::getFontFileName());
	m_txtContent->setString(strContent.c_str());
	m_btnOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_pRoot, STRING(Btn_OK)));
	m_btnOK->addTouchEventListener(CC_CALLBACK_2(CustomPop::onBtnClicked, this, CustomRetType::RET_TYPE_OK));
	m_btnCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_pRoot, STRING(Btn_Cancel)));
	m_btnCancel->addTouchEventListener(CC_CALLBACK_2(CustomPop::onBtnClicked, this, CustomRetType::RET_TYPE_CANCEL));
	m_btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_pRoot, STRING(Btn_Close)));
	m_btnClose->addTouchEventListener(CC_CALLBACK_2(CustomPop::onBtnClicked, this, CustomRetType::RET_TYPE_CLOSE));
	m_btnClose->setVisible(false);

	// CANCEL;
	if (m_type == CUSTOM_YES_NO_CANCEL)
	{
		m_btnClose->setVisible(true);
	}
	// YES;
	else if (m_type == CUSTOM_ID_YES)
	{
		m_btnCancel->setVisible(false);
		m_btnOK->setPosition(Vec2(m_btnCancel->getPosition().x + (m_btnOK->getPosition().x - m_btnCancel->getPosition().x)/2,
			m_btnOK->getPosition().y));
	}
	// YES - NO (默认布局);
	else if (m_type == CUSTOM_YES_NO)
	{
	}
	// 充值;
	else if (m_type == CUSTOM_RECHARGE_NO)
	{
		m_btnOK->setVisible(false);
		m_btnRecharge = Button::create("Image/UINewtower/Icon/btn_recharge_1.png", "Image/UINewtower/Icon/btn_recharge_1.png");
		m_btnRecharge->setPosition(m_btnOK->getPosition());
		m_btnOK->getParent()->addChild(m_btnRecharge);
		m_btnRecharge->addTouchEventListener(CC_CALLBACK_2(CustomPop::onBtnClicked, this, RET_TYPE_RECHARGE));
	}
	
	

	this->addChild(m_pRoot);
	Director::getInstance()->getRunningScene()->addChild(this, 999);
	this->setPosition(Director::getInstance()->getRunningScene()->getPosition());
	return true;
}


void CustomPop::onBtnClicked(Ref* ref, Widget::TouchEventType type, CustomRetType nRetType)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		// 渐隐效果;
// 		if (mShowPopAnimation)
// 		{
// 			Action* seq = Sequence::create(FadeOut::create(0.1f),
// 				CallFuncN::create(CC_CALLBACK_1(CustomPop::onAnimationDone, this, nRetType)),
// 				nullptr);
// 			this->runAction(seq);
// 		}
// 		else
// 		{
// 			onAnimationDone(this, nRetType);
// 		}
		onAnimationDone(this, nRetType);
		
	}
	
}

void CustomPop::onAnimationDone(Node* pSender, CustomRetType type)
{
	this->setVisible(false);
	if (nullptr != _callbackFunc)
	{
		_callbackFunc(this, type);
	}
	this->scheduleUpdate();
}


void CustomPop::update(float dt)
{
	this->unscheduleUpdate();
	this->removeFromParentAndCleanup(true);
}

void CustomPop::showPopAnimation(bool isShow)
{
	mShowPopAnimation = isShow;
}