#include "SDKComponent_ios.h"
#import "./SDKComponent_ios_Delegate.h"
#import "../../SDK_IOS/Core/U8SDK/U8Pay.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "LoginModule/LoginController.h"
#include "VIPModule/VipModel.h"
#include "LoginModule/SceneLogout.h"
#endif


SDKComponent_ios* SDKComponent_ios::m_pInstance = nullptr;

SDKComponent_ios_Delegate *_delegate = nil;

SDKComponent_ios::SDKComponent_ios(void)
{
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"U8SDK"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    [[U8SDK sharedInstance] initWithParams:sdkparams];
    //[[U8SDK sharedInstance] application:application didFinishLaunchingWithOptions:launchOptions];
    
    _delegate = [[SDKComponent_ios_Delegate alloc] init];
    [_delegate initDelegate];
    
    [[U8SDK sharedInstance] setupWithParams:nil];
    
    //if ([[U8SDK sharedInstance].defaultPay respondsToSelector:@selector(openIAP)])
    //{
    //    [[U8SDK sharedInstance].defaultPay openIAP];
    //}
    
    m_logoutState = LOGOUT_S_NONE;
}


SDKComponent_ios::~SDKComponent_ios(void)
{
    [_delegate release];
    m_logoutState = LOGOUT_S_NONE;
}

void SDKComponent_ios::OnInitSuc()
{
    
}

void SDKComponent_ios::OnLoginSuc( U8LoginResult* result )
{
#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS && LOGIN_WITH_TT == 0)
    LoginController::getInstance()->onSdkLoginAccountResult(result);
#endif
}

void SDKComponent_ios::OnSwitchLogin()
{
    
}

void SDKComponent_ios::OnLogout()
{
    // 注销之后直接调用登陆;
    // 此处不用调用了，在回到登陆界面之后，检查更新，之后会再次调用登陆;
    //login();
    
    if (m_logoutState == LOGOUT_S_NONE) {
        m_logoutState = LOGOUT_S_SDK;
    }
    
    if (m_logoutState == LOGOUT_S_SDK) {
#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        Director::getInstance()->replaceScene(SceneLogout::create());
#endif
    }
    else if (m_logoutState == LOGOUT_S_IN_GAME)
    {
        m_logoutState = LOGOUT_S_NONE;
    }
}

SDKComponent_ios* SDKComponent_ios::getInstance()
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new SDKComponent_ios;
        if (nullptr != m_pInstance)
        {
            //U8SDKInterface::getInstance()->initSDK(m_pInstance);
        }
    }
    
    return m_pInstance;
}

void SDKComponent_ios::releaseInstance()
{
    if (nullptr != m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

void SDKComponent_ios::login()
{
    NSLog(@"++++++++++ SDKComponent_ios::login");
    [[U8SDK sharedInstance].defaultUser login];
}

void SDKComponent_ios::pay( U8PayParams * data )
{
    if (nullptr == data)
        return;
    
    if ([[U8SDK sharedInstance].defaultPay respondsToSelector:@selector(openIAP)])
    {
        [[U8SDK sharedInstance].defaultPay openIAP];
    }
    
    NSLog(@"++++++++++ SDKComponent_ios::pay");
    
    /*
    NSDictionary* payInfo = [NSDictionary dictionaryWithObjectsAndKeys:
                             [NSString stringWithCString:data->productId.c_str() encoding:[NSString defaultCStringEncoding]], @"productId",
                             [NSString stringWithCString:data->productName.c_str() encoding:[NSString defaultCStringEncoding]], @"productName",
                             [NSString stringWithCString:data->productDesc.c_str() encoding:[NSString defaultCStringEncoding]], @"productFullName",
                             [NSNumber numberWithInt:data->price], @"price",
                             [NSNumber numberWithInt:data->buyNum], @"buyNum",
                             [NSNumber numberWithInt:data->coinNum], @"coinNum",
                             [NSString stringWithCString:data->serverId.c_str() encoding:[NSString defaultCStringEncoding]], @"serverId",
                             [NSString stringWithCString:data->serverName.c_str() encoding:[NSString defaultCStringEncoding]], @"serverName",
                             [NSString stringWithCString:data->roleId.c_str() encoding:[NSString defaultCStringEncoding]], @"roleId",
                             [NSString stringWithCString:data->roleName.c_str() encoding:[NSString defaultCStringEncoding]], @"roleName",
                             [NSNumber numberWithInt:data->roleLevel], @"roleLevel",
                             [NSString stringWithCString:data->vip.c_str() encoding:[NSString defaultCStringEncoding]], @"vip",
                             [NSString stringWithCString:data->orderID.c_str() encoding:[NSString defaultCStringEncoding]], @"orderID",
                             nil];
    
    [[U8SDK sharedInstance].defaultPay pay:payInfo];
     */
    
    U8ProductInfo* productInfo = [[U8ProductInfo alloc] init];
    productInfo.orderID = [NSString stringWithCString:data->orderID.c_str() encoding:[NSString defaultCStringEncoding]];
    productInfo.productName = [NSString stringWithCString:data->productName.c_str() encoding:NSUTF8StringEncoding];
    productInfo.productDesc = [NSString stringWithCString:data->productDesc.c_str() encoding:[NSString defaultCStringEncoding]];
    productInfo.productId = [NSString stringWithCString:data->productId.c_str() encoding:[NSString defaultCStringEncoding]];
    productInfo.price = [NSNumber numberWithInt:data->price];
    productInfo.buyNum = (NSInteger)data->buyNum;
    productInfo.roleId = [NSString stringWithCString:data->roleId.c_str() encoding:[NSString defaultCStringEncoding]];
    productInfo.roleName = [NSString stringWithCString:data->roleName.c_str() encoding:[NSString defaultCStringEncoding]];
    productInfo.roleLevel = [NSString stringWithFormat:@"%d", data->roleLevel];
    productInfo.serverId = [NSString stringWithCString:data->serverId.c_str() encoding:[NSString defaultCStringEncoding]];
    productInfo.serverName = [NSString stringWithCString:data->serverName.c_str() encoding:[NSString defaultCStringEncoding]];
    productInfo.vip = [NSString stringWithCString:data->vip.c_str() encoding:[NSString defaultCStringEncoding]];
    productInfo.extension = [[NSDictionary alloc] init];
    productInfo.uniqueUserId = [NSString stringWithFormat:@"%s#%s", [productInfo.roleId UTF8String], [productInfo.serverId UTF8String]];
    
    
    if ([[U8SDK sharedInstance].defaultPay respondsToSelector:@selector(openIAP)])
    {
        [[U8SDK sharedInstance].defaultPay openIAP];
    }
 
    [[U8SDK sharedInstance].defaultPay pay:productInfo];
}

void SDKComponent_ios::logout()
{
    if (m_logoutState == LOGOUT_S_SDK)
    {
        m_logoutState = LOGOUT_S_NONE;
        return;
    }
    
    m_logoutState = LOGOUT_S_IN_GAME;
    
    NSLog(@"++++++++++ SDKComponent_ios::logout");
    [[U8SDK sharedInstance].defaultUser logout];
}

void SDKComponent_ios::OnPaySuc(std::string orderId)
{
    NSLog(@"++++++++++ SDKComponent_ios::OnPaySuc");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    VipModel::getInstance()->onPaySuc(orderId);
#endif
}

void SDKComponent_ios::OnAppStorePaySuc(std::string orderId)
{
    NSLog(@"++++++++++ SDKComponent_ios::OnAppStorePaySuc");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    VipModel::getInstance()->onAppStorePaySuc(orderId);
#endif
}

/*
void SDKComponent_ios::submitExtraData( U8ExtraGameData* data )
{
    U8SDKInterface::getInstance()->submitGameData(data);
}
*/

bool SDKComponent_ios::jumoToAppStore()
{
    NSString* url = nil;
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"GAME_DEF"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    url = [sdkparams objectForKey:@"AppStoreUrl"];
    if (url != nil)
    {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:url]];
        return true;
    }
    return false;
}
