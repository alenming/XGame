/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		OpNode
 *  Description:	试炼对手节点UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			04/16/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef OpNode_h__
#define OpNode_h__

#include "TowerDef.h"
#include "LayerOp.h"

class LayerOp;
class OpNode : public Node
{
public:
	OpNode(void);
	~OpNode(void);

	static OpNode*  create(UI_TOWER_OP_PARAM param, LayerOp* parent);

	virtual bool  init(UI_TOWER_OP_PARAM param, LayerOp* parent);

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type);

	void  onCardClicked(Ref* ref, Widget::TouchEventType type);

	void  orbitCallBack();

	void  updateState(bool bEnabled);

private:
	void  initUI(UI_TOWER_OP_PARAM param);

	void  showForamtion(bool  bShow);

private:
	UI_Trail_Op_Node m_ui;

	int  m_nLv;

	LayerOp*  m_pParent;

	// 翻转临界角度;
	float  m_fCriticalAngle;
};

#endif // OpNode_h__
