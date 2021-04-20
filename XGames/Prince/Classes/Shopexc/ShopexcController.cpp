#include "ShopexcController.h"
#include "MainModule/MainModel.h"
#include "ShopexcModel.h"
#include "ToolModule/ToolModel.h"
#include "SDK_TalkingData/TalkingData.h"


ShopexcController::ShopexcController(void)
	: m_nStamp(0)
	, m_nRefreshType(1)
{
	SocketCommunicator::getInstance()->addListener(this);
}


ShopexcController::~ShopexcController(void)
{
	SocketCommunicator::getInstance()->removeListener(this);
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(ShopexcController::refreshCountDown), this);
	ShopexcModel::destroyInstance();
}

void ShopexcController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_SHOPEXC_BUY:
		procBuy(szData);
		break;

	case nMAIN_CMD_SHOPEXC_REFRESH:
		procRefresh(szData);
		break;

	case nMAIN_CMD_SHOPEXC_REFINE_REFRESH:
		procRefineRefresh(szData);
		break;

	case nMAIN_CMD_SHOPEXC_REFINE_BUY:
		procRefineBuy(szData);
		break;

	default:
		break;
	}
}

void ShopexcController::handleShopInfo(ShopType type)
{
	// 首次取当前货物;
	// flag: 1.自然刷新 2.每天N次(VIP相关)货币(商店类型相关)刷新;
	// storeTyp;
	Json::FastWriter writer;
	Json::Value root;

	int nCmdId = nMAIN_CMD_SHOPEXC_REFRESH;
	if (ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_8
		|| ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_9)
	{
		root["module"] = ShopexcModel::getInstance()->getShopType();
		root["refreshflag"] = 1;
		nCmdId = nMAIN_CMD_SHOPEXC_REFINE_REFRESH;
	}
	else
	{
		root["storeTyp"] = ShopexcModel::getInstance()->getShopType();
		root["flag"] = 1;
		nCmdId = nMAIN_CMD_SHOPEXC_REFRESH;
	}

	m_nRefreshType = 1;
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());
}

int ShopexcController::handleBuy( int nIndex )
{
	// 验证货币数量和状态，可用时返回pos;
	int nRet = ShopexcModel::getInstance()->isItemValid(nIndex);
	if (nRet >= 0)
	{
		// 购买;
		Json::FastWriter writer;
		Json::Value root;
		int nCmdId = nMAIN_CMD_SHOPEXC_BUY;
		if (ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_8
			|| ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_9)
		{
			root["pos"] = nRet;
			root["module"] = ShopexcModel::getInstance()->getShopType();
			nCmdId = nMAIN_CMD_SHOPEXC_REFINE_BUY;
		}
		else
		{
			root["pos"] = nRet;
			root["storeTyp"] = ShopexcModel::getInstance()->getShopType();
			nCmdId = nMAIN_CMD_SHOPEXC_BUY;
		}
		string json_file = writer.write(root);
		SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());
	}
	return nRet;
}

int ShopexcController::handleRefresh()
{
	// 验证可刷新次数(0点重置);
	if (ShopexcModel::getInstance()->getLeftRefreshCount() <= 0)
	{
		return -1;
	}

	// 货币数量;
	if (!ShopexcModel::getInstance()->checkRefreshCost())
	{
		return -2;
	}

	// 刷新(1-自然刷新  2-货币刷新);
	// 8卦和9宫商店特有：3-道具刷新;
	Json::FastWriter writer;
	Json::Value root;

	int nCmdId = nMAIN_CMD_SHOPEXC_REFRESH;
	if (ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_8
		|| ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_9)
	{
		// 判断剩余刷新令数量来确认是什么刷新类型;
		int nRefreshTokenCount = ToolModel::getInstance()->searchNumByTemplateId(10005);
		m_nRefreshType = nRefreshTokenCount > 0 ? 3 : 2;
		root["module"] = ShopexcModel::getInstance()->getShopType();
		root["refreshflag"] = m_nRefreshType;
		nCmdId = nMAIN_CMD_SHOPEXC_REFINE_REFRESH;
	}
	else
	{
		root["storeTyp"] = ShopexcModel::getInstance()->getShopType();
		root["flag"] = 2;
		m_nRefreshType = 2;
		nCmdId = nMAIN_CMD_SHOPEXC_REFRESH;
	}
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());
	return 0;
}

void ShopexcController::procBuy( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		Json::Value data = root["data"];



		// 取回Id和剩余货币;
		int nSrvPos = data["pos"].asInt();
		int nMoney = data["score"].asInt();

		ShopexcModel::getInstance()->onItemSold(nSrvPos, nMoney);

		// 更新背包;
		int nPkgUseSize = data["pkgUsesize"].asInt();
		int nPkgSize = data["pkgsize"].asInt();
		ToolModel::getInstance()->updateBackpackUsedCount(nPkgUseSize, nPkgSize);

		// 更新道具;
		ToolModel::getInstance()->updateToolJsonData(data["itemList"]);

		//更新金币/银币
		MainModel::getInstance()->updateData(data);
	}
}

void ShopexcController::procRefineBuy( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		Json::Value data = root["data"];

		ShopexcModel::getInstance()->setChouSuccessful(true);

		// 取回Id和剩余货币;
		int nSrvPos = data["pos"].asInt();
		int nTotalMoney = 0;
		if (ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_8)
		{
			nTotalMoney = data["hunTiandan"].asInt();
		}
		else if (ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_9)
		{
			nTotalMoney = data["jiuZhuandan"].asInt();
		}
		ShopexcModel::getInstance()->onItemSold(nSrvPos, nTotalMoney);

		// 更新背包;
		int nPkgUseSize = data["pkgUsesize"].asInt();
		int nPkgSize = data["pkgsize"].asInt();
		ToolModel::getInstance()->updateBackpackUsedCount(nPkgUseSize, nPkgSize);

		// 更新道具;
		ToolModel::getInstance()->updateToolJsonData(data["itemList"]);

		//更新金币/银币
		MainModel::getInstance()->updateData(data);

		ShopexcModel::getInstance()->notifyObservers(); //炼化炉商店兑换 要通知炼化炉列表更新

		ShopexcItemParam* ItemParam = ShopexcModel::getInstance()->getItem(nSrvPos);
		if ((ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_8
			|| ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_9) 
			&& ItemParam && ItemParam->nCount != 0 && ItemParam->costType == SHOP_COST_TYPE_GOLD)
		{
			TalkingData::onPurchase(TO_STR(ItemParam->nCount) + ItemParam->strName,1,ItemParam->nPrice);
		}
	}
}

void ShopexcController::procRefresh( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		Json::Value data = root["data"];

		// 更新时间戳;
		m_nStamp = data["seconds"].asInt();

		// 重新计时;
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(ShopexcController::refreshCountDown), this);
		Director::getInstance()->getScheduler()->schedule(
			schedule_selector(ShopexcController::refreshCountDown), this, 1.0f, kRepeatForever, 0.1f, false);

		// 更新货币;
		int nTotalMoney = data["pvpscore"].asInt();
		ShopexcModel::getInstance()->setMoney(nTotalMoney);

		// 更新已刷新次数;
		if (m_nRefreshType != 1)
		{
			ShopexcModel::getInstance()->updateRefreshCount();
		}

		// 更新货物;
		vector<ShopexcItemParam>  vcGoods;
		Json::Value goods = data["storeItemList"];
		for (unsigned int i = 0; i < goods.size(); ++i)
		{
			Json::Value single = goods[i];
			ShopexcItemParam item;
			item.nIndex = i;
			item.nShopType = ShopexcModel::getInstance()->getShopType();
			item.nId = single["id"].asInt();
			item.nPrice = single["price"].asInt();
			item.nCount = single["amount"].asInt();
			item.costType = (SHOP_COST_TYPE)single["costTyp"].asInt();
			item.bEnabled = (single["flag"].asInt() == 0) ? true : false;
			item.nRank = single["costRank"].asInt();
			item.nLimitBuyCount = single["usertopFor"].asInt();
			item.nSrvPos = single["pos"].asInt();
			vcGoods.push_back(item);
		}
		ShopexcModel::getInstance()->updateAllItems(vcGoods);
		ShopexcModel::getInstance()->updatePublicInfo();
	}
}

void ShopexcController::procRefineRefresh( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		if ((ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_8
			|| ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_9)
			&& m_nRefreshType == 2)
		{
			TalkingData::onPurchase("炼化炉商店刷新",1,ShopexcModel::getInstance()->getRefreshCost());
		}


		Json::Value data = root["data"];

		// 更新时间戳;
		m_nStamp = data["seconds"].asInt();

		// 重新计时;
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(ShopexcController::refreshCountDown), this);
		Director::getInstance()->getScheduler()->schedule(
			schedule_selector(ShopexcController::refreshCountDown), this, 1.0f, kRepeatForever, 0.1f, false);

		// 更新货币;
		Json::Value changeInfo = data["changeInfo"];
		int nTotalMoney = 0;
		if (ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_8)
		{
			nTotalMoney = changeInfo["hunTiandan"].asInt();
		}
		else if (ShopexcModel::getInstance()->getShopType() == SHOP_TYPE_REFINE_9)
		{
			nTotalMoney = changeInfo["jiuZhuandan"].asInt();
		}
		ShopexcModel::getInstance()->setMoney(nTotalMoney);
		MainModel::getInstance()->setGold(changeInfo["gold"].asInt());
		MainModel::getInstance()->setSilver(changeInfo["coin"].asInt());

		// 更新背包(包含刷新令);
		ToolModel::getInstance()->updateToolJsonData(changeInfo["itemList"]);

		// 更新已刷新次数;
		if (m_nRefreshType != 1)
		{
			ShopexcModel::getInstance()->updateRefreshCount();
		}

		// 更新货物;
		vector<ShopexcItemParam>  vcGoods;
		Json::Value goods = data["storeItemList"];
		for (unsigned int i = 0; i < goods.size(); ++i)
		{
			Json::Value single = goods[i];
			ShopexcItemParam item;
			item.nIndex = i;
			item.nShopType = ShopexcModel::getInstance()->getShopType();
			item.nId = single["id"].asInt();
			item.nPrice = single["price"].asInt();
			item.costType = (SHOP_COST_TYPE)single["costTyp"].asInt();
			item.nCount = single["amount"].asInt();
			item.bEnabled = (single["flag"].asInt() == 0) ? true : false;
			item.nRank = single["costRank"].asInt();
			item.nLimitBuyCount = single["usertopFor"].asInt();
			item.nSrvPos = single["pos"].asInt();
			vcGoods.push_back(item);
		}
		ShopexcModel::getInstance()->updateAllItems(vcGoods);
		ShopexcModel::getInstance()->updatePublicInfo();
		//Director::getInstance()->getScheduler()->schedule([=](float dt)->void
		//{
		//	ShopexcModel::getInstance()->updatePublicInfo();
		//
		//Director::getInstance()->getScheduler()->unschedule("ShopexcModel::updatePublicInfo", this);
		//},this, 0, 0, 0, false, "ShopexcModel::updatePublicInfo");

	}
}

void ShopexcController::refreshCountDown( float delta )
{
	ShopexcModel::getInstance()->onCountDown(m_nStamp);
	if (m_nStamp <= 0)
	{
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(ShopexcController::refreshCountDown), this);

		// 主动刷新;
		handleShopInfo(ShopexcModel::getInstance()->getShopType());
		return;
	}
	m_nStamp--;
}
