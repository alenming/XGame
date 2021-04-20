#include "LayerTrailRewardPreview.h"
#include "TowerController.h"
#include "TowerModel.h"
#include "Widget/ActionCreator.h"

LayerTrailRewardPreview::LayerTrailRewardPreview(void)
{
	TowerModel::getInstance()->addObserver(this);
}


LayerTrailRewardPreview::~LayerTrailRewardPreview(void)
{
	TowerModel::getInstance()->removeObserver(this);
}

bool LayerTrailRewardPreview::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();
	TowerController::getInstance()->handleTowerReward();
	return true;
}

void LayerTrailRewardPreview::onEnter()
{
	Layer::onEnter();

	ActionCreator::runCommDlgAction(m_ui.imgPreviewBg);

	// 音效;
	SoundUtils::playSoundEffect("tanchuang");

	// test;
	/*
	vector<RewardPreviewParam>  vcParam;

	for (int i = 1; i < 10; ++i)
	{
		RewardPreviewParam  param;
		param.nIndex = i*5;

		for (int k = 0; k < 4; ++k)
		{
			param.mapReward.insert(make_pair(70001+k, i*3+k));
		}
		vcParam.push_back(param);
	}
	updateRewardShow(&vcParam);
	*/
}

void LayerTrailRewardPreview::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_TOWER_REWARD_SHOW:
		{
			updateRewardShow((vector<RewardPreviewParam>*)(param->updateData));
		}
		break;

	default:
		break;
	}
}

void LayerTrailRewardPreview::update( float dt )
{
	this->unscheduleUpdate();
	this->removeFromParent();
}

void LayerTrailRewardPreview::initUI()
{
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UINewtower/UINewtower_Box.ExportJson"));
	this->addChild(m_ui.pRoot);

	m_ui.imgPreviewBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Preview_Bg"));

	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Close)));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerTrailRewardPreview::onBtnClicked, this, Btn_Close));

	m_ui.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Bg"));
	m_ui.scrollviewContent = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, "ScrollView_Content"));
	m_ui.scrollviewContent->addEventListener(CC_CALLBACK_2(LayerTrailRewardPreview::onScrollViewEvent, this));

	// 添加滚动条;
	m_ui.scrollBar = CustomScrollBar::create();
	m_ui.scrollBar->initScrollBar(m_ui.scrollviewContent->getContentSize().height);
	m_ui.scrollBar->setPosition(Vec2(33.0f, 35.0f));
	m_ui.imgBg->addChild(m_ui.scrollBar);
}

void LayerTrailRewardPreview::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
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

void LayerTrailRewardPreview::onScrollViewEvent( Ref* pSender, ScrollView::EventType type )
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

void LayerTrailRewardPreview::updateRewardShow( vector<RewardPreviewParam>* vcReward )
{
	if (vcReward->size() == 0)
	{
		return;
	}

	m_ui.scrollBar->setVisible(vcReward->size() >= 5);

	int nNodeHeight = 112.0f;
	int nScollviewWidth = m_ui.scrollviewContent->getContentSize().width;
	int nScrollviewHeight = (vcReward->size() * nNodeHeight > m_ui.scrollviewContent->getContentSize().height) ?
		vcReward->size() * nNodeHeight : m_ui.scrollviewContent->getContentSize().height;
	m_ui.scrollviewContent->setInnerContainerSize(Size(nScollviewWidth, nScrollviewHeight));

	for (unsigned int i = 0; i < vcReward->size(); ++i)
	{
		RewardPreviewNode* _node = RewardPreviewNode::create(vcReward->at(i));
		_node->setPosition(Vec2(40.0f, nScrollviewHeight - nNodeHeight + 30.0f - i * nNodeHeight));
		m_ui.scrollviewContent->addChild(_node);
	}

	m_ui.scrollBar->setScrollBarLengthPercent(m_ui.scrollviewContent->getContentSize().height / m_ui.scrollviewContent->getInnerContainerSize().height);
	m_ui.scrollBar->setScrollBarPercentY(0.0f);
}


//////////////////////////////////////////////////////////////////////////////////////////////

RewardPreviewNode::RewardPreviewNode()
{

}

RewardPreviewNode::~RewardPreviewNode()
{

}

RewardPreviewNode* RewardPreviewNode::create( RewardPreviewParam param )
{
	RewardPreviewNode* _layer = new RewardPreviewNode;
	if (nullptr != _layer && _layer->init(param))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}
bool RewardPreviewNode::init( RewardPreviewParam param )
{
	if (!Layout::init())
	{
		return false;
	}

	initUI(param);
	return true;
}

void RewardPreviewNode::initUI(RewardPreviewParam param)
{
	ostringstream oss;
	//oss << DataManager::getInstance()->searchCommonTexdtById(10110) << param.nIndex <<DataManager::getInstance()->searchCommonTexdtById(20057);

	oss << DataManager::getInstance()->searchCommonTexdtById(10110);//第
	oss << param.nIndex << DataManager::getInstance()->searchCommonTexdtById(20331);//层

	Text* txtIndex = Text::create(oss.str(), FONT_FZZHENGHEI, 40);
	txtIndex->setColor(Color3B(0xFE, 0xFF, 0xCC));
	setTextAddStroke(txtIndex, Color3B(0x93, 0x0, 0x0), 3);
	txtIndex->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	txtIndex->setPosition(Vec2(0.0f, 0.0f));
	this->addChild(txtIndex);

	ImageView* imgLine = ImageView::create("Image/Icon/global/img_tanchukuang2_fengexian.png");
	imgLine->setPosition(Vec2(txtIndex->getPosition().x + 290.0f, txtIndex->getPosition().y - 30.0f));
	this->addChild(imgLine);

	int nCount = 0;
	float fStartX = txtIndex->getPosition().x + 220.0f;
	float fStartY = txtIndex->getPosition().y + 20.0f;
	for (auto iter = param.mapReward.begin(); iter != param.mapReward.end(); ++iter, ++nCount)
	{
		// 220  20
		UIToolHeader* _item = UIToolHeader::create(iter->first);
		_item->setToolId(iter->first);
		_item->setTipsEnabled(true);
		_item->setScale(0.8f);
		_item->setNumEx(iter->second);
		_item->setPosition(Vec2(fStartX + 105.0f * nCount, fStartY));
		this->addChild(_item);
	}
}
