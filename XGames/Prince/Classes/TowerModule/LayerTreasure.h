/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerTreasure
 *  Description:	发现宝藏界面;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			04/20/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerTreasure_h__
#define LayerTreasure_h__

#include "TowerDef.h"
#include "Utils/Observer.h"
#include "LayerTrail.h"


class TreasureNode : public Node
{
public:
	~TreasureNode();

	static TreasureNode* create(int nTemplateId, int nCount, string strName);

	virtual bool  init(int nTemplateId, int nCount, string strName);

	void  setOpened(bool  bOpened);

	bool  isOpend() { return m_bIsOpend; };

	int   getCount() { return m_nCount; };

private:
	TreasureNode();

	void  initUI(int nTemplateId, int nCount, string strName);

private:
	UI_Treasure_Node  m_ui;

	bool  m_bIsOpend;

	int   m_nCount;

};

class LayerTrail;
class LayerTreasure : public Layer, public Observer
{
public:
	~LayerTreasure(void);

	static LayerTreasure* create();

	virtual bool  init();

	virtual void  onEnter();
	virtual void  onExit();

	virtual void  update(float delta);

	virtual void  updateSelf(void* pData);

	void  onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName );

	void  refreshTreasure(multimap<int, int> mapInfo);

	void  openTreasure();

	void  checkIsTreasureOpend(float delta);

	// 设置父节点;
	void  setParent(LayerTrail* parent) { m_pParent = parent; };

private:
	LayerTreasure(void);

	void  showTouchContinue();

	void  onLayerTouched(Ref* ref, Widget::TouchEventType type);

	void  initUI();

	void  updateGold();

private:

	UI_Treasure  m_ui;

	multimap<int, TreasureNode*>  m_mapTreasure;

	// 父节点;
	LayerTrail*  m_pParent;

	vector<int>  m_vcOpendTreasure;

	bool  m_bIsFirstEnter;

	Armature*  _armatureLight;

	enum UI_TREASURE_WIDGET
	{
		Lab_Gold,
		Btn_Close,
		Btn_Open,
		Lab_Cost,
		Btn_Refresh,
		Lab_Refresh_Cost
	};
};

#endif // LayerTreasure_h__
