/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		ShopexcController
 *  Description:	通用商店控制器的实现;
 *					1. 服务器数据接收/发送接口;
 *					2. 流程控制;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			05/28/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef ShopexcController_h__
#define ShopexcController_h__

#include "ShopexcDef.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"
#include "Communication/Command.h"

class ShopexcController : public Ref, public ICommListener
{
public:
	ShopexcController(void);
	~ShopexcController(void);

	// 实现接收服务器消息接口;
	virtual void  onRecvMessage(int nCmdID, const char* szData);

	// 请求商店信息;
	void  handleShopInfo(ShopType type);

	// 购买商品;
	int   handleBuy(int nIndex);

	// 刷新全部商品;
	int   handleRefresh();

	// 倒计时;
	virtual void  refreshCountDown(float delta);

private:
	void  procBuy(const char* szData);

	void  procRefineBuy(const char* szData);

	void  procRefresh(const char* szData);

	void  procRefineRefresh(const char* szData);

private:

	// 当前时间戳;
	int  m_nStamp;

	// 刷新类型(1-自然刷新  2-货币刷新);
	int  m_nRefreshType;
};

#endif // ShopexcController_h__
