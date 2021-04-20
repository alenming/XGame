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

	//����ս����Ϣ
	void onRecvMessage(int nCommandID, const char* szData);

	//����PVEս����Ϣ����������
	void sendPVEFightMsg(int nBarrierID);
	//��������ս����Ϣ
	void sendCommonFightMsg(int nFightID);
	//���Ͱ���bossս����Ϣ
	void sendGroupBossFightMsg();
	//��������bossս����Ϣ
	void sendBossFightMsg(int bossId);
	//����PVPս����Ϣ
	void sendPVPFightMsg(int enemyOrderNum,int myOrderNum,int enemyId);
	//�����Ӷ�ս����Ϣ
	void sendRobFightMsg(int index);
	//��������ս����Ϣ
	void sendTrailFightMsg(int nOpLv);
	//��������ս����Ϣ
	void sendTowerFightMsg(int nBarrierId);
	// ���ͻ�ؿ�ս����Ϣ;
	void sendActivityFightMsg(int nBarrierId);

	//����bossս������Ϣ
	void setBossFightSettlement(int startCount, int bossId, int totalDam, FIGHT_TYPE type, SceneFight* scene = nullptr);
	//ս������	
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
