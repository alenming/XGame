#ifndef HeroFateController_h__
#define HeroFateController_h__

#include "HeroModel.h"

#include "../Communication/ICommListener.h"
#include "../Communication/Json/json.h"

class HeroFateController: public ICommListener
{

public:

	HeroFateController();
	virtual ~HeroFateController();

public:

	//接收消息
	virtual void onRecvMessage(int nCommandID, const char* szData);

	//发送消息
	static void sendGetFateMsg();
	//绑定缘分
	static void sendBindFateMsg(int heroId, int pos);
	//解除缘分
	static void sendCancelFateMsg(int heroId);
	//刷新缘分
	static void sendrRefreshFateMsg(int heroId);
	//替换缘分
	static void sendReplaceFateMsg(int heroId);


private:

	//接收解析消息
	void parseGetFateMsg(const char* szData);
	void parseBindFateMsg(const char* szData);	
	void parseCancelFateMsg(const char* szData);	
	void parseRefreshFateMsg(const char* szData);	
	void parseReplaceFateMsg(const char* szData);
};



#endif // HeroFateController_h_
