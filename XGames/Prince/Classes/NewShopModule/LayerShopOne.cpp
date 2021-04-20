#include "LayerShopOne.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "ToolModule/LayerTool.h"
#include "ToolModule/ToolModel.h"
#include "Widget/ActionCreator.h"
#include "GuideCenter/GuideManager.h"
#include "Temp/CustomGoldTips.h"
#include "CommonBuy/LayerCommonBuy.h"
#include "ShopClickLayer.h"
#include "ShopModel.h"

LayerShopOne::LayerShopOne(void)
	: m_type(SHOP_TYPE::ST_NONE)
	, m_bFirstEnter(true)
{
	ShopModel::getInstance()->addObserver(this);
	m_vcAllItems.clear();
	m_vcAllItemsCuilian.clear();
}


LayerShopOne::~LayerShopOne(void)
{
	ShopModel::getInstance()->removeObserver(this);
}

LayerShopOne* LayerShopOne::create( SHOP_TYPE type )
{
	LayerShopOne* _layer = new LayerShopOne;
	if (nullptr != _layer && _layer->init(type))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

bool LayerShopOne::init(SHOP_TYPE type)
{
	if (!Layer::init())
	{
		return false;
	}

	m_type = type;
	m_vcAllItems.clear();
	m_vcAllItemsCuilian.clear();

	ShopModel::getInstance()->setShopType(m_type);
	if (m_type ==ST_CUILIAN)
	{
		initUICuiLian();
		ShopController::getInstance()->uadataCuiLianShopInfo();
	}
	else
	{
		initUINoCuilian();
		ShopController::getInstance()->handleShopInfo(m_type);
	}


	return true;
}
void LayerShopOne::onEnter()
{
	Layer::onEnter();
}

void LayerShopOne::initUINoCuilian()
{

	m_Son.Pnl_Root = ShopModel::getInstance()->m_root;
	m_Son.Pnl_Root->setVisible(true);
	m_Son.Lab_Cost =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.Pnl_Root,"Lab_Cost"));
	m_Son.Lab_Count =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.Pnl_Root,"Lab_Count"));
	m_Son.Lab_Count_Desc =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.Pnl_Root,"Lab_Count_Desc"));
	m_Son.Lab_Time =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.Pnl_Root,"Lab_Time"));
	m_Son.Lab_Time_Desc =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.Pnl_Root,"Lab_Time_Desc"));
	m_Son.ScrollView_Content =  dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_Son.Pnl_Root,"ScrollView_Content"));
	
	m_Son.Img_di3 =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_Son.Pnl_Root,"Img_di3"));
	m_Son.Img_di2 =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_Son.Pnl_Root,"Img_di2"));
	m_Son.Lab_Score =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_Son.Img_di2,"Lab_Score"));
	m_Son.Img_Score =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_Son.Img_di2,"Img_Score"));
	m_Son.Btn_Refresh =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_Son.Pnl_Root,"Btn_Refresh"));


	if (m_type == ST_ZAHUO)
	{
		m_Son.Img_di2->setVisible(false);
	}
	else
	{
		m_Son.Img_di2->setVisible(true);
		m_Son.Img_Score->loadTexture(checkScoreImg(m_type));
		m_Son.Lab_Score->setFontName(FONT_FZZHENGHEI);
		m_Son.Lab_Score->setString("");
	}
	
	m_Son.ScrollView_Content->addEventListener(CC_CALLBACK_2(LayerShopOne::onScrollViewEvent, this));

	m_Son.Img_di3->removeAllChildren();

	m_Son.scrollBar = CustomScrollBar::create();
	m_Son.scrollBar->setVisible(false);
	m_Son.scrollBar->setPosition(Vec2(5,5));
	m_Son.Img_di3->addChild(m_Son.scrollBar);
	m_Son.scrollBar->initScrollBar(m_Son.ScrollView_Content->getContentSize().height+10);

	m_Son.Btn_Refresh->addTouchEventListener(CC_CALLBACK_2(LayerShopOne::onBtnClick, this, Btn_Refresh));

	checkRefreshBtn();
	
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
}

void LayerShopOne::initUICuiLian()
{
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIShopexc2/UIShopexc.ExportJson"));
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	this->addChild(m_ui.pRoot);

	m_ui.Img_Bg =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Bg"));
	// 标题;
	m_ui.imgTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Title"));

	// 关闭按钮;
	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Close"));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerShopOne::onBtnClick, this, Btn_Close));

	// 货币总量;
	m_ui.imgScore = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Score"));

	m_ui.txtScore = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Score"));
	m_ui.txtScore->setFontName(FONT_FZZHENGHEI);
	m_ui.txtScore->setString("");

	// 物品;
	m_ui.imgContentBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Content_Bg"));
	m_ui.scrollviewContent = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, "ScrollView_Content"));
	m_ui.scrollviewContent->addEventListener(CC_CALLBACK_2(LayerShopOne::onScrollViewEvent, this));
	m_ui.scrollBar = CustomScrollBar::create();
	m_ui.imgContentBg->addChild(m_ui.scrollBar);
	m_ui.scrollBar->setPosition(Vec2(35.0f, 35.0f));
	m_ui.scrollBar->initScrollBar(m_ui.scrollviewContent->getContentSize().height);

}


void LayerShopOne::onBtnClick( Ref* ref, cocos2d::ui::Widget::TouchEventType type, int nWidgetName )
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switch (nWidgetName)
		{
		case Btn_Close:
			{
				this->scheduleUpdate();
			}
			break;

		case Btn_Refresh:
			{
				int nRet = ShopController::getInstance()->handleRefresh();
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
			break;

		default:
			break;
		}
	}
}

void LayerShopOne::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
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
	if (pSender ==  m_ui.scrollviewContent)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.scrollviewContent->getInnerContainerSize();
				Vec2 vec  = m_ui.scrollviewContent->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height -m_ui.scrollviewContent->getContentSize().height);
				m_ui.scrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			break;

		default:
			break;
		}
	}
}

void LayerShopOne::onItemClicked( int nIndex )
{
	if (nIndex < 0)
	{
		return;
	}

	// 音效;
	SoundUtils::playSoundEffect("dianji");

	int nRet = ShopController::getInstance()->handleBuy(nIndex);
	if (nRet < 0)
	{
		if (nRet == -3)
		{
			string str = getStrTypeName(1);
			str.append(DataManager::getInstance()->searchCommonTexdtById(20192));
			CustomTips::showUtf8(str);
		}
		else if (nRet == -2)
		{
			CustomTips::show(DataManager::getInstance()->searchCommonTexdtById(20193));
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

void LayerShopOne::update( float delta )
{
	this->unscheduleUpdate();
	ShopModel::getInstance()->setShopType(ST_NONE);
	this->removeFromParent();
	//this->setVisible(false);
}

void LayerShopOne::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* observerParam = (ObserverParam*)pData;
	switch (observerParam->id)
	{
	case SHOP_EVENT_TYEP_CUILIAN:
		{
			if (ShopModel::getInstance()->getShopType() == ST_CUILIAN)
			{
				refreshAllItem();
				break;
			}
		}
	case SHOP_EVENT_TYEP_CUILIAN_PUB:
		{
			if (ShopModel::getInstance()->getShopType() == ST_CUILIAN)
			{
				auto money =  MainModel::getInstance()->getMainParam()->mCuiLianMoney;
				m_ui.txtScore->setString(TO_STR(money));

				// 所有物品重新检查价格;
				for (auto it : m_vcAllItemsCuilian)
				{
					it->updatePrice();
				}
				if (ShopModel::getInstance()->getChouSuccessful())
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20194));
					ShopModel::getInstance()->setChouSuccessful(false);
				}	
			}
			break;
		}
	case SHOP_EVENT_TYEP_ALL_ITEM:
		{
			if (ShopModel::getInstance()->getShopType() != ST_CUILIAN)
			{
				refreshAllItemOther();
			}
		}
		break;

	case SHOP_EVENT_TYEP_SINGLE_ITEM:
		{
			if (ShopModel::getInstance()->getShopType() != ST_CUILIAN)
			{
				refreshSingleItem((ShopItemParam*)(observerParam->updateData));
			}
		}
		break;

	case SHOP_EVENT_TYEP_COUNT_DOWN:
		{
			if (ShopModel::getInstance()->getShopType() != ST_CUILIAN)
			{
				//m_Son.Lab_Time->setString((formatTime(*((int*)(observerParam->updateData)))));
				m_Son.Lab_Time->setString(formatTime(ShopModel::getInstance()->getStamp()));
			}
		}
		break;

	case SHOP_EVENT_TYEP_PUBLIC:
		{
			if (ShopModel::getInstance()->getShopType() != ST_CUILIAN)
			{
				ostringstream oss;
				// 积分;
				oss << ((vector<int>*)(observerParam->updateData))->at(0);
				m_Son.Lab_Score->setString(oss.str());
				oss.str("");

				// 刷新次数;
				updateRefreshCount(((vector<int>*)(observerParam->updateData))->at(1));
				for (auto it : m_vcAllItems)
				{
					it->updatePrice();
				}
				if (ShopModel::getInstance()->getChouSuccessful())
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20194));
					ShopModel::getInstance()->setChouSuccessful(false);
				}	
				if (ShopModel::getInstance()->getFreshSuccessful())
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20248));
				}
				ShopModel::getInstance()->setFreshSuccessful(false);
			}
			break;
		}
		default:
		break;
	}
}

void LayerShopOne::refreshAllItemOther()
{
	vector<ShopItemParam>* vcAllItem = ShopModel::getInstance()->getm_vcAllItems();
	if (!vcAllItem || vcAllItem->size() == 0)
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
	int nTotalHeight = nRow * 215.0f;
	m_Son.ScrollView_Content->setInnerContainerSize(Size(m_Son.ScrollView_Content->getContentSize().width, nTotalHeight));
	float fStartX = m_Son.ScrollView_Content->getContentSize().width / 8.0f;
	float fStartY = nTotalHeight - 215.0f/2.0f;

	for (int i = 0; i < nCount; ++i)
	{
		ShopItem* _item = ShopItem::create(vcAllItem->at(i), this);
		_item->setPosition(Vec2(fStartX +m_Son.ScrollView_Content->getContentSize().width/4.0f * (i%4),
			fStartY - 215.0f * (i/4)));

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

void LayerShopOne::refreshAllItem()
{
	if (ShopModel::getInstance()->getShopType() == ST_CUILIAN)
	{
		// 清除所有;
		m_vcAllItemsCuilian.clear();
		m_ui.scrollviewContent->removeAllChildren();

		auto vcAllItem = ShopModel::getInstance()->getCuiLianInfo();
		if (vcAllItem.size() ==0)
		{
			return;
		}
		// 重新计算容器尺寸;
		int nCount = vcAllItem.size();
		int nRow = nCount/4 + ((nCount%4 == 0) ? 0 : 1);
		if (nRow ==1)
		{
			nRow = 2;
		}
		int nTotalHeight = nRow * 215.0f;
		m_ui.scrollviewContent->setInnerContainerSize(Size(m_ui.scrollviewContent->getContentSize().width, nTotalHeight));
		float fStartX = m_ui.scrollviewContent->getContentSize().width / 8.0f;
		float fStartY = nTotalHeight - 215.0f/2.0f;

		for (int i = 0; i < nCount; ++i)
		{
			ShopItem* _item = ShopItem::create(vcAllItem.at(i), this);

			_item->setPosition(Vec2(fStartX + m_ui.scrollviewContent->getContentSize().width/4.0f * (i%4),fStartY - 215.0f * (i/4)));
			std::string str = "item_"+TO_STR(i);
			_item->setName(str);
			m_ui.scrollviewContent->addChild(_item);
			m_vcAllItemsCuilian.push_back(_item);
		}
		// 刷新滚动条;
		if (m_ui.scrollBar != nullptr)
		{
			m_ui.scrollBar->setVisible(nRow > 2);
			if (m_ui.scrollBar->isVisible())
			{
				m_ui.scrollBar->setScrollBarLengthPercent(m_ui.scrollviewContent->getContentSize().height / m_ui.scrollviewContent->getInnerContainerSize().height);
				m_ui.scrollBar->setScrollBarPercentY(0.0f);
			}
		}
	}
}

void LayerShopOne::refreshSingleItem( const ShopItemParam* item )
{

	std::string oss = "item_"+TO_STR(item->nIndex);
	ShopItem* _item = dynamic_cast<ShopItem*>(m_Son.ScrollView_Content->getChildByName(oss));
	assert(_item != nullptr);
	_item->updateState(item->bEnabled);
	
}

void LayerShopOne::updateRefreshCount( int nCount )
{
	if (m_type != ST_CUILIAN)
	{
		m_Son.Lab_Count ->setString(_TO_STR(nCount));
		setTextAddStroke(m_Son.Lab_Count , Color3B(0x79, 0x03, 0x03), 2);
		m_Son.Lab_Cost->setString(_TO_STR(ShopModel::getInstance()->getRefreshCost()));
	}
}

void LayerShopOne::playEnterAnimation()
{
	// 音效;
	SoundUtils::playSoundEffect("tanchuang");

	m_ui.pRoot->setVisible(true);
	Layout* pnlShop = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_Shop"));
	ImageView* imgBg = dynamic_cast<ImageView*>(pnlShop->getChildByName("Img_Bg"));

	ActionCreator::runCommDlgAction(imgBg);
}

std::string LayerShopOne::getStrTypeName(int opType /*= 0*/)
{
	string str("");
	switch (m_type)
	{
	case ST_RONGYU:
		str.append(DataManager::getInstance()->searchCommonTexdtById(20195));
		break;
	case ST_JIFENG:
		str.append(DataManager::getInstance()->searchCommonTexdtById(20196));
		break;
	case ST_SEHGNWANG:
		str.append(DataManager::getInstance()->searchCommonTexdtById(20197));
		break;
	case ST_CUILIAN:
		str.append(DataManager::getInstance()->searchCommonTexdtById(20249));
		break;
	case ST_ZAHUO:
		if (opType == 1)
		{
			str.append(DataManager::getInstance()->searchCommonTexdtById(20117));
		}
		else
		{
			str.append(DataManager::getInstance()->searchCommonTexdtById(20118));
		}
		break;
	default:
		break;
	}
	return str;
}

////////////////////////////////////////////////////////////////////////////////

ShopItem::ShopItem()
	: m_parent(nullptr)
	, m_nIndex(-1)
{
}

ShopItem::~ShopItem()
{

}

ShopItem* ShopItem::create( ShopItemParam param, LayerShopOne* parent )
{
	ShopItem* _item = new ShopItem;
	if (nullptr != _item && _item->init(param, parent))
	{
		_item->autorelease();
		return _item;
	}

	CC_SAFE_DELETE(_item);
	return nullptr;
}

bool ShopItem::init( ShopItemParam param, LayerShopOne* parent )
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

void ShopItem::initUI( )
{
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIShopexc2/UIShopitem.ExportJson"));
	this->addChild(m_ui.pRoot);

	m_ui.btnItem = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Item"));
	m_ui.btnItem->addTouchEventListener(CC_CALLBACK_2(ShopItem::onBtnClick, this));


	//卖完
	m_ui.Img_SellOut = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_SellOut"));
	m_ui.Img_SellOut->setVisible(false);

	// 物品;
	m_ui.imgItem = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Item"));
	UIToolHeader* _item = UIToolHeader::create(_param.nId);
	_item->setPosition(m_ui.imgItem->getPosition());
	_item->setScale(0.87f);
	_item->setNumEx(_param.nCount);
	_item->showLightEff();
	m_ui.imgItem->getParent()->addChild(_item);
	m_ui.imgItem->removeFromParent();

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

	m_ui.imgScore->loadTexture(LayerShopOne::checkItemScoreImg(_param));
	

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

void ShopItem::onBtnClick( Ref* ref, Widget::TouchEventType type )
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		auto _shopInfo = ShopClickLayer::create(_param, m_parent );

		if (_shopInfo )
		{
			Director::getInstance()->getRunningScene()->addChild(_shopInfo ,989);
		}
		ActionCreator::runCommDlgAction(_shopInfo->getImagviewDi());
	}
}

void ShopItem::updateState( bool bEnabled )
{
	if (!bEnabled)
	{
		this->m_ui.Img_SellOut->setVisible(true);
	}
	m_ui.btnItem->setEnabled(bEnabled);
}

void ShopItem::updatePrice()
{
		// 售罄的不用管;
		if (!m_ui.btnItem->isEnabled())
		{
			return;
		}
		// 价格不够，显示红色;
		if (ShopModel::getInstance()->checkPrice(m_nIndex))
		{
			//m_ui.txtScore->setColor(Color3B(0xE2, 0x20, 0x20));
		}
}


string LayerShopOne::checkTitleImg()
{
	string str;

	switch (m_type)
	{
	case ST_RONGYU:
		str.append("Image/UIShopexc/Img/img_rongyushangdian.png");
		break;
	case ST_JIFENG:
		str.append("Image/UIShopexc/Img/img_shilianshangdian.png");
		break;
	case ST_SEHGNWANG:
		str.append("Image/UIShopexc/Img/img_shengwangshangdian.png");
		break;
	case ST_REFINE_8:
		str.append("Image/UIShopexc/Img/img_baguashangdian.png");
		break;
	case ST_REFINE_9:
		str.append("Image/UIShopexc/Img/img_jiugongshangdian.png");
		break;
	default:
		break;
	}

	return str;
}

string LayerShopOne::checkScoreImg(SHOP_TYPE type)
{
	string str;

	switch (type)
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

	return str;
}

std::string LayerShopOne::checkItemScoreImg(ShopItemParam Param)
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

void LayerShopOne::checkRefreshBtn()
{
	switch (m_type)
	{
	case ST_RONGYU:
	//	m_Son.Btn_Refresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin_rongyu_1.png");
	//	m_Son.Btn_Refresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin_rongyu_2.png");
	//	break;
	case ST_JIFENG:
	//	m_Son.Btn_Refresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin_jifen_1.png");
	//	m_Son.Btn_Refresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin_jifen_2.png");
	//	break;
	case ST_SEHGNWANG:
	//	m_Son.Btn_Refresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin_shengwang_1.png");
	//	m_Son.Btn_Refresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin_shengwang_2.png");
	//	break;
	case ST_ZAHUO:
		m_Son.Btn_Refresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin2_1.png");
		m_Son.Btn_Refresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin2_2.png");
		break;
	case ST_REFINE_8:
	case ST_REFINE_9:
		{
			//if (ToolModel::getInstance()->searchNumByTemplateId(10005) > 0)
			//{
			//	m_ui.btnRefresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin1_1.png");
			//	m_ui.btnRefresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin1_2.png");
			//}
			//else
			//{
			//	m_ui.btnRefresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin2_1.png");
			//	m_ui.btnRefresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin2_2.png");
			//}
		}
		break;
	default:
		break;
	}
}


