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
	//֪ͨ��Ҫ�������ж����߼�
	noticeMainInfoLayer();
	StarsCampController::getInstance()->isShowNewCampRedFlag();
	// ˢ����������ֵ;
	m_param.nEnergy = MainModel::getInstance()->getMainParam()->mEnergy;
	m_param.nPower = MainModel::getInstance()->getMainParam()->mPower;

	// ��������ҳ��;
	LayerLevelUp* _layer = LayerLevelUp::create(&m_param, callback);
	Director::getInstance()->getRunningScene()->addChild(_layer, 99);

	// չʾ���֮���������;
	m_param.clear();
	return true;
}

void LevelUpController::confirmLevelUp( int nNewLv )
{
	// �����������ݽṹ���ݴ棬�ȸ�ģ����;
	m_param.nNewLv = nNewLv;
	m_param.nPreEnergy = MainModel::getInstance()->getMainParam()->mEnergy;
	m_param.nPrePower = MainModel::getInstance()->getMainParam()->mPower;

	// �µ���������ֵ��ʱ��δд�룬�ȵ�����checkʱ�뱣֤��д���µ���������ֵ;
	m_param.nEnergy = MainModel::getInstance()->getMainParam()->mEnergy;
	m_param.nPower = MainModel::getInstance()->getMainParam()->mPower;
}

void LevelUpController::noticeMainInfoLayer()
{
	LevelUpModel::getInstance()->noticeMainInfoLayer();
}
