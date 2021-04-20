/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		CommonBuyDef
 *  Description:	通用购买数据定义;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/10/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef CommonBuyDef_h__
#define CommonBuyDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include <string>
#include <vector>
#include <map>
#include "Common/Common.h"
#include "ToolModule/UIToolHeader.h"
#include "Utils/ResourceUtils.h"
#include "Temp/CustomTips.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

enum COMMON_BUY_TYPE
{
	COMMON_BUY_TYPE_GOLD_POWER,				// 金币 - 体力;
	COMMON_BUY_TYPE_GOLD_ENERGY,			// 金币 - 精力;
	COMMON_BUY_TYPE_GOLD_MAZEPOWER,			// 金币 - 迷宫行动力;
	COMMON_BUY_TYPE_GOLD_COIN				// 金币 - 银币(祈福);
};

typedef struct tagCommonBuyData
{
	COMMON_BUY_TYPE type;

	int nNum;			// 剩余可购买次数;
	int nTotalNum;		// 总共可购买次数;
	int nAddValue;		// 购买总量;
	int nCostGold;		// 消耗金币总量;

	// 多次祈福;
	int nTimes;			// 最大可祈福次数;

	void  clear()
	{
		type = COMMON_BUY_TYPE_GOLD_COIN;
		nNum = 0;
		nTotalNum = 0;
		nAddValue = 0;
		nCostGold = 0;
		nTimes = 0;
	}

	tagCommonBuyData()
	{
		memset(this, 0, sizeof(tagCommonBuyData));
	}
}CommonBuyData;

typedef struct tagUI_Common_Buy
{
	// 购买;
	Layout*		pBuyRoot;
	ImageView*	imgBuyCoinBg;
	ImageView*  imgTitle;
	ImageView*  imgVip;
	Text*		txtDesc;
	Text*		txtBuyCoin;		// 这俩只显示一个;
	Text*		txtBuyOther;	// 这俩只显示一个;
	Text*		txtCount;
	Text*		txtGold;

	ImageView*	imgDst;
	Text*		txtDst;

	Button*		btnOK;
	Button*		btnCancel;
	Button*		btnBuy;
	Button*		btnMultiBuy;
	Button*		btnClose;

	// 多次购买(祈福);
	Layout*		pMultiBuyRoot;
	ImageView*	imgBuyCoinsBg;
	Text*		txtMultiCount;
	Text*		txtMultiTips1;
	Text*		txtMultiGold;
	Text*		txtMultiTips2;
	Text*		txtMultiCoin;
	Button*		btnMultiOK;
	Button*		btnMultiCancel;
	Button*		btnMultiClose;

	// 提升VIP等级;
	Layout*		pVipRoot;
	ImageView*	imgVipBg;
	ImageView*	imgVipNext;
	Text*		txtVipTips1;
	Text*		txtVipTips2;
	Text*		txtNextCount;
	Button*		btnVipOK;
	Button*		btnVipCancel;
	Button*		btnVipClose;

	tagUI_Common_Buy()
	{
		memset(this, 0, sizeof(tagUI_Common_Buy));
	}
}UI_Common_Buy;

#endif // CommonBuyDef_h__