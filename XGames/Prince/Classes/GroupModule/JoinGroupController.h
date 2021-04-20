/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		JoinGroupController
 *  Description:	帮派加入前模块的Controller实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef JoinGroupController_h__
#define JoinGroupController_h__

#include "JoinGroupDef.h"

class JoinGroupController : public ICommListener
{
public:
	JoinGroupController();
	~JoinGroupController();

	virtual void onRecvMessage(int nCmdID, const char* szData) override;

	void getGroupInfo();
	void createGroup(string name,int resId); //申请创建帮派
	void groupFind(SEARCH_TYPE type, int id = 0, int pageNum = 1);//帮派查找
	void applyJoinGroup(int groupId); //申请加入
	void cancleApplyGroup(int groupId);//取消申请

	void procesGetGroupInfoData(const char* szData); //解析帮派信息
	void processCreateGroupData(const char* szData); //解析申请创建帮派
	void processGroupFindData(const char* szData); //解析帮派查找
	void processApplyJoinGroupData(const char* szData); //解析申请加入
	void processCancleApplyGroupData(const char* szData); //解析取消申请
};

#endif