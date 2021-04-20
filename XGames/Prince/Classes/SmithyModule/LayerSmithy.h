#ifndef LayerSmithy_h__
#define LayerSmithy_h__

#include "SmithyDef.h"
#include "UI/ModalLayer.h"
#include "Utils/Observer.h"
#include "SmithyController.h"
#include "MaterialHeader.h"
#include "LayerAdvanced.h"

class LayerSmithy: public ModalLayer, public Observer
{
public:
	LayerSmithy();
	~LayerSmithy();
	CREATE_FUNC(LayerSmithy);

	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

	virtual void updateSelf(void* pData);

	// ���û�������ֵ(��ǰ��ɫId, װ��, װ������);
	void  initSmithyData(const int nHeroId, const int nToolId, const EQUIP_ITEM_TYPE equipType, const SMITHY_OP_TYPE opType);

	// ѡ��װ��;
	void  selectEquipment(const EQUIP_ITEM_TYPE itemType, const int nToolId);

	// ѡ�в�������;
	void  onCheckBoxClicked(Ref* sender, CheckBox::EventType type, int nWidgetName);

	// ��ť��Ӧ;
	void  onBtnClicked(Ref* sender, Widget::TouchEventType type, int nWidgetName);

	// װ�����;
	void  onEquipmentClicked(Ref* sender, Widget::TouchEventType type, EQUIP_ITEM_TYPE itemType, int nToolId);

	// ���ϲ���;
	void  onGetTools(Ref* sender, Widget::TouchEventType type, int nToolId);

	// ���ذ�ť;
	void  onBtnBack(Ref* sender, Widget::TouchEventType type);	

	// ��ȡװ��Ʒ����ɫ;
	Color3B getEquipQuaColor(TOOL_ITEM_QUA qua);

	// ���/ɾ��ǿ������;
	void  modifyMaterialCallback(MaterialClickEvent event);
	void  addMaterialPageCallback(vector<SelectedMaterialParam> vcTools, bool bClean = false);

	// ˢ��ǿ������;
	void  updateQHExp();

	// ѡ�в�������;
	void  selectPage(SMITHY_OP_TYPE  opType);

private:

	// UI��ʼ��;
	void  initUI();
	void  initUIQH();
	void  initUIJJ();
	//void  initUIXL();
	void  initUICL();

	// ����ǿ��/����/ϴ�����;
	void  updateOpPanel(bool bForceUpdate = false);
	void  updateQHInfo();
	void  updateJJInfo();
	//void  updateXLInfo();
	void  updateCLInfo();

	// ���½�ɫ��Ϣ;
	void  updateRoleInfo();
	void  updateRoleFightInfo();

	// ����װ����Ϣ;
	void  updateEquipmentInfo();
	void  updateSingleEquipmentInfo(const int nToolId);

	// ��ȡӢ��װ���б�;
	void  getEquipIdList(map<int, int>&  mapEquipIdList);

	// ����ϴ�����(��ʱ��ʾ�������滻);
	//void  updateXLResult(vector<AttrParam>*  vcGro);

	// �����Ҫˢ��ҳ������;
	void  waitRefresh();

	/////////////////////////////// ��Ч ///////////////////////////////

	// ������Ч;
	void  playEnterAnimation();

	// ������Ч;
	void  playQHAnimation(int nAdvLv, bool bCrit);
	void  playJJAnimation();
	void  playCLAnimation(OBS_PARAM_TYPE_SM clType);
	void  movementEventCallFunc(Armature *armature, MovementEventType movementType, const string& movementID);
	void  frameEventCallFunc(Bone *bone, const std::string& frameEventName, int originFrameIndex, int currentFrameIndex);

	// ��ȡ����ͼ��;
	string  getAttrIcon(SMITHY_EQU_ATTR_TYPE attrType);

	// ���±�ǩҳ�ϵĺ���ʶ;
	void  updateReddot();
	void  showPageReddot(CheckBox* cbPage, bool bShow);

private:

	// UI�ؼ�����
	UI_Smithy  m_ui;

	// ��ǰӢ��Id;
	int  m_nCurHeroId;

	// ��ǰѡ�е���;
	EQUIP_ITEM_TYPE  m_itemType;
	int  m_nCurSelToolId;

	// ��ǰѡ�еĲ�������;
	SMITHY_OP_TYPE  m_nCurSelOpType;

	// ��ǰ3������е����ݶ�Ӧ��װ��Id;
	map<int, int>   m_mapOpPanelWithId;

	// ��ǰװ���б�<type, UIToolHeader*>;
	map<int, UIToolHeader*>  m_mapEquipment;

	SmithyController*  m_pController;

	// �Ƿ��һ�ν���;
	bool  m_bIsFirstEnter;

	// ǿ��/���ײ�����������ʾ��Ч;
	QH_Param	m_paramQH;
	JJ_Param	m_paramJJ;
	CL_Param	m_paramCL;

	// ǿ������;
	vector<MaterialHeader*>  m_vcQHMaterials;

	friend class LayerAdvanced;

	// ����ǿ������Ԥ��ֵ;
	int  m_nHorseBookExpPreview;

	//��ǵ�ǰ���𶯻��Ĳ����ǲ��Ǵ���(�����ſ�����Ҫ��Ե�ִ�, �������ж���ҪҪ��Ե�ִ�)			added by Jerry 2016-6-18
	bool m_bIsCL;

	enum UI_Smithy_Widget
	{
		Btn_Back,
		CheckBox_Page_QH,
		CheckBox_Page_CL,

		Btn_QH_1,
		Btn_QH_N,
		Btn_QH_Auto_Add,
		Btn_QH_HorseBook,

		Btn_JJ,

		Btn_CL,
		Btn_CL_Degrade
	};
};

#endif // 
