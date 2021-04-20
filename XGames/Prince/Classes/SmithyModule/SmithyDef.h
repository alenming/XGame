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

// װ��Ʒ����ɫ;
static Color3B  g_EquipColorWithQua[TOTAL_ITEM_QUA_COUNT] = {
	Color3B(0xFF, 0xFF, 0xFF),		// ��;
	Color3B(0x45, 0xE1, 0x6D),		// ��;
	Color3B(0x36, 0xD1, 0xF2),		// ��;
	Color3B(0xC1, 0x69, 0xFF),		// ��;
	Color3B(0xFF, 0xC1, 0x3B),		// ��;
	Color3B(0xFF, 0x28, 0x23)		// ��;
};

// װ����������;
enum SMITHY_EQU_ATTR_TYPE
{
	ATTR_TYPE_HP,					// Ѫ��;
	ATTR_TYPE_ATK,					// ����;
	ATTR_TYPE_PDD,					// ���;
	ATTR_TYPE_MDD,					// ����;

	ATTR_TYPE_HP_COE,				// Ѫ���ӳ�;
	ATTR_TYPE_ATK_COE,				// �����ӳ�;
	ATTR_TYPE_PDD_COE,				// ����ӳ�;
	ATTR_TYPE_MDD_COE,				// �����ӳ�;

	ATTR_TYPE_CRI_PRB,				// ������;
	ATTR_TYPE_CRI_RES,				// ������;
	ATTR_TYPE_HIT,					// ������;
	ATTR_TYPE_MISS,					// ������;

	// ����������������Ҫ�����߼�����;
	ATTR_TYPE__NUM
};


// �����̲�������;
enum SMITHY_OP_TYPE
{
	SMITHY_OP_TYPE_QH	= 1,		// ǿ��;
	SMITHY_OP_TYPE_JJ	= 2,		// ����;
	SMITHY_OP_TYPE_CL	= 3,		// ����;
	//SMITHY_OP_TYPE_XL	= 3,		// ϴ��;
									// ...
	SMITHY_OP_TYPE_NONE = 99		// ��;
};

// ϴ������;
/*enum SMITHY_XL_TYPE
{
	SMITHY_XL_TYPE_COIN,			// ����ϴ��;
	SMITHY_XL_TYPE_GOLD				// ���ϴ��;
};*/

enum TOOL_SOURCE_UI_ID
{
	TOOL_SOURCE_UI_NONE,
	TOOL_SOURCE_UI_PACK,			// ����;
	TOOL_SOURCE_UI_HERO,			// Ӣ��;
	TOOL_SOURCE_UI_HERO_ADV,		// Ӣ�۽��׽���;
	TOOL_SOURCE_UI_SMITHY_ADV,		// �����̽���;
	TOOL_SOURCE_UI_OTHER
};

enum OBS_PARAM_TYPE_SM
{
	OBS_PARAM_TYPE_EQUIPMENT,				// װ���б��е�Icon����;
	OBS_PARAM_TYPE_EQU_QH_RESULT,			// װ��ǿ�����;
	OBS_PARAM_TYPE_EQU_JJ_RESULT,			// װ�����׽��;
	OBS_PARAM_TYPE_EQU_CL_RESULT,			// װ���������;
	OBS_PARAM_TYPE_EQU_CL_DEGRADE_RESULT	// װ�������������;
};


// ��������ڲ���;
typedef struct tagSmithyInitData
{
	// 1. Ӣ��Id;
	int  nHeroId;

	// 2. ����Id;
	int  nToolId;

	// 3. ��������;
	EQUIP_ITEM_TYPE  itemType;

	tagSmithyInitData()
	{
		memset(this, 0, sizeof(tagSmithyInitData));
	}
}SmithyInitData;

// װ�������������ݽṹ;
typedef struct tagAttrParam
{
	int						nToolId;		// װ��Id;
	SMITHY_EQU_ATTR_TYPE	attrType;		// ��������;
	string					strAttrName;	// ����;
	double					fAttrValue;		// ����ֵ;
	double					fAttrAdd;		// ����;

	tagAttrParam()
		: nToolId(-1)
		, attrType(ATTR_TYPE_HP)
		, strAttrName("")
		, fAttrValue(0.0f)
		, fAttrAdd(0.0f)
	{
	}
}AttrParam;

// װ��ǿ������;
typedef struct tagQH_Param
{
	// ��ɫ;
	int  nHeroId;

	// װ��;
	int		nEquId;
	string  strEquName;
	TOOL_ITEM_QUA  nEquQua;
	int		nPreLv;
	int		nDstLv;
	int		nMaxLv;

	int		nEquCLId;
	int		nEquIcon;

	// ��������;
	vector<AttrParam>  vcBase;

	////////////// ��ͨǿ��; //////////////
	int		nCost;

	////////////// ����ǿ��; //////////////
	int  nExp;
	int  nMaxExp;

	// ����ǿ����֮���Ƿ��ܽ���;
	bool bEnableJJ;

	// �Ƿ���ǿ������ߵȼ�;
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

// װ�����ײ���;
typedef struct tagJJ_Param
{
	// ��ɫ;
	int  nHeroId;

	// װ��1;
	int		nEquId;
	int		nEquAdvLv;			// ǿ���ȼ�;
	TOOL_ITEM_QUA  nEquQua;
	string  strEquName;
	int		nEquCLId;
	int		nEquIcon;

	// װ��2;
	int		nEquDstId;
	int		nEquDstTemplateId;
	int		nEquDstAdvLv;		// ǿ���ȼ�(���׺���ܻή);
	TOOL_ITEM_QUA  nEquDstQua;
	string  strEquDstName;
	int		nEquDstCLId;
	int		nEquDstIcon;

	// ��������;
	vector<AttrParam>  vcBase;

	// �������Ĳ���<templateId, <ӵ������, ��Ҫ����> >;
	map<int, vector<int> >  mapTools;

	// �������������֣�����Id������(������/����);
	int		nPvpScore;
	int		nTrialScore;

	// ������������;
	int		nCost;

	// �Ƿ��ѽ���������;
	bool	bFinal;

	// ���ν����������ǵȼ�;
	int		nNeedLv;

	// �������Ľ��;
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

// װ����������;
typedef struct tagCL_Param
{
	// ��ɫ;
	int  nHeroId;

	// װ��1;
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

	// װ��2;
	int		nEquDstId;
	TOOL_ITEM_QUA  nEquDstQua;
	int		nEquDstIcon;
	string  strEquDstName;
	int		nEquDstCLLevel;
	int		nEquDstCLId;
	bool	bIsDstFate;

	// ��������;
	vector<AttrParam>  vcBase;

	// �������Ĳ���<templateId, <ӵ������, ��Ҫ����> >;
	map<int, vector<int> >  mapTools;

	// �����������Ľ��;
	int		nCostGold;

	// �Ƿ��������ߵȼ�;
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

// װ��ϴ������;
/*
typedef struct tagXL_Param
{
	// ��ɫ;
	int  nHeroId;

	// װ��;
	int		nEquId;
	string  strEquName;
	TOOL_ITEM_QUA  nEquQua;
	int		nEquLv;

	// ϴ������;
	vector<AttrParam>  vcAttr;

	// ϴ������;
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
	int		chapterId;			// �½�Id;
	int		barrierId;			// �ؿ�Id;
	string	barrierName;		// �ؿ�����;

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
		ntemplateId = 0;		//����ģ��ID	
		toolId = 0;				//����ID	
		heroId = 0;				//��ɫ����ѡ�еĵ�ǰӢ��
		equipType = 0;
		uiType = 0;
		uiSrc = 0;
	}
	//
	int ntemplateId;	//����ģ��ID	
	int toolId;			//����ID	
	int heroId;			//��ɫ����ѡ�еĵ�ǰӢ��
	int equipType;
	int uiSrc;			//��Ҫ���ص��Ľ���
	int uiType;			//������ʾģ��
}TOOL_SOURCE_DATA;

// ������װ���仯����;
typedef struct SmithyChangeParam
{
	//
	int ntemplateId;	// ����ģ��ID;
	int toolId;			// ����ID;
	int heroId;			// ��ɫ����ѡ�еĵ�ǰӢ��;
	int equipType;
}SMITHY_CHANGE_PARAM;

// ǿ��UI���;
struct UI_Smithy_QH
{
	Layout*  pnlQH;

	// װ����Ϣ;
	ImageView*  imgEqu;
	Text*		txtEquName;
	Text*		txtLvDesc;
	Text*		txtLvPre;
	ImageView*	imgArrow;
	Text*		txtLvDst;
	Text*		txtMaxLv;

	// ��������;
	Text*		txtBaseAttr[2];
	Text*		txtBaseAttrValue[2];
	Text*		txtBaseAttrAdd[2];

	///////////////// ��ͨǿ������; ////////////////
	Layout*		pnlQHCost_Normal;
	Text*		txtCostX1;
	Button*		btnQH_1;
	Button*		btnQH_N;

	///////////////// ����ǿ������; ////////////////
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

// ����UI���;
struct UI_Smithy_JJ
{
	Layout*  pnlJJ;

	// Ԥ��;
	ImageView*	imgEqu;
	Text*		txtEquName;
	ImageView*	imgEquDst;
	Text*		txtEquDstName;

	// ǿ������(����Ҳ��ʾǿ������);
	Text*		txtAttr[2];
	Text*		txtAttrValue[2];
	Text*		txtAttrAdd[2];

	///////////////// ��ͨ�������� /////////////////;
	Layout*		pnlJJCost_Normal;

	///////////////// ����������� /////////////////;
	Layout*		pnlJJCost_HorseBook;
	ImageView*	imgScoreIcon;
	Text*		txtScore;
	Text*		txtTipArena;
	Text*		txtTipTrail;

	// �ѽ��׵���ߵȼ�;
	Text*		txtNone;

	// ��������ȼ�;
	Text*		txtLv;

	// ������������;
	ImageView*  imgCost;
	Text*		txtCost;

	// ����;
	Button*		btnJJ;

	UI_Smithy_JJ()
	{
		memset(this, 0, sizeof(UI_Smithy_JJ));
	}
};

// ����UI���;
struct UI_Smithy_CL
{
	Layout*		pnlCL;

	// Ԥ��;
	ImageView*	imgEqu;
	Text*		txtEquName;
	ImageView*	imgEquDst;
	Text*		txtEquDstName;

	// ��������;
	Text*		txtAttr[2];
	Text*		txtAttrValue[2];
	Text*		txtAttrAdd[2];

	// ����;
	Text*		txtCLTitle;
	Text*		txtCLDesc;

	// ��������;
	Layout*		pnlCLCost;
	Button*		btnCL;

	// ��������;
	Button*		btnCLDegrade;

	UI_Smithy_CL()
	{
		memset(this, 0, sizeof(UI_Smithy_CL));
	}
};

// ����UI���;
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

// ϴ��UI���;
/*
struct UI_Smithy_XL
{
	Layout*  pnlXL;

	// װ����Ϣ;
	ImageView*  imgEqu;
	Text*		txtEquName;
	Text*		txtEquLv;

	// ϴ������;
	Layout*		pnlAttr;
	Text*		txtAttr[2];
	Text*		txtAttrPre[2];
	Text*		txtAttrAdd[2];
	Text*		txtAttrMax[2];
	ImageView*	imgAttrStateUp[2];
	ImageView*	imgAttrStateDown[2];
	ImageView*  imgInvalid;

	// ϴ������;
	Layout*		pnlCost;
	CheckBox*	cbCoin;
	Text*		txtCoin;
	Text*		txtCoinStone;
	CheckBox*	cbGold;
	Text*		txtGold;
	Text*		txtGoldStone;

	// ϴ��;
	Button*		btnXL_1;
	Button*		btnXL_10;
	Button*		btnReplace;

	UI_Smithy_XL()
	{
		memset(this, 0, sizeof(UI_Smithy_XL));
	}
};*/


// װ����ȡ;��;
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


// װ��������(ԭ������);
struct UI_Smithy
{
	Layout*  pRoot;

	// ��ҳ��ť*3;
	CheckBox*	cbPageQH;
	CheckBox*	cbPageCL;

	// �󲿷����;
	Layout*  pnlLeft;
	ImageView*  imgRole;
	Text*		txtRoleName;
	Text*		txtRoleLv;
	Text*		txtFc;
	Text*		txtHP;
	Text*		txtAtk;
	Text*		txtPD;
	Text*		txtMD;

	// ����ͼ��;
	ImageView*	imgAtk;

	/*
		1-ñ��  2-�·�  3-����;
		4-����  5-ս��  6-����;
	*/
	ImageView*	imgEqu[6];

	// �Ҳ������;
	Layout*  pnlRight;
	UI_Smithy_QH  uiQH;
	UI_Smithy_JJ  uiJJ;
	UI_Smithy_CL  uiCL;
	UI_Smithy_CL_Degrade uiCLDegrade;

	// ����;
	Layout*  pnlWear;

	// ��ȡ;��;
	Layout*  pnlGet;

	UI_Smithy()
	{
		memset(this, 0, sizeof(UI_Smithy));
	}
};

// ��ҽ���UI;
typedef struct tagUI_JJ_Gold
{
	Layout*			pRoot;
	ImageView*		imgBg;
	ImageView*		imgContent;

	ImageView*		imgEqu;
	Text*			txtEquName;
	ImageView*		imgEquDst;
	Text*			txtEquDstName;

	// ǿ������(����Ҳ��ʾǿ������);
	Text*		txtAttr[2];
	Text*		txtAttrValue[2];
	Text*		txtAttrAdd[2];

	// �������Ľ��;
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
