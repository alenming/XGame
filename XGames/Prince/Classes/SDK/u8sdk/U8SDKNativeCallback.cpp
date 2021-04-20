#include "U8SDKInterface.h"



U8SDKNativeCallback::U8SDKNativeCallback(U8SDKCallback* callback)
{
	this->gameCallback = callback;
}

U8SDKNativeCallback::~U8SDKNativeCallback()
{
}


void U8SDKNativeCallback::OnU8InitSuc(cocos2d::Node *sender, cocos2d::Value data)
{

	if (data.isNull() || data.getType() != cocos2d::Value::Type::MAP)
	{
		CCLOG("OnU8InitSuc Data Type Error:: curr data type is not map.");
		return;
	}

	cocos2d::ValueMap json = data.asValueMap();
	bool exit = json["isSupportExit"].asBool();
	bool accountCenter = json["isSupportAccountCenter"].asBool();
	bool logout = json["isSupportLogout"].asBool();

	U8SDKInterface::getInstance()->setSupports(exit, accountCenter, logout);

	if (this->gameCallback != nullptr)
	{
		this->gameCallback->OnInitSuc();
	}
}

void U8SDKNativeCallback::OnU8LoginSuc(cocos2d::Node *sender, cocos2d::Value data)
{
	if (data.isNull() || data.getType() != cocos2d::Value::Type::MAP)
	{
		CCLOG("OnU8LoginSuc Data Type Error:: curr data type is not map.");
		return;
	}

	U8LoginResult* result = U8LoginResult::create();
	cocos2d::ValueMap json = data.asValueMap();
	result->isSuc = json["isSuc"].asBool();
	result->isSwitchAccount = json["isSwitchAccount"].asBool();
	result->userId = json["userID"].asString();
	result->sdkUserId = json["sdkUserID"].asString();
	result->username = json["username"].asString();
	result->sdkUsername = json["sdkUsername"].asString();
	result->token = json["token"].asString();

	if (this->gameCallback != nullptr)
	{
		this->gameCallback->OnLoginSuc(result);
	}
}

void U8SDKNativeCallback::OnU8SwitchLogin(cocos2d::Node *sender, cocos2d::Value data)
{
	if (this->gameCallback != nullptr)
	{
		this->gameCallback->OnSwitchLogin();
	}
}

void U8SDKNativeCallback::OnU8Logout(cocos2d::Node *sender, cocos2d::Value data)
{
	if (this->gameCallback != nullptr)
	{
		this->gameCallback->OnLogout();
	}
}

void U8SDKNativeCallback::OnU8PaySuc(cocos2d::Node *sender, cocos2d::Value data)
{
	//单机支付使用，网游不使用该回调

	// 仍使用此接口，作为客户端收到服务器正式同步之前的一个预判标准;
	// 表明sdk充值操作的结果是否成功;
	// added by Phil 12/24/2015 @zcjoy;
	if (this->gameCallback != nullptr)
	{
		this->gameCallback->OnPaySuc();
	}
}
