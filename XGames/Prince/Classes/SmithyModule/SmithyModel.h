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

	// ���浱ǰѡ�е�Ӣ��;
	//void  setCurHeroId(const int nHeroId) { m_nCurHeroId = nHeroId; };

	// ��ȡװ��ǿ������;
	void  getQHParam(const int nToolId, QH_Param& param);

	// ��ȡװ�����ײ���;
	void  getJJParam(const int nToolId, JJ_Param& param);

	// ��ȡװ����������;
	void  getCLParam(const int nToolId, EQUIP_ITEM_TYPE itemType, CL_Param& param);

	// ��ѯװ���Ƿ�ɴ���;
	int   isEquCLValid(const int nToolId);

	// ��ѯװ������;
	EQUIP_ITEM_TYPE  getEquType(const int nToolId);

	// ��ѯװ���Ƿ�ɽ���;
	int   isEquJJValid(const int nEquId);

	// �ؿ��Ƿ�ͨ������ҽ���ǰ������;
	bool  isBarrierPassedWithTemplateId(int nToolTemplateId);
	vector<int> getBarrierListByTemplateId(int nToolTemplateId);

	// ��ѯװ���Ƿ��ǿ��(ֻ�ɲ�ѯǿ��1�ε������Ƿ�����);
	int   isEquQHValid(const int nEquId);

	// װ��ǿ�����;
	void  onEquQHResult(const TOOL_CHANGE_INFO* pChangeInfo);

	// װ�����׽��;
	void  onEquJJResult(const TOOL_CHANGE_INFO* pChangeInfo);

	// װ�����׽��;
	void  onEquCLResult(const TOOL_CHANGE_INFO* pChangeInfo, OBS_PARAM_TYPE_SM clType);

	// ��ȡ����ǿ������(4�����·���ʵ����Ҫ������4�����Ϸ������п��ò���);
	vector<const TOOL_INFO*>  getQHMaterial_HorseBook(const int nToolId, int nNeedCount, vector<const TOOL_INFO*>* vcExcept = nullptr);

	void  clearEquipChangeParam();
	void  setEquipChangeParam(const SMITHY_CHANGE_PARAM& changeParam);
	const SMITHY_CHANGE_PARAM&  getEquipChangeParam();

	int  calcCLId(const int nToolId);

	///////////// �ⲿ��ѯ�ӿ�: װ���Ƿ��ǿ��/����/���� ///////////
	bool  checkEquState(const int nToolId);

	// ��������Ҫǿ�����ǽ���;
	SMITHY_OP_TYPE  checkOpType(const int nToolId);

	//////////////////////// װ����Դ; /////////////////////////////

	// ����װ����Դ����;
	void  loadItemSource(const int nToolTemplateId);
	void  loadGuanqiaItems(const int nToolTemplateId);
	void  clearEquipSourceItems();
	vector<EQUIP_SOURCE_ITEM>& getEquipSourceItems();

private:

	// ����UI;
	void  updateUI(OBS_PARAM_TYPE_SM type, void* data);

	// �����ֶ�����ȡ��ʾUI����;
	string getUINameByAttrType(SMITHY_EQU_ATTR_TYPE attrType);

	// ����ǿ��1�ε�����;
	int    getQH_X1_Cost(const int nToolId);

	/********************************************************************
	*  ��������: getEquAttrType;
	*  ��������: ȷ��װ�����������(��װ����Ϊ׼);
	*  �����б�: [in]nToolId : װ��Id;
	*			 [in]vcAttrType: ��������;
	*  ����ֵ  : (void);
	********************************************************************/
	void   getEquAttrType( const int nToolId, vector<SMITHY_EQU_ATTR_TYPE>& mapAttrType);

	/********************************************************************
	*  ��������: getBase_Add;
	*  ��������: ��ȡװ������[����ֵ]��[����]���Բ������Ͳ��������ֱ�;
	*  �����б�: [in]nToolId : װ��Id;
	*			 [in]dataType: ��������;
	*			 [in]attrType: ��������;
	*			 [out]fBaseValue: ����ֵ;
	*			 [out]fAddValue: ����;
	*  ����ֵ  : (void);
	********************************************************************/
	void  getBase_Add(int nToolId, SMITHY_OP_TYPE dataType, SMITHY_EQU_ATTR_TYPE attrType, float& fBaseValue, float& fAddValue);
	void  getBase_Add_ByTemplateId(int nToolTemplateId, SMITHY_OP_TYPE dataType, SMITHY_EQU_ATTR_TYPE attrType, float& fBaseValue, float& fAddValue);

	/********************************************************************
	*  ��������: calcJJLevel;
	*  ��������: ����װ�����׺��ǿ���ȼ�;
	*  �����б�: [in]nToolId : ��Ҫ���׵�װ��Id;
	*  ����ֵ  : (int)�µ�ǿ���ȼ�;
	********************************************************************/
	int   calcJJLevel(const int nToolId);

	/********************************************************************
	*  ��������: calcBaseAttrValue;
	*  ��������: ����װ�������ܵĻ���ֵ(3����������Ļ���ֵΪͬһ��);
	*  �����б�: [in]nToolId : װ��Id;
	*			*[in]nTemplateId: װ����templateId(�������ڽ��׺��װ��);
	*			 [in]attrType: ��������;
	*  ����ֵ  : (float)���������ԵĻ���ֵ;
	********************************************************************/
	float  calcBaseAttrValue(const int nToolId, SMITHY_EQU_ATTR_TYPE attrType, int nQHLv);
	float  calcBaseAttrValue_JJDstEqu(const int nTemplateId, SMITHY_EQU_ATTR_TYPE attrType, int nQHLv, int nCLId);
	float  calcBaseAttrValue_CL(const int nToolId, SMITHY_EQU_ATTR_TYPE attrType, int nCLLevel);
	float  calcBaseAttrValueByFormula(float fEquBaseValue, float fEquAddValue, float fCLBaseValue, float fCLAddValue, int nQHLv);
	
	int  calcCLId(const TOOL_INFO* toolInfo, int nCLLevel);

private:

	static SmithyModel* mInstance;

	// װ����Դ;
	int  m_nEquipSourceToolTemplateId;
	vector<EQUIP_SOURCE_ITEM>  m_vecEquipSourceItems;

	///////////////////////////////////////////////////////////////////////////////

	vector<int> m_vecSelectedToolId;

	//ѡ���װ������
	EQUIP_ITEM_TYPE				m_selectEquipType;

	//����װ���仯ֵ
	vector<TOOL_CHANGE_INFO> m_vecUpdateTools;

	SMITHY_CHANGE_PARAM m_changeParam;
};

#endif //
