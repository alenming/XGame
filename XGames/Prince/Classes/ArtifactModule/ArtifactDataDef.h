#ifndef ArtifactDataDef_h__
#define ArtifactDataDef_h__

#include "iostream"

using namespace std;

//系统配置表用到的id
const int RefineCostCoin = 10000;
const int RefineCostGold = 10001;
const int RefineCostStore = 10057;

//神器战斗属性类型
typedef enum AttriType
{
	eTYPE_DEFAULT,
	eTYPE_ATK,
	eTYPE_HP,
	eTYPE_PHYDEFENSE,		//物防
	eTYPE_MGCDEFENSE,		//法防
	eTYPE_CRITPROB,
	eTYPE_UNCRITPROB,
	eTYPE_CRITDAMAGE,
	eTYPE_UNCRITDAMAGE,
	eTYPE_MISS,
	eTYPE_HIT,
	eTYPE_COMBOPROB
}eAttriType;

//神器战斗属性
typedef struct FightAttri
{
	float fValue;
	float fMaxValue;
	float fTempValue;				//洗练后的临时值
	string strChName;				//中文名称用来展示
	string strEnName;				//英文名称用来读表
	eAttriType eType;

	FightAttri()
		: fValue(0.0f), fMaxValue(0.0f), fTempValue(0)
		, strChName(""), strEnName(""), eType(eTYPE_DEFAULT) {}

}sFightAttri;

//神器普通参数属性
typedef struct CommonAttri
{
	//神器序列号
	int m_iOrderNum;

	//激活状态
	bool m_bIsActived;

	//是否洗练满属性
	bool m_bIsRefineAll;

	CommonAttri()
		: m_iOrderNum(0)
		, m_bIsActived(false), m_bIsRefineAll(false) {}

}sCommonAttri;

//神器激活条件
typedef struct Condition
{
	//是否有二级条件, 比如：5个5星英雄
	bool isExtra;

	//目标数量
	int m_iGoalNum;

	//二级目标数量
	int m_iExtraGoalNum;

	//已达成数量
	int m_iDoneNum;

	//条件类型
	int m_iType;

	//是否显示条件进度
	bool m_bIsShowProgress;

	//条件是否达成
	bool m_bIsDone;

	//是否是星宿等级条件...注.由于服务器会额外下发星宿等级的条件，所以此条件特殊处理，读取服务器字段，其它条件读取本地数据
	bool m_bIsStarSu;

	//条件描述
	string m_strDes;

	Condition()
		: m_iGoalNum(0), m_iDoneNum(0), m_iType(0)
		, isExtra(false), m_iExtraGoalNum(0), m_bIsDone(false)
		, m_bIsShowProgress(true), m_bIsStarSu(false), m_strDes("") {}

}sCondition;

//洗练方式
typedef enum RefineMethod
{
	eMETHOD_COMMON,		//普通洗练	
	eMETHOD_ADVANCE		//高级洗练

}eRefineMethod;

//洗练消耗
typedef struct RefineCost
{
	int m_iCostStore;
	int m_iCostCoin;
	int m_iCostGold;

	RefineCost()
		: m_iCostStore(0), m_iCostCoin(0), m_iCostGold(0) {}
}sRefineCost;

#endif