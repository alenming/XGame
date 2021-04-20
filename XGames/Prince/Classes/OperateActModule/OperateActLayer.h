/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		OperateActLayer
 *  Description:	OperateAct层模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			12/7/2015
 *  Others:			
 *
 ******************************************************************/
#include "OperateActHeader.h"
#include "OperateActController.h"
#include "OperateActModel.h"
#include "DataManager/DataTable.h"
#include "Widget/LayerCommFormation.h"
#include "cocos-ext.h"

USING_NS_CC_EXT;

#ifndef  OperateActLayer_h_
#define  OperateActLayer_h_

class OperateActLayer:public Layer,public Observer, public TimeLocalListener,public TableViewDelegate, public TableViewDataSource
{
public:
	~OperateActLayer();

	CREATE_FUNC(OperateActLayer);

public:
	//实现基类函数
	virtual void tableCellTouched(extension::TableView* table, extension::TableViewCell* cell);
	virtual Size cellSizeForTable(extension::TableView *table);   
	virtual extension::TableViewCell* tableCellAtIndex(extension::TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(extension::TableView *table);

	virtual void scrollViewDidScroll(extension::ScrollView* view);

	//设置cell内容
	void setCellContent(extension::TableViewCell* cell, ssize_t idx);

	//重写Button触摸事件
	void onBtnTouchEvent(Button *btn);
	 
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	//计时通知 param: 时间间隔
	virtual void timeTick(int dt);
protected:
	OperateActLayer();

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	void updateNextFram(float ft);
	void onScrollViewEvent(Ref* pSender, cocos2d::ui::ScrollView::EventType type); //滑动列表事件 

	void initUi();
	void operatSort(); //运营活动排序
	void setOperateTishi();//左边提示
	void setOperateActive();//右边面板
	void reSetOperate(bool isEndTimeOut = false);//重置
	void setOperatListView(bool isEndTimeOut = false,bool isPnlIn = false); //设置运营活动列表

	void setOperatListEndTime(); //设置运营活动列表倒计时
	bool isNeedStopTime(int &maxDaojishi);
	OPERATE_ACT_SORT_ITEM* getAllOperatActivtyItemById(int id); //获取
	OPERATE_ACT_SORT_ITEM* getAllOperatActivtyItemByTaskId(int taskId); //获取

	void closeAllPanel();
	void setAllPnlOut();
	void setAllPnlIn();

	//#1 登陆礼包
	void updateSevenInfo(); //设置七日礼包面板数据
	bool isSevenHave(); //是否可以领取七日礼包
	bool isSevenExhibition(bool isEndTimeOut = false); //七日礼包是否领取完毕
	void sevenListSort(OPERATE_ACT_SORT_ITEM* item ); //七日礼包排序
	
	//#2 月卡
	void updateRemadayInfo(); //设置超值月卡面板数据

	//#3 战力排行
	void updateFightRankInfo(); //设置战力排行面板数据
	bool isFightRankExhibition(bool isEndTimeOut = false);

	//#4 强力伙伴
	void updateSuperFriendInfo(); //设置强力伙伴面板数据
	bool isSuperFriendExhibition(bool isEndTimeOut = false);

	//#5 理财通
	void updateFinacialInfo(bool updateByIdx = false); //设置理财面板数据
	bool isFinacialHave(); //是否可以领取理财礼包
	bool isFinacialExhibition(bool isEndTimeOut = false); //理财礼包是否领取完毕

	//#6 全民福利
	void updateNationWellInfo(); //设置全民福利面板数据
	bool isNationWellHave(); //是否可以领取全民福利礼包
	bool isNationWellExhibition(bool isEndTimeOut = false); //全民福利礼包是否领取完毕
	void NationWellSort(OPERATE_ACT_SORT_ITEM* item); //全民福利排序

	//#7 通用模板A（每日5点重置）
	void updateCommomAInfo(int id); //设置通用模板A面板数据
	bool isCommomAHave(int id); //是否可以领取通用模板A礼包
	bool isCommomAExhibition(int id,bool isEndTimeOut = false); //通用模板A礼包是否领取完毕
	void CommomASort(int id); //通用模板A排序

	//#8 通用模板A_
	void _updateCommomAInfo(int id); //设置通用模板A面板数据
	bool _isCommomAHave(int id); //是否可以领取通用模板A礼包
	bool _isCommomAExhibition(int id,bool isEndTimeOut = false); //通用模板A礼包是否领取完毕
	void _CommomASort(int id); //通用模板A排序

	//#9 通用模板B(每日5点重置）
	void updateCommomBInfo(int id); //设置通用模板B面板数据
	bool isCommomBHave(int id); //是否可以领取通用模板B礼包
	bool isCommomBExhibition(int id,bool isEndTimeOut = false); //通用模板B礼包是否领取完毕
	void CommomBSort(int id); //通用模板B排序

	//#10 通用模板B_
	void _updateCommomBInfo(int id); //设置通用模板B面板数据
	bool _isCommomBHave(int id); //是否可以领取通用模板B礼包
	bool _isCommomBExhibition(int id,bool isEndTimeOut = false); //通用模板B礼包是否领取完毕
	void _CommomBSort(int id); //通用模板B排序

	//#11 单笔充值
	void updateSingleRechargeInfo(); //设置单笔充值面板数据
	bool isSingleRechargeHave(); //是否可以领取单笔充值礼包
	bool isSingleRechargeExhibition(bool isEndTimeOut = false); //单笔充值礼包是否领取完毕
	void SingleRechargeSort(); //单笔充值排序

	//#12 双倍掉落
	void updateDoubleFallInfo(int id); //设置单笔充值面板数据
	bool isDoubleFallExhibition(int id,bool isEndTimeOut = false); //单笔充值礼包是否领取完毕

	//#13 限时抢购
	void updateLimitBuyInfo(); //设置限时抢购面板数据
	bool isLimitBuyHave(); //是否可以购买限时抢购
	bool isLimitBuyExhibition(bool isEndTimeOut = false);

	//#14 超值礼包
	void updateVipGiftInfo(); //设置强力伙伴面板数据
	bool isVipGiftHave(); //是否可以购买超值礼包
	bool isVipGiftExhibition(bool isEndTimeOut = false);
	void setVipGiftCHoose(int idx);

	//设置通用模板列表按钮状态和进度
	void setCommonListState(Layout* pnlItem, int idx);
	//show奖励
	void showPopReward(vector<CHABGE_PRIZE_ITEM> reward);

public:
	static bool isOpen(OPERATE_ACT_SORT_ITEM info,bool isEndTimeOut = false); //每种条件不同
	static bool comonARefType(int refType,OPERATE_ACT_INFO_ITEM info); //A种条件不同
	static bool comonBRefType(OPERATE_ACT_INFO_ITEM info,COMMONB comB); //B种条件不同
	static void comoneDoubleFallRefType(OPERATE_ACT_INFO_ITEM info); //双倍掉落条件不同
	static bool singleRechargeRefType(OPERATE_ACT_INFO_ITEM info,vector<int> condition); //单笔充值(逐条对应)

	OPERATE_ACT_SORT_ITEM* searchActivyByType(OPERATE_ACT_TYPE type);
	OPERATE_ACT_SORT_ITEM* searchBuChongActivyById(int id);

private:

	UI_OPERATE_ACT_ENTER   m_ui;   //控件集合
	CustomScrollBar *m_OpListLicaiBar;  //理财通列表
	CustomScrollBar *m_OpSevenBar;  //七日列表
	CustomScrollBar *m_OpNationWellBar;  //全民福利列表
	CustomScrollBar *m_OpComomABar;  //全民福利列表
	CustomScrollBar *m_OpRankABar;  //全民福利列表


	bool isFirstCheckBox; //头次进来优先显示第一个
	OPERATE_ACT_TYPE isFirstOperateType;  //头次进来设置一个活动类型

	int  chooseCommonOprateId; //切换通用活动的id

	VEV_OPERATE_ACT_SORT_ITEM m_operatLocalSort; //运营活动本地列表
	VEV_OPERATE_ACT_SORT_ITEM m_operatSort; //运营活动列表排序后

	//公用tableview
	TableView* m_pCommonTabView;
	OPERATE_ACT_SORT_ITEM* m_sOprateItemData;
	OPERATE_ACT_TYPE m_eCurActType;
	bool m_bIsSelect;			//是否选中了通用匡中的按钮(包括UIToolHeader)

	int m_vipCheckBoxIdx;//超值礼包领取索引

	Vec2 m_OpListViewPos; //左边列表滑动

	bool m_isHavaSuperFriend; //是否有强力伙伴
	bool m_isShowSuperFriendTiShi;//是否有强力伙伴提示
	bool m_isChooseRemaday; //是否选择超值月卡界面
};
#endif // !PvpLayer_h_