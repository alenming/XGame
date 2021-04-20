#include "SmithyController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Json/json.h"
#include "SmithyModel.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainModel.h"
#include "SDK_TalkingData/TalkingData.h"

SmithyController::SmithyController()
	: m_CurCLType(OBS_PARAM_TYPE_EQU_CL_RESULT)
{
	SocketCommunicator::getInstance()->addListener(this);
}

SmithyController::~SmithyController()
{
	SocketCommunicator::getInstance()->removeListener(this);
}

void SmithyController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{	
	case nMAIN_CMD_EQUIP_STRENGTHEN://道具强化
	case nMAIN_CMD_EQUIP_STRENGTHEN_HORSE_BOOK:
		processEquipQHData(szData);
		break;
	case nMAIN_CMD_EQUIP_ADVANCED://道具进阶（合成）
		processEquipJJData(szData);
		break;
	case nMAIN_CMD_TOOL_EQUIP_CL:
	case nMAIN_CMD_TOOL_EQUIP_CL_DEGRADE:
		processEquipCLData(szData);
		break;
	default:
		break;
	}
	return;
}

// 道具强化;
void SmithyController::processEquipQHData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		// 更新主角数据(货币/体力/精力等);
		MainModel::getInstance()->updateData(data);

		// 更新英雄数据;
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		// 更新道具，获取变更内容;
		TOOL_INFO toolINFO;
		Json::Value items = data["itemList"];
		vector<TOOL_CHANGE_INFO> vecUpdateTools;
		ToolModel::getInstance()->updateToolJsonData(items, &vecUpdateTools);

		if (vecUpdateTools.size()>0)
		{
			vecUpdateTools.at(0).bIsCrit = (data["prbFlag"].asInt() == 1);
			auto it = find_if(vecUpdateTools.begin(), vecUpdateTools.end(), [=](TOOL_CHANGE_INFO changeInfo)->bool{
				return SmithyModel::getInstance()->getEquType(changeInfo.nid) != EQUIP_ITEM_TYPE_NONE;
			});

			if (it != vecUpdateTools.end())
			{
				SmithyModel::getInstance()->onEquQHResult(&(*it));
			}
		}

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_EQUIP_STRENGTHEN;
		HeroModel::getInstance()->notifyObservers(&obParam);
	}

}

// 道具进阶;
void SmithyController::processEquipJJData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		// 更新主角数据(货币/体力/精力等);
		MainModel::getInstance()->updateData(data);

		// 更新英雄数据;
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		// 更新道具，获取变更内容;
		TOOL_INFO toolINFO;
		Json::Value items = data["itemList"];
		vector<TOOL_CHANGE_INFO> vecUpdateTools;
		ToolModel::getInstance()->updateToolJsonData(items, &vecUpdateTools);

		for (int i = 0; i < vecUpdateTools.size(); ++i)
		{
			// 筛选出装备来更新;
			RowData* _row = DataManager::getInstance()->searchEquipmentById(vecUpdateTools.at(i).ntemplateId);
			if (_row != nullptr)
			{
				SmithyModel::getInstance()->onEquJJResult(&vecUpdateTools.at(i));
			}
		}

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_EQUIP_ADVANCED;
		HeroModel::getInstance()->notifyObservers(&obParam);
	}
}

void SmithyController::processEquipCLData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		// 更新主角数据(货币/体力/精力等);
		MainModel::getInstance()->updateData(data);

		// 更新英雄数据;
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		// 更新道具，获取变更内容;
		TOOL_INFO toolINFO;
		Json::Value items = data["itemList"];
		vector<TOOL_CHANGE_INFO> vecUpdateTools;
		ToolModel::getInstance()->updateToolJsonData(items, &vecUpdateTools);

		for (int i = 0; i < vecUpdateTools.size(); ++i)
		{
			// 筛选出装备来更新;
			RowData* _row = DataManager::getInstance()->searchEquipmentById(vecUpdateTools.at(i).ntemplateId);
			if (_row != nullptr)
			{
				SmithyModel::getInstance()->onEquCLResult(&vecUpdateTools.at(i), m_CurCLType);
			}
		}

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_TOOL_EQUIP_CL;
		HeroModel::getInstance()->notifyObservers(&obParam);
	}
}

// 装备洗炼;
/*
void SmithyController::processEquipWashData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		// 更新主角数据(货币/体力/精力等);
		MainModel::getInstance()->updateData(data);

		// 更新英雄数据;
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);

		// 更新道具(洗练石)数量;
		Json::Value itemList = data["itemList"];
		ToolModel::getInstance()->updateToolJsonData(itemList);

		// 洗练数据只显示不保存，直到用户点"替换"服务器端返回数据保存;
		Json::Value listItem = data["props"];

		// 
		if (m_washType == SMITHY_XL_TYPE_GOLD)
		{
			TalkingData::onPurchase("金币洗炼", m_washTimes, SmithyModel::getInstance()->getXL_CostMoney(SMITHY_XL_TYPE_GOLD));
		}
		
		// 增量;
		TOOL_INFO  toolINFO;
		toolINFO.nid		= listItem["itemId"].asInt();
		toolINFO.ngroAtk	= listItem["groAtk"].asInt();			// 攻击;
		toolINFO.ngroHp		= listItem["groHp"].asInt();			// 血量;
		toolINFO.ngroPdd	= listItem["groPdd"].asInt();			// 物防;
		toolINFO.ngroMdd	= listItem["groMdd"].asInt();			// 法防;

		toolINFO.fgroCtrPrb	= listItem["groCtrPrb"].asDouble();		// 连击率;
		toolINFO.fgroCriCoe	= listItem["groCriCoe"].asDouble();		// 暴伤;
		toolINFO.fgroPddCoe	= listItem["groPddCoe"].asDouble();		// 物防加成;
		toolINFO.fgroMddCoe	= listItem["groMddCoe"].asDouble();		// 法防加成;

		toolINFO.fgroCriPrb	= listItem["groCriPrb"].asDouble();		// 暴击率;
		toolINFO.fgroCriRes	= listItem["groCriRes"].asDouble();		// 抗暴率;
		toolINFO.fgroHit	= listItem["groHit"].asDouble();		// 命中率;
		toolINFO.fgroMiss	= listItem["groMiss"].asDouble();		// 闪避率;

		SmithyModel::getInstance()->onEquXLResult(&toolINFO);
	}
}
*/

// 替换洗炼结果;
/*
void SmithyController::processEquipWashReplaceResultData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		Json::Value data = root["data"];

		// 更新主角数据(货币/体力/精力等);
		MainModel::getInstance()->updateData(data);

		// 更新英雄数据;
		Json::Value petLists = data["petList"];
		HeroModel::getInstance()->updateData(petLists);
		
		//itemList
		Json::Value items = data["itemList"];
		SmithyModel::getInstance()->clearToolsAttri();
		vector<TOOL_CHANGE_INFO> &vecUpdateTools = SmithyModel::getInstance()->getUpdateToolsAttri();
		ToolModel::getInstance()->updateToolJsonData(items, &vecUpdateTools);

		assert(vecUpdateTools.size() != 0);
		SmithyModel::getInstance()->onEquXLReplaceResult(&vecUpdateTools.at(0));

		ObserverParam obParam;
		obParam.id = nMAIN_CMD_TOOL_EQUIP_WASH_RESULT;
		HeroModel::getInstance()->notifyObservers(&obParam);
	}
}
*/

int SmithyController::handleQH( const int nToolId, const int nTimes )
{
	// 1. 条件判定;
	int nRet = SmithyModel::getInstance()->isEquQHValid(nToolId);
	if (0 == nRet)
	{
		Json::FastWriter  writer;
		Json::Value root;
		root["itemId"] = nToolId;
		root["autoflag"] = (nTimes != 1) ? 1 : 0;
		std::string json_file = writer.write(root);
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_EQUIP_STRENGTHEN, json_file.c_str());
	}
	return nRet;
}

int SmithyController::handleQH_HorseBook(const int nToolId, vector<int>* vcMaterialTemplateIds)
{
	Json::FastWriter  writer;
	Json::Value root;
	root["itemId"] = nToolId;

	Json::Value itemList;
	for_each(vcMaterialTemplateIds->begin(), vcMaterialTemplateIds->end(), [=, &root](int templateId){
		root["itemList"].append(templateId);
	});

	std::string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_EQUIP_STRENGTHEN_HORSE_BOOK, json_file.c_str());
	return 0;
}

int SmithyController::handleJJ( const int nToolId, int nGold /*= 0*/ )
{
	//const TOOL_INFO* _toolInfo = ToolModel::getInstance()->GetToolInfo(nToolId);
	//CCASSERT(_toolInfo != nullptr, "");

	int nRet = 0;

	// 金币进阶是在已经判断过的情况下直接发起操作;
	if (nGold == 0)
	{
		nRet = SmithyModel::getInstance()->isEquJJValid(nToolId);
	}
	if (0 == nRet)
	{
		Json::FastWriter  writer;
		Json::Value root;
		root["itemId"] = nToolId;//->ntemplateId;
		std::string json_file = writer.write(root);
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_EQUIP_ADVANCED, json_file.c_str());
	}
	else if (-4 == nRet || -6 == nRet)
	{
		// 进阶材料不足时，有额外判断是否满足金币进阶条件;
		// 掉落关卡是否全部通关;
		// 马书是积分不足;
	}
	return nRet;
}

int SmithyController::handleCL( const int nToolId )
{
	//nMAIN_CMD_TOOL_EQUIP_CL
	Json::FastWriter  writer;
	Json::Value root;
	root["itemId"] = nToolId;
	std::string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOOL_EQUIP_CL, json_file.c_str());
	m_CurCLType = OBS_PARAM_TYPE_EQU_CL_RESULT;
	return 1;
}

int SmithyController::handleCLDegrade( const int nToolId, int nCostGold )
{
	if (MainModel::getInstance()->getMainParam()->mGold < nCostGold)
	{
		//CustomTips::show("金币不足");
		return 0;
	}

	Json::FastWriter  writer;
	Json::Value root;
	root["itemId"] = nToolId;
	std::string json_file = writer.write(root);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOOL_EQUIP_CL_DEGRADE, json_file.c_str());
	m_CurCLType = OBS_PARAM_TYPE_EQU_CL_DEGRADE_RESULT;
	return 1;
}

/*
int SmithyController::handleXL( const int nToolId, SMITHY_XL_TYPE type, int nTimes )
{
	int nRet = SmithyModel::getInstance()->isEquXLValid(nToolId, type);
	if (0 == nRet)
	{
		//@param itemId;
		//@param flag:1-银币洗练,2-金币洗练;
		//@param times:洗练次数 1或5(5次已废弃);
		// 又增加10次洗炼了，还好没删参数，哎;
		// 12/07/2015 by Phil @zcjoy
		Json::FastWriter  writer;
		Json::Value  root;
		root["itemId"] = nToolId;
		m_washType = type;
		root["flag"] = (m_washType == SMITHY_XL_TYPE_GOLD) ? 2 : 1;
		m_washTimes = nTimes;
		root["times"] = m_washTimes;
		std::string json_file = writer.write(root);
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOOL_EQUIP_WASH, json_file.c_str());
	}
	return nRet;
}
*/

/*
int SmithyController::handleXLReplace( const int nToolId )
{
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfo(nToolId);
	if (nullptr != _info)
	{
		Json::FastWriter  writer;
		Json::Value root;
		root["itemId"] = nToolId;
		std::string json_file = writer.write(root);
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_TOOL_EQUIP_WASH_RESULT, json_file.c_str());
		return 0;
	}
	return -1;
}
*/
