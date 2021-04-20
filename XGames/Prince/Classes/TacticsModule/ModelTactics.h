#ifndef ModelTactics_h__
#define ModelTactics_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "TacticsDataDef.h"

class ModelTactics : public Observable
{
public:

	static ModelTactics* getInstance();
	static void destroyInstance();

	//数据是否从服务端初始化
	bool  isInittedFromSvr();

	//排数据初始化
	void initRowData();

	//阵法数据初始化
	void initTacticsListData();

	//解析排材料消耗的银币数据
	void parseRowMaterialCost();

	//服务端数据初始化
	void initFromSvr(const Json::Value& tacData);

	//服务器回馈阵法精炼
	void updateTacticsList(const Json::Value& tacData);

	//服务器回馈使用阵法
	void updateUsedTactics(const Json::Value& tacData);

	//服务器回馈的排强化
	void updateRowData(const Json::Value& rowData);

	//排强化后，重置排数据
	void reSetRowData(int rowId, int rowIndex);

	//排升级后，检测是否有新阵法被激活
	void updateActivedNewTactics();

	//阵法精炼后刷新UI
	void reflushUIAfterUpTactics();

	//获取各排当前数据
	sCurRowData& getCurRowData(eRowType type);

	//获取阵法列表
	vector<sTacticsData>& getTacticsDataList();

	//获取当前已激活的阵法列表
	vector<sTacticsData>& getActedTacticsDataList();

	//获取当前使用的阵法
	sTacticsData& getCurUsingTacticsData();

	//当前是否有阵法正在使用
	bool isUsedTactics();

	//获取正在使用的阵法属性描述，如没有使用阵法返回“无”
	string getCurUsingTacticsAttribute();

	//获取排材料价格
	int getRowUpMaterialPrice(int id);

private:
	ModelTactics();
	virtual ~ModelTactics();

	//设置表格关键字和属性中文名
	void setTypeStrKey(ATTR_TYPE type, sAttriName& attName);

public:
	vector<sRowUpMaterialPrice> m_vRowMaterialCostCoin;

private:

	static ModelTactics* ms_pInstance;

	//各排当前数据
	vector<sCurRowData> m_vCurRowData;

	//阵法列表
	vector <sTacticsData> m_vTacticsDataList;

	//当前拥有的阵法数据
	vector<sTacticsData> m_vCurTacticsDataList;

	//当前是否有阵法正在使用
	bool m_bIsUsedTactics;

	//当前使用的阵法
	sTacticsData m_curUsingTacticsData;

	//是否从服务端初始化了数据
	bool m_bIsInit;
};

#endif