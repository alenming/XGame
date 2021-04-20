#ifndef PlayerInfoModel_h__
#define PlayerInfoModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "ToolModule/ToolModel.h"

#define UPDATE_POWER_TIME		101			//体力恢复时间变化
#define UPDATE_ENERGY_TIME		102			//精力恢复时间变化
#define POWER_COUNT_ADD			103			//体力+1
#define ENERGY_COUNT_ADD		104			//精力+1


class PlayerInfoModel : public Observable
{
public:

	static PlayerInfoModel* getInstance();
	static void destroyInstance();

	vector<TOOL_NEW_INFO>* getNewTools();

private:
	PlayerInfoModel();
	virtual ~PlayerInfoModel();

private:

	static PlayerInfoModel* ms_pInstance;

	vector<TOOL_NEW_INFO> m_vecNewTools;
};


#endif