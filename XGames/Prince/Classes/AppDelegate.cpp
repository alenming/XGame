#include "AppDelegate.h"
#include "AppMacros.h"
#include "SimpleAudioEngine.h"

#include "Communication/SocketCommunicator.h"
#include "LoginModule/SceneLogin.h"
#include "LoginModule/SceneLogo.h"
#include "Utils/SystemUtils.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "SDK/SDKComponent.h"
#include "SDK/u8sdk/U8SDKData.h"
#endif

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate() 
{

}

AppDelegate::~AppDelegate()
{

}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
    }
    
    Size frameSize = glview->getFrameSize();
    
    CCLOG("ScreenSize width:%f, height:%f", frameSize.width, frameSize.height);
    
    
    // Set the design resolution
#if (SCREEN_ORIENTATION == SCREEN_ORIENTATION_LANDSCAPE)		// 横屏
	float frameScale = frameSize.width/frameSize.height;
#elif (SCREEN_ORIENTATION == SCREEN_ORIENTATION_PORTRAIT)		// 竖屏
	float frameScale = frameSize.height/frameSize.width;
#else
#error unknown target ORIENTATION!
#endif
    
	if( frameScale < 3.0f/2.0f )			// 窄屏，屏幕宽度:高度小于3:2
	{
		glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
	}
	else
	{
		glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
	}
    
    //use fixed resource
	float fScale = MIN(designResolutionRes.size.width/designResolutionSize.width, designResolutionRes.size.height/designResolutionSize.height);
	director->setContentScaleFactor(fScale);
    
	CCLOG("setContentScaleFactor fscale: %f, res.size(%f, %f), deSize(%f, %f)", fScale, designResolutionRes.size.width, designResolutionRes.size.height, designResolutionSize.width, designResolutionSize.height);
    
    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 50);

    // create a scene. it's an autorelease object
    Scene* pScene = nullptr;

	// 是否需要自有的闪屏逻辑;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	bool bEnableFlashScreen = SystemUtils::isEnableFlashScreen();
	if (bEnableFlashScreen)
	{
		pScene = SceneLogo::create();
	}
	else
	{
		pScene = SceneLogin::create();
	}
#else
    pScene = SceneLogo::create();


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (SystemUtils::getChannelId() == CHANNEL_ID_YSDK ) //YSDK有可能自动登录 先初始化SDKComponent
	{
		SDKComponent::getInstance();
	}
#endif

#endif
    
    // run
    director->runWithScene(pScene);
    
	// 实例化CommunicationCenter对象，并启动socket
    SocketCommunicator::getInstance();

	//正交投影
	//Director::sharedDirector()->setProjection(Director::Projection::_2D);
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
}
