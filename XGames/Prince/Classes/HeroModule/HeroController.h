#ifndef HeroController_h__
#define HeroController_h__

#include "HeroModel.h"

#include "../Communication/ICommListener.h"
#include "../Communication/Json/json.h"

class HeroController: public ICommListener
{
public:

	HeroController();
	virtual ~HeroController();

public:

	//发送英雄进阶消息
	static void sendHeroAdvanceMsg(int heroId);
	//发送英雄升星消息
	static void sendHeroStarUpMsg(int heroId);
	//发送装备穿戴消息
	static void sendEquipPutOnMsg(int heroId, int toolId);
	//发送英雄使用经验道具协议
	static void sendUseExpItemMsg(int heroId, int templetId, int num = 1);
	//发送兑换碎片协议
	static void sendExchangeChipsMsg(int heroId, int num);
	

	//接收英雄进阶消息
	void parseHeroAdvanceMsg(const char* szData);
	//接收英雄升星消息
	void parseHeroStarUpMsg(const char* szData);
	//接收装备穿上消息
	void parseEquipPutOnMsg(const char* szData);
	//接受英雄使用经验道具协议
	void parseUseExpItemMsg(const char* szData);
	//接收碎片兑换协议
	void parseExchangeChipsMsg(const char* szData);

	//接收消息
	virtual void onRecvMessage(int nCommandID, const char* szData);

};



#endif // HeroController_h__
