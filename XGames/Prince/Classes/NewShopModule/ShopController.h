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
#ifndef ShopController_h__
#define ShopController_h__

#include "ShopDef.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"
#include "Communication/Command.h"

class ShopController : public Ref, public ICommListener
{
public:
	~ShopController();

	//活动单例
	static ShopController* getInstance();
	// 实现接收服务器消息接口;
	virtual void  onRecvMessage(int nCmdID, const char* szData);

	// 请求商店信息;
	void  handleShopInfo(SHOP_TYPE type);

	// 请求淬练界面信息;
	void  handleFirstShopLayerInfo();

	//加载本地淬练表信息
	void loadLocalCuilianInfo();

	//更新本地淬练商店界面信息
	void uadataCuiLianShopInfo();

	// 购买商品;
	int   handleBuy(int nIndex);

	// 刷新全部商品;
	int   handleRefresh();

	//抽
	void handleChouOneOrTen(CHOU_TYPE tpe);


	//判断主城是否要小红点
	void IsShowRedFlag();

private:
	ShopController();

	void proFirtShop(const char* szData);

	void  procBuy(const char* szData);

	void  procRefresh(const char* szData);

	void  procChouBack(const char* szData);
private:

	static ShopController* mInstance;

	// 刷新类型(1-自然刷新  2-货币刷新);
	int  m_nRefreshType;

	CHOU_TYPE m_chouType;

	bool firstFresh;

	int coIndex;
};

#endif // ShopexcController_h__
