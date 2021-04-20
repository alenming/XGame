/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		PvpModel
 *  Description:	Pvp数据模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/30/2015
 *  Others:			
 *
 ******************************************************************/
#include "PvpHeader.h"


#ifndef PvpModel_h_
#define PvpModel_h_

class PvpModel:public Observable
{
public:
	~PvpModel();

	void parsePvpTable();
	PVP_REWARD_ITEM_VEC *getRewardPvp() {return &m_rewardPvp;};
	PVP_REWARD_ITEM_VEC *getRewardWIN() {return &m_rewardWIN;};

	//活动单例
	static PvpModel* getInstance();
	static void destroyInstance();

	//通知监听者
	void notify(PVP_EVENT_TYPE eventType, void* data = nullptr);


	void parsePvpInfoData(const Json::Value &data); //解析主界面进入拉取Pvp数据
	void parsepvpSetMentAccountsData(const Json::Value &data); //解析竞技场结算数据
	void parsepvpRankData(const Json::Value &data); //解析竞技场排名
	void parsePvpPrizeData(const Json::Value &data); //解析竞技场奖励
	void parsePvpCount(const Json::Value &data);//解析竞技场购买次数


	int& getpvpOrder() {return m_pvpCurOrder;}; //获得pvp主角当前名次
	int& getpvpBestOrder() {return m_pvpBestOrder;}; //获得pvp主角最好名次

	int& getpvpOrderInc() {return m_pvpCurOrderInc;}; //获得当前上升主角当前名次
	int& getpvpBestOrderInc() {return m_pvpBestOrderInc;}; //获得历史上升主角最好名次

	int& getGoldCount() {return m_goldCount;}; //获得金币

	int& getPvpScore() {return m_pvpScore;}; //获得pvp分数
	int& getChangeFlag() {return m_changeFlag;}; //获得排名变提示
	int& getPvpDayCount() {return m_pvpWinCount;};//获得每天胜场次数
	int& getPvpCount() {return m_pvpCount;}; //获得剩余挑战次数
	int& getBuyCount() {return m_buyCount;};//购买次数

	PVP_PLAYER_DATA_VEC& getPlayerVec() {return m_player;}; //获取玩家
	PVP_PLAYER_DATA_VEC& getRank() { return m_rank; }; //获取排名
	PVP_ReWard_Show& getRewardShow() { return m_rewardShow; };//展示奖励
	vector<int>& getRewardFlag() {return m_rewardFlag;};//胜场奖励领取状态

	PVP_PLAYER_DATA* getFightEnemy() {return &m_fightEnemy;};
	void setFightEnemy(PVP_PLAYER_DATA enemy) {m_fightEnemy = enemy;};
	void setWin(bool win) {m_win = win;};
	bool getWin() {return m_win;};

private:
	PvpModel();

	static PvpModel* mInstance;

	PVP_REWARD_ITEM_VEC m_rewardPvp;
	PVP_REWARD_ITEM_VEC m_rewardWIN;

	PVP_PLAYER_DATA_VEC m_player; //玩家
	PVP_PLAYER_DATA_VEC m_rank; //排名
	PVP_ReWard_Show m_rewardShow;//展示奖励

	vector<int> m_rewardFlag;//胜场奖励领取状态

	int  m_pvpCurOrder;       //主角当前名次
	int  m_pvpBestOrder;   //主角历史最高名次
	int  m_pvpCurOrderInc;  //当前上升
	int  m_pvpBestOrderInc;  //历史上升
	int  m_goldCount;//金币

	int  m_pvpScore;   //PvP分数
	int  m_changeFlag; //排名变提示
	int  m_pvpWinCount; //每天胜场次数
	int  m_pvpCount; //剩余次数
	int  m_buyCount; //已经购买次数

	bool m_win;


	PVP_PLAYER_DATA m_fightEnemy; //挑战敌人
};


#endif