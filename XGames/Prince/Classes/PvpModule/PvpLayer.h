/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		PvpLayer
 *  Description:	Pvp层模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/30/2015
 *  Others:			
 *
 ******************************************************************/
#include "PvpHeader.h"
#include "PvpController.h"
#include "PvpModel.h"
#include "DataManager/DataTable.h"
#include "Widget/LayerCommFormation.h"
#include "cocos-ext.h"

#ifndef PvpLayer_h_
#define  PvpLayer_h_

const float PANEL_CHANGE_TIME = 0.3f;
const int FIRST_RANK_COUNT = 3; //前三名
const int MY_RANK_COUNT_CAN_PVP = 12; //多少名才能挑战前三

const int PVP_DAY_COUNT_MAX = 10;//每日胜场最大次数
const float _fadeTime = 0.4f;
const int PLAYER_ITEM_COUNT = 5;

#define PLAYER_ITEM_LIST 1000

class PvpLayer:public Layer, public extension::TableViewDelegate, public extension::TableViewDataSource,public Observer
{
public:
	~PvpLayer();

	static PvpLayer* create(bool pushScene);
	 
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);


	//设置Pvp拉取信息
	void setPvpGetInfo();//设置挑战玩家信息
	void _addPlayerList();
	void _setItemByIdx(extension::TableViewCell* cell,int idx);
	//实现基类函数
	virtual void tableCellTouched(extension::TableView* table, extension::TableViewCell* cell);
	virtual Size cellSizeForTable(extension::TableView *table);   
	virtual extension::TableViewCell* tableCellAtIndex(extension::TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(extension::TableView *table);

	virtual void scrollViewDidScroll(extension::ScrollView* view);

	void addRankPrizeList();  //添加排行榜奖励列表
	void addVictoryRewardList();  //添加胜场奖励列表
	void addRankingList();  //添加排行榜列表
	void showPopReward();//show奖励

	void setVictoryRewardTips();//添加胜场奖励提示
	void setFightEnd();
	void setAnimationlUp(); //设置排名升级动画

protected:
	PvpLayer();

	bool init(bool pushScene);
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi();
	void addHeroAnimit();
	void initPrize();
	void initFightSettlement();
	void initVictoryReward();
	void initRanking();

	//按钮事件
	void onBtnNotify(Ref* sender, Widget::TouchEventType type); //按钮
	void onBtnSendGet(Ref* sender, Widget::TouchEventType type,int idx); //获取胜场奖励
	void onBtnViewPlayerLine(Ref* sender, Widget::TouchEventType type, int idx,bool isPlayer); //查看对手玩家阵容
	void onBtnSendFight(Ref* sender, Widget::TouchEventType type,PVP_PLAYER_DATA enemy); //战斗

	void onScrollViewEvent(Ref* pSender, ScrollView::EventType type); //滑动列表事件 
private:
	UI_PVP_ENTER   m_ui;   //控件集合

	CustomScrollBar *m_RankPalyerBar;  //添加排行榜奖励列表
	CustomScrollBar *m_RankPrizeListBar;  //添加排行榜奖励列表
	CustomScrollBar *m_VictoryRewardListBar;  //添加胜场奖励列表
	CustomScrollBar *m_RankingListBar;  //添加排行榜列表

	int    m_rewardInx; //领取的奖励索引

	bool   m_isAddScrollViewEvent;
	bool   m_isSetFightEnd;
	bool   m_isPushScene;


	extension::TableView *m_tMemberList;
};




#endif // !PvpLayer_h_
