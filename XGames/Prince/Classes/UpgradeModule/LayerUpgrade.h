/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerUpgrade
 *  Description:	升级模块 - UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			11/25/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerUpgrade_h__
#define LayerUpgrade_h__

#include "UpgradeDef.h"
#include "LoginModule/LayerLogin.h"

class LayerLogin;
class LayerUpgrade : public Layer
{
public:
	~LayerUpgrade(void);

	CREATE_FUNC(LayerUpgrade);

	virtual bool  init();

	virtual void  update(float delta);

	// 设置父节点;
	void  setParent(LayerLogin* parent) { m_pParent = parent; };

	// 更新入口;
	void  checkUpgrade();

	// 按钮响应;
	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	////////////////////////////// 更新流程状态反馈; ////////////////////////////////////

	// 有未完成的下载;
	void  onDownloadUnfinished(long nNeedDownloadSize);

	// 强制更新;
	void  onForceUpgrade(long nSize);

	// 全量更新;
	void  onFullUpgrade();

	// 流程状态变化;
	void  onError(ErrorCode errCode);
	void  onProgress(float percent);
	void  onSuccess();

	// 无新版本/更新完毕;
	void  onUpgradeDone();

private:
	LayerUpgrade(void);

	void  initUI();

	// 更新文字信息;
	void  updateDownloadInfo();

	// 字节B单位转KB/MB单位;
	string  b2km(long xxb);

private:

	UI_Upgrade  m_ui;

	// 父节点;
	LayerLogin*  m_pParent;

	// 保留部分errorCode;
	ErrorCode   m_nReserveCode;

	// 更新包大小(用来计算速度);
	unsigned long  m_nPkgSize;
	timeval m_timeStart;
	float   m_fPrePercent;

	enum UI_Upgrade_Widget
	{
		Btn_Npc_OK,
		Btn_Npc_Cancel,

		Btn_Npc_Wait,
		Btn_Npc_Exit,

		Btn_Npc_Retry,
		Btn_Npc_No,

		Btn_Npc_Continue,
		Btn_Npc_Exit_2
	};

};

#endif // LayerUpgrade_h__
