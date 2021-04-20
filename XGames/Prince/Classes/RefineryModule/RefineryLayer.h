/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		RefineryLayer
 *  Description:	Refinery层模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			10/22/2015
 *  Others:			
 *
 ******************************************************************/
#include "RefineryHeader.h"
#include "RefineryController.h"
#include "RefineryModel.h"
#include "DataManager/DataTable.h"
#include "Widget/LayerCommFormation.h"
#include "ToolModule/ToolModel.h"

#ifndef RefineryLayer_h_
#define RefineryLayer_h_

class RefineryLayer:public Layer,public Observer
{
public:
	~RefineryLayer();

	static RefineryLayer* create(bool pushScene);

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);
protected:
	RefineryLayer();

	bool init(bool pushScene);
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi();
	void switchState();
	void onNotify(Ref* sender, Widget::TouchEventType type); //按钮
	void onCheckNotify(Ref* sender, CheckBox::EventType type);
	void onScrollViewEvent( Ref* pSender, ScrollView::EventType type);

	void addMaterilBaGuaList(); //添加八卦炉材料
	void addMaterilJiuGongList(); //添加九宫炉材料
	void resetLuMateril(bool openShop = false); //重置炉中材料

	void onCallMaterilAdd(float dt); //长按
	void onAddMateril(float dt);
	void updateNextFram(float ft);//删除控件

	void setLuMateril(int idx);
	void setGetDanCount(int id,bool isAdd,int amout);

	void CalculationChip(); //计算碎片
	void CalculationOther(); //计算其他材料

private:

	UI_REFINERY_ENTER   m_ui;   //控件集合
	CustomScrollBar *m_BgListBar;  //添加排行榜奖励列表
	REFINERY_NODE_VEC  m_otherVec; //其他进阶材料
	REFINERY_NODE_VEC  m_chipVec; //碎片

	REFINERY_NODE  m_LuMaterilVec[MaterialCount]; //炉子进阶材料
	bool  m_LongPress; //是否长按
	int   m_materIdx; 

	Widget *m_deleteItem;

	int  m_danCount;
	int  m_LuMaterilCount;

	RefineryController mController;

	bool m_isMove;
	bool   m_isPushScene;
	bool m_canRefinery;
};




#endif // !PvpLayer_h_
