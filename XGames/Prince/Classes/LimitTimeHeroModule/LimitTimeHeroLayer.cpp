#include "LimitTimeHeroLayer.h"
#include "MainModule/MainModel.h"
#include "MainModule/MainCityScene.h"
#include "LimitTimeHeroModel.h"
#include "LimitShopChouLayer.h"
#include "LimitTakeCardLayer.h"
#include "Widget/LayerGameRules.h"
#include "Widget/ActionCreator.h"
#include "Temp/PopReward.h"
#include "LimitShowBoxLayer.h"
#include "Temp/CustomGoldTips.h"
#include "Widget/LayerCommHeadInfo.h"


//积分盒子数量
const int boxNum = 7;

//积分、奖励列表间距
const float gapList = 4.0f;

//金币、银币ID
const int goldId = 11001;
const int coinId = 11002;

LimitTimeHeroLayer::LimitTimeHeroLayer()
{
	m_iBoxId = -1;
}

LimitTimeHeroLayer::~LimitTimeHeroLayer()
{
	
}

bool LimitTimeHeroLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void LimitTimeHeroLayer::onEnter()
{
	Layer::onEnter();

	LimitTimeHeroModel::getInstance()->addObserver(this);

	sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
	
	if(actData != nullptr)
	{
		//重置活动时间状态
		LimitTimeHeroModel::getInstance()->setActState(actData);

		if(actData->actState != eSTATE_WAIT && actData->actState != eSTATE_OVER)
		{
			auto callBack = [this](float dt)->void
			{
				m_Controller.sendGetMainInfoMsg();
				this->getScheduler()->unschedule("LimitTimeHeroLayer::sendMsg", this);
			};
			//活动有效时才拉协议
			getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LimitTimeHeroLayer::sendMsg");
		}
	}
	else
	{
		//活动已关闭
		auto callPop = [this](float dt)->void
		{
			auto call = [this](Ref* ref, CustomRetType type)->void
			{
				if (type == RET_TYPE_OK)
				{
					scheduleUpdate();
				}
			};

			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10023),call,CUSTOM_ID_YES);
			this->getScheduler()->unschedule("LimitTimeHeroLayer::showPop", this);
		};

		getScheduler()->schedule(callPop, this, 0, 0, 0, false, "LimitTimeHeroLayer::showPop");
	}

	showDownTime();
}

void LimitTimeHeroLayer::onExit()
{
	LocalTimer::getInstance()->removeLocalTimerListener(this);
	LimitTimeHeroModel::getInstance()->removeObserver(this);

	Layer::onExit();
}

void LimitTimeHeroLayer::update( float dt )
{
	Director::getInstance()->replaceScene(MainCityScene::create());
	this->unscheduleUpdate();
}

void LimitTimeHeroLayer::initUI()
{
	// 加载 UI;
	string strFileName;
	strFileName.assign("Image/UIxssj/UIxssj_main.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_advance.png");
	this->addChild(m_ui.pRoot);

	//公用返回
	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");
			scheduleUpdate();
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_LIMIT_TIME_HERO);
	this->addChild(_layerCommHeadInfo);

	//英雄展示节点
	//父节点
	ImageView* imgPar = (ImageView*)m_ui.pRoot->getChildByName("Image_juesedi");
	m_ui.imgHeroNick = (ImageView*)(Helper::seekWidgetByName(imgPar, "Img_heroNick"));
	m_ui.txtHeroName = (Text*)(Helper::seekWidgetByName(imgPar, "Lab_heroName"));
	m_ui.txtTenReward = (Text*)(Helper::seekWidgetByName(imgPar, "Label_reward"));
	m_ui.txtOneBuy = (Text*)imgPar->getChildByName("Label_danchou");
	m_ui.txtTenBuy = (Text*)imgPar->getChildByName("Label_shilian");
	//m_ui.txtHeroTalent = (TextAtlas*)imgPar->getChildByName("AtLab_zizhi");
	m_ui.imgHeroTalent = (ImageView*)imgPar->getChildByName("Img_zizhiLv");
	m_ui.imgHeroTalent->setScale(1.0f);
	m_ui.pnlHeroAni = (Layout*)imgPar->getChildByName("Pnl_hero");
	m_ui.btnHelp = (Button*)imgPar->getChildByName("Btn_help");
	m_ui.btnOneBuy = (Button*)imgPar->getChildByName("Btn_one");
	m_ui.btnTenBuy = (Button*)imgPar->getChildByName("Btn_ten");

	Text* txtFree = (Text*)imgPar->getChildByName("Label_free");
	txtFree->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(txtFree, Color3B::BLACK, 2);

	m_ui.txtHeroName->setString("");
	m_ui.txtHeroName->setFontName(FONT_FZZHENGHEI);

	m_ui.txtTenReward->setString("");
	m_ui.txtTenReward->setFontName(FONT_FZZHENGHEI);

	m_ui.txtOneBuy->setString("");
	m_ui.txtOneBuy->setFontName(FONT_FZZHENGHEI);

	m_ui.txtTenBuy->setString("");
	m_ui.txtTenBuy->setFontName(FONT_FZZHENGHEI);

	m_ui.btnHelp->addTouchEventListener(CC_CALLBACK_2(LimitTimeHeroLayer::onBtnClick, this, BTN_HELP));
	m_ui.btnOneBuy->addTouchEventListener(CC_CALLBACK_2(LimitTimeHeroLayer::onBtnClick, this, BTN_BUYONE));
	m_ui.btnTenBuy->addTouchEventListener(CC_CALLBACK_2(LimitTimeHeroLayer::onBtnClick, this, BTN_BUYTEN));

	//奖励滚动展示节点
	//父节点
	ImageView* imgScrollPar = (ImageView*)m_ui.pRoot->getChildByName("Image_paimingdi");
	m_ui.scrollScoreRank = (ScrollView*)imgScrollPar->getChildByName("Scroll_score");
	m_ui.scrollRewardRank = (ScrollView*)imgScrollPar->getChildByName("Scroll_reward");
	m_ui.scrollScoreRank->addEventListener(CC_CALLBACK_2(LimitTimeHeroLayer::onScrollViewEvent, this));
	m_ui.scrollRewardRank->addEventListener(CC_CALLBACK_2(LimitTimeHeroLayer::onScrollViewEvent, this));
	m_ui.pnlScoreItemTemp = (Layout*)m_ui.scrollScoreRank->getChildByName("Pnl_scoreItem");
	m_ui.pnlRewardItemTemp = (Layout*)m_ui.scrollRewardRank->getChildByName("Pnl_rewardItem");
	m_ui.pnlScoreItemTemp->setVisible(false);
	m_ui.pnlRewardItemTemp->setVisible(false);

	//积分进度节点
	//父节点
	//ImageView* barParent = (ImageView*)imgParent->getChildByName("Image_jindu");
	//m_ui.barScore = (LoadingBar*)barParent->getChildByName("bar_score");
	m_ui.barScore = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(m_ui.pRoot,"bar_score"));
	m_ui.barScore->setPercent(0);
	for(int i=0; i<boxNum; i++)
	{
		string uiName = "Btn_box_" + TO_STR(i+1);
		//Button* boxBtn = (Button*)barParent->getChildByName(uiName);
		Button* boxBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,uiName));
		Text* scorePoint = (Text*)boxBtn->getChildByName("Lab_score");
		scorePoint->setString("");
		scorePoint->setFontName(FONT_FZZHENGHEI);
		boxBtn->addTouchEventListener(CC_CALLBACK_2(LimitTimeHeroLayer::onBtnClick, this, BTN_SCOREBOX));
		m_ui.vecScoreBox.push_back(boxBtn);
	}

	//其它
	m_ui.txtMyScore = (Text*)(m_ui.pRoot->getChildByName("Image_di2")->getChildByName("Lab_myScore"));
	m_ui.txtMyRank = (Text*)(m_ui.pRoot->getChildByName("Image_di1")->getChildByName("Lab_myRank"));
	m_ui.txtGold = (Text*)(m_ui.pRoot->getChildByName("Image_di3")->getChildByName("Lab_gold"));
	m_ui.txtResDays = (Text*)m_ui.pRoot->getChildByName("Image_huodong")->getChildByName("Label_tian");								//活动剩余天数
	m_ui.txtResTime = (Text*)m_ui.pRoot->getChildByName("Image_huodong")->getChildByName("Label_time");								//活动剩余时分秒
	//m_ui.btnClose = (Button*)m_ui.pRoot->getChildByName("Btn_close");

	m_ui.txtMyScore->setString("");
	m_ui.txtMyScore->setFontName(FONT_FZZHENGHEI);

	m_ui.txtMyRank->setString("");
	m_ui.txtMyRank->setFontName(FONT_FZZHENGHEI);

	m_ui.txtGold->setString("");
	m_ui.txtGold->setFontName(FONT_FZZHENGHEI);

	m_ui.txtResDays->setString("");
	m_ui.txtResDays->setFontName(FONT_FZZHENGHEI);

	m_ui.txtResTime->setString("");
	m_ui.txtResTime->setFontName(FONT_FZZHENGHEI);

	//m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LimitTimeHeroLayer::onBtnClick, this, BTN_CLOSE));

	//滚动条
	m_scoreBar = CustomScrollBar::create();
	m_scoreBar->initScrollBar(m_ui.scrollScoreRank->getContentSize().height);
	m_scoreBar->setPosition(Point(8, m_ui.scrollScoreRank->getPositionY()));
	m_ui.scrollScoreRank->getParent()->addChild(m_scoreBar);
	m_scoreBar->setScrollBarLengthPercent(1.0f);
	m_scoreBar->setScrollBarPercentY(0.0f);

	m_rewardBar = CustomScrollBar::create();
	m_rewardBar->initScrollBar(m_ui.scrollRewardRank->getContentSize().height);
	m_rewardBar->setPosition(Point(8, m_ui.scrollRewardRank->getPositionY()));
	m_ui.scrollRewardRank->getParent()->addChild(m_rewardBar);
	m_rewardBar->setScrollBarLengthPercent(1.0f);
	m_rewardBar->setScrollBarPercentY(0.0f);
}

void LimitTimeHeroLayer::updateUI()
{
	sActData* actData = LimitTimeHeroModel::getInstance()->getActData();

	if(actData == nullptr)
	{
		return;
	}

	//活动英雄相关数据
	m_heroParam = HeroParam::create();
	RowData* heroAttrData = DataManager::getInstance()->searchCharacterById(actData->heroId);
	if (heroAttrData)
	{
		m_heroParam->mHeroType = heroAttrData->getIntData("tagTyp");
		m_heroParam->mAtkTyp =  heroAttrData->getIntData("atkTyp");
		m_heroParam->mJobTyp = heroAttrData->getIntData("jobType");
		m_heroParam->mName = heroAttrData->getStringData("name");
		m_heroParam->mTalent = heroAttrData->getIntData("apt");
		m_heroParam->mQuality = heroAttrData->getIntData("qua");
	}

	RowData* heroResData = DataManager::getInstance()->searchToolById(actData->heroId);
	if (heroResData)
	{
		m_heroParam->mResId = heroResData->getIntData("resId");
	}

	//英雄展示栏
	m_ui.imgHeroNick->loadTexture(ResourceUtils::getNickNamePath(m_heroParam->mResId));	//称号
	m_ui.txtHeroName->setString(m_heroParam->mName);
	setTextAddStroke(m_ui.txtHeroName, Color3B(0x82, 0x2b, 0x2b), 2);
	string strReward = "";
	if(actData->actType == eMETHOD_TAKECARD)
	{
		//strReward = "必得英雄&万能碎片";
		strReward = DataManager::getInstance()->searchCommonTexdtById(10245);
	}
	else
	{
		//strReward = "必得万能碎片宝箱";
		strReward = DataManager::getInstance()->searchCommonTexdtById(10246);
	}
	m_ui.txtTenReward->setString(strReward);
	//m_ui.txtOneBuy->setString(TO_STR(LimitTimeHeroModel::getInstance()->getCost(eTYPE_ONE)));
	m_ui.txtTenBuy->setString(TO_STR(LimitTimeHeroModel::getInstance()->getCost(eTYPE_TEN)));
	//m_ui.txtHeroTalent->setString(TO_STR(m_heroParam->mTalent));
	m_ui.imgHeroTalent->loadTexture(ResourceUtils::getHeroTalentPath(m_heroParam->mTalent));
	setTextAddStroke(m_ui.txtTenBuy, Color3B::BLACK, 2);

	//是否能免费抽
	ImageView* parent = (ImageView*)m_ui.txtOneBuy->getParent();
	ImageView* imgFree = (ImageView*)parent->getChildByName("Img_Free");
	ImageView* imgGold = (ImageView*)parent->getChildByName("Image_jinbi");
	if(LimitTimeHeroModel::getInstance()->isCanBuyFree())
	{
		imgFree->setVisible(true);
		imgGold->setVisible(false);
		m_ui.txtOneBuy->setString("");
	}
	else
	{
		imgFree->setVisible(false);
		imgGold->setVisible(true);
		m_ui.txtOneBuy->setString(TO_STR(LimitTimeHeroModel::getInstance()->getCost(eTYPE_ONE)));
		setTextAddStroke(m_ui.txtOneBuy, Color3B::BLACK, 2);
	}

	//展示英雄
	showHeroAni();

	//排名奖励信息
	upateRankRewardView();

	//刷新积分排名区域和我的积分
	updateScoreRankState();

	//刷新积分宝箱状态
	updateRewardBoxState();

	//显示倒计时
	showDownTime();

	openUITimer();
}

void LimitTimeHeroLayer::showHeroAni()
{
	int nResID = m_heroParam->mResId;
	//m_ui.pnlHeroAni->removeAllChildrenWithCleanup(true);
	SpineAnimation* pSkelFighter = (SpineAnimation*)m_ui.pnlHeroAni->getChildByName("HeroSpine");
	if(pSkelFighter == nullptr)
	{
		pSkelFighter = SpineAnimation::createFighterWithResId(nResID);
		pSkelFighter->setPosition(Vec2(m_ui.pnlHeroAni->getContentSize().width/2, 28));
		pSkelFighter->setAnimation(0, "wait", true);
		m_ui.pnlHeroAni->addChild(pSkelFighter, 1, "HeroSpine");
	}

	//粒子效果
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniCharaparticle/AniCharaparticle.ExportJson");
	Armature* particleFrontAni = (Armature*)m_ui.pnlHeroAni->getChildByName("FRONT_ANI");
	if(particleFrontAni == nullptr)
	{
		particleFrontAni = Armature::create("AniCharaparticle");
		particleFrontAni->setPosition(Point(m_ui.pnlHeroAni->getContentSize().width/2, 28));
		particleFrontAni->setVisible(false);
		particleFrontAni->setName("FRONT_ANI");
		m_ui.pnlHeroAni->addChild(particleFrontAni, 99);
	}
	
	Armature* particleBackAni = (Armature*)m_ui.pnlHeroAni->getChildByName("BACK_ANI");
	if(particleBackAni == nullptr)
	{
		particleBackAni = Armature::create("AniCharaparticle");
		particleBackAni->setPosition(Point(m_ui.pnlHeroAni->getContentSize().width/2, 28));
		particleBackAni->setVisible(false);
		particleBackAni->setName("BACK_ANI");
		m_ui.pnlHeroAni->addChild(particleBackAni, -99);
	}

	//默认显示红色
	string backName = "red_back";
	string frontName = "red_front";

	if (!frontName.empty())
	{
		particleFrontAni->setVisible(true);
		particleFrontAni->getAnimation()->play(frontName);
	}
	else
	{
		particleFrontAni->setVisible(false);
	}

	if (!backName.empty())
	{
		particleBackAni->setVisible(true);
		particleBackAni->getAnimation()->play(backName);
	}
	else
	{
		particleFrontAni->setVisible(false);
	}
}

void LimitTimeHeroLayer::showDownTime()
{
	sActData* actData = LimitTimeHeroModel::getInstance()->getActData();

	//活动进行态
	sFormatTime strTime;

	if(actData == nullptr)
	{
		//strTime.strDays = STR_UTF8("0天");
		//strTime.strHMS = STR_UTF8("00时00分00秒");
		strTime.strDays = "0" + DataManager::getInstance()->searchCommonTexdtById(10150);
		strTime.strHMS = "00" + DataManager::getInstance()->searchCommonTexdtById(10151)
					   + "00" + DataManager::getInstance()->searchCommonTexdtById(10152)
					   + "00" + DataManager::getInstance()->searchCommonTexdtById(10153);
	}
	else
	{
		formatDetailsTime(actData->actTime.actOverSec, &strTime);
	}
	
	m_ui.txtResDays->setString(strTime.strDays);
	m_ui.txtResTime->setString(strTime.strHMS);

	setTextAddStroke(m_ui.txtResDays, Color3B(0x0e, 0x79, 0x03), 2);
	setTextAddStroke(m_ui.txtResTime, Color3B(0x09, 0x4e, 0x51), 2);
}

void LimitTimeHeroLayer::upateRankRewardView()
{
	sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
	vector<sRankRewardData>* rewardData = LimitTimeHeroModel::getInstance()->getRankRewardData();

	//滚动区域
	Vector<Node*> AllChild = m_ui.scrollRewardRank->getChildren();
	for(int i=0; i<AllChild.size(); i++)
	{
		if(AllChild.at(i) != m_ui.pnlRewardItemTemp)
		{
			AllChild.at(i)->removeFromParent();
		}
	}
	Size size(0, 0);
	size.width = m_ui.scrollRewardRank->getContentSize().width;
	if(rewardData->size() < 4)
	{
		size.height = m_ui.scrollRewardRank->getContentSize().height;
	}
	else
	{
		size.height = (m_ui.pnlRewardItemTemp->getContentSize().height + gapList)*rewardData->size();
	}
	m_ui.scrollRewardRank->setInnerContainerSize(size);

	m_ui.scrollRewardRank->jumpToTop();

	m_rewardBar->setScrollBarLengthPercent(m_ui.scrollRewardRank->getContentSize().height/m_ui.scrollRewardRank->getInnerContainerSize().height);
	m_rewardBar->setScrollBarPercentY(0.0f);

	for(int i=0; i<rewardData->size(); i++)
	{
		sRankRewardData* data = &(rewardData->at(i));
		Layout* rewardItem = (Layout*)m_ui.pnlRewardItemTemp->clone();

		Text* txtRank = (Text*)rewardItem->getChildByName("Lab_rank");
		txtRank->setString("");
		txtRank->setFontName(FONT_FZZHENGHEI);

		if(data->rankSection.beginRank == data->rankSection.endRank)
		{
			//txtRank->setString(STR_UTF8("第" + TO_STR(data->rankSection.beginRank) + "名"));
			txtRank->setString(DataManager::getInstance()->searchCommonTexdtById(10110) + TO_STR(data->rankSection.beginRank)
								+ DataManager::getInstance()->searchCommonTexdtById(10247));
		}
		else
		{
			txtRank->setString(DataManager::getInstance()->searchCommonTexdtById(10110) + TO_STR(data->rankSection.beginRank) + "~" + 
				TO_STR(data->rankSection.endRank) + DataManager::getInstance()->searchCommonTexdtById(10247));
		}
		setTextAddStroke(txtRank, Color3B::BLACK, 2);
		//积分限制
		Text* labScoreLimit = (Text*)rewardItem->getChildByName("Lab_point");
		if(data->limit > 0)
		{
			//labScoreLimit->setString(STR_UTF8("积分>=") + TO_STR(data->limit));
			labScoreLimit->setString(DataManager::getInstance()->searchCommonTexdtById(20196) + ">=" + TO_STR(data->limit));
		}
		else
		{
			//labScoreLimit->setString(STR_UTF8("无积分限制"));
			labScoreLimit->setString(DataManager::getInstance()->searchCommonTexdtById(20327));
		}
		labScoreLimit->setFontName(FONT_FZZHENGHEI);

		//奖励道具
		for(int j=0; j<data->rankRewardItem.size(); j++)
		{
			Layout* pnlItem = (Layout*)rewardItem->getChildByName("Pnl_reward" + TO_STR(j+1));
			pnlItem->removeAllChildren();
			
			UIToolHeader* tool = UIToolHeader::create(data->rankRewardItem.at(j).rewardId);
			if(tool != nullptr)
			{
				pnlItem->addChild(tool);
				tool->setTipsEnabled(true);
				tool->setToolId(data->rankRewardItem.at(j).rewardId);
				tool->setNumEx(data->rankRewardItem.at(j).rewardCnt);
				tool->setScale(0.5f);
				tool->setAnchorPoint(Point(0, 0));
			}
		}

		rewardItem->setVisible(true);

		if(i == 0)
		{
			rewardItem->setPositionY(m_ui.scrollRewardRank->getInnerContainerSize().height - 2);
		}
		else
		{		
			rewardItem->setPositionY((m_ui.scrollRewardRank->getInnerContainerSize().height - 2) - 
				i*(rewardItem->getContentSize().height + gapList));
		}

		rewardItem->setAnchorPoint(Point(0, 1));
		m_ui.scrollRewardRank->addChild(rewardItem);
	}
}

void LimitTimeHeroLayer::updateScoreRankState()
{
	sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
	sScoreRank* myScoreData = LimitTimeHeroModel::getInstance()->getMyScoreData();
	vector<sScoreRank>* scoreRank = LimitTimeHeroModel::getInstance()->getScoreRankData();

	//我的积分
	m_ui.txtMyScore->setString(TO_STR(myScoreData->score));
	if(myScoreData->rank == 0)
	{
		m_ui.txtMyRank->setString(DataManager::getInstance()->searchCommonTexdtById(10031));
	}
	else
	{
		m_ui.txtMyRank->setString(TO_STR(myScoreData->rank));
	}
	
	setTextAddStroke(m_ui.txtMyScore, Color3B::BLACK, 2);
	setTextAddStroke(m_ui.txtMyRank, Color3B::BLACK, 2);

	//全服积分排名
	//滚动区域
	Vector<Node*> AllChild = m_ui.scrollScoreRank->getChildren();
	for(int i=0; i<AllChild.size(); i++)
	{
		if(AllChild.at(i) != m_ui.pnlScoreItemTemp)
		{
			AllChild.at(i)->removeFromParent();
		}
	}
	Size size(0, 0);
	size.width = m_ui.scrollScoreRank->getContentSize().width;
	if(scoreRank->size() < 4)
	{
		size.height = m_ui.scrollScoreRank->getContentSize().height;
	}
	else
	{
		size.height = (m_ui.pnlScoreItemTemp->getContentSize().height + gapList)*scoreRank->size();
	}
	m_ui.scrollScoreRank->setInnerContainerSize(size);

	m_ui.scrollScoreRank->jumpToTop();

	m_scoreBar->setScrollBarLengthPercent(m_ui.scrollScoreRank->getContentSize().height/m_ui.scrollScoreRank->getInnerContainerSize().height);
	m_scoreBar->setScrollBarPercentY(0.0f);

	for(int i=0; i<scoreRank->size(); i++)
	{
		Layout* scoreItem = (Layout*)m_ui.pnlScoreItemTemp->clone();
		ImageView* imgRank = (ImageView*)scoreItem->getChildByName("Img_rank");
		Text* txtRank = (Text*)scoreItem->getChildByName("Lab_rank");
		Text* txtScore = (Text*)scoreItem->getChildByName("Lab_score");
		Text* txtName = (Text*)scoreItem->getChildByName("Lab_name");
		imgRank->setVisible(false);
		txtRank->setString("");
		txtScore->setString("");
		txtName->setString("");
		txtRank->setFontName(FONT_FZZHENGHEI);
		txtScore->setFontName(FONT_FZZHENGHEI);
		txtName->setFontName(FONT_FZZHENGHEI);
		if(i < 3)
		{
			//前三名
			imgRank->loadTexture("Image/UIxssj/Image/img_paiming_" + TO_STR(i+1) + ".png");
			imgRank->setVisible(true);
		}
		else
		{
			imgRank->setVisible(false);
			txtRank->setString(TO_STR(scoreRank->at(i).rank));
			setTextAddStroke(txtRank, Color3B::BLACK, 2);
		}

		txtScore->setString(TO_STR(scoreRank->at(i).score));
		txtName->setString(scoreRank->at(i).name);
		setTextAddStroke(txtName, Color3B(0x7d, 0x3f, 0x1c), 2);
		scoreItem->setVisible(true);

		if(i == 0)
		{
			scoreItem->setPositionY(m_ui.scrollScoreRank->getInnerContainerSize().height - 2);
		}
		else
		{		
			scoreItem->setPositionY((m_ui.scrollScoreRank->getInnerContainerSize().height - 2) - 
				i*(scoreItem->getContentSize().height + gapList));
		}

		scoreItem->setAnchorPoint(Point(0, 1));
		m_ui.scrollScoreRank->addChild(scoreItem);
	}
}

void LimitTimeHeroLayer::updateRewardBoxState()
{
	sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
	sScoreRank* myScoreData = LimitTimeHeroModel::getInstance()->getMyScoreData();

	m_ui.txtGold->setString(TO_STR(MainModel::getInstance()->getMainParam()->mGold));
	setTextAddStroke(m_ui.txtGold, Color3B::BLACK, 2);

	//积分进度
	std::vector<sRewardBox>* vecBoxInfo = &(actData->vecBoxRewardData);
	sRewardBox boxInfo = vecBoxInfo->back();
	//loadingbar等分段显示
	int duan = 0;
	if(myScoreData->score >= vecBoxInfo->back().scorePoint)
	{
		m_ui.barScore->setPercent(100);
	}
	else
	{
		for (int i = 0; i < vecBoxInfo->size(); i++)
		{
			if (myScoreData->score < vecBoxInfo->at(i).scorePoint)
			{
				duan = i;
				break;
			}
		}

		m_ui.barScore->setPercent((((float)duan) / ((float)vecBoxInfo->size()))*100);
	}
	
	//m_ui.barScore->setPercent((((float)myScoreData->score)/((float)boxInfo.scorePoint))*100);
	//宝箱状态
	for(int i=0; i<vecBoxInfo->size(); i++)
	{
		Button* boxBtn = nullptr; 
		if(i < m_ui.vecScoreBox.size())
		{
			boxBtn = m_ui.vecScoreBox.at(i);
		}
		if(boxBtn != nullptr)
		{
			//积分
			Text* txtScore = (Text*)boxBtn->getChildByName("Lab_score");
			txtScore->setString(TO_STR(vecBoxInfo->at(i).scorePoint));
			setTextAddStroke(txtScore, Color3B::BLACK, 2);
			
			if(vecBoxInfo->at(i).isCanGet)
			{
				//可以领取
				boxBtn->loadTextures("Image/UIxssj/Image/img_baoxiang2_2_1.png", "Image/UIxssj/Image/img_baoxiang2_2_2.png");
				showCanGetAni(boxBtn);
			}
			else
			{
				if(vecBoxInfo->at(i).isGetted)
				{
					//已经领取过
					boxBtn->loadTextures("Image/UIxssj/Image/img_baoxiang2_3.png", "Image/UIxssj/Image/img_baoxiang2_3.png");
				}
				else
				{
					//未达到领取条件
					boxBtn->loadTextures("Image/UIxssj/Image/img_baoxiang2_1_1.png", "Image/UIxssj/Image/img_baoxiang2_1_2.png");
				}

				removeCanGetAni(boxBtn);
			}
		}
	}
}

void LimitTimeHeroLayer::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	Button* btn = (Button*)ref;
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case BTN_BUYONE:
			{
				sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
				if(actData == nullptr || actData->actState != eSTATE_OPEN)
				{
					//活动已关闭
					//CustomTips::show("活动已结束");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10023));
					return;		//活动结束禁止抽卡
				}
				if(LimitTimeHeroModel::getInstance()->isCanBuyFree())
				{
					//免费抽发送协议
					m_Controller.sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_ONE);
				}
				else 
				{
					if(MainModel::getInstance()->getMainParam()->mGold >= LimitTimeHeroModel::getInstance()->getCost(eTYPE_ONE))
					{
						//金币抽发送协议
						m_Controller.sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_ONE);
					}
					else
					{
						//弹窗金币不足
						CustomGoldTips *goldTips = CustomGoldTips::create();
						if (goldTips)
						{
							addChild(goldTips);
						}
					}
				}
			}
			break;

		case BTN_BUYTEN:
			{
				sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
				if(actData == nullptr || actData->actState != eSTATE_OPEN)
				{
					//活动已关闭
					//CustomTips::show("活动已结束");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10023));
					return;		//活动结束禁止抽卡
				}

				if(MainModel::getInstance()->getMainParam()->mGold >= LimitTimeHeroModel::getInstance()->getCost(eTYPE_TEN))
				{
					//发送协议
					m_Controller.sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_TEN);
				}
				else
				{
					//弹窗金币不足
					CustomGoldTips *goldTips = CustomGoldTips::create();
					if (goldTips)
					{
						addChild(goldTips);
					}
				}
			}
			break;

		case BTN_HELP:
			{
				LayerGameRules* XXSJRule = nullptr;

				if(LimitTimeHeroModel::getInstance()->getActData()->actType == eMETHOD_TAKECARD)
				{
					XXSJRule = LayerGameRules::create(LIMIT_TIME_HERO_CARD_RULE_ID);
				}
				else
				{
					XXSJRule = LayerGameRules::create(LIMIT_TIME_HERO_BOX_RULE_ID);
				}

				if (nullptr != XXSJRule)
				{
					Director::getInstance()->getRunningScene()->addChild(XXSJRule, 100);
					ActionCreator::runCommDlgAction(XXSJRule);
				}
			}
			break;

		case BTN_CLOSE:
			scheduleUpdate();
			break;

		case BTN_SCOREBOX:
			{
				onBoxClicked(btn);
			}
			break;

		default:
			break;
		}
	}
}


void LimitTimeHeroLayer::onBoxClicked( Button* btn )
{
	sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
	std::vector<sRewardBox>* vecBoxInfo = &(actData->vecBoxRewardData);

	Text* txtScore = (Text*)btn->getChildByName("Lab_score");

	//宝箱所需积分
	int score = atoi(txtScore->getString().c_str());

	//查找宝箱id
	sRewardBox* boxInfo = nullptr;
	for(int i=0; i<vecBoxInfo->size(); i++)
	{
		if(score == vecBoxInfo->at(i).scorePoint)
		{
			boxInfo = &(vecBoxInfo->at(i));
			break;
		}
	}

	if(boxInfo != nullptr)
	{
		if(boxInfo->isCanGet)
		{
			//发送协议
			m_Controller.sendGetScoreRewardMsg(boxInfo->boxId);
			m_iBoxId = boxInfo->boxId;
		}
		else
		{
			LimitShowBoxLayer* oldLayer = (LimitShowBoxLayer*)m_ui.pRoot->getChildByName("showBoxLayer");
			int oldTag = -1;
			if(oldLayer != nullptr)
			{
				oldTag = oldLayer->getTag();
				oldLayer->removeFromParentAndCleanup(true);
			}
			if(oldTag != boxInfo->boxId)
			{
				//调用展示层接口
				LimitShowBoxLayer* layer = LimitShowBoxLayer::create(boxInfo->boxId);
				layer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
				layer->setName("showBoxLayer");
				layer->setTag(boxInfo->boxId);
				m_ui.pRoot->addChild(layer);
			}	
		}
	}
}


void LimitTimeHeroLayer::showBoxRewardGetted()
{
	vector<PopRewardItem*> vecRewardItem;
	
	sRewardBox* gettedBox = LimitTimeHeroModel::getInstance()->getRewardBoxById(m_iBoxId);
	for(int i=0; i<gettedBox->vecRewardData.size(); i++)
	{
		sRankRewardItem rewardData = gettedBox->vecRewardData.at(i);
		if(rewardData.rewardId == goldId)
		{
			PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(rewardData.rewardCnt);
			vecRewardItem.push_back(rewardItem);
		}
		//银币
		else if(rewardData.rewardId == coinId)
		{
			PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(rewardData.rewardCnt);
			vecRewardItem.push_back(rewardItem);
		}
		else
		{
			PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(rewardData.rewardId, rewardData.rewardCnt);
			vecRewardItem.push_back(rewardItem);
		}
	}

	PopReward::show(vecRewardItem);
}

void LimitTimeHeroLayer::addBuyLayerByType(eChouKaType type)
{
	auto actType = LimitTimeHeroModel::getInstance()->getActData()->actType;
	if (actType == eActType::eMETHOD_OPENBOX)
	{
		this->addChild(LimitShopChouLayer::create(type));
	}
	else if (actType == eActType::eMETHOD_TAKECARD)
	{
		this->addChild(LimitTakeCardLayer::create(type));
	}
}


void LimitTimeHeroLayer::onTakeCardBoxEvent()
{
	LimitTimeHeroModel* pInstance = LimitTimeHeroModel::getInstance();
	if(!pInstance->getIsAddBuyLayer())
	{
		if(pInstance->getBuyItemParam().size() == iItemNumOne)
		{
			auto callBack = [this](float dt)->void
			{
				//抽卡一次信息返回 添加抽回界面播放动画
				addBuyLayerByType(eTYPE_ONE);
				this->getScheduler()->unschedule("LimitTimeHeroLayer::addLayer", this);
			};

			getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LimitTimeHeroLayer::addLayer");
		}
		else if(pInstance->getBuyItemParam().size() == iItemNumTen)
		{
			auto callBack = [this](float dt)->void
			{
				//抽卡一次信息返回 添加抽回界面播放动画
				addBuyLayerByType(eTYPE_TEN);
				this->getScheduler()->unschedule("LimitTimeHeroLayer::addLayer", this);
			};

			getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LimitTimeHeroLayer::addLayer");
		}

		pInstance->setIsAddBuyLayer(true);
	}
}

void LimitTimeHeroLayer::formatDetailsTime( int secs, sFormatTime* strTime )
{
	if(secs <= 0)
	{
		//strTime->strDays = STR_UTF8("0天");
		//strTime->strHMS = STR_UTF8("00时00分00秒");
		strTime->strDays = "0" + DataManager::getInstance()->searchCommonTexdtById(10150);
		strTime->strHMS = "00" + DataManager::getInstance()->searchCommonTexdtById(10151) + 
						  "00" + DataManager::getInstance()->searchCommonTexdtById(10152) + 
						  "00" + DataManager::getInstance()->searchCommonTexdtById(10153);

		return;
	}

	int days = secs / (24*3600);
	int hours = (secs % (24*3600)) / 3600;
	int mins = (secs - days*24*3600 - hours*3600) / 60;
	int sec = secs - days*24*3600 - hours*3600 - mins*60;

	string strHours = "";
	string strMins = "";
	string strSecs = "";

	if(hours <= 9)
	{
		strHours = _TO_STR(0) + _TO_STR(hours);
	}
	else
	{
		strHours = _TO_STR(hours);
	}
	
	if(mins <= 9)
	{
		strMins = _TO_STR(0) + _TO_STR(mins);
	}
	else
	{
		strMins = _TO_STR(mins);
	}
	
	if(sec <= 9)
	{
		strSecs = _TO_STR(0) + _TO_STR(sec);
	}
	else
	{
		strSecs = _TO_STR(sec);
	}

	strTime->strDays = TO_STR(days) + DataManager::getInstance()->searchCommonTexdtById(10150);
	strTime->strHMS = strHours + DataManager::getInstance()->searchCommonTexdtById(10151) + 
					  strMins + DataManager::getInstance()->searchCommonTexdtById(10152) + 
					  strSecs + DataManager::getInstance()->searchCommonTexdtById(10153);
}


void LimitTimeHeroLayer::onScrollViewEvent( Ref* pSender, ui::ScrollView::EventType type )
{
	if(pSender == m_ui.scrollRewardRank)
	{
		if (type == cocos2d::ui::ScrollView::EventType::SCROLLING)
		{
			Size size = m_ui.scrollRewardRank->getInnerContainerSize();
			Vec2 vec = m_ui.scrollRewardRank->getInnerContainer()->getPosition();

			//滚动条重设进度
				float percentY = -vec.y / (size.height - m_ui.scrollRewardRank->getContentSize().height);
				m_rewardBar->setScrollBarPercentY((1.0f - percentY)*100.0f);			
		}
	}
	else if(pSender == m_ui.scrollScoreRank)
	{
		if (type == cocos2d::ui::ScrollView::EventType::SCROLLING)
		{
			Size size = m_ui.scrollScoreRank->getInnerContainerSize();
			Vec2 vec = m_ui.scrollScoreRank->getInnerContainer()->getPosition();

			//滚动条重设进度
			float percentY = -vec.y / (size.height - m_ui.scrollScoreRank->getContentSize().height);
			m_scoreBar->setScrollBarPercentY((1.0f - percentY)*100.0f);			
		}
	}
}

void LimitTimeHeroLayer::updateSelf( void* data )
{
	ObserverParam* obParam = (ObserverParam*) data;
	if(!obParam)
	{
		return;
	}

	switch (obParam->id)
	{
	case nMAIN_CMD_LIMITTIMEHERO_INFO:
		{
			updateUI();
		}
		break;
	case nMAIN_CMD_LIMITTIMEHERO_CARD:
		{
			onTakeCardBoxEvent();
		}
		break;
	case LIMIT_TIME_HERO_ACT_SHOW_NO:
		{
			Director::getInstance()->replaceScene(MainCityScene::create());
		}
		break;
	case nMAIN_CMD_LIMITTIMEHERO_GETBOX:
		{
			updateRewardBoxState();
			showBoxRewardGetted();
		}
		break;
	default:
		break;
	}
}

void LimitTimeHeroLayer::openUITimer()
{
	sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
	if(actData != nullptr && actData->actState == eSTATE_OPEN)
	{
		//开启刷新UI定时器
		setEndTimeStamp(actData->actTime.actOverTime);
		LocalTimer::getInstance()->addLocalTimerListener(this);
	}
}

void LimitTimeHeroLayer::timeTick( int dt )
{
	sActData* actData = LimitTimeHeroModel::getInstance()->getActData();

	if(actData != nullptr && actData->actState == eSTATE_OPEN)
	{
		actData->actTime.actOverSec -= dt;

		if(actData->actTime.actOverSec <= 0)
		{
			actData->actState = eSTATE_SHOW;		//进入展示状态
			LimitTimeHeroModel::getInstance()->openTimer();
		}

		//通知活动UI，刷新倒计时
		showDownTime();
	}
}

void LimitTimeHeroLayer::showCanGetAni( Node* parentNode )
{
	if(parentNode->getParent()->getChildByName(parentNode->getName() + "_Ani") == nullptr)
	{
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniXingjijiangli/AniXingjijiangli.ExportJson");
		Armature* backAni = Armature::create("AniXingjijiangli");
		backAni->setPosition(parentNode->getPosition() + Point(6, 10));
		backAni->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		backAni->setScale(0.8f);
		backAni->setName(parentNode->getName() + "_Ani");
		parentNode->getParent()->addChild(backAni, parentNode->getLocalZOrder()-1);	
		backAni->getAnimation()->play("back");
	}
}

void LimitTimeHeroLayer::removeCanGetAni( Node* parentNode )
{
	parentNode->getParent()->removeChildByName(parentNode->getName() + "_Ani");
}










