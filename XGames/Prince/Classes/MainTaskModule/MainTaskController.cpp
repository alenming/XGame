#include "MainTaskController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "MainTaskModel.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"

MainTaskController* MainTaskController::mInstance = nullptr;

MainTaskController::MainTaskController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

MainTaskController::~MainTaskController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

MainTaskController* MainTaskController::getInstance()
{
	if (nullptr == mInstance)
	{
		mInstance = new MainTaskController;
	}
	return mInstance;
}

void MainTaskController::destroyInstance()
{
	if (nullptr != mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void MainTaskController::sendTaskReceiveMsg(int taskId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["taskId"] = taskId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TASK_RECEIVE, json_file.c_str());
}

void MainTaskController::sendGetTaskListMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TASK_GET_LIST, "");
}

//����������ȡЭ��
void MainTaskController::parseTaskReceiveMsg(const char* szData)
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
		Json::Value changeInfo = data["changeInfo"];

		//Ӣ������
		Json::Value petLists = changeInfo["petList"];
		HeroModel::getInstance()->updateData(petLists);
	
		//��������
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = changeInfo["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

		//������
		MainModel::getInstance()->updateData(changeInfo);

		//֪ͨ��mainBuildingLayer �� mainInfoLayer
		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_TASK_RECEIVE;	
		MainModel::getInstance()->notifyObservers(&observerParam);

		//����Id
		int taskId = data["taskId"].asInt();

		//�������
		MainTaskModel::getInstance()->doneTask(taskId);
	}
}

//���������б�Э��
void MainTaskController::parseGetTaskListMsg(const char* szData)
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

		MainTaskModel::getInstance()->initFromSvr(data);
	}
}

//���������б�Э��
void MainTaskController::parseTaskPushMsg(const char* szData)
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
		Json::Value taskList = data["taskList"];

		MainTaskModel::getInstance()->updateFromSvr(taskList);
	}
}

void MainTaskController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_TASK_RECEIVE:
		parseTaskReceiveMsg(szData);
		break;
	case nMAIN_CMD_TASK_GET_LIST:
		parseGetTaskListMsg(szData);
		break;
	case nMAIN_CMD_TASK_PUSH_INFO:
		parseTaskPushMsg(szData);
		break;
	default:
		break;
	}
	return;
}





