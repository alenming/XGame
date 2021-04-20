#include "SDKComponent.h"

#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_TT == 0)
#include "LoginModule/LoginController.h"
#include "VIPModule/VipModel.h"
#include "LoginModule/SceneLogout.h"
#endif

SDKComponent* SDKComponent::m_pInstance = nullptr;

SDKComponent::SDKComponent(void)
{
	m_logoutState = LOGOUT_S_NONE;
}


SDKComponent::~SDKComponent(void)
{
	m_logoutState = LOGOUT_S_NONE;
}

void SDKComponent::OnInitSuc()
{

}

void SDKComponent::OnLoginSuc( U8LoginResult* result )
{
#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_TT == 0)
	CCLOG("=====================OnLoginSuc");
	if (result->isSwitchAccount)
	{
		Director::getInstance()->replaceScene(SceneLogout::create());
	}
	else
	{
		LoginController::getInstance()->onSdkLoginAccountResult(result);
	}
#endif
}

void SDKComponent::OnSwitchLogin()
{
#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_TT == 0)
	CCLOG("=====================OnSwitchLogin");
	Director::getInstance()->replaceScene(SceneLogout::create());
#endif
}

void SDKComponent::OnLogout()
{
	if (m_logoutState == LOGOUT_S_NONE) {
        m_logoutState = LOGOUT_S_SDK;
    }

    if (m_logoutState == LOGOUT_S_SDK) {
#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_TT == 0)
	CCLOG("=====================OnLogout");
	Director::getInstance()->replaceScene(SceneLogout::create());
	// 注销之后直接调用登陆;
	// 此处不用调用了，在回到登陆界面之后，检查更新，之后会再次调用登陆;
	//login();
#endif
	}
	else if (m_logoutState == LOGOUT_S_IN_GAME)
    {
        m_logoutState = LOGOUT_S_NONE;
    }
}

SDKComponent* SDKComponent::getInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new SDKComponent;
		if (nullptr != m_pInstance)
		{
			U8SDKInterface::getInstance()->initSDK(m_pInstance);
		}
	}

	return m_pInstance;
}

void SDKComponent::releaseInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void SDKComponent::login()
{
	CCLOG("++++++++++ SDKComponent::login");
	U8SDKInterface::getInstance()->login();
}

void SDKComponent::pay( U8PayParams * data )
{
	if (nullptr == data)
		return;

	CCLOG("++++++++++ SDKComponent::pay");
	U8SDKInterface::getInstance()->pay(data);
}

void SDKComponent::logout()
{
	if (m_logoutState == LOGOUT_S_SDK)
    {
        m_logoutState = LOGOUT_S_NONE;
        return;
    }
    
    m_logoutState = LOGOUT_S_IN_GAME;
	U8SDKInterface::getInstance()->logout();
}

void SDKComponent::OnPaySuc()
{
	CCLOG("++++++++++ SDKComponent::OnPaySuc");
#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_TT == 0)
	VipModel::getInstance()->onPaySuc("");
#endif
}

void SDKComponent::submitExtraData( U8ExtraGameData* data )
{
	U8SDKInterface::getInstance()->submitGameData(data);
}
