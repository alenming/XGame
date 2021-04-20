#include "UpgradeController.h"
#include "Communication/Json/json.h"
#include "Utils/MD5/MD5Checksum.h"
#include "Utils/SystemUtils.h"
#include "UpgradeModule/LayerUpgrade.h"
#include "LoginModule/LayerLogin.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#include <io.h>
#endif

long  g_UpgradePkgSize = 0;			// 整包大小;
long  g_localLen = 0;				// 本地已下载大小;

UpgradeController* UpgradeController::m_pInstance = nullptr;

UpgradeController::UpgradeController(void)
	: m_pUpgradeView(nullptr)
	, m_pLoginView(nullptr)
	, m_pMgr(nullptr)
{
	m_strSavePath.clear();
	initDir();
}


UpgradeController::~UpgradeController(void)
{
}

UpgradeController* UpgradeController::getInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new UpgradeController;
	}
	return m_pInstance;
}

void UpgradeController::destroyInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

UpgradeManager* UpgradeController::getUpgradeManager()
{
	//static AssetsManager* pAssetsMgr = nullptr;
	if (nullptr == m_pMgr)
	{
		// refrencecount 1;
		m_pMgr = UpgradeManager::create("", SystemUtils::getVERSION_INFO_URL().c_str(), m_strSavePath.c_str());
		if (nullptr != m_pMgr)
		{
			m_pMgr->setDelegate(this);
			m_pMgr->setConnectionTimeout(UPGRADE_DOWNLOAD_TIMEOUT);

			// refrencecount 2;
			m_pMgr->retain();
			//m_pMgr->autorelease();
		}
	}
	return m_pMgr;
}


void UpgradeController::initDir()
{
	// 构建下载路径;
	string _fixedPath = FileUtils::getInstance()->getWritablePath();
	m_strSavePath = _fixedPath + UPGRADE_CUSTOM_FOLDER;
	CCLOG("downloadPath: %s", m_strSavePath.c_str());

	// 解析目录层级;
	vector<string>  vcFolderName;
	StringFormat::parseCsvStringVec(UPGRADE_CUSTOM_FOLDER, vcFolderName, '/');

	for (int i = 0; i < vcFolderName.size(); ++i)
	{
		_fixedPath += vcFolderName.at(i);

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
		DIR* pDir = opendir(_fixedPath.c_str());
		if (!pDir)
		{
			int status = mkdir(_fixedPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
			CCLOG("mkdir[%d], status = [%d]: %s", i, status, _fixedPath.c_str());
			pDir = NULL;
		}
#else
		if ((GetFileAttributesA(_fixedPath.c_str())) == INVALID_FILE_ATTRIBUTES)
		{
			CreateDirectoryA(_fixedPath.c_str(), 0);
		}
#endif
		CCLOG("mkdir[%d] done", i);
		_fixedPath += '/';
	}
}

bool UpgradeController::checkUpgrade()
{
	//////////////// test2
	//getUpgradeManager()->update();
	//return true;
	//////////////// test2 end

	//////////////// test
	//info.nUpgradeType = UPGRADE_TYPE_PATCH;
	//info.strInstallMd5 = "3C73DDA3B335688D4B67342E508224A5";
	//onSuccess();
	//return true;
	//////////////// test end

	CCLOG("++++++++++ [UpgradeController] checkUpgrade ++++++++++");

	// 先检查是否有已下载但是未安装的更新包;
	#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		{
			// Android增量更新，下载后解压得到的是patch文件;
			JniMethodInfo methodinfo;
			bool bFound = false;
		
			// 先查询静态函数getJavaActivity，目的是先要获取当前java类对象;
			bFound = JniHelper::getStaticMethodInfo(methodinfo,
													"com.zcjoy.prince.AppActivity",
													"getJavaActivity",
													"()Ljava/lang/Object;");
												
			if (bFound)
			{
				bFound = false;
				jobject activityObj;

				// 调用静态函数getJavaActivity，获取java类对象;
				activityObj = methodinfo.env->CallStaticObjectMethod(methodinfo.classID, methodinfo.methodID);

				// 有了java类对象之后，可以继续查找并调用我们的非静态函数;
				bFound = JniHelper::getMethodInfo(methodinfo,
												"com.zcjoy.prince.AppActivity",
												"checkAlreadyDownloaded",
												"(Ljava/lang/String;)V");
				if (bFound)
				{
					jstring jmsg = methodinfo.env->NewStringUTF("");
					methodinfo.env->CallVoidMethod(activityObj, methodinfo.methodID, jmsg);
				}
			}
		}
	#endif

	// 然后检查更新;
	getUpgradeManager()->checkUpdate();
	return true;
}

int UpgradeController::initUpdateStorage()
{
	int ret = -1;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo minfo;
	//getStaticMethodInfo 次函数返回一个bool值表示是否找到此函数
	bool isHave = JniHelper::getStaticMethodInfo(minfo,
		"com.zcjoy.prince.AppActivity", "initUpdateStoragePath","()Ljava/lang/String;");

	if (isHave) {
		//调用此函数
		jstring str = (jstring)minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);
		minfo.env->DeleteLocalRef(minfo.classID);
		string strRet = JniHelper::jstring2string(str);
		CCLOG("++++++++++ cpp: initUpdateStorage[s] = %s ", strRet.c_str());
		ret = atoi(strRet.c_str());
	}
#else
	ret = 0;
#endif
	CCLOG("++++++++++ cpp: initUpdateStorage[i] = %d ", ret);
	return ret;
}

void UpgradeController::upgrade()
{
	getUpgradeManager()->setNewVersion(info.strNewVersion.c_str());
	getUpgradeManager()->setPackageUrl(info.strUrl.c_str());
	getUpgradeManager()->update(info.nUpgradeType);
	this->onProgress(0);
}

void UpgradeController::onGetVersionInfo( const char* szData )
{
	Json::Reader read;
	Json::Value root;
	if (read.parse(szData, root))
	{
		bool bResult = root["result"].asBool();
		if (bResult)
		{
			Json::Value data = root["data"];

			// 解析版本信息;
			info.clear();
			info.nUpgradeType = (UPGRADE_TYPE)(data["publishTyp"].asInt());
			info.bIsForceUpdate = data["publish"].asBool();
			//[废弃]info.strOldVersion = data["oldVersion"].asString();
			info.strNewVersion = data["newVersion"].asString();
			info.nPkgSize = data["packSize"].asInt();
			g_UpgradePkgSize = info.nPkgSize;
			info.strUrl = data["download"].asString();
			info.strMd5 = data["diffMD5"].asString();
			info.strInstallMd5 = data["installMD5"].asString();

			// 包更新和全量更新要先检查空间;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			if (info.nUpgradeType == UPGRADE_TYPE_PATCH || info.nUpgradeType == UPGRADE_TYPE_FULL)
			{
				if (0 != initUpdateStorage())
					return;
			}
#endif

			// 判定更新;
			if (info.nUpgradeType == UPGRADE_TYPE_INVALID)
			{
				// 无新版本;
				this->onError(ErrorCode::NO_NEW_VERSION);

				// Set resource search path.
				setSearchPath();
			}
			else if (info.nUpgradeType == UPGRADE_TYPE_FULL)
			{
				// 全量更新，游戏内下载并安装;
				if (nullptr != m_pUpgradeView)
				{
					m_pUpgradeView->onFullUpgrade();
				}
			}
			// 包更新;
			else if (info.nUpgradeType == UPGRADE_TYPE_PATCH)
			{
				// 先判断补丁文件是否已下载并且解压完毕;
				if (isPatchReaday())
				{
					onSuccess();
				}
				else
				{
					// 强制更新;
					if (info.bIsForceUpdate)
					{
						if (nullptr != m_pUpgradeView)
						{
							// 是否有未完成的下载;
							long len = getUpgradeManager()->isUpgradePkgExist(info.strNewVersion);
							if (len > 0)
							{
								// 提示继续下载;
								m_pUpgradeView->onDownloadUnfinished(info.nPkgSize - len);

							}
							else
							{
								// 正常提示更新;
								m_pUpgradeView->onForceUpgrade(info.nPkgSize);

							}
						}
					}
					else
					{
						onSuccess();
					}
				}
			}
			// 资源更新;
			else
			{
				// 强制更新;
				if (info.bIsForceUpdate)
				{
					if (nullptr != m_pUpgradeView)
					{
						m_pUpgradeView->onForceUpgrade(info.nPkgSize);
					}
				}
				else
				{
					onSuccess();
				}
			}
		}
        else
        {
            CustomPop::show(root["msg"].asString(), [=](Ref* pSender, CustomRetType type){
                Director::getInstance()->end();
                exit(0);
            }, CUSTOM_ID_YES);
        }
	}
	else
	{
		this->onError(NETWORK);
	}

	// ;
}

bool UpgradeController::onDownloadOver( string strPkgPath )
{
	CCLOG("++++++++++ onDownloadOver ++++++++++");

	// 正在校验文件;
	assert(m_pUpgradeView != nullptr);
	m_pUpgradeView->onError(ErrorCode::CHECKING_MD5);

	//MD5 md5;
	//md5.update(ifstream(zipfileName.c_str(), ios_base::in | ios_base::binary));
	//string strMd5 = md5.toString();

	// MD5校验;
	string zipfileName = strPkgPath;
	string strMd5 = MD5Checksum::GetMD5(zipfileName);
	if (strMd5.compare(info.strMd5) != 0)
	{
		// 文件校验失败;
		m_pUpgradeView->onError(ErrorCode::CHECK_MD5_ERROR);

		// Delete unloaded zip file.
		string zipfileName = strPkgPath;
		if (remove(zipfileName.c_str()) != 0)
		{
			CCLOG("can not remove downloaded zip file %s", zipfileName.c_str());
		}

		return false;
	}

	// 记录已下载的新版本，避免重复下载;
	SetStringToXML(LOCAL_KEY_STR_DOWNLOADED_VERSION, info.strNewVersion);

	// 全量更新就不继续往下走了，返回false;
	if (info.nUpgradeType == UPGRADE_TYPE_FULL)
	{
		// 直接成功;
		onFullSuccess();
		return false;
	}

	return true;
}

void UpgradeController::onUncompressOver( string strPkgPath )
{
	// 记录新版本号;
	if (info.nUpgradeType == UPGRADE_TYPE_RES)
	{
		SetStringToXML(LOCAL_KEY_STR_CURRENT_VERSION, info.strNewVersion);
	}

	// 删除记录的已下载版本号;
	SetStringToXML(LOCAL_KEY_STR_DOWNLOADED_VERSION, "");

	// Set resource search path.
	this->setSearchPath();

	// Delete unloaded zip file.
	string zipfileName = strPkgPath;//m_strSavePath + UPGRADE_PACKAGE_NAME;
	if (remove(zipfileName.c_str()) != 0)
	{
		CCLOG("can not remove downloaded zip file %s", zipfileName.c_str());
	}

	this->onSuccess();
}


void UpgradeController::setSearchPath()
{
	vector<string> searchPaths = FileUtils::getInstance()->getSearchPaths();
	vector<string>::iterator iter = searchPaths.begin();
	searchPaths.insert(iter, m_strSavePath);
	FileUtils::getInstance()->setSearchPaths(searchPaths);
}

void UpgradeController::clearDir(const char* dirPath)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	_finddata_t fb;
	char   path[256];
    long   handle;
	int    ret;
	int    err;

    handle = 0;

    // 建立路径;
    strcpy(path, dirPath);
    strcat(path, "/*");

	// 找到第一个匹配的文件;
	handle = _findfirst(path, &fb);
	if (handle != 0)
	{
		// 若继续找到匹配的文件，继续执行;
		while (0 == _findnext(handle, &fb))
		{
			// "..""."不处理;
			if (strcmp(fb.name, "..") == 0
				|| strcmp(fb.name, ".") == 0)
				continue;

			// 构建完整路径;
			memset(path, 0, sizeof(path));
			strcpy(path, dirPath);
			strcat(path, "/");
			strcat(path, fb.name);

			// 属性值为16，则说明是文件夹，递归;
			if (fb.attrib == 16)
			{
				clearDir(path);
			}
			// 非文件夹的文件，直接删除;
			else
			{
				ret = remove(path);
				if (0 != ret)
				{
					err = errno;
				}
			}
		}

		// 关闭文件夹;
		_findclose(handle);
	}

	// 删除目录;
	if (m_strSavePath.compare(dirPath) != 0)
	{
		RemoveDirectoryA(dirPath);
	}

#else

	DIR *dir;  
	struct dirent *read_dir;  
	struct stat st;  
	char buf[256];  

	if(lstat(dirPath, &st) == 0)  
	{
		if(S_ISDIR(st.st_mode))  
		{
			dir = opendir(dirPath);
			if(dir != NULL)  
			{
				while((read_dir = readdir(dir)) != NULL)
				{
					if(strcmp(read_dir->d_name, ".") == 0 ||
						strcmp(read_dir->d_name, "..") == 0)
						continue;
					sprintf(buf, "%s/%s", dirPath, read_dir->d_name);
					clearDir(buf);
				}  
			}
			closedir(dir);
		}

		if (m_strSavePath.compare(dirPath) != 0)
		{
			remove(dirPath);
		}
	}
#endif
}


void UpgradeController::onError( ErrorCode errorCode )
{
	assert(nullptr != m_pUpgradeView);
	m_pUpgradeView->onError(errorCode);

	// 包更新完毕，清除以往资源更新内容，防止出错;
	// 补丁文件也在这里被删除;
	string strCurVersion = GetStringFromXML(LOCAL_KEY_STR_CURRENT_VERSION, "");
	string strInstallVersion = GetStringFromXML(LOCAL_KEY_STR_INSTALL_VERSION, "");
	if (errorCode == NO_NEW_VERSION	&& strCurVersion.compare(strInstallVersion) == 0)
	{
		clearDir(m_strSavePath.c_str());
	}
}

void UpgradeController::onProgress( int percent )
{
	if (percent < 0 || percent > 100)
		return;
	
	if (nullptr != m_pUpgradeView)
	{
		m_pUpgradeView->onProgress((float)percent);
	}
}

bool UpgradeController::isPatchReaday()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

	if (access(getPatchFullPath().c_str(), F_OK) == 0)
	{
		return true;
	}

#endif
	return false;
}

string UpgradeController::getPatchFullPath()
{
	/*
	 *  规则：下载路径 + 文件前缀 + 安装版本 + 最新版本 + 后缀名;
	 */
	string installVersion = GetStringFromXML(LOCAL_KEY_STR_INSTALL_VERSION, "");
	string strPatchParam = m_strSavePath + PATCH_PREFIX + installVersion + "_" + info.strNewVersion + ".patch";
	return strPatchParam;
}

void UpgradeController::onFullSuccess()
{
	CCLOG("[onFullSuccess] full upgrade download finished");
	assert(m_pUpgradeView != nullptr);

	// 全量更新下载完毕，直接开始安装;
	if (info.nUpgradeType == UPGRADE_TYPE_FULL)
	{
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			{
				// 开始执行安装;
				this->onError(INSTALLING);

				JniMethodInfo methodinfo;
				bool bFound = false;
				bFound = JniHelper::getStaticMethodInfo(methodinfo,
														"com.zcjoy.prince.AppActivity",
														"getJavaActivity",
														"()Ljava/lang/Object;");
												
				if (bFound)
				{
					__android_log_print(ANDROID_LOG_INFO, "UpgradeController.cpp", "cpp upgrade android, FOUND_1");
					bFound = false;
					jobject activityObj;
					activityObj = methodinfo.env->CallStaticObjectMethod(methodinfo.classID, methodinfo.methodID);
					bFound = JniHelper::getMethodInfo(methodinfo,
													"com.zcjoy.prince.AppActivity",
													"onFullApkDownloadFinished",
													"(Ljava/lang/String;)V");
					if (bFound)
					{
						// 传参数，全量包的路径;
						string strFinalParam = m_strSavePath + UPGRADE_PACKAGE_NAME_PREFIX + info.strNewVersion + ".apk";
						jstring jmsg = methodinfo.env->NewStringUTF(strFinalParam.c_str());
						__android_log_print(ANDROID_LOG_INFO, "UpgradeController.cpp", "cpp upgrade android, FOUND_2");
						methodinfo.env->CallVoidMethod(activityObj, methodinfo.methodID, jmsg);
					}
				}
			}
		#else
			this->onError(UPGRADE_SUCCESS);
		#endif
	}
}

void UpgradeController::onSuccess()
{
	CCLOG("[onSuccess] normal upgrade download finished");
	assert(m_pUpgradeView != nullptr);

	// 1..资源更新至此全部完成;
	if (info.nUpgradeType == UPGRADE_TYPE_RES)
	{
		this->onError(UPGRADE_SUCCESS);
	}
	// 2..包更新只针对Android;
	else if (info.nUpgradeType == UPGRADE_TYPE_PATCH)
	{
		#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			{
				// 开始合成;
				this->onError(PATCHING);

				// Android增量更新，下载后解压得到的是patch文件;
				JniMethodInfo methodinfo;
				bool bFound = false;
				/*
				bFound = JniHelper::getStaticMethodInfo(methodinfo,
														"com.zcjoy.prince.AppActivity",
														"OnPatchFileDownloadFinished",
														"(Ljava/lang/String;)V");
														*/
		
				// 先查询静态函数getJavaActivity，目的是先要获取当前java类对象;
				bFound = JniHelper::getStaticMethodInfo(methodinfo,
														"com.zcjoy.prince.AppActivity",
														"getJavaActivity",
														"()Ljava/lang/Object;");
												
				if (bFound)
				{
					__android_log_print(ANDROID_LOG_INFO, "UpgradeController.cpp", "cpp upgrade android, FOUND_1");
					/*
					jstring jmsg = methodinfo.env->NewStringUTF(m_strSavePath.c_str());
					__android_log_print(ANDROID_LOG_INFO, "UpgradeController.cpp", "cpp upgrade android, FOUND!!!");
					methodinfo.env->CallStaticObjectMethod(methodinfo.classID, methodinfo.methodID, jmsg);
					*/

					bFound = false;
					jobject activityObj;

					// 调用静态函数getJavaActivity，获取java类对象;
					activityObj = methodinfo.env->CallStaticObjectMethod(methodinfo.classID, methodinfo.methodID);

					// 有了java类对象之后，可以继续查找并调用我们的非静态函数;
					bFound = JniHelper::getMethodInfo(methodinfo,
													"com.zcjoy.prince.AppActivity",
													"OnPatchFileDownloadFinished",
													"(Ljava/lang/String;)V");
					if (bFound)
					{
						// 构造patch完整路径;
						//string installVersion = GetStringFromXML(LOCAL_KEY_STR_INSTALL_VERSION, "");
						//string strPatchParam = m_strSavePath + PATCH_PREFIX + installVersion + "_" + info.strNewVersion + ".patch";

						// 传参数;
						// 此处增加原始APK的MD5，拼接字符串，作为一个参数输出;
						string strFinalParam = getPatchFullPath() + "|" + info.strInstallMd5;
						jstring jmsg = methodinfo.env->NewStringUTF(strFinalParam.c_str());
						__android_log_print(ANDROID_LOG_INFO, "UpgradeController.cpp", "cpp upgrade android, FOUND_2");
						methodinfo.env->CallVoidMethod(activityObj, methodinfo.methodID, jmsg); 
					}
				}
			}
		#else
			this->onError(UPGRADE_SUCCESS);
		#endif
	}
}

void UpgradeController::lb_checkUpgrade()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		JniMethodInfo methodinfo;
		bool bFound = false;
		bFound = JniHelper::getStaticMethodInfo(methodinfo,
												"com.zcjoy.prince.AppActivity",
												"getJavaActivity",
												"()Ljava/lang/Object;");
		if (bFound)
		{
			bFound = false;
			jobject activityObj;
			activityObj = methodinfo.env->CallStaticObjectMethod(methodinfo.classID, methodinfo.methodID);
			bFound = JniHelper::getMethodInfo(methodinfo,
											"com.zcjoy.prince.AppActivity",
											"lb_checkUpdate",
											"()V");
			if (bFound)
			{
				methodinfo.env->CallVoidMethod(activityObj, methodinfo.methodID); 
			}
		}  
#endif 
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

JNIEXPORT jint JNICALL Java_com_zcjoy_prince_AppActivity_writeVersion(JNIEnv* env, jobject othis, jstring argv1)
{
	string strVersion = env->GetStringUTFChars(argv1, 0);
	__android_log_print(ANDROID_LOG_INFO, "UpgradeController.cpp", "cpp writeVersion %s", strVersion.c_str());

	// 每次安装时(版本变化)，都写入安装版本号/当前版本号;
	string installVersion = GetStringFromXML(LOCAL_KEY_STR_INSTALL_VERSION, "");
	if (installVersion.compare(strVersion) != 0)
	{
		SetStringToXML(LOCAL_KEY_STR_INSTALL_VERSION, strVersion.c_str());
		SetStringToXML(LOCAL_KEY_STR_CURRENT_VERSION, strVersion.c_str());
	}
	return 0;
}

JNIEXPORT jint JNICALL Java_com_zcjoy_prince_AppActivity_showUpdateMessage(JNIEnv* env, jobject othis, jstring argv1)
{
	string strMsg = env->GetStringUTFChars(argv1, 0);
	__android_log_print(ANDROID_LOG_INFO, "UpgradeController.cpp", "cpp showUpdateMessage %s", strMsg.c_str());

	// 使用游戏内UI来展示更新消息;
	Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]{
		CustomPop::show(strMsg.c_str(), [=](Ref* pSender, CustomRetType type){
			if (type == RET_TYPE_OK)
			{
				Director::getInstance()->end();
				exit(0);
			}
		}, CUSTOM_ID_YES);
	});
	return 0;
}

JNIEXPORT jint JNICALL Java_com_zcjoy_prince_AppActivity_onLbCheckUpgradeDone(JNIEnv* env, jobject othis, jstring argv1)
{
	string strResult = JniHelper::jstring2string(argv1);
	int nResult = atoi(strResult.c_str());

	// 拿到lb查询更新结果;
	/*
	* 更新结果说明:-1：请求失败
	*				1：未知错误
	*				2：没有更新
	*				3：有非强更版本
	*				4：有强更版本
	*/
	__android_log_print(ANDROID_LOG_INFO, "UpgradeController.cpp", "onLbCheckUpgradeDone result = %d", nResult);
	if (nResult != 4)
	{
		LayerLogin* _layer = UpgradeController::getInstance()->getLoginView();
		if (nullptr != _layer)
		{
			_layer->onUpgradeDone("");
		}
	}

	return 0;
}

#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

void UpgradeController::iosClearUpgradeDir()
{
    if (m_strSavePath.empty())
    {
        initDir();
    }
    clearDir(m_strSavePath.c_str());
}

#endif
