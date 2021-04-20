#include "TalkingData.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)  
#include "platform/android/jni/JniHelper.h"  
#include <jni.h>  
#include "Temp/CustomTips.h"
#endif 

#define CLASS_NAME "com/u8/sdk/TalkingData"

#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
extern "C"
{
	static jobject createJavaMapObject(JNIEnv *env, std::map<std::string, std::string>* map) {
		jclass class_Hashtable = env->FindClass("java/util/HashMap");
		jmethodID construct_method = env->GetMethodID( class_Hashtable, "<init>","()V");
		jobject obj_Map = env->NewObject( class_Hashtable, construct_method, "");
		if (map != NULL) {
			jmethodID add_method= env->GetMethodID( class_Hashtable,"put","(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
			for (std::map<std::string, std::string>::const_iterator it = map->begin(); it != map->end(); ++it) {
				env->CallObjectMethod(obj_Map, add_method, env->NewStringUTF(it->first.c_str()), env->NewStringUTF(it->second.c_str()));
			}
		}
		env->DeleteLocalRef(class_Hashtable);
		return obj_Map;
	}
}
#endif

TalkingData::TalkingData(void)
{

}

TalkingData::~TalkingData(void)
{

}

void TalkingData::loginTalkingData(string userID, string SeverId,string accountName)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		CLASS_NAME, "loginTalkingData","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");  

	if (isHave) {  
		//调用此函数 
		jstring jUserID = minfo.env->NewStringUTF(userID.c_str());
		jstring jSeverId = minfo.env->NewStringUTF(SeverId.c_str());
		jstring jaccountName = minfo.env->NewStringUTF(accountName.c_str());
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,jUserID,jSeverId,jaccountName);
		minfo.env->DeleteLocalRef(minfo.classID);
	}  
#endif 
}

void TalkingData::setLevel(int level)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		CLASS_NAME, "setLevel","(I)V");  

	if (isHave) {  
		//调用此函数 
		jint jlevel = level;
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,jlevel);
		minfo.env->DeleteLocalRef(minfo.classID);
	}  
#endif 
}

void TalkingData::onReward(double virtualCurrencyAmount, string reason)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		CLASS_NAME, "onReward","(DLjava/lang/String;)V");  

	if (isHave) {  
		//调用此函数 
		jdouble jvirtualCurrencyAmount = virtualCurrencyAmount;
		jstring jreason = minfo.env->NewStringUTF(reason.c_str());
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,jvirtualCurrencyAmount,jreason);
		minfo.env->DeleteLocalRef(minfo.classID);
	}  
#endif 
}

void TalkingData::onPurchase(string item, int itemNumber,double priceInVirtualCurrency)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		CLASS_NAME, "onPurchase","(Ljava/lang/String;ID)V");  

	if (isHave) {  
		//调用此函数 
		jstring jitem = minfo.env->NewStringUTF(item.c_str());
		jint jitemNumber = itemNumber;
		jdouble jpriceInVirtualCurrency = priceInVirtualCurrency;
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,jitem,jitemNumber,jpriceInVirtualCurrency);
		minfo.env->DeleteLocalRef(minfo.classID);
	}  
#endif 
}

void TalkingData::onUse(string item, int itemNumber)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		CLASS_NAME, "onUse","(Ljava/lang/String;I)V");  

	if (isHave) {  
		//调用此函数 
		jstring jitem = minfo.env->NewStringUTF(item.c_str());
		jint jitemNumber = itemNumber;
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,jitem,jitemNumber);
		minfo.env->DeleteLocalRef(minfo.classID);
	}  
#endif 
}

void TalkingData::onBegin(string missionId)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		CLASS_NAME, "onBegin","(Ljava/lang/String;)V");  

	if (isHave) {  
		//调用此函数 
		jstring jmissionId = minfo.env->NewStringUTF(missionId.c_str());
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,jmissionId);
		minfo.env->DeleteLocalRef(minfo.classID);
	}  
#endif 
}

void TalkingData::onCompleted(string missionId)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		CLASS_NAME, "onCompleted","(Ljava/lang/String;)V");  

	if (isHave) {  
		//调用此函数 
		jstring jmissionId = minfo.env->NewStringUTF(missionId.c_str());
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,jmissionId);
		minfo.env->DeleteLocalRef(minfo.classID);
	}  
#endif 
}

void TalkingData::onFailed(string missionId, string cause)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		CLASS_NAME, "onFailed","(Ljava/lang/String;Ljava/lang/String;)V");  

	if (isHave) {  
		//调用此函数 
		jstring jmissionId = minfo.env->NewStringUTF(missionId.c_str());
		jstring jcause = minfo.env->NewStringUTF(cause.c_str());
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,jmissionId,jcause);
		minfo.env->DeleteLocalRef(minfo.classID);
	}  
#endif 
}

void TalkingData::onEvent(string eventId, EventParamMap map)
{
#if (TALKING_DATA == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) //判断当前是否为Android平台 
	JniMethodInfo minfo;//定义Jni函数信息结构体  
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数  
	bool isHave = JniHelper::getStaticMethodInfo(minfo,  
		CLASS_NAME, "onEvent","(Ljava/lang/String;Ljava/util/Map;)V");  

	if (isHave) {  
		//调用此函数 
		jstring jeventId = minfo.env->NewStringUTF(eventId.c_str());
		jobject jparamMap = createJavaMapObject(minfo.env, &map);
		minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID,jeventId,jparamMap);
		minfo.env->DeleteLocalRef(minfo.classID);
	}  
#endif 
}
