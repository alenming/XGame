#include "LayerLogin.h"
#include "LayerLoginRole.h"
#include "CocoStudio.h"

#include "LoginController.h"
#include "Common/Common.h"
#include "ResourceManager/ResourceLoader.h"
#include "Widget/EditBoxEx.h"
#include "Widget/LayerLoading.h"
#include "Temp/CustomTips.h"
#include "Temp/CustomPop.h"
#include "MainModule/MainCityLoadingScene.h"
#include "Communication/Command.h"
#include "GuideCenter/GuideManager.h"
#include "LayerCreateRole.h"
#include "Utils/SoundUtils.h"
#include "MainModule/MainModel.h"
#include "Communication/EnvConfig.h"
#include "Utils/SystemUtils.h"
#include "Communication/EnvConfig.h"
#include "LoginModel.h"
#include "Communication/ICommListener.h"
#include "UpgradeModule/UpgradeController.h"
#include "ReYunSDK/ReyunController.h"
#include "DataManager/DataManager.h"

using namespace cocos2d;
using namespace cocos2d::ui;

bool LayerLogin::init()
{
	if(!Layer::init())
	{
		return false;
	}

	LoginController::getInstance()->onSetView(this);

	loadLocalAccount();

	//倒计时起始
	m_iDownTime = 2;

	initUI();

	return true;
}



/**********************************重写UI**************************************/

void LayerLogin::initUI()
{
	Size size = Director::getInstance()->getWinSize();
	Vec2 pos = Director::getInstance()->getVisibleOrigin();

	// UI处理
	// CocoStudio UI
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UILogin/UILogin.ExportJson"));

	// 将UILayer层加入到当前的场景
	this->addChild(m_ui.pRoot);
	m_ui.pRoot->setPosition(Vec2((size.width - m_ui.pRoot->getSize().width)/2.0f, (size.height - m_ui.pRoot->getSize().height)/2.0f));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_login.jpg");

	//初始化各面板
	initLoginUI();
	initRegiterUI();
	initModifyPwdUI();
	initActivateUI();
	initLoginTipsUI();

	/***********集成安卓sdk时登录按钮*************/
	m_ui.BtnLoginSdk = dynamic_cast<Button*>(m_ui.pRoot->getChildByName("Btn_login_SDK"));
	m_ui.BtnLoginSdk->setVisible(false);

	m_ui.txt88 = dynamic_cast<Text*>(m_ui.pRoot->getChildByName("Lab_88"));
	m_ui.txt88->setVisible(true);
	m_ui.txt88->setFontName(FONT_FZZHENGHEI);
	m_ui.txt88->setString(DataManager::getInstance()->searchCommonTexdtById(10253));

#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && LOGIN_WITH_TT == 0)
	m_ui.pLoginPnl->setVisible(false);
#endif

}

void LayerLogin::initLoginUI()
{
	/***********登录面板*************/
	m_ui.pLoginPnl = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_denglu"));
	m_ui.pLoginPnl->setVisible(false);

	m_ui.pInpLoginName = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pLoginPnl, "TF_LoginName"));
	m_ui.pEbxLoginName = EditBoxEx::create(m_ui.pInpLoginName);
	m_ui.pEbxLoginName->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxLoginName->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	m_ui.pEbxLoginName->setPosition(m_ui.pInpLoginName->getParent()->getContentSize()/2);
	if(LOGIN_WITH_TT == 1)
	{
		m_ui.pEbxLoginName->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20328).c_str());
	}
	else
	{
		m_ui.pEbxLoginName->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(10254).c_str());
	}
	m_ui.pEbxLoginName->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));
	m_ui.pInpLoginName->getParent()->addChild(m_ui.pEbxLoginName);

	m_ui.pInpLoginPwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pLoginPnl, "TF_LoginPwd"));
	m_ui.pEbxLoginPwd = EditBoxEx::create(m_ui.pInpLoginPwd);
	m_ui.pEbxLoginPwd->setPosition(Vec2(m_ui.pInpLoginPwd->getParent()->getContentSize()/2) + Vec2(-52, 0));
	m_ui.pInpLoginPwd->getParent()->addChild(m_ui.pEbxLoginPwd);
	m_ui.pEbxLoginPwd->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	m_ui.pEbxLoginPwd->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxLoginPwd->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20332).c_str());
	m_ui.pEbxLoginPwd->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));
	m_ui.pEbxLoginPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);

	m_ui.pBtnModifyPwd = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLoginPnl, "Button_ModPwd"));			//修改密码
	m_ui.pBtnRegister = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLoginPnl, "Button_FastReg"));			//快速注册
	m_ui.pBtnTryPlay = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLoginPnl, "Button_TryPlay"));			//试玩
	m_ui.pBtnLogin = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLoginPnl, "Button_Login"));				//登录

	m_ui.pBtnModifyPwd->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Btn_ModifyPwd));
	m_ui.pBtnRegister->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Btn_Register));
	m_ui.pBtnTryPlay->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Btn_TryPlay));
	m_ui.pBtnLogin->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Btn_Login));

	m_pnlCurShow = m_ui.pLoginPnl;
}

void LayerLogin::initRegiterUI()
{
	/***********快速注册面板*************/
	m_ui.pRegisterPnl = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_kuaisuzhuce"));
	m_ui.pRegisterPnl->setVisible(false);

	m_ui.pInpRegName = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pRegisterPnl, "TF_RegName"));
	m_ui.pEbxRegName = EditBoxEx::create(m_ui.pInpRegName);
	m_ui.pEbxRegName->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxRegName->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	m_ui.pEbxRegName->setPosition(m_ui.pInpRegName->getParent()->getContentSize()/2);
	if(LOGIN_WITH_TT == 1)
	{
		m_ui.pEbxRegName->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20328).c_str());
	}
	else
	{
		m_ui.pEbxRegName->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(10254).c_str());
	}
	m_ui.pEbxRegName->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));
	m_ui.pInpRegName->getParent()->addChild(m_ui.pEbxRegName);

	m_ui.pInpRegPwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pRegisterPnl, "TF_RegPwd"));
	m_ui.pEbxRegPwd = EditBoxEx::create(m_ui.pInpRegPwd);
	m_ui.pInpRegPwd->getParent()->addChild(m_ui.pEbxRegPwd);
	m_ui.pEbxRegPwd->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	m_ui.pEbxRegPwd->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxRegPwd->setPosition(Vec2(m_ui.pInpRegPwd->getParent()->getContentSize()/2) + Vec2(-16, 0));
	m_ui.pEbxRegPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);
	m_ui.pEbxRegPwd->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20332).c_str());
	m_ui.pEbxRegPwd->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));

	m_ui.pInpSurePwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pRegisterPnl, "TF_RegSurePwd"));
	m_ui.pEbxSurePwd = EditBoxEx::create(m_ui.pInpSurePwd);
	m_ui.pInpSurePwd->getParent()->addChild(m_ui.pEbxSurePwd);
	m_ui.pEbxSurePwd->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	m_ui.pEbxSurePwd->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxSurePwd->setPosition(Vec2(m_ui.pInpSurePwd->getParent()->getContentSize()/2) + Vec2(-16, 0));
	m_ui.pEbxSurePwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);
	m_ui.pEbxSurePwd->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20332).c_str());
	m_ui.pEbxSurePwd->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));

	m_ui.pBtnShowPwd = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pRegisterPnl, "CB_ShowRegPwd"));			//显示密码
	m_ui.pBtnShowSurePwd = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pRegisterPnl, "CB_ShowRegSurePwd"));	//显示确认密码
	m_ui.pBtnReg = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRegisterPnl, "Button_RegAndLogin"));			//注册并登录新账号
	m_ui.pBtnRegBack = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRegisterPnl, "Button_RegBack"));			//返回
	m_ui.pTextAgreement = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRegisterPnl, "Label_xieyi"));				//游戏免责协议
	m_ui.pTextAgreement->setFontName(FONT_FZZHENGHEI);
	m_ui.pTextAgreement->setTouchEnabled(true);
	m_ui.pTextAgreement->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Text_Agreement));
	Text* statement = dynamic_cast<Text*>(m_ui.pTextAgreement->getParent()->getChildByName("Label_wenben"));
	statement->setFontName(FONT_FZZHENGHEI);

	m_ui.pBtnReg->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Btn_Reg));
	m_ui.pBtnRegBack->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Btn_RegBack));

	m_ui.pBtnShowPwd->addEventListenerCheckBox(this, checkboxselectedeventselector(LayerLogin::onCheckBoxShowPwd));
	m_ui.pBtnShowSurePwd->addEventListenerCheckBox(this, checkboxselectedeventselector(LayerLogin::onCheckBoxShowPwd));
}

void LayerLogin::initModifyPwdUI()
{
	/***********修改密码面板*************/
	m_ui.pModifyPwdPnl = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_xiugaimima"));
	m_ui.pModifyPwdPnl->setVisible(false);

	m_ui.pInpOldName = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pModifyPwdPnl, "TF_ModName"));
	m_ui.pEbxOldName = EditBoxEx::create(m_ui.pInpOldName);
	m_ui.pEbxOldName->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxOldName->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	if(LOGIN_WITH_TT == 1)
	{
		m_ui.pEbxOldName->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20328).c_str());
	}
	else
	{
		m_ui.pEbxOldName->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(10254).c_str());
	}
	m_ui.pEbxOldName->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));
	m_ui.pEbxOldName->setPosition(m_ui.pInpOldName->getParent()->getContentSize()/2);
	m_ui.pInpOldName->getParent()->addChild(m_ui.pEbxOldName);

	m_ui.pInpOldPwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pModifyPwdPnl, "TF_ModOldPwd"));
	m_ui.pEbxOldPwd = EditBoxEx::create(m_ui.pInpOldPwd);
	m_ui.pInpOldPwd->getParent()->addChild(m_ui.pEbxOldPwd);
	m_ui.pEbxOldPwd->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	m_ui.pEbxOldPwd->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxOldPwd->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20332).c_str());
	m_ui.pEbxOldPwd->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));
	m_ui.pEbxOldPwd->setPosition(Vec2(m_ui.pInpOldPwd->getParent()->getContentSize()/2) + Vec2(-16, 0));
	m_ui.pEbxOldPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);

	m_ui.pInpNewPwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pModifyPwdPnl, "TF_ModNewPwd"));
	m_ui.pEbxNewPwd = EditBoxEx::create(m_ui.pInpNewPwd);
	m_ui.pInpNewPwd->getParent()->addChild(m_ui.pEbxNewPwd);
	m_ui.pEbxNewPwd->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	m_ui.pEbxNewPwd->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxNewPwd->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20332).c_str());
	m_ui.pEbxNewPwd->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));
	m_ui.pEbxNewPwd->setPosition(Vec2(m_ui.pInpNewPwd->getParent()->getContentSize()/2) + Vec2(-16, 0));
	m_ui.pEbxNewPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);

	m_ui.pInpSureNewPwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pModifyPwdPnl, "TF_ModSureNewPwd"));
	m_ui.pEbxSureNewPwd = EditBoxEx::create(m_ui.pInpSureNewPwd);
	m_ui.pInpSureNewPwd->getParent()->addChild(m_ui.pEbxSureNewPwd);
	m_ui.pEbxSureNewPwd->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	m_ui.pEbxSureNewPwd->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxSureNewPwd->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20332).c_str());
	m_ui.pEbxSureNewPwd->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));
	m_ui.pEbxSureNewPwd->setPosition(Vec2(m_ui.pInpSureNewPwd->getParent()->getContentSize()/2) + Vec2(-16, 0));
	m_ui.pEbxSureNewPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);

	m_ui.pBtnShowOldPwd = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pModifyPwdPnl, "CB_ModShowOldPwd"));		//显示旧密码
	m_ui.pBtnShowNewPwd = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pModifyPwdPnl, "CB_ModShowNewPwd"));		//显示新密码
	m_ui.pBtnShowSureNewPwd = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pModifyPwdPnl, "CB_ModShowSureNewPwd"));//显示确认密码
	m_ui.pBtnSureModify = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pModifyPwdPnl, "Button_Modify"));				//确认修改
	m_ui.pBtnModBack = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pModifyPwdPnl, "Button_ModBack"));				//返回

	m_ui.pBtnSureModify->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Btn_SureModify));
	m_ui.pBtnModBack->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Btn_ModBack));

	m_ui.pBtnShowOldPwd->addEventListenerCheckBox(this, checkboxselectedeventselector(LayerLogin::onCheckBoxShowPwd));
	m_ui.pBtnShowNewPwd->addEventListenerCheckBox(this, checkboxselectedeventselector(LayerLogin::onCheckBoxShowPwd));
	m_ui.pBtnShowSureNewPwd->addEventListenerCheckBox(this, checkboxselectedeventselector(LayerLogin::onCheckBoxShowPwd));
}
void LayerLogin::initActivateUI()
{
	/***********激活账号面板*************/
	m_ui.pActivatePnl = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_jihuozhanghao"));
	m_ui.pActivatePnl->setVisible(false);

	m_ui.pInpActName = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pActivatePnl, "TF_ActName"));
	m_ui.pEbxActName = EditBoxEx::create(m_ui.pInpActName);
	m_ui.pEbxActName->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxActName->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	if(LOGIN_WITH_TT == 1)
	{
		m_ui.pEbxActName->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20328).c_str());
	}
	else
	{
		m_ui.pEbxActName->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(10254).c_str());
	}
	m_ui.pEbxActName->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));
	m_ui.pEbxActName->setPosition(m_ui.pInpActName->getParent()->getContentSize()/2);
	m_ui.pInpActName->getParent()->addChild(m_ui.pEbxActName);

	m_ui.pInpActPwd = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pActivatePnl, "TF_ActPwd"));
	m_ui.pEbxActPwd = EditBoxEx::create(m_ui.pInpActPwd);
	m_ui.pInpActPwd->getParent()->addChild(m_ui.pEbxActPwd);
	m_ui.pEbxActPwd->setFontColor(Color3B(0xfd, 0xff, 0xcd));
	m_ui.pEbxActPwd->setFont(SystemUtils::getFontFileName().c_str(), 26);
	m_ui.pEbxActPwd->setPlaceHolder(DataManager::getInstance()->searchCommonTexdtById(20332).c_str());
	m_ui.pEbxActPwd->setPlaceholderFontColor(Color3B(0x7F, 0x5F, 0x43));
	m_ui.pEbxActPwd->setPosition(Vec2(m_ui.pInpActPwd->getParent()->getContentSize()/2) + Vec2(-16, 0));
	m_ui.pEbxActPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);

	m_ui.pBtnShowActPwd = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.pActivatePnl, "CB_ShowActPwd"));		//显示密码
	m_ui.pBtnActAccout = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pActivatePnl, "Button_Act"));				//激活账号
	m_ui.pBtnActBack = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pActivatePnl, "Button_ActBack"));			//返回
	m_ui.pTextSwitchAccout = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pActivatePnl, "Label_ActSwitch"));		//切换账号
	m_ui.pTextSwitchAccout->setFontName(FONT_FZZHENGHEI);
	m_ui.pTextSwitchAccout->setTouchEnabled(true);
	m_ui.pTextSwitchAccout->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Text_SwitchAccount));

	m_ui.pBtnActAccout->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Btn_ActAccout));
	m_ui.pBtnActBack->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Btn_ActBack));

	m_ui.pBtnShowActPwd->addEventListenerCheckBox(this, checkboxselectedeventselector(LayerLogin::onCheckBoxShowPwd));
}

void LayerLogin::initLoginTipsUI()
{
	/***********登录提示面板*************/
	m_ui.pLoginTipsPnl = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_zhanghaodenglutips"));
	m_ui.pLoginTipsPnl->setVisible(false);

	m_ui.pTextCntDown = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pLoginTipsPnl, "Label_CntDown"));				//倒计时
	m_ui.pTextClick = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pLoginTipsPnl, "Label_TipsSwitch"));			//切换/激活可点击文本
	m_ui.pTextClick->setTouchEnabled(true);
	m_ui.pTextClick->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Text_SwitchAccount));
	m_ui.pTextCntDown->setFontName(FONT_FZZHENGHEI);
	m_ui.pTextCntDown->setString("");
	m_ui.pTextClick->setFontName(FONT_FZZHENGHEI);

	m_ui.pLoginTipsTryPnl = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_youkedenglutips"));
	m_ui.pLoginTipsTryPnl->setVisible(false);

	m_ui.pTextTryCntDown = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pLoginTipsTryPnl, "Label_TryCntDown"));			//倒计时
	m_ui.pTextTryClick = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pLoginTipsTryPnl, "Label_TipsActTry"));			//切换/激活可点击文本
	m_ui.pTextTryClick->setTouchEnabled(true);
	m_ui.pTextTryClick->addTouchEventListener(CC_CALLBACK_2(LayerLogin::onBtnClick, this, Text_ActAccount));
	m_ui.pTextTryCntDown->setFontName(FONT_FZZHENGHEI);
	m_ui.pTextTryCntDown->setString("");
	m_ui.pTextTryClick->setFontName(FONT_FZZHENGHEI);
}

/**********************************重写UI**************************************/

void LayerLogin::onEnter()
{
	Layer::onEnter();

	m_ui.pEbxLoginName->setText(GetStringFromXML(LOCAL_KEY_STR_USERNAME,"").c_str());
	m_ui.pEbxLoginPwd->setText(GetStringFromXML(LOCAL_KEY_STR_PASSWORD,"").c_str());

	// 音乐;
	SoundUtils::playSoundMusic("login");
}

void LayerLogin::onEnterTransitionDidFinish()
{
	//判定是否开启更新
	if (UPDATE_ENABLE)
	{
		// lb更新方式仅支持android;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        // 显示更新提示文本;
        showUpgradeTips();

		lbCheckUpdate();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		checkUpdate();
#else
		onUpgradeDone("");
#endif
	}
	else
	{
		onUpgradeDone("");
	}
}

void LayerLogin::lbCheckUpdate()
{
	UpgradeController::getInstance()->setLoginView(this);
	UpgradeController::getInstance()->lb_checkUpgrade();
}

void LayerLogin::onExit()
{
	cocostudio::GUIReader::destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UILoginnew");
	LoginController::getInstance()->onRemoveView(this);
	Layer::onExit();
}

LayerLogin::LayerLogin()
{
	m_pnlCurShow = nullptr;
	m_bIsLastLoginTryGame = false;
	m_iTryGameUId = 0;
	m_txtUpgradeTips = nullptr;
}

LayerLogin::~LayerLogin()
{

}

void LayerLogin::onRecvLoginData( int nResult, const char* szMsg )
{
	switch (nResult)
	{
	case nNET_CMD_NET_HTTP_ERROR://网络http错误
		{
			//CustomPop::show("网络链接错误");
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10255));
		}
		break;
	case COMMUNICATION_FAIL:
		{
			CustomTips::showUtf8(szMsg);
		}
		break;
	case LOGIN_RESULT_USERNAME_ERROR:
	case LOGIN_RESULT_PASSWORD_ERROR:
		{
			//CustomPop::show("账号或密码错误");
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10256));
		}
		break;

	case LOGIN_RESULT_ACCONT_INVALID:
		{
			//CustomPop::show("账号异常，请联系客服");
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10257));
		}
		break;

	case LOGIN_RESULT_SUCCESS:
		{
			CCLOG("login success");
			if (SystemUtils::getChannelId() == CHANNEL_ID_APPSTORE) //IOS正版热云注册登入统计上报
			{
				//热云统计 正常账号自动登入
				ReyunController::ReYunLogin(TO_STR(LoginModel::getInstance()->getUserId()));
			}
			m_strUserName = m_ui.pEbxLoginName->getText();
			m_strPassword = m_ui.pEbxLoginPwd->getText();
			m_bIsLastLoginTryGame = false;
			saveLocalAccount();
			saveLastAccountIsTryGame(false);		//本次登陆非试玩

#if(LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)
			if (this->getChildByName("LAYER_LOGIN_ROLE"))
			{
				this->removeChildByName("LAYER_LOGIN_ROLE");
			}
			LayerLoginRole* layer = LayerLoginRole::create();
			this->addChild(layer, 9 ,"LAYER_LOGIN_ROLE");
			m_ui.pLoginTipsPnl->setVisible(false);
			m_ui.pLoginTipsTryPnl->setVisible(false);
#else
			m_pnlCurShow->setVisible(false);
			m_ui.pLoginTipsPnl->setVisible(true);
			m_pnlCurShow = m_ui.pLoginTipsPnl;

			ostringstream str;
			str << m_strUserName[0] << m_strUserName[1] << "***" << m_strUserName[m_strUserName.length()-1]
				<< DataManager::getInstance()->searchCommonTexdtById(10258) << m_iDownTime << "S";
			m_ui.pTextCntDown->setString(str.str());

			//启动定时器 2s内可以点击切换账号, 2s后进入角色登陆;
			getScheduler()->schedule(CC_CALLBACK_1(LayerLogin::downTime, this), this, 1, 2, 1,false, "downTime");
#endif
		}
		break;

	default:	
		break;
	}
}


void LayerLogin::onRecvRegisterData( int nResult, const char* szMsg )
{
	switch (nResult)
	{
	case REGISTER_RESULT_SUCCESS:
		{
			CCLOG("reg success");
			if (SystemUtils::getChannelId() == CHANNEL_ID_APPSTORE) //IOS正版热云注册登入统计上报
			{
				//热云统计  注册之后马上自动登入
				ReyunController::ReYunRegister(TO_STR(LoginModel::getInstance()->getUserId()));
				ReyunController::ReYunLogin(TO_STR(LoginModel::getInstance()->getUserId()));
			}
			m_strUserName = m_ui.pEbxRegName->getText();
			m_strPassword = m_ui.pEbxRegPwd->getText();
			m_bIsLastLoginTryGame = false;
			saveLocalAccount();
			saveLastAccountIsTryGame(false);		//注册并登录

#if(LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)
			if (this->getChildByName("LAYER_LOGIN_ROLE"))
			{
				this->removeChildByName("LAYER_LOGIN_ROLE");
			}
			LayerLoginRole* layer = LayerLoginRole::create();
			this->addChild(layer, 9 ,"LAYER_LOGIN_ROLE");
			m_ui.pLoginTipsPnl->setVisible(false);
			m_ui.pLoginTipsTryPnl->setVisible(false);
#else
			m_pnlCurShow->setVisible(false);
			m_ui.pLoginTipsPnl->setVisible(true);
			m_pnlCurShow = m_ui.pLoginTipsPnl;

			ostringstream str;
			str << m_strUserName[0] << m_strUserName[1] << "***" << m_strUserName[m_strUserName.length()-1]
				<< DataManager::getInstance()->searchCommonTexdtById(10258) << m_iDownTime << "S";
			m_ui.pTextCntDown->setString(str.str());

			//启动定时器 2s内可以点击切换账号, 2s后进入角色登陆;
			getScheduler()->schedule(CC_CALLBACK_1(LayerLogin::downTime, this), this, 1, 2, 1,false, "downTime");
#endif
		}
		break;

	default:
		{
			CustomPop::showUtf8(szMsg);
			CCLOG("onRecvRegisterData error: %d", nResult);
		}
		break;
	}
}


void LayerLogin::onRecvModifyPwdData( int nResult, const char* szMsg )
{

	switch (nResult)
	{
	case MODIFY_PWD_RESULT_SUCCESS:
		{
			//CustomTips::show("修改密码成功!");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10259));
			//保存到本地
			m_strUserName = m_ui.pEbxOldName->getText();
			m_strPassword = m_ui.pEbxNewPwd->getText();
			m_ui.pEbxLoginPwd->setText(m_strPassword.c_str());
			saveLocalAccount();

#if(LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)
			if (this->getChildByName("LAYER_LOGIN_ROLE"))
			{
				this->removeChildByName("LAYER_LOGIN_ROLE");
			}
			LayerLoginRole* layer = LayerLoginRole::create();
			this->addChild(layer, 9 ,"LAYER_LOGIN_ROLE");
			m_ui.pLoginTipsPnl->setVisible(false);
			m_ui.pLoginTipsTryPnl->setVisible(false);
#else
			m_pnlCurShow->setVisible(false);
			m_ui.pLoginTipsPnl->setVisible(true);
			m_pnlCurShow = m_ui.pLoginTipsPnl;

			ostringstream str;
			str << m_strUserName[0] << m_strUserName[1] << "***" << m_strUserName[m_strUserName.length()-1]
				<< DataManager::getInstance()->searchCommonTexdtById(10258) << m_iDownTime << "S";
			m_ui.pTextCntDown->setString(str.str());

			//启动定时器 2s内可以点击切换账号, 2s后进入角色登陆;
			getScheduler()->schedule(CC_CALLBACK_1(LayerLogin::downTime, this), this, 1, 2, 1,false, "downTime");
#endif
		}	
		break;

	default:
		{
			CustomPop::showUtf8(szMsg);
			CCLOG("onRecvModifyPwdData error: %d", nResult);
		}	
		break;
	}
}

void LayerLogin::onRecvActivateData( int nResult, const char* szMsg )
{

	switch (nResult)
	{
	case ACTIVATE_RESULT_SUCCESS:
		{
			//CustomPop::show("帐号激活成功!");
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10260));
			m_iTryGameUId = 0;			//激活成功后试玩UID置0，开始游戏
			m_strUserName = m_ui.pEbxActName->getText();
			m_strPassword = m_ui.pEbxActPwd->getText();
			m_bIsLastLoginTryGame = false;
			saveLocalAccount();
			saveTryGameLocalAccount();
			saveLastAccountIsTryGame(false);		//已激活登录就不是试玩

#if(LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)
			if (this->getChildByName("LAYER_LOGIN_ROLE"))
			{
				this->removeChildByName("LAYER_LOGIN_ROLE");
			}
			LayerLoginRole* layer = LayerLoginRole::create();
			this->addChild(layer, 9 ,"LAYER_LOGIN_ROLE");
			m_ui.pLoginTipsPnl->setVisible(false);
			m_ui.pLoginTipsTryPnl->setVisible(false);
#else
			m_pnlCurShow->setVisible(false);
			m_ui.pLoginTipsPnl->setVisible(true);
			m_pnlCurShow = m_ui.pLoginTipsPnl;

			ostringstream str;
			str << m_strUserName[0] << m_strUserName[1] << "***" << m_strUserName[m_strUserName.length()-1]
				<< DataManager::getInstance()->searchCommonTexdtById(10258) << m_iDownTime << "S";
			m_ui.pTextCntDown->setString(str.str());

			//启动定时器 2s内可以点击切换账号, 2s后进入角色登陆;
			getScheduler()->schedule(CC_CALLBACK_1(LayerLogin::downTime, this), this, 1, 2, 1,false, "downTime");
#endif
		}	
		break;
	default:
		{
			CustomPop::showUtf8(szMsg);
			CCLOG("onRecvModifyPwdData error: %d", nResult);
		}
		break;
	}
}

void LayerLogin::onRecvTryGameData( int nResult, const char* szMsg )
{
	switch (nResult)
	{
	case TRY_GAME_RESULT_SUCCESS:
		{			
			CCLOG("login success");
			//试玩成功后，服务器返回试玩uid
			m_iTryGameUId = LoginModel::getInstance()->getUserId();
			m_bIsLastLoginTryGame = true;
			saveTryGameLocalAccount();
			saveLastAccountIsTryGame(true);			//记录本次登陆是试玩

#if(LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)
			if (this->getChildByName("LAYER_LOGIN_ROLE"))
			{
				this->removeChildByName("LAYER_LOGIN_ROLE");
			}
			LayerLoginRole* layer = LayerLoginRole::create();
			this->addChild(layer, 9 ,"LAYER_LOGIN_ROLE");
			m_ui.pLoginTipsPnl->setVisible(false);
			m_ui.pLoginTipsTryPnl->setVisible(false);
#else

			m_pnlCurShow->setVisible(false);
			m_ui.pLoginTipsTryPnl->setVisible(true);
			m_pnlCurShow = m_ui.pLoginTipsTryPnl;

			ostringstream str;
			str << DataManager::getInstance()->searchCommonTexdtById(10261) << m_iDownTime << "S";
			m_ui.pTextTryCntDown->setString(str.str());

			//开启倒计时，2s后切换到角色登陆，2s内可以点击切换账号
			getScheduler()->schedule(CC_CALLBACK_1(LayerLogin::downTime, this), this, 1, 2, 1,false, "downTime");
#endif
		}
		break;
	default:
		{
			auto call = [this](Ref* ref, CustomRetType type)->void
			{
				if (type == RET_TYPE_OK)
				{
					m_pnlCurShow->setVisible(false);
					m_ui.pLoginPnl->setVisible(true);
					m_pnlCurShow = m_ui.pLoginPnl;
					m_iDownTime = 2;		//重置为2
					m_ui.pTextTryCntDown->setString("");
				}
			};
			CustomPop::showUtf8(szMsg, call, CUSTOM_ID_YES);
			CCLOG("onRecvModifyPwdData error: %d", nResult);
		}
		break;	
	}
}

void LayerLogin::onRecvCreateRoleData()
{
	/*
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	this->setVisible(false);

	// 加入开场动画;
	LayerIntro* _layer = LayerIntro::create();
	if (nullptr != _layer)
	{
		Director::getInstance()->getRunningScene()->addChild(_layer, 9);
	}
#else
	*/
	//跳转到角色创建界面
	Director::getInstance()->replaceScene(LayerCreateRole::createScene());

//#endif
}

void LayerLogin::loadLocalAccount()
{
#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)

	// Android + SDK 不保存;
#else
	m_strUserName = GetStringFromXML(LOCAL_KEY_STR_USERNAME, "");
	m_strPassword = GetStringFromXML(LOCAL_KEY_STR_PASSWORD, "");
	m_iTryGameUId = GetIntegerFromXML(LOCAL_KEY_INT_USERID, 0);
	m_bIsLastLoginTryGame = GetBoolFromXML(LOCAL_KEY_BOOL_LASTLOGIN_ACCOUT, false);
#endif
}

void LayerLogin::saveLocalAccount()
{
#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)

	// Android + SDK 不保存;
#else
	SetStringToXML(LOCAL_KEY_STR_USERNAME, m_strUserName);
	SetStringToXML(LOCAL_KEY_STR_PASSWORD, m_strPassword);
#endif
}

void LayerLogin::saveTryGameLocalAccount()
{
#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)

	// Android + SDK 不保存;
#else
	SetIntegerToXML(LOCAL_KEY_INT_USERID, m_iTryGameUId);
#endif
}

void LayerLogin::saveLastAccountIsTryGame(bool isTryGame)
{
#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)

	// Android + SDK 不保存;
#else
	SetBoolToXML(LOCAL_KEY_BOOL_LASTLOGIN_ACCOUT, isTryGame);
#endif
}

void LayerLogin::checkUpdate()
{
	if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	{
		onUpgradeDone("");
		return;
	}

	CCLOG("+++++++++++++++ checkUpdate +++++++++++++++++");

	/////// 正常检测更新流程;
 	LayerUpgrade* _layer = LayerUpgrade::create();
	if (_layer != nullptr)
	{
		// 隐藏登陆按钮;
		m_ui.pLoginPnl->setVisible(false);

		// 开始检查更新;
		this->addChild(_layer);
		_layer->setParent(this);

		// 下一帧执行;
		auto callBack = [=](float dt)->void
		{
			this->getScheduler()->unschedule("LayerLogin::checkUpdate", this);
			_layer->checkUpgrade();
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerLogin::checkUpdate");
	}
	else
	{
		CCLOG("create layerUpgrade error !!!!!!!!!!");

		//CustomPop::show("更新模块异常");
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10262));
		//onUpgradeDone();
	}
}

void LayerLogin::onUpgradeDone(string strMsg)
{
	// 取消更新提示;
	showUpgradeTips(false);

#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)

	// sdk登陆;
	loginSdk();

	if ( SystemUtils::getChannelId() != CHANNEL_ID_YSDK && SystemUtils::getChannelId() != CHANNEL_ID_UC) //应用宝不需要SDK登陆按钮
	{
		m_ui.BtnLoginSdk->setVisible(true);

		auto callLoginSdk = [this](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (type == Widget::TouchEventType::ENDED)
			{
				// sdk登陆;
				loginSdk();
			}
		};
		m_ui.BtnLoginSdk->addTouchEventListener(callLoginSdk);
	}

#else

	m_ui.pLoginPnl->setVisible(true);
	m_pnlCurShow = m_ui.pLoginPnl;
    
#if (LOGIN_WITH_TT == 1)
    m_ui.pBtnTryPlay->setVisible(false);
    m_ui.pBtnTryPlay->setEnabled(false);
#endif

	//判断是否有帐号记录，有的话直接显示登录界面	
	if(strlen(m_strUserName.c_str()) > 0 && strlen(m_strPassword.c_str()) > 0 
		&& !LoginController::getInstance()->getIsLogoutState() && !m_bIsLastLoginTryGame)
	{
		//发送协议
		LoginController::getInstance()->loginAcount(m_strUserName.c_str(), m_strPassword.c_str());
	}
	else if(m_iTryGameUId > 0 && !LoginController::getInstance()->getIsLogoutState()
		&& m_bIsLastLoginTryGame)
	{
		//发送协议
		LoginController::getInstance()->sendTryGameMsg(m_iTryGameUId);
	}
#endif
}

void LayerLogin::loginSdk()
{
#if (LOGIN_WITH_SDK == 1 && LOGIN_WITH_TT == 0)

	// 下一帧执行;
	auto callBack = [this](float dt)->void
	{
		this->getScheduler()->unschedule("LayerLogin::loginSdk", this);

		// 隐藏账号密码界面，直接登陆SDK;
		LoginController::getInstance()->loginAcount("", 0);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerLogin::loginSdk");

#endif
}

void LayerLogin::onBtnClick( Ref* ref, Widget::TouchEventType type, eUI_BTN_FALG nWidgetName )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case Btn_ModifyPwd:
			{
				m_pnlCurShow->setVisible(false);
				m_ui.pModifyPwdPnl->setVisible(true);
				m_pnlCurShow = m_ui.pModifyPwdPnl;

				//设定默认帐号文本
				m_ui.pEbxOldName->setText(GetStringFromXML(LOCAL_KEY_STR_USERNAME,"").c_str());
			}
			break;
		case Btn_Register:
			{
				m_pnlCurShow->setVisible(false);
				m_ui.pRegisterPnl->setVisible(true);
				m_pnlCurShow = m_ui.pRegisterPnl;
			}
			break;
		case Btn_TryPlay:
			{
				//试玩
				onEventTryGame();
			}
			break;
		case Btn_Login:
			{
				//登陆事件
				onEventLogin();
			}
			break;
		case Btn_Reg:
			{
				//注册事件
				onEventRegister();
			}
			break;
		case Btn_RegBack:
		case Btn_ModBack:
		case Btn_ActBack:
			{
				//返回到输入账号登录界面
				m_pnlCurShow->setVisible(false);
				m_ui.pLoginPnl->setVisible(true);
				m_pnlCurShow = m_ui.pLoginPnl;
			}
			break;
		case Btn_SureModify:
			{
				//密码修改
				onEventModifyPwd();
			}
			break;
		case Btn_ActAccout:
			{
				//激活账号
				onEventActivate();
			}
			break;
		case Text_SwitchAccount:
			{
				getScheduler()->unschedule("downTime", this);
				m_iDownTime = 2;		//重置为2

				//返回到输入账号登录界面
				m_pnlCurShow->setVisible(false);
				m_ui.pLoginPnl->setVisible(true);
				m_pnlCurShow = m_ui.pLoginPnl;
			}
			break;
		case Text_ActAccount:
			{
				getScheduler()->unschedule("downTime", this);
				m_iDownTime = 2;		//重置为2

				//返回到激活账号界面
				m_pnlCurShow->setVisible(false);
				m_ui.pActivatePnl->setVisible(true);
				m_pnlCurShow = m_ui.pActivatePnl;
			}
			break;
		case Text_Agreement:
			//免责协议
			CCLOG("+++++++++++AGREEMENT+++++++++");
			break;

		default:
			break;
		}
	}
}

void LayerLogin::onCheckBoxShowPwd( Ref* ref, CheckBoxEventType type )
{
	if(ref == m_ui.pBtnShowPwd)
	{
		if(m_ui.pBtnShowPwd->getSelectedState())
		{
			//设置为明文
			m_ui.pEbxRegPwd->setInputFlag(extension::EditBox::InputFlag::SENSITIVE);
			m_ui.pEbxRegPwd->setText(m_ui.pEbxRegPwd->getText());
		}
		else
		{
			m_ui.pEbxRegPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);
			m_ui.pEbxRegPwd->setText(m_ui.pEbxRegPwd->getText());
		}
	}
	else if(ref == m_ui.pBtnShowSurePwd)
	{
		if(m_ui.pBtnShowSurePwd->getSelectedState())
		{
			m_ui.pEbxSurePwd->setInputFlag(extension::EditBox::InputFlag::SENSITIVE);
			m_ui.pEbxSurePwd->setText(m_ui.pEbxSurePwd->getText());
		}
		else
		{
			m_ui.pEbxSurePwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);
			m_ui.pEbxSurePwd->setText(m_ui.pEbxSurePwd->getText());
		}
	}
	else if(ref == m_ui.pBtnShowOldPwd)
	{
		if(m_ui.pBtnShowOldPwd->getSelectedState())
		{
			m_ui.pEbxOldPwd->setInputFlag(extension::EditBox::InputFlag::SENSITIVE);
			m_ui.pEbxOldPwd->setText(m_ui.pEbxOldPwd->getText());
		}
		else
		{
			m_ui.pEbxOldPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);
			m_ui.pEbxOldPwd->setText(m_ui.pEbxOldPwd->getText());
		}
	}
	else if(ref == m_ui.pBtnShowNewPwd)
	{
		if(m_ui.pBtnShowNewPwd->getSelectedState())
		{
			m_ui.pEbxNewPwd->setInputFlag(extension::EditBox::InputFlag::SENSITIVE);
			m_ui.pEbxNewPwd->setText(m_ui.pEbxNewPwd->getText());
		}
		else
		{
			m_ui.pEbxNewPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);
			m_ui.pEbxNewPwd->setText(m_ui.pEbxNewPwd->getText());
		}
	}
	else if(ref == m_ui.pBtnShowSureNewPwd)
	{
		if(m_ui.pBtnShowSureNewPwd->getSelectedState())
		{
			m_ui.pEbxSureNewPwd->setInputFlag(extension::EditBox::InputFlag::SENSITIVE);
			m_ui.pEbxSureNewPwd->setText(m_ui.pEbxSureNewPwd->getText());
		}
		else
		{
			m_ui.pEbxSureNewPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);
			m_ui.pEbxSureNewPwd->setText(m_ui.pEbxSureNewPwd->getText());
		}
	}
	else if(ref == m_ui.pBtnShowActPwd)
	{
		if(m_ui.pBtnShowActPwd->getSelectedState())
		{
			m_ui.pEbxActPwd->setInputFlag(extension::EditBox::InputFlag::SENSITIVE);
			m_ui.pEbxActPwd->setText(m_ui.pEbxActPwd->getText());
		}
		else
		{
			m_ui.pEbxActPwd->setInputFlag(extension::EditBox::InputFlag::PASSWORD);
			m_ui.pEbxActPwd->setText(m_ui.pEbxActPwd->getText());
		}
	}
}

void LayerLogin::onEventLogin()
{
	//如果是正常登陆
	const char * szUserName = m_ui.pEbxLoginName->getText();
	const char * szPassword = m_ui.pEbxLoginPwd->getText();

	//先检查帐号后检查密码
	if(checkNameFormat(szUserName))
	{
		if(checkPwdFormat(szPassword))
		{
			LoginController::getInstance()->loginAcount( szUserName, szPassword);
		}
	}
}

void LayerLogin::onEventTryGame()
{
	//试玩登录
	LoginController::getInstance()->sendTryGameMsg(m_iTryGameUId);
}

void LayerLogin::onEventModifyPwd()
{
	const char * szUserName = m_ui.pEbxOldName->getText();
	const char * szOldPwd = m_ui.pEbxOldPwd->getText();
	const char * szNewPwd = m_ui.pEbxNewPwd->getText();
	const char * szSureNewPwd = m_ui.pEbxSureNewPwd->getText();

	//先检查帐号后检查密码
	if(checkNameFormat(szUserName))
	{
		if(strcmp(szOldPwd, m_strPassword.c_str()) != 0)
		{
			//CustomPop::show("原密码不正确!");
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10263));
			return;
		}
		else
		{
			if(checkPwdFormat(szNewPwd))
			{
				if(strcmp(szNewPwd, szSureNewPwd) != 0)
				{
					//CustomPop::show("两次输入新密码不一致!");		
					CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10264));
				}
				else if(strcmp(szOldPwd, szNewPwd) == 0)
				{
					//CustomPop::show("新密码和原密码不能一样!");
					CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10265));
				}
				else
				{
					//发送修改协议
					LoginController::getInstance()->sendModifyPasswordMsg(szUserName, szOldPwd, szNewPwd);
				}
			}
		}	
	}
}

void LayerLogin::onEventRegister()
{
	const char * szUserName = m_ui.pEbxRegName->getText();
	const char * szPwd = m_ui.pEbxRegPwd->getText();
	const char * szSurePwd = m_ui.pEbxSurePwd->getText();

	//先检查帐号后检查密码
	if(checkNameFormat(szUserName))
	{
		if(checkPwdFormat(szPwd))
		{
			if(strcmp(szPwd, szSurePwd) != 0)
			{
				//CustomPop::show("两次输入密码不一致!");
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10264));
			}
			else
			{
				//注册
				LoginController::getInstance()->sendRegAndLoginMsg(szUserName, szPwd);
			}		
		}
	}
}

void LayerLogin::onEventActivate()
{
	const char * szUserName = m_ui.pEbxActName->getText();
	const char * szPwd = m_ui.pEbxActPwd->getText();

	if(checkNameFormat(szUserName))
	{
		if(checkPwdFormat(szPwd))
		{
			LoginController::getInstance()->sendActAccountMsg(m_iTryGameUId, szUserName, szPwd);
		}
	}
}

void LayerLogin::downTime( float dt )
{	
	if(m_pnlCurShow == m_ui.pLoginTipsPnl)
	{	
		m_iDownTime--;
		if(m_iDownTime <= 0)
		{
			getScheduler()->unschedule("downTime", this);
			m_iDownTime = 2;		//重置为2
			if (this->getChildByName("LAYER_LOGIN_ROLE"))
			{
				this->removeChildByName("LAYER_LOGIN_ROLE");
			}
			LayerLoginRole* layer = LayerLoginRole::create();
			this->addChild(layer, 9 ,"LAYER_LOGIN_ROLE");	
		}
		else
		{
			ostringstream str;
			str << m_strUserName[0] << m_strUserName[1] << "***" << m_strUserName[m_strUserName.length()-1]
				<< DataManager::getInstance()->searchCommonTexdtById(10258)<< m_iDownTime << "S";
			m_ui.pTextCntDown->setString(str.str());
		}
	}
	else if(m_pnlCurShow == m_ui.pLoginTipsTryPnl)
	{
		m_iDownTime--;
		if(m_iDownTime <= 0)
		{
			// 切换至分区登陆页面;
			if (this->getChildByName("LAYER_LOGIN_ROLE"))
			{
				this->removeChildByName("LAYER_LOGIN_ROLE");
			}
			LayerLoginRole* layer = LayerLoginRole::create();
			this->addChild(layer, 9 ,"LAYER_LOGIN_ROLE");

			getScheduler()->unschedule("downTime", this);
			m_iDownTime = 2;		//重置为2
		}
		else
		{
			ostringstream str;
			str << DataManager::getInstance()->searchCommonTexdtById(10261) << m_iDownTime << "S";
			m_ui.pTextTryCntDown->setString(str.str());
		}
	}
}

bool LayerLogin::checkNameFormat( const char* userName )
{
	if ( 0 == strlen(userName))
	{
		//CustomPop::show("帐号不能为空!");
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10266));
		return false;
	}
	else if(!StringFormat::machString_E_N_UNDERLINE(userName))
	{
		//CustomPop::show("用户名包含非法字符!");
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10267));
		return false;
	}
	else if(strlen(userName) < 8)
	{
		if(LOGIN_WITH_TT == 1)
		{
			//CustomPop::show("帐号最少8个字符!");
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20328));
			return false;
		}
		else
		{
			if(strlen(userName) < 6)
			{
				//CustomPop::show("帐号最少6个字符!");
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10268));
				return false;
			}
		}
	}
	else if(strlen(userName) > 15)
	{
		//CustomPop::show("帐号不能超过15个字符!");
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10269));
		return false;
	}

	return true;
}

bool LayerLogin::checkPwdFormat( const char* password )
{
	if ( 0 == strlen(password))
	{
		//CustomPop::show("密码不能为空!");
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10270));
		return false;
	}
	else if(!StringFormat::machString_E_N_UNDERLINE(password))
	{
		//CustomPop::show("密码只有由字母/下划线/数字组成!");
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10271));
		return false;
	}
	else if(strlen(password) < 6)
	{
		//CustomPop::show("密码最少6个字符!");
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10272));
		return false;
	}
	else if(strlen(password) > 15)
	{
		//CustomPop::show("密码不能超过15个字符!");
		CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10273));
		return false;
	}

	return true;
}

void LayerLogin::showUpgradeTips( bool bShow /*= true*/ )
{
	if (bShow)
	{
		if (m_txtUpgradeTips == nullptr)
		{
			m_txtUpgradeTips = Text::create(DataManager::getInstance()->searchCommonTexdtById(10274), FONT_FZZHENGHEI, 24);
			m_txtUpgradeTips->setPosition(Vec2(m_ui.pRoot->getContentSize().width/2.0f, 140.0f));
			m_ui.pRoot->addChild(m_txtUpgradeTips);
		}
	}
	else
	{
		if (m_txtUpgradeTips != nullptr)
		{
			m_txtUpgradeTips->removeFromParent();
			m_txtUpgradeTips = nullptr;
		}
	}
}






