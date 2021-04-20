#include "BuffNode.h"
#include "TowerModel.h"

BuffNode::BuffNode(void)
	: m_pParent(nullptr)
	, m_nBuffId(0)
	, m_nBuffIndex(0)
{
}


BuffNode::~BuffNode(void)
{
}

BuffNode* BuffNode::create( UI_BUFF_PARAM param, LayerOp* parent )
{
	BuffNode* _node = new BuffNode;
	if (nullptr != _node && _node->init(param, parent))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool BuffNode::init( UI_BUFF_PARAM param, LayerOp* parent )
{
	if (!Node::init())
	{
		return false;
	}

	m_nBuffId = param.nBuffId;
	m_pParent = parent;
	initUI(param);
	return true;
}

void BuffNode::initUI(UI_BUFF_PARAM param)
{
	m_nBuffIndex = param.nIndex;

	// frame;
	ostringstream oss;
	oss << "Image/UINewtower/Icon/btn_buff" << param.nIndex+1 << "_1.png";
	m_ui.imgFrame = ImageView::create(oss.str());
	this->addChild(m_ui.imgFrame, 2);
	m_ui.imgFrame->setTouchEnabled(true);
	m_ui.imgFrame->addTouchEventListener(CC_CALLBACK_2(BuffNode::onBtnClicked, this));
	oss.str("");

	// buff
	oss << "Image/Icon/Buff/" << param.nBuffId << ".png";
	m_ui.imgBuff = ImageView::create(oss.str());
	m_ui.imgBuff->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgBuff->setPosition(Vec2(m_ui.imgFrame->getPosition().x - 2.0f, m_ui.imgFrame->getPosition().y + 75.0f));
	this->addChild(m_ui.imgBuff, 1);
	oss.str("");

	// buff名称;
	m_ui.txtName = Text::create(param.strBuffName, FONT_FZZHENGHEI, 48);
	m_ui.txtName->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	//m_ui.txtName->enableOutline(Color4B(Color3B::BLACK), 4);
	setTextAddStroke(m_ui.txtName, Color3B::BLACK,4);
	m_ui.txtName->setPosition(Vec2(m_ui.imgFrame->getContentSize().width/2, 170.0f));
	m_ui.imgFrame->addChild(m_ui.txtName);

	// 描述;
	m_ui.txtDesc = Text::create(param.strBuffDesc, FONT_FZZHENGHEI, 25);
	m_ui.txtDesc->ignoreContentAdaptWithSize(false);
	m_ui.txtDesc->setContentSize(Size(210.0f, 170.0f));
	m_ui.txtDesc->setTextHorizontalAlignment(TextHAlignment::CENTER);
	m_ui.txtDesc->setColor(Color3B(0xD5, 0xAF, 0x86));
	//m_ui.txtDesc->enableOutline(Color4B(Color3B(0x56, 0x21, 0x01)), 2);
	setTextAddStroke(m_ui.txtDesc, Color3B(0x56, 0x21, 0x01), 2);
	m_ui.txtDesc->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.txtDesc->setPosition(Vec2(m_ui.imgFrame->getContentSize().width/2, 40.0f));
	m_ui.imgFrame->addChild(m_ui.txtDesc);

	// 星星;
	m_ui.imgStar = ImageView::create("Image/Icon/global/img_zhanzhang.png");
	m_ui.imgStar->setPosition(Vec2(m_ui.imgFrame->getContentSize().width/2, m_ui.txtDesc->getPosition().y + 3.0f));
	m_ui.imgStar->setScale(0.8f);
	m_ui.imgFrame->addChild(m_ui.imgStar);

	// 价格;
	oss.str("");
	oss << param.nPrice;
	m_ui.txtStar = Text::create(oss.str(), FONT_FZZHENGHEI, 30);
	m_ui.txtStar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.txtStar->setPosition(Vec2(m_ui.imgStar->getPosition().x + 30.0f, m_ui.imgStar->getPosition().y - 2.0f));
	m_ui.imgFrame->addChild(m_ui.txtStar);

	m_ui.txtStarTitle = Text::create(oss.str(), FONT_FZZHENGHEI, 30);
	m_ui.txtStarTitle->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.txtStarTitle->setPosition(Vec2(m_ui.imgStar->getPosition().x - 100.0f, m_ui.imgStar->getPosition().y - 2.0f));
	m_ui.txtStarTitle->setString(DataManager::getInstance()->searchCommonTexdtById(20048)+":");
	m_ui.imgFrame->addChild(m_ui.txtStarTitle);

}

void BuffNode::updateState( bool bEnabled )
{
	if (!bEnabled)
	{
		ImageView* imgSold = ImageView::create("Image/UINewtower/Icon/img_yiduihuan.png");
		imgSold->setPosition(Vec2(m_ui.imgFrame->getContentSize().width/4.0f*3.0f - 10.0f, m_ui.imgFrame->getContentSize().height - 40.0f));
		m_ui.imgFrame->addChild(imgSold);

		m_ui.imgFrame->setTouchEnabled(false);
	}

}

void BuffNode::onBtnClicked( Ref* pSender, Widget::TouchEventType type )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		ostringstream oss;
		oss << "Image/UINewtower/Icon/btn_buff" << m_nBuffIndex+1 << "_2.png";
		m_ui.imgFrame->loadTexture(oss.str());
		oss.str("");

		// 播放音效;
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::CANCELED)
	{
		ostringstream oss;
		oss << "Image/UINewtower/Icon/btn_buff" << m_nBuffIndex+1 << "_1.png";
		m_ui.imgFrame->loadTexture(oss.str());
		oss.str("");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		ostringstream oss;
		oss << "Image/UINewtower/Icon/btn_buff" << m_nBuffIndex+1 << "_1.png";
		m_ui.imgFrame->loadTexture(oss.str());
		oss.str("");

		m_pParent->onBuffClicked(m_nBuffId);
	}
}
