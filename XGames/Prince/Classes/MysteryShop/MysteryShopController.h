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
#ifndef MysteryShopController_h__
#define MysteryShopController_h__

#include "NewShopModule/ShopDef.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"
#include "Communication/Command.h"

class MysteryShopController : public Ref, public ICommListener
{
public:
	~MysteryShopController();

	static MysteryShopController* getInstance();

	virtual void  onRecvMessage(int nCmdID, const char* szData);

	void handleShopInfo(SHOP_TYPE type);

	int handleBuy( int nIndex );

	int handleRefresh();

private:
	void procShopInfo( const char* szData );
	void  procBuy(const char* szData);
private:
	MysteryShopController();

	static MysteryShopController*  mInstance;
	int coIndex;
	int m_nRefreshType;
	bool firstFresh;
};

#endif // ShopexcController_h__
