#include "TowerFloor.h"
#include "TowerModel.h"
#include "LayerTower.h"
#include "LayerTrail.h"
#include "Widget/SpineAnimation.h"


TowerFloor::TowerFloor(void)
	: m_nTowerType(TOWER_TYPE_TOWER)
	, m_pParent(nullptr)
	, m_IsTargetClicked(false)
{
	for (int i = 0; i < 2; ++i)
	{
		m_layoutDisable[i] = nullptr;
		m_pnlCondition[i] = nullptr;
	}
}


TowerFloor::~TowerFloor(void)
{
}

TowerFloor* TowerFloor::create( TOWER_TYPE nType, int nLogicFirstFloor )
{
	TowerFloor* _node = new TowerFloor;
	if (nullptr != _node && _node->init(nType, nLogicFirstFloor))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool TowerFloor::init( TOWER_TYPE nType, int nLogicFirstFloor )
{
	if (!Layout::init())
	{
		return false;
	}

	this->setContentSize(Director::getInstance()->getWinSize());
	this->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	this->setTouchEnabled(true);
	m_nTowerType = nType;
	m_nLogicFirstFloor = nLogicFirstFloor;
	judgePosition();
	initUI();
	return true;
}

void TowerFloor::initUI()
{
	// bg;
	ImageView* imgBg[4];
	ostringstream oss;
	if (m_nTowerType == TOWER_TYPE_TOWER)
	{
		oss << "Image/Bg/img_bg_pata2.jpg";
	}
	else if (m_nTowerType == TOWER_TYPE_TRAIL)
	{
		oss << "Image/Bg/img_bg_shilian2.jpg";
	}
	for (int i = 0; i < 4; ++i)
	{
		imgBg[i] = ImageView::create(oss.str());
		imgBg[i]->setAnchorPoint(Vec2(0.0f, 0.0f));
		imgBg[i]->setRotationSkewY((i%2!=0)?180.0f:0.0f);
		imgBg[i]->setPosition(Vec2(Director::getInstance()->getWinSize().width * (i%2),
			Director::getInstance()->getWinSize().height/2 * (i/2)));
		this->addChild(imgBg[i]);
	}

	// 黑色间隔条;
	ImageView* imgLine = ImageView::create("Image/UINewtower/Icon/img_di2.png");
	imgLine->setScaleX(this->getContentSize().width / imgLine->getContentSize().width);
	imgLine->setPosition(Vec2(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f));
	this->addChild(imgLine, 6);
	ImageView* imgLine2 = dynamic_cast<ImageView*>(imgLine->clone());
	imgLine2->setPosition(Vec2(this->getContentSize().width/2.0f,
		this->getContentSize().height * (m_nTowerType == TOWER_TYPE_TOWER ? 0.0f : 1.0f)));
	this->addChild(imgLine2, 6);

	// 层数;
	ImageView* imgFloorNumFrame[2];
	ImageView* imgFloorNumBg[2];
	for (int i = 0; i < 2; ++i)
	{
		imgFloorNumBg[i] = ImageView::create("Image/UINewtower/Icon/img_pata_di3.png");
		imgFloorNumBg[i]->setPosition(m_posFloorNum[i]);
		this->addChild(imgFloorNumBg[i], 2);
	}
	int  nPreFloor, nNextFloor;
	nPreFloor = m_nLogicFirstFloor;
	nNextFloor = nPreFloor+1;

	// 前层数;
	oss.str("");

	oss << DataManager::getInstance()->searchCommonTexdtById(10110);//第
	if (m_nTowerType == TOWER_TYPE_TRAIL)
	{
		//层
		oss.str("");
		oss << DataManager::getInstance()->searchCommonTexdtById(20330);//负
	}

	oss << nPreFloor << DataManager::getInstance()->searchCommonTexdtById(20331);//层

	Text*  txtPreFloor = Text::create(oss.str(), FONT_FZZHENGHEI, 32);
	txtPreFloor->setColor(Color3B(0xFE, 0xFF, 0xCC));
	//txtPreFloor->enableOutline(Color4B(Color3B(0x0, 0x0, 0x0)), 2);
	setTextAddStroke(txtPreFloor, Color3B(0x0, 0x0, 0x0), 2);
	txtPreFloor->setPosition(m_posFloorNum[0]);
	this->addChild(txtPreFloor, 3);
	oss.str("");

	// 后层数;

	oss << DataManager::getInstance()->searchCommonTexdtById(10110);//第
	if (m_nTowerType == TOWER_TYPE_TRAIL)
	{
		//层
		oss.str("");
		oss << DataManager::getInstance()->searchCommonTexdtById(20330);//负
	}

	oss << nNextFloor << DataManager::getInstance()->searchCommonTexdtById(20331);//层
	Text*  txtNextFloor = Text::create(oss.str(), FONT_FZZHENGHEI, 32);
	txtNextFloor->setColor(Color3B(0xFE, 0xFF, 0xCC));
	//txtNextFloor->enableOutline(Color4B(Color3B(0x0, 0x0, 0x0)), 2);
	setTextAddStroke(txtNextFloor, Color3B(0x0, 0x0, 0x0), 2);
	txtNextFloor->setPosition(m_posFloorNum[1]);
	this->addChild(txtNextFloor, 3);
	oss.str("");

	// 前层target类型;
	m_Target[0] = createTarget(nPreFloor);
	m_Target[0]->setPosition(m_posTarget[0]);
	this->addChild(m_Target[0], 5);

	// 后层target类型;
	m_Target[1] = createTarget(nNextFloor);
	m_Target[1]->setPosition(m_posTarget[1]);
	this->addChild(m_Target[1], 5);

	// 特效;
	auto action = FadeIn::create(0.2f);
	m_Target[0]->setOpacity(0);
	m_Target[0]->runAction(action);
	m_Target[1]->setOpacity(0);
	m_Target[1]->runAction(action->clone());

	// 过关条件;
	if (m_nTowerType == TOWER_TYPE_TOWER)
	{
		m_pnlCondition[0] = createCondition(nPreFloor);
		this->addChild(m_pnlCondition[0]);
		m_pnlCondition[1] = createCondition(nNextFloor);
		this->addChild(m_pnlCondition[1]);
	}
}

void TowerFloor::judgePosition()
{
	//Size winSize = Director::getInstance()->getWinSize();

	// 绝对位置;
	Vec2 posFloorNum2(this->getContentSize().width/2, 25.0f);
	Vec2 posTarget2(Point(0.0f, 30.0f) + posFloorNum2);
	Vec2 posFloorNum1(Point(0.0f, this->getContentSize().height/2) + posFloorNum2);
	Vec2 posTarget1(Point(0.0f, 30.0f) + posFloorNum1);

	// 相对位置;
	if (m_nTowerType == TOWER_TYPE_TRAIL)
	{
		m_posFloorNum[0] = posFloorNum1;
		m_posFloorNum[1] = posFloorNum2;
		m_posTarget[0] = posTarget1;
		m_posTarget[1] = posTarget2;
	}
	else if (m_nTowerType == TOWER_TYPE_TOWER)
	{
		m_posFloorNum[0] = posFloorNum2;
		m_posFloorNum[1] = posFloorNum1;
		m_posTarget[0] = posTarget2;
		m_posTarget[1] = posTarget1;
	}
}

Node* TowerFloor::createTarget( int nFloor )
{
	Node* target = nullptr;

	// 确定战斗类型;
	TOWER_FIGHT_TYPE nFightType = TowerModel::getInstance()->getFightType(m_nTowerType, nFloor);

	// 生产角色;
	if (nFightType == TFT_FIGHT)
	{
		int nResId = TowerModel::getInstance()->getOpResId(m_nTowerType, nFloor);
		SpineAnimation* _spine = SpineAnimation::createFighterWithResId(nResId);
		_spine->setAnimation(0, "wait", true);
		_spine->setContentSize(Size(200.0f, 200.0f));
		_spine->setTag(1000+nFloor);

		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [=](Touch* touch, Event* event)
		{
			Point pt = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
			Size size = event->getCurrentTarget()->getContentSize();
			Rect rect(0.0f, 0.0f, size.width, size.height);
			if (rect.containsPoint(Point(pt.x + size.width/2, pt.y)))
			{
				onTargetClicked(event->getCurrentTarget(), Widget::TouchEventType::BEGAN, TFT_FIGHT);
				return true;
			}
			else
			{
				return false;
			}
		};
		listener->onTouchMoved = [=](Touch* touch, Event* event)
		{
		};
		listener->onTouchCancelled = [=](Touch* touch, Event* event)
		{
		};
		listener->onTouchEnded = [=](Touch* touch, Event* event)
		{
			onTargetClicked(event->getCurrentTarget(), Widget::TouchEventType::ENDED, TFT_FIGHT);
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _spine);
		target = _spine;
	}
	// 生产宝箱;
	else if (nFightType == TFT_REWARD)
	{
		ImageView* img = ImageView::create("Image/UINewtower/Icon/btn_baoxiang_3.png");
		img->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
		img->setScale(0.55f);
		img->setTouchEnabled(true);
		img->setTag(1000+nFloor);
		img->addTouchEventListener(CC_CALLBACK_2(TowerFloor::onTargetClicked, this, nFightType));
		target = img;
	}

	return target;
}

Layout* TowerFloor::createCondition( int nFloor )
{
	Layout*  pnlCondition = Layout::create();

	if (m_nTowerType == TOWER_TYPE_TOWER/* && TowerModel::getInstance()->isLogicCurFloor(m_nTowerType, nFloor)*/)
	{
		// 背景图;
		ImageView* imgBg = ImageView::create("Image/UINewtower/Icon/img_pata_di2.png");
		imgBg->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		pnlCondition->addChild(imgBg, 2);
		imgBg->setVisible(false);

		// 标题;
		ImageView* imgDesc = ImageView::create("Image/UINewtower/Icon/img_tongguantiaojian.png");
		imgDesc->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

		// 内容;
		Text* txtDesc = Text::create((TowerModel::getInstance()->getClearCondition(nFloor).c_str()), FONT_FZZHENGHEI, 26);
		txtDesc->setColor(Color3B(0xE8, 0xDE, 0xB5));
		//txtDesc->enableOutline(Color4B(Color3B(0x7D, 0x3F, 0x1C)), 2);
		setTextAddStroke(txtDesc, Color3B(0x7D, 0x3F, 0x1C), 2);
		txtDesc->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

		// 位置;
		int nTotalLength = imgDesc->getContentSize().width + txtDesc->getContentSize().width;
		imgBg->setScaleX(((float)(nTotalLength + 60.0f))/imgBg->getContentSize().width);
		imgDesc->setPosition(Point(0.0f, -4.0f) + imgBg->getPosition());
		txtDesc->setPosition(Vec2(imgDesc->getPosition().x + imgDesc->getContentSize().width, imgDesc->getPosition().y + 1.0f));
		pnlCondition->addChild(imgDesc, 3);
		pnlCondition->addChild(txtDesc, 3);
		pnlCondition->setContentSize(Size(nTotalLength, 40.0f));
		pnlCondition->setPosition(Vec2(m_posFloorNum[0].x - nTotalLength/2, 320.0f
			+ (nFloor - m_nLogicFirstFloor)*(Director::getInstance()->getVisibleSize().height/2)));
	}

	return pnlCondition;
}

void TowerFloor::onTargetClicked( Ref* pSender, Widget::TouchEventType type, TOWER_FIGHT_TYPE nFightType )
{
	if (m_IsTargetClicked)
		return;

	// 楼层校验;
	if (!TowerModel::getInstance()->isLogicCurFloor(m_nTowerType, ((Node*)pSender)->getTag()-1000))
	{
		return;
	}

	if (type == Widget::TouchEventType::BEGAN)
	{
		CCLOG("target clicked -----> began");
		SoundUtils::playSoundEffect("zhandou");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		m_IsTargetClicked = true;
		CCLOG("target clicked -----> end 1");
		if (m_nTowerType == TOWER_TYPE_TOWER)
		{
			LayerTower* parent = dynamic_cast<LayerTower*>(m_pParent);
			if (nullptr != parent)
			{
				parent->onFloorTargetClicked(nFightType);
			}
		}
		else if (m_nTowerType == TOWER_TYPE_TRAIL)
		{
			LayerTrail* parent = dynamic_cast<LayerTrail*>(m_pParent);
			if (nullptr != parent)
			{
				parent->onFloorTargetClicked(nFightType);
			}
		}
		CCLOG("target clicked -----> end 3");

		getScheduler()->schedule([=](float dt)->void {
			this->getScheduler()->unschedule("TowerFloor::onTargetClicked", this);
			m_IsTargetClicked = false;
			CCLOG("target clicked -----> reset");
		},
			this, 0, 0, 0, false, "TowerFloor::onTargetClicked");
	}
}

void TowerFloor::updateFloorState()
{
	// 更新两个楼层的状态;
	for (int i = 0; i < 2; ++i)
	{
		// 不是当前楼层，禁用;
		if (!TowerModel::getInstance()->isLogicCurFloor(m_nTowerType, m_nLogicFirstFloor+i))
		{
			// 黑色蒙版;
			if (m_layoutDisable[i] == nullptr)
			{
				m_layoutDisable[i] = Layout::create();
				m_layoutDisable[i]->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				m_layoutDisable[i]->setContentSize(Size(this->getContentSize().width, this->getContentSize().height/2));
				m_layoutDisable[i]->setPosition(Vec2(m_posFloorNum[i].x - this->getContentSize().width/2, m_posFloorNum[i].y - 20.0f));
				this->addChild(m_layoutDisable[i], 4);

				// 添加蒙版;
				ImageView* imgDark1 = ImageView::create("Image/Bg/img_pata_zhezhao2.png");
				imgDark1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				imgDark1->setScale(2.0f);
				m_layoutDisable[i]->addChild(imgDark1);
				ImageView* imgDark2 = dynamic_cast<ImageView*>(imgDark1->clone());
				imgDark2->setRotationSkewY(180.0f);
				imgDark2->setPosition(Point(m_layoutDisable[i]->getContentSize().width, 0.0f) + imgDark1->getPosition());
				m_layoutDisable[i]->addChild(imgDark2);

				// **黑了;
				assert(m_Target[i] != nullptr);
				m_Target[i]->setColor(Color3B::BLACK);

			}

			// 过关条件;
			if (m_pnlCondition[i] != nullptr)
			{
				m_pnlCondition[i]->setVisible(false);
			}

			// 如果是宝箱，校验状态;
			// 只有试炼有宝箱，所以此处只取试炼的当前楼层;
			if (TowerModel::getInstance()->getFightType(m_nTowerType, m_nLogicFirstFloor+i) == TFT_REWARD
				&& TowerModel::getInstance()->getTrailCurFloor() > m_nLogicFirstFloor+i)
			{
				CCLOG("++++++++++++++++ m_nTowerType = %d", m_nTowerType);
				CCLOG("++++++++++++++++ m_nLogicFirstFloor = %d", m_nLogicFirstFloor);
				CCLOG("++++++++++++++++ i = %d", i);
				CCLOG("++++++++++++++++ getFightType() = %d", TowerModel::getInstance()->getFightType(m_nTowerType, m_nLogicFirstFloor+i));
				CCLOG("++++++++++++++++ getTrailCurFloor() = %d", TowerModel::getInstance()->getTrailCurFloor());
				ImageView* img = dynamic_cast<ImageView*>(m_Target[i]);
				assert(img != nullptr);
				img->loadTexture("Image/UINewtower/Icon/btn_baoxiang_4.png");
			}
		}
		else
		{
			if (m_layoutDisable[i] != nullptr)
			{
				// 遮罩渐隐消失;
				Action* seq = Sequence::create( FadeOut::create(0.5f),
					CallFuncN::create(CC_CALLBACK_1(TowerFloor::onAnimationDone, this, m_nLogicFirstFloor+i)),
					nullptr);
				m_layoutDisable[i]->runAction(seq);
				m_Target[i]->setColor(Color3B(0xFF, 0xFF, 0xFF));
			}

			// 过关条件;
			if (m_pnlCondition[i] != nullptr)
			{
				m_pnlCondition[i]->setVisible(true);
			}
		}

		// 达到顶层后，隐藏顶层 非宝箱的monster;
		bool  bb = (m_nTowerType == TOWER_TYPE_TRAIL ? TowerModel::getInstance()->getTrailMainInfo()->bEnd : TowerModel::getInstance()->getTowerMainInfo()->bEnd);
		int   n1 = (m_nTowerType == TOWER_TYPE_TRAIL ? TowerModel::getInstance()->getTrailMainInfo()->nCurFloor : TowerModel::getInstance()->getTowerMainInfo()->nCurFloor);
		int   n2 = (m_nTowerType == TOWER_TYPE_TRAIL ? TowerModel::getInstance()->getTrailMainInfo()->nMaxFloor: TowerModel::getInstance()->getTowerMainInfo()->nMaxFloor);
		if (bb && n1 == n2 && m_nLogicFirstFloor+i == n1)
		{
			if (TowerModel::getInstance()->getFightType(m_nTowerType, m_nLogicFirstFloor+i) == TFT_FIGHT)
			{
				m_Target[i]->setVisible(false);
			}
			else if (TowerModel::getInstance()->getFightType(m_nTowerType, m_nLogicFirstFloor+i) == TFT_REWARD)
			{
				ImageView* img = dynamic_cast<ImageView*>(m_Target[i]);
				assert(img != nullptr);
				img->loadTexture("Image/UINewtower/Icon/btn_baoxiang_4.png");
			}
		}
	}
}

void TowerFloor::onAnimationDone(Node* pSender, int nFloor)
{
	// 遮罩消失;
	assert(m_layoutDisable[nFloor-m_nLogicFirstFloor] != nullptr);
	m_layoutDisable[nFloor-m_nLogicFirstFloor]->removeFromParent();
	m_layoutDisable[nFloor-m_nLogicFirstFloor] = nullptr;
}
