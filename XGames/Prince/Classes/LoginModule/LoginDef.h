#ifndef LoginDef_h__
#define LoginDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/UIDef.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "Communication/EnvConfig.h"
#include "FormationModule/CustomScrollBar.h"
#include <string>

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

// 服务器分区列表默认显示个数(1-10);
#define SERVER_ZONE_DEFAULT_COUNT	10

//static const string  STR_88 = "抵制不良游戏，拒绝盗版游戏。注意自我保护，谨防上当受骗。适度游戏益脑，沉迷游戏伤身。合理安排时间，享受健康生活";

// 服务器状态;
typedef enum enSERVER_STATE
{
	// 未开启;
	SERVER_STATE_CLOSED = 0,			// 维护;

	// 开启;
	SERVER_STATE_BUSY,					// 爆满;
	SERVER_STATE_NEW					// 新区;
}SERVER_STATE;


typedef enum enLOGIN_RESULT
{
	LOGIN_RESULT_SUCCESS				 = 1,
	LOGIN_RESULT_USERNAME_ERROR,
	LOGIN_RESULT_PASSWORD_ERROR,
	LOGIN_RESULT_ACCONT_INVALID

}LOGIN_RESULT;

typedef enum enREGISTER_RESULT
{
	REGISTER_RESULT_SUCCESS				 = 1,
	REGISTER_RESULT_USERNAME_EXIST,
	REGISTER_RESULT_DB_ERROR
}REGISTER_RESULT;


typedef enum enMODIFY_PWD_RESULT
{
	MODIFY_PWD_RESULT_SUCCESS				 = 1,
	MODIFY_PWD_RESULT_USERNAME_ERROR,
	MODIFY_PWD_RESULT_PASSWORD_ERROR,
	MODIFY_PWD_RESULT_DB_ERROR
}MODIFY_PWD_RESULT;

typedef enum enACTIVATE_RESULT
{
	ACTIVATE_RESULT_SUCCESS				= 1,
}ACTIVATE_RESULT;

typedef enum enTRY_GAME_RESULT
{
	TRY_GAME_RESULT_SUCCESS				= 1,
}TRY_GAME_RESULT;

// UI消息类型
enum OBS_PARAM_TYPE_LOGIN
{
	OBS_PARAM_TYPE_SERVER_ZONE_LIST,		// 服务器列表;
	OBS_PARAM_TYPE_SELECT_SERVER_ZONE		// 选择分区;
};

typedef struct tagUI_SERVER_ZONE_LIST
{
	Layout*			pRoot;

	// 历史角色;
	Text*			txtRoles;
	ImageView*		imgRolesBg;
	ImageView*		imgRolesSelected;

	// 服务器目录;
	cocos2d::ui::ScrollView*		scrollviewServerIndex;
	CustomScrollBar* scrollbar;
	ImageView*		imgIndexSelected;
	Text*			txtIndexItem;

	// 历史登陆;
	Text*			txtDefaultZoneName;
	Text*			txtDefaultZoneState;

	// 服务器列表;
	ImageView*		imgZoneListBg;
	Text*			txtZoneTitle;
	Layout*			pnlZoneList;

	Layout*			pnlItem;

	tagUI_SERVER_ZONE_LIST()
	{
		memset(this, 0, sizeof(tagUI_SERVER_ZONE_LIST));
	}
}UI_SERVER_ZONE_LIST;

typedef struct tagUI_LOGIN_ROLE
{
	Layout*		pRoot;
	Layout*		pnlLoginRole;
	Button*		btnEnter;
	Button*		btnZone;
	Text*		txtZoneName;
	Text*		txtZoneState;
	Button*		btnLogout;
	Button*		btnBoard;
	Text*		txtVersion;

	UI_SERVER_ZONE_LIST  uiZone;

	tagUI_LOGIN_ROLE()
	{
		memset(this, 0, sizeof(tagUI_LOGIN_ROLE));
	}

}UI_LOGIN_ROLE;


typedef struct tagServerParam
{
	// 序号;
	int  m_nZoneIndex;

	// 服务器分区ID;
	string  m_strServerID;

	// 服务器分区名称;
	string  m_strServerName;

	// 服务器分区IP;
	string  m_strServerIP;

	// 服务器分区端口号;
	string  m_strServerPort;

	// 服务器状态;
	SERVER_STATE  nServerState;

	tagServerParam()
	{
		clear();
	}

	void clear()
	{
		m_nZoneIndex = 1;
		m_strServerID.clear(),
		m_strServerName.clear(),
		m_strServerIP.clear(),
		m_strServerPort.clear();
		nServerState = SERVER_STATE_CLOSED;
	}

	bool empty()
	{
		return (m_strServerIP.empty() && m_strServerPort.empty());
	}
}ServerParam;


#endif // LoginDef_h__
