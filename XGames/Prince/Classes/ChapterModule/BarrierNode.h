/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		BarrierNode
 *  Description:	剧情关卡按钮的封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/22/2014
 *  Others:			V1.1  UI改造 12/25/2014 Phil
 *					V1.2  UI改造 增加星级和首次奖励 07/28/2015 Phil
 *
 ******************************************************************/
#ifndef BarrierNode_h__
#define BarrierNode_h__

#include "ChapterDef.h"

class BarrierNode : public Layout
{
public:
	BarrierNode(void);
	~BarrierNode(void);

	static BarrierNode* create(UI_BARRIER_PARAM* param);

	virtual	bool  init(UI_BARRIER_PARAM* param);

	virtual void  onEnter();

	// 设置逻辑父节点，主要是因为CocoStudio文件中widget层级太多;
	void  setLogicParentLayer(Layer* parent) { m_pParent = parent; };

	// 关卡节点开关;
	void  setBarrierEnable(bool bIsEnabled);

	// UI更新接口;
	void  updateSelected(bool bSelected);
	void  updateNewFlag(bool bNew);

	// 设置位置;
	void  setOriPosition(Vec2& pos) { m_ptOriPos = pos; };

private:

	void  initUI(UI_BARRIER_PARAM* param);

	// 关卡选中;
	void  onBarrierClicked(Ref* ref, Widget::TouchEventType type);

	// 数字-文字转换;
	//string  getNumText(int num);

	// 弹出;
	void  updatePopUp();

private:

	UI_BarrierNode	m_ui;

	Layer*	m_pParent;				// 逻辑父节点;

	bool	m_bIsEnabled;			// 当前关卡是否可用;

	int		m_nBarrierId;			// 关卡Id;

	int		m_nResId;				// 图标Id;

	BARRIER_LEVEL  m_nLv;			// 关卡难度;

	bool	m_bIsSelected;			// 当前是否选中;

	Point	m_ptOriPos;				// 原始位置;

	bool	m_bIsPopUp;				// 是否弹出(选中不一定弹出);

	int		m_nBossId;				// boss;

};

#endif // BarrierNode_h__