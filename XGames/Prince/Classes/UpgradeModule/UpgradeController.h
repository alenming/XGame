/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		UpgradeController
 *  Description:	升级模块的流程控制;
 *					1. cocos2d-x资源更新协议的实现;
 *					2. 资源的下载/更新流程控制;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			04/09/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef UpgradeController_h__
#define UpgradeController_h__

#include "UpgradeDef.h"
#include "LayerUpgrade.h"
#include "LoginModule/LayerLogin.h"
#include "UpgradeManager.h"


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
extern "C" {
	JNIEXPORT jint JNICALL Java_com_zcjoy_prince_AppActivity_writeVersion(JNIEnv* env, jobject othis, jstring argv1);
	JNIEXPORT jint JNICALL Java_com_zcjoy_prince_AppActivity_showUpdateMessage(JNIEnv* env, jobject othis, jstring argv1);
	JNIEXPORT jint JNICALL Java_com_zcjoy_prince_AppActivity_onLbCheckUpgradeDone(JNIEnv* env, jobject othis, jstring argv1);
}

#endif

class UpgradeController : public UpgradeManagerDelegateProtocol
{
public:
	UpgradeController(void);
	~UpgradeController(void);

	static UpgradeController* getInstance();
	static void destroyInstance();

	void  setUpgradeView(LayerUpgrade* view) { m_pUpgradeView = view; };
	void  setLoginView(LayerLogin* view) { m_pLoginView = view; };
	LayerUpgrade* getUpgradeView() { return m_pUpgradeView; };
	LayerLogin*	  getLoginView() { return m_pLoginView; };

	// 检查更新;
	bool  checkUpgrade();
	void  lb_checkUpgrade();

	// 开始更新;
	void  upgrade();

	// 获取版本信息;
	void  onGetVersionInfo(const char* szData);

	// 下载完毕;
	bool  onDownloadOver(string strPkgPath);

	// 解压完毕;
	void  onUncompressOver(string strPkgPath);

	virtual void  onError(ErrorCode errorCode);
	virtual void  onProgress(int percent);
	virtual void  onSuccess();
	virtual void  onFullSuccess();
    
    // IOS清理更新目录;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    
    void  iosClearUpgradeDir();
    
#endif

private:
	UpgradeManager*  getUpgradeManager();

	// 初始化文件存储路径;
	void  initDir();

	// 添加文件搜索路径至FileUtils;
	void  setSearchPath();

	// 清空存储路径下的所有文件;
	void  clearDir(const char* dirPath);

	// 检查更新文件是否已下载并解压;
	bool  isPatchReaday();

	// 获得补丁文件的全路径;
	string  getPatchFullPath();

	// 初始化更新所需存储空间;
	int   initUpdateStorage();

private:

	static UpgradeController* m_pInstance;

	LayerUpgrade*	m_pUpgradeView;
	LayerLogin*		m_pLoginView;

	UpgradeManager* m_pMgr;

	// 本地文件存储路径;
	string  m_strSavePath;

	// 版本信息;
	VersionInfo info;
};

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)

static void writeVersion(string strVersion)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // 清理残留更新文件;
    // 按照约定的版本规则，[a].[b].[c]其中[c]变化时是资源更新，不清理文件，其余[a]或[b]更新类型全部需要清理掉;
    string tmpCurV = GetStringFromXML(LOCAL_KEY_STR_CURRENT_VERSION, "");
    if (!tmpCurV.empty())
    {
        vector<string>  vcSingleV;
        StringFormat::parseCsvStringVec(strVersion, vcSingleV, '.');
        vector<string>  vcSingleCurV;
        StringFormat::parseCsvStringVec(tmpCurV, vcSingleCurV, '.');
        
        // 当前版本不为空，说明不是首次安装，开始对比版本号;
        if (!vcSingleCurV.empty())
        {
            // 版本号解析出应该是3段，此处容错;
            if (vcSingleV.size() == 3 && vcSingleCurV.size() == 3)
            {
                // 前两段版本号变化，需要清理掉更新目录;
                if (vcSingleV.at(0) != vcSingleCurV.at(0) || vcSingleV.at(1) != vcSingleCurV.at(1))
                {
                    UpgradeController::getInstance()->iosClearUpgradeDir();
                    
                    // 同时清空当前版本号记录;
                    SetStringToXML(LOCAL_KEY_STR_CURRENT_VERSION, "");
                }
            }
        }
    }
#endif
    
    // 每次重置安装版本号;
    SetStringToXML(LOCAL_KEY_STR_INSTALL_VERSION, strVersion.c_str());
    
    // 当前版本号只记录初始值，以后随更新变化;
    string curVersion = GetStringFromXML(LOCAL_KEY_STR_CURRENT_VERSION, "");
    if (curVersion.empty())
    {
        SetStringToXML(LOCAL_KEY_STR_CURRENT_VERSION, strVersion.c_str());
    }
}

#endif

#endif // UpgradeController_h__
