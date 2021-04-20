/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerChapter
 *  Description:	剧情章节的UI实现;
 *  Version:		1.0
 *  Author:			paddy
 *  Date:			08/20/2014
 *  Others:			V1.1 修改交互; phil 09/25/2014
 *
 ******************************************************************/
#ifndef LayerChapter_h__
#define LayerChapter_h__

#include "ChapterDef.h"
#include "Utils/Observer.h"
#include "ChapterNode.h"


// 索引定位章节时，目的位置;
enum CHAPTER_NODE_POS
{
	CHAPTER_NODE_POS_LEFT,			// 屏幕居左;
	CHAPTER_NODE_POS_MIDDLE			// 屏幕居中;
};


class LayerChapter : public Layer, public Observer
{
public:
	LayerChapter();
	~LayerChapter();

	CREATE_FUNC(LayerChapter);

	virtual bool  init();
	virtual void  onEnter();
	virtual void  onExit();

	virtual void  update(float delta);

	virtual void  setVisible(bool visible);

	// UI更新接口;
	virtual void  updateSelf(void* data);

	// 章节UI滚动事件;
	void  onScrollViewEvent(Ref* pSender, ScrollView::EventType type);
	void  onTouchScrollEvent(Ref* pSender, Widget::TouchEventType type);

	// (从章节节点中)选中章节;
	void  onChapterSelected(ChapterNode* chapterNode);
	void  onChapterSelected(int nChapterId);

	// 开启/关闭触摸事件代理;
	void  initTouchEvent();
	void  releaseTouchEvent();

	// 目录索引位置;
	void  switch2Chapter(int nChapterOrder, CHAPTER_NODE_POS dstPos = CHAPTER_NODE_POS_LEFT);

private:

	// 创建UI;
	void  initUI();

	// 创建场景地图"帧",包含一个完整的底图循环;
	void  createMapFrame();
	void  createMapLayer_1();
	void  createMapLayer_2();
	void  createMapLayer_3();
	void  createMapLayer_4();
	void  createMapLayer_5();
	void  createMapLayer_6();
	void  createMapLayer_7();

	void  initBuilding();

	// 更新章节进度;
	void  updateChapterState(UI_CHAPTER_STORY_PARAM* mapChapterInfo);

	// 联动;
	void  onChapterScroll(float offsetX_Percent, float offsetY_Percent = 0.0f);
	bool  alphaTouchCheck(ImageView* image, Touch *touch);
	bool  onTouchBegan(Touch *touch, Event* event);
	void  onTouchMoved(Touch *touch, Event* event);
	void  onTouchEnded(Touch* touch, Event* event);
	void  onTouchCancelled(Touch* touch, Event* event);


private:

	UI_Chapter			m_ui;

	float  fOriPosX;
	float  fOriTime;

	Node*  m_delegateLayer;

	Point  mTouchBeganPoint;

	ScrollView*		m_scrollviewSpeed;

	ChapterNode*	m_chapterSelected;

	bool  m_bFirstEnter;

	enum UI_CHAPTER_WIDGET
	{
		Root,
		Pnl_Map_5,
		ScrollView_5,

		// old
		ScrollView_Chapter_Story,
		Img_Cur
	};
};

#endif // LayerChapter_h__
