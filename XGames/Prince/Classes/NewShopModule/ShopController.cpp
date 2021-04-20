#include "ShopController.h"
#include "MainModule/MainModel.h"
#include "ShopModel.h"
#include "ToolModule/ToolModel.h"
#include "SDK_TalkingData/TalkingData.h"
#include "DataManager/DataManager.h"

#define  KEY_ID   10052

ShopController *ShopController::mInstance = nullptr;


ShopController::ShopController()
	:// m_nStamp(0)
	 m_nRefreshType(1)
	, firstFresh(true)
	, coIndex(-1)
{
	SocketCommunicator::getInstance()->addListener(this);
}

ShopController::~ShopController()
{
	SocketCommunicator::getInstance()->removeListener(this);
	ShopModel::destroyInstance();
}

ShopController* ShopController::getInstance()
{
	if(!mInstance)
	{
		mInstance = new ShopController();

	}
	return mInstance;
}

void ShopController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_COMMON_FREE_CHOU:
		proFirtShop(szData);
		break;
	case nMAIN_CMD_SHOPEXC_BUY:
		procBuy(szData);
		break;
	case nMAIN_CMD_SHOPEXC_REFRESH:
		procRefresh(szData);
		break;
	case nMAIN_CMD_COMMON_SHOP_CHOU:
		procChouBack(szData);
		break;
	default:
		break;
	}
}

void ShopController::handleFirstShopLayerInfo()
{
	int nCmdId = nMAIN_CMD_COMMON_FREE_CHOU;
	SocketCommunicator::getInstance()->sendMessage(nCmdId,"");
	//loadLocalCuilianInfo();
}

void ShopController::handleShopInfo(SHOP_TYPE type)
{
	// 首次取当前货物;
	// flag: 1.自然刷新 2.每天N次(VIP相关)货币(商店类型相关)刷新;
	// storeTyp;
	Json::FastWriter writer;
	Json::Value root;

	int nCmdId = nMAIN_CMD_SHOPEXC_REFRESH;
	root["storeTyp"] = type;
	root["flag"] = 1;
	
	firstFresh = true;
	ShopModel::getInstance()->setFreshSuccessful(false);
	ShopModel::getInstance()->setShopType(type);
	m_nRefreshType = 1;
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());

}

int ShopController::handleBuy( int nIndex )
{
	// 验证货币数量和状态，可用时返回pos;
	int nRet = ShopModel::getInstance()->isItemValid(nIndex);
	if (nRet >= 0)
	{
		// 购买;
		Json::FastWriter writer;
		Json::Value root;

		int nCmdId = nMAIN_CMD_SHOPEXC_BUY;	
		root["pos"] = nRet;
		root["storeTyp"] = ShopModel::getInstance()->getShopType();

		string json_file = writer.write(root);
		SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());
		coIndex = nIndex;
	}
	return nRet;
}

void ShopController::handleChouOneOrTen( CHOU_TYPE tpe )
{
	Json::FastWriter writer;
	Json::Value root;

	m_chouType = tpe;

	int nCmdId = nMAIN_CMD_COMMON_SHOP_CHOU;
	if (tpe == ONE_CHOU)
	{
		root["times"] = 1;
	}
	if (tpe == TEN_CHOU)
	{
		root["times"] = 10;
	}
	string json_file = writer.write(root);

	SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());
}

int ShopController::handleRefresh()
{
	// 验证可刷新次数(0点重置);
	if (ShopModel::getInstance()->getLeftRefreshCount() <= 0)
	{
		return -1;
	}

	// 货币数量;
	if (!ShopModel::getInstance()->checkRefreshCost())
	{
		return -2;
	}

	// 刷新(1-自然刷新  2-货币刷新);
	// 8卦和9宫商店特有：3-道具刷新;
	Json::FastWriter writer;
	Json::Value root;

	int nCmdId = nMAIN_CMD_SHOPEXC_REFRESH;
	root["storeTyp"] = ShopModel::getInstance()->getShopType();
	root["flag"] = 2;
	m_nRefreshType = 2;
	firstFresh = false;
	
	string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nCmdId, json_file.c_str());
	return 0;
}

void ShopController::procBuy( const char* szData )
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
			if (ShopModel::getInstance()->getShopType()==ST_ZAHUO)
			{
				auto onitem = ShopModel::getInstance()->getItem(coIndex);
				if (onitem->moneyType == MONEY_GOLD)
				{
					TalkingData::onPurchase(onitem->strName,1,onitem->nPrice);
				}	
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


		// 取回Id和剩余货币还有购买的个数;
		int nSrvPos = data["pos"].asInt();
		//int nMoney = data["commonscore"].asInt();
		int flag = data["flag"].asInt();

		//成功抽取
		ShopModel::getInstance()->setChouSuccessful(true);

		if (ShopModel::getInstance()->getShopType() != ST_CUILIAN &&ShopModel::getInstance()->getShopType() != ST_NONE )
		{
			ShopModel::getInstance()->onItemSold(nSrvPos, flag);
		}
		else if (ShopModel::getInstance()->getShopType() == ST_CUILIAN)
		{
			//ShopModel::getInstance()->setMoney(nMoney);
			ShopModel::getInstance()->updateCuilian();		
		}
	}
}

void ShopController::procChouBack( const char* szData )
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
		//抽之后返回的数据
		//ShopModel::getInstance()->setFreshCount(data["freshCount"].asInt());
		//ShopModel::getInstance()->setIsFree(data["isFree"].asBool());
		if(m_chouType ==ONE_CHOU)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10250),1,ShopModel::getInstance()->getOneChouPrice());
		}
		else if (m_chouType ==TEN_CHOU)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(10251),1,ShopModel::getInstance()->getTenChouPrice());
		}

		//抽到的东西
		auto allchouParam = data["newItemList"];
		std::vector<ChouItemParam> chouParam;
		ShopModel::getInstance()->m_ChouPram.clear();
		for (int i = 0; i < allchouParam.size(); i++)
		{
			ChouItemParam item;
			Json::Value single = allchouParam[i];
			item.nIndex = i;
			item.nId = single["id"].asInt();
			//auto ToolData = DataManager::getInstance()->searchToolById(item.nId);
			item.nPrice = 0;
			item.nCount = single["amount"].asInt();
			item.strName = "";//ToolData->getStringData("itemName");	//用ID去查找
			ShopModel::getInstance()->m_ChouPram.push_back(item);
		}

		random_shuffle(ShopModel::getInstance()->m_ChouPram.begin(), ShopModel::getInstance()->m_ChouPram.end()); 

		auto temp = ShopModel::getInstance()->m_ChouPram;
		// 更新道具;
		ToolModel::getInstance()->updateToolJsonData(data["itemList"]);
		//更新金币/银币
		MainModel::getInstance()->updateData(data);
		//如果返回的道具个数是1而且有免费抽 就把字段设置为false
		if (data["newItemList"].size() ==1 && ShopModel::getInstance()->getIsFree())
		{
			ShopModel::getInstance()->setIsFree(false);
		}

		//钥匙
		auto oneTool = ToolModel::getInstance()->GetToolInfoBytemplateId(10052);
		if (oneTool == nullptr)
		{
			ShopModel::getInstance()->setFreshCount(0);
		}
		else
		{
			int count = oneTool->nstack;
			ShopModel::getInstance()->setFreshCount(count);
		}
	}
	
	ShopModel::getInstance()->updateChouInfo(m_chouType);
}

void ShopController::procRefresh( const char* szData )
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

		// 更新时间戳;
		ShopModel::getInstance()->setStamp(data["seconds"].asInt());
		ShopModel::getInstance()->setLocalTime();

		if(m_nRefreshType != 1 && ShopModel::getInstance()->getShopType() ==ST_ZAHUO)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20252),1,ShopModel::getInstance()->getRefreshCost());
		}
		if(m_nRefreshType != 1 && ShopModel::getInstance()->getShopType() ==ST_RONGYU)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20253),1,ShopModel::getInstance()->getRefreshCost());
		}
		if(m_nRefreshType != 1 && ShopModel::getInstance()->getShopType() ==ST_JIFENG)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20254),1,ShopModel::getInstance()->getRefreshCost());
		}
		if(m_nRefreshType != 1 && ShopModel::getInstance()->getShopType() ==ST_SEHGNWANG)
		{
			TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20255),1,ShopModel::getInstance()->getRefreshCost());
		}

		// 更新已刷新次数;
		if (m_nRefreshType != 1)
		{
			ShopModel::getInstance()->updateRefreshCount();
		}

		// 更新货物;
		vector<ShopItemParam>  vcGoods1;
		Json::Value goods = data["storeItemList"];
		for (unsigned int i = 0; i < goods.size(); ++i)
		{
			Json::Value single = goods[i];
			ShopItemParam item;
			item.nIndex = i;
			item.nShopType = ShopModel::getInstance()->getShopType();
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
		//刷新成功
		if (!firstFresh)
		{
			ShopModel::getInstance()->setFreshSuccessful(true);
		}
		firstFresh = false;


		// 更新背包;
		int nPkgUseSize = data["pkgUsesize"].asInt();
		int nPkgSize = data["pkgsize"].asInt();
		ToolModel::getInstance()->updateBackpackUsedCount(nPkgUseSize, nPkgSize);

		// 更新道具;
		ToolModel::getInstance()->updateToolJsonData(data["itemList"]);

		//更新金币/银币
		auto changeInfo = data["changeInfo"];
		MainModel::getInstance()->updateData(changeInfo);

		
		ShopModel::getInstance()->updateAllItems(vcGoods1);
		ShopModel::getInstance()->updatePublicInfo();
	}
}

void ShopController::uadataCuiLianShopInfo()
{
	ShopModel::getInstance()->initCuiLian();
	ShopModel::getInstance()->updateCuilian();	
}

void ShopController::loadLocalCuilianInfo()
{
	//可以获得道具的数据
	//卡库
	//本地表里面有
	auto data =  DataManager::getInstance()->searchToolSystemById(10115);
	auto cangetKu = data->getStringData("value");
	vector<int> vecDst;
	StringFormat::parseCsvStringVec(cangetKu,vecDst);
	std::vector<ChouItemParam> chouItemParam;
	ShopModel::getInstance()->m_canGetPram.clear();
	for (int i = 0; i < vecDst.size(); i++)
	{
		ChouItemParam item;
		item.nIndex = i;
		item.nId = vecDst.at(i);
		ShopModel::getInstance()->m_canGetPram.push_back(item);
	}

	auto data1 =  DataManager::getInstance()->searchToolSystemById(10121);
	ShopModel::getInstance()->setOneChouPrice(data1->getIntData("value"));
	auto data2 =  DataManager::getInstance()->searchToolSystemById(10122);
	ShopModel::getInstance()->setTenChouPrice(data2->getIntData("value"));

	//淬练商店的数据
	//m_nStamp = 1;
	ShopModel::getInstance()->getCuiLianInfo().clear();

	DataTable* alldate = DataManager::getInstance()->getDTShop();
	auto aaa = alldate->getTableData();
	int i = 0;
	for (auto iterrater = aaa.begin(); iterrater!=aaa.end(); iterrater++)
	{
		auto oneDate = (RowData)iterrater->second;

		if (oneDate.getIntData("module") == 1)
		{
			ShopItemParam item;
			item.nIndex = i++;
			item.nShopType = SHOP_TYPE::ST_CUILIAN;
			auto itemList = oneDate.getStringData("itemList");
			std::vector<int> abc;
			StringFormat::parseCsvStringVecByDu(itemList,abc);

			item.nId = abc.at(0);
			item.nCount = abc.at(1);;
			item.nPrice = abc.at(2);;
			item.bEnabled = true;
			item.moneyType = MONEY_TYPE::MONEY_COMMON;//(MONEY_TYPE)data->getIntData("costTyp");
			item.buyLimit = oneDate.getIntData("buyLimit");
			item.nSrvPos = oneDate.getIntData("pos");
			ShopModel::getInstance()->getCuiLianInfo().push_back(item);
		}
	}
}	

void ShopController::proFirtShop( const char* szData )
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
		//是否为免费抽
		auto freecounttime = data["freecount"].asInt();
		ShopModel::getInstance()->setFreeChouTime(freecounttime);
		bool isfree = freecounttime<=0?true:false;//>0?true:false;

		ShopModel::getInstance()->setIsFree(isfree);

		ShopModel::getInstance()->setEndTime(freecounttime);

		//钥匙
		auto oneTool = ToolModel::getInstance()->GetToolInfoBytemplateId(KEY_ID);
		if (oneTool == nullptr)
		{
			ShopModel::getInstance()->setFreshCount(0);
		}
		else
		{
			int count = oneTool->nstack;
			ShopModel::getInstance()->setFreshCount(count);
		}
		//判断一下小红点
		IsShowRedFlag();

		ShopModel::getInstance()->updateFirstShopInfo();
	}


}

void ShopController::IsShowRedFlag()
{
	//给主城界面小红点判断使用
	int count = ShopModel::getInstance()->getFreshCount();
	bool isFree = ShopModel::getInstance()->getIsFree();

	if (count >0 || isFree)
	{
		ShopModel::getInstance()->setIsShowFlag(true);
	}
	else if(count <=0 && isFree ==false)
	{
		ShopModel::getInstance()->setIsShowFlag(false);
	}
}




