#ifndef MyGroupController_h__
#define MyGroupController_h__

#include "Communication/ICommListener.h"
#include "MyGroupDataDef.h"

class MyGroupController : public ICommListener
{
public:
	static MyGroupController* getInstance();
	static void destroyInstance();

public:
	//发送协议

	/*****************帮派界面上的协议******************/
	//发送请求帮派主界面
	void sendGetGroupMainMsg();

	//发送请求帮派动态和宣言
	void sendGetDynamicEventMsg();

	//发送请求退出帮派
	void sendExitGroupMsg();

	//发送请求解散帮派
	void sendDisbandGroupMsg();

	//发送请求修改宣言, 参数1.对内宣言, 参数2.对外宣言
	void sendModifyDeclarationMsg(string content1, string content2);

	//发送修改帮派头像
	void sendModifyHeadIconMsg(int iconId);
		

	/*****************设置，审核相关的协议********************/
	//发送请求设置帮派加入条件
	void sendSetJoinLimitMsg(sJoinLimit limit);

	//发送请求申请人列表
	void sendGetApplicantMsg();

	//发送请求新人审批, 参数1.玩家ID，参数2.是否同意
	void sendCheckJoinMsg(int uId, bool isAllow);


	/*****************成员管理协议********************/
	//发送请求任命职位
	void sendAppointMsg(int uId);

	//发送请求转让帮主
	void sendAbdicateMsg(int uId);

	//发送请求逐出帮派
	void sendExpelMsg(int uId);

	
	/******************帮派建设相关协议*******************/
	//发送请求建设界面
	void sendGetBuildInfoMsg(eBuildType type);

	//发送请求捐赠建筑
	void sendDonateBuildMsg(eBuildType type, eDonateType itemType);


	/******************声望商店相关*********************/
	//发送请求商店界面
	void sendGetPrestigeShopMsg();

	//发送请求该买商品
	void sendBuyProductMsg(int productId);

	/******************声望活动相关*********************/
	//发送点击九宫列表协议
	void sendClieckNineList(int listId);

	//发送刷新列表协议
	void sendRefreshList();

	//发送请求寻龙历史活动数据
	void sendXunlongHistory();

private:
	MyGroupController();
	~MyGroupController();

private:
	//解析协议

	/*****************帮派界面上的协议******************/
	//解析帮派主界面
	void parseGroupMainMsg(const char* szData);

	//解析帮派动态和宣言
	void parseDynamicEventMsg(const char* szData);

	//解析解散、退出、逐出、推送解散帮派
	void parseExitGroupMsg(const char* szData, int cmd);

	//解析修改宣言
	void parseModifyDeclarationMsg(const char* szData);

	//解析修改头像
	void parseModifyHeadIconMsg(const char* szData);


	/*****************设置，审核相关的协议********************/
	//解析设置帮派加入条件
	void parseSetJoinLimitMsg(const char* szData);

	//解析申请人列表
	void parseApplicantMsg(const char* szData);

	//解析新人审批
	void parseCheckJoinMsg(const char* szData);


	/*****************成员管理协议********************/
	//解析职位任命
	void parseAppointMsg(const char* szData);

	//解析转让帮主
	void parseAbdicateMsg(const char* szData);

	//解析逐出帮派
	void parseExpelMsg(const char* szData);


	/******************帮派建设相关协议*******************/
	//解析建设界面
	void parseBuildInfoMsg(const char* szData, int cmd);


	/******************声望商店相关*********************/
	//解析商店界面
	void parsePrestigeShopMsg(const char* szData);

	//解析该买商品
	void parseBuyProductMsg(const char* szData);

	/******************推送相关协议*******************/
	//推送解散帮派, 同解散帮派
	//推送逐出帮派, 同退出帮派
	//推送自动转让职位, 同转让帮主
	//推送职位任命, 同任命副帮主

	//推送新人审批结果
	void parseCheckJoinPushMsg(const char* szData);

	/******************声望活动相关*********************/
	//解析点击九宫列表协议
	void parseClickNineList(const char* szData);

	//解析刷新列表协议
	void parseRefreshList(const char* szData);

	//解析寻龙历史数据
	void parseXunLongHistory(const char* szData);


public:

	virtual void onRecvMessage(int nCmdID, const char* szData);

private:

	//玩家登陆时就需要创建
	static MyGroupController* ms_pInstance;
};

#endif	//MyGroupController