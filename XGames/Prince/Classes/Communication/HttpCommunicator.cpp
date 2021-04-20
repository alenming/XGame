#include "HttpCommunicator.h"
#include "network/HttpRequest.h"
#include "Json/json.h"
#include "Command.h"
#include "Widget/LayerWaiting.h"
#include "EnvConfig.h"
#include <ostream>
#include <cmath>
#include <cctype>
#include "Utils/SystemUtils.h"


HttpCommunicator::HttpCommunicator()
{

}

HttpCommunicator::~HttpCommunicator()
{
	HttpClient::destroyInstance();
}

HttpCommunicator* HttpCommunicator::getInstance()
{
	static HttpCommunicator httpCommunicator;
	return &httpCommunicator;
}

//发送HTTP登录，注册相关消息
void HttpCommunicator::sendHttpLoginMsg(int nCmdID, const char* szData)
{
	sendMessage(nCmdID, szData, SystemUtils::getLOGIN_SERVER_URL().c_str());
}

void HttpCommunicator::sendHttpRegisterMsg( int nCmdID, const char* szData )
{
	sendMessage(nCmdID, szData, SystemUtils::getREGISTER_URL().c_str());
}

void HttpCommunicator::sendHttpTryGameMsg( int nCmdID, const char* szData )
{
	sendMessage(nCmdID, szData, SystemUtils::getTRY_GAME_URL().c_str());
}

void HttpCommunicator::sendHttpModifyPwdMsg( int nCmdID, const char* szData )
{
	sendMessage(nCmdID, szData, SystemUtils::getMODIFY_PWD_URL().c_str());
}

void HttpCommunicator::sendHttpAtivateMsg( int nCmdID, const char* szData )
{
	sendMessage(nCmdID, szData, SystemUtils::getACTIVATE_URL().c_str());
}

//发送HTTP拉取分区列表的消息
void HttpCommunicator::sendGetZoneListMsg(int nCmdID, const char* szData)
{
	sendMessage(nCmdID, szData, SystemUtils::getZONE_LIST_URL().c_str());
}

void HttpCommunicator::sendMessage(int nCmdID, const char* szData, const char* url, bool showWaiting)
{
	if (showWaiting)
	{
		LayerWaiting::showWaiting();
	}

	//建立连接
	HttpRequest* request = new HttpRequest();
	request->setUrl(url);
	request->setRequestType(HttpRequest::Type::POST);
	request->setResponseCallback(this, httpresponse_selector(HttpCommunicator::onHttpResponse));

	//待发送数据
	string str(szData);
	str = "data=" + str;
	const char* postData = str.c_str();
	//此处strlen需注意
	request->setRequestData(postData, strlen(postData));	  
	//缓存重发消息
	HTTPREQUESTMESSAGE* pMessage = new HTTPREQUESTMESSAGE();
	pMessage->pHttpRequest = request;
	pMessage->sendTimes ++;
	request->retain();
	m_mapRequest.insert(pair<int, HTTPREQUESTMESSAGE*>(nCmdID, pMessage));
	//设置MsgId
	ostringstream oss;
	oss << nCmdID;
	request->setTag(oss.str().c_str());
	HttpClient::getInstance()->send(request);
	CCLOG("sendHttpMessage: nCmdId=%d, dataLen=%d, data=%s", nCmdID, strlen(szData), szData);
	request->release();
}

void HttpCommunicator::onHttpResponse(HttpClient *sender, HttpResponse *response) 
{ 
	if (!response) return;

	const char* tag = response->getHttpRequest()->getTag();
	CCLOG("%s completed", tag); 

	int statusCode = response->getResponseCode();
	CCLOG("HttpResponse code: %d", statusCode);
	
	//处理重发
	auto iter = m_mapRequest.find(atoi(tag));
	if (iter != m_mapRequest.end())
	{
		HTTPREQUESTMESSAGE* pMessage = iter->second;
		if (!response->isSucceed())
		{ 
			CCLOG("response failed"); 
			CCLOG("error buffer: %s", response->getErrorBuffer()); 			
			if (pMessage->sendTimes >= MAX_HTTP_REQUEST_SEND_TIMES)//重发3次不成功
			{
				m_mapRequest.erase(iter);
				delete pMessage;
				//构造错误消息
				CCLOG("net connect error");
				constructErrorMessage(nNET_CMD_NET_HTTP_ERROR, "net http error");

				LayerWaiting::hideWaiting();
				HttpClient::destroyInstance();
			}
			else//重发消息
			{
				pMessage->sendTimes ++;
				HttpClient::getInstance()->send(pMessage->pHttpRequest);
				CCLOG("sendHttpMessage: nCmdId=%d, sendTimes=%d", iter->first, pMessage->sendTimes);
				//pMessage->pHttpRequest->release();
			}
			return; 
		} 
		m_mapRequest.erase(iter);
		delete pMessage;		
	}

	//派发消息
	std::vector<char> *buffer = response->getResponseData();
	string str = "";
	for (size_t i = 0; i < buffer->size(); i++)
	{
		str += buffer->at(i);
	}

	LayerWaiting::hideWaiting();
	
	recvMessage(atoi(tag), str.c_str());

	//HttpClient::destroyInstance();

	
} 

void HttpCommunicator::sendTTCheckUName( int nCmdID, const char* szData )
{
#if (LOGIN_WITH_TT == 1)
	sendMessage(nCmdID, szData, SystemUtils::getCHECK_USER_NAME_URL().c_str());
#endif
}



