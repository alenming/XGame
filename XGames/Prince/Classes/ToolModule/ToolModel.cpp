#include "ToolModel.h"

#include "../Utils/ResourceUtils.h"
#include "../Utils/StringFormat.h"
#include "Common/Common.h"
#include "ToolDef.h"
#include "../MainModule/MainModel.h"
#include "../SmithyModule/SmithyModel.h"
#include "cocos2d.h"
#include "SDK_TalkingData/TalkingData.h"
#include "Communication/Socket/Util.h"
#include "TakeCardModule/TakeCardDef.h"
#include "TakeCardModule/TakeCardModel.h"
#include "NewShopModule/ShopModel.h"

using namespace cocos2d;

ToolModel* ToolModel::mInstance = nullptr;

ToolModel::ToolModel()
{
	m_nBackpackTotleCount	= 0;
	m_nBackpackUsedCount	= 0;
	m_IsInit = false;
	m_isUsedBox = false;
}

ToolModel::~ToolModel()
{
}

ToolModel* ToolModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new ToolModel();
	}
	return mInstance;
}

void ToolModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

//背包是否可用
bool ToolModel::backpackIsUsable()
{
	return true;
	/*if (m_nBackpackUsedCount == 0)
	{
	return true;
	}
	if (m_nBackpackTotleCount > m_nBackpackUsedCount)
	{
	return true;
	}
	else
	{
	return false;
	}*/
}

int ToolModel::getBackpackTotleCount()
{
	return m_nBackpackTotleCount;
}

int ToolModel::getBackpackUsedCount()
{
	return m_nBackpackUsedCount;
}

void ToolModel::processToolJsonData( const Json::Value& jsonValue )
{	
	for (size_t i = 0; i < jsonValue.size(); i++)
	{
		Json::Value listItem = jsonValue[i];
		TOOL_INFO toolINFO;
		toolINFO.nadvLvl		= listItem["advLvl"].asInt();
		toolINFO.nid			= listItem["id"].asInt();
		toolINFO.nwarePetId		= listItem["warePetId"].asInt();
		toolINFO.ntemplateId	= listItem["templateId"].asInt();
		toolINFO.nstack			= listItem["stack"].asInt();
		//toolINFO.nExp			= listItem["exp"].asInt();
		toolINFO.nQHExp			= listItem["exp"].asInt();
		toolINFO.nCLLevel		= listItem["level"].asInt();

		addToolInfo(toolINFO.nid, toolINFO);
	}

	searchShopCardOder();

	searchCommonShopOder();
}

void ToolModel::updateToolJsonData( const Json::Value& jsonValue, vector<TOOL_CHANGE_INFO>* vecUpdateTools, vector<TOOL_NEW_INFO>* vecNewTools )
{
	TOOL_INFO toolINFO;
	for (size_t i = 0; i < jsonValue.size(); i++)
	{
		Json::Value listItem	= jsonValue[i];	
		toolINFO.nid			= listItem["id"].asInt();
		toolINFO.ntemplateId	= listItem["templateId"].asInt();
		string strStat			= listItem["stat"].asString();
		int amount				= listItem["amount"].asInt();
		if (strStat == "D")//"D"
		{	
			deleteToolInfo(toolINFO.nid, amount);
		}
		else
		{
			
			toolINFO.nadvLvl		= listItem["advLvl"].asInt();
			toolINFO.nwarePetId		= listItem["warePetId"].asInt();
			toolINFO.nstack			= listItem["stack"].asInt();
			//toolINFO.nExp			= listItem["exp"].asInt();
			toolINFO.nQHExp			= listItem["exp"].asInt();
			toolINFO.nCLLevel		= listItem["level"].asInt();

			if (strStat == "I")
			{
				if (vecNewTools != nullptr)
				{
					TOOL_NEW_INFO newInfo;
					newInfo.nid			= toolINFO.nid;
					newInfo.nstack		= toolINFO.nstack;
					newInfo.ntemplateId = toolINFO.ntemplateId;
					vecNewTools->push_back(newInfo);
				}
				addToolInfo(toolINFO.nid, toolINFO, amount);
			}
			else if (strStat == "U")
			{
				updateToolInfo(toolINFO.nid, toolINFO, vecUpdateTools, vecNewTools);
			}
		}		
	}
	
	searchShopCardOder();

	searchCommonShopOder();
}

void ToolModel::addToolInfo( const int& id, TOOL_INFO& info, const int& ncount )
{
	auto iter = m_mapToolInfo.find(id);
	if (iter != m_mapToolInfo.end())//已经存在， 个数+ncount
	{
		TOOL_INFO* pInfo = &((*iter).second);
		pInfo->nstack += ncount;
	}
	else
	{
		addLocalInfo(&info);
		m_mapToolInfo.insert(pair<int, TOOL_INFO>(id, info));
	}
}

void ToolModel::updateToolInfo( const int& id, TOOL_INFO& info, vector<TOOL_CHANGE_INFO>* vecUpdateTools, vector<TOOL_NEW_INFO>* vecNewTools )
{
	auto iter = m_mapToolInfo.find(id);
	if (iter != m_mapToolInfo.end())
	{
		TOOL_INFO* pInfo = &((*iter).second);

		//统计改变数据
		if (vecUpdateTools != nullptr)
		{
			TOOL_CHANGE_INFO changeInfo;
			memset(&changeInfo, 0, sizeof(TOOL_CHANGE_INFO));
			changeInfo.nid			= id;
			changeInfo.ntemplateId	= info.ntemplateId;
			changeInfo.nstack 		= info.nstack - pInfo->nstack;
			changeInfo.nadvLvl 		= info.nadvLvl - pInfo->nadvLvl;

			if (changeInfo.nstack > 0 && vecNewTools != nullptr)
			{
				TOOL_NEW_INFO newInfo;
				newInfo.nid			= changeInfo.nid;
				newInfo.nstack		= changeInfo.nstack;
				newInfo.ntemplateId = changeInfo.ntemplateId;
				vecNewTools->push_back(newInfo);
			}
			vecUpdateTools->push_back(changeInfo);
		}

		//统计新增数据
		if ( vecNewTools != nullptr) 
		{
			if(info.nstack > pInfo->nstack)
			{
				TOOL_NEW_INFO newInfo;
				newInfo.nid			= pInfo->nid;
				newInfo.nstack		= info.nstack - pInfo->nstack;
				newInfo.ntemplateId = pInfo->ntemplateId;
				vecNewTools->push_back(newInfo);
			}
		}

		//消耗数据统计
		int useNum = pInfo->nstack - info.nstack;

		//更新数据
		pInfo->nadvLvl		= info.nadvLvl;
		pInfo->nwarePetId	= info.nwarePetId;
		pInfo->ntemplateId	= info.ntemplateId;
		pInfo->nstack		= info.nstack;
		//pInfo->nExp			= info.nExp;
		pInfo->nBattleFlag	= info.nBattleFlag;
		pInfo->nRoleType	= info.nRoleType;
		pInfo->nQHExp		= info.nQHExp;
		pInfo->nCLLevel		= info.nCLLevel;

		//本地信息可能会因为ntemplateId改变而改变
		addLocalInfo(pInfo);

		//消耗品与英雄碎片数量扣减时上报
		if (useNum > 0 && (pInfo->nItemType == TOOL_ITEM_TYPE_CONSUME || pInfo->nItemType == TOOL_ITEM_TYPE_HERO_CHIP))
		{
			TalkingData::onUse(pInfo->strName, useNum);
		}
	}
	else//新增加数据
	{	
		if (vecNewTools != nullptr)
		{
			TOOL_NEW_INFO newInfo;
			newInfo.nid			= info.nid;
			newInfo.nstack		= info.nstack;
			newInfo.ntemplateId = info.ntemplateId;
			vecNewTools->push_back(newInfo);
		}
		addLocalInfo(&info);
		m_mapToolInfo.insert(pair<int, TOOL_INFO>(id, info));		
	}
}

void ToolModel::deleteToolInfo( const int& id, const int& ncount )
{
	auto iter = m_mapToolInfo.find(id);
	if (iter != m_mapToolInfo.end())
	{
		TOOL_INFO* pInfo = &((*iter).second);

		//消耗品与英雄碎片数量扣减时上报
		if (pInfo->nItemType == TOOL_ITEM_TYPE_CONSUME || pInfo->nItemType == TOOL_ITEM_TYPE_HERO_CHIP)
		{
			TalkingData::onUse(pInfo->strName, pInfo->nstack);
		}

		//直接删掉，和服务端统一做法20141203
		m_mapToolInfo.erase(iter);
	}	
}

const TOOL_INFO* ToolModel::GetToolInfo(const int& id)
{
	auto iter = m_mapToolInfo.find(id);
	if (iter != m_mapToolInfo.end())
	{
		return &((*iter).second);
	}
	return nullptr;
}

const TOOL_INFO* ToolModel::GetToolInfoBytemplateId(const int& ntemplateId)
{
	for (auto iter = m_mapToolInfo.begin(); iter != m_mapToolInfo.end(); iter++)
	{
		if (iter->second.ntemplateId == ntemplateId)
		{
			return &((*iter).second);
		}
	}
	return nullptr;
}

const int ToolModel::getToolTemplateId(const int& nId)
{
	auto iter = m_mapToolInfo.find(nId);
	if (iter != m_mapToolInfo.end())
	{
		return iter->second.ntemplateId;
	}
	return -1;
}

const int ToolModel::getToolResidByTemplateId(const int& ntemplateId)
{
	RowData* pData = DataManager::getInstance()->searchToolById(ntemplateId);
	if (pData != nullptr)
	{
		return pData->getIntData("resId");
	}
	/*for (auto iter = m_mapToolInfo.begin(); iter != m_mapToolInfo.end(); iter++)
	{
	if (iter->second.ntemplateId == ntemplateId)
	{
	return iter->second.nResId;
	}
	}*/
	return -1;
}

const int ToolModel::getToolResidById(const int& nId)
{
	auto iter = m_mapToolInfo.find(nId);
	if (iter != m_mapToolInfo.end())
	{
		return iter->second.nResId;		
	}
	return -1;
}

void ToolModel::removeTool(int id)
{
	auto iter = m_mapToolInfo.find(id);
	if (iter != m_mapToolInfo.end())
	{
		m_mapToolInfo.erase(iter);
	}
}
void ToolModel::clearToolInfo()
{
	m_mapToolInfo.clear();
}

const string ToolModel::GetToolImgFilename(const int& ntemplateId)
{
	const int resId = getToolResidByTemplateId(ntemplateId);
	return ResourceUtils::getSmallIconPath(resId);
}

const string ToolModel::GetToolImgFilenameById(const int& id)
{
	const int resId = getToolResidById(id);
	return ResourceUtils::getSmallIconPath(resId);
}

const string ToolModel::GetToolImgFilenameByResId(const int& resId)
{
	return ResourceUtils::getSmallIconPath(resId);
}

void ToolModel::sortAllTools(std::vector<TOOL_INFO*>& vecTools)
{
	//排序优先级：优先显示消耗品，其次装备，最后碎片
	std::sort(vecTools.begin(), vecTools.end(), 
		[this](TOOL_INFO* t1, TOOL_INFO* t2)->bool
	{
		//类型
		if (t1->nLocat != t2->nLocat)
		{
			return t1->nLocat < t2->nLocat;
		}
		
		if (t1->nLocat == TOOL_TYPE_CONSUME)
		{
			if(t1->nItemQua != t2->nItemQua)
			{
				return t1->nItemQua > t2->nItemQua;
			}
			else
			{
				return t1->nstack > t2->nstack;
			}
		}
// 		else if(t1->nLocat == TOOL_TYPE_EQUIP)
// 		{
// 			return getEquipPRI(t1->nEquipType) < getEquipPRI(t2->nEquipType);
// 		}
		else if(t1->nLocat == TOOL_TYPE_MATERIAL)
		{
			if (t1->nItemQua != t2->nItemQua)
			{
				return t1->nItemQua > t2->nItemQua;
			}
			else
			{
				return t1->nstack > t1->nstack;
			}
		}
		else if (t1->nLocat == TOOL_TYPE_HERO_CHIP)
		{
			return t1->nstack > t2->nstack;
		}
		else
		{
			return t1->nid > t2->nid;
		}
	});
}

void ToolModel::sortConsumeTools(std::vector<TOOL_INFO*> &vecTools)
{
	//消耗品按道具品质排序，品质相同按数量排序
	std::sort(vecTools.begin(), vecTools.end(), 
		[](TOOL_INFO* t1, TOOL_INFO* t2)->bool
	{
		if(t1->nItemQua != t2->nItemQua)
		{
			return t1->nItemQua > t2->nItemQua;
		}
		else
		{
			return t1->nstack > t2->nstack;
		}
	});
}

void ToolModel::sortEquipTools(std::vector<TOOL_INFO*> &vecTools)
{
	//显示顺序:按照武器，帽子，衣服，耳环，战马，兵书（背包中不显示的已装备的道具）
	std::sort(vecTools.begin(), vecTools.end(), 
		[this](TOOL_INFO* t1, TOOL_INFO* t2)->bool
	{
		return getEquipPRI(t1->nEquipType) < getEquipPRI(t2->nEquipType);
	});
}

void ToolModel::sortHeroToolsChip(std::vector<TOOL_INFO*> &vecTools)
{
	std::sort(vecTools.begin(), vecTools.end(), 
		[](TOOL_INFO* t1, TOOL_INFO* t2)->bool
	{
		return t1->nstack > t2->nstack;
	});
}

void ToolModel::sortMaterialTools(std::vector<TOOL_INFO*> &vecTools)
{
	std::sort(vecTools.begin(), vecTools.end(), 
		[](TOOL_INFO* t1, TOOL_INFO* t2)->bool
	{
		if (t1->nItemQua != t2->nItemQua)
		{
			return t1->nItemQua > t2->nItemQua;
		}
		else
		{
			return t1->nstack > t2->nstack;
		}
	});
}

void ToolModel::addLocalInfo(TOOL_INFO* pToolInfo)
{
	RowData* pRowData = DataManager::getInstance()->searchToolById(pToolInfo->ntemplateId);
	if (pRowData != nullptr)
	{
		pToolInfo->strName		= pRowData->getStringData("itemName");
		pToolInfo->nResId		= pRowData->getIntData("resId");
		pToolInfo->nItemType	= pRowData->getIntData("itemTyp");
		pToolInfo->nLocat		= pRowData->getIntData("locat");
		pToolInfo->nMaxSta		= pRowData->getIntData("maxSta");
		pToolInfo->nItemQua		= pRowData->getIntData("itemQua");
	}

	if (pToolInfo->nLocat == TOOL_TYPE_EQUIP)
	{
		RowData* equipData = DataManager::getInstance()->searchEquipmentById(pToolInfo->ntemplateId);
		if (equipData)
		{
			pToolInfo->nEquipType = equipData->getIntData("partTyp");
			pToolInfo->nItemStars = equipData->getIntData("stars");
		}
	}
}

void ToolModel::updateBackpackUsedCount(const int& usedCount, const int& totleCount)
{
	m_nBackpackTotleCount = totleCount;
	m_nBackpackUsedCount  = usedCount;
}

//装备是否可以进阶
bool ToolModel::equipCanAdv(const int& toolId)
{
	const TOOL_INFO* pToolInfo = ToolModel::getInstance()->GetToolInfo(toolId);
	if (pToolInfo == nullptr)
	{
		return false;
	}	
	//获取进阶属性
	RowData *pEquipmentAdvanced = DataManager::getInstance()->searchEquipmentAdvancedById(pToolInfo->ntemplateId);
	if (pEquipmentAdvanced == nullptr)
	{
		return false;
	}
	
	//4种消耗材料
	int index = 0;
	if (!equipAdvMatNumIsFull("mat1Id", "amount1", pEquipmentAdvanced))
	{
		return false;//材料不足不能进阶
	}
	if (!equipAdvMatNumIsFull("mat2Id", "amount2", pEquipmentAdvanced))
	{
		return false;//材料不足不能进阶
	}
	if (!equipAdvMatNumIsFull("mat3Id", "amount3", pEquipmentAdvanced))
	{
		return false;//材料不足不能进阶
	}
	//if (!equipAdvMatNumIsFull("mat4Id", "amount4", pEquipmentAdvanced))
	//{
	//	return false;//材料不足不能进阶
	//}

	//显示消耗银币
	/*
	int advCost = SmithyModel::getInstance()->getJJCost( pEquipmentAdvanced);
	if (advCost > MainModel::getInstance()->getMainParam()->mCoin)//银币不足不能进阶
	{
		return false;//银币不足不能进阶
	}
	*/
	return true;
}

//装备进阶材料是否达到进阶数量
bool ToolModel::equipAdvMatNumIsFull(const string& matStr, const string& amountStr, RowData *pEquipment)
{
	int matid = pEquipment->getIntData(matStr);
	int amount = pEquipment->getIntData(amountStr);
	if (matid != -1 && amount != -1)
	{
		const TOOL_INFO* pMat = ToolModel::getInstance()->GetToolInfoBytemplateId(matid);
		if (pMat == nullptr)
		{
			return false;
		}
		if (pMat->nstack >= amount)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	//如果为-1，不需要改材料
	return true;//false
}

void ToolModel::setIsInit(bool isInit)
{
	m_IsInit = isInit;
}

bool ToolModel::isInit()
{
	return m_IsInit;
}

//查询碎片当前数量
int ToolModel::searchHeroDebrisNum(int heroTempletId)
{
	RowData* heroRowData = DataManager::getInstance()->searchToolById(heroTempletId);
	if (heroRowData)
	{
		int debrisId = heroRowData->getIntData("transNeed");
		const TOOL_INFO* toolInfo = GetToolInfoBytemplateId(debrisId);
		if (toolInfo)
		{
			return toolInfo->nstack;
		}
	}
	return 0;
}

//查询碎片合成英雄所需数量
int ToolModel::searchHeroDebrisMaxNum(int heroTempletId)
{
	RowData* heroRowData = DataManager::getInstance()->searchToolById(heroTempletId);

	if (heroRowData)
	{
		int debrisId = heroRowData->getIntData("transNeed");
		RowData* debrisRowData = DataManager::getInstance()->searchToolById(debrisId);
		if (debrisRowData)
		{
			int amount = debrisRowData->getIntData("amount");
			return amount;
		}
		
	}
	return -1;
}

int ToolModel::searchPutOnEquip(EQUIP_ITEM_TYPE equipType)
{
	for (auto iter = m_mapToolInfo.begin(); iter != m_mapToolInfo.end(); iter++)
	{
		TOOL_INFO& toolInfo = iter->second;
		//筛选装备类型道具，并判断是否装备在别人身上
		if (toolInfo.nLocat == TOOL_TYPE_EQUIP && toolInfo.nwarePetId <= 0)
		{
			if (toolInfo.nEquipType == equipType) 
			{
				return toolInfo.nid;
			}
		}
	}
	return -1;
}

int ToolModel::searchPutOnEquipNum(int templetId)
{
	int num = 0;
	for (auto iter = m_mapToolInfo.begin(); iter != m_mapToolInfo.end(); iter++)
	{
		TOOL_INFO& toolInfo = iter->second;
		//筛选装备类型道具，并判断是否已装备
		if (toolInfo.ntemplateId == templetId && toolInfo.nwarePetId <= 0)
		{
			num++;
		}
	}
	return num;
}

int ToolModel::searchNumByTemplateId(int templateId)
{
	int num = 0;
	for (auto iter = m_mapToolInfo.begin(); iter != m_mapToolInfo.end(); iter++)
	{
		TOOL_INFO& toolInfo = iter->second;
		if (toolInfo.ntemplateId == templateId)
		{
			num += toolInfo.nstack;
		}
	}
	return num;
}

int ToolModel::getEquipPRI(int equipType)
{
	//获取排序优先级，序号越小优先级越高
	switch (equipType)
	{
	case EQUIP_ITEM_TYPE_WEAPON:
		return 1;
	case EQUIP_ITEM_TYPE_HAT:
		return 2;
	case EQUIP_ITEM_TYPE_CLOTHES:
		return 3;
	case EQUIP_ITEM_TYPE_EARRINGS:
		return 4;
	case EQUIP_ITEM_TYPE_HORSE:
		return 5;
	case EQUIP_ITEM_TYPE_BOOK:
		return 6;
	default:
		return 999;
	}
}

vector<sBOX_TOOL_INFO>& ToolModel::getUsedBoxInfo()
{
	return m_vBoxInfo;
}

void ToolModel::setUsedBoxInfo(const Json::Value& showItems)
{
	for(int i=0; i<showItems.size(); i++)
	{
		sBOX_TOOL_INFO boxInfo;
		boxInfo.nid = showItems[i]["id"].asInt();
		boxInfo.num = showItems[i]["amount"].asInt();
		m_vBoxInfo.push_back(boxInfo);
	}
}

void ToolModel::clearBoxInfo()
{
	m_vBoxInfo.clear();
}

bool ToolModel::getIsUsedBox()
{
	return m_isUsedBox;
}

void ToolModel::setIsUsedBox(bool isUsedBox)
{
	m_isUsedBox = isUsedBox;
}

void ToolModel::searchShopCardOder()
{
	RowData* coinData = DataManager::getInstance()->getDTToolSystem()->searchDataById(SHOP_COIN_SYSTEM_ID);
	RowData* goldData = DataManager::getInstance()->getDTToolSystem()->searchDataById(SHOP_GOLG_SYSTEM_ID);

	int coinCardOrderCount = 0;
	int goldCardOrderCount = 0;
	if (ToolModel::getInstance()->GetToolInfoBytemplateId(coinData->getIntData("value")))
	{
		coinCardOrderCount = ToolModel::getInstance()->GetToolInfoBytemplateId(coinData->getIntData("value"))->nstack;
	}
	if (ToolModel::getInstance()->GetToolInfoBytemplateId(goldData->getIntData("value")))
	{
		goldCardOrderCount = ToolModel::getInstance()->GetToolInfoBytemplateId(goldData->getIntData("value"))->nstack;
	}

	TakeCardModel::getInstance()->sethaveTakeCardOrder(false);
	if (coinCardOrderCount > 0 || goldCardOrderCount > 0)
	{
		TakeCardModel::getInstance()->sethaveTakeCardOrder(true);
		ObserverParam observerParam;
		observerParam.id = (int)TakeCardNotify_BuildIng;
		observerParam.self = this;
		notifyObservers(&observerParam);
	}
}


void ToolModel::searchCommonShopOder()
{
	RowData* keyData = DataManager::getInstance()->getDTToolSystem()->searchDataById(COMMON_SHOP_KEY_SYSTEM_ID);

	int keycount = 0;

	if (ToolModel::getInstance()->GetToolInfoBytemplateId(keyData->getIntData("value")))
	{
		keycount = ToolModel::getInstance()->GetToolInfoBytemplateId(keyData->getIntData("value"))->nstack;
	}

	if (keycount > 0)
	{
		bool ret = true;
		ObserverParam observerParam;
		observerParam.id = (int)HAVE_KEY_COUTN;
		observerParam.updateData = (void*)&ret;

		notifyObservers(&observerParam);

	}
}