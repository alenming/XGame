#include "MainBuildingLayer.h"
#include "CocoStudio.h"
#include "CCRippleSprite.h"
#include "Common/Common.h"
#include "ChapterModule/SceneChapterLoading.h"
#include "TakeCardModule/SceneTakeCard.h"
#include "TowerModule/SceneTower.h"
#include "MazeModule/SceneMaze.h"
#include "LoveModule/LoveScene.h"
#include "GuideCenter/GuideManager.h"
#include "NewStarsCampModule/StarsCampScene.h"
#include "StarsSuModule/StarsSuScene.h"
#include "PvpModule/PvpScene.h"
#include "RefineryModule/RefineryScene.h"
#include "GroupModule/MyGroupScene.h"
#include "GroupModule/JoinGroupScene.h"
#include "TakeCardModule/TakeCardModel.h"
#include "ChapterModule/SceneChapter.h"
#include "NewShopModule/ShopScene.h"
#include "BossModule/BossScene.h"
#include "ChapterModule/Activity/SceneActivity.h"
#include "TakeCardModule/TakeCardController.h"
#include "BossModule/BossActStateListener.h"
#include "NewShopModule/ShopModel.h"
#include "AllPvpModule/AllPvpScene.h"

using namespace cocostudio;

float MainBuildingLayer::mScrollPercent = 0.0f;
bool MainBuildingLayer::mIsInitScrollPercent = false;

const int BUILDING_MIN_SKEW_ANGLE = -2;
const int BUILDING_MAX_SKEW_ANGLE = 3;
const int SUN_SHINE_MIN_ANGLE = -23;
const int SUN_SHINE_MAX_ANGLE = -16;
const float BUILDING_CLICK_SCALE = 0.98f;

MainBuildingLayer::MainBuildingLayer()
	: mTouchBuilding(nullptr)
	, mSunShineSprite(nullptr)
{
	//初始化滚动比例
	if (!mIsInitScrollPercent)
	{
		setInitialPercent();
		mIsInitScrollPercent = true;
	}
	
	TakeCardModel::getInstance()->addObserver(this);
	ToolModel::getInstance()->addObserver(this);
	MainModel::getInstance()->addObserver(this);
	ShopModel::getInstance()->addObserver(this);
}

MainBuildingLayer::~MainBuildingLayer()
{
	TakeCardModel::getInstance()->removeObserver(this);
	ToolModel::getInstance()->removeObserver(this);
	MainModel::getInstance()->removeObserver(this);
	ShopModel::getInstance()->removeObserver(this);
}

void MainBuildingLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	if (observerParam->self == TakeCardModel::getInstance() || observerParam->self == ToolModel::getInstance())//抽卡红点显示
	{
		if (observerParam->id == TakeCardNotify_BuildIng)
		{
			bool flag = TakeCardModel::getInstance()->gethaveFreeTakeCard() || 
				TakeCardModel::getInstance()->gethaveTakeCardOrder();
			Node* sealRedFlag = getBuildingRetFlag((int)MAIN_TAG::TAG_SEAL);
			if (sealRedFlag)
			{
				sealRedFlag->setVisible(flag);
			}
		}
	}

	switch (observerParam->id)
	{
	
	case nMAIN_CMD_BOSS_ACT_STATE:
		{
			Node* redFlag = getBuildingRetFlag((int)MAIN_TAG::TAG_BOSS);
			if(redFlag != nullptr)
			{
				redFlag->setVisible(MainModel::getInstance()->mBossActOpenState);
			}
		}
		break;
	case HAVE_KEY_COUTN:
	case FREECHOUTIME:
		{
			Node* ShopRedFlag = getBuildingRetFlag((int)MAIN_TAG::TAG_SHOP);

			if(ShopRedFlag != nullptr)
			{
				ShopRedFlag->setVisible(ShopModel::getInstance()->getIsShowFlag());
			}
		}
		break;
	case nMAIN_CMD_TASK_RECEIVE:
		{
			//任务领取经验后刷新标签开放状态
			setTileVisible();
		}
		break;
	default:
		break;
	}
}

bool MainBuildingLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	mUI.root = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UInewmain/UInewmain.ExportJson"));
	mUI.root->setSize(size);
	mUI.root->setPosition(pos);
	this->addChild(mUI.root);

	//第一层响应滚屏事件，并将滚屏事件向下逐层传递
	mUI.firstFloor = dynamic_cast<ScrollView*>(mUI.root->getChildByName("Floor_1"));
	mUI.firstFloor->addEventListener(CC_CALLBACK_2(MainBuildingLayer::onScrollViewEvent, this));
	mUI.firstFloor->setMovePercent(0.4f); //减小滑动时的移动距离

	//背景层
	auto waterFloor = dynamic_cast<ScrollView*>(mUI.root->getChildByName("Floor_7"));
	Sprite* water = Sprite::create("Image/UInewmain/floor.png");
	water->setAnchorPoint(Point(0, 0));
	waterFloor->addChild(water);


	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_zhuchengjiemian_DI/Ani_zhuchengjiemian_DI.ExportJson");
	Armature* ani = Armature::create("Ani_zhuchengjiemian_DI");
	ani->getAnimation()->play("Animation1");
	waterFloor->addChild(ani);

	//阳光
	mSunShineSprite = Sprite::create("Image/UInewmain/JingTouGuang.png");
	mSunShineSprite->setScale(10.0f);
	BlendFunc blendFunc = {GL_ONE, GL_ONE};
	mSunShineSprite->setBlendFunc(blendFunc);
	mSunShineSprite->setAnchorPoint(Point::ANCHOR_MIDDLE);
	mSunShineSprite->setPosition((Vec2)(mUI.firstFloor->getInnerContainerSize()));
	mSunShineSprite->setRotation(SUN_SHINE_MIN_ANGLE);
	mSunShineSprite->setOpacity(0);
	mUI.firstFloor->getInnerContainer()->addChild(mSunShineSprite, 999);

	//统计层数
	for (int i = 1; i <= FLOOR_COUNT; i++)
	{
		ScrollView* floor = dynamic_cast<ScrollView*>(mUI.root->getChildByName("Floor_" + TO_STR(i)));
		floor->setContentSize(size);
		floor->setClippingType(Layout::ClippingType::SCISSOR);
		mUI.vecFloor.push_back(floor);
	}

	//统计建筑物
	for (int i = 1; i <= BUILDING_COUNT; i++)
	{
		UIBuilding uiBuilding;
		uiBuilding.buildingName = "b" + TO_STR(i);
		uiBuilding.buildingEntity = dynamic_cast<ImageView*>(Helper::seekWidgetByName(mUI.root, uiBuilding.buildingName));
		uiBuilding.buildingTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(mUI.root, uiBuilding.buildingName + "_title"));
		uiBuilding.buildingId = i;
		mUI.vecBuilding.push_back(uiBuilding);


		string jsonPath = "Image/Ani_zhuchengjiemian_DH/" + uiBuilding.buildingName + "/" + uiBuilding.buildingName + ".ExportJson";
		if (FileUtils::getInstance()->isFileExist(jsonPath))
		{
			ArmatureDataManager::getInstance()->addArmatureFileInfo(jsonPath);
			Armature* ani = Armature::create(uiBuilding.buildingName);
			ani->getAnimation()->play(uiBuilding.buildingName);
			uiBuilding.buildingEntity->addChild(ani);
		}
	}

	initTouchEvent();

	//拉取商城数据
	if (!TakeCardModel::getInstance()->getloaded())
	{
		TakeCardController::getInstance()->onGetCardInfo();
	}

	bool flag = TakeCardModel::getInstance()->gethaveFreeTakeCard() || 
		TakeCardModel::getInstance()->gethaveTakeCardOrder();
	Node* sealRedFlag = getBuildingRetFlag((int)MAIN_TAG::TAG_SEAL);
	if (sealRedFlag)
	{
		sealRedFlag->setVisible(flag);
	}

	//boss开启状态
	Node* redFlag = getBuildingRetFlag((int)MAIN_TAG::TAG_BOSS);
	if(redFlag != nullptr)
	{
		redFlag->setVisible(MainModel::getInstance()->mBossActOpenState != 0);
	}
	//商城有免费抽或者有钥匙的时候
	ShopController::getInstance()->handleFirstShopLayerInfo();
	Node* ShopRedFlag = getBuildingRetFlag((int)MAIN_TAG::TAG_SHOP);

	if(ShopRedFlag != nullptr)
	{
		ShopRedFlag->setVisible(ShopModel::getInstance()->getIsShowFlag());
	}

	setTileVisible();
	return true;
}

void MainBuildingLayer::onEnter()
{
	Layer::onEnter();

	//下一帧
	auto callBack = [this](float dt)->void
	{
		setTileVisible();
		this->getScheduler()->unschedule("MainBuildingLayer::setTitleVisible", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "MainBuildingLayer::setTitleVisible");
}

void MainBuildingLayer::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	if (pSender == mUI.firstFloor)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Size size = mUI.firstFloor->getInnerContainerSize();
				Vec2 vec = mUI.firstFloor->getInnerContainer()->getPosition();
				float scrollPercent = -vec.x / (size.width - Director::getInstance()->getVisibleSize().width);
				jumpToPercent(scrollPercent);
			}
			break;

		default:
			break;
		}
	}
}

void MainBuildingLayer::jumpToCurrent()
{
	jumpToPercent(mScrollPercent);
}

void MainBuildingLayer::jumpToPercent(float percent)
{
	mScrollPercent = percent;

	//通知所有层发生滚动事件
	for (size_t i = 0; i < mUI.vecFloor.size(); i++)
	{
		ScrollView* floor = mUI.vecFloor.at(i);
		floor->jumpToPercentHorizontal(percent*100.0f);
	}

	//通知所有建筑物发生倾斜
	for (size_t i = 0; i < mUI.vecBuilding.size(); i++)
	{
		ImageView* buildingEntity = mUI.vecBuilding.at(i).buildingEntity;
		if (buildingEntity)
		{
			float skewAngle = BUILDING_MAX_SKEW_ANGLE - (BUILDING_MAX_SKEW_ANGLE - BUILDING_MIN_SKEW_ANGLE) * percent;
			buildingEntity->setSkewY(skewAngle);
		}
	}

	//调整阳光角度以及透明度
	if (percent > 0.5f)
	{
		float halfPercent = (percent - 0.5) / 0.5f;
		mSunShineSprite->setRotation(SUN_SHINE_MIN_ANGLE + (SUN_SHINE_MAX_ANGLE - SUN_SHINE_MIN_ANGLE) * halfPercent);
		mSunShineSprite->setOpacity(255 * halfPercent);
	}
	else
	{
		mSunShineSprite->setRotation(SUN_SHINE_MIN_ANGLE);
		mSunShineSprite->setOpacity(0);
	}
}

void MainBuildingLayer::scrollToPercent(float percent, const MainCityScrollCallBack& callback)
{

	//判定是否可直接定位，防止闪烁
	const float EPSINON = 0.00001;
	float dValue = mScrollPercent - percent;
	if((dValue >= -EPSINON) && (dValue <= EPSINON))
	{
		jumpToPercent(mScrollPercent);
		if (callback != nullptr)
		{
			callback();
		}
		return;
	}

	//判断是向左滚动还是像右滚动
	bool isRight = percent > mScrollPercent;

	//滚动速度
	float speed = 1.0f / 0.4f;

	auto updateFunc = [this, isRight, percent, speed, callback](float dt)->void
	{
		if (isRight)
		{
			mScrollPercent += (dt * speed);
			if (mScrollPercent > percent)
			{
				//滚动结束
				mScrollPercent = percent;
				getScheduler()->unschedule("MainBuildingLayer::scrollToPercent", this);
				if (callback != nullptr)
				{
					callback();
				}
			}
		}
		else
		{
			mScrollPercent -= (dt * speed);
			if (mScrollPercent < percent)
			{
				//滚动结束
				mScrollPercent = percent;
				getScheduler()->unschedule("MainBuildingLayer::scrollToPercent", this);
				if (callback != nullptr)
				{
					callback();
				}
			}
		}
		jumpToPercent(mScrollPercent);
	};

	getScheduler()->schedule(updateFunc, this, 0, false, "MainBuildingLayer::scrollToPercent");
}

void MainBuildingLayer::showSelectedEffect(UIBuilding* building)
{
	//放大效果
	if (building && building->buildingEntity)
	{
		building->buildingEntity->setScale(BUILDING_CLICK_SCALE);
	}

// 	//外发光效果
// 	if (building->buildingEntity)
// 	{
// 		auto fileUtiles = FileUtils::getInstance();
// 		auto fragmentFullPath = fileUtiles->fullPathForFilename("Shader/example_outline.fsh");
// 		auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
// 		auto glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());
// 		auto glprogramstate = GLProgramState::getOrCreateWithGLProgram(glprogram);
// 		Vec3 color(0.2f, 0.2f, 0.2f);
// 		GLfloat radius = 0.002f;
// 		GLfloat threshold = 3.0f;
// 		glprogramstate->setUniformVec3("u_outlineColor", color);
// 		glprogramstate->setUniformFloat("u_radius", radius);
// 		glprogramstate->setUniformFloat("u_threshold", threshold);
// 		building->buildingEntity->getVirtualRenderer()->setGLProgramState(glprogramstate);
// 	}
}

//隐藏建筑物选中效果
void MainBuildingLayer::removeSelectedEffect(UIBuilding* building)
{
	//缩放效果
	if (mTouchBuilding && mTouchBuilding->buildingEntity)
	{
		mTouchBuilding->buildingEntity->setScale(1.0f);
	}

// 	//取消外发光效果
// 	if (mTouchBuilding && mTouchBuilding->buildingEntity)
// 	{
// 		auto glprogram2 = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, ccPositionTextureColor_noMVP_frag);
// 		mTouchBuilding->buildingEntity->getVirtualRenderer()->setGLProgram(glprogram2);
// 	}
}

void MainBuildingLayer::initTouchEvent()
{
	//事件代理层，由该层触发点击事件响应
	Node* delegateLayer = Node::create();
	delegateLayer->setAnchorPoint(Point(0, 0));
	delegateLayer->setContentSize(Director::getInstance()->getVisibleSize());
	this->addChild(delegateLayer, 999);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(MainBuildingLayer::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(MainBuildingLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(MainBuildingLayer::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(MainBuildingLayer::onTouchCancelled, this);

	delegateLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, delegateLayer);
}

bool MainBuildingLayer::onTouchBegan(Touch *touch, Event* event)
{
	for (size_t i = mUI.vecBuilding.size() - 1; i > 0; i--)
	{
		UIBuilding* building = &mUI.vecBuilding.at(i);
		ImageView* buildingEntity = building->buildingEntity;
		if (buildingEntity)
		{
			Rect rect = Rect(0, 0, buildingEntity->getContentSize().width, buildingEntity->getContentSize().height);
			if (rect.containsPoint(buildingEntity->convertTouchToNodeSpace(touch)) && 
				building->buildingTitle &&  
				alphaTouchCheck(buildingEntity, touch))
			{
				showSelectedEffect(building);
				mTouchBuilding = building;
				return true;
			}
		}
	}
	return false;
}

void MainBuildingLayer::onTouchMoved(Touch *touch, Event* event)
{
	//滑动距离大于阈值，则取消点击
	if (touch->getStartLocation().distance(touch->getLocation()) > 50)
	{
		if (mTouchBuilding)
		{
			removeSelectedEffect(mTouchBuilding);
			mTouchBuilding = nullptr;
		}
	}
}

void MainBuildingLayer::onTouchEnded(Touch* touch, Event* event)
{
	if (mTouchBuilding)
	{
		SoundUtils::playSoundEffect("dianji");

		//隔一帧再执行点击事件，防止重复创建
		auto callBack = [this](float dt)->void
		{
			removeSelectedEffect(mTouchBuilding);
			onBuildingClick(mTouchBuilding->buildingId);
			mTouchBuilding = nullptr;

			this->getScheduler()->unschedule("MainBuildingLayer::touchEnd", this);
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "MainBuildingLayer::touchEnd");
	}
}

void MainBuildingLayer::onTouchCancelled(Touch* touch, Event* event)
{
	if (mTouchBuilding)
	{
		//隔一帧再执行点击事件，防止重复创建
		auto callBack = [this](float dt)->void
		{
			removeSelectedEffect(mTouchBuilding);
			mTouchBuilding = nullptr;

			this->getScheduler()->unschedule("MainBuildingLayer::touchCancel", this);
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "MainBuildingLayer::touchCancel");
	}
}

bool MainBuildingLayer::alphaTouchCheck(ImageView* image, Touch *touch)
{
	//先从ImageView中提取Sprite
	Sprite* selectSprite = static_cast<Sprite*>(image->getVirtualRenderer());
	
	//将坐标转换为相对ImageView坐标
	Point pt = image->convertTouchToNodeSpace(touch);
	
	//复制Sprite
	CCSprite* tempSprite = CCSprite::createWithSpriteFrame(selectSprite->displayFrame());
	tempSprite->setAnchorPoint(Point::ZERO);
	tempSprite->setPosition(Point::ZERO);
	//tempSprite->setSkewY(image->getSkewY());
	Rect rect = tempSprite->getBoundingBox();
	unsigned int x = pt.x;
	unsigned int y = rect.size.height - pt.y;

	//初始化render
	RenderTexture* renderer = RenderTexture::create(rect.size.width, rect.size.height);
	
	//开始绘制
	renderer->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
	tempSprite->visit();
	renderer->end();

	Director::getInstance()->getRenderer()->render();

	//获取像素点
	Image* pImage = renderer->newImage();
	unsigned char* data_ = pImage->getData();
	int pa = 4 * ((pImage->getHeight() - (int)(pt.y) - 1) * pImage->getWidth() + (int)(pt.x)) + 3;  
	unsigned int ap = data_[pa];
	pImage->release();

	if (ap > 20)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void MainBuildingLayer::onBuildingClick(int buildingTagId)
{
	switch (buildingTagId)
	{
	case MAIN_TAG::TAG_PVP:
		{
			Director::getInstance()->replaceScene(AllPvpScene::create());
		}
		break;

	case MAIN_TAG::TAG_SEAL:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_SHOP))
		{
			Director::getInstance()->replaceScene(SceneTakeCard::create());
		}
		break;

	case MAIN_TAG::TAG_SHOP:
		{
			Director::getInstance()->replaceScene(ShopScene::createScene(ST_NONE, false));
		}
		break;

	case MAIN_TAG::TAG_MAZE:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_MAZE))
		{
			Director::getInstance()->replaceScene(SceneMaze::create());
		}
		break;

	case MAIN_TAG::TAG_DAILY_PVE:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE_EVENT))
		{
			Director::getInstance()->replaceScene(SceneActivity::create());
		}
		break;
	
	case MAIN_TAG::TAG_BOSS:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_BOSS))
		{
			Director::getInstance()->replaceScene(BossScene::create());
		}
		break;

	case MAIN_TAG::TAG_LOVE:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_LOVE))
		{
			Director::getInstance()->replaceScene(LoveScene::create());
		}
		break;

	case MAIN_TAG::TAG_FACTION:
		
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_FACTION))
		{
			if(MainModel::getInstance()->getMainParam()->mGroupId > 0) //已加入帮派
			{
				Director::getInstance()->replaceScene(MyGroupScene::create());
			}
			else
			{
				Director::getInstance()->replaceScene(JoinGroupScene::create());
			}
		}
		break;

	case MAIN_TAG::TAG_REFINE:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_REFINE))
		{
			Director::getInstance()->replaceScene(RefineryScene::create());
		}
		break;

	case MAIN_TAG::TAG_TOWER:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TOWER))
		{
			Director::getInstance()->replaceScene(SceneTower::create());
		}
		break;

	case MAIN_TAG::TAG_PVE:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE))
		{
			Director::getInstance()->replaceScene(SceneChapterLoading::create());
		}
		break;

	default:
		break;
	}
}

//跳转至某场景
void MainBuildingLayer::jumpToScene(int buildingTagId)
{
	onBuildingClick(buildingTagId);
}

float MainBuildingLayer::getInitialPercent()
{
	auto glview = Director::getInstance()->getOpenGLView();
	Size frameSize = glview->getFrameSize();
	float frameScale = frameSize.width / frameSize.height;
	
	if (frameScale < (3.0f / 2.0f))
	{
		//先区分辨率小于3:2,说明是pad的分辨率，以16:9的方式显示
		return 360.0f / (1907.0f - 1280.0f);
	}
	else
	{
		//大于3:2的情况，比例 = 固定值 / (w2 - w1)
		float rw = frameScale * 720.0f;
		return 360.0f / (1907.0f - rw);
	}

}

void MainBuildingLayer::setInitialPercent()
{
	mScrollPercent = getInitialPercent();
}

Node* MainBuildingLayer::getBuildingRetFlag( int buildingTagId )
{
	for(auto build : mUI.vecBuilding)
	{
		if(build.buildingId == buildingTagId)
		{
			if(build.buildingTitle != nullptr)
			{
				Node* retFlag = (Node*)build.buildingTitle->getChildByName("dian");
				return retFlag;
			}
		}
	}

	return nullptr;
}

void MainBuildingLayer::setTileVisible()
{
	for(auto build : mUI.vecBuilding)
	{
		if(build.buildingId == MAIN_TAG::TAG_MAZE)				//幻境
		{
			build.buildingTitle->setVisible(GuideManager::getInstance()->isFunctionOpen(FUNCTION_MAZE, false));
		}
		else if(build.buildingId == MAIN_TAG::TAG_SEAL)			//封印
		{
			build.buildingTitle->setVisible(GuideManager::getInstance()->isFunctionOpen(FUNCTION_SHOP, false));
		}
		else if(build.buildingId == MAIN_TAG::TAG_PVE)			//冒险
		{
			build.buildingTitle->setVisible(GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE, false));
		}
		else if(build.buildingId == MAIN_TAG::TAG_TOWER)		//试练塔
		{
			build.buildingTitle->setVisible(GuideManager::getInstance()->isFunctionOpen(FUNCTION_TOWER, false));
		}
		else if(build.buildingId == MAIN_TAG::TAG_DAILY_PVE)	//每日挑战
		{
			build.buildingTitle->setVisible(GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE_EVENT, false));
		}
		else if(build.buildingId == MAIN_TAG::TAG_REFINE)		//炼化炉
		{
			build.buildingTitle->setVisible(GuideManager::getInstance()->isFunctionOpen(FUNCTION_REFINE, false));
		}
		else if(build.buildingId == MAIN_TAG::TAG_FACTION)		//帮派
		{
			build.buildingTitle->setVisible(GuideManager::getInstance()->isFunctionOpen(FUNCTION_FACTION, false));
		}
		else if(build.buildingId == MAIN_TAG::TAG_LOVE)			//情缘
		{
			build.buildingTitle->setVisible(GuideManager::getInstance()->isFunctionOpen(FUNCTION_LOVE, false));
		}
		else if(build.buildingId == MAIN_TAG::TAG_SHOP)			//集市，表格没配，默认开放
		{
			build.buildingTitle->setVisible(true);
		}
		else if(build.buildingId == MAIN_TAG::TAG_BOSS)			//绝地反击
		{
			build.buildingTitle->setVisible(GuideManager::getInstance()->isFunctionOpen(FUNCTION_BOSS, false));
		}
		else if(build.buildingId == MAIN_TAG::TAG_PVP)			//竞技场
		{
			build.buildingTitle->setVisible(GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVP, false));
		}
	}
}


