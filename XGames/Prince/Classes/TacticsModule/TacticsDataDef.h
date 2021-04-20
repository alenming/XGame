#ifndef TacticsDataDef_h__
#define TacticsDataDef_h__

#include <iostream>
using namespace std;

//阵法数量
#define NUM_TACTICS_LIST		7
//排最高等级
#define ROW_MAX_LEVEL			50
//阵法最高等级
#define TACTICS_MAX_LEVEL		16

//阵法精炼公用材料ID
#define TACTICS_UPGRADE_MATERIAL_ID		10094

//排强化材料列表所在的系统表ID
#define ROW_UPGRADE_MATERIAL_LIST_ID	10093

//排强化材料列表所在的系统表ID
#define ROW_UPGRADE_MATERIAL_LIST_ID1	10105

typedef struct attriName
{
	string en_keyName;		//表格对应的关键字
	string ch_name;			//中文名
	string icon_path;		//图标路径
}sAttriName;

// 属性类型;
enum ATTR_TYPE
{
	ATTR_KIND_QI_XING,				// 七星阵法;		血量
	ATTR_KIND_TIAN_GANG,			// 天罡阵法;		防御
	ATTR_KIND_CI_HUN,				// 刺魂阵法;		攻击

	ATTR_KIND_HE_YI,				// 鹤翼阵法;		暴击
	ATTR_KIND_FEI_YU,				// 飞鱼阵法;		命中
	ATTR_KIND_JIAN_XING,			// 剑型阵法;		闪避
	ATTR_KIND_TIAN_MEN				// 天门阵法;		连击
};

//阵法数据
typedef struct tacticsData
{
	//行ID
	int mRowId;

	//阵法名
	string mName;

	//阵法图标对应ID
	int mIconId;

	//升级条件(各排等级条件)
	vector<int> mLvUpLimit;

	//阵法等级
	int mLevel;

	//阵法属性/数值
	sAttriName mAttriName;
	ATTR_TYPE mAttrType;
	float mAttrValue;
	float mNextAttrVal;				//下一级增加的属性

	int mLvUpMaterialId1;			//升级所需材料1ID
	int mLvUpMaterialCnt1;			//升级所需材料1数量
	int mLvUpMaterialId2;			//升级所需材料2ID
	int mLvUpCoin;					//升级所需银币

	//是否激活状态
	bool mIsActivated;	

	//是否正在使用
	bool mIsUsing;

}sTacticsData;

//各排当前数据
typedef struct curRowData
{
	//排经验
	int mRowCurExp;

	//排类型（前、中、后）
	int mRowType;

	//排ID
	int mRowID;

	//排升级条件(表格中limit对应排需要达到的等级)
	int mRowLvLimit;

	//排等级
	int mRowLevel;

	//排属性加成
	int mRowRangeHp;					//排血量增量
	int mRowRangeAtk;					//排攻击增量
	int mRowRangePhyDef;				//排物防增量
	int mRowRangeMagicDef;				//排法防增量

	int mRowLvUpExp;					//升级所需经验
}sCurRowData;

//排强化材料数据
typedef struct rowUpMaterial
{
	int amount;
	int id;
}sRowUpMaterial;

typedef struct rowUpMaterialPrice
{
	int id;
	int costCoin;
}sRowUpMaterialPrice;

enum eRowType
{
	FRONT_ROW_TYPE,
	MIDDLE_ROW_TYPE,
	BACK_ROW_TYPE
};
#endif