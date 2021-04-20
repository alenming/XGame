/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerFrame
 *  Description:	剧情/精英/活动章节共用部分的UI实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/20/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerFrame_h__
#define LayerFrame_h__

#include "ChapterDef.h"

class LayerFrame : public Layer
{
public:
	LayerFrame(void);
	~LayerFrame(void);

	CREATE_FUNC(LayerFrame);

	virtual	bool  init();

	void  onExit();

	// 切换内容层，按钮响应;
	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	// 前景层的显隐开关，由内容层调用;
	void  setFrameEnabled(bool  bEnabled);

	// 切换内容(章节/精英/活动)层;
	void  switchLayerChapter(CHAPTER_TYPE pageType, int nLocalZOrder = 1);

	// 目录索引;
	void  onChapterIndexBtnClicked(Ref* ref, Widget::TouchEventType type, int nIndex);

private:

	void  initUI();

	void  showLayerChapter(CHAPTER_TYPE pageType);


private:

	UI_Frame		m_ui;

	// 内容层(章节/精英/活动);
	map<CHAPTER_TYPE, Layer*>   m_mapLayerChapter;
	CHAPTER_TYPE				m_CurSelPage;

	enum UI_FRAME_WIDGET
	{
		Root,
		Pnl_Frame,
		Btn_Frame_Back,
		Btn_Experience,
		Btn_Chapter_Event,
		Lab_Event
	};
};

#endif // LayerFrame_h__