#include "PlayerInfoLayer.h"
#include "Utils/StringFormat.h"
#include "Temp/CustomTips.h"
#include "Widget/ButtonSwitch.h"
#include "PlayerInfoController.h"
#include "Communication/Command.h"
#include "Widget/ActionCreator.h"
#include "PlayerInfoModule/PlayerInfoModel.h"
#include "LoginModule/LoginController.h"
#include "VIPModule/VipScene.h"
#include "DataManager/DataManager.h"
#include "ToolModule/UIToolHeader.h"
#include "Temp/PopReward.h"
#include "Temp/CustomGoldTips.h"
#include "Widget/LayerCustomerService.h"

//修改昵称需要的金币所在ID
#define CHANGE_NICK_PRICE_SYSTEM_ID		10090
#define MAX_LENTH_NAME 10

PlayerInfoLayer::PlayerInfoLayer()
{
	playerInfo = MainModel::getInstance()->getMainParam();
	mNameList.clear();
	m_vBtnSwitch.clear();
	m_bIsEnterSysSet = false;
}

PlayerInfoLayer::~PlayerInfoLayer()
{

}

void PlayerInfoLayer::onEnter()
{
	ModalLayer::onEnter();
	if(m_bIsEnterSysSet)
	{
		ActionCreator::runCommDlgAction(m_ui.imgSysSettingPnl);
	}
	else
	{
		ActionCreator::runCommDlgAction(m_ui.imgPlayerInfoPnl);
	}
	
	PlayerInfoModel::getInstance()->addObserver(this);
}

void PlayerInfoLayer::onExit()
{
	ModalLayer::onExit();
	PlayerInfoModel::getInstance()->removeObserver(this);
}

bool PlayerInfoLayer::init()
{
	if(!ModalLayer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void PlayerInfoLayer::initUI()
{
	// 加载 UI;
	string strFileName;
	strFileName.assign("Image/UIInformation/UIInformation.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.pRoot);

	//初始化玩家信息界面
	initPlayerInfoView();

	//初始化修改昵称界面
	initModifyNameView();

	//初始化系统设置界面
	initSystemSetView();

	if(m_bIsEnterSysSet)
	{
		m_ui.imgSysSettingPnl->setVisible(true);
		m_ui.imgPlayerInfoPnl->setVisible(false);

		for(int i=0; i<m_vBtnSwitch.size(); i++)
		{
			m_vBtnSwitch.at(i)->setVisible(true);
		}	
	}
}

void PlayerInfoLayer::initPlayerInfoView()
{
	//层
	m_ui.imgPlayerInfoPnl = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_Player"));

	Button* btn_infoClose = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_infoClose"));
	btn_infoClose->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_infoCloseBtn));
	Button* btn_modifyNick = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Change"));
	btn_modifyNick->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_modifyNickBtn));
	Button* btn_backToLogin = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_BackLogin"));
	btn_backToLogin->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_backToLoginBtn));
	Button* btn_sysSetting = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Set"));
	btn_sysSetting->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_sysSettingBtn));
	Button* btn_sysGm = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_GM"));
	btn_sysGm->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_sysGmBtn));

	//月卡，有的渠道可能不开放此功能
	Button* btn_MonthCard = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Card"));
	btn_MonthCard->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_monthCard));
	//btn_MonthCard->setVisible(MainModel::getInstance()->getMonthCardIsOpen());
	btn_MonthCard->setVisible(false);

	//玩家头像
	m_ui.pnlHead = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Head"));
	HeroParam* playerParam = HeroModel::getInstance()->getMainHero();
	UIToolHeader* imgHead = UIToolHeader::create(ResourceUtils::getSmallIconPath(playerParam->mResId));
	imgHead->setLvl(playerParam->mLevel);
	imgHead->setAnchorPoint(Point::ZERO);
	m_ui.pnlHead->addChild(imgHead);
	
	//玩家昵称
	m_ui.txtName = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Name"));
	m_ui.txtName->setString(playerInfo->mName);
	m_ui.txtName->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtName, Color3B::BLACK, 2);

	//vip等级
	m_ui.btnVip = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_vipLv"));
	m_ui.btnVip->setPositionX(m_ui.txtName->getPositionX() + (float)m_ui.txtName->getStringLength()*(float)m_ui.txtName->getFontSize()*0.5f + 10);
	m_ui.btnVip->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_vipLvBtn));
	ostringstream oss;
	oss << "Image/UIVip/VipLv/btn_VIP" << _TO_STR(playerInfo->mVipLevel) << ".png";
	m_ui.btnVip->loadTextureNormal(oss.str());

	//玩家帮派
	m_ui.txtGang = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Gang"));
	m_ui.txtGang->setFontName(FONT_FZZHENGHEI);
	m_ui.txtGang->setString(MainModel::getInstance()->getMainParam()->mGroupName);
	if(MainModel::getInstance()->getMainParam()->mGroupId > 0)
	{	
		m_ui.txtGang->setColor(Color3B(0xFF, 0xE9, 0x34));
	}
	else
	{
		m_ui.txtGang->setColor(Color3B(0x8F, 0x8B, 0x8D));
	}
	
	//玩家UID码
	m_ui.txtUID = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_UID"));
	m_ui.txtUID->setString(_TO_STR(playerInfo->mRoleId));
	m_ui.txtUID->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtUID, Color3B(0x7d, 0x3f, 0x1c), 2);

	//经验条
	m_ui.txtExp = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_EXP"));
	m_ui.barExp = (LoadingBar*)(Helper::seekWidgetByName(m_ui.pRoot, "Bar_EXP"));
	int heroCurExp = HeroModel::getInstance()->getMainHero()->mExp;
	int nextLvMaxExp = HeroModel::getInstance()->getMainHero()->mExpMax;
	string expStr = _TO_STR(heroCurExp) + string("/") + _TO_STR(nextLvMaxExp);
	m_ui.txtExp->setFontName(FONT_FZZHENGHEI);	
	m_ui.txtExp->setString(expStr);
	setTextAddStroke(m_ui.txtExp, Color3B(0x73, 0x1b, 0x07), 2);
	if(nextLvMaxExp != 0)
	{
		m_ui.barExp->setPercent(((float)heroCurExp)/((float)nextLvMaxExp)*100);
	}
	else
	{
		m_ui.barExp->setPercent(100.0f);		//满级
	}

	//体力相关
	m_ui.txtPower = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_PowerCount"));
	m_ui.txtPowerFull = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_PowerFull"));
	m_ui.txtPowerTime = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_PowerTime"));
	m_ui.txtPowerWillFull = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_PowerTips"));
	int curPower = playerInfo->mPower;
	int maxPower = playerInfo->mMaxPower;
	string powerStr = _TO_STR(curPower) + string("/") + _TO_STR(maxPower);
	m_ui.txtPower->setString(powerStr);
	m_ui.txtPower->setFontName(FONT_FZZHENGHEI);
	m_ui.txtPowerFull->setFontName(FONT_FZZHENGHEI);
	m_ui.txtPowerTime->setFontName(FONT_FZZHENGHEI);
	m_ui.txtPowerWillFull->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtPower, Color3B(0x00, 0x00, 0x00), 2);
	setTextAddStroke(m_ui.txtPowerFull, Color3B(0x78, 0x08, 0x00), 2);
	setTextAddStroke(m_ui.txtPowerTime, Color3B(0x18, 0x4a, 0x0b), 2);
	//体力已满
	if(curPower >= maxPower)
	{
		m_ui.txtPowerFull->setVisible(true);
		m_ui.txtPowerTime->setVisible(false);
		m_ui.txtPowerWillFull->setVisible(false);
	}
	else
	{
		m_ui.txtPowerFull->setVisible(false);
		m_ui.txtPowerTime->setVisible(true);
		m_ui.txtPowerWillFull->setVisible(true);
		//体力恢复时间
		int st = MainModel::getInstance()->powerTime;
		updateResTime(st, true);	
	}

	//精力相关
	m_ui.txtEnergy = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_EnergyCount"));
	m_ui.txtEnergyFull = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_EnergyFull"));
	m_ui.txtEnergyTime = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_EnergyTime"));
	m_ui.txtEnergyWillFull = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_EnergyTips"));
	int curEnergy = playerInfo->mEnergy;
	int maxEnergy = playerInfo->mMaxEnergy;
	string energyStr = _TO_STR(curEnergy) + string("/") + _TO_STR(maxEnergy);
	m_ui.txtEnergy->setString(energyStr);
	m_ui.txtEnergy->setFontName(FONT_FZZHENGHEI);
	m_ui.txtEnergyFull->setFontName(FONT_FZZHENGHEI);
	m_ui.txtEnergyTime->setFontName(FONT_FZZHENGHEI);
	m_ui.txtEnergyWillFull->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtEnergy, Color3B(0x00, 0x00, 0x00), 2);
	setTextAddStroke(m_ui.txtEnergyFull, Color3B(0x78, 0x08, 0x00), 2);
	setTextAddStroke(m_ui.txtEnergyTime, Color3B(0x18, 0x4a, 0x0b), 2);
	//精力已满
	if(curEnergy >= maxEnergy)
	{
		m_ui.txtEnergyFull->setVisible(true);
		m_ui.txtEnergyTime->setVisible(false);
		m_ui.txtEnergyWillFull->setVisible(false);
	}
	else
	{
		m_ui.txtEnergyFull->setVisible(false);
		m_ui.txtEnergyTime->setVisible(true);
		m_ui.txtEnergyWillFull->setVisible(true);
		//精力恢复时间
		int st = MainModel::getInstance()->energyTime;
		updateResTime(st, false);		
	}
}

void PlayerInfoLayer::initModifyNameView()
{
	//层
	m_ui.imgModifyNamePnl = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_Change"));

	Button* btn_modifyClose = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_modifyClose"));
	btn_modifyClose->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_modifyCloseBtn));
	Button* btn_dice = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Dice"));
	btn_dice->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_diceBtn));
	Button* btn_cancel = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Cancel"));
	btn_cancel->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_cancelBtn));
	Button* btn_sure = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Sure"));
	btn_sure->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_sureBtn));

	//昵称修改
	m_ui.txtInputName = dynamic_cast<cocos2d::ui::TextField*>(Helper::seekWidgetByName(m_ui.pRoot, "TF_Name"));
	m_ui.editBoxName = EditBoxEx::create(m_ui.txtInputName);
	m_ui.editBoxName->setMaxLength(m_ui.txtInputName->getMaxLength());
	m_ui.editBoxName->setText(playerInfo->mName.c_str());
	m_ui.editBoxName->setFontName(SystemUtils::getFontFileName().c_str());
	m_ui.txtInputName->getParent()->addChild(m_ui.editBoxName);

	//"本次修改需要"文本设置字体
	Text* needGoldTips = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Tips"));
	needGoldTips->setFontName(FONT_FZZHENGHEI);
	//修改需要的金币
	RowData* priceData = DataManager::getInstance()->getDTToolSystem()->searchDataById(CHANGE_NICK_PRICE_SYSTEM_ID);
	m_ui.txtPrice = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Price"));
	m_ui.txtPrice->setString(_TO_STR(priceData->getIntData("value")));
	m_ui.txtPrice->setFontName(FONT_FZZHENGHEI);
}

void PlayerInfoLayer::initSystemSetView()
{
	//层
	m_ui.imgSysSettingPnl = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Img_Set"));

	Button* btn_settingClose = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_settingClose"));
	btn_settingClose->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_settingCloseBtn));
	Button* btn_exchange = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Exchange"));
	btn_exchange->addTouchEventListener(CC_CALLBACK_2(PlayerInfoLayer::onBtnClick, this, btn_exchangeBtn));
	btn_exchange->setVisible(MainModel::getInstance()->getActCodeIsOpen());

	//兑换码输入框
	m_ui.txtInputCode = dynamic_cast<cocos2d::ui::TextField*>(Helper::seekWidgetByName(m_ui.pRoot, "TF_Input"));
	m_ui.editBoxCode = EditBoxEx::create(m_ui.txtInputCode);
	m_ui.editBoxCode->setMaxLength(m_ui.txtInputCode->getMaxLength());
	string str = DataManager::getInstance()->searchCommonTexdtById(20224);
	//m_ui.editBoxCode->setText(str.c_str());
	m_ui.editBoxCode->setPlaceHolder(str.c_str());
	m_ui.editBoxCode->setFontName(SystemUtils::getFontFileName().c_str());
	m_ui.editBoxCode->setInputMode(EditBox::InputMode::SINGLE_LINE);
	//m_ui.editBoxCode->setEnabled(false);				//没开放，不让输入兑换码
	m_ui.txtInputCode->getParent()->addChild(m_ui.editBoxCode);
	m_ui.txtInputCode->getParent()->setVisible(MainModel::getInstance()->getActCodeIsOpen());

	//版本号
	Text* curVersionTips = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Ver"));
	curVersionTips->setFontName(FONT_FZZHENGHEI);
	string strVersion("");
	strVersion.append(GetStringFromXML(LOCAL_KEY_STR_CURRENT_VERSION, ""));
	m_ui.txtVersion = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Ver1"));
	m_ui.txtVersion->setString(strVersion.c_str());
	m_ui.txtVersion->setFontName(FONT_FZZHENGHEI);

	//音乐开关
	Layout* pnlSwitchMusic = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_switchMusic"));
	ButtonSwitch* musicSwtich = ButtonSwitch::create();
	musicSwtich->setOn(SoundUtils::getInstance()->getSoundMusicOn());
	musicSwtich->setTag(switch_musicTag);
	musicSwtich->setAnchorPoint(Point::ZERO);
	musicSwtich->addTargetWithActionForControlEvents(this, cccontrol_selector(PlayerInfoLayer::valueChanged), Control::EventType::VALUE_CHANGED);
	pnlSwitchMusic->addChild(musicSwtich);
	m_vBtnSwitch.push_back(musicSwtich);

	//音效开关
	Layout* pnlSwitchSound = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_switchSound"));
	ButtonSwitch* soundSwtich= ButtonSwitch::create();
	soundSwtich->setOn(SoundUtils::getInstance()->getSoundEffectOn());
	soundSwtich->setTag(switch_soundTag);
	soundSwtich->setAnchorPoint(Point::ZERO);
	soundSwtich->addTargetWithActionForControlEvents(this, cccontrol_selector(PlayerInfoLayer::valueChanged), Control::EventType::VALUE_CHANGED);
	pnlSwitchSound->addChild(soundSwtich);
	m_vBtnSwitch.push_back(soundSwtich);

	//体力已满提示开关
	Layout* pnlSwitchPowerTips = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_switchPowerTips"));
	ButtonSwitch* powerSwtich = ButtonSwitch::create();
	powerSwtich->setTag(swtich_powerTag);
	powerSwtich->setAnchorPoint(Point::ZERO);
	powerSwtich->addTargetWithActionForControlEvents(this, cccontrol_selector(PlayerInfoLayer::valueChanged), Control::EventType::VALUE_CHANGED);
	pnlSwitchPowerTips->addChild(powerSwtich);
	m_vBtnSwitch.push_back(powerSwtich);

	//精力已满提示开关
	Layout* pnlSwitchEnergyTips = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_switchEnergyTips"));
	ButtonSwitch* energyTipsSwtich = ButtonSwitch::create();
	energyTipsSwtich->setTag(swtich_energyTag);
	energyTipsSwtich->setAnchorPoint(Point::ZERO);
	energyTipsSwtich->addTargetWithActionForControlEvents(this, cccontrol_selector(PlayerInfoLayer::valueChanged), Control::EventType::VALUE_CHANGED);
	pnlSwitchEnergyTips->addChild(energyTipsSwtich);
	m_vBtnSwitch.push_back(energyTipsSwtich);

	//免费体力提示开关
	Layout* pnlSwitchFreeTips = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_switchFreePowerTips"));
	ButtonSwitch* freeTipsSwtich = ButtonSwitch::create();
	freeTipsSwtich->setTag(swtich_freeTipsTag);
	freeTipsSwtich->setAnchorPoint(Point::ZERO);
	freeTipsSwtich->addTargetWithActionForControlEvents(this, cccontrol_selector(PlayerInfoLayer::valueChanged), Control::EventType::VALUE_CHANGED);
	pnlSwitchFreeTips->addChild(freeTipsSwtich);
	m_vBtnSwitch.push_back(freeTipsSwtich);

	//世界boss开关
	Layout* pnlSwitchBossTips = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_switchBossTips"));
	ButtonSwitch* bossTipsSwtich = ButtonSwitch::create();
	bossTipsSwtich->setTag(swtich_bossTag);
	bossTipsSwtich->setAnchorPoint(Point::ZERO);
	bossTipsSwtich->addTargetWithActionForControlEvents(this, cccontrol_selector(PlayerInfoLayer::valueChanged), Control::EventType::VALUE_CHANGED);
	pnlSwitchBossTips->addChild(bossTipsSwtich);
	m_vBtnSwitch.push_back(bossTipsSwtich);

	for(int i=0; i<m_vBtnSwitch.size(); i++)
	{
		m_vBtnSwitch.at(i)->setVisible(false);
	}
}

void PlayerInfoLayer::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if(type == Widget::TouchEventType::ENDED)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case btn_infoCloseBtn:
			{
				this->dispose();
			}
			break;
		case btn_vipLvBtn:
			{
				Director::getInstance()->pushScene(VipScene::createScene());
			}
			break;
		case btn_monthCard:
			{
				Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
			}
			break;
		case btn_backToLoginBtn:
			{
				auto callBack = [this]( Ref* pSender, CustomRetType type )->void
				{
					switch (type)
					{
					case RET_TYPE_OK:
						LoginController::getInstance()->sendLogoutMsg();
						break;
					case RET_TYPE_CANCEL:
					case RET_TYPE_CLOSE:
						break;
					default:
						break;
					}
				};

				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20225), callBack, CustomPopType::CUSTOM_YES_NO_CANCEL);
				
			}
			break;
		case btn_modifyNickBtn:
			{
				m_ui.imgPlayerInfoPnl->setEnabled(false);
				m_ui.imgPlayerInfoPnl->setOpacity(220);
				m_ui.imgModifyNamePnl->setVisible(true);

				ActionCreator::runCommDlgAction(m_ui.imgModifyNamePnl);
			}
			break;
		case btn_sysSettingBtn:
			{
				m_ui.imgPlayerInfoPnl->setVisible(false);
				m_ui.imgSysSettingPnl->setVisible(true);

				for(int i=0; i<m_vBtnSwitch.size(); i++)
				{
					m_vBtnSwitch.at(i)->setVisible(true);
				}

				ActionCreator::runCommDlgAction(m_ui.imgSysSettingPnl);
			}
			break;
		case btn_sysGmBtn:
			{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				{
					LayerCustomerService* _layer = LayerCustomerService::create();
					if (nullptr != _layer)
					{
						Director::getInstance()->getRunningScene()->addChild(_layer);
					}
				}
#endif
			}
			break;
		case btn_modifyCloseBtn:
			{
				m_ui.imgModifyNamePnl->setVisible(false);
				m_ui.imgPlayerInfoPnl->setEnabled(true);
				m_ui.imgPlayerInfoPnl->setOpacity(255);

			}
			break;
		case btn_diceBtn:
			{
				//随机名字
				setRoleName();
			}
			break;
		case btn_cancelBtn:
			{
				//取消修改名字则返回信息层
				m_ui.imgModifyNamePnl->setVisible(false);
				m_ui.imgPlayerInfoPnl->setEnabled(true);
			}
			break;
		case btn_sureBtn:
			{
				//确认修改名字
				if(!strcmp(m_ui.editBoxName->getText(), playerInfo->mName.c_str()))
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20226));
				}
				else if(StringFormat::getStringLenth(m_ui.editBoxName->getText()) > NICK_NAME_LENGTH_LIMIT)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10252));
				}
				else if(!strcmp(m_ui.editBoxName->getText(), ""))
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20228));
				}
				else
				{
					//if (StringFormat::machString(string(m_ui.editBoxName->getText())))
					//{
						RowData* priceData = DataManager::getInstance()->getDTToolSystem()->searchDataById(CHANGE_NICK_PRICE_SYSTEM_ID);
						MainParam* pMainParam = MainModel::getInstance()->getMainParam();
						if (pMainParam != nullptr && pMainParam->mGold < priceData->getIntData("value"))
						{
							//金币不够
							CustomGoldTips *_tips = CustomGoldTips::create();
							if (_tips)
							{
								Director::getInstance()->getRunningScene()->addChild(_tips);
							}
							return;
						}

						string strName = string(m_ui.editBoxName->getText());
						PlayerInfoController::sendModifyNickNameMsg(strName);
					//}
					//else
					//{
					//	CustomTips::show("含有非法字符,请重新输入");
					//	//m_ui.editBoxName->setText(playerInfo->mName.c_str());
					//}
				}
			}
			break;
		case btn_settingCloseBtn:
			{
				if(m_bIsEnterSysSet)
				{
					this->dispose();
				}
				else
				{
					m_ui.imgSysSettingPnl->setVisible(false);
					m_ui.imgPlayerInfoPnl->setVisible(true);

					for(int i=0; i<m_vBtnSwitch.size(); i++)
					{
						m_vBtnSwitch.at(i)->setVisible(false);
					}		
				}				
			}
			break;
		case btn_exchangeBtn:
			{
				if(MainModel::getInstance()->getActCodeIsOpen())
				{
					if(StringFormat::getStringLenth(m_ui.editBoxCode->getText()) > m_ui.editBoxCode->getMaxLength())
					{
						CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20229));
					}
					else
					{
						PlayerInfoController::sendExchangeMsg(m_ui.editBoxCode->getText());
					}
				}
				else
				{
					CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20230));
				}
			}
			break;
		
		default:
			break;
		}
	}
}

//修改昵称骰子
void PlayerInfoLayer::setRoleName()
{
	if (mNameList.empty())
	{
		mNameList.clear();
		mNameIndex = 0;
		PlayerInfoController::sendGetRandomNameMsg(nMAIN_CMD_RANDOM_NAMES);
	}
	if (mNameIndex < mNameList.size())
	{
		m_ui.editBoxName->setText(mNameList.at(mNameIndex).c_str());
		mNameIndex++;
	}
	else
	{
		mNameList.clear();
		mNameIndex = 0;
		PlayerInfoController::sendGetRandomNameMsg(nMAIN_CMD_RANDOM_NAMES);
	}
}

void PlayerInfoLayer::showExChangeReward()
{
		vector<TOOL_NEW_INFO>* rewardInfo = PlayerInfoModel::getInstance()->getNewTools();
		vector<PopRewardItem*> vecRewardItem;

		for(auto info : *rewardInfo)
		{
			if(info.ntemplateId == TOOL_ID_GOLD)
			{
				PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(info.nstack);
				vecRewardItem.push_back(rewardItem);
			}
			else if(info.ntemplateId == TOOL_ID_COIN)
			{
				PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(info.nstack);
				vecRewardItem.push_back(rewardItem);
			}
			else
			{
				PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(info.ntemplateId, info.nstack);
				vecRewardItem.push_back(rewardItem);
			}
		}

		PopReward::show(vecRewardItem);
}

void PlayerInfoLayer::valueChanged(Ref* sender, Control::EventType controlEvent)
{
	if(controlEvent == Control::EventType::VALUE_CHANGED)
	{
		SoundUtils::playSoundEffect("dianji");
		ButtonSwitch* switchBtn = (ButtonSwitch*)sender;
		switch (switchBtn->getTag())
		{
		case switch_musicTag:
			{
				SoundUtils::getInstance()->setSoundMusicOn(switchBtn->isOn());
				if(switchBtn->isOn())
				{
					SoundUtils::playSoundMusic("main_1");
				}
			}
			break;
		case switch_soundTag:
			{
				SoundUtils::getInstance()->setSoundEffectOn(switchBtn->isOn());
			}
			break;
		case swtich_powerTag:
			{
				//消息推送
			}
			break;
		case swtich_energyTag:
			{
				//消息推送
			}
			break;
		case swtich_freeTipsTag:
			{
				//消息推送
			}
			break;
		case swtich_bossTag:
			{
				//消息推送
			}
			break;

		default:
			break;
		}
	}
}

//更新体力/精力倒计时
void PlayerInfoLayer::updateResTime(int st, bool isPower)
{
	if(st <= 0)
	{
		//体力/精力已满
		(isPower ? m_ui.txtPowerFull : m_ui.txtEnergyFull)->setVisible(true);
		(isPower ? m_ui.txtPowerTime : m_ui.txtEnergyTime)->setVisible(false);
		(isPower ? m_ui.txtPowerWillFull : m_ui.txtEnergyWillFull)->setVisible(false);
	}
	else
	{
		(isPower ? m_ui.txtPowerFull : m_ui.txtEnergyFull)->setVisible(false);
		(isPower ? m_ui.txtPowerTime : m_ui.txtEnergyTime)->setVisible(true);
		(isPower ? m_ui.txtPowerWillFull : m_ui.txtEnergyWillFull)->setVisible(true);
		//体力/精力恢复时间
		string strResTime;
		string ht;
		string mint;
		string sect;
		int h = st/3600;
		if(h <= 9)
		{
			ht = _TO_STR(0) + _TO_STR(h);
		}
		else
		{
			ht = _TO_STR(h);
		}
		int min = (st%3600)/60;
		if(min <= 9)
		{
			mint = _TO_STR(0) + _TO_STR(min);
		}
		else
		{
			mint = _TO_STR(min);
		}
		int sec = st%60;
		if(sec <= 9)
		{
			sect = _TO_STR(0) + _TO_STR(sec);
		}
		else
		{
			sect = _TO_STR(sec);
		}

		strResTime = ht + string(":") + mint + string(":") + sect;
		(isPower ? m_ui.txtPowerTime : m_ui.txtEnergyTime)->setString(strResTime);
		setTextAddStroke(isPower ? m_ui.txtPowerTime : m_ui.txtEnergyTime, Color3B(0x18, 0x4a, 0x0b), 2);
	}
}

void PlayerInfoLayer::updateSelf(void* data)
{
	if(data == nullptr)
	{
		return;
	}
	ObserverParam* obParam = (ObserverParam*) data;

	switch (obParam->id)
	{
	case nMAIN_CMD_PLAYER_INFO_MODIFY_NAME:
		{		
			//确认修改昵称后，通知扣金币
			MainModel::getInstance()->getMainParam()->mName = string(m_ui.editBoxName->getText());
			HeroModel::getInstance()->getMainHero()->mName = string(m_ui.editBoxName->getText());
			MainModel::getInstance()->notifyObservers();
			//修改该页面昵称显示
			m_ui.txtName->setString(m_ui.editBoxName->getText());
			setTextAddStroke(m_ui.txtName, Color3B::BLACK, 2);
			m_ui.btnVip->setPositionX(m_ui.txtName->getPositionX() + (float)m_ui.txtName->getStringLength()*(float)m_ui.txtName->getFontSize()*0.5f + 10);	//同步更新vip按钮位置
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20231));
			m_ui.imgModifyNamePnl->setVisible(false);
			m_ui.imgPlayerInfoPnl->setEnabled(true);
		}
		break;
	case nMAIN_CMD_RANDOM_NAMES:
		{
			//服务器返回的随机昵称
			mNameList = mController.vecRandomNames;
			setRoleName();
		}
		break;
	case nMAIN_CMD_PLAYER_INFO_CDKEY:
		{
			//展示兑换的奖励
			showExChangeReward();
		}
		break;
	case UPDATE_POWER_TIME:
		{
			int st = MainModel::getInstance()->powerTime;
			updateResTime(st, true);
		}
		break;
	case UPDATE_ENERGY_TIME:
		{
			int st = MainModel::getInstance()->energyTime;
			updateResTime(st, false);
		}
		break;
	case POWER_COUNT_ADD:
		{
			int curPower = MainModel::getInstance()->getMainParam()->mPower;
			int maxPower = MainModel::getInstance()->getMainParam()->mMaxPower;
			string powerStr = _TO_STR(curPower) + string("/") + _TO_STR(maxPower);
			m_ui.txtPower->setString(powerStr);
			setTextAddStroke(m_ui.txtPower, Color3B::BLACK, 2);
		}
		break;
	case ENERGY_COUNT_ADD:
		{
			int curEnergy = MainModel::getInstance()->getMainParam()->mEnergy;
			int maxEnergy = MainModel::getInstance()->getMainParam()->mMaxEnergy;
			string energyStr = _TO_STR(curEnergy) + string("/") + _TO_STR(maxEnergy);
			m_ui.txtEnergy->setString(energyStr);
			setTextAddStroke(m_ui.txtEnergy, Color3B::BLACK, 2);
		}
		break;
	default:
		break;
	}
}

PlayerInfoLayer* PlayerInfoLayer::createSysSetLayer()
{
	PlayerInfoLayer* sysSetLayer = new PlayerInfoLayer();
	sysSetLayer->m_bIsEnterSysSet = true;
	if(sysSetLayer->init())
	{
		sysSetLayer->autorelease();
	}	

	return sysSetLayer;
}
