#include "LayerEquipSource.h"
#include "CocoStudio.h"
#include "SmithyModel.h"
#include "SmithyController.h"
#include "Common/Common.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "ResourceManager/ResourceLoader.h"
#include "Utils/ResourceUtils.h"
#include "UI/UIDef.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/UIToolHeader.h"
#include "HeroModule/HeroModel.h"
#include "../MainModule/MainModel.h"
#include "../ToolModule/LayerTool.h"
#include "GuideCenter/GuideManager.h"
#include "UIEquipSrcItem.h"
#include "HeroModule/LayerHero.h"
#include "HeroModule/SceneHero.h"
#include "TakeCardModule/SceneTakeCard.h"
#include "RobModule/SceneRob.h"
#include "ChapterModule/SceneChapterLoading.h"
#include "ChapterModule/ChapterModel.h"
#include "TowerModule/SceneTower.h"
#include "LoveModule/LoveScene.h"
#include "MazeModule/SceneMaze.h"
#include "PvpModule/PvpScene.h"
#include "RefineryModule/RefineryScene.h"
#include "ChapterModule/Activity/SceneActivity.h"
#include "RefineryModule/RefineryModel.h"
#include "MazeModule/MazeModel.h"
#include "TakeCardModule/TakeCardModel.h"
#include "NewShopModule/ShopScene.h"

using namespace cocos2d;
using namespace cocos2d::ui;

LayerEquipSource::LayerEquipSource()
{

}
LayerEquipSource::~LayerEquipSource()
{

}
bool LayerEquipSource::init()
{
	if(!ModalLayer::init())
	{
		return false;
	}	
	initUI();	
	return true;
}


void LayerEquipSource::initUI()
{
	Size size = Director::getInstance()->getWinSize();

	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIEquip/UIEquipSource.ExportJson"));
	this->addChild(m_ui.pRoot);

	m_ui.pnlEquipSource = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_EquipSource"));
	m_ui.btnClose = dynamic_cast<Button*>(m_ui.pnlEquipSource->getChildByName("Btn_Close"));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerEquipSource::onBtnClose, this));

	// 名字;
	m_ui.txtEquName = dynamic_cast<Text*>(m_ui.pnlEquipSource->getChildByName("Lab_Equ_Name"));
	m_ui.txtEquName->setFontName(FONT_FZZHENGHEI);

	// 列表;
	m_ui.scrollviewContent = dynamic_cast<ScrollView*>(m_ui.pnlEquipSource->getChildByName("ScrollView_Content"));
	m_ui.scrollviewContent->addEventListener((CC_CALLBACK_2(LayerEquipSource::onScrollViewEvent, this)));

	// 滚动条;
	m_ui.scrollbar = CustomScrollBar::create();
	m_ui.scrollbar->initScrollBar(m_ui.scrollviewContent->getContentSize().height);
	m_ui.scrollbar->setPosition(Vec2(385.0f, 30.0f));
	m_ui.pnlEquipSource->addChild(m_ui.scrollbar);
}

void LayerEquipSource::onExit()
{
	Layer::onExit();
}
void LayerEquipSource::onEnter()
{
	Layer::onEnter();
}


void LayerEquipSource::onBtnClose(Ref* sender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		//SmithyModel::getInstance()->resetToolSourceData();
		//SceneController::getInstance()->setLastSceneId(ModuleSceneNone);
		dispose();
	}
}

void LayerEquipSource::onBtnSrcItem(Ref* sender, cocos2d::ui::Widget::TouchEventType type, const int& method, const int& chapterId, const int& barrierId )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		dispose();
		switch (method)
		{
		case TOOL_DROP_SOURCE_SHOP:	//	1-商城
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_SHOP))
			{
				Director::getInstance()->pushScene(SceneTakeCard::create(true));
			}
			break;

		case TOOL_DROP_SOURCE_ROB:	//	2-掠夺
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_ROB))
			{
				Director::getInstance()->pushScene(SceneRob::create(true));
			}
			break;

		case TOOL_DROP_SOURCE_ACTIVE:	//	3-活动关卡
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE_EVENT))
			{
				Director::getInstance()->pushScene(SceneActivity::create(true));
			}
			break;

		case TOOL_DROP_SOURCE_TOWER:	//	4-千层塔
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TOWER))
			{
				Director::getInstance()->pushScene(SceneTower::create(TOWER_TYPE_INVALID, true));
			}
			break;
		case TOOL_DROP_SOURCE_TRIAL:	//	5-试练
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TRAIL))
			{
				Director::getInstance()->pushScene(ShopScene::createScene(ST_JIFENG, true));
			}
			break;
		case TOOL_DROP_SOURCE_RISK:			//	6-冒险
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE))
			{
				Director::getInstance()->pushScene(SceneChapterLoading::create(
					INVALID_CHAPTER_OR_BARRIER_ID, INVALID_CHAPTER_OR_BARRIER_ID, true, false));
			}
			break;

		case TOOL_DROP_SOURCE_STAGE:		//	7-关卡
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE))
			{
				UIEquipSrcItem* pItem = (UIEquipSrcItem*)sender;
				if (!pItem->isBarrierOpen())
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20172));
				}
				else
				{
					ChapterModel::getInstance()->setJumpType(CH_JUMP_TYPE_ELSE);
					Director::getInstance()->pushScene(SceneChapterLoading::create(
						chapterId, barrierId, true, true));
				}
			
			}
			break;
		case TOOL_DROP_SOURCE_LOVE:			//	8-情缘
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_LOVE))
			{
				Director::getInstance()->pushScene(LoveScene::create(true));
			}
			break;

		case TOOL_DROP_SOURCE_STAR_PLATE:	//	9-星盘(因星盘也有来源，暂不支持，会出bug)
			break;

		case TOOL_DROP_SOURCE_MAZE:			//	10-迷宫
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_MAZE))
			{
				Director::getInstance()->pushScene(SceneMaze::create(true));
			}
			break;

		case TOOL_DROP_SOURCE_PVP:			//	11-竞技场
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVP))
			{
				Director::getInstance()->pushScene(ShopScene::createScene(ST_RONGYU, true));
			}
			break;

		case TOOL_DROP_SOURCE_SHOP_BUY:		//	12-商城(购买)
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_SHOP))
			{
				Director::getInstance()->pushScene(SceneTakeCard::create(true));
			}
			break;

		case TOOL_DROP_SOURCE_REFINE:		//	13-炼化炉
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_REFINE))
			{
				Director::getInstance()->pushScene(RefineryScene::create(true));
			}
			break;

		case TOOL_DROP_SOURCE_GROUP:		//	14-帮派
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_FACTION))
			{
				if (MainModel::getInstance()->getMainParam()->mGroupId > 0)
				{
					Director::getInstance()->pushScene(ShopScene::createScene(ST_SEHGNWANG, true));
				}
				else
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20173));
				}
			}
			break;

		case TOOL_DROP_SOURCE_CL_SHOP:		//	15-淬炼商店
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_EQUIP_CL))
			{
				Director::getInstance()->pushScene(ShopScene::createScene(ST_NONE, true));
			}
			break;

		case TOOL_DROP_SOURCE_FIRST_RECHARGE:	//  16-首冲奖励(不跳转)
			break;
		case TOOL_DROP_SOURCE_VIP_PRIVILEGE:	//  17-VIP特权礼包(不跳转)
			break;
		case TOOL_DROP_SOURCE_OPERATION:		//  18-运营活动(不跳转)
			break;

		case TOOL_DROP_SOURCE_DRUGSTORE:		//  19-杂货商店
			Director::getInstance()->pushScene(ShopScene::createScene(ST_ZAHUO, true));
			break;

		case TOOL_DROP_SOURCE_TRAIL_TREASURE:	//  20-试炼宝藏(不跳转)
			break;
		case TOOL_DROP_SOURCE_SECRET_SHOP:		//  21-神秘商店(暂无)
			break;

		default:
			break;
		}
	}
}

void LayerEquipSource::onScrollViewEvent(Ref* sender, ScrollView::EventType type)
{
	ScrollView* pScroll = (ScrollView*)sender;

	Size contentSize = pScroll->getContentSize();
	Size innerContenterSize = pScroll->getInnerContainerSize();

	if (contentSize.height >= innerContenterSize.height)
	{
		return;
	}

	switch (type)
	{
	case ScrollView::EventType::SCROLLING:
		{
			Vec2 vec = pScroll->getInnerContainer()->getPosition();
			float percentY = -vec.y / (innerContenterSize.height - pScroll->getContentSize().height);
			m_ui.scrollbar->setScrollBarPercentY((1.0f - percentY)*100.0f);
		}
		break;

	default:
		break;
	}
}

void LayerEquipSource::showSrc( const int& nToolTemplateId )
{
	// 取基础信息;
	RowData* _row = DataManager::getInstance()->searchToolById(nToolTemplateId);
	if (_row == nullptr)
		return;

	// 装备;
	UIToolHeader* _equ = UIToolHeader::create(nToolTemplateId);
	_equ->setPosition(Vec2(460.0f, 545.0f));
	m_ui.pnlEquipSource->addChild(_equ);
	m_ui.txtEquName->setText(_row->getStringData("itemName"));

	SmithyModel::getInstance()->loadItemSource(nToolTemplateId);

	// 计算高度;
	
	vector<EQUIP_SOURCE_ITEM>& equipSourceItems = SmithyModel::getInstance()->getEquipSourceItems();
	int allItemsHeight = equipSourceItems.size() * MAX_SRC_NORMAL_ITEM_HEIGHT;
	float height = (m_ui.scrollviewContent->getContentSize().height > allItemsHeight) ? m_ui.scrollviewContent->getContentSize().height : allItemsHeight;
	m_ui.scrollviewContent->setInnerContainerSize(Size(m_ui.scrollviewContent->getContentSize().width, height));

	// 滚动条;
	m_ui.scrollbar->setScrollBarLengthPercent(m_ui.scrollviewContent->getContentSize().height / m_ui.scrollviewContent->getInnerContainerSize().height);
	m_ui.scrollbar->setScrollBarPercentY(0.0f);

	float fitemHeight = 0.0;
	float fdisY = 0.0;
	for (auto scrItem : equipSourceItems)
	{
		string imgname = getSrcImgName(scrItem.dropMethod);
		UIEquipSrcItem *pEquipSrcItem = nullptr;

		if (scrItem.dropMethod == TOOL_DROP_SOURCE_SHOP || scrItem.dropMethod == TOOL_DROP_SOURCE_SHOP_BUY)
		{
			
			pEquipSrcItem = UIEquipSrcItem::create(imgname,
				"Image/UIEquip/icon/btn_qianwangshangcheng_1.png",
				"Image/UIEquip/icon/btn_qianwangshangcheng_2.png");
		}
		else
		{
			pEquipSrcItem = UIEquipSrcItem::create(imgname,
				"Image/UIEquip/icon/btn_putongqianwang_1.png",
				"Image/UIEquip/icon/btn_putongqianwang_2.png");

			if (scrItem.dropMethod == TOOL_DROP_SOURCE_STAGE)
			{
				pEquipSrcItem->setBarrierName(scrItem.barrierName);
				pEquipSrcItem->setChapterId(scrItem.chapterId);
				int nCount=0;
				string strReason;
				bool isBarrierAllowed = ChapterModel::getInstance()->isBarrierAllowed(scrItem.chapterId, scrItem.barrierId, nCount, strReason, true);
				if (!isBarrierAllowed)
				{
					pEquipSrcItem->setBarrierOpenFlag(false);
				}
			}
		}

		fitemHeight += MAX_SRC_NORMAL_ITEM_HEIGHT;
		fdisY = m_ui.scrollviewContent->getInnerContainerSize().height - fitemHeight + MAX_SRC_NORMAL_ITEM_HEIGHT / 2;
		pEquipSrcItem->setPosition(Vec2(pEquipSrcItem->getContentSize().width/2 + 20.0f, fdisY));
		pEquipSrcItem->addTouchEventListener(CC_CALLBACK_2(LayerEquipSource::onBtnSrcItem, this, scrItem.dropMethod, scrItem.chapterId, scrItem.barrierId));
		m_ui.scrollviewContent->addChild(pEquipSrcItem);
	}
}

void LayerEquipSource::clearAllItems()
{
	m_ui.scrollviewContent->removeAllChildren();
}

std::string LayerEquipSource::getSrcImgName(const int dropMethodId)
{
	switch (dropMethodId)
	{
	case TOOL_DROP_SOURCE_SHOP:
		return "Image/UIEquip/icon/img_qianwangshangcheng_di.png";

	case TOOL_DROP_SOURCE_ROB:			//	2-掠夺
		return "Image/UIEquip/icon/img_putongqianwang_lveduo.png";

	case TOOL_DROP_SOURCE_ACTIVE:		//	3-活动
		return "Image/UIEquip/icon/img_putongqianwang_huodongguanqia.png";

	case TOOL_DROP_SOURCE_TOWER:		//	4-千层塔
		return "Image/UIEquip/icon/img_putongqianwang_qiancengta.png";

	case TOOL_DROP_SOURCE_TRIAL:		//	5-试练
		return "Image/UIEquip/icon/img_putongqianwang_didishilian.png";

	case TOOL_DROP_SOURCE_RISK:			//	6-冒险
		return "Image/UIEquip/icon/img_putongqianwang_maoxian.png";

	case TOOL_DROP_SOURCE_STAGE:		//	7-关卡
		return "Image/UIEquip/icon/img_putongqianwang_guanqia.png";

	case TOOL_DROP_SOURCE_LOVE:			//	8-情缘
		return "Image/UIEquip/icon/img_putongqianwang_qingyuan.png";

	case TOOL_DROP_SOURCE_STAR_PLATE:	//	9-星盘(暂不支持，会出bug)
		return "Image/UIEquip/icon/img_putongqianwang_xingpan.png";

	case TOOL_DROP_SOURCE_MAZE:			//	10-迷宫
		return "Image/UIEquip/icon/img_putongqianwang_migong.png";

	case TOOL_DROP_SOURCE_PVP:			//	11-竞技场
		return "Image/UIEquip/icon/img_putongqianwang_rongyushangdian.png";

	case TOOL_DROP_SOURCE_SHOP_BUY:		//	12-商城(购买)
		return "Image/UIEquip/icon/img_qianwangshangcheng_di.png";

	case TOOL_DROP_SOURCE_REFINE:		//	13-炼化炉
		return "Image/UIEquip/icon/img_putongqianwang_lianhualu.png";

	case TOOL_DROP_SOURCE_GROUP:		//	14-帮派
		return "Image/UIEquip/icon/img_putongqianwang_shengwaishangdian.png";

	case TOOL_DROP_SOURCE_CL_SHOP:		//	15-淬炼商店
		return "Image/UIEquip/icon/img_putongqianwang_cuilianbaoxia.png";

	case TOOL_DROP_SOURCE_FIRST_RECHARGE:	// 16-首冲奖励;
		return "Image/UIEquip/icon/img_putongqianwang_shouchongjiangli.png";

	case TOOL_DROP_SOURCE_VIP_PRIVILEGE:	//  17-VIP特权礼包
		return "Image/UIEquip/icon/img_putongqianwang_viptequanlibao.png";

	case TOOL_DROP_SOURCE_OPERATION:	//  18-运营活动
		return "Image/UIEquip/icon/img_putongqianwang_yunyinghuodong.png";

	case TOOL_DROP_SOURCE_DRUGSTORE:	//  19-杂货商店
		return "Image/UIEquip/icon/img_putongqianwang_zahuoshangdian.png";

	case TOOL_DROP_SOURCE_TRAIL_TREASURE:	//  20-试炼宝藏
		return "Image/UIEquip/icon/img_putongqianwang_shilianbaozang.png";

	case TOOL_DROP_SOURCE_SECRET_SHOP:	//  21-神秘商店
		return "Image/UIEquip/icon/img_putongqianwang_shenmishangdian.png";
		
	default:
		break;
	}
	return "";
}
