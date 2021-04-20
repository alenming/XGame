#include "ItemTipInfo.h"
#include "DataManager/DataManager.h"
#include "Common/Common.h"
#include "ToolModule/ToolModel.h"

ItemTipInfo::ItemTipInfo(void)
{
	m_ntoolId = -1;
}


ItemTipInfo::~ItemTipInfo(void)
{
}

ItemTipInfo* ItemTipInfo::create(int toolId)
{
	//背景设置
	ItemTipInfo* info = new ItemTipInfo;
	if (info && info->init())
	{
		info->autorelease();
		info->initUi(toolId);
		info->setClickEmptyDispose(true);
		return info;
	}


	CC_SAFE_DELETE(info);
	return nullptr;
}

bool ItemTipInfo::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}
	return true;
}

void ItemTipInfo::onEnter()
{
	ModalLayer::onEnter();
}

void ItemTipInfo::onExit()
{
	ModalLayer::onExit();
}

void ItemTipInfo::initUi(int toolId)
{
	m_ntoolId = toolId;
	RowData* _row = DataManager::getInstance()->searchToolById(toolId);

	Text* txtcol = Text::create("      " + _row->getStringData("itemInfo"), FONT_FZZHENGHEI, 22);
	float col = ceil(txtcol->getContentSize().width/245.0f);
	float height = 45 > col*25 + 30 ? 45 : col*25 + 30;
	ImageView* imgBg = ImageView::create("Image/Icon/global/img_Tips_di3.png");
	this->addChild(imgBg);
	imgBg->setPosition(Vec2(this->getContentSize().width/2,this->getContentSize().height/2));
	imgBg->setScale9Enabled(true);
	imgBg->setContentSize(CCSize(320,height + 115));
	ActionCreator::runCommBtnAction(imgBg);
	// 名;
	Text* txtName = Text::create("", FONT_FZZHENGHEI, 26);
	txtName->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	txtName->setPosition(Vec2(imgBg->getContentSize().width/2, imgBg->getContentSize().height - 45));
	imgBg->addChild(txtName);
	if (nullptr != _row)
	{
		txtName->setString(_row->getStringData("itemName"));
		// 品质->颜色;
		txtName->setColor(getItemQuaColor(_row->getIntData("itemQua")));
	}

	// 线;
	ImageView* imgLine = ImageView::create("Image/Icon/global/img_tanchukuang1_line.png");
	imgLine->setPosition(Vec2(imgBg->getContentSize().width*3/10, imgBg->getContentSize().height - 65));
	imgLine->setScale(0.8f);
	imgBg->addChild(imgLine);

	// 类型;
	Text* txtType = Text::create("", FONT_FZZHENGHEI, 22);
	txtType->setColor(Color3B(0xf4, 0xea, 0x7a));
	txtType->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	txtType->setPosition(Vec2(38.0f, imgBg->getContentSize().height-100));
	imgBg->addChild(txtType);
	if (nullptr != _row)
	{
		txtType->setString(getItemTypeStr(_row->getIntData("itemTyp")));
	}

	// 个数;
	Text* txtCount = Text::create("", FONT_FZZHENGHEI, 21);
	txtCount->setColor(Color3B(0xff, 0xff, 0xff));
	txtCount->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	txtCount->setPosition(Vec2(278.0f, imgBg->getContentSize().height-100));
	imgBg->addChild(txtCount);
	txtCount->setVisible(false);
	if (nullptr != _row)
	{
		const TOOL_INFO* toolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(m_ntoolId);
		if (toolInfo)
		{
			txtCount->setVisible(true);
			txtCount->setString(TO_STR(toolInfo->nstack));
		}
	}

	Text* txtCountT = Text::create("", FONT_FZZHENGHEI, 22);
	txtCountT->setColor(Color3B(0xf4, 0xea, 0x7a));
	txtCountT->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	txtCountT->setPosition(Vec2(txtCount->getPosition().x - txtCount->getContentSize().width, imgBg->getContentSize().height-100));
	imgBg->addChild(txtCountT);
	txtCount->setVisible(false);
	const TOOL_INFO* toolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(m_ntoolId);
	if (toolInfo)
	{
		txtCount->setVisible(true);
		txtCountT->setString(DataManager::getInstance()->searchCommonTexdtById(20081));
	}

	// 描述;
	string strDesc = "";
	if (nullptr != _row)
	{
		strDesc.append(_row->getStringData("itemInfo"));
	}
	Text* txtDesc = Text::create("      "+ strDesc, FONT_FZZHENGHEI, 22);
	txtDesc->ignoreContentAdaptWithSize(false);
	txtDesc->setContentSize(Size(245.0f, height));
	txtDesc->setColor(Color3B(0xac, 0x92, 0x7e));
	txtDesc->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	txtDesc->setTextAreaSize(txtDesc->getContentSize());
	txtDesc->setPosition(Vec2(txtType->getPosition().x, txtType->getPosition().y - 13.0f));
	imgBg->addChild(txtDesc);

	ImageView* imgGo = ImageView::create("Image/Icon/global/img_dianjikongbai2.png");
	this->addChild(imgGo);
	imgGo->setPosition(Vec2(this->getContentSize().width/2,imgGo->getContentSize().height));
}

std::string ItemTipInfo::getItemTypeStr( int type )
{
	string str("");
	switch (type)
	{
	case 1: str.assign(DataManager::getInstance()->searchCommonTexdtById(20082));	
		break;
	case 2:
		{
			// 读装备表，取出具体类型;
			RowData* _row = DataManager::getInstance()->getDTEquipment()->searchDataById(m_ntoolId);
			if (nullptr != _row)
			{
				str.assign(getPartTypeStr(_row->getIntData("partTyp")));
			}
		}
		break;
	case 3: str.assign(DataManager::getInstance()->searchCommonTexdtById(20083));		
		break;
	case 4: str.assign(DataManager::getInstance()->searchCommonTexdtById(20084));			
		break;
	case 5: str.assign("");				
		break;
	case 6: str.assign(DataManager::getInstance()->searchCommonTexdtById(20085));		
		break;
	case 7: str.assign(DataManager::getInstance()->searchCommonTexdtById(20086));			
		break;
	case 8: str.assign(DataManager::getInstance()->searchCommonTexdtById(20087));		
		break;
	case 9: str.assign(DataManager::getInstance()->searchCommonTexdtById(20088));			
		break;
	case 10: str.assign(DataManager::getInstance()->searchCommonTexdtById(20089));		
		break;
	case 11: str.assign(DataManager::getInstance()->searchCommonTexdtById(20090));	
		break;
	case 12: str.assign(DataManager::getInstance()->searchCommonTexdtById(20091));		
		break;
	default:
		str.assign(DataManager::getInstance()->searchCommonTexdtById(20092));
		break;
	}

	return str;
}

Color3B ItemTipInfo::getItemQuaColor( int nQua )
{
	Color3B color(0xFF, 0xFF, 0xFF);
	switch (nQua)
	{
	case 1: break;											// 白色;
	case 2: color = Color3B(0x45, 0xE1, 0x6D);	break;		// 绿色;
	case 3: color = Color3B(0x36, 0xD1, 0xF2);	break;		// 蓝色;
	case 4: color = Color3B(0xC1, 0x69, 0xFF);	break;		// 紫色;
	case 5: color = Color3B(0xFF, 0xC1, 0x3B);	break;		// 橙色;

	default:
		break;
	}

	return color;
}

std::string ItemTipInfo::getPartTypeStr( int partType )
{
	string str("");
	switch (partType)
	{
	case 1: str.assign(DataManager::getInstance()->searchCommonTexdtById(20093));		
		break;
	case 2: str.assign(DataManager::getInstance()->searchCommonTexdtById(20094));		
		break;
	case 3: str.assign(DataManager::getInstance()->searchCommonTexdtById(20095));		
		break;
	case 4: str.assign(DataManager::getInstance()->searchCommonTexdtById(20096));		
		break;
	case 5: str.assign(DataManager::getInstance()->searchCommonTexdtById(20097));		
		break;
	case 6: str.assign(DataManager::getInstance()->searchCommonTexdtById(20098));		
		break;

	default:
		break;
	}

	return str;
}