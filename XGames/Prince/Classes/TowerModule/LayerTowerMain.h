/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerMain
 *  Description:	爬塔/试炼主界面的UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			11/10/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerTowerMain_h__
#define LayerTowerMain_h__

#include "TowerDef.h"
#include "Utils/Observer.h"

enum PNL_TYPE
{
	PNL_TYPE_MAIN,
	PNL_TYPE_TOWER,
	PNL_TYPE_TRAIL
};

class LayerTowerMain : public Layer, public Observer
{
public:
	~LayerTowerMain(void);

	//CREATE_FUNC(LayerTowerMain);
	static LayerTowerMain*  create(TOWER_TYPE type = TOWER_TYPE_INVALID, bool pushScene = false);

	virtual void  onEnter();
	virtual void  onExit();
	virtual bool  init(TOWER_TYPE type = TOWER_TYPE_INVALID, bool pushScene = false);

	virtual void  updateSelf(void* data);

private:
	LayerTowerMain(void);

	void  initUI();

	// 按钮响应;
	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	// 进场动画;
	void  playEnterAnimation();

	// 扫荡检测;
	void  checkSweep();

private:

	UI_Tower_Main  m_ui;

	TOWER_TYPE  m_preType;

	// 爬塔扫荡标识;
	ImageView*  m_imgSweep;

	bool  m_bIsFirstEnter;

	bool mIsPushScene;

	enum UI_TOWER_MAIN_WIDGET
	{
		Pnl_Main,
		Btn_Main_Back,
		Btn_Enter_Tower,
		Btn_Enter_Trail,
		Btn_Help
		//Pnl_Main_Tips,
		//Btn_Reward_Tower,
		//Btn_Reward_Trail
	};
};

#endif // LayerTowerMain_h__
