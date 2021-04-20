/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved.
 *
 *  FileName:		SDKComponent_ios
 *  Description:	SDK_ios封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			1/19/2016
 *  Others: 
 *
 ******************************************************************/
#ifndef SDKComponent_ios_h__
#define SDKComponent_ios_h__

//#import <Foundation/Foundation.h>
#include "./U8SDKData_ios.h"
//#include "../../SDK_IOS/Core/U8SDK/U8SDK.h"

class SDKComponent_ios
{
public:
	~SDKComponent_ios(void);

	static SDKComponent_ios* getInstance();
	static void  releaseInstance();

	// 登陆;
	void  login();

	// 注销;
	void  logout();

	// 支付;
	void  pay(U8PayParams * data);

	// 登陆成功之后，提交查询数据;
	//void  submitExtraData(U8ExtraGameData* data);

	//////////////////// sdk ////////////////////

	virtual void OnInitSuc();

	virtual void OnLoginSuc(U8LoginResult* result);

	virtual void OnSwitchLogin();

	virtual void OnLogout();

    virtual void OnPaySuc(std::string orderId);
    
    // 新增一个接口，在AppStore发起支付成功后，发起验证之前的节点通知;
    virtual void OnAppStorePaySuc(std::string orderId);
    
    /////////////////////////////////////////////
    // 跳转到AppStore;
    bool  jumoToAppStore();

private:
	SDKComponent_ios(void);

private:
	static SDKComponent_ios*  m_pInstance;
    
    enum LOGOUT_STATE
    {
        LOGOUT_S_NONE    = 0,
        LOGOUT_S_SDK     = 1,
        LOGOUT_S_IN_GAME = 2
    };
    
    LOGOUT_STATE  m_logoutState;
};

#endif // SDKComponent_ios_h__