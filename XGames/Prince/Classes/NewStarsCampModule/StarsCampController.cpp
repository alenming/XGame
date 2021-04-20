#include "StarsCampController.h"
#include "SDK_TalkingData/TalkingData.h"
#define  GOLD_TYPE  2

StarsCampController *StarsCampController::mInstance = nullptr;


StarsCampController::StarsCampController():coStoId(-1)
{
	SocketCommunicator::getInstance()->addListener(this);
}

StarsCampController::~StarsCampController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

StarsCampController* StarsCampController::getInstance()
{
	if(!mInstance)
	{
		mInstance = new StarsCampController();

	}
	return mInstance;
}

void StarsCampController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nMAIN_CMD_GET_STAR_CAMP_INFO:
		{
			processCampInfoData(szData);
		}
		break;
	case nMAIN_CMD_SHOP_STAR_STONE:
		{
			processCampShopDate(szData);
		}
		break;
	case nMAIN_CMD_XIANG_QIAN_STAR_STONE:
		{
			processCampXiangQianDate(szData);
		}
		break;

	default:
		break;
	}
}


void StarsCampController::processCampInfoData(const char* szData)
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

		StarsCampModel::getInstance()->parseCampInfoData(data);
	}

}
void StarsCampController::processCampShopDate(const char *szData )
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

		//更新主角数据
		MainModel::getInstance()->updateData(data);
		//英雄数据更新
		Json::Value heroList = data["petList"];
		HeroModel::getInstance()->updateData(heroList);

		StarsCampModel::getInstance()->parseCampShopDate(data);
	}


}

void StarsCampController::processCampXiangQianDate(const char *szData  )
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

		if (coStoId != -1)
		{
			auto data = DataManager::getInstance()->searchAttbitureIntroduceById(coStoId);
			if (data != nullptr && data->getIntData("moneyType") == GOLD_TYPE)
			{
				TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20244),1,data->getLongData("price"));
			}
			coStoId = -1;
		}

		//更新主角数据
		MainModel::getInstance()->updateData(data);
		//英雄数据更新
		Json::Value heroList = data["petList"];
		HeroModel::getInstance()->updateData(heroList);

		StarsCampModel::getInstance()->parseCampXiangQianDate(data);
	}

}

void StarsCampController::sendGetStarsCampInfo()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GET_STAR_CAMP_INFO, "");
}

void StarsCampController::sendShopStone(int id)
{
	Json::FastWriter writer;
	Json::Value data;
	
	auto _StarsCampInfo = StarsCampModel::getInstance()->getAllStarCampInfo();
	auto _cleckWeiZhi =  StarsCampModel::getInstance()->getCleckWZ();
	auto _cleckHero = StarsCampModel::getInstance()->getCleckHero();

	data["stoneId"] = id;//石头ID
	data["heroId"] = _StarsCampInfo.at(_cleckWeiZhi).heroId.at(_cleckHero).uId;//英雄ID
	data["starId"] = _StarsCampInfo.at(_cleckWeiZhi).uid;//星盘ID

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_SHOP_STAR_STONE, json_file.c_str());
}

void StarsCampController::sendXiangQianStone(int id)
{
	Json::FastWriter writer;
	Json::Value data;


	auto _StarsCampInfo = StarsCampModel::getInstance()->getAllStarCampInfo();
	auto _cleckWeiZhi =  StarsCampModel::getInstance()->getCleckWZ();
	auto _cleckHero = StarsCampModel::getInstance()->getCleckHero();

	data["stoneId"] = id; //石头ID
	data["heroId"] = _StarsCampInfo.at(_cleckWeiZhi).heroId.at(_cleckHero).uId;//英雄ID
	data["starId"] = _StarsCampInfo.at(_cleckWeiZhi).uid; //星盘ID


	coStoId = id;

	std::string json_file = writer.write(data);

	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_XIANG_QIAN_STAR_STONE, json_file.c_str());
}

bool StarsCampController::isShowNewCampRedFlag()
{
	StarsCampModel::getInstance()->startcampOpenLevel.clear();

	DataTable* alldate = DataManager::getInstance()->getTableStarCampLinup();
	auto aaa = alldate->getTableData();
	int i = 0;
	for (auto iterrater = aaa.begin(); iterrater!=aaa.end(); iterrater++)
	{
		auto oneDate = (RowData)iterrater->second;

		StarsCampModel::getInstance()->startcampOpenLevel.push_back(oneDate.getIntData("level"));
	}
	int nowLvel = HeroModel::getInstance()->getMainHeroLevel();
	for (int i = 0; i < (StarsCampModel::getInstance()->startcampOpenLevel.size()); i++)
	{
		if (nowLvel ==  StarsCampModel::getInstance()->startcampOpenLevel.at(i))
		{
			//是新开启等级
			StarsCampModel::getInstance()->setNewCampId(i);
			UserDefault::getInstance()->setIntegerForKey("isopencamp",i+1);
			return true;
		}
	}

	return false;
}




