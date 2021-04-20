#ifndef SceneChapter_h__
#define SceneChapter_h__

#include "ChapterDef.h"

class SceneChapter: public Scene
{
public:
	SceneChapter();
	~SceneChapter();

	static SceneChapter*  create(int nChapterId = INVALID_CHAPTER_OR_BARRIER_ID,
								int nBarrierId = INVALID_CHAPTER_OR_BARRIER_ID,
								bool chapterBackFlag = false,
								bool barrierBackFlag = false);

	//获取场景返回标记，用于标记材料来源场景跳转
	static bool getChapterBackFlag() { return mChapterBackFlag; }
	static bool getBarrierBackFlag() { return mBarrierBackFlag; }

	virtual bool  init(int nChapterId = INVALID_CHAPTER_OR_BARRIER_ID,
					int nBarrierId = INVALID_CHAPTER_OR_BARRIER_ID);

	virtual void onEnter();

	virtual void onExit();


private:

	void  initUI();

private:
	int  m_nDstChapterId;
	int  m_nDstBarrierId;

	//章节界面、关卡界面返回标记，用于道具来源的场景跳转
	static bool mChapterBackFlag;
	static bool mBarrierBackFlag;

};

#endif // SceneChapter_h__
