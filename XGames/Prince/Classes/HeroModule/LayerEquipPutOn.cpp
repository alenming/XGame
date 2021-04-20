#include "LayerEquipPutOn.h"
#include "ToolModule/ToolModel.h"
#include "ToolModule/UIToolHeader.h"
#include "HeroModule/HeroController.h"
#include "Communication/Command.h"
#include "SmithyModule/SmithyModel.h"
#include "Widget/ActionCreator.h"
#include "GuideCenter/GuideManager.h"


LayerEquipPutOn::LayerEquipPutOn()
	: mEquipId(-1)
	, mHeroId(-1)
{
	HeroModel::getInstance()->addObserver(this);
}

LayerEquipPutOn::~LayerEquipPutOn()
{
	HeroModel::getInstance()->removeObserver(this);
}

LayerEquipPutOn* LayerEquipPutOn::create(int heroId, int equipId)
{
	LayerEquipPutOn* layer = new LayerEquipPutOn();
	if (layer && layer->init(heroId, equipId))
	{
		layer->autorelease();
		return layer;
	}
	else
	{
		delete layer;
		return nullptr;
	}
}

bool LayerEquipPutOn::init(int heroId, int equipId)
{
	if (!ModalLayer::init())
	{
		return false;
	}

	mHeroId = heroId;
	mEquipId = equipId;
	initUI();
	return true;
}

void LayerEquipPutOn::onEnter()
{
	ModalLayer::onEnter();

	auto func = [](){ GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_EQUIP_PUT_ON); };
 	ActionCreator::runCommDlgAction(mUI.mBackImg, func);
}

void LayerEquipPutOn::onExit()
{
	ModalLayer::onExit();
}

void LayerEquipPutOn::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;
	if (obParam->id == nMAIN_CMD_TOOL_PUTON)
	{
		//收到装备穿戴消息，关闭窗口
		dispose();
	}
}

void LayerEquipPutOn::initUI()
{
	auto root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UIArray/UIPutOnEquip.ExportJson"));
	this->addChild(root);

	mUI.mRoot = root;
	mUI.mBackImg = dynamic_cast<ImageView*>(root->getChildByName("Image_di"));
	mUI.mCloseBtn = dynamic_cast<Button*>(mUI.mBackImg->getChildByName("Btn_Wear_Close"));
	mUI.mCloseBtn->addTouchEventListener(CC_CALLBACK_2(LayerEquipPutOn::onBtnClicked, this, HERO_BTN_ID::BACK));
	mUI.mPutOnBtn = dynamic_cast<Button*>(mUI.mBackImg->getChildByName("Btn_Wear"));
	mUI.mPutOnBtn->addTouchEventListener(CC_CALLBACK_2(LayerEquipPutOn::onBtnClicked, this, HERO_BTN_ID::BTN_WEAR));
	mUI.mEquipIcon = dynamic_cast<ImageView*>(mUI.mBackImg->getChildByName("Img_Wear_Equ"));
	mUI.mEquipName = dynamic_cast<Text*>(mUI.mBackImg->getChildByName("Lab_Wear_Equ_Name"));
	mUI.mEquipNum = dynamic_cast<Text*>(mUI.mBackImg->getChildByName("Lab_Wear_Equ_Count"));
	mUI.mEquipAttr1 = dynamic_cast<Text*>(mUI.mBackImg->getChildByName("Lab_Wear_Equ_Attr_1"));
	mUI.mEquipAttr2 = dynamic_cast<Text*>(mUI.mBackImg->getChildByName("Lab_Wear_Equ_Attr_2"));
	mUI.mEquipDes = dynamic_cast<Text*>(mUI.mBackImg->getChildByName("Lab_Wear_Equ_Desc"));
	mUI.mTips = dynamic_cast<Text*>(mUI.mBackImg->getChildByName("Lab_Wear_Tips"));
	mUI.mEquipName->setFontName(FONT_FZZHENGHEI);
	mUI.mEquipNum->setFontName(FONT_FZZHENGHEI);
	mUI.mEquipAttr1->setFontName(FONT_FZZHENGHEI);
	mUI.mEquipAttr2->setFontName(FONT_FZZHENGHEI);
	mUI.mEquipDes->setFontName(FONT_FZZHENGHEI);
	mUI.mTips->setFontName(FONT_FZZHENGHEI);

	const TOOL_INFO* toolInfo = ToolModel::getInstance()->GetToolInfo(mEquipId);
	mUI.mEquipName->setString(toolInfo->strName);
	UIToolHeader* equipIcon = UIToolHeader::create(toolInfo->ntemplateId);
	equipIcon->setAnchorPoint(Point::ANCHOR_MIDDLE);
	equipIcon->setPosition(mUI.mEquipIcon->getContentSize() / 2);
	mUI.mEquipIcon->addChild(equipIcon);

	RowData* data = DataManager::getInstance()->searchToolById(toolInfo->ntemplateId);
	string info = data->getStringData("itemInfo");
	mUI.mEquipDes->setString(info);

	int num = ToolModel::getInstance()->searchPutOnEquipNum(toolInfo->ntemplateId);
	//mUI.mEquipNum->setString(STR_UTF8("拥有" + TO_STR(num) + "件"));
	mUI.mEquipNum->setString(DataManager::getInstance()->searchCommonTexdtById(10219) + TO_STR(num) +
							 DataManager::getInstance()->searchCommonTexdtById(10220));

	QH_Param qhParam;
	SmithyModel::getInstance()->getQHParam(mEquipId, qhParam);
	if (qhParam.vcBase.size() >= 2)
	{
		if (qhParam.vcBase[0].attrType < ATTR_TYPE_HP_COE)
		{
			mUI.mEquipAttr1->setString(STR_UTF8(qhParam.vcBase[0].strAttrName + "  " + TO_STR((int)(qhParam.vcBase[0].fAttrValue))));
		}
		else
		{
			mUI.mEquipAttr1->setString(STR_UTF8(qhParam.vcBase[0].strAttrName + "  " + _F_TO_STR(qhParam.vcBase[0].fAttrValue * 100) + "%"));
		}
		
		if (qhParam.vcBase[0].attrType < ATTR_TYPE_HP_COE)
		{
			mUI.mEquipAttr2->setString(STR_UTF8(qhParam.vcBase[1].strAttrName + "  " + TO_STR((int)(qhParam.vcBase[1].fAttrValue))));
		}
		else
		{
			mUI.mEquipAttr2->setString(STR_UTF8(qhParam.vcBase[1].strAttrName + "  " + _F_TO_STR(qhParam.vcBase[1].fAttrValue * 100) + "%"));
		}
	}
}

void LayerEquipPutOn::onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId)
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
		case HERO_BTN_ID::BTN_WEAR:
			HeroController::sendEquipPutOnMsg(mHeroId, mEquipId);
			break;
		default:
			break;
		}
	}
}