/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		CustomScrollBar
 *  Description:	对ui::ScrollView的封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			12/16/2014
 *  Others:			封装的目的是增加滚动条;
 *
 ******************************************************************/
#ifndef CustomScrollView_h__
#define CustomScrollView_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "cocostudio.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;

typedef struct tagUI_ScrollBar
{
	ImageView*  imgBg_head;
	ImageView*  imgBg_body;
	ImageView*  imgBg_tail;

	ImageView*  imgBar_head;
	ImageView*  imgBar_body;
	ImageView*  imgBar_tail;

	tagUI_ScrollBar()
	{
		imgBg_head = nullptr;
		imgBg_body = nullptr;
		imgBg_tail = nullptr;
		imgBar_head  = nullptr;
		imgBar_body  = nullptr;
		imgBar_tail = nullptr;
	}
}UI_ScrollBar;

class CustomScrollBar : public Node
{
public:
	~CustomScrollBar(void);

	CREATE_FUNC(CustomScrollBar);

	virtual bool  init();

private:
	CustomScrollBar(void);


public:
	// 设置滚动条容器外观尺寸(只设置长度，宽度由滚动条背景图片决定);
	void  initScrollBar(const float contentHeight);

	// 设置滚动条外观比例;
	void  setScrollBarLengthPercent(float fLengthPercent);

	// 设置滚动条当前位置(Y轴方向比例);
	void  setScrollBarPercentY(float fPercentY);

private:

	UI_ScrollBar  m_uiBar;

	// 滚动条长度;
	float  m_fScrollBarLength;
};

#endif // CustomScrollView_h__
