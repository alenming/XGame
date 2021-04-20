#ifndef MainTaskController_h__
#define MainTaskController_h__

#include "Communication/ICommListener.h"

class MainTaskController : public ICommListener
{

public:

	MainTaskController();
	virtual ~MainTaskController();

	static MainTaskController* getInstance();
	static void destroyInstance();

public:

	virtual void onRecvMessage(int nCmdID, const char* szData);

	//发送任务领取协议：nMAIN_CMD_TASK_RECEIVE
	static void sendTaskReceiveMsg(int taskId);

	//发送任务列表获取协议
	static void sendGetTaskListMsg();

private:

	//解析任务领取协议
	void parseTaskReceiveMsg(const char* szData);

	//解析任务列表协议
	void parseGetTaskListMsg(const char* szData);

	//解析后台推送消息
	void parseTaskPushMsg(const char* szData);

private:

	static MainTaskController* mInstance;

};




#endif //MainTaskController_h__