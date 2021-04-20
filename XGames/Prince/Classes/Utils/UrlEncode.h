/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		UrlEncode
 *  Description:	(C++)UrlEncode的标准实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			10/26/2015
 *  Others:			
 *
 ******************************************************************/

#ifndef UrlEncode_h__
#define UrlEncode_h__

#include <iostream>
#include <string>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

class UrlEncode
{
public:
	static string Encode(const std::string& str);
	static string Decode(const std::string& str);
};

#endif
