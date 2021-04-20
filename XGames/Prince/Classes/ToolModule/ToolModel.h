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

	//�����Ƿ����
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
	
	//װ���Ƿ���Խ���
	bool equipCanAdv(const int& toolId);
	//װ�����ײ����Ƿ�ﵽ��������
	bool equipAdvMatNumIsFull(const string& matStr, const string& amountStr, RowData *pEquipment);

	void setIsInit(bool isInit);

	bool isInit();

	//��ѯ��Ƭ��ǰ����
	int searchHeroDebrisNum(int heroTempletId);

	//��ѯ��Ƭ�ϳ�Ӣ����������
	int searchHeroDebrisMaxNum(int heroTempletId);

	//��ѯ�Ƿ���ָ�����͵Ŀ�װ�����ߣ����ص���id
	int searchPutOnEquip(EQUIP_ITEM_TYPE equipType);

	//��ѯָ��templetId�Ŀɴ���װ������
	int searchPutOnEquipNum(int templetId);

	//��ѯָ����������
	int searchNumByTemplateId(int templateId);

	//��ȡװ�����������ȼ�
	int getEquipPRI(int equipType);

	//��ȡʹ�õı�����Ϣ
	vector<sBOX_TOOL_INFO>& getUsedBoxInfo();
	//����ʹ�õı�����Ϣ
	void setUsedBoxInfo(const Json::Value& jsonValue);
	//��ձ�������
	void clearBoxInfo();

	//�Ƿ�ʹ���˱���
	bool getIsUsedBox();
	void setIsUsedBox(bool isUsedBox);

	void searchShopCardOder(); //�����̳ǳ鿨���Ƿ����

	void searchCommonShopOder(); //���ұ�ϻԿ���Ƿ����
private:

	void addLocalInfo(TOOL_INFO* pToolInfo);

private:

	static ToolModel* mInstance;

	TOOL_INFO_MAP m_mapToolInfo;

	int m_nBackpackTotleCount;
	int m_nBackpackUsedCount;

	//�Ƿ��ʼ��
	bool m_IsInit;

	//�����ı�����Ϣ
	vector<sBOX_TOOL_INFO> m_vBoxInfo;
	//�Ƿ�ʹ���˱���
	bool m_isUsedBox;
};

#endif // LoginController_h__
