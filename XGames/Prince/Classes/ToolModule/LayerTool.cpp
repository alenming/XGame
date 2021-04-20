#include "LayerTool.h"
#include "CocoStudio.h"
#include "ToolModel.h"
#include "ToolController.h"
#include "Common/Common.h"
#include "Communication/Command.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "ResourceManager/ResourceLoader.h"
#include "UI/UIDef.h"
#include "Widget/LayerLoading.h"
#include "../HeroModule/LayerHero.h"
#include "../SmithyModule/SceneSmithy.h"
#include "../SmithyModule/LayerSmithy.h"
#include "../Utils/ResourceUtils.h"
#include "Temp/CustomPop.h"
#include "Temp/CustomTips.h"
#include "../FormationModule/CustomScrollBar.h"
#include "HeroModule/HeroModel.h"
#include "../MainModule/MainCityScene.h"
#include "GuideCenter/GuideManager.h"
#include "MainModule/MainModel.h"
#include "CommonBuy/LayerCommonBuy.h"
#include "SmithyModule/SmithyModel.h"
#include "Widget/LayerCommHeadInfo.h"
#include "Widget/QuantitySlideBar.h"


const float MAX_TOOL_ICON_WIDTH		= 104.4;
const float MAX_TOOL_ICON_HEIGHT	= 104.4;
#define QUANTITY_SLIDEBAR_SELL_TAG		10001
#define QUANTITY_SLIDEBAR_USE_TAG		10002

using namespace cocos2d;
using namespace cocos2d::ui;

LayerTool::LayerTool()
{
	m_nsellNum					= 1;
	m_nsellMaxNum				= 0;
	m_nsellPrice				= 0;
	m_CurrentToolId				= 0;
	m_curGroupToolNum			= 0;
	m_pCurrentTool				= nullptr;
	m_nDetailDisCount			= 0;
	mUseNum						= 1;
	m_bResetSellSliderPos		= false;
	m_bResetUseSliderPos		= false;
	m_toolType = TOOL_TYPE_NONE;
	ToolModel::getInstance()->addObserver(this);
}

LayerTool::~LayerTool()
{
	ToolModel::getInstance()->removeObserver(this);
}

bool LayerTool::init()
{
	if(!ModalLayer::init())
	{
		return false;
	}

	initUI();
	initUIDetail();

	if (ToolModel::getInstance()->GetAllToolInfo().size() == 0)
	{
		m_ui.pBtnAll->setSelectedState(true);
		m_ui.pBtnAll->loadTextureBackGround("Image/UIbeibao/btn_fenye_quanbu_1.png");
	}
	else
	{
		m_ui.pBtnAll->setSelectedState(true);
		m_ui.pBtnAll->loadTextureBackGround("Image/UIbeibao/btn_fenye_quanbu_1.png");
		ShowToolData(TOOL_TYPE_ALL);
	}

	return true;
}


void LayerTool::initUI()
{
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UIbeibao/UIbeibao.ExportJson"));
	m_ui.pRoot->setSize(Director::getInstance()->getVisibleSize());
	m_ui.pRoot->setPosition(Director::getInstance()->getVisibleOrigin());
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_ui.pRoot);

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(
		CC_CALLBACK_2(LayerTool::onBtnBack, this), HEAD_INFO_TYPE_BAG);
	this->addChild(_layerCommHeadInfo);
	
	m_ui.pLayerBeibao = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_beibao"));
	m_ui.pLayerDetailConsume = dynamic_cast<Layout*>(m_ui.pLayerBeibao->getChildByName("Pnl_item"));
	m_ui.pLayerDetailConsume->setVisible(false);
	m_ui.pLayerDetailEquip = dynamic_cast<Layout*>(m_ui.pLayerBeibao->getChildByName("Pnl_equ"));
	m_ui.pLayerDetailEquip->setVisible(false);

	m_ui.pBtnAll = dynamic_cast<CheckBox*>(m_ui.pLayerBeibao->getChildByName("Cb_all"));
	m_ui.pBtnAll->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnAll, this));
	m_ui.pBtnChip = dynamic_cast<CheckBox*>(m_ui.pLayerBeibao->getChildByName("Cb_patch"));
	m_ui.pBtnChip->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnChip, this));
	m_ui.pBtnEquip = dynamic_cast<CheckBox*>(m_ui.pLayerBeibao->getChildByName("Cb_equ"));
	m_ui.pBtnEquip->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnEquip, this));
	m_ui.pBtnConsume = dynamic_cast<CheckBox*>(m_ui.pLayerBeibao->getChildByName("Cb_item"));
	m_ui.pBtnConsume->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnConsume, this));

	//滚动面板
	m_ui.pImgBeibao = dynamic_cast<ImageView*>(m_ui.pLayerBeibao->getChildByName("Image_beibao"));
	ImageView* pImgBeibaoDi = dynamic_cast<ImageView*>(m_ui.pImgBeibao->getChildByName("Img_di"));
	
	m_ui.pScrollView = dynamic_cast<ui::ScrollView*>(pImgBeibaoDi->getChildByName("ScrollView_tool"));
	m_ui.pScrollView->addEventListenerScrollView(this, (SEL_ScrollViewEvent)&LayerTool::onScrollViewEvent);

	m_ui.listHeroBar = CustomScrollBar::create();
	pImgBeibaoDi->addChild(m_ui.listHeroBar);
	m_ui.listHeroBar->setPosition(Vec2(40.0f, 30.0f));
	m_ui.listHeroBar->initScrollBar(m_ui.pScrollView->getContentSize().height - 10);
	
	m_ui.pBtnPnl = dynamic_cast<Layout*>(m_ui.pLayerBeibao->getChildByName("Pnl_btu1"));
	m_ui.pBtnSell = dynamic_cast<Button*>(m_ui.pBtnPnl->getChildByName("Btn_sell"));
	m_ui.pBtnSell->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnToolDetailSell, this));
	m_ui.pBtnUse = dynamic_cast<Button*>(m_ui.pBtnPnl->getChildByName("Btn_use"));
	m_ui.pBtnUse->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnToolDetailUse, this));
	m_ui.pBtnJump = dynamic_cast<Button*>(m_ui.pBtnPnl->getChildByName("Btn_jump"));
	m_ui.pointPosSell = m_ui.pBtnSell->getPosition();
	m_ui.pointPosUse = m_ui.pBtnUse->getPosition();

	//出售价格
	m_ui.pImgPrice  = dynamic_cast<ImageView*>(m_ui.pLayerBeibao->getChildByName("Img_price"));
	m_ui.pTextCost = dynamic_cast<Text*>(m_ui.pImgPrice->getChildByName("Lab_price"));
	m_ui.pTextCostNum = dynamic_cast<Text*>(m_ui.pImgPrice->getChildByName("Lab_num"));
	m_ui.pTextCost->setFontName(FONT_FZZHENGHEI);
	m_ui.pTextCostNum->setFontName(FONT_FZZHENGHEI);

}

void LayerTool::initUIDetail()
{
	//装备
	m_ui.uiEquip.pImgTitle  = dynamic_cast<ImageView*>(m_ui.pLayerDetailEquip->getChildByName("Img_name"));
	Layout *pLayHeader = dynamic_cast<Layout*>(m_ui.uiEquip.pImgTitle->getChildByName("Pnl_header"));
	m_ui.uiEquip.pHeader = UIToolHeader::create();
	m_ui.uiEquip.pHeader->setPosition(cocos2d::Vec2(pLayHeader->getContentSize().width/2, pLayHeader->getContentSize().height/2));
	pLayHeader->addChild(m_ui.uiEquip.pHeader);
	m_ui.uiEquip.pTextName = dynamic_cast<Text*>(m_ui.uiEquip.pImgTitle->getChildByName("Lab_name"));
	m_ui.uiEquip.pTextNum = dynamic_cast<Text*>(m_ui.uiEquip.pImgTitle->getChildByName("Lab_num"));
	m_ui.uiEquip.pTextName->setFontName(FONT_FZZHENGHEI);
	m_ui.uiEquip.pTextNum->setFontName(FONT_FZZHENGHEI);
	m_ui.uiEquip.pImgDetail = dynamic_cast<ImageView*>(m_ui.pLayerDetailEquip->getChildByName("Img_info"));
	m_ui.uiEquip.vecPosImgTitle = m_ui.uiEquip.pImgTitle->getPosition();
	m_ui.uiEquip.vecPosImgDetail = m_ui.uiEquip.pImgDetail->getPosition();
	ImageView* pImgInfo = m_ui.uiEquip.pImgDetail;
	//基础属性
	m_ui.uiEquip.pTextAtrri1 = dynamic_cast<Text*>(pImgInfo->getChildByName("Lab_base1"));
	m_ui.uiEquip.pTextAtrri2 = dynamic_cast<Text*>(pImgInfo->getChildByName("Lab_base2"));
	m_ui.uiEquip.pTextAtrri1->setFontName(FONT_FZZHENGHEI);
	m_ui.uiEquip.pTextAtrri2->setFontName(FONT_FZZHENGHEI);

	//消耗品、碎片
	m_ui.uiConsume.pImgTitle  = dynamic_cast<ImageView*>(m_ui.pLayerDetailConsume->getChildByName("Img_name"));
	pLayHeader = dynamic_cast<Layout*>(m_ui.uiConsume.pImgTitle->getChildByName("Pnl_header"));
	m_ui.uiConsume.pHeader = UIToolHeader::create();
	m_ui.uiConsume.pHeader->setPosition(cocos2d::Vec2(pLayHeader->getContentSize().width/2, pLayHeader->getContentSize().height/2));//pImageView2->getPosition()
	pLayHeader->addChild(m_ui.uiConsume.pHeader);
	m_ui.uiConsume.pTextName = dynamic_cast<Text*>(m_ui.uiConsume.pImgTitle->getChildByName("Lab_name"));
	m_ui.uiConsume.pTextNum = dynamic_cast<Text*>(m_ui.uiConsume.pImgTitle->getChildByName("Lab_num"));
	m_ui.uiConsume.pTextName->setFontName(FONT_FZZHENGHEI);
	m_ui.uiConsume.pTextNum->setFontName(FONT_FZZHENGHEI);
	m_ui.uiConsume.pImgDetail  = dynamic_cast<ImageView*>(m_ui.pLayerDetailConsume->getChildByName("Img_info"));
	m_ui.uiConsume.vecPosImgTitle = m_ui.uiConsume.pImgTitle->getPosition();
	m_ui.uiConsume.vecPosImgDetail = m_ui.uiConsume.pImgDetail->getPosition();
	m_ui.uiConsume.pTextDetail = dynamic_cast<Text*>(m_ui.uiConsume.pImgDetail->getChildByName("Lab_info"));
	m_ui.uiConsume.pTextDetail->setFontName(FONT_FZZHENGHEI);

	//出售弹出框
	m_ui.pLayerPopSell = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_sell"));
	ImageView* popSellBack = dynamic_cast<ImageView*>(m_ui.pLayerPopSell->getChildByName("Img_di"));
	pLayHeader = dynamic_cast<Layout*>(popSellBack->getChildByName("Pnl_tool"));
	m_ui.uiSell.pHeader = UIToolHeader::create();
	m_ui.uiSell.pHeader->setPosition(cocos2d::Vec2(pLayHeader->getContentSize().width/2, pLayHeader->getContentSize().height/2));
	pLayHeader->addChild(m_ui.uiSell.pHeader);
	m_ui.uiSell.pTextName = dynamic_cast<Text*>(popSellBack->getChildByName("Lab_daojuming"));
	m_ui.uiSell.pTextNum = dynamic_cast<Text*>(popSellBack->getChildByName("Lab_Pnum"));
	ImageView* priceBack = dynamic_cast<ImageView*>(popSellBack->getChildByName("Image_jiagedi"));
	m_ui.uiSell.pTextTotle = dynamic_cast<Text*>(priceBack->getChildByName("Lab_price_totle"));
	m_ui.uiSell.pTextTotleNum = dynamic_cast<Text*>(priceBack->getChildByName("Lab_Pnum_totle"));

	m_ui.uiSell.pTextName->setFontName(FONT_FZZHENGHEI);
	m_ui.uiSell.pTextNum->setFontName(FONT_FZZHENGHEI);
	m_ui.uiSell.pTextTotle->setFontName(FONT_FZZHENGHEI);
	m_ui.uiSell.pTextTotleNum->setFontName(FONT_FZZHENGHEI);
	m_ui.uiSell.pBtnCancel = dynamic_cast<Button*>(popSellBack->getChildByName("Btn_close"));
	m_ui.uiSell.pBtnCancel->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnSellClose, this));
	m_ui.uiSell.pBtnConfirm = dynamic_cast<Button*>(popSellBack->getChildByName("Btn_makesure"));
	m_ui.uiSell.pBtnConfirm->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnSellConfrim, this));	

	//通用数量条滑动
	QuantitySlideBar* pSlideBar = QuantitySlideBar::create();
	pSlideBar->setPosition(Vec2(priceBack->getPositionX(), priceBack->getPositionY() + 90));
	pSlideBar->setAnchorPoint(Point::ZERO);
	pSlideBar->setPositionY(pSlideBar->getPositionY() + 1000);		//先移出，和物品触摸有屏蔽作用
	m_ui.uiSell.pBtnConfirm->getParent()->addChild(pSlideBar, 1, QUANTITY_SLIDEBAR_SELL_TAG);
	//mark:最小响应

	//使用弹出框
	m_ui.pLayerPopUse = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_use"));
	ImageView* popUseBack = dynamic_cast<ImageView*>(m_ui.pLayerPopUse->getChildByName("Img_di"));
	pLayHeader = dynamic_cast<Layout*>(popUseBack->getChildByName("Pnl_tool"));
	m_ui.uiUse.pHeader = UIToolHeader::create();
	m_ui.uiUse.pHeader->setPosition(cocos2d::Vec2(pLayHeader->getContentSize().width/2, pLayHeader->getContentSize().height/2));
	pLayHeader->addChild(m_ui.uiUse.pHeader);
	m_ui.uiUse.pTextName = dynamic_cast<Text*>(popUseBack->getChildByName("Lab_daojuming"));
	m_ui.uiUse.pTextNum = dynamic_cast<Text*>(popUseBack->getChildByName("Lab_Pnum"));

	m_ui.uiUse.pTextName->setFontName(FONT_FZZHENGHEI);
	m_ui.uiUse.pTextNum->setFontName(FONT_FZZHENGHEI);
	
	m_ui.uiUse.pBtnCancel = dynamic_cast<Button*>(popUseBack->getChildByName("Btn_close"));
	m_ui.uiUse.pBtnCancel->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnUseClose, this));
	m_ui.uiUse.pBtnConfirm = dynamic_cast<Button*>(popUseBack->getChildByName("Btn_makesure"));
	m_ui.uiUse.pBtnConfirm->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnUseConfrim, this));	

	//通用数量条滑动
	QuantitySlideBar* pSlideBarUse = QuantitySlideBar::create();
	pSlideBarUse->setPosition(Vec2(m_ui.uiUse.pBtnConfirm->getPositionX(), m_ui.uiUse.pBtnConfirm->getPositionY() + 90));
	pSlideBarUse->setAnchorPoint(Point::ZERO);
	pSlideBarUse->setPositionY(pSlideBarUse->getPositionY() + 1000);	//先移出，和物品触摸有屏蔽作用
	m_ui.uiUse.pBtnConfirm->getParent()->addChild(pSlideBarUse, 1, QUANTITY_SLIDEBAR_USE_TAG);

	//默认隐藏详情页面
	m_ui.pLayerDetailConsume->setVisible(false);
	m_ui.pLayerDetailEquip->setVisible(false);
	m_ui.pLayerPopUse->setVisible(false);
	m_ui.pLayerPopSell->setVisible(false);
}

void LayerTool::onEnter()
{
	Layer::onEnter();
}

void LayerTool::onExit()
{
	Layer::onExit();
	ResourceLoader::getInstance()->removeUIResource("UIbeibao");
}

void LayerTool::onScrollViewEvent(Ref* sender, ScrollviewEventType type)
{
	//log("LayerTool::onScrollViewEvent type = %d", type);
	ui::ScrollView* pScroll = (ui::ScrollView*)sender;
	Size contentSize = pScroll->getContentSize();
	Size size = m_ui.pScrollView->getInnerContainerSize();
	if (contentSize.height>=size.height)
	{
		return;
	}
	
	switch (type)
	{
	case SCROLLVIEW_EVENT_SCROLLING:
		{
			
			Vec2 vec = m_ui.pScrollView->getInnerContainer()->getPosition();
			CCLOG("LayerTool::onScrollViewEvent Position size height=%f, y = %f", size.height, vec.y);
			//float xPercent = -vec.y / (size.height - Director::getInstance()->getVisibleSize().height);
			if (vec.y > (contentSize.height-size.height)/2)
			{
				//startBottomAnimation();
			}
			else if (vec.y <= contentSize.height-size.height)
			{
				//startTopAnimation();
			}
			else
			{

			}
			float percentY = -vec.y / (size.height - m_ui.pScrollView->getContentSize().height);
			m_ui.listHeroBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
		}
		break;
	case SCROLLVIEW_EVENT_BOUNCE_TOP:
		break;
	case SCROLLVIEW_EVENT_BOUNCE_BOTTOM:
		break;
	case SCROLLVIEW_EVENT_SCROLL_TO_TOP:
		break;
	case SCROLLVIEW_EVENT_SCROLL_TO_BOTTOM:
		break;
	default:
		break;
	}
}

void LayerTool::onBtnBack( Ref* sender, cocos2d::ui::Widget::TouchEventType type )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		//SceneController::getInstance()->setLastSceneId(ModuleSceneNone);
		Director::getInstance()->replaceScene(MainCityScene::create());
	}
}

// 全部按钮
void LayerTool::onBtnAll(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		CheckBox *pCheckBox = (CheckBox*)sender;
		pCheckBox->setSelectedState(false);
		if (m_toolType == TOOL_TYPE_ALL)
		{
			return;
		}
		m_ui.pBtnConsume->setSelectedState(false);
		m_ui.pBtnEquip->setSelectedState(false);
		m_ui.pBtnChip->setSelectedState(false);
		m_ui.pBtnConsume->loadTextureBackGround("Image/UIbeibao/btn_fenye_xiaohaopin_2.png");
		m_ui.pBtnEquip->loadTextureBackGround("Image/UIbeibao/btn_cailiao_2.png");
		m_ui.pBtnChip->loadTextureBackGround("Image/UIbeibao/btn_fenye_suipian_2.png");
		m_ui.pBtnAll->loadTextureBackGround("Image/UIbeibao/btn_fenye_quanbu_1.png");
		m_CurrentToolId = 0;
		ShowToolData(TOOL_TYPE_ALL);
	}
}

// 碎片按钮
void LayerTool::onBtnChip(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		CheckBox *pCheckBox = (CheckBox*)sender;
		pCheckBox->setSelectedState(false);
		if (m_toolType == TOOL_TYPE_HERO_CHIP)
		{
			return;
		}
		m_ui.pBtnConsume->loadTextureBackGround("Image/UIbeibao/btn_fenye_xiaohaopin_2.png");
		m_ui.pBtnEquip->loadTextureBackGround("Image/UIbeibao/btn_cailiao_2.png");
		m_ui.pBtnChip->loadTextureBackGround("Image/UIbeibao/btn_fenye_suipian_1.png");
		m_ui.pBtnAll->loadTextureBackGround("Image/UIbeibao/btn_fenye_quanbu_2.png");
		m_ui.pBtnConsume->setSelectedState(false);
		m_ui.pBtnEquip->setSelectedState(false);
		//m_ui.pBtnChip->setSelectedState(false);
		m_ui.pBtnAll->setSelectedState(false);
		m_CurrentToolId = 0;
		ShowToolData(TOOL_TYPE_HERO_CHIP);
	}
}

// 消耗按钮
void LayerTool::onBtnConsume(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		CheckBox *pCheckBox = (CheckBox*)sender;
		pCheckBox->setSelectedState(false);
		if (m_toolType == TOOL_TYPE_CONSUME)
		{
			return;
		}	
		m_ui.pBtnConsume->loadTextureBackGround("Image/UIbeibao/btn_fenye_xiaohaopin_1.png");
		m_ui.pBtnEquip->loadTextureBackGround("Image/UIbeibao/btn_cailiao_2.png");
		m_ui.pBtnChip->loadTextureBackGround("Image/UIbeibao/btn_fenye_suipian_2.png");
		m_ui.pBtnAll->loadTextureBackGround("Image/UIbeibao/btn_fenye_quanbu_2.png");
		//m_ui.pBtnConsume->setSelectedState(false);
		m_ui.pBtnEquip->setSelectedState(false);
		m_ui.pBtnChip->setSelectedState(false);
		m_ui.pBtnAll->setSelectedState(false);

		m_CurrentToolId = 0;
		ShowToolData(TOOL_TYPE_CONSUME);
	}
}
// 装备按钮
void LayerTool::onBtnEquip(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		CheckBox *pCheckBox = (CheckBox*)sender;
		pCheckBox->setSelectedState(false);
		if (m_toolType == TOOL_TYPE_MATERIAL)
		{
			return;
		}
		m_ui.pBtnConsume->loadTextureBackGround("Image/UIbeibao/btn_fenye_xiaohaopin_2.png");
		m_ui.pBtnEquip->loadTextureBackGround("Image/UIbeibao/btn_cailiao_1.png");
		m_ui.pBtnChip->loadTextureBackGround("Image/UIbeibao/btn_fenye_suipian_2.png");
		m_ui.pBtnAll->loadTextureBackGround("Image/UIbeibao/btn_fenye_quanbu_2.png");
		m_ui.pBtnConsume->setSelectedState(false);
		//m_ui.pBtnEquip->setSelectedState(false);
		m_ui.pBtnChip->setSelectedState(false);
		m_ui.pBtnAll->setSelectedState(false);

		m_CurrentToolId = 0;
		ShowToolData(TOOL_TYPE_MATERIAL);
	}
}

void LayerTool::onRecvUseToolData(int amount)
{	
	m_ui.pLayerPopUse->setVisible(false);
	if(m_bResetUseSliderPos)
	{
		QuantitySlideBar* slideBar = (QuantitySlideBar*)m_ui.uiUse.pBtnConfirm->getParent()->getChildByTag(QUANTITY_SLIDEBAR_USE_TAG);
		slideBar->setPositionY(slideBar->getPositionY() + 1000);	
	}
	if (m_pCurrentTool && m_pCurrentTool->getNum() > amount)
	{
		//ShowDetail( m_CurrentToolId, m_pCurrentTool->getNum() - amount);
		m_ui.uiConsume.pTextNum->setString(DataManager::getInstance()->searchCommonTexdtById(20099)+
			_TO_STR(m_pCurrentTool->getNum() - amount)+DataManager::getInstance()->searchCommonTexdtById(20077));
		m_pCurrentTool->setNumEx(m_pCurrentTool->getNum() - amount);
	}
	else
	{
		m_CurrentToolId = 0;
		//ShowToolData(m_toolType);
	}

	ShowToolData(m_toolType, false);

	//如果使用的是宝箱，展示宝箱掉落
	if(ToolModel::getInstance()->getIsUsedBox())
	{
		showBoxInfo();

		//重置记录的宝箱数据状态
		ToolModel::getInstance()->setIsUsedBox(false);
		sBOX_TOOL_INFO boxInfo;
		ToolModel::getInstance()->clearBoxInfo();
	}

	//显示情缘好感度提示条
	//CustomTips::show("使用成功！");
}

void LayerTool::onRecvSellToolsData(int amount)
{
	//刷新界面
	m_ui.pLayerPopSell->setVisible(false);
	if(m_bResetSellSliderPos)
	{
		QuantitySlideBar* slideBar = (QuantitySlideBar*)m_ui.uiSell.pBtnConfirm->getParent()->getChildByTag(QUANTITY_SLIDEBAR_SELL_TAG);
		slideBar->setPositionY(slideBar->getPositionY() + 1000);
	}	
	if (m_pCurrentTool && m_pCurrentTool->getNum() > amount)
	{
		ShowDetail( m_CurrentToolId, m_pCurrentTool->getNum() - amount);
		m_pCurrentTool->setNumEx(m_pCurrentTool->getNum() - amount);
	}
	else
	{
		m_CurrentToolId = 0;
		ShowToolData(m_toolType);
	}

	//mark
	//CustomTips::show("出售道具成功");
}

void LayerTool::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;
	switch (obParam->id)
	{
	case nMAIN_CMD_TOOL_USE:
		{
			int useCount = *((int*)obParam->updateData);
			onRecvUseToolData(useCount);
		}
		break;
	case nMAIN_CMD_RECYCLE_SHOP_TOOLS:
		{
			int sellCount = *((int*)obParam->updateData);
			onRecvSellToolsData(sellCount);
		}
		break;
	default:
		break;
	}
	
}

void LayerTool::ClearToolData()
{
	//m_ui.pScrollView->removeAllChildren();
	Vector<Node*>& children = m_ui.pScrollView->getChildren();
	for (auto child:children)
	{
		child->setVisible(false);
	}
}
void LayerTool::ShowToolData(TOOL_TYPE type, bool isFlushDetail)
{
	m_toolType = type;
	if (m_ui.pLayerDetailConsume->isVisible() && isFlushDetail)
	{
		outShowDetail(m_ui.uiConsume.pImgTitle, m_ui.uiConsume.vecPosImgTitle
			, m_ui.uiConsume.pImgDetail, m_ui.uiConsume.vecPosImgDetail, false);
	}
	if (m_ui.pLayerDetailEquip->isVisible() && isFlushDetail)
	{
		outShowDetail(m_ui.uiEquip.pImgTitle, m_ui.uiEquip.vecPosImgTitle
			, m_ui.uiEquip.pImgDetail, m_ui.uiEquip.vecPosImgDetail, false);
	}	

	bool isFirst = true;
	if (m_ui.pScrollView->getChildrenCount()>0)
	{
		isFirst = false;
	}
	
	ClearToolData();
	std::vector<TOOL_INFO*> vecTools;
	//显示道具
	const TOOL_INFO_MAP& toolList = ToolModel::getInstance()->GetAllToolInfo();
	if (toolList.size() == 0)
	{
		return;
	}

	//查找分类道具
	for(auto iter=toolList.begin(); iter != toolList.end(); iter++)
	{
		RowData *pRowData = DataManager::getInstance()->searchToolById(iter->second.ntemplateId);
		if (pRowData == nullptr)
		{
			continue;
		}

		//道具分类
		int locat = iter->second.nLocat;
		
		TOOL_ITEM_TYPE itemtooltype = (TOOL_ITEM_TYPE)iter->second.nItemType;

		if (TOOL_TYPE_ALL == type)//显示全部
		{
			//只显示分类为消费道具、装备、英雄碎片, 装备只显示未佩戴的装备
			if (locat == TOOL_TYPE_CONSUME || locat == TOOL_TYPE_MATERIAL || locat == TOOL_TYPE_HERO_CHIP)
			{
				TOOL_INFO* pToolInfo = (TOOL_INFO*)&iter->second;
				vecTools.push_back(pToolInfo);
				continue;
			}
		}
		else if (type == TOOL_TYPE_CONSUME || type == TOOL_TYPE_MATERIAL || type == TOOL_TYPE_HERO_CHIP)
		{
			if (locat == type)
			{
				TOOL_INFO* pToolInfo = (TOOL_INFO*)&iter->second;
				vecTools.push_back(pToolInfo);
				continue;
			}
		}
	}

	//	排序
	if (type == TOOL_TYPE_CONSUME)//1-消耗品
	{
		//1.	点击使用后直接使用道具增加其他数值
		//2.	点击道具图标时，打开道具详细说明界面
		ToolModel::getInstance()->sortConsumeTools(vecTools);		
	}
	else if (type == TOOL_TYPE_ALL)//全部
	{		
		ToolModel::getInstance()->sortAllTools(vecTools);
	}
	else if (type == TOOL_TYPE_HERO_CHIP)//4-英雄
	{
		ToolModel::getInstance()->sortHeroToolsChip(vecTools);
	}
	else if (type == TOOL_TYPE_MATERIAL)
	{
		ToolModel::getInstance()->sortMaterialTools(vecTools);
	}
	
	//计算显示大小
	int amount = 0;
	for(TOOL_INFO* toolInfo : vecTools)
	{
		int maxSta = toolInfo->nMaxSta;//maxSta 占用一个格子空间可放置的道具数量
		if (toolInfo->nstack > maxSta)
		{
			int nCount = toolInfo->nstack;
			while (nCount > 0)//堆叠数量超过最大限制要增加显示
			{
				if (nCount > maxSta)
				{
					amount ++; //增加显示位置
				}
				nCount-=maxSta;
			}
		}
		amount++;
	}
	//设置ScrollView显示大小
	cocos2d::Size  scroolviewSize = m_ui.pScrollView->getContentSize();
	scroolviewSize.height = (MAX_TOOL_ICON_HEIGHT + 5)*(amount/4 + ((amount%4==0)?0:1));// + MAX_TOOL_ITEM_HEIGHT
	scroolviewSize.height = scroolviewSize.height > m_ui.pScrollView->getContentSize().height?scroolviewSize.height:m_ui.pScrollView->getContentSize().height;
	m_ui.pScrollView->setInnerContainerSize(scroolviewSize);

	// 滚动条;
	m_ui.listHeroBar->setScrollBarLengthPercent(m_ui.pScrollView->getContentSize().height / m_ui.pScrollView->getInnerContainerSize().height);
	m_ui.listHeroBar->setScrollBarPercentY(0.0f);

	int index = 0;
	for(TOOL_INFO* toolInfo : vecTools)
	{		
		ShowToolData(toolInfo->nid, toolInfo, type, index, isFirst);			
		index++;
	}
	//
	m_ui.pScrollView->jumpToTop();
	if (vecTools.size()<=0)
	{
		return;
	}
	
	//默认显示第一个详情
	if (m_CurrentToolId <= 0)
	{
		auto iter = vecTools.begin();
		TOOL_INFO* firstTool = (*iter);
		int maxSta = firstTool->nMaxSta;
		m_CurrentToolId = firstTool->nid;
		if (firstTool->nstack > maxSta)
			ShowDetail(m_CurrentToolId, maxSta);
		else
			ShowDetail(m_CurrentToolId, firstTool->nstack);
		const Vector<Node*>& children = m_ui.pScrollView->getChildren();
		//找到第一个icon，设置选中状态
		for(auto child:children)
		{	
			if (child->isVisible())
			{
				UIToolHeader* pIcon = (UIToolHeader*)child;	
				if (pIcon->getToolId() == m_CurrentToolId && pIcon->getToolDisIndex() == 0)
				{
					m_pCurrentTool = pIcon;			//默认当前选中的是第一个道具
					pIcon->setSelected(true);
					//break;
				}
				else
				{
					pIcon->setSelected(false);
				}
			}
		}		
	}
}

void LayerTool::showBoxInfo()
{
	vector<sBOX_TOOL_INFO> boxInfo = ToolModel::getInstance()->getUsedBoxInfo();
	vector<PopRewardItem*> vecRewardItem;
	
	for(auto info : boxInfo)
	{
		if(info.nid == TOOL_ID_GOLD)
		{
			PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(info.num);
			vecRewardItem.push_back(rewardItem);
		}
		else if(info.nid == TOOL_ID_COIN)
		{
			PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(info.num);
			vecRewardItem.push_back(rewardItem);
		}
		else
		{
			PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(info.nid, info.num);
			vecRewardItem.push_back(rewardItem);
		}
	}

	PopReward::show(vecRewardItem);

}

void LayerTool::ShowToolData(const int& id, const TOOL_INFO* pToolInfo, TOOL_TYPE type, int& index, bool isFirst)
{	
	//堆叠数量超过最大限制要增加显示	
	int maxSta = pToolInfo->nMaxSta;//maxSta 占用一个格子空间可放置的道具数量		 
	//
	if (pToolInfo->nstack > maxSta)
	{
		int nCount = pToolInfo->nstack;
		int disIndex = 0;
		while (nCount > 0)
		{
			if (nCount>maxSta)
			{
				UIToolHeader* pUIToolIcon = addToolIcon(id, pToolInfo, maxSta, disIndex, isFirst);
				setSingleToolIcon(pUIToolIcon, index, id);
				index ++; //要增加显示位置
				disIndex ++;
			}
			else
			{
				UIToolHeader* pUIToolIcon = addToolIcon(id, pToolInfo, nCount, disIndex, isFirst);
				setSingleToolIcon(pUIToolIcon, index, id);
			}
			nCount-=maxSta;
		}	
	}
	else
	{
		UIToolHeader* pUIToolIcon = addToolIcon(id, pToolInfo, pToolInfo->nstack, 0,isFirst);
		setSingleToolIcon(pUIToolIcon, index, id);
	}
}

//
void LayerTool::setSingleToolIcon(UIToolHeader* pUIToolIcon, const int& index, const int& id)
{
	if (pUIToolIcon != nullptr)
	{
		//从上往下显示
		float fY = m_ui.pScrollView->getInnerContainerSize().height - MAX_TOOL_ICON_HEIGHT/2 - index/4*(MAX_TOOL_ICON_HEIGHT + 5);
		pUIToolIcon->setPosition(cocos2d::Vec2(MAX_TOOL_ICON_WIDTH/2 + MAX_TOOL_ICON_WIDTH*(index%4)+5, fY));		
	}
}

UIToolHeader* LayerTool::addToolIcon(const int& id, const TOOL_INFO* pToolInfo, const int& count, const int& disIndex, bool isFirst)
{
	UIToolHeader* pUITool = nullptr;
	if (!isFirst)
	{
		pUITool = getToolIcon(id, disIndex);
	}
	if (pUITool == nullptr)
	{
		pUITool = UIToolHeader::create( ResourceUtils::getSmallIconPath(pToolInfo->nResId) );
		pUITool->setToolId(id);
		pUITool->setIconQua(pToolInfo->nItemQua);
		pUITool->setToolDisIndex(disIndex);
		m_ui.pScrollView->addChild(pUITool, 5);
		pUITool->setTag(id);//id作为标记
		pUITool->setScale(0.87);
		pUITool->addTouchEventListener(CC_CALLBACK_2(LayerTool::onBtnToolIcon, this, id));		
		pUITool->setTouchEnabled(true);
	}	
	else
	{
		pUITool->setVisible(true);
	}
	SetToolIconAttribute(id, pUITool, pToolInfo, count);
	return pUITool;
}

UIToolHeader* LayerTool::getToolIcon(const int& id, const int& disIndex)
{
	const Vector<Node*>& children = m_ui.pScrollView->getChildren();
	for (auto child:children)
	{
		UIToolHeader* pTool = dynamic_cast<UIToolHeader*>(child);
		if (pTool->getToolId() == id && pTool->getToolDisIndex() == disIndex)
		{
			return pTool;
		}		
	}
	return nullptr;
}

void LayerTool::SetToolIconAttribute(const int& id, UIToolHeader* pUIToolIcon, const TOOL_INFO* pToolInfo, const int& count, bool isDetail)
{
	//
	if (pToolInfo->nItemType == TOOL_ITEM_TYPE_EQUIP )
	{
		pUIToolIcon->setLvl(pToolInfo->nadvLvl);//强化等级
		pUIToolIcon->setEquipStars(pToolInfo->nItemStars);	//装备星级
	}
	else if (pToolInfo->nItemType == TOOL_ITEM_TYPE_HERO_CHIP)
	{
		pUIToolIcon->checkChip(pToolInfo->ntemplateId);
		pUIToolIcon->setNumEx(count);
	}
	else // if (itemTyp == TOOL_ITEM_TYPE_CONSUME )
	{
		pUIToolIcon->setNumEx(count);
	}
}

//
void LayerTool::ShowDetail(const int& toolId, const int& count, bool isShowAnimi)
{
	const TOOL_INFO* pToolInfo = ToolModel::getInstance()->GetToolInfo(toolId);
	if (pToolInfo == nullptr)
	{
		return;
	}
	//显示详情
	RowData *pRowData = DataManager::getInstance()->searchToolById(pToolInfo->ntemplateId);
	if (count == 0)
	{
		m_nDetailDisCount = 1;
	}
	else
	{
		m_nDetailDisCount = count;
	}	

	TOOL_TYPE type = TOOL_TYPE(pToolInfo->nLocat);
	TOOL_ITEM_TYPE itemType = (TOOL_ITEM_TYPE)pToolInfo->nItemType;


	//消耗道具， 英雄级别高于使用级别才可以使用
	int usaLvl = pRowData->getIntData("usaLvl");
	int useTyp = pRowData->getIntData("useTyp");//是否可使用	0-背包中可使用		1-功能场景下使用	
	bool useBtnVisible = (useTyp == 0 && HeroModel::getInstance()->getMainHeroLevel() >= usaLvl);
	m_ui.pBtnUse->setVisible(useBtnVisible);

	//1-银币回收		2-不可回收
	int recTyp = pRowData->getIntData("recTyp");
	int recPri = pRowData->getIntData("recPri");
	if (recTyp == 1 && recPri > 0)
	{
		m_ui.pImgPrice->setVisible(true);
		m_ui.pBtnSell->setVisible(true);
		m_ui.pTextCostNum->setText(TO_STR(recPri));
	}
	else
	{
		m_ui.pImgPrice->setVisible(false);
		m_ui.pBtnSell->setVisible(false);
	}

	//暂时屏蔽跳过按钮
	m_ui.pBtnJump->setVisible(false);

	//重新设置售出、使用位置
	if (m_ui.pBtnUse->isVisible() && m_ui.pBtnSell->isVisible())
	{
		m_ui.pBtnUse->setPosition(m_ui.pointPosUse);
		m_ui.pBtnSell->setPosition(m_ui.pointPosSell);
	}
	else if (m_ui.pBtnUse->isVisible() && !m_ui.pBtnSell->isVisible())
	{
		//居中
		m_ui.pBtnUse->setPositionX(m_ui.pBtnPnl->getContentSize().width / 2);
	}
	else if (!m_ui.pBtnUse->isVisible() && m_ui.pBtnSell->isVisible())
	{
		m_ui.pBtnSell->setPositionX(m_ui.pBtnPnl->getContentSize().width / 2);
	}

	//根据道具类型显示信息
	if (type == TOOL_TYPE_EQUIP)
	{
		RowData *pEquipData = DataManager::getInstance()->searchEquipmentById(pToolInfo->ntemplateId);	
		if (pEquipData == nullptr)
		{
			return;
		}
		m_ui.pLayerDetailConsume->setVisible(false);
		m_ui.pLayerDetailEquip->setVisible(true);		
		m_ui.uiEquip.pHeader->setMainIcon(ToolModel::getInstance()->GetToolImgFilenameByResId(pToolInfo->nResId));
		m_ui.uiEquip.pHeader->setIconQua(pToolInfo->nItemQua);
		m_ui.uiEquip.pHeader->setEquipStars(pToolInfo->nItemStars);
		m_ui.uiEquip.pTextName->setString(pRowData->getStringData("itemName"));
		m_ui.uiEquip.pTextNum->setString(DataManager::getInstance()->searchCommonTexdtById(20100)+
			_TO_STR(pToolInfo->nadvLvl)+DataManager::getInstance()->searchCommonTexdtById(20101));//pToolInfo->nstack
		
		QH_Param qhParam;
		SmithyModel::getInstance()->getQHParam(toolId, qhParam);
		if (qhParam.vcBase.size() >= 2)
		{
			if (qhParam.vcBase[0].attrType < ATTR_TYPE_HP_COE)
			{
				m_ui.uiEquip.pTextAtrri1->setString(STR_UTF8(qhParam.vcBase[0].strAttrName + "  " + TO_STR((int)(qhParam.vcBase[0].fAttrValue))));
			}
			else
			{
				m_ui.uiEquip.pTextAtrri1->setString(STR_UTF8(qhParam.vcBase[0].strAttrName + "  " + _F_TO_STR(qhParam.vcBase[0].fAttrValue * 100) + "%"));
			}

			if (qhParam.vcBase[0].attrType < ATTR_TYPE_HP_COE)
			{
				m_ui.uiEquip.pTextAtrri2->setString(STR_UTF8(qhParam.vcBase[1].strAttrName + "  " + TO_STR((int)(qhParam.vcBase[1].fAttrValue))));
			}
			else
			{
				m_ui.uiEquip.pTextAtrri2->setString(STR_UTF8(qhParam.vcBase[1].strAttrName + "  " + _F_TO_STR(qhParam.vcBase[1].fAttrValue * 100) + "%"));
			}
		}

		if (isShowAnimi)
			outShowDetail(m_ui.uiEquip.pImgTitle, m_ui.uiEquip.vecPosImgTitle, m_ui.uiEquip.pImgDetail, 
			m_ui.uiEquip.vecPosImgDetail, true);
	}	
	else//消耗、碎片
	{
		m_ui.pLayerDetailEquip->setVisible(false);
		m_ui.pLayerDetailConsume->setVisible(true);
		m_ui.uiConsume.pHeader->setMainIcon(ToolModel::getInstance()->GetToolImgFilenameByResId(pToolInfo->nResId));
		m_ui.uiConsume.pHeader->checkChip(pToolInfo->ntemplateId);
		m_ui.uiConsume.pHeader->setIconQua(pToolInfo->nItemQua);
		m_ui.uiConsume.pTextName->setText(pRowData->getStringData("itemName"));
		m_ui.uiConsume.pTextDetail->setText(pRowData->getStringData("itemInfo"));
		m_ui.uiConsume.pTextNum->setText(DataManager::getInstance()->searchCommonTexdtById(20099)+
			_TO_STR(count)+DataManager::getInstance()->searchCommonTexdtById(20077));

		if (isShowAnimi)
			outShowDetail(m_ui.uiConsume.pImgTitle, m_ui.uiConsume.vecPosImgTitle, m_ui.uiConsume.pImgDetail, m_ui.uiConsume.vecPosImgDetail, true);
	}
}

void LayerTool::onBtnToolIcon(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int id)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		UIToolHeader* tool = (UIToolHeader*)sender;
		//清除选中项
		Vector<Node*>& listItems = m_ui.pScrollView->getChildren();
		for(auto iter = listItems.begin();iter != listItems.end(); iter++ )
		{
			UIToolHeader* pHeader = (UIToolHeader*)*iter;
			if (pHeader!= nullptr )
			{
				if (pHeader->isSelected())
				{
					pHeader->setSelected(false);
				}
			}
		}

		UIToolHeader* pUIToolItem = (UIToolHeader*)sender;
		pUIToolItem->setSelected(true);
		m_CurrentToolId = pUIToolItem->getTag();

		m_pCurrentTool = pUIToolItem;
		//显示详情
		ShowDetail(m_CurrentToolId, pUIToolItem->getNum());
	}
}

//出售
void LayerTool::onBtnToolDetailSell(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		const TOOL_INFO *pInfo = ToolModel::getInstance()->GetToolInfo(m_CurrentToolId);
		if (pInfo != nullptr)
		{
			m_nsellNum = 1;
			m_nsellMaxNum = m_nDetailDisCount;
			RowData *pRowData = DataManager::getInstance()->searchToolById(pInfo->ntemplateId);

			CCASSERT(pRowData != nullptr, "LayerTool::onBtnToolDetailSell:pRowData==nullptr");

			//if (pInfo->nstack > 1)
			if(m_pCurrentTool != nullptr)
			{
				m_curGroupToolNum = m_pCurrentTool->getNum();
			}
			
			if(m_curGroupToolNum > 1)
			{
				m_ui.pLayerPopSell->setVisible(true);
				m_ui.uiSell.pHeader->setMainIcon(ResourceUtils::getSmallIconPath(pInfo->nResId));
				m_ui.uiSell.pHeader->setIconQua(pInfo->nItemQua);
				m_ui.uiSell.pTextName->setText(pRowData->getStringData("itemName"));
				//m_ui.uiSell.pTextNum->setText(STR_UTF8("数量：") + _TO_STR(m_nDetailDisCount));//pInfo->nstack
				m_ui.uiSell.pTextNum->setText(DataManager::getInstance()->searchCommonTexdtById(20102) + _TO_STR(m_curGroupToolNum));
				

				//1-银币回收		2-不可回收
				int recTyp = pRowData->getIntData("recTyp");
				int recPri = pRowData->getIntData("recPri");
				if (recTyp == 1 && recPri > 0)
				{
					m_ui.uiSell.pTextTotleNum->setText(StringFormat::intToStr(recPri));
					m_nsellPrice = recPri;
				}
				else
				{
					m_nsellPrice = 0;
				}

				//设置数量条最大值
				QuantitySlideBar* slideBar = (QuantitySlideBar*)m_ui.uiSell.pBtnConfirm->getParent()->getChildByTag(QUANTITY_SLIDEBAR_SELL_TAG);
				slideBar->setPositionY(slideBar->getPositionY() - 1000);		
				//slideBar->setMaxQuantity(m_nsellMaxNum);
				slideBar->setMaxQuantity(m_curGroupToolNum);
				slideBar->initDivisorText(m_ui.uiSell.pTextTotleNum, m_nsellPrice);
			}
			//else if (pInfo->nstack == 1)
			else if(m_curGroupToolNum == 1)
			{
				//道具数量为1，则直接弹窗提示是否出售
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20103), [this, pInfo](Ref* pSender, CustomRetType type)->void
				{
					switch (type)
					{
					case RET_TYPE_OK:
						{
							ToolController::sendSellToolsMsg(pInfo->nid, 1);
							m_bResetSellSliderPos = false;
						}
						break;
					case RET_TYPE_CANCEL:
					case RET_TYPE_CLOSE:
						break;
					default:
						break;
					}

				}, CUSTOM_YES_NO);
			}
		}	
	}
}

//使用
void LayerTool::onBtnToolDetailUse(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		const TOOL_INFO *pInfo = ToolModel::getInstance()->GetToolInfo(m_CurrentToolId);

		if (pInfo != nullptr)
		{		
			mUseNum = 1;
			RowData *pRowData = DataManager::getInstance()->searchToolById(pInfo->ntemplateId);

			CCASSERT(pRowData != nullptr, "LayerTool::onBtnToolDetailUse:pRowData==nullptr");
			//if (pInfo->nstack > 1)
			if(m_pCurrentTool != nullptr)
			{
				m_curGroupToolNum = m_pCurrentTool->getNum();
			}
			if(m_curGroupToolNum > 1)
			{
				m_ui.pLayerPopUse->setVisible(true);
				m_ui.uiUse.pHeader->setMainIcon(ResourceUtils::getSmallIconPath(pInfo->nResId));
				m_ui.uiUse.pHeader->setIconQua(pInfo->nItemQua);
				m_ui.uiUse.pTextName->setText(pRowData->getStringData("itemName"));
				//m_ui.uiUse.pTextNum->setText(STR_UTF8("数量：") + _TO_STR(m_nDetailDisCount));
				m_ui.uiUse.pTextNum->setText(DataManager::getInstance()->searchCommonTexdtById(20102) + _TO_STR(m_curGroupToolNum));

				//设置数量条最大值
				QuantitySlideBar* slideBar = (QuantitySlideBar*)m_ui.uiUse.pBtnConfirm->getParent()->getChildByTag(QUANTITY_SLIDEBAR_USE_TAG);				
				slideBar->setPositionY(slideBar->getPositionY() - 1000);
				//slideBar->setMaxQuantity(m_nDetailDisCount);
				slideBar->setMaxQuantity(m_curGroupToolNum);
			}
			//else if (pInfo->nstack == 1)
			else if(m_curGroupToolNum == 1)
			{
				//数量为1，直接发送使用道具消息
				ToolController::sendUseToolsMsg(pInfo->nid, 1);
				if(pRowData->getIntData("itemTyp") == 7)
				{
					ToolModel::getInstance()->setIsUsedBox(true);
				}						
				m_bResetUseSliderPos = false;
			}

		}
	}
}

void LayerTool::onBtnEquipDetail(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		//跳转到铁匠铺界面
		const TOOL_INFO *pInfo = ToolModel::getInstance()->GetToolInfo(m_CurrentToolId);
		if (pInfo != nullptr)
		{
			RowData *pEquipData = DataManager::getInstance()->searchEquipmentById(pInfo->ntemplateId);
			if (pEquipData != nullptr)
			{
				//SceneController::getInstance()->setLastSceneId(ModuleSceneTools);
				//马书只能强化，出售；不能洗练、进阶
				int typeEquip = pEquipData->getIntData("partTyp");
				/*LayerSmithy* pSmithy = LayerSmithy::create();
				this->addChild(pSmithy);	
				pSmithy->showEquip((EQUIP_ITEM_TYPE)typeEquip, m_CurrentToolId);//, pInfo->nwarePetId*/
				CustomPop::show("come back soon");
			}		
		}
	}
}

void LayerTool::onBtnSellClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		m_ui.pLayerPopSell->setVisible(false);
		QuantitySlideBar* slideBar = (QuantitySlideBar*)m_ui.uiSell.pBtnConfirm->getParent()->getChildByTag(QUANTITY_SLIDEBAR_SELL_TAG);
		slideBar->setPositionY(slideBar->getPositionY() + 1000);
	}
}

void LayerTool::onBtnSellConfrim(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		const TOOL_INFO *pInfo = ToolModel::getInstance()->GetToolInfo(m_CurrentToolId);
		QuantitySlideBar* slideBar = (QuantitySlideBar*)m_ui.uiSell.pBtnConfirm->getParent()->getChildByTag(QUANTITY_SLIDEBAR_SELL_TAG);
		m_nsellNum = slideBar->getCurrentQuantity();
		if (pInfo != nullptr)
		{
			ToolController::sendSellToolsMsg(pInfo->nid, m_nsellNum);
			m_bResetSellSliderPos = true;
		}
	}
}

void LayerTool::onBtnUseClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		m_ui.pLayerPopUse->setVisible(false);
		QuantitySlideBar* slideBar = (QuantitySlideBar*)m_ui.uiUse.pBtnConfirm->getParent()->getChildByTag(QUANTITY_SLIDEBAR_USE_TAG);
		slideBar->setPositionY(slideBar->getPositionY() + 1000);
	}
}

void LayerTool::onBtnUseConfrim(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		const TOOL_INFO *pInfo = ToolModel::getInstance()->GetToolInfo(m_CurrentToolId);
		QuantitySlideBar* slideBar = (QuantitySlideBar*)m_ui.uiUse.pBtnConfirm->getParent()->getChildByTag(QUANTITY_SLIDEBAR_USE_TAG);
		mUseNum = slideBar->getCurrentQuantity();
		if (pInfo != nullptr)
		{
			ToolController::sendUseToolsMsg(pInfo->nid, mUseNum);
			if(pInfo->nItemType == TOOL_ITEM_TYPE_CHEST)
			{
				ToolModel::getInstance()->setIsUsedBox(true);
			}	
			m_bResetUseSliderPos = true;
		}
	}
}

void LayerTool::onTopBtnClick(Ref* sender, cocos2d::ui::Widget::TouchEventType type, TOOL_TOP_BTN_TAG tag)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switch (tag)
		{
		case TOOL_TOP_BTN_TAG::POWER:
			{
				LayerCommonBuy* _layer = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
				this->addChild(_layer);
			}
			break;
		case TOOL_TOP_BTN_TAG::ENERGY:
			{
				LayerCommonBuy* _layer = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_ENERGY);
				this->addChild(_layer);
			}
			break;
		case TOOL_TOP_BTN_TAG::GOLD:
			break;
		case TOOL_TOP_BTN_TAG::SILVER:
			{
				LayerCommonBuy* _layer = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
				this->addChild(_layer);
			}
			break;
		default:
			break;
		}
	}
}

//动画显示详情页
void LayerTool::outShowDetail(Node* pTitle, cocos2d::Vec2 vecPosTitle, Node* pDetail, cocos2d::Vec2 vecPosDetail, bool isOut)
{
	isShowDetailOut = isOut;
	float fTime = 0.3;
	MoveTo *pMove = nullptr;
	pDetail->setVisible(false);
	//标题
	if (isOut)
	{
		pTitle->setPosition(cocos2d::Vec2(vecPosTitle.x+500, vecPosTitle.y));
		pMove = MoveTo::create(fTime, vecPosTitle);
	}
	else
	{
		pTitle->setPosition(vecPosTitle);
		pMove = MoveTo::create(fTime, cocos2d::Vec2(vecPosTitle.x+500, vecPosTitle.y));
	}
	EaseBackOut * easeBack = EaseBackOut::create(pMove);
	//CCCallFuncND *pCallFuncN = CCCallFuncND::create(this, cocos2d::SEL_CallFuncND(&LayerTool::onMoveTitleCallFunN), (void*)pDetail);
	Sequence *actionSequence = Sequence::create(easeBack, nullptr);//  按序列执行动作，这会让节点连续执行几个动作。
	pTitle->stopAllActions();
	pTitle->runAction(actionSequence);
	//详情信息
	MoveTo *pMove2 = nullptr;
	DelayTime *delayTime = DelayTime::create(fTime);
	if (isOut)
	{
		pDetail->setPosition(cocos2d::Vec2(vecPosDetail.x+500, vecPosDetail.y));
		pMove2 = MoveTo::create(fTime, vecPosDetail);
	}
	else
	{
		pDetail->setPosition(vecPosDetail);
		pMove2 = MoveTo::create(fTime, cocos2d::Vec2(vecPosDetail.x+500, vecPosDetail.y));
	}
	auto visibleAction = Show::create();
	EaseBackOut * easeBack2 = EaseBackOut::create(pMove2);
	CCCallFuncND *pCallFuncN2 = CCCallFuncND::create(this, cocos2d::SEL_CallFuncND(&LayerTool::onMoveTitleCallFunN), (void*)pDetail);
	Sequence *actionSequence2 = Sequence::create(delayTime, visibleAction, easeBack2, pCallFuncN2, nullptr);//  按序列执行动作，这会让节点连续执行几个动作。
	pDetail->stopAllActions();
	pDetail->runAction(actionSequence2);
}

void LayerTool::onMoveTitleCallFunN(cocos2d::Node* pNode, void* pPram)
{
	if (!isShowDetailOut)
	{
		m_ui.pLayerDetailConsume->setVisible(false);
		m_ui.pLayerDetailEquip->setVisible(false);
	}
}