#include "RefineryModel.h"

RefineryModel* RefineryModel::mInstance = nullptr;

RefineryModel::RefineryModel()
{
	m_state = REFINERY_STATE_BAGUA;
}

RefineryModel::~RefineryModel()
{

}

RefineryModel* RefineryModel::getInstance()
{
	if (nullptr == mInstance)
	{
		mInstance = new RefineryModel();		
	}

	return mInstance;
}

void RefineryModel::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void RefineryModel::notify(REFINERY_EVENT_TYPE eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void RefineryModel::parseRefineryData(const Json::Value &data)
{
	// 更新背包;
	Json::Value _itemList = data["itemList"];
	ToolModel::getInstance()->updateToolJsonData(_itemList);


	MainParam* mainParam = MainModel::getInstance()->getMainParam();
	mainParam->mHunTiandan = data["hunTiandan"].asInt();
	mainParam->mJiuZhuandan = data["jiuZhuandan"].asInt();
	MainModel::getInstance()->notifyObservers();

	notify(REFINERY_EVENT_TYPE_SEND);
}
