#include "JoinGroupModel.h"

JoinGroupModel* JoinGroupModel::ms_pInstance = nullptr;

JoinGroupModel::JoinGroupModel()
{
	m_createSpend = 0;
	m_totalNum = 0;
	m_groupInfo.clear();
	m_applyId.clear();
}

JoinGroupModel::~JoinGroupModel()
{

}

JoinGroupModel* JoinGroupModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new JoinGroupModel();
	}
	return ms_pInstance;
}

void JoinGroupModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void JoinGroupModel::notify(int eventTypeCode, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventTypeCode;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void JoinGroupModel::parseGetGroupInfoData(const Json::Value &data)
{
	Json::Value applyList = data["applyList"];
	m_totalNum = applyList["totalNum"].asInt();
	m_createSpend = applyList["costGold"].asInt();
	m_groupInfo.clear();
	for (int i = 0; i < applyList["bangpaiList"].size(); i++)
	{
		GROUP_INFO info;

		info.orderNum = applyList["bangpaiList"][i]["orderNum"].asInt(); //帮派排名
		info.resId = applyList["bangpaiList"][i]["resId"].asInt();//头像res
		info.name = applyList["bangpaiList"][i]["name"].asString(); //帮派名字
		info.level = applyList["bangpaiList"][i]["level"].asInt(); //帮派等级
		info.curNum = applyList["bangpaiList"][i]["curNum"].asInt(); //当前人数
		info.maxNum = applyList["bangpaiList"][i]["maxNum"].asInt(); //最高上限
		info.id = applyList["bangpaiList"][i]["id"].asInt();//帮派id
		info.needLvl = applyList["bangpaiList"][i]["needLvl"].asInt(); //加入等级
		info.checkFlag = applyList["bangpaiList"][i]["checkFlag"].asBool(); //需要审核
		info.remark2 = applyList["bangpaiList"][i]["remark2"].asString(); //对外宣传
		info.leader = applyList["bangpaiList"][i]["leader"].asString(); //公会会长名

		m_groupInfo.push_back(info);
	}

	m_applyId.clear();
	for (int i = 0; i < applyList["applyList"].size(); i++)
	{
		m_applyId.push_back(applyList["applyList"][i].asInt());
	}

	notify(nMAIN_CMD_GROUP_MAIN_INFO);
}

void JoinGroupModel::parseCreateGroupData(const Json::Value &data)
{
	int getGold = data["getGold"].asInt();
	MainModel::getInstance()->getMainParam()->mGroupId = data["bangpaiId"].asInt();
	MainModel::getInstance()->getMainParam()->mGroupName = data["bangpaiName"].asString(); 
	MainModel::getInstance()->getMainParam()->mGroupMoney = data["credit"].asInt();

	MainModel::getInstance()->setGold(getGold);
	MainModel::getInstance()->notifyObservers();

	notify(nMAIN_CMD_GROUP_CREATE);


}

void JoinGroupModel::parseGroupFindData(const Json::Value &data)
{
	Json::Value bangpaiInfo = data["bangpaiInfo"];
	m_totalNum = data["totalNum"].asInt();
	m_groupInfo.clear();
	for (int i = 0; i < bangpaiInfo.size(); i++)
	{
		GROUP_INFO info;

		info.orderNum = bangpaiInfo[i]["orderNum"].asInt(); //帮派排名
		info.resId = bangpaiInfo[i]["resId"].asInt();//头像res
		info.name = bangpaiInfo[i]["name"].asString(); //帮派名字
		info.level = bangpaiInfo[i]["level"].asInt(); //帮派等级
		info.curNum = bangpaiInfo[i]["curNum"].asInt(); //当前人数
		info.maxNum = bangpaiInfo[i]["maxNum"].asInt(); //最高上限
		info.id = bangpaiInfo[i]["id"].asInt();//帮派id
		info.needLvl = bangpaiInfo[i]["needLvl"].asInt(); //加入等级
		info.checkFlag = bangpaiInfo[i]["checkFlag"].asBool(); //需要审核
		info.remark2 = bangpaiInfo[i]["remark2"].asString(); //对外宣传
		info.leader = bangpaiInfo[i]["leader"].asString(); //公会会长名

		m_groupInfo.push_back(info);
	}

	notify(nMAIN_CMD_GROUP_SEARCH);
}

void JoinGroupModel::parseApplyJoinGroupData(const Json::Value &data)
{
	bool flag = data["flag"].asBool();

	m_applyId.clear();
	for (int i = 0; i < data["applyList"].size(); i++)
	{
		m_applyId.push_back(data["applyList"][i].asInt());
	}

	notify(nMAIN_CMD_GROUP_APPLY_JOIN,(void*)&flag);
}

void JoinGroupModel::parseCancleApplyGroupData(const Json::Value &data)
{
	notify(nMAIN_CMD_GROUP_CANCE_APLLY);
}