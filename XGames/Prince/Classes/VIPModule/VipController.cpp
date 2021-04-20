#include "VipController.h"
#include "Communication/Command.h"
#include "Communication/SocketCommunicator.h"
#include "VipModel.h"
#include "VipLayer.h"
#include "MainModule/MainModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "SDK_TalkingData/TalkingData.h"


VipController* VipController::ms_pInstance = nullptr;

VipController* VipController::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new VipController();
	}
	return ms_pInstance;
}

void VipController::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

VipController::VipController()
{
	SocketCommunicator::getInstance()->addListener(this);
}

VipController::~VipController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

//解析VIP主界面信息
void VipController::parseVipGetListMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		VipModel::getInstance()->initFromSvr(data);

		//通知UI, 礼包是否已购买
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_VIP_GET_LIST;
		VipModel::getInstance()->notifyObservers(&obParam);
	}
}

//解析购买的VIP数据
void VipController::parseVipBuyMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		//记录付费点
		RowData* rowData = DataManager::getInstance()->searchVipDescriptionById(VipModel::getInstance()->getCurBuyVipBoxId());
		if(rowData)
		{
			TalkingData::onPurchase(rowData->getStringData("name"), 1, rowData->getIntData("price"));
		}		

		Json::Value data = root["data"];

		MainModel::getInstance()->updateData(data);

		//英雄数据
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		//道具数据
		vector<TOOL_NEW_INFO> vecNewTools;
		Json::Value items = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

		//更改当前购买的特权礼包状态
		VipModel::getInstance()->getVecVipInfo().at(VipLayer::m_curBuyVipLv).mIsTaked = true;

		//通知UI, 购买了礼包
		ObserverParam obParam;
		obParam.id = nMAIN_CMD_VIP_BUY_GIFT;
		VipModel::getInstance()->notifyObservers(&obParam);
	}
}

void VipController::parseOrderIdMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if(read.parse(szData, root))
	{
		if(throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];
		string orderId = data["orderNo"].asString();
		string extension = data["extension"].asString();



		OrderInfo orderInfo;
		orderInfo.orderId = orderId;
		orderInfo.extension = extension;
        
        // Model记录订单号;
        VipModel::getInstance()->setOrderId(orderId);

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_VIP_GET_ORDERID;
		obParam.updateData = (void *)&orderInfo;
		VipModel::getInstance()->notifyObservers(&obParam);
	}
}

//获取VIP主界面信息
void VipController::sendGetVipListMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_VIP_GET_LIST, "");
}

//购买vip获得的数据
void VipController::sendGetVipBuyMsg(int vipId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["vipId"] = vipId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_VIP_BUY_GIFT, json_file.c_str());
}

//发送获取订单号获得的相关道具数据
void VipController::sendGetOrderId(int productId)
{
	Json::FastWriter writer;
	Json::Value data;
	data["productId"] = productId;
	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_VIP_GET_ORDERID, json_file.c_str());
}

void VipController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_VIP_GET_LIST:
		parseVipGetListMsg(szData);
		break;
	case nMAIN_CMD_VIP_BUY_GIFT:
		parseVipBuyMsg(szData);
		break;
	case nMAIN_CMD_VIP_GET_ORDERID:
		parseOrderIdMsg(szData);
	default:
		break;
	}
}

