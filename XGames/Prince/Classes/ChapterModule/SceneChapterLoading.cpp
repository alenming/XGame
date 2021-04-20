#include "SceneChapterLoading.h"
#include "Widget/LayerLoading.h"
#include "SceneChapter.h"
#include "ResourceManager/ResourceLoader.h"


SceneChapterLoading::SceneChapterLoading(void)
	: m_nChapterId(INVALID_CHAPTER_OR_BARRIER_ID)
	, m_nBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
	, mChapterBackFlag(false)
	, mBarrierBackFlag(false)
{
}


SceneChapterLoading::~SceneChapterLoading(void)
{
}

SceneChapterLoading* SceneChapterLoading::create( int nChapterId /*= INVALID_CHAPTER_OR_BARRIER_ID*/,
												int nBarrierId /*= INVALID_CHAPTER_OR_BARRIER_ID*/ ,
												bool chapterBackFlag /*= false*/,
												bool barrierBackFlag /*= false*/)
{
	SceneChapterLoading* _scene = new SceneChapterLoading;
	if (nullptr != _scene && _scene->init(nChapterId, nBarrierId, chapterBackFlag, barrierBackFlag))
	{
		_scene->autorelease();
		return _scene;
	}

	CC_SAFE_DELETE(_scene);
	return nullptr;
}

bool SceneChapterLoading::init( int nChapterId /*= INVALID_CHAPTER_OR_BARRIER_ID*/,
								int nBarrierId /*= INVALID_CHAPTER_OR_BARRIER_ID*/ ,
								bool chapterBackFlag /*= false*/,
								bool barrierBackFlag /*= false*/)
{
	if (!Scene::init())
	{
		return false;
	}

	m_nChapterId = nChapterId;
	m_nBarrierId = nBarrierId;
	mChapterBackFlag = chapterBackFlag;
	mBarrierBackFlag = barrierBackFlag;

	// Loading;
	LayerLoading* _layer = LayerLoading::create();
	if (nullptr != _layer)
	{
		this->addChild(_layer);
	}

	return true;
}

void SceneChapterLoading::onEnter()
{
	Scene::onEnter();

	// 清理cache并开始异步加载;
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	//log(Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
	startAsyncLoading();
}

void SceneChapterLoading::startAsyncLoading()
{
	/*
	ostringstream oss;
	for (size_t i = 0; i < 7; ++i)
	{
		for (size_t k = 0; k < 4; ++k)
		{
			CC_BREAK_IF(i >= 6 && k >=2);
			oss.str("");
			oss << "Image/UIChapter/Bg/Map" << i+1 << "_" << k+1 << ".png";
			ResourceLoader::getInstance()->addAsyncImageRes(oss.str());
		}
	}

	ResourceLoader::getInstance()->addAsyncImageRes("Image/UIChapter/Bg/light.png");
	ResourceLoader::getInstance()->addAsyncImageRes("Image/UIChapter/Bg/yun.png");
	ResourceLoader::getInstance()->addAsyncImageRes("Image/UIChapter/Bg/tiankong.png");
	*/
	ResourceLoader::getInstance()->addAsyncUIRes("UIChapter");
	ResourceLoader::getInstance()->addAsyncUIRes("UIChapter/Bg");
	ResourceLoader::getInstance()->addAsyncUIRes("UIBarrier");

	// 启动异步加载;
	ResourceLoader::getInstance()->startAsyncLoading(CC_CALLBACK_0(SceneChapterLoading::onAsyncLoadedCallBack, this));
}

void SceneChapterLoading::onAsyncLoadedCallBack()
{
	// 正式进入章节场景;
	SceneChapter* _scene = SceneChapter::create(
		m_nChapterId, m_nBarrierId, 
		mChapterBackFlag, mBarrierBackFlag);
	if (nullptr != _scene)
	{
		Director::getInstance()->replaceScene(_scene);
	}
}
