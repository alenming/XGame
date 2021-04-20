/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerSign
 *  Description:	ÃÔ¹¬·­ÅÆ½±Àø£¬ÉÌ³Ç³é¿¨½±ÀøÍ¨ÓÃ;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			06/29/2015
 *  Others:			
 *
 ******************************************************************/
#ifndef UICardPrize_h_
#define UICardPrize_h_

#include "CocosGUI.h"
#include "cocos2d.h"
#include "CocoStudio.h"
#include "ToolDef.h"
#include "DataManager/DataManager.h"
#include "Utils/ResourceUtils.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

enum UI_Card_Prize_Type
{
	CARD_TYPE_WHITE = 1,   //°×
	CARD_TYPE_GREEN,   //ÂÌ
	CARD_TYPE_BLUE,    //À¶
	CARD_TYPE_PURPLE,  //×Ï
	CARD_TYPE_ORANGE,  //³È
};
const int _STAR_CONT = 6;

class UICardPrize :public Layout
{
public:
	UICardPrize(void);
	~UICardPrize(void);

	//Æ·ÖÊ£¬ÎÄ¼þÃû£¬ÊÇ·ñÊÇ¿¨ÅÆ£¬ÊÇ·ñÊÇËéÆ¬
	static UICardPrize* create(int id);

	bool initUi(int id);

	void setCardCount(const int& count);
	void setCardName(const string& name);

private:
	Layout*      m_pRoot;
	ImageView*   m_pBackImg;   //±³¾°Í¼Æ¬£¨°×£¬ÂÌ£¬À¶£¬×Ï£¬³È£©
	ImageView*   m_pCardImg;   //¿ªÅÆÍ¼Æ¬

	ImageView*   m_pChipImg;   //ËéÆ¬Í¼Æ¬
	ImageView*   m_pStarPanelImg;   //ÐÇÐÇmainboad
	ImageView*   m_pStar[_STAR_CONT];   //ÐÇÐÇ

	Text*        m_pCount;     //ÊýÁ¿
	Text*        m_pName;      //Ãû×Ö
};
#endif 
