/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerTrailReward
 *  Description:	试炼奖励界面;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			04/20/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerTrailReward_h__
#define LayerTrailReward_h__

#include "TowerDef.h"
#include "LayerTrail.h"
#include "Utils/Observer.h"

class LayerTrail;
class LayerTrailReward : public LayerColor, public Observer
{
public:
	~LayerTrailReward(void);

	//CREATE_FUNC(LayerTrailReward);

	static LayerTrailReward* create();

	virtual bool  init();

	virtual void  onEnter();

	virtual void  update(float delta);

	virtual void  updateSelf(void* pData);

	// 设置父节点;
	void  setParent(LayerTrail* parent) { m_pParent = parent; };

private:
	LayerTrailReward(void);

	void  showTouchContinue();

	void  onLayerTouched(Ref* ref, Widget::TouchEventType type);

	void  initUI();

	void  playItemAnimation(Node* pSender, int nIndex);

private:

	Layout*  pRoot;

	// 父节点;
	LayerTrail*  m_pParent;

	//map<int, int>  m_mapReward;
	vector<UIToolHeader*>  m_vcReward;
};

#endif // LayerTrailReward_h__
