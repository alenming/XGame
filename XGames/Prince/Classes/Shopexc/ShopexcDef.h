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


// �̵�����(�����������������ƥ�䣬10/30/2015��app������ȷ�Ϲ�);
enum ShopType
{
	SHOP_TYPE_NONE		= 0,
	SHOP_TYPE_ARENA		= 1,		// �������̵�;
	SHOP_TYPE_TRAIL		= 2,		// �����̵�;

	SHOP_TYPE_FACTION	= 5,		// �����̵�;
	SHOP_TYPE_REFINE_8	= 7,		// ����¯ - �����̵�;
	SHOP_TYPE_REFINE_9	= 8			// ����¯ - �Ź��̵�;
};

// UI��Ϣ����
enum OBS_PARAM_TYPE_SHOPESC
{
	OBS_PARAM_TYPE_SHOPESC_PUBLIC,			// �̵깫����Ϣ(��������/���ˢ�´���);
	OBS_PARAM_TYPE_SHOPESC_ALL_ITEM,		// ȫ����Ʒ��Ϣ;
	OBS_PARAM_TYPE_SHOPESC_SINGLE_ITEM,		// ������Ʒ��Ϣ;
	OBS_PARAM_TYPE_SHOPESC_COUNT_DOWN		// �̵�ˢ��ʱ�䵹��ʱ;
};

// �̵깺���������
enum SHOP_COST_TYPE
{
	SHOP_COST_TYPE_NONE, 
	SHOP_COST_TYPE_GOLD,  // ���
	SHOP_COST_TYPE_COIN,      // ����
	SHOP_COST_TYPE_COMMON,    // ͨ��
};

// �̵���Ʒ����;
typedef struct tagShopexcItemParam
{
	ShopType	nShopType;		// �̵�����;
	int			nIndex;			// ���(0~N);
	int			nId;			// Id;
	string		strName;		// ����;
	int			nCount;			// ����;
	int			nPrice;			// �۸�;
	SHOP_COST_TYPE costType;       // ���ѻ������ͣ�1 ���, 2 ����, 3 ��Ӧģ����ҹ���
	bool		bEnabled;		// ״̬;
	int			nLimitBuyCount;	// �޹�����;
	int			nSrvPos;		// ��Ʒ������Pos;

	// ����״̬;
	int			nRank;			// ��Ҫ����(������)(�������ô��ֶ�);

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

// չʾ��Ʒ;
typedef struct tagUIShopexcItem
{
	Layout* pRoot;

	// ����;
	ImageView*  imgRank;
	Text*		txtRank;

	ImageView* Img_SellOut;
	// ��Ʒ;
	Button*		btnItem;
	ImageView*	imgItem;
	ImageView*	imgLimit;
	Text*		txtName;

	// ����;
	ImageView*	imgScore;
	Text*		txtScore;

	tagUIShopexcItem()
	{
		memset(this, 0, sizeof(tagUIShopexcItem));
	}
}UIShopexcItem;

// �̵�UI���;
typedef struct tagUIShopexc
{
	Layout*  pRoot;

	Button*  btnClose;

	// ����;
	ImageView*  imgTitle;

	// �ҵĻ���;
	ImageView*  imgScore;
	Text*  txtScore;

	// ˢ����(����¯�̵�);
	ImageView*  imgRefreshTokenBg;
	Text*  txtRefreshToken;

	// ��Ʒ;
	ImageView*  imgContentBg;
	ScrollView* scrollviewContent;
	CustomScrollBar* scrollBar;

	// ˢ��;
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
