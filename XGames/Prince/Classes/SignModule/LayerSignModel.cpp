#include "LayerSignModel.h"

LayerSignModel* LayerSignModel::mInstance = nullptr;


LayerSignModel::LayerSignModel(void)
{
	isLoad = false;
	mServerTime = LocalTimer::getInstance()->getCurServerTime();
}


LayerSignModel::~LayerSignModel(void)
{
}

LayerSignModel* LayerSignModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new LayerSignModel();
	}
	return mInstance;
}

void LayerSignModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void LayerSignModel::parseSignData(const Json::Value &val)
{
	isLoad = true;
	mLayerSignParam.clear();

	Json::Value rewardItem = val["rewardItem"];
	Json::Value rewardList = val["rewardList"];

	showHeroId = val["showHeroId"].asInt();
	showHeroCount = val["showHeroAmount"].asInt();
	prize = val["price"].asInt();

	for (int i = 0; i < rewardItem.size(); i++)
	{
		LayerSignParam* signParam = LayerSignParam::create();

		signParam->signCondition = i+1;
		if (rewardItem[i]["templateId"].asInt() > 0)
		{
			SignRewardInfo signRewardInfo;
			signRewardInfo.mItemId = rewardItem[i]["templateId"].asInt();
			signRewardInfo.mItemCount = rewardItem[i]["amount"].asInt();
			signParam->signReward = signRewardInfo;
		}

		signParam->vipLevel = rewardItem[i]["vipLevel"].asInt();
		signParam->vipDouble = rewardItem[i]["vipDouble"].asInt();
		signParam->rewardBet = rewardItem[i]["rewardBet"].asInt();
		signParam->stat = (SIGN_STAT_TYPE)rewardItem[i]["stat"].asInt();

		mLayerSignParam.pushBack(signParam);
	}

	mSignState.day = rewardList["step"].asInt();
	mSignState.month = rewardList["moon"].asInt();

	//通知初始化
	notify(SIGN_EVENT_INIT);
}


void LayerSignModel::parseSignGetData(const Json::Value &val)
{
	Json::Value changeInfo = val["changeInfo"];
	Json::Value rewardList = val["rewardList"];
	prize = val["price"].asInt();

	petId = val["petId"].asInt();

	rewardShow.mCoin = changeInfo["coin"].asInt() - MainModel::getInstance()->getMainParam()->mCoin;
	rewardShow.mGold = val["addgold"].asInt();
	rewardShow.mVecRewardInfo.clear();

	//道具数据
	vector<TOOL_NEW_INFO> vecNewTools;
	Json::Value items = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(items, nullptr, &vecNewTools);

	for (int i = 0; i < vecNewTools.size(); i++)
	{
		SignRewardInfo itemIfo;
		itemIfo.mItemId = vecNewTools[i].ntemplateId;
		itemIfo.mItemCount = vecNewTools[i].nstack;

		rewardShow.mVecRewardInfo.push_back(itemIfo);
	}

	//主数据
	MainModel::getInstance()->updateData(changeInfo);

	//英雄数据
	Json::Value petLists = changeInfo["petList"];
	HeroModel::getInstance()->updateData(petLists);

	mSignState.day = rewardList["step"].asInt();
	mSignState.month = rewardList["moon"].asInt();

	signIdx1 = -1;
	signIdx2 = -1;
	for (int i = 0; i < mLayerSignParam.size(); i++)
	{
		if (val["num"].asInt() == i)
		{
			signIdx1 = val["num"].asInt();
			mLayerSignParam.at(i)->stat = (SIGN_STAT_TYPE)val["signInfo"]["stat"].asInt();
		}

		if (val["num2"].asInt() == i)
		{
			signIdx2 = val["num2"].asInt();
			mLayerSignParam.at(i)->stat = (SIGN_STAT_TYPE)val["signInfo2"]["stat"].asInt();
		}
	}

	//通知初始化
	notify(SIGN_EVENT_RECEIVE);
}


void LayerSignModel::notify(SIGN_EVENT_TYPE eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}
