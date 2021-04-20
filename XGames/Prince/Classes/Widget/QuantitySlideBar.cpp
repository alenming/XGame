#include "QuantitySlideBar.h"
#include "Utils/StringFormat.h"
#include "UI/UIDef.h"
#include "Common/Common.h"
#include "Utils/SoundUtils.h"

#define SLIDEBAR_TAG		101
#define IMAGE_NUM_BG_TAG	102
QuantitySlideBar::QuantitySlideBar()
{
	m_iQuantity = 1;
	m_iMaxQuantity = 1;
	m_unitData = 0;
	m_iSizeWidth = 0;
	m_bIsNeedStroke = false;
	m_numText = nullptr;
	m_extenalText = nullptr;
	m_color = Color3B::WHITE;
}

QuantitySlideBar::~QuantitySlideBar()
{

}

bool QuantitySlideBar::init()
{
	if(!Node::init())
	{
		return false;
	}

	this->setContentSize(Size(360, 80));

	Sprite* bgSprite = Sprite::create("Image/UIbeibao/img_tiao_shuliang_2.png");
	Sprite* proSprite = Sprite::create("Image/UIbeibao/img_tiao_shuliang_1.png");
	Sprite* thumbNor = Sprite::create("Image/UIbeibao/btn_shuliang_1.png");
	Sprite* thumbSel = Sprite::create("Image/UIbeibao/btn_shuliang_2.png");
	ControlSlider* slideBar = ControlSlider::create(bgSprite, proSprite, thumbNor, thumbSel);
	
	if(slideBar != nullptr)
	{
		slideBar->setMinimumValue(0.0f);
		slideBar->setMinimumAllowedValue(0.15f);
		slideBar->setMaximumValue(1.0f);
		slideBar->setMaximumAllowedValue(0.85f);
		slideBar->setValue(0.15);
		slideBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		slideBar->setPosition(Point::ZERO);		
		this->addChild(slideBar, 0, SLIDEBAR_TAG);

		//最大/最小按钮
		Button* btnMaxValue = Button::create("Image/UIbeibao/btn_MAX_1.png", "Image/UIbeibao/btn_MAX_2.png");
		btnMaxValue->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		btnMaxValue->setPosition(Point(ccpAdd(slideBar->getPosition(), Point(slideBar->getContentSize().width*0.5 + 
			btnMaxValue->getContentSize().width*0.5f-15, 0))));
		this->addChild(btnMaxValue);
		btnMaxValue->addTouchEventListener(CC_CALLBACK_2(QuantitySlideBar::selectMaxValue, this));

		Button* btnMinValue = Button::create("Image/UIbeibao/btn_MIN_1.png", "Image/UIbeibao/btn_MIN_2.png");
		btnMinValue->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		btnMinValue->setPosition(ccpSub(slideBar->getPosition(), Point(slideBar->getContentSize().width*0.5 + 
			btnMinValue->getContentSize().width*0.5f-15, 0)));
		btnMinValue->addTouchEventListener(CC_CALLBACK_2(QuantitySlideBar::selectMinValue, this));
		this->addChild(btnMinValue);

		//数量提示
		ImageView* numTextBg = ImageView::create("Image/UIbeibao/img_shuliang_di2.png");
		numTextBg->setPosition(slideBar->convertToWorldSpace(Vec2(slideBar->getThumbSprite()->getPositionX(),
			slideBar->getThumbSprite()->getPositionY() + slideBar->getThumbSprite()->getContentSize().height)));
		numTextBg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_numText = Text::create();
		m_numText->setString(_TO_STR(1));
		m_numText->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_numText->setFontName(FONT_FZZHENGHEI);
		m_numText->setFontSize(20);
		m_numText->setPosition(Vec2(numTextBg->getContentSize().width*0.5f, numTextBg->getContentSize().height*0.5f+5));
		numTextBg->addChild(m_numText);
		this->addChild(numTextBg,0,IMAGE_NUM_BG_TAG);

		slideBar->addTargetWithActionForControlEvents(this, cccontrol_selector(QuantitySlideBar::valueChanged), Control::EventType::VALUE_CHANGED);
	}		

	return true;
}

void QuantitySlideBar::selectMaxValue(Ref* pSender, ui::Widget::TouchEventType type)
{
	if(type == Widget::TouchEventType::ENDED)
	{
		SoundUtils::playSoundEffect("dianji");
		ControlSlider* slideBar = (ControlSlider*)this->getChildByTag(SLIDEBAR_TAG);
		slideBar->setValue(slideBar->getMaximumAllowedValue());
	}
}

void QuantitySlideBar::selectMinValue(Ref* pSender, ui::Widget::TouchEventType type)
{
	if(type == Widget::TouchEventType::ENDED)
	{
		SoundUtils::playSoundEffect("dianji");
		ControlSlider* slideBar = (ControlSlider*)this->getChildByTag(SLIDEBAR_TAG);
		slideBar->setValue(slideBar->getMinimumAllowedValue());
	}
}

void QuantitySlideBar::setMaxQuantity(int maxNum)
{
	m_iMaxQuantity = maxNum;
	m_iQuantity = 1;
	ControlSlider* slideBar = (ControlSlider*)this->getChildByTag(SLIDEBAR_TAG);
	slideBar->setValue(0.15);
}

int QuantitySlideBar::getCurrentQuantity()
{
	return m_iQuantity;
}

void QuantitySlideBar::initDivisorText(Text* pText, int unitData)
{
	CCASSERT(pText != nullptr, "");
	m_extenalText = pText;
	m_unitData = unitData;
	m_extenalText->setString(_TO_STR(m_iQuantity*m_unitData));
}

void QuantitySlideBar::isNeedStroke(bool isStroke, Color3B color, int sizeWidth)
{
	if(isStroke)
	{
		m_bIsNeedStroke = isStroke;
		m_color = color;
		m_iSizeWidth = sizeWidth;
		setTextAddStroke(m_extenalText, color, sizeWidth);
	}
}

void QuantitySlideBar::setDivisorText()
{
	if(m_extenalText != nullptr)
	{
		m_extenalText->setString(_TO_STR(m_iQuantity * m_unitData));
		if(m_bIsNeedStroke)
		{
			setTextAddStroke(m_extenalText, m_color, m_iSizeWidth);
		}
	}
}

void QuantitySlideBar::valueChanged(Ref* pSender, Control::EventType type)
{
	ControlSlider* slideBar = (ControlSlider*)pSender;
	ImageView* imgBg = (ImageView*)this->getChildByTag(IMAGE_NUM_BG_TAG);

	if(type == Control::EventType::VALUE_CHANGED)
	{
		float slideValue = slideBar->getValue()-slideBar->getMinimumAllowedValue();	
		float totalValue = slideBar->getMaximumAllowedValue()-slideBar->getMinimumAllowedValue();
		float unitValue = totalValue/m_iMaxQuantity;
		int count = (int)(slideValue*m_iMaxQuantity/totalValue);

		//移动文字坐标
		imgBg->setPosition(Vec2(slideBar->getThumbSprite()->getPositionX() - slideBar->getContentSize().width*0.5f, imgBg->getPositionY()));
		m_iQuantity = unitValue*count == totalValue ? count : (count+1);
		m_numText->setString(_TO_STR(m_iQuantity));

		//改变其它和数量相关的文本
		setDivisorText();
	}
}
