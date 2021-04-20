#include "SweepSettlementNode.h"
#include "DataManager/DataManager.h"


SweepSettlementNode::SweepSettlementNode(void)
	: m_nSweepOrder(0)
	, m_fSingleHeight(0.0f)
{
	m_vcItem.clear();
}


SweepSettlementNode::~SweepSettlementNode(void)
{
}

SweepSettlementNode* SweepSettlementNode::create(SweepSettlement settlement)
{
	SweepSettlementNode* _node = new SweepSettlementNode;
	if (nullptr != _node && _node->init(settlement))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}


bool SweepSettlementNode::init(SweepSettlement settlement)
{
	if (!Layout::init())
	{
		return false;
	}

	m_nSweepOrder = settlement.nOrder;

	initUI(settlement);
	return true;
}

void SweepSettlementNode::onEnter()
{
	Layout::onEnter();

	// 进场特效;
	playEnterAnimation();
}


void SweepSettlementNode::initUI(SweepSettlement settlement)
{
	// 分割线;
	float nStartY = 5.0f;
	if (settlement.nOrder != 1)
	{
		m_ui.imgSpace = ImageView::create("Image/UIBarrier/SweepTitle/img_saodang_fengexian.png");
		m_ui.imgSpace->setPosition(Vec2(0.0f, -5.0f));
		this->addChild(m_ui.imgSpace);
		nStartY = m_ui.imgSpace->getContentSize().height;
	}

	// 标题;
	ostringstream strText;
	strText << "Image/UIBarrier/SweepTitle/img_" << settlement.nOrder << "ci.png";
	m_ui.imgTitle = ImageView::create(strText.str());
	if (nullptr != m_ui.imgTitle)
	{
		this->addChild(m_ui.imgTitle);
		m_ui.imgTitle->setPosition(Vec2(0.0f, -m_ui.imgTitle->getContentSize().height/2 - nStartY));
	}
	strText.str("");

	// 物品展示;
	int ntmp = (settlement.vcDropItems.size() > 1) ? (settlement.vcDropItems.size() - 1) : 1;
	int nItemsHeight = (ntmp/5 + 1) * 100.0f;
	for (unsigned int i = 0; i < settlement.vcDropItems.size(); i++)
	{
		UIToolHeader* item = UIToolHeader::create(settlement.vcDropItems.at(i).first);
		if (nullptr != item)
		{
			this->addChild(item);
			item->setVisible(false);
			m_vcItem.push_back(item);
			item->setPosition(Vec2(-270.0f + (i%SWEEP_SETTLEMENT_LINE_COUNT) * 135.0f,
				m_ui.imgTitle->getPosition().y - m_ui.imgTitle->getContentSize().height/2.0f - 5.0f - 100.0f/2.0f
				- (i/SWEEP_SETTLEMENT_LINE_COUNT) * 100.0f));
			item->setNumEx(settlement.vcDropItems.at(i).second);
		}
	}

	// 货币;
	strText << settlement.nCoin;
	m_ui.txtCoin = Text::create(STR_UTF8(strText.str()), FONT_NAME_FOUNDER_BOLD, 22);
	if (nullptr != m_ui.txtCoin)
	{
		m_ui.txtCoin->setColor(Color3B(0xea, 0xe9, 0xe2));
		m_ui.txtCoin->setVisible(true);
		m_ui.txtCoin->setAnchorPoint(Vec2(0.0f, 0.0f));
		m_ui.txtCoin->setPosition(Vec2(-290.0f, m_ui.imgTitle->getPosition().y - m_ui.imgTitle->getContentSize().height/2
			- nItemsHeight - m_ui.txtCoin->getContentSize().height - 20.0f));
		this->addChild(m_ui.txtCoin);
		m_ui.imgCoin = ImageView::create("Image/Icon/global/img_yinbi.png");
		m_ui.imgCoin->setPosition(Vec2(m_ui.txtCoin->getPosition().x - 20.0f, m_ui.txtCoin->getPosition().y + 14.0f));
		this->addChild(m_ui.imgCoin);
	}
	strText.str("");

	// 经验;
	strText << settlement.nExp;
	m_ui.txtExp = Text::create(STR_UTF8(strText.str()), FONT_NAME_FOUNDER_BOLD, 22);
	if (nullptr != m_ui.txtExp)
	{
		m_ui.txtExp->setColor(Color3B(0xea, 0xe9, 0xe2));
		m_ui.txtExp->setVisible(true);
		m_ui.txtExp->setAnchorPoint(Vec2(0.0f, 0.0f));
		m_ui.txtExp->setPosition(Vec2(m_ui.txtCoin->getPosition().x + 170.0f, m_ui.txtCoin->getPosition().y));
		this->addChild(m_ui.txtExp);
		m_ui.imgExp = ImageView::create("Image/Icon/global/img_exp.png");
		m_ui.imgExp->setScale(0.75f);
		m_ui.imgExp->setPosition(Vec2(m_ui.txtExp->getPosition().x - 40.0f, m_ui.txtExp->getPosition().y + 14.0f));
		this->addChild(m_ui.imgExp);
	}
	strText.str("");

	// 扣除体力;
	strText << settlement.nPower;
	m_ui.txtPower = Text::create(STR_UTF8(strText.str()), FONT_NAME_FOUNDER_BOLD, 22);
	if (nullptr != m_ui.txtPower)
	{
		m_ui.txtPower->setColor(Color3B(0xE2, 0x20, 0x20));
		m_ui.txtPower->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		m_ui.txtPower->setPosition(Vec2(m_ui.txtCoin->getPosition().x + 540.0f, m_ui.txtCoin->getPosition().y));
		this->addChild(m_ui.txtPower);
		m_ui.imgPower = ImageView::create("Image/Icon/global/img_tili.png");
		m_ui.imgPower->setPosition(Vec2(m_ui.txtPower->getPosition().x - 40.0f, m_ui.txtPower->getPosition().y + 14.0f));
		this->addChild(m_ui.imgPower);
	}
}

void SweepSettlementNode::playEnterAnimation()
{
	if (m_nSweepOrder == 1)
	{
		// 当前扫荡次数飞入;
		shiftWidgetX(m_ui.imgTitle, -1);
	
		// 获得经验/货币，体力消耗等飞入;
		shiftWidgetX(m_ui.imgCoin, -1);
		shiftWidgetX(m_ui.txtCoin, -1);
		shiftWidgetX(m_ui.imgExp, -1);
		shiftWidgetX(m_ui.txtExp, -1);
		shiftWidgetX(m_ui.imgPower, -1);
		shiftWidgetX(m_ui.txtPower, -1, true);
	}
	else
	{
		onAnimationDone(nullptr, 0);
	}
}

void SweepSettlementNode::shiftWidgetX( Widget* _widget, int _direction_x/* = 1*/, bool bCallback/* = false*/ )
{
	// 移动距离和时间;
	float shiftPos_x = 700.0f;
	float shiftTime = 0.4f;

	_widget->setPosition(Point(shiftPos_x * _direction_x * -1, 0.0f) + _widget->getPosition());
	if (bCallback)
	{
		Sequence* action = Sequence::create(MoveBy::create(shiftTime, Vec2(shiftPos_x * _direction_x, 0.0f)),
			CallFuncN::create(CC_CALLBACK_1(SweepSettlementNode::onAnimationDone, this, 0)), nullptr);
		_widget->runAction(action);
	}
	else
	{
		auto action = EaseBackOut::create(MoveBy::create(shiftTime, Vec2(shiftPos_x * _direction_x, 0.0f)));
		_widget->runAction(action);
	}
}

void SweepSettlementNode::onAnimationDone( Node* pSender, int nIndex )
{
	if (m_vcItem.empty())
		return;

	if (nIndex >= m_vcItem.size())
		return;

	m_vcItem.at(nIndex)->setScale(1.5f);
	m_vcItem.at(nIndex)->setVisible(true);

	// 获得物品飞入;
	Sequence* action = Sequence::create(EaseSineIn::create(ScaleTo::create(0.2f, 1.0f)),
		CallFuncN::create(CC_CALLBACK_1(SweepSettlementNode::onAnimationDone, this, nIndex+1)), nullptr);
	m_vcItem.at(nIndex)->runAction(action);
}
