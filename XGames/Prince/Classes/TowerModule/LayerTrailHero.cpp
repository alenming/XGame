#include "LayerTrailHero.h"
#include "TowerModel.h"
#include "LayerOp.h"

LayerTrailHero::LayerTrailHero(void)
{
}


LayerTrailHero::~LayerTrailHero(void)
{
}

bool LayerTrailHero::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();
	return true;
}

void LayerTrailHero::onEnter()
{
	Layer::onEnter();

	// 音效;
	SoundUtils::playSoundEffect("tanchuang");
}

void LayerTrailHero::initUI()
{
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UINewtower/UINewtower_hero.ExportJson"));
	this->addChild(m_ui.pRoot);

	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Close)));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerTrailHero::onBtnClicked, this, Btn_Close));

	m_ui.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Bg"));
	m_ui.scrollviewContent = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, "ScrollView_Content"));
	m_ui.scrollviewContent->addEventListener(CC_CALLBACK_2(LayerTrailHero::onScrollViewEvent, this));

	// 添加滚动条;
	m_ui.scrollBar = CustomScrollBar::create();
	m_ui.scrollBar->initScrollBar(m_ui.scrollviewContent->getContentSize().height);
	m_ui.scrollBar->setPosition(Vec2(33.0f, 35.0f));
	m_ui.imgBg->addChild(m_ui.scrollBar);

	// 英雄列表;
	vector<HeroParam*> vcDstHeroList;
	TowerModel::getInstance()->getBuffDstHeros(vcDstHeroList);
	for (int i = 0; i < vcDstHeroList.size(); ++i)
	{
		TrailHeroNode* _node = TrailHeroNode::create(vcDstHeroList.at(i));
		_node->setLogicParent(this);
		int nRow = (vcDstHeroList.size()/3 + ((vcDstHeroList.size() % 3) == 0 ? 0 : 1));
		int scrollViewHeight = ((nRow * 190.0f) > m_ui.scrollviewContent->getContentSize().height)
			? (nRow * 190.0f) : getContentSize().height;
		m_ui.scrollviewContent->setInnerContainerSize(Size(m_ui.scrollviewContent->getContentSize().width, scrollViewHeight));
		Size innerSize = m_ui.scrollviewContent->getInnerContainerSize();
		_node->setPosition(Vec2((i % 3) * (innerSize.width/3.0f),
			innerSize.height - 190.0f - (i/3) * 190.0f));
		m_ui.scrollviewContent->addChild(_node);
	}

	// 滚动条;
	m_ui.scrollBar->setScrollBarLengthPercent(m_ui.scrollviewContent->getContentSize().height / m_ui.scrollviewContent->getInnerContainerSize().height);
	m_ui.scrollBar->setScrollBarPercentY(0.0f);
}

void LayerTrailHero::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (nWidgetName)
		{
		case Btn_Close:
			{
				this->scheduleUpdate();
			}
			break;

		default:
			break;
		}
	}
}

void LayerTrailHero::update( float delta )
{
	this->unscheduleUpdate();
	this->removeFromParent();
}

void LayerTrailHero::onScrollViewEvent( Ref* pSender, ScrollView::EventType type )
{
	if (pSender == m_ui.scrollviewContent)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.scrollviewContent->getInnerContainerSize();
				Vec2 vec = m_ui.scrollviewContent->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.scrollviewContent->getContentSize().height);
				//log("training scrolling: x = %f, y = %f, per: %f", vec.x, vec.y, percentY);

				m_ui.scrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			break;

		default:
			break;
		}
	}
}

void LayerTrailHero::onSelectHero( const int nHeroId )
{
	LayerOp* parent = dynamic_cast<LayerOp*>(this->getParent());
	if (parent != nullptr)
	{
		parent->onSelectHeroConfirm(nHeroId);
		this->scheduleUpdate();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

TrailHeroNode::TrailHeroNode()
	: m_nHeroId(-1)
	, m_parent(nullptr)
{

}

TrailHeroNode::~TrailHeroNode()
{

}

TrailHeroNode* TrailHeroNode::create( HeroParam* param )
{
	TrailHeroNode* _node = new TrailHeroNode;
	if (nullptr != _node && _node->init(param))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool TrailHeroNode::init( HeroParam* param )
{
	if (!Layout::init())
	{
		return false;
	}

	m_nHeroId = param->mHeroId;
	initUI(param);
	return true;
}

void TrailHeroNode::initUI( HeroParam* param )
{
	Layout* pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UINewtower/UINewtower_choose.ExportJson"));
	this->addChild(pRoot);

	Button* btnHero = dynamic_cast<Button*>(Helper::seekWidgetByName(pRoot, "Btn_Hero"));
	btnHero->addTouchEventListener(CC_CALLBACK_2(TrailHeroNode::onBtnClicked, this));

	// 1-力  2-智  3-敏(默认图标);
	ImageView* imgType = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pRoot, "Img_Type"));
	if (param->mHeroType == 1)
	{
		imgType->loadTexture("Image/Icon/global/img_shuxing11_li.png");
	}
	else if (param->mHeroType == 2)
	{
		imgType->loadTexture("Image/Icon/global/img_shuxing33_zhi.png");
	}

	// 英雄名称;
	Text* txtName = dynamic_cast<Text*>(Helper::seekWidgetByName(pRoot, "Lab_Name"));
	txtName->setString(param->getName());
	txtName->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(txtName, Color3B(0x82, 0x2B, 0x2B), 2);

	// 战斗力;
	Text* txtFc = dynamic_cast<Text*>(Helper::seekWidgetByName(pRoot, "Lab_Fc"));
	txtFc->setString(STR_UTF8(_TO_STR(param->mFighting)));
	txtFc->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(txtFc, Color3B(0x82, 0x2B, 0x2B), 2);

	// 英雄;
	ImageView* imgHero = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pRoot, "Img_Hero"));
	UIToolHeader* _hero = UIToolHeader::create(param->mTemplateId);
	_hero->setLvl(param->mLevel);
	_hero->setStarsNum(param->mHeroStars, STAR_CONTAIN_BOTTOM);
	_hero->setPosition(imgHero->getPosition());
	imgHero->getParent()->addChild(_hero);
	imgHero->removeFromParent();

	// 血量;
	float percent = (float)(param->mHpCur)/(float)(param->mHp);
	//ImageView* imgBlood = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pRoot, "Img_Blood"));
	//imgBlood->setVisible(false);
	LoadingBar* _blood = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(pRoot, "ProgressBar_12"));
	_blood->setPercent(percent);
	//LoadingBar* _blood = LoadingBar::create(imgBlood->getVirtualRenderer()->getName(), percent);
	//_blood->setPosition(imgBlood->getPosition());
	//imgBlood->getParent()->addChild(_blood, 2);
}

void TrailHeroNode::onBtnClicked( Ref* ref, Widget::TouchEventType type )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (m_parent != nullptr)
		{
			LayerTrailHero* _layer = dynamic_cast<LayerTrailHero*>(m_parent);
			if (nullptr != _layer)
			{
				_layer->onSelectHero(m_nHeroId);
			}
		}
	}
}
