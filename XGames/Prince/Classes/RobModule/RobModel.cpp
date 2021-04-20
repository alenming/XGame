#include "RobModel.h"
#include "DataManager/DataManager.h"
#include "ToolModule/ToolModel.h"

RobModel* RobModel::ms_pInstance = nullptr;

RobModel::RobModel()
{
	//m_pView = nullptr;
	m_nAvoidamount	= 0;
	m_nFightId		= 0;
	m_mapTools.clear();
}
RobModel::~RobModel()
{

}

RobModel* RobModel::getInstance()
{
	if (nullptr == ms_pInstance)
	{
		ms_pInstance = new RobModel();
	}
	return ms_pInstance;
}

void RobModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

const vector<ROB_PLAYER_INFO>& RobModel::getRobEnemyInfo()
{
	return m_vecRobEnemyInfo;
}

int RobModel::getRobCostEnergy()
{
	DataTable* dtSystem = DataManager::getInstance()->getDTToolSystem();
	RowData* data = dtSystem->searchDataById(10038);
	if (nullptr != data)
	{
		return data->getIntData("value");
	}
	return 0;
}

void RobModel::initTools()
{
	m_mapTools.clear();

	// 1. 读系统表，取得默认显示道具种类;
	RowData* _row = DataManager::getInstance()->searchToolSystemById(10095);
	if (_row != nullptr)
	{
		vector<int>  vcDefault;
		StringFormat::parseCsvStringVec(_row->getStringData("value"), vcDefault);
		for_each(vcDefault.begin(), vcDefault.end(), [=](int id){
			m_mapTools.insert(make_pair(id, map<int, int>()));
		});
	}

	// 2. 读系统表，确定道具种类范围;
	_row = DataManager::getInstance()->searchToolSystemById(10104);
	if (_row != nullptr)
	{
		vector<int>  vcAllTools;
		StringFormat::parseCsvStringVec(_row->getStringData("value"), vcAllTools);
		assert(!vcAllTools.empty());

		// 3. 读背包，在以上结果中筛选出有碎片的道具种类;
		for (auto id : vcAllTools)
		{
			// 查询合成该道具需要的所有碎片类型;
			RowData* _row = DataManager::getInstance()->searchToolById(id);
			assert(_row != nullptr);
			vector<int> vcChips;
			StringFormat::parseCsvStringVec(_row->getStringData("transNeed"), vcChips);

			// 确认碎片数量;
			bool bShow = false;
			map<int, int>  mapChips;
			for (int i = 0; i < vcChips.size(); ++i)
			{
				int nCount = ToolModel::getInstance()->searchNumByTemplateId(vcChips.at(i));
				bShow = (bShow || nCount > 0);
				mapChips.insert(make_pair(vcChips.at(i), nCount));
			}

			// 默认值总是显示;
			auto iter = m_mapTools.find(id);
			if (iter != m_mapTools.end())
			{
				iter->second = mapChips;
			}
			else if (bShow)
			{
				m_mapTools.insert(make_pair(id, mapChips));
			}
		}
	}
}

map<int, map<int, int> >* RobModel::getTools()
{
	initTools();
	return &m_mapTools;
}

map<int, int>* RobModel::getChips( const int nToolId )
{
	auto it = m_mapTools.find(nToolId);
	if (it != m_mapTools.end())
	{
		return &(it->second);
	}
	return nullptr;
}

void RobModel::updateUI( OBS_PARAM_TYPE_ROB type, void* data )
{
	ObserverParam param;
	param.id = type;
	param.updateData = data;
	this->notifyObservers((void*)&param);
}

void RobModel::updateMainInfo( Rob_Main_Info* info )
{
	if (nullptr == info)
		return;

	updateUI(OBS_PARAM_TYPE_ROB_MAIN_INFO, (void*)info);
}

void RobModel::updateEnemy( vector<ROB_PLAYER_INFO>* vcEnemy )
{
	m_vecRobEnemyInfo.clear();
	m_vecRobEnemyInfo = *vcEnemy;
	updateUI(OBS_PARAM_TYPE_ROB_REFRESH_ENEMY, (void*)vcEnemy);
}

void RobModel::getToolInfo( int nTemplateId, Rob_Tool_Detail_Info& toolInfo )
{
	toolInfo.nTemplateId = nTemplateId;
	toolInfo.nAmount = ToolModel::getInstance()->searchNumByTemplateId(nTemplateId);
	RowData* _row = DataManager::getInstance()->searchToolById(nTemplateId);
	if (nullptr != _row)
	{
		toolInfo.strName = _row->getStringData("itemName");
		toolInfo.strDesc = _row->getStringData("itemInfo");
	}
}

void RobModel::updateRobRecord( vector<ROB_RECORDS_INFO>* record )
{
	// 遍历，取道具名称;
	RowData* _row = nullptr;
	for (auto &it : *record)
	{
		_row = DataManager::getInstance()->searchToolById(it.itemId);
		if (nullptr != _row)
		{
			it.strItemName = _row->getStringData("itemName");
		}
	}
	updateUI(OBS_PARAM_TYPE_ROB_RECOED, (void*)record);
}

void RobModel::onCountDown( int sec )
{
	updateUI(OBS_PARAM_TYPE_ROB_REST_TIME, (void*)(&sec));
}

void RobModel::onComposeResult()
{
	updateUI(OBS_PARAM_TYPE_ROB_COMPOSE_RESULT, (void*)(""));
}

void RobModel::updateRobTenResult( ROB_TEN_RESULT* robResult )
{
	// 协议中免战牌没有明确指出，所以只有这样，每次判断;
	const TOOL_INFO* _info = ToolModel::getInstance()->GetToolInfoBytemplateId(10006);
	if (nullptr != _info)
	{
		setAvoidamount(_info->nstack);
	}

	// 碎片名称;
	RowData* _row = DataManager::getInstance()->searchToolById(robResult->fragId);
	if (nullptr != _row)
	{
		robResult->fragName = _row->getStringData("itemName");
	}
	updateUI(OBS_PARAM_TYPE_ROB_TEN_RESULT, (void*)(robResult));
}

bool RobModel::checkComposeValid( int nToolId )
{
	map<int, int>* mapChips = getChips(nToolId);
	if (mapChips != nullptr)
	{
		auto iter0 = find_if(mapChips->begin(), mapChips->end(), [=](map<int, int>::value_type  it)->bool{
			return it.second == 0;
		});
		return (iter0 == mapChips->end());
	}
	else
	{
		// 不清楚为什么会走到这里，从来没有重现到，逻辑上在UI能调用这里的一定能查到;
		// 暂时这么处理，至少解决线上的极少量崩溃问题;
		return false;
	}
}

int RobModel::getRestCost()
{
	return 20;
}

void RobModel::updateRobToolsAndChips()
{
	updateUI(OBS_PARAM_TYPE_ROB_PUSH_UPDATE, (void*)(""));
}

