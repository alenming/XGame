#ifndef LoginModel_h__
#define LoginModel_h__

#include "LoginDef.h"
#include "Utils/Observer.h"
#include <string>

using namespace std;

class LoginModel : public Observable
{

public:

	LoginModel(void);
	~LoginModel(void);

	static LoginModel*  getInstance();
	static void  destroyInstance();

public:

	//设置/获取游戏服务器信息
	void  setServerParam(const ServerParam& param);
	ServerParam* getServerParam();

	//获取创建角色的名称，注意：如果没有创建角色，获取不到角色名称
	void setRoleName(string roleName);
	string getRoleName();

	// 服务器列表;
	void  updateServerZoneList(vector<ServerParam>* vcServerParam);

	// 取分区列表(10个以内);
	void  getZoneList(int nIndex, vector<ServerParam>& vcZoneList, int nNeedCount = SERVER_ZONE_DEFAULT_COUNT);

	// 取当前选中的服务器;
	ServerParam*  getDefaultServer();

	//==========自有账号登录数据==========

	//用户id
	void setUserId(int userId);
	int getUserId();

	//sessionId
	void setSessionId(const string& sessionId);
	string getSessionId();

	//==========SDK账号登录数据==========

	// 设置/获取U8用户Id;
	void setU8UserId(string userId);
	string getU8UserId();

	// 设置/获取U8用户Id;
	void setU8UserName(string userName);
	string getU8UserName();

	//设置/获取sdk用户id
	void setSdkUserId(string userId);
	string getSdkUserId();

	//设置/获取sdk_token
	void setToken(string token);
	string  getToken();

	//设置/获取sdk用户名
	void setSdkUserName(string userName);
	string getSdkUserName();

private:

	// 从列表中取一个可用的分区，取一个新区即可;
	ServerParam*  getValidServer();
	
private:

	static LoginModel*  m_pInstance;

	//游戏角色服务器信息
	vector<ServerParam>  m_vcServerList;
	ServerParam  m_ServerParam;

	//创建账号缓存的角色名称
	string mRoleName;

	//==========自有账号登录数据==========

	//登录中心服务器下发的用户id，该字段仅用于自有账号登录
	int mUserId;
	//登录下发的sessionId，该字段仅用于自有账号登录
	string mSessionId;

	//==========SDK账号登录数据==========

	// sdk登陆token;
	string mToken;
	// sdk用户Id
	string mSdkUserId;
	//sdk用户名
	string mSdkUserName;

	// U8用户Id;
	string mU8UserId;
	// U8用户名;
	string mU8UserName;
};

#endif // LoginModel_h__
