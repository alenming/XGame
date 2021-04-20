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

	// 道具列表;
	void  initTools();
	void  resetTools() { initTools(); };
	map<int, map<int, int> >*  getTools();

	// 查询道具碎片;
	map<int, int>*  getChips(const int nToolId);

	// 查询道具详情;
	void  getToolInfo(int nTemplateId, Rob_Tool_Detail_Info& toolInfo);

	// 更新主界面信息;
	void  updateMainInfo(Rob_Main_Info* info);

	// 更新对手信息;
	void  updateEnemy(vector<ROB_PLAYER_INFO>* vcEnemy);

	// 更新掠夺记录;
	void  updateRobRecord(vector<ROB_RECORDS_INFO>* record);

	// 更新免战时间;
	void  onCountDown(int sec);

	// 合成结果;
	void  onComposeResult();

	// 掠夺10次结果;
	void  updateRobTenResult(ROB_TEN_RESULT* robResult);

	// 检查道具是否可合成;
	bool  checkComposeValid(int nToolId);

	const vector<ROB_PLAYER_INFO>& getRobEnemyInfo();

	int  getRobCostEnergy();

	void setAvoidamount(int amount)	{ m_nAvoidamount = amount; };
	int getAvoidamount() { return m_nAvoidamount; };

	void setRobFightId(int fightId) { m_nFightId = fightId; };
	int getRobFightId() { return m_nFightId; };

	// 查询免战消耗金币;
	int  getRestCost();

	// 掠夺数据变更;
	void updateRobToolsAndChips();

private:
	RobModel();

	// 更新UI;
	void  updateUI(OBS_PARAM_TYPE_ROB type, void* data);

private:
	static RobModel* ms_pInstance;

	// 宝物以及碎片<Id, <ChipId, Count> >;
	map<int, map<int, int> >  m_mapTools;

	vector<BW_CHIPS_ITEM_INFO> m_vBwChips;

	vector<ROB_PLAYER_INFO> m_vecRobEnemyInfo;
	
	int m_nAvoidamount;
	int m_nFightId;
};

#endif // LoginController_h__
