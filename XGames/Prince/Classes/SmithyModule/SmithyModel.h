#ifndef SmithyModel_h__
#define SmithyModel_h__

#include "SmithyDef.h"
#include "Utils/Observer.h"

class SmithyModel : public Observable
{
public:
	static SmithyModel* getInstance();
	static void destroyInstance();

private:

	SmithyModel();
	~SmithyModel();

public:

	// 保存当前选中的英雄;
	//void  setCurHeroId(const int nHeroId) { m_nCurHeroId = nHeroId; };

	// 获取装备强化参数;
	void  getQHParam(const int nToolId, QH_Param& param);

	// 获取装备进阶参数;
	void  getJJParam(const int nToolId, JJ_Param& param);

	// 获取装备淬炼参数;
	void  getCLParam(const int nToolId, EQUIP_ITEM_TYPE itemType, CL_Param& param);

	// 查询装备是否可淬炼;
	int   isEquCLValid(const int nToolId);

	// 查询装备类型;
	EQUIP_ITEM_TYPE  getEquType(const int nToolId);

	// 查询装备是否可进阶;
	int   isEquJJValid(const int nEquId);

	// 关卡是否通过，金币进阶前置条件;
	bool  isBarrierPassedWithTemplateId(int nToolTemplateId);
	vector<int> getBarrierListByTemplateId(int nToolTemplateId);

	// 查询装备是否可强化(只可查询强化1次的条件是否满足);
	int   isEquQHValid(const int nEquId);

	// 装备强化结果;
	void  onEquQHResult(const TOOL_CHANGE_INFO* pChangeInfo);

	// 装备进阶结果;
	void  onEquJJResult(const TOOL_CHANGE_INFO* pChangeInfo);

	// 装备进阶结果;
	void  onEquCLResult(const TOOL_CHANGE_INFO* pChangeInfo, OBS_PARAM_TYPE_SM clType);

	// 获取马书强化材料(4个以下返回实际需要数量，4个以上返回所有可用材料);
	vector<const TOOL_INFO*>  getQHMaterial_HorseBook(const int nToolId, int nNeedCount, vector<const TOOL_INFO*>* vcExcept = nullptr);

	void  clearEquipChangeParam();
	void  setEquipChangeParam(const SMITHY_CHANGE_PARAM& changeParam);
	const SMITHY_CHANGE_PARAM&  getEquipChangeParam();

	int  calcCLId(const int nToolId);

	///////////// 外部查询接口: 装备是否可强化/进阶/淬炼 ///////////
	bool  checkEquState(const int nToolId);

	// 检查道具需要强化或是进阶;
	SMITHY_OP_TYPE  checkOpType(const int nToolId);

	//////////////////////// 装备来源; /////////////////////////////

	// 加载装备来源数据;
	void  loadItemSource(const int nToolTemplateId);
	void  loadGuanqiaItems(const int nToolTemplateId);
	void  clearEquipSourceItems();
	vector<EQUIP_SOURCE_ITEM>& getEquipSourceItems();

private:

	// 更新UI;
	void  updateUI(OBS_PARAM_TYPE_SM type, void* data);

	// 根据字段名获取显示UI名称;
	string getUINameByAttrType(SMITHY_EQU_ATTR_TYPE attrType);

	// 计算强化1次的消耗;
	int    getQH_X1_Cost(const int nToolId);

	/********************************************************************
	*  函数名称: getEquAttrType;
	*  功能描述: 确定装备的属性类别(以装备表为准);
	*  参数列表: [in]nToolId : 装备Id;
	*			 [in]vcAttrType: 属性类型;
	*  返回值  : (void);
	********************************************************************/
	void   getEquAttrType( const int nToolId, vector<SMITHY_EQU_ATTR_TYPE>& mapAttrType);

	/********************************************************************
	*  函数名称: getBase_Add;
	*  功能描述: 读取装备属性[基础值]和[增量]，以操作类型参数来区分表;
	*  参数列表: [in]nToolId : 装备Id;
	*			 [in]dataType: 操作类型;
	*			 [in]attrType: 属性类型;
	*			 [out]fBaseValue: 基础值;
	*			 [out]fAddValue: 增量;
	*  返回值  : (void);
	********************************************************************/
	void  getBase_Add(int nToolId, SMITHY_OP_TYPE dataType, SMITHY_EQU_ATTR_TYPE attrType, float& fBaseValue, float& fAddValue);
	void  getBase_Add_ByTemplateId(int nToolTemplateId, SMITHY_OP_TYPE dataType, SMITHY_EQU_ATTR_TYPE attrType, float& fBaseValue, float& fAddValue);

	/********************************************************************
	*  函数名称: calcJJLevel;
	*  功能描述: 计算装备进阶后的强化等级;
	*  参数列表: [in]nToolId : 需要进阶的装备Id;
	*  返回值  : (int)新的强化等级;
	********************************************************************/
	int   calcJJLevel(const int nToolId);

	/********************************************************************
	*  函数名称: calcBaseAttrValue;
	*  功能描述: 计算装备属性总的基础值(3个操作界面的基础值为同一个);
	*  参数列表: [in]nToolId : 装备Id;
	*			*[in]nTemplateId: 装备的templateId(仅适用于进阶后的装备);
	*			 [in]attrType: 属性类型;
	*  返回值  : (float)该类型属性的基础值;
	********************************************************************/
	float  calcBaseAttrValue(const int nToolId, SMITHY_EQU_ATTR_TYPE attrType, int nQHLv);
	float  calcBaseAttrValue_JJDstEqu(const int nTemplateId, SMITHY_EQU_ATTR_TYPE attrType, int nQHLv, int nCLId);
	float  calcBaseAttrValue_CL(const int nToolId, SMITHY_EQU_ATTR_TYPE attrType, int nCLLevel);
	float  calcBaseAttrValueByFormula(float fEquBaseValue, float fEquAddValue, float fCLBaseValue, float fCLAddValue, int nQHLv);
	
	int  calcCLId(const TOOL_INFO* toolInfo, int nCLLevel);

private:

	static SmithyModel* mInstance;

	// 装备来源;
	int  m_nEquipSourceToolTemplateId;
	vector<EQUIP_SOURCE_ITEM>  m_vecEquipSourceItems;

	///////////////////////////////////////////////////////////////////////////////

	vector<int> m_vecSelectedToolId;

	//选择的装备类型
	EQUIP_ITEM_TYPE				m_selectEquipType;

	//保存装备变化值
	vector<TOOL_CHANGE_INFO> m_vecUpdateTools;

	SMITHY_CHANGE_PARAM m_changeParam;
};

#endif //
