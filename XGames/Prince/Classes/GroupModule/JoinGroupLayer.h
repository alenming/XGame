/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		JoinGroupLayer
 *  Description:	帮派加入前模块的Layer实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef JoinGroupLayer_h__
#define JoinGroupLayer_h__

#include "JoinGroupDef.h"
#include "JoinGroupController.h"

class JoinGroupLayer : public Layer, public Observer
{
public:
	JoinGroupLayer();
	~JoinGroupLayer(void);

	CREATE_FUNC(JoinGroupLayer);

	virtual bool  init();

	virtual void onEnter();
	virtual void onExit(); 

	virtual void updateSelf(void* data);
	void updateNextFram(float ft);

	void  initUI();
	void  initGroupInfoPanel();
	void  onBtnNotify(Ref* sender, Widget::TouchEventType type); //按钮

	void  addGroupList();//添加公会列表
	void  setApplyCancleNodeState(bool isAplly = true);

	//旗帜
	void  addFlagList(); //添加旗帜
	void  onScrollViewEvent( Ref* pSender, ScrollView::EventType type);

	//创建
	void  setCreateGroupFlag(int id = 1); //设置创建旗帜
	void  viewGroupInfo(GROUP_INFO info);//查看公会信息
private:
	UI_JOIN_GROUP m_ui;
	CustomScrollBar *m_customScrollBar;

	JoinGroupController m_JoinGroupController;

	int   m_groupResId; //当前帮派旗帜
	int   m_curPage;
	int   m_chooseApplyCancleId;

	SEARCH_TYPE  m_findState;
};

#endif
