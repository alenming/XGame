/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ShopScene
 *  Description:	集市模块的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef ShopScene_h_
#define ShopScene_h_

#include "ShopLayer.h"
class ShopScene:public Scene
{
public:
	~ShopScene();
	 static Scene* createScene(SHOP_TYPE type,bool isPush);
protected:
	virtual bool init(SHOP_TYPE type,bool isPush) ;
	virtual void onEnter() override;
	virtual void onExit() override;

	ShopScene();
private:

	ShopLayer*  m_ShopLayer;
};
#endif //ShopScene_h_
