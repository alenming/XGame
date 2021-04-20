#include "MainInfoLayer.h"
#include "CocoStudio.h"
#include "MainModel.h"
#include "HeroModule/HeroModel.h"
#include "Utils/StringFormat.h"
#include "UI/UIDef.h"
#include "FormationModule/LayerFormation.h"
#include "HeroModule/LayerHero.h"
#include "HeroModule/SceneHero.h"
#include "ToolModule/LayerTool.h"
#include "ToolModule/SceneTool.h"
#include "LoveModule/LoveLayer.h"
#include "LoveModule/LoveScene.h"
#include "MazeModule/SceneMaze.h"
#include "MazeModule/LayerMaze.h"
#include "RobModule/SceneRob.h"
#include "Temp/CustomPop.h"
#include "VIPModule/VipScene.h"
#include "VIPModule/VipModel.h"
#include "GuideCenter/GuideManager.h"
#include "TalentModule/TalentLayer.h"
#include "LoginModule/LayerCreateRole.h"
#include "FriendModule/LayerChatMessage.h"
#include "FriendModule/LayerChat.h"
#include "MailModule/LayerMail.h"
#include "MainTaskModule/MainTaskLayer.h"
#include "MainTaskModule/MainTaskModel.h"
#include "ChapterModule/Activity/LayerActivity.h"
#include "SignModule/LayerSign.h"
#include "FightModule/SceneFightLoading.h"
#include "FightModule/FightController.h"
#include "StarsSuModule/StarsSuScene.h"
#include "PlayerInfoModule/PlayerInfoLayer.h"
#include "MarvellousActive/MarvellousActiveScene.h"
#include "FriendModule/LayerFriend.h"
#include "Widget/ActionCreator.h"
#include "NewAreaActivityModule/NewAreaActivityScene.h"
#include "Utils/SystemUtils.h"
#include "BossModule/BossScene.h"
#include "NewAreaActivityModule/NewAreaActivityModel.h"
#include "RankTopModule/RankTopScene.h"
#include "OperateActModule/OperateActScene.h"
#include "FirstRecharge/FirstRechargeLayer.h"
#include "FirstRecharge/FirstRechargeModel.h"
#include "BossModule/BossActStateListener.h"
#include "ToolModule/AniButton.h"
#include "GroupModule/MyGroupController.h"
#include "ArtifactModule/ArtifactScene.h"
#include "MazeModule/MazeModel.h"
#include "TacticsModule/SceneTactics.h"
#include "NewStarsCampModule/StarsCampScene.h"
#include "LevelUp/LevelUpModel.h"
#include "LimitTimeHeroModule/LimitTimeHeroScene.h"
#include "LimitTimeHeroModule/LimitTimeHeroModel.h"
#include "LimitTimeHeroModule/LimitTimeHeroDataDef.h"
#include "LimitTimeHeroModule/LimitTimeHeroController.h"
#include "GoldActivity/GoldActivityScene.h"
#include "GroupBossModule/GroupBossScene.h"
#include "MysteryShop/MysteryInterLayer.h"
#include "MysteryShop/MysteryShopModel.h"
#include "MysteryShop/MysteryShopScene.h"
#include "Widget/LayerHeroRelation.h"

MainInfoLayer::MainInfoLayer()
{
	MainModel::getInstance()->addObserver(this);
	HeroModel::getInstance()->addObserver(this);
	MainTaskModel::getInstance()->addObserver(this);
	LayerSignModel::getInstance()->addObserver(this);
	MailModel::getInstance()->addObserver(this);
	FriendModel::getInstance()->addObserver(this);
	NewAreaActivityModel::getInstance()->addObserver(this);
	FirstRechargeModel::getInstance()->addObserver(this);
	OperateActModel::getInstance()->addObserver(this);
	ChatController::getInstance()->addObserver(this);
	isFirst = true;
	BossActStateListener::getInstance();		//创建一次单例
	MyGroupController::getInstance();			//创建一次单例
	LevelUpModel::getInstance()->addObserver(this);
	LimitTimeHeroModel::getInstance()->addObserver(this);
	VipModel::getInstance()->addObserver(this);
	GoldActivityModel::getInstance()->addObserver(this);
	MysteryShopModel::getInstance()->addObserver(this);
}

MainInfoLayer::~MainInfoLayer()
{
	MainModel::getInstance()->removeObserver(this);
	HeroModel::getInstance()->removeObserver(this);
	MainTaskModel::getInstance()->removeObserver(this);
	LayerSignModel::getInstance()->removeObserver(this);
	MailModel::getInstance()->removeObserver(this);
	FriendModel::getInstance()->removeObserver(this);
	NewAreaActivityModel::getInstance()->removeObserver(this);
	FirstRechargeModel::getInstance()->removeObserver(this);
	OperateActModel::getInstance()->removeObserver(this);
	ChatController::getInstance()->removeObserver(this);
	LevelUpModel::getInstance()->removeObserver(this);
	LimitTimeHeroModel::getInstance()->removeObserver(this);
	VipModel::getInstance()->removeObserver(this);
	GoldActivityModel::getInstance()->removeObserver(this);
	MysteryShopModel::getInstance()->removeObserver(this);
}

bool MainInfoLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	mUI.root = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UImainui2/UImainui2_1.ExportJson"));

	mUI.root->setSize(size);
	mUI.root->setPosition(pos);
	// 将UILayer层加入到当前的场景
	this->addChild(mUI.root);
	mUI.root->setTouchEnabled(false);

	//主角头像部分信息
	Layout* topContainer = static_cast<Layout*>(mUI.root->getChildByName("Panel_top_container"));
	mUI.roleImg = static_cast<ImageView*>(topContainer->getChildByName("bg_juese"));
	mUI.roleImgPos = mUI.roleImg->getPosition();
	mUI.levelAtl = static_cast<TextAtlas*>(ui::Helper::seekWidgetByName(mUI.roleImg, "atl_dengji"));
	mUI.vipBtn = static_cast<Button*>(mUI.roleImg->getChildByName("btn_vip"));
	mUI.vipBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::VIP));
	mUI.nameText = static_cast<Text*>(mUI.roleImg->getChildByName("lab_nicheng"));
	mUI.nameText->setFontName(FONT_FZZHENGHEI);
	bool isMale = HeroModel::getInstance()->getMainHero()->mSex == HERO_SEX_TYPE::HERO_SEX_MALE;
	mUI.playerInfoBtn = static_cast<Button*>(mUI.roleImg->getChildByName("btn_playerInfo"));
	//默认是男主头像，如果是女主，更换头像
	if(!isMale)
	{
		mUI.playerInfoBtn->loadTextures("Image/UImainui2/NEW_UI2/UI_zhujiemian/img_nvzhujue.png", "Image/UImainui2/NEW_UI2/UI_zhujiemian/img_nvzhujue.png");
	}
	mUI.playerInfoBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::PLAYERINFO));
	mUI.fightingText = static_cast<Text*>(mUI.roleImg->getChildByName("lab_zhandouli"));
	mUI.fightingText->setFontName(FONT_FZZHENGHEI);

	//经验进度条
	LoadingBar* bar = static_cast<LoadingBar*>(mUI.roleImg->getChildByName("img_jingyantiao"));
	mUI.expBar = ProgressTimer::create(Sprite::create("Image/UImainui2/img_tiao_exp_1.png"));
	mUI.expBar->setType(ProgressTimer::Type::RADIAL);
	mUI.expBar->setReverseProgress(true);
	mUI.expBar->setMidpoint(Vec2(0.5f, 0.6f));
	mUI.expBar->setBarChangeRate(Vec2(1, 0));
	mUI.expBar->setPosition(this->getContentSize() / 2);
	mUI.expBar->setPosition(bar->getPosition());
	mUI.roleImg->addChild(mUI.expBar);

	//体力、精力、金币、银币
	ImageView* powBackImg = static_cast<ImageView*>(mUI.roleImg->getChildByName("img_tili_tiao"));
	powBackImg->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::ADDPOWER));
	ImageView* energyBackImg = static_cast<ImageView*>(mUI.roleImg->getChildByName("img_jinli_tiao"));
	energyBackImg->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::ADDENERGY));
	ImageView* goldBackImg = static_cast<ImageView*>(mUI.roleImg->getChildByName("img_jinbi_tiao"));
	goldBackImg->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::ADDGOLD));
	ImageView* silverBackImg = static_cast<ImageView*>(mUI.roleImg->getChildByName("img_yinbi_tiao"));
	silverBackImg->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::ADDSILVER));
	mUI.curPowText = static_cast<Text*>(powBackImg->getChildByName("lab_tili2"));
	mUI.curEnergyText = static_cast<Text*>(energyBackImg->getChildByName("lab_jinli2"));
	mUI.goldText = static_cast<Text*>(goldBackImg->getChildByName("lab_jinbi2"));
	mUI.silverText = static_cast<Text*>(silverBackImg->getChildByName("lab_yinbi2"));
	mUI.curPowText->setFontName(FONT_FZZHENGHEI);
	mUI.curEnergyText->setFontName(FONT_FZZHENGHEI);
	mUI.goldText->setFontName(FONT_FZZHENGHEI);
	mUI.silverText->setFontName(FONT_FZZHENGHEI);

	//右边按钮
	mUI.leftBackImg = static_cast<ImageView*>(mUI.root->getChildByName("img_youce"));
	mUI.leftBackImgPos = mUI.leftBackImg->getPosition();
	mUI.heroBtn = static_cast<Button*>(mUI.leftBackImg->getChildByName("btn_yingxiong"));
	mUI.heroBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::HERO));
	mUI.formationBtn = static_cast<Button*>(mUI.leftBackImg->getChildByName("btn_zhenxing"));
	mUI.formationBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::FORMATION));
	mUI.packBtn = static_cast<Button*>(mUI.leftBackImg->getChildByName("btn_beibao"));
	mUI.packBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::PACK));
	mUI.taskBtn = static_cast<Button*>(mUI.leftBackImg->getChildByName("btn_renwu"));
	mUI.taskBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::TASK));
	mUI.rankTopBtn = static_cast<Button*>(mUI.leftBackImg->getChildByName("btn_paihang"));
	mUI.rankTopBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::RANKTOP));
	mUI.dequeBtn = static_cast<Button*>(mUI.leftBackImg->getChildByName("btn_duiwu"));
	mUI.dequeBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::DEQUE));
	mUI.jiantou = static_cast<ImageView*>(mUI.dequeBtn->getChildByName("Image_jiantou")); 
	mUI.jiantouPos = mUI.jiantou->getPosition();
	Layout *Panel_clip = static_cast<Layout*>(mUI.leftBackImg->getChildByName("Panel_clip"));
	Panel_clip->setClippingEnabled(true);
	mUI.dequeDi = static_cast<Layout*>(Panel_clip->getChildByName("Panel_Move"));
	mUI.dequeDiPos = mUI.dequeDi->getPosition();
	mUI.zhenFaBtn = static_cast<Button*>(mUI.dequeDi->getChildByName("Button_zhenfa"));
	mUI.zhenFaBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::ZHENFA));
	mUI.shenQiBtn = static_cast<Button*>(mUI.dequeDi->getChildByName("Button_shenqi"));
	mUI.shenQiBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::SHENQI));
	mUI.starPanleBtn = static_cast<Button*>(mUI.dequeDi->getChildByName("Button_xingpan"));
	mUI.starPanleBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::STARTPAN));

	//好友
	mUI.friendBtn = static_cast<Button*>(mUI.root->getChildByName("btn_haoyou"));
	mUI.friendBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::FRIEND));
	if (!FriendModel::getInstance()->getIsLoad()) //好友提示
	{
		FriendController::getInstance()->onRequestFriendList();
	}
	else
	{
		setFriendFlag(false);
	}

	//设置
	mUI.setBtn = static_cast<Button*>(mUI.root->getChildByName("btn_shezhi"));
	mUI.setBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::SETGAME));
	//聊天
	mUI.chatBtn = static_cast<Button*>(mUI.root->getChildByName("btn_liaotian"));
	mUI.chatBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::CHAT));
	//邮件
	mUI.mailBtn = static_cast<Button*>(mUI.root->getChildByName("btn_youjian"));
	mUI.mailBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::MAIL));
	if (!MailModel::getInstance()->isLoaded()) //邮件提示
	{
		MailController::getInstance()->onGetMails();
	}
	else
	{
		setMailFlag();
	}
	//签到提示
	mUI.signBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(mUI.root,"btn_qiandao"));
	mUI.signBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::REGISTER));
	if (!LayerSignModel::getInstance()->getIsLoad()) 
	{
		LayerSignController::getInstance()->sendGetSignList();
	}
	else
	{
		setSignFlag();
	}

	//功能开放按钮
	mUI.tipsDetailsPnl = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UImainui2/UImainui2_2.ExportJson"));
	mUI.tipsDetailsPnl->setVisible(false);
	this->addChild(mUI.tipsDetailsPnl);
	mUI.funcOpenImg = static_cast<ImageView*>(mUI.root->getChildByName("Img_newFunc"));
	mUI.funcOpenBtn = (Button*)mUI.funcOpenImg->getChildByName("Btn_newFunc");
	mUI.openTipsText = (Text*)mUI.funcOpenImg->getChildByName("Lab_open");
	mUI.openTipsText->setFontName(FONT_FZZHENGHEI);
	mUI.openTipsText->setString("");

	//判定星盘红点
	auto  isopencamp = UserDefault::getInstance()->getIntegerForKey("isopencamp");
	if (isopencamp != 0 )
	{
		auto duiwudian = static_cast<ImageView*>(mUI.dequeBtn->getChildByName("dian"));
		auto xinpandiang = static_cast<ImageView*>(mUI.starPanleBtn->getChildByName("dian"));
		if (duiwudian && xinpandiang)
		{
			duiwudian->setVisible(true);
			xinpandiang->setVisible(true);
		}
	}
	
	allActivityButton();
	setUserInfo();
	setHeroAdvFlag();
	setTaskFlag();
	setFuncState();
	addChatMsgItem();	
	return true;
}

void MainInfoLayer::allActivityButton()
{
	//中间活动图标
	Layout* Icon = dynamic_cast<Layout*>(Helper::seekWidgetByName(mUI.root,"icon"));
	m_allActivityPos.clear();
	m_allActivityShowBtn.clear();
	sort(Icon->getChildren().begin(),Icon->getChildren().end(),[=](Node* a,Node* b)
	{
		return a->getTag() < b->getTag();
	});

	for (auto iter = Icon->getChildren().begin(); iter != Icon->getChildren().end(); iter++)
	{
		Button* btn = dynamic_cast<Button*>(*iter);
		if (btn)
		{
			m_allActivityPos.push_back(btn->getPosition());

			SHOWE_ACT_NODE item;
			item.node = btn;
			item.visible = false;
			m_allActivityShowBtn.push_back(item);
		}
	}

	sort(m_allActivityPos.begin(),m_allActivityPos.end(),[=](Vec2 a,Vec2 b)
	{
		return a.x < b.x;
	});

	//首充状态
	mUI.firstRechargeBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(Icon,"btn_shouchong"));
	AniButton::create(mUI.firstRechargeBtn,"Anishouchonglibao",CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::FIRSTRECHARGE));
	setActivityButtonVisible(mUI.firstRechargeBtn,false);
	switch (FirstRechargeModel::getInstance()->mFirstRechargeState)
	{
	case 0:
		//首充还在，不可领取
		{
			setActivityButtonVisible(mUI.firstRechargeBtn,true);
			mUI.firstRechargeBtn->getChildByName("Img_redDot")->setVisible(false);
		}
		break;
	case 1:
		//首充还在，可以领取
		{
			setActivityButtonVisible(mUI.firstRechargeBtn,true);
			mUI.firstRechargeBtn->getChildByName("Img_redDot")->setVisible(true);
		}
		break;
	case 2:
		//首充领完
		setActivityButtonVisible(mUI.firstRechargeBtn,false);
		break;
	default:
		break;
	}

	//vip礼包
	mUI.chongzhi = dynamic_cast<Button*>(Helper::seekWidgetByName(Icon,"btn_chongzhianniu"));
	mUI.chongzhi->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::VIP));
	setActivityButtonVisible(mUI.chongzhi,true);
	if (!VipModel::getInstance()->getIsInitted())
	{
		VipController::getInstance()->sendGetVipListMsg();
	}
	else
	{
		setVipFlag();
	}

	//检测是否有运营活动领取
	mUI.depositBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(Icon,"btn_fulidating"));
	AniButton::create(mUI.depositBtn,"Anifulidating",CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::OPERATE));
	setActivityButtonVisible(mUI.depositBtn,true);
	OperateActController::getInstance()->getOprateSuppleMentTable();
	setOperateFlag();

	//是否显示开服奖励图标和其红点
	mUI.openBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(Icon,"btn_kuanghuan"));
	mUI.openBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::OPEN));
	setActivityButtonVisible(mUI.openBtn,false);	//默认隐藏，要不会闪一下
	if(!NewAreaActivityModel::getInstance()->getIsLoaded())
	{
		NewAreaActivityModel::getInstance()->setIsFristLoad(true);
		NewAreaActivityController::getInstance()->sendGetMainInfoMsg();
	}
	else
	{
		setActivityButtonVisible(mUI.openBtn,NewAreaActivityModel::getInstance()->getActStates());
		setActivityFlag();	
	}

	//为了防止限时神将按钮乱闪，需额外做判定
	mUI.limitTimeHeroBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(Icon,"btn_xianshishenjiang"));
	mUI.limitTimeHeroBtn->getChildByName("Img_redDot")->setZOrder(99);
	//mUI.limitTimeHeroBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::LIMITTIMEHERO));
	AniButton::create(mUI.limitTimeHeroBtn,"Anixianshishenjiang",CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::LIMITTIMEHERO));
	//限时神将小红点
	setLimitTimeHeroFlag();
	//获取限时神将活动状态
	if(!LimitTimeHeroModel::getInstance()->isGettedTableData())
	{
		m_pController.sendGetTableDataMsg();
	}
	if(!LimitTimeHeroModel::getInstance()->isGettedTableData())
	{
		setActivityButtonVisible(mUI.limitTimeHeroBtn,false);
	}
	else
	{
		sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
		if(actData != nullptr && (actData->actState == eSTATE_OPEN || actData->actState == eSTATE_SHOW))
		{
			setActivityButtonVisible(mUI.limitTimeHeroBtn,true);
		}
		else
		{
			setActivityButtonVisible(mUI.limitTimeHeroBtn,false);
		}
	}

	//天降金币
	mUI.tianjiangjinbi = dynamic_cast<Button*>(Helper::seekWidgetByName(Icon,"btn_tianjiangjinbi"));
	mUI.tianjiangjinbi->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::TIANJIANGJINBI));
	setActivityButtonVisible(mUI.tianjiangjinbi,false);
	if (!GoldActivityModel::getInstance()->isLoaded()) //邮件提示
	{
		GoldActivityController::getInstance()->getTableInfo();
	}
	else
	{
		if (GoldActivityModel::getInstance()->getOpenTimeSec() == 0)
		{
			setActivityButtonVisible(mUI.tianjiangjinbi,true);
			setGoldActiviryFlag();
		}
		else
		{
			setActivityButtonVisible(mUI.tianjiangjinbi,false);
		}
	}

	//神秘商店
	mUI.steryShop = dynamic_cast<Button*>(Helper::seekWidgetByName(Icon,"btn_steryShop"));
	mUI.steryShop->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::STERYSHOP));
	mUI.steryShopTiem = static_cast<Text*>(mUI.steryShop->getChildByName("Label_Time"));
	
	if (MysteryShopModel::getInstance()->getIsMainShow() && !MysteryShopModel::getInstance()->getIsInterShow())
	{
		mUI.steryShopTiem->setVisible(true);
		setActivityButtonVisible(mUI.steryShop,true);
		auto stamp = MysteryShopModel::getInstance()->getDaoTime();
		mUI.steryShopTiem->setString(formatTime(stamp));
		mUI.steryShopTiem->setFontName(FONT_FZZHENGHEI);
		mUI.steryShopTiem->setColor(Color3B(0xd8, 0xff, 0x61));
		setTextAddStroke(mUI.steryShopTiem , Color3B(0x0c,0x50,0x1a), 2);
	}
	else
	{
		setActivityButtonVisible(mUI.steryShop,false);
	}
}

void MainInfoLayer::setActivityButtonVisible(Button * sender, bool visible)
{
	for (auto iter = m_allActivityShowBtn.begin(); iter != m_allActivityShowBtn.end(); iter++)
	{
		if (iter->node == sender)
		{
			if (iter->visible == visible)
			{
				return;
			}
			else
			{
				iter->visible = visible;
				break;
			}
		}
	}

	ShowActivityButton();
}

void MainInfoLayer::ShowActivityButton()
{
	int idx = 0;
	for (auto iter = m_allActivityShowBtn.begin(); iter != m_allActivityShowBtn.end(); iter++)
	{
		if (iter->visible)
		{
			iter->node->setVisible(true);
			iter->node->setPosition(m_allActivityPos.at(idx));
			idx++;
		}
		else
		{
			iter->node->setVisible(false);
		}
	}
}

void MainInfoLayer::setUserInfo()
{
	MainParam* pMainParam = MainModel::getInstance()->getMainParam();
	HeroParam* pHeroParam = HeroModel::getInstance()->getMainHero();

	if (pHeroParam)
	{
		mUI.levelAtl->setString(TO_STR(pHeroParam->mLevel));
		string str = STR_ANSI(pHeroParam->getRealName());
		mUI.nameText->setString(pHeroParam->getRealName());
		mUI.fightingText->setString(TO_STR(HeroModel::getInstance()->getTotalFighting()));

		RowData* expData = DataManager::getInstance()->searchHeroExp(pHeroParam->mLevel);
		int maxExp = expData->getIntData("expRequired");
		int curExp = pHeroParam->mExp;
		int min = 13, max = 66;
		mUI.expBar->setPercentage(min + (max - min) * (curExp / (float)maxExp));
	}

	if (pMainParam)
	{
		mUI.goldText->setString(StringFormat::formatNumber(pMainParam->mGold));
		mUI.silverText->setString(StringFormat::formatNumber(pMainParam->mCoin));
		mUI.curPowText->setString(TO_STR(pMainParam->mPower) + "/" + TO_STR(pMainParam->mMaxPower));
		mUI.curEnergyText->setString(TO_STR(pMainParam->mEnergy) + "/" + TO_STR(pMainParam->mMaxEnergy));
		mUI.vipBtn->loadTextureNormal("Image/Icon/vip/btn_VIP" + TO_STR(pMainParam->mVipLevel) + ".png");
	}
}

//设置角色可进阶标志
void MainInfoLayer::setHeroAdvFlag()
{
	bool advFlag = false;
	Vector<HeroParam*>* vecHeroList = HeroModel::getInstance()->getHeroList();
	for (auto iter = vecHeroList->begin(); iter != vecHeroList->end(); iter++)
	{
		HeroParam* heroParam = *iter;
		//英雄可进阶，或者装备可进阶，或者有装备可穿戴，或者有可招募英雄，显示小红点
		if ((heroParam->isHeroAdvValid() && heroParam->isOnBattle())
			|| (heroParam->isHeroEquipAdvValid() && heroParam->isOnBattle())
			|| heroParam->isHeroEquipPutOn()
			|| HeroModel::getInstance()->hasRecruitableHero() 
			|| (heroParam->isHeroStarUpValid() && heroParam->isOnBattle()))
		{
			advFlag = true;
			break;
		}
	}
	//设置角色可进阶标志
	Node* pAdvIcon = mUI.heroBtn->getChildByName("adv_lab");
	if (pAdvIcon == nullptr)
	{
		pAdvIcon = cocos2d::ui::ImageView::create("Image/Icon/global/img_dian.png");
		pAdvIcon->setName("adv_lab");
		pAdvIcon->setPosition(cocos2d::Vec2(mUI.heroBtn->getContentSize().width - pAdvIcon->getContentSize().width/2 - 10,
			mUI.heroBtn->getContentSize().height - pAdvIcon->getContentSize().height/2 - 10));// 
		mUI.heroBtn->addChild(pAdvIcon, 9);		
	}
	pAdvIcon->setVisible(advFlag);
}

void MainInfoLayer::setTaskFlag()
{
	MainTaskModel* taskModel = MainTaskModel::getInstance();

	Node* taskFlag = mUI.taskBtn->getChildByName("task_flg");
	if (!taskFlag)
	{
		taskFlag = cocos2d::ui::ImageView::create("Image/Icon/global/img_dian.png");
		taskFlag->setPosition(cocos2d::Vec2(mUI.taskBtn->getContentSize().width - taskFlag->getContentSize().width/2 - 10, 
			mUI.taskBtn->getContentSize().height - taskFlag->getContentSize().height/2 - 10));
		taskFlag->setName("task_flg");
		mUI.taskBtn->addChild(taskFlag, 9);
	}
	//任务功能未开放不显示小红点
	bool active = GuideManager::getInstance()->isFunctionOpen(FUNCTION_TASK, false);
	taskFlag->setVisible(taskModel->hasActivedTask() && active);
}

void MainInfoLayer::setSignFlag()
{
	Vector<LayerSignParam*>* mLayerSignParam = LayerSignModel::getInstance()->getSignParam();
	bool hasAttach = false;
	for (int i = 0; i < mLayerSignParam->size(); i++)
	{
		if (mLayerSignParam->at(i)->stat == SIGN_STAT_TYPE_CAN)
		{
			hasAttach = true;
			break;
		}
	}

	ImageView* rewardFlag = (ImageView*)mUI.signBtn->getChildByName("Img_redDot");
	rewardFlag->setVisible(hasAttach ? true : false);
}



void MainInfoLayer::setMailFlag()
{
	const MAIL_INFOS_DEQUE& mailList = MailModel::getInstance()->getMails();
		
	bool hasAttach = false;
	for (auto& iter:mailList)
	{
		if (iter.vAccessory.size() > 0  && MAIL_STAT_GOT > iter.stat)
		{
			hasAttach = true;
			break;
		}
	}
	Node* mailFlag = mUI.mailBtn->getChildByName("Img_redDot");
	if (hasAttach)
	{
		mailFlag->setVisible(true);
	}
	else
	{
		mailFlag->setVisible(false);
	}
}

void MainInfoLayer::setGoldActiviryFlag()
{
	Node* goldFlag = mUI.tianjiangjinbi->getChildByName("Img_redDot");
	if (!goldFlag)
	{
		goldFlag = cocos2d::ui::ImageView::create("Image/Icon/global/img_dian.png");
		goldFlag->setPosition(cocos2d::Vec2(mUI.tianjiangjinbi->getContentSize().width - goldFlag->getContentSize().width/2, 
			mUI.tianjiangjinbi->getContentSize().height - goldFlag->getContentSize().height/2));
		goldFlag->setName("mail_flg");
		mUI.tianjiangjinbi->addChild(goldFlag, 9);
	}

	if (GoldActivityModel::getInstance()->isHaveChange())
	{
		goldFlag->setVisible(true);
	}
	else
	{
		goldFlag->setVisible(false);
	}
}

void MainInfoLayer::setLimitTimeHeroFlag()
{
	//有免费抽或者有可以领取的积分宝箱，显示小红点
	mUI.limitTimeHeroBtn->getChildByName("Img_redDot")->setVisible(false);
	LimitTimeHeroModel* pInstance = LimitTimeHeroModel::getInstance();
	if(pInstance->isCanBuyFree() || pInstance->isCanGetScoreBox())
	{
		mUI.limitTimeHeroBtn->getChildByName("Img_redDot")->setVisible(true);
	}
}

void MainInfoLayer::setChatFlag()
{
	//有私聊消息未读
	if (!FriendModel::getInstance()->getHavePrivateChatMessagesNotRead())
	{
		Node* chatFlag = mUI.chatBtn->getChildByName("Img_redDot");
		chatFlag->setVisible(false);
	}
	else
	{
		Node* chatFlag = mUI.chatBtn->getChildByName("Img_redDot");
		chatFlag->setVisible(true);
	}
}

void MainInfoLayer::setFriendFlag(bool first)
{
	bool hasAttach = false;

	if (first)
	{
		const ADD_FRIEND_INFO_LIST& Applyfriends = FriendModel::getInstance()->getApplyfriends();
		if (Applyfriends.size() > 0)
		{
			hasAttach = true;
			FriendModel::getInstance()->getHaveFriend() = true;
		}
	}
	else
	{
		if (FriendModel::getInstance()->getHaveFriend() || FriendModel::getInstance()->getHaveNews())
		{
			hasAttach = true;
		}
	}
	
	Node* FriendFlag = mUI.friendBtn->getChildByName("Img_redDot");
	if (hasAttach)
	{
		FriendFlag->setVisible(true);
	}
	else
	{
		FriendFlag->setVisible(false);
	}
}

void MainInfoLayer::setActivityFlag()
{
	ImageView* rewardFlag = (ImageView*)mUI.openBtn->getChildByName("Img_redDot");
	rewardFlag->setVisible(NewAreaActivityModel::getInstance()->isHaveCanGetReward() ? true : false);
}

void MainInfoLayer::setOperateFlag()
{
	ImageView* rewardFlag = (ImageView*)mUI.depositBtn->getChildByName("Img_redDot");
	rewardFlag->setVisible(OperateActModel::getInstance()->isHaveGet() ? true : false);
}

void MainInfoLayer::setVipFlag()
{
	mUI.chongzhi->getChildByName("Img_dian")->setVisible(VipModel::getInstance()->isCanGetVipBox());
}

void MainInfoLayer::onEnter()
{
	Layer::onEnter();

	//进入动画
	mUI.roleImg->setPosition(mUI.roleImgPos + Point(0, 150));
	mUI.roleImg->runAction(EaseSineOut::create(MoveTo::create(0.25f, mUI.roleImgPos)));

	for (auto iter = m_allActivityShowBtn.begin(); iter != m_allActivityShowBtn.end(); iter++)
	{
		if (iter->visible)
		{
			ActionCreator::runCommBtnAction(iter->node);
		}
	}

	//切换进来右下方收起
	isDuiWuCanRun = true;
	isDuiWuOpen = false;
	mUI.dequeDi->setPosition(Vec2(mUI.dequeDi->getContentSize().width,mUI.dequeDiPos.y));
	mUI.jiantou->setPosition(mUI.jiantouPos);
	mUI.jiantou->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5f,Vec2(2,0)),
		CCMoveBy::create(0.5f,Vec2(-2,0)),nullptr)));

	setHeroAdvFlag();
	setVipFlag();

	//下一帧
	auto callBack = [this](float dt)->void
	{
		setFuncState();
		this->getScheduler()->unschedule("MainInfoLayer::setFuncState", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "MainInfoLayer::setFuncState");
}

void MainInfoLayer::onExit()
{
	Layer::onExit();
}

void MainInfoLayer::updateSelf( void* data )
{

	ObserverParam* observerParam = (ObserverParam*) data;

	if (observerParam->self == LayerSignModel::getInstance())
	{
		setUserInfo();

		setSignFlag();
		if (observerParam->id == SIGN_EVENT_RECEIVE)
		{
			//签到获得道具需刷新英雄可进阶标记
			setHeroAdvFlag();
		}
	}
	else if(observerParam->self == MailModel::getInstance())
	{
		setUserInfo();
		setFuncState();

		setMailFlag();
	}
	else if (observerParam->self == HeroModel::getInstance())
	{
		setUserInfo();

		setOperateFlag();
	}
	else if(observerParam->self == FriendModel::getInstance())
	{
		setUserInfo();

		if (isFirst)
		{
			setFriendFlag(true);
			isFirst = false;
		}
		else
		{
			setFriendFlag(false);
		}
	}
	else if(observerParam->self == NewAreaActivityModel::getInstance())
	{
		setUserInfo();
		setActivityButtonVisible(mUI.openBtn,NewAreaActivityModel::getInstance()->getActStates());
		setActivityFlag();
	}
	else if(observerParam->self == OperateActModel::getInstance())
	{
		setUserInfo();

		switch (observerParam->id)
		{
		case OPRATEACT_EVENT_TYPE_GETTABLE:
			{
				OperateActController::getInstance()->getOprateActInfo();
			}
			break;
		case OPRATEACT_EVENT_TYPE_INFO:
			{
				setOperateFlag();
			}
			break;
		}
	}
	else if(observerParam->self == FirstRechargeModel::getInstance())
	{
		setUserInfo();

		//首充状态
		switch (FirstRechargeModel::getInstance()->mFirstRechargeState)
		{
		case 0:
			{
				setActivityButtonVisible(mUI.firstRechargeBtn,true);
				mUI.firstRechargeBtn->getChildByName("Img_redDot")->setVisible(false);
			}
			break;
		case 1:
			{
				setActivityButtonVisible(mUI.firstRechargeBtn,true);
				mUI.firstRechargeBtn->getChildByName("Img_redDot")->setVisible(true);
			}
			break;
		case 2:
			setActivityButtonVisible(mUI.firstRechargeBtn,false);
			break;
		default:
			break;
		}
	}
	else if(observerParam->self == MainTaskModel::getInstance())
	{
		setUserInfo();

		switch (observerParam->id)
		{
		case TASK_EVENT_RECEIVE:
			{
				setTaskFlag();
				//任务领取获得新道具需更新英雄可进阶标记
				setHeroAdvFlag();
				//更新图标的开放状态提示
			}
			break;
		case TASK_EVENT_UPDATE:
		case TASK_EVENT_INIT:
			setTaskFlag();
			break;
		default:
		break;
		}
	}
	else if (observerParam->self == ChatController::getInstance())
	{
		setChatFlag();
	}
	else if(observerParam->self == LevelUpModel::getInstance())
	{
		setUserInfo();
		setFuncState();

		//每次升级会通知到这
		switch (observerParam->id)
		{
		case NOTICE_MAINGBUILDING:
			{
				setNewStartCampFlage();
			}
			break;
		default:
			break;
		}
	}
	else if(observerParam->self == LimitTimeHeroModel::getInstance())
	{
		if(observerParam->id == LIMIT_TIME_HERO_ACT_SHOW_NO)
		{
			//隐藏活动图标
			setActivityButtonVisible(mUI.limitTimeHeroBtn,false);
		}
		else if(observerParam->id == LIMIT_TIME_HERO_ACT_SHOW_YES)
		{
			//显示活动图标
			setActivityButtonVisible(mUI.limitTimeHeroBtn,true);

			//先隐藏小红点
			mUI.limitTimeHeroBtn->getChildByName("Img_redDot")->setVisible(false);

			//为了显示坑爹的小红点
			LimitTimeHeroController::sendGetMainInfoMsg();
		}
		else if(observerParam->id == nMAIN_CMD_LIMITTIMEHERO_INFO)
		{
			setLimitTimeHeroFlag();
		}
	}
	else if(observerParam->self == VipModel::getInstance())
	{
		setUserInfo();

		if(observerParam->id == nMAIN_CMD_VIP_GET_LIST)
		{
			setVipFlag();
		}	
	}
	else if (observerParam->self == GoldActivityModel::getInstance())
	{
		if (observerParam->id == GoldActivityNotify_Type_Tips)
		{
			if (GoldActivityModel::getInstance()->getOpenTimeSec() == 0)
			{
				setActivityButtonVisible(mUI.tianjiangjinbi,true);
				setGoldActiviryFlag();
			}
			else
			{
				setActivityButtonVisible(mUI.tianjiangjinbi,false);
			}
		}
	}
	else if (observerParam->self == MysteryShopModel::getInstance())
	{
		if (observerParam->id == MYSTERY_SHOW)
		{
			bool  isClose = ((vector<bool>*)(observerParam->updateData))->at(0);
			if (isClose == true)//关闭
			{
				setActivityButtonVisible(mUI.steryShop,false);
			}
		}
		if (observerParam->id == SHOP_EVENT_TYEP_COUNT_DOWN)
		{
			if (mUI.steryShopTiem)
			{
				setActivityButtonVisible(mUI.steryShop,true);
				mUI.steryShopTiem->setVisible(true);
				mUI.steryShopTiem->setString(formatTime(MysteryShopModel::getInstance()->getDaoTime()));
				mUI.steryShopTiem->setFontName(FONT_FZZHENGHEI);
				mUI.steryShopTiem->setColor(Color3B(0xd8, 0xff, 0x61));
				setTextAddStroke(mUI.steryShopTiem , Color3B(0x0c,0x50,0x1a), 2);
			}
		}
	}
	else 
	{
		setUserInfo();
		setFuncState();
	}
}

void MainInfoLayer::onBtnClick(Ref* ref, cocos2d::ui::Widget::TouchEventType type, MAIN_BTN_TAG btnFlag)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		//隔一帧再执行点击事件，防止重复创建
		auto callBack = [this, btnFlag](float dt)->void
		{
			jumpToScene(btnFlag);
			this->getScheduler()->unschedule("MainInfoLayer::onBtnClick", this);
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "MainInfoLayer::onBtnClick");
	}
}

void MainInfoLayer::jumpToScene(MAIN_BTN_TAG btnFlag)
{
	switch (btnFlag)
	{
	case MAIN_BTN_TAG::FORMATION:
		{
			Director::getInstance()->replaceScene(SceneFormation::create(F_MODE_FORMATION));
		}
		break;
	case MAIN_BTN_TAG::PACK:
		{
			Director::getInstance()->replaceScene(SceneTool::create());
		}
		break;

	case MAIN_BTN_TAG::HERO:
		{
			SceneAllHero* scene = SceneAllHero::create();
			Director::getInstance()->replaceScene(scene);
		}
		break;

	case MAIN_BTN_TAG::STARSU:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_STAR_SU))
		{
			Director::getInstance()->replaceScene(StarsSUScene::create());
		}
		break;

	case MAIN_BTN_TAG::TASK:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TASK))
		{
			auto _layer = MainTaskLayer::create();
			Director::getInstance()->getRunningScene()->addChild(_layer);
		}
		break;

	case MAIN_BTN_TAG::BOSS:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_BOSS))
		{
			Director::getInstance()->replaceScene(BossScene::create());
		}
		break;
		
	case MAIN_BTN_TAG::FIRSTRECHARGE:
		{
			FirstRechargeLayer* _layer = FirstRechargeLayer::create();
			if (nullptr != _layer)
			{
				Director::getInstance()->getRunningScene()->addChild(_layer);
			}
		}
		break;
	case MAIN_BTN_TAG::OPERATE:
		{
			Director::getInstance()->replaceScene(OperateActScene::create());
		}
		break;
	case MAIN_BTN_TAG::REGISTER:
		{
			LayerSign* _layer = LayerSign::create();
			if (nullptr != _layer)
			{
				Director::getInstance()->getRunningScene()->addChild(_layer);
			}
		}
		
		break;
	case MAIN_BTN_TAG::LIMITTIMEHERO:
		{
			Director::getInstance()->replaceScene(LimitTimeHeroScene::create());
		}
		break;
	case MAIN_BTN_TAG::TIANJIANGJINBI:
		{
			Director::getInstance()->replaceScene(GoldActivityScene::create());
		}
		break;
	case MAIN_BTN_TAG::STERYSHOP:
		{
			Director::getInstance()->replaceScene(MysteryShopScene::createScene(ST_MysteryShop));
		}
		break;
	case MAIN_BTN_TAG::FRIEND:
		{
			LayerFriend *pLayerFriend = LayerFriend::create();
			Director::getInstance()->getRunningScene()->addChild(pLayerFriend);

			Node* FriendFlag = mUI.friendBtn->getChildByName("Img_redDot");
			if (FriendFlag)
			{
				FriendFlag->setVisible(false);
			}
		}
		break;
	case MAIN_BTN_TAG::SETGAME: //设置
		{
			PlayerInfoLayer* _layer = PlayerInfoLayer::createSysSetLayer();
			this->addChild(_layer,10);
		}
		break;
	case MAIN_BTN_TAG::CHAT: //聊天
		{
			LayerChat* _layer = LayerChat::create();
			this->addChild(_layer,10);

			Node* chatFlag = mUI.chatBtn->getChildByName("Img_redDot");
			chatFlag->setVisible(false);
		}
		break;
	case MAIN_BTN_TAG::ADDGOLD:
	case MAIN_BTN_TAG::VIP:
		{
			Director::getInstance()->pushScene(VipScene::createScene());
		}
		break;
	case MAIN_BTN_TAG::ROB:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_ROB))
		{
			Director::getInstance()->replaceScene(SceneRob::create());
		}
		break;
	case MAIN_BTN_TAG::MAIL:
		{
			LayerMail* pMail = LayerMail::create();
			this->addChild(pMail, 10);
		}
		break;
	case MAIN_BTN_TAG::MAZE:
		if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_MAZE))
		{
			Director::getInstance()->replaceScene(SceneMaze::create());
		}
		break;
	case MAIN_BTN_TAG::PLAYERINFO:
		{
			PlayerInfoLayer* _layer = PlayerInfoLayer::create();
			if (nullptr != _layer)
			{
				Director::getInstance()->getRunningScene()->addChild(_layer);
			}
		}
		break;
	case MAIN_BTN_TAG::ADDSILVER:
		{
			LayerCommonBuy* buyCoin = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
			Director::getInstance()->getRunningScene()->addChild(buyCoin);
		}
		break;
	case MAIN_BTN_TAG::ADDPOWER:
		{
			LayerCommonBuy* buyPower = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
			Director::getInstance()->getRunningScene()->addChild(buyPower);
		}
		break;
	case MAIN_BTN_TAG::ADDENERGY:
		{
			LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_ENERGY);
			Director::getInstance()->getRunningScene()->addChild(buyEnergy);
		}
		break;
	case MAIN_BTN_TAG::OPEN:
		{
			Director::getInstance()->replaceScene(NewAreaActivityScene::create());
		}
		break;
	case MAIN_BTN_TAG::RANKTOP:
		{
			if(GuideManager::getInstance()->isFunctionOpen(FUNCTION_RANK_TOP))
			{
				Director::getInstance()->replaceScene(RankTopScene::createScene(false));
			}
			

			//临时改动
// 			int stageId = 101900;
// 			auto fightLoadingScene = SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_GUIDE, stageId);
// 			Director::getInstance()->replaceScene(fightLoadingScene);
// 			FightController::getInstance()->sendCommonFightMsg(stageId);

			//临时改动
// 			LevelUpController::getInstance()->confirmLevelUp(12);
// 			LevelUpController::getInstance()->checkLevelUp();
		}
		break;
	case MAIN_BTN_TAG::DEQUE:
		{
			if (isDuiWuCanRun)
			{
				if (isDuiWuOpen)
				{
					auto call = [this]()->void
					{
						isDuiWuOpen = false;
						isDuiWuCanRun = true;
						mUI.jiantou->setPosition(mUI.jiantouPos);
						mUI.jiantou->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5f,Vec2(2,0)),
							CCMoveBy::create(0.5f,Vec2(-2,0)),nullptr)));
					};
					isDuiWuCanRun = false;
					mUI.dequeDi->runAction(EaseSineIn::create(CCSequence::create(CCMoveTo::create(0.3f,Vec2(mUI.dequeDi->getContentSize().width,mUI.dequeDiPos.y)),
						CCCallFunc::create(call),nullptr)));
				}
				else
				{
					auto call = [this]()->void
					{
						isDuiWuOpen = true;
						isDuiWuCanRun = true;
						mUI.jiantou->stopAllActions();
					};
					isDuiWuCanRun = false;
					mUI.dequeDi->runAction(EaseSineIn::create(CCSequence::create(CCMoveTo::create(0.3f,Vec2(0,mUI.dequeDiPos.y)),
						CCCallFunc::create(call),nullptr)));
				}
			}
		}
		break;
	case MAIN_BTN_TAG::ZHENFA:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TACTICS))
			{
				Director::getInstance()->replaceScene(SceneTactics::create(false));
			}
		}
		break;
	case MAIN_BTN_TAG::SHENQI:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_SHENQI))
			{
				Director::getInstance()->replaceScene(ArtifactScene::create());
			}
		}
		break;
	case MAIN_BTN_TAG::STARTPAN:
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_STAR_PLATE))
			{
				Director::getInstance()->replaceScene(StarsCampScene::create());
			}
			break;
		}
		break;
	default:
		break;
	}
}

//增加聊天滚动条
void MainInfoLayer::addChatMsgItem()
{
	Point pos = Director::getInstance()->getVisibleOrigin();
	Size size = Director::getInstance()->getVisibleSize();
	LayerChatMessage* pChatMsg = LayerChatMessage::create();
	pChatMsg->setPosition(pos);
	this->addChild(pChatMsg, 9, "LayerChatMessage");//mUI.root
}

//测试代码
Sprite* MainInfoLayer::getBlurScreenShot()
{
	Size screenSize = Director::getInstance()->getWinSize();
	//获取屏幕尺寸，初始化一个空的渲染纹理对象，需开启深度渲染
	auto renderTexture = RenderTexture::create(screenSize.width, screenSize.height, Texture2D::PixelFormat::RGBA8888, GL_DEPTH24_STENCIL8);
	//清空并开始获取
	renderTexture->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
	//遍历场景节点对象，填充纹理到RenderTexture中
	Director::getInstance()->getRunningScene()->visit();
	//结束获取
	renderTexture->end();

	Director::getInstance()->getRenderer()->render();

	string filename = "screenshot.png";
	renderTexture->saveToFile(filename);

	string fullpath = FileUtils::getInstance()->getWritablePath() + filename;

	auto scheduleCallback = [this, fullpath](float dt)
	{
		Sprite* sp = Sprite::create(fullpath);
		sp->setPosition(this->getContentSize() / 2);
		
		GLchar * fragSource = (GLchar*) String::createWithContentsOfFile(
			FileUtils::getInstance()->fullPathForFilename("Shader/example_Blur.fsh").c_str())->getCString();  
		auto program = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource);

		auto glProgramState = GLProgramState::getOrCreateWithGLProgram(program);
		sp->setGLProgramState(glProgramState);

		auto size = sp->getTexture()->getContentSizeInPixels();
		sp->getGLProgramState()->setUniformVec2("resolution", size);
		sp->getGLProgramState()->setUniformFloat("blurRadius", 13.0f);
		sp->getGLProgramState()->setUniformFloat("sampleNum", 7.0f);
		
		this->addChild(sp);
	};
	auto _schedule = Director::getInstance()->getRunningScene()->getScheduler();
	_schedule->schedule(scheduleCallback, this, 0.0f,0,0.0f, false, "screenshot");

	return nullptr;
}

void MainInfoLayer::setNewStartCampFlage()
{
	auto duiwudian = static_cast<ImageView*>(mUI.dequeBtn->getChildByName("dian"));
	auto xinpandiang = static_cast<ImageView*>(mUI.starPanleBtn->getChildByName("dian"));
	if (duiwudian && xinpandiang)
	{
		bool isshow = StarsCampController::getInstance()->isShowNewCampRedFlag();
		duiwudian->setVisible(isshow);
		xinpandiang->setVisible(isshow);
	}
}

void MainInfoLayer::setFuncState()
{
	if(MainModel::getInstance()->mBossActOpenState != 0 
		&& GuideManager::getInstance()->isFunctionOpen(FUNCTION_BOSS, false))
	{
		//有boss活动且等级达到开放条件,展示boss活动进行状态，其它活动依次else if
		setActiveState(ACTIVE_TYPE::ACTIVE_BOSS);
	}
	else
	{
		//展示即将开放的功能
		setNextFuncOpen();
	}

	
}

void MainInfoLayer::setNextFuncOpen()
{
	//有活动提示动画先移除
	mUI.funcOpenImg->removeChildByName("Active_Ani");
	mUI.funcOpenImg->removeChildByName("Func_Ani");
	mUI.openTipsText->setString("");
	mUI.funcOpenImg->setVisible(false);

	int curMainLevel = HeroModel::getInstance()->getMainHeroLevel();
	
	//查功能开放等级表并排序;
	DataTable* openFuncDT = DataManager::getInstance()->getDTOpenFunction();
	if(openFuncDT == nullptr)
	{
		return;
	}

	vector<sOpenFuncData> vcData;
	for (auto iter = openFuncDT->getTableData().begin(); iter != openFuncDT->getTableData().end(); ++iter)
	{
		RowData* rowData = &(iter->second);
		
		sOpenFuncData data;
		data.id = rowData->getIntData("id");
		data.level = rowData->getIntData("level");
		data.iconName = rowData->getStringData("name");
		data.resPath = rowData->getStringData("icon");
		data.strDes = rowData->getStringData("dec");
		vcData.push_back(data);
	}
	std::sort(vcData.begin(), vcData.end(), 
		[](const sOpenFuncData& p1, const sOpenFuncData& p2)->bool
	{
		return p1.level < p2.level;
	});

	//计算下一功能开放所需等级;
	bool isNeedOpen = false;

	for (int i = 0; i < vcData.size(); ++i)
	{
		if (vcData.at(i).level > curMainLevel)
		{
			m_sData = vcData.at(i);
			isNeedOpen = true;
			break;
		}
	}

	if(isNeedOpen)
	{
		mUI.funcOpenImg->setVisible(true);

		string normalPath = "Image/Icon/Func/" + m_sData.resPath + "_1.png";
		string selectPath = "Image/Icon/Func/" + m_sData.resPath + "_2.png";
		mUI.funcOpenBtn->loadTextures(normalPath, selectPath);
	
		mUI.openTipsText->setString(TO_STR(m_sData.level) + DataManager::getInstance()->searchCommonTexdtById(10001));
		//setTextAddStroke(mUI.openTipsText, Color3B(0x66, 0x12, 0x12), 2);
		setTextAddStroke(mUI.openTipsText, Color3B::BLACK, 2);

		auto tipsCallBack = [this](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::ENDED)
			{
				SoundUtils::playSoundEffect("dianji");
				showFuncOpenTips();
			}
		};
		mUI.funcOpenBtn->addTouchEventListener(tipsCallBack);
	}

	//功能提示动画
	showActiveAni(false);
}

void MainInfoLayer::setActiveState(ACTIVE_TYPE type)
{
	switch (type)
	{
	case ACTIVE_TYPE::ACTIVE_BOSS:
		{
			mUI.funcOpenImg->setVisible(true);
			mUI.openTipsText->setString(DataManager::getInstance()->searchCommonTexdtById(20275));
			//setTextAddStroke(mUI.openTipsText, Color3B(0x66, 0x12, 0x12), 2);
			setTextAddStroke(mUI.openTipsText, Color3B::BLACK, 2);

			//添加动画
			showActiveAni(true);
			string actNorPath = "Image/Icon/Func/btn_90025_1.png";
			string actSelPath = "Image/Icon/Func/btn_90025_2.png";
			mUI.funcOpenBtn->loadTextures(actNorPath, actSelPath);
			mUI.funcOpenBtn->addTouchEventListener(CC_CALLBACK_2(MainInfoLayer::onBtnClick, this, MAIN_BTN_TAG::BOSS));
		}
		break;
	default:
		break;
	}
}

void MainInfoLayer::showFuncOpenTips()
{
	ImageView* funcIcon = (ImageView*)(Helper::seekWidgetByName(mUI.tipsDetailsPnl, "Img_funcIcon"));
	Button* funcCloseBtn = (Button*)(Helper::seekWidgetByName(mUI.tipsDetailsPnl, "Btn_Close"));
	Text* funcLevel = (Text*)(Helper::seekWidgetByName(mUI.tipsDetailsPnl, "Lab_Level"));
	Text* funcDetails = (Text*)(Helper::seekWidgetByName(mUI.tipsDetailsPnl, "Lab_Details"));

	funcIcon->loadTexture("Image/Icon/Func/" + m_sData.resPath + "_1.png");

	funcLevel->setFontName(FONT_FZZHENGHEI);
	funcLevel->setString(TO_STR(m_sData.level) + DataManager::getInstance()->searchCommonTexdtById(10001));
	funcDetails->setFontName(FONT_FZZHENGHEI);
	funcDetails->setString(m_sData.strDes);

	mUI.tipsDetailsPnl->setVisible(true);
	ActionCreator::runCommDlgAction(mUI.tipsDetailsPnl->getChildByName("Img_Bg"));

	auto tipsCallBack = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");
			mUI.tipsDetailsPnl->setVisible(false);
		}
	};
	funcCloseBtn->addTouchEventListener(tipsCallBack);
}

void MainInfoLayer::showActiveAni(bool isActive)
{
	mUI.funcOpenImg->removeChildByName("Active_Ani");
	mUI.funcOpenImg->removeChildByName("Func_Ani");
	if(isActive)
	{
		//添加后尾动画
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniXingjijiangli/AniXingjijiangli.ExportJson");
		Armature* backAni = Armature::create("AniXingjijiangli");
		backAni->setPosition(mUI.funcOpenBtn->getPosition());
		backAni->setName("Active_Ani");
		backAni->setScale(0.2f);
		mUI.funcOpenImg->addChild(backAni, mUI.funcOpenBtn->getLocalZOrder()-1);	
		backAni->getAnimation()->play("back");
	}
	else
	{
		//添加前尾动画
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniGongnengtishi/AniGongnengtishi.ExportJson");
		Armature* frontAni = Armature::create("AniGongnengtishi");
		frontAni->setPosition(mUI.funcOpenBtn->getPosition());
		frontAni->setName("Func_Ani");
		frontAni->setScale(0.2f);
		mUI.funcOpenImg->addChild(frontAni, mUI.funcOpenBtn->getLocalZOrder()+1);	
		frontAni->getAnimation()->play("Animation1");
	}
}