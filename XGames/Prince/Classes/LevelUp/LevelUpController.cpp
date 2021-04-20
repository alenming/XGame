#include "LevelUpController.h"
#include "MainModule/MainModel.h"
#include "LevelUpModel.h"
#include "NewStarsCampModule/StarsCampController.h"

LevelUpController* LevelUpController::m_pInstance = nullptr;

LevelUpController::LevelUpController(void)
{
	SocketCommunicator::getInstance()->addListener(this);
}


LevelUpController::~LevelUpController(void)
{
	SocketCommunicator::getInstance()->removeListener(this);
}

LevelUpController* LevelUpController::getInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new LevelUpController;
	}

	return m_pInstance;
}

void LevelUpController::destroyInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void LevelUpController::onRecvMessage( int nCmdID, const char* szData )
{
	switch (nCmdID)
	{
	case nMAIN_CMD_LEVEL_UP:
		//procLevelMsg(szData);
		break;

	default:
		break;
	}
}

bool LevelUpController::checkLevelUp( LevelUpCallback callback/* = nullptr*/ )
{
	if (m_param.empty())
	{
		return false;
	}
	//通知需要做升级判定的逻辑
	noticeMainInfoLayer();
	StarsCampController::getInstance()->isShowNewCampRedFlag();
	// 刷新体力精力值;
	m_param.nEnergy = MainModel::getInstance()->getMainParam()->mEnergy;
	m_param.nPower = MainModel::getInstance()->getMainParam()->mPower;

	// 创建升级页面;
	LayerLevelUp* _layer = LayerLevelUp::create(&m_param, callback);
	Director::getInstance()->getRunningScene()->addChild(_layer, 99);

	// 展示完毕之后清空数据;
	m_param.clear();
	return true;
}

void LevelUpController::confirmLevelUp( int nNewLv )
{
	// 构造升级数据结构并暂存，等各模块检查;
	m_param.nNewLv = nNewLv;
	m_param.nPreEnergy = MainModel::getInstance()->getMainParam()->mEnergy;
	m_param.nPrePower = MainModel::getInstance()->getMainParam()->mPower;

	// 新的体力精力值暂时还未写入，等到调用check时须保证已写入新的体力精力值;
	m_param.nEnergy = MainModel::getInstance()->getMainParam()->mEnergy;
	m_param.nPower = MainModel::getInstance()->getMainParam()->mPower;
}

void LevelUpController::noticeMainInfoLayer()
{
	LevelUpModel::getInstance()->noticeMainInfoLayer();
}
