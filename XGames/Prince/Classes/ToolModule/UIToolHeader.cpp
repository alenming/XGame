#include "UIToolHeader.h"
#include "Common/Common.h"
#include "Utils/StringFormat.h"
#include "ToolModel.h"
#include "cocos2d.h"
#include "CCArmature.h"
#include "Utils/ResourceUtils.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;
const float MAX_SCALE_STAR = 0.40;
const int MAX_LABLE_ORDER_NUM = 9;


UIToolHeader::UIToolHeader()
{
	m_Num = 0;
	m_pFrontImgUnselected = nullptr;
	m_pFrontImgSelected = nullptr;
	m_pUseText = nullptr;
	m_pUseIcon = nullptr;
	m_pAdvIcon = nullptr;
	m_pRedIcon = nullptr;
	m_pRedDot = nullptr;
	m_pLvlIcon = nullptr;
	m_pLvlText = nullptr;
	m_pMainIcon = nullptr;
	m_pNumExIcon = nullptr;
	m_pTiedFateIcon = nullptr;
	m_pName = nullptr;
	m_pNum = nullptr;
	m_pChipMaskIcon = nullptr;
	m_pMaskIcon = nullptr;
	m_pMaskSelectIcon = nullptr;
	m_pBloodBg = nullptr;
	m_pBloodBar = nullptr;
	m_pDiedIcon = nullptr;
	m_pNumEx	 = nullptr;
	m_isSelected = false;
	m_fScaleValue = 1.0;
	m_nIconQua = -1;
	m_pCurNumAndNeedNum = nullptr;
	m_pTextBottomNum = nullptr;
	m_ntoolId = -1;
	m_ntoolDisIndex = 0;
	initUI();
	m_bEnableTips = false;
	m_pnlTips = nullptr;
	m_pHeroConerIcon = nullptr;
	m_pAddIcon = nullptr;
	m_imgTianMing = nullptr;
	m_imgLock = nullptr;
	m_imgJJLv = nullptr;
}


UIToolHeader* UIToolHeader::create(int templateId)
{
	UIToolHeader* icon = new UIToolHeader();
	if (icon && icon->init("Image/Icon/global/bg_tongyong.png"))
	{	
		icon->setStateImg("Image/Icon/global/btn_kuang_zhuangbei_1.png", 
			"Image/Icon/global/btn_kuang_zhuangbei_2.png");
		icon->setTouchEnabled(false);
		

		//碎片类型需额外加蒙板
		RowData* rowData = DataManager::getInstance()->searchToolById(templateId);
		//CCASSERT(rowData != nullptr, "UIToolHeader#create:rowData==nullptr!templateId=%d", templateId);
		if(rowData)
		{
			int resId = rowData->getIntData("resId");
			icon->setMainIcon(ResourceUtils::getSmallIconPath(resId));
			icon->checkChip(templateId);
			icon->setIconQua(rowData->getIntData("itemQua"));
			icon->autorelease();
			icon->setToolId(templateId);		//创建成功后保存ID

			return icon;
		}		
	}
	CC_SAFE_DELETE(icon);
	return nullptr;
}

UIToolHeader* UIToolHeader::create()
{
	/*UIToolHeader *btn = new UIToolHeader;
	if (btn && btn->init()) {
	btn->autorelease();
	return btn;
	}*/
	UIToolHeader *btn = new UIToolHeader();	
	if (btn && btn->init("Image/Icon/global/bg_tongyong.png"))
	{	
		btn->setStateImg("Image/Icon/global/btn_kuang_zhuangbei_1.png", "Image/Icon/global/btn_kuang_zhuangbei_2.png");//"Image/Icon/global/btn_kuang_zhuangbei_2.png"
		btn->autorelease();
		btn->setTouchEnabled(false);
		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;
}
UIToolHeader* UIToolHeader::create(const std::string &mainImage, const int id )
{
	UIToolHeader *btn = new UIToolHeader();
	if (btn && btn->init("Image/Icon/global/bg_tongyong.png"))
	{	
		btn->setStateImg("Image/Icon/global/btn_kuang_zhuangbei_1.png", "Image/Icon/global/btn_kuang_zhuangbei_2.png");// "Image/Icon/global/btn_kuang_zhuangbei_2.png"
		btn->setTouchEnabled(id != -1);
		btn->setMainIcon(mainImage);
		btn->setToolId(id);
		btn->autorelease();

		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;
}

UIToolHeader* UIToolHeader::create(const std::string& mainImage, const std::string &normalImage,
					   const std::string& selectedImage ,
					   const std::string& disableImage)
{
	UIToolHeader *btn = new UIToolHeader();
	if (btn && btn->init("Image/Icon/global/bg_tongyong.png"))
	{	
		btn->setStateImg(normalImage, selectedImage, disableImage);
		btn->setTouchEnabled(false);
		btn->setMainIcon(mainImage);
		btn->autorelease();
		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;
}

//品质
// UIToolHeader* UIToolHeader::create(const int& nQua)
// {		
// 	UIToolHeader *btn = new UIToolHeader();
// 	if (btn && btn->init(btn->getFilenameByQua(nQua)))
// 	{
// 		btn->m_nIconQua = nQua;
// 		btn->setStateImg("Image/Icon/global/btn_kuang_zhuangbei_1.png", "Image/Icon/global/btn_kuang_zhuangbei_2.png");
// 		btn->setTouchEnabled(false);
// 		btn->autorelease();
// 		return btn;
// 	}
// 	CC_SAFE_DELETE(btn);
// 	return nullptr;
// }

UIToolHeader* UIToolHeader::createEx(const std::string& selImage,
							  const std::string& unselImage,
							  const std::string &normalImage,
							  const std::string& selectedImage,
							  const std::string& disableImage)
{
	UIToolHeader *btn = new UIToolHeader();
	if (btn && btn->init(unselImage, selImage))
	{	
		btn->setStateImg(normalImage, selectedImage, disableImage);
		btn->setTouchEnabled(false);
		btn->autorelease();
		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;
}

void UIToolHeader::onPressStateChangedToNormal()
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
	float offset = (getTouchBeganPosition() - getTouchEndPosition()).getLength();
	if (offset < 20.0f)
	{
		showTips(true);
	}
}


void UIToolHeader::onPressStateChangedToPressed()
{
	Button::onPressStateChangedToPressed();	
	//Button::setScale(0.9*m_fScaleValue);
	if(m_pFrontImgUnselected != nullptr)
		m_pFrontImgUnselected->setVisible(false);
	if(m_pFrontImgSelected != nullptr)
		m_pFrontImgSelected->setVisible(true);

	//showTips(true);
}

void UIToolHeader::onPressStateChangedToDisabled()
{
	Button::onPressStateChangedToDisabled();
}

void UIToolHeader::setIconQua(const int& nQua)
{
	m_nIconQua = nQua;
	string filename = getFilenameByQua(nQua);
	this->loadTextures(filename, filename);
	//由于图片大小不一样，位置可能发生变化
	if (m_pFrontImgUnselected != nullptr)
	{		
		m_pFrontImgUnselected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
	}
	if (m_pFrontImgSelected != nullptr)
	{		
		m_pFrontImgSelected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
	}
}

void UIToolHeader::setMainIcon(const std::string& normalImage)
{
	if (normalImage == "")
	{
		return;
	}
	if (m_pMainIcon == nullptr)
	{		
		m_pMainIcon = cocos2d::ui::ImageView::create(normalImage);
		this->addChild(m_pMainIcon, 1);
		m_pMainIcon->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
	}
	else
	{
		m_pMainIcon->loadTexture(normalImage);
	}
}
void UIToolHeader::setMainIconVisible(bool isVisible)
{
	if (m_pMainIcon != nullptr)
	{
		m_pMainIcon->setVisible(isVisible);
	}
}
void UIToolHeader::setSelectedImg(const std::string& normalImage)
{
	if (m_pFrontImgSelected == nullptr)
	{
		m_pFrontImgSelected = cocos2d::ui::ImageView::create(normalImage);
		this->addChild(m_pFrontImgSelected, 5);
		m_pFrontImgSelected->setVisible(false);
		m_pFrontImgSelected->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
	}
}

void UIToolHeader::setSelected(const bool& isSelected)
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
const bool& UIToolHeader::isSelected()
{
	return m_isSelected;
}

void UIToolHeader::initUI()
{	
	for (int i=0; i<MAX_STARS; i++)
	{
		m_pStar[i] = nullptr;
	}
	for(int i=0; i<MAX_EQUIPSTARS; i++)
	{
		m_pEquipStar[i] = nullptr;
	}
	m_pSkillIcon	= nullptr;
	m_pUseIcon		= nullptr;
}

//左上角的技能图标
void UIToolHeader::setSkillIcon( const std::string& normalImage)
{
	if (m_pSkillIcon != nullptr)
	{
		this->removeChild(m_pSkillIcon);		
	}
	m_pSkillIcon = cocos2d::ui::ImageView::create(normalImage);
	if (m_pSkillIcon == nullptr)
	{
		return;
	}
	m_pSkillIcon->setVisible(true);
	m_pSkillIcon->setPosition(cocos2d::Vec2(m_pSkillIcon->getContentSize().width/2-10, this->getContentSize().height-m_pSkillIcon->getContentSize().height/2+10));
	this->addChild(m_pSkillIcon, MAX_LABLE_ORDER_NUM);
}

//右下角使用图标
void UIToolHeader::setUseIcon( const std::string& normalImage)
{
	if (normalImage == "")
	{
		if (m_pUseIcon != nullptr)
		{
			this->removeChild(m_pUseIcon);
			m_pUseIcon = nullptr;
		}
		return;
	}
	if (m_pUseIcon != nullptr)
	{
		this->removeChild(m_pUseIcon);		
	}
	m_pUseIcon = cocos2d::ui::ImageView::create(normalImage);
	if (m_pUseIcon == nullptr)
	{
		return;
	}
	m_pUseIcon->setVisible(true);
	m_pUseIcon->setPosition(cocos2d::Vec2(this->getContentSize().width-10
		, this->getContentSize().height - m_pUseIcon->getContentSize().height/2 + 15));
	this->addChild(m_pUseIcon, MAX_LABLE_ORDER_NUM);
}
void UIToolHeader::setUseText( const std::string& text)
{	
	//if (m_pUseIcon == nullptr)
	//{
	//	setUseIcon("Image/UInewbackpack/img_di3.png");
	//}

	//if (m_pUseText == nullptr)
	//{
	//	m_pUseText = cocos2d::ui::Text::create();
	//	m_pUseText->setFontSize(16);
	//	m_pUseText->setColor(cocos2d::Color3B(0xda,0x53,0x2a));
	//	m_pUseText->setFontName(FONT_FZZHENGHEI);
	//	m_pUseText->setTextHorizontalAlignment(TextHAlignment::CENTER);
	//	m_pUseText->setTextVerticalAlignment(TextVAlignment::TOP);
	//	m_pUseText->ignoreContentAdaptWithSize(false);//固定大小
	//	m_pUseText->setSize(Size(20, 70));
	//	m_pUseText->setTextAreaSize(Size(20, 70));
	//	m_pUseIcon->addChild(m_pUseText);
	//	m_pUseText->setTouchEnabled(false);
	//	//m_pUseText->setTextVerticalAlignment();
	//}
	//m_pUseText->setText(text);
	//m_pUseText->setPosition(cocos2d::Vec2(m_pUseIcon->getContentSize().width/2
	//	, m_pUseIcon->getContentSize().height/2 ));
}


//右上角结缘图标
void UIToolHeader::setTiedFateIcon( const std::string& normalImage)
{
	if (m_pTiedFateIcon == nullptr)
	{
		m_pTiedFateIcon = cocos2d::ui::ImageView::create(normalImage);
		m_pTiedFateIcon->setPosition(cocos2d::Vec2(this->getContentSize().width - m_pTiedFateIcon->getContentSize().width/2+10, this->getContentSize().height - m_pTiedFateIcon->getContentSize().height/2+10));
		this->addChild(m_pTiedFateIcon, MAX_LABLE_ORDER_NUM);
	}
	else
	{
		m_pTiedFateIcon->loadTexture(normalImage);
	}
	m_pTiedFateIcon->setVisible(true);
}
void UIToolHeader::setTiedFateIconFlag(bool isVisible)
{
	if (isVisible)
	{
		if (m_pTiedFateIcon == nullptr)
		{
			setTiedFateIcon("Image/Icon/global/img_yijieyuan.png");
		}
		else
		{
			m_pTiedFateIcon->setVisible(isVisible);
		}
	}
	else
	{
		if (m_pTiedFateIcon != nullptr)
		{
			m_pTiedFateIcon->setVisible(isVisible);
		}
	}
}
//右下角小红点图标
void UIToolHeader::setRedIcon( const std::string& normalImage)
{	
	if (m_pRedIcon == nullptr)
	{
		m_pRedIcon = cocos2d::ui::ImageView::create(normalImage);
		m_pRedIcon->setPosition(cocos2d::Vec2(this->getContentSize().width - m_pRedIcon->getContentSize().width/2+10, 
			this->getContentSize().height - m_pRedIcon->getContentSize().height/2+10));
		this->addChild(m_pRedIcon, MAX_LABLE_ORDER_NUM);
	}
	m_pRedIcon->setVisible(true);
}
void UIToolHeader::setRedIconFlag( bool flag)
{
	/*if (m_pRedIcon != nullptr)
	{
	this->removeChild(m_pRedIcon, true);
	m_pRedIcon = nullptr;
	}*/
	if (flag)
	{
		if (m_pRedIcon == nullptr)
		{
			setRedIcon("Image/Icon/global/img_gantanhao2.png");
		}
		else
		{
			m_pRedIcon->setVisible(flag);
		}
	}
	else
	{
		if (m_pRedIcon != nullptr)
		{
			m_pRedIcon->setVisible(flag);
		}
	}
}

void UIToolHeader::setRedDot(bool bShow)
{
	if (bShow)
	{
		if (m_pRedDot == nullptr)
		{
			setRedIcon("Image/Icon/global/img_dian.png");
		}
		else
		{
			m_pRedDot->setVisible(bShow);
		}
	}
	else
	{
		if (m_pRedDot != nullptr)
		{
			m_pRedDot->setVisible(bShow);
		}
	}
}

//设置血量
void UIToolHeader::setBlood(const float& fPercent)
{
	if (m_pBloodBg == nullptr)
	{
		m_pBloodBg = cocos2d::ui::ImageView::create("Image/Icon/global/img_tiao_xueliang_2.png");
		m_pBloodBg->setPosition(cocos2d::Vec2(63
			, getContentSize().height-15));
		this->addChild(m_pBloodBg, MAX_LABLE_ORDER_NUM);
	}
	if (m_pBloodBar == nullptr)
	{
		m_pBloodBar = cocos2d::ui::LoadingBar::create("Image/Icon/global/img_tiao_xueliang_1.png");
		m_pBloodBar->setPosition(cocos2d::Vec2(m_pBloodBg->getContentSize().width/2, m_pBloodBg->getContentSize().height/2));
		m_pBloodBg->addChild(m_pBloodBar);
	}
	m_pBloodBar->setPercent(fPercent);
}

//设置阵亡标记
void UIToolHeader::setDiedFlag( bool flag)
{
	if (flag)
	{
		if (m_pDiedIcon == nullptr)
		{
			m_pDiedIcon = cocos2d::ui::ImageView::create("Image/Icon/global/img_zhenwang.png");
			m_pDiedIcon->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2 - m_pDiedIcon->getContentSize().height/2));
			this->addChild(m_pDiedIcon, MAX_LABLE_ORDER_NUM);
		}
		m_pDiedIcon->setVisible(true);
	}
	else
	{
		if (m_pDiedIcon != nullptr)
			m_pDiedIcon->setVisible(false);
	}
}

//遮罩图标
void UIToolHeader::setMaskIcon( const std::string& normalImage)
{
	if (m_pMaskIcon == nullptr)
	{
		m_pMaskIcon = cocos2d::ui::ImageView::create(normalImage);
		m_pMaskIcon->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
		m_pMaskIcon->setName("MASK_ICON");
		this->addChild(m_pMaskIcon, 4);
	}
	else
	{
		m_pMaskIcon->loadTexture(normalImage);
	}
}
void UIToolHeader::setMaskFlag( bool isMask, bool isSelected)
{
	if (isMask)
	{
		if (m_pMaskIcon == nullptr)
		{
			setMaskIcon("Image/Icon/global/btn_kuang_juese_4.png");
		}
		else
		{
			m_pMaskIcon->setVisible(true);
		}
		if (isSelected)
		{
			if (m_pMaskSelectIcon == nullptr)
			{
				m_pMaskSelectIcon = cocos2d::ui::ImageView::create("Image/Icon/global/img_xuanzhong.png");
				m_pMaskSelectIcon->setPosition(cocos2d::Vec2(m_pMaskIcon->getContentSize().width - m_pMaskSelectIcon->getContentSize().width/2 - 20.0f,
					m_pMaskSelectIcon->getContentSize().height/2));
				this->addChild(m_pMaskSelectIcon, MAX_LABLE_ORDER_NUM+1);
			}			
			m_pMaskSelectIcon->setVisible(true);
		}
		else
		{
			if (m_pMaskSelectIcon != nullptr)
				m_pMaskSelectIcon->setVisible(false);
		}
	}
	else
	{
		if (m_pMaskIcon != nullptr)
			m_pMaskIcon->setVisible(false);

		if (m_pMaskSelectIcon != nullptr)
			m_pMaskSelectIcon->setVisible(false);
	}
}

// 设置碎片遮罩;
void UIToolHeader::checkChip(const int nItemId)
{
	RowData* rowData = DataManager::getInstance()->searchToolById(nItemId);
	if (nullptr != rowData)
	{
		setChipMaskIcon((TOOL_ITEM_TYPE)rowData->getIntData("itemTyp"),
			(TOOL_ITEM_QUA)rowData->getIntData("itemQua"));
	}
}

//设置碎片遮罩
void UIToolHeader::setChipMaskIcon( TOOL_ITEM_TYPE itemType, TOOL_ITEM_QUA itemQua )
{
	bool bShow = (itemType == TOOL_ITEM_TYPE_HERO_CHIP || itemType == TOOL_ITEM_TYPE_BWCHIP);

	if (bShow)
	{
		string strName("");
		if (itemType == TOOL_ITEM_TYPE_BWCHIP)
		{
			string tmp = "btn_kuang_juese_suipian" + _TO_STR(max((int)(itemQua - TOOL_ITEM_QUA_GREEN), 0)) +".png";
			strName = ResourceUtils::getIconGlobalPath(tmp);
		}
		else
		{
			strName = ResourceUtils::getIconGlobalPath("btn_kuang_juese_suipian3.png");
		}

		if (m_pChipMaskIcon == nullptr)
		{
			m_pChipMaskIcon = cocos2d::ui::ImageView::create(strName);
			m_pChipMaskIcon->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));
			this->addChild(m_pChipMaskIcon, 4);
		}
		else
		{
			m_pChipMaskIcon->loadTexture(strName);
		}
		m_pChipMaskIcon->setVisible(true);
	}
	else
	{
		if (m_pChipMaskIcon)
		{
			m_pChipMaskIcon->setVisible(false);
		}
	}
	
}

//装备可进阶
void UIToolHeader::setAdvFlag( bool flag)
{
	if (flag)
	{
		if (m_pAdvIcon == nullptr)
		{
			m_pAdvIcon = cocos2d::ui::ImageView::create("Image/Icon/global/img_jiantou3.png");
			m_pAdvIcon->setScale(1.5f);
			m_pAdvIcon->setPosition(cocos2d::Vec2(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f));
			this->addChild(m_pAdvIcon, MAX_LABLE_ORDER_NUM);

			// 增加动画效果;
			Sequence* seq = Sequence::create(MoveBy::create(0.5f, Vec2(0.0f, 6.0f)), MoveBy::create(0.5f, Vec2(0.0f, -6.0f)), nullptr);
			m_pAdvIcon->runAction(RepeatForever::create(seq));
		}
		m_pAdvIcon->setVisible(true);
	}
	else
	{
		if (m_pAdvIcon != nullptr)
		{
			m_pAdvIcon->stopAllActions();
			m_pAdvIcon->removeFromParent();
			m_pAdvIcon = nullptr;
		}
	}
}

void UIToolHeader::setEquipStars(int stars)
{
	for (int i=0; i<MAX_EQUIPSTARS; i++)
	{
		if (m_pEquipStar[i] != nullptr)
			m_pEquipStar[i]->setVisible(false);
	}
	if (stars<=0 || stars>MAX_EQUIPSTARS)
	{
		return;
	}	

	//居左显示
	for (int i=0; i<stars; i++)
	{
		if (m_pEquipStar[i] == nullptr)
		{
			m_pEquipStar[i] = cocos2d::ui::ImageView::create("Image/Icon/global/img_icon_jieshu.png");
			m_pEquipStar[i]->setTouchEnabled(false);
			this->addChild(m_pEquipStar[i], MAX_LABLE_ORDER_NUM);
		}
		else
		{
			m_pEquipStar[i]->setVisible(true);
		}
		m_pEquipStar[i]->setPosition(Point(m_pEquipStar[i]->getContentSize()/2) + Point(i*m_pEquipStar[i]->getContentSize().width, 2));
	}	
}

const int UIToolHeader::getNum()
{
	return m_Num;
}

void UIToolHeader::setNumEx(int num, cocos2d::Color3B color)
{	
	if (num < 0)
	{
		if (m_pNumExIcon != nullptr)
			m_pNumExIcon->setVisible(false);
		if (m_pNumEx != nullptr)
		{
			m_pNumEx->setText("");
		}
		return;
	}
	m_Num = num;
	string textString = StringFormat::formatNumber(num, false); //显示的数字

	if (m_pNumExIcon == nullptr)
	{	
		//自动根据数量大小做相应缩放
		m_pNumExIcon = ImageView::create("Image/Icon/global/img_shuliang_di.png");
		m_pNumExIcon->setTouchEnabled(false);
		Text* text = Text::create(textString + " ",FONT_FZZHENGHEI,20);
		m_pNumExIcon->setScale9Enabled(true);
		float widthSize = 36.0f;
		if (text->getContentSize().width > widthSize) //最小要36像素
		{
			widthSize = text->getContentSize().width * 1.2;
		}
		m_pNumExIcon->setContentSize(CCSize(widthSize,m_pNumExIcon->getCustomSize().height));
		m_pNumExIcon->setPosition(cocos2d::Vec2(this->getContentSize().width - widthSize/2, 
			m_pNumExIcon->getCustomSize().height/2));
		this->addChild(m_pNumExIcon, 4);
	}
	else //可能数字改变 又要变宽度
	{
		Text* text = Text::create(textString + " ",FONT_FZZHENGHEI,20);
		float widthSize = 36.0f;
		if (text->getContentSize().width > widthSize) //最小要36像素
		{
			widthSize = text->getContentSize().width * 1.2;
		}
		m_pNumExIcon->setContentSize(CCSize(widthSize,m_pNumExIcon->getCustomSize().height));
		m_pNumExIcon->setPosition(cocos2d::Vec2(this->getContentSize().width - widthSize/2, 
			m_pNumExIcon->getCustomSize().height/2));
	}

	if (m_pNumEx == nullptr)
	{
		m_pNumEx = cocos2d::ui::Text::create();
		m_pNumEx->setFontSize(24);
		m_pNumEx->setColor(color);
		//m_pNumEx->enableOutline(cocos2d::Color4B(0xff,0x00,0x00,0x00), 1);
		setTextAddStroke(m_pNumEx, cocos2d::Color3B(0xff,0x00,0x00),1);
		m_pNumEx->setFontName(FONT_FZZHENGHEI);
		m_pNumExIcon->addChild(m_pNumEx);
		m_pNumEx->setTouchEnabled(false);
 		m_pNumEx->setPosition(cocos2d::Vec2(m_pNumExIcon->getContentSize().width/2
 			, m_pNumExIcon->getContentSize().height/2));
	}
	else
	{
		m_pNumEx->setPosition(cocos2d::Vec2(m_pNumExIcon->getContentSize().width/2
			, m_pNumExIcon->getContentSize().height/2));
	}
	m_pNumExIcon->setVisible(true);
	m_pNumEx->setText(textString);

	//如果是英雄图标则显示星级
	showPetBaseHeroStars();
}
//星级
void UIToolHeader::setStarsNum(int num, EnumStarContain starContain)
{
	for (int i=0; i<MAX_STARS; i++)
	{
		if (m_pStar[i] != nullptr)
			m_pStar[i]->setVisible(false);
	}
	if (num<=0 || num>MAX_STARS)
	{
		return;
	}	
	float fbeginX = this->getContentSize().width/2 - (float)num/2.0f * 47*MAX_SCALE_STAR;//MAX_SCALE_STAR*
	float fY = -10;
	if (starContain == STAR_CONTAIN_BOTTOM)
	{
		fY = 10;
	}
	else if (starContain == STAR_CONTAIN_BOTTOM_BESIDE)
	{
		fY = -10;
	}
	else if (starContain == STAR_CONTAIN_TOP_BESIDE)
	{
		fY = this->getContentSize().height + 15;
	}
	else //STAR_CONTAIN_TOP
	{
		fY = this->getContentSize().height - 15;
	}
	//居中显示
	for (int i=num-1; i>=0; i--)
	{
		if (m_pStar[i] == nullptr)
		{
			m_pStar[i] = cocos2d::ui::ImageView::create("Image/Icon/global/img_star1.png");
			//m_pStar[i]->setScale(MAX_SCALE_STAR);
			m_pStar[i]->setTouchEnabled(false);
			this->addChild(m_pStar[i], MAX_LABLE_ORDER_NUM);
			m_pStar[i]->setScale(0.5f);
		}
		else
		{
			m_pStar[i]->setVisible(true);
		}
		/*
		m_pStar[i]->setPosition(cocos2d::Vec2(
				fbeginX + ((m_pStar[i]->getContentSize().width-8)*i)*MAX_SCALE_STAR + m_pStar[i]->getContentSize().width/2* MAX_SCALE_STAR + (num-1)*4/3
				, fY ));//m_pStar[i]->getContentSize().height/2 * MAX_SCALE_STAR
		*/
	}
	//反逻辑的星星层级，只好再写次循环控制坐标
	for(int i=0; i<num; i++)
	{
		if(i == 0)
		{
			m_pStar[i]->setPosition(cocos2d::Vec2(14, fY ));
		}
		m_pStar[i]->setPosition(cocos2d::Vec2(m_pStar[0]->getPositionX() + (m_pStar[0]->getContentSize().width/2 - 9)*i, fY ));
	}
}

//品质角标
void UIToolHeader::setQuaCornerIcon(int heroTemplateId, int heroAdvLvl, bool isRobot)
{
	int id = heroTemplateId * 100 + heroAdvLvl;
	setQuaCornerIcon(id, isRobot);
}

void UIToolHeader::setQuaCornerIcon(int heroAdvanceId, bool isRobot)
{
	RowData* advInfo = nullptr;
	if(!isRobot)		//真实玩家
	{
		advInfo = DataManager::getInstance()->searchHeroAdvanceById(heroAdvanceId);
	}
	else                //机器人
	{
		advInfo = DataManager::getInstance()->searchMonsterById(heroAdvanceId);
	}
	if(advInfo && advInfo->getIntData("qua") >= 2)		//绿色以上品质才有角标
	{
		if(m_pHeroConerIcon == nullptr)
		{
			string iconId = "img_juexing_" + _TO_STR(advInfo->getIntData("qua")-1) + "_" + _TO_STR(advInfo->getIntData("mark"));
			m_pHeroConerIcon = cocos2d::ui::ImageView::create(ResourceUtils::getCornerIconPatn(iconId));
			if(m_pHeroConerIcon)
			{
				this->addChild(m_pHeroConerIcon, MAX_LABLE_ORDER_NUM-1);
				m_pHeroConerIcon->setPosition(Vec2(getContentSize().width - 15, getContentSize().height - 15));
			}
		}
		else
		{
			string iconId = "img_juexing_" + _TO_STR(advInfo->getIntData("qua")-1) + "_" + _TO_STR(advInfo->getIntData("mark"));
			m_pHeroConerIcon->loadTexture(ResourceUtils::getCornerIconPatn(iconId));
		}
		m_pHeroConerIcon->setVisible(true);
	}
	else
	{
		if(m_pHeroConerIcon)
		{
			m_pHeroConerIcon->setVisible(false);
		}
	}
}

//等级
void UIToolHeader::setLvl(int nLvl)
{
	if (nLvl <= 0)
	{
		return;
	}
	if (m_pLvlIcon == nullptr)
	{	
		m_pLvlIcon = ImageView::create("Image/Icon/global/img_dengji_di.png");
		float fx = 10;
		m_pLvlIcon->setPosition(cocos2d::Vec2(fx
			, getContentSize().height-10));
		m_pLvlIcon->setTouchEnabled(false);
		this->addChild(m_pLvlIcon, MAX_LABLE_ORDER_NUM);
	}
	m_pLvlIcon->setVisible(true);
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
	m_pLvlText->setVisible(true);

	if(this->getChildByName("LvText") != nullptr)
	{
		this->getChildByName("LvText")->removeFromParent();
	}
	
}

//不带圆圈背景等级
void UIToolHeader::setLvlWithoutBg(int nLvl)
{
	if (nLvl <= 0)
	{
		return;
	}
	if(m_pLvlIcon)
	{
		m_pLvlIcon->setVisible(false);
	}
	if(m_pLvlText)
	{
		m_pLvlText->setVisible(false);
	}
	cocos2d::ui::Text* lvText = nullptr;
	if(this->getChildByName("LvText") != nullptr)
	{
		lvText = (ui::Text*)this->getChildByName("LvText");
	}
	else
	{
		lvText = cocos2d::ui::Text::create();
		lvText->setFontName(FONT_PHATGUY);
		lvText->setFontSize(25);
		lvText->setColor(cocos2d::Color3B(0xff,0xfc,0x22));
		this->addChild(lvText, MAX_LABLE_ORDER_NUM);
		lvText->setTouchEnabled(false);
		lvText->setName("LvText");
		lvText->setPosition(cocos2d::Vec2(10
			, getContentSize().height-10));
	}
	lvText->setString(TO_STR(nLvl));
	setTextAddStroke(lvText, Color3B(0x17, 0x00, 0x00), 3);
}

//进阶等级
void UIToolHeader::setJJLvl( TOOL_ITEM_QUA nQua, int nLv )
{
	ostringstream oss;
	oss << "Image/Icon/global/";
	switch (nQua)
	{
	case TOOL_ITEM_QUA_GREEN:	oss << "img_juexing_1_";	break;
	case TOOL_ITEM_QUA_BLUE:	oss << "img_juexing_2_";	break;
	case TOOL_ITEM_QUA_PURPLE:	oss << "img_juexing_3_";	break;
	case TOOL_ITEM_QUA_ORANGE:	oss << "img_juexing_4_";	break;
	case TOOL_ITEM_QUA_RED:		oss << "img_juexing_5_";	break;
	default:
		{
			// 其余品质不处理，但不报错;
			// 需要隐藏进阶图标，否则切换英雄时获取到的UI图标还会显示该进阶图标
			if(m_imgJJLv)
			{
				m_imgJJLv->setVisible(false);
			}
			
			return;
		}
		break;
	}

	// 等级限制;
	if (nLv < 0 || nLv > 4)
		return;

	oss << nLv << ".png";
	if (m_imgJJLv == nullptr)
	{
		m_imgJJLv = cocos2d::ui::ImageView::create(oss.str());
		m_imgJJLv->setPosition(Vec2(this->getContentSize().width - m_imgJJLv->getContentSize().width/2.0f + 12.0f,
			this->getContentSize().height - m_imgJJLv->getContentSize().height/2.0f + 12.0f));
		this->addChild(m_imgJJLv, 9);
	}
	else
	{
		m_imgJJLv->setVisible(true);
		m_imgJJLv->loadTexture(oss.str());
	}
}

void UIToolHeader::setCurNumAndNeedNum(int curNum, int needNum)
{
	if (m_pCurNumAndNeedNum == nullptr)
	{
		if (m_pNumExIcon == nullptr)
		{	
			m_pNumExIcon = ImageView::create("Image/Icon/global/img_shuliang_di.png");
			m_pNumExIcon->setAnchorPoint(Point(1.0f, 0.0f));
			m_pNumExIcon->setPosition(cocos2d::Vec2(getContentSize().width, 0.0f));
			m_pNumExIcon->setTouchEnabled(false);
			this->addChild(m_pNumExIcon, MAX_LABLE_ORDER_NUM);
		}
		m_pCurNumAndNeedNum = cocos2d::ui::Text::create();
		m_pCurNumAndNeedNum->setFontSize(20);
		m_pCurNumAndNeedNum->setFontName(FONT_FZZHENGHEI);
		this->addChild(m_pCurNumAndNeedNum, MAX_LABLE_ORDER_NUM);
		//m_pNumExIcon->addChild(m_pCurNumAndNeedNum);
		//m_pCurNumAndNeedNum->setPosition(cocos2d::Vec2(m_pNumExIcon->getContentSize().width/2
		//	, m_pNumExIcon->getContentSize().height/2));
		m_pCurNumAndNeedNum->setTouchEnabled(false);
	}


	string str = TO_STR(curNum) + "/" + TO_STR(needNum);
	m_pCurNumAndNeedNum->setString(str);
	m_pCurNumAndNeedNum->setAnchorPoint(Point(1.0f, 0.0f));
	m_pCurNumAndNeedNum->setPosition(Point(getContentSize().width, 0.0f));

	if (m_pCurNumAndNeedNum->getContentSize().width > m_pNumExIcon->getContentSize().width)
	{
		m_pNumExIcon->setScaleX((m_pCurNumAndNeedNum->getContentSize().width+5)/m_pNumExIcon->getContentSize().width);
	}	
}

void UIToolHeader::setCurNumAndNeedNumColor(cocos2d::Color3B clr)
{
	m_pCurNumAndNeedNum->setColor(clr);
}

//设置底部数字
void UIToolHeader::setBottomNum(std::string strText)
{
	if (m_pNumExIcon == nullptr)
	{	
		m_pNumExIcon = ImageView::create("Image/Icon/global/img_icon_di.png");
		//m_pNumExIcon->setAnchorPoint(Point(1.0f, 0.0f));
		m_pNumExIcon->setPosition(cocos2d::Vec2(getContentSize().width/2, m_pNumExIcon->getContentSize().height/2+5));
		m_pNumExIcon->setTouchEnabled(false);
		this->addChild(m_pNumExIcon, MAX_LABLE_ORDER_NUM);
		m_pNumExIcon->setScaleX(1.5);
		if(m_pAddIcon)			//如果有遮罩
		{
			m_pNumExIcon->setZOrder(0);
		}	
	}
	if (m_pTextBottomNum == nullptr)
	{
		m_pTextBottomNum = cocos2d::ui::Text::create();
		m_pTextBottomNum->setFontSize(20);
		m_pTextBottomNum->setFontName(FONT_FZZHENGHEI);
		m_pTextBottomNum->setColor(cocos2d::Color3B(0x54, 0xdd,0x51));
		this->addChild(m_pTextBottomNum, MAX_LABLE_ORDER_NUM+1);
		m_pTextBottomNum->setPosition(m_pNumExIcon->getPosition() - Point(0, 2));	
	}	
	//
	m_pTextBottomNum->setString(strText);	
}

void UIToolHeader::setBottomNumColor(Color3B color)
{
	if (m_pTextBottomNum != nullptr)
	{
		m_pTextBottomNum->setColor(color);
	}
}

void UIToolHeader::setAddCornerIcon( bool bAdd /*= true*/ )
{
	if(m_pAddIcon != nullptr)
	{
		this->removeChild(m_pAddIcon);
		m_pAddIcon = nullptr;
	}

	if (!bAdd)
		return;
	
	m_pAddIcon = ImageView::create("Image/Icon/global/btn_kuang_zhuangbei_jia2.png");
	if(m_pAddIcon == nullptr)
	{
		return;
	}
	m_pAddIcon->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_pAddIcon->setPosition(this->getContentSize()/2);
	
	this->addChild(m_pAddIcon, 1);

	if(m_pNumExIcon)
	{
		m_pNumExIcon->setZOrder(0);
	}
}

//重置状态
void UIToolHeader::reset()
{
	if (m_pUseIcon != nullptr)
	{
		m_pUseIcon->setVisible(false);
	}
	/*if (m_pSkillIcon != nullptr)
	{
	m_pSkillIcon->setVisible(false);
	}*/
	for (int i=0; i<MAX_STARS; i++)
	{
		if (m_pStar[i] != nullptr)
		{
			m_pStar[i]->setVisible(false);
		}
	}
	for (int i=0; i<MAX_EQUIPSTARS; i++)
	{
		if(m_pEquipStar[i] != nullptr)
		{
			m_pEquipStar[i]->setVisible(false);
		}
	}
	if (m_pName != nullptr)
	{
		m_pName->setText("");
	}
	if (m_pNum != nullptr)
	{
		m_pNum->setText("");
	}
	if (m_pLvlIcon != nullptr)
	{
		m_pLvlIcon->setVisible(false);
	}
	if (m_pLvlText != nullptr)
	{
		m_pLvlText->setText("");
	}
	if (m_pHeroConerIcon != nullptr)
	{
		m_pHeroConerIcon->setVisible(false);
	}
	if (m_pAddIcon != nullptr)
	{
		m_pAddIcon->setVisible(false);
	}
	setAdvFlag(false);
	m_Num = 0;
}
void UIToolHeader::setStateImg(const std::string &normalImage,
			const std::string& selectedImage,
			const std::string& disableImage)
{
	//cocos2d::ui::Button* pBtn = cocos2d::ui::Button::create(normalImage, selectedImage, disableImage);
	//this->addChild(pBtn, 9);
	////pBtn->setTouchEnabled(false);
	//pBtn->setPosition(cocos2d::Vec2(this->getContentSize().width/2, this->getContentSize().height/2));

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

void UIToolHeader::setScale(float fScale)
{
	Button::setScale(fScale);
	m_fScaleValue = fScale;
}

string UIToolHeader::getFilenameByQua(const int& nQua)
{
	if (nQua == TOOL_ITEM_QUA_GREEN)//绿
	{
		return "Image/Icon/global/bg_green.png";
	}
	else if (nQua == TOOL_ITEM_QUA_BLUE)//蓝
	{
		return "Image/Icon/global/bg_blue.png";
	}
	else if (nQua == TOOL_ITEM_QUA_PURPLE)//紫
	{
		return "Image/Icon/global/bg_purple.png";
	}
	else if (nQua == TOOL_ITEM_QUA_ORANGE)//橙
	{
		return "Image/Icon/global/bg_yellow.png";
	}
	else if (nQua == TOOL_ITEM_QUA_RED)
	{
		return "Image/Icon/global/bg_red.png";
	}
	else
	{
		return "Image/Icon/global/bg_tongyong.png";
	}
}


void UIToolHeader::setGray(const bool isGray)
{
	if (m_pMainIcon != nullptr)
	{		
		Node* sprite = m_pMainIcon->getVirtualRenderer();
		if(sprite)
		{	
			GLProgram * p = nullptr;
			if (isGray)
			{		
				//p->initWithFilenames("shader/gray.vsh", "shader/gray.fsh");
				//p->initWithByteArrays
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
				//p->initWithFilenames("shader/gray.vsh", "shader/grayReset.fsh");
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

void UIToolHeader::setToolId(const int& id)
{
	m_ntoolId = id;
}
int UIToolHeader::getToolId()
{
	return m_ntoolId;
}
void UIToolHeader::setToolDisIndex(const int& index)
{
	m_ntoolDisIndex = index;
}
int UIToolHeader::getToolDisIndex()
{
	return m_ntoolDisIndex;
}

void UIToolHeader::showEquipEffe()
{
	this->removeChildByName("AniZhuangbeiEffe");
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniZhuangbeiEffe/AniZhuangbeiEffe.ExportJson");
	Armature* pArmature = Armature::create("AniZhuangbeiEffe");//名字需要和 .ExportJson一致

	//pArmature->setScale(1.124);//1.15	
	pArmature->getAnimation()->playByIndex(0);
	//设置动画精灵位置 
	pArmature->setPosition(this->getContentSize().width/2,this->getContentSize().height/2);
	this->addChild(pArmature,19, "AniZhuangbeiEffe");
}

void UIToolHeader::removeEquipEffe()
{
	this->removeChildByName("AniZhuangbeiEffe");
}

void UIToolHeader::showMatEffect(bool showEffect)
{
	if (showEffect)
	{
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniCailiaoEffe/AniCailiaoEffe.ExportJson");
		Armature* pArmature = Armature::create("AniCailiaoEffe");
		pArmature->setScale(1.3);
		pArmature->setPosition(this->getContentSize() / 2);
		this->addChild(pArmature, 19, "AniCailiaoEffe");
	}
	else
	{
		this->removeChildByName("AniZhuangbeiEffe");
	}
}

void UIToolHeader::setTipsEnabled(bool bEnabled)
{
	this->setTouchEnabled(bEnabled);
	m_bEnableTips = bEnabled;
}

void UIToolHeader::showTips( bool bShow )
{
	if (!m_bEnableTips || m_ntoolId == -1)
	{
		return;
	}

	// 显示tips;
	if (bShow && !Director::getInstance()->getRunningScene()->getChildByName("m_pnlTips"))
	{
		m_pnlTips = ItemTipInfo::create(m_ntoolId);
		Director::getInstance()->getRunningScene()->addChild(m_pnlTips,999,"m_pnlTips");
	}
}

void UIToolHeader::showTianMingFlag( bool bShow )
{
	if (bShow)
	{
		if (m_imgTianMing == nullptr)
		{
			m_imgTianMing = ImageView::create("Image/Icon/global/img_tianming.png");
			m_imgTianMing->setPosition(Vec2(15.0f, this->getContentSize().height/2.0f));
			this->addChild(m_imgTianMing, 9);
		}
	}
	else
	{
		if (m_imgTianMing != nullptr)
		{
			m_imgTianMing->removeFromParent();
			m_imgTianMing = nullptr;
		}
	}
}

void UIToolHeader::setCLLevel( int nCLLevel )
{
	int level = max(0, nCLLevel);
	level = min(level, 5);

	float fRight = this->getContentSize().width + 2.0f;
	float fSpace = 17.0f;
	ostringstream oss;
	for (int i = 1; i <= 5; ++i)
	{
		oss.str("");
		oss << "cl_level_img_" << i;

		ImageView* img = dynamic_cast<ImageView*>(this->getChildByName(oss.str()));
		if (i <= level)
		{
			if (img == nullptr)
			{
				img = ImageView::create("Image/Icon/global/img_icon_jieshu.png");
				img->setPosition(Vec2(fRight - fSpace/2.0f - fSpace*(i-1), 2.0f));
				img->setName(oss.str());
				img->setScale(0.8f);
				this->addChild(img, 9);
			}
		}
		else
		{
			if (img != nullptr)
			{
				img->removeFromParent();
			}
		}

	}
}

void UIToolHeader::lock( bool blocked )
{
	if (blocked)
	{
		setMaskFlag(true, false);
		if (m_imgLock == nullptr)
		{
			m_imgLock = ImageView::create("Image/Icon/global/img_lock.png");
			m_imgLock->setPosition(Vec2(this->getContentSize().width/2.0f, this->getContentSize().height/2.0f));
			this->addChild(m_imgLock, 5);
		}
	}
	else
	{
		setMaskFlag(false, false);
		if (m_imgLock != nullptr)
		{
			m_imgLock->removeFromParent();
			m_imgLock = nullptr;
		}
	}
	this->setTouchEnabled(!blocked);
}

void UIToolHeader::showTipsNow( bool isShow, float pos )
{
	if(m_pFrontImgUnselected != nullptr)
		m_pFrontImgUnselected->setVisible(!isShow);
	if(m_pFrontImgSelected != nullptr)
		m_pFrontImgSelected->setVisible(isShow);

	if (pos < 20.0f)
	{
		showTips(isShow);
	}
}

void UIToolHeader::showPetBaseHeroStars()
{
	RowData* rowData = DataManager::getInstance()->searchToolById(m_ntoolId);
	if(rowData)
	{
		//如果是英雄，则不显示数量，显示星级
		if(rowData->getIntData("itemTyp") == 4)
		{
			setNumEx(-1);		//数量

			//查pet_base表中星级
			RowData* rowDataPet = DataManager::getInstance()->searchCharacterById(m_ntoolId);
			if(rowDataPet)
			{
				setStarsNum(rowDataPet->getIntData("stars"), STAR_CONTAIN_BOTTOM);
			}
		}
	}
}

void UIToolHeader::showLightEff()
{
	RowData* rowData = DataManager::getInstance()->searchToolById(m_ntoolId);
	if(rowData)
	{
		//道具是否要展示流光特效
		if(rowData->getIntData("isEff") == 1)
		{
			ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anislashlight/Anislashlight.ExportJson");
			Armature* lightAni = Armature::create("Anislashlight");
			lightAni->setPosition(Point(this->getContentSize()/2) + Point(0, this->getContentSize().height/2));
			lightAni->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
			lightAni->setName("LIGHT_ANI");
			lightAni->getAnimation()->play("Animation1");
			this->addChild(lightAni, 99);	
		}
	}		
}

