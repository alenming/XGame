#ifndef FightController_h__
#define FightController_h__

#include "../Communication/ICommListener.h"
#include "../Communication/Json/json.h"

#include "SceneFight.h"

class FightController: public ICommListener
{

public:

	FightController();
	virtual ~FightController();

public:

	static FightController*	getInstance();

	static void destroyInstance();

	//接受战斗消息
	void onRecvMessage(int nCommandID, const char* szData);

	//发送PVE战斗消息，包含阵形
	void sendPVEFightMsg(int nBarrierID);
	//发送其他战斗信息
	void sendCommonFightMsg(int nFightID);
	//发送帮派boss战斗消息
	void sendGroupBossFightMsg();
	//发送世界boss战斗消息
	void sendBossFightMsg(int bossId);
	//发送PVP战斗消息
	void sendPVPFightMsg(int enemyOrderNum,int myOrderNum,int enemyId);
	//发送掠夺战斗消息
	void sendRobFightMsg(int index);
	//发送试练战斗消息
	void sendTrailFightMsg(int nOpLv);
	//发送爬塔战斗消息
	void sendTowerFightMsg(int nBarrierId);
	// 发送活动关卡战斗消息;
	void sendActivityFightMsg(int nBarrierId);

	//发送boss战结算消息
	void setBossFightSettlement(int startCount, int bossId, int totalDam, FIGHT_TYPE type, SceneFight* scene = nullptr);
	//战斗结算	
	void handleSettlement(FIGHT_TYPE nFightType, int nBarrierId, int nFlag, SceneFight* pScene);

protected:

	void parseFightData(const char* szData);

	bool parseHeroList(const Json::Value& data);
	bool praseMonsterList(const Json::Value& data);
	bool parseAssistList(const Json::Value& data);
	bool parseFighter(const Json::Value& data, Fighter* fighter);

	void parseSettlementConfirm(const char* szData);
	void parsePvpSettlementConfirm(const char* szData);

	void parseTowerSettlement(const char* szData);
	void parseTrailSettlement(const char* szData);

	void parseRobSettlement(const char* szData);

	void parseBossSettlement(const char* szData);
	void parseGroupBossSettlement(const char* szData);
	

	//virtual bool throwException(const Json::Value& root)

private:

	static FightController* ms_pInstance;

	SceneFight*	m_pSceneFight;
};

#endif // FightController_h__
