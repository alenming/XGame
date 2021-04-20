#ifndef HttpPublisher_h__
#define HttpPublisher_h__

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "network/HttpResponse.h"
#include "network/HttpRequest.h"

USING_NS_CC;
using namespace std;
using namespace network;

class HttpPublisher
{

public:

	HttpPublisher();
	~HttpPublisher();

	//获取单实例
	static HttpPublisher* getInstance();

public:

	//发送打开游戏信息，打开游戏时发送
	void sendOpenGameMsg();

	//发送登录游戏信息，选择分区进入游戏后发送
	void sendLoginGameMsg();

	//发送创建角色信息，创建角色后发送
	void sendCreateRoleMsg();

	//获取客服系统Url
	string getCustomerServiceSystemUrl();

	//苹果正版为IDFA   安卓为MAC
	string getUmacid();

private:

	void onOpenGameRsp(HttpClient *sender, HttpResponse *response);
	void onLoginGameRsp(HttpClient *sender, HttpResponse *response);
	void onCreateRoleMsg(HttpClient *sender, HttpResponse *response);

};




#endif //HttpPublisher_h__
