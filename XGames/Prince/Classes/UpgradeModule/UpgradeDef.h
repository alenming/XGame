/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		UpgradeDef.h
 *  Description:	升级模块相关数据定义;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			11/25/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef UpgradeDef_h__
#define UpgradeDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include <string>
#include <vector>
#include <map>
#include "Common/Common.h"
#include "Temp/CustomPop.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "Communication/EnvConfig.h"

// Android平台用到JNI的反向调用;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

USING_NS_CC;
//USING_NS_CC_EXT;
using namespace cocos2d::ui;
using namespace std;


// Path;
#define UPGRADE_CUSTOM_FOLDER					"ZCJoy/Prince/Update/" 

#define KEY_OF_VERSION							"current-version"
#define KEY_OF_DOWNLOADED_VERSION				"downloaded-version"
#define UPGRADE_PACKAGE_NAME_PREFIX				"prince_upgrade_pkg_"
#define PATCH_PREFIX							"prince_"

// 下载超时定义(s);
#define UPGRADE_DOWNLOAD_TIMEOUT				10

// 文本定义;ID
#define UPGRADE_TIPS_FORCE						20034
#define UPGRADE_TIPS_OPTIONAL					20035
#define UPGRADE_ERR_STR_CREATE_FILE				20036
#define UPGRADE_ERR_STR_NETWORK					20037
#define UPGRADE_ERR_STR_NO_NEW_VERSION			20038
#define UPGRADE_ERR_STR_CHECKING_MD5			20039
#define UPGRADE_ERR_STR_CHECK_MD5				20040
#define UPGRADE_ERR_STR_UNCOMPRESSING			20041
#define UPGRADE_ERR_STR_UNCOMPRESS				20042
#define UPGRADE_ERR_STR_PATCHING				20043
#define UPGRADE_ERR_STR_SUCCESS					20044

// 更新类型定义;
enum UPGRADE_TYPE
{
	UPGRADE_TYPE_INVALID = 0,		// 不可用;
	UPGRADE_TYPE_RES	 = 1,		// 资源更新;
	UPGRADE_TYPE_PATCH	 = 2,		// 差异更新;
	UPGRADE_TYPE_FULL	 = 3		// 全量更新;
};

enum ErrorCode
{
    // Error caused by creating a file to store downloaded data
    CREATE_FILE,
    /** Error caused by network
        -- network unavailable
        -- timeout
        -- ...
        */
    NETWORK,
    /** There is not a new version
        */
    NO_NEW_VERSION,
    /** Error caused in uncompressing stage
        -- can not open zip file
        -- can not read file global information
        -- can not read file information
        -- can not create a directory
        -- ...
        */
    UNCOMPRESS,

	//////////////////// add state by Phil@zcjoy ////////////////////
	// lost connection;
	DISCONNECTED,

	// download successful;
	DOWNLOAD_SUCCESS,

	// check md5 ing...;
	CHECKING_MD5,

	// check md5 failed;
	CHECK_MD5_ERROR,

	// uncompress ing...;
	UNCOMPRESSING,

	// patch;
	PATCHING,

	// installing;
	INSTALLING,

	// over;
	UPGRADE_SUCCESS
};

// 版本信息结构;
typedef struct tagVersionInfo
{
	// 1. 更新类型;
	UPGRADE_TYPE  nUpgradeType;

	// 2. 是否强制更新;
	bool	bIsForceUpdate;

	// 3. 旧版本号[废弃];
	string  strOldVersion;

	// 4. 新版本号;
	string  strNewVersion;

	// 5. 包大小(K);
	unsigned long  nPkgSize;

	// 6. 下载地址;
	string  strUrl;

	// 7. 补丁包(.zip)md5;
	string  strMd5;

	// 8. 原包md5(安装版本);
	string  strInstallMd5;

	tagVersionInfo()
	{
		clear();
	}

	void clear()
	{
		nUpgradeType = UPGRADE_TYPE_INVALID;
		bIsForceUpdate = true;
		nPkgSize = 0;
		strOldVersion.clear();
		strNewVersion.clear();
		strUrl.clear();
		strMd5.clear();
		strInstallMd5.clear();
	}
}VersionInfo;


// 更新UI;
typedef struct tagUI_Upgrade
{
	Layout*			pRoot;
	ImageView*		imgBg;

	// 有可用更新;
	Layout*			pnlNpc;
	Text*			txtContent;
	Text*			txtContent0;
	Text*			txtContent1;
	Text*			txtSize;
	Button*			btnNpcOK;
	Button*			btnNpcCancel;

	// 失去网络连接;
	Layout*			pnlDisconnected;
	Text*			txtTips1;
	Button*			btnRetry;
	Button*			btnNo;

	// 进游戏时有未完成的下载;
	Layout*			pnlNpc2;
	Button*			btnContinue;
	Button*			btnExit2;

	// 更新服务器连不上;
	Layout*			pnlNpc1;
	Button*			btnExit;
	Button*			btnWait;

	// 下载进度;
	Layout*			pnlProgress;
	ImageView*		imgProgressBg;
	LoadingBar*		lbDownloadProgress;
	Text*			txtState;
	Text*			txtSpeed;
	ImageView*		imgPopProgressBg;
	Text*			txtPercent;

	// 正在检查更新提示;
	Text*  txtChecking;

	tagUI_Upgrade()
	{
		memset(this, 0, sizeof(tagUI_Upgrade));
	}
}UI_Upgrade;


#endif // UpgradeDef_h__