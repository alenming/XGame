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

	// 设置基本属性值(当前角色Id, 装备, 装备类型);
	void  initSmithyData(const int nHeroId, const int nToolId, const EQUIP_ITEM_TYPE equipType, const SMITHY_OP_TYPE opType);

	// 选中装备;
	void  selectEquipment(const EQUIP_ITEM_TYPE itemType, const int nToolId);

	// 选中操作类型;
	void  onCheckBoxClicked(Ref* sender, CheckBox::EventType type, int nWidgetName);

	// 按钮响应;
	void  onBtnClicked(Ref* sender, Widget::TouchEventType type, int nWidgetName);

	// 装备点击;
	void  onEquipmentClicked(Ref* sender, Widget::TouchEventType type, EQUIP_ITEM_TYPE itemType, int nToolId);

	// 材料不足;
	void  onGetTools(Ref* sender, Widget::TouchEventType type, int nToolId);

	// 返回按钮;
	void  onBtnBack(Ref* sender, Widget::TouchEventType type);	

	// 获取装备品质颜色;
	Color3B getEquipQuaColor(TOOL_ITEM_QUA qua);

	// 添加/删除强化材料;
	void  modifyMaterialCallback(MaterialClickEvent event);
	void  addMaterialPageCallback(vector<SelectedMaterialParam> vcTools, bool bClean = false);

	// 刷新强化经验;
	void  updateQHExp();

	// 选中操作类型;
	void  selectPage(SMITHY_OP_TYPE  opType);

private:

	// UI初始化;
	void  initUI();
	void  initUIQH();
	void  initUIJJ();
	//void  initUIXL();
	void  initUICL();

	// 更新强化/进阶/洗练面板;
	void  updateOpPanel(bool bForceUpdate = false);
	void  updateQHInfo();
	void  updateJJInfo();
	//void  updateXLInfo();
	void  updateCLInfo();

	// 更新角色信息;
	void  updateRoleInfo();
	void  updateRoleFightInfo();

	// 更新装备信息;
	void  updateEquipmentInfo();
	void  updateSingleEquipmentInfo(const int nToolId);

	// 获取英雄装备列表;
	void  getEquipIdList(map<int, int>&  mapEquipIdList);

	// 更新洗练结果(临时显示，不是替换);
	//void  updateXLResult(vector<AttrParam>*  vcGro);

	// 标记需要刷新页面内容;
	void  waitRefresh();

	/////////////////////////////// 特效 ///////////////////////////////

	// 进场特效;
	void  playEnterAnimation();

	// 动作特效;
	void  playQHAnimation(int nAdvLv, bool bCrit);
	void  playJJAnimation();
	void  playCLAnimation(OBS_PARAM_TYPE_SM clType);
	void  movementEventCallFunc(Armature *armature, MovementEventType movementType, const string& movementID);
	void  frameEventCallFunc(Bone *bone, const std::string& frameEventName, int originFrameIndex, int currentFrameIndex);

	// 获取属性图标;
	string  getAttrIcon(SMITHY_EQU_ATTR_TYPE attrType);

	// 更新标签页上的红点标识;
	void  updateReddot();
	void  showPageReddot(CheckBox* cbPage, bool bShow);

private:

	// UI控件集合
	UI_Smithy  m_ui;

	// 当前英雄Id;
	int  m_nCurHeroId;

	// 当前选中道具;
	EQUIP_ITEM_TYPE  m_itemType;
	int  m_nCurSelToolId;

	// 当前选中的操作类型;
	SMITHY_OP_TYPE  m_nCurSelOpType;

	// 当前3类面板中的数据对应的装备Id;
	map<int, int>   m_mapOpPanelWithId;

	// 当前装备列表<type, UIToolHeader*>;
	map<int, UIToolHeader*>  m_mapEquipment;

	SmithyController*  m_pController;

	// 是否第一次进入;
	bool  m_bIsFirstEnter;

	// 强化/进阶参数，用来显示特效;
	QH_Param	m_paramQH;
	JJ_Param	m_paramJJ;
	CL_Param	m_paramCL;

	// 强化材料;
	vector<MaterialHeader*>  m_vcQHMaterials;

	friend class LayerAdvanced;

	// 马书强化经验预览值;
	int  m_nHorseBookExpPreview;

	//标记当前发起动画的操作是不是淬炼(淬炼才可能需要弹缘分窗, 用来做判定是要要弹缘分窗)			added by Jerry 2016-6-18
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
