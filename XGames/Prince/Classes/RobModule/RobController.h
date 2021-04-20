#ifndef RobController_h__
#define RobController_h__

#include "RobDef.h"
#include "Communication/ICommListener.h"


class RobController : public Ref, public ICommListener
{
public:
	RobController();
	~RobController();

	static RobController* getInstance();
	static void destroyInstance();

	// ���������Ϣ;
	void  handleRobMainInfo();

	// ˢ�¶���;
	void  handleRefreshEnemy(int nChipId);

	// �Ӷ��¼;
	void  handleRobRecord();

	// ��ս;
	void  handleRest(int flag);

	// �ϳ�;
	void  handleCompose(const int& id, const int& flag);

	//�Ӷ�10��
	void  handleRobTen();

	// ��ս����ʱ;
	void  startRestTimer(int nRestTime);

	//��Ϣ���սӿ�
	virtual	void onRecvMessage(int nCmdID, const char* szData);


private:

	// ��������Ϣ;
	void  procRobMainInfo(const char* szData);

	// ������Ϣ;
	void  procRefreshEnemy(const char* szData);

	// �Ӷ��¼;
	void procRobRecord(const char* szData);

	// �ϳɽ��;
	void procCompose(const char* szData);

	// �Ӷ�10�ν��;
	void  procRobTenResult(const char* szData);

	// �Ӷ�������Ϣ;
	void  procRobPushUpdate(const char* szData);

	// ��ս;
	void  procRest(const char* szData);
	void  refreshCountDown(float delta);

private:
	static RobController*  m_pInstance;

	// ��սʱ��;
	int  m_nRestTime;

	// ��ս����;
	int  m_restType;
};

#endif 
