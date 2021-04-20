/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ArtifactController.h
 *  Description:	神器模块的Controller实现;
 *  Version:		1.0
 *  Author:			Jerry
 *  Date:			02/20/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef ArtifactController_h__
#define ArtifactController_h__

#include "Communication/ICommListener.h"

class ArtifactController : public ICommListener
{
public:
	ArtifactController();
	~ArtifactController();

	//发送获取神器主界面协议
	static void sendGetArtifactMainMsg();

	//发送神器激活协议
	static void sendActiveArtifactMsg(int ArtifactID);

	//发送洗练
	static void sendRefineArtifactMsg(int ArtifactID, int times, int refType);

	//发送保存洗练
	static void sendSaveRefineMsg(int ArtifactID);

private:
	
	//解析神器主界面
	void parseArtifactMainMsg(const char* szData);

	//解析激活协议
	void parseActiveMsg(const char* szData);

	//解析洗练协议
	void parseRefineMsg(const char* szData);

	//解析保存协议
	void parseSaveRefineMsg(const char* szData);

private:
	virtual void onRecvMessage(int nCmdID, const char* szData) override;
};

#endif