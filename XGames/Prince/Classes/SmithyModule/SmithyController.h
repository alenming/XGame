#ifndef SmithyController_h__
#define SmithyController_h__

#include "SmithyDef.h"
#include "Communication/ICommListener.h"

class SmithyController: public ICommListener
{
public:

	SmithyController();
	~SmithyController();

	//��Ϣ���սӿ�
	virtual	void onRecvMessage(int nCmdID, const char* szData);

public:

	// ����ǿ��;
	int handleQH(const int nToolId, const int nTimes);
	int handleQH_HorseBook(const int nToolId, vector<int>* vcMaterialTemplateIds);

	// �������;
	int handleJJ(const int nToolId, int nGold = 0);

	// ����;
	int handleCL(const int nToolId);
	int handleCLDegrade(const int nToolId, int nCostGold);

	// ����ϴ��;
	//int handleXL(const int nToolId, SMITHY_XL_TYPE type, int nTimes = 1);
	//int handleXLReplace(const int nToolId);

private:

	// ����ǿ��;
	void processEquipQHData(const char* szData);

	// ���߽���;
	void processEquipJJData(const char* szData);

	// ���ߴ������;
	void processEquipCLData(const char* szData);

	//װ��ϴ��
	//void processEquipWashData(const char* szData);

	//�滻ϴ�����
	//void processEquipWashReplaceResultData(const char* szData);

private:
	//SMITHY_XL_TYPE  m_washType;
	//int  m_washTimes;

	// ��������(����/��������);
	OBS_PARAM_TYPE_SM  m_CurCLType;
};

#endif // LoginController_h__
