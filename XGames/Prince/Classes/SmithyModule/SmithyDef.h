#ifndef SmithyDef_h__
#define SmithyDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolDef.h"
#include "FormationModule/CustomScrollBar.h"
#include "Temp/CustomPop.h"
#include "Temp/CustomTips.h"
#include "Utils/ResourceUtils.h"
#include "Common/Common.h"
#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace cocos2d::ui;

#define TOTAL_ITEM_QUA_COUNT	6

// 装备品质颜色;
static Color3B  g_EquipColorWithQua[TOTAL_ITEM_QUA_COUNT] = {
	Color3B(0xFF, 0xFF, 0xFF),		// 白;
	Color3B(0x45, 0xE1, 0x6D),		// 绿;
	Color3B(0x36, 0xD1, 0xF2),		// 蓝;
	Color3B(0xC1, 0x69, 0xFF),		// 紫;
	Color3B(0xFF, 0xC1, 0x3B),		// 橙;
	Color3B(0xFF, 0x28, 0x23)		// 红;
};

// 装备属性类型;
enum SMITHY_EQU_ATTR_TYPE
{
	ATTR_TYPE_HP,					// 血量;
	ATTR_TYPE_ATK,					// 攻击;
	ATTR_TYPE_PDD,					// 物防;
	ATTR_TYPE_MDD,					// 法防;

	ATTR_TYPE_HP_COE,				// 血量加成;
	ATTR_TYPE_ATK_COE,				// 攻击加成;
	ATTR_TYPE_PDD_COE,				// 物防加成;
	ATTR_TYPE_MDD_COE,				// 法防加成;

	ATTR_TYPE_CRI_PRB,				// 暴击率;
	ATTR_TYPE_CRI_RES,				// 抗暴率;
	ATTR_TYPE_HIT,					// 命中率;
	ATTR_TYPE_MISS,					// 闪避率;

	// 用来计算数量，不要参与逻辑计算;
	ATTR_TYPE__NUM
};


// 铁匠铺操作类型;
enum SMITHY_OP_TYPE
{
	SMITHY_OP_TYPE_QH	= 1,		// 强化;
	SMITHY_OP_TYPE_JJ	= 2,		// 进阶;
	SMITHY_OP_TYPE_CL	= 3,		// 淬炼;
	//SMITHY_OP_TYPE_XL	= 3,		// 洗练;
									// ...
	SMITHY_OP_TYPE_NONE = 99		// 无;
};

// 洗练类型;
/*enum SMITHY_XL_TYPE
{
	SMITHY_XL_TYPE_COIN,			// 银币洗练;
	SMITHY_XL_TYPE_GOLD				// 金币洗练;
};*/

enum TOOL_SOURCE_UI_ID
{
	TOOL_SOURCE_UI_NONE,
	TOOL_SOURCE_UI_PACK,			// 背包;
	TOOL_SOURCE_UI_HERO,			// 英雄;
	TOOL_SOURCE_UI_HERO_ADV,		// 英雄进阶界面;
	TOOL_SOURCE_UI_SMITHY_ADV,		// 铁匠铺进阶;
	TOOL_SOURCE_UI_OTHER
};

enum OBS_PARAM_TYPE_SM
{
	OBS_PARAM_TYPE_EQUIPMENT,				// 装备列表中的Icon更新;
	OBS_PARAM_TYPE_EQU_QH_RESULT,			// 装备强化结果;
	OBS_PARAM_TYPE_EQU_JJ_RESULT,			// 装备进阶结果;
	OBS_PARAM_TYPE_EQU_CL_RESULT,			// 装备淬炼结果;
	OBS_PARAM_TYPE_EQU_CL_DEGRADE_RESULT	// 装备淬炼降级结果;
};


// 铁匠铺入口参数;
typedef struct tagSmithyInitData
{
	// 1. 英雄Id;
	int  nHeroId;

	// 2. 道具Id;
	int  nToolId;

	// 3. 道具类型;
	EQUIP_ITEM_TYPE  itemType;

	tagSmithyInitData()
	{
		memset(this, 0, sizeof(tagSmithyInitData));
	}
}SmithyInitData;

// 装备单条属性数据结构;
typedef struct tagAttrParam
{
	int						nToolId;		// 装备Id;
	SMITHY_EQU_ATTR_TYPE	attrType;		// 属性类型;
	string					strAttrName;	// 名称;
	double					fAttrValue;		// 属性值;
	double					fAttrAdd;		// 增量;

	tagAttrParam()
		: nToolId(-1)
		, attrType(ATTR_TYPE_HP)
		, strAttrName("")
		, fAttrValue(0.0f)
		, fAttrAdd(0.0f)
	{
	}
}AttrParam;

// 装备强化参数;
typedef struct tagQH_Param
{
	// 角色;
	int  nHeroId;

	// 装备;
	int		nEquId;
	string  strEquName;
	TOOL_ITEM_QUA  nEquQua;
	int		nPreLv;
	int		nDstLv;
	int		nMaxLv;

	int		nEquCLId;
	int		nEquIcon;

	// 基础属性;
	vector<AttrParam>  vcBase;

	////////////// 普通强化; //////////////
	int		nCost;

	////////////// 马书强化; //////////////
	int  nExp;
	int  nMaxExp;

	// 本次强化完之后是否还能进阶;
	bool bEnableJJ;

	// 是否已强化至最高等级;
	bool bFinal;

	void  clear()
	{
		nHeroId = -1;
		nEquId = -1;
		strEquName = "";
		nEquQua = TOOL_ITEM_QUA_NONE;
		nPreLv = 1;
		nDstLv = 1;
		nMaxLv = 1;
		vcBase.clear();
		//vcXL.clear();
		nCost = 0;
		nExp = 0;
		nMaxExp = 0;
		nEquCLId = -1;
		nEquIcon = -1;
		bEnableJJ = true;
		bFinal = false;
	}

	tagQH_Param()
	{
		clear();
	}
}QH_Param;

// 装备进阶参数;
typedef struct tagJJ_Param
{
	// 角色;
	int  nHeroId;

	// 装备1;
	int		nEquId;
	int		nEquAdvLv;			// 强化等级;
	TOOL_ITEM_QUA  nEquQua;
	string  strEquName;
	int		nEquCLId;
	int		nEquIcon;

	// 装备2;
	int		nEquDstId;
	int		nEquDstTemplateId;
	int		nEquDstAdvLv;		// 强化等级(进阶后可能会降);
	TOOL_ITEM_QUA  nEquDstQua;
	string  strEquDstName;
	int		nEquDstCLId;
	int		nEquDstIcon;

	// 基础属性;
	vector<AttrParam>  vcBase;

	// 进阶消耗材料<templateId, <拥有数量, 需要数量> >;
	map<int, vector<int> >  mapTools;

	// 马书进阶所需积分，类型Id及数量(竞技场/试炼);
	int		nPvpScore;
	int		nTrialScore;

	// 进阶消耗银币;
	int		nCost;

	// 是否已进阶至顶级;
	bool	bFinal;

	// 本次进阶所需主角等级;
	int		nNeedLv;

	// 进阶消耗金币;
	int		nCostGold;

	void  clear()
	{
		nHeroId = -1;
		nEquId = -1;
		nEquAdvLv = 1;
		nEquQua = TOOL_ITEM_QUA::TOOL_ITEM_QUA_NONE;
		strEquName = "";
		nEquCLId = -1;
		nEquIcon = -1;
		nEquDstId = -1;
		nEquDstTemplateId = -1;
		nEquDstAdvLv = 1;
		nEquDstQua = TOOL_ITEM_QUA::TOOL_ITEM_QUA_NONE;
		strEquDstName = "";
		nEquDstCLId = -1;
		nEquDstIcon = -1;
		vcBase.clear();
		mapTools.clear();
		nPvpScore = 0;
		nTrialScore = 0;
		nCost = 0;
		bFinal = false;
		nNeedLv = 1;
		nCostGold = 0;
	}

	tagJJ_Param()
	{
		clear();
	}
}JJ_Param;

// 装备淬炼参数;
typedef struct tagCL_Param
{
	// 角色;
	int  nHeroId;

	// 装备1;
	int		nEquId;
	int		nEquTemplateId;
	TOOL_ITEM_QUA  nEquQua;
	int		nEquIcon;
	string  strEquName;
	int		nEquCLLevel;
	int		nEquCLId;
	bool	bIsPreFate;
	string	strTianMingDesc;
	int		nEquAdvLv;

	// 装备2;
	int		nEquDstId;
	TOOL_ITEM_QUA  nEquDstQua;
	int		nEquDstIcon;
	string  strEquDstName;
	int		nEquDstCLLevel;
	int		nEquDstCLId;
	bool	bIsDstFate;

	// 基础属性;
	vector<AttrParam>  vcBase;

	// 淬炼消耗材料<templateId, <拥有数量, 需要数量> >;
	map<int, vector<int> >  mapTools;

	// 淬炼降级消耗金币;
	int		nCostGold;

	// 是否淬炼至最高等级;
	bool	bFinal;

	void  clear()
	{
		nHeroId = -1;

		nEquId = -1;
		nEquTemplateId = -1;
		nEquQua = TOOL_ITEM_QUA::TOOL_ITEM_QUA_NONE;
		nEquIcon = -1;
		strEquName = "";
		nEquCLLevel = 0;
		nEquCLId = -1;
		bIsPreFate = false;
		strTianMingDesc = "";
		nEquAdvLv = -1;

		nEquDstId = -1;
		nEquDstQua = TOOL_ITEM_QUA::TOOL_ITEM_QUA_NONE;
		nEquDstIcon = -1;
		strEquDstName = "";
		nEquDstCLLevel = 0;
		nEquDstCLId = -1;
		bIsDstFate = false;

		vcBase.clear();


		mapTools.clear();
		nCostGold = 0;

		bFinal = false;
	}

	bool empty()
	{
		return nHeroId == -1;
	}

	tagCL_Param()
	{
		clear();
	}
}CL_Param;

// 装备洗练参数;
/*
typedef struct tagXL_Param
{
	// 角色;
	int  nHeroId;

	// 装备;
	int		nEquId;
	string  strEquName;
	TOOL_ITEM_QUA  nEquQua;
	int		nEquLv;

	// 洗练属性;
	vector<AttrParam>  vcAttr;

	// 洗练消耗;
	int		nCoin;
	int		nCoinStone;
	int		nGold;
	int		nGoldStone;

	void clear()
	{
		nHeroId = -1;
		nEquId = -1;
		strEquName = "";
		nEquQua = TOOL_ITEM_QUA_NONE;
		nEquLv = 0;
		vcAttr.clear();
		nCoin = 0;
		nCoinStone = 0;
		nGold = 0;
		nGoldStone = 0;
	}

	tagXL_Param()
	{
		clear();
	}
}XL_Param;*/

typedef struct EquipSourceItem
{
	int		dropMethod;
	int		chapterId;			// 章节Id;
	int		barrierId;			// 关卡Id;
	string	barrierName;		// 关卡名字;

	EquipSourceItem()
		: dropMethod(-1)
		, chapterId(-1)
		, barrierId(-1)
		, barrierName("")
	{
	}
}EQUIP_SOURCE_ITEM;

typedef class ToolSourceData
{
public:
	ToolSourceData()
	{
		ntemplateId = 0;		//道具模板ID	
		toolId = 0;				//道具ID	
		heroId = 0;				//角色界面选中的当前英雄
		equipType = 0;
		uiType = 0;
		uiSrc = 0;
	}
	//
	int ntemplateId;	//道具模板ID	
	int toolId;			//道具ID	
	int heroId;			//角色界面选中的当前英雄
	int equipType;
	int uiSrc;			//需要返回到的界面
	int uiType;			//界面显示模块
}TOOL_SOURCE_DATA;

// 铁匠铺装备变化参数;
typedef struct SmithyChangeParam
{
	//
	int ntemplateId;	// 道具模板ID;
	int toolId;			// 道具ID;
	int heroId;			// 角色界面选中的当前英雄;
	int equipType;
}SMITHY_CHANGE_PARAM;

// 强化UI组件;
struct UI_Smithy_QH
{
	Layout*  pnlQH;

	// 装备信息;
	ImageView*  imgEqu;
	Text*		txtEquName;
	Text*		txtLvDesc;
	Text*		txtLvPre;
	ImageView*	imgArrow;
	Text*		txtLvDst;
	Text*		txtMaxLv;

	// 基础属性;
	Text*		txtBaseAttr[2];
	Text*		txtBaseAttrValue[2];
	Text*		txtBaseAttrAdd[2];

	///////////////// 普通强化消耗; ////////////////
	Layout*		pnlQHCost_Normal;
	Text*		txtCostX1;
	Button*		btnQH_1;
	Button*		btnQH_N;

	///////////////// 马书强化消耗; ////////////////
	Layout*		pnlQHCost_HorseBook;
	LoadingBar*	progressBarExp;
	LoadingBar*	progressBarExpPreview;
	Text*		txtExp;
	Layout*		pnlQHMaterial;
	Button*		btnQH_AutoAdd;
	Button*		btnQH_HorseBook;

	UI_Smithy_QH()
	{
		memset(this, 0, sizeof(UI_Smithy_QH));
	}
};

// 进阶UI组件;
struct UI_Smithy_JJ
{
	Layout*  pnlJJ;

	// 预览;
	ImageView*	imgEqu;
	Text*		txtEquName;
	ImageView*	imgEquDst;
	Text*		txtEquDstName;

	// 强化属性(进阶也显示强化属性);
	Text*		txtAttr[2];
	Text*		txtAttrValue[2];
	Text*		txtAttrAdd[2];

	///////////////// 普通进阶消耗 /////////////////;
	Layout*		pnlJJCost_Normal;

	///////////////// 马书进阶消耗 /////////////////;
	Layout*		pnlJJCost_HorseBook;
	ImageView*	imgScoreIcon;
	Text*		txtScore;
	Text*		txtTipArena;
	Text*		txtTipTrail;

	// 已进阶到最高等级;
	Text*		txtNone;

	// 进阶所需等级;
	Text*		txtLv;

	// 进阶消耗银币;
	ImageView*  imgCost;
	Text*		txtCost;

	// 进阶;
	Button*		btnJJ;

	UI_Smithy_JJ()
	{
		memset(this, 0, sizeof(UI_Smithy_JJ));
	}
};

// 淬炼UI组件;
struct UI_Smithy_CL
{
	Layout*		pnlCL;

	// 预览;
	ImageView*	imgEqu;
	Text*		txtEquName;
	ImageView*	imgEquDst;
	Text*		txtEquDstName;

	// 基础属性;
	Text*		txtAttr[2];
	Text*		txtAttrValue[2];
	Text*		txtAttrAdd[2];

	// 天命;
	Text*		txtCLTitle;
	Text*		txtCLDesc;

	// 淬炼消耗;
	Layout*		pnlCLCost;
	Button*		btnCL;

	// 淬炼降级;
	Button*		btnCLDegrade;

	UI_Smithy_CL()
	{
		memset(this, 0, sizeof(UI_Smithy_CL));
	}
};

// 淬炼UI组件;
struct UI_Smithy_CL_Degrade
{
	Layout*		pnlCLDegrade;
	ImageView*	imgBg;
	Text*		txtCostGold;
	Button*		btnOK;
	Button*		btnCancel;
	Button*		btnClose;

	UI_Smithy_CL_Degrade()
	{
		memset(this, 0, sizeof(UI_Smithy_CL_Degrade));
	}
};

// 洗练UI组件;
/*
struct UI_Smithy_XL
{
	Layout*  pnlXL;

	// 装备信息;
	ImageView*  imgEqu;
	Text*		txtEquName;
	Text*		txtEquLv;

	// 洗练属性;
	Layout*		pnlAttr;
	Text*		txtAttr[2];
	Text*		txtAttrPre[2];
	Text*		txtAttrAdd[2];
	Text*		txtAttrMax[2];
	ImageView*	imgAttrStateUp[2];
	ImageView*	imgAttrStateDown[2];
	ImageView*  imgInvalid;

	// 洗练消耗;
	Layout*		pnlCost;
	CheckBox*	cbCoin;
	Text*		txtCoin;
	Text*		txtCoinStone;
	CheckBox*	cbGold;
	Text*		txtGold;
	Text*		txtGoldStone;

	// 洗练;
	Button*		btnXL_1;
	Button*		btnXL_10;
	Button*		btnReplace;

	UI_Smithy_XL()
	{
		memset(this, 0, sizeof(UI_Smithy_XL));
	}
};*/


// 装备获取途径;
typedef struct UI_Smithy_Equip_Source
{
	Layout*				pRoot;
	Layout*				pnlEquipSource;
	Button*				btnClose;
	ImageView*			imgEqu;
	Text*				txtEquName;
	cocos2d::ui::ScrollView*			scrollviewContent;
	CustomScrollBar*	scrollbar;

	UI_Smithy_Equip_Source()
	{
		memset(this, 0, sizeof(UI_Smithy_Equip_Source));
	}
}Smithy_Equip_Source;


// 装备主界面(原铁匠铺);
struct UI_Smithy
{
	Layout*  pRoot;

	// 分页按钮*3;
	CheckBox*	cbPageQH;
	CheckBox*	cbPageCL;

	// 左部分面板;
	Layout*  pnlLeft;
	ImageView*  imgRole;
	Text*		txtRoleName;
	Text*		txtRoleLv;
	Text*		txtFc;
	Text*		txtHP;
	Text*		txtAtk;
	Text*		txtPD;
	Text*		txtMD;

	// 攻击图标;
	ImageView*	imgAtk;

	/*
		1-帽子  2-衣服  3-武器;
		4-耳环  5-战马  6-兵书;
	*/
	ImageView*	imgEqu[6];

	// 右部分面板;
	Layout*  pnlRight;
	UI_Smithy_QH  uiQH;
	UI_Smithy_JJ  uiJJ;
	UI_Smithy_CL  uiCL;
	UI_Smithy_CL_Degrade uiCLDegrade;

	// 穿戴;
	Layout*  pnlWear;

	// 获取途径;
	Layout*  pnlGet;

	UI_Smithy()
	{
		memset(this, 0, sizeof(UI_Smithy));
	}
};

// 金币进阶UI;
typedef struct tagUI_JJ_Gold
{
	Layout*			pRoot;
	ImageView*		imgBg;
	ImageView*		imgContent;

	ImageView*		imgEqu;
	Text*			txtEquName;
	ImageView*		imgEquDst;
	Text*			txtEquDstName;

	// 强化属性(进阶也显示强化属性);
	Text*		txtAttr[2];
	Text*		txtAttrValue[2];
	Text*		txtAttrAdd[2];

	// 进阶消耗金币;
	Text*		txtGold;

	Button*		btnClose;
	Button*		btnJJ;

	tagUI_JJ_Gold()
	{
		memset(this, 0, sizeof(tagUI_JJ_Gold));
	}
}UI_JJ_Gold;


typedef struct tagSelectedMaterialParam
{
	const TOOL_INFO* toolInfo;
	int  nExp;
	int  nLogicOrder;

	bool empty()
	{
		return (toolInfo == nullptr
			&& nExp == 0
			&& nLogicOrder == 0);
	}

	void clear()
	{
		toolInfo = nullptr;
		nExp = 0;
		nLogicOrder = 0;
	}

	bool operator == (const tagSelectedMaterialParam& _param) const 
	{
		return (toolInfo == _param.toolInfo
			&& nExp == _param.nExp
			&& nLogicOrder == _param.nLogicOrder);
	}

	tagSelectedMaterialParam()
	{
		clear();
	}
}SelectedMaterialParam;

#endif // SmithyDef_h__
