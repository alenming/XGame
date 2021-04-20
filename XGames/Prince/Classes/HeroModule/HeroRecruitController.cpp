#include "HeroRecruitController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Json/json.h"
#include "HeroModel.h"
#include "Common/Common.h"
#include "../MainModule/MainModel.h"
#include "ToolModule/ToolModel.h"


HeroRecruitController::HeroRecruitController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

HeroRecruitController::~HeroRecruitController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}


void HeroRecruitController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_TOOL_SYNTHESIS: 
		parseRecruitMsg(szData);
		break;
	default:
		break;
	}
}

//碎片合成道具
void HeroRecruitController::sendRecruitMsg(const int id)
{
	Json::FastWriter  writer;
	Json::Value toolId;
	toolId["itemId"] = id;
	std::string  json_file=writer.write(toolId);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOOL_SYNTHESIS, json_file.c_str());
}

//合成道具
void HeroRecruitController::parseRecruitMsg(const char* szData)
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

		//主数据更新
		MainModel::getInstance()->updateData(data);

		//英雄数据更新
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//道具数据更新
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items);

		//甄选出新增的英雄
		int heroId = 0;
		for(int i=0; i<petLists.size(); i++)
		{
			if(petLists[i]["stat"].asString() == HERO_ADD_FLAG)
			{
				heroId = petLists[i]["templateId"].asInt();
				break;
			}
		}

		/*****
		不管有没有成功合成出新英雄，都通知UI去刷新卡牌
		注: 如果没有正确获取到新增英雄的标识，则不会创建获得新英雄卡牌的动画，
			但英雄卡牌本身会通过查询heroHodel数据而正确显示
		理论上，后台数据发对了是不会造成合成失败的现象，暂时没有查到具体出现bug的原因
		留了个Log
		*****/

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_TOOL_SYNTHESIS;
		obParam.updateData = &heroId;
		HeroModel::getInstance()->notifyObservers(&obParam);

		string strLog = string(szData);
		CCLOG("%s", strLog.c_str());
		CCLOG("++++++++++HeroRecruitController.cpp: nHeroId == %d ++++++++", heroId);

		/*
		//通知更新
		if(heroId != 0)
		{
			ObserverParam obParam;
			obParam.id = nMAIN_CMD_TOOL_SYNTHESIS;
			obParam.updateData = &heroId;
			HeroModel::getInstance()->notifyObservers(&obParam);
		}
		*/
	}
}