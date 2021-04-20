/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		JoinGroupLayer
 *  Description:	帮派成员操作的Layer实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef MyGroupPlayerInfoLayer_h__
#define MyGroupPlayerInfoLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "MyGroupDataDef.h"

USING_NS_CC;

using namespace cocos2d::ui;	
using namespace std;

enum GROUP_INFOLAYER_CALLBACK
{
	//退出
	BTN_INFO_EXIT,
	//降低职位
	BTN_LOWER_DUTY,
	//提升职位
	BTN_UP_DUTY,
	//逐出帮派
	BTN_EXPEL_GROUP,
	//转让
	BTN_TRANSFER,
	//确认
	BTN_AFFIRM
};

struct UI_GroupPlayerInfo
{
	//根结点
	cocos2d::ui::Layout* uiRoot;

	//退出
	cocos2d::ui::Button* uiExit;
	//左边BUTTON
	cocos2d::ui::Button* uiBtnLeftUP;
	cocos2d::ui::Button* uiBtnLeftDown;
	//中间
	cocos2d::ui::Button* uiBtnmiddle;//逐出
	cocos2d::ui::Button* uiBtnSure;//确认
	//右边
	cocos2d::ui::Button* uiBtnRight;

	//战斗力
	cocos2d::ui::Text* uiTexFightPower;
	//贡献
	cocos2d::ui::Text* uiTexContribution;
	//职务
	cocos2d::ui::Text* uiTexDuty;
	//名字
	cocos2d::ui::Text* uiTexName;	
	//登入时间
	cocos2d::ui::Text* uiTexOnlineTime;
	//VIP等级
	cocos2d::ui::ImageView* uiImaVip;



};

class MyGroupPlayerInfoLayer : public ModalLayer,public Observer
{
public:
	MyGroupPlayerInfoLayer();
	~MyGroupPlayerInfoLayer();
	static Layer* create(int playerID);
	virtual void updateSelf(void* data);
public:
	bool init(int playerID);

	virtual void onEnter();

	virtual void onExit();
private:
	//初始化各种UI
	void  initUI();
	//初始化三个按钮的显示
	void intAutyUI();
	//初始化阵形
	void initToolUI();
	//初始化文本UI
	void initTextUI();

	void  onBtnClick( Ref* reff, Widget::TouchEventType type,GROUP_INFOLAYER_CALLBACK _callBackType);
	//设置玩家职位显示
	void setPowerType();
	//更新玩家职位显示
	void updateUserAuty();
private:
	UI_GroupPlayerInfo	m_ui;

	sPlayerInfo* _playerInfo;

	int _playerID;

	//bool _isChaozhou;
};

#endif //MyGroupPlayerInfoLayer_h__
