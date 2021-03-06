#ifndef __U8SDK_CALLBACK_H__
#define __U8SDK_CALLBACK_H__

#include "U8SDKData.h"
#include "U8SDKInterface.h"

class U8SDKCallback
{

public:

	virtual void OnInitSuc() =0;

	virtual void OnLoginSuc(U8LoginResult* result) =0;

	virtual void OnSwitchLogin() =0;

	virtual void OnLogout() =0;

	// 新增接口，作为客户端收到服务器正式同步之前的一个预判标准;
	// 表明sdk充值操作的结果是否成功;
	// added by Phil 12/24/2015 @zcjoy;
	virtual void OnPaySuc() =0;
};
#endif

