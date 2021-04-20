#ifndef ToolController_h__
#define ToolController_h__

#include "Communication/ICommListener.h"

class ToolController: public ICommListener
{
public:

	ToolController();
	~ToolController();

public:

	//��Ϣ���սӿ�
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	//ʹ�õ���
	static void sendUseToolsMsg(const int id, int num);

	//���۵���
	static void sendSellToolsMsg(const int& id, const int& amount);

	//������
	static void sendOpenChestMsg(const int id);

	
private:

	//ʹ�õ���
	void parseUseToolsMsg(const char* szData);

	//���۵���
	void parseSellToolsMsg(const char* szData);

	//�򿪱���
	void parseOpenChestMsg(const char* szData);

};

#endif // LoginController_h__
