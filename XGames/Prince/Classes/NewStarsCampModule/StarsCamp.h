/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsCampNewModel
 *  Description:	具体某个星盘的Layer;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			02/19/2016
 *  Others:			
 *
 ******************************************************************/
#include "StarsCampDef.h"
#include "math.h"
#include "StarsCampModel.h"

#ifndef  StarsCampNew_h_
#define  StarsCampNew_h_


class StarsCamp  
{
public:
	~StarsCamp();

	static StarsCamp* create();

	void initUI();

private:
	StarsCamp();

};

#endif //StarsCampNew_h_