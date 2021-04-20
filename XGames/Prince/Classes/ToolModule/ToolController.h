#ifndef ToolController_h__
#define ToolController_h__

#include "Communication/ICommListener.h"

class ToolController: public ICommListener
{
public:

	ToolController();
	~ToolController();

public:

	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	//使用道具
	static void sendUseToolsMsg(const int id, int num);

	//出售道具
	static void sendSellToolsMsg(const int& id, const int& amount);

	//开宝箱
	static void sendOpenChestMsg(const int id);

	
private:

	//使用道具
	void parseUseToolsMsg(const char* szData);

	//出售道具
	void parseSellToolsMsg(const char* szData);

	//打开宝箱
	void parseOpenChestMsg(const char* szData);

};

#endif // LoginController_h__
