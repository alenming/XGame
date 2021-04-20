/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerSign
 *  Description:	�Թ����ƽ������̳ǳ鿨����ͨ��;
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
	CARD_TYPE_WHITE = 1,   //��
	CARD_TYPE_GREEN,   //��
	CARD_TYPE_BLUE,    //��
	CARD_TYPE_PURPLE,  //��
	CARD_TYPE_ORANGE,  //��
};
const int _STAR_CONT = 6;

class UICardPrize :public Layout
{
public:
	UICardPrize(void);
	~UICardPrize(void);

	//Ʒ�ʣ��ļ������Ƿ��ǿ��ƣ��Ƿ�����Ƭ
	static UICardPrize* create(int id);

	bool initUi(int id);

	void setCardCount(const int& count);
	void setCardName(const string& name);

private:
	Layout*      m_pRoot;
	ImageView*   m_pBackImg;   //����ͼƬ���ף��̣������ϣ��ȣ�
	ImageView*   m_pCardImg;   //����ͼƬ

	ImageView*   m_pChipImg;   //��ƬͼƬ
	ImageView*   m_pStarPanelImg;   //����mainboad
	ImageView*   m_pStar[_STAR_CONT];   //����

	Text*        m_pCount;     //����
	Text*        m_pName;      //����
};
#endif 
