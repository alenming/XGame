#ifndef TacticsController_h__
#define TacticsController_h__

#include "Communication/ICommListener.h"
#include "TacticsDataDef.h"

class TacticsController : public ICommListener
{
public:
	TacticsController();
	virtual ~TacticsController();

public:

	static TacticsController* getInstance();
	static void destroyInstance();

	virtual void onRecvMessage(int nCmdID, const char* szData);
	
	//发送获取阵法信息消息
	static void sendGetTacticsListMsg();
	
	//发送使用阵法
	static void sendUseTacticsMsg(int matrixId);

	//发送阵法精炼
	static void sendUpgradeTacticsMsg(int matrixId);

	//发送排强化
	static void sendUpgradeRowMsg(vector<sRowUpMaterial> vecItemList, int rowId);

private:
	static TacticsController* ms_pInstance;

	//解析获取的阵法信息
	void parseTacticsMsg(const char* szData);

	//解析使用阵法
	void parseUseTacticsMsg(const char* szData);

	//解析阵法精炼
	void parseUpgradeTacticsMsg(const char* szData);

	//解析排强化
	void parseUpgradeRowMsg(const char* szData);
};

#endif