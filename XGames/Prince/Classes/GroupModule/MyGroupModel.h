#ifndef MyGroupModel_h__
#define MyGroupModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "MyGroupDataDef.h"


class MyGroupModel : public Observable
{
public:
	static MyGroupModel* getInstance();

	static void destroyInstance();

	//获取所在帮派信息
	sMyGroupData* getMyGroupInfo();

	//获取所有帮众信息
	vector<sPlayerInfo>* getAllPlayerInfo();

	//获取指定UID的帮派成员信息
	sPlayerInfo* getPlayerInfoByUid(int uId);

	//获取我的信息
	sMyInfo* getMyInfo();

	//根据职位获取职位名称
	string	getPostPowerName(ePostPower post);

	//获取申请人信息列表
	vector<sProposerInfo>* getAllProposerInfo();

	//获取宣言
	sDeclaration* getGroupDeclaration();

	//获取动态事件日期列表
	vector<sEventDate>* getGroupEventDate();

	//根据日期获取动态事件, param 1. 日期, 2. 接收事件的结构体
	void getGroupEventByDate(sEventDate date, vector<sDynamicEvent>* vecEvent);

	//获取帮派加入条件
	sJoinLimit* getJoinLimit();

	//获取帮派建筑信息
	vector<sBuildInfo>* getGroupBuildInfo();

	//获取指定的帮派建筑信息
	sBuildInfo* getBuildInfoByID(eBuildType type);

	//获取帮派活动建筑信息
	vector<sActivityBuildInfo>* getGroupActivityBuildInfo();

	sActivityInfo* getGroupActivityInfo();

	//获取指定的帮派活动建筑信息
	sActivityBuildInfo* getGroupActivityBuildInfoByID(eActivityType type);

	//获取玩家最后登录的时间描述, param: 玩家uid
	string getLastLoginTimeDes(int uId);

	//根据帮派图标资源Id获取帮派图标路径
	string getGroupIconPath(int iconId);

	//根据建筑资源Id获取建筑图标路径
	string getBuildIconPath(int buildId);

	//根据捐赠类型查询捐赠数据, param: 1. 捐赠类型, 2. 接收数据的指针
	void getDonateDataByType(eDonateType type, sDonateLocalDT* donateData);

public:

	//更新帮派主界面
	void updateGroupInfoFromSvr(const Json::Value& data);

	//更新帮众信息
	void updateAllPlayerInfoFromSvr(const Json::Value& data, int* viceRecord);

	//更新我的信息
	void updateMyInfo(const Json::Value& data);

	//更新帮派宣言
	void updateGroupDeclaration();

	//更新帮派事件
	void updateGroupEvent(const Json::Value& data);

	//更新帮派加入条件
	void updateJoinLimit(sJoinLimit limit);

	//更新帮派建筑信息
	void updateGroupBuildInfo(const Json::Value& data);

	//更新指定帮派建筑信息
	void updateBuildInfoByType(const Json::Value& data, eBuildType type, int cmd);

	//更新服务器申请人列表信息
	void updateProposerList(const Json::Value& data);

	//更新申请人信息
	void updateProposerInfo();

	//更新某个成员职位权限
	void updateMemberPower(int cmd);

	//解散, 逐出, 退出, 推送解散, 推送逐出处理
	void updateExitGroup(int cmd);

	//帮派活动在主界面信息拉取时调用
	void updateGroupActivity(const Json::Value& data);

	//帮派活动寻龙刷新后更新界面
	void updateGroupActivityRefresh(const Json::Value& data);

	//帮派寻龙活动点击之后更新界面
	void updateGroupActivityClick(const Json::Value& data);

	//帮派寻龙活动历史数据后刷新界面
	void updateGroupActivityHistory(const Json::Value& data);

	//旗帜总数
	CC_SYNTHESIZE_READONLY(int, m_iFlagNum, FlagNum);

	//推送消息标志
	CC_SYNTHESIZE(int, m_iFlagPushMsg, FlagPushMsg);

	/***************发送协议前界面记录的一些数据****************/
	//帮派旗帜ID
	CC_SYNTHESIZE(int, m_iFlagTempID, FlagTempID);
	//帮派转让: 受转让ID, 原帮主ID
	CC_SYNTHESIZE(int, m_newLeaderID, NewLeaderID);
	CC_SYNTHESIZE(int, m_oldLeaderID, OldLeaderID);
	//帮派职位任命: 被降低者ID, 被提升者ID
	CC_SYNTHESIZE(int, m_oldViceID, OldViceID);
	CC_SYNTHESIZE(int, m_newViceID, NewViceID);
	//新人审核ID和是否同意
	CC_SYNTHESIZE(int, m_applicantID, ApplicantID);
	CC_SYNTHESIZE(bool, m_bIsAllow, TempIsAllow);
	//逐出帮派ID
	CC_SYNTHESIZE(int, m_expelID, ExpelID);
	//帮派加入条件
	CC_SYNTHESIZE(sJoinLimit, m_joinLimit, TempJoinLimit);
	//帮派宣言
	CC_SYNTHESIZE(string, m_inNotice, TempInNotice);
	CC_SYNTHESIZE(string, m_outNotice, TempOutNotice);
	//建筑类型
	CC_SYNTHESIZE(eBuildType, m_eBuildType, TempBuildType);
	//捐献类型和捐献花费的金币
	CC_SYNTHESIZE(eDonateType, m_eDonateType, TempDonateType);

	CC_SYNTHESIZE(int , mIsXunLONG,IsXunLong);
private:
	MyGroupModel();
	virtual ~MyGroupModel();

	//成员数据排序
	void sortAllPlayer();

	//初始化本地配置表相关数据
	//初始化动态事件数据表
	void initDynEventLocalData();

	//读取表格构建所有建筑信息，param 传id则读取指定ID数据并更新指定类型的建筑，不传ID则全部更新
	void initBuildInfo(eBuildType type = eBUILD_DEFAULT_TYPE, int buildId = 0);

	//帮派活动建筑
	void initGroupActivityBuildInfo();

	//初始化捐献数据表
	void initDonateLocalData();

	//根据id取得对应的捐赠数据
	void getDonateDataById(int id, sDonateLocalDT* donateData);
	
	//旗帜数量
	void initFlagNum();

	//格式化时间 xx:xx	时分
	string formatTime(int hour, int min);

	//格式化事件描述
	string formatEventByType(string keyField, int type);

private:

	static MyGroupModel* ms_pInstance;

	//帮派信息
	sMyGroupData m_sMyGroupInfo;
	//帮众信息
	vector<sPlayerInfo> m_vPlayerInfo;
	//我的信息
	sMyInfo m_sMyInfo;
	//申请人信息
	vector<sProposerInfo> m_vProposerInfo;

	//宣言
	sDeclaration m_sDeclaration;
	
	//动态事件日期
	vector<sEventDate> m_vEventDate;

	//帮派动态事件
	vector<sDynamicEvent> m_vGroupEvent;

	//帮派建筑信息
	vector<sBuildInfo> m_vBuildInfo;

	//捐赠数据表
	vector<sDonateLocalDT> m_vDonateDT;

	//帮派活动建筑信息
	std::vector<sActivityBuildInfo> m_vActivityBuildInfo;

	//帮派加入条件
	sJoinLimit m_sJoinLimit;

	//活动数据
	sActivityInfo m_sActivityInfo;

	//点击回来之后是下一次的金币消耗,此变量用来保存这一次的的消耗金额
	int priceOld;
};

#endif