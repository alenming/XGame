#include "HeroFateController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "FateModel.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainModel.h"
#include "Temp/CustomTips.h"


HeroFateController::HeroFateController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

HeroFateController::~HeroFateController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

//发送消息
void HeroFateController::sendGetFateMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_HERO_FATE_GET_INFO, "");
}

void HeroFateController::sendBindFateMsg(int heroId, int pos)
{
	Json::FastWriter writer;
	Json::Value val;
	val["petId"] = heroId;
	val["pos"] = pos;
	std::string json_file = writer.write(val);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_HERO_FATE_TIED_FATES, json_file.c_str());
}

void HeroFateController::sendCancelFateMsg(int heroId)
{
	Json::FastWriter writer;
	Json::Value val;
	val["petId"] = heroId;
	std::string json_file = writer.write(val);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_HERO_FATE_CANCEL_FATES, json_file.c_str());
}

void HeroFateController::sendrRefreshFateMsg(int heroId)
{
	Json::FastWriter writer;
	Json::Value val;
	val["petId"] = heroId;
	std::string json_file = writer.write(val);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_HERO_FATE_REFRESH_FATES, json_file.c_str());
}

void HeroFateController::sendReplaceFateMsg(int heroId)
{
	Json::FastWriter writer;
	Json::Value val;
	val["petId"] = heroId;
	std::string json_file = writer.write(val);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_HERO_FATE_REPLACE_FATES, json_file.c_str());
}

//接收解析消息
//
void HeroFateController::parseGetFateMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		FateModel::getInstance()->setInited();
		//
		Json::Value fateInfos = data["fateInfos"];
		for (int i = 0; i < fateInfos.size(); i++)
		{
			Json::Value fate = fateInfos[i];
			FateItemInfo item;
			item.heroId = fate["petId"].asInt();
			item.proType = fate["propTyp"].asString();
			item.fProVal = fate["propVal"].asDouble();
			item.fProQua = fate["qua"].asInt();
			item.pos = fate["pos"].asInt();
			item.fNewProVal = 0;		//初始化
			item.isTied = true;
			FateModel::getInstance()->addFateItem(item);
		}

		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_HERO_FATE_GET_INFO;
		HeroModel::getInstance()->notifyObservers(&observerParam);
	}
}

void HeroFateController::parseBindFateMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		//
		int petId = data["petId"].asInt();
		string proType = data["propTyp"].asString();
		float fProVal = data["propVal"].asDouble();	
		int fProQua = data["qua"].asInt();

		FateModel::getInstance()->updateFateInfo(petId, proType, fProVal, fProQua);

		Json::Value heroInfo = data["petList"];
		HeroModel::getInstance()->updateData(heroInfo);

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_HERO_FATE_TIED_FATES;
		observerParam.updateData = &petId;
		HeroModel::getInstance()->notifyObservers(&observerParam);

		//通知到标题栏通用控件
		FateModel::getInstance()->notifyObservers();
	}
}

void HeroFateController::parseCancelFateMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		int petId = data["petId"].asInt();	
		FateItemInfo* pItemInfo = FateModel::getInstance()->getFateInfo(petId);
		if (pItemInfo != nullptr)
		{
			pItemInfo->isTied = false;
		}

		Json::Value heroInfo = data["petList"];
		HeroModel::getInstance()->updateData(heroInfo);

		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_HERO_FATE_CANCEL_FATES;
		observerParam.updateData = &petId;
		HeroModel::getInstance()->notifyObservers(&observerParam);
	}
}

void HeroFateController::parseRefreshFateMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		int petId = data["petId"].asInt();	
		//更新数据
		FateItemInfo* pInfo = FateModel::getInstance()->getFateInfo(petId);
		if (pInfo != nullptr)
		{
			pInfo->fProVal =  data["oldVal"].asDouble();//刷新前的值
			pInfo->fNewProVal =  data["newVal"].asDouble();//刷新后的值
			pInfo->fNewQua = data["qua"].asInt();		//刷新后的品质
		}

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_HERO_FATE_REFRESH_FATES;
		observerParam.updateData = &petId;
		HeroModel::getInstance()->notifyObservers(&observerParam);

		//通知到标题栏通用控件
		FateModel::getInstance()->notifyObservers();
	}
}

void HeroFateController::parseReplaceFateMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];		
		int petId = data["petId"].asInt();	
		FateItemInfo* pItemInfo = FateModel::getInstance()->getFateInfo(petId);
		if (pItemInfo != nullptr)
		{
			pItemInfo->fProVal = pItemInfo->fNewProVal;
			pItemInfo->fProQua = pItemInfo->fNewQua;
		}

		Json::Value heroInfo = data["petList"];
		HeroModel::getInstance()->updateData(heroInfo);

		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_HERO_FATE_REPLACE_FATES;
		observerParam.updateData = &petId;
		HeroModel::getInstance()->notifyObservers(&observerParam);
	}
}

void HeroFateController::onRecvMessage( int nCmdID, const char* szData )
{	
	switch (nCmdID)
	{
	case nMAIN_CMD_HERO_FATE_GET_INFO://获取主角缘分主界面信息
		parseGetFateMsg(szData);
		break;		
	case nMAIN_CMD_HERO_FATE_TIED_FATES://结识主角缘分
		parseBindFateMsg(szData);
		break;
	case nMAIN_CMD_HERO_FATE_CANCEL_FATES://取消主角缘分
		parseCancelFateMsg(szData);
		break;
	case nMAIN_CMD_HERO_FATE_REFRESH_FATES://刷新缘分
		parseRefreshFateMsg(szData);
		break;
	case nMAIN_CMD_HERO_FATE_REPLACE_FATES://替换缘分
		parseReplaceFateMsg(szData);
		break;
	default:
		break;
	}
	return;
}
