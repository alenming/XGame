/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *
 *  FileName:		DoubleBonusListener
 *  Description:	双倍奖励活动 - 定时器实例
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			06/13/2016
 *  Others: 
 *
 ******************************************************************/
#ifndef DoubleBonusListener_h__
#define DoubleBonusListener_h__

#include "DoubleBonusDef.h"

typedef std::function<void()>  DoubleBonusListenerCallback;

class DoubleBonusListener : public TimeStampListener
{
public:
	DoubleBonusListener(DoubleBonusListenerCallback callbackFunc);
	~DoubleBonusListener(void);

	void  start(int nStamp);
	virtual void  timeOut();

private:
	DoubleBonusListenerCallback  _callback;
};

#endif // DoubleBonusListener_h__
