/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *  FileName:		LevelUpController
 *  Description:	��ɫ��������;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/28/2015
 *  Others: 
 ******************************************************************/

#include "Communication/SocketCommunicator.h"
#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"
#include "Communication/Command.h"
#include "LayerLevelUp.h"

class LevelUpController : public ICommListener
{
public:
	~LevelUpController(void);

	static LevelUpController*  getInstance();
	static void destroyInstance();

	// ʵ�ֽ��շ�������Ϣ�ӿ�;
	virtual void  onRecvMessage(int nCmdID, const char* szData);

	// 1.ȷ����������(��MainModel��HeroModel����);
	void  confirmLevelUp(int nNewLv);

	// 2.��ģ�����������ݽӿ�;
	bool  checkLevelUp(LevelUpCallback callback = nullptr);
	//֪ͨ����������
	void noticeMainInfoLayer();

private:
	LevelUpController(void);

private:
	// 
	static LevelUpController* m_pInstance;

	// �������ݽṹ;
	UI_LEVEL_UP_PARAM  m_param;
};

