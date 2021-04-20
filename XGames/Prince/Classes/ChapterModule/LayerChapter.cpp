#include "LayerChapter.h"
#include "LayerBarrier.h"
#include "ChapterController.h"
#include "ChapterModel.h"
#include "LayerFrame.h"

LayerChapter::LayerChapter()
	: fOriPosX(0.0f)
	, fOriTime(0.0f)
	, m_chapterSelected(nullptr)
	, m_delegateLayer(nullptr)
	, m_scrollviewSpeed(nullptr)
	, m_bFirstEnter(true)
{
	ChapterController::getInstance()->setChapterView(this);
	ChapterModel::getInstance()->addObserver(this);
}

LayerChapter::~LayerChapter()
{
	//ChapterController::getInstance()->setChapterView(nullptr);
	ChapterModel::getInstance()->removeObserver(this);
}


bool LayerChapter::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}


void LayerChapter::onEnter()
{
	Layer::onEnter();

	if (m_bFirstEnter)
	{
		m_bFirstEnter = false;
		SoundUtils::playSoundEffect("quanping");
	}
}


void LayerChapter::onExit()
{
	// 释放资源;
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIChapter");
	ResourceLoader::getInstance()->removeUIResource("UIChapter/Bg");

	Layer::onExit();
}


void LayerChapter::initUI()
{
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 加载剧情关卡UI;
	string strFileName;
	strFileName.assign("Image/UIChapter/UIChapter.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

	// Bg;
	//ImageView* imgBg = ImageView::create("Image/UIChapter/Bg/tiankong.png");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Image/UIChapter/Bg/Bg.plist");
	Sprite* imgBg = Sprite::createWithSpriteFrameName("tiankong.png");
	imgBg->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	imgBg->setScale(2.0f);
	imgBg->setPosition(Vec2(pos.x, visibleSize.height - imgBg->getContentSize().height * 2.0f));
	this->addChild(imgBg);
	this->addChild(m_ui.pRoot, imgBg->getLocalZOrder()+1);

	// 分层初始化背景;
	// 共7层Map，加1控制层;
	// 由近至远: 前5层(0/1/2/3/4)正向滚动，后5层(5/6)反向滚动;

	// 初始化;
	ostringstream oss;
	for (unsigned int i = 0; i < 7; i++)
	{
		// 容器;
		oss.str("");
		oss << "ScrollView_" << i+1;
		m_ui.scrollViewMap[i] = static_cast<ScrollView*>(m_ui.pRoot->getChildByName(oss.str()));
		m_ui.scrollViewMap[i]->setContentSize(visibleSize);
		m_ui.scrollViewMap[i]->setDirection(ScrollView::Direction::HORIZONTAL);
		m_ui.scrollViewMap[i]->setClippingType(Layout::ClippingType::SCISSOR);
		m_ui.scrollViewMap[i]->setTouchEnabled(false);
		m_ui.scrollViewMap[i]->setPosition(Vec2(pos.x, 0.0f));
	}

	// 创建地图;
	createMapFrame();

	// 创建建筑物层;
	//initBuilding();

	// 触摸代理层;
	initTouchEvent();

	// 添加一层空的ScrollView，做速度控制;
	// 需要以当前层次中的哪一层为滑动参照，就将速度控制层的长度设置为该层长度;
	m_scrollviewSpeed = ScrollView::create();
	if (nullptr != m_scrollviewSpeed)
	{
		this->m_ui.pRoot->addChild(m_scrollviewSpeed, 99);
		m_scrollviewSpeed->setPosition(m_ui.scrollViewMap[4]->getPosition());
		m_scrollviewSpeed->setContentSize(m_ui.scrollViewMap[4]->getContentSize());
		m_scrollviewSpeed->setInnerContainerSize(m_ui.scrollViewMap[4]->getInnerContainerSize());
		m_scrollviewSpeed->setDirection(ScrollView::Direction::HORIZONTAL);
		m_scrollviewSpeed->addEventListener(CC_CALLBACK_2(LayerChapter::onScrollViewEvent, this));
	}

	// 光照;
	//Sprite* imgLight = Sprite::create("Image/UIChapter/Bg/light.png");
	Sprite* imgLight = Sprite::createWithSpriteFrameName("barrier_light.png");
	if (nullptr != imgLight)
	{
		this->addChild(imgLight, m_ui.scrollViewMap[4]->getLocalZOrder() - 1);
		imgLight->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
		imgLight->setScale(2.0f);
		imgLight->setPosition(Vec2(pos.x + visibleSize.width / 2, visibleSize.height));
		imgLight->setBlendFunc(BlendFunc::ADDITIVE);
		ActionInterval* easeAct1 =  EaseSineInOut::create(SkewTo::create(5.0f, 0.0f, 20.0f));
		ActionInterval* easeAct2 =  EaseSineInOut::create(SkewTo::create(5.0f, 0.0f, -20.0f));
		ActionInterval* seq = Sequence::create(easeAct1, easeAct2, nullptr);
		imgLight->runAction(RepeatForever::create(seq));
	}
}

void LayerChapter::createMapFrame()
{
	createMapLayer_1();		// 远山1;
	createMapLayer_2();		// 远山2;
	createMapLayer_3();		// 远山3;
	createMapLayer_4();		// 云层;
	createMapLayer_5();		// 地面;
	createMapLayer_6();		// 近山1;
	createMapLayer_7();		// 近山2;
}

void LayerChapter::createMapLayer_1()
{
	// 本层容器;
	ScrollView* _container = m_ui.scrollViewMap[0];
	if (nullptr == _container)
		return;

	// 本层图片高度;
	float ptY = _container->getContentSize().height - 440.0f;

	// 单次循环图片个数;
	int nImgCount = 2;

	// 循环次数;
	int fLoop = 5.0f;

	// 图片总量;
	int nTotalImgCount = (int)ceil(nImgCount * fLoop);

	// 加载图片;
	ostringstream oss;
	float nWidth = 0.0f;
	for (unsigned int k = 0; k < nTotalImgCount; ++k)
	{
		oss.str("");
		oss << "Map1_"<< k%nImgCount + 1 << ".png";
		Sprite* img = Sprite::createWithSpriteFrameName(oss.str());
		img->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		img->setScale(2.0f);
		img->setPosition(Vec2(nWidth, ptY));
		_container->addChild(img);
		nWidth += img->getContentSize().width * 2.0f;
	}
	_container->setInnerContainerSize(Size(nWidth, Director::getInstance()->getVisibleSize().height));

	// 倒转;
	_container->setRotationSkewY(180.0f);
	_container->setPosition(Point(_container->getContentSize().width, 0) + _container->getPosition());
}

void LayerChapter::createMapLayer_2()
{
	// 本层容器;
	ScrollView* _container = m_ui.scrollViewMap[1];
	if (nullptr == _container)
		return;

	// 本层图片高度;
	float ptY = _container->getContentSize().height - 420.0f;

	// 单次循环图片个数;
	int nImgCount = 2;

	// 循环次数;
	float fLoop = 4.0f;

	// 图片总量;
	int nTotalImgCount = (int)ceil(nImgCount * fLoop);

	// 加载图片;
	ostringstream oss;
	float nWidth = 0.0f;
	for (unsigned int k = 0; k < nTotalImgCount; ++k)
	{
		oss.str("");
		oss << "Map2_"<< k%nImgCount + 1 << ".png";
		Sprite* img = Sprite::createWithSpriteFrameName(oss.str());
		img->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		img->setScale(2.0f);
		img->setPosition(Vec2(nWidth, ptY));
		_container->addChild(img);
		nWidth += img->getContentSize().width * 2.0f;
	}
	_container->setInnerContainerSize(Size(nWidth, Director::getInstance()->getVisibleSize().height));

	// 倒转;
	_container->setRotationSkewY(180.0f);
	_container->setPosition(Point(_container->getContentSize().width, 0) + _container->getPosition());
}

void LayerChapter::createMapLayer_3()
{
	// 本层容器;
	ScrollView* _container = m_ui.scrollViewMap[2];
	if (nullptr == _container)
		return;

	// 本层图片高度;
	float ptY = _container->getContentSize().height - 492.0f;

	// 单次循环图片个数;
	int nImgCount = 2;

	// 循环次数;
	float fLoop = 3.0f;

	// 图片总量;
	int nTotalImgCount = (int)ceil(nImgCount * fLoop);

	// 加载图片;
	ostringstream oss;
	float nWidth = 0.0f;
	for (unsigned int k = 0; k < nTotalImgCount; ++k)
	{
		oss.str("");
		oss << "Map3_"<< k%nImgCount + 1 << ".png";
		Sprite* img = Sprite::createWithSpriteFrameName(oss.str());
		img->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		img->setPosition(Vec2(nWidth, ptY));
		_container->addChild(img);
		nWidth += img->getContentSize().width;
	}
	_container->setInnerContainerSize(Size(nWidth, Director::getInstance()->getVisibleSize().height));
}

void LayerChapter::createMapLayer_4()
{
	// 本层容器;
	ScrollView* _container = m_ui.scrollViewMap[3];
	if (nullptr == _container)
		return;

	// 本层图片高度;
	float ptY = _container->getContentSize().height - 426.0f;

	// 单次循环图片个数;
	int nImgCount = 1;

	// 循环次数;
	float fLoop = 8.0f;

	// 图片总量;
	int nTotalImgCount = (int)ceil(nImgCount * fLoop);

	// 加载图片;
	float nWidth = 0.0f;
	for (unsigned int k = 0; k < nTotalImgCount; ++k)
	{
		Sprite* img = Sprite::createWithSpriteFrameName("yun.png");
		img->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		img->setScale(2.0f);
		img->setPosition(Vec2(nWidth, ptY));
		_container->addChild(img);
		nWidth += img->getContentSize().width * 2.0f;
	}
	_container->setInnerContainerSize(Size(nWidth, Director::getInstance()->getVisibleSize().height));
}

void LayerChapter::createMapLayer_5()
{
	// 本层容器;
	ScrollView* _container = m_ui.scrollViewMap[4];
	if (nullptr == _container)
		return;

	// 本层图片高度;
	float ptY = 0.0f;

	// 单次循环图片个数;
	int nImgCount = 2;

	// 循环次数;
	float fLoop = 4.0f;

	// 图片总量;
	int nTotalImgCount = (int)ceil(nImgCount * fLoop);

	// 加载图片;
	ostringstream oss;
	float nWidth = 0.0f;
	for (unsigned int k = 0; k < nTotalImgCount; ++k)
	{
		oss.str("");
		oss << "Map4_"<< k%nImgCount + 1 << ".png";
		Sprite* img = Sprite::createWithSpriteFrameName(oss.str());
		img->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		img->setPosition(Vec2(nWidth, ptY));
		_container->addChild(img);
		nWidth += img->getContentSize().width;
	}
	_container->setInnerContainerSize(Size(nWidth, Director::getInstance()->getVisibleSize().height));
}

void LayerChapter::createMapLayer_6()
{
	// 本层容器;
	ScrollView* _container = m_ui.scrollViewMap[5];
	if (nullptr == _container)
		return;

	// 本层图片高度;
	float ptY = 0.0f;

	// 单次循环图片个数;
	int nImgCount = 2;

	// 循环次数;
	float fLoop = 7.0f;

	// 图片总量;
	int nTotalImgCount = (int)ceil(nImgCount * fLoop);

	// 加载图片;
	ostringstream oss;
	float nWidth = 0.0f;
	for (unsigned int k = 0; k < nTotalImgCount; ++k)
	{
		oss.str("");
		oss << "Map6_"<< k%nImgCount + 1 << ".png";
		Sprite* img = Sprite::createWithSpriteFrameName(oss.str());
		img->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		img->setScale(2.0f);
		img->setPosition(Vec2(nWidth, ptY));
		_container->addChild(img);
		nWidth += img->getContentSize().width * 2.0f;
	}
	_container->setInnerContainerSize(Size(nWidth, Director::getInstance()->getVisibleSize().height));
}

void LayerChapter::createMapLayer_7()
{
	// 本层容器;
	ScrollView* _container = m_ui.scrollViewMap[6];
	if (nullptr == _container)
		return;

	// 本层图片高度;
	float ptY = 0.0f;

	// 单次循环图片个数;
	int nImgCount = 2;

	// 循环次数;
	float fLoop = 6.0f;

	// 图片总量;
	int nTotalImgCount = (int)ceil(nImgCount * fLoop);

	// 加载图片;
	ostringstream oss;
	float nWidth = 0.0f;
	for (unsigned int k = 0; k < nTotalImgCount; ++k)
	{
		oss.str("");
		oss << "Map7_"<< k%nImgCount + 1 << ".png";
		Sprite* img = Sprite::createWithSpriteFrameName(oss.str());
		img->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		img->setScale(2.0f);
		img->setPosition(Vec2(nWidth, ptY));
		_container->addChild(img);
		nWidth += img->getContentSize().width * 2.0f;
	}
	_container->setInnerContainerSize(Size(nWidth, Director::getInstance()->getVisibleSize().height));
}

void LayerChapter::initBuilding()
{
	/*
	m_ui.scrollViewBuilding = dynamic_cast<ScrollView*>(m_ui.scrollViewMap[4]->getChildByName("ScrollView_Building"));
	m_ui.scrollViewBuilding->setLocalZOrder(2);

	for (unsigned int i = 0; i < 3; ++i)
	{
		ScrollView* _scroll = (ScrollView*)(m_ui.scrollViewBuilding->clone());
		_scroll->setPosition(Point(m_ui.scrollViewBuilding->getInnerContainerSize().width * (i+1), 0) + m_ui.scrollViewBuilding->getPosition());
		m_ui.scrollViewMap[4]->addChild(_scroll);
	}

	//m_ui.scrollViewBuilding->setVisible(false);
	//return;

	int nIndex = 10001;
	for (unsigned int i = 0; i < 10; ++i)
	{
		ChapterNode* chapter = ChapterNode::create(nIndex++);
		chapter->setLogicParent(this);
		chapter->setEnabled(false);
	}

	ostringstream oss;
	oss << "Img_Building_" << nIndex;
	ImageView* imgChapter = dynamic_cast<ImageView*>(m_ui.scrollViewBuilding->getChildByName(oss.str()));
	while (imgChapter != nullptr)
	{
		ChapterNode* chapter = ChapterNode::create(imgChapter);
		chapter->setLogicParent(this);
		chapter->setEnabled(false);
		this->addChild(chapter);
		m_ui.vcChapterList.push_back(pair<int, ChapterNode*>(nIndex++, chapter));
		oss.str("");
		oss << "Img_Building_" << nIndex;
		imgChapter = dynamic_cast<ImageView*>(m_ui.scrollViewBuilding->getChildByName(oss.str()));
	}
	*/
}


void LayerChapter::updateSelf(void* data)
{
	if (nullptr == data)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) data;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_CHAPTER_STATE:
		{
			updateChapterState((UI_CHAPTER_STORY_PARAM*)(param->updateData));
		}
		break;

	default:
		break;
	}
}


void LayerChapter::updateChapterState(UI_CHAPTER_STORY_PARAM* chapterState)
{
	if (nullptr == chapterState || chapterState->mapChapterState.empty())
	{
		return;
	}

	for (auto iter = chapterState->mapChapterState.begin(); iter != chapterState->mapChapterState.end(); ++iter)
	{
		auto ittt = m_ui.mapChapterNode.find(iter->first);
		if (ittt != m_ui.mapChapterNode.end())
		{
			// 第二次更新进度状态;
			ittt->second->setEnabled(true);
			ittt->second->updateStarCount(iter->second.at(0), iter->second.at(1));

			// 星级奖励状态;
			auto iter1 = chapterState->mapStarRewardState.find(iter->first);
			if (iter1 != chapterState->mapStarRewardState.end())
			{
				ittt->second->updateStarReward(iter1->second);
			}
		}
		else
		{
			// 第一次更新全局状态;
			ChapterNode* chapter = ChapterNode::create(iter->first);
			m_ui.mapChapterNode.insert(pair<int, ChapterNode*>(iter->first, chapter));
			chapter->setLogicParent(this);
			if (ChapterModel::getInstance()->getChapterOrderFromChapterId(iter->first) == 1)
			{
				chapter->setEnabled(true);
			}
			else
			{
				chapter->setEnabled(false);
			}
			chapter->updateStarCount(iter->second.at(0), iter->second.at(1));
			Vec2 csvPos = ChapterModel::getInstance()->getChapterPos(iter->first);
			chapter->setPosition(Vec2(csvPos.x + 120.0f, csvPos.y - 150.0f));
			m_ui.scrollViewMap[4]->addChild(chapter, 9);
		}
	}

	/*
	// 更新关卡状态;
	auto it = m_ui.vcChapterList.begin();
	auto iter = chapterState->mapChapterState.begin();
	for (; it != m_ui.vcChapterList.end() && iter != chapterState->mapChapterState.end(); it++, iter++)
	{
		// id赋值;
		it->first = iter->first;
		it->second->setEnabled(true);
		it->second->updateStarCount(iter->second.at(0), iter->second.at(1));
	}
	*/
}

void LayerChapter::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	if (pSender == m_scrollviewSpeed)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Vec2 vec = m_scrollviewSpeed->getInnerContainer()->getPosition();
				float xPercent = -vec.x / (m_scrollviewSpeed->getInnerContainerSize().width - m_scrollviewSpeed->getContentSize().width);
				//log("scrolling: x = %f, y = %f, per: %f", vec.x, vec.y, xPercent);

				//LayerFrame* pParent = static_cast<LayerFrame*>(this->getParent()->getParent());
				//if (nullptr != pParent)
				//{
					onChapterScroll(xPercent);
				//}
			}
			break;

		default:
			break;
		}
	}
}


void LayerChapter::onTouchScrollEvent(Ref* pSender, Widget::TouchEventType type)
{
	/*
	if (type == Widget::TouchEventType::BEGAN)
	{
		//log("touch begin");
		// 记录触摸起始位置和起始时间;
		Point ptStart = m_ui.scrollViewChapter->getInnerContainer()->getPosition();
		fOriPosX = -ptStart.x;
		fOriTime = getCurrTime();
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		//log("touch end");
		// 计算位移和速度，并判断是否需要惯性滑动;
		Point ptEnd = m_ui.scrollViewChapter->getInnerContainer()->getPosition();
		float dis = fabs(-ptEnd.x - fOriPosX);
		float slidTime = getCurrTime() - fOriTime;
		float speed = dis / (slidTime);
		//log("dis = %f, speed = %f", dis, speed);

		// 通过模拟操作，提取出2个位移节点，分别是100和600;
		// 100和600单位是像素，是参照960*540分辨率得出的，在实际分辨率下应同比折算;
		float dis_1 = (Director::getInstance()->getVisibleSize().width * (100.0f)) / 960.0f;
		float dis_2 = (Director::getInstance()->getVisibleSize().width * (600.0f)) / 960.0f;

		// 根据不同位移下的手感，进行分段判断;
		// 其中speed的阀值可以根据效果进行调整;
		if ((dis <= dis_1 && speed > 0.3f)
			|| (dis > dis_1 && dis <= dis_2 && speed > 1.0f)
			|| (dis > dis_2 && speed > 1.8f))
		{
			//log("speed+++++++++++");
			m_ui.scrollViewChapter->setInertiaScrollEnabled(true);
		}
		else
		{
			//log("speed-----------");
			m_ui.scrollViewChapter->setInertiaScrollEnabled(false);
		}
	}
	*/
}


void LayerChapter::onChapterSelected(ChapterNode* chapterNode)
{
	for (auto iter = m_ui.mapChapterNode.begin(); iter != m_ui.mapChapterNode.end(); iter++)
	{
		if (iter->second == chapterNode)
		{
			// 音效;
			SoundUtils::playSoundEffect("dianji");

			// 选择章节;
			onChapterSelected(iter->first);
			break;
		}
	}
}


void LayerChapter::onChapterSelected(int nChapterId)
{
	// 选择章节;
	ChapterController::getInstance()->onChapterSelected(nChapterId);

	LayerBarrier* barrier = LayerBarrier::create();
	if (nullptr != barrier)
	{
		// 添加至frame，与本层同级，避免隐藏本层之后，新的barrier层一起被隐藏;
		this->getParent()->getParent()->addChild(barrier, this->getLocalZOrder()+1);
	}
}

void LayerChapter::onChapterScroll(float offsetX_Percent, float offsetY_Percent)
{
	// 前5层正向，后2层反向;
	float percentX = offsetX_Percent;
	for (unsigned int i = 0; i < 7; i++)
	{
		percentX = /*(i < 2) ? (1.0f - offsetX_Percent) : */offsetX_Percent;
		m_ui.scrollViewMap[i]->scrollToPercentHorizontal(percentX * 100.0f, 0.2f, true);
	}
}


void LayerChapter::initTouchEvent()
{
	if (nullptr == m_delegateLayer)
	{
		//事件代理层，由该层触发点击事件响应;
		m_delegateLayer = Node::create();
		m_delegateLayer->setAnchorPoint(Point(0, 0));
		m_delegateLayer->setContentSize(Director::getInstance()->getVisibleSize());
		this->addChild(m_delegateLayer, 999);

		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = CC_CALLBACK_2(LayerChapter::onTouchBegan, this);
		listener->onTouchMoved = CC_CALLBACK_2(LayerChapter::onTouchMoved, this);
		listener->onTouchEnded = CC_CALLBACK_2(LayerChapter::onTouchEnded, this);
		listener->onTouchCancelled = CC_CALLBACK_2(LayerChapter::onTouchCancelled, this);

		m_delegateLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, m_delegateLayer);
	}
}

void LayerChapter::releaseTouchEvent()
{
	if (nullptr == m_delegateLayer)
		return;

	m_delegateLayer->getEventDispatcher()->removeEventListenersForTarget(m_delegateLayer);
	m_delegateLayer->removeFromParent();
	m_delegateLayer = nullptr;
}

bool LayerChapter::onTouchBegan(Touch *touch, Event* event)
{
	mTouchBeganPoint = touch->getStartLocation();
	for (auto it = m_ui.mapChapterNode.begin(); it != m_ui.mapChapterNode.end(); ++it)
	//for (unsigned int i = 0; i < m_ui.vcChapterList.size(); ++i)
	{
		ChapterNode* _node = it->second;//m_ui.vcChapterList.at(i).second;
		if (!_node->IsEnabled())
		{
			continue;
		}
		ImageView* imgBuilding = _node->getBuilding();
		ImageView* imgTitle = _node->getChapterTitle();
		Rect rectBuilding = Rect(0, 0, imgBuilding->getContentSize().width, imgBuilding->getContentSize().height);
		Rect rectTitle = Rect(0, 0, imgTitle->getContentSize().width, imgTitle->getContentSize().height);
		if (rectBuilding.containsPoint(imgBuilding->convertTouchToNodeSpace(touch))
			|| rectTitle.containsPoint(imgTitle->convertTouchToNodeSpace(touch)))
		{
			m_chapterSelected = _node;
			_node->onChapterClicked(true);
			return true;
		}
	}
	return false;
}

void LayerChapter::onTouchMoved(Touch *touch, Event* event)
{
	// 滑动距离大于阈值，则取消点击;
	if (mTouchBeganPoint.distance(touch->getLocation()) > 20)
	{
		if (m_chapterSelected != nullptr)
		{
			m_chapterSelected->onChapterClicked(false);
			m_chapterSelected = nullptr;
		}
	}
}

void LayerChapter::onTouchEnded(Touch* touch, Event* event)
{
	if (nullptr != m_chapterSelected)
	{
		m_chapterSelected->onChapterClicked(false);
		onChapterSelected(m_chapterSelected);
		m_chapterSelected = nullptr;
	}
}

void LayerChapter::onTouchCancelled(Touch* touch, Event* event)
{
	onTouchMoved(touch, event);
}

bool LayerChapter::alphaTouchCheck(ImageView* image, Touch *touch)
{
	//先从ImageView中提取Sprite
	Sprite* selectSprite = static_cast<Sprite*>(image->getVirtualRenderer());

	//将坐标转换为相对ImageView坐标
	Point pt = image->convertTouchToNodeSpace(touch);

	//复制Sprite
	CCSprite* tempSprite = CCSprite::createWithSpriteFrame(selectSprite->displayFrame());
	tempSprite->setAnchorPoint(Point::ZERO);
	tempSprite->setPosition(Point::ZERO);
	//tempSprite->setSkewY(image->getSkewY());
	Rect rect = tempSprite->getBoundingBox();
	unsigned int x = pt.x;
	unsigned int y = rect.size.height - pt.y;

	//初始化render;
	RenderTexture* renderer = RenderTexture::create(rect.size.width, rect.size.height);

	//开始绘制;
	renderer->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
	tempSprite->visit();
	renderer->end();

	Director::getInstance()->getRenderer()->render();

	//获取像素点;
	Image* pImage = renderer->newImage();
	unsigned char* data_ = pImage->getData();
	int pa = 4 * ((pImage->getHeight() - (int)(pt.y) - 1) * pImage->getWidth() + (int)(pt.x)) + 3;  
	unsigned int ap = data_[pa];
	pImage->release();

	// 	//获取像素数据; 
	// 	ccColor4B color4B(0, 0, 0, 0);
	// 	unsigned char* data_ = pImage->getData();  
	// 	unsigned int *pixel = (unsigned int *)data_;  
	// 	pixel = pixel + (y * (int)pImage->getWidth()) * 1 + x * 1;
	// 	color4B.r = *pixel & 0xff;
	// 	color4B.g = (*pixel >> 8) & 0xff;
	// 	color4B.b = (*pixel >> 16) & 0xff;
	// 	color4B.a = (*pixel >> 24) & 0xff;

	if (ap > 20)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void LayerChapter::switch2Chapter( int nChapterOrder, CHAPTER_NODE_POS dstPos/* = CHAPTER_NODE_POS_LEFT*/ )
{
	int nMaxOrder = m_ui.mapChapterNode.size();
	if (nChapterOrder <= 0)
	{
		nChapterOrder = 1;
	}
	if (nChapterOrder > nMaxOrder)
	{
		nChapterOrder = nMaxOrder;
	}

	// 查找章节;
	int nChapterId = ChapterModel::getInstance()->getChapterIdFromChapterOrder(nChapterOrder);
	auto iter = m_ui.mapChapterNode.find(nChapterId);
	assert(iter != m_ui.mapChapterNode.end());

	// 计算滚动区域总长度;
	float fTotalShiftWidth = m_scrollviewSpeed->getInnerContainerSize().width - m_scrollviewSpeed->getContentSize().width;
	if (fTotalShiftWidth < 0.000001f)
	{
		fTotalShiftWidth = 1.0f;
	}

	// 根据目的位置计算百分比;
	float percent = 0.0f;
	if (dstPos == CHAPTER_NODE_POS_LEFT)
	{
		percent = (iter->second->getPosition().x - 150.0f) / fTotalShiftWidth;
	}
	else if (dstPos == CHAPTER_NODE_POS_MIDDLE)
	{
		percent = (iter->second->getPosition().x - m_scrollviewSpeed->getContentSize().width/2) / fTotalShiftWidth;
	}

	// 容器两端容错;
	if (percent > 1.0f)
	{
		percent = 1.0f;
	}
	if (percent < 0.000001f)
	{
		percent = 0.0f;
	}
	//log("############ before: pos.x = %lf, percent = %lf", m_scrollviewSpeed->getInnerContainer()->getPosition().x, percent);
	m_scrollviewSpeed->scrollToPercentHorizontal(percent * 100.00f, 0.5f, true);
}

void LayerChapter::update( float delta )
{

}

void LayerChapter::setVisible( bool visible )
{
	Layer::setVisible(visible);

	if (visible)
	{
		initTouchEvent();
	}
	else
	{
		releaseTouchEvent();
	}
}
