/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LoveModel
 *  Description:	情缘数据模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/23/2015
 *  Others:			
 *
 ******************************************************************/
#include "LoveHeader.h"


#ifndef LoveModel_h_
#define LoveModel_h_


const int LUZHISHEN = 41001;

class LoveModel:public Observable
{
public:
	~LoveModel();

	//活动单例
	static LoveModel* getInstance();
	static void destroyInstance();

	//通知监听者
	void notify(LOVE_EVENT_TYPE eventType, void* data = nullptr);

	void parseHeroListInfoData(const Json::Value &data); //解析英雄列表数据
	void parseHeroSendGiftData(const Json::Value &data); //解析送礼数据
	void parseHeroBuyGiftData(const Json::Value &data);  //解析买礼
	void parseHeroFingerGuessData(const Json::Value &data);  //解析猜拳
	void parseHeroPlayMapData(const Json::Value &data); //解析游玩地图
	void parseHeroChooseMapData(const Json::Value &data); //解析选择地图信息
	void parseHeroChooseMapEvent(const Json::Value &data);//解析选择事件
	void parseGuessCountInfo(const Json::Value &data);//解析购买猜拳信息
	void parseGuessCount(const Json::Value &data);//解析购买猜拳次数
	void parseAddAttribute(const Json::Value &data);//解析增加属性点
	void parseResetAttribute(const Json::Value &data);//解析重置属性点
	void parseChangeGuess(const Json::Value &data);//解析修改猜拳结果
	void parseGetGuessPrize(const Json::Value &data); //解析领取奖励

	LOVE_HERO_INFO_VEC* getLoveHeroInfoVec() { return &m_heroInfoVec; };
	LOVE_HERO_INFO* getHeroByHeroId(int petId);
	LOVE_GIFT_INFO_VEC* getLoveGiftInfoVec() { return &m_giftInfoVec; };
	SEND_GIFT_REWARD_VEC* getLoveGiftReeardVec() { return &m_reward; };
	SEND_GIFT_REWARD_VEC* getExtraReward() {return &m_extraReward;};

	PLAY_MAP_VEC* getPlayMap() {return &m_playMap;};
	int& getplayMapStepCount() {return m_playMapStepCount;};
	PLAY_MAP_NODE_INFO_VEC* getPlayMapNode() {return &m_playMapNode;};
	vector<int>* getPlayStepChange() {return &m_playStepChange;};
	SEND_GIFT_REWARD_VEC* getBoxItem() {return &m_boxItem;}; //游玩宝箱
	int getCurCHooseXy() {return curCHooseXy;};
	int getPlayFightId() {return m_playFightId;}; //战斗id
	SEND_GIFT_REWARD_VEC* getPlayRewardItem() {return &m_playRewardItem;};
	int& getPlayFlag() {return m_PlayFlag;};
	bool& getPlayMapBack() {return isPlayMapBack;};

	const int& getAllPlayCount() {return m_playCount;};
	const int& getGuessCount() {return m_guessCount;};

	vector<int>* getGoodWillAddVec() {return &m_goodWillAdd;};
	vector<int>* getFingerGoodWillAddVec() {return &m_fingerGoodWillAdd;};
	int  getFingerOutCome() {return m_FingerOutCome;};

	//解锁新英雄
	void addNewPetId(int petId,bool isBarrier = false);
	void removeNewPetId(int petId);
	bool findNewPetId(int petId);
	bool isNewPetIdEmpty() {return m_newPetId.empty();};

	int& getCurOpenLoveHeroPetId() {return m_curOpenLoveHeroPetId;}; //获取当前解锁英雄id
	bool& getIsBarrier() {return m_isBarrier;};
	int getBuyGuessPrice() {return m_buyGuessPrice;};
	int getChangeGuessPrice() {return m_changeGuessPrice;};
	int getBuyGuessCurCount() {return m_buyGuessCurCount;};
	int getBuyGuessMaxCount() {return m_buyGuessMaxCount;};

private:

	static LoveModel* mInstance;

	LoveModel();

	LOVE_HERO_INFO_VEC m_heroInfoVec; //英雄信息 

	LOVE_GIFT_INFO_VEC m_giftInfoVec; //礼物信息

	SEND_GIFT_REWARD_VEC m_reward; //送礼升级反馈

	SEND_GIFT_REWARD_VEC m_extraReward; //有概率出现的额外奖励

	PLAY_MAP_VEC m_playMap; //游玩地图
	int m_playMapStepCount;
	vector<int>  m_playStepChange; //步数增加
	SEND_GIFT_REWARD_VEC m_boxItem; //游玩宝箱
	int  m_playFightId; //战斗id
	SEND_GIFT_REWARD_VEC m_playRewardItem; //游玩总奖励
	PLAY_MAP_NODE_INFO_VEC m_playMapNode; //地图节点信息
	int curCHooseXy;//当前寻找的点
	int m_PlayFlag;
	bool isPlayMapBack;  //是否是playMap返回到情缘界面

	vector<int>  m_newPetId;
	int  m_curOpenLoveHeroPetId;//当前解锁的情缘英雄
	bool m_isBarrier;//是否是副本开启

	int  m_playCount; //总的剩余游玩次数
	int  m_guessCount; //总的剩余猜拳次数

	vector<int>  m_goodWillAdd; //送礼增加好感

	vector<int>  m_fingerGoodWillAdd; //送礼增加好感

	int  m_FingerOutCome; //猜拳输赢
	int  m_buyGuessPrice; //购买猜拳价格
	int  m_changeGuessPrice; //修改猜拳价格
	int  m_buyGuessCurCount; //购买猜拳当前次数
	int  m_buyGuessMaxCount; //购买猜拳总次数

};


#endif