/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		RankNode
 *  Description:	排行榜节点的UI封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			11/18/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef RankNode_h__
#define RankNode_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include <string>
#include <vector>
#include <map>
#include "Common/Common.h"
#include "ToolModule/UIToolHeader.h"
#include "Utils/ResourceUtils.h"
#include "PvpModule/PvpHeader.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

// 排行榜数据;
typedef struct tagRankingData
{
	// Id;
	int nPlayerId;

	// 排名序号;
	int nRankingIndex;

	// 玩家类型(0:AI 1:Player);
	int nPlayerType;

	// ResId;
	int nResId;

	// 玩家名称;
	string  strPlayerName;

	// 玩家等级;
	int  nLv;

	// 战斗力;
	int  nFc;

	// (试炼)历史最高层数;
	int  nMaxFloor;

	// 阵型;
	vector<matrixData>  vcFormation;

	// 是否显示排名图标;
	bool  bShowRankingIcon;

	//玩家vip等级
	int nVipLv;

	//玩家帮派名称
	string strGroupName;

	tagRankingData()
	{
		nPlayerId = 0;
		nRankingIndex = 0;
		nPlayerType = 0;
		strPlayerName.clear();
		nLv = 0;
		nVipLv = 0;
		strGroupName = "";
		bShowRankingIcon = false;
	}
}RankingData;



// 排行榜UI节点;
typedef struct tagUI_Rank_Node
{
	// 排名图标(可能为空，前*名才有);
	ImageView*  imgRanking;
	Text*		txtRanking;

	// 头像;
	UIToolHeader*  imgPlayer;

	// 玩家名;
	Text* txtPlayerName;

	// 玩家最高层;
	Text* txtPlayerMaxFloor;

	// 分割线;
	ImageView* imgLine;

	tagUI_Rank_Node()
	{
		memset(this, 0, sizeof(tagUI_Rank_Node));
	}
}UI_Rank_Node;

class RankNode : public Node
{
public:
	~RankNode(void);

	static RankNode* create(Layer* pLogicParent, RankingData* pData);

	virtual bool  init(Layer* pLogicParent, RankingData* pData);

	// 按钮响应;
	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	// 点击玩家查看详情;
	void  onPlayerClicked(Ref* ref, Widget::TouchEventType type, int nIndex);

private:
	RankNode(void);

	void  initUI(RankingData* pData);

private:

	UI_Rank_Node  m_ui;

	// 逻辑父节点，一般是排行榜页面;
	Layer*  m_pLogicParent;

	enum UI_RANK_NODE_WIDGET
	{
		Btn_Gift
	};
};

#endif // RankNode_h__
