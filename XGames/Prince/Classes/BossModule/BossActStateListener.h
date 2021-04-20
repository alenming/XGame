#ifndef BossActStateListener_h__
#define BossActStateListener_h__

#include "Communication/ICommListener.h"

class BossActStateListener : public ICommListener
{
public:
	
	virtual ~BossActStateListener();

	static BossActStateListener* getInstance();
	static void destroyInstance();

private:

	BossActStateListener();

	//解析boss活动开关状态推送
	void parseBossActStatePushMsg(const char* szData);

	virtual void onRecvMessage(int nCmdID, const char* szData);

private:
	static BossActStateListener* ms_pInstance;
};

#endif