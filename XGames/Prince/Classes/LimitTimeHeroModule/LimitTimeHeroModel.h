#ifndef LimitTimeHeroModel_h__
#define LimitTimeHeroModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "LimitTimeHeroDataDef.h"
#include "DataManager/DataManager.h"
#include "Utils/LocalTimer.h"


class LimitTimeHeroModel : public Observable, public TimeStampListener
{
public:
	static LimitTimeHeroModel* getInstance();
	static void destroyInstance();

public:

	//是否能免费抽
	bool isCanBuyFree();

	//是否有积分宝箱可以领取
	bool isCanGetScoreBox();

	//活动数据
	sActData* getActData();

	//活动排名数据
	vector<sScoreRank>* getScoreRankData();

	//活动排名奖励数据
	vector<sRankRewardData>* getRankRewardData();

	//根据id查找积分宝箱
	sRewardBox* getRewardBoxById(int boxId);

	//抽卡数据
	vector<sGetItemParam>& getBuyItemParam();

	//我的积分数据
	sScoreRank* getMyScoreData();

	//获取抽卡花费
	int getCost(eChouKaType type);

	//获取是否添加了抽道具层
	bool getIsAddBuyLayer();

	//获取活动列表
	vector<sActData>* getActList();

	//是否拉取过表格数据(只拉一次)
	bool isGettedTableData();

public:

	//活动主界面信息解析
	void initMainInfoFromSvr(Json::Value& data);

	//设置抽卡获得道具
	void setTakeCardData(Json::Value& data);

	//设置开宝匣获得道具
	void setOpenBoxData(Json::Value& data);

	//设置是否添加抽道具层
	void setIsAddBuyLayer(bool isAdd);

	//解析活动配置表
	void initActInfoFromSvr(Json::Value& data);

	//领取积分宝箱
	void getGetScoreBox(Json::Value& data);

	//计算活动开启状态
	void setActState(sActData* actData);

	//开启定时器
	void openTimer();

	//定时器1.
	void timeOut();

private:

	//设置宝箱领取数据
	void setBoxOpenState(Json::Value& data);

	//设置积分排名数据
	void setScoreRankData(Json::Value& data);

	//计算活动积分宝箱奖励
	void setScoreBoxRewardData(Json::Value& scoreData, Json::Value& rewardData, sActData* actData);

	//计算活动排名段的奖励
	void setRankRewardData(Json::Value& rankRewardList, sActData* actData);

	//通知UI工程
	void updateUI(int id );
private:
	LimitTimeHeroModel();
	virtual ~LimitTimeHeroModel();

private:

	static LimitTimeHeroModel* ms_pInstance;

	//免费抽的时间戳
	time_t m_tFreeChou;

	//当前是否能免费抽
	bool m_bIsFree;

	//是否拉取过表格
	bool m_bIsGettedTable;

	//活动数据
	sActData* m_sActData;
	//积分排名数据
	vector<sScoreRank> m_vScoreRank;
	
	//抽卡/宝匣的奖励数据
	vector<sGetItemParam> m_vItemParam;

	//我的积分
	sScoreRank m_sMyScoreRank;

	//单抽花费
	int m_iOneCost;
	//十抽花费
	int m_iTenCost;

	//是否添加了抽道具层
	bool m_bIsAddBuyLayer;

	//活动列表
	vector<sActData> m_vActList;
};

#endif