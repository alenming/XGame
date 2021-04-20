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

	//��ȡ��ʵ��
	static HttpPublisher* getInstance();

public:

	//���ʹ���Ϸ��Ϣ������Ϸʱ����
	void sendOpenGameMsg();

	//���͵�¼��Ϸ��Ϣ��ѡ�����������Ϸ����
	void sendLoginGameMsg();

	//���ʹ�����ɫ��Ϣ��������ɫ����
	void sendCreateRoleMsg();

	//��ȡ�ͷ�ϵͳUrl
	string getCustomerServiceSystemUrl();

	//ƻ������ΪIDFA   ��׿ΪMAC
	string getUmacid();

private:

	void onOpenGameRsp(HttpClient *sender, HttpResponse *response);
	void onLoginGameRsp(HttpClient *sender, HttpResponse *response);
	void onCreateRoleMsg(HttpClient *sender, HttpResponse *response);

};




#endif //HttpPublisher_h__
