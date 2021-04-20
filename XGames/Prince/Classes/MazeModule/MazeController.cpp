#include "MazeController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/HttpCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Json/json.h"
#include "../HeroModule/HeroModel.h"
#include "../ToolModule/ToolModel.h"
#include "Common/Common.h"
#include "MazeDef.h"
#include "MazeModel.h"
#include "MainModule/MainModel.h"
#include "Temp/CustomTips.h"


#include "cocos2d.h"
using namespace cocos2d;

MazeController* MazeController::ms_pInstance = nullptr;
MazeController::MazeController()
{
	m_pView = nullptr;
	SocketCommunicator::getInstance()->addListener(this);
	//memset(&m_lastMazeParam, 0, sizeof(MazeParam));
}
MazeController::~MazeController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

MazeController* MazeController::getInstance()
{
	if (nullptr == ms_pInstance)
	{
		ms_pInstance = new MazeController();
	}

	return ms_pInstance;
}

void MazeController::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

//设置UI接口
void MazeController::onSetView(IMazeView* pView)
{
	m_pView = pView;
}


void MazeController::onRecvMessage(int nCmdID, const char* szData)
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
	case nMAIN_CMD_MAZE_GET_ALLINFO:			// 获取迷宫信息
		{
			processGetAllMazesData(szData);
		}
		break;
	case nMAIN_CMD_MAZE_RANDOM_ROAD:			// 迷宫随机路口事件
		{
			processRandomRoadData(szData);
		}
		break;
	case nMAIN_CMD_MAZE_EVENT_THINGS_INFO:     // 迷宫已经随机出来事件获取相应信息
		{
			processEventThingInfo(szData);
		}
		break;
	case nMAIN_CMD_MAZE_OPEN_BOX:				// 迷宫打开宝箱
		{
			processOpenBoxData(szData);
		}
		break;
	case  nMAIN_CMD_MAZA_OPEN_BOX_END:         // 迷宫宝箱关闭
		{
			processOpenBoxEndData(szData);
		}
		break;
	case nMAIN_CMD_MAZE_OPEN_MAP_BOX:
		{
			processOpenMapBoxData(szData);    // 迷宫宝箱牌子
		}
		break;
	default:
		break;
	}
	return;
}
void MazeController::processGetAllMazesData(const char* szData)
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

	//
	processAllMazesData(data);
		
	m_pView->onGetAllMazesResult(nResult);
}

void MazeController::processAllMazesData(Json::Value& data)
{
	MAZE_INFOS &mazeInfos = MazeModel::getInstance()->getMazeInfos();

	mtLoginServerTime = time_t(data["nowdate"].asInt());
	mazeInfos.mazeId = data["id"].asInt();
	mazeInfos.curPower		= data["movePower"].asInt();
	MainModel::getInstance()->getMainParam()->mMazeId = mazeInfos.mazeId;
	MainModel::getInstance()->getMainParam()->mMazePower = mazeInfos.curPower;
	

	mazeInfos.times			= data["needTime"].asInt();
	mazeInfos.mMazeBuyTime	= data["buyTime"].asInt();
	mazeInfos.mMazedrawPetId = data["drawItemId"].asInt();
	mazeInfos.mMazeBuyCount	=  data["drawCount"].asInt();
	mazeInfos.mMazeBuyPrice	=  data["goldPrice"].asInt();
	mazeInfos.mMazeBuyCurTime = secondNow();

	MainModel::getInstance()->setGold(data["gold"].asInt());
	MainModel::getInstance()->notifyObservers();
	
	//地图碎片
	Json::Value mapList	= data["maplist"];
	MAZE_MAP_ITEM mapItem;
	mazeInfos.vecMaps.clear();
	for (int i = 0; i < mapList.size(); i++)
	{
		//'id':10001,'num':10
		mapItem.id				= mapList[i]["id"].asInt();
		mapItem.num				= mapList[i]["count"].asInt();
		mazeInfos.vecMaps.push_back(mapItem);
	}

	//上次选取宝藏id
	mazeInfos.openBoxId = data["openBoxId"].asInt();
	Json::Value boxItemList	= data["boxItemList"];
	CHOOSEED_ITEM itemItem;
	mazeInfos.vecChoosedMap.clear();
	for (int i = 0; i < boxItemList.size(); i++)
	{
		//'id':10001,'num':10
		itemItem.id				= boxItemList[i]["id"].asInt();
		itemItem.count				= boxItemList[i]["count"].asInt();
		itemItem.pos				= boxItemList[i]["pos"].asInt();

		mazeInfos.vecChoosedMap.push_back(itemItem);
	}
}

void MazeController::processMazesDropData(Json::Value& data, MAZE_DROP_INFO& dropInfo)
{
	dropInfo.getCoin			= data["costExp"].asInt();
	dropInfo.getExp				= data["costCoin"].asInt();
	Json::Value dropoutList		= data["dropoutList"];
	for (int i = 0; i < dropoutList.size(); i++)
	{
		TOOL_NEW_INFO newInfo;
		newInfo.nid = dropoutList[i]["id"].asInt();
		newInfo.nstack = dropoutList[i]["num"].asInt();
		dropInfo.vecNewTools.push_back(newInfo);
	}
}

void MazeController::processRandomRoadData(const char* szData)
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

	MazeModel::getInstance()->clearMazEvent();
	MazeModel::getInstance()->setEventId(data["eventId"].asInt());

	MAZE_INFOS &mazeInfos = MazeModel::getInstance()->getMazeInfos();
	mazeInfos.curPower = data["movePower"].asInt();
	MainModel::getInstance()->getMainParam()->mMazePower = mazeInfos.curPower;
	MainModel::getInstance()->notifyObservers();

	mazeInfos.times = data["needTime"].asInt();
	mtLoginServerTime = time_t(data["nowdate"].asInt());

	RowData * eventData = DataManager::getInstance()->searchMazeThingById(MazeModel::getInstance()->getEventId());
	int eventType = eventData->getIntData("type");
	if (eventType == MAZE_RANDOMROAD_LIMIT_BUY)
	{
		vector<int> info;
		StringFormat::parseCsvStringVecByDu(eventData->getStringData("goldItem"),info);


		mazeInfos.mMazeBuyTime	= data["buyTime"].asInt();//秒
		mazeInfos.mMazedrawPetId	= info.at(0);//限时抽卡抽到的英雄Id
		mazeInfos.mMazeBuyCount	= info.at(1);//个数
		mazeInfos.mMazeBuyPrice	= info.at(2);//金币价格
		mazeInfos.mMazeBuyCurTime = secondNow();
	}
		
	m_pView->onRandomRoadResult(nResult);
}


void MazeController::processEventThingInfo(const char* szData)
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

	//判断是哪种事件
	int eventId = MazeModel::getInstance()->getEventId();
	RowData * eventData = DataManager::getInstance()->searchMazeThingById(eventId);
	if (eventData)
	{
		MazeModel::getInstance()->processMazesData(data);
	}
	else
	{
		MazeController::getInstance()->onGetView()->gotoFirst();
	}
}


void MazeController::processOpenBoxData(const char* szData)
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


	Json::Value date = root["data"];

	int boxId	= date["boxId"].asInt();

	MAZE_INFOS &mazeInfos = MazeModel::getInstance()->getMazeInfos();


	//地图碎片
	Json::Value mapList	= date["maplist"];
	mazeInfos.vecMaps.clear();
	for (int i = 0; i < mapList.size(); i++)
	{
		//'id':10001,'num':10
		MAZE_MAP_ITEM mapItem;
		mapItem.id				= mapList[i]["id"].asInt();
		mapItem.num				= mapList[i]["count"].asInt();
		mazeInfos.vecMaps.push_back(mapItem);
	}

		
	m_pView->onOpenBoxResult(boxId);
}

void MazeController::processOpenBoxEndData(const char* szData)
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

	m_pView->onOpenBoxEndResult();
}

void MazeController::processOpenMapBoxData(const char* szData)
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

	MazeModel::getInstance()->processMazesOpenCardData(data);
}


// 获取迷宫信息
void MazeController::onGetAllMazes()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAZE_GET_ALLINFO, "");
}

//迷宫随机路口事件
void MazeController::onRandomRoad()
{	
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAZE_RANDOM_ROAD, "");
}

//获取迷宫已获得的特定事件反馈消息(包括需要获取对话奖励，战斗胜利奖励)
void MazeController::onGetEventThingInfo(const int& eventId)
{
	Json::FastWriter  writer;
	Json::Value vParam;
	vParam["eventId"] = eventId;
	std::string  json_file=writer.write(vParam);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAZE_EVENT_THINGS_INFO, json_file.c_str());
}

//迷宫打开宝箱
void MazeController::onOpenBox(const int& id)
{
	Json::FastWriter  writer;
	Json::Value vParam;
	vParam["mapId"]			= id;
	std::string  json_file=writer.write(vParam);
	CCLOG("%s",json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAZE_OPEN_BOX, json_file.c_str());
}

//迷宫打开宝箱结束
void MazeController::onOpenBoxEnd()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAZA_OPEN_BOX_END, "");
}

//迷宫打开藏宝图宝箱
void MazeController::onOpenMapBox(const int& boxId ,const int& id)
{
	Json::FastWriter  writer;
	Json::Value vParam;
	vParam["boxId"] = boxId;
	vParam["pos"] = id;

	std::string  json_file=writer.write(vParam);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_MAZE_OPEN_MAP_BOX, json_file.c_str());
}

void MazeController::onRemoveView()
{
	m_pView = nullptr;
}
