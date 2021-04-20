#include "UIToolRob.h"
#include "Common/Common.h"
#include "Utils/StringFormat.h"

#include "cocos2d.h"
using namespace cocos2d;
using namespace cocos2d::ui;
const float MAX_SCALE_STAR = 0.40;
const int MAX_LABLE_ORDER_NUM = 9;

UIToolRob::UIToolRob()
{
	m_Num = 0;
	m_pFrontImgUnselected = nullptr;
	m_pFrontImgSelected = nullptr;
	m_pLableIcon = nullptr;
	m_pLvlIcon = nullptr;
	m_pLvlText = nullptr;
	m_pMainIcon = nullptr;
	m_pNumExIcon = nullptr;
	m_pNumEx	 = nullptr;	
	m_fScaleValue = 1.0;
	initUI();
}
UIToolRob* UIToolRob::create()
{
	/*UIToolRob *btn = new UIToolRob;
	if (btn && btn->init()) {
	btn->autorelease();
	return btn;
	}*/
	UIToolRob *btn = new UIToolRob();	
	if (btn && btn->init())
	{		
		btn->autorelease();
		btn->setTouchEnabled(false);
		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;
}
UIToolRob* UIToolRob::create(const std::string &mainImage )
{
	UIToolRob *btn = new UIToolRob();
	if (btn && btn->init(mainImage))
	{	
		btn->setTouchEnabled(false);
		//btn->setMainIcon(mainImage);
		btn->autorelease();
		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;
}

void UIToolRob::setMainIcon(const std::string& normalImage)
{
	this->loadTextures(normalImage, normalImage);
	if (m_pFrontImgUnselected != nullptr)
	{		
		m_pFrontImgUnselected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
	}
	if (m_pFrontImgSelected != nullptr)
	{		
		m_pFrontImgSelected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
	}
	return;
	if (m_pMainIcon == nullptr)
	{		
		m_pMainIcon = cocos2d::ui::ImageView::create(normalImage);
		this->addChild(m_pMainIcon);
		m_pMainIcon->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
		//在上面盖一层蒙版
		/*if (m_pFrontImgUnselected == nullptr)
		{
		m_pFrontImgUnselected = cocos2d::ui::ImageView::create("Image/Icon/Small/img_TY_icon1_1.png");
		this->addChild(m_pFrontImgUnselected);			
		m_pFrontImgUnselected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
		}
		if (m_pFrontImgSelected == nullptr)
		{
			m_pFrontImgSelected = cocos2d::ui::ImageView::create("Image/Icon/Small/img_TY_icon1_3.png");
			this->addChild(m_pFrontImgSelected);
			m_pFrontImgSelected->setVisible(false);
			m_pFrontImgSelected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
		}*/
	}
	else
	{
		m_pMainIcon->loadTexture(normalImage);
	}	
}

void UIToolRob::setLableIcon(const int& index)
{
	string strFilename = "Image/Icon/global/lveduo_" + _TO_STR(index+1) + ".png";
	setLableIcon(strFilename);
}
void UIToolRob::setLableIcon(const std::string& normalImage)
{	
	if (m_pLableIcon == nullptr)
	{	
		m_pLableIcon = ImageView::create(normalImage);
		m_pLableIcon->setPosition(cocos2d::Vec2(this->getContentSize().width/2
			, this->getContentSize().height-15));
		m_pLableIcon->setTouchEnabled(false);
		this->addChild(m_pLableIcon);
	}
	else
	{
		m_pLableIcon->loadTexture(normalImage);
	}
}


void UIToolRob::initUI()
{	
	for (int i=0; i<5; i++)
	{
		m_pStar[i] = nullptr;
	}
	m_pName = cocos2d::ui::Text::create();
	m_pName->setFontSize(22);
	m_pName->setColor(cocos2d::Color3B(0xe5,0xf0,0x34));
	m_pName->setFontName(FONT_FZZHENGHEI);
	m_pName->setPosition(cocos2d::Vec2(10+m_pName->getContentSize().width/2, this->getContentSize().height*3/4));
	this->addChild(m_pName, MAX_LABLE_ORDER_NUM);
	m_pName->setTouchEnabled(false);
	
	m_pNum = cocos2d::ui::Text::create();
	m_pNum->setFontSize(18);
	m_pNum->setColor(cocos2d::Color3B(0xff,0xff,0xff));
	m_pNum->enableOutline(cocos2d::Color4B(0xff, 0x00,0x00,0x00), 2);
	m_pNum->setFontName(FONT_FZZHENGHEI);
	m_pNum->setPosition(cocos2d::Vec2(this->getContentSize().width/2, 10));
	this->addChild(m_pNum, MAX_LABLE_ORDER_NUM);
	m_pNum->setTouchEnabled(false);

	m_pAdvLvl = cocos2d::ui::Text::create();
	m_pAdvLvl->setFontSize(26);
	m_pAdvLvl->setColor(cocos2d::Color3B(0x17,0xc6,0x46));
	m_pAdvLvl->setFontName(FONT_FZZHENGHEI);
	m_pAdvLvl->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height*1/2));
	//m_pUseLvl->setTextAreaSize(Size(200, 70));
	this->addChild(m_pAdvLvl, MAX_LABLE_ORDER_NUM);
	m_pAdvLvl->setTouchEnabled(false);
}

//名称
void UIToolRob::setTitle( const std::string& name)
{
	//顶部居中显示
	m_pName->setText(name);
	m_pName->setPosition(cocos2d::Vec2((getContentSize().width)/2, this->getContentSize().height-m_pName->getContentSize().height/2-3));
}

//数量
void UIToolRob::setNum(int num)
{	
	m_Num = num;
	m_pNum->setText(_TO_STR(num));
	m_pNum->setPosition(cocos2d::Vec2(this->getContentSize().width/2, 5));
}
const int UIToolRob::getNum()
{
	return m_Num;
}

void UIToolRob::setNumEx(int num)
{	
	if (num <= 0)
	{
		if (m_pNumExIcon != nullptr)
			m_pNumExIcon->setVisible(false);
		/*if (m_pNumEx != nullptr)
		{
		m_pNumEx->setText("");
		}*/
		return;
	}
	m_Num = num;
	if (m_pNumExIcon == nullptr)
	{	
		m_pNumExIcon = ImageView::create("Image/Icon/global/img_shuliang_di.png");
		m_pNumExIcon->setPosition(cocos2d::Vec2(this->getContentSize().width - 15
			, 5));
		m_pNumExIcon->setTouchEnabled(false);
		this->addChild(m_pNumExIcon, MAX_LABLE_ORDER_NUM);
	}
	//
	if (m_pNumEx == nullptr)
	{
		m_pNumEx = cocos2d::ui::Text::create();
		m_pNumEx->setFontSize(16);
		m_pNumEx->setColor(cocos2d::Color3B(0xff,0x40,0x2c));
		m_pNumEx->setFontName(FONT_FZZHENGHEI);
		m_pNumExIcon->addChild(m_pNumEx, MAX_LABLE_ORDER_NUM);
		m_pNumEx->setTouchEnabled(false);
		m_pNumEx->setPosition(cocos2d::Vec2(m_pNumExIcon->getContentSize().width/2
			, m_pNumExIcon->getContentSize().height/2));
	}
	m_pNumExIcon->setVisible(true);
	m_pNumEx->setText(_TO_STR(num));
}
//星级
void UIToolRob::setStarsNum(int num, bool isContain)
{
	if (num<=0 || num>5)
	{
		return;
	}
	for (int i=0; i<5; i++)
	{
		if (m_pStar[i] != nullptr)
			m_pStar[i]->setVisible(false);
	}
	float fbeginX = this->getContentSize().width/2 - (float)num/2.0f * 47*MAX_SCALE_STAR;//MAX_SCALE_STAR*
	float fY = -10;
	if (isContain)
	{
		fY = this->getContentSize().height+10;//15
	}
	//居中显示
	for (int i=0; i<num; i++)
	{
		if (m_pStar[i] == nullptr)
		{
			m_pStar[i] = cocos2d::ui::ImageView::create("Image/Icon/global/img_star.png");
			m_pStar[i]->setScale(MAX_SCALE_STAR);
			m_pStar[i]->setTouchEnabled(false);
			this->addChild(m_pStar[i], MAX_LABLE_ORDER_NUM);
		}
		else
		{
			m_pStar[i]->setVisible(true);
		}
		m_pStar[i]->setPosition(cocos2d::Vec2(
				fbeginX + (m_pStar[i]->getContentSize().width*i)*MAX_SCALE_STAR  + m_pStar[i]->getContentSize().width/2* MAX_SCALE_STAR
				, fY ));//m_pStar[i]->getContentSize().height/2 * MAX_SCALE_STAR
	}	
}
//等级
void UIToolRob::setLvl(int nLvl)
{
	if (nLvl <= 0)
	{
		return;
	}
	if (m_pLvlIcon == nullptr)
	{	
		m_pLvlIcon = ImageView::create("Image/Icon/global/img_dengji_di.png");
		float fx = 15;
		m_pLvlIcon->setPosition(cocos2d::Vec2(fx
			, getContentSize().height-15));
		m_pLvlIcon->setTouchEnabled(false);
		this->addChild(m_pLvlIcon, MAX_LABLE_ORDER_NUM);
	}
	//
	if (m_pLvlText == nullptr)
	{
		m_pLvlText = cocos2d::ui::Text::create();
		m_pLvlText->setFontSize(20);
		m_pLvlText->setColor(cocos2d::Color3B(0xff,0xfc,0x22));
		m_pLvlText->setFontName(FONT_FZZHENGHEI);
		m_pLvlIcon->addChild(m_pLvlText, MAX_LABLE_ORDER_NUM);
		m_pLvlText->setTouchEnabled(false);
		m_pLvlText->setPosition(cocos2d::Vec2(m_pLvlIcon->getContentSize().width/2
			, m_pLvlIcon->getContentSize().height/2));
	}
	m_pLvlText->setText(TO_STR(nLvl));
	
}
//进阶等级
void UIToolRob::setAdvLvl(int nLvl)
{
	if (nLvl <= 0)
	{
		return;
	}
	string str = STR_UTF8("+%d");
	char szLvl[100];
	memset(&szLvl, 0, 100);
	sprintf(szLvl, str.c_str(), nLvl);
	str = szLvl;
	m_pAdvLvl->setText(str);
	m_pAdvLvl->setPosition(cocos2d::Vec2(getContentSize().width/2, getContentSize().height/2));
}

void UIToolRob::setGray(const bool isGray)
{
	//if (m_pMainIcon != nullptr)
	{		
		Node* sprite = this->_buttonNormalRenderer;//->getVirtualRenderer();
		if(sprite)
		{	
			GLProgram * p = nullptr;
			if (isGray)
			{			
				p = GLProgram::createWithByteArrays("attribute vec4 a_position;\
										attribute vec2 a_texCoord;\
										attribute vec4 a_color;\
										varying vec4 v_fragmentColor;\
										varying vec2 v_texCoord;\
										void main()\
										{\
										gl_Position = CC_PMatrix * a_position;\
										v_fragmentColor = a_color;\
										v_texCoord = a_texCoord;\
										}", 
										"varying vec4 v_fragmentColor;\
										varying vec2 v_texCoord;\
										void main()\
										{\
										vec4 v_orColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);\
										float gray = dot(v_orColor.rgb, vec3(0.299, 0.587, 0.114));\
										gl_FragColor = vec4(gray, gray, gray, v_orColor.a);\
										}");		
			}
			else
			{
				p = GLProgram::createWithByteArrays("attribute vec4 a_position;\
									  attribute vec2 a_texCoord;\
									  attribute vec4 a_color;\
									  varying vec4 v_fragmentColor;\
									  varying vec2 v_texCoord;\
									  void main()\
									  {\
									  gl_Position = CC_PMatrix * a_position;\
									  v_fragmentColor = a_color;\
									  v_texCoord = a_texCoord;\
									  }", 
									  "varying vec4 v_fragmentColor;\
									  varying vec2 v_texCoord;\
									  void main()\
									  {\
											gl_FragColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);\
									  }");				
			}
			p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
			p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
			p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
			p->link();
			p->updateUniforms();
			sprite->setShaderProgram(p);
		}
	}
}