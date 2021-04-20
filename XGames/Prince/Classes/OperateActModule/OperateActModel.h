/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		OperateActModel
 *  Description:	OperateAct数据模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			12/7/2015
 *  Others:			
 *
 ******************************************************************/
#include "OperateActHeader.h"


#ifndef OperateActModel_h_
#define OperateActModel_h_

class OperateActModel:public Observable
{
public:
	~OperateActModel();

	//活动单例
	static OperateActModel* getInstance();
	static void destroyInstance();

	//通知监听者
	void notify(OPRATEACT_EVENT_TYPE eventType, void* data = nullptr);

	void parseOprateSuppleMentTable(const Json::Value &data); //解析获取运营活动补充表
	void parseOprateActInfo(const Json::Value &data); //解析获取运营活动信息
	void parseOprateActPrize(const Json::Value &data); //解析领取奖励
	void parseOprateActFinancial(const Json::Value &data); //解析理财通
	void parseVipGift(const Json::Value &data);//解析超值礼包

	VEV_OPERATE_ACT_SORT_ITEM getOperatActivty() {return m_operatActivty;}; //运营活动补充列表排
	COMMONA* getComon7ById(int id);
	COMMONA* getComon8ById(int id);
	COMMONB* getComon9ById(int id);
	COMMONB* getComon10ById(int id);

	int getLoginTimes() {return m_LoginTimes;};//获取登陆天数
	vector<int>& getActTyp1() {return m_ActTyp1;};  //#记录已领取的7天奖励ID集合
	vector<CHABGE_PRIZE_ITEM> getRewardShow() {return rewardShow;}; //奖励

	int getRemaday() {return m_Remaday;}; //获取月卡剩余天数
	bool getCard2Status() {return m_Card2Status;}; //是否购买至尊月卡
	void setBuyMonthCard();
	void setBuyExtremeCard();
	bool& getIsBuyMonthCard() {return m_isBuyMonthCard;}; 
	bool& getIsBuyExtremeCard() {return m_isBuyExtremeCard;}; 

	int	getMyRankId() {return m_MyRankId;};	     //我的排名
	int	getMyTotalAtk() {return m_MyTotalAtk;};	 //我的战斗力
	vector<OPERATE_PLAYER_RANK> getPlayerRank() {return m_playerRank;};

	bool getBuyTyp5() {return m_BuyTyp5;};		// # 是否购买理财通 0:false
	int  getMyLevel() {return m_Level;};		// # 我的等级
	vector<int>& getActTyp5() {return m_ActTyp5;}; // # 已领取的领柴通奖励ID

	int getAllBuyCount() {return m_AllBuyCount;};   // #全民福利购买次数
	vector<int>& getActTyp6() {return m_ActTyp6;};  //#已领取的全民福利ID
	
	SINGLE_RECHARGE* getActInfo11() {return &m_ActInfo11;}; //已经领取的单笔充值数据

	vector<int>& getActTyp14() {return m_ActTyp14;};  //#已领取的超值礼包ID

	int&  getCreateTime() {return m_createTime;};	 //活动结束剩余时间（角色创建时间timeTyp2）时间戳
	int&  getOpenTime() {return m_openTime;};	 //活动结束剩余时间（开服时间timeTyp1） 时间戳

	int getCurTaskId() {return m_CurTaskId;}; //当前id 

	bool isHaveGet(); //是否有可以领取的东西
	bool getIsLoad() {return isLoad;};
	vector<int>&  getPetIdList() {return m_petIdList;};
private:
	OperateActModel();

	static OperateActModel* mInstance;

	VEV_OPERATE_ACT_SORT_ITEM m_operatActivty; //运营活动补充列表排

	//#1 七日礼包
	int m_LoginTimes;  //# 登录天数
	vector<int>	m_ActTyp1;  //#记录已领取的7天奖励ID集合

	//#2 月卡
	int m_Remaday;     //#月卡剩余天数 <=0:false
	bool m_Card2Status; // #是否已购买 至尊月卡 0:false
	vector<CHABGE_PRIZE_ITEM> rewardShow; //奖励
	bool m_isBuyMonthCard; //是否跳转到充值界面购买月卡
	bool m_isBuyExtremeCard;//是否跳转到充值界面购买尊卡

	//#3 战力排行
	int	m_MyRankId;	     //我的排名
	int	m_MyTotalAtk;	 //我的战斗力
	vector<OPERATE_PLAYER_RANK> m_playerRank;

	//#4 强力伙伴

	//#5 理财通
	bool m_BuyTyp5;		// # 是否购买理财通 0:false
	int  m_Level;		// # 我的等级
	vector<int> m_ActTyp5; // # 已领取的领柴通奖励ID
	
	//#6 全民福利
	int m_AllBuyCount;   // #全民福利购买次数
	vector<int> m_ActTyp6;  //#已领取的全民福利ID

	//#7  通用模板A
	VEC_COMMONA m_comon7;

	//#8  通用模板A_
	VEC_COMMONA m_comon8;
	
	//#9  通用模板B
	VEC_COMMONB m_comon9;
	//#10 通用模板B_
	VEC_COMMONB m_comon10;

	//#11 单笔充值
	SINGLE_RECHARGE m_ActInfo11;  //#已领取的单笔充值

	//#14 超值礼包
	vector<int> m_ActTyp14;  //#已领取的超值礼包ID

	int  m_createTime;	 //角色创建时间
	int  m_openTime;	 //服务器开启时间

	//当前taskid
	int m_CurTaskId; 
	bool isLoad;

	vector<int> m_petIdList; // -1 是没有整卡 	有的话就是卡牌的ID
};


#endif