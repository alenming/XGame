#include "LayerMaze.h"

#include "CocoStudio.h"
#include "MazeModel.h"
#include "MazeController.h"
#include "Common/Common.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "ResourceManager/ResourceLoader.h"
#include "UI/UIDef.h"
#include "Widget/LayerLoading.h"
#include "../ToolModule/ToolModel.h"
#include "Temp/CustomPop.h"
#include "Temp/CustomTips.h"
#include "HeroModule/HeroModel.h"
#include "Utils/ResourceUtils.h"
#include "Utils/StringFormat.h"
#include "../MainModule/MainModel.h"
#include "../FightModule/SceneFightLoading.h"
#include "../FightModule/FightController.h"
#include "MainModule/MainCityScene.h"
#include "Temp/PopReward.h"
#include "Utils/StringFormat.h"
#include "Widget/SpineAnimation.h"
#include "ToolModule/UICardPrize.h"
#include "Utils/ResourceUtils.h"
#include "MarvellousActive/MarvellousActiveScene.h"
#include "Temp/CustomGoldTips.h"
#include "SDK_TalkingData/TalkingData.h"

using namespace cocos2d;
using namespace cocos2d::ui;

#define  TALK_NOTICE_COLOR Color3B(0xff,0xfc,0xec)
#define  TALK_TALK_COLOR Color3B(0xff,0xc0,0x46)
const cocos2d::Vec2 vec2LimitBuyItemPos = cocos2d::Vec2(100, 400);
const cocos2d::Vec2 _vec2LimitBuyItemPos = cocos2d::Vec2(160, 507);
const float OrbitCameraTime = 0.2f;

//逻辑事件：点主界面到岔路口， 点岔路到相应事件， 事件结束到主界面


LayerMaze::LayerMaze()
{	
	m_isLimitFlag = false;
	m_stepState = MAZE_STEP_ENTER;
	m_spendGoldCount = 0;
	m_isFilishedFlag	= true;
	m_isAnimitionRunning = false;
	m_isGetDataFlag = false;
	m_pMazeController = MazeController::getInstance();
	m_pMazeController->onSetView(this);
	MainModel::getInstance()->addObserver(this);
}

LayerMaze::~LayerMaze()
{
	m_pMazeController->onRemoveView();
	//delete m_pMazeController;
	m_pMazeController = nullptr;
	this->getScheduler()->unscheduleAllForTarget(this);
	MainModel::getInstance()->removeObserver(this);
	LocalTimer::getInstance()->removeTimeStampListener(this);
	CCLOG("LayerMaze destroy");
}

void LayerMaze::onEnter()
{
	Layer::onEnter();	
}

void LayerMaze::onExit()
{
	CCLOG("LayerMaze onExit");	
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIMaze");

	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(LayerMaze::callBackTalkOut), this);
	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(LayerMaze::callBackReturnFirst), this);
	Layer::onExit();
}

void LayerMaze::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	if (observerParam->self == MainModel::getInstance())
	{
		updateMazeInfo();
	}
}

LayerMaze* LayerMaze::create(bool pushScene)
{
	LayerMaze *pRet = new LayerMaze();
	if (pRet && pRet->init(pushScene))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool LayerMaze::init(bool pushScene)
{
	m_isPushScene = pushScene;

	if(!ModalLayer::init())
	{
		return false;
	}
	
	initUI();
	MAZE_INFOS& mazeInfos = MazeModel::getInstance()->getMazeInfos();
	int eventId = MazeModel::getInstance()->getEventId();
	if (mazeInfos.mazeId > 0 && eventId > 0)
	{	
		//再次进入直接再次触发此事件
		updateMazeInfo();
		updateRandomInfo( eventId);
	}
	else
	{
		m_pMazeController->onGetAllMazes();
		nextBtnAnimition(true);
	}
	
	showLimitBuyItem();
	return true;
}


void LayerMaze::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();	
	// UI处理
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIMaze/UIMaze.ExportJson"));
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	// 将UILayer层加入到当前的场景
	this->addChild(m_ui.pRoot);
	ImageView* pImgBg			= ImageView::create("Image/Bg/img_maze_bg.jpg");
	pImgBg->setPosition(cocos2d::Vec2(m_ui.pRoot->getContentSize().width/2, m_ui.pRoot->getContentSize().height/2));	
	m_ui.pRoot->addChild(pImgBg);
	//
	m_ui.pLayMain				= dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_main"));
	m_ui.pLayMain->setSize(size);
	
	//触发事件
	m_ui.pLayGo					= dynamic_cast<Layout*>(m_ui.pLayMain->getChildByName("Pnl_go"));
	m_ui.pLayGo->addTouchEventListener(CC_CALLBACK_2(LayerMaze::onMainClicked, this));
	
	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			//迷宫退出
			if (m_isFilishedFlag)
			{
				MazeModel::getInstance()->clearMazeInfo();
				MazeModel::getInstance()->clearMazEvent();
			}

			if (m_isPushScene)
			{
				Director::getInstance()->popScene();
			}
			else
			{
				Director::getInstance()->replaceScene(MainCityScene::create());
			}
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_DREAM_LAND);
	this->addChild(_layerCommHeadInfo);

	//行走抖动图片
	m_ui.pImgMainBackup			= dynamic_cast<ImageView*>(m_ui.pLayMain->getChildByName("Img_bg"));//ImageView::create("Image/Bg/img_maze_bg.jpg");
	m_ui.pImgMainBackup->loadTexture("Image/Bg/img_maze_bg.jpg");


	/////////////////////////////////对话事件///////////////////////////////////////////
	m_ui.pLayMainHero			= dynamic_cast<Layout*>(m_ui.pLayMain->getChildByName("Pnl_hero"));
	m_ui.pImgMainHero			= ImageView::create();
	m_ui.pImgMainHero->setPosition(cocos2d::Vec2(m_ui.pLayMainHero->getContentSize().width/2,
											m_ui.pLayMainHero->getContentSize().height*0.32));
	m_ui.pLayMainHero->addChild(m_ui.pImgMainHero);

	//对白
	m_ui.pLayMainTalk			= dynamic_cast<Layout*>(m_ui.pLayMain->getChildByName("Pnl_talk"));
	m_ui.pLayMainTalk->setSize(cocos2d::Size(size.width, m_ui.pLayMainTalk->getContentSize().height));
	ImageView* pImgTalkBg		= dynamic_cast<ImageView*>(m_ui.pLayMainTalk->getChildByName("Img_talkBg"));
	m_ui.pImgTalkNameBg			= dynamic_cast<ImageView*>(pImgTalkBg->getChildByName("Img_nameBg"));
	m_ui.pTextTalkName			= dynamic_cast<Text*>(m_ui.pImgTalkNameBg->getChildByName("Lab_name"));
	m_ui.pTextTalkContent		= dynamic_cast<Text*>(pImgTalkBg->getChildByName("Lab_talk"));
	m_ui.pBtnTalkNext			= dynamic_cast<Button*>(pImgTalkBg->getChildByName("Btn_next"));
	m_ui.pBtnTalkNext->setPosition(Vec2(size.width-77, m_ui.pBtnTalkNext->getPosition().y));
	m_ui.vecBtnTalkNext			= m_ui.pBtnTalkNext->getPosition();
	pImgTalkBg->addTouchEventListener(CC_CALLBACK_2(LayerMaze::onBtnTalkNext, this));
	m_ui.pBtnTalkNext->addTouchEventListener(CC_CALLBACK_2(LayerMaze::onBtnTalkNext, this));
	m_ui.pTextTalkName->setFontName(FONT_FZZHENGHEI);
	m_ui.pTextTalkContent->setFontName(FONT_FZZHENGHEI);
	for (int i = 1; i <= 10; i++)
	{
		ImageView* img = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pImgTalkBg,"Image_" + TO_STR(i)));
		img->setVisible(false);
		m_ui.imgAotuOpen.push_back(img);
	}
	

	/////////////////////////////////偶遇事件///////////////////////////////////////////
	m_ui.meet = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_fight"));
	m_ui.meet->setSize(Size(size.width, m_ui.meet->getContentSize().height));
	m_ui.meetChallenge = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.meet,"Btn_fight1"));
	m_ui.meetRound = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.meet,"Btn_fight2"));


	/////////////////////////////////Boss事件///////////////////////////////////////////
	m_ui.hengfu = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_hengfu"));
	m_ui.hengfu->setSize(Size(size.width, m_ui.hengfu->getContentSize().height));
	m_ui.hengfu->setVisible(false);
	m_ui.henfuBoss = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.hengfu,"Img_boss"));
	m_ui.henfuQiangDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.hengfu,"Img_qiangdi"));
	m_ui.henfuBaozang = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.hengfu,"Img_baozang"));


	/////////////////////////////////藏宝事件///////////////////////////////////////////
	Layout *Panel_45 = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pLayMain,"Panel_45"));
	m_ui.pLayMainMapChip = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pLayMain,"Pnl_mapchip"));
	Vec2 pLayMainMapChipPos = m_ui.pLayMainMapChip->getPosition();
	m_ui.pLayMainMapChip->setPosition(Vec2(Panel_45->getContentSize().width,pLayMainMapChipPos.y));

	auto moveCall = [this]()->void
	{
		GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_MAZE);
	};
	m_ui.pLayMainMapChip->runAction(CCSequence::create(CCEaseSineOut::create(CCMoveTo::create(0.5f,pLayMainMapChipPos)),
		CCCallFunc::create(moveCall),nullptr));

	m_ui.pLayGo->setSize(Size(size.width - m_ui.pLayMainMapChip->getContentSize().width,size.height));
	m_ui.pImgMapTips			= dynamic_cast<ImageView*>(m_ui.pLayMainMapChip->getChildByName("Img_Tips"));
	m_ui.pListMapchip			= dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.pLayMainMapChip,"ListView_22"));
	//宝箱
	m_ui.pLayBox				= dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_box"));
	m_ui.pLayBox->setSize(Size(size.width, m_ui.pLayBox->getContentSize().height));
	m_ui.pBtnBoxMain			= dynamic_cast<Button*>(m_ui.pLayBox->getChildByName("Btn_box"));

	m_ui.layLimitBuy = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("limitBuy"));
	//m_ui.layLimitBuy->setSize(Size(size.width, m_ui.layLimitBuy->getContentSize().height));
	m_ui.layLimitBuy->setVisible(true);
}


void LayerMaze::updateMazeInfo()
{	
	MAZE_INFOS& mazeInfos = MazeModel::getInstance()->getMazeInfos();
	m_nCurMazeId = mazeInfos.mazeId;
	RowData* pMazeData = DataManager::getInstance()->searchMazeBaseById(m_nCurMazeId);
	MainParam *mainPram = MainModel::getInstance()->getMainParam();

	startCountDown(mazeInfos.times);
	//藏宝图列表
	//右侧展示藏宝图收集进度，如果一张也没拥有，则显示遮罩
	m_ui.pLayMainMapChip->setVisible(true);
	m_ui.pImgMapTips->setVisible(false);
	m_ui.pListMapchip->removeAllItems();
	for (int i = 0; i < mazeInfos.vecMaps.size(); i++)
	{
		int id = mazeInfos.vecMaps.at(i).id;
		int num = mazeInfos.vecMaps.at(i).num;
		RowData* mazeBoxData =  DataManager::getInstance()->searchMazeBaotuById(id);
		int needNum = mazeBoxData->getIntData("num");
		int resId = mazeBoxData->getIntData("resId");
		string fileStr = "Image/Icon/maze/"+ TO_STR(resId) +".png";
		UIToolHeader *head = UIToolHeader::create(fileStr);
		head->setScale(0.88f);
		if (head != nullptr)
		{
			if (mazeInfos.vecMaps.at(i).num <= 0)
			{
				head->setMaskFlag(true,false);
			}
			else
			{
				head->setMaskFlag(false,false);
			}
			head->addTouchEventListener(CC_CALLBACK_2(LayerMaze::onMapListClicked, this, resId,i));
			head->setTouchEnabled(true);	

			head->setCurNumAndNeedNum(num, needNum);
			head->setCurNumAndNeedNumColor(Color3B(0xfe,0xe5,0xe5));
			m_ui.pListMapchip->pushBackCustomItem(head);
		}
	}
}
//
void LayerMaze::updateRandomInfo(const int& eventId)
{
	m_isFilishedFlag = false;
	m_nCurEventId = eventId;
	MazeModel::getInstance()->setCurEventId(m_nCurEventId);
	RowData * eventData = DataManager::getInstance()->searchMazeThingById(m_nCurEventId);
	int eventType = eventData->getIntData("type");

	setNotice(eventData->getStringData("say1"));

	if (eventType == MAZE_RANDOMROAD_TALK)//1-对话
	{		
		m_stepState = MAZE_STEP_TALK;
		talkEvent(eventId);
	}
	else if (eventType == MAZE_RANDOMROAD_MEET)//2-遭遇
	{	
		m_stepState = MAZE_STEP_MEET;
		hengFuAnimition(HENGFU_QIANGDI);
	}
	else if (eventType == MAZE_RANDOMROAD_BOSS)//3-boss
	{
		m_stepState = MAZE_STEP_BOSS;
		hengFuAnimition(HENGFU_BOSS);
	}
	else if (eventType == MAZE_RANDOMROAD_LIMIT_BUY) //限时购买
	{
		m_stepState = MAZE_STEP_LIMIT_BUY;
		m_isFilishedFlag = true;
		showLimitBuyItem();
		showLimitBuy();
	}
}
//限时购买
void LayerMaze::showLimitBuy()
{
	m_isLimitFlag = false;
	MAZE_INFOS& mazeInfos = MazeModel::getInstance()->getMazeInfos();
	if (mazeInfos.mMazeBuyTime > 0)
	{
		//
		LayerMazeLimitBuy *pLayer = LayerMazeLimitBuy::create();				
		m_ui.layLimitBuy->addChild(pLayer, 5, "layerMazeLimitBuy");
		Size size = Director::getInstance()->getWinSize();
		pLayer->setPosition(cocos2d::Vec2(0, 0));
		pLayer->setTouchEnabled(true);
		pLayer->setLimitBuyObserver(this);
		pLayer->updateInfo(mazeInfos.mMazedrawPetId, mazeInfos.mMazeBuyTime, mazeInfos.mMazeBuyPrice, _vec2LimitBuyItemPos,m_ui.layLimitBuy, mazeInfos.mMazeBuyCount);
	}
}

void LayerMaze::showLimitBuyItem()
{
	//迷宫限时购买信息
	MAZE_INFOS& mazeInfos = MazeModel::getInstance()->getMazeInfos();
	if (mazeInfos.mMazeBuyTime > 0)
	{
		long disTime = secondNow()-mazeInfos.mMazeBuyCurTime;
		if (mazeInfos.mMazeBuyTime-disTime > 0)
		{
			cocos2d::Node* pNode = m_ui.layLimitBuy->getChildByName("layerMazeLimit");
			LayerMazeLimitBuyItem *pLayer = nullptr;
			if (pNode == nullptr)
			{
				pLayer = LayerMazeLimitBuyItem::create();
				pLayer->setObserver(this);
				m_ui.layLimitBuy->addChild(pLayer, 1, "layerMazeLimit");
				pLayer->setPosition(vec2LimitBuyItemPos);
				pLayer->setTouchEnabled(true);
				//pLayer->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnMazeLimitClick, this));
			}
			else
			{
				pLayer = static_cast<LayerMazeLimitBuyItem*>(pNode);
			}
			//
			pLayer->updateInfo(mazeInfos.mMazedrawPetId, mazeInfos.mMazeBuyTime-disTime,mazeInfos.mMazeBuyCount);
		}			
	}
}

void LayerMaze::onMazeLimitClicked()
{
	MAZE_INFOS& mazeInfos = MazeModel::getInstance()->getMazeInfos();
	cocos2d::Node* pNode = m_ui.layLimitBuy->getChildByName("layerMazeLimitBuy");
	LayerMazeLimitBuy *pLayer = nullptr;
	if (pNode == nullptr)
	{
		pLayer = LayerMazeLimitBuy::create();				
		m_ui.layLimitBuy->addChild(pLayer, 5, "layerMazeLimitBuy");
		Size size = Director::getInstance()->getVisibleSize();
		//pLayer->setPosition(cocos2d::Vec2(0, 0));
		pLayer->setTouchEnabled(true);
		pLayer->setLimitBuyObserver(this);
		//pLayer->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnMazeLimitClick, this));
	}
	else
	{
		pLayer = static_cast<LayerMazeLimitBuy*>(pNode);
	}
	//
	cocos2d::Node* pNodeItem = m_ui.layLimitBuy->getChildByName("layerMazeLimit");		
	if (pNodeItem != nullptr)
	{
		LayerMazeLimitBuyItem *pLayerItem = static_cast<LayerMazeLimitBuyItem*>(pNodeItem);
		pLayer->updateInfo(mazeInfos.mMazedrawPetId, pLayerItem->getLastCountDown(), mazeInfos.mMazeBuyPrice, pLayerItem->getHeroPostion(),m_ui.layLimitBuy, mazeInfos.mMazeBuyCount);
	}
}

void LayerMaze::onMazeLimitBuyEnd()
{
	updateMazeInfo();
	m_ui.layLimitBuy->removeChildByName("layerMazeLimit");
	if (m_isFilishedFlag)
	{
		gotoFirst();
	}
}


void LayerMaze::onMazeLimitBuyWait()
{
	if (m_isFilishedFlag)
	{
		gotoFirst();
	}
}

void LayerMaze::onGetAllMazesResult(const int& nResult)
{
	m_isGetDataFlag = false;
	if (nResult == 1)
	{
		MAZE_INFOS& mazeInfos = MazeModel::getInstance()->getMazeInfos();
		//如果翻牌没有翻完可以继续翻
		if (mazeInfos.vecChoosedMap.size() > 0)
		{
			updateMazeInfo();	
			onOpenUnCompleteBoxResult(mazeInfos.openBoxId);
			setNotice(DataManager::getInstance()->searchCommonTexdtById(20266));
		}
		else
		{
			//1、左右移动镜头，播放对白：这里便是幻境么？
			if (m_stepState <= MAZE_STEP_NONE)
			{
				setNotice(DataManager::getInstance()->searchCommonTexdtById(20267));
				updateMazeInfo();	
			}
			else
			{
				MAZE_INFOS& mazeInfos = MazeModel::getInstance()->getMazeInfos();
				m_nCurMazeId = mazeInfos.mazeId;
				RowData* pMazeData = DataManager::getInstance()->searchMazeBaseById(m_nCurMazeId);
				MainParam *mainPram = MainModel::getInstance()->getMainParam();

				startCountDown(mazeInfos.times);
			}
		}
		

		showLimitBuyItem();
	}
}
	
void LayerMaze::onRandomRoadResult(const int& nResult)
{
	m_isGetDataFlag = false;
	m_ui.pLayGo->setVisible(false);
	//1-对话 //2-遭遇  //3-Boss  //4-限时购买 //5-宝藏
	if (nResult == 1)
	{
		RowData* pMazeData = DataManager::getInstance()->searchMazeBaseById(m_nCurMazeId);
		MAZE_INFOS& mazeInfos = MazeModel::getInstance()->getMazeInfos();
		MainParam *mainPram = MainModel::getInstance()->getMainParam();

		startCountDown(mazeInfos.times);
		
		//触发时间之前会遮罩屏幕，播放事件标签动画（对话，遭遇，Boss，限时购买等）
		int eventId = MazeModel::getInstance()->getEventId();
		updateRandomInfo(eventId);		
	}
}


void LayerMaze::onOpenUnCompleteBoxResult(const int& boxId)
{
	m_BoxId = boxId;
	addChooseCardPanel();

	RowData*pData = DataManager::getInstance()->searchMazeBaotuById(m_BoxId);

	MAZE_INFOS& mazeInfos = MazeModel::getInstance()->getMazeInfos();
	setChooseCardFreeCount(mazeInfos.vecChoosedMap.size());

	for (int i = 0; i < 10; i++)
	{
		ImageView *imgBack = m_ui.cardVecBack[i];
		ImageView *imgFront = m_ui.cardVecFront[i];
		imgBack->setVisible(true);
		imgFront->setVisible(false);
		imgFront->stopAllActions();
		imgBack->stopAllActions();
	}

	for (int i = 0; i < mazeInfos.vecChoosedMap.size(); i++)
	{
		CHOOSEED_ITEM info = mazeInfos.vecChoosedMap[i];
		ImageView *imgBack = m_ui.cardVecBack[info.pos];
		ImageView *imgFront = m_ui.cardVecFront[info.pos];
		imgBack->setVisible(false);
		imgFront->setVisible(true);
		imgFront->stopAllActions();
		imgBack->stopAllActions();

		RowData* rowData = DataManager::getInstance()->searchToolById(info.id);
		UICardPrize *cardPrize = UICardPrize::create(info.id);
		imgFront->addChild(cardPrize);
		cardPrize->setCardCount(info.count);
		cardPrize->setCardName(rowData->getStringData("itemName"));
		m_isOrbitCamera = false;
	}
}


void LayerMaze::onOpenBoxResult(const int& boxId)
{
	addChooseCardPanel();
	m_BoxId = boxId;
	setChooseCardFreeCount(0);

	vector<string> boxItem;
	RowData *data = DataManager::getInstance()->searchMazeBoxById(boxId);
	StringFormat::parseCsvStringVec(data->getStringData("boxItem"), boxItem);

	for (int i = 0; i < boxItem.size(); i++)
	{
		ImageView *imgBack = m_ui.cardVecBack[i];
		ImageView *imgFront = m_ui.cardVecFront[i];

		imgFront->stopAllActions();
		imgBack->stopAllActions();
		
		vector<int> BoxItemInfo;
		StringFormat::parseCsvStringVecByDu(boxItem[i], BoxItemInfo);

		RowData* rowData = DataManager::getInstance()->searchToolById(BoxItemInfo[0]);
		UICardPrize *cardPrize = UICardPrize::create(BoxItemInfo[0]);
		imgFront->addChild(cardPrize);
		cardPrize->setCardCount(BoxItemInfo[1]);
		cardPrize->setCardName(rowData->getStringData("itemName"));


		// 动画序列（延时，隐藏，延时，隐藏） 
		CCSequence *pBackSeq = CCSequence::create(CCDelayTime::create(OrbitCameraTime),CCHide::create(),
			CCDelayTime::create(OrbitCameraTime),CCHide::create(),NULL); 
		CCScaleTo* pScaleBack = CCScaleTo::create(OrbitCameraTime*2,-0.5,0.5);
		CCSpawn* pSpawnBack = CCSpawn::create(pBackSeq,pScaleBack,NULL);
		// 动画序列（延时，显示，延时，显示） 
		CCSequence *pFrontSeq = CCSequence::create(CCDelayTime::create(OrbitCameraTime),CCShow::create(),
			CCDelayTime::create(OrbitCameraTime),CCShow::create(),NULL);
		CCScaleTo* pScaleFront = CCScaleTo::create(OrbitCameraTime*2,0.5,0.5);
		CCSpawn* pSpawnFront = CCSpawn::create(pFrontSeq,pScaleFront,NULL);

		m_isOrbitCamera = true;

		if (i >= 9 )
		{
			CallFunc *calNext = CallFunc::create(CC_CALLBACK_0(LayerMaze::cardObCamera,this));

			imgFront->setScaleX(0);
			imgBack->runAction(pSpawnBack); 
			imgFront->runAction(CCSequence::create(pSpawnFront,CCDelayTime::create(2.0f),calNext,nullptr));
		}
		else
		{
			imgFront->setScaleX(0);
			imgBack->runAction(pSpawnBack); 
			imgFront->runAction(pSpawnFront);
		}
	}
}

void LayerMaze::cardObCamera()
{
	for (int i = 0; i < 10; i++)
	{
		ImageView *imgBack = m_ui.cardVecBack[i];
		ImageView *imgFront = m_ui.cardVecFront[i];
		imgFront->stopAllActions();
		imgBack->stopAllActions();


		// 动画序列（延时，隐藏，延时，隐藏） 
		CCSequence *pFrontSeq = CCSequence::create(CCDelayTime::create(OrbitCameraTime),CCHide::create(),
			CCDelayTime::create(OrbitCameraTime),CCHide::create(),NULL); 
		CCScaleTo* pScaleFront = CCScaleTo::create(OrbitCameraTime*2,-0.5,0.5);
		CCSpawn* pSpawnFront = CCSpawn::create(pFrontSeq,pScaleFront,NULL);
		// 动画序列（延时，显示，延时，显示） 
		CCSequence *pBackSeq = CCSequence::create(CCDelayTime::create(OrbitCameraTime),CCShow::create(),
			CCDelayTime::create(OrbitCameraTime),CCShow::create(),NULL);
		CCScaleTo* pScaleBack = CCScaleTo::create(OrbitCameraTime*2,0.5,0.5);
		CCSpawn* pSpawnBack = CCSpawn::create(pBackSeq,pScaleBack,NULL);


		Size size = Director::getInstance()->getVisibleSize();
		Point pos = Director::getInstance()->getVisibleOrigin();	

		MoveTo *moveTo = MoveTo::create(0.4f,Vec2(m_ui.cardPanel->getContentSize().width/2,m_ui.cardPanel->getContentSize().height/2));
		CCDelayTime *delay = CCDelayTime::create(0.2f);
		MoveTo *moveBack = MoveTo::create(0.2f,Vec2(imgFront->getPosition().x,imgFront->getPosition().y));

		auto fuc = [this]()->void{
		m_isOrbitCamera = false;
		};
		CallFunc *moveBackCal = CallFunc::create(fuc);

		imgBack->setScaleX(0);
		imgFront->runAction(pSpawnFront); 
		imgBack->runAction(Sequence::create(pSpawnBack,CCDelayTime::create(0.2f),moveTo,delay,moveBack,moveBackCal,nullptr));
	}
}

void LayerMaze::baozangClose(Ref* sender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		RowData *data = DataManager::getInstance()->searchMazeBoxById(m_BoxId);

		int free = data->getIntData("freeDraw");
		if (m_openCount < free)
		{
			m_ui.layChoose->setVisible(true);
			ImageView *img_tips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.layChoose,"img_tips"));
			ActionCreator::runCommDlgAction(img_tips);
		}
		else
		{
			MazeController::getInstance()->onOpenBoxEnd();
		}
	}
}


void LayerMaze::onOpenBoxEndResult()
{
	m_ui.pBaozan->setVisible(false);
	gotoFirst();
}


void LayerMaze::onRewardResult(Maze_ReWard_Show reward)
{
	m_isGetDataFlag = false;
	m_isFilishedFlag = true;
	vector<PopRewardItem*> vecRewardItem;
	//道具
	for (size_t i = 0; i < reward.mVecRewardInfo.size(); i++)
	{
		MazeRewardInfo& signRewardInfo = reward.mVecRewardInfo.at(i);
		PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(signRewardInfo.mItemId, signRewardInfo.mItemCount);
		vecRewardItem.push_back(rewardItem);
	}

	//藏宝图
	for (size_t i = 0; i < reward.mVecRewardMap.size(); i++)
	{
		MazeRewardInfo& signRewardInfo = reward.mVecRewardMap.at(i);
		PopRewardItem_CangBao* rewardItem = new PopRewardItem_CangBao(signRewardInfo.mItemId, signRewardInfo.mItemCount);
		vecRewardItem.push_back(rewardItem);
	}

	//金币
	if (reward.mGold > 0)
	{
		PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(reward.mGold);
		vecRewardItem.push_back(rewardItem);
	}

	//银币
	if (reward.mCoin > 0)
	{
		PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(reward.mCoin);
		vecRewardItem.push_back(rewardItem);
	}

	//银币
	if (reward.mExp > 0)
	{
		PopRewardItem_Exp* rewardItem = new PopRewardItem_Exp(reward.mExp);
		vecRewardItem.push_back(rewardItem);
	}

	PopReward::show(vecRewardItem);

	updateMazeInfo();
	returnFirst(2.0f);
}

void LayerMaze::onRewardOpenCardResult(CHOOSEED_ITEM_REWARD reward)
{
	if (m_spendGoldCount > 0)
	{
		TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20268),1,m_spendGoldCount);
	}
	
	updateMazeInfo();

	ImageView *imgBack = m_ui.cardVecBack.at(reward.pos);
	ImageView *imgFront = m_ui.cardVecFront.at(reward.pos);

	imgFront->stopAllActions();
	imgBack->stopAllActions();
	imgBack->setTouchEnabled(false);

	//////////////////////////////////////////////////////////////////////////
	setChooseCardFreeCount(reward.openCount);

	RowData* rowData = DataManager::getInstance()->searchToolById(reward.drawItemId);
	UICardPrize *cardPrize = UICardPrize::create(reward.drawItemId);
	imgFront->addChild(cardPrize);
	cardPrize->setCardCount(reward.drawItemCount);
	cardPrize->setCardName(rowData->getStringData("itemName"));

	// 动画序列（延时，隐藏，延时，隐藏） 
	CCSequence *pBackSeq = CCSequence::create(CCDelayTime::create(OrbitCameraTime),CCHide::create(),
		CCDelayTime::create(OrbitCameraTime),CCHide::create(),NULL); 
	CCScaleTo* pScaleBack = CCScaleTo::create(OrbitCameraTime*2,-0.5,0.5);
	CCSpawn* pSpawnBack = CCSpawn::create(pBackSeq,pScaleBack,NULL);
	// 动画序列（延时，显示，延时，显示） 
	CCSequence *pFrontSeq = CCSequence::create(CCDelayTime::create(OrbitCameraTime),CCShow::create(),
		CCDelayTime::create(OrbitCameraTime),CCShow::create(),NULL);
	CCScaleTo* pScaleFront = CCScaleTo::create(OrbitCameraTime*2,0.5,0.5);
	CCSpawn* pSpawnFront = CCSpawn::create(pFrontSeq,pScaleFront,NULL);

	imgFront->setScaleX(0);
	imgBack->runAction(pSpawnBack); 
	imgFront->runAction(pSpawnFront);
}

void LayerMaze::onBtnTalkNext(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (!m_isFilishedFlag)
		{
			return;
		}
		if (m_isAnimitionRunning)
		{
			return;
		}	
		nextBtnAnimition(false);
		if (m_stepState > MAZE_STEP_NONE)
		{
			gotoFirst();
		}	
		else
		{
			if (MAZE_STEP_ENTER == m_stepState)
			{
				m_stepState = MAZE_STEP_NONE;
				RowData* pMazeData = DataManager::getInstance()->searchMazeBaseById(m_nCurMazeId);
				if (pMazeData != nullptr)
				{
					setNotice(pMazeData->getStringData("moveSay"));
				}
				m_ui.pLayGo->setVisible(true);
			}		
		}
	}
}

void LayerMaze::onBtnMeetChallenge(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int nId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("zhandou");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_isFilishedFlag)
		{
			return;
		}
		if (m_isGetDataFlag)
		{
			return;
		}	
		int eventId =  MazeModel::getInstance()->getEventId();
		RowData * eventData = DataManager::getInstance()->searchMazeThingById(eventId);
		int fightId = eventData->getIntData("fightId");
		fight(fightId);
		m_ui.pLayMainHero->setVisible(false);
		m_ui.meet->setVisible(false);
		m_isFilishedFlag = true;
	}
}
void LayerMaze::onBtnMeetRound(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int nId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		RowData* pData = DataManager::getInstance()->searchMazeThingById(nId);
		setTalkInfo( pData->getStringData("Name"), pData->getStringData("say2"));
		m_ui.pLayMainHero->setVisible(false);
		m_ui.meet->setVisible(false);
		m_isFilishedFlag = true;

		returnFirst(1.5f);
	}
}


void LayerMaze::onMainClicked(Ref* sender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_stepState == MAZE_STEP_ENTER)
		{
			nextBtnAnimition(false);
			if (m_stepState > MAZE_STEP_NONE)
			{
				gotoFirst();
			}	
			else
			{
				if (MAZE_STEP_ENTER == m_stepState)
				{
					m_stepState = MAZE_STEP_NONE;
					RowData* pMazeData = DataManager::getInstance()->searchMazeBaseById(m_nCurMazeId);
					if (pMazeData != nullptr)
					{
						setNotice(pMazeData->getStringData("moveSay"));
					}
				}		
			}
			return;
		}	
		//当行动力为0时候提示：行动力不足，等待恢复吧
		if (MainModel::getInstance()->getMainParam()->mMazePower <=0 )
		{
			showPowerLimit();
			return;
		}
		goAnimition();
	}
}

void LayerMaze::showPowerLimit()
{
	LayerCommonBuy *buyMazePower = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_MAZEPOWER);
	this->addChild(buyMazePower);
}

void LayerMaze::onChooseCard(Ref* sender,Widget::TouchEventType type,int index)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED && !m_isOrbitCamera)
	{
		RowData *data = DataManager::getInstance()->searchMazeBoxById(m_BoxId);

		vector<int> goldCount;
		StringFormat::parseCsvStringVec(data->getStringData("goldPay"), goldCount);

		int free = data->getIntData("freeDraw") - m_openCount;
		int spendGold = 0;
		m_spendGoldCount = 0;
		if (free <= 0 && free + data->getIntData("goldDraw") > 0)
		{
			spendGold = free + data->getIntData("goldDraw") ;
			m_spendGoldCount = goldCount[goldCount.size()-spendGold];
		}

		MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
		if (m_openCount >= data->getIntData("freeDraw")+data->getIntData("goldDraw"))
		{
			return;
		}
		else if (pMainParam->mGold < m_spendGoldCount)
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
			}
			return;
		}

		MazeController::getInstance()->onOpenMapBox(m_BoxId,index);
	}
}

void LayerMaze::onGiveUpChooseCard(Ref* sender,Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		m_ui.layChoose->setVisible(false);
		MazeController::getInstance()->onOpenBoxEnd();
	}
}

void LayerMaze::onCancleGiveUpChooseCard(Ref* sender,Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		m_ui.layChoose->setVisible(false);
	}
}

void LayerMaze::onMapListClicked(Ref* resenderf, Widget::TouchEventType type, int nId,int i)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
		RowData* pData = DataManager::getInstance()->searchMazeBaotuById(nId);
		if (pData != nullptr)
		{
			string itemTitle = pData->getStringData("tips1");
			string itemContex = pData->getStringData("tips2");

			m_ui.pImgMapTips->setVisible(true);

			m_ui.pImgMapTips->setPosition(Vec2(m_ui.pImgMapTips->getPosition().x,400-120*i));

			Text* pTextTitle = dynamic_cast<Text*>(m_ui.pImgMapTips->getChildByName("Lab_title"));
			Text* pTextIntro = dynamic_cast<Text*>(m_ui.pImgMapTips->getChildByName("Lab_intro"));

			pTextTitle->setText(itemTitle);
			pTextIntro->setText(itemContex);
			pTextTitle->setFontName(FONT_FZZHENGHEI);
			pTextIntro->setFontName(FONT_FZZHENGHEI);
		}
	}
	else if (type == Widget::TouchEventType::ENDED
		|| type == Widget::TouchEventType::CANCELED)		
	{
		m_ui.pImgMapTips->setVisible(false);
	}
}

void LayerMaze::talkEvent(const int& nId)
{
	RowData* pData = DataManager::getInstance()->searchMazeThingById(nId);
	if (pData != nullptr)
	{
		//人物弹出
		m_ui.pLayMainHero->setVisible(true);
		if (pData->getIntData("resId") <= 0)
		{
			//这里分性别 玩家玩的一张 女的一张
			if (HeroModel::getInstance()->getMainHero()->mSex == HERO_SEX_MALE)
			{
				m_ui.pImgMainHero->loadTexture("Image/Icon/integrityCard/41099.png");	
			}
			else
			{
				m_ui.pImgMainHero->loadTexture("Image/Icon/integrityCard/41098.png");
			}
		}
		else
		{
			m_ui.pImgMainHero->loadTexture(ResourceUtils::getHeroCompleteCardPath(pData->getIntData("resId")));	
			RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(pData->getIntData("resId"));
			vector<float> pointVect;
			StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
			m_ui.pImgMainHero->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));
		}

		Vec2 pos = m_ui.pImgMainHero->getPosition();
		m_ui.pImgMainHero->setPosition(Vec2(0,pos.y));
		m_ui.pImgMainHero->setScale(0.5f);

		CCMoveTo *moveTo = CCMoveTo::create(0.5f,pos);
		CCScaleTo *scaleTo = CCScaleTo::create(0.5f,1.0f);
		auto callFucTalk = [this]()->void
		{
			//对话事件还需请求奖励物品
			m_isGetDataFlag = true;
			m_pMazeController->onGetEventThingInfo(m_nCurEventId);
		};

		CCSpawn *spawn = CCSpawn::create(moveTo,scaleTo,nullptr);
		CCSequence *seq = CCSequence::create(spawn,CallFunc::create(callFucTalk),nullptr);
		m_ui.pImgMainHero->stopAllActions();
		m_ui.pImgMainHero->runAction(seq);

		//引导说话
		setTalkInfo( pData->getStringData("Name"), pData->getStringData("say1"));
	}
}


void LayerMaze::meetEvent(const int& nId)
{
	RowData* pData = DataManager::getInstance()->searchMazeThingById(nId);
	if (pData != nullptr)
	{
		//人物弹出
		m_ui.pLayMainHero->setVisible(true);
		m_ui.meet->setVisible(true);
		m_ui.pImgMainHero->loadTexture(ResourceUtils::getHeroCompleteCardPath(pData->getIntData("resId")));	
		RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(pData->getIntData("resId"));
		vector<float> pointVect;
		StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
		m_ui.pImgMainHero->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));

		Vec2 pos = m_ui.pImgMainHero->getPosition();
		m_ui.pImgMainHero->setPosition(Vec2(0,pos.y));
		m_ui.pImgMainHero->setScale(0.5f);

		CCMoveTo *moveTo = CCMoveTo::create(0.5f,pos);
		CCScaleTo *scaleTo = CCScaleTo::create(0.5f,1.0f);
		CCSpawn *spawn = CCSpawn::create(moveTo,scaleTo,nullptr);
		m_ui.pImgMainHero->stopAllActions();
		m_ui.pImgMainHero->runAction(spawn);

		//引导说话
		setTalkInfo( pData->getStringData("Name"), pData->getStringData("say1"));

		//偶遇选择
		m_ui.meetChallenge->addTouchEventListener(CC_CALLBACK_2(LayerMaze::onBtnMeetChallenge, this, nId));
		m_ui.meetRound->addTouchEventListener(CC_CALLBACK_2(LayerMaze::onBtnMeetRound, this, nId));

		Text* pTextChallenge = dynamic_cast<Text*>(m_ui.meetChallenge->getChildByName("Lab_sel1"));
		Text* pTextRound = dynamic_cast<Text*>(m_ui.meetRound->getChildByName("Lab_sel1"));
		pTextChallenge->setFontName(FONT_FZZHENGHEI);
		pTextRound->setFontName(FONT_FZZHENGHEI);

		pTextChallenge->setString(DataManager::getInstance()->searchCommonTexdtById(20269));
		pTextRound->setString(DataManager::getInstance()->searchCommonTexdtById(20270));
	}
}

void LayerMaze::boxEvent(const int& nId)
{
	RowData*pData = DataManager::getInstance()->searchMazeBaotuById(nId);
	if (pData != nullptr)
	{
		setNotice( pData->getStringData("say"));
		m_ui.pBtnBoxMain->stopAllActions();

		m_ui.pLayBox->setVisible(true);

		CCMoveBy *moveBy1 = CCMoveBy::create(0.5,Vec2(0,10));
		CCMoveBy *moveBy2 = CCMoveBy::create(0.5,Vec2(0,-10));

		CCRepeatForever *repeat = CCRepeatForever::create(CCSequence::create(moveBy1,moveBy2,nullptr));
		m_ui.pBtnBoxMain->runAction(repeat);

		m_ui.pBtnBoxMain->addTouchEventListener(CC_CALLBACK_2(LayerMaze::onBtnBoxOpen, this, nId));//pBtnBoxOpen
	}
}

void LayerMaze::onBtnBoxOpen(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int nId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (m_isFilishedFlag)
		{
			return;
		}
		if (m_isGetDataFlag)
		{
			return;
		}	
		m_isGetDataFlag = true;
		m_pMazeController->onOpenBox(nId);
	}
}

bool LayerMaze::baozangEvent()
{
	MAZE_INFOS& mazeInfos = MazeModel::getInstance()->getMazeInfos();
	m_TreasureMapId = 0;
	for (int i = 0; i < mazeInfos.vecMaps.size(); i++)
	{
		int id = mazeInfos.vecMaps.at(i).id;
		int num = mazeInfos.vecMaps.at(i).num;
		RowData* mazeBoxData =  DataManager::getInstance()->searchMazeBaotuById(id);
		int needNum = mazeBoxData->getIntData("num");

		if (num >= needNum)
		{
			m_TreasureMapId = id;   //本次打开的藏宝图ID
			return true;
		}
	}

	return false;
}

void LayerMaze::setNotice(const string& strContent)
{
	m_ui.pLayMainTalk->setVisible(true);
	m_ui.pImgTalkNameBg->setVisible(false);
	m_ui.pTextTalkContent->setText("");
	m_ui.pTextTalkContent->setColor(TALK_NOTICE_COLOR);
	animationTalkOut(strContent);	
}

void LayerMaze::setTalkInfo(const string& strName, const string& strContent)
{
	m_ui.pLayMainTalk->setVisible(true);
	m_ui.pImgTalkNameBg->setVisible(true);
	if (strName == "")
	{
		m_ui.pImgTalkNameBg->setVisible(false);
	}
	else
	{
		m_ui.pImgTalkNameBg->setVisible(true);
		m_ui.pTextTalkName->setText(strName);
	}
	
	m_ui.pTextTalkContent->setText("");//strContent
	m_ui.pTextTalkContent->setColor(TALK_TALK_COLOR);
	animationTalkOut(strContent);
}

//
void LayerMaze::hengFuAnimition(int idx)
{
	if (m_ui.pLayBox->isVisible())
	{
		return;
	}

	m_ui.hengfu->setVisible(true);
	float fD = 0.4;
	FadeIn* pFadeIn = FadeIn::create(fD);
	FadeOut* pFadeOut = FadeOut::create(fD);

	CallFuncN *pCallFuncN = CallFuncN::create(CC_CALLBACK_1(LayerMaze::hengFuAnimitionCallFunN,this,idx));
	Sequence* pSequence = Sequence::create( pFadeOut, pFadeIn, nullptr);
	Repeat *repeat = Repeat::create(pSequence, 2);
	Sequence* pSequence2 = Sequence::create( repeat, pCallFuncN, nullptr);
	switch (idx)
	{
	case HENGFU_QIANGDI:
		{
			m_ui.henfuQiangDi->setVisible(true);
			m_ui.henfuBoss->setVisible(false);
			m_ui.henfuBaozang->setVisible(false);

			m_ui.henfuQiangDi->runAction(pSequence2);
		}
		break;
	case HENGFU_BOSS:
		{
			m_ui.henfuQiangDi->setVisible(false);
			m_ui.henfuBoss->setVisible(true);
			m_ui.henfuBaozang->setVisible(false);

			m_ui.henfuBoss->runAction(pSequence2);
		}
		break;
	case HENGFU_BAOZANG:
		{		
			m_ui.henfuQiangDi->setVisible(false);
			m_ui.henfuBoss->setVisible(false);
			m_ui.henfuBaozang->setVisible(true);

			m_ui.henfuBaozang->runAction(pSequence2);
		}
		break;
	default:
		break;
	}
}

void LayerMaze::hengFuAnimitionCallFunN(cocos2d::Node* pNode,int idx)
{
	m_ui.hengfu->setVisible(false);
	switch (idx)
	{
	case HENGFU_QIANGDI:
		{
			meetEvent(MazeModel::getInstance()->getEventId());
		}
		break;
	case HENGFU_BOSS:
		{
			//播放BOSS动画和对白1秒后进入战斗
			int eventId =  MazeModel::getInstance()->getEventId();
			RowData * eventData = DataManager::getInstance()->searchMazeThingById(eventId);
			int fightId = eventData->getIntData("fightId");
			fight(fightId);
		}
		break;
	case HENGFU_BAOZANG:
		{
			boxEvent(m_TreasureMapId);
		}
		break;
	default:
		break;
	}
}

void LayerMaze::goAnimition()
{
	m_ui.pLayGo->setVisible(false);
	if (m_isAnimitionRunning)
	{
		return;
	}
	SoundUtils::playSoundEffect("xingzou");
	m_isAnimitionRunning = true;
	//m_ui.pImgMainBg->setVisible(false);
	//放大、上下抖动、消失、循环
	m_ui.pImgMainBackup->setOpacity(255);
	m_ui.pImgMainBackup->setScale(1.0);
	cocos2d::Vec2 vecPos = m_ui.pImgMainBackup->getPosition();
	float fD = 0.5;
	FadeIn* pFadeIn = FadeIn::create(0.1);
	//放大
	ScaleTo* pScale3 = ScaleTo::create(1.35, 1.6);
	//上下抖动
	MoveTo* pMoveUp = MoveTo::create(0.3, cocos2d::Vec2(vecPos.x, vecPos.y+30));
	MoveTo* pMoveDown = MoveTo::create(0.15, cocos2d::Vec2(vecPos.x, vecPos.y));

	Sequence* pSequence = Sequence::create( pMoveUp, pMoveDown, nullptr);
	
	Repeat *repeatUpDown = Repeat::create(pSequence, 3);//循环//上下抖动重复3次
	//消失
	FadeOut* pFadeOut = FadeOut::create(0.1);
	//
	Spawn *pSpawn = Spawn::create(pScale3, repeatUpDown, nullptr);
	//执行完动作回调
	auto goAnimitionCall = [this](Node* pNode)->void
	{
		m_isAnimitionRunning = false;
		m_isGetDataFlag = true;
		m_pMazeController->onRandomRoad();
	};
	CallFuncN *pCallFuncN = CallFuncN::create(goAnimitionCall);
	Sequence* pSequenceGo = Sequence::create( pSpawn, pFadeOut, pCallFuncN, nullptr);
	
	m_ui.pImgMainBackup->runAction(pSequenceGo);
}


void LayerMaze::nextBtnAnimition(bool isStart)
{
	if (isStart)
	{
		if (!m_isFilishedFlag)
		{
			return;
		}
		MoveTo* pMoveDown = MoveTo::create(0.5, Vec2(m_ui.vecBtnTalkNext.x, m_ui.vecBtnTalkNext.y-30));
		MoveTo* pMoveUp = MoveTo::create(0.5, m_ui.vecBtnTalkNext);
		Sequence *pSequence = Sequence::create( pMoveDown, pMoveUp, nullptr);
		RepeatForever* pRepeat = RepeatForever::create(pSequence);
		
		m_ui.pBtnTalkNext->runAction(pRepeat);
	}
	else
	{		
		m_ui.pBtnTalkNext->stopAllActions();
		m_ui.pBtnTalkNext->setPosition(m_ui.vecBtnTalkNext);
	}
}

void LayerMaze::fight(const int& fightId)
{
	Director::getInstance()->pushScene(SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_MAZE_TRUE, fightId));
	FightController::getInstance()->sendCommonFightMsg(fightId);
}

void LayerMaze::returnFirst(const float& fDelay)
{
	if (fDelay > 0.0)
	{
		//自动关闭
		Director::getInstance()->getScheduler()->schedule(
			schedule_selector(LayerMaze::callBackReturnFirst), this, 1, 3, fDelay, false);
	}	
}
//销毁回调
void LayerMaze::callBackReturnFirst(float dt)
{
	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(LayerMaze::callBackReturnFirst), this);
	gotoFirst();
}

void LayerMaze::gotoFirst()
{
	m_ui.pLayMainHero->setVisible(false);
	m_ui.meet->setVisible(false);
	m_isFilishedFlag = false;
	m_isGetDataFlag = false;
	MazeModel::getInstance()->clearMazEvent();

	auto call = [this]()->void
	{
		if(baozangEvent())
		{
			hengFuAnimition(HENGFU_BAOZANG);
			return;
		}

		//返回行走提示
		RowData* pMazeData = DataManager::getInstance()->searchMazeBaseById(m_nCurMazeId);
		if (pMazeData != nullptr)
		{
			setNotice(pMazeData->getStringData("moveSay"));
		}
		m_stepState = MAZE_STEP_NONE;
		m_ui.pLayGo->setVisible(true);
	};

	if(LevelUpController::getInstance()->checkLevelUp(call))
	{
		return;
	}

	call();
}


//对话动态显示
void LayerMaze::animationTalkOut(string strText, bool isL2R)
{
	m_strTalkContent = strText;

	m_nTalkContentLength = 0;
	//m_ui.pTextTalkContent->ignoreContentAdaptWithSize(false);//固定大小
	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(LayerMaze::callBackTalkOut), this);

	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(LayerMaze::callBackTalkOut), this, 0.03, kRepeatForever, 0, false);	
}

void LayerMaze::callBackTalkOut(float dt)
{
	if (m_nTalkContentLength > m_strTalkContent.length())
	{
		Director::getInstance()->getScheduler()->unschedule(
			schedule_selector(LayerMaze::callBackTalkOut), this);
		m_nTalkContentLength = 0;
	}
	else if (m_nTalkContentLength == m_strTalkContent.length())
	{
		m_nTalkContentLength ++;
		m_ui.pTextTalkContent->setText(m_strTalkContent.substr(0, m_nTalkContentLength));
	}
	else
	{
		char cValue = m_strTalkContent.at(m_nTalkContentLength);
		if (cValue>0 && cValue<127 )
		{
			m_nTalkContentLength ++;
		}
		else//utf8中文占3个字符
		{
			m_nTalkContentLength += 3;
			if (m_nTalkContentLength > m_strTalkContent.length()+1)
				m_nTalkContentLength = m_strTalkContent.length()+1;
		}
		m_ui.pTextTalkContent->setText(m_strTalkContent.substr(0, m_nTalkContentLength));
	}	
}

void LayerMaze::setImgJump(vector<ImageView*> vec)
{
	//每个文字跳动动画
	Vector<FiniteTimeAction*> arrayOfActions;
	for (int i = 0; i < vec.size(); i++)
	{
		ImageView* img = vec.at(i);
		auto *cal = CallFunc::create(CC_CALLBACK_0(LayerMaze::callFuc,this,img));
		arrayOfActions.pushBack(cal);
	}

	this->runAction(CCRepeatForever::create(Sequence::create(arrayOfActions)));
}

void LayerMaze::callFuc(ImageView *img)
{
	MoveBy* moveBy1 = MoveBy::create(0.1f,Vec2(0,10));
	MoveBy* moveBy2 = MoveBy::create(0.1f,Vec2(0,-10));
	CCDelayTime* delay = CCDelayTime::create(0.1f);
	img->runAction(Sequence::create(moveBy1,moveBy2,delay,nullptr));
}


void LayerMaze::startCountDown(int sec)
{
	if (sec > 0)
	{
		setTimeStamp(sec + m_pMazeController->getLoginServerTime());
		LocalTimer::getInstance()->addTimeStampListener(this);
	}
	else
	{
		setTimeStamp(0);
	}
}

void LayerMaze::timeOut()
{
	RowData* pMazeData = DataManager::getInstance()->searchMazeBaseById(MainModel::getInstance()->getMainParam()->mMazeId);
	int totalmove = pMazeData->getIntData("move");
	if (MainModel::getInstance()->getMainParam()->mMazePower < totalmove)
	{
		m_pMazeController->onGetAllMazes();
	}
}

void LayerMaze::addChooseCardPanel()
{
	updateMazeInfo();

	m_isGetDataFlag = false;
	m_ui.pLayBox->setVisible(false);

	Node* node = this->getChildByName("baozangmianban");
	if (node)
	{
		this->removeChildByName("baozangmianban");
	}
	//显示宝藏
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();	

	m_ui.pBaozan = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIMaze/UIMaze_Choose.ExportJson"));
	m_ui.pBaozan->setSize(size);
	m_ui.pBaozan->setPosition(pos);
	
	this->addChild(m_ui.pBaozan,5,"baozangmianban");

	//翻牌提示
	m_ui.layChoose = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pBaozan,"Pnl_tips"));
	m_ui.layChoose->setVisible(false);
	Text* layCHooseTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.layChoose,"Lab_1"));
	Text* layCHooseTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.layChoose,"Lab_2"));
	Button* layCHooseEnter = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.layChoose,"Btn_queren"));
	Button* layCHooseCancle = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.layChoose,"Btn_quxiao"));
	layCHooseTips1->setFontName(FONT_FZZHENGHEI);
	layCHooseTips2->setFontName(FONT_FZZHENGHEI);
	layCHooseEnter->addTouchEventListener(CC_CALLBACK_2(LayerMaze::onGiveUpChooseCard,this));
	layCHooseCancle->addTouchEventListener(CC_CALLBACK_2(LayerMaze::onCancleGiveUpChooseCard,this));

	m_ui.freeCard = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pBaozan,"Pnl_free"));
	m_ui.freeCardNum = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.freeCard,"Lab_num"));
	m_ui.goldCard = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pBaozan,"Pnl_gold"));
	m_ui.goldCardNum = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.goldCard,"Lab_num"));
	m_ui.cardPanel = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pBaozan,"Pnl_draw"));
	Text* Lab_free = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.freeCard,"Lab_free"));
	Text* Lab_gold = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.goldCard,"Lab_gold"));
	Button* BtnBaoZang = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pBaozan,"Btn_close"));
	BtnBaoZang->addTouchEventListener(CC_CALLBACK_2(LayerMaze::baozangClose, this));
	m_ui.cardUp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pBaozan,"Lab_up"));

	Lab_free->setFontName(FONT_FZZHENGHEI);
	Lab_gold->setFontName(FONT_FZZHENGHEI);
	m_ui.cardUp->setFontName(FONT_FZZHENGHEI);
	m_ui.freeCardNum->setFontName(FONT_FZZHENGHEI);
	m_ui.goldCardNum->setFontName(FONT_FZZHENGHEI);

	m_ui.cardVecBack.clear();
	m_ui.cardVecFront.clear();
	for (int i = 1; i <= 10; i++)
	{
		ImageView *img1 = nullptr;
		img1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.cardPanel,"Img_card"+TO_STR(i)));
		m_ui.cardVecBack.push_back(img1);

		ImageView *img2 = nullptr;
		img2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.cardPanel,"Img_card"+TO_STR(i)+"_0"));
		m_ui.cardVecFront.push_back(img2);

		img1->setVisible(true);
		img2->setVisible(false);


		img1->addTouchEventListener(CC_CALLBACK_2(LayerMaze::onChooseCard, this ,i-1));
	}
}

void LayerMaze::setChooseCardFreeCount(int openCount)
{
	RowData *data = DataManager::getInstance()->searchMazeBoxById(m_BoxId);

	int free = data->getIntData("freeDraw") - openCount;
	int spendGold = 0;
	m_openCount = openCount;
	if (free > 0)
	{
		spendGold = 0;
	}
	else
	{
		spendGold = data->getIntData("freeDraw") + data->getIntData("goldDraw") - m_openCount;
	}

	if (free > 0)
	{
		m_ui.freeCard->setVisible(true);
		m_ui.goldCard->setVisible(false);
		m_ui.cardUp->setVisible(false);

		m_ui.freeCardNum->setString(TO_STR(free));
	}
	else if (spendGold > 0)
	{
		m_ui.freeCard->setVisible(false);
		m_ui.goldCard->setVisible(true);
		m_ui.cardUp->setVisible(false);

		vector<int> goldCount;
		StringFormat::parseCsvStringVec(data->getStringData("goldPay"), goldCount);

		m_ui.goldCardNum->setString(TO_STR(goldCount[goldCount.size() - 1 - spendGold]));
	}
	else
	{
		m_ui.freeCard->setVisible(false);
		m_ui.goldCard->setVisible(false);
		m_ui.cardUp->setVisible(true);
	}
}

