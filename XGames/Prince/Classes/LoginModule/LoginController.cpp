#include "LoginController.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/HttpCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Json/json.h"
#include "MainModule/MainModel.h"
#include "../ToolModule/ToolModel.h"
#include "Common/Common.h"
#include "../Temp/CustomPop.h"
#include "cocos2d.h"
#include "GuideCenter/GuideManager.h"
#include "MainModule/MainCityLoadingScene.h"
#include "SceneLogin.h"
#include "FormationModule/FormationModel.h"
#include "FriendModule/ChatController.h"
#include "Utils/LocalTimer.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "Communication/HttpPublisher.h"
#include "LoginModel.h"
#include "DataCollection.h"
#include "Temp/CustomPop.h"
#include "Widget/LayerLoading.h"
#include "Utils/SystemUtils.h"
#include "FirstRecharge/FirstRechargeModel.h"
#include "SceneLogout.h"
#include "VIPModule/VipModel.h"
#include "Utils/MD5/MD5Checksum.h"
#include "MysteryShop/MysteryShopModel.h"
#include "ReYunSDK/ReyunController.h"

using namespace cocos2d;

LoginController* LoginController::ms_pInstance = nullptr;

LoginController::LoginController()
	: m_pView(nullptr)
	, mIsRoleLoginSuccess(false)
	, mIsLogoutState(false)
	, m_isAutoLogin(false)
	, m_strTmpUName("")
	, m_strTmpUPwd("")
	, m_iTryUID(0)
{
	SocketCommunicator::getInstance()->addListener(this);
	HttpCommunicator::getInstance()->addListener(this);
	CCLOG("++++++++++++ logincontroller created ++++++++++++");
}

LoginController::~LoginController()
{
	SocketCommunicator::getInstance()->removeListener(this);
	HttpCommunicator::getInstance()->removeListener(this);

	CCLOG("++++++++++++ logincontroller destroy ++++++++++++");

#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SDKComponent_ios::releaseInstance();
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	SDKComponent::releaseInstance();
#endif
	
#endif

}

LoginController* LoginController::getInstance()
{
	if (nullptr == ms_pInstance)
	{
		ms_pInstance = new LoginController();
	}
	return ms_pInstance;
}

void LoginController::onSetView( ILoginView* pView )
{
	m_pView = pView;
}

void LoginController::onRemoveView(ILoginView* pView)
{
	if (pView == m_pView) //该判定解决在登录界面注销切换到登录界面的问题
	{
		m_pView = nullptr;
	}
}

void LoginController::onRecvMessage(int nCmdID, const char* szData)
{
	switch (nCmdID)
	{
	case nNET_CMD_NET_HTTP_ERROR://网络http错误
		{
			if (m_pView)
			{
				m_pView->onRecvLoginData(nCmdID, "");
			}
		}
		break;
	case nMAIN_CMD_LOGIN:
	case nMAIN_CMD_FIRST_TRY_GAME:
	case nMAIN_CMD_TRY_GAME:
	case nMAIN_CMD_ACTIVATE:
	case nMAIN_CMD_REGISTER:
	case nMAIN_CMD_MODIFY_PWD:
		processLoginData(szData, nCmdID);
		break;

	case nMAIN_CMD_GET_ZONE_LIST:
		processZoneListData(szData);
		break;

	//角色登录与角色创建回包一致
	case nMAIN_CMD_LOGIN_ROLE:
	case nMAIN_CMD_SDK_LOGIN_ROLE:
		processLoginRoleData(szData);
		break;
	case nMAIN_CMD_CREATE_ROLE:
	case nMAIN_CMD_SDK_CREATE_ROLE:
		{
			processLoginRoleData(szData, true);
		}
		break;

	case nMAIN_CMD_HERO_INFO:
		proccessHeroListData(szData);
		break;

	case nMAIN_CMD_GET_ALL_TOOL_INFO:// 获取玩家所有道具信息
		processGetToolsData(szData);
		break;

	case nMAIN_CMD_RANDOM_NAMES:
		processRandomNamesData(szData);
		break;

	case nNET_CMD_RECONNECT:
		{
			processReconnectRet(szData);
		}
		break;

	case nMAIN_CMD_PAY_RESULT:
		{
			processPayResult(szData);
		}
		break;
	case nNET_CMD_LOGOUT:
		{
			processLogoutMsg(szData);
		}
		break;

	case nNET_CMD_REPEAT_LOGIN:
		{
			processRepeatLoginMsg(szData);
		}
		break;

#if (LOGIN_WITH_TT == 1)
	case nMAIN_CMD_TT_CHECK_ACCOUNT:
		{
			onCheckUserNameRet(szData);
		}
		break;
#endif

	default:
		break;
	}
	return;
}

void LoginController::sendGetHeroDataMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_HERO_INFO, "");
}

void LoginController::sendGetToolsMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_GET_ALL_TOOL_INFO, "");
}

void LoginController::sendGetRandomNameMsg(bool isMale)
{
	Json::FastWriter  writer;
	Json::Value val;
	val["sex"] = isMale ? 0 : 1;
	std::string  json_file = writer.write(val);
	SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_RANDOM_NAMES, json_file.c_str());
}

void LoginController::sendLogoutMsg()
{
	SocketCommunicator::getInstance()->sendMessage(nNET_CMD_LOGOUT, "");
}

void LoginController::processLoginData( const char* szData, int nCmd )
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		int nResult = root["result"].asInt();
		string strMsg = root["msg"].asString();

		if(LOGIN_RESULT_SUCCESS == nResult)
		{
			Json::Value data = root["data"];
			int userId = data["userID"].asInt();
			string sessionId = data["token"].asString();
			string sdkUserID = data["sdkUserID"].asString();
			string username = data["username"].asString();
			string sdkUserName = data["sdkUserName"].asString();

			//保存userId，保存sessionId
			LoginModel::getInstance()->setUserId(userId);
			LoginModel::getInstance()->setSessionId(sessionId);
			//LoginModel::getInstance()->setToken(result->token);
			LoginModel::getInstance()->setU8UserId(TO_STR(userId));
			LoginModel::getInstance()->setU8UserName(username);
			LoginModel::getInstance()->setSdkUserId(sdkUserID);
			LoginModel::getInstance()->setSdkUserName(sdkUserName);


			mIsLogoutState = false;
		}

		if (m_pView)
		{
			if(nCmd == nMAIN_CMD_LOGIN)
			{
				m_pView->onRecvLoginData(nResult, strMsg.c_str());
			}
			else if(nCmd == nMAIN_CMD_FIRST_TRY_GAME || nCmd == nMAIN_CMD_TRY_GAME)
			{
				if (SystemUtils::getChannelId() == CHANNEL_ID_APPSTORE) //IOS正版热云注册登入统计上报
				{
					if(m_iTryUID == 0)
					{
						//试玩注册之后自动登入
						ReyunController::ReYunRegister(TO_STR(LoginModel::getInstance()->getUserId()));
						ReyunController::ReYunLogin(TO_STR(LoginModel::getInstance()->getUserId()));
					}
					else
					{
						//试玩登录
						ReyunController::ReYunLogin(TO_STR(LoginModel::getInstance()->getUserId()));
					}
				}

				m_pView->onRecvTryGameData(nResult, strMsg.c_str());
			}
			else if(nCmd == nMAIN_CMD_REGISTER)
			{
				m_pView->onRecvRegisterData(nResult, strMsg.c_str());
			}
			else if(nCmd == nMAIN_CMD_ACTIVATE)
			{
				if(nResult == ACTIVATE_RESULT_SUCCESS)
				{
					//清空试玩identify
					SetStringToXML(LOCAL_KEY_STR_TRY_IDENTIFY, "");
				}
				m_pView->onRecvActivateData(nResult, strMsg.c_str());
			}
			else if(nCmd == nMAIN_CMD_MODIFY_PWD)
			{
				m_pView->onRecvModifyPwdData(nResult, strMsg.c_str());	
			}
		}

		LayerWaiting::hideWaiting();
	}
}

void LoginController::processZoneListData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		LayerWaiting::hideWaiting();

		// 解析分区列表;
		Json::Value data = root["data"];
		vector<ServerParam>  vcServerParam;
		for (int i = 0; i < data.size(); ++i)
		{
			Json::Value single = data[i];
			ServerParam param;
			param.m_nZoneIndex = single["index"].asInt();
			param.m_strServerID = _TO_STR(single["id"].asInt());
			param.m_strServerName = STR_ANSI(single["name"].asString());
			param.m_strServerIP = single["ip"].asString();
			param.m_strServerPort = _TO_STR(single["port"].asInt());
			param.nServerState = (SERVER_STATE)(single["status"].asInt());
			vcServerParam.push_back(param);
		}
		LoginModel::getInstance()->updateServerZoneList(&vcServerParam);
	}
}

void LoginController::processLoginRoleData( const char* szData, bool isCreateRoleMsg )
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		bool hasRole = root["hasRole"].asBool();
		Json::Value roleData = root["data"];

		if (hasRole)
		{
			//聊天消息Controller，登录成功后 后台会推送聊天消息
			ChatController::getInstance();
			//标记角色登录成功
			mIsRoleLoginSuccess = true;

			MainModel::getInstance()->initData(roleData);

			//写入充值总额；fix by rain:每次登录同步玩家当前服务器记录的充值额度，防止支付推送消息没有收到导致玩家一直不能支付
			int totalPay = roleData["totalPay"].asInt();
			SetIntegerToXML(LOCAL_KEY_STR_TOTAL_PAY_SRV, totalPay);

			//获取玩家上次登录的角色id
			string oldRoleId = GetStringFromXML(LOCAL_KEY_STR_ROLE_ID, "");
			//渠道号+分服id+角色id构成玩家的唯一标识
			stringstream ossNewRoleId;
			ossNewRoleId << SystemUtils::getChannelId() << LoginModel::getInstance()->getServerParam()->m_strServerID
				<< MainModel::getInstance()->getMainParam()->mRoleId;
			string newRoleId = ossNewRoleId.str();
			if (oldRoleId != newRoleId)
			{
				//写入新登录的角色ID
				SetStringToXML(LOCAL_KEY_STR_ROLE_ID, newRoleId);
				//将战斗加速与自动战斗恢复到默认值
				SetIntegerToXML(LOCAL_KEY_INT_STR_FIGHT_SPEED, 1);
				SetBoolToXML(LOCAL_KEY_BOOL_AUTO_FIGHT, false);

				// 写入本地充值总额; fix by rain:玩家第一次登录时记录该，解决充值未到账退出游戏再登录仍然可以充值的问题
				SetIntegerToXML(LOCAL_KEY_STR_TOTAL_PAY_LOCAL, totalPay);
			}

			//初始化引导
			Json::Value guideData = roleData["guideMap"];
			GuideManager::getInstance()->initGuideProcess(guideData);

			//角色登录成功，无新手引导直接跳转到主城界面
			if (!GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_LOGIN))
			{
				Director::getInstance()->replaceScene(MainCityLoadingScene::create());
			}
			// 更新阵型;
			Json::Value matrixData = roleData["matrix"];
			vector<int> vcFormation;
			for (size_t i = 0; i < matrixData.size(); i++)
			{
				vcFormation.push_back(matrixData[i].asInt());
			}
			FormationModel::getInstance()->setFormation(vcFormation);

			//启动时间定时器，同步服务器时间
			time_t serverTime = roleData["timestamp"].asUInt();
			LocalTimer::getInstance()->startTimer(serverTime);

			//神秘商店状态
			auto timeM = roleData["guankaStoreTime"].asInt();
			MysteryShopModel::getInstance()->setStamp(timeM);
			if (timeM >LocalTimer::getInstance()->getCurServerTime())//大于服务器时间
			{
				MysteryShopModel::getInstance()->setIsMainShow(true);//主城显示
				MysteryShopModel::getInstance()->setIsInterShow(false); //关卡场景不显示
			}
			

			//向天拓数据统计平台发送角色登录消息
			HttpPublisher::getInstance()->sendLoginGameMsg();

			//TalkingData统计玩家帐户
			TalkingData::loginTalkingData(
				newRoleId,
				LoginModel::getInstance()->getServerParam()->m_strServerID,
				MainModel::getInstance()->getMainParam()->mName);

			if (isCreateRoleMsg)
			{
				//向天拓数据统计平台发送角色创建信息
				HttpPublisher::getInstance()->sendCreateRoleMsg();

				//创建角色数据上报
#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_TT == 0)
				if (SystemUtils::getChannelId() == CHANNEL_ID_U9)
				{
					submitExtraData(U8ExtraGameData::TYPE_CREATE_ROLE);
				}
#endif
			}
		}
		else
		{			
			//解析名字并缓存
			mNameParam.vecMaleNames.clear();
			mNameParam.vecFemaleNames.clear();
			Json::Value maleNames = roleData["boyNames"];
			for (size_t i = 0; i < maleNames.size(); i++)
			{
				mNameParam.vecMaleNames.push_back(maleNames[i].asString());
			}
			Json::Value femalNames = roleData["grilNames"];
			for (size_t i = 0; i < femalNames.size(); i++)
			{
				mNameParam.vecFemaleNames.push_back(femalNames[i].asString());
			}

			//通知UI创建角色
			if (m_pView != nullptr)
			{
				m_pView->onRecvCreateRoleData();
			}
		}
	}
}

void LoginController::proccessHeroListData(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			//Director::getInstance()->replaceScene(SceneLogin::create());
			return;
		}

		Json::Value heroList = root["data"];
		HeroModel::getInstance()->initData(heroList);

		//登陆数据上报
		#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_TT == 0)
			submitExtraData(U8ExtraGameData::TYPE_ENTER_GAME);
		#endif
	}
}

void LoginController::submitExtraData(int type)
{
#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_TT == 0)
	U8ExtraGameData  data;
	data.dataType = type;
	data.roleId = _TO_STR(MainModel::getInstance()->getMainParam()->mRoleId);
	data.roleName = MainModel::getInstance()->getMainParam()->mName;
	data.roleLevel = HeroModel::getInstance()->getMainHeroLevel();
	data.serverId = atoi(LoginModel::getInstance()->getServerParam()->m_strServerID.c_str());
	data.serverName = LoginModel::getInstance()->getServerParam()->m_strServerName;
	data.moneyNum = MainModel::getInstance()->getMainParam()->mGold;
	data.roleVip = MainModel::getInstance()->getMainParam()->mVipLevel;
	data.roleGroup = MainModel::getInstance()->getMainParam()->mGroupName;

	CCLOG("VipLayer:: data.dataType = %d", data.dataType);
	CCLOG("VipLayer:: data.roleId = %s", data.roleId.c_str());
	CCLOG("VipLayer:: data.roleName = %s", data.roleName.c_str());
	CCLOG("VipLayer:: data.roleLevel = %d", data.roleLevel);
	CCLOG("VipLayer:: data.serverId = %d", data.serverId);
	CCLOG("VipLayer:: data.serverName = %s", data.serverName.c_str());
	CCLOG("VipLayer:: data.moneyNum = %d", data.moneyNum);
	CCLOG("VipLayer:: data.roleVip = %d", data.roleVip);
	CCLOG("VipLayer:: data.roleGroup = %s", data.roleGroup.c_str());

	SDKComponent::getInstance()->submitExtraData(&data);
#endif
}

void LoginController::processGetToolsData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			//Director::getInstance()->replaceScene(SceneLogin::create());
			return;
		}

		Json::Value data = root["data"];
		int pkgUsesize = data["pkgUsesize"].asInt();
		int pkgsize = data["pkgsize"].asInt();
		ToolModel::getInstance()->updateBackpackUsedCount(pkgUsesize, pkgsize);
		
		Json::Value itemList = data["itemList"];
		ToolModel::getInstance()->processToolJsonData(itemList);
		ToolModel::getInstance()->setIsInit(true);
	}
}

void LoginController::processRandomNamesData(const char* szData)
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		if (throwException(root))
		{
			return;
		}

		NameParam nameParam;

		Json::Value data = root["data"];
		Json::Value nameList = data["nameList"];
		int sex = data["sex"].asInt();
		if (sex == 0) //男性
		{
			for (size_t i = 0; i < nameList.size(); i++)
			{
				nameParam.vecMaleNames.push_back(nameList[i].asString());
			}
		}
		else //女性
		{
			for (size_t i = 0; i < nameList.size(); i++)
			{
				nameParam.vecFemaleNames.push_back(nameList[i].asString());
			}
		}

		ObserverParam observerParam;
		observerParam.id = nMAIN_CMD_RANDOM_NAMES;
		observerParam.updateData = &nameParam;
		MainModel::getInstance()->notifyObservers(&observerParam);
	}
}

void LoginController::processReconnectRet( const char* szData )
{
	Json::Reader read;
	Json::Value  root;

	if (read.parse(szData, root))
	{
		bool bResult = root["result"].asBool();

		// 成功: 重连成功，后续不做任何处理;
		// 失败: 重连成功，但因超时或其他原因需重新登陆;
		if (!bResult)
		{
			//隔一帧销毁菊花，防止出现重连成功后发送缓存消息导致的菊花
			auto hideWaitingCallBack = [this](float dt)->void
			{
				LayerWaiting::hideWaiting();
				Director::getInstance()->getScheduler()->unschedule("LoginController::processReconnectRet", this);
			};
			Director::getInstance()->getScheduler()->schedule(hideWaitingCallBack, this, 0, 0, 0, false, "LoginController::processReconnectRet");


			auto callBack = [this]( Ref* pSender, CustomRetType type )->void
			{
				switch (type)
				{
				case RET_TYPE_OK:
				case RET_TYPE_CANCEL:
				case RET_TYPE_CLOSE:
					Director::getInstance()->replaceScene(SceneLogout::create());
					break;
				default:
					break;
				}
			};

			//CustomPop::show("登录失效，点击确定返回登录", callBack);
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10283), callBack);
			
		}
	}
}

NameParam* LoginController::getNameParam()
{
	return &mNameParam;
}

void LoginController::createRole( const string& name, int templetId )
{
	LoginModel::getInstance()->setRoleName(name);

	Json::FastWriter  writer;
	Json::Value jsonValue;

	jsonValue["petId"] = templetId;
	jsonValue["nickName"] = name;
	jsonValue["channelId"] = SystemUtils::getChannelId();
	jsonValue["subChannelId"] = SystemUtils::getSubChannel();

#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)

		jsonValue["userId"] = atoi(LoginModel::getInstance()->getU8UserId().c_str());
		jsonValue["token"] = LoginModel::getInstance()->getToken();
		std::string  json_file = writer.write(jsonValue);
		CCLOG("++++++++++ createRole:%s", json_file.c_str());
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_SDK_CREATE_ROLE, json_file.c_str());

#else

		jsonValue["userId"] = LoginModel::getInstance()->getUserId();
		jsonValue["token"] = LoginModel::getInstance()->getSessionId();
		std::string  json_file = writer.write(jsonValue);
		SocketCommunicator::getInstance()->sendMessage(nMAIN_CMD_CREATE_ROLE, json_file.c_str());

#endif
}

void LoginController::loginRole()
{
	Json::FastWriter  writer;
	Json::Value val;

	int nCmdID = nMAIN_CMD_LOGIN_ROLE;

#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)

		val["userId"] = atoi(LoginModel::getInstance()->getU8UserId().c_str());
		val["token"] = LoginModel::getInstance()->getToken();
		val["subChannelId"] = SystemUtils::getSubChannel();
		nCmdID = nMAIN_CMD_SDK_LOGIN_ROLE;

#else

		val["userId"] = LoginModel::getInstance()->getUserId();
		val["token"] = LoginModel::getInstance()->getSessionId();
		val["subChannelId"] = SystemUtils::getSubChannel();
		nCmdID = nMAIN_CMD_LOGIN_ROLE;

#endif

	//角色登录未成功，发送角色登录消息
	//角色登录成功，发送断线重连消息
	if (mIsRoleLoginSuccess)
	{
		nCmdID = nNET_CMD_RECONNECT;
	}

	std::string json_file = writer.write(val);
	CCLOG("++++++++++ loginRole:%s", json_file.c_str());
	SocketCommunicator::getInstance()->sendMessage(nCmdID, json_file.c_str());
}

#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)
void LoginController::onSdkLoginAccountResult( U8LoginResult* result )
{
	SetStringToXML(LOCAL_KEY_STR_USERNAME, result->username);
	LoginModel::getInstance()->setToken(result->token);
	LoginModel::getInstance()->setU8UserId(result->userId);
	LoginModel::getInstance()->setU8UserName(result->username);
	LoginModel::getInstance()->setSdkUserId(result->sdkUserId);
	LoginModel::getInstance()->setSdkUserName(result->sdkUsername);
	
	CCLOG("++++++++++ u8UserId: %s ++++++++++", result->userId.c_str());
	CCLOG("++++++++++ u8UserName: %s ++++++++++", result->username.c_str());
	CCLOG("++++++++++ sdkUserId: %s ++++++++++", result->sdkUserId.c_str());
	CCLOG("++++++++++ sdkUserName: %s ++++++++++", result->sdkUsername.c_str());

	if (m_pView)
	{
		CCLOG("++++++++++ LOGIN_RESULT_SUCCESS ++++++++++");
		m_pView->onRecvLoginData(LOGIN_RESULT_SUCCESS, "");
	}
	else
	{
		CCLOG("++++++++++ m_isAutoLogin = true ++++++++++");
		m_isAutoLogin = true;
	}
}
#endif


void LoginController::loginAcount(const char* szID, const char* szPwd)
{
#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (SystemUtils::getChannelId() == CHANNEL_ID_YSDK && m_isAutoLogin) //YSDK有可能自动登录
	{
		if (m_pView)
		{
			m_pView->onRecvLoginData(LOGIN_RESULT_SUCCESS, "");
		}
	}
	else
	{
		SDKComponent::getInstance()->login();
	}
#else
        SDKComponent_ios::getInstance()->login();
#endif
    
#else
	Json::FastWriter  writer;
	Json::Value account;

#if (LOGIN_WITH_TT == 1)

	string uname = szID;
	string upwd = MD5Checksum::GetMD5OfString(string(szID) + string(szPwd));
	string ulife = _TO_STR(1);
	string umail = "";
	string utime = string(_TO_STR(time(0)));
	string gid = _TO_STR(SystemUtils::getTianTuoGid());
	string umacid = getIdentify();
	string key = SystemUtils::getTianTuoKey();
	string sign = MD5Checksum::GetMD5OfString(uname + upwd + ulife + gid + utime + umacid + key);

	account["ac"] = 4;
	account["uname"] = uname.c_str();
	account["upwd"] = upwd.c_str();
	account["ulife"] = ulife.c_str();
	account["umail"] = umail.c_str();
	account["utime"] = utime.c_str();
	account["gid"] = gid.c_str();
	account["umacid"] = umacid.c_str();
	account["sign"] = sign.c_str();
	account["appID"] = CONTACT_INT_APPID;
	account["channelID"] = SystemUtils::getChannelId();

#else

	string md5Pwd = MD5Checksum::GetMD5OfString(string(szID) + string(szPwd));
	string idfa = getIdentify().c_str();

	account["appID"] = CONTACT_INT_APPID;
	account["channelID"] = SystemUtils::getChannelId();
	account["idfa"] = idfa;
	account["password"] = md5Pwd;
	account["username"] = szID;
	account["userID"] = 0;
	
	ostringstream ossSign;
	ossSign <<"appID=" << CONTACT_INT_APPID << "channelID=" << 
		SystemUtils::getChannelId() << "password=" << md5Pwd << "username=" << szID << CONTACT_KEY;
	string sign = MD5Checksum::GetMD5OfString(ossSign.str());
	account["sign"] = sign;
#endif

	std::string  json_file = writer.write(account);
	HttpCommunicator::getInstance()->sendHttpLoginMsg(nMAIN_CMD_LOGIN, json_file.c_str());
#endif
}

void LoginController::logoutAccount()
{
	//登出需要标记为角色登录未成功
	mIsRoleLoginSuccess = false;

	//当前处于注销状态
	mIsLogoutState = true;

	//注销重置自动登录状态
	m_isAutoLogin = false;

	//清理网络连接;
	SocketCommunicator::getInstance()->clear();

	//清理图片缓存;
	Director::getInstance()->getTextureCache()->removeUnusedTextures();

	//清理数据缓存
	DataCollection::clearData();

#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)
    //注销;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//不需要主动调用SDK的logOut by sonic 7/4/2016
	SDKComponent::getInstance()->logout();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SDKComponent_ios::getInstance()->logout();
#endif
#endif

	
}

void LoginController::processPayResult( const char* szData )
{
	if (nullptr == szData || 0 == strlen(szData))
	{
		return;
	}

	// 
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		Json::Value data = root["data"];

		// 更新金币;
		int nCurGold = data["gold"].asInt();
		int nPreGold = MainModel::getInstance()->getMainParam()->mGold;
		MainModel::getInstance()->setGold(nCurGold, true);
		CCLOG("++++++++++++++++ PayResult: [gold]%d -> %d ", nPreGold, nCurGold);

		//额外赠送金币包括首冲双倍赠送直接上报TalkingData
		int presentGold = data["presentGold"].asInt();
		TalkingData::onReward(presentGold);

		// 更新vip经验;
		int nVipLv = data["vipLevel"].asInt();
		int nPreVipLv = MainModel::getInstance()->getMainParam()->mVipLevel;
		MainModel::getInstance()->getMainParam()->mVipLevel = nVipLv;
		CCLOG("++++++++++++++++ PayResult: [vipLevel]%d -> %d ", nPreVipLv, nVipLv);

		int nVipExp = data["vipexp"].asInt();
		int nPreVipExp = MainModel::getInstance()->getMainParam()->mVipExp;
		MainModel::getInstance()->getMainParam()->mVipExp = nVipExp;
		CCLOG("++++++++++++++++ PayResult: [vipExp]%d -> %d ", nPreVipExp, nVipExp);

		//如果是首充，记录状态
		if(FirstRechargeModel::getInstance()->mFirstRechargeState == 0)
		{
			FirstRechargeModel::getInstance()->mFirstRechargeState = data["payReward"].asInt();
			ObserverParam obParam;
			obParam.id = nMAIN_CMD_PAY_RESULT;
			FirstRechargeModel::getInstance()->notifyObservers(&obParam);
		}

		MainModel::getInstance()->notifyObservers();

		//充值界面数据更新
		VipModel::getInstance()->payResultPush(data);

		// 同步充值总额;
		SetIntegerToXML(LOCAL_KEY_STR_TOTAL_PAY_SRV, data["totalPay"].asInt());
		SetIntegerToXML(LOCAL_KEY_STR_TOTAL_PAY_LOCAL, data["totalPay"].asInt());
	}

}

void LoginController::processLogoutMsg(const char* szData)
{

	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		//处理登出操作
		Director::getInstance()->replaceScene(SceneLogout::create());
	}
}

void LoginController::processRepeatLoginMsg(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		auto callBack = [this]( Ref* pSender, CustomRetType type )->void
		{
			switch (type)
			{
			case RET_TYPE_OK:
			case RET_TYPE_CANCEL:
			case RET_TYPE_CLOSE:
				Director::getInstance()->replaceScene(SceneLogout::create());
				break;
			default:
				break;
			}
		};

		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10284), callBack);
	}
}

void LoginController::getServerZoneList()
{
	Json::FastWriter writer;
	Json::Value root;
	root["mac"] = getIdentify().c_str();
	root["channelId"] = SystemUtils::getChannelId();
    root["installId"] = SystemUtils::getGameVersion();
	std::string json_file = writer.write(root);
	HttpCommunicator::getInstance()->sendGetZoneListMsg(nMAIN_CMD_GET_ZONE_LIST, json_file.c_str());
}

void LoginController::ttCheckUserName(const char* szUserName)
{
#if (LOGIN_WITH_TT == 1)
	Json::FastWriter  writer;
	Json::Value account;

	string uname = szUserName;
	string gid = _TO_STR(SystemUtils::getTianTuoGid());

	account["ac"] = 2;
	account["gid"] = gid.c_str();
	account["uname"] = uname.c_str();
	account["appID"] = CONTACT_INT_APPID;
	account["channelID"] = SystemUtils::getChannelId();

	std::string  json_file = writer.write(account);
	HttpCommunicator::getInstance()->sendTTCheckUName(nMAIN_CMD_TT_CHECK_ACCOUNT, json_file.c_str());

#endif
}

void LoginController::onCheckUserNameRet(const char* szData)
{
	Json::Reader read;
	Json::Value root;

	if (read.parse(szData, root))
	{
		if (throwException(root))
			return;

		bool bRet = root["result"].asBool();
		if (!bRet)
		{
			string strMsg = root["msg"].asString();
			CustomPop::show(strMsg);
		}
		else
		{
			// 下一帧执行;
			Director::getInstance()->getScheduler()->schedule([=](float dt)->void {
				// 注册;
				ttRegister(m_strTmpUName.c_str(), m_strTmpUPwd.c_str());
				m_strTmpUName.clear();
				m_strTmpUPwd.clear();
			},
				this, 0, 0, 0, false, "LoginController::onCheckUserNameRet");
			
		}
	}
}

void LoginController::ttRegister(const char* userName, const char* szPwd)
{
#if (LOGIN_WITH_TT == 1)

	Json::FastWriter  writer;
	Json::Value account;

	string uname = userName;
	string upwd = MD5Checksum::GetMD5OfString(string(userName) + string(szPwd));
	string umail = "";
	string utime = string(_TO_STR(time(0)));
	string gid = _TO_STR(SystemUtils::getTianTuoGid());
	string sid = _TO_STR(0);
	string uaid = _TO_STR(SystemUtils::getUaid());
	string uwid = _TO_STR(SystemUtils::getUwid());
	string uadid = _TO_STR(0);
	string usite = _TO_STR(0);
	string umacid = getIdentify();
	string key = SystemUtils::getTianTuoKey();
	string sign = MD5Checksum::GetMD5OfString(uname + upwd + umail + utime + gid + sid + uaid + uwid + uadid + usite + umacid + key);

	account["ac"] = 1;
	account["uname"] = uname.c_str();
	account["upwd"] = upwd.c_str();
	account["umail"] = umail.c_str();
	account["utime"] = utime.c_str();
	account["gid"] = gid.c_str();
	account["sid"] = sid.c_str();
	account["uaid"] = uaid.c_str();
	account["uwid"] = uwid.c_str();
	account["uadid"] = uadid.c_str();
	account["usite"] = usite.c_str();
	account["umacid"] = umacid.c_str();
	account["key"] = key.c_str();
	account["sign"] = sign.c_str();
	account["appID"] = CONTACT_INT_APPID;
	account["channelID"] = SystemUtils::getChannelId();

	std::string  json_file = writer.write(account);
	HttpCommunicator::getInstance()->sendHttpRegisterMsg(nMAIN_CMD_REGISTER, json_file.c_str());

#endif
}

void LoginController::sendRegAndLoginMsg( const char* userName, const char* szPwd )
{
#if (LOGIN_WITH_TT == 1)

	m_strTmpUName.clear();
	m_strTmpUName.assign(userName);
	m_strTmpUPwd.clear();
	m_strTmpUPwd.assign(szPwd);
	ttCheckUserName(userName);

#else

	Json::FastWriter  writer;
	Json::Value account;

	string md5Pwd = MD5Checksum::GetMD5OfString(string(userName) + string(szPwd));
	string idfa = getIdentify().c_str();

	account["appID"] = CONTACT_INT_APPID;
	account["channelID"] = SystemUtils::getChannelId();
	account["idfa"] = idfa;
	account["password"] = md5Pwd;
	account["username"] = userName;
	account["userID"] = 0;

	ostringstream ossSign;
	ossSign << "appID=" << CONTACT_INT_APPID << "channelID=" << SystemUtils::getChannelId()
		<< "idfa=" << idfa << "password=" << md5Pwd << "username=" << userName << CONTACT_KEY;
	string sign = MD5Checksum::GetMD5OfString(ossSign.str());
	account["sign"] = sign;

	std::string  json_file = writer.write(account);
	HttpCommunicator::getInstance()->sendHttpRegisterMsg(nMAIN_CMD_REGISTER, json_file.c_str());

#endif
}

void LoginController::sendModifyPasswordMsg( const char* userName, const char* szOldPwd, const char* szNewPwd )
{
	Json::FastWriter  writer;
	Json::Value account;

	string md5NewPwd = MD5Checksum::GetMD5OfString(string(userName) + string(szNewPwd));
	string md5Pwd = MD5Checksum::GetMD5OfString(string(userName) + string(szOldPwd));
	string idfa = getIdentify().c_str();

	account["appID"] = CONTACT_INT_APPID;
	account["channelID"] = SystemUtils::getChannelId();
	account["password"] = md5Pwd;
	account["newPassword"] = md5NewPwd;
	account["username"] = userName;
	account["userID"] = 0;
	account["idfa"] = idfa;

	ostringstream ossSign;
	ossSign << "appID=" << CONTACT_INT_APPID << "channelID=" << SystemUtils::getChannelId() << 
		"newPassword=" << md5NewPwd << "password=" << md5Pwd <<"username=" << userName << CONTACT_KEY;
	string sign = MD5Checksum::GetMD5OfString(ossSign.str());
	account["sign"] = sign;

	std::string  json_file = writer.write(account);
	HttpCommunicator::getInstance()->sendHttpModifyPwdMsg(nMAIN_CMD_MODIFY_PWD, json_file.c_str());
}

void LoginController::sendTryGameMsg(int uID)
{
	Json::FastWriter  writer;
	Json::Value account;

	//string idfa = getIdentify().c_str();
	string idfa = getTryGameIdentify().c_str();

	account["appID"] = CONTACT_INT_APPID;
	account["channelID"] = SystemUtils::getChannelId();
	account["idfa"] = idfa;
	account["password"] = "";
	account["username"] = "";
	account["userID"] = uID;

	ostringstream ossSign;

	m_iTryUID = uID;

	if(uID == 0)
	{
		//第一次试玩
		ossSign << "appID=" << CONTACT_INT_APPID << "channelID=" << SystemUtils::getChannelId() << "idfa=" << idfa << CONTACT_KEY;
		string sign = MD5Checksum::GetMD5OfString(ossSign.str());
		account["sign"] = sign;

		std::string  json_file = writer.write(account);
		HttpCommunicator::getInstance()->sendHttpTryGameMsg(nMAIN_CMD_FIRST_TRY_GAME, json_file.c_str());
	}
	else
	{
		//老游客登陆
		ossSign << "appID=" << CONTACT_INT_APPID << "channelID=" << SystemUtils::getChannelId() << "userID=" << uID << CONTACT_KEY;
		string sign = MD5Checksum::GetMD5OfString(ossSign.str());
		account["sign"] = sign;

		std::string  json_file = writer.write(account);
		HttpCommunicator::getInstance()->sendHttpLoginMsg(nMAIN_CMD_TRY_GAME, json_file.c_str());
	}
}

void LoginController::sendActAccountMsg( int uID, const char* userName, const char* szPwd )
{
	Json::FastWriter  writer;
	Json::Value account;

	string md5Pwd = MD5Checksum::GetMD5OfString(string(userName) + string(szPwd));

	account["appID"] = CONTACT_INT_APPID;
	account["channelID"] = SystemUtils::getChannelId();
	account["userID"] = uID;
	account["password"] = md5Pwd;
	account["username"] = userName;
	account["idfa"] = getTryGameIdentify().c_str();
	//account["idfa"] = getIdentify().c_str();

	ostringstream ossSign;
	ossSign << "appID=" << CONTACT_INT_APPID << "channelID=" << SystemUtils::getChannelId() << "password=" << md5Pwd << "userID=" << uID << "username=" << userName << CONTACT_KEY;
	string sign = MD5Checksum::GetMD5OfString(ossSign.str());

	account["sign"] = sign;

	std::string  json_file = writer.write(account);
	HttpCommunicator::getInstance()->sendHttpAtivateMsg(nMAIN_CMD_ACTIVATE, json_file.c_str());
}

bool LoginController::getIsLogoutState()
{
	return mIsLogoutState;
}

string LoginController::getIdentify()
{
	string ret = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	ret = SystemUtils::getIMEI();//getMAC();
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ret = SystemUtils::getIDFA();
#else
	time_t t = time(nullptr);
	int num = CCRANDOM_0_1()*10000;
	ostringstream str;
	str << t << num;
	ret = str.str();
	//ret = "00:00:00:00:00:00";
#endif
	return ret;
}

std::string LoginController::getTryGameIdentify()
{
	string ret = GetStringFromXML(LOCAL_KEY_STR_TRY_IDENTIFY, "");

	if(ret == "")
	{
		time_t t = time(nullptr);
		int num = CCRANDOM_0_1()*10000;
		ostringstream str;
		str << t << num;
		ret = str.str();

		SetStringToXML(LOCAL_KEY_STR_TRY_IDENTIFY, ret);
	}

	return ret;
}

void LoginController::sendDeviceId()
{
	Json::FastWriter  writer;
	Json::Value root;

	int appId = CONTACT_INT_APPID;
	int userId = -1;
	string idfv = SystemUtils::getIDFV();
#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)
	userId = atoi(LoginModel::getInstance()->getU8UserId().c_str());
#else
	userId = LoginModel::getInstance()->getUserId();
#endif

	string umacid = getIdentify();
	string sign = "appID=" + _TO_STR(appId) + "userId=" + _TO_STR(userId) + "umacid=" + umacid + "idfv=" + idfv + CONTACT_KEY;
	sign = MD5Checksum::GetMD5OfString(sign);

	root["appID"] = appId;
	root["userId"] = userId;
	root["umacid"] = umacid;
	root["idfv"] = idfv;
	root["sign"] = sign;
	std::string  json_file = writer.write(root);
	HttpCommunicator::getInstance()->sendMessage(nMAIN_CMD_SEND_DEV_ID, json_file.c_str(), SystemUtils::getSEND_DEV_ID_URL().c_str(), false);
}
