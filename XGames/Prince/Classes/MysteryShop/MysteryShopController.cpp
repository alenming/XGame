#include "MysteryShopController.h"
#include "MysteryShopModel.h"
#include "SDK_TalkingData/TalkingData.h"

MysteryShopController *MysteryShopController::mInstance = nullptr;


MysteryShopController::MysteryShopController():m_nRefreshType(0),coIndex(0)	,firstFresh(true)
{
	SocketCommunicator::getInstance()->addListener(this);
}

MysteryShopController::~MysteryShopController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

MysteryShopController* MysteryShopController::getInstance()
{
	if(!mInstance)
	{
		mInstance = new MysteryShopController();

	}
	return mInstance;
}

void MysteryShopController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_SHOPEXC_REFRESH:
		procShopInfo(szData);
		break;
	case nMAIN_CMD_SHOPEXC_BUY:
		procBuy(szData);
		break;
	default:
		break;
	}
}


void MysteryShopController::handleShopInfo(SHOP_TYPE type)
{
	// 首次取当前货物;
	// flag: 1.自然刷新 2.每天N次(VIP相关)货币(商店类型相关)刷新;3:过图后4:主城进入
	// storeTyp;
	Json::FastWriter writer;
	Json::Value root;

	int nCmdId = nMAIN_CMD_SHOPEXC_REFRESH;
	root["storeTyp"] = type;

	root["flag"] = 4;
	m_nRefreshType = 1;
	firstFresh = true;
	MysteryShopModel::getInstance()->setFreshSuccessful(false);
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());

}

int MysteryShopController::handleRefresh()
{
	// 验证可刷新次数(0点重置);
	if (MysteryShopModel::getInstance()->getLeftRefreshCount() <= 0)
	{
		return -1;
	}

	// 货币数量;
	if (!MysteryShopModel::getInstance()->checkRefreshCost())
	{
		return -2;
	}

	// 刷新(1-自然刷新  2-货币刷新);
	// 8卦和9宫商店特有：3-道具刷新;
	Json::FastWriter writer;
	Json::Value root;

	int nCmdId = nMAIN_CMD_SHOPEXC_REFRESH;
	root["storeTyp"] = MysteryShopModel::getInstance()->getShopType();
	root["flag"] = 2;
	m_nRefreshType = 2;
	firstFresh = false;

	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());
	return 0;
}

int MysteryShopController::handleBuy( int nIndex )
{
	// 验证货币数量和状态，可用时返回pos;
	int nRet = MysteryShopModel::getInstance()->isItemValid(nIndex);
	if (nRet >= 0)
	{
		// 购买;
		Json::FastWriter writer;
		Json::Value root;

		int nCmdId = nMAIN_CMD_SHOPEXC_BUY;	
		root["pos"] = nRet;
		root["storeTyp"] = MysteryShopModel::getInstance()->getShopType();

		string json_file = writer.write(root);
		SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());
		coIndex = nIndex;
	}
	return nRet;
}


void MysteryShopController::procShopInfo( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		Json::Value data = root["data"];

		//更新金币/银币
		MainModel::getInstance()->updateData(data);

		// 更新时间戳;
		//MysteryShopModel::getInstance()->setStamp(data["guankaStoreTime"].asInt());

		if(m_nRefreshType != 1)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20265),1,
				MysteryShopModel::getInstance()->getRefreshCost());
		}

		int reFreshCount = data["daycount"].asInt();
		MysteryShopModel::getInstance()->updateRefreshCount(reFreshCount);

		// 更新货物;
		vector<ShopItemParam>  vcGoods1;
		Json::Value goods = data["storeItemList"];
		for (unsigned int i = 0; i < goods.size(); ++i)
		{
			Json::Value single = goods[i];
			ShopItemParam item;
			item.nIndex = i;
			item.nShopType = MysteryShopModel::getInstance()->getShopType();
			item.nId = single["id"].asInt();
			item.nPrice = single["price"].asInt();
			item.nCount = single["amount"].asInt();
			item.moneyType = (MONEY_TYPE)single["costTyp"].asInt();
			item.buyLimit = single["buyLimit"].asInt();//buyLimit-1无限买,其它是对应的次数
			item.flag = single["flag"].asInt();
			if (item.buyLimit != -1 && item.buyLimit>item.flag)
			{
				item.bEnabled = true;
			}
			else
			{
				// 已售;
				item.bEnabled = false;
			}
			item.nSrvPos = single["pos"].asInt();
			vcGoods1.push_back(item);
		}

		MysteryShopModel::getInstance()->updateAllItems(vcGoods1);

		//刷新成功
		if (!firstFresh)
		{
			MysteryShopModel::getInstance()->setFreshSuccessful(true);
		}
		firstFresh = false;

		MysteryShopModel::getInstance()->updatePublicInfo();
	}

}

void MysteryShopController::procBuy( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}
		if (coIndex !=-1)
		{

			auto onitem = MysteryShopModel::getInstance()->getItem(coIndex);
			if (onitem->moneyType == MONEY_GOLD)
			{
				//金币消耗上报
				TalkingData::onPurchase(onitem->strName,1,onitem->nPrice);
			}	
			coIndex = -1;
		}

		Json::Value data = root["data"];

		// 更新背包;
		int nPkgUseSize = data["pkgUsesize"].asInt();
		int nPkgSize = data["pkgsize"].asInt();
		ToolModel::getInstance()->updateBackpackUsedCount(nPkgUseSize, nPkgSize);

		// 更新道具;
		ToolModel::getInstance()->updateToolJsonData(data["itemList"]);

		//更新金币/银币
		MainModel::getInstance()->updateData(data);

		//成功购买
		MysteryShopModel::getInstance()->setShopSuccessful(true);

		// 取回Id和剩余货币还有购买的个数;
		int nSrvPos = data["pos"].asInt();
		//int nMoney = data["commonscore"].asInt();
		int flag = data["flag"].asInt();

		MysteryShopModel::getInstance()->onItemSold(nSrvPos, flag);
		MysteryShopModel::getInstance()->updatePublicInfo();
	}
}

