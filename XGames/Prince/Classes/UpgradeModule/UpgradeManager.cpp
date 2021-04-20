#include "UpgradeManager.h"
#include "UpgradeController.h"
#include "Utils/SystemUtils.h"

#include "curl.h"
#include "easy.h"
#include <stdio.h>
#include <vector>
#include <thread>

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32) && (CC_TARGET_PLATFORM != CC_PLATFORM_WP8) && (CC_TARGET_PLATFORM != CC_PLATFORM_WINRT)
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#endif

#include "unzip.h"

#define BUFFER_SIZE						8192
#define MAX_FILENAME					512

#define LOW_SPEED_LIMIT					1L
#define LOW_SPEED_TIME					5L

extern  long g_UpgradePkgSize;
extern  long g_localLen;

UpgradeManager::UpgradeManager(const char* packageUrl/* =nullptr */, const char* versionFileUrl/* =nullptr */, const char* storagePath/* =nullptr */)
:  _storagePath(storagePath)
, _version("")
, _packageUrl(packageUrl)
, _downloadedVersion("")
, _curl(nullptr)
, _connectionTimeout(0)
, _delegate(nullptr)
, _isDownloading(false)
, _shouldDeleteDelegateWhenExit(false)
, _newVersionCode("")
{
	_versionFileUrl.clear();
	_versionFileUrl.assign(versionFileUrl);
    checkStoragePath();
}

UpgradeManager::~UpgradeManager()
{
    if (_shouldDeleteDelegateWhenExit)
    {
        delete _delegate;
    }
}

void UpgradeManager::checkStoragePath()
{
    if (_storagePath.size() > 0 && _storagePath[_storagePath.size() - 1] != '/')
    {
        _storagePath.append("/");
    }
}

// Multiple key names
static std::string keyWithHash( const char* prefix, const std::string& url )
{
    char buf[256];
    sprintf(buf,"%s%zd",prefix,std::hash<std::string>()(url));
    return buf;
}

// hashed version
std::string UpgradeManager::keyOfVersion() const
{
    return keyWithHash(KEY_OF_VERSION,_packageUrl);
}

// hashed version
std::string UpgradeManager::keyOfDownloadedVersion() const
{
    return keyWithHash(KEY_OF_DOWNLOADED_VERSION,_packageUrl);
}

static size_t getVersionCode(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    string *version = (string*)userdata;
    version->append((char*)ptr, size * nmemb);
    
    return (size * nmemb);
}

string UpgradeManager::getIdentifier()
{
	string ret = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	ret = SystemUtils::getMAC();
#elif(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	ret = SystemUtils::getIDFA();
#else
	ret = "00:00:00:00:00:00";
#endif
	return ret;
}

bool UpgradeManager::checkUpdate()
{
	CCLOG("++++++++++ [UpgradeManager] checkUpdate, _versionFileUrl = %s ++++++++++", _versionFileUrl.c_str());
    if (_versionFileUrl.size() == 0)
		return false;
    
    _curl = curl_easy_init();
    if (! _curl)
    {
        CCLOG("can not init curl in f[checkUpdate]");
        return false;
    }
    
    // Clear _version before assign new value.
    _version.clear();

	// read;
	string curVersion = GetStringFromXML(LOCAL_KEY_STR_CURRENT_VERSION, "xx");
	string installVersion = GetStringFromXML(LOCAL_KEY_STR_INSTALL_VERSION, "xx");
	CCLOG("####################### curVer = %s, installVer = %s ########################", curVersion.c_str(), installVersion.c_str());
	Json::FastWriter  writer;
	Json::Value account;
	account["versionId"] = curVersion;
	account["installId"] = installVersion;
	account["channelId"] = SystemUtils::getChannelId();
	account["mac"] = getIdentifier().c_str();
	std::string  json_file = writer.write(account);
    
    // 改变临时的url，保留原始url字符串;
    string strNewVersionFileUrl = _versionFileUrl;
	strNewVersionFileUrl.append("?data=");
	strNewVersionFileUrl.append(json_file.c_str());
    
    CURLcode res;
    curl_easy_setopt(_curl, CURLOPT_URL, strNewVersionFileUrl.c_str());
    curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, getVersionCode);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_version);
    if (_connectionTimeout) curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, _connectionTimeout);
    curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_LIMIT, LOW_SPEED_LIMIT);
    curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_TIME, LOW_SPEED_TIME);
    res = curl_easy_perform(_curl);
    
    if (res != 0)
    {
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, this]{
            if (this->_delegate)
                this->_delegate->onError(ErrorCode::NETWORK);
        });
        CCLOG("can not get version file content, error code is %d", res);
        curl_easy_cleanup(_curl);
		_curl = nullptr;
        return false;
    }

	// 后续流程在controller中处理;
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, this]{
		if (this->_delegate)
		{
			UpgradeController* _controller = dynamic_cast<UpgradeController*>(this->_delegate);
			if (_controller != nullptr)
			{
				_controller->onGetVersionInfo(_version.c_str());
			}
		}
	});

    return true;
}

void UpgradeManager::t_download()
{
    if (_downloadedVersion != _newVersionCode)//_version)
    {
        if (downLoad())
		{
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, this]{
				if (this->_delegate)
				{
					UpgradeController* _controller = dynamic_cast<UpgradeController*>(this->_delegate);
					assert(_controller != nullptr);
					if (_controller->onDownloadOver(getDownloadPkgFullPath()))
					{
						auto t2 = std::thread(&UpgradeManager::t_uncompress, this);
						t2.detach();
					}
				}
			});
		}

    }
	else if (_upgradeType == UPGRADE_TYPE_PATCH || _upgradeType == UPGRADE_TYPE_RES)
	{
		auto t2 = std::thread(&UpgradeManager::t_uncompress, this);
		t2.detach();
	}
	else if (_upgradeType == UPGRADE_TYPE_FULL)
	{
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, this]{
			if (this->_delegate)
			{
				UpgradeController* _controller = dynamic_cast<UpgradeController*>(this->_delegate);
				assert(_controller != nullptr);
				_controller->onFullSuccess();
			}
		});
	}

    _isDownloading = false;
}

void UpgradeManager::t_uncompress()
{
	// 正在解压状态;
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, this]{
		if (this->_delegate)
			this->_delegate->onError(ErrorCode::UNCOMPRESSING);
	});

    // Uncompress zip file.
    if (! uncompress())
    {
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, this]{
            if (this->_delegate)
                this->_delegate->onError(ErrorCode::UNCOMPRESS);
        });
    }
	else
	{
		Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, this] {
			if (this->_delegate)
			{
				UpgradeController* _controller = dynamic_cast<UpgradeController*>(this->_delegate);
				assert(_controller != nullptr);
				_controller->onUncompressOver(getDownloadPkgFullPath());
			}
		});
	}
}

void UpgradeManager::update( UPGRADE_TYPE type )
{
	//////////////// test
	//_versionFileUrl = "http://119.29.21.196/1.2/prince_201511040422_release1.0.apk";
	//_packageUrl = "http://119.29.21.196/1.2/prince_201511040422_release1.0.apk";
	//_isDownloading = false;
	//_newVersionCode = "1.4";
	//_curl = curl_easy_init();
	//////////////// test end

    if (_isDownloading) return;
    
    _isDownloading = true;
	_upgradeType = type;
    
    // 1. Urls of package and version should be valid;
    // 2. Package should be a zip file.
    if (_versionFileUrl.size() == 0 ||
        _packageUrl.size() == 0/* ||
        std::string::npos == _packageUrl.find(".zip")*/)
    {
        //CCLOG("no version file url, or no package url, or the package is not a zip file");
		CCLOG("no version file url, or no package url");
        _isDownloading = false;
        return;
    }

    // Is package already downloaded?
	_downloadedVersion = GetStringFromXML(LOCAL_KEY_STR_DOWNLOADED_VERSION, "");

	// 创建下载线程;
    auto t = std::thread(&UpgradeManager::t_download, this);
    t.detach();
}

bool UpgradeManager::uncompress()
{
    // Open the zip file
    string outFileName = getDownloadPkgFullPath();//_storagePath + UPGRADE_PACKAGE_NAME;
    unzFile zipfile = unzOpen(outFileName.c_str());
    if (! zipfile)
    {
        CCLOG("can not open downloaded zip file %s", outFileName.c_str());
        return false;
    }
    
    // Get info about the zip file
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
    {
        CCLOG("can not read file global info of %s", outFileName.c_str());
        unzClose(zipfile);
        return false;
    }
    
    // Buffer to hold data read from the zip file
    char readBuffer[BUFFER_SIZE];
    
    CCLOG("start uncompressing");
    
    // Loop to extract all files.
    uLong i;
    for (i = 0; i < global_info.number_entry; ++i)
    {
        // Get info about current file.
        unz_file_info fileInfo;
        char fileName[MAX_FILENAME];
        if (unzGetCurrentFileInfo(zipfile,
                                  &fileInfo,
                                  fileName,
                                  MAX_FILENAME,
                                  nullptr,
                                  0,
                                  nullptr,
                                  0) != UNZ_OK)
        {
            CCLOG("can not read file info");
            unzClose(zipfile);
            return false;
        }
        
        const string fullPath = _storagePath + fileName;
        
        // Check if this entry is a directory or a file.
        const size_t filenameLength = strlen(fileName);
        if (fileName[filenameLength-1] == '/')
        {
            // Entry is a directory, so create it.
            // If the directory exists, it will failed silently.
            if (!createDirectory(fullPath.c_str()))
            {
                CCLOG("can not create directory %s", fullPath.c_str());
                unzClose(zipfile);
                return false;
            }
        }
        else
        {
            //There are not directory entry in some case.
            //So we need to test whether the file directory exists when uncompressing file entry
            //, if does not exist then create directory
            const string fileNameStr(fileName);
            
            size_t startIndex=0;
            
            size_t index=fileNameStr.find("/",startIndex);
            
            while(index != std::string::npos)
            {
                const string dir=_storagePath+fileNameStr.substr(0,index);
                
                FILE *out = fopen(dir.c_str(), "r");
                
                if(!out)
                {
                    if (!createDirectory(dir.c_str()))
                    {
                        CCLOG("can not create directory %s", dir.c_str());
                        unzClose(zipfile);
                        return false;
                    }
                    else
                    {
                        CCLOG("create directory %s",dir.c_str());
                    }
                }
                else
                {
                    fclose(out);
                }
                
                startIndex=index+1;
                
                index=fileNameStr.find("/",startIndex);
                
            }
            
            
            
            // Entry is a file, so extract it.
            
            // Open current file.
            if (unzOpenCurrentFile(zipfile) != UNZ_OK)
            {
                CCLOG("can not open file %s", fileName);
                unzClose(zipfile);
                return false;
            }
            
            // Create a file to store current file.
            FILE *out = fopen(fullPath.c_str(), "wb");
            if (! out)
            {
                CCLOG("can not open destination file %s", fullPath.c_str());
                unzCloseCurrentFile(zipfile);
                unzClose(zipfile);
                return false;
            }
            
            // Write current file content to destinate file.
            int error = UNZ_OK;
            do
            {
                error = unzReadCurrentFile(zipfile, readBuffer, BUFFER_SIZE);
                if (error < 0)
                {
                    CCLOG("can not read zip file %s, error code is %d", fileName, error);
                    unzCloseCurrentFile(zipfile);
                    unzClose(zipfile);
                    return false;
                }
                
                if (error > 0)
                {
                    fwrite(readBuffer, error, 1, out);
                }
            } while(error > 0);
            
            fclose(out);
        }
        
        unzCloseCurrentFile(zipfile);
        
        // Goto next entry listed in the zip file.
        if ((i+1) < global_info.number_entry)
        {
            if (unzGoToNextFile(zipfile) != UNZ_OK)
            {
                CCLOG("can not read next file");
                unzClose(zipfile);
                return false;
            }
        }
    }
    
    CCLOG("end uncompressing");
    unzClose(zipfile);
    
    return true;
}

/*
 * Create a directory is platform depended.
 */
bool UpgradeManager::createDirectory(const char *path)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    mode_t processMask = umask(0);
    int ret = mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
    umask(processMask);
    if (ret != 0 && (errno != EEXIST))
    {
        return false;
    }
    
    return true;
#else
    BOOL ret = CreateDirectoryA(path, nullptr);
	if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
	{
		return false;
	}
    return true;
#endif
}

void UpgradeManager::setSearchPath()
{
    vector<string> searchPaths = FileUtils::getInstance()->getSearchPaths();
    vector<string>::iterator iter = searchPaths.begin();
    searchPaths.insert(iter, _storagePath);
    FileUtils::getInstance()->setSearchPaths(searchPaths);
}

static size_t downLoadPackage(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    FILE *fp = (FILE*)userdata;
    size_t written = fwrite(ptr, size, nmemb, fp);
    return written;
}

int upgradeManagerProgressFunc(void *ptr, double totalToDownload, double nowDownloaded, double totalToUpLoad, double nowUpLoaded)
{
	//float fTmpTotal = g_UpgradePkgSize;
    static int percent = 0;
    int tmp = (int)((nowDownloaded + (double)g_localLen) / g_UpgradePkgSize * 100.0f);
	//CCLOG("nowDownloaded = %lf, g_total = %ld, f_total = %lf", nowDownloaded, g_UpgradePkgSize, totalToDownload);
    
    if (percent != tmp)
    {
        percent = tmp;
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]{
            auto manager = static_cast<UpgradeManager*>(ptr);
            if (manager->_delegate)
                manager->_delegate->onProgress(percent);
        });
        
        CCLOG("percent = %d%%", percent);
    }
    
    return 0;
}

bool UpgradeManager::downLoad()
{
	if (!_curl)
	{
		_curl = curl_easy_init();
		if (!_curl)
		{
			CCLOG("can not init curl in f[download]");
			return false;
		}
	}

    // Create a file to save package.
    const string outFileName = getDownloadPkgFullPath();//_storagePath + UPGRADE_PACKAGE_NAME;

	// 断点续传;
	g_localLen = GetLocalFileLenth(outFileName.c_str());
	CCLOG("download: locallen = %ld", g_localLen);

    FILE *fp = fopen(outFileName.c_str(), "ab+");
    if (! fp)
    {
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, this]{
            if (this->_delegate)
                this->_delegate->onError(ErrorCode::CREATE_FILE);
        });
        CCLOG("can not create file %s", outFileName.c_str());
        return false;
    }
	int n = fseek(fp, 0, SEEK_END);
	CCLOG("fseek ret = %d", n);
    
    // Download package
    CURLcode res;
    curl_easy_setopt(_curl, CURLOPT_URL, _packageUrl.c_str());
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, downLoadPackage);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, false);
    curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, upgradeManagerProgressFunc);
    curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, this);
    curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_LIMIT, LOW_SPEED_LIMIT);
    curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_TIME, LOW_SPEED_TIME);

	curl_easy_setopt(_curl, CURLOPT_RESUME_FROM, g_localLen);

    res = curl_easy_perform(_curl);
    curl_easy_cleanup(_curl);
	_curl = nullptr;
    if (res != 0 && res != CURLE_PARTIAL_FILE)
    {
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([&, this]{
            if (this->_delegate)
                this->_delegate->onError(ErrorCode::DISCONNECTED);
        });
        CCLOG("error when download package");
        fclose(fp);
        return false;
    }
    
    CCLOG("succeed downloading package %s", _packageUrl.c_str());
    
    fclose(fp);
    return true;
}

const char* UpgradeManager::getPackageUrl() const
{
    return _packageUrl.c_str();
}

void UpgradeManager::setPackageUrl(const char *packageUrl)
{
    _packageUrl = packageUrl;
}

const char* UpgradeManager::getStoragePath() const
{
    return _storagePath.c_str();
}

void UpgradeManager::setStoragePath(const char *storagePath)
{
    _storagePath = storagePath;
    checkStoragePath();
}

const char* UpgradeManager::getVersionFileUrl() const
{
    return _versionFileUrl.c_str();
}

void UpgradeManager::setVersionFileUrl(const char *versionFileUrl)
{
    _versionFileUrl = versionFileUrl;
}

string UpgradeManager::getVersion()
{
    return UserDefault::getInstance()->getStringForKey(keyOfVersion().c_str());
}

void UpgradeManager::deleteVersion()
{
    UserDefault::getInstance()->setStringForKey(keyOfVersion().c_str(), "");
}

void UpgradeManager::setDelegate(UpgradeManagerDelegateProtocol *delegate)
{
    _delegate = delegate;
}

void UpgradeManager::setConnectionTimeout(unsigned int timeout)
{
    _connectionTimeout = timeout;
}

unsigned int UpgradeManager::getConnectionTimeout()
{
    return _connectionTimeout;
}

UpgradeManager* UpgradeManager::create(const char* packageUrl, const char* versionFileUrl, const char* storagePath )
{
	auto* manager = new UpgradeManager(packageUrl,versionFileUrl,storagePath);
	manager->_shouldDeleteDelegateWhenExit = true;
	manager->autorelease();
	return manager;
}

void UpgradeManager::createStoragePath()
{
    // Remove downloaded files
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    DIR *dir = nullptr;
    
    dir = opendir (_storagePath.c_str());
    if (!dir)
    {
        mkdir(_storagePath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    }
#else    
    if ((GetFileAttributesA(_storagePath.c_str())) == INVALID_FILE_ATTRIBUTES)
    {
        CreateDirectoryA(_storagePath.c_str(), 0);
    }
#endif
}

void UpgradeManager::destroyStoragePath()
{
    // Delete recorded version codes.
    deleteVersion();
    
    // Remove downloaded files
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    string command = "rm -r ";
    // Path may include space.
    command += "\"" + _storagePath + "\"";
    system(command.c_str());    
#else
    string command = "rd /s /q ";
    // Path may include space.
    command += "\"" + _storagePath + "\"";
    system(command.c_str());
#endif
}

void UpgradeManager::setNewVersion(const char* strNewVersion)
{
	_newVersionCode.clear();
	_newVersionCode.assign(strNewVersion);
};

long UpgradeManager::isUpgradePkgExist( string strVersion )
{
	string path = getDownloadPkgFullPath(strVersion);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

	if (access(path.c_str(), F_OK) == 0)
	{
		return GetLocalFileLenth(path.c_str());
	}

#endif
	return 0;
}

long UpgradeManager::GetLocalFileLenth(const char* fileName)
{
	char strTemp[256] = {0};
	strcpy(strTemp, fileName);
	FILE* fp = fopen(strTemp, "rb");
	if(fp != NULL)
	{
		/*
		// 此函数只在windows环境使用;
		long localLen = _filelength(_fileno(fp));
		*/

		// std方法，可跨平台使用，获取文件长度;
		fseek(fp, 0L, SEEK_END);
		long localLen = ftell(fp);
		fclose(fp);
		return localLen;
	}
	return 0;
}

string UpgradeManager::getDownloadPkgFullPath( string strVersion /*= ""*/ )
{
	string path = "";
	string pathSuffix = (_upgradeType == UPGRADE_TYPE_FULL) ? ".apk" : ".zip";
	if (strVersion.empty())
	{
		path = _storagePath + UPGRADE_PACKAGE_NAME_PREFIX + _newVersionCode + pathSuffix;
	}
	else
	{
		path = _storagePath + UPGRADE_PACKAGE_NAME_PREFIX + strVersion + pathSuffix;
	}
	return path;
}