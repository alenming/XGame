#include "PlayerInfoModel.h"

PlayerInfoModel* PlayerInfoModel::ms_pInstance = nullptr;
PlayerInfoModel::PlayerInfoModel()
{
	m_vecNewTools.clear();
}

PlayerInfoModel::~PlayerInfoModel()
{

}

PlayerInfoModel* PlayerInfoModel::getInstance()
{
	if(!ms_pInstance)
	{
		ms_pInstance = new PlayerInfoModel();
	}
	return ms_pInstance;
}

void PlayerInfoModel::destroyInstance()
{
	if (ms_pInstance)
	{
		delete ms_pInstance;
		ms_pInstance = nullptr;
	}
}

vector<TOOL_NEW_INFO>* PlayerInfoModel::getNewTools()
{
	return &m_vecNewTools;
}