/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerTrailRanking
 *  Description:	试炼排行榜;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			05/15/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerTrailRanking_h__
#define LayerTrailRanking_h__

#include "TowerDef.h"
#include "Utils/Observer.h"
#include "RankNode.h"

class LayerTrailRanking : public Layer, public Observer
{
public:
	~LayerTrailRanking(void);

	//CREATE_FUNC(LayerTrailRanking);
	static LayerTrailRanking* create(TOWER_TYPE towerType, R_WND_TYPE wndType = R_WND_TYPE_RANKING);

	virtual bool  init(TOWER_TYPE towerType, R_WND_TYPE wndType = R_WND_TYPE_RANKING);
	virtual void  onEnter();
	virtual void  onExit();

	virtual void  updateSelf(void* pData);

	void  onRankingNodeClicked(const int nIndex);

private:
	LayerTrailRanking(void);

	void  update(float dt);

	void  initUI();

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	void  onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	void  updateRanking(UI_TRAIL_RANKING_PARAM* data);

private:

	UI_TrailRanking  m_ui;

	map<int, RankNode*>  m_mapRankingNode;

	R_WND_TYPE  m_wndType;

	TOWER_TYPE  m_towerType;

	// 通用阵容页面;
	UI_TRAIL_RANKING_PARAM  m_RankingParam;;

	enum UI_RANK_WIDGET
	{
		Btn_Close,
		Img_Title_Reward,
		Img_Title_Ranking,
		Img_Ranking_Desc,
		Lab_Ranking,
		Img_Max_Floor_Desc,
		Lab_Max_Floor,
		Lab_Reward_Desc,
		Img_Content_Bg,
		ScrollView_Content
	};
};

#endif // LayerTrailRanking_h__
