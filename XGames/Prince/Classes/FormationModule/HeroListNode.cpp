#include "HeroListNode.h"
#include "HeroModule/HeroModel.h"

HeroListNode::HeroListNode(void)
	: m_hero(nullptr)
	, m_imgPreState(nullptr)
	, m_imgArrow(nullptr)
	, m_imgNextState(nullptr)
	, m_imgNoneState(nullptr)
	, m_imgSame(nullptr)
{
}


HeroListNode::~HeroListNode(void)
{
}

HeroListNode* HeroListNode::create( const UI_HERO_LIST_PARAM* pParam )
{
	HeroListNode* _node = new HeroListNode;
	if (nullptr != _node && _node->init(pParam))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool HeroListNode::init( const UI_HERO_LIST_PARAM* pParam )
{
	if (!Button::init("Image/UIFormation/Icon/btn_di_1.png"))
	{
		return false;
	}
	m_param = *pParam;

	initUI(pParam);
	return true;
}

void HeroListNode::initUI( const UI_HERO_LIST_PARAM* pParam )
{
	// Hero;
	string strName = ResourceUtils::getSmallIconPath(pParam->nResId);
	m_hero = UIToolHeader::create(strName.c_str());
	m_hero->setTouchEnabled(false);
	m_hero->setPosition(Vec2(73.0f, 110.0f));
	this->addChild(m_hero);
	this->setTouchEnabled(false);

	// 进阶等级角标;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(pParam->nHeroId);
	assert(heroParam != nullptr);
	m_hero->setQuaCornerIcon(heroParam->mTemplateId, heroParam->mAdvLvl);

	// 星级;
	m_hero->setStarsNum(pParam->nStar, EnumStarContain::STAR_CONTAIN_BOTTOM);

	// 等级;
	m_hero->setLvlWithoutBg(pParam->nLv);

	// 攻防先辅;
	ImageView* imgJob = ImageView::create(getJobIcon((HERO_JOB_TYPE)heroParam->mJobTyp));
	if (imgJob != nullptr)
	{
		imgJob->setPosition(Vec2(30.0f, 125.0f));
		this->addChild(imgJob);
	}

	// 是否上阵;
	updateOnfieldState(pParam->bIsOnField);

	// 技能前置/后置;
	m_imgPreState = ImageView::create(getSkillStateIcon(pParam->nPreState));
	m_imgPreState->setPosition(Vec2(40.0f, 27.0f));
	this->addChild(m_imgPreState);

	m_imgArrow = ImageView::create("Image/UIFormation/Icon/img_jineng_jiantou_2.png");
	m_imgArrow->setPosition(Vec2(73.0f, 27.0f));
	this->addChild(m_imgArrow);
	m_imgArrow->setVisible(pParam->nNextState != AbstractSkill::NONE_STATE);

	m_imgNextState = ImageView::create(getSkillStateIcon(pParam->nNextState));
	m_imgNextState->setPosition(Vec2(105.0f, 27.0f));
	this->addChild(m_imgNextState);

	// 未激活状态;
	if (pParam->nPreState == AbstractSkill::NONE_STATE && pParam->nNextState == AbstractSkill::NONE_STATE)
	{
		m_imgNoneState = ImageView::create("Image/UIFormation/Icon/img_shangweijihuo.png");
		m_imgNoneState->setPosition(Vec2(70.0f, 27.0f));
		this->addChild(m_imgNoneState);
	}

	// 血量;
	if (pParam->nMode == F_MODE_TRAIL)
	{
		//m_hero->setBlood(pParam->fHpPercent);
		m_hero->setDiedFlag(pParam->fHpPercent < 0.00001f);

		// 新的血条位置;
		ImageView* bloodBg = ImageView::create("Image/Fight/img_xueliang_1.png");
		bloodBg->setPosition(Vec2(73.0f, 50.0f));
		bloodBg->setScaleX(1.5f);
		this->addChild(bloodBg);
		LoadingBar* blood = LoadingBar::create("Image/Fight/img_xueliang_2.png");
		blood->setPosition(Vec2(3.5f, 6.5f));
		blood->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		blood->setPercent(pParam->fHpPercent);
		bloodBg->addChild(blood);
		bloodBg->setVisible(pParam->fHpPercent > 0.0001f);
	}
}

void HeroListNode::updateSelected( bool bSelected )
{
	if (bSelected)
	{
		this->loadTextureNormal("Image/UIFormation/Icon/btn_di_2.png");
	}
	else
	{
		this->loadTextureNormal("Image/UIFormation/Icon/btn_di_1.png");
	}
}

void HeroListNode::updateOnfieldState( bool bIsOnfield )
{
	m_hero->setMaskFlag(bIsOnfield);
}

void HeroListNode::updateCombo( UI_HERO_LIST_PARAM* pParam )
{
	m_param = *pParam;
	if (pParam->nPreState == AbstractSkill::NONE_STATE && pParam->nNextState == AbstractSkill::NONE_STATE)
	{
		if (m_imgNoneState == nullptr)
		{
			m_imgNoneState = ImageView::create("Image/UIFormation/Icon/img_shangweijihuo.png");
			m_imgNoneState->setPosition(Vec2(70.0f, 27.0f));
			this->addChild(m_imgNoneState);
		}
		else
		{
			m_imgNoneState->setVisible(true);
		}
	}
	else
	{
		if (m_imgNoneState != nullptr)
		{
			m_imgNoneState->removeFromParent();
			m_imgNoneState = nullptr;
		}
	}

	if (nullptr != m_imgArrow)
	{
		m_imgArrow->setVisible(pParam->nNextState != AbstractSkill::NONE_STATE);
	}

	if (nullptr != m_imgPreState)
	{
		m_imgPreState->setVisible(true);
		m_imgPreState->loadTexture(getSkillStateIcon(pParam->nPreState));
	}

	if (nullptr != m_imgNextState)
	{
		m_imgNextState->setVisible(true);
		m_imgNextState->loadTexture(getSkillStateIcon(pParam->nNextState));
	}
}

void HeroListNode::updateActivityFlag( vector<int> vcDstTemplateId )
{
	int nTemplateId = HeroModel::getInstance()->searchHeroById(m_param.nHeroId)->mTemplateId;
	auto it = find(vcDstTemplateId.begin(), vcDstTemplateId.end(), nTemplateId);
	if (it != vcDstTemplateId.end())
	{
		if (m_imgSame == nullptr)
		{
			m_imgSame = ImageView::create("Image/UIFormation/Icon/img_xiangtong.png");
			m_imgSame->setPosition(Vec2(70.0f, 27.0f));
			this->addChild(m_imgSame);

			if (m_imgPreState != nullptr)	m_imgPreState->setVisible(false);
			if (m_imgArrow != nullptr)		m_imgArrow->setVisible(false);
			if (m_imgNextState != nullptr)	m_imgNextState->setVisible(false);
			if (m_imgNoneState != nullptr)	m_imgNoneState->setVisible(false);
		}
	}
	else
	{
		if (m_imgSame != nullptr)
		{
			m_imgSame->removeFromParent();
			m_imgSame = nullptr;
		}
		updateCombo(&m_param);
	}
}
