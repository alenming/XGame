/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerTrail
 *  Description:	试炼界面;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			04/20/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerTrail_h__
#define LayerTrail_h__

#include "TowerDef.h"
#include "Utils/Observer.h"
#include "TowerFloor.h"
#include "LayerOp.h"
#include "LayerTreasure.h"

class LayerTrail : public Layer, public Observer
{
public:
	~LayerTrail(void);

	CREATE_FUNC(LayerTrail);

	virtual bool init();

	virtual void onEnter();

	virtual void onExit();

	virtual void updateSelf(void* pData);

	void  update( float delta );

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	void  onLayerOpClose(POP_WND_TYPE wndType);

	void  onLayerTrailRewardClose();

	void  onLayerTreasureClose();

	void  onFloorTargetClicked(TOWER_FIGHT_TYPE nFightType);

	//void  onResetConfirm(Ref* pSender, CustomRetType type);

	//void  onRechargeConfirm(Ref* pSender, CustomRetType type);

private:
	LayerTrail(void);

	void  initUI();

	// 更新场景公共信息(包含进度);
	void  updateMainInfo(TrailMainInfo* info);

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

	void  updateProgress();
	void  calcProgressNode( int total, int node[], int size );
	Vec2  calcCursorPosition(int total, int cur, int node[], int size);

private:
	UI_Trail  m_ui;

	// 楼层封装;
	TowerFloor  *m_pFloor1, *m_pFloor2;

	// 已创建的最大楼层数;
	int  m_nMaxFloorNum;

	// 是否到达顶层;
	bool  m_bIsMaxFloor;

	// 右侧buff展示<buffId, item>;
	map<int, UIToolHeader*>  m_mapBuff;

	// buff层;
	LayerOp*  m_layerBuff;

	// 当前楼层;
	int   m_nInitCurFloor;

	// buff数量;
	int   m_nBuffCount;

	bool  m_bFirstEnter;

	enum UI_TRAIL_WIDGET
	{
		Pnl_Trail,
		Btn_Trail_Back,

		//Btn_Trail_Ranking,
		Btn_Store,
		Btn_Trail_Reset,

		Lab_Trail_Gold,
		Lab_Trail_Score,
		Lab_Trail_Star,

		//Img_Buff_Bg,

		//Pnl_Buff_Tips,
		//Lab_Buff_Name,
		//Lab_Buff_Desc
		Btn_Buff
	};
};

#endif // LayerTrail_h__
