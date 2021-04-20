#ifndef RobModel_h__
#define RobModel_h__

#include "RobDef.h"
#include "Utils/Observer.h"

class RobModel : public Observable
{
public:
	~RobModel();
	static RobModel* getInstance();
	static void destroyInstance();

	// �����б�;
	void  initTools();
	void  resetTools() { initTools(); };
	map<int, map<int, int> >*  getTools();

	// ��ѯ������Ƭ;
	map<int, int>*  getChips(const int nToolId);

	// ��ѯ��������;
	void  getToolInfo(int nTemplateId, Rob_Tool_Detail_Info& toolInfo);

	// ������������Ϣ;
	void  updateMainInfo(Rob_Main_Info* info);

	// ���¶�����Ϣ;
	void  updateEnemy(vector<ROB_PLAYER_INFO>* vcEnemy);

	// �����Ӷ��¼;
	void  updateRobRecord(vector<ROB_RECORDS_INFO>* record);

	// ������սʱ��;
	void  onCountDown(int sec);

	// �ϳɽ��;
	void  onComposeResult();

	// �Ӷ�10�ν��;
	void  updateRobTenResult(ROB_TEN_RESULT* robResult);

	// �������Ƿ�ɺϳ�;
	bool  checkComposeValid(int nToolId);

	const vector<ROB_PLAYER_INFO>& getRobEnemyInfo();

	int  getRobCostEnergy();

	void setAvoidamount(int amount)	{ m_nAvoidamount = amount; };
	int getAvoidamount() { return m_nAvoidamount; };

	void setRobFightId(int fightId) { m_nFightId = fightId; };
	int getRobFightId() { return m_nFightId; };

	// ��ѯ��ս���Ľ��;
	int  getRestCost();

	// �Ӷ����ݱ��;
	void updateRobToolsAndChips();

private:
	RobModel();

	// ����UI;
	void  updateUI(OBS_PARAM_TYPE_ROB type, void* data);

private:
	static RobModel* ms_pInstance;

	// �����Լ���Ƭ<Id, <ChipId, Count> >;
	map<int, map<int, int> >  m_mapTools;

	vector<BW_CHIPS_ITEM_INFO> m_vBwChips;

	vector<ROB_PLAYER_INFO> m_vecRobEnemyInfo;
	
	int m_nAvoidamount;
	int m_nFightId;
};

#endif // LoginController_h__
