#ifndef LoginController_h__
#define LoginController_h__

#include "ILoginView.h"
#include "HeroModule/HeroModel.h"
#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"
#include "LoginDef.h"

#if (LOGIN_WITH_SDK == 1)

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "SDK/SDKComponent.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "SDK/SDKComponent_ios.h"
//#include "SDK/u8SDKData_ios.h"
#endif

#endif

#include "SDK_TalkingData/TalkingData.h"

struct NameParam
{
	//男性名字
	vector<string> vecMaleNames;

	//女性名字
	vector<string> vecFemaleNames;
};


class LoginController: public ICommListener
{

public:

	LoginController();
	~LoginController();

	static LoginController* getInstance();

public:

	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	//设置UI接口
	void onSetView(ILoginView* pView);

	//移除UI接口
	void onRemoveView(ILoginView* pView);

	// 登陆角色;
	void  loginRole();

	// 创建角色;
	void  createRole(const string& name, int templetId);

	// 拉取服务器列表;
	void  getServerZoneList();

	// 注销;
	void  logoutAccount();

	//发送获取英雄数据信息
	void sendGetHeroDataMsg();

	//发送获取道具消息
	void sendGetToolsMsg();

	//发送获取随机名字消息
	void sendGetRandomNameMsg(bool isMale);

	//发送登出消息
	void sendLogoutMsg();

	//获取缓存的名字信息
	NameParam* getNameParam();

#if (LOGIN_WITH_SDK == 1)

	// sdk登陆结果;
	void  onSdkLoginAccountResult(U8LoginResult* result);

#endif

	void submitExtraData(int type);

	//获取地址 android: mac   ios: idfa
	string getIdentify();

	//获取游客试玩标识
	string getTryGameIdentify();

	//非试玩帐号登陆;
	void  loginAcount(const char* szID, const char* szPwd);

	//试玩帐号登陆 param: uID=0第一次试玩，uID!=0老游客登陆
	void  sendTryGameMsg(int uID = 0);

 	//发送用户注册消息
 	void sendRegAndLoginMsg(const char* userName, const char* szPwd);
 
 	//发送密码修改消息
 	void sendModifyPasswordMsg(const char* userName, const char* szOldPwd, const char* szNewPwd);

	//发送用户激活试玩帐号消息
	void sendActAccountMsg(int uID, const char* userName, const char* szPwd);

	//当前是否注销状态
	bool getIsLogoutState();

	// 向服务器发送设备唯一标识;
	void sendDeviceId();

	//////////////////////////// 

	// [天拓登陆]校验用户名;
	void ttCheckUserName(const char* szUserName);
	void onCheckUserNameRet(const char* szData);

	// [天拓登陆]注册;
	void ttRegister(const char* userName, const char* szPwd);

	void setAutoLogin(bool autoLogin) {m_isAutoLogin = autoLogin;}; //设置自动登录
protected:

	//登录角色
	void sendLoginRoleMsg();

	//发送断线重连消息;
	//void sendReconnectMsg();

	//解析登录消息
	void processLoginData(const char* szData, int nCmd);

	//解析分区列表消息
	void processZoneListData(const char* szData);

	//解析登录角色消息
	void processLoginRoleData(const char* szData, bool isCreateRoleMsg = false);

	//解析英雄列表数据
	void proccessHeroListData(const char* szData);

	//解析获取道具消息
	void processGetToolsData(const char* szData);	

	//解析随机名字消息
	void processRandomNamesData(const char* szData);

	// 解析断线重连成功之后服务器下发的状态消息;
	void processReconnectRet(const char* szData);

	// 充值结果推送;
	void processPayResult(const char* szData);

	// 处理登出回包
	void processLogoutMsg(const char* szData);

	// 处理重复登录
	void processRepeatLoginMsg(const char* szData);

private:

	//登录UI接口
	ILoginView* m_pView;

	//单实例
	static LoginController* ms_pInstance;

	//第一次登录创建角色时缓存的名字
	NameParam mNameParam;

	//角色登录是否成功
	bool mIsRoleLoginSuccess;

	//是否通过注销回到登录界面
	//由于现在要求游戏启动时有帐号记录3s后自动登录，为防止注销后开启登录定时器，此处做个标记
	bool mIsLogoutState;

	// [天拓登陆]暂存用户名和密码;
	string  m_strTmpUName;
	string  m_strTmpUPwd;

	bool m_isAutoLogin;  //是否通过SDK自动登录 （主要用在YSDK用上次登录信息自动登录）

	//记录试玩UID  0:注册试玩  !=0试玩登录
	int m_iTryUID;
};

#endif // LoginController_h__
