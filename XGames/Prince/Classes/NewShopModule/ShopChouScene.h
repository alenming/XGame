/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ShopChouScene
 *  Description:	集市模块的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef ShopChouScene_h_
#define ShopChouScene_h_

#include "ShopChouLayer.h"

class ShopChouScene:public Scene
{
public:
	~ShopChouScene();

	static Scene* createScene(CHOU_TYPE tyep);
	 
protected:
	virtual bool init(CHOU_TYPE type);
	virtual void onEnter() override;
	virtual void onExit() override;

	ShopChouScene();
private:

	ShopChouLayer*  m_ShopChouLayer;
	CHOU_TYPE m_type;
};
#endif //ShopChouScene_h_
