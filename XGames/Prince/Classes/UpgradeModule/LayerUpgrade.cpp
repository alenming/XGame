#include "LayerUpgrade.h"
#include "LoginModule/SceneLogin.h"
#include "UpgradeController.h"
#include "Utils/SystemUtils.h"
#include "DataManager/DataManager.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "SDK/SDKComponent_ios.h"
#endif


LayerUpgrade::LayerUpgrade(void)
	: m_pParent(nullptr)
{
	UpgradeController::getInstance()->setUpgradeView(this);
	gettimeofday(&m_timeStart, NULL);
	m_fPrePercent = 0.0f;
}


LayerUpgrade::~LayerUpgrade(void)
{
}

bool LayerUpgrade::init()
{
	if (!Layer::init())
	{
		return false;
	}


	initUI();
	return true;
}



void LayerUpgrade::initUI()
{
	Size size = Director::getInstance()->getWinSize();

	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIUpgrade/UIUpgrade.ExportJson"));
	this->addChild(m_ui.pRoot);

	// Bg;
	//m_ui.imgBg = ImageView::create("Image/Bg/img_bg_login.jpg");
	//m_ui.pRoot->addChild(m_ui.imgBg, 1);

	// 正在检查更新;
	m_ui.txtChecking = dynamic_cast<Text*>(m_ui.pRoot->getChildByName("Lab_Checking"));
	m_ui.txtChecking->setFontName(FONT_FZZHENGHEI);
	m_ui.txtChecking->setVisible(false);
	m_ui.txtChecking->setString("");
	m_ui.txtChecking->setString(DataManager::getInstance()->searchCommonTexdtById(10274));
	m_ui.txtChecking->setVisible(true);

	// NPC;
	m_ui.pnlNpc = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_Npc"));
	m_ui.txtContent = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Content"));
	m_ui.txtContent->setFontName(FONT_FZZHENGHEI);
	m_ui.txtContent->setColor(Color3B(0xFF, 0xFF, 0xFF));
	//m_ui.txtContent->enableOutline(Color4B(Color3B(0x81, 0x0, 0x0)), 2);
	setTextAddStroke(m_ui.txtContent, Color3B(0x81, 0x0, 0x0), 2);
	m_ui.txtContent0 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Content_0"));
	m_ui.txtContent0->setFontName(FONT_FZZHENGHEI);
	m_ui.txtContent0->setColor(Color3B(0xFF, 0xFF, 0xFF));
	//m_ui.txtContent0->enableOutline(Color4B(Color3B(0x81, 0x0, 0x0)), 2);
	setTextAddStroke(m_ui.txtContent0, Color3B(0x81, 0x0, 0x0), 2);
	m_ui.txtContent1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Content_1"));
	m_ui.txtContent1->setFontName(FONT_FZZHENGHEI);
	m_ui.txtContent1->setColor(Color3B(0xFF, 0xFF, 0xFF));
	//m_ui.txtContent1->enableOutline(Color4B(Color3B(0x81, 0x0, 0x0)), 2);
	setTextAddStroke(m_ui.txtContent1, Color3B(0x81, 0x0, 0x0), 2);
	m_ui.txtSize = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Size"));
	m_ui.txtSize->setFontName(FONT_FZZHENGHEI);
	m_ui.txtSize->setColor(Color3B(0x8E, 0xEF, 0x3F));
	//m_ui.txtSize->enableOutline(Color4B(Color3B(0x0, 0x2F, 0x2D)), 2);
	setTextAddStroke(m_ui.txtSize, Color3B(0x0, 0x2F, 0x2D), 2);
	m_ui.btnNpcOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Npc_OK)));
	m_ui.btnNpcOK->addTouchEventListener(CC_CALLBACK_2(LayerUpgrade::onBtnClicked, this, Btn_Npc_OK));
	m_ui.btnNpcCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Npc_Cancel)));
	m_ui.btnNpcCancel->addTouchEventListener(CC_CALLBACK_2(LayerUpgrade::onBtnClicked, this, Btn_Npc_Cancel));
	m_ui.pnlNpc->setVisible(false);

	// 断线;
	m_ui.pnlDisconnected = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_Disconnected"));
	m_ui.txtTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Content_1"));
	m_ui.txtTips1->setFontName(FONT_FZZHENGHEI);
	m_ui.txtTips1->setColor(Color3B(0xE8, 0xDE, 0xB5));
	m_ui.btnRetry = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Npc_Retry)));
	m_ui.btnRetry->addTouchEventListener(CC_CALLBACK_2(LayerUpgrade::onBtnClicked, this, Btn_Npc_Retry));
	m_ui.btnNo = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Npc_No)));
	m_ui.btnNo->addTouchEventListener(CC_CALLBACK_2(LayerUpgrade::onBtnClicked, this, Btn_Npc_No));
	m_ui.pnlDisconnected->setVisible(false);

	// 进入游戏时有未完成的下载;
	m_ui.pnlNpc2 = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_Npc_2"));
	dynamic_cast<Text*>(m_ui.pnlNpc2->getChildByName("Lab_Content_1"))->setFontName(FONT_FZZHENGHEI);
	m_ui.btnContinue = dynamic_cast<Button*>(m_ui.pnlNpc2->getChildByName(STRING(Btn_Npc_Continue)));
	m_ui.btnContinue->addTouchEventListener(CC_CALLBACK_2(LayerUpgrade::onBtnClicked, this, Btn_Npc_Continue));
	m_ui.btnExit2 = dynamic_cast<Button*>(m_ui.pnlNpc2->getChildByName(STRING(Btn_Npc_Exit_2)));
	m_ui.btnExit2->addTouchEventListener(CC_CALLBACK_2(LayerUpgrade::onBtnClicked, this, Btn_Npc_Exit_2));
	m_ui.pnlNpc2->setVisible(false);

	// 更新服务器连不上;
	m_ui.pnlNpc1 = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_Npc_1"));
	m_ui.btnWait = dynamic_cast<Button*>(m_ui.pnlNpc1->getChildByName(STRING(Btn_Npc_Wait)));
	m_ui.btnWait->addTouchEventListener(CC_CALLBACK_2(LayerUpgrade::onBtnClicked, this, Btn_Npc_Wait));
	m_ui.btnExit = dynamic_cast<Button*>(m_ui.pnlNpc1->getChildByName(STRING(Btn_Npc_Exit)));
	m_ui.btnExit->addTouchEventListener(CC_CALLBACK_2(LayerUpgrade::onBtnClicked, this, Btn_Npc_Exit));
	m_ui.pnlNpc1->setVisible(false);

	// 下载进度;
	m_ui.pnlProgress = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_Progress"));
	m_ui.imgProgressBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Progress_Bg"));
	m_ui.txtState = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_State"));
	m_ui.txtState->setFontName(FONT_FZZHENGHEI);
	m_ui.txtState->setString(DataManager::getInstance()->searchCommonTexdtById(20033));
	m_ui.txtSpeed = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Speed"));
	m_ui.txtSpeed->setFontName(FONT_FZZHENGHEI);
	m_ui.txtSpeed->setVisible(false);
	m_ui.imgPopProgressBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Pop_Progress_Bg"));
	m_ui.txtPercent = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Percent"));
	m_ui.txtPercent->setFontName(FONT_FZZHENGHEI);
	m_ui.lbDownloadProgress = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(m_ui.pRoot, "ProgressBar_23"));
	m_ui.lbDownloadProgress->setPercent(0.0f);
	m_ui.pnlProgress->setVisible(false);

}

void LayerUpgrade::onError( ErrorCode errCode )
{
	string strText = "";
	switch (errCode)
	{
	case CREATE_FILE:
		strText.assign(DataManager::getInstance()->searchCommonTexdtById(UPGRADE_ERR_STR_CREATE_FILE));
		break;
	case NETWORK:
		strText.assign(DataManager::getInstance()->searchCommonTexdtById(UPGRADE_ERR_STR_NETWORK));
		break;
	case NO_NEW_VERSION:
		strText.assign(DataManager::getInstance()->searchCommonTexdtById(UPGRADE_ERR_STR_NO_NEW_VERSION));
		break;
	case UNCOMPRESSING:
		strText.assign(DataManager::getInstance()->searchCommonTexdtById(UPGRADE_ERR_STR_UNCOMPRESSING));
		break;
	case UNCOMPRESS:
		strText.assign(DataManager::getInstance()->searchCommonTexdtById(UPGRADE_ERR_STR_UNCOMPRESS));
		break;
	case CHECKING_MD5:
		strText.assign(DataManager::getInstance()->searchCommonTexdtById(UPGRADE_ERR_STR_CHECKING_MD5));
		break;
	case CHECK_MD5_ERROR:
		strText.assign(DataManager::getInstance()->searchCommonTexdtById(UPGRADE_ERR_STR_CHECK_MD5));
		break;
	case PATCHING:
		strText.assign(DataManager::getInstance()->searchCommonTexdtById(UPGRADE_ERR_STR_PATCHING));
		break;
	case UPGRADE_SUCCESS:
		strText.assign(DataManager::getInstance()->searchCommonTexdtById(UPGRADE_ERR_STR_SUCCESS));
		break;
	default:
		break;
	}
	m_ui.txtState->setString((strText.c_str()));
	m_ui.txtSpeed->setVisible(false);

	CCLOG("++++++++++ [onError] errCode = %d ++++++++++", errCode);

	if (errCode == NETWORK)
	{
		m_ui.pnlNpc1->setVisible(true);
		m_ui.txtChecking->setVisible(false);
	}
	else if (errCode == DISCONNECTED)
	{
		m_ui.txtChecking->setVisible(false);

		// 断线重连;
		m_ui.pnlDisconnected->setVisible(true);
	}
	else if (errCode == NO_NEW_VERSION || errCode == UPGRADE_SUCCESS)
	{
		m_nReserveCode = errCode;

		// 更新完毕;
		onUpgradeDone();
	}
}

void LayerUpgrade::onProgress( float percent )
{
	CCLOG("[onProgress]-1 percent = %f%%", percent);

	// 有内容可以更新时才显示;
	m_ui.pnlProgress->setVisible(true);

	// 进度;
	m_ui.txtSpeed->setVisible(true);
	m_ui.txtState->setString(DataManager::getInstance()->searchCommonTexdtById(20045));
	m_ui.lbDownloadProgress->setPercent(percent > m_ui.lbDownloadProgress->getPercent() ? percent : m_ui.lbDownloadProgress->getPercent());
	ostringstream oss;
	oss << (int)percent;
	m_ui.txtPercent->setString(oss.str());
	oss.str("");
	float fShift = 50.0f + (m_ui.imgProgressBg->getContentSize().width-100.0f) * (percent/100.0f);
	m_ui.imgPopProgressBg->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.imgPopProgressBg->setPosition(Vec2(m_ui.imgProgressBg->getPosition().x - m_ui.imgProgressBg->getContentSize().width/2 + fShift,
		m_ui.imgPopProgressBg->getPosition().y));

	// 速度;
	if (percent < 0.000001f)
	{
		// 起始时间;
		gettimeofday(&m_timeStart, NULL);
		return;
	}

	// 本次耗时(ms);
	timeval now;
	gettimeofday(&now, NULL);
	long tick = (now.tv_sec - m_timeStart.tv_sec)*1000 + (now.tv_usec - m_timeStart.tv_usec)/1000;
	tick = max(1L, tick);
	CCLOG("[onProgress]-2 now.sec = %ld, now.usec = %ld, tick = %ld", now.tv_sec, now.tv_usec, tick);
	m_timeStart = now;

	// 本次下载(B);
	float download = ((float)m_nPkgSize) * (((percent - m_fPrePercent > 0.000001f) ? percent-m_fPrePercent : 0.0f)/100.0f);
	m_fPrePercent = percent;

	// 即时速度(B/s ---> KB/s、M/s);
	long nSpeed = (long)(download/(((float)tick)/1000.0f));
	string strSpeed = b2km(nSpeed);
	CCLOG("[onProgress]-3 speed = %s[%ld/ms], download = %f(B), tick = %ld(ms)", strSpeed.c_str(), nSpeed, download, tick);
	strSpeed.append("/s");
	m_ui.txtSpeed->setString(strSpeed.c_str());
}

void LayerUpgrade::onSuccess()
{
	onUpgradeDone();
}

void LayerUpgrade::checkUpgrade()
{
	CCLOG("++++++++++ [LayerUpgrade] checkUpgrade ++++++++++");

	// 先隐藏进度，等到有实际下载可用时显示;
	m_ui.txtChecking->setVisible(true);
	m_ui.pnlProgress->setVisible(false);

	// 检查更新;
	UpgradeController::getInstance()->checkUpgrade();
}

void LayerUpgrade::onDownloadUnfinished(long nNeedDownloadSize)
{
	// 剩余未下载文件大小暂不显示;
	m_ui.pnlNpc2->setVisible(true);
}

void LayerUpgrade::onForceUpgrade( long nSize )
{
	m_ui.pnlNpc->setVisible(true);

	if (nSize <= 0)
	{
		nSize = 0;
	}
	m_nPkgSize = nSize;

	// Size;
	m_ui.txtSize->setString(b2km(nSize).c_str());
}

void LayerUpgrade::onUpgradeDone()
{
	// 更新完毕，隐藏升级界面，剩余流程交由上级节点处理;
	this->setVisible(false);
	this->scheduleUpdate();
}

void LayerUpgrade::update(float delta)
{
	// 通知父节点开启后续流程;
	string strMsg("");
	switch (m_nReserveCode)
	{
	case NETWORK:
	case DISCONNECTED:
		strMsg.assign(DataManager::getInstance()->searchCommonTexdtById(20046));
		break;
	case NO_NEW_VERSION:
	case UPGRADE_SUCCESS:
		//strMsg.assign(UPGRADE_ERR_STR_NO_NEW_VERSION);
		break;
	default:
		break;
	}

	CCLOG("++++++++++ [onUpgradeDone] m_nReserveCode = %d, msg = %s ++++++++++", m_nReserveCode, strMsg.c_str());

	assert(m_pParent != nullptr);
	m_pParent->onUpgradeDone(strMsg);

	// 销毁;
	this->unscheduleUpdate();
	this->removeFromParentAndCleanup(true);
}

void LayerUpgrade::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::ENDED)
	{
		switch (nWidgetName)
		{
		case Btn_Npc_OK:
			{
				UpgradeController::getInstance()->upgrade();
				m_ui.pnlNpc->setVisible(false);
			}
			break;

		case Btn_Npc_Retry:
			{
				m_ui.pnlDisconnected->setVisible(false);
				m_ui.pnlProgress->setVisible(true);
				m_ui.txtChecking->setVisible(false);

				// 断点续传;
				UpgradeController::getInstance()->upgrade();
			}
			break;

		case Btn_Npc_Continue:
			{
				m_ui.pnlNpc2->setVisible(false);
				m_ui.pnlProgress->setVisible(true);
				m_ui.txtChecking->setVisible(false);

				// 断点续传;
				UpgradeController::getInstance()->upgrade();
			}
			break;

		case Btn_Npc_Cancel:
		case Btn_Npc_No:
		case Btn_Npc_Exit:
		case Btn_Npc_Exit_2:
			{
				Director::getInstance()->end();
				exit(0);
			}
			break;

		case Btn_Npc_Wait:
			{
				m_ui.pnlNpc1->setVisible(false);
				m_ui.txtChecking->setVisible(true);
				m_ui.pnlProgress->setVisible(false);

				// 下一帧执行;
				auto callBack = [this](float dt)->void
				{
					this->getScheduler()->unschedule("LayerUpgrade::onBtnClicked", this);

					// 重新检测更新;
					checkUpgrade();
				};
				getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerUpgrade::onBtnClicked");
			}
			break;

		default:
			break;
		}
	}
}

string LayerUpgrade::b2km( long xxb )
{
	string str;
	ostringstream oss;
	float fSize = (float)xxb;
	if (fSize/1024.0f < 1000.0f)
	{
		oss << xxb/1024 << "KB";
	}
	else
	{
		float fSizeM = fSize/1048576.0f; // 1024*1024;
		int nTmp = fSizeM * 10;
		fSizeM = ((float)nTmp)/10.0f;
		oss << fSizeM << "M";
	}
	str.append(oss.str());
	oss.str("");
	return str;
}

void LayerUpgrade::onFullUpgrade()
{
	m_ui.txtChecking->setVisible(false);

	CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20047), [=](Ref* pSender, CustomRetType type){
		if (type == RET_TYPE_OK)
		{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			UpgradeController::getInstance()->upgrade();
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            if (!SDKComponent_ios::getInstance()->jumoToAppStore())
            {
                Director::getInstance()->end();
                exit(0);
            }
#endif
		}
		else if (type == RET_TYPE_CANCEL)
		{
			Director::getInstance()->end();
			exit(0);
		}
	}, CUSTOM_YES_NO);

}
