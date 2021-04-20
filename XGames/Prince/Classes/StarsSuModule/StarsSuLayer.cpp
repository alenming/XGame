#include "StarsSuLayer.h"
#include "MainModule/MainCityScene.h"
#include "HeroModule/LayerHeroStarsUp.h"

string StarMaleAttributeNameImg[] = 
{
	"Image/Icon/global/img_xueliang.png",//血量
	"Image/Icon/global/img_wufang.png",//物防
	"Image/Icon/global/img_mofang.png",//法防
	"Image/Icon/global/img_gongji.png", //攻击
	"Image/Icon/global/img_shuxing_mingzhong.png",//命中
	"Image/Icon/global/img_shuxing_baoji.png",//暴击
	"Image/Icon/global/img_shuxing_baokang.png",//暴抗
	"Image/Icon/global/img_shuxing_shanbi.png",//闪避
	"Image/Icon/global/img_shuxing_lianji.png",//连击
	"Image/Icon/global/img_shuxing_baoshang.png",//暴伤
	"Image/Icon/global/img_shuxing_baokang.png",//暴免
};

string StarFeMaleAttributeNameImg[] = 
{
	"Image/Icon/global/img_xueliang.png",//血量
	"Image/Icon/global/img_wufang.png",//物防
	"Image/Icon/global/img_mofang.png",//法防
	"Image/Icon/global/img_fagong.png", //法攻
	"Image/Icon/global/img_shuxing_mingzhong.png",//命中
	"Image/Icon/global/img_shuxing_baoji.png",//暴击
	"Image/Icon/global/img_shuxing_baokang.png",//暴抗
	"Image/Icon/global/img_shuxing_shanbi.png",//闪避
	"Image/Icon/global/img_shuxing_lianji.png",//连击
	"Image/Icon/global/img_shuxing_baoshang.png",//暴伤
	"Image/Icon/global/img_shuxing_baokang.png",//暴免
};


string StarAttribute[] = 
{
	"hp",
	"pdd",
	"mdd",
	"atk",
	"hit",
	"criPrb",
	"criRes",
	"miss",
	"ctrPrb",
	"criCoe",
	"coeRes",
};
/*
string StarMaleAttributeName[] = 
{
	STR_UTF8("血量上限"),
	STR_UTF8("物理防御"),
	STR_UTF8("法术防御"),
	STR_UTF8("物理攻击"),
	STR_UTF8("命中几率"),
	STR_UTF8("暴击几率"),
	STR_UTF8("抗暴几率"),
	STR_UTF8("闪避几率"),
	STR_UTF8("连击几率"),
	STR_UTF8("暴击伤害"),
	STR_UTF8("暴伤免疫"),
};

string StarFeMalAttributeName[] = 
{
	STR_UTF8("血量上限"),
	STR_UTF8("物理防御"),
	STR_UTF8("法术防御"),
	STR_UTF8("法术攻击"),
	STR_UTF8("命中几率"),
	STR_UTF8("暴击几率"),
	STR_UTF8("抗暴几率"),
	STR_UTF8("闪避几率"),
	STR_UTF8("连击几率"),
	STR_UTF8("暴击伤害"),
	STR_UTF8("暴伤免疫"),
};
*/

const int StarAttributeCount = 11;

string StarsSuLayer::getStarAttributeName(int n)
{
	if (MainModel::getInstance()->getMainParam()->mSex == HERO_SEX_MALE)
	{
		return StarMaleAttributeName[n];
	}
	else
	{
		return StarFeMalAttributeName[n];
	}
}


StarsSuLayer::StarsSuLayer()
{
	m_StarsSuController = new StarsSuController();
	StarsSuModel::getInstance()->addObserver(this);
	HeroModel::getInstance()->addObserver(this);
	m_SendType = ASTROLOGY_TYPE::ASTROLOGY_TYPE_NONE;
}

StarsSuLayer::~StarsSuLayer()
{
	SAFE_DELETE_ELEMENT(m_StarsSuController);
	m_StarsSuController = nullptr;
	StarsSuModel::getInstance()->removeObserver(this);
	HeroModel::getInstance()->removeObserver(this);
}

StarsSuLayer* StarsSuLayer::create(bool pushScene /*= false*/)
{
	StarsSuLayer *pRet = new StarsSuLayer();
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

bool StarsSuLayer::init(bool pushScene)
{
	m_isPushScene = pushScene;
	if (!Layer::init())
	{
		return false;
	}
	//加载文本,读表
	string commText = DataManager::getInstance()->searchCommonTexdtById(20153);
	StarMaleAttributeName[0] = commText;
	StarFeMalAttributeName[0] = commText;

	commText.clear();
	commText = DataManager::getInstance()->searchCommonTexdtById(20154);
	StarMaleAttributeName[1] = commText;
	StarFeMalAttributeName[1] = commText;

	commText.clear();
	commText = DataManager::getInstance()->searchCommonTexdtById(20155);
	StarMaleAttributeName[2] = commText;
	StarFeMalAttributeName[2] = commText;

	commText.clear();
	commText = DataManager::getInstance()->searchCommonTexdtById(20156);
	StarMaleAttributeName[3] = commText;
	StarFeMalAttributeName[3] = commText;

	commText.clear();
	commText = DataManager::getInstance()->searchCommonTexdtById(20157);
	StarMaleAttributeName[4] = commText;
	StarFeMalAttributeName[4] = commText;

	commText.clear();
	commText = DataManager::getInstance()->searchCommonTexdtById(20158);
	StarMaleAttributeName[5] = commText;
	StarFeMalAttributeName[5] = commText;

	commText.clear();
	commText = DataManager::getInstance()->searchCommonTexdtById(20159);
	StarMaleAttributeName[6] = commText;
	StarFeMalAttributeName[6] = commText;

	commText.clear();
	commText = DataManager::getInstance()->searchCommonTexdtById(20160);
	StarMaleAttributeName[7] = commText;
	StarFeMalAttributeName[7] = commText;

	commText.clear();
	commText = DataManager::getInstance()->searchCommonTexdtById(20161);
	StarMaleAttributeName[8] = commText;
	StarFeMalAttributeName[8] = commText;

	commText.clear();
	commText = DataManager::getInstance()->searchCommonTexdtById(20162);
	StarMaleAttributeName[9] = commText;
	StarFeMalAttributeName[9] = commText;

	commText.clear();
	commText = DataManager::getInstance()->searchCommonTexdtById(20163);
	StarMaleAttributeName[10] = commText;
	StarFeMalAttributeName[10] = commText;


	initUi();
	return true;
}

void StarsSuLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	//添加星宿根节点
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UINewstar/UINewstar.ExportJson"));
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	this->addChild(m_ui.pRoot);

	if (MainModel::getInstance()->getMainParam()->mSex == HERO_SEX_MALE)
	{
		m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg1.jpg");
	}
	else
	{
		m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg2.jpg");
	}

	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
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
	
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_STAR_SU);
	this->addChild(_layerCommHeadInfo);

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anixingbeijing/Anixingbeijing.ExportJson");
	Armature* armature = Armature::create("Anixingbeijing");
	armature->getAnimation()->play("Animation1");
	Layout *cankao = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_cankao"));
	armature->setPosition(Vec2(cankao->getContentSize().width/2,cankao->getContentSize().height/2));
	cankao->addChild(armature);


	Layout* Panel_ = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Panel_"));
	Layout* Pnl_shuxing = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_shuxing"));
	Vec2 Pnl_shuxingPos = Pnl_shuxing->getPosition();
	Pnl_shuxing->setPosition(Vec2(Pnl_shuxingPos.x + Pnl_shuxing->getContentSize().width,Pnl_shuxingPos.y));

	auto moveCall = []()->void
	{
		GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_XINGXIU);
	};

	Pnl_shuxing->runAction(CCSequence::create(CCEaseSineOut::create(CCMoveTo::create(0.5f,Pnl_shuxingPos)),
		CCCallFunc::create(moveCall),nullptr));


	m_ui.pAstrology = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Button_zhanxing"));//占星
	m_ui.pAstrology->addTouchEventListener(CC_CALLBACK_2(StarsSuLayer::sendButtonMsg,this,
		ASTROLOGY_TYPE_BUTTON::ASTROLOGY_TYPE_BUTTON_ZHAN));
	m_ui.pOneKeyAstrology = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Button_Onekey"));//一键占星
	m_ui.pOneKeyAstrology->addTouchEventListener(CC_CALLBACK_2(StarsSuLayer::sendButtonMsg,this,
		ASTROLOGY_TYPE_BUTTON::ASTROLOGY_TYPE_BUTTON_ONE_KEY_ZHAN));
	m_ui.pBreakOut  = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Button_chongpo")); //突破
	m_ui.pBreakOut->addTouchEventListener(CC_CALLBACK_2(StarsSuLayer::sendButtonMsg,this,
		ASTROLOGY_TYPE_BUTTON::ASTROLOGY_TYPE_BUTTON_TUPO));
	m_ui.pBreakOutLv = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Lab_rank")); //几重天
	m_ui.freeCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"freeCount"));

	m_ui.starCountTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Lab_Tips")); //说明
	m_ui.starCountTips->setFontName(FONT_FZZHENGHEI);
	m_ui.pAttributeList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.pRoot,"ListView_64"));//属性列表
	ImageView* parent = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_di2"));

	for (int i = 0; i < HERO_STAR_COUNT; i++)
	{
		m_ui.pHeroStar[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(parent,"Img_Rank"+TO_STR(i+1)));
	}

	//滚动条
	m_CustomScrollBar = CustomScrollBar::create();
	m_CustomScrollBar->initScrollBar(m_ui.pAttributeList->getContentSize().height);
	m_CustomScrollBar->setPosition(m_ui.pAttributeList->getPosition() + Point(-12, 0));
	parent->addChild(m_CustomScrollBar);

	m_CustomScrollBar->setScrollBarLengthPercent(1.0f);
	m_CustomScrollBar->setScrollBarPercentY(0.0f);
	dynamic_cast<ScrollView*>(m_ui.pAttributeList)->addEventListener(CC_CALLBACK_2(StarsSuLayer::onScrollViewEvent, this));


	m_ui.pAstrologySpendImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Astrology_coin"));
	m_ui.pAstrologySpend = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Astrology_coin_count"));//占星花费
	m_ui.pBreakOutSpendImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_star"));
	m_ui.pBreakOutSpend = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"star_count"));//突破花费

	m_ui.pStarPanel.clear();
	for (int i = 0; i < STAR_TYPE_COUNT; i++)
	{
		Layout* starPanel = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_type"+TO_STR(i+1)));
		starPanel->setVisible(false);
		m_ui.pStarPanel.push_back(starPanel);
		for (int j = 0; j < STAR_SU_COUNT; j++)
		{
			ImageView* img = dynamic_cast<ImageView*>(Helper::seekWidgetByName(starPanel,"Img_star"+TO_STR(j+1)));
			
			img->addTouchEventListener(CC_CALLBACK_2(StarsSuLayer::lookStarInfo,this,j+1));
			m_ui.pStar[i].push_back(img);
		}
	}
	m_ui.pStatTips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"tips"));
	m_ui.pStatTips->setVisible(false);

	m_ui.pAstrologyBall = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"_Astrology_ball"));
	m_ui.pbreakOutBall = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"_breakOut_ball"));
	Text* Lab_chongpo = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pbreakOutBall,"Lab_chongpo"));
	Lab_chongpo->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Lab_chongpo,Color3B(0x7f,0x10,0x10),2);

	m_ui.pLoadingBar = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(m_ui.pRoot,"ProgressBar_11"));//占星进度
	m_ProgressTimer = ProgressTimer::create(Sprite::create("Image/Icon/global/img_tiao_zhanxing_1.png"));
	m_ProgressTimer->setType(ProgressTimer::Type::BAR);
	m_ProgressTimer->setPercentage(0);
	m_ProgressTimer->setMidpoint(ccp(0,0.5f));
	m_ProgressTimer->setBarChangeRate(ccp(1,0));

	m_ProgressTimer->setPosition(Vec2(m_ui.pLoadingBar->getContentSize().width/2,m_ui.pLoadingBar->getContentSize().height/2));
	m_ui.pLoadingBar->addChild(m_ProgressTimer);

	m_ui.pLoadingBarCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Astrology_percent"));
	m_ui.pAstrologyBallDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Astrology_ball_desc"));
	m_ui.pAstrologyBallDescCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Astrology_ball_desc_count"));
	m_ui.pbreakOutBallDesc1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"breakOut_ball_desc1"));
	m_ui.pbreakOutBallDesc1count = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"breakOut_ball_desc_1count"));
	m_ui.pbreakOutBallDesc2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"breakOut_ball_desc_2"));
	m_ui.pbreakOutBallDesc2count = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"breakOut_ball_desc_2count"));

	m_ui.Pbaoji = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"baoji"));
	m_ui.baojiPos = m_ui.Pbaoji->getPosition();

	//暂时屏蔽
	Text* tips1 = dynamic_cast<Text*>(m_ui.pRoot->getChildByName("Lab_1"));
	Text* tips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Lab_2"));
	Text* tips1_0 = dynamic_cast<Text*>(m_ui.pRoot->getChildByName("Lab_1_0"));
	Text* tips2_0 = dynamic_cast<Text*>(Panel_->getChildByName("Lab_2_0"));
	setTextAddStroke(tips1,Color3B(0x00,0x00,0x00),2);
	setTextAddStroke(tips2,Color3B(0x00,0x00,0x00),2);
	setTextAddStroke(tips1_0,Color3B(0x00,0x00,0x00),2);
	setTextAddStroke(tips2_0,Color3B(0x00,0x00,0x00),2);
	tips1->setFontName(FONT_FZZHENGHEI);
	tips2->setFontName(FONT_FZZHENGHEI);
	tips1_0->setFontName(FONT_FZZHENGHEI);
	tips2_0->setFontName(FONT_FZZHENGHEI);

	m_ui.pBreakOutLv->setFontName(FONT_FZZHENGHEI);
	m_ui.pAstrologySpend->setFontName(FONT_FZZHENGHEI);
	m_ui.pBreakOutSpend->setFontName(FONT_FZZHENGHEI);
	m_ui.pLoadingBarCount->setFontName(FONT_FZZHENGHEI);
	m_ui.pAstrologyBallDesc->setFontName(FONT_FZZHENGHEI);
	m_ui.pAstrologyBallDescCount->setFontName(FONT_FZZHENGHEI);
	m_ui.pbreakOutBallDesc1->setFontName(FONT_FZZHENGHEI);
	m_ui.pbreakOutBallDesc1count->setFontName(FONT_FZZHENGHEI);
	m_ui.pbreakOutBallDesc2->setFontName(FONT_FZZHENGHEI);
	m_ui.pbreakOutBallDesc2count->setFontName(FONT_FZZHENGHEI);
	m_ui.freeCount->setFontName(FONT_FZZHENGHEI);
}

void StarsSuLayer::onEnter()
{
	Layer::onEnter();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(StarsSuLayer::updateNextFram), this, 0, false);
}

void StarsSuLayer::updateNextFram(float ft)
{
	// 保证只拉取一次数据，省通信
	if (StarsSuModel::getInstance()->getStatSuInfoCount() <= 0)
	{
		m_StarsSuController->onGetStarsInfo();
	}
	else
	{
		upDateGetStarInfo(true);
	}
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(StarsSuLayer::updateNextFram), this);
}


void StarsSuLayer::onExit()
{
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UINewstar");
	ArmatureDataManager::destroyInstance();
	Layer::onExit();
}

void StarsSuLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	if (observerParam->self == HeroModel::getInstance())
	{
		auto _LayerHeroStarsUp = LayerHeroStarsUp::create(HeroModel::getInstance()->getMainHero()->mHeroId);
		_LayerHeroStarsUp->show(this);
	}
	else
	{
		switch (observerParam->id)
		{
		case STARSSU_EVENT_TYPE_GET_INFO: //拉取
			{
				upDateGetStarInfo(true);
			}
			break;
		case STARSSU_EVENT_TYPE_ASTROLOGY_BREAK_OUT:
			{
				if (m_SendType == ASTROLOGY_TYPE::ASTROLOGY_TYPE_BREAK_OUT)//突破
				{
					upDateGetStarInfo(false,true);
					animationBreakOut();
					animationChongpoOut();
					setAtrChangeAnimat();
					SoundUtils::playSoundEffect("yangcheng_1");
				}
				else //占星
				{
					upDateGetStarInfo(false,true);
					animationAstrology();
				}
			}
			break;
		default:
			break;
		}
	}
}

void StarsSuLayer::upDateGetStarInfo(bool isInit,bool isShowAnimi)
{
	//金币银币
	int starNum = MainModel::getInstance()->getMainParam()->mStarNum;
	//获取星宿信息
	const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
	RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId);
	//获取主角星级
	int starLevel = AstrologyData->getIntData("stars");
	for (int i= 0; i < HERO_STAR_COUNT; i++)
	{
		if (i < starLevel)
		{
			m_ui.pHeroStar[i]->loadTexture("Image/Icon/global/img_star.png");
		}
		else
		{
			m_ui.pHeroStar[i]->loadTexture("Image/Icon/global/img_star2.png");
		}	
	}
	//升星需要突破至N重天
	if (starLevel >= HERO_STAR_COUNT)
	{
		m_ui.starCountTips->setString(DataManager::getInstance()->searchCommonTexdtById(10235));
		setTextAddStroke(m_ui.starCountTips,Color3B(0x00,0x00,0x00),2);
	}
	else
	{
		int Nrank = 1;
		while (DataManager::getInstance()->searchStarById(Nrank))
		{
			RowData *data = DataManager::getInstance()->searchStarById(Nrank);
			if (data->getIntData("stars") > starLevel)
			{
				break;
			}
			Nrank++;
		}

		RowData *NrankData = DataManager::getInstance()->searchStarById(Nrank);
		m_ui.starCountTips->setString(DataManager::getInstance()->searchCommonTexdtById(20165) + NrankData->getStringData("name"));
		setTextAddStroke(m_ui.starCountTips,Color3B(0x00,0x00,0x00),2);
	}

	if (AstrologyData->getIntData("position") == 0)//表示可以突破
	{
		Text* tips1 = dynamic_cast<Text*>(m_ui.pRoot->getChildByName("Lab_1"));
		Text* tips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Lab_2"));
		Text* tips1_0 = dynamic_cast<Text*>(m_ui.pRoot->getChildByName("Lab_1_0"));
		Text* tips2_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Lab_2_0"));
		tips1->setVisible(true);
		tips2->setVisible(true);
		tips1_0->setVisible(false);
		tips2_0->setVisible(false);

		m_ui.pBreakOut->setVisible(true);
		m_ui.pbreakOutBall->setVisible(true);

		m_ui.pAstrology->setVisible(false);
		m_ui.pOneKeyAstrology->setVisible(false);
		m_ui.pAstrologyBall->setVisible(false);

		m_ui.pBreakOutSpendImg->setVisible(true);
		m_ui.freeCount->setVisible(false);
		m_ui.pAstrologySpendImg->setVisible(false);

		//RowData *breakDataNext = DataManager::getInstance()->searchStarRankById(starsInfo.bId);
		m_ui.pBreakOutLv->setString(AstrologyData->getStringData("name"));
		setTextAddStroke(m_ui.pBreakOutLv,Color3B(0x3f,0x22,0x6a),3);
		m_ui.pBreakOutSpend->setString(TO_STR(starNum)+"/"+TO_STR(AstrologyData->getIntData("needStar")));
		if (starNum < AstrologyData->getIntData("needStar"))
		{
			m_ui.pBreakOutSpend->setColor(Color3B(0xff,0x17,0x17));
		}
		else
		{
			m_ui.pBreakOutSpend->setColor(Color3B(0xff,0xff,0xff));
		}

		if (isShowAnimi)
		{
			animationChontOut();
			setAtrChangeAnimat();
		}
		setBreakOutPalyerDesc();

		m_ProgressTimer->stopAllActions();
		m_ProgressTimer->setPercentage(100);
	}
	else
	{
		Text* tips1 = dynamic_cast<Text*>(m_ui.pRoot->getChildByName("Lab_1"));
		Text* tips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Lab_2"));
		Text* tips1_0 = dynamic_cast<Text*>(m_ui.pRoot->getChildByName("Lab_1_0"));
		Text* tips2_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Lab_2_0"));
		tips1->setVisible(false);
		tips2->setVisible(false);
		tips1_0->setVisible(true);
		tips2_0->setVisible(true);

		m_ui.pBreakOut->setVisible(false);
		m_ui.pAstrology->setVisible(true);
		m_ui.pOneKeyAstrology->setVisible(true);
		m_ui.pbreakOutBall->setVisible(false);
		m_ui.pAstrologyBall->setVisible(true);

		if (starsInfo.freeCount > 0)//免费
		{
			m_ui.freeCount->setVisible(true);
			m_ui.pAstrologySpendImg->setVisible(false);
			m_ui.freeCount->setString(DataManager::getInstance()->searchCommonTexdtById(20166) + ": "+TO_STR(starsInfo.freeCount));
		}
		else
		{
			m_ui.freeCount->setVisible(false);
			m_ui.pAstrologySpendImg->setVisible(true);
			m_ui.pAstrologySpend->setString(TO_STR(starsInfo.coinCost));
			m_coinSpendCount = starsInfo.coinCost;
		}
		m_ui.pBreakOutSpendImg->setVisible(false);

		int last = starsInfo.lastPoint;
		int cur = starsInfo.curPoint;
		int max = AstrologyData->getIntData("needStar");
		m_ui.pLoadingBarCount->setString(TO_STR(cur)+"/"+TO_STR(max));
		setTextAddStroke(m_ui.pLoadingBarCount,Color3B(0x7f,0x10,0x10),3);

		if (!isInit)
		{
			int ceng = starsInfo.CursId - starsInfo.lastsId;
			if (ceng == 0)
			{
				ProgressFromTo *fromTo = ProgressFromTo::create((cur-last)*LoadTime/max,last*100/max,cur*100/max);
				m_ProgressTimer->runAction(fromTo);
			}
			else if (ceng == 1)
			{
				auto call = [this,isShowAnimi]()->void
				{
					//CCShake *shake = CCShake::create(0.2f,10);
					//m_ui.pRoot->stopAllActions();
					//m_ui.pRoot->runAction(shake);
					if (isShowAnimi)
					{
						animationChontOut();
						setAtrChangeAnimat();
					}
				};

				if (m_SendType == ASTROLOGY_TYPE::ASTROLOGY_TYPE_BREAK_OUT)//突破
				{
					ProgressFromTo *fromTo = ProgressFromTo::create(LoadTime*cur/max,0,cur*100/max);

					m_ProgressTimer->runAction(CCSequence::create(CCCallFunc::create(call),fromTo,nullptr));
				}
				else //占星
				{
					ProgressFromTo *fromTo1 = ProgressFromTo::create(LoadTime*(max-last)/max,last*100/max,100);
					ProgressFromTo *fromTo2 = ProgressFromTo::create(LoadTime*cur/max,0,cur*100/max);

					m_ProgressTimer->runAction(CCSequence::create(fromTo1,CCCallFunc::create(call),fromTo2,nullptr));
				}
			}
			else
			{
				auto call = [this,isShowAnimi]()->void
				{
					//CCShake *shake = CCShake::create(0.2f,10);
					//m_ui.pRoot->stopAllActions();
					//m_ui.pRoot->runAction(shake);
					if (isShowAnimi)
					{
						animationChontOut();
						setAtrChangeAnimat();
					}
				};

				Vector<FiniteTimeAction*> action;
				ProgressFromTo *fromTo1 = ProgressFromTo::create(LoadTime*(max-last)/max,last*100/max,100);
				action.pushBack(fromTo1);
				action.pushBack(CCCallFunc::create(call));
				for (int i = 0; i < ceng-1; i++)
				{
					ProgressFromTo *fromTo = ProgressFromTo::create(LoadTime,0,100);
					action.pushBack(fromTo);
					action.pushBack(CCCallFunc::create(call));
				}

				ProgressFromTo *fromTo2 = ProgressFromTo::create(LoadTime*cur/max,0,cur*100/max);
				action.pushBack(fromTo2);


				m_ProgressTimer->runAction(CCSequence::create(action));
			}
		}
		else
		{
			ProgressFromTo *fromTo = ProgressFromTo::create(cur*LoadTime/max,0,cur*100/max);
			m_ProgressTimer->runAction(fromTo);
		}
		
		//m_ui.pLoadingBar->setPercent(cur*100/max);
		m_ui.pBreakOutLv->setString(AstrologyData->getStringData("name"));
		setTextAddStroke(m_ui.pBreakOutLv,Color3B(0x3f,0x22,0x6a),3);

		setAstrologyPalyerDesc();
	}

	setStarType(isInit);
	addAttributeItem();
}

void StarsSuLayer::animationAstrology()
{
	const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();

	int ceng = starsInfo.CursId - starsInfo.lastsId;
	int count = 0;
	if (ceng == 0)
	{
		count = starsInfo.curPoint - starsInfo.lastPoint;
	}
	else if (ceng == 1)
	{
		RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId-1);

		count = AstrologyData->getIntData("needStar")-starsInfo.lastPoint + starsInfo.curPoint;
	}
	else
	{
		RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId-ceng);
		count = AstrologyData->getIntData("needStar")-starsInfo.lastPoint;

		for (int i = 0; i < ceng-1; i++)
		{
			RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId-(i+1));

			count = count + AstrologyData->getIntData("needStar");
		}

		count = count + starsInfo.curPoint;
	}

	Text* Pbaoji = Text::create();
	Pbaoji->setPosition(Vec2(40,0));
	m_ui.Pbaoji->addChild(Pbaoji);

	Pbaoji->setString("+"+TO_STR(count));
	Pbaoji->setFontName(FONT_FZZHENGHEI);
	Pbaoji->setFontSize(40);
	setTextAddStroke(Pbaoji,Color3B(0x7f,0x10,0x10),2);

	if (starsInfo.criPrb > 1)
	{
		Pbaoji->setScale(2.0f);
		CCMoveBy *moveBy = CCMoveBy::create(0.5f,Vec2(0,100));
		CCScaleTo *scaleTo1 = CCScaleTo::create(0.1f,3.0f);
		CCScaleTo *scaleTo2 = CCScaleTo::create(0.1f,2.0f);
		CCDelayTime *delay = CCDelayTime::create(0.3f);
		CCFadeOut *fadeOut = CCFadeOut::create(0.2f);
		auto call = [Pbaoji]()->void
		{
			Pbaoji->removeFromParent();
		};
		Pbaoji->runAction(CCSequence::create(CCEaseOut::create(moveBy,1),scaleTo1,scaleTo2,delay,
			fadeOut,CCCallFunc::create(call),nullptr));
	}
	else
	{
		Pbaoji->setScale(1.0f);
		CCMoveBy *moveBy = CCMoveBy::create(0.5f,Vec2(0,100));
		CCScaleTo *scaleTo1 = CCScaleTo::create(0.1f,1.5f);
		CCScaleTo *scaleTo2 = CCScaleTo::create(0.1f,1.0f);
		CCDelayTime *delay = CCDelayTime::create(0.3f);
		CCFadeOut *fadeOut = CCFadeOut::create(0.2f);
		auto call = [Pbaoji]()->void
		{
			Pbaoji->removeFromParent();
		};
		Pbaoji->runAction(CCSequence::create(CCEaseOut::create(moveBy,1),scaleTo1,scaleTo2,delay,
			fadeOut,CCCallFunc::create(call),nullptr));
	}
}

void StarsSuLayer::animationBreakOut()
{
	string aniName1 = "Anidown";
	string aniName2 = "Anicircle";

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anitupo1/Anitupo1.ExportJson");
	Armature* armature = Armature::create("Anitupo1");

	auto onResultAnimationCallFunc = [this](Armature *armature, MovementEventType eventType, const std::string& strName)->void
	{
		if (eventType == MovementEventType::COMPLETE)
		{
			armature->removeFromParent();
		}
	};
	armature->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
	armature->setScale(0.8f);
	armature->getAnimation()->play(aniName1);

	Bone*  bone = armature->getBone("TIHUAN_TEXT2");
	Bone*  bone1 = armature->getBone("TIHUAN_TEXT");
	if(bone && bone1)
	{
		Skin* _newSkin = Skin::create("Image/Icon/ActTip/img_tupochenggong.png");
		bone->addDisplay(_newSkin,0);
		Skin* _newSkin1 = Skin::create("Image/Icon/ActTip/img_tupochenggong.png");
		bone1->addDisplay(_newSkin1, 0);
	}

	armature->setPosition(Vec2(m_ui.pRoot->getChildByName("Pnl_star")->getContentSize().width/2+100,0));
	m_ui.pRoot->getChildByName("Pnl_star")->addChild(armature);
}

void StarsSuLayer::animationChongpoOut()
{
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anidianliang/Anidianliang.ExportJson");
	Armature* armature = Armature::create("Anidianliang");

	auto onResultAnimationCallFunc = [this](Armature *armature, MovementEventType eventType, const std::string& strName)->void
	{
		if (eventType == MovementEventType::COMPLETE)
		{
			armature->removeFromParent();
		}
	};
	armature->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
	armature->getAnimation()->play("Animation1",-1,0);
	armature->setPosition(Vec2(m_ui.pRoot->getChildByName("Pnl_jindu")->getContentSize().width/2 - 46,18));
	m_ui.pRoot->getChildByName("Pnl_jindu")->addChild(armature,2);
}

void StarsSuLayer::animationChontOut()
{
	const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
	RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId-1);

	int type = AstrologyData->getIntData("resType");
	if (AstrologyData->getIntData("position") == 0)
	{
		return;
	}
	ImageView *star = m_ui.pStar[type-1][AstrologyData->getIntData("position")-1];

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anichongpo/Anichongpo.ExportJson");
	Armature* armature = Armature::create("Anichongpo");

	auto onResultAnimationCallFunc = [this](Armature *armature, MovementEventType eventType, const std::string& strName)->void
	{
		if (eventType == MovementEventType::COMPLETE)
		{
			armature->removeFromParent();
		}
	};
	armature->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
	armature->getAnimation()->play("Animation1",-1,0);
	armature->setPosition(Vec2(star->getContentSize().width/2,star->getContentSize().height/2));
	star->addChild(armature);
}

void StarsSuLayer::sendButtonMsg(Ref* sender,Widget::TouchEventType type,ASTROLOGY_TYPE_BUTTON kind)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (kind)
		{
		case ASTROLOGY_TYPE_BUTTON::ASTROLOGY_TYPE_BUTTON_ZHAN:
			{
				const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
				if (starsInfo.freeCount > 0)//免费
				{
					m_StarsSuController->onAstrologyAndBreakOut(ASTROLOGY_TYPE::ASTROLOGY_TYPE_FREE);
					m_SendType = ASTROLOGY_TYPE::ASTROLOGY_TYPE_FREE;
				}
				else
				{
					MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
					if (pMainParam->mCoin < m_coinSpendCount)
					{
						LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
						Director::getInstance()->getRunningScene()->addChild(buyEnergy);
					}
					else
					{
						m_StarsSuController->onAstrologyAndBreakOut(ASTROLOGY_TYPE::ASTROLOGY_TYPE_COIN);
						m_SendType = ASTROLOGY_TYPE::ASTROLOGY_TYPE_COIN;
					}
				}
			}
			break;
		case ASTROLOGY_TYPE_BUTTON::ASTROLOGY_TYPE_BUTTON_ONE_KEY_ZHAN:
			{
				//获取星宿信息
				const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
				RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId);
				int cur = starsInfo.curPoint;
				int max = AstrologyData->getIntData("needStar");
				int starPoint = AstrologyData->getIntData("starPoint");
				int spend = starsInfo.coinCost*(max - cur)/starPoint;

				auto enter = [this,spend](Ref* ref, CustomRetType type)->void
				{
					if(type == CustomRetType::RET_TYPE_OK)
					{
						MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
						if (pMainParam->mCoin < spend)
						{
							LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
							Director::getInstance()->getRunningScene()->addChild(buyEnergy);
						}
						else
						{
							m_StarsSuController->onAstrologyAndBreakOut(ASTROLOGY_TYPE::ASTROLOGY_TYPE_ONE_KEY);
							m_SendType = ASTROLOGY_TYPE::ASTROLOGY_TYPE_ONE_KEY;
						}
					}
				};

				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20167)
					+ TO_STR(spend) +
					DataManager::getInstance()->searchCommonTexdtById(20168),enter,CUSTOM_YES_NO);
			}
			break;
		case ASTROLOGY_TYPE_BUTTON::ASTROLOGY_TYPE_BUTTON_TUPO:
			{
				const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
				RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId);
				if (!AstrologyData)
				{
					return;
				}
				if (MainModel::getInstance()->getMainParam()->mStarNum < AstrologyData->getIntData("needStar"))
				{
					auto enterCancle = [](Ref* ref, CustomRetType type)->void
					{
						if(type == CustomRetType::RET_TYPE_OK)
						{
							if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE))
							{
								//此处需清空场景缓存
								Director::getInstance()->popToRootScene();
								Director::getInstance()->replaceScene(SceneChapterLoading::create());
							}
						}
					};

					CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20169),enterCancle,CUSTOM_YES_NO);
				}
				else
				{
					m_StarsSuController->onAstrologyAndBreakOut(ASTROLOGY_TYPE::ASTROLOGY_TYPE_BREAK_OUT);
					m_SendType = ASTROLOGY_TYPE::ASTROLOGY_TYPE_BREAK_OUT;
				}
			}
			break;
		default:
			break;
		}
	}
}

void StarsSuLayer::lookStarInfo(Ref* sender,Widget::TouchEventType type,int idx)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		{
			SoundUtils::playSoundEffect("dianji");
		
			m_ui.pStatTips->setVisible(true);
			Text *tex1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pStatTips,"name"));
			Text *tex2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pStatTips,"count"));

			tex1->setFontName(FONT_FZZHENGHEI);
			tex2->setFontName(FONT_FZZHENGHEI);

			const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
			RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId);	
			int rank = AstrologyData->getIntData("rank");

			string str1 = "";
			string str2 = "";
			
			int j = 1;
			while (DataManager::getInstance()->searchStarById(j))
			{
				RowData *data = DataManager::getInstance()->searchStarById(j);
				if (data->getIntData("rank") == rank && data->getIntData("position") == idx)
				{
					RowData *_data = DataManager::getInstance()->searchStarById(j-1);

					for (int k = 0; k < StarAttributeCount; k++  )
					{
						float atrribute = 0.0f;
						if (_data)
						{
							atrribute = data->getFloatData(StarAttribute[k]) - _data->getFloatData(StarAttribute[k]);
						}
						else
						{
							atrribute = data->getFloatData(StarAttribute[k]);
						}
						if (atrribute > 0)
						{
							str1 = getStarAttributeName(k);
							if (atrribute >= 1.0f)
							{
								str2 = "+" + _TO_STR(atrribute);
							}
							else
							{
								float _AttributeCount = atrribute*100;
								str2 = "+" + _F_TO_STR(_AttributeCount)+"%";
							}
							break;
						}
					}
				}

				if (str1 != "")
				{
					break;
				}
				j++;
			}

			m_ui.pStatTips->setPosition(m_ui.pStar[AstrologyData->getIntData("resType")-1][idx-1]->getPosition());

			tex1->setString(str1);
			tex2->setString(str2);
			m_ui.pStatTips->setContentSize(Size(tex1->getCustomSize().width+tex2->getCustomSize().width+18*2+9,
				m_ui.pStatTips->getContentSize().height));

			tex2->setPosition(Vec2(tex1->getPosition().x+tex1->getContentSize().width+9,tex2->getPosition().y));
			setTextAddStroke(tex1,Color3B(0x10,0x13,0x77),2);
			setTextAddStroke(tex2,Color3B(0x10,0x13,0x77),2);
		}
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		{
			m_ui.pStatTips->setVisible(false);
		}
		break;
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		{
			m_ui.pStatTips->setVisible(false);
		}
		break;
	default:
		break;
	}
}


void StarsSuLayer::setAstrologyPalyerDesc()
{
	const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
	RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId);
	RowData *_AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId-1);

	string str1 = "";
	string str2 = "";

	for (int k = 0; k < StarAttributeCount; k++  )
	{
		float atrribute = 0.0f;
		if (_AstrologyData)
		{
			atrribute = AstrologyData->getFloatData(StarAttribute[k]) - _AstrologyData->getFloatData(StarAttribute[k]);
		}
		else
		{
			atrribute = AstrologyData->getFloatData(StarAttribute[k]);
		}

		if (atrribute > 0)
		{
			if (str1 == "")
			{
				str1 = getStarAttributeName(k);
				if (atrribute >= 1.0f)
				{
					str2 = "+" + _TO_STR(atrribute);
				}
				else
				{
					float _AttributeCount = atrribute*100;
					str2 = "+" + _F_TO_STR(_AttributeCount)+"%";
				}
			}
			break;
		}
	}

	m_ui.pAstrologyBallDesc->setString(str1);
	m_ui.pAstrologyBallDescCount->setString(str2);
	m_ui.pAstrologyBallDescCount->setPosition(Vec2(m_ui.pAstrologyBallDesc->getPosition().x+m_ui.pAstrologyBallDesc->getContentSize().width+10,
		m_ui.pAstrologyBallDescCount->getPosition().y));

	setTextAddStroke(m_ui.pAstrologyBallDesc,Color3B(0x10,0x13,0x77),2);
	setTextAddStroke(m_ui.pAstrologyBallDescCount,Color3B(0x10,0x13,0x77),2);
}

void StarsSuLayer::setBreakOutPalyerDesc()
{
	const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
	RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId);
	RowData *_AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId-1);
	string str1 = "";
	string str2 = "";
	string str3 = "";
	string str4 = "";

	for (int k = 0; k < StarAttributeCount; k++  )
	{
		float atrribute = 0.0f;
		if (_AstrologyData)
		{
			atrribute = AstrologyData->getFloatData(StarAttribute[k]) - _AstrologyData->getFloatData(StarAttribute[k]);
		}
		else
		{
			atrribute = AstrologyData->getFloatData(StarAttribute[k]);
		}

		if (atrribute > 0)
		{
			if (str1 == "")
			{
				str1 = getStarAttributeName(k);
				if (atrribute >= 1.0f)
				{
					str2 = "+" + _TO_STR(atrribute);
				}
				else
				{
					float _AttributeCount = atrribute*100;
					str2 = "+" + _F_TO_STR(_AttributeCount)+"%";
				}
			}
			else if (str3 == "")
			{
				str3 = getStarAttributeName(k);
				if (atrribute >= 1.0f)
				{
					str4 = "+" + _TO_STR(atrribute);
				}
				else
				{
					float _AttributeCount = atrribute*100;
					str4 = "+" + _F_TO_STR(_AttributeCount)+"%";
				}
			}
		}
	}

	m_ui.pbreakOutBallDesc1->setString(str1);
	m_ui.pbreakOutBallDesc1count->setString(str2);
	m_ui.pbreakOutBallDesc1count->setPosition(Vec2(m_ui.pbreakOutBallDesc1->getPosition().x+m_ui.pbreakOutBallDesc1->getContentSize().width+10,
		m_ui.pbreakOutBallDesc1count->getPosition().y));

	m_ui.pbreakOutBallDesc2->setString(str3);
	m_ui.pbreakOutBallDesc2->setPosition(Vec2(m_ui.pbreakOutBallDesc1count->getPosition().x+m_ui.pbreakOutBallDesc1count->getContentSize().width+10,
		m_ui.pbreakOutBallDesc2->getPosition().y));

	m_ui.pbreakOutBallDesc2count->setString(str4);
	m_ui.pbreakOutBallDesc2count->setPosition(Vec2(m_ui.pbreakOutBallDesc2->getPosition().x+m_ui.pbreakOutBallDesc2->getContentSize().width+10,
		m_ui.pbreakOutBallDesc2count->getPosition().y));

	setTextAddStroke(m_ui.pbreakOutBallDesc1,Color3B(0x10,0x13,0x77),2);
	setTextAddStroke(m_ui.pbreakOutBallDesc1count,Color3B(0x10,0x13,0x77),2);
	setTextAddStroke(m_ui.pbreakOutBallDesc2,Color3B(0x10,0x13,0x77),2);
	setTextAddStroke(m_ui.pbreakOutBallDesc2count,Color3B(0x10,0x13,0x77),2);
}

void StarsSuLayer::setAtrChangeAnimat()
{
	const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
	RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId-1);
	RowData *_AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId-2);

	string str1 = "";
	string str2 = "";
	int idx1 = -1;
	int idx2 = -1;

	for (int k = 0; k < StarAttributeCount; k++  )
	{
		float atrribute = 0.0f;
		if (_AstrologyData)
		{
			atrribute = AstrologyData->getFloatData(StarAttribute[k]) - _AstrologyData->getFloatData(StarAttribute[k]);
		}
		else
		{
			atrribute = AstrologyData->getFloatData(StarAttribute[k]);
		}

		if (atrribute > 0)
		{
			if (idx1 == -1)
			{
				idx1 = k;
				if (atrribute >= 1.0f)
				{
					str1 = "+" + _TO_STR(atrribute);
				}
				else
				{
					float _AttributeCount = atrribute*100;
					str1 = "+" + _F_TO_STR(_AttributeCount)+"%";
				}
			}
			else if (idx2 == -1)
			{
				idx2 = k;
				if (atrribute >= 1.0f)
				{
					str2 = "+" + _TO_STR(atrribute);
				}
				else
				{
					float _AttributeCount = atrribute*100;
					str2 = "+" + _F_TO_STR(_AttributeCount)+"%";
				}
			}
		}
		else
		{
			if (idx1 > -1 && idx2 > -1)
			{
				break;
			}
		}
	}

	Layout *parent = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_cankao"));
	if (idx1 > -1)
	{
		string strFileName;
		strFileName.assign("Image/UINewstar/UINewstarAtrrChange.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		ImageView* img = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_gongji_0"));
		if (MainModel::getInstance()->getMainParam()->mSex == HERO_SEX_MALE)
		{
			img->loadTexture(StarMaleAttributeNameImg[idx1]);
		}
		else
		{
			img->loadTexture(StarFeMaleAttributeNameImg[idx1]);
		}

		Text* name = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_gongji"));
		name->setFontName(FONT_FZZHENGHEI);
		name->setString(getStarAttributeName(idx1));
		setTextAddStroke(name,Color3B(0x10,0x13,0x77),2);

		Text* count = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_shuzhi1"));
		count->setFontName(FONT_FZZHENGHEI);
		count->setString(str1);
		setTextAddStroke(count,Color3B(0x10,0x13,0x77),2);

		auto call = [layer]()->void
		{
			layer->removeFromParent();
		};

		auto _call = [=]()->void
		{
			if (idx2 > -1)
			{
				string strFileName;
				strFileName.assign("Image/UINewstar/UINewstarAtrrChange.ExportJson");
				Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

				ImageView* img = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_gongji_0"));
				if (MainModel::getInstance()->getMainParam()->mSex == HERO_SEX_MALE)
				{
					img->loadTexture(StarMaleAttributeNameImg[idx2]);
				}
				else
				{
					img->loadTexture(StarFeMaleAttributeNameImg[idx2]);
				}

				Text* name = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_gongji"));
				name->setFontName(FONT_FZZHENGHEI);
				name->setString(getStarAttributeName(idx2));
				setTextAddStroke(name,Color3B(0x10,0x13,0x77),2);

				Text* count = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_shuzhi1"));
				count->setFontName(FONT_FZZHENGHEI);
				count->setString(str2);
				setTextAddStroke(count,Color3B(0x10,0x13,0x77),2);

				auto call = [layer]()->void
				{
					layer->removeFromParent();
				};
				name->runAction(CCSequence::create(EaseSineOut::create(Spawn::create(ScaleTo::create(0.2, 1.5f),
					MoveBy::create(0.2, Vec2(0.0f, 21.0f)), nullptr)),
					EaseSineIn::create(Spawn::create(ScaleTo::create(0.2, 1.0f),
					MoveBy::create(0.2, Vec2(0.0f, 5.0f)), nullptr)),
					MoveBy::create(0.8, Vec2(0.0f, 102.0f)),
					Spawn::create(FadeOut::create(0.2), MoveBy::create(0.2, Vec2(0.0f, 22.0f)), nullptr),
					CCCallFunc::create(call),nullptr));

				layer->setPosition(Vec2(-100,-60));
				parent->addChild(layer);
			}
		};

		name->runAction(CCSequence::create(EaseSineOut::create(Spawn::create(ScaleTo::create(0.2, 1.5f),
			MoveBy::create(0.2, Vec2(0.0f, 21.0f)), nullptr)),
			EaseSineIn::create(Spawn::create(ScaleTo::create(0.2, 1.0f),
			MoveBy::create(0.2, Vec2(0.0f, 5.0f)), nullptr)),
			CCCallFunc::create(_call),
			MoveBy::create(0.8, Vec2(0.0f, 102.0f)),
			Spawn::create(FadeOut::create(0.2), MoveBy::create(0.2, Vec2(0.0f, 22.0f)), nullptr),
			CCCallFunc::create(call),nullptr));

		layer->setPosition(Vec2(-100,-70));
		parent->addChild(layer);
	}
}

void StarsSuLayer::setStarType(bool isInit)
{
	const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
	RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId);
	
	int type = AstrologyData->getIntData("resType");
	for (int i = 0; i < STAR_TYPE_COUNT; i++)
	{
		m_ui.pStarPanel[i]->setVisible(false);
	}
	m_ui.pStarPanel[type-1]->setVisible(true);


	if (isInit)
	{
		for (int i = 1; i <= StarsSuCont; i++)
		{
			m_ui.pStar[type-1][i-1]->setOpacity(0);
		}
		for (int i = 1; i <= StarsSuCont; i++)
		{
			m_ui.pStar[type-1][i-1]->runAction(CCFadeIn::create(0.5f));
		}
	}

	if (AstrologyData->getIntData("position") == 0)//升阶
	{
		for (int i = 1; i <= StarsSuCont; i++)
		{
			m_ui.pStar[type-1][i-1]->loadTexture("Image/Icon/global/img_StarSu1.png");
		}
	}
	else
	{
		int idx = AstrologyData->getIntData("position") % StarsSuCont;
		if (idx != 0)
		{
			for (int i = 1; i <= StarsSuCont; i++)
			{
				if (i < idx)
				{
					m_ui.pStar[type-1][i-1]->loadTexture("Image/Icon/global/img_StarSu1.png");
				}
				else if (i == idx)
				{
					m_ui.pStar[type-1][i-1]->loadTexture("Image/Icon/global/img_StarSu2.png");
				}
				else
				{
					m_ui.pStar[type-1][i-1]->loadTexture("Image/Icon/global/img_StarSu3.png");
				}
			}
		}
		else
		{
			for (int i = 1; i <= StarsSuCont; i++)
			{
				if (i < StarsSuCont)
				{
					m_ui.pStar[type-1][i-1]->loadTexture("Image/Icon/global/img_StarSu1.png");
				}
				else
				{
					m_ui.pStar[type-1][i-1]->loadTexture("Image/Icon/global/img_StarSu2.png");
				}
			}
		}
	}
}

void StarsSuLayer::addAttributeItem()
{
	m_ui.pAttributeList->removeAllItems();
	for (int i = 0; i < StarAttributeCount; i++)
	{
		string strFileName;
		strFileName.assign("Image/UINewstar/UINewstar_3.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		ImageView* img = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_gongji_0"));
		if (MainModel::getInstance()->getMainParam()->mSex == HERO_SEX_MALE)
		{
			img->loadTexture(StarMaleAttributeNameImg[i]);
		}
		else
		{
			img->loadTexture(StarFeMaleAttributeNameImg[i]);
		}

		Text* name = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_gongji"));
		name->setFontName(FONT_FZZHENGHEI);
		name->setString(getStarAttributeName(i));

		Text* count = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_shuzhi1"));
		count->setFontName(FONT_FZZHENGHEI);
		count->setString("+" + getAttributeData(i));

		m_ui.pAttributeList->pushBackCustomItem(layer);
	}

	int scrollViewHeight = StarAttributeCount * 40+10;
	int scrollViewWidth = m_ui.pAttributeList->getInnerContainerSize().width;
	m_ui.pAttributeList->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	m_CustomScrollBar->setScrollBarLengthPercent(
		m_ui.pAttributeList->getContentSize().height / m_ui.pAttributeList->getInnerContainerSize().height);
	m_CustomScrollBar->setScrollBarPercentY(0.0f);
}


void StarsSuLayer::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	switch(type)
	{

	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			Size size = m_ui.pAttributeList->getInnerContainerSize();
			Vec2 vec = m_ui.pAttributeList->getInnerContainer()->getPosition();
			float percentY = -vec.y / (size.height - m_ui.pAttributeList->getContentSize().height);

			m_CustomScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
		}

		break;
	default:
		break;
	}
}

string StarsSuLayer::getAttributeData(int id)
{
	const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
	string str = "";
	RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId - 1);
	if (AstrologyData)
	{
		float AttributeCount = AstrologyData->getFloatData(StarAttribute[id]);
		if (AttributeCount >= 1.0f)
		{
			str = _TO_STR(AttributeCount);
		}
		else
		{
			float _AttributeCount = AttributeCount*100;
			str = _F_TO_STR(_AttributeCount)+"%";
		}
	}
	else
	{
		str = "0";
	}
	
	return str;
}
