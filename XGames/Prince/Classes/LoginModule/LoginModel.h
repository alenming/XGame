#ifndef LoginModel_h__
#define LoginModel_h__

#include "LoginDef.h"
#include "Utils/Observer.h"
#include <string>

using namespace std;

class LoginModel : public Observable
{

public:

	LoginModel(void);
	~LoginModel(void);

	static LoginModel*  getInstance();
	static void  destroyInstance();

public:

	//����/��ȡ��Ϸ��������Ϣ
	void  setServerParam(const ServerParam& param);
	ServerParam* getServerParam();

	//��ȡ������ɫ�����ƣ�ע�⣺���û�д�����ɫ����ȡ������ɫ����
	void setRoleName(string roleName);
	string getRoleName();

	// �������б�;
	void  updateServerZoneList(vector<ServerParam>* vcServerParam);

	// ȡ�����б�(10������);
	void  getZoneList(int nIndex, vector<ServerParam>& vcZoneList, int nNeedCount = SERVER_ZONE_DEFAULT_COUNT);

	// ȡ��ǰѡ�еķ�����;
	ServerParam*  getDefaultServer();

	//==========�����˺ŵ�¼����==========

	//�û�id
	void setUserId(int userId);
	int getUserId();

	//sessionId
	void setSessionId(const string& sessionId);
	string getSessionId();

	//==========SDK�˺ŵ�¼����==========

	// ����/��ȡU8�û�Id;
	void setU8UserId(string userId);
	string getU8UserId();

	// ����/��ȡU8�û�Id;
	void setU8UserName(string userName);
	string getU8UserName();

	//����/��ȡsdk�û�id
	void setSdkUserId(string userId);
	string getSdkUserId();

	//����/��ȡsdk_token
	void setToken(string token);
	string  getToken();

	//����/��ȡsdk�û���
	void setSdkUserName(string userName);
	string getSdkUserName();

private:

	// ���б���ȡһ�����õķ�����ȡһ����������;
	ServerParam*  getValidServer();
	
private:

	static LoginModel*  m_pInstance;

	//��Ϸ��ɫ��������Ϣ
	vector<ServerParam>  m_vcServerList;
	ServerParam  m_ServerParam;

	//�����˺Ż���Ľ�ɫ����
	string mRoleName;

	//==========�����˺ŵ�¼����==========

	//��¼���ķ������·����û�id�����ֶν����������˺ŵ�¼
	int mUserId;
	//��¼�·���sessionId�����ֶν����������˺ŵ�¼
	string mSessionId;

	//==========SDK�˺ŵ�¼����==========

	// sdk��½token;
	string mToken;
	// sdk�û�Id
	string mSdkUserId;
	//sdk�û���
	string mSdkUserName;

	// U8�û�Id;
	string mU8UserId;
	// U8�û���;
	string mU8UserName;
};

#endif // LoginModel_h__
