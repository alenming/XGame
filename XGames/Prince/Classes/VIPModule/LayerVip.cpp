#include "LayerVip.h"
#include "Temp/CustomPop.h"

LayerVip::LayerVip(void)
{
	m_nVipLv = 4;
	m_nVipLvShow = 4;
}


LayerVip::~LayerVip(void)
{
}

bool LayerVip::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void LayerVip::initUI()
{
	// 加载VIP UI;
	string strFileName;
	strFileName.assign("Image/UIVip/UIVip.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.pRoot);

	// VipLv;
	m_ui.pnlVip = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_Vip"));
	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Close)));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerVip::onBtnClick, this, Btn_Close));
	m_ui.imgVipLv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Vip_Lv"));
	m_ui.txtTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Tips)));
	m_ui.txtTips->setFontName(FONT_FZZHENGHEI);

	// PnlPrivilege;
	m_ui.pnlPrivilege = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Privilege"));
	m_ui.btnDeposit = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Deposit)));
	m_ui.btnDeposit->addTouchEventListener(CC_CALLBACK_2(LayerVip::onBtnClick, this, Btn_Deposit));
	m_ui.btnLeft = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Left)));
	m_ui.btnLeft->addTouchEventListener(CC_CALLBACK_2(LayerVip::onBtnClick, this, Btn_Left));
	m_ui.btnRight = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Right)));
	m_ui.btnRight->addTouchEventListener(CC_CALLBACK_2(LayerVip::onBtnClick, this, Btn_Right));
	m_ui.imgVipLvShow = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Vip_Lv_Show"));
	m_ui.scrollViewLvShow = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(ScrollView_LvShow)));
	ostringstream oss;
	for (unsigned int i = 0; i < TOTAL_VIP_LV; ++i)
	{
		oss << "Lab_Lv_Show_" << i+1;
		m_ui.txtLvShow[i] = dynamic_cast<Text*>(m_ui.scrollViewLvShow->getChildByName(oss.str()));
		if (nullptr != m_ui.txtLvShow[i])
		{
			m_ui.txtLvShow[i]->setFontName(FONT_FZZHENGHEI);
		}
		oss.str("");
	}

	// PnlDeposit
	m_ui.pnlDeposit = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Deposit"));
	m_ui.btnPrivilege = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Privilege)));
	m_ui.btnPrivilege->addTouchEventListener(CC_CALLBACK_2(LayerVip::onBtnClick, this, Btn_Privilege));
	m_ui.btnGold1 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Gold1)));
	m_ui.btnGold1->addTouchEventListener(CC_CALLBACK_2(LayerVip::onBtnClick, this, Btn_Gold1));
	m_ui.btnGold2 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Gold2)));
	m_ui.btnGold2->addTouchEventListener(CC_CALLBACK_2(LayerVip::onBtnClick, this, Btn_Gold2));
	m_ui.btnGold3 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Gold3)));
	m_ui.btnGold3->addTouchEventListener(CC_CALLBACK_2(LayerVip::onBtnClick, this, Btn_Gold3));

}

void LayerVip::update( float delta )
{
	this->unscheduleUpdate();
	this->removeFromParent();
}

void LayerVip::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		switch (nWidgetName)
		{
		case Btn_Close:
			{
				this->scheduleUpdate();
			}
			break;

		case Btn_Deposit:
		case Btn_Privilege:
			{
				m_ui.pnlDeposit->setVisible(nWidgetName == Btn_Deposit);
				m_ui.pnlPrivilege->setVisible(nWidgetName == Btn_Privilege);
			}
			break;

		case Btn_Left:
		case Btn_Right:
			{
				m_nVipLvShow += ((nWidgetName == Btn_Left) ? -1 : 1);
				if (m_nVipLvShow <= 1)
				{
					m_nVipLvShow = 1;
				}
				else if (m_nVipLvShow >= TOTAL_VIP_LV)
				{
					m_nVipLvShow = TOTAL_VIP_LV;
				}
				m_ui.btnLeft->setVisible(!(m_nVipLvShow <= 1));
				m_ui.btnRight->setVisible(!(m_nVipLvShow >= 8));

				// 更新展示;
				updateLvShow();
			}
			break;

		case Btn_Gold1:
		case Btn_Gold2:
		case Btn_Gold3:
			{
				CustomPop::show("暂未开放");
			}
			break;

		default:
			break;
		}
	}
}

void LayerVip::updateLvShow()
{
	if (m_nVipLvShow < 1 || m_nVipLvShow >TOTAL_VIP_LV)
	{
		m_nVipLvShow = m_nVipLv;
	}

	// 更换标题;
	ostringstream oss;
	oss << "Image/UIVip/VipLv/img_viptequan" << m_nVipLvShow << ".png";
	m_ui.imgVipLvShow->loadTexture(oss.str());
}
