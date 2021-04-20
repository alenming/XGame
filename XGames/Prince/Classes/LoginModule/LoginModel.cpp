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

	// 本地存一下;
	m_vcServerList = *vcServerParam;

	// test;
	/*for (int i = 1; i <= 105; ++i)
	{
		ServerParam param;
		param.m_nZoneIndex = m_vcServerList.size() + i;
		param.m_strServerID = _TO_STR(10+i);
		param.m_strServerIP = "127.0.0.1";
		param.m_strServerPort = "8080";
		param.m_strServerName = "拳打镇关西";
		param.nServerState = SERVER_STATE::SERVER_STATE_NEW;
		m_vcServerList.push_back(param);
	}*/

	// 按照ServerIndex重新排序(由小到大);
	sort(m_vcServerList.begin(), m_vcServerList.end(), [=](ServerParam param1, ServerParam param2)->bool {
		return param1.m_nZoneIndex < param2.m_nZoneIndex;
	});

	// 更新至UI;
	// 只提供消息通知，具体内容由UI主动来取;
	ObserverParam param;
	param.id = OBS_PARAM_TYPE_SERVER_ZONE_LIST;
	int nMaxCount = m_vcServerList.rbegin()->m_nZoneIndex;
	param.updateData = (void*)(&nMaxCount);
	this->notifyObservers((void*)&param);
}

ServerParam* LoginModel::getDefaultServer()
{
	// 先取到本地登陆记录;
	string strDefaultSrvId = GetStringFromXML(LOCAL_KEY_STR_DEFAULT_SERVER_ID, "");

	// 无记录:第一次登陆或新安装游戏;
	if (strDefaultSrvId.empty())
	{
		// 取列表中第一个[新区];
		return getValidServer();
	}
	// 有记录:需判断该服是否过期;
	else
	{
		auto iter = find_if(m_vcServerList.begin(), m_vcServerList.end(),
			[=](ServerParam param)->bool {
				return param.m_strServerID == strDefaultSrvId;
		});

		// 过期，取列表中第一个[新区];
		if (iter == m_vcServerList.end())
		{
			return getValidServer();
		}
		// 未过期，返回该区;
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

	// 1. 优先选择状态为[新区]的区服;
	auto iter = find_if(m_vcServerList.rbegin(), m_vcServerList.rend(), [=](ServerParam _serverParam)->bool {
		return _serverParam.nServerState == SERVER_STATE::SERVER_STATE_NEW;
	});
	if (iter != m_vcServerList.rend())
	{
		return &(*iter);
	}

	// 2. 若无[新区]，取列表中最近的一个[开启]状态的服务器;
	for (int i = m_vcServerList.size()-1; i >= 0; i--)
	{
		// 由大到小，取得一个[最新]的[开启]状态的服务器;
		if (m_vcServerList.at(i).nServerState != SERVER_STATE::SERVER_STATE_CLOSED)
		{
			return &m_vcServerList.at(i);
		}
	}

	// 3. 全部维护，显示最新一个;
	return &(*m_vcServerList.rbegin());
}

void LoginModel::getZoneList( int nIndex, vector<ServerParam>& vcZoneList, int nNeedCount /*= SERVER_ZONE_DEFAULT_COUNT*/ )
{
	if (m_vcServerList.empty() || nNeedCount <= 0)
		return;

	// 标签范围校验;
	int nTotalIndexCount = m_vcServerList.rbegin()->m_nZoneIndex / SERVER_ZONE_DEFAULT_COUNT;
	nTotalIndexCount += ((m_vcServerList.size()%SERVER_ZONE_DEFAULT_COUNT > 0) ? 1 : 0);
	assert(nIndex >= 1 && nIndex <= nTotalIndexCount);

	// 取对应范围的所有服务器分区;
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

