#ifndef HeroRelationDataDef_h__
#define HeroRelationDataDef_h__

#include "iostream"
using namespace std;

//系统表中英雄缘分激活条件ID
#define HERO_RELATION_LIMIT_ID	10138

//淬炼前道具资源ID计算规则： 英雄templateId*100 + 基数
//淬炼前马书道具资源ID 基数
#define HORSE_ICON_ID_BASE	51

//淬炼前兵书道具资源ID
#define BOOK_ICON_ID_BASE	61

//淬炼前武器道具资源ID
#define WEAPON_ICON_ID_BASE	31

//缘分属性
typedef struct fateAttri
{
	int fateId;
	int fateType;
	int fateLevel;
	int fateHeroId;			//单向发起缘分的英雄ID
	string fateObjName;		//单向发起缘分的对象名称
	
	float attriValue;

	bool isFateActed;		//缘分是否激活
	bool isFateMaxLv;		//是否达到缘分最高级

	string leftSkillName;
	string rightSkillName;

	string attriEnName;
	string attriChName;
	string attriDes;		//描述1
	string attriDes2;		//描述2
	string fateName;		//缘分名称
}sFateAttri;

//赋予缘分的英雄id和类型
//缘分的激活条件和顺序
typedef struct fateHeroOrder
{
	int valueData;
	int order;
}sFateHeroOrder;

#endif