/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		JoinGroupModel
 *  Description:	帮派加入前模块的Model实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef JoinGroupModel_h__
#define JoinGroupModel_h__

#include "JoinGroupDef.h"

class JoinGroupModel : public Observable
{
public:
	static JoinGroupModel* getInstance();

	static void destroyInstance();

	//通知监听者
	void notify(int eventTypeCode, void* data = nullptr);

	void parseGetGroupInfoData(const Json::Value &data);//解析帮派信息
	void parseCreateGroupData(const Json::Value &data);//解析申请创建帮派
	void parseGroupFindData(const Json::Value &data);//解析帮派查找
	void parseApplyJoinGroupData(const Json::Value &data);//解析申请加入
	void parseCancleApplyGroupData(const Json::Value &data);//解析取消申请


	int  getCreateSpend() {return m_createSpend;};
	int  getTotalNum() {return m_totalNum;};
	VEC_GROUP_INFO& getGroupInfo() {return m_groupInfo;};
	vector<int>&  getApplyId() {return m_applyId;};

private:
	JoinGroupModel();
	virtual ~JoinGroupModel();

private:

	static JoinGroupModel* ms_pInstance;

	int   m_createSpend; //创建帮派花费
	int   m_totalNum;  //个数
	VEC_GROUP_INFO m_groupInfo;
	vector<int>  m_applyId;
};

#endif