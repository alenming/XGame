#include "TakeCardModel.h"


#include "Common/Common.h"
#include "TakeCardDef.h"
#include "Utils/StringFormat.h"
#include "cocos2d.h"
using namespace cocos2d;

TakeCardModel*	TakeCardModel::ms_pInstance	= nullptr;

TakeCardModel::TakeCardModel()
{
	memset(&m_TakeCardInfo, 0, sizeof(TAKE_CARD_INFO));
	FirstGet = false;
	CardInfo = false;
	loaded = false;
	goldCardSeconds = 0; //金币抽卡CD倒计时
	coinCardSeconds = 0; //银币抽卡CD倒计时
	haveFreeTakeCard = false;
	haveTakeCardOrder = false;
}	

TakeCardModel* TakeCardModel::getInstance()
{
	if (nullptr == ms_pInstance)
	{
		ms_pInstance = new TakeCardModel();		
	}
	return ms_pInstance;
}

TakeCardModel::~TakeCardModel()
{
	LocalTimer::getInstance()->removeLocalTimerListener(this);
}

void TakeCardModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

void TakeCardModel::notify(int eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void TakeCardModel::parseGetTakeCardInfoData(const Json::Value &data)
{
	loaded = true;

	memset(&m_TakeCardInfo, 0, sizeof(TAKE_CARD_INFO));
	//coindraw
	Json::Value coindraw			= data["coindraw"];
	coinCardSeconds = coindraw["seconds"].asInt();
	m_TakeCardInfo.coinItem.onecost		= coindraw["onecost"].asInt();
	m_TakeCardInfo.coinItem.tencost		= coindraw["tencost"].asInt();
	m_TakeCardInfo.coinItem.times			= coindraw["times"].asInt();
	m_TakeCardInfo.coinItem.limitCount    = 5;
	m_TakeCardInfo.coinItem.CurCount      = coindraw["freecount"].asInt();
	m_TakeCardInfo.coinItem.firstFlag     = coindraw["firstflag"].asInt();
	m_TakeCardInfo.coinItem.tenFlag       = coindraw["tenflag"].asBool();
	//golddraw
	Json::Value golddraw			= data["golddraw"];
	goldCardSeconds = golddraw["seconds"].asInt();
	m_TakeCardInfo.goldItem.onecost		= golddraw["onecost"].asInt();
	m_TakeCardInfo.goldItem.tencost		= golddraw["tencost"].asInt();
	m_TakeCardInfo.goldItem.times			= golddraw["times"].asInt();
	m_TakeCardInfo.goldItem.tenFlag       = golddraw["tenflag"].asBool();

	FirstGet = m_TakeCardInfo.goldItem.tenFlag;//显示是否十连抽

	setLocalTime();
	notify(TakeCardNotify_GetInfo);
}

void TakeCardModel::parseTakeCardResultData(const Json::Value &data)
{
	Json::Value changeInfo = data["changeInfo"];
	MainModel::getInstance()->updateData(changeInfo);
	//新增的英雄数据	
	m_vectakeCardResultHeros.clear();
	Json::Value newpetLists = data["newItemList"];//#抽到的道具列表 id:xx,count:xx
	for (size_t i = 0; i < newpetLists.size(); i++)
	{
		NEW_CARD_INFO newCard;
		newCard.id = newpetLists[i]["id"].asInt();
		newCard.amount = newpetLists[i]["amount"].asInt();
		newCard.countChips = newpetLists[i]["count2"].asInt();
		m_vectakeCardResultHeros.push_back(newCard);
	}

	//随机打乱顺序
	random_shuffle(m_vectakeCardResultHeros.begin(),m_vectakeCardResultHeros.end());

	//英雄数据
	Json::Value petLists = changeInfo["petList"];
	HeroModel::getInstance()->updateData(petLists);
	//
	Json::Value items = changeInfo["itemList"];
	ToolModel::getInstance()->updateToolJsonData(items);		

	TAKE_CARD_INFO cardInfo;
	memset(&cardInfo, 0, sizeof(TAKE_CARD_INFO));
	const TAKE_CARD_INFO& _cardInfo = m_TakeCardInfo;

	coinCardSeconds = data["coinSeconds"].asInt();
	cardInfo.coinItem.onecost		= _cardInfo.coinItem.onecost;
	cardInfo.coinItem.tencost		= _cardInfo.coinItem.tencost;
	cardInfo.coinItem.times			= data["coinleftTimes"].asInt();
	cardInfo.coinItem.limitCount    = _cardInfo.coinItem.limitCount;
	cardInfo.coinItem.CurCount      = data["freecount"].asInt();
	cardInfo.coinItem.firstFlag     = data["firstflag"].asInt();
	//golddraw
	goldCardSeconds = data["goldSeconds"].asInt();
	cardInfo.goldItem.onecost		= _cardInfo.goldItem.onecost;
	cardInfo.goldItem.tencost		= _cardInfo.goldItem.tencost;
	cardInfo.goldItem.times			= data["goldleftTimes"].asInt();
	cardInfo.goldItem.tenFlag       = data["goldtenflag"].asBool();
	FirstGet = cardInfo.goldItem.tenFlag;

	m_TakeCardInfo = cardInfo;

	setLocalTime();
	notify(TakeCardNotify_GetCard);
}

void TakeCardModel::updateTakeCardInfo(const TAKE_CARD_INFO &takeCardInfo)
{
	m_TakeCardInfo = takeCardInfo;
}
const TAKE_CARD_INFO &TakeCardModel::getTakeCardInfo()
{
	return m_TakeCardInfo;
}

vector<NEW_CARD_INFO>& TakeCardModel::gettakeCardResulHeros()
{
	return m_vectakeCardResultHeros;
}

void TakeCardModel::setLocalTime()
{
	if (coinCardSeconds > 0 || goldCardSeconds > 0)
	{
		setEndTimeStamp((coinCardSeconds > goldCardSeconds ? coinCardSeconds : goldCardSeconds) + 
			LocalTimer::getInstance()->getCurServerTime());

		LocalTimer::getInstance()->addLocalTimerListener(this);
	}
	
	setIsHaveFreeCard();
}

void TakeCardModel::timeTick(int dt)
{
	//通知主界面是否有提示
	if (coinCardSeconds <= 0 || goldCardSeconds <= 0)
	{
		setIsHaveFreeCard();
	}

	if (coinCardSeconds <= 0 && goldCardSeconds <= 0)
	{
		coinCardSeconds = 0;
		goldCardSeconds = 0;

		//停止监听器
		setEndTimeStamp(0);
	}
	//银币倒计时
	if (coinCardSeconds >= dt)
	{
		coinCardSeconds -= dt;
	}

	//金币倒计时
	if (goldCardSeconds >= dt)
	{
		goldCardSeconds -= dt;
	}

	notify(TakeCardNotify_Timer);
}

void TakeCardModel::setIsHaveFreeCard()
{
	haveFreeTakeCard = false;
	if ((coinCardSeconds <= 0 && m_TakeCardInfo.coinItem.CurCount > 0) || goldCardSeconds <= 0)
	{
		haveFreeTakeCard = true;
		notify(TakeCardNotify_BuildIng);		
	}
}

