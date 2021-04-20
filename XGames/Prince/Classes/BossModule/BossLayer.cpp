#include "BossLayer.h"
#include "BossModel.h"
#include "BossController.h"
#include "Widget/LayerCommHeadInfo.h"
#include "HeroModule/HeroModel.h"
#include "FightModule/FightController.h"
#include "Widget/LayerGameRules.h"
#include "MarvellousActive/MarvellousActiveScene.h"
#include "Temp/CustomGoldTips.h"
#include "MainModule/MainCityScene.h"


#define GAP_RANK_LIST 20.0f
#define GOLD_ITEM_ID  11001
#define COIN_ITEM_ID  11002
#define REWARD_TOOL_TAG	1001
#define SPINE_BOSS_TAG	1002
BossLayer::BossLayer()
{
	m_restTime = 0;
	m_coolTime = 0;
	m_iClearCoolCost = 0;
	m_pnlGettedRewardItem = nullptr;
	m_vecInspireCost.clear();
	m_vecRewardInfo.clear();
	m_vBoss.clear();
	m_bIsGettedRewardOnce = false;
	m_bIsNeedConfirmInspire = true;
	m_bIsNeedGetRank = true;
	m_bIsActEnded = false;	
	m_bIsInittedSpine = false;

	BossModel::getInstance()->addObserver(this);
}

BossLayer::~BossLayer()
{
	BossModel::getInstance()->removeObserver(this);
}

bool BossLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void BossLayer::onEnter()
{
	Layer::onEnter();

	auto callBack = [this](float dt)->void
	{
		if(BossModel::getInstance()->m_iIsEndActive != 1)
		{
			m_bossController.sendGetBossMainMsg();
		}
		else
		{
			//最后一个boss死亡直接走这里刷新界面
			updateViewWithUnschedule();
		}
		this->getScheduler()->unschedule("BossLayer::sendMsg", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "BossLayer::sendMsg");
}

void BossLayer::onExit()
{
	Layer::onExit();
	//unschedule(schedule_selector(BossLayer::updateBySec));

	//关闭刷新boss界面的定时器
	if(this->isScheduled(schedule_selector(BossLayer::updateActiveState)))
	{
		this->unschedule(schedule_selector(BossLayer::updateActiveState));					
	}
	if(this->isScheduled(schedule_selector(BossLayer::updateBySec)))
	{
		this->unschedule(schedule_selector(BossLayer::updateBySec));
	}
}

void BossLayer::update(float dt)
{
	this->unscheduleUpdate();
	Director::getInstance()->replaceScene(MainCityScene::create());
}

void BossLayer::initUI()
{
	//加载Boss主界面
	string strFileName;
	strFileName.assign("Image/UI_BOSS/UI_BOSS.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_BossBg.jpg");
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

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_BOSS);
	m_ui.pRoot->addChild(_layerCommHeadInfo);

	initUnactivatedUI();
	initLastTimeResultUI();
	initActivityUI();
	initRewardListUI();

	//滚动条1 伤害排行榜
	m_RankScrollBar = CustomScrollBar::create();
	m_RankScrollBar->initScrollBar(m_ui.scrollRankList->getContentSize().height);
	m_RankScrollBar->setPosition(m_ui.scrollRankList->getPosition() - Point(m_ui.scrollRankList->getContentSize().width*0.5-62, 10));
	m_ui.layerLastResult->getChildByName("Pnl_LastRank")->getChildByName("Img_di2")->addChild(m_RankScrollBar);
	m_RankScrollBar->setScrollBarLengthPercent(1.0f);
	m_RankScrollBar->setScrollBarPercentY(0.0f);

	//滚动条2 奖励列表
	m_RewardScrollBar = CustomScrollBar::create();
	m_RewardScrollBar->initScrollBar(m_ui.scrollGetReward->getContentSize().height);
	m_RewardScrollBar->setAnchorPoint(Point(0.5, 0.25));
	m_RewardScrollBar->setPosition(m_ui.scrollGetReward->getPosition() - 
		Point(m_ui.scrollGetReward->getContentSize().width*0.5-80, -20));
	m_ui.layerRewardListView->getChildByName("Img_di2")->addChild(m_RewardScrollBar);
	m_RewardScrollBar->setScrollBarLengthPercent(m_ui.scrollGetReward->getContentSize().height/m_ui.scrollGetReward->getInnerContainerSize().height);	
	m_RewardScrollBar->setScrollBarPercentY(0.0f);	

	//先屏蔽掉未开启界面显示，否则活动时战斗切回来会闪
	m_ui.layerNotOpen->setVisible(false);
}

void BossLayer::initUnactivatedUI()
{
	Button* btnGetReward = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Reward1"));
	btnGetReward->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_GetReward));
	Button* btnRules = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Rule1"));
	btnRules->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_Rules));
	ImageView* imgRedDot = (ImageView*)btnGetReward->getChildByName("Img_dot");
	imgRedDot->setVisible(false);
	
	m_ui.layerNotOpen = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Layer_NotOpen"));
	m_ui.txtRankFrist = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_1ST"));
	m_ui.txtRankFrist->setFontName(FONT_FZZHENGHEI);
	m_ui.txtRankFrist->setString("");
	setTextAddStroke(m_ui.txtRankFrist, Color3B(0x7d, 0x1f, 0x3d), 2);
	m_ui.txtRankSecond = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_2ND"));
	m_ui.txtRankSecond->setFontName(FONT_FZZHENGHEI);
	m_ui.txtRankSecond->setString("");
	setTextAddStroke(m_ui.txtRankSecond, Color3B(0x7d, 0x1f, 0x3d), 2);
	m_ui.txtRankThird = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_3RD"));
	m_ui.txtRankThird->setFontName(FONT_FZZHENGHEI);
	m_ui.txtRankThird->setString("");
	setTextAddStroke(m_ui.txtRankThird, Color3B(0x7d, 0x1f, 0x3d), 2);
	m_ui.txtCoolDown = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_CoolDown"));
	m_ui.txtCoolDown->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtCoolDown, Color3B(0x09, 0x4e, 0x51), 2);
	m_ui.btnLastTimeResult = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_LastRank"));
	m_ui.btnLastTimeResult->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_LastTimeResult));
}

void BossLayer::initLastTimeResultUI()
{
	m_ui.layerLastResult = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UI_BOSS/UI_Boss_Rank.ExportJson"));
	this->addChild(m_ui.layerLastResult);
	m_ui.layerLastResult->setVisible(false);
	m_ui.txtMyRank = (Text*)(Helper::seekWidgetByName(m_ui.layerLastResult, "Lab_Rank"));
	m_ui.txtMyRank->setFontName(FONT_FZZHENGHEI);
	m_ui.txtMyRank->setString("");
	m_ui.txtMyDamage = (Text*)(Helper::seekWidgetByName(m_ui.layerLastResult, "Lab_Damage1"));
	m_ui.txtMyDamage->setFontName(FONT_FZZHENGHEI);
	m_ui.txtMyDamage->setString("");

	m_ui.scrollRankList = (ScrollView*)(Helper::seekWidgetByName(m_ui.layerLastResult, "ScrollView_RANK"));
	m_ui.scrollRankList->addEventListener(CC_CALLBACK_2(BossLayer::onScrollViewEvent, this));

	//m_ui.pnlLastResultItem = (Layout*)(Helper::seekWidgetByName(m_ui.layerLastResult, "Pnl_List"));
	//m_ui.pnlLastResultItem->setVisible(false);
	
	m_ui.btnClose1 = (Button*)(Helper::seekWidgetByName(m_ui.layerLastResult, "Btn_Close1"));
	m_ui.btnClose1->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_Close1));
}

void BossLayer::initActivityUI()
{
	m_ui.layerActivity = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Layer_Activity"));

	Button* btnGetReward = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Reward2"));
	btnGetReward->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_GetReward));
	Button* btnRules = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Rule2"));
	btnRules->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_Rules));
	ImageView* imgRedDot = (ImageView*)btnGetReward->getChildByName("Img_dot");
	imgRedDot->setVisible(false);

	//伤害榜
	m_ui.vecResultText.clear();
	for(int i=0; i<4; i++)
	{
		sFightResultRank resultRank;
		resultRank.txtPlayerName = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Top"+_TO_STR(i+1)));
		resultRank.txtPlayerName->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(resultRank.txtPlayerName, Color3B(0x7d, 0x1f, 0x3d), 2);
		resultRank.txtPlayerDamage = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Damage"+_TO_STR(i+1)));
		resultRank.txtPlayerDamage->setFontName(FONT_FZZHENGHEI);
		resultRank.rank = 0;
		m_ui.vecResultText.push_back(resultRank);
	}
	
	//boss信息
	m_ui.vecBossInfo.clear();
	for(int i=0; i<4; i++)
	{
		sBossInfo boss;
		boss.pnlBoss = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_BOSS"+_TO_STR(i+1)));
		boss.imgBossAttribute = (ImageView*)boss.pnlBoss->getChildByName("Img_Attribute");
		boss.barHp = (LoadingBar*)boss.pnlBoss->getChildByName("Bar_hp");
		boss.imgBossImage = nullptr;
		boss.imgKilledFlag = (ImageView*)boss.pnlBoss->getChildByName("Img_DieFlag");
		boss.txtBossName = (Text*)boss.pnlBoss->getChildByName("Lab_BossName");
		boss.txtBossName->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(boss.txtBossName, Color3B(0x66, 0x15, 0x2a), 2);
		boss.txtKillerName = (Text*)boss.pnlBoss->getChildByName("Lab_Player");
		boss.txtKillerName->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(boss.txtKillerName, Color3B(0x7d, 0x1f, 0x3d), 2);
		boss.txtBossLevel = (Text*)boss.pnlBoss->getChildByName("Img_Level")->getChildByName("Lab_bossLevel");
		boss.txtBossLevel->setFontName(FONT_FZZHENGHEI);
		boss.bossId = 0;
		boss.isLive = false;
		m_ui.vecBossInfo.push_back(boss);
	}

	m_ui.pnlClearCool = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Reflush"));
	m_ui.txtCoolTime = (Text*)m_ui.pnlClearCool->getChildByName("Lab_CoolDown");
	m_ui.txtCoolTime->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtCoolTime, Color3B(0x4b, 0x07, 0x07), 2);
	Button* btnReflush = (Button*)m_ui.pnlClearCool->getChildByName("Btn_Refresh");
	btnReflush->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_ClearCoolTime));
	m_ui.txtClearCoolCost = (Text*)btnReflush->getChildByName("Lab_Price");
	m_ui.txtClearCoolCost->setFontName(FONT_FZZHENGHEI);
	Button* btnInspire = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Inspire"));
	btnInspire->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_Inspire));
	m_ui.txtInspireCost = (Text*)btnInspire->getChildByName("Lab_Price");
	m_ui.txtInspireCost->setFontName(FONT_FZZHENGHEI);
	m_ui.txtDamageAdded = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_AddDamage"));
	m_ui.txtDamageAdded->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtDamageAdded, Color3B(0x4b, 0x07, 0x07), 2);

	m_ui.pnlBossKilledTips = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_BossDieTips"));
	m_ui.txtBossKilledTips = (Text*)m_ui.pnlBossKilledTips->getChildByName("Img_Tips1")->getChildByName("Lab_Kill");
	m_ui.txtBossKilledTips->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtBossKilledTips, Color3B(0xb3, 0x22, 0x05), 2);
	m_ui.txtBossKillerTips = (Text*)m_ui.pnlBossKilledTips->getChildByName("Img_Tips2")->getChildByName("Lab_Killer");
	m_ui.txtBossKillerTips->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtBossKillerTips, Color3B(0x7d, 0x1f, 0x3d), 2);

	//未上榜
	m_ui.txtNotInRank = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_MyRank"));
	m_ui.txtNotInRank->setFontName(FONT_FZZHENGHEI);
	Text* cooling = (Text*)m_ui.pnlClearCool->getChildByName("Lab_CD");
	cooling->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(cooling, Color3B(0x4b, 0x07, 0x07), 2);
	m_ui.txtRestTime = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Lab_CntDown"));
	m_ui.txtRestTime->setFontName(FONT_FZZHENGHEI);
}

void BossLayer::initBossSpine()
{
	//初始化4个boss
	BossModel* pInstance = BossModel::getInstance();
	for(int i=0; i<4; i++)
	{
		if(i >= pInstance->m_vBossInfoFromSvr.size() || i >= m_ui.vecBossInfo.size())
		{
			return;
		}
		RowData* rowData = DataManager::getInstance()->searchMonsterById(pInstance->m_vBossInfoFromSvr.at(i).bossId);
		if(rowData == nullptr)
		{
			continue;
		}
		//boss待机动画
		int nResID = rowData->getIntData("resID");
		SpineAnimation* pSkelFighter = SpineAnimation::createFighterWithResId(nResID);
		pSkelFighter->setPosition(Vec2(m_ui.vecBossInfo.at(i).pnlBoss->getContentSize().width/2, 40));
		pSkelFighter->setAnimation(0, "wait", true);
		pSkelFighter->setSkin("skin1");
		pSkelFighter->setScale(0.86);
		m_vBoss.push_back(pSkelFighter);
		if(i < m_vBoss.size())
		{
			m_ui.vecBossInfo.at(i).pnlBoss->addChild(m_vBoss.at(i), 1, SPINE_BOSS_TAG);
			m_ui.vecBossInfo.at(i).pnlBoss->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_Boss1+i));
		}
	}

	m_bIsInittedSpine = true;
}

void BossLayer::initRewardListUI()
{
	BossModel* pInstance = BossModel::getInstance();
	m_ui.layerRewardListView = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UI_BOSS/UI_Boss_Reward.ExportJson"));

	this->addChild(m_ui.layerRewardListView);
	m_ui.layerRewardListView->setVisible(false);
	m_ui.scrollGetReward = (ScrollView*)(Helper::seekWidgetByName(m_ui.layerRewardListView, "ScrollView_Reward"));

	m_ui.pnlRewardItem = (Layout*)(Helper::seekWidgetByName(m_ui.layerRewardListView, "Pnl_ItemList"));
	m_ui.pnlRewardItem->setVisible(false);

	Button* btnClose = (Button*)m_ui.layerRewardListView->getChildByName("Btn_Close2");
	btnClose->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_Close2));

	Button* btnGetAllReward = (Button*)m_ui.layerRewardListView->getChildByName("Btn_Quick");
	btnGetAllReward->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_GetAllReward));

	Size size(0, 0);
	size.width = m_ui.scrollGetReward->getContentSize().width;
	int itemNum = BOSS_REWARD_ID_END - BOSS_REWARD_ID_BEGIN + 1;
	if(itemNum < 4)
	{
		size.height = m_ui.scrollGetReward->getContentSize().height;
	}
	else
	{
		size.height = (m_ui.pnlRewardItem->getContentSize().height + GAP_RANK_LIST)*itemNum;
	}
	m_ui.scrollGetReward->setInnerContainerSize(size);
	
	for(int i=BOSS_REWARD_ID_BEGIN; i<=BOSS_REWARD_ID_END; i++)
	{
		RowData* rowData = DataManager::getInstance()->searchBossRewardById(i);

		sRewardInfo rewardItem;
		rewardItem.pnlRewardItem = (Layout*)m_ui.pnlRewardItem->clone();
		rewardItem.txtDamageGoalTips = (Text*)rewardItem.pnlRewardItem->getChildByName("Img_title")->getChildByName("Lab_DamgTips");
		rewardItem.txtDamageGoal = (Text*)rewardItem.pnlRewardItem->getChildByName("Img_title")->getChildByName("Lab_DamageGoal");
		rewardItem.txtMyTotalDamage = (Text*)rewardItem.pnlRewardItem->getChildByName("Img_title")->getChildByName("Lab_MyDamage");
		rewardItem.pnlProp1 = (Layout*)rewardItem.pnlRewardItem->getChildByName("pnl_prop1");
		rewardItem.pnlProp2 = (Layout*)rewardItem.pnlRewardItem->getChildByName("pnl_prop2");
		rewardItem.pnlProp3 = (Layout*)rewardItem.pnlRewardItem->getChildByName("pnl_prop3");
		rewardItem.pnlProp4 = (Layout*)rewardItem.pnlRewardItem->getChildByName("pnl_prop4");
		rewardItem.btnGet = (Button*)rewardItem.pnlRewardItem->getChildByName("Btn_Get");
		rewardItem.btnGet->addTouchEventListener(CC_CALLBACK_2(BossLayer::onBtnClicked, this, Btn_GetItemReward));
		rewardItem.imgGettedFlag = (ImageView*)rewardItem.pnlRewardItem->getChildByName("Img_GettedFlag");
		rewardItem.rewardId = i;

		rewardItem.btnGet->setVisible(true);
		rewardItem.imgGettedFlag->setVisible(false);
		
		Text* txtRewardId = Text::create();
		txtRewardId->setString(_TO_STR(i));
		txtRewardId->setName("rewardId");
		txtRewardId->setVisible(false);
		rewardItem.pnlRewardItem->addChild(txtRewardId);							//记录一下奖励ID，方便点击领取时发给服务器
		rewardItem.txtDamageGoalTips->setFontName(FONT_FZZHENGHEI);
		rewardItem.txtDamageGoal->setFontName(FONT_FZZHENGHEI);
		rewardItem.txtMyTotalDamage->setFontName(FONT_FZZHENGHEI);

		rewardItem.txtDamageGoal->setString(StringFormat::formatNumber(rowData->getIntData("damage"), false));
		rewardItem.txtMyTotalDamage->setString(_TO_STR(0) + string("/") + 
			StringFormat::formatNumber(rowData->getIntData("damage"), false));
		string toolInfo = rowData->getStringData("item");
		vector<string> vecToolInfo;
		StringFormat::parseCsvStringVec(toolInfo, vecToolInfo);
		vector<sRewardTool> vecRewardTool;
		vecRewardTool.clear();
		for(string strTmp : vecToolInfo)
		{
			int nIndex1 = strTmp.find_first_of(',', 0);
			string strTmp1 = strTmp.substr(0, nIndex1);
			string strTmp2 = strTmp.erase(0, nIndex1 + 1);
			int ToolId = atoi(strTmp1.c_str());
			int count = atoi(strTmp2.c_str());

			sRewardTool toolInfo;
			toolInfo.id = ToolId;
			toolInfo.count = count;
			vecRewardTool.push_back(toolInfo);
		}
		for(int j=0; j<vecRewardTool.size(); j++)
		{
			UIToolHeader* tool = UIToolHeader::create(vecRewardTool.at(j).id);	
			tool->setToolId(vecRewardTool.at(j).id);
			tool->setTipsEnabled(true);
			tool->setNumEx(vecRewardTool.at(j).count);
			tool->setScale(0.9f);
			tool->setPosition(Point(rewardItem.pnlProp1->getContentSize().width*0.5f, rewardItem.pnlProp1->getContentSize().height*0.5f-10));
			switch (j)
			{
			case 0:
				{
					rewardItem.pnlProp1->addChild(tool, 1, REWARD_TOOL_TAG);
				}
				break;
			case 1:
				{
					rewardItem.pnlProp2->addChild(tool, 1, REWARD_TOOL_TAG);
				}
				break;
			case 2:
				{
					rewardItem.pnlProp3->addChild(tool, 1, REWARD_TOOL_TAG);
				}
				break;
			case 3:
				{
					rewardItem.pnlProp4->addChild(tool, 1, REWARD_TOOL_TAG);
				}
				break;
			default:
				break;
			}
		}

		rewardItem.pnlRewardItem->setVisible(true);
		m_ui.scrollGetReward->addChild(rewardItem.pnlRewardItem, 100);
		rewardItem.pnlRewardItem->setAnchorPoint(Point(0, 1));
		rewardItem.pnlRewardItem->setPositionX(m_ui.pnlRewardItem->getPositionX());
		if(i == BOSS_REWARD_ID_BEGIN)
		{
			rewardItem.pnlRewardItem->setPositionY(m_ui.scrollGetReward->getInnerContainerSize().height - 8);
		}
		else
		{
			rewardItem.pnlRewardItem->setPositionY(m_ui.scrollGetReward->getInnerContainerSize().height - 8 - 
				(i-BOSS_REWARD_ID_BEGIN)*(rewardItem.pnlRewardItem->getContentSize().height + GAP_RANK_LIST));
		}
		m_vecRewardInfo.push_back(rewardItem);
	}
	m_ui.scrollGetReward->addEventListener(CC_CALLBACK_2(BossLayer::onScrollViewEvent, this));
}

void BossLayer::onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	if(pSender == m_ui.scrollRankList)
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				{
					Size size = m_ui.scrollRankList->getInnerContainerSize();
					Vec2 vec = m_ui.scrollRankList->getInnerContainer()->getPosition();

					//滚动条重设进度
					float percentY = -vec.y / (size.height - m_ui.scrollRankList->getContentSize().height);
					m_RankScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);			
				}
			}
			break;

		default:
			break;
		}
	}
	else if(pSender == m_ui.scrollGetReward)
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				{
					Size size = m_ui.scrollGetReward->getInnerContainerSize();
					Vec2 vec = m_ui.scrollGetReward->getInnerContainer()->getPosition();

					//滚动条重设进度
					float percentY = -vec.y / (size.height - m_ui.scrollGetReward->getContentSize().height);
					m_RewardScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);			
				}
			}
			break;

		default:
			break;
		}
	}
}

void BossLayer::onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName)
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case Btn_GetReward:
			{
				updateRewardListView();
				m_ui.layerRewardListView->setVisible(true);
			}
			break;

		case Btn_Rules:
			{
				LayerGameRules* bossRule = LayerGameRules::create(BOSS_RULE_ID);
				if (nullptr != bossRule)
				{
					Director::getInstance()->getRunningScene()->addChild(bossRule, 100);
					ActionCreator::runCommDlgAction(bossRule);
				}
			}
			break;

		case Btn_LastTimeResult:
			{
				m_ui.layerLastResult->setVisible(true);
				if(m_bIsNeedGetRank)
				{
					BossController::sendGetDamageRankMsg();
					m_bIsNeedGetRank = false;
				}			
			}
			break;

		case Btn_ClearCoolTime:
			{
				if(MainModel::getInstance()->getMainParam()->mGold < m_iClearCoolCost)
				{
					CustomGoldTips *_tips = CustomGoldTips::create();
					if (_tips)
					{
						Director::getInstance()->getRunningScene()->addChild(_tips);
					}
				}
				else
				{
					//先记录花费
					BossModel::getInstance()->m_iCurClearCoolCost = m_iClearCoolCost;
					//发送清除冷却指令
					BossController::sendClearCoolTimeMsg();
				}
			}
			break;

		case Btn_Inspire:
			{
				//最多只能鼓舞5次
				if(BossModel::getInstance()->m_iInspireTimes < 5)
				{
					if(BossModel::getInstance()->m_iInspireTimes == 0 && m_bIsNeedConfirmInspire)
					{
						auto call = [this](Ref* ref, CustomRetType type)->void
						{
							if (type == RET_TYPE_OK)
							{
								//如果点了确认就发送鼓舞指令
								BossController::sendInspireMsg();
							}
						};
						//第一次鼓舞弹提示框
						//CustomPop::show("花费金币提升对所有boss造成的伤害(仅本次提示)", call, CUSTOM_YES_NO_CANCEL);
						CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10021), call, CUSTOM_YES_NO_CANCEL);
						m_bIsNeedConfirmInspire = false;
					}
					else if(MainModel::getInstance()->getMainParam()->mGold < m_vecInspireCost.at(BossModel::getInstance()->m_iInspireTimes))
					{
						CustomGoldTips *_tips = CustomGoldTips::create();
						if (_tips)
						{
							Director::getInstance()->getRunningScene()->addChild(_tips);
						}
					}
					else
					{
						//先记录花费
						BossModel::getInstance()->m_iCurInspireCost = m_vecInspireCost.at(BossModel::getInstance()->m_iInspireTimes);
						//发送鼓舞指令
						BossController::sendInspireMsg();
					}				
				}
				else
				{
					//CustomTips::show("鼓舞次数已达上限");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10022));
				}
			}
			break;

		case Btn_Close1:
			{
				m_ui.layerLastResult->setVisible(false);
			}
			break;

		case Btn_Close2:
			{
				m_ui.layerRewardListView->setVisible(false);
			}
			break;

		case Btn_Boss1:
		case Btn_Boss2:
		case Btn_Boss3:
		case Btn_Boss4:
			{
				if(m_bIsActEnded)
				{
					//CustomTips::show("活动已结束");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10023));
				}
				else if(HeroModel::getInstance()->getMainHeroLevel() < atoi(m_ui.vecBossInfo.at(nWidgetName-Btn_Boss1).txtBossLevel->getString().c_str()))
				{
					string str = _TO_STR(atoi(m_ui.vecBossInfo.at(nWidgetName-Btn_Boss1).txtBossLevel->getString().c_str())) + DataManager::getInstance()->searchCommonTexdtById(10024);
					CustomTips::showUtf8(str);
				}
				else if(nWidgetName != Btn_Boss1 && m_ui.vecBossInfo.at(nWidgetName-Btn_Boss1-1).isLive)						
				{
					//CustomTips::show("请先击杀上一个BOSS");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10025));
				}
				else if(!m_ui.vecBossInfo.at(nWidgetName-Btn_Boss1).isLive)
				{
					//CustomTips::show("该BOSS已被击杀");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10026));
				}
				else if(m_ui.pnlClearCool->isVisible())
				{
					//CustomTips::show("挑战冷却中");	
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10027));	
				}
				else
				{
					BossModel::getInstance()->m_iAtkBossId = m_ui.vecBossInfo.at(nWidgetName-Btn_Boss1).bossId;
					Director::getInstance()->replaceScene(SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_BOSS, INVALID_CHAPTER_OR_BARRIER_ID));
					FightController::getInstance()->sendBossFightMsg(m_ui.vecBossInfo.at(nWidgetName-Btn_Boss1).bossId);					
				}
				BossModel::getInstance()->m_iCurSeledBoss = nWidgetName-Btn_Boss1;
			}
			break;

		case Btn_GetItemReward:
			{
				m_bIsGettedRewardOnce = false;
				Button* btn = (Button*)ref;
				m_pnlGettedRewardItem = (Layout*)btn->getParent();
				Text* rewardIdText = (Text*)btn->getParent()->getChildByName("rewardId");
				int rewardId = atoi(rewardIdText->getString().c_str());
				for(int i=0; i<BossModel::getInstance()->m_vRewardIDInfo.size(); i++)
				{
					if(rewardId == BossModel::getInstance()->m_vRewardIDInfo.at(i).rewardID)
					{
						if(!BossModel::getInstance()->m_vRewardIDInfo.at(i).isCanGetted)
						{
							//CustomTips::show("未达到领奖条件");
							CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10028));
						}
						else
						{
							//发送获取单条奖励指令
							BossController::sendGetRewardMsg(rewardId);
						}
					}
				}
			}
			break;

		case Btn_GetAllReward:
			{
				//发送一键领取指令
				//先判定是否有东西领
				bool isHaveReward = false;
				for(int i=0; i<BossModel::getInstance()->m_vRewardIDInfo.size(); i++)
				{
					if(BossModel::getInstance()->m_vRewardIDInfo.at(i).isCanGetted)
					{
						isHaveReward = true;
						break;
					}
				}
				if(isHaveReward)
				{
					m_bIsGettedRewardOnce = true;
					BossController::sendGetRewardMsg(-1);
				}
				else
				{
					//CustomTips::show("没有可领取的奖励");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10029));
				}
			}
			break;

		default:
			break;
		}
	}
}

string BossLayer::formatTime(int st, eTimeFormat format)
{
	string strResTime = "";
	if(st/3600<24)
	{
		string ht;
		string mint;
		string sect;
		int h = st/3600;
		if(h <= 9)
		{
			ht = _TO_STR(0) + _TO_STR(h);
		}
		else
		{
			ht = _TO_STR(h);
		}
		int min = (st%3600)/60;
		if(min <= 9)
		{
			mint = _TO_STR(0) + _TO_STR(min);
		}
		else
		{
			mint = _TO_STR(min);
		}
		int sec = st%60;
		if(sec <= 9)
		{
			sect = _TO_STR(0) + _TO_STR(sec);
		}
		else
		{
			sect = _TO_STR(sec);
		}

		if(format == eFormatHMS)
		{
			strResTime = ht + string(":") + mint + string(":") + sect;
		}
		else if(format == eFormatMS)
		{
			strResTime = mint + string(":") + sect;
		}
	}
	else
	{
		//strResTime = STR_UTF8("时间未到");
		strResTime = DataManager::getInstance()->searchCommonTexdtById(10030);
	}

	return strResTime;
}

void BossLayer::updateUnactivatedView()
{
	BossModel* pInstance = BossModel::getInstance();

	m_ui.layerNotOpen->setVisible(true);
	m_ui.layerActivity->setVisible(false);

	m_ui.layerNotOpen->getChildByName("Btn_Reward1")->getChildByName("Img_dot")->setVisible(false);
	m_ui.layerActivity->getChildByName("Btn_Reward2")->getChildByName("Img_dot")->setVisible(false);
	for(int i=0; i<pInstance->m_vRewardIDInfo.size(); i++)
	{
		if(pInstance->m_vRewardIDInfo.at(i).isCanGetted)
		{
			m_ui.layerNotOpen->getChildByName("Btn_Reward1")->getChildByName("Img_dot")->setVisible(true);
			m_ui.layerActivity->getChildByName("Btn_Reward2")->getChildByName("Img_dot")->setVisible(true);
			break;
		}
	}

	string restTime = formatTime(m_restTime, eFormatHMS);
	m_ui.txtCoolDown->setString(restTime);
	setTextAddStroke(m_ui.txtCoolDown, Color3B(0x09, 0x4e, 0x51), 2);
	m_restTime--;

	m_ui.txtRankFrist->setString(pInstance->m_sTop[0].name);
	setTextAddStroke(m_ui.txtRankFrist, Color3B(0x7d, 0x1f, 0x3d), 2);
	m_ui.txtRankSecond->setString(pInstance->m_sTop[1].name);
	setTextAddStroke(m_ui.txtRankSecond, Color3B(0x7d, 0x1f, 0x3d), 2);
	m_ui.txtRankThird->setString(pInstance->m_sTop[2].name);
	setTextAddStroke(m_ui.txtRankThird, Color3B(0x7d, 0x1f, 0x3d), 2);
}

void BossLayer::updateActivityView()
{
	BossModel* pInstance = BossModel::getInstance();

	m_ui.layerActivity->setVisible(true);
	m_ui.layerNotOpen->setVisible(false);

	m_ui.layerNotOpen->getChildByName("Btn_Reward1")->getChildByName("Img_dot")->setVisible(false);
	m_ui.layerActivity->getChildByName("Btn_Reward2")->getChildByName("Img_dot")->setVisible(false);
	for(int i=0; i<pInstance->m_vRewardIDInfo.size(); i++)
	{
		if(pInstance->m_vRewardIDInfo.at(i).isCanGetted)
		{
			m_ui.layerNotOpen->getChildByName("Btn_Reward1")->getChildByName("Img_dot")->setVisible(true);
			m_ui.layerActivity->getChildByName("Btn_Reward2")->getChildByName("Img_dot")->setVisible(true);
			break;
		}
	}

	//反击剩余时间
	string restTime = formatTime(m_restTime, eFormatHMS);
	m_ui.txtRestTime->setString(restTime);
	setTextAddStroke(m_ui.txtRestTime, Color3B(0x09, 0x4e, 0x51), 2);
	m_restTime--;

	//伤害量前三排名
	for(int i=0; i<3; i++)
	{
		m_ui.vecResultText.at(i).txtPlayerName->setString(pInstance->m_sTop[i].name);
		setTextAddStroke(m_ui.vecResultText.at(i).txtPlayerName, Color3B(0x7d, 0x1f, 0x3d), 2);
		m_ui.vecResultText.at(i).txtPlayerDamage->setString(pInstance->m_sTop[i].damage);
	}
	//我的伤害
	m_ui.vecResultText.at(3).txtPlayerName->setString(MainModel::getInstance()->getMainParam()->mName);
	setTextAddStroke(m_ui.vecResultText.at(3).txtPlayerName, Color3B(0x7d, 0x1f, 0x3d), 2);
	m_ui.vecResultText.at(3).txtPlayerDamage->setString(pInstance->m_myTotalDamage);
	if(pInstance->m_iMyRank <= 3)
	{
		if(pInstance->m_iMyRank == 0)
		{
			//m_ui.txtNotInRank->setString(STR_UTF8("未上榜"));
			m_ui.txtNotInRank->setString(DataManager::getInstance()->searchCommonTexdtById(10031));
		}
		else
		{
			m_ui.txtNotInRank->setString(_TO_STR(pInstance->m_iMyRank));
		}	
	}
	else
	{
		m_ui.txtNotInRank->setString(_TO_STR(pInstance->m_iMyRank));
	}
	
	if(!m_bIsInittedSpine)
	{
		initBossSpine();
	}

	//四个boss
	for(int i=0; i<4; i++)
	{
		m_ui.vecBossInfo.at(i).txtBossLevel->setString(_TO_STR(pInstance->m_vBossInfoFromSvr.at(i).needLv));
		m_ui.vecBossInfo.at(i).txtBossName->setString(pInstance->m_vBossInfoFromSvr.at(i).bossName);
		setTextAddStroke(m_ui.vecBossInfo.at(i).txtBossName, Color3B(0x66, 0x15, 0x2a), 2);
		m_ui.vecBossInfo.at(i).isLive = pInstance->m_vBossInfoFromSvr.at(i).state == 2 ? false : true;
		m_ui.vecBossInfo.at(i).txtKillerName->setString(pInstance->m_vBossInfoFromSvr.at(i).killer);
		setTextAddStroke(m_ui.vecBossInfo.at(i).txtKillerName, Color3B(0x7d, 0x1f, 0x3d), 2);
		//属性图
		if(pInstance->m_vBossInfoFromSvr.at(i).attribute == eTypeFire)
		{
			m_ui.vecBossInfo.at(i).imgBossAttribute->loadTexture("Image/Icon/global/img_shuxing11_li.png");
		}
		else if(pInstance->m_vBossInfoFromSvr.at(i).attribute == eTypeWater)
		{
			m_ui.vecBossInfo.at(i).imgBossAttribute->loadTexture("Image/Icon/global/img_shuxing33_zhi.png");
		}
		else if(pInstance->m_vBossInfoFromSvr.at(i).attribute == eTypeWood)
		{
			m_ui.vecBossInfo.at(i).imgBossAttribute->loadTexture("Image/Icon/global/img_shuxing22_min.png");
		}

		//越界判定
		if(i < m_vBoss.size())
		{
			//处理boss朝向
			m_vBoss.at(i)->setScale(-1, 1);

			if(i == 3)
			{
				m_vBoss.at(i)->setScale(-1.2, 1.2);				//最后一个boss显示1.2倍大小
			}
			if(pInstance->m_vBossInfoFromSvr.at(i).state == 0)									//存活但不可攻击
			{
				m_vBoss.at(i)->setColor(Color3B(0x5C, 0x53, 0x59));
				Vector <Node*> child = m_ui.vecBossInfo.at(i).pnlBoss->getChildren();
				for(int j=0; j<child.size(); j++)
				{
					child.at(j)->setVisible(false);
				}
				m_vBoss.at(i)->setVisible(true);
			}
			else if(pInstance->m_vBossInfoFromSvr.at(i).state == 1)								//存活可攻击
			{
				Vector <Node*> child = m_ui.vecBossInfo.at(i).pnlBoss->getChildren();
				for(int j=0; j<child.size(); j++)
				{
					child.at(j)->setVisible(true);
				}
				m_ui.vecBossInfo.at(i).imgKilledFlag->setVisible(false);
				m_ui.vecBossInfo.at(i).txtKillerName->setVisible(false);
				m_ui.vecBossInfo.at(i).pnlBoss->getChildByName("Img_Killer")->setVisible(false);
			}
			else if(pInstance->m_vBossInfoFromSvr.at(i).state == 2)								//死亡，停掉动作，boss变灰
			{
				m_vBoss.at(i)->stopAnimation();
				m_vBoss.at(i)->setColor(Color3B(0x44, 0x44, 0x44));
				Vector <Node*> child = m_ui.vecBossInfo.at(i).pnlBoss->getChildren();
				for(int j=0; j<child.size(); j++)
				{
					child.at(j)->setVisible(true);
				}
			}
		}
		m_ui.vecBossInfo.at(i).bossId = pInstance->m_vBossInfoFromSvr.at(i).bossId;

		//血量百分比
		RowData* rowData = DataManager::getInstance()->searchMonsterById(pInstance->m_vBossInfoFromSvr.at(i).bossId);
		int bossTotalHp = rowData->getIntData("hp");
		float rate = (float)pInstance->m_vBossInfoFromSvr.at(i).bossHp/(float)bossTotalHp;
		m_ui.vecBossInfo.at(i).barHp->setPercent(rate*100);
	}
	
	//是否战斗冷却
	if(m_coolTime > 0)
	{
		m_ui.pnlClearCool->setVisible(true);
		string coolTime = formatTime(m_coolTime, eFormatMS);
		m_ui.txtCoolTime->setString(coolTime);
		setTextAddStroke(m_ui.txtCoolTime, Color3B(0x4b, 0x07, 0x07), 2);

		--m_coolTime;
	}
	else
	{
		m_ui.pnlClearCool->setVisible(false);
	}

	//刷新花费
	RowData* clearTimeCost = DataManager::getInstance()->getDTToolSystem()->searchDataById(CLEAR_COOL_TIME_COST);
	m_iClearCoolCost = clearTimeCost->getIntData("value");
	m_ui.txtClearCoolCost->setString(_TO_STR(m_iClearCoolCost));

	//伤害加深
	m_ui.txtDamageAdded->setString(DataManager::getInstance()->searchCommonTexdtById(10032) + _TO_STR(pInstance->m_iDamageAdd*100) + string("%"));
	setTextAddStroke(m_ui.txtDamageAdded, Color3B(0x4b, 0x07, 0x07), 2);
	RowData* inspireCostData = DataManager::getInstance()->getDTToolSystem()->searchDataById(INSPIRE_COST);
	string inspireCost = inspireCostData->getStringData("value");
	StringFormat::parseCsvStringVec(inspireCost, m_vecInspireCost);
	//最多鼓舞5次
	if(pInstance->m_iInspireTimes < 5)
	{
		m_ui.txtInspireCost->setString(_TO_STR(m_vecInspireCost.at(pInstance->m_iInspireTimes)));
	}
	else
	{
		//m_ui.txtInspireCost->setString(STR_UTF8("已满"));
		m_ui.txtInspireCost->setString(DataManager::getInstance()->searchCommonTexdtById(10033));
	}
}

void BossLayer::showBossKilledView()
{
	BossModel* pInstance = BossModel::getInstance();
	RowData* rowData = DataManager::getInstance()->searchMonsterById(pInstance->m_iAtkBossId);
	string bossName = rowData->getStringData("name");
	m_ui.pnlBossKilledTips->setVisible(true);
	m_ui.txtBossKilledTips->setString(bossName + DataManager::getInstance()->searchCommonTexdtById(10034));
	setTextAddStroke(m_ui.txtBossKilledTips, Color3B(0xb3, 0x22, 0x05), 2);
	for(int i=0; i<pInstance->m_vBossInfoFromSvr.size(); i++)
	{
		if(pInstance->m_vBossInfoFromSvr.at(i).bossId == pInstance->m_iAtkBossId)
		{
			m_ui.txtBossKillerTips->setString(pInstance->m_vBossInfoFromSvr.at(i).killer);
			setTextAddStroke(m_ui.txtBossKillerTips, Color3B(0x7d, 0x1f, 0x3d), 2);
		}
	}
	
	FiniteTimeAction* fadeIn = FadeIn::create(0.5f);
	FiniteTimeAction* fadeOut = FadeOut::create(0.5f);
	MoveBy* moveBy = MoveBy::create(2.0f, Point(0, 200));

	m_ui.pnlBossKilledTips->runAction(Sequence::create(fadeIn, moveBy, fadeOut, nullptr));
}

void BossLayer::updateRewardListView()
{
	BossModel* pInstance = BossModel::getInstance();

	m_ui.layerNotOpen->getChildByName("Btn_Reward1")->getChildByName("Img_dot")->setVisible(false);
	m_ui.layerActivity->getChildByName("Btn_Reward2")->getChildByName("Img_dot")->setVisible(false);
	for(int i=0; i<pInstance->m_vRewardIDInfo.size(); i++)
	{
		if(pInstance->m_vRewardIDInfo.at(i).isCanGetted)
		{
			m_ui.layerNotOpen->getChildByName("Btn_Reward1")->getChildByName("Img_dot")->setVisible(true);
			m_ui.layerActivity->getChildByName("Btn_Reward2")->getChildByName("Img_dot")->setVisible(true);
			break;
		}
	}

	for(int i=0; i<m_vecRewardInfo.size(); i++)
	{
		RowData* rowData = DataManager::getInstance()->searchBossRewardById(m_vecRewardInfo.at(i).rewardId);
		for(int j=0; j<pInstance->m_vRewardIDList.size(); j++)
		{
			if(pInstance->m_vRewardIDList.at(j) == m_vecRewardInfo.at(i).rewardId)
			{
				m_vecRewardInfo.at(i).btnGet->setVisible(false);
				m_vecRewardInfo.at(i).imgGettedFlag->setVisible(true);
				break;
			}		
		}
		m_vecRewardInfo.at(i).txtMyTotalDamage->setString(string("(") + StringFormat::formatNumber(pInstance->m_iTotalDamage, false) + string("/") + 
			StringFormat::formatNumber(rowData->getIntData("damage"), false) + string(")"));
		if(pInstance->m_iTotalDamage < rowData->getIntData("damage"))
		{
			m_vecRewardInfo.at(i).txtMyTotalDamage->setColor(Color3B(0xd9, 0x2e, 0x2e));
		}
		else
		{
			m_vecRewardInfo.at(i).txtMyTotalDamage->setColor(Color3B(0x54, 0xdd, 0x51));
		}
	}
}

void BossLayer::updateLastRankListView()
{
	m_ui.pnlLastResultItem = nullptr;
	m_ui.pnlLastResultItem = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UI_BOSS/UI_BOSS_Rank_Item.ExportJson"));
	
	m_ui.scrollRankList->jumpToTop();
	BossModel* pInstance = BossModel::getInstance();
	/*Vector<Node*> AllChild = m_ui.scrollRankList->getChildren();
	for(auto &item : AllChild)
	{
		if(item != m_ui.pnlLastResultItem)
		{
			item->removeFromParent();
		}
	}*/
	m_ui.scrollRankList->removeAllChildren();
	Size size(0, 0);
	size.width = m_ui.scrollRankList->getContentSize().width;
	if(pInstance->m_vLastRankList.size() < 4)
	{
		size.height = m_ui.scrollRankList->getContentSize().height;
	}
	else
	{
		Layout* pnlChild = (Layout*)m_ui.pnlLastResultItem->getChildByName("Pnl_List");
		size.height = (pnlChild->getContentSize().height + GAP_RANK_LIST)*pInstance->m_vLastRankList.size();
	}
	m_ui.scrollRankList->setInnerContainerSize(size);

	//我的伤害和排名
	m_ui.txtMyRank->setString(_TO_STR(pInstance->m_iMyLastRank));
	m_ui.txtMyDamage->setString(pInstance->m_myLastTotalDam);
	
	for(int i=0; i<pInstance->m_vLastRankList.size(); i++)
	{
		sLastResultRankInfo lastRankInfoItem;
		lastRankInfoItem.pnlRankItem = (Layout*)((Layout*)m_ui.pnlLastResultItem->getChildByName("Pnl_List"))->clone();
		lastRankInfoItem.imgRankBg = (ImageView*)lastRankInfoItem.pnlRankItem->getChildByName("Img_RankBg");
		lastRankInfoItem.txtRankTxt = (Text*)lastRankInfoItem.imgRankBg->getChildByName("Img_RankTxt");
		lastRankInfoItem.pnlPlayerHead = (Layout*)lastRankInfoItem.pnlRankItem->getChildByName("Pnl_PlayerIcon");
		lastRankInfoItem.txtPlayerName = (Text*)lastRankInfoItem.pnlRankItem->getChildByName("Lab_PlayerName");
		lastRankInfoItem.txtDamage = (Text*)lastRankInfoItem.pnlRankItem->getChildByName("Lab_Damg");
		lastRankInfoItem.txtPlayerName->setFontName(FONT_FZZHENGHEI);
		lastRankInfoItem.txtRankTxt->setFontName(FONT_FZZHENGHEI);
		lastRankInfoItem.txtDamage->setFontName(FONT_FZZHENGHEI);

		if(pInstance->m_vLastRankList.at(i).rank < 4)
		{
			lastRankInfoItem.imgRankBg->loadTexture(string("Image/Icon/global/") + string("img_") + _TO_STR(pInstance->m_vLastRankList.at(i).rank) + string("ming.png"));		
			lastRankInfoItem.txtRankTxt->setVisible(false);
		}
		else
		{
			lastRankInfoItem.imgRankBg->loadTexture("Image/Icon/global/img_4ming.png");
			lastRankInfoItem.txtRankTxt->setString(_TO_STR(pInstance->m_vLastRankList.at(i).rank));
		}
		lastRankInfoItem.txtPlayerName->setString(pInstance->m_vLastRankList.at(i).name);
		UIToolHeader* header = UIToolHeader::create(pInstance->m_vLastRankList.at(i).resID);
		header->setLvl(pInstance->m_vLastRankList.at(i).level);
		header->setPosition(Point(lastRankInfoItem.pnlPlayerHead->getContentSize().width*0.5f+8, lastRankInfoItem.pnlPlayerHead->getContentSize().height*0.5f));
		lastRankInfoItem.pnlPlayerHead->addChild(header);
		lastRankInfoItem.txtDamage->setString(pInstance->m_vLastRankList.at(i).totalDam);

		lastRankInfoItem.pnlRankItem->setVisible(true);
		lastRankInfoItem.pnlRankItem->setAnchorPoint(Point(0, 1));
		m_ui.scrollRankList->addChild(lastRankInfoItem.pnlRankItem);
		lastRankInfoItem.pnlRankItem->setPositionX(m_ui.pnlLastResultItem->getPositionX());
		if(i == 0)
		{
			lastRankInfoItem.pnlRankItem->setPositionY(m_ui.scrollRankList->getInnerContainerSize().height - 12);
		}
		else
		{		
			lastRankInfoItem.pnlRankItem->setPositionY((m_ui.scrollRankList->getInnerContainerSize().height - 12) - 
				i*(lastRankInfoItem.pnlRankItem->getContentSize().height + GAP_RANK_LIST));
		}
	}
	m_RankScrollBar->setScrollBarLengthPercent(m_ui.scrollRankList->getContentSize().height/m_ui.scrollRankList->getInnerContainerSize().height);
	m_RankScrollBar->setScrollBarPercentY(0.0f);
}

void BossLayer::updateBySec(float dt)
{
	BossModel* pInstance = BossModel::getInstance();
	string restTime = formatTime(m_restTime, eFormatHMS);
	if(!pInstance->m_bIsOpen)
	{	
		m_ui.txtCoolDown->setString(restTime);
		setTextAddStroke(m_ui.txtCoolDown, Color3B(0x09, 0x4e, 0x51), 2);
	}
	else
	{
		m_ui.txtRestTime->setString(restTime);
		setTextAddStroke(m_ui.txtRestTime, Color3B(0x09, 0x4e, 0x51), 2);

		//是否战斗冷却
		if(m_coolTime != 0)
		{
			m_ui.pnlClearCool->setVisible(true);
			string coolTime = formatTime(m_coolTime, eFormatMS);
			m_ui.txtCoolTime->setString(coolTime);
			setTextAddStroke(m_ui.txtCoolTime, Color3B(0x4b, 0x07, 0x07), 2);

			--m_coolTime;
		}
		else
		{
			m_ui.pnlClearCool->setVisible(false);
		}
	}
	if(--m_restTime <= 1)
	{
		this->unschedule(schedule_selector(BossLayer::updateActiveState));
	}
	if(m_restTime <= 0)
	{
		if(!pInstance->m_bIsOpen)
		{
			m_bossController.sendGetBossMainMsg();
			m_restTime = 0;
			this->unschedule(schedule_selector(BossLayer::updateBySec));
		}
		else
		{
			m_bIsActEnded = true;
			m_restTime = 0;
			//m_ui.txtRestTime->setString(STR_UTF8("活动已结束"));
			m_ui.txtRestTime->setString(DataManager::getInstance()->searchCommonTexdtById(10023));
			setTextAddStroke(m_ui.txtRestTime, Color3B(0x09, 0x4e, 0x51), 2);		
			this->unschedule(schedule_selector(BossLayer::updateBySec));
		}
	};
}

void BossLayer::updateActiveState(float dt)
{
	BossController::sendGetBossMainMsg();
}

void BossLayer::showGetReward()
{
	if(m_bIsGettedRewardOnce)
	{
		//CustomTips::show("领取奖励成功");
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10035));
		return;
	}
	vector<PopRewardItem*> vecRewardItem;

	//最多四个道具
	for(int i=1; i<=4; i++)
	{
		Layout* pnlProp = (Layout*)m_pnlGettedRewardItem->getChildByName(string("pnl_prop") + _TO_STR(i));
		if(pnlProp->getChildrenCount() != 0)
		{
			UIToolHeader* tool = (UIToolHeader*)pnlProp->getChildByTag(REWARD_TOOL_TAG);
			//金币
			if(tool->getToolId() == GOLD_ITEM_ID)
			{
				PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(tool->getNum());
				vecRewardItem.push_back(rewardItem);
			}
			//银币
			else if(tool->getToolId() == COIN_ITEM_ID)
			{
				PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(tool->getNum());
				vecRewardItem.push_back(rewardItem);
			}
			else
			{
				PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(tool->getToolId(), tool->getNum());
				vecRewardItem.push_back(rewardItem);
			}
		}
	}

	PopReward::show(vecRewardItem);

}

void BossLayer::updateViewWithUnschedule()
{
	BossModel* pInstance = BossModel::getInstance();
	m_coolTime = pInstance->m_iFightCoolSeconds;			
	updateActivityView();
	if(BossModel::getInstance()->m_iIsEndActive != 1)
	{
		//关闭定时器刷新boss界面
		if(!this->isScheduled(schedule_selector(BossLayer::updateActiveState)))
		{
			schedule(schedule_selector(BossLayer::updateActiveState), 5.0, kRepeatForever, 0);
		}
		//关闭计时定时器
		if(!this->isScheduled(schedule_selector(BossLayer::updateBySec)))
		{
			schedule(schedule_selector(BossLayer::updateBySec), 1.0, kRepeatForever, 0);
		}			
	}
	else
	{
		BossModel::getInstance()->m_iIsEndActive = 0;
		//m_ui.txtRestTime->setString(STR_UTF8("活动已结束"));
		m_ui.txtRestTime->setString(DataManager::getInstance()->searchCommonTexdtById(10023));
		m_bIsActEnded = true;
		setTextAddStroke(m_ui.txtRestTime, Color3B(0x09, 0x4e, 0x51), 2);
		//关闭刷新boss界面的定时器
		if(this->isScheduled(schedule_selector(BossLayer::updateActiveState)))
		{
			this->unschedule(schedule_selector(BossLayer::updateActiveState));					
		}
		if(this->isScheduled(schedule_selector(BossLayer::updateBySec)))
		{
			this->unschedule(schedule_selector(BossLayer::updateBySec));
		}
	}
}

void BossLayer::updateSelf(void* data)
{
	if(!data)
	{
		return;
	}
	ObserverParam* obParam = (ObserverParam*) data;

	switch (obParam->id)
	{
	case nMAIN_CMD_BOSS_MAIN_LIST:
		{
			BossModel* pInstance = BossModel::getInstance();
			m_restTime = pInstance->m_iSeconds;
			if(!pInstance->m_bIsOpen)
			{				
				updateUnactivatedView();
				if(this->isScheduled(schedule_selector(BossLayer::updateActiveState)))
				{
					this->unschedule(schedule_selector(BossLayer::updateActiveState));
				}
				//开启计时定时器
				if(!this->isScheduled(schedule_selector(BossLayer::updateBySec)))
				{
					schedule(schedule_selector(BossLayer::updateBySec), 1.0, kRepeatForever, 0);
				}
			}
			else
			{
				updateViewWithUnschedule();
			}		
		}
		break;

	case nMAIN_CMD_BOSS_SETTLEMENT:
		{
			if(BossModel::getInstance()->m_vBossInfoFromSvr.at(BossModel::getInstance()->m_iCurSeledBoss).state == 2)
			{
				//显示boss死亡和击杀者
				showBossKilledView();
			}
		}
		break;

	case nMAIN_CMD_BOSS_INSPIRE:
		{
			m_ui.txtDamageAdded->setString(DataManager::getInstance()->searchCommonTexdtById(10032) + _TO_STR(BossModel::getInstance()->m_iDamageAdd*100) + string("%"));
			setTextAddStroke(m_ui.txtDamageAdded, Color3B(0x4b, 0x07, 0x07), 2);
			if(BossModel::getInstance()->m_iInspireTimes < 5)
			{
				m_ui.txtInspireCost->setString(_TO_STR(m_vecInspireCost.at(BossModel::getInstance()->m_iInspireTimes)));
				//CustomTips::show("鼓舞成功");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10036));
			}
			else
			{
				//m_ui.txtInspireCost->setString(STR_UTF8("已满"));
				m_ui.txtInspireCost->setString(DataManager::getInstance()->searchCommonTexdtById(10033));
			}
		}
		break;

	case nMAIN_CMD_BOSS_CLEAR_COOLTIME:
		{
			m_ui.pnlClearCool->setVisible(false);
			//CustomTips::show("挑战冷却清除成功");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10037));
			m_coolTime = 0;
		}
		break;

	case nMAIN_CMD_BOSS_GET_REWARD:
		{
			updateRewardListView();
			showGetReward();
		}
		break;

	case nMAIN_CMD_BOSS_CUR_DAMAGE_RANK:
		{
			updateLastRankListView();
		}
		break;

	default:
		break;
	}
}
