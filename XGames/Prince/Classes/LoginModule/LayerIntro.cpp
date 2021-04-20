#include "LayerIntro.h"
#include "LayerCreateRole.h"


LayerIntro::LayerIntro(void)
	: m_btnSkip(nullptr)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	m_pVideoPlayer = nullptr;
#endif
}


LayerIntro::~LayerIntro(void)
{
}

bool LayerIntro::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void LayerIntro::initUI()
{
	// 获得设计尺寸;
	Size winSize = Director::getInstance()->getWinSize();

	// 获得当前设备显示尺寸;
	Size visibleSize = Director::getInstance()->getVisibleSize();

	// 获得当前显示尺寸在设计尺寸基础上的位置;
	Point pos = Director::getInstance()->getVisibleOrigin();

	m_pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIIntro/UIIntro.ExportJson"));
	this->addChild(m_pRoot, 2);

	m_pnlIntro = dynamic_cast<Layout*>(m_pRoot->getChildByName("Pnl_Intro"));
	m_pnlIntro->setContentSize(visibleSize);
	m_btnSkip = dynamic_cast<Button*>(m_pnlIntro->getChildByName("Btn_Skip"));
	m_btnSkip->addTouchEventListener(CC_CALLBACK_2(LayerIntro::onBtnClicked, this, Btn_Skip));
	m_btnSkip->setVisible(false);

	// 加入开场动画的视频播放;
	// cocos2d-x从3.1版本开始支持video播放，目前仅支持Android和IOS平台;
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		experimental::ui::VideoPlayer* _player = experimental::ui::VideoPlayer::create();
		if (nullptr != _player)
		{
			_player->setFileName("Video/Intro.mp4");
			Director::getInstance()->getRunningScene()->addChild(_player);
			_player->setKeepAspectRatioEnabled(true);
			_player->setFullScreenEnabled(true);
			_player->addEventListener(CC_CALLBACK_2(LayerIntro::onVideoPlayEvent, this));
			_player->play();
		}
	}
	#endif
}

void LayerIntro::onEnter()
{
	Layer::onEnter();
}

void LayerIntro::onExit()
{
	// 释放资源;
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIIntro");
	Layer::onExit();
}

void LayerIntro::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::ENDED)
	{
		if (nWidgetName == Btn_Skip)
		{
			// 进入主城;
			enterGame();
		}
	}
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void LayerIntro::onVideoPlayEvent( Ref* pSender, experimental::ui::VideoPlayer::EventType type )
{
	if (type == experimental::ui::VideoPlayer::EventType::COMPLETED
		|| type == experimental::ui::VideoPlayer::EventType::STOPPED)
	{
		// 进入主城;
		enterGame();
	}
}
#endif


void LayerIntro::enterGame()
{
	//视频播放结束，跳转到角色创建界面
	Director::getInstance()->replaceScene(LayerCreateRole::createScene());
}