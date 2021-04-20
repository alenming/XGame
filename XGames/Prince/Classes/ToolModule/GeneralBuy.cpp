#include "GeneralBuy.h"
#include "Temp/CustomGoldTips.h"
#include "DataManager/DataManager.h"
GeneralBuy::GeneralBuy(void)
{
	m_nLimitShopBuyNum = 0;
	m_pirce = 0;
	m_nShopBuyNum = 0;
	m_nNeedMoney = 0; 
}


GeneralBuy::~GeneralBuy(void)
{
}

GeneralBuy* GeneralBuy::create(int LimitShopBuyNum,int price,GeneralBuyCallback calBack)
{
	//背景设置
	GeneralBuy* buy = new GeneralBuy;
	if (buy && buy->init())
	{
		buy->autorelease();
		buy->initUi(LimitShopBuyNum,price,calBack);

		return buy;
	}


	CC_SAFE_DELETE(buy);
	return nullptr;
}

bool GeneralBuy::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}
	return true;
}

void GeneralBuy::onEnter()
{
	ModalLayer::onEnter();
}

void GeneralBuy::onExit()
{
	ModalLayer::onExit();
}

void GeneralBuy::initUi(int LimitShopBuyNum,int price,GeneralBuyCallback calBack)
{
	m_nLimitShopBuyNum = LimitShopBuyNum;
	m_pirce = price;
	_callBack = calBack;

	Size size = Director::getInstance()->getWinSize();
	layerToolsShop.pLayerPopup = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UImall/UImall_buy.ExportJson"));
	layerToolsShop.pLayerPopup->setPosition(cocos2d::Vec2((size.width-layerToolsShop.pLayerPopup->getContentSize().width)/2, (size.height - layerToolsShop.pLayerPopup->getContentSize().height)/2));

	// 将UILayer层加入到当前的场景
	this->addChild(layerToolsShop.pLayerPopup);
	//layerToolsShop.pLayerPopup		= dynamic_cast<Layout*>(pRoot->getChildByName("Panel_check"));
	cocos2d::ui::ImageView* pImageBg	= dynamic_cast<ImageView*>(layerToolsShop.pLayerPopup->getChildByName("img_goumaidaojudi"));
	ActionCreator::runCommDlgAction(pImageBg);

	layerToolsShop.pBtnAdd1		= dynamic_cast<Button*>(pImageBg->getChildByName("btn_jia1"));
	layerToolsShop.pBtnAdd10		= dynamic_cast<Button*>(pImageBg->getChildByName("btn_jia10"));
	layerToolsShop.pBtnDec1		= dynamic_cast<Button*>(pImageBg->getChildByName("btn_jian1"));
	layerToolsShop.pBtnDec10		= dynamic_cast<Button*>(pImageBg->getChildByName("btn_jian10"));
	layerToolsShop.pBtnConfirm		= dynamic_cast<Button*>(pImageBg->getChildByName("btn_queren"));
	layerToolsShop.pBtnCancel		= dynamic_cast<Button*>(pImageBg->getChildByName("btn_quxiao"));

	layerToolsShop.pTextBuyNum		= dynamic_cast<Text*>(pImageBg->getChildByName("lab_tishiwenzi"));
	layerToolsShop.pTextBuyNumTip	= dynamic_cast<Text*>(pImageBg->getChildByName("lab_tishiwenzi2"));

	cocos2d::ui::ImageView* pImageNumBg	= dynamic_cast<ImageView*>(pImageBg->getChildByName("img_shuliangkuang"));
	layerToolsShop.pTextAtlasNum	= dynamic_cast<Text*>(pImageNumBg->getChildByName("lab_shuliang"));
	layerToolsShop.pTextPriceNum	= dynamic_cast<Text*>(pImageBg->getChildByName("lab_jinbi"));
	Text* pTextPrice	= dynamic_cast<Text*>(pImageBg->getChildByName("lab_xiaohao"));
	pTextPrice->setString(DataManager::getInstance()->searchCommonTexdtById(20074));

	layerToolsShop.pTextBuyNum->setFontName(FONT_FZZHENGHEI);
	layerToolsShop.pTextBuyNumTip->setFontName(FONT_FZZHENGHEI);
	layerToolsShop.pTextAtlasNum->setFontName(FONT_FZZHENGHEI);
	layerToolsShop.pTextPriceNum->setFontName(FONT_FZZHENGHEI);
	pTextPrice->setFontName(FONT_FZZHENGHEI);

	layerToolsShop.pBtnAdd1->addTouchEventListener(CC_CALLBACK_2(GeneralBuy::onBtnShopAdd1, this));
	layerToolsShop.pBtnAdd10->addTouchEventListener(CC_CALLBACK_2(GeneralBuy::onBtnShopAdd10, this));	
	layerToolsShop.pBtnDec1->addTouchEventListener(CC_CALLBACK_2(GeneralBuy::onBtnShopDec1, this));
	layerToolsShop.pBtnDec10->addTouchEventListener(CC_CALLBACK_2(GeneralBuy::onBtnShopDec10, this));	
	layerToolsShop.pBtnConfirm->addTouchEventListener(CC_CALLBACK_2(GeneralBuy::onBtnShopConfirm, this));
	layerToolsShop.pBtnCancel->addTouchEventListener(CC_CALLBACK_2(GeneralBuy::onBtnShopCancel, this));

	if (m_nLimitShopBuyNum == -1)
	{
		layerToolsShop.pTextBuyNum->setString(DataManager::getInstance()->searchCommonTexdtById(20075));
	}
	else
	{
		layerToolsShop.pTextBuyNum->setString(DataManager::getInstance()->searchCommonTexdtById(20076) +
			_TO_STR(m_nLimitShopBuyNum) + DataManager::getInstance()->searchCommonTexdtById(20077));
	}

	m_nShopBuyNum = 1;
	showBuyNum(m_nShopBuyNum);
}

void GeneralBuy::onBtnShopAdd1(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_nLimitShopBuyNum == 0)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20078));
			return;
		}
		if (m_nLimitShopBuyNum == -1)
		{
			m_nShopBuyNum ++;
			showBuyNum(m_nShopBuyNum);
		}
		else if (m_nShopBuyNum < 100 
			&& m_nShopBuyNum < m_nLimitShopBuyNum)//中间显示当前选中数量，默认为1，最多100
		{
			m_nShopBuyNum ++;
			showBuyNum(m_nShopBuyNum);
		}
		else if (m_nShopBuyNum == m_nLimitShopBuyNum)	
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20078));
		}
	}
}

void GeneralBuy::onBtnShopAdd10(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_nLimitShopBuyNum == 0)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20078));
			return;
		}
		if (m_nLimitShopBuyNum == -1)
		{
			m_nShopBuyNum += 10;
			showBuyNum(m_nShopBuyNum);
		}
		else
		{
			if (m_nShopBuyNum <= 90
				&& m_nShopBuyNum+10 < m_nLimitShopBuyNum)//中间显示当前选中数量，默认为1，最多100
			{
				m_nShopBuyNum += 10;
				showBuyNum(m_nShopBuyNum);
			}
			else if (m_nShopBuyNum == m_nLimitShopBuyNum)	
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20078));
			}
			else
			{
				//商城中无法使用+10时，浮动提示：今日可购买次数不足10次，使用-10提示：已选择数量不足10个
				m_nShopBuyNum = m_nLimitShopBuyNum;
				showBuyNum(m_nShopBuyNum);
			}
		}
	}
}

void GeneralBuy::onBtnShopDec1(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_nShopBuyNum > 1)//中间显示当前选中数量，默认为1，最多100
		{
			m_nShopBuyNum --;
			showBuyNum(m_nShopBuyNum);
		}
		else if (m_nShopBuyNum == 1)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20079));
		}	
	}	
}

void GeneralBuy::onBtnShopDec10(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_nShopBuyNum > 10)//中间显示当前选中数量，默认为1，最多100
		{
			m_nShopBuyNum -= 10;
			showBuyNum(m_nShopBuyNum);
		}
		else if (m_nShopBuyNum == 1)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20079));
		}	
		else
		{
			m_nShopBuyNum = 1;
			showBuyNum(m_nShopBuyNum);
		}
	}
}

void GeneralBuy::onBtnShopConfirm(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_nLimitShopBuyNum <= 0)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20080));
			dispose();
			return;
		}

		MainParam* mainPram = MainModel::getInstance()->getMainParam();
		if (mainPram->mGold < m_nNeedMoney)
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
			}
			dispose();
		}
		else
		{
			if (_callBack)
			{
				_callBack(m_nShopBuyNum);
			}

			dispose();
		}
	}
}

void GeneralBuy::onBtnShopCancel(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		dispose();
	}
}

void GeneralBuy::showBuyNum(const int& num)
{
	layerToolsShop.pTextAtlasNum->setString(TO_STR(num));
	m_nNeedMoney = m_pirce*num;
	layerToolsShop.pTextPriceNum->setString(TO_STR(m_nNeedMoney));
}
