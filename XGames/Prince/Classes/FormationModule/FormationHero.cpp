#include "FormationHero.h"
#include "Widget/SpineAnimation.h"


FormationHero::FormationHero(void)
	: m_imgHpBarBg(nullptr)
	, m_hpBar(nullptr)
	, m_imgPreState(nullptr)
	, m_imgNextState(nullptr)
{
}


FormationHero::~FormationHero(void)
{
}

FormationHero* FormationHero::create( const UI_HERO_LIST_PARAM  param )
{
	FormationHero* _node = new FormationHero;
	if (nullptr != _node && _node->init(param))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool FormationHero::init( const UI_HERO_LIST_PARAM param )
{
	if (!Node::init())
	{
		return false;
	}

	m_HeroData = param;
	initUI();

	return true;
}

void FormationHero::initUI()
{
	// 创建骨骼动画;
	m_pSkelHero = SpineAnimation::createFighterWithResId(m_HeroData.nResId);
	m_pSkelHero->setPosition(Vec2(40.0f, 0.0f));
	m_pSkelHero->setAnimation(0, szANI_WAIT, true);
	m_pSkelHero->setScale(0.9f);
	this->setContentSize(Size(80.0f, 200.0f));
	this->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	this->addChild(m_pSkelHero);

	// 技能;
	// 技能前置/后置;
	//if (m_HeroData.nPreState != AbstractSkill::NONE_STATE
	//	&& m_HeroData.nNextState != AbstractSkill::NONE_STATE)
	//{
		m_imgPreState = ImageView::create(getSkillStateIcon(m_HeroData.nPreState));
		m_imgPreState->setPosition(Vec2(10.0f, 5.0f));
		this->addChild(m_imgPreState);

		ImageView* imgArrow = ImageView::create("Image/UIFormation/Icon/img_jineng_jiantou_2.png");
		imgArrow->setPosition(Vec2(40.0f, 5.0f));
		this->addChild(imgArrow);
		imgArrow->setVisible(m_HeroData.nNextState != AbstractSkill::NONE_STATE);

		m_imgNextState = ImageView::create(getSkillStateIcon(m_HeroData.nNextState));
		m_imgNextState->setPosition(Vec2(70.0f, 5.0f));
		this->addChild(m_imgNextState);
	//}

	// 血条;
	if (m_HeroData.nMode == F_MODE_TRAIL)
	{
		m_imgHpBarBg = ImageView::create("Image/Fight/img_xueliang_1.png");
		m_imgHpBarBg->setPosition(Vec2(m_pSkelHero->getPosition().x, m_pSkelHero->getPosition().y + 210.0f));
		this->addChild(m_imgHpBarBg);
		m_hpBar = LoadingBar::create("Image/Fight/img_xueliang_2.png");
		m_hpBar->setPosition(Vec2(3.5f, 6.5f));
		m_hpBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		m_hpBar->setPercent(m_HeroData.fHpPercent);
		m_imgHpBarBg->addChild(m_hpBar);
		m_imgHpBarBg->setVisible(m_HeroData.fHpPercent > 0.0001f);
	}
}

void FormationHero::setColor( Color3B color )
{
	Node::setColor(color);
	m_pSkelHero->setColor(color);
}

void FormationHero::setTransparent( int nValue )
{
	int nOpacity = nValue;
	if (nValue < 0)
	{
		nOpacity = 0;
	}
	else if (nValue > 255)
	{
		nOpacity = 255;
	}

	// 设置透明度(0~255, 0=全透明，255=不透明);
	m_pSkelHero->setOpacity(nOpacity);
}

void FormationHero::setHpBarPercent( float fPercent )
{
	if (fPercent < 0.0f || fPercent > 100.0f)
	{
		return;
	}

	if (!m_imgHpBarBg->isVisible())
	{
		m_imgHpBarBg->setVisible(true);
	}

	m_hpBar->setPercent(fPercent);
}

void FormationHero::updateCombo( UI_HERO_LIST_PARAM param )
{
	if (nullptr != m_imgPreState)
	{
		m_imgPreState->loadTexture(getSkillStateIcon(param.nPreState));
	}

	if (nullptr != m_imgNextState)
	{
		m_imgNextState->loadTexture(getSkillStateIcon(param.nNextState));
	}
}
