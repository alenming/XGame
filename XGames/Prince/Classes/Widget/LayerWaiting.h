/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerWaiting
 *  Description:	等待网络连接的UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			01/31/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerWaiting_h__
#define LayerWaiting_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "Utils/ResourceUtils.h"
#include "ui/UIDef.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

//waiting动画自动销毁时间
#define WAITING_AUTO_DELETE_SECONDS 45

typedef struct tagUI_Waiting
{
	Layout*		pRoot;
	ImageView*	imgWaiting;
	tagUI_Waiting()
	{
		memset(this, 0, sizeof(tagUI_Waiting));
	}
}UI_Waiting;


class LayerWaiting : public LayerColor
{

public:

	~LayerWaiting(void);

	CREATE_FUNC(LayerWaiting);

public:

	virtual bool  init();
	virtual void  onEnter();
	virtual void  onExit();

	static void showWaiting();
	static void hideWaiting();

private:

	LayerWaiting(void);

	void  initUI();

	void timerKillWaiting(float dt);

private:

	UI_Waiting  m_ui;

	static LayerWaiting* mLayerWaiting;


};

#endif // LayerWaiting_h__
