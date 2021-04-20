/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		TowerFloor
 *  Description:	爬塔/试炼 地板层封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			04/23/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef TowerFloor_h__
#define TowerFloor_h__

#include "TowerDef.h"

class TowerFloor : public Layout
{
public:
	TowerFloor(void);
	~TowerFloor(void);

	static TowerFloor* create(TOWER_TYPE nType, int nLogicFirstFloor);

	virtual bool  init(TOWER_TYPE nType, int nLogicFirstFloor);

	void  setParent(Layer* parent) { m_pParent = parent; };

	// 更新楼层状态;
	void  updateFloorState();

	// 锁消失;
	void  onAnimationDone(Node* pSender, int nFloor);

private:
	void  initUI();

	// 确定一些基础位置;
	void  judgePosition();

	// 生成目标;
	Node*  createTarget(int nFloor);

	// 过关条件;
	Layout* createCondition(int nFloor);

	// 点击响应;
	void  onTargetClicked(Ref* pSender, Widget::TouchEventType type, TOWER_FIGHT_TYPE nFightType);

private:

	// 类型;
	TOWER_TYPE  m_nTowerType;

	// 父节点;
	Layer*  m_pParent;

	// 类型限定下的起始楼层号;
	int  m_nLogicFirstFloor;

	// 楼层标识位置，由1至2;
	Vec2  m_posFloorNum[2];

	// 目标位置，由1至2;
	Vec2  m_posTarget[2];

	// 怪物/宝箱;
	Node*  m_Target[2];

	// 过关条件;
	Layout* m_pnlCondition[2];

	// 禁用状态;
	Layout*  m_layoutDisable[2];
	//Layout*  m_layoutLock[2];

	// ;
	bool  m_IsTargetClicked;
};

#endif // TowerFloor_h__
