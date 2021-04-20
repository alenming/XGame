/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LevelUpModel
 *  Description:	角色升级的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			04/11/2016
 *  Others: 
 *
 ******************************************************************/
#ifndef LevelUpModel_h__
#define LevelUpModel_h__

#include "Utils/Observer.h"


enum LEVELUP_NOTICE
{
	NOTICE_MAINGBUILDING = 10086
};

class LevelUpModel : public Observable
{
public:
	LevelUpModel(void);
	~LevelUpModel(void);

	static LevelUpModel*  getInstance();
	static void  destroyInstance();

	//有升级时通知mainbuildinglayer
	void noticeMainInfoLayer();

private:
	static LevelUpModel* m_pInstance;

};

#endif // LevelUpModel_h__
