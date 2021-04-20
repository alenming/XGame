#include "CustomScrollBar.h"


CustomScrollBar::CustomScrollBar(void)
{
	m_fScrollBarLength = 0.0f;
}

CustomScrollBar::~CustomScrollBar(void)
{
}

bool CustomScrollBar::init()
{
	if (!Node::init())
	{
		return false;
	}

	return true;
}

void CustomScrollBar::initScrollBar( const float contentHeight )
{
	// 设置容器尺寸;
	this->setContentSize(Size(18.0f, contentHeight));

	// 底图;
	m_uiBar.imgBg_head = ImageView::create("Image/UIFormation/ScrollViewBar/img_tanchukuang2_huatiao2_1.png");
	m_uiBar.imgBg_body = ImageView::create("Image/UIFormation/ScrollViewBar/img_tanchukuang2_huatiao2_2.png");
	m_uiBar.imgBg_tail = static_cast<ImageView*>(m_uiBar.imgBg_head->clone());
	m_uiBar.imgBg_tail->setRotationSkewX(180.0f);
	this->addChild(m_uiBar.imgBg_head);
	this->addChild(m_uiBar.imgBg_body);
	this->addChild(m_uiBar.imgBg_tail);

	// 设置底图位置;
	m_uiBar.imgBg_head->setPosition(Vec2(5.0f, contentHeight - 4.0f));
	float yyy = (contentHeight - 18.0f)/9.0f;
	m_uiBar.imgBg_body->setPosition(Vec2(m_uiBar.imgBg_head->getPosition().x, contentHeight/2));
	m_uiBar.imgBg_tail->setPosition(Vec2(m_uiBar.imgBg_body->getPosition().x, 5.0f));
	m_uiBar.imgBg_body->setScaleY(yyy);

	// 滚动条;
	m_uiBar.imgBar_head = ImageView::create("Image/UIFormation/ScrollViewBar/img_tanchukuang2_huatiao1_1.png");
	m_uiBar.imgBar_body = ImageView::create("Image/UIFormation/ScrollViewBar/img_tanchukuang2_huatiao1_2.png");
	m_uiBar.imgBar_tail = static_cast<ImageView*>(m_uiBar.imgBar_head->clone());
	m_uiBar.imgBar_tail->setRotationSkewX(180.0f);
	this->addChild(m_uiBar.imgBar_head);
	this->addChild(m_uiBar.imgBar_body);
	this->addChild(m_uiBar.imgBar_tail);

	// 初始化滚动条位置;
	setScrollBarLengthPercent(1.0f);
	setScrollBarPercentY(0.0f);
	
}

void CustomScrollBar::setScrollBarLengthPercent( float fLengthPercent )
{
	// 计算长度比例并设置滚动条位置;
	//float fLengthPercent = contentSize.height / innerContainerSize.height;
	if (fLengthPercent > 1.0f)
	{
		fLengthPercent = 1.0f;
	}
	if (fLengthPercent < 0.0f)
	{
		fLengthPercent = 0.0f;
	}
	m_fScrollBarLength = this->getContentSize().height * fLengthPercent;
	float YPercent = (m_fScrollBarLength - 2 * 9.0f) /3.0f;
	m_uiBar.imgBar_body->setScaleY(YPercent);
}

void CustomScrollBar::setScrollBarPercentY( float fPercentY )
{
	// 实际的反向比例;
	float fFinalPercent = (100.0f - fPercentY)/100.0f;
	//log("fPercentY = %f, FinalPercent = %f", fPercentY, fFinalPercent);

	// 误差范围(同时也纠正滚动回弹带来的越界);
	float fNorm = 0.001f;
	if (fFinalPercent - 0.0f < fNorm)
	{
		fFinalPercent = 0.0f;
	}
	else if (fFinalPercent - 1.0f > fNorm)
	{
		fFinalPercent = 1.0f;
	}

	// 滚动条移位;
	m_uiBar.imgBar_body->setPosition(Vec2(5.5f, 
		(this->getContentSize().height - m_fScrollBarLength) * fFinalPercent + m_fScrollBarLength/2));
	m_uiBar.imgBar_head->setPosition(Vec2(m_uiBar.imgBar_body->getPosition().x,
		(this->getContentSize().height - m_fScrollBarLength) * fFinalPercent + m_fScrollBarLength - 9.0f));
	m_uiBar.imgBar_tail->setPosition(Vec2(m_uiBar.imgBar_body->getPosition().x,
		(this->getContentSize().height - m_fScrollBarLength) * fFinalPercent + 9.0f));
}

