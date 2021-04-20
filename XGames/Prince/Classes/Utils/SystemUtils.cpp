#include "SystemUtils.h"
#include "Communication/EnvConfig.h"
#if(CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID)  
#include "platform/android/jni/JniHelper.h"  
#include <jni.h>  
#include "Temp/CustomTips.h"
#endif 

#include "LoginModule/DataCollection.h"

std::string SystemUtils::fontName = "fonts/GillSans-Condensed.ttf";
std::string SystemUtils::getIDFA()
{
	return "";
}

std::string SystemUtils::getIDFV()
{
	return "";
}

std::string SystemUtils::getMAC()
{
	string macStr = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		"com.zcjoy.prince.AppActivity", "getMacAdress","()Ljava/lang/String;");  

	if (isHave) {  
		//调用此函数  
		jstring str=(jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		macStr = JniHelper::jstring2string(str);
	}  
#endif 
	return macStr;
}

std::string SystemUtils::getIMEI()
{
	string imeiStr = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		"com.zcjoy.prince.AppActivity", "getIMEI","()Ljava/lang/String;");  

	if (isHave) {  
		//调用此函数  
		jstring str=(jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		imeiStr = JniHelper::jstring2string(str);
	}  
#endif 
	return imeiStr;
}

std::string SystemUtils::getIPAddresses()
{
	string ipStr = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		"com.zcjoy.prince.AppActivity", "getIpAdress","()Ljava/lang/String;");  

	if (isHave) {  
		//调用此函数  
		jstring str=(jstring) minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		ipStr = JniHelper::jstring2string(str);
	}  
#endif 
	return ipStr;
}

void SystemUtils::setScreenLight(bool light)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	if (light)
	{
		bool isHave = JniHelper::getStaticMethodInfo(minfo,  
				"com.zcjoy.prince.AppActivity", "setScreenLightOn","()Z");  

		if (isHave) {  
			//调用此函数  
			minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
			minfo.env->DeleteLocalRef(minfo.classID);
		}  
	}
	else
	{
		bool isHave = JniHelper::getStaticMethodInfo(minfo,  
			"com.zcjoy.prince.AppActivity", "setScreenLightOff","()Z");  

		if (isHave) {  
			//调用此函数  
			minfo.env->CallStaticBooleanMethod(minfo.classID, minfo.methodID);
			minfo.env->DeleteLocalRef(minfo.classID);
		}  
	}
#endif 
}

int SystemUtils::getChannelId()
{
	int channelId = 1001;
	if(LOGIN_WITH_SDK == 0)
	{
		// 以IOS正版渠道号为默认值，包含PC;
		channelId = 1001;
	}
	else
	{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JniMethodInfo minfo;
		bool isHave = JniHelper::getStaticMethodInfo(minfo, "com.zcjoy.prince.AppActivity", "getChannelId","()Ljava/lang/String;");
		if (isHave)
		{
			jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
			minfo.env->DeleteLocalRef(minfo.classID);
			string str = JniHelper::jstring2string(jstr);
			channelId = atoi(str.c_str());
		}
#endif

	}

	CCLOG("++++++++++ cpp: getChannelId = %d", channelId);
	return channelId;
}

int SystemUtils::getUaid()
{
	int uaid = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo, "com.zcjoy.prince.AppActivity", "getUaid","()Ljava/lang/String;");
	if (isHave)
	{
		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		string str = JniHelper::jstring2string(jstr);
		uaid = atoi(str.c_str());
	}
#endif
	CCLOG("++++++++++ cpp: getUaid = %d", uaid);
	return uaid;
}

int SystemUtils::getUwid()
{
	int uwid = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo, "com.zcjoy.prince.AppActivity", "getUwid","()Ljava/lang/String;");
	if (isHave)
	{
		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		string str = JniHelper::jstring2string(jstr);
		uwid = atoi(str.c_str());
	}
#endif
	CCLOG("++++++++++ cpp: getUwid = %d", uwid);
	return uwid;
}

std::string SystemUtils::getLogoImg()
{
	string imgLogo = "Image/Bg/logo.jpg";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_SDK == 1)
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo, "com.zcjoy.prince.AppActivity", "getLogoImg","()Ljava/lang/String;");
	if (isHave)
	{
		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		imgLogo = JniHelper::jstring2string(jstr);
	}
#endif
	CCLOG("++++++++++ cpp: getLogoImg = %s", imgLogo.c_str());
	return imgLogo;
}

std::string SystemUtils::getCentralServer()
{
	string strIP = "127.0.0.1";
#ifdef LOGIN_RELEASE

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_SDK == 1)
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo, "com.zcjoy.prince.AppActivity", "getCentralServer","()Ljava/lang/String;");
	if (isHave)
	{
		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		strIP = JniHelper::jstring2string(jstr);
	}
#endif
	CCLOG("++++++++++ cpp: getCentralServer = %s", strIP.c_str());
	return strIP;

#elif defined LOGIN_TEST

	return _TEST_CENTRAL_SERVER_IP_;

#endif
	return strIP;
}

bool SystemUtils::isTianTuoConfig()
{
	bool isTT = false;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_SDK == 1)
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo, "com.zcjoy.prince.AppActivity", "isTianTuoConfig","()Ljava/lang/String;");
	if (isHave)
	{
		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		string strFlag = JniHelper::jstring2string(jstr);
		isTT = (atoi(strFlag.c_str()) == 0) ? false : true;
	}
#endif
	return isTT;
}

std::string SystemUtils::getLOGIN_SERVER_URL()
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

std::string SystemUtils::getREGISTER_URL()
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
	int TuoGid = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_SDK == 1)
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo, "com.zcjoy.prince.AppActivity", "getTianTuoGid","()Ljava/lang/String;");
	if (isHave)
	{
		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		string str = JniHelper::jstring2string(jstr);
		TuoGid = atoi(str.c_str());
	}
#endif
	return TuoGid;
}

std::string SystemUtils::getTianTuoKey()
{
	std::string TuoKey = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_SDK == 1)
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo, "com.zcjoy.prince.AppActivity", "getTianTuoKey","()Ljava/lang/String;");
	if (isHave)
	{
		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		TuoKey = JniHelper::jstring2string(jstr);
	}
#endif
	return TuoKey;
}

int SystemUtils::getSubChannel()
{
	int subChannel = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	bool isHave = JniHelper::getStaticMethodInfo(minfo, "com.zcjoy.prince.AppActivity", "getSubChannel","()Ljava/lang/String;");
	if (isHave)
	{
		jstring jstr = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		string str = JniHelper::jstring2string(jstr);
		subChannel = atoi(str.c_str());
	}
#endif
	CCLOG("++++++++++ cpp: getSubChannel = %d", subChannel);
	return subChannel;
}

std::string SystemUtils::getFontFileName()
{
	if (!fontName.empty())
	{
		return fontName;
	}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (SystemUtils::getChannelId() == CHANNEL_ID_VEGA)
	{
		fontName = "fonts/UTM Mabella.ttf";
	}
	else
	{
		fontName = "fonts/FZZhengHeiS-B-GB.ttf";
	}
#else
	fontName = "fonts/FZZhengHeiS-B-GB.ttf";
#endif
	return fontName;
}

string SystemUtils::getGameVersion()
{
	return "";
}