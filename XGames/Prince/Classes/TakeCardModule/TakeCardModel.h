#ifndef TakeCardModel_h__
#define TakeCardModel_h__

#include "../DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "TakeCardDef.h"
class TakeCardModel:public Observable,public TimeLocalListener
{
public:
	static TakeCardModel* getInstance();	
	static void destroyInstance();

	//通知监听者
	void notify(int eventType, void* data = nullptr);
	
	//计时通知 param: 时间间隔
	virtual void timeTick(int dt);

	void setLocalTime();
private:

	TakeCardModel();
	~TakeCardModel();
public:	
	//抽卡
	void parseGetTakeCardInfoData(const Json::Value &data);
	void parseTakeCardResultData(const Json::Value &data);

	void updateTakeCardInfo(const TAKE_CARD_INFO &takeCardInfo);
	const TAKE_CARD_INFO &getTakeCardInfo();

	vector<NEW_CARD_INFO>& gettakeCardResulHeros();

	CC_SYNTHESIZE(bool,FirstGet,FirstGet);//是否首次十连抽抽到13资质英雄
	CC_SYNTHESIZE(bool,CardInfo,CardInfo);//是否进主界面获取抽卡信息

	CC_SYNTHESIZE(int,coinCardSeconds,coinCardSeconds);//是否进主界面获取抽卡信息
	CC_SYNTHESIZE(int,goldCardSeconds,goldCardSeconds);//是否进主界面获取抽卡信息

	CC_SYNTHESIZE(int,loaded,loaded);//是否拉取过信息

	CC_SYNTHESIZE(int,haveFreeTakeCard,haveFreeTakeCard);//有免费抽卡
	CC_SYNTHESIZE(int,haveTakeCardOrder,haveTakeCardOrder);//有免费抽卡


	void setIsHaveFreeCard();

private:
	TAKE_CARD_INFO  m_TakeCardInfo;

	vector<NEW_CARD_INFO> m_vectakeCardResultHeros;
	static TakeCardModel* ms_pInstance;
};

#endif
