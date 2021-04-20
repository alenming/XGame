#include "MarvellousActiveModel.h"

MarvellousActiveModel* MarvellousActiveModel::mInstance = nullptr;


MarvellousActiveModel::MarvellousActiveModel()
{
	isAddLayerActivity = false;
}

MarvellousActiveModel::~MarvellousActiveModel()
{

}

MarvellousActiveModel* MarvellousActiveModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new MarvellousActiveModel();
	}
	return mInstance;
}

void MarvellousActiveModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void MarvellousActiveModel::notify(MA_EVENT_TYPE eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void MarvellousActiveModel::parseMaInfoData(const Json::Value &data)
{
	m_MaInfo.clear();
	for (int i = 0; i < data["actList"].size(); i++)
	{
		MA_INFO maInfo;
		maInfo.id = data["actList"][i]["id"].asInt();
		maInfo.time = data["actList"][i]["time"].asInt();
		maInfo.flg = data["actList"][i]["flg"].asInt();

		m_MaInfo.push_back(maInfo);
	}

	sort(m_MaInfo.begin(),m_MaInfo.end(),[](MA_INFO a,MA_INFO b)
	{
		int actSortA = DataManager::getInstance()->searchBestActById(a.id)->getIntData("actSort");
		int actSortB = DataManager::getInstance()->searchBestActById(b.id)->getIntData("actSort");
		//判断A是否能领取
		if (actSortA < actSortB)
		{
			return true;
		}
		else
		{
			return false;
		}
	});

	notify(MA_EVENT_TYPE_GET_INFO);
}
