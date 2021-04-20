#ifndef LayerRob_h__
#define LayerRob_h__

#include "RobDef.h"
#include "UI/ModalLayer.h"
#include "Utils/Observer.h"
#include "RobController.h"

///////////////////////////////////////////////////

class RobResultItem : public Layout
{
public:
	RobResultItem() {};
	~RobResultItem() {};

	static RobResultItem* create(ROB_RESULT_INFO param);
	virtual bool  init(ROB_RESULT_INFO param);

private:
	UI_Rob_Result_Item  m_ui;
};

///////////////////////////////////////////////////

class LayerRob;
class RobItem : public Layout
{
public:
	RobItem()
		: m_parent(nullptr)
	{};
	~RobItem() {};

	static RobItem* create(ROB_PLAYER_INFO param);
	virtual bool  init(ROB_PLAYER_INFO param);
	void  setLogicParent(LayerRob* parent) { m_parent = parent; };

private:
	UI_Rob_Item  m_ui;
	LayerRob* m_parent;
};

///////////////////////////////////////////////////

class RecordItem : public Layout
{
public:
	RecordItem()
		: m_parent(nullptr)
	{};
	~RecordItem() {};

	static RecordItem* create(ROB_RECORDS_INFO param);
	virtual bool  init(ROB_RECORDS_INFO param);
	void  setLogicParent(LayerRob* parent) { m_parent = parent; };

private:
	UI_Rob_Record_Item  m_ui;
	LayerRob* m_parent;
};

///////////////////////////////////////////////////


class LayerRob: public ModalLayer, public Observer
{

public:
	LayerRob();
	~LayerRob();

	static LayerRob* create(bool pushScene = false);
	//CREATE_FUNC(LayerRob);

	virtual bool init(bool pushScene);
	virtual void onEnter();
	virtual void onExit();

	virtual void updateSelf(void* pData);

	// 掠夺;
	void  onRob(int nPlayerId, int nTimes = 1);

private:

	// UI初始化;
	void  initUI();
	void  initUIRob();
	void  initUIResult();
	void  initUIDetail();
	void  initUIRest();
	void  initUIRecord();

	// 更新主界面信息;
	virtual void  updateMainInfo(const Rob_Main_Info* info);

	// 道具列表;
	void  updateTools();

	// 初始化道具和碎片状态;
	void  initToolsAndChips();

	// 选中道具;
	void  selectTool(UIToolHeader* tool);
	void  selectTool(int nToolId);

	// 计算碎片位置;
	void  calcChipsPosition(int nCount, vector<Point>& vcPos);

	// 刷新碎片显示;
	void  updateChips(bool bShowAnimation = true);
	void  updateChipsAfterAnimation();

	// 展示掠夺窗口;
	void  showRob(bool bRefresh = true);

	// 展示详情页;
	void  showDetail(int nToolId);

	// 展示掠夺记录;
	void  showRobRecord();

	// 免战;
	void  showRest();

	// 倒计时;
	void  onCountDown(int sec);

	// 刷新对手;
	void  updateEnemy(vector<ROB_PLAYER_INFO>* vcEnemy);

	// 掠夺记录;
	void  updateRobRecord(vector<ROB_RECORDS_INFO>* record);

	// 掠夺10次结果;
	void  showRobResult();
	void  updateRobTenResult(ROB_TEN_RESULT* robResult);

	// 按钮事件;
	void  onBtnBack(Ref* pSender, Widget::TouchEventType type);
	void  onBtnClicked(Ref* pSender, Widget::TouchEventType type, int nWidgetName);

private:

	// UI控件集合;
	UI_Rob_Main	 m_ui;

	RobController*  m_pRobController;

	// UI道具列表;
	map<int, UIToolHeader*>  m_mapUITools;

	// 是否在免战时间;
	bool  m_bIsRest;

	// 当前选中的道具;
	int  m_nSelectToolId;

	// 当前选中的碎片;
	int m_nSelectChipId;

	// 掠夺10次结果;
	bool  m_bIsRobTenSuccess;

	// 是否首次进入;
	bool  m_bIsFirstEnter;

	int		m_nTurnCardIndex;
	bool	m_bTurnCardResultAnimationIsRun;
	int		m_nSelectRobTimes;
	bool	m_isRobTenSuccess;

	bool mIsPushScene;

	enum UI_ROB_WIDGET
	{
		Btn_Compose,
		Btn_RapidCompose,
		Btn_Record,
		Btn_Rest

	};
};

#endif // 
