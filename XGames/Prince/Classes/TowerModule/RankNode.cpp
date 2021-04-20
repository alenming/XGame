#include "RankNode.h"
#include "TowerModule/LayerTrailRanking.h"

RankNode::RankNode(void)
{
	m_pLogicParent = nullptr;
}


RankNode::~RankNode(void)
{
}

RankNode* RankNode::create( Layer* pLogicParent, RankingData* pData )
{
	RankNode* _node = new RankNode;
	if (nullptr != _node && _node->init(pLogicParent, pData))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool RankNode::init( Layer* pLogicParent, RankingData* pData )
{
	if (!Node::init())
	{
		return false;
	}

	m_pLogicParent = pLogicParent;

	initUI(pData);

	return true;
}

void RankNode::initUI( RankingData* pData )
{
	ostringstream strText;

	// 头像;
	UIToolHeader* player = UIToolHeader::create(ResourceUtils::getSmallIconPath(pData->nResId));
	player->setScale(0.7f);
	player->setPosition(Vec2(150.0f, 30.0f));
	this->addChild(player);
	player->setTouchEnabled(true);
	player->addTouchEventListener(CC_CALLBACK_2(RankNode::onPlayerClicked, this, pData->nRankingIndex));

	// 排名图标(可能为空，前*名才有);
	if (pData->bShowRankingIcon)
	{
		strText << "Image/Icon/global/img_" << pData->nRankingIndex+1 << "ming.png";
		m_ui.imgRanking = ImageView::create(strText.str());
		if (nullptr != m_ui.imgRanking)
		{
			m_ui.imgRanking->setPosition(Vec2(player->getPosition().x - 110.0f, player->getPosition().y));
			this->addChild(m_ui.imgRanking);
		}
		strText.str("");
	}
	else
	{
		ImageView* img = ImageView::create("Image/Icon/global/img_4ming.png");
		img->setPosition(Vec2(player->getPosition().x - 110.0f, player->getPosition().y));
		this->addChild(img);
		strText << pData->nRankingIndex+1;
		m_ui.txtRanking = Text::create(STR_UTF8(strText.str()), FONT_ARLRDBD, 26);
		if (nullptr != m_ui.txtRanking)
		{
			//m_ui.txtRanking->setColor(Color3B(0xEE, 0xB0, 0x73));
			//m_ui.txtRanking->enableOutline(Color4B(Color3B(0x7B, 0x29, 0x22)), 3);
			m_ui.txtRanking->setPosition(Vec2(player->getPosition().x - 112.0f, player->getPosition().y + 2.0f));
			this->addChild(m_ui.txtRanking);
		}
		strText.str("");
	}

	// 玩家名;
	m_ui.txtPlayerName = Text::create(pData->strPlayerName, FONT_FZZHENGHEI, 26);
	m_ui.txtPlayerName->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_ui.txtPlayerName->setColor(Color3B(0xE8, 0xDE, 0xB5));
	//m_ui.txtPlayerName->enableOutline(Color4B(Color3B(0x7D, 0x3F, 0x1C)), 2);
	setTextAddStroke(m_ui.txtPlayerName, Color3B(0x7D, 0x3F, 0x1C), 2);
	m_ui.txtPlayerName->setPosition(Vec2(player->getPosition().x + 50.0f, player->getPosition().y - m_ui.txtPlayerName->getContentSize().height/2));
	this->addChild(m_ui.txtPlayerName);

	// 玩家最高层;
	strText.str("");
	strText << pData->nMaxFloor << DataManager::getInstance()->searchCommonTexdtById(20057);
	m_ui.txtPlayerMaxFloor = Text::create(strText.str(), FONT_FZZHENGHEI, 26);
	m_ui.txtPlayerMaxFloor->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_ui.txtPlayerMaxFloor->setColor(Color3B(0xFE, 0xFF, 0xCC));
	m_ui.txtPlayerMaxFloor->setPosition(Point(200.0f, 0.0f) + m_ui.txtPlayerName->getPosition());
	this->addChild(m_ui.txtPlayerMaxFloor);

	// 分割线;
	m_ui.imgLine = ImageView::create("Image/Icon/global/img_tanchukuang2_fengexian.png");
	if (nullptr != m_ui.imgLine)
	{
		m_ui.imgLine->setPosition(Vec2(player->getPosition().x + 100.0f, player->getPosition().y - 45.0f));
		this->addChild(m_ui.imgLine);
	}
}


void RankNode::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::ENDED)
	{
		switch (nWidgetName)
		{
		case Btn_Gift:
			{
				// 送体力;
				//CustomPop::show("你体力多啊？现在不让送！");
			}
			break;

		default:
			break;
		}
	}
}

void RankNode::onPlayerClicked( Ref* ref, Widget::TouchEventType type, int nIndex )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		LayerTrailRanking* _parent = dynamic_cast<LayerTrailRanking*>(m_pLogicParent);
		if (nullptr != _parent)
		{
			_parent->onRankingNodeClicked(nIndex);
		}
	}
}


