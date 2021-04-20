/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		JoinGroupLayer
 *  Description:	帮派建设操作的Layer实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef MyGroupBuildLayer_h__
#define MyGroupBuildLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "MyGroupDataDef.h"

USING_NS_CC;

using namespace cocos2d::ui;	
using namespace std;

enum BUILD_CALLBACK_TYPE
{
	//退出
	BTN_BUILD_EXIT,
	//普通捐献
	BTN_COMMON,
	//高级捐献
	BTN_HIGHTLEVEL,
	//土豪捐献
	BTN_MONEY
};
struct UI_JUAN_XIAN
{
	cocos2d::ui::Text* uiTexExp;			//经验值
	cocos2d::ui::Text* uiTexShengWang;		//声望值
	cocos2d::ui::Text* uiTexMoney;			//金钱
	cocos2d::ui::Button* uiBtnDonate;		//捐献按钮
};
struct UI_GroupBuild
{
	cocos2d::ui::Layout* uiRoot;//根结点
	cocos2d::ui::Button* uiExit;//退出
	cocos2d::ui::Layout* uiLayTouXiang;//建设图片
	cocos2d::ui::Text* uiTexBuildName;//建设名字
	cocos2d::ui::Text* uiTexBuildLevel;//建设等级

	cocos2d::ui::LoadingBar* uiLoaBuildExp;//建筑经验条
	cocos2d::ui::LoadingBar* uiLoaTodayExp;//当天经验条
	cocos2d::ui::Text* uiTexBuildExp;//建筑经验条文本
	cocos2d::ui::Text* uiTexTodayExp;//当天经验条文本

	cocos2d::ui::Text* uiTexNextLevelEffeck;//下一级效果
	cocos2d::ui::Text* uiTexToDayJuanXian; //今日捐献
	cocos2d::ui::Text* uiTexToDayCount;//当天捐献次数
	
	cocos2d::ui::LoadingBar* uiLodbShip;//商店经验
	cocos2d::ui::LoadingBar* uiLodbGroup;//帮派经验 

	UI_JUAN_XIAN	uiOrdinary;//普通
	UI_JUAN_XIAN	uiSenior;//高级
	UI_JUAN_XIAN	uiTuHao; //土豪
};

class MyGroupBuildLayer : public ModalLayer,public Observer
{
public:
	MyGroupBuildLayer();
	~MyGroupBuildLayer();

	static Layer* create(eBuildType buildType);
	virtual void updateSelf(void* data);
public:
	bool init(eBuildType buildType);

	virtual void onEnter();

	virtual void onExit();
private:
	//初始化各种UI
	void  initUI();
	//初始化帮派信息
	void initBuildInfo();
	//初始化捐献信息
	void iniJuanXian();

	void  onBtnClick( Ref* reff, Widget::TouchEventType type, BUILD_CALLBACK_TYPE mBtnCallBackType);
	//退出回调
	void  onBtnExitCallBack();
	//普通捐献回调
	void onBtnCommonCallBack();
	//高级捐献回调	
	void  onBtnHoightCallBack();
	//土豪捐献回调
	void  onBtnMoneyCallBack();
	//刷新
	void refreshBuildUIinfo();
private:
	UI_GroupBuild	m_ui;
	eBuildType _buildType;
	bool isChaoZhou;

};

#endif //MyGroupBuildLayer_h__