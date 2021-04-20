/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerVip
 *  Description:	VIP模块的UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			01/10/2015
 *  Others:			
 *
 ******************************************************************/
#ifndef LayerVip_h__
#define LayerVip_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

#define TOTAL_VIP_LV	8

typedef struct tagUI_Vip 
{
	Layout*		pRoot;
	Layout*		pnlVip;
	Button*		btnClose;
	ImageView*	imgVipLv;
	Text*		txtTips;

	Layout*		pnlDeposit;
	Button*		btnPrivilege;
	Button*		btnGold1;
	Button*		btnGold2;
	Button*		btnGold3;

	Layout*		pnlPrivilege;
	Button*		btnDeposit;
	ImageView*	imgPrivilegeTitle;
	ImageView*	imgVipLvShow;
	Button*		btnLeft;
	Button*		btnRight;
	ScrollView*	scrollViewLvShow;
	Text*		txtLvShow[TOTAL_VIP_LV];

	tagUI_Vip()
	{
		memset(this, 0, sizeof(tagUI_Vip));
	}
}UI_Vip;

class LayerVip : public Layer
{
public:
	~LayerVip(void);

	CREATE_FUNC(LayerVip);

	virtual bool  init();

	virtual void  update(float delta);

	void  onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );

private:
	LayerVip(void);

	void  initUI();

	void  updateLvShow();

private:

	UI_Vip		m_ui;

	int			m_nVipLv;
	int			m_nVipLvShow;

	enum UI_Vip_Widget
	{
		Btn_Close,
		Lab_Tips,
		Btn_Deposit,
		Btn_Privilege,
		Btn_Left,
		Btn_Right,
		ScrollView_LvShow,
		Btn_Gold1,
		Btn_Gold2,
		Btn_Gold3
	};

};

#endif // LayerVip_h__
