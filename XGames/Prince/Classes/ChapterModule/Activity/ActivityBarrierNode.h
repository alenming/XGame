/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		ActivityBarrierNode
 *  Description:	活动关卡节点的UI实现
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			12/18/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef ActivityBarrierNode_h__
#define ActivityBarrierNode_h__

#include "ActivityDef.h"
#include "LayerActivity.h"

class ActivityBarrierNode : public Node
{
public:
	ActivityBarrierNode(void);
	~ActivityBarrierNode(void);

	static ActivityBarrierNode* create(Layer* pParent, int nActivityId, ACTIVITY_TYPE type, ActivityBarrierInfo* barrierInfo, int nLv);

	virtual bool  init(Layer* pParent, int nActivityId, ACTIVITY_TYPE type, ActivityBarrierInfo* barrierInfo, int nLv);

private:

	void initUI(ActivityBarrierInfo* barrierInfo, int nLv);

private:

	UI_ActivityBarrierNode	 m_ui;
	Layer*  m_pParent;
	int  m_nActivityId;
	int  m_nActivityBarrierId;
	ACTIVITY_TYPE m_nType;
};

#endif // ActivityBarrierNode_h__
