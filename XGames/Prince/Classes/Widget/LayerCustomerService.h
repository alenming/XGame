/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerCustomerService
 *  Description:	客服系统UI实现;
 *  Version:		1.0
 *  Author:			Sonic
 *  Date:			10/30/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerCustomerService_h__
#define LayerCustomerService_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "Utils/ResourceUtils.h"
#include "ui/UIDef.h"
#include "UI/ModalLayer.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;


class LayerCustomerService : public ModalLayer
{

public:

	~LayerCustomerService(void);

	CREATE_FUNC(LayerCustomerService);

public:

	virtual bool  init();
	virtual void  onEnter();
	virtual void  onExit();

private:

	LayerCustomerService(void);

	void  initUI();
private:

};

#endif // LayerCustomerService_h__
