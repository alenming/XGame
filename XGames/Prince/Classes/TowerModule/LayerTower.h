/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerTower
 *  Description:	爬塔界面;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			04/20/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerTower_h__
#define LayerTower_h__

#include "TowerDef.h"
#include "Utils/Observer.h"
#include "TowerFloor.h"

class LayerTower : public Layer, public Observer
{
public:
	~LayerTower(void);

	CREATE_FUNC(LayerTower);

	virtual bool  init();

	virtual void onEnter();

	virtual void updateSelf(void* pData);

	void  update( float delta );

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	void  onFloorTargetClicked( TOWER_FIGHT_TYPE nFightType );

	void  checkBuyCondition();
	string  getVipIcon(int nVipLv);

	void  onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	void  updateProgress();
	void  calcProgressNode(int total, int node[], int size);
	Vec2  calcCursorPosition(int total, int cur, int node[], int size);

private:
	LayerTower(void);

	void  initUI();

	void  initVipUI_1();
	void  initVipUI_2();

	// 更新场景公共信息(包含进度);
	void  updateMainInfo(TowerMainInfo* info);

	// 初始化楼层;
	void  initFloor(int nCurFloor);

	// 创建楼层;
	TowerFloor*  createFloor(int nFloor);

	// 滚动;
	void  scrollFloor();

	// 循环创建楼层;
	void  reCreateFloor();

	// 重置楼层;
	void  resetFloor();

	// 进入战斗;
	void  enterChallenge();

	// 更新奖励信息;
	void  updateReward();

	// 进场特效;
	void  playEnterAnimation();

private:

	UI_Tower  m_ui;

	// 楼层封装;
	TowerFloor  *m_pFloor1, *m_pFloor2;

	// 已创建的最大楼层数;
	int  m_nMaxFloorNum;

	// 是否到达顶层;
	bool  m_bIsMaxFloor;

	// 初始时的当前楼层;
	int   m_nInitCurFloor;

	bool  m_bFirstEnter;

	enum UI_TOWER_WIDGET
	{
		Pnl_Tower,
		Btn_Tower_Back,
		Btn_Tower_Ranking,
		Btn_Tower_Reward,
		Btn_Box,
		Btn_Tower_Reset,
		Btn_Tower_Sweep,
		Lab_Max_Floor,
		Btn_Sweep_OK,
		Btn_Sweep_Cancel,

		Btn_Vip_OK,
		Btn_Vip_Cancel,
		Btn_Vip_Close,

		Btn_Vip_State_OK,
		Btn_Vip_State_Cancel,
		Btn_Vip_State_Close,

		Btn_Sweep_Settlement_Close
	};

};

#endif // LayerTower_h__
