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

	//����������ȡЭ�飺nMAIN_CMD_TASK_RECEIVE
	static void sendTaskReceiveMsg(int taskId);

	//���������б��ȡЭ��
	static void sendGetTaskListMsg();

private:

	//����������ȡЭ��
	void parseTaskReceiveMsg(const char* szData);

	//���������б�Э��
	void parseGetTaskListMsg(const char* szData);

	//������̨������Ϣ
	void parseTaskPushMsg(const char* szData);

private:

	static MainTaskController* mInstance;

};




#endif //MainTaskController_h__