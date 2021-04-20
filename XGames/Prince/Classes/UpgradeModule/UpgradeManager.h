/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		UpgradeManager
 *  Description:	升级模块管理;
 *					1. 改写cocos2d-x原生AssetsManager;
 *					2. 资源的下载/更新操作;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			04/11/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef UpgradeManager_h__
#define UpgradeManager_h__

#include "UpgradeDef.h"
#include <mutex>
#include <string>


class UpgradeManagerDelegateProtocol;

class UpgradeManager : public Node
{

public:
    
    /* @brief Creates a UpgradeManager with new package url, version code url and storage path.
     *
     * @param packageUrl URL of new package, the package should be a zip file.
     * @param versionFileUrl URL of version file. It should contain version code of new package.
     * @param storagePath The path to store downloaded resources.
     * @js NA
     */
    UpgradeManager(const char* packageUrl = nullptr, const char* versionFileUrl = nullptr, const char* storagePath = nullptr);
    /**
     * @js NA
     * @lua NA
     */
    virtual ~UpgradeManager();
    
    //typedef std::function<void(int)> ErrorCallback;
    //typedef std::function<void(int)> ProgressCallback;
    //typedef std::function<void(void)> SuccessCallback;

    /* @brief To access within scripting environment
     */
    static UpgradeManager* create(const char* packageUrl, const char* versionFileUrl, const char* storagePath);

    /* @brief Check out if there is a new version resource.
     *        You may use this method before updating, then let user determine whether
     *        he wants to update resources.
     */
    virtual bool checkUpdate();
    
    using Node::update;
    /* @brief Download new package if there is a new version, and uncompress downloaded zip file.
     *        Of course it will set search path that stores downloaded files.
     */
    virtual void update(UPGRADE_TYPE type);
    
    /* @brief Gets url of package.
     */
    const char* getPackageUrl() const;
    
    /* @brief Sets package url.
     */
    void setPackageUrl(const char* packageUrl);
    
    /* @brief Gets version file url.
     */
    const char* getVersionFileUrl() const;
    
    /* @brief Gets version file url.
     */
    void setVersionFileUrl(const char* versionFileUrl);
    
    /* @brief Gets current version code.
     */
    std::string getVersion();
    
    /* @brief Deletes recorded version code.
     */
    void deleteVersion();
    
    /* @brief Gets storage path.
     */
    const char* getStoragePath() const;
    
    /* @brief Sets storage path.
     *
     * @param storagePath The path to store downloaded resources.
     * @warm The path should be a valid path.
     */
    void setStoragePath(const char* storagePath);
    
    /** @brief Sets delegate, the delegate will receive messages
     * @js NA
     * @lua NA
     */
    void setDelegate(UpgradeManagerDelegateProtocol *delegate);
    
    /**
     * @js NA
     * @lua NA
     */
    UpgradeManagerDelegateProtocol* getDelegate() const { return _delegate ;}
    
    /** @brief Sets connection time out in seconds
     */
    void setConnectionTimeout(unsigned int timeout);
    
    /** @brief Gets connection time out in second
     */
    unsigned int getConnectionTimeout();
    
    /* downloadAndUncompress is the entry of a new thread 
     */
    friend int upgradeManagerProgressFunc(void *, double, double, double, double);

	///////////////////////
	// 设置新版本号，下载时用到;
	void  setNewVersion(const char* strNewVersion);

	// 更新包是否已正在下载(返回值当前文件长度);
	long  isUpgradePkgExist(string strVersion);

protected:
    bool downLoad();
    void checkStoragePath();
    bool uncompress();
    bool createDirectory(const char *path);
    void setSearchPath();

	// 拆分为两个线程来执行，中间插入MD5校验;
    //void downloadAndUncompress();
	void t_download();
	void t_uncompress();

	// 获取更新文件全路径;
	string  getDownloadPkgFullPath(string strVersion = "");

	// 获取本地已下载的文件长度;
	long GetLocalFileLenth(const char* fileName);

private:
    /** @brief Initializes storage path.
     */
    void createStoragePath();
    
    /** @brief Destroys storage path.
     */
    void destroyStoragePath();

	//获取设备唯一标识，IOS获取IDFA，android获取mac
	string getIdentifier();
    
private:
    //! The path to store downloaded resources.
    std::string _storagePath;
    
    //! The version of downloaded resources.
    std::string _version;
	std::string _newVersionCode;
    
    std::string _packageUrl;
    std::string _versionFileUrl;
    
    std::string _downloadedVersion;
    
    void *_curl;

    unsigned int _connectionTimeout;
    
    UpgradeManagerDelegateProtocol *_delegate; 
    
    bool _isDownloading;
    bool _shouldDeleteDelegateWhenExit;

	UPGRADE_TYPE _upgradeType;
    
    std::string keyOfVersion() const;
    std::string keyOfDownloadedVersion() const;
};

class UpgradeManagerDelegateProtocol
{
public:
    virtual ~UpgradeManagerDelegateProtocol(){};
public:
    /* @brief Call back function for error
       @param errorCode Type of error
     * @js NA
     * @lua NA
     */
    virtual void onError(ErrorCode errorCode) {};
    /** @brief Call back function for recording downloading percent
        @param percent How much percent downloaded
        @warning    This call back function just for recording downloading percent.
              UpgradeManager will do some other thing after downloading, you should
              write code in onSuccess() after downloading. 
     * @js NA
     * @lua NA
     */
    virtual void onProgress(int percent) {};
    /** @brief Call back function for success
     * @js NA
     * @lua NA
     */
    virtual void onSuccess() {};
};


#endif // UpgradeManager_h__
