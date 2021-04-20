/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerLoginRole
 *  Description:	角色登陆(选择分区入口)
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			10/26/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerLoginRole_h__
#define LayerLoginRole_h__

#include "LoginDef.h"
#include "Utils/Observer.h"

class LayerLoginRole : public Layer, public Observer
{
public:
	~LayerLoginRole(void);

	CREATE_FUNC(LayerLoginRole);

	virtual bool  init();
	virtual void  onEnter();
	virtual void  onExit();

	virtual void  updateSelf(void* pData);

	virtual void  onBtnClicked(Ref* sender, Widget::TouchEventType type, int nWidgetName);

	// 点击分区响应;
	void  onZoneClicked(Ref* ref, Widget::TouchEventType type, string serverId);

	void  onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	void  onZoneIndexSelected(Ref* ref, Widget::TouchEventType type, int nIndex);

private:
	LayerLoginRole(void);

	void  initUI();

	// 更新服务器索引列表;
	void  updateServerZoneIndex(int nTotalCount);

	// 更新服务器列表;
	void  updateServerZoneList(int nIndex);

	// 更新默认选中;
	void  updateDefaultServer();
	void  updateDefaultServer(ServerParam* defaultServer);

	// 选中服务器;
	void  selectServer(ServerParam* param);

	// 服务器状态;
	void  getUIServerState(ServerParam* param, string& strState, Color3B& color);

	// 显示公告板;
	void  showBoard();

private:

	UI_LOGIN_ROLE  m_ui;

	vector<ServerParam> m_vcZone;

	ServerParam  m_serverParam;

	string m_strWeriHu;
	string m_strBaoMan;
	string m_strXinQu;


	enum LOGIN_ROLE_WIDGET
	{
		Btn_Enter,
		Btn_Logout,
		Btn_Board,
		Btn_Zone
	};
};

#endif // LayerLoginRole_h__
