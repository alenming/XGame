/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerGameBoard
 *  Description:	游戏公告UI实现;
 *  Version:		1.0
 *  Author:			Sonic
 *  Date:			10/30/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerGameBoard_h__
#define LayerGameBoard_h__

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


class LayerGameBoard : public ModalLayer
{

public:

	~LayerGameBoard(void);

	CREATE_FUNC(LayerGameBoard);

public:

	virtual bool  init();
	virtual void  onEnter();
	virtual void  onExit();

private:

	LayerGameBoard(void);

	void  initUI();
private:
	Layout *m_root;
};

#endif // LayerGameBoard_h__
