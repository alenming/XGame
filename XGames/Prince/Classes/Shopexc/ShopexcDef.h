#ifndef ShopexcDef_h__
#define ShopexcDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ui/UIDef.h"
#include <vector>
#include <map>
#include "Common/Common.h"
#include "ToolModule/UIToolHeader.h"
#include "Utils/ResourceUtils.h"
#include "Temp/CustomTips.h"
#include "FormationModule/CustomScrollBar.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;


// 商店类型(跟服务器定义的类型匹配，10/30/2015跟app和老王确认过);
enum ShopType
{
	SHOP_TYPE_NONE		= 0,
	SHOP_TYPE_ARENA		= 1,		// 竞技场商店;
	SHOP_TYPE_TRAIL		= 2,		// 试炼商店;

	SHOP_TYPE_FACTION	= 5,		// 帮派商店;
	SHOP_TYPE_REFINE_8	= 7,		// 炼化炉 - 八卦商店;
	SHOP_TYPE_REFINE_9	= 8			// 炼化炉 - 九宫商店;
};

// UI消息类型
enum OBS_PARAM_TYPE_SHOPESC
{
	OBS_PARAM_TYPE_SHOPESC_PUBLIC,			// 商店公共信息(货币总量/免费刷新次数);
	OBS_PARAM_TYPE_SHOPESC_ALL_ITEM,		// 全部商品信息;
	OBS_PARAM_TYPE_SHOPESC_SINGLE_ITEM,		// 单个商品信息;
	OBS_PARAM_TYPE_SHOPESC_COUNT_DOWN		// 商店刷新时间倒计时;
};

// 商店购买货币类型
enum SHOP_COST_TYPE
{
	SHOP_COST_TYPE_NONE, 
	SHOP_COST_TYPE_GOLD,  // 金币
	SHOP_COST_TYPE_COIN,      // 银币
	SHOP_COST_TYPE_COMMON,    // 通用
};

// 商店物品数据;
typedef struct tagShopexcItemParam
{
	ShopType	nShopType;		// 商店类型;
	int			nIndex;			// 序号(0~N);
	int			nId;			// Id;
	string		strName;		// 名称;
	int			nCount;			// 数量;
	int			nPrice;			// 价格;
	SHOP_COST_TYPE costType;       // 花费货币类型（1 金币, 2 银币, 3 相应模块货币购买）
	bool		bEnabled;		// 状态;
	int			nLimitBuyCount;	// 限购次数;
	int			nSrvPos;		// 物品服务器Pos;

	// 附加状态;
	int			nRank;			// 需要排名(竞技场)(试炼复用此字段);

	bool operator==(const int& _srvPos) const
	{
		return (nSrvPos == _srvPos);
	}

	tagShopexcItemParam()
		: nShopType(SHOP_TYPE_NONE)
		, nIndex(0)
		, nId(-1)
		, strName("")
		, nCount(0)
		, nPrice(0)
		, costType(SHOP_COST_TYPE_NONE)
		, bEnabled(true)
		, nLimitBuyCount(-1)
		, nRank(-1)
		, nSrvPos(0)
	{}
}ShopexcItemParam;

// 展示物品;
typedef struct tagUIShopexcItem
{
	Layout* pRoot;

	// 排名;
	ImageView*  imgRank;
	Text*		txtRank;

	ImageView* Img_SellOut;
	// 物品;
	Button*		btnItem;
	ImageView*	imgItem;
	ImageView*	imgLimit;
	Text*		txtName;

	// 货币;
	ImageView*	imgScore;
	Text*		txtScore;

	tagUIShopexcItem()
	{
		memset(this, 0, sizeof(tagUIShopexcItem));
	}
}UIShopexcItem;

// 商店UI组件;
typedef struct tagUIShopexc
{
	Layout*  pRoot;

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
	ScrollView* scrollviewContent;
	CustomScrollBar* scrollBar;

	// 刷新;
	Button*		btnRefresh;
	Text*		txtCost;
	Text*		txtTimeDesc;
	Text*		txtTime;
	Text*		txtCountDesc;
	Text*		txtCount;

	tagUIShopexc()
	{
		memset(this, 0, sizeof(tagUIShopexc));
	}
}UIShopexc;


#endif // ShopexcDef_h__
