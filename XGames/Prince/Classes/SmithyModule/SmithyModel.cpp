#include "SmithyModel.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainModel.h"

#include "DataManager/DataTable.h"
#include "DataManager/DataManager.h"
#include <math.h>
#include "TowerModule/TowerModel.h"
#include "PvpModule/PvpModel.h"
#include "ChapterModule/ChapterModel.h"

SmithyModel* SmithyModel::mInstance;

SmithyModel::SmithyModel()
{
}

SmithyModel::~SmithyModel()
{
}

SmithyModel* SmithyModel::getInstance()
{
	if(!mInstance)
	{
		mInstance = new SmithyModel();
	}
	return mInstance;
}

void SmithyModel::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void SmithyModel::updateUI( OBS_PARAM_TYPE_SM type, void* data )
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}

void SmithyModel::getQHParam( const int nToolId, QH_Param& param )
{
	// 基础数据;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	assert(_info != nullptr);
	param.nEquId = _info->nid;
	param.nEquQua = (TOOL_ITEM_QUA)_info->nItemQua;
	param.nEquCLId = calcCLId(_info, _info->nCLLevel);
	RowData* _rowCL = DataManager::getInstance()->searchEquipmentQuenchById(param.nEquCLId);
	CCASSERT(_rowCL != nullptr, "");
	param.nEquIcon = _rowCL->getIntData("icon");
	param.strEquName = _rowCL->getStringData("name");

	RowData* _row = DataManager::getInstance()->searchEquipmentById(_info->ntemplateId);
	assert(_row != nullptr);
	param.nMaxLv = _row->getIntData("maxLevel");
	param.nPreLv = _info->nadvLvl;
	param.nDstLv = (param.nPreLv+1 > param.nMaxLv) ? param.nMaxLv : param.nPreLv+1;

	// 马书强化经验值;
	_row = DataManager::getInstance()->searchEquipmentExp(_info->nadvLvl);
	CCASSERT(_row != nullptr, "");
	param.nExp = _info->nQHExp;
	param.nMaxExp = _row->getIntData("exp");

	// 判断是否还能进阶，以及是否强化到顶端;
	_row = DataManager::getInstance()->searchEquipmentAdvancedById(_info->ntemplateId);
	param.bEnableJJ = (_row != nullptr);
	param.bFinal = (param.nPreLv >= param.nMaxLv && !param.bEnableJJ);

	// 属性计算;
	// 获取属性类型;
	vector<SMITHY_EQU_ATTR_TYPE>  vcAttrType;
	getEquAttrType(nToolId, vcAttrType);
	//assert(vcAttrType.size() == 1);

	for (int i = 0; i < vcAttrType.size(); ++i)
	{
		// 根据类型计算基础值和增量;
		float fBaseValue = calcBaseAttrValue(nToolId, vcAttrType.at(i), param.nPreLv);
		float fBaseValue_Dst = calcBaseAttrValue(nToolId, vcAttrType.at(i), param.nDstLv);

		AttrParam  attrParam;
		attrParam.attrType = vcAttrType.at(i);
		attrParam.strAttrName = getUINameByAttrType(attrParam.attrType);
		attrParam.fAttrValue = fBaseValue;
		attrParam.fAttrAdd = max(0.0f, fBaseValue_Dst - fBaseValue);
		param.vcBase.push_back(attrParam);
	}
	
	// 强化1次消耗;
	param.nCost = getQH_X1_Cost(nToolId);
}

void SmithyModel::getJJParam( const int nToolId, JJ_Param& param )
{
	// 基础数据;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	assert(_info != nullptr);
	param.nEquId = _info->nid;
	param.nEquAdvLv = _info->nadvLvl;
	param.nEquQua = (TOOL_ITEM_QUA)_info->nItemQua;
	param.nEquCLId = calcCLId(_info, _info->nCLLevel);
	RowData* _rowCL = DataManager::getInstance()->searchEquipmentQuenchById(param.nEquCLId);
	CCASSERT(_rowCL != nullptr, "");
	param.nEquIcon = _rowCL->getIntData("icon");
	param.strEquName = _rowCL->getStringData("name");

	// 查进阶表，读取dst物品Id和消耗;
	int nDstAdvLv = 1;
	RowData* _rowJJ = DataManager::getInstance()->searchEquipmentAdvancedById(_info->ntemplateId);
	if (nullptr != _rowJJ)
	{
		param.nNeedLv = _rowJJ->getIntData("level");
		param.nEquDstId = nToolId;
		param.nCost = _rowJJ->getIntData("costCoin");
		param.nEquDstTemplateId = _rowJJ->getIntData("newItemId");
		param.nEquDstAdvLv = calcJJLevel(param.nEquId);

		// 进阶后的道具在背包里可能没有，此处直接读表;
		RowData* _dst = DataManager::getInstance()->searchToolById(param.nEquDstTemplateId);
		assert(_dst != nullptr);
		param.nEquDstQua = (TOOL_ITEM_QUA)_dst->getIntData("itemQua");

		// 区分类型，计算材料消耗;
		EQUIP_ITEM_TYPE equType = getEquType(nToolId);
		if (equType < EQUIP_ITEM_TYPE_HORSE)
		{
			// 进阶材料;
			for (unsigned int i = 0; i < 3; ++i)
			{
				// 材料Id;
				ostringstream oss;
				oss << "mat" << i+1 << "Id";
				int nMatTemplateId = _rowJJ->getIntData(oss.str());

				if (nMatTemplateId != -1)
				{
					// 当前拥有数量;
					vector<int>  vcAmount;
					int nCount = ToolModel::getInstance()->searchNumByTemplateId(nMatTemplateId);
					vcAmount.push_back(nCount);

					// 需要数量;
					oss.str("");
					oss << "amount" << i+1;
					int nNeedCount = _rowJJ->getIntData(oss.str());
					vcAmount.push_back(nNeedCount);
					oss.str("");

					// 数量不足时需要的金币，累加;
					oss.str("");
					oss << "gold" << i+1;
					nNeedCount = ((nNeedCount - nCount <= 0) ? 0 : (nNeedCount - nCount));
					int nPrice = _rowJJ->getIntData(oss.str());
					param.nCostGold += nPrice*nNeedCount;

					param.mapTools.insert(make_pair(nMatTemplateId, vcAmount));
				}
			}
		}
		else if (equType == EQUIP_ITEM_TYPE_HORSE)
		{
			// 战马消耗试炼积分;
			param.nTrialScore = _rowJJ->getIntData("trials");

			// 积分不足的情况下消耗金币;
			param.nCostGold = _rowJJ->getIntData("trialsGold");
		}
		else if (equType == EQUIP_ITEM_TYPE_BOOK)
		{
			// 兵书进阶消耗竞技场积分;
			param.nPvpScore = _rowJJ->getIntData("pvp");

			// 积分不足的情况下消耗金币;
			param.nCostGold = _rowJJ->getIntData("pvpGold");
		}
	}
	else
	{
		// 查不到，将被认为是已进阶到最高等级;
		param.bFinal = true;
		param.nEquDstId = _info->nid;
		param.nEquDstTemplateId = _info->ntemplateId;		// 和左侧templateId相同;
		param.nEquDstAdvLv = _info->nadvLvl;				// 进阶等级也和左侧相同;
		param.nEquDstQua = (TOOL_ITEM_QUA)_info->nItemQua;
		param.strEquDstName = param.strEquName;
		//return;
	}

	// 已达最高等级，右侧装备与左侧装备展示相同;
	param.nEquDstIcon = param.nEquIcon;
	param.strEquDstName = param.strEquName;

	// 4.属性计算;
	// 获取属性类型;
	vector<SMITHY_EQU_ATTR_TYPE>  vcAttrType;
	getEquAttrType(nToolId, vcAttrType);
	//assert(vcAttrType.size() == 1);

	for (int i = 0; i < vcAttrType.size(); ++i)
	{
		// 根据类型计算基础值和增量;
		float fBaseValue = calcBaseAttrValue(nToolId, vcAttrType.at(i), _info->nadvLvl);
		float fBaseValue_Dst = calcBaseAttrValue_JJDstEqu(param.nEquDstTemplateId, vcAttrType.at(i), param.nEquDstAdvLv, param.nEquCLId);

		AttrParam  attrParam;
		attrParam.attrType = vcAttrType.at(i);
		attrParam.strAttrName = getUINameByAttrType(attrParam.attrType);
		attrParam.fAttrValue = fBaseValue;
		attrParam.fAttrAdd = max(0.0f, fBaseValue_Dst - fBaseValue);
		param.vcBase.push_back(attrParam);
	}

}

void SmithyModel::getCLParam( const int nToolId, EQUIP_ITEM_TYPE itemType, CL_Param& param )
{
	// 基础数据;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	assert(_info != nullptr);
	param.nEquId = _info->nid;
	param.nEquTemplateId = _info->ntemplateId;
	param.nEquQua = (TOOL_ITEM_QUA)_info->nItemQua;
	param.nEquCLLevel = _info->nCLLevel;
	param.nEquCLId = calcCLId(_info, param.nEquCLLevel);
	param.nEquAdvLv = _info->nadvLvl;

	// 查淬炼表，
	// 1.读取左侧装备Icon和名称
	RowData* _rowCL = DataManager::getInstance()->searchEquipmentQuenchById(param.nEquCLId);
	if (nullptr != _rowCL)
	{
		param.nEquIcon = _rowCL->getIntData("icon");
		param.strEquName = _rowCL->getStringData("name");
		param.nCostGold = _rowCL->getIntData("gold");

		// 天命属性;
		param.bIsPreFate = (_rowCL->getIntData("fate") == 1 ? true : false);
		param.strTianMingDesc = _rowCL->getStringData("skillDes");

		// 2.读取淬炼消耗(材料是配在淬炼前的那条数据);
		map<int, int>  mapCLMaterials;
		ostringstream oss;
		for (int i = 1; i <= 3; ++i)
		{
			// 材料Id;
			oss.str("");
			oss << "mat" << i << "Id";
			int nNeedMaterialId = _rowCL->getIntData(oss.str());
			if (nNeedMaterialId == -1)
				continue;

			vector<int> vcAmount;

			// 拥有数量;
			int nAmount = ToolModel::getInstance()->searchNumByTemplateId(nNeedMaterialId);
			vcAmount.push_back(nAmount);

			// 需要数量;
			oss.str("");
			oss << "amount" << i;
			int nNeedMaterialAmount = _rowCL->getIntData(oss.str());
			vcAmount.push_back(nNeedMaterialAmount);
			param.mapTools.insert(make_pair(nNeedMaterialId, vcAmount));
		}
	}

	// 查淬炼表;
	// 3.读取右侧装备Icon和名称;
	param.nEquDstId = param.nEquId;
	param.nEquDstQua = param.nEquQua;
	if (param.nEquCLLevel >= 5)
	{
		// 已达最高等级，右侧装备与左侧装备展示相同;
		param.bFinal = true;
		param.nEquDstIcon = param.nEquIcon;
		param.strEquDstName = param.strEquName;
		param.nEquDstCLLevel = param.nEquCLLevel;
		param.nEquDstCLId = param.nEquCLId;
		param.bIsDstFate = param.bIsPreFate;
	}
	else
	{
		param.bFinal = false;
		param.nEquDstCLLevel = param.nEquCLLevel + 1;
		param.nEquDstCLId = param.nEquCLId + 1;
		_rowCL = DataManager::getInstance()->searchEquipmentQuenchById(param.nEquDstCLId);
		CCASSERT(_rowCL != nullptr, "");
		param.nEquDstIcon = _rowCL->getIntData("icon");
		param.strEquDstName = _rowCL->getStringData("name");
		param.bIsDstFate = (_rowCL->getIntData("fate") == 1 ? true : false);
	}

	// 4.属性计算;
	// 获取属性类型;
	vector<SMITHY_EQU_ATTR_TYPE>  vcAttrType;
	getEquAttrType(nToolId, vcAttrType);
	//assert(vcAttrType.size() == 1);

	for (int i = 0; i < vcAttrType.size(); ++i)
	{
		// 根据类型计算基础值和增量;
		float fBaseValue = calcBaseAttrValue_CL(nToolId, vcAttrType.at(i), param.nEquCLLevel);
		float fBaseValue_Dst = calcBaseAttrValue_CL(nToolId, vcAttrType.at(i), param.nEquDstCLLevel);

		AttrParam  attrParam;
		attrParam.attrType = vcAttrType.at(i);
		attrParam.strAttrName = getUINameByAttrType(attrParam.attrType);
		attrParam.fAttrValue = fBaseValue;
		attrParam.fAttrAdd = max(0.0f, fBaseValue_Dst - fBaseValue);
		param.vcBase.push_back(attrParam);
	}
}

/********************************************************************
*  函数名称: getEquAttrType;
*  功能描述: 查询装备属性类型(以装备表为准);
*  参数列表: [in]nToolId : 装备Id;
*			 [in]vcAttrType: 属性类型;
*  返回值  : 
********************************************************************/
void SmithyModel::getEquAttrType( const int nToolId, vector<SMITHY_EQU_ATTR_TYPE>& vcAttrType )
{
	vcAttrType.clear();

	// 基础数据;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	assert(_info != nullptr);

	// 读装备表，确定每条装备的属性类型;
	RowData* _row = DataManager::getInstance()->searchEquipmentById(_info->ntemplateId);
	if (nullptr == _row)
		return;

	// 遍历所有字段;
	///////////////// 血量/攻击/物防/法防;
	float fValue = _row->getFloatData("iniHp");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_HP);
	}
	fValue = _row->getFloatData("iniAtk");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_ATK);
	}
	fValue = _row->getFloatData("iniPdd");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_PDD);
	}
	fValue = _row->getFloatData("iniMdd");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_MDD);
	}

	///////////////// 血量加成/攻击加成/物防加成/法防加成;
	fValue = _row->getFloatData("iniHpCoe");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_HP_COE);
	}
	fValue = _row->getFloatData("iniAtkCoe");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_ATK_COE);
	}
	fValue = _row->getFloatData("iniPddCoe");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_PDD_COE);
	}
	fValue = _row->getFloatData("iniMddCoe");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_MDD_COE);
	}

	///////////////// 暴击率/抗暴率/命中/闪避;
	fValue = _row->getFloatData("iniCriPrb");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_CRI_PRB);
	}
	fValue = _row->getFloatData("iniCriRes");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_CRI_RES);
	}
	fValue = _row->getFloatData("iniHit");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_HIT);
	}
	fValue = _row->getFloatData("iniMiss");
	if (fValue > 0.000001f)
	{
		vcAttrType.push_back(ATTR_TYPE_MISS);
	}
}

EQUIP_ITEM_TYPE SmithyModel::getEquType( const int nToolId )
{
	EQUIP_ITEM_TYPE itemType = EQUIP_ITEM_TYPE_NONE;

	// 查表;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	if (nullptr != _info)
	{
		RowData* _row = DataManager::getInstance()->searchEquipmentById(_info->ntemplateId);
		if (nullptr != _row)
		{
			itemType = (EQUIP_ITEM_TYPE)_row->getIntData("partTyp");
		}
	}

	return itemType;
}

int SmithyModel::isEquJJValid( const int nEquId )
{
	if (!GuideManager::getInstance()->isFunctionOpen(FUNCTION_EQUIP_ADVANCE, false))
		return -8;

	const TOOL_INFO *pToolInfo = ToolModel::getInstance()->GetToolInfo(nEquId);
	if (pToolInfo == nullptr)
		return -1;

	// 1.查询进阶属性;
	RowData *pEquipmentAdvanced = DataManager::getInstance()->searchEquipmentAdvancedById(pToolInfo->ntemplateId);
	if (pEquipmentAdvanced == nullptr)
	{
		//CustomTips::show("该装备不能进阶！");
		return -2;
	}

	// 2. 新增条件(等级限制 03/05/2016 added by phil);
	int advNeedLv = pEquipmentAdvanced->getIntData("level");
	if (HeroModel::getInstance()->getMainHeroLevel() < advNeedLv)
	{
		return -5;
	}

	if (checkOpType(nEquId) != SMITHY_OP_TYPE_JJ)
		return -10;

	// 3.校验进阶材料数量;
	// 马书不需要材料;
	EQUIP_ITEM_TYPE equipType = getEquType(nEquId);
	if (equipType < EQUIP_ITEM_TYPE_HORSE)
	{
		if (!ToolModel::getInstance()->equipCanAdv(nEquId))
		{
			// CustomTips::show("材料不足！");
			return -4;
		}
	}
	else
	{
		// 马书有自身的开放等级(覆盖强化/进阶/淬炼);
		if (!GuideManager::getInstance()->isFunctionOpen(FUNCTION_HORSE_BOOK, false))
		{
			return -9;
		}

		// 马书需要积分;
		int nNeedScore = 0;
		int nTotalScore = 0;
		if (equipType == EQUIP_ITEM_TYPE_HORSE)
		{
			nNeedScore = pEquipmentAdvanced->getIntData("trials");
			nTotalScore = max(0, MainModel::getInstance()->getMainParam()->mTrialScore);
		}
		else if (equipType == EQUIP_ITEM_TYPE_BOOK)
		{
			nNeedScore = pEquipmentAdvanced->getIntData("pvp");
			nTotalScore = max(0, MainModel::getInstance()->getMainParam()->mPvpScore);
		}
		else
			return -6; // 积分不足;

		if (nTotalScore < nNeedScore)
			return -6;
	}

	// 4.查询进阶消耗银币;
	int advCost = pEquipmentAdvanced->getIntData("costCoin");
	if (advCost > MainModel::getInstance()->getMainParam()->mCoin)//银币不足不能进阶
	{
		//CustomTips::show("银币不足！");
		return -3;
	}

	// 强化前不提示进阶，这个限制比较弱，可以不提示;
	RowData* _row = DataManager::getInstance()->searchEquipmentById(pToolInfo->ntemplateId);
	assert(_row != nullptr);
	int nMaxLv = _row->getIntData("maxLevel");
	if (pToolInfo->nadvLvl < nMaxLv)
	{
		return -7;
	}

	return 0;
}

int SmithyModel::isEquQHValid( const int nEquId )
{
	if (!GuideManager::getInstance()->isFunctionOpen(FUNCTION_EQUIP_STRENGTHEN, false))
		return -6;

	if (checkOpType(nEquId) != SMITHY_OP_TYPE_QH)
		return -8;

	// 马书强化时以材料数量为准;
	if (getEquType(nEquId) >= EQUIP_ITEM_TYPE_HORSE)
	{
		// 马书有自身的开放等级(覆盖强化/进阶/淬炼);
		if (!GuideManager::getInstance()->isFunctionOpen(FUNCTION_HORSE_BOOK, false))
		{
			return -7;
		}

		const TOOL_INFO* pInfo = ToolModel::getInstance()->GetToolInfo(nEquId);
		CCASSERT(pInfo != nullptr, "");
		RowData *pEquipment = DataManager::getInstance()->searchEquipmentById(pInfo->ntemplateId);
		if (pEquipment == nullptr)
		{
			return -1;
		}
		if (pInfo->nadvLvl >= pEquipment->getIntData("maxLevel"))
		{
			return -2;
		}

		vector<const TOOL_INFO*> vcMaterial = getQHMaterial_HorseBook(nEquId, 1);
		if (vcMaterial.size() > 0)
		{
			return 0;
		}
		else
		{
			return -5;
		}
	}
	else
	{
		const TOOL_INFO* pInfo = ToolModel::getInstance()->GetToolInfo(nEquId);
		if (pInfo != nullptr)
		{
			RowData *pEquipment = DataManager::getInstance()->searchEquipmentById(pInfo->ntemplateId);
			if (pEquipment == nullptr)
			{
				return -1;
			}

			// 当已达到最大等级时，提示”装备已强化至最大等级“;
			// 装备最大强化等级 = 角色等级 * 2; (已废除此标准，以读表为准);
			if (pInfo->nadvLvl >= pEquipment->getIntData("maxLevel"))//HeroModel::getInstance()->getMainHeroLevel()*2)
			{
				//当装备强化至当前最高等级时，点击后提示“提升主角等级可增加强化上限”
				//CustomTips::show("提升主角等级可增加强化上限");
				return -2;
			}

			// 当银币不足时提示“银币不足“;
			MainParam *pMainParam = MainModel::getInstance()->getMainParam();
			if (getQH_X1_Cost(nEquId) > pMainParam->mCoin)
			{
				//CustomPop::show("银币不足!");
				return -3;
			}

			return 0;
		}
	}

	return -4;
}

int SmithyModel::isEquCLValid( const int nToolId )
{
	// 1. 等级开放;
	if (!GuideManager::getInstance()->isFunctionOpen(FUNCTION_EQUIP_CL, false))
	{
		return -1;
	}

	// 马书有自身的开放等级(覆盖强化/进阶/淬炼);
	if (getEquType(nToolId) >= EQUIP_ITEM_TYPE_HORSE)
	{
		if (!GuideManager::getInstance()->isFunctionOpen(FUNCTION_HORSE_BOOK, false))
		{
			return -4;
		}
	}

	// 2. 淬炼是否达到顶级;
	CL_Param _param;
	getCLParam(nToolId, getEquType(nToolId), _param);
	if (_param.bFinal)
	{
		return -2;
	}

	// 3. 淬炼条件;
	for (auto iter = _param.mapTools.begin(); iter != _param.mapTools.end(); ++iter)
	{
		if (iter->second.at(0) < iter->second.at(1))
		{
			return -3;
		}
	}

	return 0;
}

void SmithyModel::onEquQHResult( const TOOL_CHANGE_INFO* pChangeInfo )
{
	// 更新UI;
	updateUI(OBS_PARAM_TYPE_EQU_QH_RESULT, (void*)(pChangeInfo));
}

void SmithyModel::onEquJJResult( const TOOL_CHANGE_INFO* pChangeInfo )
{
	// 更新UI;
	updateUI(OBS_PARAM_TYPE_EQU_JJ_RESULT, (void*)(pChangeInfo));
}

void SmithyModel::onEquCLResult(const TOOL_CHANGE_INFO* pChangeInfo, OBS_PARAM_TYPE_SM clType)
{
	// 更新UI;
	updateUI(clType, (void*)(pChangeInfo));
}

std::string SmithyModel::getUINameByAttrType( SMITHY_EQU_ATTR_TYPE attrType )
{
	string str("");
	switch (attrType)
	{
	case ATTR_TYPE_HP:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10008));
		break;
	case ATTR_TYPE_ATK:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10009));
		break;
	case ATTR_TYPE_PDD:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10010));
		break;
	case ATTR_TYPE_MDD:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10011));
		break;

	case ATTR_TYPE_HP_COE:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10008));
		break;
	case ATTR_TYPE_ATK_COE:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10009));
		break;
	case ATTR_TYPE_PDD_COE:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10010));
		break;
	case ATTR_TYPE_MDD_COE:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10011));
		break;

	case ATTR_TYPE_CRI_PRB:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10014));
		break;
	case ATTR_TYPE_CRI_RES:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10015));
		break;
	case ATTR_TYPE_HIT:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10012));
		break;
	case ATTR_TYPE_MISS:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10013));
		break;
	default:
		break;
	}

	return str;
}

void SmithyModel::clearEquipSourceItems()
{
	m_vecEquipSourceItems.clear();
}

vector<EQUIP_SOURCE_ITEM>& SmithyModel::getEquipSourceItems()
{
	return m_vecEquipSourceItems;
}


/********************************************************************
*  函数名称: calcJJLevel;
*  功能描述: 计算装备进阶后的强化等级;
*  参数列表: [in]nToolId : 需要进阶的装备Id;
*  返回值  : (int)新的强化等级;
********************************************************************/
int SmithyModel::calcJJLevel( const int nToolId )
{
	int nNewAdvLv = 1;

	// 读取装备当前强化等级;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	assert(_info != nullptr);
	int nOldAdvLv = _info->nadvLvl;
	nNewAdvLv = nOldAdvLv;

	// 读取系数(进阶表);
	RowData* _row = DataManager::getInstance()->searchEquipmentAdvancedById(_info->ntemplateId);
	assert(_row != nullptr);
	float factor = _row->getFloatData("factor");

	// 计算公式(向上取整);
	//   进阶后强化等级 = ROUNDUP(系数 *（当前装备强化等级 +9） - 9,0）
	//nNewAdvLv = (int)ceil(factor * (nOldAdvLv + 9) - 9);

	// 新的计算公式(08/13/2015 Phil @zcjoy);
	//   进阶后强化等级 = ROUNDUP(系数 *当前装备强化等级 ,0）
	nNewAdvLv = (int)ceil(factor * nOldAdvLv);
	return nNewAdvLv;
}

/********************************************************************
*  函数名称: calcBaseAttrValue;
*  功能描述: 计算装备属性总的基础值(3个操作界面的基础值为同一个);
*  参数列表: [in]nToolId : 装备Id;
*			 [in]attrType: 属性类型;
*  返回值  : (float)该类型属性的基础值;
********************************************************************/
float SmithyModel::calcBaseAttrValue( const int nToolId, SMITHY_EQU_ATTR_TYPE attrType, int nQHLv )
{
	// 获取装备信息;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	assert(_info != nullptr);

	// 读取属性强化基础值和增量，并计算总的基础值;
	float fEquBaseValue, fEquAddValue, fCLBaseValue, fCLAddValue;
	getBase_Add(nToolId, SMITHY_OP_TYPE_QH, attrType, fEquBaseValue, fEquAddValue);
	getBase_Add_ByTemplateId(calcCLId(_info, _info->nCLLevel), SMITHY_OP_TYPE_CL, attrType, fCLBaseValue, fCLAddValue);
	return calcBaseAttrValueByFormula(fEquBaseValue, fEquAddValue, fCLBaseValue, fCLAddValue, nQHLv);
}

float SmithyModel::calcBaseAttrValue_JJDstEqu( const int nTemplateId, SMITHY_EQU_ATTR_TYPE attrType, int nQHLv, int nCLId )
{
	// 读取属性强化基础值和增量，并计算总的基础值;
	float fEquBaseValue, fEquAddValue, fCLBaseValue, fCLAddValue;
	getBase_Add_ByTemplateId(nTemplateId, SMITHY_OP_TYPE_QH, attrType, fEquBaseValue, fEquAddValue);
	getBase_Add_ByTemplateId(nCLId, SMITHY_OP_TYPE_CL, attrType, fCLBaseValue, fCLAddValue);
	return calcBaseAttrValueByFormula(fEquBaseValue, fEquAddValue, fCLBaseValue, fCLAddValue, nQHLv);
}

float SmithyModel::calcBaseAttrValue_CL( const int nToolId, SMITHY_EQU_ATTR_TYPE attrType, int nCLLevel )
{
	// 获取装备信息;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	assert(_info != nullptr);
	int nQHLv = _info->nadvLvl;

	// 读取属性强化基础值和增量，并计算总的基础值;
	float fEquBaseValue, fEquAddValue, fCLBaseValue, fCLAddValue;
	getBase_Add(nToolId, SMITHY_OP_TYPE_QH, attrType, fEquBaseValue, fEquAddValue);
	getBase_Add_ByTemplateId(calcCLId(_info, nCLLevel), SMITHY_OP_TYPE_CL, attrType, fCLBaseValue, fCLAddValue);
	return calcBaseAttrValueByFormula(fEquBaseValue, fEquAddValue, fCLBaseValue, fCLAddValue, nQHLv);
}

float SmithyModel::calcBaseAttrValueByFormula(float fEquBaseValue, float fEquAddValue, float fCLBaseValue, float fCLAddValue, int nQHLv)
{
	// 计算公式(03/09/2016);
	//   基础值 = 装备属性基础值 + 淬炼属性基础值 +（强化等级-1）* （装备属性成长值 + 淬炼属性成长值）;
	return  fEquBaseValue + fCLBaseValue + (nQHLv - 1) * (fEquAddValue + fCLAddValue);
}

/********************************************************************
*  函数名称: get_Base_Add;
*  功能描述: 读取装备属性[基础值]和[增量]，以操作类型参数来区分表;
*  参数列表: [in]nToolId : 装备Id;
*			 [in]dataType: 操作类型;
*			 [in]attrType: 属性类型;
*			 [out]fBaseValue: 基础值;
*			 [out]fAddValue: 增量;
*  返回值  : (void);
********************************************************************/
void SmithyModel::getBase_Add( int nToolId, SMITHY_OP_TYPE dataType, SMITHY_EQU_ATTR_TYPE attrType, float& fBaseValue, float& fAddValue )
{
	// 查询装备信息;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	assert(_info != nullptr);
	getBase_Add_ByTemplateId(_info->ntemplateId, dataType, attrType, fBaseValue, fAddValue);

}
void SmithyModel::getBase_Add_ByTemplateId( int nToolTemplateId, SMITHY_OP_TYPE dataType, SMITHY_EQU_ATTR_TYPE attrType, float& fBaseValue, float& fAddValue )
{
	RowData* _row = nullptr;
	switch (dataType)
	{
	case SMITHY_OP_TYPE_QH:
		_row = DataManager::getInstance()->searchEquipmentById(nToolTemplateId);
		break;
	/*case SMITHY_OP_TYPE_JJ:
		{
			_row = DataManager::getInstance()->searchEquipmentAdvancedById(_info->ntemplateId);
		}
		break;
		*/
	case SMITHY_OP_TYPE_CL:
		{
			// 淬炼表比较特殊，传进来的Id其实是构造出来的淬炼道具Id;
			int nCLId = nToolTemplateId;
			_row = DataManager::getInstance()->searchEquipmentQuenchById(nCLId);
		}
		break;
	default:
		break;
	}

	if (_row == nullptr)
		return;

	// 读装备表, 确定数据表中的属性类型字段;
	string strIni, strGro;
	switch (attrType)
	{
	case ATTR_TYPE_HP:
		{
			strIni.assign("iniHp");
			strGro.assign("groHp");
		}
		break;
	case ATTR_TYPE_ATK:
		{
			strIni.assign("iniAtk");
			strGro.assign("groAtk");
		}
		break;
	case ATTR_TYPE_PDD:
		{
			strIni.assign("iniPdd");
			strGro.assign("groPdd");
		}
		break;
	case ATTR_TYPE_MDD:
		{
			strIni.assign("iniMdd");
			strGro.assign("groMdd");
		}
		break;
	case ATTR_TYPE_HP_COE:
		{
			strIni.assign("iniHpCoe");
			strGro.assign("groHpCoe");
		}
		break;
	case ATTR_TYPE_ATK_COE:
		{
			strIni.assign("iniAtkCoe");
			strGro.assign("groAtkCoe");
		}
		break;
	case ATTR_TYPE_PDD_COE:
		{
			strIni.assign("iniPddCoe");
			strGro.assign("groPddCoe");
		}
		break;
	case ATTR_TYPE_MDD_COE:
		{
			strIni.assign("iniMddCoe");
			strGro.assign("groMddCoe");
		}
		break;
	default:
		break;
	}

	// 获得具体属性类型的[基础值]和[强化增量];
	fBaseValue = _row->getFloatData(strIni);
	if (fBaseValue < 0.000001f)
	{
		fBaseValue = 0.0f;
	}
	fAddValue = _row->getFloatData(strGro);
	if (fAddValue < 0.000001f)
	{
		fAddValue = 0.0f;
	}
}

int SmithyModel::calcCLId( const int nToolId )
{
	// 获取装备信息;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	assert(_info != nullptr);
	return calcCLId(_info, _info->nCLLevel);
}

int SmithyModel::calcCLId( const TOOL_INFO* toolInfo, int nCLLevel )
{
	HeroParam* _hero = HeroModel::getInstance()->searchHeroById(toolInfo->nwarePetId);
	CCASSERT(_hero != nullptr, "");
	EQUIP_ITEM_TYPE equType = getEquType(toolInfo->nid);
	int nCLId = _hero->mTemplateId*100 + equType*10 + nCLLevel;
	return nCLId;
}

int SmithyModel::getQH_X1_Cost( const int nToolId )
{
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	assert(nullptr != _info);
	RowData* _row = DataManager::getInstance()->searchEquipmentById(_info->ntemplateId);
	assert(nullptr != _row);

	// 装备强化消耗公式：装备品质系数*（A*level+B），A默认值为150，B默认值为750;
	// 品质系数：白色=1.0 绿色=1.3 蓝色=1.6 紫色=1.9 橙色=2.2;
	// level = 需要强化到的等级;

	// 新公式;
	// 装备品质系数 * (level * level) + A * level + b;
	float fQuaCoe = _row->getFloatData("quaCoe");
	float aCoe = _row->getFloatData("aCoe");
	float bCoe = _row->getFloatData("bCoe");

	//int nCost = (int)(fQuaCoe * (aCoe * (_info->nadvLvl+1) + bCoe));
	int nCost = (int)(fQuaCoe * (_info->nadvLvl)*(_info->nadvLvl) + aCoe*(_info->nadvLvl) + bCoe);
	return nCost;
}

void SmithyModel::loadItemSource( const int nToolTemplateId )
{
	// 1.无数据或需刷新时重新加载;
	if (m_nEquipSourceToolTemplateId != nToolTemplateId || m_vecEquipSourceItems.empty())
	{
		RowData* _row = DataManager::getInstance()->searchToolById(nToolTemplateId);

		// 有可能templateId查不到，可能这是一件进阶后的装备;
		// 虽然逻辑上进阶后装备Id不可能传到这里，但是没必要报错，返回就好了;
		if (_row == nullptr)
		{
			return;
		}
		
		// 清理旧数据并保存当前templateId;
		clearEquipSourceItems();
		m_nEquipSourceToolTemplateId = nToolTemplateId;

		// 查询掉落途径;
		std::string dropMethod = _row->getStringData("dropMethod");//	多个掉落通过|隔开
		vector<int> vecMethods;
		StringFormat::parseCsvStringVec(dropMethod, vecMethods);
		for (auto iter : vecMethods)
		{
			if (iter >= TOOL_DROP_SOURCE_NONE)
			{
				switch (iter)
				{
				case TOOL_DROP_SOURCE_STAGE:	//	7-关卡
					{
						loadGuanqiaItems(nToolTemplateId);
					}
					break;
				default:
					{
						EQUIP_SOURCE_ITEM sourceItem;
						sourceItem.dropMethod = iter;
						m_vecEquipSourceItems.push_back(sourceItem);
					}
					break;
				}			
			}		
		}
	}

	// 2.刷新UI;

}

void SmithyModel::loadGuanqiaItems(const int nToolTemplateId)
{
	CCLOG("SmithyModel::loadGuanqiaItems begin %f", getCurrTime());

	// 获取整表数据;
	DataTable*  dataTable = DataManager::getInstance()->getDTBarrier();
	multimap<int, RowData>& allItems = dataTable->getTableData();
	for (auto iter = allItems.begin(); iter != allItems.end(); iter++)
	{
		std::string rewardShow = iter->second.getStringData("rewardShow");
		vector<int> vecTools;
		StringFormat::parseCsvStringVec(rewardShow, vecTools);
		for (auto iterTool : vecTools)
		{
			if (iterTool == nToolTemplateId)
			{
				//关卡id、关卡名字
				std::string barrierName = iter->second.getStringData("name");
				int barrierId = iter->first;

				//查找对应章节id、章节名字
				int chapterId = 0;
				DataTable*  dataTableChapter = DataManager::getInstance()->getDTChapter();
				multimap<int, RowData>& allChapters = dataTableChapter->getTableData();
				for (auto iter = allChapters.begin(); iter != allChapters.end(); iter++)
				{
					std::string rewardShow = iter->second.getStringData("raids");
					vector<int>vecBarriers;
					StringFormat::parseCsvStringVec(rewardShow, vecBarriers);

					for (auto barrId : vecBarriers)
					{
						if (barrId == barrierId)
						{
							chapterId = iter->first;
							break;
						}
					}
					if (chapterId>0)
					{
						break;
					}
				}
				//
				EQUIP_SOURCE_ITEM sourceItem;
				sourceItem.dropMethod		= TOOL_DROP_SOURCE_STAGE;
				sourceItem.barrierId		= barrierId;
				sourceItem.chapterId		= chapterId;
				sourceItem.barrierName		= barrierName;
				m_vecEquipSourceItems.push_back(sourceItem);
			}
		}
	}
	CCLOG("SmithyModel::loadGuanqiaItems end %f", getCurrTime());
}

vector<const TOOL_INFO*> SmithyModel::getQHMaterial_HorseBook( const int nToolId, int nNeedCount, vector<const TOOL_INFO*>* vcExcept /*= nullptr*/ )
{
	vector<const TOOL_INFO*> vcMaterials;

	// 类型限定;
	EQUIP_ITEM_TYPE equType = getEquType(nToolId);
	if (equType < EQUIP_ITEM_TYPE::EQUIP_ITEM_TYPE_HORSE)
		return vcMaterials;

	// 读道具表，确定道具种类;
	int nId = -1;
	if (equType == EQUIP_ITEM_TYPE::EQUIP_ITEM_TYPE_HORSE)
		nId = 10109;
	else if (equType == EQUIP_ITEM_TYPE::EQUIP_ITEM_TYPE_BOOK)
		nId = 10110;

	if (nId == -1)
		return vcMaterials;

	vector<int>  vcMaterialType;
	RowData* _row = DataManager::getInstance()->searchToolSystemById(nId);
	CCASSERT(_row != nullptr, "");
	StringFormat::parseCsvStringVec(_row->getStringData("value"), vcMaterialType);

	// 先对type排序;
	sort(vcMaterialType.begin(), vcMaterialType.end(), [=](int type1, int type2){
		RowData* _row1 = DataManager::getInstance()->searchToolById(type1);
		RowData* _row2 = DataManager::getInstance()->searchToolById(type2);
		if (_row1 != nullptr && _row2 != nullptr)
		{
			return _row1->getIntData("itemQua") <_row2->getIntData("itemQua");
		}
		return true;
	});

	// 从背包中筛选指定的材料;
	for_each(vcMaterialType.begin(), vcMaterialType.end(), [=, &vcMaterials](int materialTypeId){
		int nCount = ToolModel::getInstance()->searchNumByTemplateId(materialTypeId);
		if (nCount > 0)
		{
			const TOOL_INFO* _material = ToolModel::getInstance()->GetToolInfoBytemplateId(materialTypeId);
			while (nCount--)
				vcMaterials.push_back(_material);
		}
	});

	// 没有更多材料的情况;
	if (vcMaterials.empty() || (vcExcept != nullptr && vcMaterials.size() <= vcExcept->size()))
	{
		vcMaterials.clear();
		return vcMaterials;
	}

	// 剔除外部已选中的;
	if (vcExcept != nullptr && vcExcept->size() > 0)
	{
		for (int i = 0; i < vcExcept->size(); ++i)
		{
			const TOOL_INFO* dst = vcExcept->at(i);
			auto it = find_if(vcMaterials.begin(), vcMaterials.end(), [=](const TOOL_INFO* _tool)->bool {
				return _tool == dst;
			});
			if (it != vcMaterials.end())
			{
				vcMaterials.erase(it);
			}
		}
	}

	// 根据需要数量，再次整理;
	if (nNeedCount <= 4)
	{
		// 按品质排序;
		sort(vcMaterials.begin(), vcMaterials.end(), [=](const TOOL_INFO* tool1, const TOOL_INFO* tool2)->bool {
			return tool1->nItemQua < tool2->nItemQua;
		});

		vector<const TOOL_INFO*> vcFinal;
		for (int i = 0; i < min(nNeedCount, (int)vcMaterials.size()); ++i)
			vcFinal.push_back(vcMaterials.at(i));

		return vcFinal;
	}
	return vcMaterials;
}

//装备变化参数，用于角色界面显示动画
void SmithyModel::clearEquipChangeParam()
{
	memset(&m_changeParam, 0, sizeof(SMITHY_CHANGE_PARAM));
}

void SmithyModel::setEquipChangeParam(const SMITHY_CHANGE_PARAM& changeParam)
{
	m_changeParam = changeParam;
}

const SMITHY_CHANGE_PARAM& SmithyModel::getEquipChangeParam()
{
	return m_changeParam;
}

vector<int> SmithyModel::getBarrierListByTemplateId( int nToolTemplateId )
{
	vector<int>  vcBarriers;

	// 获取整表数据;
	DataTable* dataTable = DataManager::getInstance()->getDTBarrier();
	multimap<int, RowData>& allItems = dataTable->getTableData();
	for (auto iter = allItems.begin(); iter != allItems.end(); iter++)
	{
		std::string rewardShow = iter->second.getStringData("rewardShow");
		vector<int> vecTools;
		StringFormat::parseCsvStringVec(rewardShow, vecTools);

		auto it = find_if(vecTools.begin(), vecTools.end(), [=](int toolTemplateId)->bool {
			return toolTemplateId == nToolTemplateId;
		});

		if (it != vecTools.end())
		{
			vcBarriers.push_back(iter->first);
		}
	}

	return vcBarriers;
}

bool SmithyModel::isBarrierPassedWithTemplateId( int nToolTemplateId )
{
	bool bPassed = false;

	vector<int> vcBarriers = getBarrierListByTemplateId(nToolTemplateId);
	if (vcBarriers.empty())
		return bPassed;

	for (int i = 0; i < vcBarriers.size(); ++i)
	{
		int nChapterId = ChapterModel::getInstance()->constructChapterId(vcBarriers.at(i));
		FR_ChapterStory fr;
		ChapterModel::getInstance()->getFightRecordData(fr);
		for (auto it : fr.mapPrevBarrier)
		{
			auto iter = find_if(it.second.begin(), it.second.end(), [=](map<int, FR_Barrier>::value_type frb)->bool {
				return (frb.second.nBarrierId == vcBarriers.at(i) && frb.second.nStar > 0);
			});
			if (iter != it.second.end())
			{
				bPassed = true;
				break;
			}
		}
		CC_BREAK_IF(bPassed);
	}

	return bPassed;
}

bool SmithyModel::checkEquState( const int nToolId )
{
	int nRet = isEquQHValid(nToolId);
	if (nRet != 0)
	{
		nRet = isEquJJValid(nToolId);
		if (nRet != 0)
		{
			nRet = isEquCLValid(nToolId);
		}
	}

	return (nRet == 0);
}

SMITHY_OP_TYPE SmithyModel::checkOpType( const int nToolId )
{
	SMITHY_OP_TYPE type = SMITHY_OP_TYPE_QH;

	// 强化等级达到最高时切换为进阶;
	QH_Param param;
	getQHParam(nToolId, param);
	if (param.nPreLv >= param.nMaxLv && param.bEnableJJ)
	{
		type = SMITHY_OP_TYPE_JJ;
	}

	return type;
}