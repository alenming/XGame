/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ChapterNode
 *  Description:	�����½ڰ�ť�ķ�װ;
 *  Version:		2.0
 *  Author:			Phil
 *  Date:			v1.0  09/25/2014
 *					v2.0  06/10/2015
 *  Others:			2.0����ȥ���½ڽ�������²���;
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

	// �߼����ڵ�;
	void  setLogicParent(Layer* pParent) { m_pLogicParent = pParent;};

	// ��ȡ�����ﱾ��ͼƬ;
	ImageView*  getBuilding() { return m_ui.imgChapter; };

	// ��ȡ����;
	ImageView*  getChapterTitle() { return m_ui.imgTitle; };

	// ��ť��Ӧ;
	//void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);
	void  onChapterClicked(bool bClicked);

	// ���ùؿ���ť״̬;
	void  setEnabled(bool bEnabled);
	bool  IsEnabled() { return m_bIsEnabled; };

	// ��������;
	void  updateStarCount(int nCur, int nTotal);

	// �����Ǽ�������ʾ״̬;
	void  updateStarReward(bool bValid);


private:

	void  initUI();

private:

	// UI�����;
	UI_ChapterNode		m_ui;

	// ���½�Id;
	int  m_nChapterId;

	// �߼����ڵ�;
	Layer*	m_pLogicParent;

	// �����������;
	//ImageView*  m_imgBuildingFrame;

	// �Ƿ����;
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
