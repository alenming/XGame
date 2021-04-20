#ifndef SmithyController_h__
#define SmithyController_h__

#include "SmithyDef.h"
#include "Communication/ICommListener.h"

class SmithyController: public ICommListener
{
public:

	SmithyController();
	~SmithyController();

	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);

public:

	// 发起强化;
	int handleQH(const int nToolId, const int nTimes);
	int handleQH_HorseBook(const int nToolId, vector<int>* vcMaterialTemplateIds);

	// 发起进阶;
	int handleJJ(const int nToolId, int nGold = 0);

	// 淬炼;
	int handleCL(const int nToolId);
	int handleCLDegrade(const int nToolId, int nCostGold);

	// 发起洗练;
	//int handleXL(const int nToolId, SMITHY_XL_TYPE type, int nTimes = 1);
	//int handleXLReplace(const int nToolId);

private:

	// 道具强化;
	void processEquipQHData(const char* szData);

	// 道具进阶;
	void processEquipJJData(const char* szData);

	// 道具淬炼结果;
	void processEquipCLData(const char* szData);

	//装备洗炼
	//void processEquipWashData(const char* szData);

	//替换洗炼结果
	//void processEquipWashReplaceResultData(const char* szData);

private:
	//SMITHY_XL_TYPE  m_washType;
	//int  m_washTimes;

	// 淬炼类型(淬炼/淬炼降级);
	OBS_PARAM_TYPE_SM  m_CurCLType;
};

#endif // LoginController_h__
