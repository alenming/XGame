#ifndef MyGroupController_h__
#define MyGroupController_h__

#include "Communication/ICommListener.h"
#include "MyGroupDataDef.h"

class MyGroupController : public ICommListener
{
public:
	static MyGroupController* getInstance();
	static void destroyInstance();

public:
	//����Э��

	/*****************���ɽ����ϵ�Э��******************/
	//�����������������
	void sendGetGroupMainMsg();

	//����������ɶ�̬������
	void sendGetDynamicEventMsg();

	//���������˳�����
	void sendExitGroupMsg();

	//���������ɢ����
	void sendDisbandGroupMsg();

	//���������޸�����, ����1.��������, ����2.��������
	void sendModifyDeclarationMsg(string content1, string content2);

	//�����޸İ���ͷ��
	void sendModifyHeadIconMsg(int iconId);
		

	/*****************���ã������ص�Э��********************/
	//�����������ð��ɼ�������
	void sendSetJoinLimitMsg(sJoinLimit limit);

	//���������������б�
	void sendGetApplicantMsg();

	//����������������, ����1.���ID������2.�Ƿ�ͬ��
	void sendCheckJoinMsg(int uId, bool isAllow);


	/*****************��Ա����Э��********************/
	//������������ְλ
	void sendAppointMsg(int uId);

	//��������ת�ð���
	void sendAbdicateMsg(int uId);

	//���������������
	void sendExpelMsg(int uId);

	
	/******************���ɽ������Э��*******************/
	//�������������
	void sendGetBuildInfoMsg(eBuildType type);

	//���������������
	void sendDonateBuildMsg(eBuildType type, eDonateType itemType);


	/******************�����̵����*********************/
	//���������̵����
	void sendGetPrestigeShopMsg();

	//�������������Ʒ
	void sendBuyProductMsg(int productId);

	/******************��������*********************/
	//���͵���Ź��б�Э��
	void sendClieckNineList(int listId);

	//����ˢ���б�Э��
	void sendRefreshList();

	//��������Ѱ����ʷ�����
	void sendXunlongHistory();

private:
	MyGroupController();
	~MyGroupController();

private:
	//����Э��

	/*****************���ɽ����ϵ�Э��******************/
	//��������������
	void parseGroupMainMsg(const char* szData);

	//�������ɶ�̬������
	void parseDynamicEventMsg(const char* szData);

	//������ɢ���˳�����������ͽ�ɢ����
	void parseExitGroupMsg(const char* szData, int cmd);

	//�����޸�����
	void parseModifyDeclarationMsg(const char* szData);

	//�����޸�ͷ��
	void parseModifyHeadIconMsg(const char* szData);


	/*****************���ã������ص�Э��********************/
	//�������ð��ɼ�������
	void parseSetJoinLimitMsg(const char* szData);

	//�����������б�
	void parseApplicantMsg(const char* szData);

	//������������
	void parseCheckJoinMsg(const char* szData);


	/*****************��Ա����Э��********************/
	//����ְλ����
	void parseAppointMsg(const char* szData);

	//����ת�ð���
	void parseAbdicateMsg(const char* szData);

	//�����������
	void parseExpelMsg(const char* szData);


	/******************���ɽ������Э��*******************/
	//�����������
	void parseBuildInfoMsg(const char* szData, int cmd);


	/******************�����̵����*********************/
	//�����̵����
	void parsePrestigeShopMsg(const char* szData);

	//����������Ʒ
	void parseBuyProductMsg(const char* szData);

	/******************�������Э��*******************/
	//���ͽ�ɢ����, ͬ��ɢ����
	//�����������, ͬ�˳�����
	//�����Զ�ת��ְλ, ͬת�ð���
	//����ְλ����, ͬ����������

	//���������������
	void parseCheckJoinPushMsg(const char* szData);

	/******************��������*********************/
	//��������Ź��б�Э��
	void parseClickNineList(const char* szData);

	//����ˢ���б�Э��
	void parseRefreshList(const char* szData);

	//����Ѱ����ʷ����
	void parseXunLongHistory(const char* szData);


public:

	virtual void onRecvMessage(int nCmdID, const char* szData);

private:

	//��ҵ�½ʱ����Ҫ����
	static MyGroupController* ms_pInstance;
};

#endif	//MyGroupController