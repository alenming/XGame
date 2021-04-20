#include "GroupBossLayer.h"
#include "Utils/SoundUtils.h"
#include "Widget/LayerCommHeadInfo.h"
#include "MainModule/MainCityScene.h"
#include "GroupBossRank.h"
#include "GroupBossSkillTips.h"
#include "GroupBossGetReward.h"
#include "GroupBossShowBoxLayer.h"
#include "FightModule/SceneFightLoading.h"
#include "Common/Common.h"
#include "GroupModule/MyGroupScene.h"

GroupBossLayer::GroupBossLayer()
{
	m_iBossResId = 0;
	m_vGroupRank.clear();
	m_vPlayerRank.clear();
	GroupBossModel::getInstance()->addObserver(this);
}

GroupBossLayer::~GroupBossLayer( void )
{
	GroupBossModel::getInstance()->removeObserver(this);
}

GroupBossLayer* GroupBossLayer::create( bool isPush )
{
	GroupBossLayer* _layer = new GroupBossLayer();
	if (_layer->init(isPush))
	{
		_layer->autorelease();
		return _layer;
	}
	else
	{
		delete _layer;
		return nullptr;
	}
}

bool GroupBossLayer::init( bool isPush )
{
	if (!Layer::init())
	{
		return false;
	}

	m_bIsPush = isPush;

	initUI();

	return true;
}

void GroupBossLayer::onEnter()
{
	Layer::onEnter();

	auto callBack = [this](float dt)->void
	{
		GroupBossController::getInstance()->sendGetMainInfoMsg();

		this->getScheduler()->unschedule("GroupBossLayer::sendMsg", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "GroupBossLayer::sendMsg");
}

void GroupBossLayer::onExit()
{
	Layer::onExit();

	//设定冷却时间需要重新拉取
	GroupBossModel::getInstance()->setIsNeedUpdateCoolTime(true);

	//关闭刷新主界面的定时器
	if(this->isScheduled(schedule_selector(GroupBossLayer::updateMainUI)))
	{
		this->unschedule(schedule_selector(GroupBossLayer::updateMainUI));					
	}
	LocalTimer::getInstance()->removeLocalTimerListener(this);
}

void GroupBossLayer::initUI()
{
	// 加载 UI;
	string strFileName;
	strFileName.assign("Image/UI_bangpaiBOSS/UI_bangpaiBOSS.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_ui.pRoot);

	//返回
	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");
			scheduleUpdate();
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_GROUP_BOSS);
	this->addChild(_layerCommHeadInfo);

	//活动关闭标识
	m_ui.imgActEnd = dynamic_cast<ImageView*>(m_ui.pRoot->getChildByName("Img_actTime"));
	m_ui.imgActEnd->setVisible(false);

	//帮派排名
	m_vGroupRank.clear();
	m_ui.pnlGroupRank = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Panel_GangRanking"));
	m_ui.imgMyGroupRank = dynamic_cast<ImageView*>(m_ui.pnlGroupRank->getChildByName("Img_myGroupRank"));
	m_ui.imgMyGroupRank->setVisible(false);
	//帮派前三名
	for(int i=0; i<3; i++)
	{
		Layout* groupRankItem = dynamic_cast<Layout*>(m_ui.pnlGroupRank->getChildByName("Panel_GangRankingEntry_" + TO_STR(i)));
		groupRankItem->setVisible(false);		//先隐藏，防止闪
		m_vGroupRank.push_back(groupRankItem);
	}

	//个人排名
	m_vPlayerRank.clear();
	m_ui.pnlPlayerRank = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Panel_PersonalRanking"));
	m_ui.playerRankTemp = dynamic_cast<Layout*>(m_ui.pnlPlayerRank->getChildByName("Panel_PersonalRankingEntry"));
	m_ui.imgPlayerRank = dynamic_cast<ImageView*>(m_ui.pnlPlayerRank->getChildByName("Img_myRank"));
	m_ui.imgPlayerRank->setVisible(false);
	m_vPlayerRank.push_back(m_ui.playerRankTemp);
	m_ui.playerRankTemp->setVisible(false);		//先隐藏，防止闪
	//复制两个排名
	for(int i=1; i<3; i++)
	{
		Layout* playerRankItem = (Layout*)m_ui.playerRankTemp->clone();
		playerRankItem->setPosition(m_ui.playerRankTemp->getPosition() + Point(0, -43*i));
		playerRankItem->setVisible(false);		//先隐藏，防止闪
		m_ui.pnlPlayerRank->addChild(playerRankItem);
		m_vPlayerRank.push_back(playerRankItem);
	}

	//击退boss宝箱
	ImageView* parent1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Base_DownReward"));
	ImageView* parent2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "DownReward"));
	m_ui.txtBossName = dynamic_cast<Text*>(parent1->getChildByName("Text_BossName"));
	m_ui.btnGetAll = dynamic_cast<Button*>(parent1->getChildByName("BTN_AllReward"));
	m_ui.btnGetAll->addTouchEventListener(CC_CALLBACK_2(GroupBossLayer::onBtnClick, this, BTN_GETALL_REWARD));
	m_ui.barBoxReward = dynamic_cast<LoadingBar*>(parent2->getChildByName("ProgressBar_90"));
	m_ui.vBtnBox.clear();
	for(int i=0; i<5; i++)
	{
		Button* btnBoxReward = dynamic_cast<Button*>(parent2->getChildByName("reward_box_" + TO_STR(i+1)));
		if(btnBoxReward != nullptr)
		{
			Text* level = (Text*)btnBoxReward->getChildByName("text_Level");
			level->setFontName(FONT_FZZHENGHEI);
			setTextAddStroke(level, Color3B::BLACK, 2);
			m_ui.vBtnBox.push_back(btnBoxReward);
			btnBoxReward->addTouchEventListener(CC_CALLBACK_2(GroupBossLayer::onBtnClick, this, BTN_BOX_REWARD+i));

			Text* boxId = Text::create();
			boxId->setVisible(false);
			boxId->setName("BOX_ID");
			btnBoxReward->addChild(boxId);
		}
	}

	//排行榜入口按钮
	m_ui.btnRankEnterGroup = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "BTN_RankingToGang"));
	m_ui.btnRankEnterGroup->addTouchEventListener(CC_CALLBACK_2(GroupBossLayer::onBtnClick, this, BTN_GROUP_RANK));
	m_ui.btnRankEnterPersonal = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "BTN_RankingToPersonal"));
	m_ui.btnRankEnterPersonal->addTouchEventListener(CC_CALLBACK_2(GroupBossLayer::onBtnClick, this, BTN_PLAYER_RANK));

	//右侧UI
	m_ui.txtDownTime = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Text_TimeLimit"));
	m_ui.btnHelp = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "BTN_Help"));
	m_ui.btnHelp->addTouchEventListener(CC_CALLBACK_2(GroupBossLayer::onBtnClick, this, BTN_BOSS_HELP));
	m_ui.btnInspire = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "BTN_Encouragement"));
	m_ui.btnInspire->addTouchEventListener(CC_CALLBACK_2(GroupBossLayer::onBtnClick, this, BTN_INSPIRE));
	m_ui.btnAtk = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "BTN_Challenge"));
	m_ui.btnAtk->addTouchEventListener(CC_CALLBACK_2(GroupBossLayer::onBtnClick, this, BTN_ATTACK));
	m_ui.btnExchangeReward = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "BTN_Reward"));
	m_ui.btnExchangeReward->addTouchEventListener(CC_CALLBACK_2(GroupBossLayer::onBtnClick, this, BTN_EXCHANGE_REWARD));
	m_ui.txtDamageAdd = dynamic_cast<Text*>(m_ui.btnInspire->getChildByName("Text_Buff"));

	//英雄角色UI
	m_ui.imgAttriBg = dynamic_cast<ImageView*>(m_ui.pRoot->getChildByName("BossHealth"));
	m_ui.txtBossLv = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(m_ui.imgAttriBg, "number_Level"));
	m_ui._txtBossName = dynamic_cast<Text*>(m_ui.imgAttriBg->getChildByName("Text_HeroName"));
	m_ui.barBossHp = dynamic_cast<LoadingBar*>(m_ui.imgAttriBg->getChildByName("BossHealth"));
	m_ui.pnlBossAni = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Hero_Painting"));
	m_ui.pnlRevive = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_relive"));
	m_ui.pnlRevive->setVisible(false);
	m_ui.txtReviveTime = dynamic_cast<Text*>(m_ui.pnlRevive->getChildByName("Lab_coolTime"));
	m_ui.txtBossOrder = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(m_ui.pRoot, "number_train"));
	m_ui.imgBossLife.clear();
	m_ui.imgSkillBg.clear();
	for(int i=0; i<4; i++)
	{
		ImageView* imgBossLife = dynamic_cast<ImageView*>(m_ui.imgAttriBg->getChildByName("BossLife_" + TO_STR(i+1)));
		if(imgBossLife != nullptr)
		{
			m_ui.imgBossLife.push_back(imgBossLife);
		}
	}
	for(int i=0; i<5; i++)
	{
		ImageView* imgSkillBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Base_Skill_" + TO_STR(i+1)));
		if(imgSkillBg != nullptr)
		{
			m_ui.imgSkillBg.push_back(imgSkillBg);
		}
	}

	m_ui.txtBossName->setString("");
	m_ui.txtDownTime->setString("");
	m_ui.txtDamageAdd->setString("");
	m_ui._txtBossName->setString("");
	m_ui.txtReviveTime->setString("");

	m_ui.txtBossName->setFontName(FONT_FZZHENGHEI);
	m_ui.txtDownTime->setFontName(FONT_FZZHENGHEI);
	m_ui.txtDamageAdd->setFontName(FONT_FZZHENGHEI);
	m_ui._txtBossName->setFontName(FONT_FZZHENGHEI);
	m_ui.txtReviveTime->setFontName(FONT_FZZHENGHEI);

	//一些需要描边的文本
	Text* inspireCost = (Text*)(m_ui.btnInspire->getChildByName("Text_Price"));
	inspireCost->setFontName(FONT_FZZHENGHEI);
	inspireCost->setString("");
	Text* attackCount = (Text*)(m_ui.btnAtk->getChildByName("Text_number"));
	attackCount->setFontName(FONT_FZZHENGHEI);
	attackCount->setString("");

	//添加boss背景粒子动画
	showBossBgAni();
}

void GroupBossLayer::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case BTN_GROUP_RANK:
			//添加排行榜层, 默认帮派伤害榜单
			this->addChild(GroupBossRank::create(eRankType::eTYPE_GROUP));
			break;
		case BTN_PLAYER_RANK:
			//添加排行榜层, 默认个人全服伤害榜单
			this->addChild(GroupBossRank::create(eRankType::eTYPE_ALL));
			break;
		case BTN_GETALL_REWARD:
			{
				//一键领取
				if(GroupBossModel::getInstance()->isCanGetBossReward())
				{
					GroupBossController::sendGetBossRewardMsg(eMETHOD_ALL);
				}
				else
				{
					//CustomTips::show("没有可领取的奖励");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10029));
				}
			}
			break;
		case BTN_BOX_REWARD:
			{
				Button* btn = (Button*)ref;
				Text* boxIdText = (Text*)btn->getChildByName("BOX_ID");
				int boxId = atoi(boxIdText->getString().c_str());
				sBoxReward* boxRewardData = GroupBossModel::getInstance()->getBoxRewardByID(boxId);
				if(boxRewardData != nullptr)
				{
					if(boxRewardData->isCanGet)
					{
						//发协议
						GroupBossController::sendGetBossRewardMsg(eMETHOD_SINGLE, boxId);
						//记录
						GroupBossModel::getInstance()->setRewardId(boxId);
					}
					else
					{
						//显示奖励展示弹窗
						this->addChild(GroupBossShowBoxLayer::create(boxId));
					}
				}
			}
			break;
		case BTN_BOSS_HELP:
			break;
		case BTN_EXCHANGE_REWARD:
			{
				//添加伤害奖励弹窗
				this->addChild(GroupBossGetReward::create(eGetType::eTYPE_HURT));
			}
			break;
		case BTN_INSPIRE:
			{
				GroupBossModel* pInstance = GroupBossModel::getInstance();
				if(!pInstance->isActOpen())
				{
					//CustomTips::show("活动未开启");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10159));
				}
				else if(pInstance->getInspireCount() >= pInstance->getMaxInspireCount())
				{
					//CustomTips::show("已达到鼓舞次数上限");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10160));
				}
				else if(MainModel::getInstance()->getMainParam()->mGold < pInstance->getNextCost())
				{
					//CustomTips::show("金币不足");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10007));
				}
				else
				{
					GroupBossController::sendInspireMsg();
				}
			}
			break;
		case BTN_ATTACK:
			{
				GroupBossModel* pInstance = GroupBossModel::getInstance();
				if(!pInstance->isActOpen())
				{
					//CustomTips::show("活动未开启");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10159));
				}
				else if(pInstance->getBossInfo()->reviveCoolTime > 0)
				{
					//CustomTips::show("战斗冷却中");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10161));
				}
				else if(pInstance->getRestAtkCount() <= 0)
				{
					//CustomTips::show("挑战次数已用完");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10162));
				}
				else
				{
					//战斗
					//场景资源ID
					RowData* rowData = DataManager::getInstance()->searchToolSystemById(BOSS_FIGHT_SCENE_RESID);
					if(rowData != nullptr)
					{
						int sceneResId = rowData->getIntData("value");
						Director::getInstance()->replaceScene(SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_GROUP_BOSS, sceneResId));
						FightController::getInstance()->sendGroupBossFightMsg();	
					}
				}
			}
			break;

		default:
			break;
		}
	}
}

void GroupBossLayer::update( float dt )
{
	this->unscheduleUpdate();
	if(m_bIsPush)
	{
		Director::getInstance()->popScene();
	}
	else
	{
		Director::getInstance()->replaceScene(MyGroupScene::create());
	}
}

void GroupBossLayer::updateUI()
{
	updateRankUI();
	updateBoxUI();
	updateBossUI();
	updateInspireUI();

	//开启定时器刷新主界面
	if(!this->isScheduled(schedule_selector(GroupBossLayer::updateMainUI)))
	{
		schedule(schedule_selector(GroupBossLayer::updateMainUI), 5.0, kRepeatForever, 0);
	}
}

void GroupBossLayer::updateRankUI()
{
	GroupBossModel* pInstance = GroupBossModel::getInstance();

	//帮派排名, 只显示前三 和 我的帮派
	vector<sGroupDamRank>* groupRankData = pInstance->getMainGroupRankData();
	for(int i=0; i<3; i++)
	{
		m_vGroupRank.at(i)->setVisible(false);
		if(i < groupRankData->size())
		{
			Layout* pnlItem = m_vGroupRank.at(i);
			pnlItem->setVisible(true);

			ImageView* imgRank = (ImageView*)pnlItem->getChildByName("IMG_GangRanking");
			imgRank->loadTexture("Image/UIxssj/Image/img_paiming_" + TO_STR(groupRankData->at(i).rank) + ".png");
			Text* txtName = (Text*)pnlItem->getChildByName("text_GangName");
			txtName->setString(groupRankData->at(i).groupName);
			txtName->setFontName(FONT_FZZHENGHEI);
			setTextAddStroke(txtName, Color3B(0x7d, 0x3f, 0x1c), 2);

			Text* txtDamage = (Text*)pnlItem->getChildByName("Text_KillAndDamge");
			txtDamage->setString(TO_STR(groupRankData->at(i).killedBossNum) + "(" + groupRankData->at(i).damageVal + ")");
			txtDamage->setFontName(FONT_FZZHENGHEI);
		}
	}
	sGroupDamRank* myGroupRankData = pInstance->getMyGroupRankData();
	m_ui.imgMyGroupRank->setVisible(true);
	ImageView* imgMyGroupRank = (ImageView*)m_ui.imgMyGroupRank->getChildByName("Img_rank");
	Text* txtMyGroupRank = (Text*)m_ui.imgMyGroupRank->getChildByName("Lab_rank");
	if(myGroupRankData->rank < 4)
	{
		txtMyGroupRank->setVisible(false);
		imgMyGroupRank->setVisible(true);
		if(myGroupRankData->rank == 0)
		{
			imgMyGroupRank->setVisible(false);
			txtMyGroupRank->setVisible(true);
			txtMyGroupRank->setString(DataManager::getInstance()->searchCommonTexdtById(10031));
			txtMyGroupRank->setFontName(FONT_FZZHENGHEI);
		}
		else
		{
			imgMyGroupRank->loadTexture("Image/UIxssj/Image/img_paiming_" + TO_STR(myGroupRankData->rank) + ".png");
		}
	}
	else
	{
		txtMyGroupRank->setVisible(true);
		imgMyGroupRank->setVisible(false);
		txtMyGroupRank->setFontName(FONT_FZZHENGHEI);
		txtMyGroupRank->setString(TO_STR(myGroupRankData->rank));
		setTextAddStroke(txtMyGroupRank, Color3B::BLACK, 2);
	}
	Text* txtMyGroupName = (Text*)m_ui.imgMyGroupRank->getChildByName("Lab_groupName");
	txtMyGroupName->setFontName(FONT_FZZHENGHEI);
	txtMyGroupName->setString(MainModel::getInstance()->getMainParam()->mGroupName);
	Text* txtGroupDamage = (Text*)m_ui.imgMyGroupRank->getChildByName("Lab_damage");
	txtGroupDamage->setString(TO_STR(myGroupRankData->killedBossNum) + "(" + myGroupRankData->damageVal + ")");
	txtGroupDamage->setFontName(FONT_FZZHENGHEI);

	//个人排名，只显示前三 和 我的排名
	vector<sPlayerDamRank>* playerRankData = pInstance->getMainPlayerRankData();
	for(int i=0; i<3; i++)
	{
		m_vPlayerRank.at(i)->setVisible(false);
		if(i < playerRankData->size())
		{
			Layout* pnlItem = m_vPlayerRank.at(i);
			pnlItem->setVisible(true);

			ImageView* imgRank = (ImageView*)pnlItem->getChildByName("IMG_PersonalRanking");
			imgRank->loadTexture("Image/UIxssj/Image/img_paiming_" + TO_STR(playerRankData->at(i).rank) + ".png");
			Text* txtName = (Text*)pnlItem->getChildByName("Text_playerName");
			txtName->setString(playerRankData->at(i).playerName);
			txtName->setFontName(FONT_FZZHENGHEI);
			setTextAddStroke(txtName, Color3B(0x7d, 0x3f, 0x1c), 2);

			Text* txtDamage = (Text*)pnlItem->getChildByName("Text_Damge");
			txtDamage->setString(playerRankData->at(i).damageVal);
			txtDamage->setFontName(FONT_FZZHENGHEI);
		}
	}
	sPlayerDamRank* myRankData = pInstance->getMyRankData(eTYPE_ALL);
	m_ui.imgPlayerRank->setVisible(true);
	ImageView* imgMyRank = (ImageView*)m_ui.imgPlayerRank->getChildByName("Img_rank");
	Text* txtMyRank = (Text*)m_ui.imgPlayerRank->getChildByName("Lab_rank");
	if(myRankData->rank < 4)
	{
		txtMyRank->setVisible(false);
		imgMyRank->setVisible(true);
		if(myRankData->rank == 0)
		{
			//未上榜
			imgMyRank->setVisible(false);
			txtMyRank->setVisible(true);
			txtMyRank->setString(DataManager::getInstance()->searchCommonTexdtById(10031));
			txtMyRank->setFontName(FONT_FZZHENGHEI);
		}
		else
		{
			imgMyRank->loadTexture("Image/UIxssj/Image/img_paiming_" + TO_STR(myRankData->rank) + ".png");
		}
	}
	else
	{
		txtMyRank->setVisible(true);
		imgMyRank->setVisible(false);
		txtMyRank->setFontName(FONT_FZZHENGHEI);
		txtMyRank->setString(TO_STR(myRankData->rank));
		setTextAddStroke(txtMyRank, Color3B::BLACK, 2);
	}
	Text* txtMyName = (Text*)m_ui.imgPlayerRank->getChildByName("Lab_playerName");
	txtMyName->setFontName(FONT_FZZHENGHEI);
	txtMyName->setString(MainModel::getInstance()->getMainParam()->mName);
	Text* txtMyDamage = (Text*)m_ui.imgPlayerRank->getChildByName("Lab_damage");
	txtMyDamage->setString(myRankData->damageVal);
	txtMyDamage->setFontName(FONT_FZZHENGHEI);
}

void GroupBossLayer::updateBoxUI()
{
	GroupBossModel* pInstance = GroupBossModel::getInstance();
	vector<sBoxReward>* boxRewardData = pInstance->getBoxReward();
	for(int i=0; i<m_ui.vBtnBox.size(); i++)
	{
		Button* boxBtn = m_ui.vBtnBox.at(i);
		boxBtn->setVisible(true);
		if(i < boxRewardData->size())
		{
			sBoxReward boxData = boxRewardData->at(i);
			if(boxData.isCanGet)
			{
				boxBtn->loadTextures("Image/UIxssj/Image/img_baoxiang2_2_1.png", "Image/UIxssj/Image/img_baoxiang2_2_2.png");
				showCanGetAni(boxBtn);
			}
			else if(boxData.isGetted)
			{
				boxBtn->loadTextures("Image/UIxssj/Image/img_baoxiang2_3.png", "Image/UIxssj/Image/img_baoxiang2_3.png");	
				removeCanGetAni(boxBtn);
			}
			else
			{
				boxBtn->loadTextures("Image/UIxssj/Image/img_baoxiang2_1_1.png", "Image/UIxssj/Image/img_baoxiang2_1_2.png");
				removeCanGetAni(boxBtn);
			}
			Text* boxId = (Text*)boxBtn->getChildByName("BOX_ID");
			if(boxId == nullptr)
			{
				boxId = Text::create();
				boxId->setVisible(false);
				boxId->setName("BOX_ID");
				boxBtn->addChild(boxId);
			}
			boxId->setString(TO_STR(boxData.boxId));
			boxBtn->addTouchEventListener(CC_CALLBACK_2(GroupBossLayer::onBtnClick, this, BTN_BOX_REWARD));
		}
		else
		{
			boxBtn->setVisible(false);
		}
	}

	//宝箱进度条
	int curIdx = 0;
	for(int i=0; i<boxRewardData->size(); i++)
	{
		if(boxRewardData->at(i).isCanGet || boxRewardData->at(i).isGetted)
		{
			curIdx = i;
		}
		else
		{
			break;
		}
	}
	m_ui.barBoxReward->setPercent((float)(curIdx*100/4));

	//boss名字
	RowData* rowData = DataManager::getInstance()->searchMonsterById(pInstance->getCurRewardBossId());
	if(rowData != nullptr)
	{
		m_ui.txtBossName->setString(rowData->getStringData("name"));
		setTextAddStroke(m_ui.txtBossName, Color3B(0x82, 0x2b, 0x2b), 2);
	}
}

void GroupBossLayer::updateBossUI()
{
	//活动是否开启
	m_ui.imgActEnd->setVisible(!GroupBossModel::getInstance()->isActOpen());

	sGroupBossInfo* bossData = GroupBossModel::getInstance()->getBossInfo();

	m_ui.txtBossLv->setString(TO_STR(bossData->bossLv));
	m_ui.barBossHp->setPercent((float)(bossData->curHp*100/bossData->totalHp));
	m_ui.txtBossOrder->setString(TO_STR(bossData->bossOrder));
	m_ui._txtBossName->setString(bossData->bossName);

	//挑战次数
	Text* atkCount = (Text*)m_ui.btnAtk->getChildByName("Text_number");
	atkCount->setString("(" + TO_STR(GroupBossModel::getInstance()->getRestAtkCount()) + ")");

	showRedFlag();

	showBossLifes();

	showDownTime(0);

	//boss技能
	initSkillInfo();

	//boss待机动画
	showBossAni();

	//开启定时器
	openTimer();
}

void GroupBossLayer::updateInspireUI()
{
	GroupBossModel* pInstance = GroupBossModel::getInstance();

	m_ui.txtDamageAdd->setString(DataManager::getInstance()->searchCommonTexdtById(10163) + "+" + TO_STR((int)pInstance->getDamAdded()) + "%");
	setTextAddStroke(m_ui.txtDamageAdd, Color3B(0x4b, 0x07, 0x07), 2);

	Text* inspireCost = (Text*)m_ui.btnInspire->getChildByName("Text_Price");
	if(pInstance->getInspireCount() >= pInstance->getMaxInspireCount())
	{
		inspireCost->setString(DataManager::getInstance()->searchCommonTexdtById(10164));
	}
	else
	{
		inspireCost->setString(TO_STR(pInstance->getNextCost()));
	}
}

void GroupBossLayer::showDownTime(int dt)
{
	int& downTime = GroupBossModel::getInstance()->getRestTime();
	downTime -= dt;
	if(downTime <= 0)
	{
		//活动已结束，显示图片
		//m_ui.txtDownTime->setString(STR_UTF8("00时00分00秒"));
		m_ui.txtDownTime->setString("00" + DataManager::getInstance()->searchCommonTexdtById(10151) +
								    "00" + DataManager::getInstance()->searchCommonTexdtById(10152) +
									"00" + DataManager::getInstance()->searchCommonTexdtById(10153));
	}
	else
	{
		//隐藏图片
		m_ui.txtDownTime->setString(formartTime(downTime));
		setTextAddStroke(m_ui.txtDownTime, Color3B(0x09, 0x4e, 0x51), 2);
	}
}

void GroupBossLayer::showCoolTime(int dt)
{
	int& coolTime = GroupBossModel::getInstance()->getBossInfo()->reviveCoolTime;
	if(coolTime <= 0)
	{
		m_ui.pnlRevive->setVisible(false);
		m_ui.txtReviveTime->setString("");
		setTextAddStroke(m_ui.txtReviveTime, Color3B::BLACK, 2);
		GroupBossModel::getInstance()->setIsNeedUpdateCoolTime(true);

		showBossLifes();
		showBossAni();
	}
	else
	{	
		m_ui.pnlRevive->setVisible(true);
		m_ui.txtReviveTime->setString(formartTime(coolTime));
		setTextAddStroke(m_ui.txtReviveTime, Color3B::BLACK, 2);
		GroupBossModel::getInstance()->setIsNeedUpdateCoolTime(false);
		coolTime -= dt;
	}
}

void GroupBossLayer::timeTick(int dt)
{
	showDownTime(dt);
	showCoolTime(dt);
}

void GroupBossLayer::showBossAni()
{
	sGroupBossInfo* bossData = GroupBossModel::getInstance()->getBossInfo();
	int nResID = bossData->bossResId;
	if(bossData->reviveCoolTime > 0)
	{
		//冷却中，显示灰色静态图
		/*
		ImageView* imgBoss = (ImageView*)m_ui.pnlBossAni->getChildByName("BOSS_IMAGE");
		if(imgBoss == nullptr)
		{
			imgBoss = ImageView::create();
			imgBoss->setName("BOSS_IMAGE");
			m_ui.pnlBossAni->addChild(imgBoss);
		}
		imgBoss->loadTexture("Image/Icon/BossShow/" + TO_STR(bossData->bossResId) + ".png");
		imgBoss->setPosition(Point(m_ui.pnlBossAni->getContentSize()/2) + Point(40, 30));
		setImageGray(imgBoss, true);
		imgBoss->setVisible(true);
		*/

		//停止动画
		SpineAnimation* pSkelFighter = (SpineAnimation*)m_ui.pnlBossAni->getChildByName("HeroSpine");
		if(pSkelFighter == nullptr)
		{
			pSkelFighter = SpineAnimation::createFighterWithResId(nResID);
			pSkelFighter->setPosition(Point(m_ui.pnlBossAni->getContentSize().width/2, 0));
			pSkelFighter->setAnimation(0, "wait", true);
			m_ui.pnlBossAni->addChild(pSkelFighter, 1, "HeroSpine");
		}	
		pSkelFighter->stopAnimation();
		pSkelFighter->setColor(Color3B(0x44, 0x44, 0x44));

		if(m_ui.pnlBossAni->getChildByName("FrontAni") != nullptr)
		{
			m_ui.pnlBossAni->getChildByName("FrontAni")->setVisible(false);
		}
		if(m_ui.pnlBossAni->getChildByName("BackAni") != nullptr)
		{
			m_ui.pnlBossAni->getChildByName("BackAni")->setVisible(false);
		}
	}
	else
	{
		if(m_ui.pnlBossAni->getChildByName("HeroSpine") == nullptr || m_iBossResId != nResID)
		{
			m_ui.pnlBossAni->removeChildByName("HeroSpine");
			SpineAnimation* pSkelFighter = SpineAnimation::createFighterWithResId(nResID);
			pSkelFighter->setPosition(Point(m_ui.pnlBossAni->getContentSize().width/2, 0));
			pSkelFighter->setAnimation(0, "wait", true);
			m_ui.pnlBossAni->addChild(pSkelFighter, 1, "HeroSpine");
		}

		//隐藏静态图
		if(m_ui.pnlBossAni->getChildByName("HeroSpine") != nullptr)
		{
			//m_ui.pnlBossAni->getChildByName("HeroSpine")->setVisible(true);

			SpineAnimation* heroSpine = (SpineAnimation*)(m_ui.pnlBossAni->getChildByName("HeroSpine"));
			heroSpine->resumeAnimation();
			heroSpine->setColor(Color3B::WHITE);
		}
		if(m_ui.pnlBossAni->getChildByName("BOSS_IMAGE") != nullptr)
		{
			m_ui.pnlBossAni->getChildByName("BOSS_IMAGE")->setVisible(false);
		}
		if(m_ui.pnlBossAni->getChildByName("FrontAni") != nullptr)
		{
			m_ui.pnlBossAni->getChildByName("FrontAni")->setVisible(true);
		}
		if(m_ui.pnlBossAni->getChildByName("BackAni") != nullptr)
		{
			m_ui.pnlBossAni->getChildByName("BackAni")->setVisible(true);
		}
	}


	m_iBossResId = nResID;
}

void GroupBossLayer::initSkillInfo()
{
	sGroupBossInfo* bossData = GroupBossModel::getInstance()->getBossInfo();
	RowData* pRowPetData = DataManager::getInstance()->searchCharacterById(bossData->bossResId);

	for(int i=0; i<m_ui.imgSkillBg.size(); i++)
	{
		ImageView* skillImg = (ImageView*)(m_ui.imgSkillBg.at(i)->getChildByName("Img_skillIcon"));
		string skillType = "";
		Color3B skillColor = Color3B::WHITE;
		if(i == 0)
		{
			skillType = "atkID";
			skillColor = Color3B(231, 219, 169);
		}
		else if(i == 1)
		{
			skillType = "a_sklID";
			skillColor = Color3B(240, 119, 184);
		}
		else if(i == 2)
		{
			skillType = "p1_sklID";
			skillColor = Color3B(244, 188, 234);
		}
		else if(i == 3)
		{
			skillType = "p2_sklID";
			skillColor = Color3B(182, 225, 243);
		}
		else if(i == 4)
		{
			skillType = "p3_sklID";
			skillColor = Color3B(174, 231, 169);
		}
		skillImg->setTouchEnabled(true);
		skillImg->addTouchEventListener(CC_CALLBACK_2(GroupBossLayer::onBtnSkillClick,this,i,skillType,skillColor));
		
		setBossSkill(i,pRowPetData,skillType);
	}
}

void GroupBossLayer::setBossSkill(const int idx,RowData* pRowPetData,const string& str)
{
	RowData* pRowSkillData = DataManager::getInstance()->searchSkillById(pRowPetData->getIntData(str));
	if (pRowSkillData && idx < m_ui.imgSkillBg.size())
	{
		ImageView* skillIcon = (ImageView*)m_ui.imgSkillBg.at(idx)->getChildByName("Img_skillIcon");
		skillIcon->loadTexture(ResourceUtils::getSmallIconPath(pRowSkillData->getIntData("resId")));
	}
}

void GroupBossLayer::onBtnSkillClick( Ref* ref, Widget::TouchEventType type, int idx,const string& str,Color3B color )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");

		int bossId = GroupBossModel::getInstance()->getBossInfo()->bossResId;
		GroupBossSkillTips* oldTipsLayer = (GroupBossSkillTips*)m_ui.pRoot->getChildByName("SKILL_TIPS");
		if(oldTipsLayer == nullptr)
		{		
			oldTipsLayer = GroupBossSkillTips::create(bossId, idx, str, color);
			oldTipsLayer->setName("SKILL_TIPS");
			m_ui.pRoot->addChild(oldTipsLayer, 2);
		}
		else
		{
			
			if(idx != oldTipsLayer->getSkillIdx())
			{
				GroupBossSkillTips* newTipsLayer = GroupBossSkillTips::create(bossId, idx, str, color);
				newTipsLayer->setName("SKILL_TIPS");
				oldTipsLayer->removeFromParent();
				m_ui.pRoot->addChild(newTipsLayer, 2);
			}
			else
			{
				oldTipsLayer->removeFromParent();	
			}
		}
	}
}

void GroupBossLayer::updateMainUI(float dt)
{
	GroupBossController::sendGetMainInfoMsg();
}

void GroupBossLayer::showBossBgAni()
{
	//粒子效果
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniCharaparticle/AniCharaparticle.ExportJson");
	Armature* particleFrontAni = Armature::create("AniCharaparticle");
	particleFrontAni->setPosition(Point(m_ui.pnlBossAni->getContentSize()/2));
	particleFrontAni->setVisible(false);
	m_ui.pnlBossAni->addChild(particleFrontAni, 99, "FrontAni");
	Armature* particleBackAni = Armature::create("AniCharaparticle");
	particleBackAni->setPosition(Point(m_ui.pnlBossAni->getContentSize().width/2, 28));
	particleBackAni->setVisible(false);
	m_ui.pnlBossAni->addChild(particleBackAni, -99, "BackAni");

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

void GroupBossLayer::showBossLifes()
{
	GroupBossModel* pInstance = GroupBossModel::getInstance();
	sGroupBossInfo* bossData = pInstance->getBossInfo();

	int restReviveTimes = bossData->reviveCoolTime > 0 ? pInstance->getRestLife() : bossData->restReviveTimes;

	//boss重生图片
	if(restReviveTimes <= m_ui.imgBossLife.size())
	{
		//鲜活的生命
		for(int i=0; i<restReviveTimes; i++)
		{
			m_ui.imgBossLife.at(i)->loadTexture("Image/UI_bangpaiBOSS/Image/img_icon_xin1.png");
		}

		//死掉的生命
		for(int i=restReviveTimes; i<m_ui.imgBossLife.size(); i++)
		{
			m_ui.imgBossLife.at(i)->loadTexture("Image/UI_bangpaiBOSS/Image/img_icon_xin2.png");
		}

		//最后一个boss最后一条命是个不死鸟
		if(bossData->bossOrder >= bossData->totalBossNum)
		{
			m_ui.imgBossLife.front()->loadTexture("Image/UI_bangpaiBOSS/Image/img_icon_xin0.png");
		}
	}
}

void GroupBossLayer::showRedFlag()
{
	m_ui.btnExchangeReward->getChildByName("NewReward")->setVisible(GroupBossModel::getInstance()->isCanGetDamReward());
}

void GroupBossLayer::showBossReward(void* data)
{
	vector<TOOL_NEW_INFO>* rewardDatas = (vector<TOOL_NEW_INFO>*)data;
	if(rewardDatas != nullptr)
	{
		vector<PopRewardItem*> vecRewardItem;
		vecRewardItem.clear();

		for(int i=0; i<rewardDatas->size(); i++)
		{		
			PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(rewardDatas->at(i).ntemplateId, rewardDatas->at(i).nstack);
			vecRewardItem.push_back(rewardItem);
		}

		PopReward::show(vecRewardItem);	
	}
}

void GroupBossLayer::showCanGetAni( Node* parentNode )
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

void GroupBossLayer::removeCanGetAni( Node* parentNode )
{
	parentNode->getParent()->removeChildByName(parentNode->getName() + "_Ani");
}

void GroupBossLayer::openTimer()
{
	GroupBossModel* pInstance = GroupBossModel::getInstance();
	int downTime = pInstance->getRestTime();
	int coolTime = pInstance->getBossInfo()->reviveCoolTime;

	if(downTime >0 || coolTime > 0)
	{
		setEndTimeStamp(LocalTimer::getInstance()->getCurServerTime() + (downTime > coolTime ? downTime : coolTime));
		LocalTimer::getInstance()->addLocalTimerListener(this);
	}
}

std::string GroupBossLayer::formartTime(int restTime)
{
	if(restTime <= 0)
	{
		//return STR_UTF8("00时00分00秒");
		return "00" + DataManager::getInstance()->searchCommonTexdtById(10151) +
			   "00" + DataManager::getInstance()->searchCommonTexdtById(10152) +
			   "00" + DataManager::getInstance()->searchCommonTexdtById(10153);
	}
	string strResTime;
	string ht;
	string mint;
	string sect;
	int h = restTime/3600;
	if(h <= 9)
	{
		ht = _TO_STR(0) + _TO_STR(h);
	}
	else
	{
		ht = _TO_STR(h);
	}
	int min = (restTime%3600)/60;
	if(min <= 9)
	{
		mint = _TO_STR(0) + _TO_STR(min);
	}
	else
	{
		mint = _TO_STR(min);
	}
	int sec = restTime%60;
	if(sec <= 9)
	{
		sect = _TO_STR(0) + _TO_STR(sec);
	}
	else
	{
		sect = _TO_STR(sec);
	}

	//strResTime = ht + STR_UTF8("时") + mint + STR_UTF8("分") + sect + STR_UTF8("秒");
	strResTime = ht + DataManager::getInstance()->searchCommonTexdtById(10151) 
				+ mint + DataManager::getInstance()->searchCommonTexdtById(10152) 
				+ sect + DataManager::getInstance()->searchCommonTexdtById(10153);

	return strResTime;
}

void GroupBossLayer::updateSelf( void* data )
{
	ObserverParam* obParam = (ObserverParam*) data;

	switch (obParam->id)
	{
	case nMAIN_CMD_GROUP_BOSS_MAIN:
		{
			updateUI();
		}
		break;
	case nMAIN_CMD_GROUP_BOSS_RANK:
		break;
	case nMAIN_CMD_GROUP_BOSS_INSPIRE:
		updateInspireUI();
		break;
	case nMAIN_CMD_GROUP_BOSS_GET_REWARD:
		showRedFlag();
		break;
	case nMAIN_CMD_GROUP_BOSS_GET_BOSS_REWARD:
		{
			if(obParam->updateData != nullptr)
			{
				showBossReward(obParam->updateData);
			}
			updateBoxUI();
		}
		break;
	default:
		break;
	}
}









