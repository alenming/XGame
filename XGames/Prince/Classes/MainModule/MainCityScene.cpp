#include "MainCityScene.h"
#include "Widget/SpineAnimation.h"
#include "Utils/ResourceUtils.h"
#include "GuideCenter/GuideLayer.h"
#include "GuideCenter/GuideManager.h"
#include "Utils/StringFormat.h"
#include "ChapterModule/ChapterController.h"
#include "GuideCenter/GuideLayer.h"



MainCityScene::MainCityScene()
	: mPerspectLayer(nullptr)
	, mMainInfoLayer(nullptr)
{

}

MainCityScene::~MainCityScene()
{

}

bool MainCityScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	mPerspectLayer = MainBuildingLayer::create();
	this->addChild(mPerspectLayer);

	mMainInfoLayer = MainInfoLayer::create();
	this->addChild(mMainInfoLayer);

	return true;
}

void MainCityScene::onEnter()
{
	Scene::onEnter();

	// 音乐;
	SoundUtils::playSoundMusic("main_1");

	//先跳转到记录的位置
	mPerspectLayer->jumpToCurrent();

	//滚动到到百分比
	auto scrollCallBack = []()->void{ GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_MAIN); };
	GuideNode* guideNode = GuideManager::getInstance()->isGuideExecutable(GUIDE_INTERFACE_MAIN);
	if (guideNode != nullptr)
	{
		switch (guideNode->mScrollType)
		{
		case GUIDE_NODE_SCROLL_LEFT:
			mPerspectLayer->scrollToPercent(0.0f, scrollCallBack);
			break;
		case GUIDE_NODE_SCROLL_RIGHT:
			mPerspectLayer->scrollToPercent(1.0f, scrollCallBack);
			break;
		case GUIDE_NODE_SCROLL_MIDDLE:
			mPerspectLayer->scrollToPercent(MainBuildingLayer::getInitialPercent(), scrollCallBack);
			break;
		default:
			scrollCallBack();
			break;
		}
	}

	// 关卡本地数据初始化;
	ChapterController::getInstance()->initLocalData();

	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	//log(Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}

void MainCityScene::onExit()
{
	ArmatureDataManager::destroyInstance();
	cocostudio::GUIReader::destroyInstance();
	DictionaryHelper::destroyInstance();
	Scene::onExit();
}
