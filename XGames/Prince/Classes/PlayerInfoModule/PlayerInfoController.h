#ifndef PlayerInfoController_h__
#define PlayerInfoController_h__

#include "Communication/ICommListener.h"
#include "LoginModule/LoginController.h"

class PlayerInfoController : public ICommListener
{
public:

	PlayerInfoController();
	virtual ~PlayerInfoController();

public:

	virtual void onRecvMessage(int nCmdID, const char* szData);

	//发送获取随机名称指令
	static void sendGetRandomNameMsg(int sex);

	//发送修改昵称指令
	static void sendModifyNickNameMsg(string str);

	//发送兑换指令
	static void sendExchangeMsg(string key);

private:

	//解析服务器修改昵称返回的数据(花费)
	void parseModifyNickNameMsg(const char* szData);

	//解析随机名称数据
	void parseGetRandomNameMsg(const char* szData);

	//解析兑换结果
	void parseExchangeMsg(const char* szData);

public:
	//随机角色昵称缓存的名字
	vector<string> vecRandomNames;
};

#endif