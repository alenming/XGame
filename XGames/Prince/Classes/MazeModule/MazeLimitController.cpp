#include "MazeLimitController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/HttpCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Json/json.h"
#include "../HeroModule/HeroModel.h"
#include "../ToolModule/ToolModel.h"
#include "Common/Common.h"
#include "MazeDef.h"
#include "MazeModel.h"
#include "../MainModule/MainModel.h"
#include "Temp/CustomTips.h"
#include "cocos2d.h"
#include "SDK_TalkingData/TalkingData.h"

using namespace cocos2d;


MazeLimitController::MazeLimitController()
{
	m_pView = nullptr;
	SocketCommunicator::getInstance()->addListener(this);
}

MazeLimitController::~MazeLimitController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}


//设置UI接口
void MazeLimitController::onSetView(IMazeLimitView* pView)
{
	m_pView = pView;
}

void MazeLimitController::onRecvMessage(int nCmdID, const char* szData)
{
	if (nullptr == m_pView)
	{
		return;
	}
	if ( NULL==szData || 0==strlen(szData))
	{
		return;
	}
	
	switch (nCmdID)
	{	
	case nMAIN_CMD_MAZE_LIMIT_BUY:
		{
			processLimitBuyData(szData);
		}
		break;
	
	default:
		break;
	}
	return;
}
void MazeLimitController::processLimitBuyData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData,root))
	{
		if (throwException(root))
		{
			return;
		}
	}

	int nResult			= root["result"].asInt();
	string strMsg		= root["msg"].asString();
	Json::Value data	= root["data"];

	//英雄数据
	//
	vector<TOOL_NEW_INFO> vecNewTools;
	//道具数据
	Json::Value items = data["itemList"];
	Json::Value petLists = data["petList"];

	//整卡
	if (petLists.size() > 0)
	{
		for (int i = 0; i < data["petList"].size(); i++)
		{
			TOOL_NEW_INFO item;
			item.nid =  data["petList"][i]["id"].asInt();
			item.ntemplateId =  data["petList"][i]["resID"].asInt();
			item.nstack =  1;

			vecNewTools.push_back(item);
		}

		ToolModel::getInstance()->updateToolJsonData(items);
	}
	else
	{
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);
	}

	HeroModel::getInstance()->updateData(petLists);

	MazeInfoParam &pMainParam = MazeModel::getInstance()->getMazeInfos();
	setTalkingDataPurchase(vecNewTools);
	pMainParam.mMazeBuyTime	= 0;//限时抽卡抽到的英雄Id
	pMainParam.mMazedrawPetId	= 0;//秒
	pMainParam.mMazeBuyPrice	= 0;//金币价格
	pMainParam.mMazeBuyCount	= 0;//

	MainModel::getInstance()->updateData(data);
	m_pView->onLimitBuyResult(nResult, vecNewTools);
}

void MazeLimitController::setTalkingDataPurchase(const vector<TOOL_NEW_INFO>& vecNewTools)
{
	//获得道具
	MazeInfoParam &pMainParam = MazeModel::getInstance()->getMazeInfos();
	for (auto iter = vecNewTools.begin(); iter < vecNewTools.end(); iter++)
	{
		RowData* pToolData = DataManager::getInstance()->searchToolById(iter->ntemplateId);
		if (pToolData != nullptr)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20272)
				+ pToolData->getStringData("itemName"),1,pMainParam.mMazeBuyPrice);
			break;
		}							
	}		
}

// 获取迷宫信息
void MazeLimitController::onLimitBuy()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAZE_LIMIT_BUY, "");
}

void MazeLimitController::onRemoveView()
{
	m_pView = nullptr;
}