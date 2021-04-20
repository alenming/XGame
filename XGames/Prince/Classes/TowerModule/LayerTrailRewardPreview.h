/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerTrailRewardPreview
 *  Description:	试炼奖励预览;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/07/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerTrailRewardPreview_h__
#define LayerTrailRewardPreview_h__

#include "TowerDef.h"
#include "Utils/Observer.h"

class RewardPreviewNode : public Layout
{
public:
	~RewardPreviewNode();
	
	static RewardPreviewNode* create(RewardPreviewParam param);

	virtual bool  init(RewardPreviewParam param);

private:
	RewardPreviewNode();

	void  initUI(RewardPreviewParam param);
};

class LayerTrailRewardPreview : public Layer, public Observer
{
public:
	~LayerTrailRewardPreview(void);

	CREATE_FUNC(LayerTrailRewardPreview);

	virtual bool  init();

	virtual void  onEnter();

	virtual void  updateSelf(void* pData);

private:
	LayerTrailRewardPreview(void);

	void  update(float dt);

	void  initUI();

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	void  onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	void  updateRewardShow(vector<RewardPreviewParam>* vcReward);

private:

	UI_Reward_Preview  m_ui;

	enum UI_TRAIL_REWARD_PREVIEW_WIDGET
	{
		Btn_Close
	};
};

#endif // LayerTrailRewardPreview_h__
