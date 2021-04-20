#include "RefineryLayer.h"
#include "Widget/ActionCreator.h"
#include "Widget/LayerGameRules.h"

RefineryLayer::RefineryLayer()
{
	RefineryModel::getInstance()->addObserver(this);
	ShopexcModel::getInstance()->addObserver(this);
	m_BgListBar = nullptr;
	m_LongPress = false;
	m_isMove = false;
	m_canRefinery = true;
	m_danCount = 0;
	m_LuMaterilCount = 0;
	CalculationChip();
	CalculationOther();
}

RefineryLayer::~RefineryLayer()
{
	RefineryModel::getInstance()->removeObserver(this);
	ShopexcModel::getInstance()->removeObserver(this);
}

RefineryLayer* RefineryLayer::create(bool pushScene)
{
	RefineryLayer *pRet = new RefineryLayer();
	if (pRet && pRet->init(pushScene))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool RefineryLayer::init(bool pushScene)
{
	m_isPushScene = pushScene;
	if (!Layer::init())
	{
		return false;
	}

	initUi();
	return true;
}

void RefineryLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIRefinery/UIRefinery.ExportJson"));
	this->addChild(m_ui.pRoot);	
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");

	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			if (m_isPushScene)
			{
				Director::getInstance()->popScene();
			}
			else
			{
				Director::getInstance()->replaceScene(MainCityScene::create());
			}
		}
	};
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_REFINING_FURNACE);
	m_ui.pRoot->getChildByName("Panel_13")->addChild(_layerCommHeadInfo);

	m_ui.PanelLeft = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Refinery")); //炉子
	m_ui.PanelRight = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Material")); //材料
	m_ui.PanelLeft->setVisible(true);
	m_ui.PanelRight->setVisible(true);
	Vec2 PanelLeftPos = m_ui.PanelLeft->getPosition();
	Vec2 PanelRightPos = m_ui.PanelRight->getPosition();
	m_ui.PanelLeft->setPosition(Vec2(-m_ui.PanelLeft->getContentSize().width,PanelLeftPos.y));
	m_ui.PanelRight->setPosition(Vec2(size.width + pos.x,PanelRightPos.y));
	m_ui.PanelLeft->runAction(EaseSineOut::create(CCMoveTo::create(0.3f,PanelLeftPos)));
	m_ui.PanelRight->runAction(EaseSineOut::create(CCMoveTo::create(0.3f,PanelRightPos)));

	m_ui.checkBaGua = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.PanelLeft,"bagua"));
	m_ui.checkJiuGong = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.PanelLeft,"jiugong"));
	m_ui.checkBaGua->addEventListener(CC_CALLBACK_2(RefineryLayer::onCheckNotify,this));
	m_ui.checkJiuGong->addEventListener(CC_CALLBACK_2(RefineryLayer::onCheckNotify,this));

	m_ui.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Image_Bg_1")); //背景
	for (int i = 0; i < MaterialCount; i++)
	{
		ImageView *ImageItem = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Image_Item_" + TO_STR(i+1)));
		m_ui.materialItem[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(ImageItem,"Image_k"));
	}
	m_ui.baGuaShop = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Button_Shop_1"));
	m_ui.jiuGongShop = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Button_Shop_2"));
	m_ui.refining = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Button_Refining"));
	m_ui.baGuaShop->addTouchEventListener(CC_CALLBACK_2(RefineryLayer::onNotify,this));
	m_ui.jiuGongShop->addTouchEventListener(CC_CALLBACK_2(RefineryLayer::onNotify,this));
	m_ui.refining->addTouchEventListener(CC_CALLBACK_2(RefineryLayer::onNotify,this));
	
	m_ui.imgShowBagua = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Image_Pill_1"));
	m_ui.imgShowJiuGong = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Image_Pill_2"));
	m_ui.imgGetBagua = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Image_pill_3"));
	m_ui.imgGetJiuGong = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Image_pill_4"));
	m_ui.getAmount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Label_amount"));
	m_ui.tips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PanelLeft,"Label_tips"));
	m_ui.materialList = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.PanelRight,"ScrollView_ItemList"));
	ImageView* pmaterialListDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PanelRight,"Image_Bg_1"));
	m_ui.meTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PanelRight,"meTips"));
	m_BgListBar = CustomScrollBar::create();
	m_BgListBar->initScrollBar(m_ui.materialList->getContentSize().height);
	m_BgListBar->setPosition(m_ui.materialList->getPosition() + Point(-12, 0));
	pmaterialListDi->addChild(m_BgListBar);
	m_BgListBar->setScrollBarLengthPercent(1.0f);
	m_BgListBar->setScrollBarPercentY(0.0f);
	m_ui.materialList->addEventListener(CC_CALLBACK_2(RefineryLayer::onScrollViewEvent, this));
	m_ui.getAmount->setFontName(FONT_FZZHENGHEI);
	m_ui.tips->setFontName(FONT_FZZHENGHEI);
	m_ui.meTips->setFontName(FONT_FZZHENGHEI);
	m_danCount = 0;
	m_ui.getAmount->setString(TO_STR(m_danCount));
	setTextAddStroke(m_ui.getAmount,Color3B(0xb3,0x22,0x05),2);
	setTextAddStroke(m_ui.tips,Color3B(0x7d,0x1f,0x3d),2);
	setTextAddStroke(m_ui.meTips,Color3B(0x7d,0x1f,0x3d),2);


	switchState();
}

void RefineryLayer::onEnter()
{
	Layer::onEnter();
}

void RefineryLayer::onExit()
{
	cocostudio::GUIReader::destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIRefinery");
	this->unschedule(schedule_selector(RefineryLayer::onCallMaterilAdd));
	this->unschedule(schedule_selector(RefineryLayer::onAddMateril));
	Layer::onExit();
}

void RefineryLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	//炼化炉商店兑换 要通知炼化炉列表更新
	if (observerParam->self == ShopexcModel::getInstance())
	{
		if (observerParam->id != OBS_PARAM_TYPE_SHOPESC_COUNT_DOWN)
		{
			CalculationOther();
			CalculationChip();

			switch (RefineryModel::getInstance()->getRefineryState())
			{
			case REFINERY_STATE_BAGUA:
				{
					addMaterilBaGuaList();
				}
				break;
			case REFINERY_STATE_JIUGONG:
				{
					addMaterilJiuGongList();
				}
				break;
			default:
				break;
			}
		}
	}
	else
	{
		switch (observerParam->id)
		{
		case REFINERY_EVENT_TYPE_SEND:
			{
				m_LuMaterilCount = 0;
				for (int i = 0; i < MaterialCount; i++)
				{
					if (m_LuMaterilVec[i].id == 0)
					{
						break;
					}
					m_LuMaterilCount++;
				}

				//播放特效
				switch (RefineryModel::getInstance()->getRefineryState())
				{
				case REFINERY_STATE_BAGUA:
					{
						ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anilianhua_BaGua/Anilianhua_BaGua.ExportJson");
						Armature* armature = Armature::create("Anilianhua_BaGua");
						armature->getAnimation()->play(TO_STR(m_LuMaterilCount) + "_GE");

						auto call = [this](Armature *armature, MovementEventType eventType, const std::string& strName)->void
						{
							if (eventType == MovementEventType::COMPLETE)
							{
								armature->removeFromParentAndCleanup(true);	
								m_canRefinery = true;
							}
						} ;
						armature->getAnimation()->setMovementEventCallFunc(call);
						armature->setPosition(Vec2(m_ui.imgShowBagua->getContentSize().width/2,m_ui.imgShowBagua->getContentSize().height/2));
						m_ui.imgShowBagua->addChild(armature);
					}
					break;
				case REFINERY_STATE_JIUGONG:
					{
						ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anilianhua_BaGua/Anilianhua_BaGua.ExportJson");
						Armature* armature = Armature::create("Anilianhua_BaGua");
						armature->getAnimation()->play(TO_STR(m_LuMaterilCount) + "_GE");

						auto call = [this](Armature *armature, MovementEventType eventType, const std::string& strName)->void
						{
							if (eventType == MovementEventType::COMPLETE)
							{
								armature->removeFromParentAndCleanup(true);	
								m_canRefinery = true;
							}
						} ;
						armature->getAnimation()->setMovementEventCallFunc(call);
						armature->setPosition(Vec2(m_ui.imgShowJiuGong->getContentSize().width/2,m_ui.imgShowJiuGong->getContentSize().height/2));
						m_ui.imgShowJiuGong->addChild(armature);
					}
					break;
				default:
					break;
				}
				auto call = [this]()->void
				{
					for (int i = 0; i < MaterialCount; i++)
					{
						m_ui.materialItem[i]->removeAllChildren();

						m_LuMaterilVec[i].amount = 0;
						m_LuMaterilVec[i].id = 0;
						m_LuMaterilVec[i].idx = -1;
					}
				};
				m_ui.getAmount->runAction(CCSequence::create(CCDelayTime::create(0.5f),CCCallFunc::create(call),nullptr));
				m_danCount = 0;
				m_ui.getAmount->setString(TO_STR(m_danCount));
				setTextAddStroke(m_ui.getAmount,Color3B(0xb3,0x22,0x05),2);

				switch (RefineryModel::getInstance()->getRefineryState())
				{
				case REFINERY_STATE_BAGUA:
					{
						CalculationOther();
						addMaterilBaGuaList();
					}
					break;
				case REFINERY_STATE_JIUGONG:
					{
						CalculationChip();
						addMaterilJiuGongList();
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}
}

void RefineryLayer::switchState()
{
	m_danCount = 0;
	switch (RefineryModel::getInstance()->getRefineryState())
	{
	case REFINERY_STATE_BAGUA:
		{
			m_ui.checkBaGua->setSelectedState(true);
			m_ui.checkBaGua->setTouchEnabled(false); 

			m_ui.checkJiuGong->setSelectedState(false);
			m_ui.checkJiuGong->setTouchEnabled(true); 

			m_ui.baGuaShop->setVisible(true);
			m_ui.jiuGongShop->setVisible(false);
			m_ui.imgShowBagua->setVisible(true);
			m_ui.imgShowJiuGong->setVisible(false);
			m_ui.imgGetBagua->setVisible(true);
			m_ui.imgGetJiuGong->setVisible(false);
			resetLuMateril();
			addMaterilBaGuaList();

			m_danCount = 0;
			m_ui.getAmount->setString(TO_STR(m_danCount));
			setTextAddStroke(m_ui.getAmount,Color3B(0xb3,0x22,0x05),2);
		}
		break;
	case REFINERY_STATE_JIUGONG:
		{
			m_ui.checkBaGua->setSelectedState(false);
			m_ui.checkBaGua->setTouchEnabled(true); 

			m_ui.checkJiuGong->setSelectedState(true);
			m_ui.checkJiuGong->setTouchEnabled(false); 

			m_ui.baGuaShop->setVisible(false);
			m_ui.jiuGongShop->setVisible(true);
			m_ui.imgShowBagua->setVisible(false);
			m_ui.imgShowJiuGong->setVisible(true);
			m_ui.imgGetBagua->setVisible(false);
			m_ui.imgGetJiuGong->setVisible(true);
			resetLuMateril();
			addMaterilJiuGongList();

			m_danCount = 0;
			m_ui.getAmount->setString(TO_STR(m_danCount));
			setTextAddStroke(m_ui.getAmount,Color3B(0xb3,0x22,0x05),2);
		}
		break;
	default:
		break;
	}
}

void RefineryLayer::onNotify(Ref* sender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (sender == m_ui.baGuaShop) //八卦炉商店
		{
			LayerShopexc* _layerShop = LayerShopexc::create(SHOP_TYPE_REFINE_8);
			if (nullptr != _layerShop)
			{
				this->addChild(_layerShop);
			}
			resetLuMateril(true);
			ShopexcModel::getInstance()->addObserver(this);
		}
		else if (sender == m_ui.jiuGongShop) //九宫炉商店
		{
			LayerShopexc* _layerShop = LayerShopexc::create(SHOP_TYPE_REFINE_9);
			if (nullptr != _layerShop)
			{
				this->addChild(_layerShop);
			}
			resetLuMateril(true);
			ShopexcModel::getInstance()->addObserver(this);
		}
		else if (sender == m_ui.refining) //炼化
		{
			REFINERY_NODE_VEC nodeVec;
			for (int i = 0; i < MaterialCount; i++)
			{
				if (m_LuMaterilVec[i].idx >= 0)
				{
					REFINERY_NODE node;
					node.id = m_LuMaterilVec[i].id;
					node.amount = m_LuMaterilVec[i].amount;

					nodeVec.push_back(node);
				}
			}
			if (nodeVec.size() <= 0)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20212));
				return;
			}
			if (m_canRefinery)
			{
				m_canRefinery = false;
				RefineryController::refiney(nodeVec);
			}
		}
	}
}

void RefineryLayer::onCheckNotify(Ref* sender, CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED)
	{
		m_canRefinery = true;
		SoundUtils::playSoundEffect("dianji");
		if(sender == m_ui.checkBaGua) 
		{
			RefineryModel::getInstance()->getRefineryState() = REFINERY_STATE_BAGUA;
			switchState();
		}
		else if (sender == m_ui.checkJiuGong) //添加好友
		{
			RefineryModel::getInstance()->getRefineryState() = REFINERY_STATE_JIUGONG;
			switchState();
		}
	}
}

void RefineryLayer::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	switch(type)
	{
	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			if (pSender == m_ui.materialList)
			{
				Size size = m_ui.materialList->getInnerContainerSize();
				Vec2 vec = m_ui.materialList->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.materialList->getContentSize().height);
				m_BgListBar->setScrollBarPercentY((1.0f - percentY)*100.0f);

				m_isMove = true;
			}
		}
		break;
	default:
		if (pSender == m_ui.materialList)
		{
			m_isMove = false;
		}
		break;
	}
}

void RefineryLayer::addMaterilJiuGongList()
{
	int row = ceil(m_chipVec.size()/3.0f);
	int scrollViewHeight = row * 130;
	scrollViewHeight = scrollViewHeight >  m_ui.materialList->getContentSize().height ? scrollViewHeight : m_ui.materialList->getContentSize().height;
	int scrollViewWidth = m_ui.materialList->getInnerContainerSize().width;
	m_ui.materialList->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	//添加奖励面板控件
	m_ui.materialList->removeAllChildren();
	for (int i = 0; i < m_chipVec.size(); i++)
	{
		int rowNumber = i/3;
		int colNumber = i%3;
		UIToolHeader* item = UIToolHeader::create(m_chipVec.at(i).id);
		item->setAnchorPoint(Vec2(0,0));
		item->setNumEx(m_chipVec.at(i).amount);
		item->setScale(1.0f);
		item->setToolId(m_chipVec.at(i).id);
		//item->setTipsEnabled(true);
		item->setTouchEnabled(true);
		item->setPosition(Vec2(colNumber*130 + 20, scrollViewHeight-(rowNumber+1)*130+20 ));
		m_ui.materialList->addChild(item,0,"JiuGong"+TO_STR(i));

		auto callBack = [this,i](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
				m_LongPress = false;
				m_materIdx = i;

				this->schedule(schedule_selector(RefineryLayer::onCallMaterilAdd), 0.5f);
			}
			else if (type == Widget::TouchEventType::MOVED)
			{
				//this->unschedule(schedule_selector(RefineryLayer::onCallMaterilAdd));
			}
			else
			{
				this->unschedule(schedule_selector(RefineryLayer::onCallMaterilAdd));
				this->unschedule(schedule_selector(RefineryLayer::onAddMateril));

				if (!m_LongPress)
				{
					setLuMateril(i);
				}
			}
		};
		item->addTouchEventListener(callBack);
	}

	//初始化完毕再设置滚动条比例
	m_BgListBar->setScrollBarLengthPercent(
		m_ui.materialList->getContentSize().height / m_ui.materialList->getInnerContainerSize().height);
	m_BgListBar->setScrollBarPercentY(0.0f);
	m_ui.materialList->jumpToTop();
}

void RefineryLayer::resetLuMateril(bool openShop)
{
	switch (RefineryModel::getInstance()->getRefineryState())
	{
	case REFINERY_STATE_BAGUA:
		{
			for (int i = 0; i < MaterialCount; i++)
			{
				m_ui.materialItem[i]->removeAllChildren();

				if (!openShop)
				{
					int chipIdx = m_LuMaterilVec[i].idx;
					if (chipIdx >= 0)
					{
						m_chipVec.at(chipIdx).amount = m_chipVec.at(chipIdx).amount + m_LuMaterilVec[i].amount;
						m_chipVec.at(chipIdx).idx = -1;
					}
				}

				m_LuMaterilVec[i].amount = 0;
				m_LuMaterilVec[i].id = 0;
				m_LuMaterilVec[i].idx = -1;
			}
		}
		break;
	case  REFINERY_STATE_JIUGONG:
		{
			for (int i = 0; i < MaterialCount; i++)
			{
				m_ui.materialItem[i]->removeAllChildren();

				if (!openShop)
				{
					int otherIdx = m_LuMaterilVec[i].idx;
					if (otherIdx >= 0)
					{
						m_otherVec.at(otherIdx).amount = m_otherVec.at(otherIdx).amount + m_LuMaterilVec[i].amount;
						m_otherVec.at(otherIdx).idx = -1;
					}
				}

				m_LuMaterilVec[i].amount = 0;
				m_LuMaterilVec[i].id = 0;
				m_LuMaterilVec[i].idx = -1;
			}
		}
		break;
	default:
		break;
	}
}

void RefineryLayer::addMaterilBaGuaList()
{
	int row = ceil(m_otherVec.size()/3.0f);
	int scrollViewHeight = row * 130;
	scrollViewHeight = scrollViewHeight >  m_ui.materialList->getContentSize().height ? scrollViewHeight : m_ui.materialList->getContentSize().height;
	int scrollViewWidth = m_ui.materialList->getInnerContainerSize().width;
	m_ui.materialList->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	//添加奖励面板控件
	m_ui.materialList->removeAllChildren();
	for (int i = 0; i < m_otherVec.size(); i++)
	{
		int rowNumber = i/3;
		int colNumber = i%3;
		UIToolHeader* item = UIToolHeader::create(m_otherVec.at(i).id);
		item->setAnchorPoint(Vec2(0,0));
		item->setNumEx(m_otherVec.at(i).amount);
		item->setScale(1.0f);
		item->setTouchEnabled(true);
		item->setToolId(m_otherVec.at(i).id);
		//item->setTipsEnabled(true);
		item->showLightEff();
		item->setPosition(Vec2(colNumber*130 + 20, scrollViewHeight-(rowNumber+1)*130+20 ));
		m_ui.materialList->addChild(item,0,"BaGua"+TO_STR(i));

		auto callBack = [this,i](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
				m_LongPress = false;
				m_materIdx = i;

				this->schedule(schedule_selector(RefineryLayer::onCallMaterilAdd), 0.5f);
			}
			else if (type == Widget::TouchEventType::MOVED)
			{
				//this->unschedule(schedule_selector(RefineryLayer::onCallMaterilAdd));
			}
			else
			{
				this->unschedule(schedule_selector(RefineryLayer::onCallMaterilAdd));
				this->unschedule(schedule_selector(RefineryLayer::onAddMateril));

				if (!m_LongPress)
				{
					setLuMateril(i);
				}
			}
		};
		item->addTouchEventListener(callBack);
	}

	//初始化完毕再设置滚动条比例
	m_BgListBar->setScrollBarLengthPercent(
		m_ui.materialList->getContentSize().height / m_ui.materialList->getInnerContainerSize().height);
	m_BgListBar->setScrollBarPercentY(0.0f);
	m_ui.materialList->jumpToTop();
}

void RefineryLayer::onCallMaterilAdd(float dt)
{
	m_LongPress = true;
	this->unschedule(schedule_selector(RefineryLayer::onCallMaterilAdd));
	this->schedule(schedule_selector(RefineryLayer::onAddMateril), 0.1f, kRepeatForever, 0.0f);
}

void RefineryLayer::onAddMateril(float dt)
{
	setLuMateril(m_materIdx);
}

void RefineryLayer::setLuMateril(int idx)
{
	if (m_isMove)
	{
		return;
	}

	switch (RefineryModel::getInstance()->getRefineryState())
	{
	case REFINERY_STATE_BAGUA:
		{
			if (m_otherVec.at(idx).amount <= 0)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20213));
				this->unschedule(schedule_selector(RefineryLayer::onCallMaterilAdd));
				this->unschedule(schedule_selector(RefineryLayer::onAddMateril));
				return;
			}

			int findIdx = -1;
			for (int i = 0; i < MaterialCount; i++)
			{
				RowData* pRowData = DataManager::getInstance()->searchToolById(m_LuMaterilVec[i].id);
				if (m_LuMaterilVec[i].idx < 0 || (m_LuMaterilVec[i].id == m_otherVec.at(idx).id && 
					m_LuMaterilVec[i].amount < pRowData->getIntData("maxSta")
					&& m_LuMaterilVec[i].idx == idx))
				{
					findIdx = i;
					break;
				}
			}
			if (findIdx < 0)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20214));
				return;
			}

			//设置八卦材料
			m_otherVec.at(idx).amount--;
			UIToolHeader* BaGuaMateril = (UIToolHeader*)m_ui.materialList->getChildByName("BaGua"+TO_STR(idx));
			//材料位保存对应炉子信息
			BaGuaMateril->setNumEx(m_otherVec.at(idx).amount);
			setGetDanCount(m_otherVec.at(idx).id,true,1);

			//设置炉子位置材料
			if (m_otherVec.at(idx).idx < 0)
			{
				//炉位保存对应材料信息
				m_LuMaterilVec[findIdx].id = m_otherVec.at(idx).id;
				m_LuMaterilVec[findIdx].amount++;
				m_LuMaterilVec[findIdx].idx = idx;

				//材料位保存对应炉子信息
				m_otherVec.at(idx).idx = findIdx;

				//炉子位更新数据
				UIToolHeader* LuMateril = (UIToolHeader*)m_ui.materialItem[findIdx]->getChildByName("LuMateril");
				if (!LuMateril)
				{
					LuMateril = UIToolHeader::create(m_LuMaterilVec[findIdx].id);
					LuMateril->setAnchorPoint(Vec2(0.5,0.5));
					LuMateril->setScale(1.0f);
					LuMateril->showLightEff();
					LuMateril->setPosition(Vec2(m_ui.materialItem[findIdx]->getContentSize().width/2,
						m_ui.materialItem[findIdx]->getContentSize().height/2));
					LuMateril->setTouchEnabled(true);
					m_ui.materialItem[findIdx]->addChild(LuMateril,0,"LuMateril");

					auto callBack = [this,findIdx](Ref* sender, Widget::TouchEventType type)->void
					{
						if (type == Widget::TouchEventType::BEGAN)
						{
							SoundUtils::playSoundEffect("dianji");
						}
						else if (type == Widget::TouchEventType::ENDED)
						{
							setGetDanCount(m_LuMaterilVec[findIdx].id,false,m_LuMaterilVec[findIdx].amount);

							//材料位还原
							int otherIdx = m_LuMaterilVec[findIdx].idx;
							m_otherVec.at(otherIdx).idx = -1;
							m_otherVec.at(otherIdx).amount = m_otherVec.at(otherIdx).amount + m_LuMaterilVec[findIdx].amount;
							UIToolHeader* BaGuaMateril = (UIToolHeader*)m_ui.materialList->getChildByName("BaGua"+TO_STR(otherIdx));
							//材料位保存对应炉子信息
							BaGuaMateril->setNumEx(m_otherVec.at(otherIdx).amount);

							//炉位清空
							m_LuMaterilVec[findIdx].id = 0;
							m_LuMaterilVec[findIdx].amount = 0;
							m_LuMaterilVec[findIdx].idx = -1;

							m_deleteItem = (Widget*)sender;
							Director::getInstance()->getScheduler()->schedule(schedule_selector(RefineryLayer::updateNextFram), this, 0, false);
						}
					};

					LuMateril->addTouchEventListener(callBack);
				}

				LuMateril->setNumEx(m_LuMaterilVec[findIdx].amount);
			}
			else
			{
				//炉位保存对应材料信息
				int findIdx = m_otherVec.at(idx).idx;
				m_LuMaterilVec[findIdx].amount++;

				//炉子位更新数据
				UIToolHeader* LuMateril = (UIToolHeader*)m_ui.materialItem[findIdx]->getChildByName("LuMateril");
				LuMateril->setNumEx(m_LuMaterilVec[findIdx].amount);
			}
		}
		break;
	case REFINERY_STATE_JIUGONG:
		{
			if (m_chipVec.at(idx).amount <= 0)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20215));
				this->unschedule(schedule_selector(RefineryLayer::onCallMaterilAdd));
				this->unschedule(schedule_selector(RefineryLayer::onAddMateril));
				return;
			}
			int findIdx = -1;
			for (int i = 0; i < MaterialCount; i++)
			{
				RowData* pRowData = DataManager::getInstance()->searchToolById(m_LuMaterilVec[i].id);
				if (m_LuMaterilVec[i].idx < 0 || (m_LuMaterilVec[i].id == m_chipVec.at(idx).id && 
					m_LuMaterilVec[i].amount < pRowData->getIntData("maxSta")
					&& m_LuMaterilVec[i].idx == idx))
				{
					findIdx = i;
					break;
				}
			}
			if (findIdx < 0)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20214));
				return;
			}

			//设置九宫材料
			m_chipVec.at(idx).amount--;
			UIToolHeader* JiuGongMateril = (UIToolHeader*)m_ui.materialList->getChildByName("JiuGong"+TO_STR(idx));
			//材料位保存对应炉子信息
			JiuGongMateril->setNumEx(m_chipVec.at(idx).amount);
			setGetDanCount(m_chipVec.at(idx).id,true,1);

			//设置炉子位置材料
			if (m_chipVec.at(idx).idx < 0)
			{
				//炉位保存对应材料信息
				m_LuMaterilVec[findIdx].id = m_chipVec.at(idx).id;
				m_LuMaterilVec[findIdx].amount++;
				m_LuMaterilVec[findIdx].idx = idx;

				//材料位保存对应炉子信息
				m_chipVec.at(idx).idx = findIdx;

				//炉子位更新数据
				UIToolHeader* LuMateril = (UIToolHeader*)m_ui.materialItem[findIdx]->getChildByName("LuMateril");
				if (!LuMateril)
				{
					LuMateril = UIToolHeader::create(m_LuMaterilVec[findIdx].id);
					LuMateril->setAnchorPoint(Vec2(0.5,0.5));
					LuMateril->setScale(1.0f);
					LuMateril->showLightEff();
					LuMateril->setPosition(Vec2(m_ui.materialItem[findIdx]->getContentSize().width/2,
						m_ui.materialItem[findIdx]->getContentSize().height/2));
					LuMateril->setTouchEnabled(true);
					m_ui.materialItem[findIdx]->addChild(LuMateril,0,"LuMateril");

					auto callBack = [this,findIdx](Ref* sender, Widget::TouchEventType type)->void
					{
						if (type == Widget::TouchEventType::BEGAN)
						{
							SoundUtils::playSoundEffect("dianji");
						}
						else if (type == Widget::TouchEventType::ENDED)
						{
							setGetDanCount(m_LuMaterilVec[findIdx].id,false,m_LuMaterilVec[findIdx].amount);

							//材料位还原
							int chipIdx = m_LuMaterilVec[findIdx].idx;
							m_chipVec.at(chipIdx).idx = -1;
							m_chipVec.at(chipIdx).amount = m_chipVec.at(chipIdx).amount + m_LuMaterilVec[findIdx].amount;
							UIToolHeader* BaGuaMateril = (UIToolHeader*)m_ui.materialList->getChildByName("JiuGong"+TO_STR(chipIdx));
							//材料位保存对应炉子信息
							BaGuaMateril->setNumEx(m_chipVec.at(chipIdx).amount);

							//炉位清空
							m_LuMaterilVec[findIdx].id = 0;
							m_LuMaterilVec[findIdx].amount = 0;
							m_LuMaterilVec[findIdx].idx = -1;

							m_deleteItem = (Widget*)sender;
							Director::getInstance()->getScheduler()->schedule(schedule_selector(RefineryLayer::updateNextFram), this, 0, false);
						}
					};

					LuMateril->addTouchEventListener(callBack);
				}

				LuMateril->setNumEx(m_LuMaterilVec[findIdx].amount);
			}
			else
			{
				//炉位保存对应材料信息
				int findIdx = m_chipVec.at(idx).idx;
				m_LuMaterilVec[findIdx].amount++;

				//炉子位更新数据
				UIToolHeader* LuMateril = (UIToolHeader*)m_ui.materialItem[findIdx]->getChildByName("LuMateril");
				LuMateril->setNumEx(m_LuMaterilVec[findIdx].amount);
			}
		}
		break;
	default:
		break;
	}
}

void RefineryLayer::setGetDanCount(int id,bool isAdd,int amout)
{
	DataTable* dataSmelter = DataManager::getInstance()->getSmelter();
	multimap<int, RowData>&  TableData =  dataSmelter->getTableData();

	RowData *itemData = DataManager::getInstance()->searchToolById(id);
	if (!itemData)
	{
		return;
	}

	for (auto iter = TableData.begin(); iter != TableData.end(); iter++)
	{
		if (((*iter).second).getIntData("itemTyp") == itemData->getIntData("itemTyp") &&
			((*iter).second).getIntData("itemQua") == itemData->getIntData("itemQua"))
		{
			if (((*iter).second).getIntData("type") == 1) //八卦炉
			{
				if (isAdd)
				{
					m_danCount = m_danCount + amout*((*iter).second).getIntData("hunTiandan");
				}
				else
				{
					m_danCount = m_danCount - amout*((*iter).second).getIntData("hunTiandan");
				}
			}
			else if(((*iter).second).getIntData("type") == 2)
			{
				if (isAdd)
				{
					m_danCount = m_danCount + amout*((*iter).second).getIntData("jiuZhuandan");
				}
				else
				{
					m_danCount = m_danCount - amout*((*iter).second).getIntData("jiuZhuandan");
				}
			}

			break;
		}
	}

	//m_danCount = max(0,m_danCount);
	m_ui.getAmount->setString(TO_STR(m_danCount));
	setTextAddStroke(m_ui.getAmount,Color3B(0xb3,0x22,0x05),2);
}

void RefineryLayer::updateNextFram(float ft)
{
	if (m_deleteItem)
	{
		m_deleteItem->removeFromParentAndCleanup(true);
	}
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(RefineryLayer::updateNextFram), this);
}

void RefineryLayer::CalculationChip()
{
	m_chipVec.clear();
	const TOOL_INFO_MAP& toolList = ToolModel::getInstance()->GetAllToolInfo();
	if (toolList.size() == 0)
	{
		return;
	}

	for(auto iter=toolList.begin(); iter != toolList.end(); iter++)
	{
		RowData *pRowData = DataManager::getInstance()->searchToolById(iter->second.ntemplateId);
		if (pRowData == nullptr)
		{
			continue;
		}

		//碎片分类
		int nItemType = iter->second.nItemType;
		if (nItemType == TOOL_ITEM_TYPE_HERO_CHIP)
		{
			int maxSta = iter->second.nMaxSta;//maxSta 占用一个格子空间可放置的道具数量
			if (iter->second.nstack > maxSta)
			{
				int nCount = iter->second.nstack;
				while (nCount > maxSta)//堆叠数量超过最大限制要增加显示
				{
					REFINERY_NODE refineryItem;
					refineryItem.id = iter->second.ntemplateId;
					refineryItem.amount = maxSta;
					refineryItem.idx = -1;
					m_chipVec.push_back( refineryItem );

					nCount-=maxSta;	
				}

				if (nCount > 0)
				{
					REFINERY_NODE refineryItem;
					refineryItem.id = iter->second.ntemplateId;
					refineryItem.amount = nCount;
					refineryItem.idx = -1;
					m_chipVec.push_back( refineryItem );
				}
			}
			else
			{
				REFINERY_NODE refineryItem;
				refineryItem.id = iter->second.ntemplateId;
				refineryItem.amount = iter->second.nstack;
				refineryItem.idx = -1;
				m_chipVec.push_back( refineryItem );
			}
		}
	}

	std::sort(m_chipVec.begin(), m_chipVec.end(), 
		[](REFINERY_NODE t1, REFINERY_NODE t2)->bool
	{
		return t1.amount > t2.amount;
	});
}

void RefineryLayer::CalculationOther()
{
	m_otherVec.clear();
	const TOOL_INFO_MAP& toolList = ToolModel::getInstance()->GetAllToolInfo();
	if (toolList.size() == 0)
	{
		return;
	}

	for(auto iter=toolList.begin(); iter != toolList.end(); iter++)
	{
		//其他分类
		int nItemType = iter->second.nItemType;
		if (nItemType == TOOL_ITEM_TYPE_MATERIAL)
		{
			int maxSta = iter->second.nMaxSta;//maxSta 占用一个格子空间可放置的道具数量
			if (iter->second.nstack > maxSta)
			{
				int nCount = iter->second.nstack;
				while (nCount > maxSta)//堆叠数量超过最大限制要增加显示
				{
					REFINERY_NODE refineryItem;
					refineryItem.id = iter->second.ntemplateId;
					refineryItem.amount = maxSta;
					refineryItem.idx = -1;
					m_otherVec.push_back( refineryItem );

					nCount-=maxSta;	
				}

				if (nCount > 0)
				{
					REFINERY_NODE refineryItem;
					refineryItem.id = iter->second.ntemplateId;
					refineryItem.amount = nCount;
					refineryItem.idx = -1;
					m_otherVec.push_back( refineryItem );
				}
			}
			else
			{
				REFINERY_NODE refineryItem;
				refineryItem.id = iter->second.ntemplateId;
				refineryItem.amount = iter->second.nstack;
				refineryItem.idx = -1;
				m_otherVec.push_back( refineryItem );
			}
		}
	}

	std::sort(m_otherVec.begin(), m_otherVec.end(), 
		[](REFINERY_NODE t1, REFINERY_NODE t2)->bool
	{
		RowData *dataT1 = DataManager::getInstance()->searchToolById(t1.id);
		RowData *dataT2 = DataManager::getInstance()->searchToolById(t2.id);

		if (dataT1 && dataT2)
		{
			if (dataT1->getIntData("itemQua") < dataT2->getIntData("itemQua"))
			{
				return true;
			}
			else if (dataT1->getIntData("itemQua") > dataT2->getIntData("itemQua"))
			{
				return false;
			}
			else
			{
				return t1.amount > t2.amount;
			}
		}
	});
}