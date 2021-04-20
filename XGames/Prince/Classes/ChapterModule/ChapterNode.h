/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ChapterNode
 *  Description:	剧情章节按钮的封装;
 *  Version:		2.0
 *  Author:			Phil
 *  Date:			v1.0  09/25/2014
 *					v2.0  06/10/2015
 *  Others:			2.0改造去掉章节建筑物，重新布局;
 *
 ******************************************************************/
#ifndef ChapterNode_h__
#define ChapterNode_h__

#include "ChapterDef.h"

class ChapterNode : public Node
{
public:
	ChapterNode(void);
	~ChapterNode(void);

	static ChapterNode* create(const int nChapterId);

	virtual bool  init(const int nChapterId);

	// 逻辑父节点;
	void  setLogicParent(Layer* pParent) { m_pLogicParent = pParent;};

	// 获取建筑物本体图片;
	ImageView*  getBuilding() { return m_ui.imgChapter; };

	// 获取标题;
	ImageView*  getChapterTitle() { return m_ui.imgTitle; };

	// 按钮响应;
	//void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);
	void  onChapterClicked(bool bClicked);

	// 设置关卡按钮状态;
	void  setEnabled(bool bEnabled);
	bool  IsEnabled() { return m_bIsEnabled; };

	// 更新星数;
	void  updateStarCount(int nCur, int nTotal);

	// 更新星级奖励提示状态;
	void  updateStarReward(bool bValid);


private:

	void  initUI();

private:

	// UI组件集;
	UI_ChapterNode		m_ui;

	// 本章节Id;
	int  m_nChapterId;

	// 逻辑父节点;
	Layer*	m_pLogicParent;

	// 建筑发光外观;
	//ImageView*  m_imgBuildingFrame;

	// 是否可用;
	bool  m_bIsEnabled;

	enum UI_CHAPTER_NODE_WIDGET
	{
		Img_Chapter,
		Img_Title,
		Img_Count_Bg,
		Img_Pass,
		AtlasLabel_Cur,
		AtlasLabel_Total
	};
};

#endif // ChapterNode_h__
