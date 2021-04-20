#include "UIFighterHp.h"
#include "Fighter.h"
#include "Utils/ResourceUtils.h"

using namespace cocos2d;
using namespace cocos2d::ui;

const int nVER_SPACE_HP_2_FIGHTER = 190;

UIFighterHp::UIFighterHp()
{

}

UIFighterHp::~UIFighterHp()
{

}

UIFighterHp* UIFighterHp::create(FightStateMachine::E_SIDE side, int heroType)
{
	UIFighterHp* fighterHp = new UIFighterHp();
	if (fighterHp && fighterHp->init(side, heroType))
	{
		fighterHp->autorelease();
		return fighterHp;
	}
	else
	{
		delete fighterHp;
		return nullptr;
	}
}

bool UIFighterHp::init(FightStateMachine::E_SIDE side, int heroType)
{
	if (!Node::init())
	{
		return false;
	}

	m_pBg = Sprite::create("Image/Fight/img_xueliang_1.png");
	m_pBg->setAnchorPoint(Point::ANCHOR_MIDDLE);
	m_pBg->setPosition(Point::ZERO);
	this->addChild(m_pBg);

	m_pBar = LoadingBar::create("Image/Fight/img_xueliang_2.png");
	m_pBar->setAnchorPoint(Point::ANCHOR_MIDDLE);
	m_pBg->setPosition(Point::ZERO);
	this->addChild(m_pBar);

	Sprite* heroTypeImg = nullptr;

	switch (heroType)
	{
	case Fighter::ATTACK_PROPERTY_POW:
		heroTypeImg = Sprite::create(ResourceUtils::getIconGlobalPath("img_shuxing11_li.png"));
		break;
	case Fighter::ATTACK_PROPERTY_AGI:
		heroTypeImg = Sprite::create(ResourceUtils::getIconGlobalPath("img_shuxing22_min.png"));
		break;
	case Fighter::ATTACK_PROPERTY_INT:
		heroTypeImg = Sprite::create(ResourceUtils::getIconGlobalPath("img_shuxing33_zhi.png"));
		break;
	default:
		break;
	}

	if (heroTypeImg)
	{
		heroTypeImg->setAnchorPoint(Point::ANCHOR_MIDDLE_RIGHT);
		heroTypeImg->setPosition(Point(10, m_pBar->getContentSize().height / 2));
		heroTypeImg->setScale(0.6f);
		m_pBar->addChild(heroTypeImg);
	}

	setPercent(100);

	Point pos(0, nVER_SPACE_HP_2_FIGHTER);
	this->setPosition(pos);

	return true;
}

void UIFighterHp::onEnter()
{
	Node::onEnter();
}

void UIFighterHp::onExit()
{
	Node::onExit();
}

void UIFighterHp::setPercent( float fPercent )
{
	CCASSERT(m_pBar!=nullptr, "m_pBar null");
	m_pBar->setPercent(fPercent);
}

float UIFighterHp::getPercent()
{
	CCASSERT(m_pBar!=nullptr, "m_pBar null");
	return m_pBar->getPercent();
}
