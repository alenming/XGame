#include "LayerHeroInfo.h"
#include "ToolModule/UICardHead.h"
#include "Utils/StringFormat.h"
#include "DataManager/DataManager.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolModel.h"
#include "UI/UIDef.h"
#include "Common/Common.h"
#include "SmithyModule/SmithyModel.h"
#include "GuideCenter/GuideManager.h"
#include "TowerModule/SceneTower.h"
#include "TakeCardModule/SceneTakeCard.h"
#include "ChapterModule/SceneChapterLoading.h"
#include "ChapterModule/ChapterModel.h"
#include "RobModule/SceneRob.h"
#include "Common/Common.h"
#include "ChapterModule/ChapterModel.h"
#include "SmithyModule/LayerEquipSource.h"


LayerHeroInfo::LayerHeroInfo()
	: mTemplateId(-1)
	, mDebrisTemplateId(-1)
{

}

LayerHeroInfo::~LayerHeroInfo()
{

}

LayerHeroInfo* LayerHeroInfo::create( int templateId )
{
	LayerHeroInfo* layerHeroInfo = new LayerHeroInfo();

	if (layerHeroInfo && layerHeroInfo->init(templateId))
	{
		layerHeroInfo->autorelease();
		return layerHeroInfo;
	}
	else
	{
		delete layerHeroInfo;
		layerHeroInfo = nullptr;
		return layerHeroInfo;
	}
}

bool LayerHeroInfo::init(int templateId)
{
	if (!ModalLayer::init())
	{
		return false;
	}

	mTemplateId = templateId;
	initUI();
	return true;
}

void LayerHeroInfo::initUI()
{
	auto root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UINewhero/UINewhero_info.ExportJson"));
	this->addChild(root);

	mUI.root = root;

	mUI.iconPnl = dynamic_cast<Layout*>(Helper::seekWidgetByName(root, "Panel_suipian"));
	mUI.nameTxt = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "Label_daojuming"));
	mUI.debrisNumTxt = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "Label_shuliang"));
	mUI.desTxt = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "Label_miaoshu"));
	mUI.cardPnl = dynamic_cast<Layout*>(Helper::seekWidgetByName(root, "Panel_card"));
	mUI.closeBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(root, "Button_guanbi"));
	mUI.closeBtn->addTouchEventListener(CC_CALLBACK_2(LayerHeroInfo::onBtnClicked, this, HERO_BTN_ID::BACK));
	mUI.heroTag = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "Image_shuxing"));
	mUI.heroTxt = dynamic_cast<Text*>(Helper::seekWidgetByName(root, "Label_shuxingmiaoshu"));
	mUI.heroJob = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "Image_job"));
	//mUI.heroTalent = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(root, "AtlasLabel_22"));
	mUI.heroTalent = dynamic_cast<ImageView*>(Helper::seekWidgetByName(root, "Img_zizhiLv"));
	mUI.getWayBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(root, "Button_huoqutujing"));
	mUI.getWayBtn->addTouchEventListener(CC_CALLBACK_2(LayerHeroInfo::onBtnClicked, this, HERO_BTN_ID::BTN_GETWAY));

	mUI.nameTxt->setFontName(FONT_FZZHENGHEI);
	mUI.debrisNumTxt->setFontName(FONT_FZZHENGHEI);
	mUI.desTxt->setFontName(FONT_FZZHENGHEI);
	mUI.heroTxt->setFontName(FONT_FZZHENGHEI);

	//显示卡牌
	UICardHead* card = UICardHead::create(mTemplateId);
	card->setScale(0.9f);
	mUI.cardPnl->addChild(card);

	//显示碎片
	RowData* heroRowData = DataManager::getInstance()->searchToolById(mTemplateId);
	if (heroRowData)
	{
		mDebrisTemplateId = heroRowData->getIntData("transNeed");
		UIToolHeader* debrisIcon = UIToolHeader::create(mDebrisTemplateId);
		debrisIcon->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
		mUI.iconPnl->addChild(debrisIcon);

		//英雄描述
		string info = heroRowData->getStringData("itemInfo");
		mUI.desTxt->setString(info);

		RowData* debrisData = DataManager::getInstance()->searchToolById(mDebrisTemplateId);
		if (debrisData)
		{
			//碎片名称
			string name = debrisData->getStringData("itemName");
			mUI.nameTxt->setString(name);

			//碎片数量
			int num = ToolModel::getInstance()->searchHeroDebrisNum(mTemplateId);
			//mUI.debrisNumTxt->setString(STR_UTF8("拥有" + TO_STR(num) + "件"));
			mUI.debrisNumTxt->setString(TO_STR(num));
		}

		RowData* heroData = DataManager::getInstance()->searchCharacterById(mTemplateId);
		if(heroData == nullptr)
		{
			return;
		}
		//英雄资质
		//mUI.heroTalent->setString(_TO_STR(heroData->getIntData("apt")));
		mUI.heroTalent->loadTexture(ResourceUtils::getHeroTalentPath(heroData->getIntData("apt")));
		
		int heroType = heroData->getIntData("tagTyp");
		switch (heroType)
		{
		case HERO_PROPERTY_TYPE::HERO_STR: //火属性英雄
			{
				mUI.heroTag->loadTexture(ResourceUtils::getIconGlobalPath("img_shuxing11_li.png"));
				//mUI.heroTxt->setString(STR_UTF8("克制水属性英雄，造成额外伤害"));
			}
			break;
		case HERO_PROPERTY_TYPE::HERO_AGI: //木属性英雄
			{
				mUI.heroTag->loadTexture(ResourceUtils::getIconGlobalPath("img_shuxing22_min.png"));
				//mUI.heroTxt->setString(STR_UTF8("克制水属性英雄，造成额外伤害"));
			}
			break;
		case HERO_PROPERTY_TYPE::HERO_INT:	//水属性英雄
			{
				mUI.heroTag->loadTexture(ResourceUtils::getIconGlobalPath("img_shuxing33_zhi.png"));
				//mUI.heroTxt->setString(STR_UTF8("克制火属性英雄，造成额外伤害"));
			}
			break;
		default:
			{
				mUI.heroTag->setVisible(false);
				//mUI.heroTxt->setString(STR_UTF8("不受其他属性英雄克制"));
			}
			break;
		}
		mUI.heroTxt->setVisible(false);			//不显示这条了

		string imgPath;
		switch (heroData->getIntData("jobType"))
		{
		case HERO_ATK: //攻击
			{
				imgPath = ResourceUtils::getIconJobPath("img_shuxing1_gong.png");
				mUI.heroJob->loadTexture(imgPath);
			}
			break;
		case HERO_DEFENSE: //防御
			{
				imgPath = ResourceUtils::getIconJobPath("img_shuxing1_fang.png");
				mUI.heroJob->loadTexture(imgPath);
			}
			break;
		case HERO_HELP: //辅助
			{
				imgPath = ResourceUtils::getIconJobPath("img_shuxing1_fu.png");
				mUI.heroJob->loadTexture(imgPath);
			}
			break;
		case HERO_FIRST: //先手
			{
				imgPath = ResourceUtils::getIconJobPath("img_shuxing1_xian.png");
				mUI.heroJob->loadTexture(imgPath);
			}
			break;

		default:
			break;
		}

	}

}

void LayerHeroInfo::onEnter()
{
	ModalLayer::onEnter();
}

void LayerHeroInfo::onExit()
{
	ModalLayer::onExit();
}

void LayerHeroInfo::onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switch (btnId)
		{
		case HERO_BTN_ID::BACK:
			dispose();
			break;
		case HERO_BTN_ID::BTN_GETWAY:
			{
				LayerEquipSource* pSmithy = LayerEquipSource::create();
				this->getParent()->addChild(pSmithy);
				pSmithy->showSrc(mDebrisTemplateId);
			}
			break;
		default:
			break;
		}
	}
}



