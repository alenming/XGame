#include "LoginModel.h"

LoginModel* LoginModel::m_pInstance = nullptr;

LoginModel::LoginModel(void)
	: mUserId(-1)
	, mSdkUserId("")
	, mSdkUserName("")
	, mU8UserId("")
	, mU8UserName("")
{

}


LoginModel::~LoginModel(void)
{
}

LoginModel* LoginModel::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new LoginModel;
	}
	return m_pInstance;
}

void LoginModel::destroyInstance()
{
	if (nullptr != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void LoginModel::setServerParam( const ServerParam& param )
{
	m_ServerParam.clear();
	m_ServerParam = param;
}

ServerParam* LoginModel::getServerParam()
{
	return &m_ServerParam;
}

void LoginModel::setRoleName(string roleName)
{
	mRoleName = roleName;
}

string LoginModel::getRoleName()
{
	return mRoleName;
}

void LoginModel::setUserId(int userId)
{
	mUserId = userId;
}

int LoginModel::getUserId()
{
	return mUserId;
}

void LoginModel::setSessionId(const string& sessionId)
{
	mSessionId = sessionId;
}

string LoginModel::getSessionId()
{
	return mSessionId;
}

void LoginModel::setSdkUserId(string userId) 
{ 
	mSdkUserId = userId; 
}

string LoginModel::getSdkUserId() 
{
	return mSdkUserId; 
}

void LoginModel::setToken(string token) 
{
	mToken = token;
}

string LoginModel::getToken() 
{
	return mToken; 
}

void LoginModel::setSdkUserName(string userName)
{
	mSdkUserName = userName;
}

string LoginModel::getSdkUserName()
{
	return mSdkUserName;
}

void LoginModel::setU8UserId( string userId )
{
	mU8UserId = userId;
}

string LoginModel::getU8UserId()
{
	return mU8UserId;
}

void LoginModel::setU8UserName( string userName )
{
	mU8UserName = userName;
}

string LoginModel::getU8UserName()
{
	return mU8UserName;
}

void LoginModel::updateServerZoneList( vector<ServerParam>* vcServerParam )
{
	if (vcServerParam == nullptr)
		return;

	// ���ش�һ��;
	m_vcServerList = *vcServerParam;

	// test;
	/*for (int i = 1; i <= 105; ++i)
	{
		ServerParam param;
		param.m_nZoneIndex = m_vcServerList.size() + i;
		param.m_strServerID = _TO_STR(10+i);
		param.m_strServerIP = "127.0.0.1";
		param.m_strServerPort = "8080";
		param.m_strServerName = "ȭ�������";
		param.nServerState = SERVER_STATE::SERVER_STATE_NEW;
		m_vcServerList.push_back(param);
	}*/

	// ����ServerIndex��������(��С����);
	sort(m_vcServerList.begin(), m_vcServerList.end(), [=](ServerParam param1, ServerParam param2)->bool {
		return param1.m_nZoneIndex < param2.m_nZoneIndex;
	});

	// ������UI;
	// ֻ�ṩ��Ϣ֪ͨ������������UI������ȡ;
	ObserverParam param;
	param.id = OBS_PARAM_TYPE_SERVER_ZONE_LIST;
	int nMaxCount = m_vcServerList.rbegin()->m_nZoneIndex;
	param.updateData = (void*)(&nMaxCount);
	this->notifyObservers((void*)&param);
}

ServerParam* LoginModel::getDefaultServer()
{
	// ��ȡ�����ص�½��¼;
	string strDefaultSrvId = GetStringFromXML(LOCAL_KEY_STR_DEFAULT_SERVER_ID, "");

	// �޼�¼:��һ�ε�½���°�װ��Ϸ;
	if (strDefaultSrvId.empty())
	{
		// ȡ�б��е�һ��[����];
		return getValidServer();
	}
	// �м�¼:���жϸ÷��Ƿ����;
	else
	{
		auto iter = find_if(m_vcServerList.begin(), m_vcServerList.end(),
			[=](ServerParam param)->bool {
				return param.m_strServerID == strDefaultSrvId;
		});

		// ���ڣ�ȡ�б��е�һ��[����];
		if (iter == m_vcServerList.end())
		{
			return getValidServer();
		}
		// δ���ڣ����ظ���;
		else
		{
			return &(*iter);
		}
	}
}

ServerParam* LoginModel::getValidServer()
{
	if (m_vcServerList.empty())
		return nullptr;

	// 1. ����ѡ��״̬Ϊ[����]������;
	auto iter = find_if(m_vcServerList.rbegin(), m_vcServerList.rend(), [=](ServerParam _serverParam)->bool {
		return _serverParam.nServerState == SERVER_STATE::SERVER_STATE_NEW;
	});
	if (iter != m_vcServerList.rend())
	{
		return &(*iter);
	}

	// 2. ����[����]��ȡ�б��������һ��[����]״̬�ķ�����;
	for (int i = m_vcServerList.size()-1; i >= 0; i--)
	{
		// �ɴ�С��ȡ��һ��[����]��[����]״̬�ķ�����;
		if (m_vcServerList.at(i).nServerState != SERVER_STATE::SERVER_STATE_CLOSED)
		{
			return &m_vcServerList.at(i);
		}
	}

	// 3. ȫ��ά������ʾ����һ��;
	return &(*m_vcServerList.rbegin());
}

void LoginModel::getZoneList( int nIndex, vector<ServerParam>& vcZoneList, int nNeedCount /*= SERVER_ZONE_DEFAULT_COUNT*/ )
{
	if (m_vcServerList.empty() || nNeedCount <= 0)
		return;

	// ��ǩ��ΧУ��;
	int nTotalIndexCount = m_vcServerList.rbegin()->m_nZoneIndex / SERVER_ZONE_DEFAULT_COUNT;
	nTotalIndexCount += ((m_vcServerList.size()%SERVER_ZONE_DEFAULT_COUNT > 0) ? 1 : 0);
	assert(nIndex >= 1 && nIndex <= nTotalIndexCount);

	// ȡ��Ӧ��Χ�����з���������;
	int nGetCount = 0;
	for (int i = 0; i < m_vcServerList.size(); ++i)
	{
		if (nGetCount < nNeedCount)
		{
			if (m_vcServerList.at(i).m_nZoneIndex > (nIndex-1)*SERVER_ZONE_DEFAULT_COUNT
				&& m_vcServerList.at(i).m_nZoneIndex <= nIndex*SERVER_ZONE_DEFAULT_COUNT)
			{
				vcZoneList.push_back(m_vcServerList.at(i));
				nGetCount++;
			}
		}
		else
		{
			break;
		}
	}
}

