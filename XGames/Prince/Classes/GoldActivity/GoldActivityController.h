#ifndef GoldActivityController_h__
#define GoldActivityController_h__

#include "Communication/ICommListener.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "GoldActivityModel.h"

class GoldActivityController :public ICommListener 
{
public:
	~GoldActivityController();
	//活动单例
	static GoldActivityController* getInstance();
	static void destroyInstance();

	virtual void onRecvMessage(int nCmdID, const char* szData);

	void getTableInfo(); //獲取表格信息
	void changeGold(); //兌換金幣
	void otherChangeGoldInfo(); //他人兌換金幣記錄


	void processTableInfo(const char* szData); //解析獲取表格信息
	void processchangeGold(const char* szData); //解析兌換金幣
	void processotherChangeGoldInfo(const char* szData); //解析他人兌換金幣記錄


private:

	GoldActivityController();

	static GoldActivityController* mInstance;
};

#endif
