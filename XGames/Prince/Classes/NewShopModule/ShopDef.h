/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ShopDef
 *  Description:	集市模块的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef ShopDef_h__
#define ShopDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "Communication/ICommListener.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Socket/Util.h"
#include "Common/Common.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainCityScene.h"
#include "Utils/StringFormat.h"
#include "Common/Common.h"
#include "Utils/SoundUtils.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;


#define  TENCHOU	10

enum SHOP_TYPE
{
	ST_NONE			= 0,
	ST_CUILIAN		= 1,			// 淬练商店;
	ST_ZAHUO		= 3,			// 杂货商店;
	ST_RONGYU		= 4,			// 荣誉商店;竞技场
	ST_JIFENG		= 5,			// 积分商店;试练
	ST_SEHGNWANG	= 6,			// 声望商店;
	ST_REFINE_8		= 7,			// 炼化炉 - 八卦商店;
	ST_REFINE_9		= 8,			// 炼化炉 - 九宫商店;
	ST_MysteryShop  = 10			// 神秘商店
};

enum CHOU_TYPE
{
	ONE_CHOU,
	TEN_CHOU
};

// 商店购买货币类型
enum MONEY_TYPE
{
	MONEY_NONE, 
	MONEY_GOLD,		 // 金币
	MONEY_COIN,		 // 银币
	MONEY_COMMON,    // 通用
};

// UI消息类型
enum SHOP_EVENT_TYEP
{
	SHOP_EVENT_TYEP_PUBLIC,			// 商店公共信息(货币总量/免费刷新次数);
	SHOP_EVENT_TYEP_ALL_ITEM,		// 全部商品信息;
	SHOP_EVENT_TYEP_SINGLE_ITEM,	// 单个商品信息;
	SHOP_EVENT_TYEP_COUNT_DOWN,		// 商店刷新时间倒计时;
	SHOP_EVENT_TYEP_FIRST_SHOP,		// 初始界面信息
	SHOP_EVENT_TYEP_ONE_CHOU,
	SHOP_EVENT_TYEP_TEN_CHOU,
	SHOP_EVENT_TYEP_CUILIAN,
	SHOP_EVENT_TYEP_CUILIAN_PUB
};

// 抽到物品数据;
typedef struct tagChouItemParam
{
	int			nIndex;			// 序号(0~N);
	int			nId;			// Id;
	string		strName;		// 名称;
	int			nCount;			// 数量;
	int			nPrice;			// 价格;

	tagChouItemParam()
		: nIndex(0)
		, nId(-1)
		, strName("")
		, nCount(0)
		, nPrice(0)
	{}
}ChouItemParam;
// 商店物品数据;
typedef struct tagShopItemParam
{
	SHOP_TYPE	nShopType;		// 商店类型;
	int			nIndex;			// 序号(0~N);
	int			nId;			// Id;
	string		strName;		// 名称;
	int			nCount;			// 数量;
	int			nPrice;			// 价格;
	MONEY_TYPE moneyType;       // 花费货币类型（1 金币, 2 银币, 3 相应模块货币购买）
	bool		bEnabled;		// -1;
	int			buyLimit;		// 限购次数;
	int			flag;			//已经购买的次数 当butlimit!= -1时用来判决是否已经出售完
	int			nSrvPos;		// 物品服务器Pos;

	// 附加状态;
	int			nRank;			// 需要排名(竞技场)(试炼复用此字段);

	bool operator==(const int& _srvPos) const
	{
		return (nSrvPos == _srvPos);
	}

	tagShopItemParam()
		: nShopType(ST_NONE)
		, nIndex(0)
		, nId(-1)
		, strName("")
		, nCount(0)
		, nPrice(0)
		, moneyType(MONEY_NONE)
		, bEnabled(true)
		, buyLimit(-1)
		, flag(0)
		, nRank(-1)
		, nSrvPos(0)
	{}
}ShopItemParam;


//开启一次
struct UI_Onece
{
	ImageView	*Img_Once;
	Button		*Btn_Once;
	ImageView	*Img_Tips;
	ImageView	*Img_Key;
	ImageView	*Img_Free;
	Text		*Lab_KeyCost;
	ImageView	*Img_Gold;
	Text		*Lab_GoldCost;
};

//开启十次
struct UI_Tenth
{
	ImageView	*Img_Tenth;
	Button		*Btn_Tenth;
	ImageView	*Img_Tips2;
	ImageView	*Img_Key;
	Text		*Lab_KeyCost;
	ImageView	*Img_Gold;
	Text		*Lab_GoldCost;
	ImageView	*Img_Design1;
	ImageView	*Img_Design2;
};

//淬练界面
struct UI_Box
{
	Layout		*Pnl_Box;						//淬练容器

	Text		*Lab_Tips;						//每次开启必得淬炼碎片
	Layout		*Pnl_centerBox;
	//底部UI
	ImageView	*Img_di;
	Text		*Lab_Fragment;					//淬练碎片
	Text		*Lab_Count;						//碎片数量
	std::vector<Layout*>		laFiveItem;
	Button		*Btn_Exchange;					//兑换
	//开启一次
	UI_Onece	ui_Onece;
	//开启十次
	UI_Tenth	ui_Tenth;

};

struct UI_OneChou
{
	Layout	*Pnl_Item;
	Text	*Lab_Item;

};

struct UI_CHOU
{
	Layout		*ui_Root;		//
	Layout		*Panel_38;

	Button		*Btn_Once;
	ImageView	*Img_Key;
	ImageView	*Img_Gold;
	ImageView	*Img_Free;
	Text		*Lab_KeyCost;
	Text		*Lab_GoldCost;

	Button		*Btn_Tenth;
	ImageView	*Img_KeyT;
	ImageView	*Img_GoldT;
	ImageView	*Img_FreeT;
	Text		*Lab_KeyCostT;
	Text		*Lab_GoldCostT;

	Layout*   Pnl_Tenth;
	Layout*   Pnl_Once;

	UI_OneChou	oneChou;
	std::vector<UI_OneChou> tenChou;
};

//4个商店通用的界面
struct UI_Son
{
	Layout		*Pnl_Root;		//

	Text		*Lab_Tips;
	ImageView	*Img_Score;
	Text		*Lab_Score;		//数量
	ImageView	*Img_di3;
	ImageView	*Img_di2;
	// 物品;
	ImageView	*imgContentBg;
	ui::ScrollView	*ScrollView_Content;
	CustomScrollBar	*scrollBar;

	// 刷新;
	Button		*Btn_Refresh;
	Text		*Lab_Count_Desc;	//手动刷新次数 :
	Text		*Lab_Count;			//99
	Text		*Lab_Time_Desc;		//下次自动刷新 :
	Text		*Lab_Time;			//99

	Text		*Lab_Cost;//按钮上的刷新价格
	UI_Son():scrollBar(nullptr)
	{
	}
};

//总界面
struct UI_ShopLayer
{
	Layout *ui_Root;		//根结点

	Button      *Btn_Box;			//淬练
	Button      *Btn_zahuo;			//杂货
	Button      *Btn_rongyu;		//荣誉
	Button      *Btn_jifen;			//积分
	Button      *Btn_shengwang;     //声望
	ImageView	*Img_di;

	UI_Son		ui_shop;
	UI_Box		ui_Box;

	//UI_Son		ui_Zahuo;
	//UI_Son		ui_Rongyu;
	//UI_Son		ui_Jifeng;
	//UI_Son		ui_Shengwang;
	
};

// 商店UI组件;
typedef struct tagUIShop
{
	Layout*  pRoot;
	ImageView	*Img_Bg;
	Button*  btnClose;

	// 标题;
	ImageView*  imgTitle;

	// 我的货币;
	ImageView*  imgScore;
	Text*  txtScore;

	// 刷新令(炼化炉商店);
	ImageView*  imgRefreshTokenBg;
	Text*  txtRefreshToken;

	// 物品;
	ImageView*  imgContentBg;
	ui::ScrollView* scrollviewContent;
	CustomScrollBar* scrollBar;

	// 刷新;
	Button*		btnRefresh;
	Text*		txtCost;
	Text*		txtTimeDesc;
	Text*		txtTime;
	Text*		txtCountDesc;
	Text*		txtCount;

	tagUIShop()
	{
		memset(this, 0, sizeof(tagUIShop));
	}
}UIShop;

// 展示物品;
typedef struct tagUIShopexcItems
{
	Layout* pRoot;

	// 排名;
	ImageView*  imgRank;
	Text*		txtRank;

	// 物品;
	Button*		btnItem;
	ImageView*	imgItem;
	ImageView*	imgLimit;
	Text*		txtName;

	// 货币;
	ImageView*	imgScore;
	Text*		txtScore;

	ImageView*	Img_SellOut;

	tagUIShopexcItems()
	{
		memset(this, 0, sizeof(tagUIShopexcItems));
	}
}UIShopItem;

//一个商品的界面
typedef struct tagUI_OneItem
{
	Layout *ui_Root;		//根结点
	Layout *Pnl_Item;		//商品头像
	ImageView	*Img_di1;
	Button*		Btn_Close;
	Button*		Btn_Buy;

	Text*		Lab_ItemName;//商品名称
	Text*		Lab_Count;//已经拥有的数量
	Text*		Lab_Tips;//商品说明
	Text*		Lab_Cost;//货币数量
	ImageView	*Img_CosIcon;//货币图片 

}UIOneItem;

#endif //ShopDef_h__