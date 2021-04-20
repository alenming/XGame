/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerActivity
 *  Description:	活动关卡的UI实现
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			11/05/2014
 *  Others:			12/06/2015 第3次重做 by Phil
 *
 ******************************************************************/
#ifndef LayerActivity_h__
#define LayerActivity_h__

#include "ChapterModule/ChapterDef.h"
#include "Utils/Observer.h"
#include "ChapterModule/LayerFrame.h"
#include "ActivityNode.h"

class ActivityNode;
class LayerActivity : public Layer, public Observer
{
public:
	LayerActivity(void);
	~LayerActivity(void);

	static LayerActivity* create(bool pushScene = false);

	virtual bool  init(bool pushScene);
	virtual void  onEnter();
	virtual void  onExit();

	// UI更新接口;
	virtual void  updateSelf(void* data);

	// 选中某个活动;
	void  onActivityClicked(Node* node, Widget::TouchEventType type, int nIndex);
	void  onActivityEnter(int nActivityId);

	// 发起挑战;
	void  onChallenge(int nActivityBarrierId);
	bool  preChallenge(int nActivityBarrierId);

	// 扫荡;
	void  onSweep(int nActivityBarrierId);

private:

	void  initUI();

	void  initVipUI_1();
	void  initVipUI_2();

	void  update(float dt);

	// 按钮响应;
	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	// 更新活动列表;
	void  updateActivityList(vector<UI_ACTIVITY_NODE_PARAM>* vcActivityInfo);

	// 更新活动节点;
	void  updateActivityNode(ActivityState* state);

	string  getVipIcon(int nVipLv);
	void    checkBuyCondition();

private:

	UI_Activity	 m_ui;

	map<int, ActivityNode*> m_mapUIActivityList;
	vector<Node*> m_vcUIActivitys;

	// 当前选中的活动;
	int  m_nCurSelActivityId;
	ACTIVITY_TYPE m_nCurSelActivityType;
	int  m_nCurSelActivityBarrierId;

	// 进入战斗的类型(正常进入-0 扫荡-1);
	int  m_nChallengeType;

	// 是否首次进入;
	bool  m_bIsFirstEnter;

	bool mIsPushScene;

	bool  m_bIsEnableFight;

	enum UI_ACTIVITY_WIDGET
	{
		Btn_Back,

		Btn_Vip_OK,
		Btn_Vip_Cancel,
		Btn_Vip_Close,

		Btn_Vip_State_OK,
		Btn_Vip_State_Cancel,
		Btn_Vip_State_Close,
	};
};

#endif // LayerActivity_h__