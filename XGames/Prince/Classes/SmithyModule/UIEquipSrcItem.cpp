#include "UIEquipSrcItem.h"

#include "Common/Common.h"
#include "Utils/StringFormat.h"
#include "UI/UIDef.h"
#include "cocos2d.h"
using namespace cocos2d;


UIEquipSrcItem::UIEquipSrcItem()
{
	m_pFrontImgUnselected = nullptr;
	m_pFrontImgSelected = nullptr;
	m_pBarrierName = nullptr;
	m_pImgChapter = nullptr;
	m_pImgBarrierOpen = nullptr;
	m_isSelected = false;
	m_isBarrierOpen = true;
}

UIEquipSrcItem* UIEquipSrcItem::create(const std::string& mainImage, const std::string &normalImage, const std::string& selectedImage )
{
	UIEquipSrcItem *btn = new UIEquipSrcItem();
	if (btn && btn->init(mainImage, mainImage)) {
		btn->autorelease();
		btn->setStateImg(normalImage, selectedImage);
		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;
}

//名称
void UIEquipSrcItem::setBarrierName( const std::string& name)
{
	if (m_pBarrierName == nullptr)
	{
		m_pBarrierName = cocos2d::ui::Text::create();
		m_pBarrierName->setFontSize(30);
		m_pBarrierName->setColor(cocos2d::Color3B(0xe5,0xf0,0x34));
		m_pBarrierName->setFontName(FONT_FZZHENGHEI);
		m_pBarrierName->setPosition(cocos2d::Vec2(this->getContentSize().width - 50 - m_pBarrierName->getContentSize().width/2, m_pBarrierName->getContentSize().height));
		this->addChild(m_pBarrierName, 9);
		m_pBarrierName->setTouchEnabled(false);
	}
	//顶部居中显示
	m_pBarrierName->setText(name);
	m_pBarrierName->setPosition(cocos2d::Vec2(this->getContentSize().width - 50 - m_pBarrierName->getContentSize().width/2, m_pBarrierName->getContentSize().height));
}

//说明
void UIEquipSrcItem::setChapterId( const int chapterId)
{
	string imgName = getChapterImgName(chapterId);
	if (imgName == "")
	{
		return;
	}
	if (m_pImgChapter == nullptr)
	{
		m_pImgChapter = cocos2d::ui::ImageView::create(imgName);
		m_pImgChapter->setPosition(cocos2d::Vec2(this->getContentSize().width/4, this->getContentSize().height/3));
		this->addChild(m_pImgChapter, 4);
	}
	else
	{
		m_pImgChapter->loadTexture(imgName);
	}
}

//设置关卡开启状态
void UIEquipSrcItem::setBarrierOpenFlag(bool flag)
{
	m_isBarrierOpen = flag;	 
	if (m_pImgBarrierOpen == nullptr)
	{
		m_pImgBarrierOpen = cocos2d::ui::ImageView::create("Image/Icon/global/img_weikaiqi.png");
		m_pImgBarrierOpen->setPosition(cocos2d::Vec2(this->getContentSize().width - 100, this->getContentSize().height*2/3));
		this->addChild(m_pImgBarrierOpen, 4);
	}
	m_pImgBarrierOpen->setVisible(!flag);
}
bool UIEquipSrcItem::isBarrierOpen()
{
	return m_isBarrierOpen;
}
//重置状态
void UIEquipSrcItem::reset()
{	
}


void UIEquipSrcItem::setSelected(const bool& isSelected)
{
	if (m_pFrontImgSelected == nullptr)
	{
		m_pFrontImgSelected = cocos2d::ui::ImageView::create("Image/Icon/global/btn_kuang_zhuangbei_2.png");
		this->addChild(m_pFrontImgSelected, 5);
		m_pFrontImgSelected->setVisible(false);
		m_pFrontImgSelected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
	}
	if (m_pFrontImgUnselected == nullptr)
	{
		m_pFrontImgUnselected = cocos2d::ui::ImageView::create("Image/Icon/global/btn_kuang_zhuangbei_1.png");
		this->addChild(m_pFrontImgUnselected, 5);
		m_pFrontImgUnselected->setVisible(false);		
		m_pFrontImgUnselected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
	}
	if (isSelected)
	{
		m_pFrontImgUnselected->setVisible(false);
		m_pFrontImgSelected->setVisible(true);
	}
	else
	{
		m_pFrontImgUnselected->setVisible(true);
		m_pFrontImgSelected->setVisible(false);
	}
	m_isSelected = isSelected;
}
const bool& UIEquipSrcItem::isSelected()
{
	return m_isSelected;
}
void UIEquipSrcItem::onPressStateChangedToNormal()
{
	Button::onPressStateChangedToNormal();
	if (m_isSelected)
	{
		if(m_pFrontImgUnselected != nullptr)
			m_pFrontImgUnselected->setVisible(false);
		if(m_pFrontImgSelected != nullptr)
			m_pFrontImgSelected->setVisible(true);
	}
	else
	{
		if(m_pFrontImgUnselected != nullptr)
			m_pFrontImgUnselected->setVisible(true);
		if(m_pFrontImgSelected != nullptr)
			m_pFrontImgSelected->setVisible(false);
	}
}


void UIEquipSrcItem::onPressStateChangedToPressed()
{
	Button::onPressStateChangedToPressed();	
	//Button::setScale(0.9*m_fScaleValue);
	if(m_pFrontImgUnselected != nullptr)
		m_pFrontImgUnselected->setVisible(false);
	if(m_pFrontImgSelected != nullptr)
		m_pFrontImgSelected->setVisible(true);
}

void UIEquipSrcItem::onPressStateChangedToDisabled()
{
	Button::onPressStateChangedToDisabled();
}

void UIEquipSrcItem::setStateImg(const std::string &normalImage,
							   const std::string& selectedImage,
							   const std::string& disableImage)
{
	if (m_pFrontImgUnselected == nullptr)
	{
		m_pFrontImgUnselected = cocos2d::ui::ImageView::create(normalImage);
		this->addChild(m_pFrontImgUnselected, 5);
		m_pFrontImgUnselected->setVisible(true);
		if (this->getContentSize().width == 0 || this->getContentSize().height == 0)
		{
			this->setContentSize(m_pFrontImgUnselected->getContentSize());
		}
		m_pFrontImgUnselected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
	}
	else
	{
		m_pFrontImgUnselected->loadTexture(normalImage);
	}
	if (selectedImage == "")
	{
		return;
	}
	if (m_pFrontImgSelected == nullptr)
	{
		m_pFrontImgSelected = cocos2d::ui::ImageView::create(selectedImage);
		this->addChild(m_pFrontImgSelected, 5);
		m_pFrontImgSelected->setVisible(false);
		m_pFrontImgSelected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
	}
	else
	{
		m_pFrontImgSelected->loadTexture(selectedImage);
	}
}

std::string UIEquipSrcItem::getChapterImgName(const int chapterId)
{
	if (chapterId > 0)
	{
		int id = chapterId%10000;
		return "Image/UIEquip/icon/img_zhuangjie_" + _TO_STR(id) + ".png";
	}	
	return "";
}