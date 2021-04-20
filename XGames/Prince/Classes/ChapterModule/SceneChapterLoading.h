/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		SceneChapterLoading
 *  Description:	章节场景资源异步加载的实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			02/11/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef SceneChapterLoading_h__
#define SceneChapterLoading_h__

#include "ChapterDef.h"

class SceneChapterLoading : public Scene
{
public:
	~SceneChapterLoading(void);

	static SceneChapterLoading*  create(int nChapterId = INVALID_CHAPTER_OR_BARRIER_ID,
										int nBarrierId = INVALID_CHAPTER_OR_BARRIER_ID,
										bool chapterBackFlag = false,
										bool barrierBackFlag = false);

	virtual bool init(int nChapterId = INVALID_CHAPTER_OR_BARRIER_ID,
					int nBarrierId = INVALID_CHAPTER_OR_BARRIER_ID,
					bool chapterBackFlag = false,
					bool barrierBackFlag = false);

	virtual void onEnter();

private:
	SceneChapterLoading(void);

	void startAsyncLoading();

	void onAsyncLoadedCallBack();

private:
	int	 m_nChapterId;
	int	 m_nBarrierId;

	bool mChapterBackFlag;
	bool mBarrierBackFlag;

};

#endif // SceneChapterLoading_h__
