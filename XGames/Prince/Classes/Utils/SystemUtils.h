/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		SystemUtils
 *  Description:	(C++)SystemUtils的标准实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			11/2/2015
 *  Others:			
 *
 ******************************************************************/

#ifndef SystemUtils_h__
#define SystemUtils_h__

#include <iostream>
#include <string>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "cocos2d.h"
#include "Communication/EnvConfig.h"

USING_NS_CC;
using namespace std;

enum CHANNEL_ID
{
	CHANNEL_ID_YSDK = 35,    //应用宝
	CHANNEL_ID_UC = 10,      //UC
	CHANNEL_ID_U9 = 1002,    //U9
	CHANNEL_ID_VEGA = 1006,  //越南
    CHANNEL_ID_VEGA_IOS = 1007, //越南IOS
	CHANNEL_ID_APPSTORE = 1001 //IOS正版
};

class SystemUtils
{
public:
	static string getIDFA();
	static string getIDFV();
	static string getMAC();
	static string getIMEI();
	static string getIPAddresses();
	static void  setScreenLight(bool light);

	static int getChannelId();
	static int getUaid();
	static int getUwid();
	static string getLogoImg();
	static int getTianTuoGid();
	static string getTianTuoKey();
	static string getFontFileName();
	static int getSubChannel();
	static string getGameVersion();

	/*登陆参数*/
	static string  getCentralServer();		// 中心服务器地址;
	static bool    isTianTuoConfig();		// 是否天拓官网配置;
	static string  getLOGIN_SERVER_URL();																				// 账号登陆;
	static string  getZONE_LIST_URL()		{ return "http://" + getCentralServer() + "/userver/game/getSvrList"; };	// 获取分区;
	static string  getVERSION_INFO_URL()	{ return "http://" + getCentralServer() + "/userver/game/checkVersion"; };	// 获取版本信息;
	static string  getGAME_BOARD_URL()		{ return "http://" + getCentralServer() + "/userver/game/notice"; };		// 公告;
	static string  getREGISTER_URL();																					// 注册账号;
	static string  getMODIFY_PWD_URL()		{ return "http://" + getCentralServer() + "/userver/game/changePwd"; };		// 修改密码;
	static string  getTRY_GAME_URL()		{ return "http://" + getCentralServer() + "/userver/game/regTasteUser"; };	// 试玩;
	static string  getACTIVATE_URL()		{ return "http://" + getCentralServer() + "/userver/game/activeUser"; };	// 激活账号;
	static string  getSEND_DEV_ID_URL()		{ return "http://" + getCentralServer() + "/userver/game/updateMacId"; };	// 发送设备唯一标识;
	static string  getCHECK_USER_NAME_URL()	{ return "http://" + getCentralServer() + "/userver/game/checkUsername"; };	// [天拓]校验用户名;
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    static string getGameBuild();
    static bool   isEnableFlashScreen();
#endif

	static string fontName;
};

#endif
