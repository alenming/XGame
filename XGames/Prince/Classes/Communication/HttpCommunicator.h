#ifndef HttpCenter_h__
#define HttpCenter_h__

#include "AbstractCommunicator.h"
#include "network/HttpClient.h"
#include "network/HttpResponse.h"

USING_NS_CC;
using namespace network;
const int MAX_HTTP_REQUEST_SEND_TIMES = 3;
typedef class HTTPRequestMessage
{
public:
	HTTPRequestMessage()
	{
		sendTimes = 0;
		pHttpRequest = NULL;
	}
	~HTTPRequestMessage()
	{
		if (pHttpRequest != NULL)
		{
			pHttpRequest->release();
		}
	}
	int sendTimes;
	HttpRequest* pHttpRequest;
}HTTPREQUESTMESSAGE;



class HttpCommunicator : public AbstractCommunicator
{

public:

	HttpCommunicator();
	virtual ~HttpCommunicator();

public:

	//获取单实例
	static HttpCommunicator* getInstance();

	//发送Http消息
	void sendMessage(int nCmdID, const char* szData, const char* url, bool showWaiting = true);

	//发送HTTP登录消息
	void sendHttpLoginMsg(int nCmdID, const char* szData);

	//发送HTTP注册消息
	void sendHttpRegisterMsg(int nCmdID, const char* szData);

	//发送Http试玩消息
	void sendHttpTryGameMsg(int nCmdID, const char* szData);

	//发送Http修改密码消息
	void sendHttpModifyPwdMsg(int nCmdID, const char* szData);

	//发送Http激活帐号消息
	void sendHttpAtivateMsg(int nCmdID, const char* szData);

	//发送HTTP拉取分区列表的消息
	void sendGetZoneListMsg(int nCmdID, const char* szData);

	//Http回调相应
	void onHttpResponse(HttpClient *sender, HttpResponse *response);

	///////////////////////////////

	// [天拓登陆]用户名校验;
	void sendTTCheckUName(int nCmdID, const char* szData);

private:

	std::map<int, HTTPREQUESTMESSAGE*> m_mapRequest; 
};



#endif //HttpCenter_h__