#include "LayerRoleSelect.h"

#include "CocoStudio.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace cocos2d::ui;

LayerRoleSelect::LayerRoleSelect()
{

}

LayerRoleSelect::~LayerRoleSelect()
{

}

bool LayerRoleSelect::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void LayerRoleSelect::onEnter()
{
	Layer::onEnter();
}

void LayerRoleSelect::onExit()
{
	Layer::onExit();
}

void LayerRoleSelect::initUI()
{
	// UI处理
	// CocoStudio UI
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIRoleSel/UIRoleSel.ExportJson"));

	// 将UILayer层加入到当前的场景
	this->addChild(m_ui.pRoot);

	m_ui.pPgvRole = dynamic_cast<cocos2d::ui::PageView*>(m_ui.pRoot->getChildByName(STRING(UIRoleSel_Pgv_Role)));
	m_ui.pPgvRole->addEventListener(CC_CALLBACK_2(LayerRoleSelect::onPageRole, this));
	m_ui.pPgvRole->Layout::setClippingEnabled(false);

	m_ui.pChkMale = dynamic_cast<cocos2d::ui::CheckBox*>(m_ui.pRoot->getChildByName(STRING(UIRoleSel_Chk_Male)));
	m_ui.pChkMale->setTag(ROLE_MALE);
	m_ui.pChkMale->addEventListener(CC_CALLBACK_2(LayerRoleSelect::onSelectRole, this));

	m_ui.pChkFemale = dynamic_cast<cocos2d::ui::CheckBox*>(m_ui.pRoot->getChildByName(STRING(UIRoleSel_Chk_Female)));
	m_ui.pChkFemale->setTag(ROLE_FEMALE);
	m_ui.pChkFemale->addEventListener(CC_CALLBACK_2(LayerRoleSelect::onSelectRole, this));

	m_ui.pBtnRoleMale = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(UIRoleSel_Btn_Role_Male)));
	m_ui.pBtnRoleMale->setTag(ROLE_MALE);
	m_ui.pBtnRoleMale->addTouchEventListener(CC_CALLBACK_2(LayerRoleSelect::onBtnRole, this));

	m_ui.pBtnRoleFemale = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(UIRoleSel_Btn_Role_Female)));
	m_ui.pBtnRoleFemale->setTag(ROLE_FEMALE);
	m_ui.pBtnRoleFemale->addTouchEventListener(CC_CALLBACK_2(LayerRoleSelect::onBtnRole, this));

	selectRole(ROLE_MALE);
}

void LayerRoleSelect::onSelectRole( Ref* pSender, CheckBox::EventType type )
{
	switch (type)
	{
	case CheckBox::EventType::SELECTED:
		{
			CCLOG("onSelectRoleMale");
			int nTag = dynamic_cast<Node*>(pSender)->getTag();
			selectRole(nTag);
			m_ui.pPgvRole->scrollToPage(nTag);
		}
		break;
	case CheckBox::EventType::UNSELECTED:
		dynamic_cast<cocos2d::ui::CheckBox*>(pSender)->setSelectedState(true);
		break;
	}
}

void LayerRoleSelect::selectRole( int nType )
{
	switch (nType)
	{
	case ROLE_MALE:
		selectRoleMale();
		break;

	case ROLE_FEMALE:
		selectRoleFemale();
		break;

	default:
		CCLOG("error type in selectRole: %d", nType);
		return;
	}
}

void LayerRoleSelect::selectRoleMale()
{
	m_ui.pChkMale->setSelectedState(true);
	m_ui.pBtnRoleMale->setBright(true);
	m_ui.pBtnRoleMale->setScale(1.0f);

	m_ui.pChkFemale->setSelectedState(false);
	m_ui.pBtnRoleFemale->setBright(false);
	m_ui.pBtnRoleFemale->setScale(0.9f);
}

void LayerRoleSelect::selectRoleFemale()
{
	m_ui.pChkMale->setSelectedState(false);
	m_ui.pBtnRoleMale->setBright(false);
	m_ui.pBtnRoleMale->setScale(0.9f);

	m_ui.pChkFemale->setSelectedState(true);
	m_ui.pBtnRoleFemale->setBright(true);
	m_ui.pBtnRoleFemale->setScale(1.0f);
}

void LayerRoleSelect::onPageRole( Ref* pSender, PageView::EventType type )
{
	if (PageView::EventType::TURNING == type)
	{
		selectRole(m_ui.pPgvRole->getCurPageIndex());
	}
}

void LayerRoleSelect::onBtnRole( Ref* pSender, Widget::TouchEventType type )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		int nTag = dynamic_cast<Node*>(pSender)->getTag();
		selectRole(nTag);
		m_ui.pPgvRole->scrollToPage(nTag);
	}
}
