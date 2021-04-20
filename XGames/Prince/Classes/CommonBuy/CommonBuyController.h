/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		CommonBuyController
 *  Description:	货币购买控制器;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/10/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef CommonBuyController_h__
#define CommonBuyController_h__

#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"
#include "CommonBuyDef.h"
#include "LayerCommonBuy.h"

class LayerCommonBuy;
class CommonBuyController : public ICommListener
{
public:
	CommonBuyController(void);
	~CommonBuyController(void);

	virtual void  onRecvMessage(int nCmdID, const char* szData);

	// 请求初始数据;
	void  handleInitData(COMMON_BUY_TYPE type);

	// 购买;
	void  handleBuy();

	// 祈福/祈福多次;
	void  handleMultiBuy(bool bMulti = false);

private:
	void  procBuyResult(const char* szData);

private:

	// 购买类型;
	COMMON_BUY_TYPE  m_type;

	// 购买次数;
	int   m_nTimes;
};

#endif // CommonBuyController_h__
