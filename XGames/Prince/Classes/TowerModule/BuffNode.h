/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		BuffNode
 *  Description:	试炼buff节点UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			04/16/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef BuffNode_h__
#define BuffNode_h__

#include "TowerDef.h"
#include "LayerOp.h"

class LayerOp;
class BuffNode : public Node
{
public:
	BuffNode(void);
	~BuffNode(void);

	static BuffNode*  create(UI_BUFF_PARAM param, LayerOp* parent);

	virtual bool  init(UI_BUFF_PARAM param, LayerOp* parent);

	void  updateState(bool  bEnabled);

private:

	void  initUI(UI_BUFF_PARAM param);

	void  onBtnClicked(Ref* pSender, Widget::TouchEventType type);

private:

	UI_Buff_Node m_ui;

	int  m_nBuffId;

	int  m_nBuffIndex;

	LayerOp*  m_pParent;
};

#endif // BuffNode_h__
