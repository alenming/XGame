/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		MarvellousActiveScene
 *  Description:	精彩活动UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			10/13/2015
 *  Others:			
 *
 ******************************************************************/
#include "MarvellousActiveHeader.h"
#include "MarvellousActiveLayer.h"

#ifndef MarvellousActiveScene_h_
#define MarvellousActiveScene_h_


class MarvellousActiveScene:public Scene
{
public:
	~MarvellousActiveScene();

	CREATE_FUNC(MarvellousActiveScene);
	 

protected:
	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;


	MarvellousActiveScene();
private:

	MarvellousActiveLayer*  m_MarvellousActiveLayer;
};


#endif // !LoveScene_h_
