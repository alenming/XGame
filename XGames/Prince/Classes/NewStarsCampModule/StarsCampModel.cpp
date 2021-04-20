#include "StarsCampModel.h"

StarsCampModel* StarsCampModel::mInstance = nullptr;

StarsCampModel::StarsCampModel():_cleckWeiZhi(0),_cleckHero(0),_shopStoneId(0),_newcampId(-1)
{
}

StarsCampModel::~StarsCampModel()
{

}

StarsCampModel* StarsCampModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new StarsCampModel();
	}
	return mInstance;
}

void StarsCampModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void StarsCampModel::notify(int eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}


void StarsCampModel::parseCampInfoData(const Json::Value &data)
{
	//每次进入星盘摘取数据前将玩家等级与Model里保存的等级进行比较 如果达到星盘开启等级 播放动画
	//int nowLevel  = HeroModel::getInstance()->getMainHeroLevel();

	_StarsCampInfo.clear();
	Json::Value starList = data["starList"];
	_cleckWeiZhi = data["starId"].asInt()-1;

	for(int i=0; i<starList.size(); i++)
	{
		
		One_StarsCamp_Attribute oneStarCamp;
		oneStarCamp.uid = starList[i]["starId"].asInt();

		auto oneLinup = DataManager::getInstance()->searchStarCampLinupById(oneStarCamp.uid);
		oneStarCamp.type = oneLinup->getIntData("type");
		//最大值 
		oneStarCamp.maxgj = oneLinup->getLongData("value1");
		oneStarCamp.maxxl = oneLinup->getLongData("value2");
		oneStarCamp.maxmf = oneLinup->getLongData("value3");
		oneStarCamp.maxwf = oneLinup->getLongData("value4");
		//oneStarCamp.tagTyp = oneLinup->getLongData("tagTyp");

		oneStarCamp.goLevel = oneLinup->getIntData("level");
		for (int j = 0; j < FIVEHERO; j++)
		{
			One_Hero_Attribute oneHero;
			oneHero.uId = oneLinup->getIntData("hero"+TO_STR(j+1));
			oneHero.stoneId = starList[i]["stoneId"+TO_STR(j+1)].asInt();
			//石头列表
			auto stoneList = starList[i]["stoneList"+TO_STR(j+1)];
			for (int k = 0; k < stoneList.size(); k++)
			{
				oneHero.haveStroneId.push_back(stoneList[k].asInt());
			}
			//BUff  ID   激活的为0
			auto buffList = starList[i]["buffList"+TO_STR(j+1)];
		  
			for (int k = 0; k < buffList.size(); k++)
			{
				if (k == 0)
				{
					oneHero.Collect_ID = buffList[k].asInt();
				}
				if (k == 1)
				{
					oneHero.Star_ID = buffList[k].asInt();
				}
				if (k == 2)
				{
					oneHero.Quality_ID = buffList[k].asInt();
				}
				if (k == 3)
				{
					oneHero.Jewel_ID = buffList[k].asInt();
				}
			}

			auto data = DataManager::getInstance()->searchAttbitureIntroduceById(oneHero.stoneId);
			if (data)
			{
				oneHero.stoneName = data->getStringData("name");
				oneHero.stoneiconID = data->getStringData("icon");
			}
			oneStarCamp.heroId.push_back(oneHero);
		}
		_StarsCampInfo.push_back(oneStarCamp);
	}

	computeOneHeroAttrubute();

	notify(SC_EVENT_TYPE_GET_INFO);
}


One_StarsCamp_Attribute* StarsCampModel::getOndeStarCampInfoByUid(int uId)
{
	for(auto &info : _StarsCampInfo)
	{
		if(info.uid == uId)
		{
			return &info;
		}
	}

	return nullptr;
}
One_StarsCamp_Attribute& StarsCampModel::getOndeStarCampInfoByWz(int wz)
{
	if (wz<_StarsCampInfo.size())
	{
		return _StarsCampInfo.at(wz);
	}
	else
	{
		return _StarsCampInfo.at(0);
	}
}

One_Hero_Attribute  StarsCampModel::getOndeHeroInfoByWz(int wz)
{
	auto Allhero = _StarsCampInfo.at(_cleckWeiZhi).heroId;
	if (wz<Allhero.size())
	{
		return Allhero.at(wz);
	}
	else
	{
		return Allhero.at(0);
	}
}

std::vector<One_StarsCamp_Attribute>& StarsCampModel::getAllStarCampInfo()
{
	return _StarsCampInfo;
}

int	StarsCampModel::getYeShu()
{
	if (_StarsCampInfo.size() ==0)
	{
		return 0;
	}

	int yeshu = _StarsCampInfo.size()/3+1;

	if (_StarsCampInfo.size()%3 == 0)
	{
		yeshu--;
	}
	return yeshu;
}

void StarsCampModel::parseCampShopDate( const Json::Value &data )
{
	notify(SC_EVENT_TYPE_SHOP);
}

void StarsCampModel::parseCampXiangQianDate( const Json::Value &data )
{
	notify(SC_EVENT_TYPE_XIANGQIAN);
}

void StarsCampModel::computeOneHeroAttrubute()
{
	for (int i = 0; i < _StarsCampInfo.size(); i++)
	{
		for (int  j=0; j<FIVEHERO; j++)
		{
			//ID为0 说明没有镶嵌石头 所有属性为0
			if (_StarsCampInfo.at(i).heroId.at(j).stoneId == 0)
			{
				_StarsCampInfo.at(i).heroId.at(j).gongji = 0;
				_StarsCampInfo.at(i).heroId.at(j).xueliang = 0;
				_StarsCampInfo.at(i).heroId.at(j).wufang = 0;
				_StarsCampInfo.at(i).heroId.at(j).mofang = 0;
				_StarsCampInfo.at(i).heroId.at(j).jiaChen = 0;
			}
			else
			{
				auto data = DataManager::getInstance()->searchAttbitureIntroduceById(_StarsCampInfo.at(i).heroId.at(j).stoneId);
				if (data)
				{
					//计算当前值
					_StarsCampInfo.at(i).heroId.at(j).gongji = data->getLongData("atk");
					_StarsCampInfo.at(i).heroId.at(j).xueliang = data->getLongData("hp");
					_StarsCampInfo.at(i).heroId.at(j).wufang =data->getLongData("pdd");
					_StarsCampInfo.at(i).heroId.at(j).mofang = data->getLongData("mdd");

					_StarsCampInfo.at(i).heroId.at(j).jiaChen = 0;

					auto BuffData1 = DataManager::getInstance()->searchHeroStoneById(_StarsCampInfo.at(i).heroId.at(j).Collect_ID);
					if (BuffData1)
					{
						_StarsCampInfo.at(i).heroId.at(j).jiaChen += (BuffData1->getFloatData("value")*100);
					}
					auto BuffData2 = DataManager::getInstance()->searchHeroStoneById(_StarsCampInfo.at(i).heroId.at(j).Quality_ID);
					if (BuffData2)
					{
						_StarsCampInfo.at(i).heroId.at(j).jiaChen += (BuffData2->getFloatData("value")*100);
					}
					auto BuffData3 = DataManager::getInstance()->searchHeroStoneById(_StarsCampInfo.at(i).heroId.at(j).Star_ID);
					if (BuffData3)
					{
						_StarsCampInfo.at(i).heroId.at(j).jiaChen += (BuffData3->getFloatData("value")*100);
					}
					auto BuffData4 = DataManager::getInstance()->searchHeroStoneById(_StarsCampInfo.at(i).heroId.at(j).Jewel_ID);
					if (BuffData4)
					{
						_StarsCampInfo.at(i).heroId.at(j).jiaChen += (BuffData4->getFloatData("value")*100);
					}

					_StarsCampInfo.at(i).gongji += _StarsCampInfo.at(i).heroId.at(j).gongji *(1+_StarsCampInfo.at(i).heroId.at(j).jiaChen/100);
					_StarsCampInfo.at(i).xueliang += _StarsCampInfo.at(i).heroId.at(j).xueliang*(1+_StarsCampInfo.at(i).heroId.at(j).jiaChen/100);
					_StarsCampInfo.at(i).wufang += _StarsCampInfo.at(i).heroId.at(j).wufang*(1+_StarsCampInfo.at(i).heroId.at(j).jiaChen/100);
					_StarsCampInfo.at(i).mofang += _StarsCampInfo.at(i).heroId.at(j).mofang*(1+_StarsCampInfo.at(i).heroId.at(j).jiaChen/100);
				}
			}
		}
	}
}
