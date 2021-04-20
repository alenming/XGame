#include "MysteryShopClick.h"
#include "DataManager/DataManager.h"
#include "ToolModule/ToolModel.h"

MysteryShopClick::~MysteryShopClick()
{

}

MysteryShopClick::MysteryShopClick():parrent(nullptr)
{

}

 MysteryShopClick* MysteryShopClick::create(ShopItemParam index, MysteryShopLayer* la) 
{ 
	MysteryShopClick *pRet = new MysteryShopClick(); 
	if (pRet && pRet->init(index, la)) 
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

bool MysteryShopClick::init(ShopItemParam index, MysteryShopLayer* la)
{
	if (!ModalLayer::init())
	{
		return false;
	}
	this->index = index;
	this->parrent = la;
	initUi();

	return true;
}

void MysteryShopClick::onEnter()
{
	ModalLayer::onEnter();
	updataInfo();
}

void MysteryShopClick::onExit()
{
	ModalLayer::onExit();
}

void MysteryShopClick::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()
		->widgetFromJsonFile("Image/UIShopexc2/UIBuyConfirm.ExportJson"));

	m_ui.Btn_Buy =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root, "Btn_Buy"));
	m_ui.Btn_Close =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root, "Btn_Close"));
	m_ui.Btn_Close->addTouchEventListener(CC_CALLBACK_2(MysteryShopClick::onBtnClick ,this, ONE_ITEMS_EXIT));
	m_ui.Btn_Buy->addTouchEventListener(CC_CALLBACK_2(MysteryShopClick::onBtnClick,this, ONE_ITEMS_BUY));

	m_ui.Img_di1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root, "Img_di1"));
	m_ui.Img_CosIcon =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root, "Img_CosIcon"));
	m_ui.Lab_Cost=  dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root, "Lab_Cost"));
	m_ui.Lab_Count =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root, "Lab_Count"));
	m_ui.Lab_ItemName =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root, "Lab_ItemName"));
	m_ui.Lab_Tips =  dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root, "Lab_Tips"));
	m_ui.Pnl_Item =  dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Root, "Pnl_Item"));

	m_ui.ui_Root->setSize(size);
	m_ui.ui_Root->setPosition(pos);
	this->addChild(m_ui.ui_Root);
}

void MysteryShopClick::updataInfo()
{
	// 物品;

	UIToolHeader* _item = UIToolHeader::create(index.nId);
	if (_item)
	{
		_item->setAnchorPoint(Vec2(0,0));
		_item->setScale(0.87f);
		_item->setNumEx(index.nCount);
		m_ui.Pnl_Item->addChild(_item);
	}

	auto info = DataManager::getInstance()->searchToolById(index.nId);
	auto intrudution = info->getStringData("itemInfo");
	m_ui.Lab_Tips->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_Tips->setString(intrudution);

	// 名称;
	m_ui.Lab_ItemName->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_ItemName->setString(index.strName);
	setTextAddStroke(m_ui.Lab_ItemName, Color3B(0x79, 0x03, 0x03), 2);

	// 消耗货币;
	if (index.nShopType == ST_ZAHUO)
	{
		if (index.moneyType == MONEY_COIN || index.moneyType == MONEY_GOLD)
		{
			m_ui.Img_CosIcon->setScale(1.2f);
		}
	}

	m_ui.Img_CosIcon->loadTexture(MysteryShopClick::checkItemScoreImg(index));

	m_ui.Lab_Cost->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_Cost->setString(_TO_STR(index.nPrice));
	//已经拥有的数量
	int count = ToolModel::getInstance()->searchNumByTemplateId(index.nId);
	m_ui.Lab_Count->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_Count->setString(_TO_STR(count));
}


void MysteryShopClick::onBtnClick( Ref* reff, Widget::TouchEventType type, ONEITEM_CBACK _callback )
{
	if(type == Widget::TouchEventType::BEGAN)
	{

	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		switch (_callback)
		{
		case ONE_ITEMS_EXIT:
			this->dispose();
			break;
		case ONE_ITEMS_BUY:
			{
				if (nullptr != parrent)
				{
					MysteryShopLayer* _parent = dynamic_cast<MysteryShopLayer*>(parrent);
					if (nullptr != _parent)
					{
						_parent->onItemClicked(index.nIndex);
						this->dispose();
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

Layout* MysteryShopClick::getSon()
{
	return m_ui.ui_Root;
}



std::string MysteryShopClick::checkItemScoreImg(ShopItemParam Param)
{
	string str;

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

ImageView* MysteryShopClick::getImagviewDi()
{
	return m_ui.Img_di1;
}
