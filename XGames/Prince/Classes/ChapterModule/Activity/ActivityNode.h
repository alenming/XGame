/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		ActivityNode
 *  Description:	活动关卡大卡牌节点的UI实现
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			12/16/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef ActivityNode_h__
#define ActivityNode_h__

#include "ActivityDef.h"
#include "LayerActivity.h"
#include "Widget/ActionCreator.h"
#include "DoubleBonus/DoubleBonusDef.h"

class ActivityNode : public Node, public Observer
{
public:
	ActivityNode(void);
	~ActivityNode(void);

	static ActivityNode* create(Layer* pParent, UI_ACTIVITY_NODE_PARAM param);

	virtual bool  init(Layer* pParent, UI_ACTIVITY_NODE_PARAM param);

	// 选中;
	void  setSelected(bool bSelected);

	// 更新状态;
	void  updateState(ActivityState* state);

	virtual void  updateSelf(void* data);

	// 获取Id;
	int   getActivityId() { return m_nActivityId; };
	ACTIVITY_TYPE getActivityType() { return m_nType; };

	// 重载"==";
	bool  operator == (const ActivityNode& _node) const
	{
		return (this->m_nActivityId == _node.m_nActivityId);
	}
	bool  operator == (const int& activityId) const
	{
		return (this->m_nActivityId == activityId);
	}

private:

	void initUI(UI_ACTIVITY_NODE_PARAM param);

	int  getResId(int nActivityId);
	string getTips(ACTIVITY_TYPE type);

	void  showDoubleBonusTips(bool bShow);
	ACTIVITY_TYPE checkTypeByDoubleBonusType(DOUBLE_BONUS_TYPE type);
	DOUBLE_BONUS_TYPE checkTypeByActivityType();

private:

	UI_ActivityNode	 m_ui;
	Layer*  m_pParent;
	int  m_nActivityId;
	ACTIVITY_TYPE m_nType;

	enum UI_ACTIVITY_NODE_WIDGET
	{
		Btn_Enter
	};
};

#endif // EventNode_h__
