#include "LevelUpModel.h"


LevelUpModel* LevelUpModel::m_pInstance = nullptr;

LevelUpModel::LevelUpModel( void )
{

}

LevelUpModel::~LevelUpModel( void )
{

}

LevelUpModel* LevelUpModel::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new LevelUpModel;
	}

	return m_pInstance;
}

void LevelUpModel::destroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}


void LevelUpModel::noticeMainInfoLayer()
{
	//计算一下开启的星盘ID

	ObserverParam param;
	param.id = NOTICE_MAINGBUILDING;
	param.updateData = nullptr;
	this->notifyObservers((void*)&param);
}

