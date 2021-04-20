/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		SweepSettlementNode
 *  Description:	扫荡结果展示UI;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			10/15/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef SweepSettlementNode_h__
#define SweepSettlementNode_h__

#include "SweepDef.h"

class SweepSettlementNode : public Layout
{
public:
	~SweepSettlementNode(void);

	//CREATE_FUNC(SweepSettlementNode);
	static SweepSettlementNode* create(SweepSettlement settlement);

	virtual void  onEnter();

	virtual bool  init(SweepSettlement settlement);

	void   setSingleHeight(float nHeight) { m_fSingleHeight = nHeight;};
	float  getSingleHeight() { return m_fSingleHeight; };

private:
	
	SweepSettlementNode(void);

	void  initUI(SweepSettlement settlement);

	// 进场特效;
	void  playEnterAnimation();

	// _direction: -1-x轴反向飞入  1-x轴正向飞入;
	void  shiftWidgetX(Widget* _widget, int _direction_x = 1, bool bCallback = false);

	// 飞入结束;
	void  onAnimationDone(Node* pSender, int nIndex);

private:

	UI_SweepSettlementNode  m_ui;

	float m_fSingleHeight;

	int m_nSweepOrder;

	vector<Widget*>  m_vcItem;
};

#endif // SweepSettlementNode_h__
