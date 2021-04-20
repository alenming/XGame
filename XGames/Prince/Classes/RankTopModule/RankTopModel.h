#ifndef RankTopModel_h__
#define RankTopModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "RankTopDef.h"

class RankTopModel : public Observable
{
public:

	static RankTopModel* getInstance();
	static void destroyInstance();

	//服务端数据初始化
	void initFromSvr(const Json::Value& rankTop);
	
	vector<sRankTopInfo>& getRankTopData(rankTopType rankType);

	sMyRankTopInfo& getMyRankInfo();

	bool& getIsGettedLevelRank() { return mIsGetLevelRank; };

	bool& getIsGettedPowerRank() { return mIsGetPowerRank; }; 

	//设置当前要显示的排行榜类型
	void setCurRankTopType(rankTopType type);

	//获取当前要显示的排行榜类型
	rankTopType getCurRankTopType();

private:
	RankTopModel();
	virtual ~RankTopModel();

private:

	static RankTopModel* ms_pInstance;

	vector<sRankTopInfo> m_vLevelRankTop;
	vector<sRankTopInfo> m_vPowerRankTop;
	vector<sRankTopInfo> m_vPvpRankTop;
	vector<sRankTopInfo> m_vGroupRankTop;

	sMyRankTopInfo m_myRankInfo;

	//是否拉取过等级榜
	bool mIsGetLevelRank;

	//是否拉取过战力榜
	bool mIsGetPowerRank;

	//当前要显示的排行榜类型
	rankTopType m_curRankTopType;
};

#endif