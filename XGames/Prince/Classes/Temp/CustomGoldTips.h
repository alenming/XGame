/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		CustomGoldTips
 *  Description:	公共弹窗;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			10/30/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef CustomGoldTips_h__
#define CustomGoldTips_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "cocostudio.h"
#include "UI/UIDef.h"
#include "Common/Common.h"
#include "UI/ModalLayer.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class CustomGoldTips : public ModalLayer
{
public:
	~CustomGoldTips(void);

	CREATE_FUNC(CustomGoldTips);

public:
	virtual bool  init();
	virtual void  onEnter();
	virtual void  onExit();

private:
	CustomGoldTips(void);
	void  initUI();

	Layout* m_root;
};


#endif // CustomGoldTips_h__