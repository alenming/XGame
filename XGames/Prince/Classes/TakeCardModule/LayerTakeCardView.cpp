#include "LayerTakeCardView.h"


bool sortHero(Card_View_Node a, Card_View_Node b)
{
	//是否结识
	if (a.sortIdx < b.sortIdx)
	{
		return true;
	}
	else
	{
		return false;
	}
}

LayerTakeCardView::LayerTakeCardView()
{
	m_CoinAll.clear();
	m_CoinItem.clear();
	m_CoinPiece.clear();

	m_GoldAll.clear();
	m_GoldAtk.clear();
	m_GoldDef.clear();
	m_GoldSup.clear();
	m_GoldRush.clear();
	m_customScrollBar1 = nullptr; //滚动条
	m_customScrollBar2 = nullptr; //滚动条
	openTableInfo();
}

LayerTakeCardView::~LayerTakeCardView()
{
	
}

void LayerTakeCardView::onEnter()
{
	ModalLayer::onEnter();
}

void LayerTakeCardView::onExit()
{
	ModalLayer::onExit();
}

LayerTakeCardView* LayerTakeCardView::create(CardViewType type)
{
	LayerTakeCardView *pRet = new LayerTakeCardView();
	if (pRet && pRet->init(type))
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

bool LayerTakeCardView::init(CardViewType type)
{
	m_type = type;
	if (!ModalLayer::init())
	{
		return false;
	}
	initUI();

	switch (m_type)
	{
	case CardViewType_Coin:
		{
			m_ui.Img_Coin->setVisible(true);
			m_ui.Img_Gold->setVisible(false);

			setCoinAll();
		}
		break;
	case CardViewType_Gold:
		{
			m_ui.Img_Coin->setVisible(false);
			m_ui.Img_Gold->setVisible(true);
			setGoldAll();
		}
		break;
	default:
		break;
	}
	return true;
}

void LayerTakeCardView::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// UI处理
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UImall/UImall_preview.ExportJson"));
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	// 将UILayer层加入到当前的场景
	this->addChild(m_ui.pRoot,0);

	m_ui.Img_Coin = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Coin"));
	m_ui.Btn_CloseCoin = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Img_Coin,"Btn_Close"));
	m_ui.Btn_CloseCoin->addTouchEventListener(CC_CALLBACK_2(LayerTakeCardView::onBtnClick,this));
	m_ui.listCoin = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.Img_Coin,"ScrollView_36"));
	ImageView* CoinpHeroListDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Img_Coin,"Image_19"));
	m_customScrollBar1 = CustomScrollBar::create();
	m_customScrollBar1->initScrollBar(m_ui.listCoin->getContentSize().height);
	m_customScrollBar1->setPosition(m_ui.listCoin->getPosition() + Point(-12, 0));
	CoinpHeroListDi->addChild(m_customScrollBar1);
	m_customScrollBar1->setScrollBarLengthPercent(1.0f);
	m_customScrollBar1->setScrollBarPercentY(0.0f);
	m_ui.listCoin->addEventListener(CC_CALLBACK_2(LayerTakeCardView::onScrollViewEvent, this));

	m_ui.allCoin = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.Img_Coin,"CheckBox_All"));
	m_ui.ItemCoin = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.Img_Coin,"CheckBox_Item"));
	m_ui.pieceCoin = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.Img_Coin,"CheckBox_Piece"));
	m_ui.allCoin->addEventListener(CC_CALLBACK_2(LayerTakeCardView::onCheckClick,this));
	m_ui.ItemCoin->addEventListener(CC_CALLBACK_2(LayerTakeCardView::onCheckClick,this));
	m_ui.pieceCoin->addEventListener(CC_CALLBACK_2(LayerTakeCardView::onCheckClick,this));

	m_ui.Img_Gold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Gold"));
	m_ui.Btn_CloseGold = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Img_Gold,"Btn_Close"));
	m_ui.Btn_CloseGold->addTouchEventListener(CC_CALLBACK_2(LayerTakeCardView::onBtnClick,this));
	m_ui.listGold = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.Img_Gold,"ScrollView_35"));
	ImageView* GoldpHeroListDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Img_Gold,"Image_19_0"));
	m_customScrollBar2 = CustomScrollBar::create();
	m_customScrollBar2->initScrollBar(m_ui.listGold->getContentSize().height);
	m_customScrollBar2->setPosition(m_ui.listGold->getPosition() + Point(-12, 0));
	GoldpHeroListDi->addChild(m_customScrollBar2);
	m_customScrollBar2->setScrollBarLengthPercent(1.0f);
	m_customScrollBar2->setScrollBarPercentY(0.0f);
	m_ui.listGold->addEventListener(CC_CALLBACK_2(LayerTakeCardView::onScrollViewEvent, this));

	m_ui.allGold = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.Img_Gold,"CheckBox_All"));
	m_ui.atkGold = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.Img_Gold,"CheckBox_Attack"));
	m_ui.defGold = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.Img_Gold,"CheckBox_Defense"));
	m_ui.supGold = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.Img_Gold,"CheckBox_Support"));
	m_ui.rushGold = dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.Img_Gold,"CheckBox_Rush"));
	m_ui.allGold->addEventListener(CC_CALLBACK_2(LayerTakeCardView::onCheckClick,this));
	m_ui.atkGold->addEventListener(CC_CALLBACK_2(LayerTakeCardView::onCheckClick,this));
	m_ui.defGold->addEventListener(CC_CALLBACK_2(LayerTakeCardView::onCheckClick,this));
	m_ui.supGold->addEventListener(CC_CALLBACK_2(LayerTakeCardView::onCheckClick,this));
	m_ui.rushGold->addEventListener(CC_CALLBACK_2(LayerTakeCardView::onCheckClick,this));
}

void LayerTakeCardView::openTableInfo()
{
	//信息表
	multimap<int, RowData>& DToperatMap = DataManager::getInstance()->getDTShopCardView()->getTableData();
	for(auto iter = DToperatMap.begin(); iter != DToperatMap.end(); iter++)
	{
		RowData* rowData = &(iter->second);
		if (rowData->getIntData("type") == 1) //地煞封印
		{
			Card_View_Node item;
			item.id = rowData->getIntData("itemID");
			item.sortIdx = rowData->getIntData("sort");

			if (rowData->getIntData("page") == 1) //道具
			{
				m_CoinItem.push_back(item);
			}
			else if (rowData->getIntData("page") == 2) //道具
			{
				m_CoinPiece.push_back(item);
			}

			m_CoinAll.push_back(item);
		}
		else if (rowData->getIntData("type") == 2) //天罡封印
		{
			Card_View_Node item;
			item.id = rowData->getIntData("itemID");
			item.sortIdx = rowData->getIntData("sort");
			m_GoldAll.push_back(item);

			RowData* heroData = DataManager::getInstance()->searchCharacterById(item.id);
			if(heroData)
			{
				if (heroData->getIntData("jobType") == 1) //防御
				{
					m_GoldDef.push_back(item);
				}
				else if (heroData->getIntData("jobType") == 2) //攻击
				{
					m_GoldAtk.push_back(item);
				}
				else if (heroData->getIntData("jobType") == 3) //辅助
				{
					m_GoldSup.push_back(item);
				}
				else if (heroData->getIntData("jobType") == 4) //先手
				{
					m_GoldRush.push_back(item);
				}
			}
		}
		else if (rowData->getIntData("type") == 3) //神秘封印
		{
		}
	}

	sort(m_CoinAll.begin(),m_CoinAll.end(),sortHero);//排序
	sort(m_CoinItem.begin(),m_CoinItem.end(),sortHero);//排序
	sort(m_CoinPiece.begin(),m_CoinPiece.end(),sortHero);//排序
	sort(m_GoldAll.begin(),m_GoldAll.end(),sortHero);//排序
	sort(m_GoldAtk.begin(),m_GoldAtk.end(),sortHero);//排序
	sort(m_GoldDef.begin(),m_GoldDef.end(),sortHero);//排序
	sort(m_GoldSup.begin(),m_GoldSup.end(),sortHero);//排序
	sort(m_GoldRush.begin(),m_GoldRush.end(),sortHero);//排序
}

void LayerTakeCardView::onBtnClick(Ref* ref, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		dispose();
	}
}

void LayerTakeCardView::onCheckClick(Ref* sender,CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED)
	{
		SoundUtils::playSoundEffect("dianji");
		if (sender == m_ui.allCoin)
		{
			setCoinAll();
		}
		else if (sender == m_ui.ItemCoin)
		{
			setCoinItem();
		}
		else if (sender == m_ui.pieceCoin)
		{
			setCoinPiece();
		}
		else if (sender == m_ui.allGold)
		{
			setGoldAll();
		}
		else if (sender == m_ui.atkGold)
		{
			setGoldAtk();
		}
		else if (sender == m_ui.defGold)
		{
			setGoldDef();
		}
		else if (sender == m_ui.supGold)
		{
			setGoldSup();
		}
		else if (sender == m_ui.rushGold)
		{
			setGoldRush();
		}
	}
}

void LayerTakeCardView::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	switch(type)
	{
	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			if (pSender == m_ui.listCoin)
			{
				Size size = m_ui.listCoin->getInnerContainerSize();
				Vec2 vec = m_ui.listCoin->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.listCoin->getContentSize().height);

				m_customScrollBar1->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			else if (pSender == m_ui.listGold)
			{
				Size size = m_ui.listGold->getInnerContainerSize();
				Vec2 vec = m_ui.listGold->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.listGold->getContentSize().height);

				m_customScrollBar2->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
		}

		break;
	default:
		break;
	}
}

void LayerTakeCardView::reSetCoinCheckBox()
{
	m_ui.allCoin->setSelectedState(false);
	m_ui.allCoin->setTouchEnabled(true);
	m_ui.ItemCoin->setSelectedState(false);
	m_ui.ItemCoin->setTouchEnabled(true);
	m_ui.pieceCoin->setSelectedState(false);
	m_ui.pieceCoin->setTouchEnabled(true);
}

void LayerTakeCardView::setCoinAll()
{
	reSetCoinCheckBox();
	m_ui.allCoin->setSelectedState(true);
	m_ui.allCoin->setTouchEnabled(false);
	setCoinList(m_CoinAll);
}

void LayerTakeCardView::setCoinItem()
{
	reSetCoinCheckBox();
	m_ui.ItemCoin->setSelectedState(true);
	m_ui.ItemCoin->setTouchEnabled(false);
	setCoinList(m_CoinItem);
}

void LayerTakeCardView::setCoinPiece()
{
	reSetCoinCheckBox();
	m_ui.pieceCoin->setSelectedState(true);
	m_ui.pieceCoin->setTouchEnabled(false);
	setCoinList(m_CoinPiece);
}

void LayerTakeCardView::reSetGoldCheckBox()
{
	m_ui.allGold->setSelectedState(false);
	m_ui.allGold->setTouchEnabled(true);
	m_ui.atkGold->setSelectedState(false);
	m_ui.atkGold->setTouchEnabled(true);
	m_ui.defGold->setSelectedState(false);
	m_ui.defGold->setTouchEnabled(true);
	m_ui.supGold->setSelectedState(false);
	m_ui.supGold->setTouchEnabled(true);
	m_ui.rushGold->setSelectedState(false);
	m_ui.rushGold->setTouchEnabled(true);
}

void LayerTakeCardView::setGoldAll()
{
	reSetGoldCheckBox();
	m_ui.allGold->setSelectedState(true);
	m_ui.allGold->setTouchEnabled(false);
	setGoldList(m_GoldAll);
}

void LayerTakeCardView::setGoldAtk()
{
	reSetGoldCheckBox();
	m_ui.atkGold->setSelectedState(true);
	m_ui.atkGold->setTouchEnabled(false);
	setGoldList(m_GoldAtk);
}

void LayerTakeCardView::setGoldDef()
{
	reSetGoldCheckBox();
	m_ui.defGold->setSelectedState(true);
	m_ui.defGold->setTouchEnabled(false);
	setGoldList(m_GoldDef);
}

void LayerTakeCardView::setGoldSup()
{
	reSetGoldCheckBox();
	m_ui.supGold->setSelectedState(true);
	m_ui.supGold->setTouchEnabled(false);
	setGoldList(m_GoldSup);
}

void LayerTakeCardView::setGoldRush()
{
	reSetGoldCheckBox();
	m_ui.rushGold->setSelectedState(true);
	m_ui.rushGold->setTouchEnabled(false);
	setGoldList(m_GoldRush);
}

void LayerTakeCardView::setCoinList(vector<Card_View_Node> itemList)
{
	//添加奖励面板控件
	int row = ceil(itemList.size()/5.0f);
	int scrollViewHeight = row * 155+30;
	scrollViewHeight = scrollViewHeight >  m_ui.listCoin->getContentSize().height ? scrollViewHeight : m_ui.listCoin->getContentSize().height;
	int scrollViewWidth = m_ui.listCoin->getInnerContainerSize().width;
	m_ui.listCoin->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	m_ui.listCoin->removeAllChildren();
	for (int i = 0; i < itemList.size(); i++)
	{
		int rowNumber = i/5;
		int colNumber = i%5;

		UIToolHeader* icon = NULL;
		icon = UIToolHeader::create(itemList.at(i).id);
		icon->setTouchEnabled(true);
		icon->setTipsEnabled(true);
		icon->setAnchorPoint(Vec2(0,0));
		icon->setPosition(Vec2(colNumber*180 + 45, scrollViewHeight-(rowNumber+1)*155+30 ));

		RowData* rowData = DataManager::getInstance()->searchToolById(itemList.at(i).id);
		Text *name = Text::create(rowData->getStringData("itemName"),FONT_FZZHENGHEI,24);
		name->setColor(Color3B(0xeb,0xad,0x52));
		name->setPosition(Vec2(icon->getContentSize().width/2,-20));
		icon->addChild(name);

		m_ui.listCoin->addChild(icon,0,"icon"+TO_STR(i));
	}
	m_customScrollBar1->setScrollBarLengthPercent(
		m_ui.listCoin->getContentSize().height / m_ui.listCoin->getInnerContainerSize().height);
	m_customScrollBar1->setScrollBarPercentY(0.0f);
	m_ui.listCoin->jumpToTop();
}

void LayerTakeCardView::setGoldList(vector<Card_View_Node> itemList)
{
	//添加奖励面板控件
	int row = ceil(itemList.size()/5.0f);
	int scrollViewHeight = row * 155+30;
	scrollViewHeight = scrollViewHeight >  m_ui.listGold->getContentSize().height ? scrollViewHeight : m_ui.listGold->getContentSize().height;
	int scrollViewWidth = m_ui.listGold->getInnerContainerSize().width;
	m_ui.listGold->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	m_ui.listGold->removeAllChildren();
	for (int i = 0; i < itemList.size(); i++)
	{
		int rowNumber = i/5;
		int colNumber = i%5;

		UIToolHeader* icon = NULL;
		icon = UIToolHeader::create(itemList.at(i).id);
		icon->setNumEx(1);
		icon->setTipsEnabled(true);
		icon->setTouchEnabled(true);
		icon->setAnchorPoint(Vec2(0,0));
		icon->setPosition(Vec2(colNumber*180 + 45, scrollViewHeight-(rowNumber+1)*155+30 ));

		RowData* rowData = DataManager::getInstance()->searchToolById(itemList.at(i).id);
		Text *name = Text::create(rowData->getStringData("itemName"),FONT_FZZHENGHEI,24);
		name->setColor(Color3B(0xeb,0xad,0x52));
		name->setPosition(Vec2(icon->getContentSize().width/2,-20));
		icon->addChild(name);

		m_ui.listGold->addChild(icon,0,"icon"+TO_STR(i));
	}
	m_customScrollBar2->setScrollBarLengthPercent(
		m_ui.listGold->getContentSize().height / m_ui.listGold->getInnerContainerSize().height);
	m_customScrollBar2->setScrollBarPercentY(0.0f);
	m_ui.listGold->jumpToTop();
}

