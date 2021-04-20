/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		TowerController
 *  Description:	爬塔模块控制器;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			11/21/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef TowerController_h__
#define TowerController_h__

#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"
#include "TowerDef.h"

class TowerController : public Ref, public ICommListener
{
public:
	~TowerController(void);

	static TowerController* getInstance();
	static void destroyInstance();

	virtual void  onRecvMessage(int nCmdID, const char* szData);

	// (试炼)请求主界面信息;
	void  handleTailMainInfo();

	// (试炼/爬塔)查询排行榜数据;
	void  handleRanking(TOWER_TYPE nType);

	// (试炼)查询奖励;
	bool  handleTrailReward();

	// (试炼/爬塔)校验当前楼层;
	void  checkCurFloor(TOWER_TYPE nType);

	// (试炼)请求buff页内容;
	void  handleBuffContent();

	// (试炼)兑换buff;
	void  handleBuyBuff(int nBuffId, int nHeroId = -1);

	// (试炼)关闭buff页面(判定是否领取完成);
	bool  handleCloseBuff();

	// (试炼)关闭奖励页面(判定是否触发宝藏);
	bool  handleRewardClose();

	// (试炼)开启宝藏;
	bool  handleOpenTreasure();

	// (试炼)刷新宝藏;
	int  handleRefreshTreasure();

	// (试炼)重置试炼;
	//bool  handlePreResetTrail();
	//void  handleResetTrail();

	// (试炼)获取对手;
	void  handleGetTrailOp();

	// (试炼)关闭宝藏页面;
	void  handleTreasureClose();

	// (爬塔)请求爬塔主界面信息;
	void  handleTowerMainInfo();

	// (爬塔)重置爬塔;
	bool  handlePreResetTower();
	bool  handlePreResetTower2();
	void  handleResetTower();

	// (爬塔)购买挑战次数;
	//int  handleBuyChallengeTimes();

	// (爬塔)进入战斗;
	//int   handleTowerFight();

	// (爬塔)奖励预览;
	void  handleTowerReward();

	// (爬塔)扫荡;
	void  handleTowerSweep();
	void  handleTowerSweepSettlement();

	// (扫荡)倒计时;
	virtual void  refreshCountDown(float delta);

	// (爬塔)退出主界面，不在Model倒计时了，直到下次再拉取;
	void  onTowerViewExit();

	// (爬塔)检查是否有未显示的扫荡结果;
	void  checkUISweepSettlement(bool bShown = false);

private:
	TowerController(void);

	// 解析试炼主界面信息;
	void  procTrailMainInfo(const char* szData);

	// 解析试炼阵型;
	void  procTrailFormation(const char* szData);

	// 试炼奖励内容;
	void  procTrailReward(const char* szData);

	// 重置试炼;
	//void  procResetTrail(const char* szData);

	// 获取试炼对手;
	void  procGetTrailOp(const char* szData);

	// 试炼开启宝藏的返回结果;
	void  procOpenTreasure(const char* szData);

	// 试炼刷新宝藏的返回结果;
	void  procRefreshTreasure(const char* szData);

	// 兑换buff的返回结果;
	void  procBuyBuff(const char* szData);

	// 爬塔/试炼排行榜;
	void  procRanking(int nCmd, const char* szData);

	// 爬塔主界面信息;
	void  procTowerMainInfo(const char* szData);

	// 重置爬塔;
	void  procResetTower(const char* szData);

	// (爬塔)解析爬塔本地配置信息;
	void  parseTowerLocalInfo();

	// (爬塔)购买挑战次数;
	//void  procBuyChallengeTimes(const char* szData);

	// (爬塔)扫荡请求回包;
	void  procTowerSweep(const char* szData);

	// (爬塔)扫荡结果单次;
	void  procTowerSweepSettlement(const char* szData);

	// 数据变化;
	void  procChangeInfo(const Json::Value& changeInfo);

private:

	static TowerController*  m_pInstance;

	// 当前时间戳;
	int  m_nStamp;

	// 扫荡结果;
	TowerSweepSettlement  m_towerSweepSettlement;
};


#endif // TowerController_h__