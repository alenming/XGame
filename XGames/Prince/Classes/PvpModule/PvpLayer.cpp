#include "PvpLayer.h"
#include "Widget/ActionCreator.h"
#include "Widget/LayerGameRules.h"
#include "NewShopModule/ShopScene.h"
#include "PvpAddLayer.h"

const int pvpSpendEnegy = 2;

PvpLayer::PvpLayer()
{
	PvpModel::getInstance()->addObserver(this);
	HeroModel::getInstance()->addObserver(this);

	m_RankPalyerBar = nullptr;
	m_RankPrizeListBar = nullptr;
	m_VictoryRewardListBar = nullptr;
	m_RankingListBar = nullptr;

	m_rewardInx = -1;
	m_isAddScrollViewEvent = false;
	m_isSetFightEnd = false;
}

PvpLayer::~PvpLayer()
{
	PvpModel::getInstance()->removeObserver(this);
	HeroModel::getInstance()->removeObserver(this);
}

PvpLayer* PvpLayer::create(bool pushScene)
{
	PvpLayer *pRet = new PvpLayer();
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

bool PvpLayer::init(bool pushScene)
{
	m_isPushScene = pushScene;
	if (!Layer::init())
	{
		return false;
	}

	if (PvpModel::getInstance()->getWin())
	{
		PvpModel::getInstance()->setWin(false);

		m_isSetFightEnd = true;
	}
	else
	{
		m_isSetFightEnd = false;
	}

	PvpController::getInstance()->getPvpInfo();
	initUi();
	addHeroAnimit();
	return true;
}

void PvpLayer::addHeroAnimit()
{
	Vector<HeroParam*>* HeroParamVec =  HeroModel::getInstance()->getHeroList();
	HeroParam* mHeroParam = HeroParamVec->at(0);
	for (int i = 0; i < HeroParamVec->size(); i++)
	{
		if (HeroParamVec->at(i)->isMainHero())
		{
			mHeroParam = HeroParamVec->at(i);
			break;
		}
	}

	int nResID = mHeroParam->mResId;
	SpineAnimation* pSkelFighter = SpineAnimation::createFighterWithResId(nResID);
	pSkelFighter->setPosition(Vec2(m_ui.mineHero->getContentSize().width/2, 28));
	pSkelFighter->setAnimation(0, "wait", true);
	m_ui.mineHero->addChild(pSkelFighter, 1);

	string frontName = "";
	string backName = "";
	switch (mHeroParam->mQuality)
	{
	case TOOL_ITEM_QUA_WHITE:
		backName = "white_back";
		break;
	case TOOL_ITEM_QUA_GREEN:
		backName = "green_back";
		break;
	case TOOL_ITEM_QUA_BLUE:
		backName = "blue_back";
		frontName = "blue_front";
		break;
	case TOOL_ITEM_QUA_PURPLE:
		backName = "purple_back";
		frontName = "purple_front";
		break;
	case TOOL_ITEM_QUA_ORANGE:
		backName = "orange_back";
		frontName = "orange_front";
		break;
	case TOOL_ITEM_QUA_RED:
		backName = "red_back";
		frontName = "red_front";
		break;
	default:
	break;
	}

	if (!frontName.empty())
	{
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniCharaparticle/AniCharaparticle.ExportJson");
		Armature* frontAni = Armature::create("AniCharaparticle");
		frontAni->setPosition(Point(m_ui.mineHero->getContentSize().width/2, 28));
		frontAni->getAnimation()->play(frontName);
		m_ui.mineHero->addChild(frontAni, 2);
	}

	if (!backName.empty())
	{
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniCharaparticle/AniCharaparticle.ExportJson");
		Armature* backAni = Armature::create("AniCharaparticle");
		backAni->setPosition(Point(m_ui.mineHero->getContentSize().width/2, 28));
		backAni->getAnimation()->play(backName);
		m_ui.mineHero->addChild(backAni, 0);
	}
}

void PvpLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIPvp/UIPvp.ExportJson"));
	this->addChild(m_ui.pRoot);	
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");

	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
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
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_PVP);
	m_ui.pRoot->getChildByName("Panel_13")->addChild(_layerCommHeadInfo);


	//挑战的玩家
	m_ui.panelList = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Panel_List"));
	m_ui.pvpPlayer = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Image_dadi"));
	string strFileName;
	strFileName.assign("Image/UIPvp/UIPvp_HeroList.ExportJson");
	m_ui.panelListNode = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.panelListNode->setVisible(false);
	this->addChild(m_ui.panelListNode);

	m_ui.playerListDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pvpPlayer,"Image_dadi_1"));
	m_ui.tableView = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pvpPlayer,"tableView"));
	
	m_ui.PvpCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pvpPlayer,"Label_energy_1"));  //精力消耗
	m_ui.PvpCost->setFontName(FONT_FZZHENGHEI);
	m_ui.pBtnhelp = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pvpPlayer,"Button_instructions"));    //显示帮助
	m_ui.pBtnhelp->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));

	m_ui.pBtAddPvpvCount = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pvpPlayer,"Button_zengjia"));    //pvp次数
	m_ui.pBtAddPvpvCount->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));

	//角色信息
	m_ui.heroInfoPanle = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Panel_juese"));
	m_ui.imgNamePanel = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Image_name_di"));
	m_ui.rankImgOder = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.imgNamePanel,"Image_rank_1"));   //排名
	m_ui.rankOrder = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.imgNamePanel,"Label_rank_1"));   //排名
	m_ui.mineName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.imgNamePanel,"Label_name"));  //名字
	m_ui.mineHero = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Panel_kapai")); //英雄
	m_ui.mineFight = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Label_force_2")); //战斗力
	m_ui.Image_force_di = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Image_force_di"));
	m_ui.Image_force_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Image_force_1"));
	m_ui.Label_force_2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Label_force_2"));
	m_ui.texTodayRank = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Label_reward")); //
	m_ui.Image_reward_di = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Image_reward_di")); //
	m_ui.todayHonorCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Label_honor"));  //
	m_ui.pBtnRankPrize = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Button_details"));
	m_ui.pBtnAdjustment = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Button_formation")); //调整整形
	m_ui.pBtnWinPrize = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Button_reward"));
	m_ui.pBtnHonorShop = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Button_shop"));
	m_ui.pBtnRankList = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.heroInfoPanle,"Button_rank"));
	m_ui.Image_reward_di->setVisible(false);
	m_ui.imgNamePanel->setVisible(false);
	m_ui.Image_force_di->setVisible(false);
	m_ui.Image_force_1->setVisible(false);
	m_ui.Label_force_2->setVisible(false);
	m_ui.rankOrder->setFontName(FONT_FZZHENGHEI);
	m_ui.mineName->setFontName(FONT_FZZHENGHEI);
	m_ui.mineFight->setFontName(FONT_FZZHENGHEI);
	m_ui.texTodayRank->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.texTodayRank,Color3B(0x8e,0x00,0x00),2);
	m_ui.todayHonorCount->setFontName(FONT_FZZHENGHEI);
	m_ui.pBtnRankPrize->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));
	m_ui.pBtnAdjustment->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));
	m_ui.pBtnWinPrize->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));
	m_ui.pBtnHonorShop->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));
	m_ui.pBtnRankList->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));

	//排行榜奖励
	m_ui.Panel_RankReward = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Panel_RankReward"));
	m_ui.RankReward = nullptr;

	//战斗
	m_ui.fightPanel = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Panel_fight"));
	m_ui.fightDesc = nullptr;

	//胜场奖励
	m_ui.Panel_VictoryReward = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Panel_VictoryReward"));
	m_ui.VictoryReward = nullptr;

	//排行榜
	m_ui.Panel_Ranking = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Panel_RankingList"));
	m_ui.Ranking = nullptr;

	m_ui.pvpPlayer->setVisible(true);
	m_ui.heroInfoPanle->setVisible(true);
	m_ui.Panel_RankReward->setVisible(false);
	m_ui.fightPanel->setVisible(false);
	m_ui.Panel_VictoryReward->setVisible(false);
	m_ui.Panel_Ranking->setVisible(false);

	//UI动画
	Vec2 pvpPlayergPos = m_ui.pvpPlayer->getPosition();
	m_ui.pvpPlayer->setPosition(Vec2(m_ui.panelList->getContentSize().width + m_ui.pvpPlayer->getContentSize().width/2,pvpPlayergPos.y));

	auto call = [this]()->void
	{
		GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ARENA);

		if (m_isSetFightEnd)
		{
			m_isSetFightEnd = false;
			setFightEnd();
		}
	};
	m_ui.pvpPlayer->runAction(Sequence::create(EaseSineOut::create(CCMoveTo::create(0.3f,pvpPlayergPos)),
		CCCallFunc::create(call),nullptr));

	ActionCreator::runCommBtnAction(m_ui.pBtnAdjustment);
	ActionCreator::runCommBtnAction(m_ui.pBtnWinPrize);
	ActionCreator::runCommBtnAction(m_ui.pBtnHonorShop);
	ActionCreator::runCommBtnAction(m_ui.pBtnRankList);
}

void PvpLayer::initPrize()
{
	if (!m_ui.RankReward)
	{
		m_ui.RankReward = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Panel_RankReward,"RankReward"));
		m_ui.RankPrizeList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.RankReward,"ListView_RewardList"));
		//滚动条
		m_RankPrizeListBar = CustomScrollBar::create();
		m_RankPrizeListBar->initScrollBar(m_ui.RankPrizeList->getContentSize().height);
		m_RankPrizeListBar->setPosition(m_ui.RankPrizeList->getPosition() + Point(-12, 0));
		m_ui.RankReward->addChild(m_RankPrizeListBar);

		m_RankPrizeListBar->setScrollBarLengthPercent(1.0f);
		m_RankPrizeListBar->setScrollBarPercentY(0.0f);
		dynamic_cast<ScrollView*>(m_ui.RankPrizeList)->addEventListener(CC_CALLBACK_2(PvpLayer::onScrollViewEvent, this));
		m_ui.pBtnRankRewardClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.RankReward,"Button_close"));
		m_ui.pBtnRankRewardClose->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));
	}
}

void PvpLayer::initFightSettlement()
{
	if (!m_ui.fightDesc)
	{
		m_ui.fightDesc = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.fightPanel,"Panel_Ranking"));//战斗总结
		ImageView *Image_di = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.fightPanel,"Image_di"));//当前排名图片
		m_ui.imgCurRank = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Image_di,"Image_CurrentRanking"));//当前排名图片
		m_ui.imgHistoryBestRank = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Image_di,"Image_formerly"));//历史最高排名图片
		m_ui.curRank = dynamic_cast<Text*>(Helper::seekWidgetByName(Image_di,"Label_rank_1"));//当前名次
		m_ui.bestRank = dynamic_cast<Text*>(Helper::seekWidgetByName(Image_di,"Label_rank_2"));//最高名次
		m_ui.curUp = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Image_di,"Image_up_1"));//当前箭头
		m_ui.bestUp = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Image_di,"Image_up_2"));//最高箭头
		m_ui.curUpCount = dynamic_cast<Text*>(Helper::seekWidgetByName(Image_di,"Label_up_1"));//当前上升名次
		m_ui.bestUpCount = dynamic_cast<Text*>(Helper::seekWidgetByName(Image_di,"Label_up_2"));//最高上升名次
		m_ui.getPrizeGold = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.fightDesc,"Label_silver"));//获得银币
		m_ui.getPrizeTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.fightDesc,"Label_tips"));//提示
		m_ui.curRank->setFontName(FONT_FZZHENGHEI);
		m_ui.bestRank->setFontName(FONT_FZZHENGHEI);
		m_ui.curUpCount->setFontName(FONT_FZZHENGHEI);
		m_ui.bestUpCount->setFontName(FONT_FZZHENGHEI);
		m_ui.getPrizeGold->setFontName(FONT_FZZHENGHEI);
		m_ui.getPrizeTips->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(m_ui.getPrizeTips,Color3B(0x00,0x00,0x00),2);
	}
}

void PvpLayer::initVictoryReward()
{
	if (!m_ui.VictoryReward)
	{
		m_ui.VictoryReward = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Panel_VictoryReward,"VictoryReward"));
		m_ui.VictoryRewardList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.VictoryReward,"ListView_RewardList"));//胜场奖励列表
		//滚动条
		m_VictoryRewardListBar = CustomScrollBar::create();
		m_VictoryRewardListBar->initScrollBar(m_ui.VictoryRewardList->getContentSize().height);
		m_VictoryRewardListBar->setPosition(m_ui.VictoryRewardList->getPosition() + Point(-12, 0));
		m_ui.VictoryReward->addChild(m_VictoryRewardListBar);

		m_VictoryRewardListBar->setScrollBarLengthPercent(1.0f);
		m_VictoryRewardListBar->setScrollBarPercentY(0.0f);
		dynamic_cast<ScrollView*>(m_ui.VictoryRewardList)->addEventListener(CC_CALLBACK_2(PvpLayer::onScrollViewEvent, this));

		m_ui.pBtnVictoryRewardClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.VictoryReward,"Button_close"));//胜场列表关闭
		m_ui.VictoryRewardCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.VictoryReward,"Label_Win_1"));//胜场次数描述
		m_ui.VictoryRewardCount->setFontName(FONT_FZZHENGHEI);
		m_ui.pBtnVictoryRewardClose->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));
	}
}

void PvpLayer::initRanking()
{
	if (!m_ui.Ranking)
	{
		m_ui.Ranking = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Panel_Ranking,"RankingList"));
		m_ui.RankingList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.Ranking,"ListView_RankingList"));//排行榜列表
		//滚动条
		m_RankingListBar = CustomScrollBar::create();
		m_RankingListBar->initScrollBar(m_ui.RankingList->getContentSize().height);
		m_RankingListBar->setPosition(m_ui.RankingList->getPosition() + Point(-12, 0));
		m_ui.Ranking->addChild(m_RankingListBar);

		m_RankingListBar->setScrollBarLengthPercent(1.0f);
		m_RankingListBar->setScrollBarPercentY(0.0f);
		dynamic_cast<ScrollView*>(m_ui.RankingList)->addEventListener(CC_CALLBACK_2(PvpLayer::onScrollViewEvent, this));

		m_ui.pBtnRankingListClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Ranking,"Button_close"));//关闭
		m_ui.RankingCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Ranking,"Label_MyRanking"));//我的排名
		m_ui.RankingBestCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Ranking,"Lab_TopRanking"));
		m_ui.RankingCount->setFontName(FONT_FZZHENGHEI);
		m_ui.RankingBestCount->setFontName(FONT_FZZHENGHEI);
		m_ui.pBtnRankingListClose->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));
	}
}
	

void PvpLayer::onEnter()
{
	Layer::onEnter();
}

void PvpLayer::onExit()
{
	cocostudio::GUIReader::destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIPvp");

	Layer::onExit();
}

void PvpLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;

	switch (observerParam->id)
	{
	case PVP_EVENT_TYPE_GET_PVP_INFO:
		{
			setPvpGetInfo();
		}
		break;
	case PVP_EVENT_TYPE_LAUCH_PVP_FIGHT:
		{
		}
		break;
	case PVP_EVENT_TYPE_SETMENT_ACCOUNT:
		{
		}
		break;
	case PVP_EVENT_TYPE_RANK:
		{
			m_ui.Panel_Ranking->setVisible(true);
			initRanking();
			ActionCreator::runCommDlgAction(m_ui.Ranking);
			addRankingList();  //添加排行榜列表
		}
		break;
	case PVP_EVENT_TYPE_WIN_PRIZE:
		{
			//按钮变灰
			showPopReward();
			vector<int>& rewardFlag = PvpModel::getInstance()->getRewardFlag();//胜场奖励领取状态
			rewardFlag[m_rewardInx] = PVP_DAY_WIN_TYPE_GETED;

			addVictoryRewardList();  //添加胜场奖励列表
		}
		break;
	case nMAIN_CMD_DOWNLOAD_FORMATION:
		{
			m_ui.mineFight->setString(TO_STR(HeroModel::getInstance()->getTotalFighting())); //先置零
			setTextAddStroke(m_ui.mineFight,Color3B(0x7d,0x3f,0x1c),2);

			MainParam* mainParam = MainModel::getInstance()->getMainParam();
			vector<HeroParam*> BattleHero = HeroModel::getInstance()->getOnBattleHero();
			PVP_PLAYER_DATA_VEC& playerVec = PvpModel::getInstance()->getPlayerVec();

			int idx = -1;
			for (int i = 0; i < playerVec.size(); i++)
			{
				if (mainParam->mRoleId == playerVec.at(i).id)
				{
					idx = i;
					//自己阵容可能改变
					for (int j = 0; j < playerVec.at(i).matrixIds.size(); j++)
					{
						playerVec.at(i).matrixIds[j].id = -1;
						playerVec.at(i).matrixIds[j].stars = 0;
					}

					for (int k = 0; k < BattleHero.size(); k++)
					{
						playerVec.at(i).matrixIds[BattleHero.at(k)->mBattleFlag-1].id = BattleHero.at(k)->mTemplateId*100 + BattleHero.at(k)->mAdvLvl;
						playerVec.at(i).matrixIds[BattleHero.at(k)->mBattleFlag-1].stars = BattleHero.at(k)->mHeroStars;
					}

					break;
				}
			}
			extension::TableViewCell* cell = m_tMemberList->cellAtIndex(idx);
			if (cell)
			{
				_setItemByIdx(cell,idx);
			}
		}
		break;
	case PVP_EVENT_TYPE_BUY_COUNT:
		{
			m_ui.PvpCost->setString(TO_STR(PvpModel::getInstance()->getPvpCount()) + "/"
				+ TO_STR(DataManager::getInstance()->searchToolSystemById(PVP_FREE_COUNT)->getIntData("value")));
			setTextAddStroke(m_ui.PvpCost,Color3B(0x59,0x00,0x00),2);

			if (PvpModel::getInstance()->getPvpCount() <= 0)
			{
				m_ui.PvpCost->setColor(Color3B(Color3B::RED));
			}
			else
			{
				m_ui.PvpCost->setColor(Color3B(Color3B::WHITE));
			}
			setTextAddStroke(m_ui.PvpCost,Color3B(0x59,0x00,0x00),2);
			m_ui.pBtAddPvpvCount->setPosition(Vec2(m_ui.PvpCost->getPosition().x+m_ui.PvpCost->getContentSize().width+5,
				m_ui.pBtAddPvpvCount->getPosition().y));
		}
		break;
	default:
		break;
	}
}

void PvpLayer::onBtnNotify(Ref* sender, Widget::TouchEventType type)
{
	Size size = Director::getInstance()->getVisibleSize();
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (sender == m_ui.pBtnRankPrize) //排行奖励
		{
			m_ui.Panel_RankReward->setVisible(true);
			initPrize();
			ActionCreator::runCommDlgAction(m_ui.RankReward);
			addRankPrizeList();  //添加排行榜奖励列表
		}
		else if (sender == m_ui.pBtnRankRewardClose) //关闭排行奖励
		{
			m_ui.Panel_RankReward->setVisible(false);
		}
		else if (sender == m_ui.pBtnAdjustment) //调整整形
		{
			// 利用定时器特性，防止复杂模块在较短时间内被重复创建;
			// 其他UI界面可复用此方法;
			// added by Phil 12/28/2015 @zcjoy ;
			auto callBack = [this](float dt)->void
			{
				this->getScheduler()->unschedule("PvpLayer::onBtnNotify-create-formation", this);
				Director::getInstance()->pushScene(SceneFormation::create(F_MODE_PVP));
			};
			getScheduler()->schedule(callBack, this, 0, 0, 0, false, "PvpLayer::onBtnNotify-create-formation");

			//调用阵容界面
		}
		else if (sender == m_ui.pBtnWinPrize) //胜场奖励
		{
			m_ui.Panel_VictoryReward->setVisible(true);
			initVictoryReward();
			ActionCreator::runCommDlgAction(m_ui.VictoryReward);
			addVictoryRewardList();  //添加胜场奖励列表
		}
		else if (sender == m_ui.pBtnVictoryRewardClose) //关闭胜场奖励
		{
			m_ui.Panel_VictoryReward->setVisible(false);
			setVictoryRewardTips();
		}
		else if (sender == m_ui.pBtnHonorShop)
		{
			Director::getInstance()->pushScene(ShopScene::createScene(ST_RONGYU,true));
		}
		else if (sender == m_ui.pBtnRankList) //排行榜
		{
			PvpController::getInstance()->getRank();
		}
		else if (sender == m_ui.pBtnRankingListClose) //关闭排行榜
		{
			m_ui.Panel_Ranking->setVisible(false);
		}
		else if (sender == m_ui.fightPanel)
		{
			if (m_ui.fightPanel->getChildByName("Image_MyRanking")->getChildByName("armaturGood"))
			{
				m_ui.fightPanel->getChildByName("Image_MyRanking")->removeChildByName("armaturGood");
			}

			m_ui.fightPanel->setVisible(false); //隐藏pvp名词上升结算
		}
		else if(sender == m_ui.pBtnhelp)
		{
			LayerGameRules* PvpRule = LayerGameRules::create(PVP_ARENA_RULE_ID);
			this->addChild(PvpRule, 1);

			ActionCreator::runCommDlgAction(PvpRule);
		}
		else if (sender == m_ui.pBtAddPvpvCount)
		{
			PvpAddLayer* pvpAddLayer = PvpAddLayer::create();
			this->addChild(pvpAddLayer, 1);
		}
	}
}

void PvpLayer::onBtnSendGet(Ref* sender, Widget::TouchEventType type,int idx)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		//领取胜场奖励
		m_rewardInx = idx;
		PvpController::getInstance()->getPvpPrize(idx);
	}
}

void PvpLayer::onBtnViewPlayerLine(Ref* sender, Widget::TouchEventType type,int idx,bool isPlayer)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (isPlayer)
		{
			PVP_PLAYER_DATA_VEC& Vec = PvpModel::getInstance()->getPlayerVec();
			LayerCommFormation *_LayerCommFormation = LayerCommFormation::create(Vec.at(idx));
			_LayerCommFormation->show(Director::getInstance()->getRunningScene());
		}
		else
		{
			PVP_PLAYER_DATA_VEC& Vec = PvpModel::getInstance()->getRank();
			LayerCommFormation *_LayerCommFormation = LayerCommFormation::create(Vec.at(idx));
			_LayerCommFormation->show(Director::getInstance()->getRunningScene());
		}
	}
}

void PvpLayer::onBtnSendFight(Ref* sender, Widget::TouchEventType type,PVP_PLAYER_DATA enemy)
{
	// 进入战斗;
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("zhandou");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (PvpModel::getInstance()->getPvpCount() <= 0)
		{
			auto call = [this](Ref* ref, CustomRetType type)->void
			{
				if (type == RET_TYPE_OK)
				{
					PvpAddLayer* pvpAddLayer = PvpAddLayer::create();
					this->addChild(pvpAddLayer, 1);
				}
			};
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20219),call,CUSTOM_YES_NO);
		}
		else if (enemy.pvporder <= FIRST_RANK_COUNT && PvpModel::getInstance()->getpvpOrder() > MY_RANK_COUNT_CAN_PVP )
		{
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20220) + TO_STR(MY_RANK_COUNT_CAN_PVP) +
				DataManager::getInstance()->searchCommonTexdtById(20221));
		}
		else
		{
			Director::getInstance()->replaceScene(SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_PVP_ARENA, INVALID_CHAPTER_OR_BARRIER_ID));
			FightController::getInstance()->sendPVPFightMsg(enemy.pvporder,PvpModel::getInstance()->getpvpOrder(),enemy.id);
			PvpModel::getInstance()->setFightEnemy(enemy);
		}
	}
}

void PvpLayer::setPvpGetInfo()
{
	m_ui.Image_reward_di->setVisible(true);
	m_ui.imgNamePanel->setVisible(true);
	m_ui.Image_force_di->setVisible(true);
	m_ui.Image_force_1->setVisible(true);
	m_ui.Label_force_2->setVisible(true);

	m_ui.PvpCost->setString(TO_STR(PvpModel::getInstance()->getPvpCount()) + "/"
		+ TO_STR(DataManager::getInstance()->searchToolSystemById(PVP_FREE_COUNT)->getIntData("value")));
	if (PvpModel::getInstance()->getPvpCount() <= 0)
	{
		m_ui.PvpCost->setColor(Color3B(Color3B::RED));
	}
	else
	{
		m_ui.PvpCost->setColor(Color3B(Color3B::WHITE));
	}
	setTextAddStroke(m_ui.PvpCost,Color3B(0x59,0x00,0x00),2);
	m_ui.pBtAddPvpvCount->setPosition(Vec2(m_ui.PvpCost->getPosition().x+m_ui.PvpCost->getContentSize().width+5,
		m_ui.pBtAddPvpvCount->getPosition().y));

	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	if (PvpModel::getInstance()->getpvpOrder() <= FIRST_RANK_COUNT)
	{
		m_ui.rankOrder->setVisible(false);
		m_ui.rankImgOder->loadTexture("Image/Icon/global/img_" + TO_STR(PvpModel::getInstance()->getpvpOrder()) + "ming.png");
	}
	else
	{
		m_ui.rankOrder->setVisible(true);
		m_ui.rankOrder->setString(TO_STR(PvpModel::getInstance()->getpvpOrder()));
		setTextAddStroke(m_ui.rankOrder,Color3B(0x00,0x00,0x00),2);
	}
	m_ui.mineName->setString(mainParam->mName);

	m_ui.mineFight->setString(TO_STR(HeroModel::getInstance()->getTotalFighting())); //先置零
	setTextAddStroke(m_ui.mineFight,Color3B(0x7d,0x3f,0x1c),2);

	int pvpRank = PvpModel::getInstance()->getpvpOrder();
	int i = 0;
	PVP_REWARD_ITEM_VEC *rewardPvp = PvpModel::getInstance()->getRewardPvp();
	while (i < rewardPvp->size())
	{
		if (pvpRank >= rewardPvp->at(i).top && pvpRank <= rewardPvp->at(i).bot)
		{
			m_ui.todayHonorCount->setString(TO_STR(rewardPvp->at(i).prize[rewardPvp->at(i).prize.size() - 1].amount));
			break;
		}
		i++;
	}
	if (i >= rewardPvp->size())
	{
		m_ui.todayHonorCount->setString(TO_STR(rewardPvp->at(rewardPvp->size()-1).prize[rewardPvp->at(rewardPvp->size()-1).prize.size() - 1].amount));
	}
	setTextAddStroke(m_ui.todayHonorCount,Color3B(0x00,0x00,0x00),2);

	setVictoryRewardTips();
	
	_addPlayerList();
}

void PvpLayer::addRankPrizeList()
{
	m_ui.RankPrizeList->removeAllItems();
	PVP_REWARD_ITEM_VEC *rewardPvp = PvpModel::getInstance()->getRewardPvp();
	for (int i = 0; i < rewardPvp->size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIPvp/UIPvp_RankReward.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
		ImageView *imgRank = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Image_rank_1"));
		Text *texRank = dynamic_cast<Text*>(Helper::seekWidgetByName(layer, "Label_rank_2"));
		texRank->setFontName(FONT_FZZHENGHEI);

		if (rewardPvp->at(i).bot == rewardPvp->at(i).top)
		{
			texRank->setVisible(false);

			imgRank->loadTexture("Image/Icon/global/img_" + TO_STR(rewardPvp->at(i).bot) + "ming.png");
		}
		else
		{
			texRank->setVisible(true);

			texRank->setString(TO_STR(rewardPvp->at(i).top) + "~" + TO_STR(rewardPvp->at(i).bot));
			setTextAddStroke(texRank, Color3B(0x00, 0x00, 0x00), 2);
		}

		for (int k = 0; k < rewardPvp->at(i).prize.size(); k++)
		{
			Layout *prize = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer, "Panel_reward_" + TO_STR(k+1)));
			
			UIToolHeader *icon = UIToolHeader::create(rewardPvp->at(i).prize[k].templateId);
			icon->setNumEx(rewardPvp->at(i).prize[k].amount);
			icon->setAnchorPoint(Vec2(0,0));
			icon->setScale(0.8f);
			icon->setToolId(rewardPvp->at(i).prize[k].templateId);
			icon->setTipsEnabled(true);
			prize->addChild(icon);
		}
			
		m_ui.RankPrizeList->pushBackCustomItem(layer);
	}

	int scrollViewHeight = rewardPvp->size() * 115+10;
	int scrollViewWidth = m_ui.RankPrizeList->getInnerContainerSize().width;
	m_ui.RankPrizeList->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	m_RankPrizeListBar->setScrollBarLengthPercent(
		m_ui.RankPrizeList->getContentSize().height / m_ui.RankPrizeList->getInnerContainerSize().height);
	m_RankPrizeListBar->setScrollBarPercentY(0.0f);
}

void PvpLayer::addVictoryRewardList()
{
	int cur = PvpModel::getInstance()->getPvpDayCount();
	int max = PVP_DAY_COUNT_MAX;
	m_ui.VictoryRewardCount->setString(DataManager::getInstance()->searchCommonTexdtById(20222) + TO_STR(cur) + "/" + TO_STR(max));
	setTextAddStroke(m_ui.VictoryRewardCount, Color3B(0x88, 0x0b, 0x0b), 2);

	m_ui.VictoryRewardList->removeAllItems();
	PVP_REWARD_ITEM_VEC *rewardWIN = PvpModel::getInstance()->getRewardWIN();
	sort(rewardWIN->begin(),rewardWIN->end(),[=](PVP_REWARD_ITEM a,PVP_REWARD_ITEM b)
	{
		//是否已领取
		vector<int>& rewardFlag = PvpModel::getInstance()->getRewardFlag();//胜场奖励领取状态

		bool canA = false;
		bool canB = false;
		//判断A是否能领取
		if (rewardFlag[a.bot-1] == PVP_DAY_WIN_TYPE_GET)
		{
			canA = true;
		}

		//判断B是否能领取
		if(rewardFlag[b.bot-1] == PVP_DAY_WIN_TYPE_GET)
		{
			canB = true;
		}

		if (canA && !canB)
		{
			return true;
		}
		else if (!canA && canB)
		{
			return false;
		}
		else
		{
			bool HaveA = false;
			bool HaveB = false;
			//判断A是已经领取
			if ( rewardFlag[a.bot-1] == PVP_DAY_WIN_TYPE_GETED ) //找到 领取
			{
				HaveA = true;
			}

			//判断B是已经领取
			if ( rewardFlag[b.bot-1] == PVP_DAY_WIN_TYPE_GETED ) //找到 领取
			{
				HaveB = true;
			}

			if(!HaveA && HaveB)
			{
				return true;
			}
			else if(HaveA && !HaveB)
			{
				return false;
			}
			else
			{
				return a.bot < b.bot;
			}
		}
	});

	for (int i = 0; i < rewardWIN->size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIPvp/UIPvp_VictoryReward.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
		//胜场次数
		ImageView *imgRank = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer, "Image_number"));
		imgRank->loadTexture("Image/Icon/global/img_icon_leiji_" + TO_STR(rewardWIN->at(i).bot) + ".png");


		for (int k = 0; k < rewardWIN->at(i).prize.size(); k++)
		{
			Layout *prize = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer, "Panel_reward_" + TO_STR(k + 1)));

			UIToolHeader *icon = UIToolHeader::create(rewardWIN->at(i).prize[k].templateId);
			icon->setNumEx(rewardWIN->at(i).prize[k].amount);
			icon->setAnchorPoint(Vec2(0,0));
			icon->setScale(0.8f);
			icon->setToolId(rewardWIN->at(i).prize[k].templateId);
			icon->setTipsEnabled(true);
			prize->addChild(icon);
			//icon->setPosition(Vec2(i * 70 + 100, layer->getContentSize().height / 2));
		}

		Button *getPrize = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Button_get_1"));
		ImageView *getPrizeEd = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Image_get_2"));

		//是否已领取
		vector<int>& rewardFlag = PvpModel::getInstance()->getRewardFlag();//胜场奖励领取状态
		if (rewardFlag[rewardWIN->at(i).bot-1] == PVP_DAY_WIN_TYPE_NONE)
		{
			getPrize->setVisible(true);
			getPrize->setBright(false);

			getPrizeEd->setVisible(false);
		}
		else if (rewardFlag[rewardWIN->at(i).bot-1] == PVP_DAY_WIN_TYPE_GET)
		{
			getPrizeEd->setVisible(false);

			getPrize->setVisible(true);
			getPrize->setBright(true);
			getPrize->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnSendGet, this,rewardWIN->at(i).bot-1));
		}
		else if (rewardFlag[rewardWIN->at(i).bot-1] == PVP_DAY_WIN_TYPE_GETED)
		{
			getPrize->setVisible(false);
			getPrizeEd->setVisible(true);
		}

		m_ui.VictoryRewardList->pushBackCustomItem(layer);
	}

	int scrollViewHeight = rewardWIN->size() * 155+10;
	int scrollViewWidth = m_ui.VictoryRewardList->getInnerContainerSize().width;
	m_ui.VictoryRewardList->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	m_VictoryRewardListBar->setScrollBarLengthPercent(
		m_ui.VictoryRewardList->getContentSize().height / m_ui.VictoryRewardList->getInnerContainerSize().height);
	m_VictoryRewardListBar->setScrollBarPercentY(0.0f);
}

void PvpLayer::addRankingList()
{
	m_ui.RankingCount->setString(TO_STR(PvpModel::getInstance()->getpvpOrder()));
	setTextAddStroke(m_ui.RankingCount, Color3B(0x00, 0x00, 0x00), 2);

	m_ui.RankingBestCount->setString(TO_STR(PvpModel::getInstance()->getpvpBestOrder()));
	setTextAddStroke(m_ui.RankingCount, Color3B(0x00, 0x00, 0x00), 2);

	m_ui.RankingList->removeAllItems();
	PVP_PLAYER_DATA_VEC& rankVec = PvpModel::getInstance()->getRank();
	for (int i = 0; i < rankVec.size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIPvp/UIPvp_RankingList.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
		ImageView *imgRank = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer, "Image_rank_1"));
		Text *texRank = dynamic_cast<Text*>(Helper::seekWidgetByName(layer, "Label_rank"));
		texRank->setFontName(FONT_FZZHENGHEI);

		if (i < FIRST_RANK_COUNT)
		{
			texRank->setVisible(false);
			imgRank->loadTexture("Image/Icon/global/img_" + TO_STR(rankVec[i].pvporder) + "ming.png");
		}
		else
		{
			imgRank->loadTexture("Image/Icon/global/img_4ming.png");
			texRank->setVisible(true);
			texRank->setString(TO_STR(rankVec[i].pvporder));
			setTextAddStroke(texRank,Color3B(0x00,0x00,0x00),2);
		}

		Layout *imgHero = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer, "Panel_hero"));
		UIToolHeader *icon = UIToolHeader::create(rankVec[i].resID);
		icon->setScale(0.8f);
		icon->setAnchorPoint(Vec2(0,0));
		icon->setLvl(rankVec[i].level);
		imgHero->addChild(icon);
		icon->setTouchEnabled(true);

		Text *Label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(layer, "Label_name"));
		Label_name->setString(rankVec[i].name);
		Label_name->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(Label_name, Color3B(0x7d, 0x3f, 0x1c), 2);

		Text *Label_force = dynamic_cast<Text*>(Helper::seekWidgetByName(layer, "Label_rank_2"));
		Label_force->setString(TO_STR(rankVec[i].atk));
		Label_force->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(Label_force, Color3B(0x7d, 0x3f, 0x1c), 2);

		icon->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnViewPlayerLine, this, i,false));

		m_ui.RankingList->pushBackCustomItem(layer);
	}

	int scrollViewHeight = rankVec.size() * 120+10;
	int scrollViewWidth = m_ui.RankingList->getInnerContainerSize().width;
	m_ui.RankingList->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	m_RankingListBar->setScrollBarLengthPercent(
		m_ui.RankingList->getContentSize().height / m_ui.RankingList->getInnerContainerSize().height);
	m_RankingListBar->setScrollBarPercentY(0.0f);
}

void PvpLayer::showPopReward()
{
	PVP_ReWard_Show& showParam = PvpModel::getInstance()->getRewardShow();
	vector<PopRewardItem*> vecRewardItem;
	//金币
	if (showParam.mGold > 0)
	{
		PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(showParam.mGold);
		vecRewardItem.push_back(rewardItem);
	}

	//银币
	if (showParam.mCoin > 0)
	{
		PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(showParam.mCoin);
		vecRewardItem.push_back(rewardItem);
	}

	//道具
	for (size_t i = 0; i < showParam.mVecRewardInfo.size(); i++)
	{
		PVP_SignRewardInfo& signRewardInfo = showParam.mVecRewardInfo.at(i);
		PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(signRewardInfo.mItemId, signRewardInfo.mItemCount);
		vecRewardItem.push_back(rewardItem);
	}

	PopReward::show(vecRewardItem);

}

void PvpLayer::setVictoryRewardTips()
{
	//是否已领取
	if (m_ui.pBtnWinPrize->getChildByName("REWARD_GET_TIPS"))
	{
		m_ui.pBtnWinPrize->removeChildByName("REWARD_GET_TIPS");
	}

	vector<int>& rewardFlag = PvpModel::getInstance()->getRewardFlag();//胜场奖励领取状态
	for (int i = 0; i < rewardFlag.size(); i++ )
	{
		if (rewardFlag[i] == PVP_DAY_WIN_TYPE_GET)
		{
			ImageView *imgTip = ImageView::create("Image/Icon/global/img_dian.png");
			imgTip->setPosition(Vec2(m_ui.pBtnWinPrize->getContentSize().width - 15,m_ui.pBtnWinPrize->getContentSize().height - 15));
			m_ui.pBtnWinPrize->addChild(imgTip,1,"REWARD_GET_TIPS");
			break;
		}
	}
}

void PvpLayer::setFightEnd()
{
	if (PvpModel::getInstance()->getpvpOrderInc() > 0)
	{
		GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_PVP_RANK);

		m_ui.fightPanel->setVisible(true);  //战斗
		initFightSettlement();

		m_ui.fightPanel->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnNotify,this));

		m_ui.fightPanel->getChildByName("Image_back")->setVisible(false);
		m_ui.fightPanel->getChildByName("Panel_Ranking")->setVisible(false);
		m_ui.fightPanel->getChildByName("Image_di")->setVisible(false);
		setAnimationlUp();
		m_ui.curRank->setString(TO_STR(PvpModel::getInstance()->getpvpOrder()));//当前名次
		setTextAddStroke(m_ui.curRank,Color3B(0x00,0x00,0x00),2);
		if (PvpModel::getInstance()->getpvpOrderInc() > 0)
		{
			//当前名次箭头
			m_ui.curUp->setVisible(true);
			m_ui.curUp->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5f,Vec2(0,10)),
				CCMoveBy::create(0.5f,Vec2(0,-10)),nullptr)));
			m_ui.curUpCount->setVisible(true);
			m_ui.curUpCount->setString(TO_STR(PvpModel::getInstance()->getpvpOrderInc()));//当前上升名次
		}
		else
		{
			m_ui.curUp->setVisible(false);
			m_ui.curUpCount->setVisible(false);
		}

		m_ui.bestRank->setString(TO_STR(PvpModel::getInstance()->getpvpBestOrder()));//历史最高
		setTextAddStroke(m_ui.bestRank,Color3B(0x00,0x00,0x00),2);

		if (PvpModel::getInstance()->getpvpBestOrderInc() > 0)
		{
			m_ui.bestUp->setVisible(true);
			m_ui.bestUpCount->setVisible(true);
			m_ui.bestUp->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5f,Vec2(0,10)),
				CCMoveBy::create(0.5f,Vec2(0,-10)),nullptr)));
			m_ui.bestUpCount->setString(TO_STR(PvpModel::getInstance()->getpvpOrderInc()));//当前上升名次
		}
		else
		{
			m_ui.bestUp->setVisible(false);
			m_ui.bestUpCount->setVisible(false);
		}

		m_ui.getPrizeGold->setString(TO_STR(PvpModel::getInstance()->getGoldCount()));
	}
	else
	{
		m_ui.fightPanel->setVisible(false);  //战斗
	}
}
 
void PvpLayer::setAnimationlUp()
{
	string aniName1 = "Anidown";
	string aniName2 = "Anicircle";

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anitupo1/Anitupo1.ExportJson");

	Armature* armaturGood = Armature::create("Anitupo1");

	armaturGood->getAnimation()->play(aniName1);

	auto onResultAnimationCallFunc = [=](Armature *armature, MovementEventType eventType, const std::string& strName)->void
	{
		if (eventType == MovementEventType::COMPLETE)
		{
			armaturGood->getAnimation()->play(aniName2,-1,-1);
			armaturGood->getAnimation()->setMovementEventCallFunc(nullptr);
		}
	};
	armaturGood->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
	armaturGood->getAnimation()->play(aniName1,-1,-1);

	Bone*  bone = armaturGood->getBone("TIHUAN_TEXT2");
	Bone*  bone1 = armaturGood->getBone("TIHUAN_TEXT");
	if(bone && bone1)
	{
		Skin* _newSkin = Skin::create("Image/Icon/ActTip/img_wodepaiming.png");
		bone->addDisplay(_newSkin,0);
		Skin* _newSkin1 = Skin::create("Image/Icon/ActTip/img_wodepaiming.png");
		bone1->addDisplay(_newSkin1, 0);
	}

	armaturGood->setScale(0.6f);
	armaturGood->setPosition(Vec2(m_ui.fightPanel->getChildByName("Image_MyRanking")->getContentSize().width/2 + 10,-130));
	m_ui.fightPanel->getChildByName("Image_MyRanking")->addChild(armaturGood,2,"armaturGood");

	auto call = [this]()->void
	{
		auto _call = [this]()->void
		{
			if (PvpModel::getInstance()->getGoldCount() > 0)
			{
				auto __call = [this]()->void
				{
					m_ui.fightPanel->getChildByName("Image_back")->setVisible(true);
					ActionCreator::runCommContinueAction(m_ui.fightPanel->getChildByName("Image_back"));
				};
				m_ui.fightPanel->getChildByName("Panel_Ranking")->setVisible(true);
				m_ui.fightPanel->getChildByName("Panel_Ranking")->setOpacity(0);
				m_ui.fightPanel->getChildByName("Panel_Ranking")->runAction(CCSequence::create(CCFadeIn::create(_fadeTime),CCCallFunc::create(__call),nullptr));
			}
			else
			{
				m_ui.fightPanel->getChildByName("Image_back")->setVisible(true);
				ActionCreator::runCommContinueAction(m_ui.fightPanel->getChildByName("Image_back"));
			}
		};

		m_ui.fightPanel->getChildByName("Image_di")->setVisible(true);
		m_ui.fightPanel->getChildByName("Image_di")->setOpacity(0);
		m_ui.fightPanel->getChildByName("Image_di")->runAction(CCSequence::create(CCFadeIn::create(_fadeTime),CCCallFunc::create(_call),nullptr));
	};
	m_ui.fightPanel->getChildByName("Image_MyRanking")->runAction(CCSequence::create(CCDelayTime::create(0.3f),CCCallFunc::create(call),nullptr));
}

void PvpLayer::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	switch(type)
	{

	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			if (pSender == m_ui.RankPrizeList)
			{
				Size size = m_ui.RankPrizeList->getInnerContainerSize();
				Vec2 vec = m_ui.RankPrizeList->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.RankPrizeList->getContentSize().height);

				m_RankPrizeListBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			else if (pSender == m_ui.VictoryRewardList)
			{
				Size size = m_ui.VictoryRewardList->getInnerContainerSize();
				Vec2 vec = m_ui.VictoryRewardList->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.VictoryRewardList->getContentSize().height);

				m_VictoryRewardListBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			else if (pSender == m_ui.RankingList)
			{
				Size size = m_ui.RankingList->getInnerContainerSize();
				Vec2 vec = m_ui.RankingList->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.RankingList->getContentSize().height);

				m_RankingListBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
		}
		break;
	default:
		break;
	}
}

void PvpLayer::_addPlayerList()
{
	m_tMemberList = new extension::TableView();
	if(m_tMemberList)
	{
		m_tMemberList->initWithViewSize(m_ui.tableView->getContentSize(), NULL);
		m_tMemberList->autorelease();
		m_tMemberList->setDataSource(this);
	}
	m_tMemberList->setDirection(extension::ScrollView::Direction::VERTICAL);
	m_tMemberList->setVerticalFillOrder(extension::TableView::VerticalFillOrder::TOP_DOWN);
	m_tMemberList->reloadData();
	m_tMemberList->setDelegate(this);
	m_tMemberList->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_tMemberList->setPositionY(0);
	m_ui.tableView->addChild(m_tMemberList);

	m_RankPalyerBar = CustomScrollBar::create();
	m_RankPalyerBar->initScrollBar(m_ui.tableView->getContentSize().height);
	m_RankPalyerBar->setPosition(m_ui.tableView->getPosition() + Point(-12, 0));
	m_ui.playerListDi->addChild(m_RankPalyerBar);
	m_RankPalyerBar->setScrollBarLengthPercent(1.0f);
	m_RankPalyerBar->setScrollBarPercentY(0.0f);

	m_RankPalyerBar->setScrollBarLengthPercent(m_tMemberList->getViewSize().height / m_tMemberList->getContentSize().height);
	m_RankPalyerBar->setScrollBarPercentY(0.0f);

	Vec2 minOffset = m_tMemberList->minContainerOffset(); 
	PVP_PLAYER_DATA_VEC& playerVec = PvpModel::getInstance()->getPlayerVec();
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	int idx = 0;
	for (int i = 0; i < playerVec.size(); i++)
	{
		if (mainParam->mRoleId == playerVec.at(i).id)
		{
			idx = i+1; //找到哪个是自己
			break;
		}
	}
	float offsetY = minOffset.y;
	if (offsetY < (idx - (int)playerVec.size())*m_ui.panelListNode->getCustomSize().height)
	{
		offsetY = (idx - (int)playerVec.size())*m_ui.panelListNode->getCustomSize().height;
	}
	
	m_tMemberList->setContentOffset(Vec2(0,offsetY));
}

void PvpLayer::tableCellTouched(extension::TableView* table, extension::TableViewCell* cell)
{
	int idx = cell->getIdx();
}

Size PvpLayer::cellSizeForTable(extension::TableView *table)
{
	return m_ui.panelListNode->getContentSize();
}

extension::TableViewCell* PvpLayer::tableCellAtIndex(extension::TableView *table, ssize_t idx)
{
	extension::TableViewCell* cell = table->dequeueCell();
	if(!cell)
	{
		//创建cell
		cell = new extension::TableViewCell();
		cell->autorelease();

		string strFileName;
		strFileName.assign("Image/UIPvp/UIPvp_HeroList.ExportJson");
		Layout *item = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
		item->setPosition(cell->getContentSize()/2);
		item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		item->setVisible(true);
		cell->addChild(item, 1, PLAYER_ITEM_LIST);
	}

	//设置cell
	_setItemByIdx(cell, idx);

	return cell;
}

ssize_t PvpLayer::numberOfCellsInTableView(extension::TableView *table)
{
	return PvpModel::getInstance()->getPlayerVec().size();
}

void PvpLayer::_setItemByIdx(extension::TableViewCell* cell,int idx)
{
	PVP_PLAYER_DATA_VEC& playerVec = PvpModel::getInstance()->getPlayerVec();
	MainParam* mainParam = MainModel::getInstance()->getMainParam();
	Layout* layer = (Layout*)cell->getChildByTag(PLAYER_ITEM_LIST);
	if(!layer)
	{
		return;
	}

	ImageView *imgRank = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Image_rank"));
	Text *texRank = dynamic_cast<Text*>(Helper::seekWidgetByName(layer, "Label_rank_2"));

	if (idx < FIRST_RANK_COUNT)
	{
		texRank->setVisible(false);
		imgRank->loadTexture("Image/Icon/global/img_" + TO_STR(idx + 1) + "ming.png");
	}
	else
	{
		imgRank->loadTexture("Image/Icon/global/img_4ming.png");
		texRank->setVisible(true);
		texRank->setString(TO_STR(playerVec.at(idx).pvporder));
		texRank->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(texRank,Color3B(0x00,0x00,0x00),2);
	}

	Layout *imgHero = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Panel_hero"));
	UIToolHeader *icon = UIToolHeader::create(playerVec.at(idx).resID);
	icon->setScale(0.8f);
	icon->setLvl(playerVec.at(idx).level);
	icon->setAnchorPoint(Vec2(0,0));
	if (imgHero->getChildByName("imgHero"))
	{
		imgHero->removeChildByName("imgHero");
	}
	imgHero->addChild(icon,0,"imgHero");

	icon->setTouchEnabled(true);

	Text *Label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Label_name"));
	Label_name->setString(playerVec.at(idx).name);
	Label_name->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Label_name, Color3B(0x7d, 0x3f, 0x1c), 2);

	Text *Label_force = dynamic_cast<Text*>(Helper::seekWidgetByName(layer, "Label_force"));
	Label_force->setFontName(FONT_FZZHENGHEI);

	Button *Button_fight = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Button_fight"));
	if (mainParam->mRoleId == playerVec.at(idx).id)
	{
		Button_fight->setVisible(false);//自己不能打自己
		
		Label_force->setString(TO_STR(HeroModel::getInstance()->getTotalFighting()));
		setTextAddStroke(Label_force, Color3B(0x7d, 0x3f, 0x1c), 2);
	}
	else
	{
		Button_fight->setVisible(true);

		Label_force->setString(TO_STR(playerVec.at(idx).atk));
		setTextAddStroke(Label_force, Color3B(0x7d, 0x3f, 0x1c), 2);
	}

	icon->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnViewPlayerLine, this,idx,true));
	Button_fight->addTouchEventListener(CC_CALLBACK_2(PvpLayer::onBtnSendFight,this,playerVec.at(idx)));
}

void PvpLayer::scrollViewDidScroll(extension::ScrollView* view)
{
	Size size = view->getContentSize();
	Vec2 vec = view->getContentOffset();

	//滚动条重设进度
	float percentY = -vec.y / (size.height - view->getViewSize().height);
	m_RankPalyerBar->setScrollBarPercentY((1.0f - percentY)*100.0f);			
}
