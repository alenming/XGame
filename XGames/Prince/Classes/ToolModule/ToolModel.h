#ifndef ToolModel_h__
#define ToolModel_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "../HeroModule/HeroModel.h"
#include "../DataManager/DataManager.h"

const int SHOP_GOLG_SYSTEM_ID = 10062;
const int SHOP_COIN_SYSTEM_ID = 10063;
const int COMMON_SHOP_KEY_SYSTEM_ID = 10052;

struct ToolModelData
{
	RowData* pRowData;
	TOOL_INFO* pToolInfo;
};
using namespace cocos2d;
using namespace cocos2d::ui;
class ToolModel: public Observable
{

public:

	ToolModel();
	~ToolModel();

	static ToolModel* getInstance();
	static void destroyInstance();

public:

	//背包是否可用
	bool backpackIsUsable();
	int getBackpackTotleCount();
	int getBackpackUsedCount();
	//
	void processToolJsonData( const Json::Value& jsonValue );
	void updateToolJsonData( const Json::Value& jsonValue, vector<TOOL_CHANGE_INFO>* vecUpdateTools=nullptr, vector<TOOL_NEW_INFO>* vecNewTools=nullptr );
	//
	void addToolInfo( const int& id, TOOL_INFO& info, const int& ncount=1 );
	void updateToolInfo( const int& id, TOOL_INFO& info, vector<TOOL_CHANGE_INFO>* vecUpdateTools=nullptr, vector<TOOL_NEW_INFO>* vecNewTools=nullptr );
	void deleteToolInfo( const int& id, const int& ncount=1 );

	const TOOL_INFO* GetToolInfo(const int& id);
	const TOOL_INFO* GetToolInfoBytemplateId(const int& ntemplateId);
	const int getToolTemplateId(const int& nId);
	const int getToolResidByTemplateId(const int& ntemplateId);
	const int getToolResidById(const int& nId);
	
	const TOOL_INFO_MAP& GetAllToolInfo()
	{
		return m_mapToolInfo;
	}
	void removeTool(int id);
	void clearToolInfo();

	const string GetToolImgFilename(const int& ntemplateId);
	const string GetToolImgFilenameById(const int& id);
	const string GetToolImgFilenameByResId(const int& resId);

	void sortAllTools(std::vector<TOOL_INFO*>& vecTools);
	void sortConsumeTools(std::vector<TOOL_INFO*>& vecTools);
	void sortEquipTools(std::vector<TOOL_INFO*>& vecTools);
	void sortHeroToolsChip(std::vector<TOOL_INFO*>& vecTools);
	void sortMaterialTools(std::vector<TOOL_INFO*>& vecTools);

	void updateBackpackUsedCount(const int& usedCount, const int& totleCount);
	
	//装备是否可以进阶
	bool equipCanAdv(const int& toolId);
	//装备进阶材料是否达到进阶数量
	bool equipAdvMatNumIsFull(const string& matStr, const string& amountStr, RowData *pEquipment);

	void setIsInit(bool isInit);

	bool isInit();

	//查询碎片当前数量
	int searchHeroDebrisNum(int heroTempletId);

	//查询碎片合成英雄所需数量
	int searchHeroDebrisMaxNum(int heroTempletId);

	//查询是否有指定类型的可装备道具，返回道具id
	int searchPutOnEquip(EQUIP_ITEM_TYPE equipType);

	//查询指定templetId的可穿戴装备数量
	int searchPutOnEquipNum(int templetId);

	//查询指定道具数量
	int searchNumByTemplateId(int templateId);

	//获取装备的排序优先级
	int getEquipPRI(int equipType);

	//获取使用的宝箱信息
	vector<sBOX_TOOL_INFO>& getUsedBoxInfo();
	//设置使用的宝箱信息
	void setUsedBoxInfo(const Json::Value& jsonValue);
	//清空宝箱容器
	void clearBoxInfo();

	//是否使用了宝箱
	bool getIsUsedBox();
	void setIsUsedBox(bool isUsedBox);

	void searchShopCardOder(); //查找商城抽卡令是否存在

	void searchCommonShopOder(); //查找宝匣钥匙是否存在
private:

	void addLocalInfo(TOOL_INFO* pToolInfo);

private:

	static ToolModel* mInstance;

	TOOL_INFO_MAP m_mapToolInfo;

	int m_nBackpackTotleCount;
	int m_nBackpackUsedCount;

	//是否初始化
	bool m_IsInit;

	//开启的宝箱信息
	vector<sBOX_TOOL_INFO> m_vBoxInfo;
	//是否使用了宝箱
	bool m_isUsedBox;
};

#endif // LoginController_h__
