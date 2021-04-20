#include "LayerShopexc.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "ToolModule/LayerTool.h"
#include "ToolModule/ToolModel.h"
#include "Widget/ActionCreator.h"
#include "GuideCenter/GuideManager.h"
#include "Temp/CustomGoldTips.h"
#include "CommonBuy/LayerCommonBuy.h"
#include "ShopexcClickLayer.h"

LayerShopexc::LayerShopexc(void)
	: m_type(ShopType::SHOP_TYPE_ARENA)
	, m_bFirstEnter(true)
{
	ShopexcModel::getInstance()->addObserver(this);
	m_pController = new ShopexcController;
	m_vcAllItems.clear();
}


LayerShopexc::~LayerShopexc(void)
{
	ShopexcModel::getInstance()->removeObserver(this);
	if (nullptr != m_pController)
	{
		delete m_pController;
		m_pController = nullptr;
	}
}

LayerShopexc* LayerShopexc::create( ShopType type )
{
	LayerShopexc* _layer = new LayerShopexc;
	if (nullptr != _layer && _layer->init(type))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

bool LayerShopexc::init(ShopType type)
{
	if (!Layer::init())
	{
		return false;
	}

	m_type = type;
	ShopexcModel::getInstance()->setShopType(m_type);

	initUI();
	m_pController->handleShopInfo(m_type);
	return true;
}

void LayerShopexc::onEnter()
{
	Layer::onEnter();
}

void LayerShopexc::initUI()
{
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIShopexc/UIShopexc.ExportJson"));
	this->addChild(m_ui.pRoot);
	m_ui.pRoot->setVisible(false);

	// 标题;
	m_ui.imgTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Title"));
	//if (m_type != SHOP_TYPE_ARENA)
	//{
	//	m_ui.imgTitle->loadTexture(checkTitleImg());
	//}
	m_ui.imgTitle->loadTexture(checkTitleImg());
	// 关闭按钮;
	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Close)));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerShopexc::onBtnClick, this, Btn_Close));

	// 货币总量;
	m_ui.imgScore = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Score"));
	m_ui.imgScore->loadTexture(checkScoreImg(m_type));
	//if (m_type != SHOP_TYPE_ARENA)
	//{
	//	m_ui.imgScore->loadTexture(checkScoreImg(m_type));
	//}
	
	m_ui.txtScore = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Score)));
	m_ui.txtScore->setFontName(FONT_FZZHENGHEI);
	m_ui.txtScore->setString("");

	// 刷新令数量;
	m_ui.imgRefreshTokenBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_di3"));
	m_ui.txtRefreshToken = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Amount"));
	m_ui.txtRefreshToken->setFontName(FONT_FZZHENGHEI);
	m_ui.txtRefreshToken->setString("");
	m_ui.imgRefreshTokenBg->setVisible(m_type == SHOP_TYPE_REFINE_8 || m_type == SHOP_TYPE_REFINE_9);

	// 物品;
	m_ui.imgContentBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Content_Bg"));
	m_ui.scrollviewContent = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, "ScrollView_Content"));
	m_ui.scrollviewContent->addEventListener(CC_CALLBACK_2(LayerShopexc::onScrollViewEvent, this));
	m_ui.scrollBar = CustomScrollBar::create();
	m_ui.imgContentBg->addChild(m_ui.scrollBar);
	m_ui.scrollBar->setPosition(Vec2(35.0f, 35.0f));
	m_ui.scrollBar->initScrollBar(m_ui.scrollviewContent->getContentSize().height);

	// 刷新;
	m_ui.btnRefresh = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Refresh)));
	m_ui.btnRefresh->addTouchEventListener(CC_CALLBACK_2(LayerShopexc::onBtnClick, this, Btn_Refresh));
	//if (m_type != SHOP_TYPE_ARENA)
	//{
	//	checkRefreshBtn();
	//}
	checkRefreshBtn();
	m_ui.txtCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Cost"));
	m_ui.txtCost->setFontName(FONT_FZZHENGHEI);
	m_ui.txtCost->setString(_TO_STR(ShopexcModel::getInstance()->getRefreshCost()));

	// 刷新次数;
	m_ui.txtCountDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Count_Desc"));
	m_ui.txtCountDesc->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtCountDesc, Color3B(0x79, 0x03, 0x03), 2);
	m_ui.txtCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Count"));
	m_ui.txtCount->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtCount, Color3B(0x79, 0x03, 0x03), 2);
	m_ui.txtCount->setString("0");

	// 倒计时;
	m_ui.txtTimeDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Time_Desc"));
	m_ui.txtTimeDesc->setFontName(FONT_FZZHENGHEI);
	m_ui.txtTime = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Time)));
	m_ui.txtTime->setFontName(FONT_FZZHENGHEI);
	m_ui.txtTime->setString("");
}

string LayerShopexc::checkTitleImg()
{
	string str;

	switch (m_type)
	{
	case SHOP_TYPE_ARENA:
		str.append("Image/UIShopexc/Img/img_rongyushangdian.png");
		break;
	case SHOP_TYPE_TRAIL:
		str.append("Image/UIShopexc/Img/img_shilianshangdian.png");
		break;
	case SHOP_TYPE_FACTION:
		str.append("Image/UIShopexc/Img/img_shengwangshangdian.png");
		break;
	case SHOP_TYPE_REFINE_8:
		str.append("Image/UIShopexc/Img/img_baguashangdian.png");
		break;
	case SHOP_TYPE_REFINE_9:
		str.append("Image/UIShopexc/Img/img_jiugongshangdian.png");
		break;
	default:
		break;
	}

	return str;
}

string LayerShopexc::checkScoreImg(ShopType type)
{
	string str;

	switch (type)
	{
	case SHOP_TYPE_ARENA:
		str.append("Image/UIShopexc/Img/img_icon_rongyu.png");
		break;
	case SHOP_TYPE_TRAIL:
		str.append("Image/UIShopexc/Img/img_shilianjifen_icon.png");
		break;
	case SHOP_TYPE_FACTION:
		str.append("Image/UIShopexc/Img/img_shengwaihuobi.png");
		break;
	case SHOP_TYPE_REFINE_8:
		str.append("Image/UIShopexc/Img/img_huntiandan1.png");
		break;
	case SHOP_TYPE_REFINE_9:
		str.append("Image/UIShopexc/Img/img_jiuzhuandan1.png");
		break;
	default:
		break;
	}

	return str;
}

std::string LayerShopexc::checkItemScoreImg(ShopexcItemParam Param)
{
	string str;

	switch (Param.costType)
	{
	case SHOP_COST_TYPE_GOLD:
		{
			str.append("Image/Icon/global/img_yuanbao.png");
		}
		break;
	case SHOP_COST_TYPE_COIN:
		{
			str.append("Image/Icon/global/img_yinbi.png");
		}
		break;
	case SHOP_COST_TYPE_COMMON:
		{
			switch (Param.nShopType)
			{
			case SHOP_TYPE_ARENA:
				str.append("Image/UIShopexc/Img/img_icon_rongyu.png");
				break;
			case SHOP_TYPE_TRAIL:
				str.append("Image/UIShopexc/Img/img_shilianjifen_icon.png");
				break;
			case SHOP_TYPE_FACTION:
				str.append("Image/UIShopexc/Img/img_shengwaihuobi.png");
				break;
			case SHOP_TYPE_REFINE_8:
				str.append("Image/UIShopexc/Img/img_huntiandan1.png");
				break;
			case SHOP_TYPE_REFINE_9:
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

void LayerShopexc::checkRefreshBtn()
{
	switch (m_type)
	{
	case SHOP_TYPE_ARENA:
		m_ui.btnRefresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin_rongyu_1.png");
		m_ui.btnRefresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin_rongyu_2.png");
		break;
	case SHOP_TYPE_TRAIL:
		m_ui.btnRefresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin_jifen_1.png");
		m_ui.btnRefresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin_jifen_2.png");
		break;
	case SHOP_TYPE_FACTION:
		m_ui.btnRefresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin_shengwang_1.png");
		m_ui.btnRefresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin_shengwang_2.png");
		break;

	case SHOP_TYPE_REFINE_8:
	case SHOP_TYPE_REFINE_9:
		{
			if (ToolModel::getInstance()->searchNumByTemplateId(10005) > 0)
			{
				m_ui.btnRefresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin1_1.png");
				m_ui.btnRefresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin1_2.png");
			}
			else
			{
				m_ui.btnRefresh->loadTextureNormal("Image/UIShopexc/Img/btn_shuaxin2_1.png");
				m_ui.btnRefresh->loadTexturePressed("Image/UIShopexc/Img/btn_shuaxin2_2.png");
			}
		}
		break;
	default:
		break;
	}
}

void LayerShopexc::onBtnClick( Ref* ref, cocos2d::ui::Widget::TouchEventType type, int nWidgetName )
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
				int nRet = m_pController->handleRefresh();
				if (nRet != 0)
				{
					if (nRet == -1)
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20191));
					}
					else if (nRet == -2)
					{
						string str = getStrTypeName();
						str.append(DataManager::getInstance()->searchCommonTexdtById(20192));
						CustomTips::showUtf8(str);
					}
				}
			}
			break;

		default:
			break;
		}
	}
}

void LayerShopexc::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
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
				//log("formation scrolling: x = %f, y = %f, per: %f", vec.x, vec.y, percentY);

				m_ui.scrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			break;

		default:
			break;
		}
	}
}

void LayerShopexc::onItemClicked( int nIndex )
{
	if (nIndex < 0)
	{
		return;
	}

	// 音效;
	SoundUtils::playSoundEffect("dianji");

	int nRet = m_pController->handleBuy(nIndex);
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
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20193));
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

void LayerShopexc::update( float delta )
{
	this->unscheduleUpdate();
	this->removeFromParent();
}

void LayerShopexc::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* observerParam = (ObserverParam*)pData;
	switch (observerParam->id)
	{
	case OBS_PARAM_TYPE_SHOPESC_ALL_ITEM:
		{
			refreshAllItem((vector<ShopexcItemParam>*)(observerParam->updateData));

		}
		break;

	case OBS_PARAM_TYPE_SHOPESC_SINGLE_ITEM:
		{
			refreshSingleItem((ShopexcItemParam*)(observerParam->updateData));
		}
		break;

	case OBS_PARAM_TYPE_SHOPESC_COUNT_DOWN:
		{
			m_ui.txtTime->setString(formatTime(*((int*)(observerParam->updateData))));
		}
		break;

	case OBS_PARAM_TYPE_SHOPESC_PUBLIC:
		{
			ostringstream oss;

			// 积分;
			oss << ((vector<int>*)(observerParam->updateData))->at(0);
			m_ui.txtScore->setString(oss.str());
			oss.str("");

			// 刷新次数;
			updateRefreshCount(((vector<int>*)(observerParam->updateData))->at(1));

			// 刷新令;
			if (m_type == SHOP_TYPE_REFINE_8 || m_type == SHOP_TYPE_REFINE_9)
			{
				m_ui.txtRefreshToken->setString(_TO_STR(ToolModel::getInstance()->searchNumByTemplateId(10005)));
			}
			if (ShopexcModel::getInstance()->getChouSuccessful())
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20194));
				ShopexcModel::getInstance()->setChouSuccessful(false);
			}
			// 所有物品重新检查价格;
			for (auto it : m_vcAllItems)
				it->updatePrice();

			if (m_bFirstEnter)
			{
				playEnterAnimation();
				m_bFirstEnter = false;
			}
		}
		break;

	default:
		break;
	}
}

void LayerShopexc::refreshAllItem( const vector<ShopexcItemParam>* vcAllItem )
{
	// 清除所有;
	m_vcAllItems.clear();
	m_ui.scrollviewContent->removeAllChildren();

	// 重新计算容器尺寸;
	int nCount = vcAllItem->size();
	int nRow = nCount/4 + ((nCount%4 == 0) ? 0 : 1);
	if (nRow ==1)
	{
		nRow = 2;
	}
	int nTotalHeight = nRow * 215.0f;
	m_ui.scrollviewContent->setInnerContainerSize(Size(m_ui.scrollviewContent->getContentSize().width, nTotalHeight));
	float fStartX = m_ui.scrollviewContent->getContentSize().width / 8.0f;
	float fStartY = nTotalHeight - 215.0f/2.0f;

	ostringstream oss;
	for (int i = 0; i < nCount; ++i)
	{
		ShopexcItem* _item = ShopexcItem::create(vcAllItem->at(i), this);
		_item->setPosition(Vec2(fStartX + m_ui.scrollviewContent->getContentSize().width/4.0f * (i%4),
			fStartY - 215.0f * (i/4)));
		oss.str("");
		oss << "item_" << i;
		_item->setName(oss.str());
		m_ui.scrollviewContent->addChild(_item);
		m_vcAllItems.push_back(_item);
	}

	// 刷新滚动条;
	m_ui.scrollBar->setVisible(nRow > 2);
	if (m_ui.scrollBar->isVisible())
	{
		m_ui.scrollBar->setScrollBarLengthPercent(m_ui.scrollviewContent->getContentSize().height / m_ui.scrollviewContent->getInnerContainerSize().height);
		m_ui.scrollBar->setScrollBarPercentY(0.0f);
	}

	// 刷新按钮(主要是针对炼化炉商店);
	if (m_type == SHOP_TYPE_REFINE_8 || m_type == SHOP_TYPE_REFINE_9)
	{
		checkRefreshBtn();
		m_ui.txtRefreshToken->setString(_TO_STR(ToolModel::getInstance()->searchNumByTemplateId(10005)));
	}
}

void LayerShopexc::refreshSingleItem( const ShopexcItemParam* item )
{
	ostringstream oss;
	oss << "item_" << item->nIndex;
	ShopexcItem* _item = dynamic_cast<ShopexcItem*>(m_ui.scrollviewContent->getChildByName(oss.str()));
	assert(_item != nullptr);
	_item->updateState(item->bEnabled);
}

void LayerShopexc::updateRefreshCount( int nCount )
{
	m_ui.txtCount->setString(_TO_STR(nCount));
	setTextAddStroke(m_ui.txtCount, Color3B(0x79, 0x03, 0x03), 2);
	m_ui.txtCost->setString(_TO_STR(ShopexcModel::getInstance()->getRefreshCost()));
}

void LayerShopexc::playEnterAnimation()
{
	// 音效;
	SoundUtils::playSoundEffect("tanchuang");

	m_ui.pRoot->setVisible(true);
	Layout* pnlShop = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_Shop"));
	ImageView* imgBg = dynamic_cast<ImageView*>(pnlShop->getChildByName("Img_Bg"));

	ActionCreator::runCommDlgAction(imgBg);//, callBack);
}

std::string LayerShopexc::getStrTypeName(int opType /*= 0*/)
{
	string str("");
	switch (m_type)
	{
	case SHOP_TYPE_ARENA:
		str.append(DataManager::getInstance()->searchCommonTexdtById(20195));
		break;
	case SHOP_TYPE_TRAIL:
		str.append(DataManager::getInstance()->searchCommonTexdtById(20196));
		break;
	case SHOP_TYPE_FACTION:
		str.append(DataManager::getInstance()->searchCommonTexdtById(20197));
		break;
	case SHOP_TYPE_REFINE_8:
		{
			if (opType == 1)
			{
				str.append(DataManager::getInstance()->searchCommonTexdtById(20114));
			}
			else
			{
				str.append(DataManager::getInstance()->searchCommonTexdtById(20117));
			}
		}
		break;
	case SHOP_TYPE_REFINE_9:
		{
			if (opType == 1)
			{
				str.append(DataManager::getInstance()->searchCommonTexdtById(20115));
			}
			else
			{
				str.append(DataManager::getInstance()->searchCommonTexdtById(20117));
			}
		}
		break;
	default:
		break;
	}
	return str;
}

ImageView* LayerShopexc::getImageViewBg()
{
	Layout* pnlShop = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_Shop"));
	ImageView* imgBg = dynamic_cast<ImageView*>(pnlShop->getChildByName("Img_Bg"));
	return imgBg;
}

////////////////////////////////////////////////////////////////////////////////

ShopexcItem::ShopexcItem()
	: m_parent(nullptr)
	, m_nIndex(-1)
{
}

ShopexcItem::~ShopexcItem()
{

}

ShopexcItem* ShopexcItem::create( ShopexcItemParam param, LayerShopexc* parent )
{
	ShopexcItem* _item = new ShopexcItem;
	if (nullptr != _item && _item->init(param, parent))
	{
		_item->autorelease();
		return _item;
	}

	CC_SAFE_DELETE(_item);
	return nullptr;
}

bool ShopexcItem::init( ShopexcItemParam param, LayerShopexc* parent )
{
	if (!Layout::init())
	{
		return false;
	}

	m_parent = parent;
	m_nIndex = param.nIndex;
	this->_param = param;

	initUI(param);
	updateState(param.bEnabled);
	updatePrice();
	return true;
}

void ShopexcItem::initUI( ShopexcItemParam param )
{
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIShopexc2/UIShopitem.ExportJson"));//Image/UIShopexc/UIShopitem.ExportJson"));
	this->addChild(m_ui.pRoot);

	m_ui.btnItem = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Item"));
	m_ui.btnItem->addTouchEventListener(CC_CALLBACK_2(ShopexcItem::onBtnClick, this));


	//卖完
	m_ui.Img_SellOut = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_SellOut"));
	m_ui.Img_SellOut->setVisible(false);
	// 排行条件;
	//m_ui.imgRank = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Rank"));
	//m_ui.imgRank->setVisible(param.nRank != -1);
	//m_ui.imgRank->setVisible(false);
	//m_ui.txtRank = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Rank"));
	//m_ui.txtRank->setVisible(false);
	//m_ui.txtRank->setFontName(FONT_FZZHENGHEI);
	//m_ui.txtRank->setString(_TO_STR(param.nRank));
	//setTextAddStroke(m_ui.txtRank, Color3B::BLACK, 3);

	// 物品;
	m_ui.imgItem = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Item"));
	UIToolHeader* _item = UIToolHeader::create(param.nId);
	_item->setPosition(m_ui.imgItem->getPosition());
	_item->setScale(0.87f);
	_item->setNumEx(param.nCount);
	_item->showLightEff();
	m_ui.imgItem->getParent()->addChild(_item);
	m_ui.imgItem->removeFromParent();

	// 限购次数;
	//m_ui.imgLimit = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Limit"));
	//m_ui.imgLimit->setVisible(param.nLimitBuyCount >= 1);

	// 名称;
	m_ui.txtName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Name"));
	m_ui.txtName->setFontName(FONT_FZZHENGHEI);
	m_ui.txtName->setString(param.strName);
	setTextAddStroke(m_ui.txtName, Color3B(0x79, 0x03, 0x03), 2);

	// 消耗货币;
	m_ui.imgScore = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Score"));
	m_ui.imgScore->loadTexture(LayerShopexc::checkItemScoreImg(param));
	//if (param.nShopType != SHOP_TYPE_ARENA)
	//{
	//	if (param.costType == SHOP_COST_TYPE_COIN || param.costType == SHOP_COST_TYPE_GOLD)
	//	{
	//		m_ui.imgScore->setScale(1.2f);
	//	}
	//	else
	//	{
	//		m_ui.imgScore->setScale(0.7f);
	//	}
	//	m_ui.imgScore->loadTexture(LayerShopexc::checkItemScoreImg(param));
	//}
	m_ui.txtScore = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Score"));
	m_ui.txtScore->setFontName(FONT_FZZHENGHEI);
	m_ui.txtScore->setString(_TO_STR(param.nPrice));
}

void ShopexcItem::onBtnClick( Ref* ref, Widget::TouchEventType type )
{
	if (type == Widget::TouchEventType::ENDED)
	{
		auto _shopInfo = ShopexcClickLayer::create(_param, m_parent );

		if (_shopInfo )
		{
			Director::getInstance()->getRunningScene()->addChild(_shopInfo ,989);
		}
		ActionCreator::runCommDlgAction(_shopInfo->getImagviewDi());
	}
}

void ShopexcItem::updateState( bool bEnabled )
{
	//if (!bEnabled)
	//{
	//	ImageView* img1 = ImageView::create("Image/UIShopexc/Img/btn_shangdian_kuang_zhezhao.png");
	//	this->addChild(img1, 9);
	//	ImageView* img2 = ImageView::create("Image/UIShopexc/Img/img_shouqing.png");
	//	this->addChild(img2, img1->getLocalZOrder()+1);
	//}
	//m_ui.btnItem->setEnabled(bEnabled);

	if (!bEnabled)
	{
		this->m_ui.Img_SellOut->setVisible(true);
	}
	m_ui.btnItem->setEnabled(bEnabled);
}

void ShopexcItem::updatePrice()
{
	// 售罄的不用管;
	if (!m_ui.btnItem->isEnabled())
	{
		return;
	}

	// 价格不够，显示红色;
	if (ShopexcModel::getInstance()->checkPrice(m_nIndex))
	{
		//m_ui.txtScore->setColor(Color3B(0xE2, 0x20, 0x20));
	}
}
