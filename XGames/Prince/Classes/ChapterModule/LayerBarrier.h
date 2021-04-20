/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerBarrier
 *  Description:	剧情关卡属性页的UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			09/26/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerBarrier_h__
#define LayerBarrier_h__

#include "ChapterDef.h"
#include "Utils/Observer.h"
#include "BarrierNode.h"

class LayerBarrier : public Layer, public Observer
{
public:

	LayerBarrier();
	~LayerBarrier();

	CREATE_FUNC(LayerBarrier);

	virtual bool  init();
	virtual void  onEnter();
	virtual void  onExit();

	// UI更新接口;
	virtual void  updateSelf(void* data);

	// (从关卡节点)选中关卡;
	//void  onBarrierSelected(BarrierNode* node);
	void  onBarrierSelected(int nBarrierId);

	// 控制显隐;
	//void  setEnabled(bool bEnabled);

	// 更新体力;
	void  updatePower(int nTotalPower);

	// 更新挑战次数;
	void  updateDayCount(int nBarrierId = INVALID_CHAPTER_OR_BARRIER_ID);

	// 星级奖励页面触摸;
	void  onLayerTouched(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	// 检查剩余次数以及VIP购买/重置相关;
	void  checkBuyCondition();

private:

	void  initUI();

	void  initVipUI_1();
	void  initVipUI_2();

	void  update(float dt);

	// 更新章节关卡信息(公共区域，不包含关卡列表);
	void  updateChapterInfo(UI_CHAPTER_DETAIL_PARAM* data);

	// 更新关卡列表;
	void  updateBarrierList(UI_BARRIER_LIST_PARAM* data);

	// 更新关卡属性展示(遇敌/掉落/通关限制等);
	void  updateBarrierInfo(UI_BARRIER_INFO_PARAM* data);

	// 更新星级奖励;
	void  updateStarReward(UI_STAR_REWARD_PARAM* param);

	// 按钮响应;
	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	// 定位关卡;
	void  fixBarrierPosition(int nBarrierId);

	// 滚动位置记忆;
	void  setBarrierScrollPositionPercent(float fPercent);

	// 难度切换;
	void  selectBarrierLv(BARRIER_LEVEL nLv);

	// 难度开关;
	void  enabledBarrierLv(BARRIER_LEVEL nLv, bool bEnabled = true);

	// 新开启关卡;
	void  showNewBarrier(int nBarrierId);

	// 新开启难度;
	void  showNewLv(BARRIER_LEVEL nLv);

	// 定位难度按钮;
	//void  fixButtonLvPosition(BARRIER_LEVEL nMaxLv);

	// 剩余挑战次数VIP相关;
	bool  checkLeftTimesWithVip();
	string  getVipIcon(int nVipLv);
	void  updateVipData();

	// 进场特效;
	void  playEnterAnimation();

	// 双倍掉落;
	void  showDoubleBonusTips_Lv0(bool bShow);
	void  showDoubleBonusTips_Lv1(bool bShow);

private:

	UI_Barrier  m_ui;

	// 当前选中关卡;
	int  m_nCurSelBarrierId;

	// 当前选中难度;
	BARRIER_LEVEL  m_nCurSelLv;

	// 关卡列表;
	map<int, BarrierNode*>  m_mapBarriers;

	// 关卡消耗体力，保存是为了创建扫荡层;
	int  m_nConsume;

	// 关卡已获星级;
	int  m_nStarCount;

	// 战斗按钮;
	Armature* armatureFight;
	bool  m_bEnableFightBtn;

	// 关卡难度动画;
	Armature* armatureNewLv;

	// 星级奖励动画;
	Armature* armatureStarReward;
	Armature* armatureBack[3];
	Armature* armatureFront[3];

	// 难度按钮原始位置;
	Vec2	m_oriButtonPos[3];

	// 星级奖励参数;
	UI_STAR_REWARD_PARAM  m_starRewardParam;

	bool  m_bFirstEnter;

	// 销毁时是否进入战斗;
	bool  m_bIsEnterFighting;

	enum UI_BARRIER_WIDGET
	{
		Root,
		Pnl_Barrier,
		Btn_Back,
		AtlasLab_Num1,
		AtlasLab_Num2,
		AtlasLab_Num3,
		Btn_Box_1,
		Btn_Box_2,
		Btn_Box_3,

		ScrollView_BarrierList,

		Btn_Normal,
		Btn_Hard,

		Lab_Barrier_Name,
		Lab_Barrier_Desc,
		Img_Drop_Line,
		Lab_Coin,
		Lab_Exp,
		Lab_Power,
		Pnl_DayCount,
		Lab_DayCount_Cur,
		Lab_DayCount_Total,

		Btn_Sweep1,
		Btn_Sweep10,
		Btn_Fight,

		Pnl_Star,
		Pnl_Star_Content,

		Btn_Vip_State_OK,
		Btn_Vip_State_Cancel,
		Btn_Vip_State_Close,
		Btn_Vip_OK,
		Btn_Vip_Cancel,
		Btn_Vip_Close
	};

};

#endif // LayerBarrier_h__
