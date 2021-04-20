#include "SceneChapter.h"
#include "LayerFrame.h"
#include "ChapterController.h"
#include "Widget/LayerLoading.h"
#include "GuideCenter/GuideManager.h"

bool SceneChapter::mChapterBackFlag = false;
bool SceneChapter::mBarrierBackFlag = false;

SceneChapter::SceneChapter()
	: m_nDstChapterId(INVALID_CHAPTER_OR_BARRIER_ID)
	, m_nDstBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
{

}

SceneChapter::~SceneChapter()
{

}

void SceneChapter::onExit()
{
	Scene::onExit();

	// 释放资源;
	GUIReader::destroyInstance();
	ArmatureDataManager::destroyInstance();
	DictionaryHelper::getInstance()->destroyInstance();
	SpriteFrameCache::getInstance()->destroyInstance();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	//log(Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}

void SceneChapter::onEnter()
{
	Scene::onEnter();

	// 音乐;
	if (m_nDstChapterId != INVALID_CHAPTER_OR_BARRIER_ID
		|| m_nDstBarrierId != INVALID_CHAPTER_OR_BARRIER_ID)
	{
		SoundUtils::playSoundMusic("main_1");
	}

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_CHAPTER);
}


SceneChapter* SceneChapter::create(int nChapterId, int nBarrierId,
								   bool chapterBackFlag, bool barrierBackFlag)
{
	SceneChapter* _scene = new SceneChapter;
	if (nullptr != _scene && _scene->init(nChapterId, nBarrierId))
	{
		mChapterBackFlag = chapterBackFlag;
		mBarrierBackFlag = barrierBackFlag;
		_scene->autorelease();
		return _scene;
	}

	CC_SAFE_DELETE(_scene);
	return nullptr;
}


bool SceneChapter::init(int nChapterId, int nBarrierId)
{
	if (!Scene::init())
	{
		return false;
	}
	m_nDstChapterId = nChapterId;
	m_nDstBarrierId = nBarrierId;

	initUI();

	return true;
}


void SceneChapter::initUI()
{
	// 预设章节和关卡;
	if (m_nDstChapterId != INVALID_CHAPTER_OR_BARRIER_ID)
	{
		ChapterController::getInstance()->setDstChapter(m_nDstChapterId);
	}
	if (m_nDstBarrierId != INVALID_CHAPTER_OR_BARRIER_ID)
	{
		ChapterController::getInstance()->setDstBarrier(m_nDstBarrierId);
	}

	// 创建UI;
	auto pLayerFrame = LayerFrame::create();
	if (nullptr != pLayerFrame)
	{
		this->addChild(pLayerFrame);

		// 请求PVE数据;
		ChapterController::getInstance()->handlePveData();
	}
}
