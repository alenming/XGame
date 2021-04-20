#include "HttpPublisher.h"
#include "EnvConfig.h"
#include "Utils/MD5/MD5Checksum.h"
#include "LoginModule/LoginModel.h"
#include <sstream>
#include "MainModule/MainModel.h"
#include "Utils/UrlEncode.h"
#include "Utils/MD5/MD5Checksum.h"
#include "Utils/SystemUtils.h"

//����������ݣ�����Ķ�
const int gid = 37;
const string key = "bzsh_uTk42^sad";
const int uadid = 0;
const int usite = 0;


HttpPublisher::HttpPublisher()
{

}

HttpPublisher::~HttpPublisher()
{

}

HttpPublisher* HttpPublisher::getInstance()
{
	static HttpPublisher httpPublisher;
	return &httpPublisher;
}

void HttpPublisher::sendOpenGameMsg()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return;
#endif

	string url = "";
	if (PUBLISHER_ENV == 1)
	{
		url = "http://un.huolug.com/news/get.game.gameopen.php";
	}
	else
	{
		url = "http://un.huolug.com/test/get.game.gameopen.php";
	}

	time_t utime = time(nullptr);
	string umacid = getUmacid();
	string uip = SystemUtils::getIPAddresses();
	int uaid = SystemUtils::getUaid();
	int uwid = SystemUtils::getUwid();

	ostringstream ossSign;
	ossSign << utime << gid << uaid << uwid << uadid << usite << umacid << uip << key;
	string sign = MD5Checksum::GetMD5OfString(ossSign.str());

	//�����ַ
	ostringstream oss;
	oss << url << "?";
	oss << "utime=" << utime << "&";
	oss << "gid=" << gid << "&";
	oss << "uaid=" << uaid << "&";
	oss << "uwid=" << uwid << "&";
	oss << "uadid=" << uadid << "&";
	oss << "usite=" << usite << "&";
	oss << "umacid=" << umacid << "&";
	oss << "uip=" << uip << "&";
	oss << "sign=" << sign;

	CCLOG("sendOpenGameMsg:%s", oss.str().c_str());

	HttpRequest* request = new HttpRequest();
	request->setUrl(oss.str().c_str());
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(HttpPublisher::onOpenGameRsp, this));
	HttpClient::getInstance()->send(request);
	request->release();
}

void HttpPublisher::sendLoginGameMsg()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return;
#endif

	string url = "";
	if (PUBLISHER_ENV == 1)
	{
		url = "http://un.huolug.com/news/get.game.gamelogin.php";
	}
	else
	{
		url = "http://un.huolug.com/test/get.game.gamelogin.php";
	}

	time_t utime = time(nullptr);
	string umacid = getUmacid();
	string uip = SystemUtils::getIPAddresses();
	string uid = LoginModel::getInstance()->getU8UserId() + "_bzsh";
	string uname = LoginModel::getInstance()->getSdkUserName();
	string sid = LoginModel::getInstance()->getServerParam()->m_strServerID;
	int uaid = SystemUtils::getUaid();
	int uwid = SystemUtils::getUwid();

	ostringstream ossSign;
	ossSign << uid << uname << utime << gid << sid << uaid << uwid << uadid << usite << umacid << uip << key;
	string sign = MD5Checksum::GetMD5OfString(ossSign.str());

	//�����ַ
	ostringstream oss;
	oss << url << "?";
	oss << "uid=" << uid << "&";
	oss << "uname=" << uname << "&";
	oss << "utime=" << utime << "&";
	oss << "gid=" << gid << "&";
	oss << "sid=" << sid << "&";
	oss << "uaid=" << uaid << "&";
	oss << "uwid=" << uwid << "&";
	oss << "uadid=" << uadid << "&";
	oss << "usite=" << usite << "&";
	oss << "umacid=" << umacid << "&";
	oss << "uip=" << uip << "&";
	oss << "sign=" << sign;

	CCLOG("sendLoginGameMsg:%s", oss.str().c_str());

	HttpRequest* request = new HttpRequest();
	request->setUrl(oss.str().c_str());
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(HttpPublisher::onLoginGameRsp, this));
	HttpClient::getInstance()->send(request);
	request->release();
}

void HttpPublisher::sendCreateRoleMsg()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return;
#endif

	string url = "";
	if (PUBLISHER_ENV == 1)
	{
		url = "http://un.huolug.com/news/get.game.gamereg.php";
	}
	else
	{
		url = "http://un.huolug.com/test/get.game.gamereg.php";
	}

	time_t utime = time(nullptr);
	string umacid = getUmacid();
	string uip = SystemUtils::getIPAddresses();
	string uid = LoginModel::getInstance()->getU8UserId() + "_bzsh";	
	string uname = LoginModel::getInstance()->getSdkUserName();
	int uguid = 1;
	string ugname = LoginModel::getInstance()->getRoleName();
	string sid = LoginModel::getInstance()->getServerParam()->m_strServerID;
	int uaid = SystemUtils::getUaid();
	int uwid = SystemUtils::getUwid();

	ostringstream ossSign;
	ossSign << uid << uname << utime << gid << sid << uaid << uwid << uadid << usite << umacid << uguid << ugname << uip << key;
	string sign = MD5Checksum::GetMD5OfString(ossSign.str());

	//�����ַ
	ostringstream oss;
	oss << url << "?";
	oss << "uid=" << uid << "&";
	oss << "uname=" << uname << "&";
	oss << "uguid=" << uguid << "&";
	oss << "ugname=" << ugname << "&";
	oss << "utime=" << utime << "&";
	oss << "gid=" << gid << "&";
	oss << "sid=" << sid << "&";
	oss << "uaid=" << uaid << "&";
	oss << "uwid=" << uwid << "&";
	oss << "uadid=" << uadid << "&";
	oss << "usite=" << usite << "&";
	oss << "umacid=" << umacid << "&";
	oss << "uip=" << uip << "&";
	oss << "sign=" << sign;

	CCLOG("sendCreateRoleMsg:%s", oss.str().c_str());

	HttpRequest* request = new HttpRequest();
	request->setUrl(oss.str().c_str());
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(HttpPublisher::onCreateRoleMsg, this));
	HttpClient::getInstance()->send(request);
	request->release();
}

void HttpPublisher::onOpenGameRsp(HttpClient *sender, HttpResponse *response)
{
	if (!response) return;

	std::vector<char>* buffer = response->getResponseData();
	string str = "";
	for (size_t i = 0; i < buffer->size(); i++)
	{
		str += buffer->at(i);
	}

	CCLOG("onOpenGameRsp:%s", str.c_str());
}

void HttpPublisher::onLoginGameRsp(HttpClient *sender, HttpResponse *response)
{
	if (!response) return;

	std::vector<char>* buffer = response->getResponseData();
	string str = "";
	for (size_t i = 0; i < buffer->size(); i++)
	{
		str += buffer->at(i);
	}

	CCLOG("onLoginGameRsp:%s", str.c_str());
}

void HttpPublisher::onCreateRoleMsg(HttpClient *sender, HttpResponse *response)
{
	if (!response) return;

	std::vector<char>* buffer = response->getResponseData();
	string str = "";
	for (size_t i = 0; i < buffer->size(); i++)
	{
		str += buffer->at(i);
	}

	CCLOG("onCreateRoleMsg:%s", str.c_str());
}

string HttpPublisher::getUmacid()
{
	string ret = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	ret = SystemUtils::getMAC();
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ret = SystemUtils::getIDFA();
#else
	ret = "00:00:00:00:00:00";
#endif
	return ret;
}

std::string HttpPublisher::getCustomerServiceSystemUrl()
{
	int cid = SystemUtils::getUaid();//����id Ĭ��ֵΪ1 ����Ӫͬ���ṩ����ƽ̨��uaid����Ϊcid��ֵ

	string key =  "bzsh_fdsfdkljrle49320jjlkwqre";
	string androidUrl = "http://bzsh.kf.3737.com/bzsh_android_client.index.php";
	string iosUrl = "http://bzsh.kf.3737.com/bzsh_ios_client.index.php";

	string uname = MainModel::getInstance()->getMainParam()->mName; //����ǳ�
	string sid = LoginModel::getInstance()->getServerParam()->m_strServerID; //��Ϸ��id
	time_t loginTime = time(nullptr);//��¼ʱ�䣬unixʱ���
	int uid = MainModel::getInstance()->getMainParam()->mRoleId; //�����Ϸid
	string openid = LoginModel::getInstance()->getSdkUserId(); //���openid  ����Ѷ�ϣ���Ӧ��Ѷ�ϵ�openid �ڵ���ƽ̨,��Ӧ����ƽ̨���û�id
	string sign; //�� cidΪ 1 ��֤�룺MD5���ܴ�(uname+openid+uid+sid+time+key) �� cid��Ϊ1 ��֤�룺MD5���ܴ�(uname+openid+uid+sid+time+cid+key)
	//keyΪ��˫��Э�����ɣ�+Ϊ���ӷ���������md5����
	//�� unameΪa openedΪb uidΪc sidΪ1 timeΪ2 cidΪ1
	//�� sign=md5(abc12key)
	if (cid == 1)
	{
		ostringstream ossSign;
		ossSign << uname << openid << uid << sid << loginTime << key;
		sign = MD5Checksum::GetMD5OfString(ossSign.str());
	}
	else
	{
		ostringstream ossSign;
		ossSign << uname << openid << uid << sid << loginTime << cid << key;
		sign = MD5Checksum::GetMD5OfString(ossSign.str());
	}
	int ac = 1;
	int pay = MainModel::getInstance()->getMainParam()->mTotalPay;

	ostringstream urlStr;
	urlStr << "?uname=" << UrlEncode::Encode(uname);
	urlStr << "&sign=" << sign;
	urlStr << "&sid=" << sid;
	urlStr << "&time=" << loginTime;
	urlStr << "&uid=" << uid;
	urlStr << "&openid=" << openid;
	urlStr << "&ac=" << ac;
	urlStr << "&cid=" << cid;
	urlStr << "&pay=" <<pay;

	string completeUrl;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	completeUrl = androidUrl+urlStr.str();
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	completeUrl = iosUrl+urlStr.str();
#endif

	CCLOG("CustomerServiceSystemUrl = %s", completeUrl.c_str());
	return completeUrl;
}
