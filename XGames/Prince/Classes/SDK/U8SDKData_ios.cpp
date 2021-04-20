#include "U8SDKData_ios.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

U8PayParams::U8PayParams()
{

}
U8PayParams::~U8PayParams()
{

}

U8PayParams* U8PayParams::create()
{
	auto ret = new U8PayParams();
	if (ret)
	{
		return ret;
	}
	return nullptr;
}


U8ExtraGameData::U8ExtraGameData()
{

}
U8ExtraGameData::~U8ExtraGameData()
{

}

U8ExtraGameData* U8ExtraGameData::create()
{
	auto ret = new U8ExtraGameData();
	if (ret)
	{
		return ret;
	}
	return nullptr;
}

U8LoginResult::U8LoginResult()
    : isSuc(false)
    , isSwitchAccount(false)
    , userId("")
    , username("")
    , sdkUserId("")
    , sdkUsername("")
    , token("")
{

}

U8LoginResult::~U8LoginResult()
{

}

U8LoginResult* U8LoginResult::create()
{
	auto ret = new U8LoginResult();
	if (ret)
	{
		return ret;
	}
	return nullptr;
}

#endif