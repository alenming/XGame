/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *  FileName:		LevelUpController
 *  Description:	角色升级控制;
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

	// 实现接收服务器消息接口;
	virtual void  onRecvMessage(int nCmdID, const char* szData);

	// 1.确认升级数据(由MainModel或HeroModel触发);
	void  confirmLevelUp(int nNewLv);

	// 2.各模块检查升级数据接口;
	bool  checkLevelUp(LevelUpCallback callback = nullptr);
	//通知主城升级了
	void noticeMainInfoLayer();

private:
	LevelUpController(void);

private:
	// 
	static LevelUpController* m_pInstance;

	// 升级数据结构;
	UI_LEVEL_UP_PARAM  m_param;
};

