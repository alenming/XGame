#include "UICardPrize.h"
#include "Utils/StringFormat.h"
#include "UI/UIDef.h"
#include "Common/Common.h"

UICardPrize::UICardPrize(void)
{
	m_pRoot = nullptr;
	m_pBackImg = nullptr;
	m_pCardImg = nullptr;

	m_pChipImg = nullptr;
	m_pStarPanelImg = nullptr;
	for (int i = 0; i < _STAR_CONT; i++)
	{
		m_pStar[i] = nullptr;
	}

	m_pCount = nullptr;  
	m_pName = nullptr;   
}


UICardPrize::~UICardPrize(void)
{
	
}

UICardPrize* UICardPrize::create(int id)
{
	//背景设置
	UICardPrize* card = new UICardPrize;
	if (card && card->init())
	{
		card->autorelease();
		card->initUi(id);

		return card;
	}


	CC_SAFE_DELETE(card);
	return nullptr;
}


bool UICardPrize::initUi(int id)
{
	RowData* rowData = DataManager::getInstance()->searchToolById(id);
	int type = rowData->getIntData("itemTyp");
	int qua = 0;
	string fileName = "";
	if (type == TOOL_ITEM_TYPE_HERO)
	{
		RowData* pRowPetData = DataManager::getInstance()->searchCharacterById(id);
		qua = pRowPetData->getIntData("stars");
		fileName = ResourceUtils::getHalfCardPath(rowData->getIntData("resId"));
	}
	else if (type == TOOL_ITEM_TYPE_HERO_CHIP)
	{
		qua = rowData->getIntData("itemQua");
		fileName = ResourceUtils::getHalfCardPath(rowData->getIntData("resId"));
	}
	else
	{
		qua = rowData->getIntData("itemQua");
		fileName = ResourceUtils::getSmallIconPath(rowData->getIntData("resId"));
	}
	// 加载Sign UI;
	string strFileName;
	strFileName.assign("Image/UICommon/Card_prize.ExportJson");

	// Root;
	m_pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_pRoot);

	//背景
	m_pBackImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"img_back"));
	m_pBackImg->loadTexture("Image/Icon/global/img_kapai_xiao_" + TO_STR(qua)+".png");

	//卡牌
	m_pCardImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"img_card"));
	m_pCardImg->loadTexture(fileName);

	//星星
	m_pStarPanelImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"star_Panel"));
	m_pStarPanelImg->setVisible(false);
	for (int i = 0; i < _STAR_CONT; i++)
	{
		m_pStar[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"star_"+TO_STR(i+1)));
		m_pStar[i]->setVisible(false);
	}


	switch (type)
	{
	case TOOL_ITEM_TYPE_HERO_CHIP:
		{
			m_pChipImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"img_chip"));
			m_pChipImg->setVisible(true);
		}
		break;
	case TOOL_ITEM_TYPE_HERO:
		{
			m_pChipImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"img_chip"));
			m_pChipImg->setVisible(false);

			for (int i = 0; i < qua; i++)
			{
				m_pStar[i]->setVisible(true);
			}
			m_pStarPanelImg->setVisible(true);
		}
		break;
	default:
		{
			m_pCardImg->setScale(2.0f);
			m_pCardImg->setPosition(Vec2(m_pCardImg->getPosition().x,m_pCardImg->getPosition().y+30));

			m_pChipImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"img_chip"));
			m_pChipImg->setVisible(false);
		}
		
		break;
	}


	m_pCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot,"count"));
	m_pCount->setFontName(FONT_FZZHENGHEI);

	m_pName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot,"name"));
	m_pName->setFontName(FONT_FZZHENGHEI);

	return true;
}

void UICardPrize::setCardCount(const int& count)
{
	if (count <= 1)
	{
		m_pCount->setVisible(false);
	}
	else
	{
		m_pCount->setVisible(true);
		m_pCount->setText(DataManager::getInstance()->searchCommonTexdtById(20102) +(": ") + TO_STR(count));
		setTextAddStroke(m_pCount,Color3B(0x00, 0x00, 0x00),3);
	}
}

void UICardPrize::setCardName(const string& name)
{
	m_pName->setText(name);
	setTextAddStroke(m_pName,Color3B(0x79, 0x03, 0x03),2);
}