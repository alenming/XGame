#include "PvpAddLayer.h"
#include "Utils/StringFormat.h"
#include "PvpModule/PvpModel.h"
#include "PvpModule/PvpController.h"

PvpAddLayer::PvpAddLayer(void)
{
	m_count = 1;
	m_spend = 0;
	StringFormat::parseCsvStringVec(DataManager::getInstance()->searchToolSystemById(PVP_BUY_COUNT_SPEND)->getStringData("value"),
		m_VecSpend);
}


PvpAddLayer::~PvpAddLayer(void)
{
}

PvpAddLayer* PvpAddLayer::create()
{
	//背景设置
	PvpAddLayer* pvpAddLayer = new PvpAddLayer;
	if (pvpAddLayer && pvpAddLayer->init())
	{
		pvpAddLayer->autorelease();
		pvpAddLayer->initUi();

		return pvpAddLayer;
	}


	CC_SAFE_DELETE(pvpAddLayer);
	return nullptr;
}

bool PvpAddLayer::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}
	return true;
}

void PvpAddLayer::onEnter()
{
	ModalLayer::onEnter();
}

void PvpAddLayer::onExit()
{
	ModalLayer::onExit();
}

void PvpAddLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIPvp/UIPvp_buy.ExportJson"));
	this->addChild(m_ui.pRoot);	
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);

	m_ui.Img_di = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Image_di"));
	ActionCreator::runCommDlgAction(m_ui.Img_di);

	m_ui.Label_wenben1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Label_wenben1"));
	m_ui.Label_wenben2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Label_wenben2"));
	m_ui.Label_wenben3 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Label_wenben3"));
	m_ui.Label_jinbi = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Label_jinbi"));
	m_ui.addCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"addCount"));
	m_ui.Label_wenben1->setFontName(FONT_FZZHENGHEI);
	m_ui.Label_wenben2->setFontName(FONT_FZZHENGHEI);
	m_ui.Label_wenben3->setFontName(FONT_FZZHENGHEI);
	m_ui.Label_jinbi->setFontName(FONT_FZZHENGHEI);
	m_ui.addCount->setFontName(FONT_FZZHENGHEI);

	m_ui.Button_jianyi = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Button_jianyi"));
	m_ui.Button_jiayi = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Button_jiayi"));
	m_ui.Button_quxiao = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Button_quxiao"));
	m_ui.Button_queren = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Button_queren"));
	m_ui.Button_jianyi->addTouchEventListener(CC_CALLBACK_2(PvpAddLayer::onBtnNotify,this));
	m_ui.Button_jiayi->addTouchEventListener(CC_CALLBACK_2(PvpAddLayer::onBtnNotify,this));
	m_ui.Button_quxiao->addTouchEventListener(CC_CALLBACK_2(PvpAddLayer::onBtnNotify,this));
	m_ui.Button_queren->addTouchEventListener(CC_CALLBACK_2(PvpAddLayer::onBtnNotify,this));

	updataSpend();
}

void PvpAddLayer::updataSpend()
{
	m_spend = 0;
	int buyCount = PvpModel::getInstance()->getBuyCount();
	for (int i = 0; i < m_count; i++)
	{
		if (buyCount + i < m_VecSpend.size())
		{
			m_spend += m_VecSpend.at(buyCount + i);
		}
		else
		{
			m_spend += m_VecSpend.at(m_VecSpend.size()-1);
		}
	}

	m_ui.addCount->setString(TO_STR(m_count));
	m_ui.Label_jinbi->setString(TO_STR(m_spend));
}

void PvpAddLayer::onBtnNotify(Ref* sender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (sender == m_ui.Button_jianyi)
		{
			if (m_count > 1)
			{
				m_count -= 1;
				updataSpend();
			}
			else
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20218));
			}
		}
		else if(sender == m_ui.Button_jiayi)
		{
			m_count += 1;
			updataSpend();
		}
		else if (sender == m_ui.Button_quxiao)
		{
			dispose();
		}
		else if (sender == m_ui.Button_queren)
		{
			dispose();
			MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
			if (pMainParam->mGold < m_spend)
			{
				//充值界面
				CustomGoldTips *_tips = CustomGoldTips::create();
				if (_tips)
				{
					Director::getInstance()->getRunningScene()->addChild(_tips);
				}
			}
			else
			{
				PvpController::getInstance()->buyPvpCount(m_count);
			}
		}
	}
}