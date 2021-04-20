/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerTrailHero
 *  Description:	试炼BUFF选择英雄界面的UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/08/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerTrailHero_h__
#define LayerTrailHero_h__

#include "TowerDef.h"
#include "Utils/Observer.h"
#include "HeroModule/HeroModel.h"

typedef struct tagUI_Trail_Hero_List
{
	Layout*		pRoot;
	Button*		btnClose;
	ImageView*  imgBg;
	ScrollView*	scrollviewContent;
	CustomScrollBar* scrollBar;

	tagUI_Trail_Hero_List()
	{
		memset(this, 0, sizeof(tagUI_Trail_Hero_List));
	}
}UI_Trail_Hero_List;

class TrailHeroNode : public Layout
{
public:
	~TrailHeroNode();

	static TrailHeroNode* create(HeroParam* param);

	virtual bool  init(HeroParam* param);

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type);

	void  setLogicParent(Layer* parent) { m_parent = parent; };

private:
	TrailHeroNode();

	void  initUI(HeroParam* param);

private:
	int m_nHeroId;

	Layer*  m_parent;
};

class LayerTrailHero : public Layer
{
public:
	~LayerTrailHero(void);

	CREATE_FUNC(LayerTrailHero);

	virtual bool  init();

	virtual void  onEnter();

	virtual void  update(float delta);

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	void  onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	void  onSelectHero(const int nHeroId);

private:
	LayerTrailHero(void);

	void  initUI();

private:
	UI_Trail_Hero_List  m_ui;

	enum UI_TRAIL_HERO_WIDGET
	{
		Btn_Close
	};
};

#endif // LayerTrailHero_h__
