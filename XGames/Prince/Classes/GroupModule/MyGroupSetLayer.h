/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		JoinGroupLayer
 *  Description:	帮派设置操作的Layer实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef MyGroupSetLayer_h__
#define MyGroupSetLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"

#include "MyGroupDataDef.h"

USING_NS_CC;

using namespace cocos2d::ui;	
using namespace std;


#define USER_MAX_LEVEL 10108   //玩家最大等级上限
#define USER_OPEN_FUNCTION 10016  //帮派开启等级,也是最低加入帮派等级


enum SETLAYER_CALLBACK_TYPE
{
	//退出
	BTN_SETLAYER_EXIT,
	//解散
	BTN_DISSOLVE_FAC,
	//保存
	BTN_SAVE,
	//设置
	BTN_SET,
	//审核
	BTN_AUDIT,
	//+++
	BTN_ADD,
	//----
	BTN_SUBTRACT
};

struct UI_GroupSet
{
	//根结点
	cocos2d::ui::Layout* uiRoot;

	cocos2d::ui::Layout* uiSetRoot;
	cocos2d::ui::Layout* uiAuditRoot;

	//退出
	cocos2d::ui::Button* uiExit;
	//成员数
	cocos2d::ui::Text* uiTexMember;

	//勾
	cocos2d::ui::CheckBox* uiCheckBoxOn;
	//勾
	cocos2d::ui::CheckBox* uiCheckBoxOff;
	//+++
	cocos2d::ui::Button* uiAdd;
	//-----
	cocos2d::ui::Button* uiSubtract;
	//加入等级Text
	cocos2d::ui::Text* uiTexJoinInLevel;
	//保存
	cocos2d::ui::Button* uiSave;

	//解散
	cocos2d::ui::Button* uiDissolved;
	//设置
	cocos2d::ui::Button* uiSet;
	//审核
	cocos2d::ui::Button* uiAudit;

	cocos2d::ui::ListView* memberList;


};

class MyGroupSetLayer : public ModalLayer,public Observer
{
public:
	MyGroupSetLayer();
	~MyGroupSetLayer();
	static Layer* create();
	virtual void updateSelf(void* data);

public:
	bool init();

	virtual void onEnter();
	void myupdateADD(float dt);
	void myupdateSUB(float dt);
	virtual void onExit();
private:
	//初始化各种UI
	void  initUI();
	//申请成员界面
	void initAutyUi();
	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type, SETLAYER_CALLBACK_TYPE mBtnCallBackType);
	//回调汇总
	void  onBtnClickADDorSub( Ref* reff, Widget::TouchEventType type, SETLAYER_CALLBACK_TYPE mBtnCallBackType);
	//复选框回调
	void  onCheckBoxCallBackON(Ref* reff,CheckBoxEventType type);
	void  onCheckBoxCallBackOFF(Ref* reff,CheckBoxEventType type);
	//解散帮派回调
	void  onBtnDissolveCallBack();
	//保存设置回调
	void  onBtnSaveCallBack();
	//同意加入回调
	void  onBtnAcceptCallBack(Ref* reff, Widget::TouchEventType type,int uid);
	//拒绝加入回调
	void  onBtnRefuseCallBack(Ref* reff, Widget::TouchEventType type,int uid);
	//设置标记
	void setJoinInFlag( );
	//更新加入等级Text
	void setJoinInLevelText(int mJoinInLevel);
	//设置审核方式显示
	void setJoinLimitCheckBox(eJoinType _first);
	//设置+与-之后的显示
	void setAddOrSubtract(bool _bb);
	//设置成员数值显示
	void setMemberText();
	//重新刷新listView
	void ReflushListView();

	//判定长按
	void judgeLongPressed(float delta);

	void onScrollViewEvent(Ref* pSender, ScrollView::EventType type); //滑动列表事件 
private:
	UI_GroupSet	m_ui;
	int mJoinInLevelMin;
	int mJoinInLevelMax;
	sJoinLimit mJoinlimit;

	CustomScrollBar* m_RankPalyerBar;
	bool firstRunToAuty;
	bool isCanLa;

	//是否进入长按状态
	bool isLongPressed;

};

#endif //MyGroupSetLayer_h__