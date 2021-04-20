#include "MysteryShopLayer.h"
#include "Common/LocalData.h"
#include "MysteryShopClick.h"
#include "Temp/CustomGoldTips.h"
#include "MainModule/MainCityScene.h"

MysteryShopLayer::MysteryShopLayer():_shopType(ST_MysteryShop)
{
	MysteryShopModel::getInstance()->addObserver(this);
}

MysteryShopLayer::~MysteryShopLayer()
{
	MysteryShopModel::getInstance()->removeObserver(this);
}

MysteryShopLayer* MysteryShopLayer::create(SHOP_TYPE type ) 
{ 
	MysteryShopLayer *pRet = new MysteryShopLayer(); 
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

bool MysteryShopLayer::init(SHOP_TYPE type)
{
	if (!Layer::init())
	{
		return false;
	}

	_shopType = type;

	MysteryShopModel::getInstance()->setShopType(type);

	initUI();

	return true;
}

void MysteryShopLayer::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_Son.m_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UI_SecretStore/UI_SecretStore_store.ExportJson"));
	m_Son.m_Root->setSize(size);
	m_Son.m_Root->setPosition(pos);
	m_Son.m_Root->setBackGroundImage("Image/Bg/img_bg_quanping.jpg",ui::Widget::TextureResType::LOCAL);
	this->addChild(m_Son.m_Root);

	m_Son.Image_ScrollView = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_Son.m_Root,"Image_ScrollView"));

	m_Son.Btn_Refresh =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_Son.m_Root,"Button_fresh"));
	m_Son.Lab_Cost =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.Btn_Refresh,"text_Flush"));

	m_Son.Lab_Count =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.m_Root,"Lab_Count"));
	m_Son.Lab_Count_Desc =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.m_Root,"Lab_Count_Desc"));
	m_Son.Lab_Time =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.m_Root,"Lab_Time"));
	m_Son.Lab_Time_Desc =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.m_Root,"Lab_Time_Desc"));
	m_Son.ScrollView_Content =  dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_Son.m_Root,"ScrollView_Content"));

	m_Son.ScrollView_Content->addEventListener(CC_CALLBACK_2(MysteryShopLayer::onScrollViewEvent, this));

	m_Son.scrollBar = CustomScrollBar::create();
	m_Son.scrollBar->setVisible(false);
	m_Son.scrollBar->setPosition(Vec2(5,5));
	m_Son.Image_ScrollView->addChild(m_Son.scrollBar);
	m_Son.scrollBar->initScrollBar(m_Son.ScrollView_Content->getContentSize().height+10);

	m_Son.Btn_Refresh->addTouchEventListener(CC_CALLBACK_2(MysteryShopLayer::onBtnClickRefresh, this));


	// 刷新次数;
	m_Son.Lab_Cost->setFontName(FONT_FZZHENGHEI);
	m_Son.Lab_Cost->setString("");

	m_Son.Lab_Count_Desc->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_Son.Lab_Count_Desc, Color3B(0x79, 0x03, 0x03), 2);

	m_Son.Lab_Count->setFontName(FONT_FZZHENGHEI);
	m_Son.Lab_Count->setString("");
	setTextAddStroke(m_Son.Lab_Count, Color3B(0x79, 0x03, 0x03), 2);
	// 倒计时;
	m_Son.Lab_Time->setFontName(FONT_FZZHENGHEI);
	m_Son.Lab_Time->setString("");

	m_Son.Lab_Time_Desc->setFontName(FONT_FZZHENGHEI);

	
	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			Director::getInstance()->popToRootScene();
			Director::getInstance()->replaceScene(MainCityScene::create());
		}  
	};
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_MYSTERY_SHOP);
	this->addChild(_layerCommHeadInfo,99);

	Director::getInstance()->getScheduler()->schedule(schedule_selector(MysteryShopLayer::updateNextFram), this, 0, false);
}

void MysteryShopLayer::onEnter()
{
	Layer::onEnter();
	
}

void MysteryShopLayer::updateNextFram(float ft)
{
	MysteryShopController::getInstance()->handleShopInfo(ST_MysteryShop);
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(MysteryShopLayer::updateNextFram), this);
}

void MysteryShopLayer::updateNextFramRemove(float ft)
{
	Director::getInstance()->replaceScene(MainCityScene::create());
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(MysteryShopLayer::updateNextFramRemove), this);
}

void MysteryShopLayer::onExit()
{
	Layer::onExit();
}



void MysteryShopLayer::onBtnClickRefresh( Ref* ref, cocos2d::ui::Widget::TouchEventType type)
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		//刷新
		int nRet = MysteryShopController::getInstance()->handleRefresh();
		if (nRet != 0)
		{
			if (nRet == -1)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20191));
			}
			else if (nRet == -2)
			{
				CustomGoldTips *_tips = CustomGoldTips::create();
				if (_tips)
				{
					Director::getInstance()->getRunningScene()->addChild(_tips);
				}
			}
		}
	}
}

void MysteryShopLayer::onItemClicked( int nIndex )
{
	if (nIndex < 0)
	{
		return;
	}

	// 音效;
	SoundUtils::playSoundEffect("dianji");

	int nRet = MysteryShopController::getInstance()->handleBuy(nIndex);
	if (nRet < 0)
	{
		if (nRet == -3)
		{
			//string str = getStrTypeName(1);
			//str.append("不足");
			//CustomTips::show(str);
		}
		else if (nRet == -2)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20193));
		}
		else if (nRet == -4)
		{
			CustomTips::show(DataManager::getInstance()->searchCommonTexdtById(20247));//碎片不足
		}
		else if (nRet == -5) //金币不足
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
			}
		}
		else if (nRet == -6) //银币不足
		{
			LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
			Director::getInstance()->getRunningScene()->addChild(buyEnergy);
		}
	}
}


void MysteryShopLayer::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* observerParam = (ObserverParam*)pData;
	switch (observerParam->id)
	{
	case SHOP_EVENT_TYEP_ALL_ITEM:
		{
			//所有商品刷新显示
			refreshAllItemOther();
		}
		break;

	case SHOP_EVENT_TYEP_SINGLE_ITEM:
		{

			refreshSingleItem((ShopItemParam*)(observerParam->updateData));
		}
		break;

	case SHOP_EVENT_TYEP_COUNT_DOWN:
		{
			//倒计时
			m_Son.Lab_Time->setString(formatTime(MysteryShopModel::getInstance()->getDaoTime()));
		}
		break;

	case SHOP_EVENT_TYEP_PUBLIC:
		{
				ostringstream oss;
				//剩余刷新次数
				int num = ((vector<int>*)(observerParam->updateData))->at(0);
				m_Son.Lab_Count->setString(TO_STR(num));
				setTextAddStroke(m_Son.Lab_Count , Color3B(0x79, 0x03, 0x03), 2);

				num = ((vector<int>*)(observerParam->updateData))->at(1);
				m_Son.Lab_Cost->setString(TO_STR(num));
				//setTextAddStroke(m_Son.Lab_Cost , Color3B(0x79, 0x03, 0x03), 2);

				for (auto it : m_vcAllItems)
				{
					it->updatePrice();
				}
				if (MysteryShopModel::getInstance()->getShopSuccessful())
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20194));
					MysteryShopModel::getInstance()->setShopSuccessful(false);
				}	
				if (MysteryShopModel::getInstance()->getFreshSuccessful())
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20248));
				}
				MysteryShopModel::getInstance()->setFreshSuccessful(false);
		}
		break;
	case MYSTERY_SHOW:
		{
			//时间到了如果停留在此界面移除他
			Director::getInstance()->getScheduler()->schedule(schedule_selector(MysteryShopLayer::updateNextFramRemove), this, 0, false);
		}
		break;
	default:
		break;
	}
}

void MysteryShopLayer::refreshAllItemOther()
{
	vector<ShopItemParam>* vcAllItem = MysteryShopModel::getInstance()->getm_vcAllItems();
	if (vcAllItem->size() == 0)
	{
		return;
	}
	// 清除所有;
	m_vcAllItems.clear();
	m_Son.ScrollView_Content->removeAllChildren();

	// 重新计算容器尺寸;
	int nCount = vcAllItem->size();
	int nRow = nCount/4 + ((nCount%4 == 0) ? 0 : 1);
	if (nRow ==1)
	{
		nRow = 2;
	}
	int nTotalHeight = nRow * 250.0f;
	m_Son.ScrollView_Content->setInnerContainerSize(Size(m_Son.ScrollView_Content->getContentSize().width, nTotalHeight));
	float fStartX = m_Son.ScrollView_Content->getContentSize().width / 8.0f;
	float fStartY = nTotalHeight - 250.0f/2.0f;

	for (int i = 0; i < nCount; ++i)
	{
		MysteryShopItem* _item = MysteryShopItem::create(vcAllItem->at(i), this);
		_item->setPosition(Vec2(fStartX +m_Son.ScrollView_Content->getContentSize().width/4.0f * (i%4),
			fStartY - 250.0f * (i/4)));

		std::string str = "item_"+TO_STR(i);
		_item->setName(str);
		m_Son.ScrollView_Content->addChild(_item);
		m_vcAllItems.push_back(_item);
	}

	// 刷新滚动条;
	if (m_Son.scrollBar != nullptr)
	{
		m_Son.scrollBar->setVisible(nRow > 2);
		if (m_Son.scrollBar->isVisible())
		{
			m_Son.scrollBar->setScrollBarLengthPercent(m_Son.ScrollView_Content->getContentSize().height / m_Son.ScrollView_Content->getInnerContainerSize().height);
			m_Son.scrollBar->setScrollBarPercentY(0.0f);
		}
	}
}

void MysteryShopLayer::refreshSingleItem( const ShopItemParam* item )
{

	std::string oss = "item_"+TO_STR(item->nIndex);
	MysteryShopItem* _item = dynamic_cast<MysteryShopItem*>(m_Son.ScrollView_Content->getChildByName(oss));
	assert(_item != nullptr);
	_item->updateState(item->bEnabled);

}

std::string MysteryShopLayer::checkItemScoreImg(ShopItemParam Param)
{
	string str = "";

	switch (Param.moneyType)
	{
	case MONEY_GOLD:
		{
			str.append("Image/Icon/global/img_yuanbao.png");
		}
		break;
	case MONEY_COIN:
		{
			str.append("Image/Icon/global/img_yinbi.png");
		}
		break;
	case MONEY_COMMON:
		{
			switch (Param.nShopType)
			{
			case ST_RONGYU:
				str.append("Image/UIShopexc/Img/img_icon_rongyu.png");
				break;
			case ST_JIFENG:
				str.append("Image/UIShopexc/Img/img_shilianjifen_icon.png");
				break;
			case ST_SEHGNWANG:
				str.append("Image/UIShopexc/Img/img_shengwaihuobi.png");
				break;
			case ST_REFINE_8:
				str.append("Image/UIShopexc/Img/img_huntiandan1.png");
				break;
			case ST_REFINE_9:
				str.append("Image/UIShopexc/Img/img_jiuzhuandan1.png");
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	} 

	return str;
}

void MysteryShopLayer::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	if (pSender == m_Son.ScrollView_Content)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Size size = m_Son.ScrollView_Content->getInnerContainerSize();
				Vec2 vec = m_Son.ScrollView_Content->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height -m_Son.ScrollView_Content->getContentSize().height);
				m_Son.scrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			break;

		default:
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

MysteryShopItem::MysteryShopItem()
	: m_parent(nullptr)
	, m_nIndex(-1)
{
}

MysteryShopItem::~MysteryShopItem()
{

}

MysteryShopItem* MysteryShopItem::create( ShopItemParam param, MysteryShopLayer* parent )
{
	MysteryShopItem* _item = new MysteryShopItem;
	if (nullptr != _item && _item->init(param, parent))
	{
		_item->autorelease();
		return _item;
	}

	CC_SAFE_DELETE(_item);
	return nullptr;
}

bool MysteryShopItem::init( ShopItemParam param, MysteryShopLayer* parent )
{
	if (!Layout::init())
	{
		return false;
	}

	m_parent = parent;
	_param = param;
	m_nIndex = param.nIndex;

	initUI();
	if (_param.nShopType != ST_CUILIAN)
	{	
		updateState(param.bEnabled);
	}
	updatePrice();
	return true;
}

void MysteryShopItem::initUI( )
{
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIShopexc2/UIShopitem.ExportJson"));
	this->addChild(m_ui.pRoot);

	m_ui.btnItem = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Item"));
	m_ui.btnItem->addTouchEventListener(CC_CALLBACK_2(MysteryShopItem::onBtnClick, this));


	//卖完
	m_ui.Img_SellOut = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_SellOut"));
	m_ui.Img_SellOut->setVisible(false);

	// 物品;
	m_ui.imgItem = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Item"));
	UIToolHeader* _item = UIToolHeader::create(_param.nId);
	if (_item)
	{
		_item->setPosition(m_ui.imgItem->getPosition());
		_item->setScale(0.87f);
		_item->setNumEx(_param.nCount);
		_item->showLightEff();
		m_ui.imgItem->getParent()->addChild(_item);
		m_ui.imgItem->removeFromParent();
	}
	// 限购次数;
	//m_ui.imgLimit = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Limit"));
	//m_ui.imgLimit->setVisible(param.nLimitBuyCount >= 1);

	// 名称;
	m_ui.txtName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Name"));
	m_ui.txtName->setFontName(FONT_FZZHENGHEI);
	m_ui.txtName->setString(_param.strName);
	setTextAddStroke(m_ui.txtName, Color3B(0x79, 0x03, 0x03), 2);

	// 消耗货币;
	m_ui.imgScore = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Score"));

	m_ui.imgScore->loadTexture(MysteryShopLayer::checkItemScoreImg(_param));


	if (_param.nShopType == ST_ZAHUO)
	{
		if (_param.moneyType == MONEY_COIN || _param.moneyType == MONEY_GOLD)
		{
			m_ui.imgScore->setScale(1.2f);
		}
	}

	m_ui.txtScore = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Score"));
	m_ui.txtScore->setFontName(FONT_FZZHENGHEI);
	m_ui.txtScore->setString(_TO_STR(_param.nPrice));
}

void MysteryShopItem::onBtnClick( Ref* ref, Widget::TouchEventType type )
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		auto _shopInfo = MysteryShopClick::create(_param, m_parent );

		if (_shopInfo )
		{
			Director::getInstance()->getRunningScene()->addChild(_shopInfo ,989);
		}
		ActionCreator::runCommDlgAction(_shopInfo->getImagviewDi());
	}
}

void MysteryShopItem::updateState( bool bEnabled )
{
	if (!bEnabled)
	{
		this->m_ui.Img_SellOut->setVisible(true);
	}
	m_ui.btnItem->setEnabled(bEnabled);
}

void MysteryShopItem::updatePrice()
{
	// 售罄的不用管;
	if (!m_ui.btnItem->isEnabled())
	{
		return;
	}
}

