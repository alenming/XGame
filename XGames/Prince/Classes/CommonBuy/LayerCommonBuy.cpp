#include "LayerCommonBuy.h"
#include "CommonBuyModel.h"
#include "VIPModule/VipModel.h"
#include "VIPModule/VipScene.h"
#include "Widget/ActionCreator.h"
#include "MainModule/MainModel.h"
#include "Temp/CustomGoldTips.h"
#include "DataManager/DataManager.h"
#include "DoubleBonus/DoubleBonusModel.h"

LayerCommonBuy::LayerCommonBuy(void)
	: m_type(COMMON_BUY_TYPE_GOLD_POWER)
	, m_bFirstEnter(true)
{
	m_pController = new CommonBuyController;
	CommonBuyModel::getInstance()->addObserver(this);
	DoubleBonusModel::getInstance()->addObserver(this);
}


LayerCommonBuy::~LayerCommonBuy(void)
{
	CommonBuyModel::getInstance()->removeObserver(this);
	DoubleBonusModel::getInstance()->removeObserver(this);
	CC_SAFE_DELETE(m_pController);
}

LayerCommonBuy* LayerCommonBuy::create( COMMON_BUY_TYPE type )
{
	LayerCommonBuy* _layer = new LayerCommonBuy;
	if (nullptr != _layer && _layer->init(type))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

bool LayerCommonBuy::init( COMMON_BUY_TYPE type )
{
	if (!Layer::init())
	{
		return false;
	}

	m_type = type;
	initUI();
	m_pController->handleInitData(m_type);
	return true;
}

void LayerCommonBuy::onEnter()
{
	Layer::onEnter();

	if (m_bFirstEnter)
	{
		if (m_data.nNum <= 0)
		{
			m_ui.pBuyRoot->setVisible(false);
			checkBuyCondition();
		}
		ActionCreator::runCommDlgAction(m_ui.imgBuyCoinBg);
		SoundUtils::playSoundEffect("tanchuang");
		m_bFirstEnter = false;
	}
}

void LayerCommonBuy::initUI()
{
	//////////////////////// 购买;
	m_ui.pBuyRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UiNewbuy/UiBuyCoin.ExportJson"));
	this->addChild(m_ui.pBuyRoot, 1);

	m_ui.imgBuyCoinBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pBuyRoot, "Img_di"));

	// 关闭按钮;
	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pBuyRoot, STRING(Btn_Close)));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_Close));

	// 标题适配(默认祈福);
	m_ui.imgTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pBuyRoot, "Img_Title"));
	if (m_type == COMMON_BUY_TYPE_GOLD_ENERGY)
	{
		m_ui.imgTitle->loadTexture("Image/UiNewbuy/Title/img_goumaijingli.png");
	}
	else if (m_type == COMMON_BUY_TYPE_GOLD_POWER)
	{
		m_ui.imgTitle->loadTexture("Image/UiNewbuy/Title/img_goumaitili.png");
	}
	else if (m_type == COMMON_BUY_TYPE_GOLD_MAZEPOWER)
	{
		m_ui.imgTitle->loadTexture("Image/UiNewbuy/Title/img_goumaixingdongli.png");
	}

	// VIP等级;
	m_ui.imgVip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pBuyRoot, "Img_Vip"));
	m_ui.imgVip->loadTexture(getVipIcon(VipModel::getInstance()->getCurVipLv()).c_str());
	
	// 购买次数;
	m_ui.txtCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBuyRoot, "Lab_Count1"));
	m_ui.txtCount->setFontName(FONT_FZZHENGHEI);
	m_ui.txtCount->setString("(0/0)");

	// 提示文本;
	m_ui.txtDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBuyRoot, "Lab_Tips1"));
	m_ui.txtDesc->setFontName(FONT_FZZHENGHEI);
	m_ui.txtBuyCoin = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBuyRoot, "Lab_Tips2"));
	m_ui.txtBuyCoin->setFontName(FONT_FZZHENGHEI);
	m_ui.txtBuyCoin->setVisible(m_type == COMMON_BUY_TYPE_GOLD_COIN);
	m_ui.txtBuyOther = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBuyRoot, "Lab_Tips2_0"));
	m_ui.txtBuyOther->setFontName(FONT_FZZHENGHEI);
	m_ui.txtBuyOther->setVisible(m_type != COMMON_BUY_TYPE_GOLD_COIN);

	// 按钮;
	m_ui.btnOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pBuyRoot, STRING(Btn_OK)));
	m_ui.btnOK->setVisible(m_type != COMMON_BUY_TYPE_GOLD_COIN);
	m_ui.btnOK->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_OK));
	m_ui.btnCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pBuyRoot, STRING(Btn_Cancel)));
	m_ui.btnCancel->setVisible(m_ui.btnOK->isVisible());
	m_ui.btnCancel->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_Cancel));
	m_ui.btnBuy = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pBuyRoot, STRING(Btn_Buy)));
	m_ui.btnBuy->setVisible(m_type == COMMON_BUY_TYPE_GOLD_COIN);
	m_ui.btnBuy->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_Buy));
	m_ui.btnMultiBuy = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pBuyRoot, STRING(Btn_Multi_Buy)));
	m_ui.btnMultiBuy->setVisible(m_ui.btnBuy->isVisible());
	m_ui.btnMultiBuy->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_Multi_Buy));

	// 货币;
	m_ui.txtGold = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBuyRoot, "Lab_Count2"));
	m_ui.txtGold->setFontName(FONT_FZZHENGHEI);
	m_ui.txtGold->setString("0");
	m_ui.txtDst = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBuyRoot, "Lab_Count3"));
	m_ui.txtDst->setFontName(FONT_FZZHENGHEI);
	m_ui.txtDst->setString("0");

	// 货币图标(默认祈福);
	m_ui.imgDst = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pBuyRoot, "Img_Coin"));
	if (m_type == COMMON_BUY_TYPE_GOLD_ENERGY)
	{
		m_ui.imgDst->loadTexture("Image/Icon/global/img_jingli.png");
	}
	else if (m_type == COMMON_BUY_TYPE_GOLD_POWER)
	{
		m_ui.imgDst->loadTexture("Image/Icon/global/img_tili.png");
	}
	else if (m_type == COMMON_BUY_TYPE_GOLD_MAZEPOWER)
	{
		m_ui.imgDst->loadTexture("Image/Icon/global/img_xingdongli.png");
		m_ui.imgDst->setScale(0.9f);
	}

	//////////////////////// 多次祈福;
	if (m_type == COMMON_BUY_TYPE_GOLD_COIN)
	{
		m_ui.pMultiBuyRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UiNewbuy/UiBuyCoins.ExportJson"));
		m_ui.imgBuyCoinsBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pMultiBuyRoot, "Img_di"));
		m_ui.txtMultiCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pMultiBuyRoot, "Lab_Multi_Count"));
		m_ui.txtMultiCount->setFontName(FONT_FZZHENGHEI);
		m_ui.txtMultiTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pMultiBuyRoot, "Lab_Tips1"));
		m_ui.txtMultiTips1->setFontName(FONT_FZZHENGHEI);
		m_ui.txtMultiGold = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pMultiBuyRoot, "Lab_Multi_Gold"));
		m_ui.txtMultiGold->setFontName(FONT_FZZHENGHEI);
		m_ui.txtMultiTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pMultiBuyRoot, "Lab_Tips2"));
		m_ui.txtMultiTips2->setFontName(FONT_FZZHENGHEI);
		m_ui.txtMultiCoin = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pMultiBuyRoot, "Lab_Multi_Coin"));
		m_ui.txtMultiCoin->setFontName(FONT_FZZHENGHEI);
		m_ui.btnMultiOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pMultiBuyRoot, STRING(Btn_Multi_OK)));
		m_ui.btnMultiOK->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_Multi_OK));
		m_ui.btnMultiCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pMultiBuyRoot, STRING(Btn_Multi_Cancel)));
		m_ui.btnMultiCancel->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_Multi_Cancel));
		m_ui.btnMultiClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pMultiBuyRoot, STRING(Btn_Multi_Close)));
		m_ui.btnMultiClose->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_Multi_Close));
		this->addChild(m_ui.pMultiBuyRoot, 2);
		m_ui.pMultiBuyRoot->setVisible(false);
	}

	//////////////////////// 提升VIP;
	m_ui.pVipRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UiNewbuy/UiVipUp.ExportJson"));
	this->addChild(m_ui.pVipRoot, 3);
	m_ui.pVipRoot->setVisible(false);

	m_ui.imgVipBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Img_di"));
	m_ui.txtVipTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Tips1"));
	m_ui.txtVipTips1->setFontName(FONT_FZZHENGHEI);

	// 更改显示名称;
	string str = "";
	switch (m_type)
	{
	case COMMON_BUY_TYPE_GOLD_POWER:
		//str.assign("今日购买体力次数已用完");
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10079));
		break;
	case COMMON_BUY_TYPE_GOLD_ENERGY:
		//str.assign("今日购买精力次数已用完");
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10080));
		break;
	case COMMON_BUY_TYPE_GOLD_MAZEPOWER:
		//str.assign("今日购买行动力次数已用完");
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10081));
		break;
	case COMMON_BUY_TYPE_GOLD_COIN:
		//str.assign("今日祈福次数已用完");
		str.assign(DataManager::getInstance()->searchCommonTexdtById(10082));
		break;
	default:
		break;
	}
	m_ui.txtVipTips1->setString(str.c_str());

	m_ui.txtVipTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Tips2"));
	m_ui.txtVipTips2->setFontName(FONT_FZZHENGHEI);
	m_ui.imgVipNext = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Img_Vip"));
	m_ui.txtNextCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Count"));
	m_ui.txtNextCount->setFontName(FONT_FZZHENGHEI);
	m_ui.btnVipOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_OK)));
	m_ui.btnVipOK->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_Vip_OK));
	m_ui.btnVipCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_Cancel)));
	m_ui.btnVipCancel->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_Vip_Cancel));
	m_ui.btnVipClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_Close)));
	m_ui.btnVipClose->addTouchEventListener(CC_CALLBACK_2(LayerCommonBuy::onBtnClicked, this, Btn_Vip_Close));

	// 检查双倍奖励当前状态;
	showDoubleBonusTips(DoubleBonusModel::getInstance()->isDoubleBonusValid(CommonBuyModel::getInstance()->checkTypeByBuyType(m_type))
		&& CommonBuyModel::getInstance()->isDoubleValidByBuyTimes());
}

void LayerCommonBuy::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (nWidgetName)
		{
		case Btn_Cancel:
		case Btn_Close:
			this->scheduleUpdate();
			break;

		case Btn_OK:
			{
				if (checkBuyCondition())
				{
					MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
					if (pMainParam->mGold < m_buySpend*m_buyCount)
					{
						CustomGoldTips *_tips = CustomGoldTips::create();
						if (_tips)
						{
							Director::getInstance()->getRunningScene()->addChild(_tips);
						}
						this->scheduleUpdate();
						return;
					}
					m_pController->handleBuy();
				}
			}
			break;

		case Btn_Buy:
			{
				if (checkBuyCondition())
				{
					MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
					if (pMainParam->mGold < m_buySpend*m_buyCount)
					{
						CustomGoldTips *_tips = CustomGoldTips::create();
						if (_tips)
						{
							Director::getInstance()->getRunningScene()->addChild(_tips);
						}
						this->scheduleUpdate();
						return;
					}

					m_pController->handleMultiBuy(false);
				}
			}
			break;

		case Btn_Multi_Buy:
			{
				if (checkBuyCondition())
				{
					m_ui.pMultiBuyRoot->setVisible(true);
					ActionCreator::runCommDlgAction(m_ui.imgBuyCoinsBg);
				}
			}
			break;

		case Btn_Multi_OK:
			{
				if (checkBuyCondition())
				{
					MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
					if (pMainParam->mGold < m_MutiBuyCoinCostGoldCount)
					{
						CustomGoldTips *_tips = CustomGoldTips::create();
						if (_tips)
						{
							Director::getInstance()->getRunningScene()->addChild(_tips);
						}
						this->scheduleUpdate();
						return;
					}

					m_pController->handleMultiBuy(true);
					m_ui.pMultiBuyRoot->setVisible(false);
				}
			}
			break;

		case Btn_Multi_Cancel:
		case Btn_Multi_Close:
			{
				m_ui.pMultiBuyRoot->setVisible(false);
			}
			break;

		case Btn_Vip_OK:
			{
				// Vip;
				Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				this->scheduleUpdate();
			}
			break;

		case Btn_Vip_Cancel:
		case Btn_Vip_Close:
			{
				m_ui.pVipRoot->setVisible(false);
				if (!m_ui.pBuyRoot->isVisible())
				{
					this->scheduleUpdate();
				}
			}
			break;

		default:
			break;
		}
	}
}

void LayerCommonBuy::update( float delta )
{
	this->unscheduleUpdate();
	this->removeFromParent();
}

void LayerCommonBuy::updateUI( const CommonBuyData data )
{
	if (data.type != m_type)
	{
		return;
	}

	m_data = data;

	ostringstream oss;
	oss << "(" << data.nNum << "/" << data.nTotalNum << ")";
	m_ui.txtCount->setString(oss.str());
	oss.str("");

	m_ui.txtGold->setString(_TO_STR(data.nCostGold));
	m_ui.txtDst->setString(_TO_STR(data.nAddValue));

	m_buyCount = 1;
	m_buySpend = data.nCostGold;

	if (m_type == COMMON_BUY_TYPE_GOLD_COIN)
	{
		oss << DataManager::getInstance()->searchCommonTexdtById(10083) << data.nTimes 
			<< DataManager::getInstance()->searchCommonTexdtById(10049);
		m_ui.txtMultiCount->setString(oss.str());
		oss.str("");
		m_buyMutiCount = data.nTimes;
		m_MutiBuyCoinCostGoldCount = geteMultiBuySpendGold(data);
		m_ui.txtMultiGold->setString(_TO_STR(m_MutiBuyCoinCostGoldCount));
		m_ui.txtMultiCoin->setString(_TO_STR(data.nAddValue * data.nTimes));
	}
}

int LayerCommonBuy::geteMultiBuySpendGold(CommonBuyData const data)
{
	RowData* systemData = DataManager::getInstance()->searchToolSystemById(10064);
	vector<int> vecDst;
	StringFormat::parseCsvStringVec(systemData->getStringData("value"), vecDst);

	int spendGold = 0;
	for (int i = 0; i < data.nTimes; i++)
	{
		spendGold += vecDst[i + data.nTotalNum - data.nNum];
	}

	return spendGold;
}

std::string LayerCommonBuy::getVipIcon(int nVipLv)
{
	string strPath = "Image/UIVip/VipLv/img_vip_";
	strPath.append(_TO_STR(nVipLv));
	strPath.append(".png");
	return strPath;
}

bool LayerCommonBuy::checkBuyCondition()
{
	// 1.当前剩余次数;
	if (m_data.nNum > 0)
	{
		return true;
	}

	// 2.当前可提升次数;
	int nNextValue = 0;
	int nNextLv = CommonBuyModel::getInstance()->getNextValidVip(nNextValue);
	if (nNextValue > CommonBuyModel::getInstance()->getCurMaxTimes())
	{
		// 弹出特权提升面板;
		ostringstream oss;
		oss << DataManager::getInstance()->searchCommonTexdtById(10084) << nNextValue 
			<< DataManager::getInstance()->searchCommonTexdtById(10049);
		m_ui.txtNextCount->setString(oss.str());
		oss.str("");
		m_ui.imgVipNext->loadTexture(getVipIcon(nNextLv));
		m_ui.pVipRoot->setVisible(true);
		SoundUtils::playSoundEffect("tanchuang");
		ActionCreator::runCommDlgAction(m_ui.imgVipBg);
		return false;
	}

	// 不可再购买;
	//CustomTips::show("今日购买次数不足");
	CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10050));

	if (!m_ui.pBuyRoot->isVisible() && !m_ui.pVipRoot->isVisible())
	{
		this->scheduleUpdate();
	}
	return false;
}

void LayerCommonBuy::updateSelf( void* data )
{
	if (nullptr == data)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) data;

	switch (param->id)
	{
	case DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE::START:
	case DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE::END:
		{
			DOUBLE_BONUS_TYPE doublebonusType = *((DOUBLE_BONUS_TYPE*)(param->updateData));
			if (m_type == CommonBuyModel::getInstance()->checkTypeByDoubleBonusType(doublebonusType))
			{
				showDoubleBonusTips(param->id == DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE::START);
			}
		}
		break;

	default:
		{
			updateUI(*((CommonBuyData*)(param->updateData)));

			// 次数发生变化时，检查双倍奖励当前状态;
			showDoubleBonusTips(DoubleBonusModel::getInstance()->isDoubleBonusValid(CommonBuyModel::getInstance()->checkTypeByBuyType(m_type))
				&& CommonBuyModel::getInstance()->isDoubleValidByBuyTimes());
		}
		break;
	}

}

void LayerCommonBuy::showDoubleBonusTips( bool bShow )
{
	ImageView* img = dynamic_cast<ImageView*>(m_ui.imgBuyCoinBg->getChildByName("double_bonus"));
	if (bShow)
	{
		if (nullptr == img)
		{
			if (m_type == COMMON_BUY_TYPE_GOLD_COIN)
			{
				img = ImageView::create("Image/Icon/global/20005.png");
			}
			else if (m_type == COMMON_BUY_TYPE_GOLD_POWER)
			{
				img = ImageView::create("Image/Icon/global/20004.png");
			}

			if (nullptr != img)
			{
				img->setName("double_bonus");
				img->setPosition(Vec2(m_ui.imgBuyCoinBg->getContentSize().width/2.0f + 200.0f,
					m_ui.imgBuyCoinBg->getContentSize().height/2.0f + 5.0f));
				m_ui.imgBuyCoinBg->addChild(img);
			}
		}
	}
	else
	{
		if (img != nullptr)
		{
			img->removeFromParent();
			img = nullptr;
		}
	}
}

