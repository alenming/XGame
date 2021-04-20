#ifndef ToolDef_h__
#define ToolDef_h__

#include "cocos2d.h"
#include <string>
#include <vector>
#include <map>
using namespace std;
using namespace cocos2d;
enum TOOL_TYPE
{	
	TOOL_TYPE_NONE,
	TOOL_TYPE_CONSUME = 1,		//1-消耗品
	TOOL_TYPE_EQUIP,			//2-装备
	TOOL_TYPE_HERO,				//3-英雄
	TOOL_TYPE_HERO_CHIP,		//4-英雄
	TOOL_TYPE_MATERIAL,			//5-材料
	TOOL_TYPE_ALL = 20			//全部
};

enum TOOL_ITEM_TYPE
{		
	TOOL_ITEM_TYPE_NONE,
	TOOL_ITEM_TYPE_CONSUME,		//1-消耗道具
	TOOL_ITEM_TYPE_EQUIP,		//2-装备道具
	TOOL_ITEM_TYPE_HERO_CHIP,	//3-英雄碎片
	TOOL_ITEM_TYPE_HERO,		//4-英雄
	TOOL_ITEM_TYPE_SKILL,		//5-技能
	TOOL_ITEM_TYPE_MATERIAL,	//6-装备进阶材料
	TOOL_ITEM_TYPE_CHEST,		//7-宝箱道具
	TOOL_ITEM_TYPE_BWCHIP,		//8-宝物碎片
	TOOL_ITEM_TYPE_GIFT,		//9-礼物
	TOOL_ITEM_TYPE_STAR_SPRITE,	//10-星灵
	TOOL_ITEM_TYPE_STAR_SPRITECHIP,	//11-星灵碎片
	TOOL_ITEM_TYPE_ALL=20		//20-全部
};

enum TOOL_ITEM_QUA
{	
	TOOL_ITEM_QUA_NONE,
	TOOL_ITEM_QUA_WHITE,	//白
	TOOL_ITEM_QUA_GREEN,	//绿
	TOOL_ITEM_QUA_BLUE,		//蓝
	TOOL_ITEM_QUA_PURPLE,	//紫
	TOOL_ITEM_QUA_ORANGE,	//橙
	TOOL_ITEM_QUA_RED		//红
		
};

enum EQUIP_ITEM_TYPE
{		
	EQUIP_ITEM_TYPE_NONE,
	EQUIP_ITEM_TYPE_HAT,		//1-帽子
	EQUIP_ITEM_TYPE_CLOTHES,	//2-衣服
	EQUIP_ITEM_TYPE_WEAPON,		//3-武器
	EQUIP_ITEM_TYPE_EARRINGS,	//4-耳环
	EQUIP_ITEM_TYPE_HORSE,		//5-战马
	EQUIP_ITEM_TYPE_BOOK,		//6-兵书
	EQUIP_ITEM_TYPE_ALL
};

//第一条激活属性类型
//	1-攻击力
//	2-法防
//	3-物防
//	4-血量
//	5-暴击
//	6-暴伤
//	7-连击
//	8-闪避
enum TOOL_ITEM_ATTRIBUTE_TYPE
{
	TOOL_ITEM_ATTRIBUTE_NONE,
	TOOL_ITEM_ATTRIBUTE_ATK,	
	TOOL_ITEM_ATTRIBUTE_MDD,
	TOOL_ITEM_ATTRIBUTE_PDD,
	TOOL_ITEM_ATTRIBUTE_HP,
	TOOL_ITEM_ATTRIBUTE_CRIPRB,
	TOOL_ITEM_ATTRIBUTE_CRICOE,
	TOOL_ITEM_ATTRIBUTE_CTRPRB,
	TOOL_ITEM_ATTRIBUTE_MISPRD
};

enum Smithy_Event_ID
{	
	Smithy_Event_NONE,
	Smithy_Event_QH,
	Smithy_Event_JJ,
	Smithy_Event_XL,
	Smithy_Event_BW,
	Smithy_Event_CD //穿戴
};
enum Tool_Select_State
{
	Tool_Select_State_Single=1,
	Tool_Select_State_Multiple
};
enum Tool_Select_Type
{
	Tool_Select_None,
	Tool_Select_Consume,
	Tool_Select_Hero,
	Tool_Select_Hero_Replace,		//角色：替换英雄
	Tool_Select_Equip,
	Tool_Select_Equip_HAT,			//帽子
	Tool_Select_Equip_CLOTHES,
	Tool_Select_Equip_WEAPON,
	Tool_Select_Equip_EARRINGS,
	Tool_Select_Equip_HORSE,
	Tool_Select_Equip_BOOK,
	Tool_Select_Equip_Horse_book,
	Tool_Select_Skill,
	Tool_Select_Skill_NORMAL,		//1、普通技能
	Tool_Select_Skill_ATIVE,		//2、主动技能
	Tool_Select_Skill_PASSIVITY1,	//3、被动技能1
	Tool_Select_Skill_PASSIVITY2,   //4、被动技能2
	Tool_Select_Skill_PASSIVITY3,	//5、被动技能3
	Tool_Select_Hero_Skill,
	Tool_Select_Other
};
//道具出售方式
enum TOOL_ITEM_SALE_TYPE
{
	TOOL_ITEM_SALE_NONE,
	TOOL_ITEM_SALE_GOLD,		//1金币出售
	TOOL_ITEM_SALE_COIN,		//2银币出售
	TOOL_ITEM_SALE_SOCIETY,		//3公会声望出售
	TOOL_ITEM_SALE_ARENA_SCORE,	//4竞技场积分
	TOOL_ITEM_SALE_MAZE_SCORE,	//5迷宫积分
	TOOL_ITEM_SALE_UNSALE = 9	//9不可出售
};

//道具掉落途径
enum TOOL_DROP_SOURCE
{
	TOOL_DROP_SOURCE_NONE,
	TOOL_DROP_SOURCE_SHOP,			//	1-商城
	TOOL_DROP_SOURCE_ROB,			//	2-掠夺
	TOOL_DROP_SOURCE_ACTIVE,		//	3-活动
	TOOL_DROP_SOURCE_TOWER,			//	4-千层塔
	TOOL_DROP_SOURCE_TRIAL,			//	5-试练
	TOOL_DROP_SOURCE_RISK,			//	6-冒险
	TOOL_DROP_SOURCE_STAGE,			//	7-关卡
	TOOL_DROP_SOURCE_LOVE,			//	8-情缘
	TOOL_DROP_SOURCE_STAR_PLATE,	//	9-星盘(暂不支持，会出bug)
	TOOL_DROP_SOURCE_MAZE,			//	10-迷宫
	TOOL_DROP_SOURCE_PVP,			//	11-竞技场
	TOOL_DROP_SOURCE_SHOP_BUY,		//	12-商城(购买)
	TOOL_DROP_SOURCE_REFINE,		//	13-炼化炉
	TOOL_DROP_SOURCE_GROUP,			//	14-帮派
	TOOL_DROP_SOURCE_CL_SHOP,		//	15-淬炼商店
	TOOL_DROP_SOURCE_FIRST_RECHARGE,//  16-首冲奖励
	TOOL_DROP_SOURCE_VIP_PRIVILEGE,	//  17-VIP特权礼包
	TOOL_DROP_SOURCE_OPERATION,		//  18-运营活动
	TOOL_DROP_SOURCE_DRUGSTORE,		//  19-杂货商店
	TOOL_DROP_SOURCE_TRAIL_TREASURE,//  20-试炼宝藏
	TOOL_DROP_SOURCE_SECRET_SHOP	//  21-神秘商店
};

//道具类型id
enum TOOL_DROP_ID_TYPE
{
	TOOL_ID_DEFAULT,
	TOOL_ID_GOLD = 11001,
	TOOL_ID_COIN = 11002
};

typedef struct tagTOOL_INFO
{		
	int nadvLvl;		//强化等级
	
	int nid;			//道具ID
	int nwarePetId;		//该装备是否穿上，穿上则不算占堆叠数。0-未穿上，非0则表示当前穿到哪个英雄身上
	int ntemplateId;	//物品模版id
	int nstack;			//物品的堆叠数。itemTyp为装备道具时stack为1，其他类型的道具可以叠加

	//英雄特有字段
	//int	nExp;			//英雄当前经验
	int nBattleFlag;	//0:未上阵,1:已上阵, 2：小伙伴
	int nRoleType;

	//本地数据
	string strName;
	int nResId;//资源id
	int nItemType;//分类
	int nLocat;//显示分类
	int nMaxSta;//占用一个格子空间可放置的道具数量
	int nItemQua;//品质
	int nItemStars;	//装备星级
	int nEquipType; //装备类型，如果不是装备，该字段为-1

	// 强化经验值(马书);
	int nQHExp;

	// 淬炼等级;
	int nCLLevel;

	//
	tagTOOL_INFO()
	{
		nadvLvl = 0;		//强化等级
		nid = 0;			//道具ID
		nwarePetId = 0;		//该装备是否穿上，穿上则不算占堆叠数。0-未穿上，非0则表示当前穿到哪个英雄身上
		ntemplateId = 0;	//物品模版id
		nstack = 0;			//物品的堆叠数。itemTyp为装备道具时stack为1，其他类型的道具可以叠加
		//英雄特有字段
		//nExp = 0;			//英雄当前经验
		nBattleFlag = 0;	//0:未上阵,1:已上阵, 2：小伙伴
		nRoleType = 0;
		//本地数据
		strName = "";
		nResId = 0;//资源id
		nItemType = 0;//分类
		nLocat = 0;//显示分类
		nMaxSta = 0;//占用一个格子空间可放置的道具数量
		nItemQua = 0;//品质系数
		nItemStars = 0;
		nEquipType = -1;
		nQHExp = 0;
		nCLLevel = 0;
	}
}TOOL_INFO;

typedef std::vector<TOOL_INFO*> TOOL_INFO_VECTOR;

//表数据, 通过id字段索引
typedef map<int, TOOL_INFO> TOOL_INFO_MAP;

//使用道具结果
typedef struct tagUseTOOLRESULTItem
{	
	int namount;	//
	string strStat;	//状态 I增加 D删除 U修改
	int nid;
	int ntemplateId;
}USETOOLRESULTITEM;
typedef std::vector<USETOOLRESULTITEM> USETOOLRESULTITEM_VECTOR;
//使用道具结果
typedef struct tagUseTOOLRESULT
{	
	int ngold;	//金币
	int nlevel;	//等级
	int nenergy;//能量
	int npower;	//体力
	int nexp;	//经验值
	int ncoin;	//银币

	USETOOLRESULTITEM_VECTOR vecItems;
}USE_TOOL_RESULT;

typedef struct tagTOOL_Change_INFO
{	
	int nid;
	int ntemplateId;	//物品模版id
	int nstack;			//物品的堆叠数。itemTyp为装备道具时stack为1，其他类型的道具可以叠加
	int nadvLvl;		//强化等级

	bool  bIsCrit;		// 强化时是否发生暴击;

	tagTOOL_Change_INFO()
	{
		memset(this, 0, sizeof(tagTOOL_Change_INFO));
	}

}TOOL_CHANGE_INFO;

//宝箱
typedef struct BOX_TOOL_INFO
{
	int nid;
	int num;

	BOX_TOOL_INFO()
	{
		memset(this, 0, sizeof(BOX_TOOL_INFO));
	}
}sBOX_TOOL_INFO;

//新增加的道具
typedef struct tagTOOL_New_INFO
{	
	int nid;
	int ntemplateId;	//物品模版id
	int nstack;			//物品的堆叠数。itemTyp为装备道具时stack为1，其他类型的道具可以叠加
}TOOL_NEW_INFO;

class LayerToolObserver
{
public:
	virtual void onLayerToolClose(Ref* sender) = 0;
	virtual void onSelectConfirm() = 0;
};

class LayerHeroToolObserver
{
public:
	virtual void onSelectHeroConfirm(const int& id) = 0;
};

class LayerSmithyObserver
{
public:
	virtual void onLayerSmithyClose() = 0;
};

#endif // ToolDef_h__
