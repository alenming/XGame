/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		SDKComponent
 *  Description:	SDK封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			10/19/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef SDKComponent_h__
#define SDKComponent_h__

#include "cocos2d.h"
#include "Communication/EnvConfig.h"
#include "SDK/u8sdk/U8SDKInterface.h"

USING_NS_CC;
using namespace std;


class SDKComponent : public U8SDKCallback
{
public:
	~SDKComponent(void);

	static SDKComponent* getInstance();
	static void  releaseInstance();

	// 登陆;
	void  login();

	// 注销;
	void  logout();

	// 支付;
	void  pay(U8PayParams * data);

	// 登陆成功之后，提交查询数据;
	void  submitExtraData(U8ExtraGameData* data);

	//////////////////// sdk ////////////////////

	virtual void OnInitSuc();

	virtual void OnLoginSuc(U8LoginResult* result);

	virtual void OnSwitchLogin();

	virtual void OnLogout();

	virtual void OnPaySuc();

private:
	SDKComponent(void);

private:
	static SDKComponent*  m_pInstance;
	enum LOGOUT_STATE
    {
        LOGOUT_S_NONE    = 0,
        LOGOUT_S_SDK     = 1,
        LOGOUT_S_IN_GAME = 2
    };
    
    LOGOUT_STATE  m_logoutState;
};

#endif // SDKComponent_h__
