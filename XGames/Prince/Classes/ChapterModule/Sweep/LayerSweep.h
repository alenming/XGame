/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerSweep
 *  Description:	关卡模块 - 扫荡UI的实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			10/08/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerSweep_h__
#define LayerSweep_h__

#include "SweepDef.h"
#include "Utils/Observer.h"
#include "ChapterModule/ChapterController.h"
#include "ChapterModule/ChapterModel.h"
#include "ChapterModule/BarrierModel.h"
#include "MainModule/MainModel.h"
#include "SweepSettlementNode.h"

class LayerSweep : public Layer, public Observer
{
public:
	LayerSweep(void);
	~LayerSweep(void);

	static LayerSweep* create(int nConsume, int nBaseCount, int nRealCount, int nFlag = 0);

	virtual bool  init(int nConsume, int nBaseCount, int nRealCount, int nFlag = 0);

	virtual void  onEnter();
	virtual void  onExit();

	virtual void  updateSelf(void* data);

	void  onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	void  setLogicParent(LayerBarrier* parent) { m_parent = parent; };

private:

	void  initUI();

	void  update(float delta);

	// 按钮响应;
	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	// 更新扫荡结果展示;
	void  updateSweepSettlement(vector<SweepSettlement> vcSettlement);

private:

	UI_Sweep  m_ui;

	// 预期扫荡次数(1/10);
	int			m_nBaseTimes;

	// 实际可扫荡次数(0-10);
	int			m_nRealTimes;

	int m_nFlag;

	// 当前选中的数字标签;
	int  m_nCurSweepCountTag;

	// 单次耗体力;
	int  m_nConsume;

	// 滚动高度;
	float  m_fTmpHeight;

	// 父节点;
	LayerBarrier*  m_parent;

	enum UI_SWEEP_WIDGET
	{
		Root,
		Pnl_Sweep,
		Btn_Close,
		Img_Title,
		Img_Scroll_Bg,
		ScrollView_Settlement,
		Lab_Power,
		Lab_Energy,
		Btn_Continue

	};
};

#endif // LayerSweep_h__
