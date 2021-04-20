#include "SystemUtils.h"
#include "IosUtil.h"


std::string SystemUtils::getIDFA()
{
    string IDFA = "";
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IDFA = [[IosUtil deviceIDFA] UTF8String];
#endif
	return IDFA;
}

std::string SystemUtils::getIDFV()
{
    string IDFV = "";
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    IDFV = [[IosUtil deviceIDFV] UTF8String];
#endif
	return IDFV;
}

std::string SystemUtils::getMAC()
{
	return "";
}

std::string SystemUtils::getIPAddresses()
{
	string ipStr = "";
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ipStr = [[IosUtil deviceIPAdress] UTF8String];
#endif
    return ipStr;
}

void SystemUtils::setScreenLight(bool light)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  if(light)
  {
      [IosUtil setScreenLightOn];
  }
  else
  {
      [IosUtil setScreenLightOff];
  }
#endif
}

int SystemUtils::getChannelId()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"U8SDK"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    int channel = [[sdkparams valueForKey:@"Channel"] intValue];
    return channel;
#else
    // 以IOS正版渠道号为默认值，包含PC;
    return 1001;
#endif
}

int SystemUtils::getUaid()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"U8SDK"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    int Uaid = [[sdkparams valueForKey:@"Uaid"] intValue];
    return Uaid;
#else
    return 1;
#endif
}

int SystemUtils::getUwid()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"U8SDK"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    int Uwid = [[sdkparams valueForKey:@"Uwid"] intValue];
    return Uwid;
#else
    return 1;
#endif
}

string SystemUtils::getLogoImg()
{
    string imgLogo = "Image/Bg/logo.jpg";
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"GAME_DEF"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    NSString* nslogo = [sdkparams objectForKey:@"LogoPath"];
    if (nslogo != nil)
    {
        string logo = [nslogo UTF8String];
        if (!logo.empty())
        {
            imgLogo = logo;
        }
    }
#endif
    return imgLogo;
}

string SystemUtils::getCentralServer()
{
    string defaultServer = "127.0.0.1";
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"GAME_DEF"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    NSString* nsserver = [sdkparams objectForKey:@"CENTRAL_SERVER"];
    if (nsserver != nil)
    {
        string server = [nsserver UTF8String];
        if (!server.empty())
        {
            defaultServer = server;
        }
    }
#endif
    return defaultServer;
}

bool SystemUtils::isTianTuoConfig()
{
    bool defaultFlag = false;
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"GAME_DEF"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    NSString* nsflag = [sdkparams objectForKey:@"LoginWithTianTuo"];
    if (nsflag != nil)
    {
        int nFlag = [nsflag intValue];
        defaultFlag = (nFlag == 0) ? false : true;
    }
#endif
    return defaultFlag;
}

string SystemUtils::getLOGIN_SERVER_URL()
{
    string str1 = "http://" + getCentralServer();
    string str2 = "/userver/game/login";
    if (isTianTuoConfig())
    {
        str2.clear();
        str2 = "/userver/game/login2";
    }
    return (str1 + str2);
}

string SystemUtils::getREGISTER_URL()
{
    string str1 = "http://" + getCentralServer();
    string str2 = "/userver/game/regUser";
    if (isTianTuoConfig())
    {
        str2.clear();
        str2 = "/userver/game/regUser2";
    }
    return (str1 + str2);
}

int SystemUtils::getTianTuoGid()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"TianTuo"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    NSString* nsGid = [sdkparams objectForKey:@"gid"];
    if (nsGid != nil)
    {
        int nGid = [nsGid intValue];
        return nGid;
    }
#endif
    return 1;
}

string SystemUtils::getTianTuoKey()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"TianTuo"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    NSString* nsKey = [sdkparams objectForKey:@"key"];
    if (nsKey != nil)
    {
        string key = [nsKey UTF8String];
        return key;
    }
#endif
    return "";
}

int SystemUtils::getSubChannel()
{
    return 0;
}

string SystemUtils::getGameVersion()
{
    string strVersion("");
    NSDictionary* infoDic = [[NSBundle mainBundle] infoDictionary];
    NSString* nsVersion = nil;
    nsVersion = [infoDic objectForKey:@"CFBundleShortVersionString"];
    strVersion.assign([nsVersion UTF8String]);
    return strVersion;
}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
string SystemUtils::getGameBuild()
{
    string strBuild("");
    NSDictionary* infoDic = [[NSBundle mainBundle] infoDictionary];
    NSString* nsBuild = nil;
    nsBuild = [infoDic objectForKey:@"CFBundleVersion"];
    strBuild.assign([nsBuild UTF8String]);
    return strBuild;
}

bool SystemUtils::isEnableFlashScreen()
{
    // 海马(51)要有两种闪屏，一种是海马，一种是我们自己的;
    NSDictionary *sdkconfig = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"GAME_DEF"];
    NSMutableDictionary* sdkparams = sdkconfig ? [NSMutableDictionary dictionaryWithDictionary:sdkconfig] : [NSMutableDictionary dictionary];
    int flag = [[sdkparams objectForKey:@"IsEnableLogo"] intValue];
    return ((flag == 0) ? false : true);
}
#endif

std::string SystemUtils::getFontFileName()
{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (SystemUtils::getChannelId() == CHANNEL_ID_VEGA_IOS)
	{
		return "fonts/UTM Mabella.ttf";
	}
	else
	{
		return "fonts/FZZhengHeiS-B-GB.ttf";
	}
#else
	fontName = "fonts/FZZhengHeiS-B-GB.ttf";
#endif

}