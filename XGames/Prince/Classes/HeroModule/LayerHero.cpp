#include "LayerHero.h"
#include "CocoStudio.h"
#include "UI/UIDef.h"
#include "Utils/StringFormat.h"
#include "ToolModule/UIToolHeader.h"
#include "FormationModule/FormationDef.h"
#include "FormationModule/LayerFormation.h"
#include "ToolModule/ToolModel.h"
#include "DataManager/DataManager.h"
#include "Utils/ResourceUtils.h"
#include "Communication/Command.h"
#include "Utils/ResourceUtils.h"
#include "../SmithyModule/LayerSmithy.h"
#include "../SmithyModule/SmithyModel.h"
#include "../SmithyModule/LayerEquipSource.h"
#include "ToolModule/LayerTool.h"
#include "LayerHeroFate.h"
#include "GuideCenter/GuideManager.h"
#include "TalentModule/TalentLayer.h"
#include "LayerEquipPutOn.h"
#include "Utils/StringFormat.h"
#include "MainModule/MainModel.h"
#include "LayerHeroAdvance.h"
#include "Widget/SkillIcon.h"
#include "LayerHeroLevel.h"
#include "SmithyModule/SceneSmithy.h"
#include "Widget/ActionCreator.h"
#include "SceneHeroFate.h"
#include "LayerHeroStarsUp.h"
#include "TalentModule/TalentScene.h"
#include "StarsSuModule/StarsSuScene.h"
#include "StarsSuModule/StarsSuModel.h"
#include "HeroRelationModule/HeroRelationScene.h"
#include "HeroRelationModule/HeroRelationModel.h"
#include "HeroRelationModule/HeroRelationDataDef.h"
#include "Widget/LayerHeroRelation.h"

//兵书马书开放等级表ID
#define HORSE_EQUIP_OPEN_LV_ID		10113
#define BOOK_EQUIP_OPEN_LV_ID		10114

LayerHero::LayerHero()
	: mCurrentHeroId(-1)
	, mCurrentHeroIndex(-1)
	, mHeroParam(nullptr)
	, mIsFirstEnter(true)
	, mMatKindsCount(0)
	, mPlayAniHeroId(0)
	, m_starsAttriScrollBar(nullptr)
	, m_StarsSuController(nullptr)
	, mCurPanel(CARD_PANEL)
{
	//兵书马书开放等级查表
	m_iHorseEquipOpenLv = 0;
	m_iBookEquipOpenLv = 0;
	RowData* sysData1 = DataManager::getInstance()->searchToolSystemById(HORSE_EQUIP_OPEN_LV_ID);
	RowData* sysData2 = DataManager::getInstance()->searchToolSystemById(BOOK_EQUIP_OPEN_LV_ID);
	if(sysData1 != nullptr)
	{
		m_iHorseEquipOpenLv = sysData1->getIntData("value");
	}
	if(sysData2 != nullptr)
	{
		m_iBookEquipOpenLv = sysData2->getIntData("value");
	}
}

LayerHero::~LayerHero()
{
	
}

bool LayerHero::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void LayerHero::onExit()
{
	Layer::onExit();
	ResourceLoader::getInstance()->removeUIResource("UIArray");

	HeroModel::getInstance()->removeObserver(this);

	if(m_StarsSuController != nullptr)
	{
		delete(m_StarsSuController);
		m_StarsSuController = nullptr;
	}
}

void LayerHero::onEnter()
{
	Layer::onEnter();

	HeroModel::getInstance()->addObserver(this);

	//非首次进入该界面需要刷新UI
	if (mIsFirstEnter)
	{
		mIsFirstEnter = false;

		m_StarsSuController = new StarsSuController();
		//首次进入需要拉取星宿数据
		auto callBack = [this](float dt)->void
		{
			if (StarsSuModel::getInstance()->getStatSuInfoCount() <= 0)
			{
				m_StarsSuController->onGetStarsInfo();
			}
			this->getScheduler()->unschedule("LayerHero::sendStarsSuMsg", this);
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerHero::sendStarsSuMsg");
	}
	else
	{
		//更新所有装备
		updateAllEquip();
		//英雄属性
		updateUIAttri();
		//进阶属性
		updateAdvInfo();
		//英雄可进阶标记
		updateHeroRedFlag();
		//装备可进阶标记
		updateEquipRedFlag();
		//缘分更新
		updateFateUI();
		//更新星级
		updateHeroStarUpInfo();
		//更新英雄卡牌
		updateHeroCard();

		scrollHeroList();
	}

	setScrollViewClickEnabled(true);
	setAdvBreakBtnClickEnabled(true);
	showEnterAnimation();
	showBtnEnterAnimation();

	GuideManager::getInstance()->startGuideNextFrame(GUIDE_INTERFACE_HERO);
}

void LayerHero::showEnterAnimation()
{
	Node* panel = nullptr;
	Point point = Point::ZERO;
	if (mUI.mHeroPanel->isVisible())
	{
		panel = mUI.mHeroPanel;
		point = mUI.mHeroPanelPos;
	}
	else if(mUI.mHeroInfoPanel->isVisible())
	{
		panel = mUI.mHeroInfoPanel;
		point = mUI.mHeroInfoPanelPos;
	}
	else if(mUI.mAdvInfoPanel->isVisible())
	{
		panel = mUI.mAdvInfoPanel;
		point = mUI.mAdvInfoPanelPos;
	}
	else if(mUI.mPnlStarsUp->isVisible())
	{
		panel = mUI.mPnlStarsUp;
		point = mUI.mStarsUpPnlPos;
	}
	if (panel)
	{
		panel->stopAllActions();
		panel->setPosition(point + Point(350, 0));
		Sequence* seq = Sequence::create(EaseBackOut::create(MoveTo::create(0.4f, point)), nullptr);
		panel->runAction(seq);
	}
}

void LayerHero::showBtnEnterAnimation()
{
	/*************重写---start---*************/
	/*
	if (mUI.mAdvanceBtn->isVisible())
	{
		ActionCreator::runCommBtnAction(mUI.mAdvanceBtn);
	}

	if (mUI.mTianfuBtn->isVisible())
	{
		ActionCreator::runCommBtnAction(mUI.mTianfuBtn);
	}

	if (mUI.mAttriBtn->isVisible())
	{
		ActionCreator::runCommBtnAction(mUI.mAttriBtn);
	}
	
	if (mUI.mLevelUpBtn->isVisible())
	{
		ActionCreator::runCommBtnAction(mUI.mLevelUpBtn);
	}

	if (mUI.mSkillBtn->isVisible())
	{
		ActionCreator::runCommBtnAction(mUI.mSkillBtn);
	}
	*/
	/*************重写---start---*************/

	ActionCreator::runCommBtnAction(mUI.mWeaponImg);
	ActionCreator::runCommBtnAction(mUI.mClothesImg);
	ActionCreator::runCommBtnAction(mUI.mHatImg);
	ActionCreator::runCommBtnAction(mUI.mEarringImg);
	ActionCreator::runCommBtnAction(mUI.mBookImg);
	ActionCreator::runCommBtnAction(mUI.mHorseImg);
}

void LayerHero::scrollHeroList()
{
	int heroCount = HeroModel::getInstance()->getHeroList()->size();
	if (mCurrentHeroIndex != 0 && heroCount > 0)
	{
		mUI.mHeroListScrollView->scrollToPercentVertical((float)(mCurrentHeroIndex + 1) / heroCount * 100, 0.5f, true);
	}
}

//显示英雄
void LayerHero::showHeroList(int curId)
{
	initHeroList(curId);
}

void LayerHero::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	mUI.mRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIArray/UIArray.ExportJson"));
	this->addChild(mUI.mRoot);
	mUI.mRoot->setSize(size);
	Point pos = Director::getInstance()->getVisibleOrigin();
	mUI.mRoot->setPosition(pos);
	mUI.mRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	
	//英雄列表面板
	Layout* pHeroList = dynamic_cast<Layout*>(mUI.mRoot->getChildByName("Panel_liebiao"));
	mUI.mBackBtn = dynamic_cast<Button*>(pHeroList->getChildByName("Button_fanhui"));
	mUI.mBackBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BACK));
	mUI.mHeroListPanel = dynamic_cast<Layout*>(pHeroList->getChildByName("Pnl_HeroList2"));
	mUI.mHeroListScrollView = dynamic_cast<ScrollView*>(mUI.mHeroListPanel->getChildByName("ScrollView_liebiao"));
	mUI.mHeroListScrollView->addEventListenerScrollView(this, (SEL_ScrollViewEvent)&LayerHero::onScrollViewEvent);
	mUI.mScrollUpBtn = dynamic_cast<ImageView*>(mUI.mHeroListPanel->getChildByName("Image_zhezhao_1"));
	mUI.mScrollDownBtn = dynamic_cast<ImageView*>(mUI.mHeroListPanel->getChildByName("Image_zhezhao_2"));
	
	//角色面板
	Layout* pRoleInfoPanel = dynamic_cast<Layout*>(mUI.mRoot->getChildByName("Panel_juese"));
	//主角按钮面板
	mUI.mMainHeroBtnPanle = dynamic_cast<Layout*>(pRoleInfoPanel->getChildByName("Panel_zhujueanniu"));
	//主角天赋按钮
	Button* mainHeroTianfuBtn = dynamic_cast<Button*>(mUI.mMainHeroBtnPanle->getChildByName("Button_tianfu"));
	mainHeroTianfuBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_TIANFU));
	//主角进阶按钮
	Button* mainHeroAdvanceBtn = dynamic_cast<Button*>(mUI.mMainHeroBtnPanle->getChildByName("Button_juexing"));
	mainHeroAdvanceBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_ADVANCE));
	mUI.mMainHeroAdvFlagImg = dynamic_cast<ImageView*>(mainHeroAdvanceBtn->getChildByName("Img_lab"));
	//主角技能按钮
	Button* mainHeroSkillBtn = dynamic_cast<Button*>(mUI.mMainHeroBtnPanle->getChildByName("Button_jineng"));
	mainHeroSkillBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_SKILL));
	//主角属性按钮
	Button* mainHeroAttriBtn = dynamic_cast<Button*>(mUI.mMainHeroBtnPanle->getChildByName("Button_shuxing"));
	mainHeroAttriBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_ATTR));
	//主角升星按钮(星宿入口)
	Button* mainHeroStarUp = dynamic_cast<Button*>(mUI.mMainHeroBtnPanle->getChildByName("Button_xingxiu"));
	mainHeroStarUp->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_STARUP));
	mUI.mStarsSuFlagImg = dynamic_cast<ImageView*>(mainHeroStarUp->getChildByName("Img_lab"));
	mUI.mStarsSuFlagImg->setVisible(false);

	//英雄按钮面板
	mUI.mHeroBtnPanel = dynamic_cast<Layout*>(pRoleInfoPanel->getChildByName("Panel_yingxionganniu"));
	//英雄升级按钮
	Button* heroLevelUpBtn = dynamic_cast<Button*>(mUI.mHeroBtnPanel->getChildByName("Button_shengji"));
	heroLevelUpBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_LEVELUP));
	//英雄觉醒按钮
	Button* heroAdvanceBtn = dynamic_cast<Button*>(mUI.mHeroBtnPanel->getChildByName("Button_juexing"));
	heroAdvanceBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_ADVANCE));
	mUI.mHeroAdvFlagImg = dynamic_cast<ImageView*>(heroAdvanceBtn->getChildByName("Img_lab"));
	//英雄技能按钮
	Button* heroSkillBtn = dynamic_cast<Button*>(mUI.mHeroBtnPanel->getChildByName("Button_jineng"));
	heroSkillBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_SKILL));
	//英雄属性按钮
	Button* heroAttriBtn = dynamic_cast<Button*>(mUI.mHeroBtnPanel->getChildByName("Button_shuxing"));
	heroAttriBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_ATTR));
	//英雄升星按钮
	Button* heroStarUpBtn = dynamic_cast<Button*>(mUI.mHeroBtnPanel->getChildByName("Button_shengxing"));
	heroStarUpBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_STARUP));
	mUI.mHeroStarUpFlagImg = dynamic_cast<ImageView*>(heroStarUpBtn->getChildByName("Img_lab"));
	mUI.mHeroStarUpFlagImg->setVisible(false);
	//英雄升星进度
	mUI.mMidStarsUpBar = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(mUI.mHeroBtnPanel, "ProgressBar_jindu"));
	mUI.mMidStarsUpBar->setPercent(0.0f);
	mUI.mMidChipCount = dynamic_cast<Text*>(Helper::seekWidgetByName(mUI.mHeroBtnPanel, "Label_jindu"));
	mUI.mMidChipCount->setFontName(FONT_FZZHENGHEI);
	mUI.mMidChipCount->setString("");

	//装备底图
	mUI.mWeaponImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pRoleInfoPanel, "Image_wuqi"));
	mUI.mClothesImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pRoleInfoPanel, "Image_yifu"));
	mUI.mHatImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pRoleInfoPanel, "Image_maozi"));
	mUI.mEarringImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pRoleInfoPanel, "Image_erhuan"));
	mUI.mBookImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pRoleInfoPanel, "Image_book"));
	mUI.mHorseImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pRoleInfoPanel, "Image_horse"));
	//装备按钮
	Button* weaponBtn = dynamic_cast<Button*>(mUI.mWeaponImg->getChildByName("Button_kuang"));
	Button* clothesBtn = dynamic_cast<Button*>(mUI.mClothesImg->getChildByName("Button_kuang"));
	Button* hatBtn = dynamic_cast<Button*>(mUI.mHatImg->getChildByName("Button_kuang"));
	Button* earringBtn = dynamic_cast<Button*>(mUI.mEarringImg->getChildByName("Button_kuang"));
	Button* bookBtn = dynamic_cast<Button*>(mUI.mBookImg->getChildByName("Button_kuang"));
	Button* horseBtn = dynamic_cast<Button*>(mUI.mHorseImg->getChildByName("Button_kuang"));
	weaponBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_WEAPON));
	clothesBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_CLOTHES));
	horseBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_HORSE));
	hatBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_HAT));
	earringBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_EARRING));
	bookBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_BOOK));
	initEquipIcon(&mUI.mWeapon, weaponBtn, HERO_BTN_ID::BTN_WEAPON);
	initEquipIcon(&mUI.mClothes, clothesBtn, HERO_BTN_ID::BTN_CLOTHES);
	initEquipIcon(&mUI.mHat, hatBtn, HERO_BTN_ID::BTN_HAT);
	initEquipIcon(&mUI.mEarring, earringBtn, HERO_BTN_ID::BTN_EARRING);
	initEquipIcon(&mUI.mBook, bookBtn, HERO_BTN_ID::BTN_BOOK);
	initEquipIcon(&mUI.mHorse, horseBtn, HERO_BTN_ID::BTN_HORSE);

	//等级
	mUI.mImgTalent = dynamic_cast<ImageView*>(pRoleInfoPanel->getChildByName("Img_zizhiLv"));
	mUI.mImgTalent->setScale(1.0f);

	mUI.mImgAttriType = dynamic_cast<ImageView*>(pRoleInfoPanel->getChildByName("Image_zhiye"));
	//名字
	ImageView* pImgHeroName = dynamic_cast<ImageView*>(pRoleInfoPanel->getChildByName("Image_mingzi"));  
	mUI.mNameText = dynamic_cast<Text*>(pImgHeroName->getChildByName("Label_mingzi"));
	//mUI.mImgAdvLvl = dynamic_cast<ImageView*>(pImgHeroName->getChildByName("Image_juexing"));
	mUI.mNickNameImg = dynamic_cast<ImageView*>(pImgHeroName->getChildByName("Image_chenghao"));
	mUI.mNameText->setFontName(FONT_FZZHENGHEI);
	//星级
	Layout* topStarPnl = dynamic_cast<Layout*>(Helper::seekWidgetByName(mUI.mRoot, "Panel_star"));
	for (int i = 0, j = STAR_COUNT-1; i < STAR_COUNT; i++, j--)
	{
		mUI.mTopStarImg[j] = dynamic_cast<ImageView*>(topStarPnl->getChildByName("Image_star_" + TO_STR(i) + "_" + TO_STR(i)));
	}

	//英雄基础属性
	mUI.mHeroAttriImg = dynamic_cast<ImageView*>(pRoleInfoPanel->getChildByName("Image_shuxingdi"));
	mUI.mheroAttrImgOriginPos = mUI.mHeroAttriImg->getPosition();
	mUI.mAtkTextNum = dynamic_cast<Text*>(mUI.mHeroAttriImg->getChildByName("Label_gongji"));
	mUI.mHpTextNum = dynamic_cast<Text*>(mUI.mHeroAttriImg->getChildByName("Label_xueliang"));
	mUI.mPddTextNum = dynamic_cast<Text*>(mUI.mHeroAttriImg->getChildByName("Label_wufang"));
	mUI.mMddTextNum = dynamic_cast<Text*>(mUI.mHeroAttriImg->getChildByName("Label_fafang"));
	mUI.mFightTextNum = dynamic_cast<Text*>(mUI.mHeroAttriImg->getChildByName("Label_zhandouli"));
	mUI.mAtkTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mHpTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mPddTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mMddTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mFightTextNum->setFontName(FONT_FZZHENGHEI);

	mUI.mAtkIcon = dynamic_cast<ImageView*>(mUI.mHeroAttriImg->getChildByName("Image_gongji"));
	

	//骨骼动画
	mUI.mHeroSpinePanel = dynamic_cast<Layout*>(pRoleInfoPanel->getChildByName("Panel_guge"));
	//缘分面板
	mUI.mHeroFatePanel = dynamic_cast<Layout*>(pRoleInfoPanel->getChildByName("Panel_zhujueyuanfen"));
	mUI.mHeroFateTitle = dynamic_cast<ImageView*>(pRoleInfoPanel->getChildByName("Image_zhujueyuanfen"));

	//英雄详情
	Layout* pPanelDetail = dynamic_cast<Layout*>(mUI.mRoot->getChildByName("Panel_xiangqing"));
	//卡牌面板
	mUI.mHeroPanel = dynamic_cast<Layout*>(pPanelDetail->getChildByName("Panel_kapai"));
	mUI.mHeroPanelPos = mUI.mHeroPanel->getPosition();
	
	//英雄信息面板
	mUI.mHeroInfoPanel = dynamic_cast<Layout*>(pPanelDetail->getChildByName("Panel_shuxing"));
	mUI.mHeroInfoPanelPos = mUI.mHeroInfoPanel->getPosition();
	//属性信息
	ImageView* pImgHeroDetail = dynamic_cast<ImageView*>(mUI.mHeroInfoPanel->getChildByName("Image_di"));
	ImageView* pImgHeroProperty = dynamic_cast<ImageView*>(pImgHeroDetail->getChildByName("Image_dadi"));	//属性
	mUI.mScrollPanel = dynamic_cast<ScrollView*>(pImgHeroProperty->getChildByName("ScrollView_shuxing"));
	//mUI.mHeroTalent = dynamic_cast<TextAtlas*>(mUI.mScrollPanel->getChildByName("AtlasLabel_zizhi"));
	mUI.mImgHeroTalent = dynamic_cast<ImageView*>(mUI.mScrollPanel->getChildByName("Img_zizhiLevel"));
	Text* speed = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_sudu"));
	Text* atkAdd = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_gongji"));
	Text* hpAdd = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_xueliang"));
	Text* mddAdd = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_fafang"));
	Text* pddAdd = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_wufang"));
	Text* crit = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_baoji"));
	Text* critCoe = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_baoshang"));
	Text* miss = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_shanbi"));
	Text* hit = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_mingzhong"));
	Text* critResis = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_baokang"));
	Text* combo = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_duoji"));
	speed->setFontName(FONT_FZZHENGHEI);
	atkAdd->setFontName(FONT_FZZHENGHEI);
	hpAdd->setFontName(FONT_FZZHENGHEI);
	mddAdd->setFontName(FONT_FZZHENGHEI);
	pddAdd->setFontName(FONT_FZZHENGHEI);
	crit->setFontName(FONT_FZZHENGHEI);
	critCoe->setFontName(FONT_FZZHENGHEI);
	miss->setFontName(FONT_FZZHENGHEI);
	hit->setFontName(FONT_FZZHENGHEI);
	critResis->setFontName(FONT_FZZHENGHEI);
	combo->setFontName(FONT_FZZHENGHEI);
	mUI.mSpeed = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_sudu_1"));
	mUI.mAtkAdd = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_gongji_1"));
	mUI.mHpAdd = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_xueliang_1"));
	mUI.mMddAdd = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_fafang_1"));
	mUI.mPddAdd = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_wufang_1"));
	mUI.mCritTextNum = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_baoji_1"));
	mUI.mCritCoeTextNum = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_baoshang_1"));
	mUI.mMisPrbTextNum = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_shanbi_1"));
	mUI.mHitPrbTextNum = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_mingzhong_1"));
	mUI.mCritResisTextNum = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_baokang_1"));	
	mUI.mComboTextNum = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_duoji_1"));
	mUI.mSpeed->setFontName(FONT_FZZHENGHEI);
	mUI.mAtkAdd->setFontName(FONT_FZZHENGHEI);
	mUI.mHpAdd->setFontName(FONT_FZZHENGHEI);
	mUI.mMddAdd->setFontName(FONT_FZZHENGHEI);
	mUI.mPddAdd->setFontName(FONT_FZZHENGHEI);
	mUI.mCritTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mCritCoeTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mMisPrbTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mHitPrbTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mCritResisTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mComboTextNum->setFontName(FONT_FZZHENGHEI);
	//英雄信息，介绍等
	ImageView* pImgHeroInfo = dynamic_cast<ImageView*>(pImgHeroDetail->getChildByName("Image_xiaodi"));
	mUI.mHeroIntroText = dynamic_cast<Text*>(pImgHeroInfo->getChildByName("Label_jieshao"));
	mUI.mHeroIntroText->setFontName(FONT_FZZHENGHEI);
	//mUI.mHeroTypeIntroText = dynamic_cast<Text*>(pImgHeroInfo->getChildByName("Label_leixingjieshao"));
	//mUI.mHeroTypeIntroText->setFontName(FONT_FZZHENGHEI);
	mUI.mImgJobType = dynamic_cast<ImageView*>(pImgHeroInfo->getChildByName("Image_shuxing1"));
	mUI.mImgHeroType = dynamic_cast<ImageView*>(pImgHeroInfo->getChildByName("Image_shuxing"));
	//需根据英雄攻击类型区分
	//mUI.mInfoAtkAddText = dynamic_cast<Text*>(pImgHeroProperty->getChildByName("Label_gongji"));
	//mUI.mInfoAtkAddText->setFontName(FONT_FZZHENGHEI);
	//mUI.mInfoAtkAddIcon = dynamic_cast<ImageView*>(pImgHeroProperty->getChildByName("Image_gongji"));
	
	//觉醒突破共用/技能提示星级、段数
	mUI.mAdvInfoPanel = dynamic_cast<Layout*>(pPanelDetail->getChildByName("Panel_grow"));
	mUI.mAdvInfoPanelPos = mUI.mAdvInfoPanel->getPosition();
	ImageView* pImgPublic = dynamic_cast<ImageView*>(mUI.mAdvInfoPanel->getChildByName("Image_di")->getChildByName("Image_dadi"));
	mUI.mAdvanceTips1 = dynamic_cast<Text*>(pImgPublic->getChildByName("Label_juexing_1"));
	mUI.mAdvanceTips1->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(mUI.mAdvanceTips1, Color3B::BLACK, 2);
	mUI.mAdvanceSkillPanel = dynamic_cast<Layout*>(pImgPublic->getChildByName("Panel_juexingjineng"));

	//名字颜色变化
	mUI.mQuaName1 = dynamic_cast<Text*>(pImgPublic->getChildByName("Label_yingxing1"));
	mUI.mQuaName1->setFontName(FONT_FZZHENGHEI);
	mUI.mQuaName1->setString("");
	mUI.mQuaName2 = dynamic_cast<Text*>(pImgPublic->getChildByName("Label_yingxing2"));
	mUI.mQuaName2->setFontName(FONT_FZZHENGHEI);
	mUI.mQuaName2->setString("");
	//进阶进度
	mUI.mQuaUpBar = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(pImgPublic, "ProgressBar_juexing"));
	mUI.mQuaUpBar->setPercent(0.0f);
	mUI.mQuaUpJinDu = dynamic_cast<Text*>(Helper::seekWidgetByName(pImgPublic, "Label_juexingfenshu"));
	mUI.mQuaUpJinDu->setFontName(FONT_FZZHENGHEI);
	mUI.mQuaUpJinDu->setString("");

	//觉醒消耗
	ImageView* consumePanel = dynamic_cast<ImageView*>(mUI.mAdvInfoPanel->getChildByName("Image_di")->getChildByName("Image_xiaodi"));
	mUI.mAdvanceCoin = dynamic_cast<Text*>(consumePanel->getChildByName("Label_yinbi"));
	mUI.mAdvanceCoin->setFontName(FONT_FZZHENGHEI);
	mUI.mAdvanceMatPanel = dynamic_cast<Layout*>(consumePanel->getChildByName("Panel_daoju"));
	mUI.mAdvanceMaxLv = dynamic_cast<Text*>(consumePanel->getChildByName("Label_maxLv"));
	mUI.mAdvanceMaxLv->setFontName(FONT_FZZHENGHEI);
	mUI.mAdvanceMaxLv->setVisible(false);
	//所需等级
	mUI.mNeedLevelTxt = dynamic_cast<Text*>(mUI.mAdvInfoPanel->getChildByName("Label_dengji"));
	mUI.mNeedLevelTxt->setFontName(FONT_FZZHENGHEI);

	//觉醒属性
	mUI.mAdvTitle1 = dynamic_cast<ImageView*>(mUI.mAdvInfoPanel->getChildByName("Image_xiaoguo")->getChildByName("Image_juexing"));
	mUI.mAdvTitle2 = dynamic_cast<ImageView*>(mUI.mAdvInfoPanel->getChildByName("Image_xiaohao")->getChildByName("Image_juexing"));
	mUI.mBtnStartAdv = dynamic_cast<Button*>(mUI.mAdvInfoPanel->getChildByName("Button_juexing"));
	addAnimationStartAdv(mUI.mBtnStartAdv);
	mUI.mBtnStartAdv->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_START_ADV));
	mUI.mAdvPropertyPanel = dynamic_cast<Layout*>(pImgPublic->getChildByName("Panel_juexing"));
	mUI.mAdvanceAtkTextNum = dynamic_cast<Text*>(mUI.mAdvPropertyPanel->getChildByName("Label_gongji_1"));
	mUI.mAdvanceHpTextNum = dynamic_cast<Text*>(mUI.mAdvPropertyPanel->getChildByName("Label_xueliang_1"));
	mUI.mAdvancePddTextNum = dynamic_cast<Text*>(mUI.mAdvPropertyPanel->getChildByName("Label_wufang_1"));
	mUI.mAdvanceMddTextNum = dynamic_cast<Text*>(mUI.mAdvPropertyPanel->getChildByName("Label_fafang_1"));
	mUI.mAdvanceAtkTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mAdvanceHpTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mAdvancePddTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mAdvanceMddTextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mArrow1 = dynamic_cast<ImageView*>(mUI.mAdvPropertyPanel->getChildByName("Image_up1"));
	mUI.mArrow2 = dynamic_cast<ImageView*>(mUI.mAdvPropertyPanel->getChildByName("Image_up2"));
	mUI.mArrow3 = dynamic_cast<ImageView*>(mUI.mAdvPropertyPanel->getChildByName("Image_up3"));
	mUI.mArrow4 = dynamic_cast<ImageView*>(mUI.mAdvPropertyPanel->getChildByName("Image_up4"));
	//Text* advanceAtkText =  dynamic_cast<Text*>(mUI.mAdvPropertyPanel->getChildByName("Label_gongji"));
	Text* advanceHpText =  dynamic_cast<Text*>(mUI.mAdvPropertyPanel->getChildByName("Label_xueliang"));
	Text* advancePddText =  dynamic_cast<Text*>(mUI.mAdvPropertyPanel->getChildByName("Label_wufang"));
	Text* advanceMddText =  dynamic_cast<Text*>(mUI.mAdvPropertyPanel->getChildByName("Label_fafang"));
	//advanceAtkText->setFontName(FONT_FZZHENGHEI);
	advanceHpText->setFontName(FONT_FZZHENGHEI);
	advancePddText->setFontName(FONT_FZZHENGHEI);
	advanceMddText->setFontName(FONT_FZZHENGHEI);
	//需根据攻击类型区分
	mUI.mAdvAtkText = dynamic_cast<Text*>(mUI.mAdvPropertyPanel->getChildByName("Label_gongji"));
	mUI.mAdvAtkText->setFontName(FONT_FZZHENGHEI);
	mUI.mAdvAtkIcon = dynamic_cast<ImageView*>(mUI.mAdvPropertyPanel->getChildByName("Image_gongji"));

	//突破
	mUI.mBreakTitle1 = dynamic_cast<ImageView*>(mUI.mAdvInfoPanel->getChildByName("Image_xiaoguo")->getChildByName("Image_tupo"));
	mUI.mBreakTitle2 = dynamic_cast<ImageView*>(mUI.mAdvInfoPanel->getChildByName("Image_xiaohao")->getChildByName("Image_tupo"));
	//突破按钮
	mUI.mBreakBtn = dynamic_cast<Button*>(mUI.mAdvInfoPanel->getChildByName("Button_tupo"));
	mUI.mBreakBtn->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_START_BREAK));
	addAnimationStartAdv(mUI.mBreakBtn);

	//英雄升星面板UI(新改动内容)
	mUI.mPnlStarsUp = dynamic_cast<Layout*>(pPanelDetail->getChildByName("Panel_growstar"));
	mUI.mStarsUpPnlPos = mUI.mPnlStarsUp->getPosition();
	ImageView* imgParentDaDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(mUI.mPnlStarsUp, "Image_dadi"));	
	mUI.mPnlStarsOld = dynamic_cast<Layout*>(imgParentDaDi->getChildByName("Panel_star1"));				//升星前星级
	mUI.mPnlStarsNew = dynamic_cast<Layout*>(imgParentDaDi->getChildByName("Panel_star2"));				//升星后星级
	mUI.mStasrsUpTxt = dynamic_cast<Text*>(imgParentDaDi->getChildByName("Label_juexing_1"));
	mUI.mImgFullStars = dynamic_cast<ImageView*>(imgParentDaDi->getChildByName("Image_manxing"));
	mUI.mPnlStarsAttri = dynamic_cast<Layout*>(imgParentDaDi->getChildByName("Panel_shengxing"));
	ImageView* imgParentXiaoDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(mUI.mPnlStarsUp, "Image_xiaodi"));	
	mUI.mPnlAdvChips = dynamic_cast<Layout*>(imgParentXiaoDi->getChildByName("Panel_wangneng"));		//万能碎片
	mUI.mPnlHeroChips = dynamic_cast<Layout*>(imgParentXiaoDi->getChildByName("Panel_huoqu"));			//英雄碎片
	ImageView* imgParentJinDu = dynamic_cast<ImageView*>(Helper::seekWidgetByName(mUI.mPnlStarsUp, "Image_jindudi"));
	mUI.mHeroStarUpBar = dynamic_cast<LoadingBar*>(imgParentJinDu->getChildByName("ProgressBar_jindu"));			//英雄升星进度条
	mUI.mHeroChipCount = dynamic_cast<Text*>(imgParentJinDu->getChildByName("Label_jindu"));						//英雄升星碎片提示
	mUI.mBtnStarsUp = dynamic_cast<Button*>(mUI.mPnlStarsUp->getChildByName("Button_shengxing"));
	mUI.mBtnStarsUp->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_START_STARUP));
	//升星属性相关
	mUI.mAtkCurNum = dynamic_cast<Text*>(mUI.mPnlStarsAttri->getChildByName("Label_gongji_1"));
	mUI.mAtkNextNum = dynamic_cast<Text*>(mUI.mPnlStarsAttri->getChildByName("Label_gongji_2"));
	mUI.mHpCurNum = dynamic_cast<Text*>(mUI.mPnlStarsAttri->getChildByName("Label_xueliang_1"));
	mUI.mHpNextNum = dynamic_cast<Text*>(mUI.mPnlStarsAttri->getChildByName("Label_xueliang_2"));
	mUI.mPddCurNum = dynamic_cast<Text*>(mUI.mPnlStarsAttri->getChildByName("Label_wufang_1"));
	mUI.mPddNextNum = dynamic_cast<Text*>(mUI.mPnlStarsAttri->getChildByName("Label_wufang_2"));
	mUI.mMddCurNum = dynamic_cast<Text*>(mUI.mPnlStarsAttri->getChildByName("Label_fafang_1"));
	mUI.mMddNextNum = dynamic_cast<Text*>(mUI.mPnlStarsAttri->getChildByName("Label_fafang_2"));

	mUI.mAtkArrow = dynamic_cast<ImageView*>(mUI.mPnlStarsAttri->getChildByName("Image_up1"));
	mUI.mHpArrow = dynamic_cast<ImageView*>(mUI.mPnlStarsAttri->getChildByName("Image_up2"));
	mUI.mPddArrow = dynamic_cast<ImageView*>(mUI.mPnlStarsAttri->getChildByName("Image_up3"));
	mUI.mMddArrow = dynamic_cast<ImageView*>(mUI.mPnlStarsAttri->getChildByName("Image_up4"));

	mUI.mHeroStarUpBar->setPercent(0.0f);
	
	mUI.mStasrsUpTxt->setFontName(FONT_FZZHENGHEI);
	mUI.mStasrsUpTxt->setString("");

	mUI.mHeroChipCount->setFontName(FONT_FZZHENGHEI);
	mUI.mHeroChipCount->setString("");

	mUI.mAtkCurNum->setFontName(FONT_FZZHENGHEI);
	mUI.mAtkCurNum->setString("");
	mUI.mAtkNextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mAtkNextNum->setString("");
	mUI.mHpCurNum->setFontName(FONT_FZZHENGHEI);
	mUI.mHpCurNum->setString("");
	mUI.mHpNextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mHpNextNum->setString("");
	mUI.mPddCurNum->setFontName(FONT_FZZHENGHEI);
	mUI.mPddCurNum->setString("");
	mUI.mPddNextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mPddNextNum->setString("");
	mUI.mMddCurNum->setFontName(FONT_FZZHENGHEI);
	mUI.mMddCurNum->setString("");
	mUI.mMddNextNum->setFontName(FONT_FZZHENGHEI);
	mUI.mMddNextNum->setString("");

	Text* atk = (Text*)mUI.mPnlStarsAttri->getChildByName("Label_gongji");
	atk->setFontName(FONT_FZZHENGHEI);
	Text* hp = (Text*)mUI.mPnlStarsAttri->getChildByName("Label_xueliang");
	hp->setFontName(FONT_FZZHENGHEI);
	Text* pdd = (Text*)mUI.mPnlStarsAttri->getChildByName("Label_wufang");
	pdd->setFontName(FONT_FZZHENGHEI);
	Text* mdd = (Text*)mUI.mPnlStarsAttri->getChildByName("Label_fafang");
	mdd->setFontName(FONT_FZZHENGHEI);

	//滚动条
	m_starsAttriScrollBar = CustomScrollBar::create();
	m_starsAttriScrollBar->initScrollBar(mUI.mScrollPanel->getContentSize().height);
	m_starsAttriScrollBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_starsAttriScrollBar->setPosition(Point(20, pImgHeroProperty->getContentSize().height/2));
	pImgHeroProperty->addChild(m_starsAttriScrollBar);
	m_starsAttriScrollBar->setScrollBarLengthPercent(1.0f);
	m_starsAttriScrollBar->setScrollBarPercentY(0.0f);
	mUI.mScrollPanel->addEventListener(CC_CALLBACK_2(LayerHero::onStarsUpScrollViewEvent, this));
	mUI.mScrollPanel->jumpToTop();

	m_starsAttriScrollBar->setScrollBarLengthPercent(mUI.mScrollPanel->getContentSize().height / mUI.mScrollPanel->getInnerContainerSize().height);
	m_starsAttriScrollBar->setScrollBarPercentY(0.0f);

	//星级
	for (int i = 0, j = STAR_COUNT-1; i < STAR_COUNT; i++, j--)
	{
		mUI.mOldStarImg[j] = dynamic_cast<ImageView*>(mUI.mPnlStarsOld->getChildByName("Image_star_" + TO_STR(i) + "_" + TO_STR(i)));
		mUI.mNewStarImg[j] = dynamic_cast<ImageView*>(mUI.mPnlStarsNew->getChildByName("Image_star_" + TO_STR(i) + "_" + TO_STR(i)));
	}

	//万能碎片面板
	mUI.mAlmightyPanel = dynamic_cast<Layout*>(mUI.mRoot->getChildByName("Panel_wanneng"));
	mUI.mAlmightyPanel->setVisible(false);				//默认不显示
	ImageView* imgParentDi = dynamic_cast<ImageView*>(mUI.mAlmightyPanel->getChildByName("Image_di"));
	ImageView* imgParentWNDi = dynamic_cast<ImageView*>(imgParentDi->getChildByName("Image_xiaodi"));
	mUI.mPnlAlmightyIcon = dynamic_cast<Layout*>(imgParentWNDi->getChildByName("Panel_wanneng"));
	mUI.mPnlHeroIcon = dynamic_cast<Layout*>(imgParentWNDi->getChildByName("Panel_yingxiong"));
	mUI.mTextAlmighty = dynamic_cast<Text*>(imgParentWNDi->getChildByName("Label_wanneng"));
	mUI.mTextHero = dynamic_cast<Text*>(imgParentWNDi->getChildByName("Label_yingxiongsuipian"));
	mUI.mBarNumProgress = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(imgParentWNDi, "ProgressBar_jindu"));
	mUI.mTextNumProgress = dynamic_cast<Text*>(Helper::seekWidgetByName(imgParentWNDi, "Label_jindu"));
	mUI.mTextExchangeTips = dynamic_cast<Text*>(imgParentWNDi->getChildByName("Label_tishi"));
	mUI.mBtnExChangeOne = dynamic_cast<Button*>(imgParentDi->getChildByName("Button_duihuan1"));
	mUI.mBtnExChangeOne->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_EXCHANGE_ONE));
	mUI.mBtnExChangeTen = dynamic_cast<Button*>(imgParentDi->getChildByName("Button_duihuan10"));
	mUI.mBtnExChangeTen->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_EXCHANGE_TEN));
	mUI.mBtnClose = dynamic_cast<Button*>(imgParentDi->getChildByName("Button_Close"));
	mUI.mBtnClose->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_CLOSE));

	mUI.mTextAlmighty->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(mUI.mTextAlmighty, Color3B(0x79, 0x03, 0x03), 2);

	mUI.mTextHero->setFontName(FONT_FZZHENGHEI);
	mUI.mTextHero->setString("");

	mUI.mTextNumProgress->setFontName(FONT_FZZHENGHEI);
	mUI.mTextNumProgress->setString("");

	mUI.mTextExchangeTips->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(mUI.mTextExchangeTips, Color3B::BLACK, 2);

	//粒子效果
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniCharaparticle/AniCharaparticle.ExportJson");
	mUI.mParticleFrontAni = Armature::create("AniCharaparticle");
	mUI.mParticleFrontAni->setPosition(Point(mUI.mHeroSpinePanel->getContentSize().width/2, 28));
	mUI.mParticleFrontAni->setVisible(false);
	mUI.mHeroSpinePanel->addChild(mUI.mParticleFrontAni, 2);
	mUI.mParticleBackAni = Armature::create("AniCharaparticle");
	mUI.mParticleBackAni->setPosition(Point(mUI.mHeroSpinePanel->getContentSize().width/2, 28));
	mUI.mParticleBackAni->setVisible(false);
	mUI.mHeroSpinePanel->addChild(mUI.mParticleBackAni, 0);

	//默认显示英雄面板
	mUI.mHeroPanel->setVisible(true);
	mUI.mHeroInfoPanel->setVisible(false);
	mUI.mAdvInfoPanel->setVisible(false);

	//默认显示属性按钮
	//mUI.mSkillBtn->setVisible(false);
	//mUI.mAttriBtn->setVisible(true);
}

//初始化装备图标
void LayerHero::initEquipIcon(UIToolHeader** equipIcon, Node* parent, HERO_BTN_ID btnId)
{
	(*equipIcon) = UIToolHeader::create();
	(*equipIcon)->setTouchEnabled(true);
	(*equipIcon)->setAnchorPoint(Point::ANCHOR_MIDDLE);
	(*equipIcon)->setPosition(parent->getContentSize() / 2);
	(*equipIcon)->setVisible(false);
	(*equipIcon)->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, btnId));
	parent->addChild(*equipIcon);
}

void LayerHero::initHeroList(int curId)
{	
	int curHeroId = curId;
	Vector<HeroParam*>* vecHeroList = HeroModel::getInstance()->getHeroList();
	int nScrollViewOriHeight = mUI.mHeroListScrollView->getSize().height;
	int scrollRealHeight = vecHeroList->size() * 120;
	int scrollViewHeight = (scrollRealHeight > nScrollViewOriHeight) ? scrollRealHeight : nScrollViewOriHeight;
	//
	if (nScrollViewOriHeight>=scrollViewHeight)
	{
		mUI.mScrollUpBtn->setVisible(false);
		mUI.mScrollDownBtn->setVisible(false);
	}
	else
	{
		mUI.mScrollUpBtn->setVisible(false);
		mUI.mScrollDownBtn->setVisible(true);
	}
	
	mUI.mVecHeroIcon.clear();
	mUI.mHeroListScrollView->setInnerContainerSize(Size( mUI.mHeroListScrollView->getContentSize().width, scrollViewHeight));
	cocos2d::Size innerSize = mUI.mHeroListScrollView->getInnerContainerSize();
	for (size_t i = 0; i < vecHeroList->size(); i++)
	{
		HeroParam* heroParam = vecHeroList->at(i);
		UIToolHeader* pHeader = UIToolHeader::create(ResourceUtils::getSmallIconPath(heroParam->mResId), 
			"Image/Icon/global/btn_kuang_juese_1.png", "Image/Icon/global/btn_kuang_juese_2.png");
		if (pHeader != nullptr)
		{
			//如果是绿色以上(含绿色)品质(qua >= 2),需要设置角标
			if(heroParam->mQuality >= 2)
			{
				pHeader->setQuaCornerIcon(heroParam->mTemplateId, heroParam->mAdvLvl);
			}	
			pHeader->setStarsNum(heroParam->mHeroStars, STAR_CONTAIN_BOTTOM);
			pHeader->setPosition(Vec2(innerSize.width / 2.0f, innerSize.height - 120/2.0f - i * ( 120 )));
			pHeader->setTag(heroParam->mHeroId);
			pHeader->setLvlWithoutBg(heroParam->mLevel);
			pHeader->addTouchEventListener(CC_CALLBACK_2(LayerHero::onHeroListClicked, this, heroParam->mHeroId));
			pHeader->setTouchEnabled(true);
			mUI.mVecHeroIcon.push_back(pHeader);
			mUI.mHeroListScrollView->addChild(pHeader);
		}
		//设置第一项为选中项
		if (curHeroId == 0)
		{
			curHeroId = heroParam->mHeroId;
			pHeader->setSelected(true);
		}
		else if (curHeroId == heroParam->mHeroId)
		{
			pHeader->setSelected(true);
			mCurrentHeroIndex = i;
		}
	}

	scrollHeroList();
	setCurrentHeroId(curHeroId);
}

void LayerHero::onHeroListClicked(Ref* ref, Widget::TouchEventType type, int nHeroId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		//解决快速点击会卡死的问题
		auto callBack = [this, ref, nHeroId](float dt)->void
		{
			//清除选中项
			Vector<Node*>& listItems = mUI.mHeroListScrollView->getChildren();
			for(auto iter = listItems.begin();iter != listItems.end(); iter++ )
			{
				UIToolHeader* pHeader = (UIToolHeader*)*iter;
				if (pHeader!= nullptr )
				{
					if (pHeader->isSelected())
					{
						pHeader->setSelected(false);
					}
				}
			}
			UIToolHeader* pTool = (UIToolHeader*)ref;
			pTool->setSelected(true);
			setCurrentHeroId(nHeroId);
			showEnterAnimation();

			this->getScheduler()->unschedule("LayerHero::onHeroListClick", this);
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerHero::onHeroListClick");
	}
}


void LayerHero::onScrollViewEvent(Ref* sender, ScrollviewEventType type)
{
	ScrollView* pScroll = (ScrollView*)sender;
	Size contentSize = pScroll->getContentSize();
	Size size = pScroll->getInnerContainerSize();
	if (contentSize.height>=size.height)
	{
		return;
	}
	switch (type)
	{
	case SCROLLVIEW_EVENT_SCROLL_TO_TOP:
		{
		}
		break;
	case SCROLLVIEW_EVENT_SCROLL_TO_BOTTOM:
		{
		}
		break;
	case SCROLLVIEW_EVENT_SCROLLING:
		{
			Vec2 vec = pScroll->getInnerContainer()->getPosition();
			if (vec.y > (contentSize.height-size.height)/2)
			{
				mUI.mScrollDownBtn->setVisible(true);
			}
			else if (vec.y <= contentSize.height-size.height)
			{
				mUI.mScrollUpBtn->setVisible(true);
			}
			else
			{
			}
		}
		break;
	case SCROLLVIEW_EVENT_BOUNCE_TOP:
		{
			mUI.mScrollUpBtn->setVisible(false);
		}
		break;
	case SCROLLVIEW_EVENT_BOUNCE_BOTTOM:
		{
			mUI.mScrollDownBtn->setVisible(false);
		}
		break;
	default:
		break;
	}
}

void LayerHero::onBtnFateClicked(Ref* ref, Widget::TouchEventType type, int nIndex)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (mCurrentHeroId == HeroModel::getInstance()->getMainHero()->mHeroId)
		{
			if(GuideManager::getInstance()->isFunctionOpen(FUNCTION_MAIN_HERO_FATE))
			{
				auto callBack = [this](float dt)->void
				{
					this->getScheduler()->unschedule("LayerHero::switchFate", this);
					//主角缘分
					Director::getInstance()->pushScene(SceneHeroFate::create());
				};

				getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerHero::switchFate");	
			}
		}
		else
		{
			auto callBack = [=](float dt)->void
			{
				this->getScheduler()->unschedule("LayerHero::switchHeroFate", this);
				//英雄缘分
				//Director::getInstance()->replaceScene(HeroRelationScene::create(mHeroParam->mTemplateId, false));
				Director::getInstance()->pushScene(HeroRelationScene::create(mHeroParam->mTemplateId, true, this));
			};

			getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerHero::switchHeroFate");	
		}	
	}
}

//进阶材料不足，点击到数据来源界面
void LayerHero::onBtnMatClicked(Ref* ref, Widget::TouchEventType type, int ntmplateId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		LayerEquipSource* pSmithy = LayerEquipSource::create();
		this->addChild(pSmithy);
		pSmithy->showSrc(ntmplateId);
	}
}
//
void LayerHero::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
	case nMAIN_CMD_TALENT_REPLACE:
		updateHeroCard();
		break;
	case nMAIN_CMD_HERO_ADVANCE_INFO:
		{
			updateUIAttri();
			updateHeroCard(); //进阶可能更新技能
			updateAdvInfo();

			//播放预览动画
			startPreviewNextValue();
			//播放觉醒动画
			startAnimationAdv();
		}
		break;
	case nMAIN_CMD_HERO_STAR_UP_INFO:
		{
			updateUIAttri();
			updateHeroStarUpInfo();

			//还需要更新卡牌的星级
			updateHeroCard();
			showAdvComfirmLayer(STAR_UP_TYPE);
		}
		break;
	case nMAIN_CMD_TOOL_PUTON:
		{
			updateAllEquip();
			updateUIAttri();
			updateAdvInfo();

			//显示装备效果
			int equipId = *((int*)observerParam->updateData);
			const TOOL_INFO* toolInfo = ToolModel::getInstance()->GetToolInfo(equipId);
			auto equip = this->getEquipBtn((EQUIP_ITEM_TYPE)toolInfo->nEquipType);
			if (equip)
			{
				equip->showEquipEffe();
			}
		}
		break;
	case nMAIN_CMD_TRAINING_ADD_EXP:
		{
			updateUIAttri();
			updateAdvInfo();
		}
		break;
	case nMAIN_CMD_EXCHANGE_CHIPS:
		{
			updateChipsDetails();
		}
		break;
	default:
		break;
	}
	updateHeroRedFlag();
	updateEquipRedFlag();
}

void LayerHero::updateAllUI()
{
	if (mHeroParam)
	{
		bool isMainHero = HERO_TYPE::MAIN_HERO == mHeroParam->mRoleType;
		mUI.mMainHeroBtnPanle->getChildByName("Button_tianfu")->setVisible(isMainHero);
		mUI.mHeroBtnPanel->getChildByName("Button_shengji")->setVisible(!isMainHero);

		//更新缘分
		updateFateUI();
	
		//添加骨骼动画
		addHeroSpine();

		//更新所有装备
		updateAllEquip();

		//英雄属性
		updateUIAttri();

		//进阶属性
		updateAdvInfo();

		//更新升星属性
		updateHeroStarUpInfo();

		//英雄卡牌
		addHeroCard();

		//英雄可进阶标记
		updateHeroRedFlag();

		//装备可进阶标记
		updateEquipRedFlag();
	}
}

void LayerHero::showAdvComfirmLayer(growType type)
{
	switch (type)
	{
	case QUATITY_TYPE:
		{
			auto layer = LayerHeroAdvance::create(mPlayAniHeroId);
			layer->show(this);
		}
		break;
	case STAR_UP_TYPE:
		{
			auto layer = LayerHeroStarsUp::create(mPlayAniHeroId);
			layer->show(this);
		}
		break;
	default:
		break;
	}
	
}

void LayerHero::addHeroSpine()
{
	if (mHeroParam)
	{
		int nResID = mHeroParam->mResId;
		mUI.mHeroSpinePanel->removeChildByName("HeroSpine");
		SpineAnimation* pSkelFighter = SpineAnimation::createFighterWithResId(nResID);
		//pSkelFighter->setSkin(FIGHTER_SKIN_1);
		pSkelFighter->setPosition(Vec2(mUI.mHeroSpinePanel->getContentSize().width/2, 28));
		pSkelFighter->setAnimation(0, "wait", true);
		mUI.mHeroSpinePanel->addChild(pSkelFighter, 1, "HeroSpine");
	}
}

void LayerHero::updateHeroParticle()
{
	if(mHeroParam)
	{
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
			mUI.mParticleFrontAni->setVisible(true);
			mUI.mParticleFrontAni->getAnimation()->play(frontName);
		}
		else
		{
			mUI.mParticleFrontAni->setVisible(false);
		}
		
		if (!backName.empty())
		{
			mUI.mParticleBackAni->setVisible(true);
			mUI.mParticleBackAni->getAnimation()->play(backName);
		}
		else
		{
			mUI.mParticleFrontAni->setVisible(false);
		}
	}
}

//添加卡牌
void LayerHero::addHeroCard()
{
	if (mHeroParam)
	{
		mUI.mHeroPanel->removeChild(mUI.mHeroCard);
		mUI.mHeroCard = UICardHead::create(mHeroParam->mTemplateId, true);
		mUI.mHeroCard->setAnchorPoint(Point::ANCHOR_MIDDLE);
		mUI.mHeroCard->setPosition(mUI.mHeroPanel->getContentSize() / 2);
		mUI.mHeroPanel->addChild(mUI.mHeroCard);
	}
}

//更新英雄红色标记
void LayerHero::updateHeroRedFlag()
{
	for (int i = 0; i < mUI.mVecHeroIcon.size(); i++)
	{
		UIToolHeader* heroIcon = mUI.mVecHeroIcon.at(i);
		HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(heroIcon->getTag());
		bool redVisible = heroParam && (heroParam->isHeroAdvValid() || heroParam->isHeroEquipAdvValid() 
			|| heroParam->isHeroEquipPutOn() || heroParam->isHeroStarUpValid());
		heroIcon->setRedIconFlag(redVisible);

		//实时刷新星级和品质角标
		heroIcon->setStarsNum(heroParam->mHeroStars, STAR_CONTAIN_BOTTOM);
		heroIcon->setQuaCornerIcon(heroParam->mTemplateId, heroParam->mAdvLvl);
		heroIcon->setLvlWithoutBg(heroParam->mLevel);
	}
	//进阶按钮小红点
	if(mHeroParam->isMainHero())
	{
		mUI.mMainHeroAdvFlagImg->setVisible(HeroModel::getInstance()->isHeroAdvValid(mCurrentHeroId));
	}
	else
	{
		mUI.mHeroAdvFlagImg->setVisible(HeroModel::getInstance()->isHeroAdvValid(mCurrentHeroId));
	}
	//升星按钮小红点
	if(!mHeroParam->isMainHero())
	{
		mUI.mHeroStarUpFlagImg->setVisible(HeroModel::getInstance()->isHeroStarUpValid(mCurrentHeroId));
		if(!mUI.mHeroStarUpFlagImg->isVisible() && mHeroParam->mHeroStars < STAR_CONT)
		{
			//mUI.mHeroChipsAddFlagImg->setVisible(true);
		}
		else
		{
			//mUI.mHeroChipsAddFlagImg->setVisible(false);
		}
	}
	else
	{
		//主角更新星宿小红点
		int starNum = MainModel::getInstance()->getMainParam()->mStarNum;
		//获取星宿信息
		const STARSTSU_INFO& starsInfo = StarsSuModel::getInstance()->getStarssSuInfo();
		RowData *AstrologyData = DataManager::getInstance()->searchStarById(starsInfo.CursId);
		if(AstrologyData)
		{
			mUI.mStarsSuFlagImg->setVisible(starNum >= AstrologyData->getIntData("needStar"));
		}	
	}
}

void LayerHero::updateEquipRedFlag()
{
	if (mHeroParam->mWeaponId > 0)
	{
		mUI.mWeapon->setAdvFlag(SmithyModel::getInstance()->checkEquState(mHeroParam->mWeaponId));
	}
	if (mHeroParam->mBodyId > 0)
	{
		mUI.mClothes->setAdvFlag(SmithyModel::getInstance()->checkEquState(mHeroParam->mBodyId));
	}
	if (mHeroParam->mHeaderId > 0)
	{
		mUI.mHat->setAdvFlag(SmithyModel::getInstance()->checkEquState(mHeroParam->mHeaderId));
	}
	if (mHeroParam->mEarringId > 0)
	{
		mUI.mEarring->setAdvFlag(SmithyModel::getInstance()->checkEquState(mHeroParam->mEarringId));
	}
	if (mHeroParam->mBookId > 0)
	{
		mUI.mBook->setAdvFlag(SmithyModel::getInstance()->checkEquState(mHeroParam->mBookId));
	}
	if (mHeroParam->mHorseId > 0)
	{
		mUI.mHorse->setAdvFlag(SmithyModel::getInstance()->checkEquState(mHeroParam->mHorseId));
	}

}

//更新英雄属性
void LayerHero::updateUIAttri()
{
	if (mHeroParam)
	{
		//战斗力
		mUI.mFightTextNum->setString(TO_STR(mHeroParam->mFighting));

		//速度
		mUI.mSpeed->setString(TO_STR(mHeroParam->mSpeed));

		//属性
		mUI.mAtkTextNum->setString(TO_STR(mHeroParam->mAtk));
		mUI.mHpTextNum->setString(TO_STR(mHeroParam->mHp));
		mUI.mPddTextNum->setString(TO_STR(mHeroParam->mPDef));
		mUI.mMddTextNum->setString(TO_STR(mHeroParam->mMDef));

		//详细属性
		//区分攻击
		Text* txtAtkType = dynamic_cast<Text*>(mUI.mScrollPanel->getChildByName("Label_gongji"));
		ImageView* imgAtkType = dynamic_cast<ImageView*>(mUI.mScrollPanel->getChildByName("Image_gongji"));
		if(mHeroParam->mAtkTyp == HERO_ATK_PHY)
		{
			//txtAtkType->setString(STR_UTF8("物攻成长"));
			txtAtkType->setString(DataManager::getInstance()->searchCommonTexdtById(10221));
			imgAtkType->loadTexture(ResourceUtils::getIconGlobalPath("img_gongji2.png"));
		}
		else
		{
			//txtAtkType->setString(STR_UTF8("法攻成长"));
			txtAtkType->setString(DataManager::getInstance()->searchCommonTexdtById(10222));
			imgAtkType->loadTexture(ResourceUtils::getIconGlobalPath("img_fagong2.png"));
		}
		mUI.mAtkAdd->setString(TO_STR(mHeroParam->mAtkGrow));
		mUI.mHpAdd->setString(TO_STR(mHeroParam->mHpGrow));
		mUI.mMddAdd->setString(TO_STR(mHeroParam->mMddGrow));
		mUI.mPddAdd->setString(TO_STR(mHeroParam->mPddGrow));
		mUI.mHitPrbTextNum->setString(TO_STR(mHeroParam->mHitPrb) + "%");
		mUI.mCritTextNum->setString(TO_STR(mHeroParam->mCritP) + "%");
		mUI.mCritCoeTextNum->setString(TO_STR(mHeroParam->mCritCoe) + "%");
		mUI.mComboTextNum->setString(TO_STR(mHeroParam->mComboP) + "%");
		mUI.mMisPrbTextNum->setString(TO_STR(mHeroParam->mMissP) + "%");
		mUI.mCritResisTextNum->setString(TO_STR(mHeroParam->mCriResist) + "%");

		//名字
		mUI.mNameText->setText(mHeroParam->getAdvName());
		setNameColor(mUI.mNameText, mHeroParam->mQuality);

		//资质
		//mUI.mTalentText->setString(_TO_STR(mHeroParam->mTalent));
		//mUI.mHeroTalent->setString(_TO_STR(mHeroParam->mTalent));
		mUI.mImgTalent->loadTexture(ResourceUtils::getHeroTalentPath(mHeroParam->mTalent));
		mUI.mImgHeroTalent->loadTexture(ResourceUtils::getHeroTalentPath(mHeroParam->mTalent));

		//介绍
		RowData* pHeroInfo = DataManager::getInstance()->searchToolById(mHeroParam->mTemplateId);
		string info = pHeroInfo->getStringData("itemInfo");
		mUI.mHeroIntroText->setString(info);
		//称号
		mUI.mNickNameImg->loadTexture(ResourceUtils::getNickNamePath(mHeroParam->mResId));

		//调整昵称*名字坐标
		setNickNamePos();

		//星级
		for (int i = 0; i < STAR_CONT; i++)
		{
			bool visible = i < mHeroParam->mHeroStars;
			mUI.mTopStarImg[i]->setVisible(visible);
		}

		mUI.mImgHeroType->setVisible(true);
		switch (mHeroParam->mJobTyp)
		{
		case HERO_JOB_TYPE::HERO_DEFENSE:
			{
				mUI.mImgAttriType->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_fang.png"));
				mUI.mImgJobType->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_fang.png"));	
			}	
			break;
		case HERO_JOB_TYPE::HERO_ATK:
			{
				mUI.mImgAttriType->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_gong.png"));
				mUI.mImgJobType->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_gong.png"));
			}
			break;
		case HERO_JOB_TYPE::HERO_HELP:
			{
				mUI.mImgAttriType->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_fu.png"));
				mUI.mImgJobType->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_fu.png"));
			}
			break;
		case HERO_JOB_TYPE::HERO_FIRST:
			{
				mUI.mImgAttriType->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_xian.png"));
				mUI.mImgJobType->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_xian.png"));
			}
			break;

		default:
			break;
		}

		switch (mHeroParam->mHeroType)
		{
		case HERO_PROPERTY_TYPE::HERO_STR: //火属性英雄
			{
				mUI.mImgHeroType->loadTexture(ResourceUtils::getIconGlobalPath("img_shuxing11_li.png"));
				//mUI.mHeroTypeIntroText->setString(STR_UTF8("克制木属性英雄，造成额外伤害"));
			}
			break;
		case HERO_PROPERTY_TYPE::HERO_AGI: //木属性英雄
			{
				mUI.mImgHeroType->loadTexture(ResourceUtils::getIconGlobalPath("img_shuxing22_min.png"));
				//mUI.mHeroTypeIntroText->setString(STR_UTF8("克制水属性英雄，造成额外伤害"));
			}
			break;
		case HERO_PROPERTY_TYPE::HERO_INT:	//水属性英雄
			{
				mUI.mImgHeroType->loadTexture(ResourceUtils::getIconGlobalPath("img_shuxing33_zhi.png"));
				//mUI.mHeroTypeIntroText->setString(STR_UTF8("克制火属性英雄，造成额外伤害"));
			}
			break;
		default:
			{
				mUI.mImgHeroType->setVisible(false);
				//mUI.mHeroTypeIntroText->setString(STR_UTF8("不受其他属性英雄克制"));
			}
			break;
		}

		if (mHeroParam->mAtkTyp == HERO_ATK_PHY)
		{
			//mUI.mInfoAtkAddText->setString(STR_UTF8("物攻成长"));
			//mUI.mInfoAtkAddIcon->loadTexture(ResourceUtils::getIconGlobalPath("img_gongji2.png"));
			mUI.mAtkIcon->loadTexture(ResourceUtils::getIconGlobalPath("img_gongji.png"));
		}
		else
		{
			//mUI.mInfoAtkAddText->setString(STR_UTF8("法攻成长"));
			//mUI.mInfoAtkAddIcon->loadTexture(ResourceUtils::getIconGlobalPath("img_fagong2.png"));
			mUI.mAtkIcon->loadTexture(ResourceUtils::getIconGlobalPath("img_fagong.png"));
		}

		mUI.mScrollPanel->jumpToTop();
	}
}

void LayerHero::updateHeroCard()
{
	if (mUI.mHeroCard)
	{
		mUI.mHeroCard->updataUi();
	}
}

void LayerHero::updateAllEquip()
{
	if (mHeroParam)
	{
		updateEquip(mUI.mWeaponImg, mHeroParam->mWeaponId, EQUIP_ITEM_TYPE_WEAPON);
		updateEquip(mUI.mClothesImg, mHeroParam->mBodyId, EQUIP_ITEM_TYPE_CLOTHES);
		updateEquip(mUI.mHatImg, mHeroParam->mHeaderId, EQUIP_ITEM_TYPE_HAT);
		updateEquip(mUI.mEarringImg, mHeroParam->mEarringId, EQUIP_ITEM_TYPE_EARRINGS);
		updateEquip(mUI.mHorseImg, mHeroParam->mHorseId, EQUIP_ITEM_TYPE_HORSE);
		updateEquip(mUI.mBookImg, mHeroParam->mBookId, EQUIP_ITEM_TYPE_BOOK);
	}
}

void LayerHero::updateEquip(Node* equipNode, int equipId, EQUIP_ITEM_TYPE equipType)
{
	UIToolHeader* icon = getEquipBtn(equipType);
	if (equipId > 0)
	{
		equipNode->getChildByName("Image_kechuandai")->setVisible(false);
		equipNode->getChildByName("Image_wuzhuangbei")->setVisible(false);
		//如果有装备则显示装备图标
		const TOOL_INFO* pToolInfo = ToolModel::getInstance()->GetToolInfo(equipId);

		//计算装备淬炼ID
		int nCLId = SmithyModel::getInstance()->calcCLId(equipId);
		RowData* _row = DataManager::getInstance()->searchEquipmentQuenchById(nCLId);
		CCASSERT(_row != nullptr, "");
		
		if (pToolInfo != nullptr)
		{
			RowData *pEquipment = DataManager::getInstance()->searchEquipmentById(pToolInfo->ntemplateId);
			if(pEquipment)
			{
				icon->setJJLvl((TOOL_ITEM_QUA)pToolInfo->nItemQua, pEquipment->getIntData("stars"));				
			}
			icon->setCLLevel(pToolInfo->nCLLevel);
			//天命
			bool bIsFate = (_row->getIntData("fate") == 1 ? true : false);
			icon->showTianMingFlag(bIsFate && (pToolInfo->nCLLevel > 0));

			icon->setLvlWithoutBg(pToolInfo->nadvLvl);
			icon->setMainIcon(ResourceUtils::getSmallIconPath(_row->getIntData("icon")));		
			icon->setIconQua(pToolInfo->nItemQua);
			icon->setAdvFlag(SmithyModel::getInstance()->checkEquState(equipId));	
		}
		icon->setVisible(pToolInfo != nullptr);
		//新需求，兵书马书开放状态添加主角等级限制条件
		if(equipType == EQUIP_ITEM_TYPE_HORSE || equipType == EQUIP_ITEM_TYPE_BOOK)
		{
			int limitLevel = equipType == EQUIP_ITEM_TYPE_HORSE ? m_iHorseEquipOpenLv : m_iBookEquipOpenLv;
			icon->lock(HeroModel::getInstance()->getMainHeroLevel() < limitLevel);
		}
	}
	else
	{
		UIToolHeader* icon = getEquipBtn(equipType);
		icon->setVisible(false);
		if (ToolModel::getInstance()->searchPutOnEquip(equipType) > 0)
		{
			//如果没有装备则判定是否有可佩戴装备，并显示可装备标记
			equipNode->getChildByName("Image_kechuandai")->setVisible(true);
			equipNode->getChildByName("Image_wuzhuangbei")->setVisible(false);
		}
		else
		{
			//如果没有可佩戴装备，则显示无装备
			equipNode->getChildByName("Image_kechuandai")->setVisible(false);
			equipNode->getChildByName("Image_wuzhuangbei")->setVisible(true);
		}
	}
}

//增加技能动画
void LayerHero::addAnimationSkill(Button* skillBtn)
{
	skillBtn->removeChildByName("AniJinengLogo");
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniJinengLogo/AniJinengLogo.ExportJson");

	Armature* pArmature = Armature::create("AniJinengLogo");
	pArmature->getAnimation()->playByIndex(0);
	//设置动画精灵位置 
	pArmature->setPosition(skillBtn->getContentSize().width/2, skillBtn->getContentSize().height/2);
	skillBtn->addChild(pArmature,9, "AniJinengLogo");
}

void LayerHero::removeAnimationSkill(Button* skillBtn)
{
	skillBtn->removeChildByName("AniJinengLogo");
}

void LayerHero::removeAllEquipEffe()
{
// 	mUI.mWeapon->removeEquipEffe();
// 	mUI.mClothes->removeEquipEffe();
// 	mUI.mHorse->removeEquipEffe();
// 	mUI.mHat->removeEquipEffe();
// 	mUI.mEarring->removeEquipEffe();
// 	mUI.mBook->removeEquipEffe();
}

void LayerHero::setNameColor(Text* text, int color)
{
	switch (color)
	{
	case WHITE_COLOR:
		text->setColor(Color3B::WHITE);
		setTextAddStroke(text, Color3B(0x62, 0x3e, 0x17), 2);
		break;
	case GREEN_COLOR:
		text->setColor(Color3B(0xa9, 0xff, 0x5d));
		setTextAddStroke(text, Color3B(0x10, 0x60, 0x14), 2);
		break;
	case BLUE_COLOR:
		text->setColor(Color3B(0x84, 0xff, 0xef));
		setTextAddStroke(text, Color3B(0x2b, 0x3f, 0x82), 2);
		break;
	case PURPLE_COLOR:
		text->setColor(Color3B(0xe7, 0x9d, 0xff));
		setTextAddStroke(text, Color3B(0x41, 0x17, 0x6b), 2);
		break;
	case ORANGE_COLOR:
		text->setColor(Color3B(0xfa, 0xff, 0x6a));
		setTextAddStroke(text, Color3B(0x65, 0x27, 0x0c), 2);
		break;
	case RED_COLOR:
		text->setColor(Color3B(0xff, 0x00, 0x00));
		setTextAddStroke(text, Color3B(0x63, 0x06, 0x06), 2);
		break;
	default:
		break;
	}
}

//更新进阶信息
void LayerHero::updateAdvInfo()
{
	if (!mHeroParam)
	{
		return;
	}

	RowData* advInfo1 = DataManager::getInstance()->searchHeroAdvanceById(mHeroParam->mTemplateId, mHeroParam->mAdvLvl);
	RowData* advInfo2 = DataManager::getInstance()->searchHeroAdvanceById(mHeroParam->mTemplateId, mHeroParam->mAdvLvl + 1);
	//是否达到进阶上限, true为未达到上限
	bool advLimit = advInfo1 != nullptr && advInfo2 != nullptr;

	int curAdvValue = advInfo1->getIntData("value");
	int maxAdvValue = advInfo1->getIntData("maxValue");
	if(advLimit)
	{
		mUI.mQuaUpBar->setPercent(float(curAdvValue)/float(maxAdvValue) * 100.0f);
		mUI.mQuaUpJinDu->setString(_TO_STR(curAdvValue) + "/" + _TO_STR(maxAdvValue));
	}
	else
	{
		mUI.mQuaUpBar->setPercent(100.0f);
		//mUI.mQuaUpJinDu->setString(STR_UTF8("进阶到最高级"));
		mUI.mQuaUpJinDu->setString(DataManager::getInstance()->searchCommonTexdtById(10223));
	}

	mUI.mQuaName1->setString(advInfo1->getStringData("name"));
	mUI.mQuaName2->setString(advInfo1->getStringData("nextName"));	
	setNameColor(mUI.mQuaName1, advInfo1->getIntData("qua"));
	setNameColor(mUI.mQuaName2, advInfo1->getIntData("nextQua"));

	//觉醒tips，以及下一段觉醒技能提示
	mUI.mAdvanceSkillPanel->removeAllChildren();
	string tips = advInfo1->getStringData("show");
	vector<string> vecTips;
	StringFormat::parseCsvStringVec(tips, vecTips);

	mUI.mAdvanceTips1->setVisible(!mHeroParam->isMainHero());
	if (!vecTips.empty())
	{
		mUI.mAdvanceTips1->setString(vecTips[0]);
		if (vecTips.size() > 1)
		{
			int skillId = atoi(vecTips[1].c_str());
			SkillIcon* skillIcon = SkillIcon::create(skillId);
			skillIcon->setShowTips(true);
			skillIcon->setScale(0.7f);
			skillIcon->setPosition(mUI.mAdvanceSkillPanel->getContentSize() / 2);
			mUI.mAdvanceSkillPanel->addChild(skillIcon);
		}
	}

	//觉醒消耗

	if(advLimit)
	{
		mUI.mAdvanceCoin->setVisible(true);
		mUI.mAdvanceCoin->getParent()->getChildByName("Image_yinbi")->setVisible(true);
		mUI.mAdvanceCoin->setString(TO_STR(mHeroParam->mAdvCoin));
		mUI.mAdvanceMaxLv->setVisible(false);
		if (MainModel::getInstance()->getMainParam()->mCoin >= mHeroParam->mAdvCoin)
		{
			mUI.mAdvanceCoin->setColor(Color3B::WHITE);
		}
		else
		{
			mUI.mAdvanceCoin->setColor(Color3B::RED);
		}
	}		
	else
	{
		//如果达到最高觉醒等级
		mUI.mAdvanceCoin->setVisible(false);
		mUI.mAdvanceCoin->getParent()->getChildByName("Image_yinbi")->setVisible(false);
		mUI.mAdvanceMaxLv->setVisible(true);
		//mUI.mAdvanceMaxLv->setString(STR_UTF8("已觉醒到最高品质"));
		mUI.mAdvanceMaxLv->setString(DataManager::getInstance()->searchCommonTexdtById(10224));
	}

	mUI.mAdvanceMatPanel->setVisible(advLimit);
	mUI.mAdvanceMatPanel->removeAllChildren();
	int matCount = mHeroParam->mVecAdvMat.size();
	for (int i = 0; i < mHeroParam->mVecAdvMat.size(); i++)
	{
		AdvMatParam& advMatParam = mHeroParam->mVecAdvMat.at(i);
		UIToolHeader* icon = UIToolHeader::create(advMatParam.itemId);
		int curNum = ToolModel::getInstance()->searchNumByTemplateId(advMatParam.itemId);
		icon->setBottomNum(TO_STR(curNum) + "/" + TO_STR(advMatParam.needNum));
		icon->setScale(0.70f);
		icon->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
		int padding = 20;
		int iconWidth = icon->getBoundingBox().size.width;
		int margin = (mUI.mAdvanceMatPanel->getContentSize().width 
			- matCount * iconWidth - (matCount - 1) * padding) / 2;
		int xx = margin + i * (padding + iconWidth);
		int yy = mUI.mAdvanceMatPanel->getContentSize().height / 2 + 10;
		icon->setPosition(Point(xx, yy));
		if (curNum < advMatParam.needNum)
		{
			//当前道具数量小于所需数量时，道具可点击并显示来源
			icon->setTouchEnabled(true);
			icon->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnMatClicked, this,advMatParam.itemId));	
			icon->setBottomNumColor(Color3B(0xd9, 0x2e, 0x2e));
			//加遮罩			
			icon->setAddCornerIcon();
		}
		else
		{
			//mark
// 			icon->setTouchEnabled(true);
// 			icon->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnMatClicked, this,advMatParam.itemId));	
// 			icon->setToolId(advMatParam.itemId);
// 			icon->setTipsEnabled(true);
			icon->showMatEffect(true);
			icon->setBottomNumColor(Color3B(0x54, 0xdd, 0x51));
		}
		mUI.mAdvanceMatPanel->addChild(icon);
	}

	mUI.mNeedLevelTxt->setVisible(advLimit);
	//mUI.mNeedLevelTxt->setString(STR_UTF8("所需等级：" + TO_STR(mHeroParam->mAdvNeedLevel)));
	mUI.mNeedLevelTxt->setString(DataManager::getInstance()->searchCommonTexdtById(10225) + TO_STR(mHeroParam->mAdvNeedLevel));
	if (mHeroParam->mLevel >= mHeroParam->mAdvNeedLevel)
	{
		mUI.mNeedLevelTxt->setColor(Color3B(0xda, 0xdb, 0x5b));
	}
	else
	{
		mUI.mNeedLevelTxt->setColor(Color3B::RED);
	}

	bool advValid = mHeroParam->isHeroAdvValid();
	
	//判断是突破还是觉醒
	if (maxAdvValue != curAdvValue + 1)
	{
		startAnimationStartAdv(mUI.mBtnStartAdv, advValid);

		//显示觉醒标题、按钮
		mUI.mAdvTitle1->setVisible(true);
		mUI.mAdvTitle2->setVisible(true);
		mUI.mAdvPropertyPanel->setVisible(true);
		mUI.mBtnStartAdv->setVisible(true);
		//隐藏突破标题、按钮
		mUI.mBreakTitle1->setVisible(false);
		mUI.mBreakTitle2->setVisible(false);
		mUI.mBreakBtn->setVisible(false);
	}
	else
	{
		startAnimationStartAdv(mUI.mBreakBtn, advValid);

		//隐藏觉醒
		mUI.mAdvTitle1->setVisible(false);
		mUI.mAdvTitle2->setVisible(false);
		mUI.mAdvPropertyPanel->setVisible(true);
		mUI.mBtnStartAdv->setVisible(false);
		//显示突破
		mUI.mBreakTitle1->setVisible(true);
		mUI.mBreakTitle2->setVisible(true);
		mUI.mBreakBtn->setVisible(true);
	}

	//如果不可进阶，说明达到进阶上限，箭头不显示
	mUI.mArrow1->setVisible(advLimit);
	mUI.mArrow2->setVisible(advLimit);
	mUI.mArrow3->setVisible(advLimit);
	mUI.mArrow4->setVisible(advLimit);

	if (advLimit)
	{
		//显示预览信息
		mUI.mAdvanceAtkTextNum->setString("+" + TO_STR(advInfo2->getIntData("atk") - advInfo1->getIntData("atk")));
		mUI.mAdvanceHpTextNum->setString("+" + TO_STR(advInfo2->getIntData("hp") - advInfo1->getIntData("hp")));
		mUI.mAdvancePddTextNum->setString("+" + TO_STR(advInfo2->getIntData("pdd") - advInfo1->getIntData("pdd")));
		mUI.mAdvanceMddTextNum->setString("+" + TO_STR(advInfo2->getIntData("mdd") - advInfo1->getIntData("mdd")));
	}
	else
	{
		mUI.mAdvanceAtkTextNum->setString(TO_STR(mHeroParam->mAtk));
		mUI.mAdvanceHpTextNum->setString(TO_STR(mHeroParam->mHp));
		mUI.mAdvancePddTextNum->setString(TO_STR(mHeroParam->mPDef));
		mUI.mAdvanceMddTextNum->setString(TO_STR(mHeroParam->mMDef));
	}

	if (mHeroParam->mAtkTyp == HERO_ATK_PHY)
	{
		//mUI.mAdvAtkText->setString(STR_UTF8("物攻"));
		mUI.mAdvAtkText->setString(DataManager::getInstance()->searchCommonTexdtById(10226));
		mUI.mAdvAtkIcon->loadTexture(ResourceUtils::getIconGlobalPath("img_gongji.png"));
	}
	else
	{
		//mUI.mAdvAtkText->setString(STR_UTF8("法攻"));
		mUI.mAdvAtkText->setString(DataManager::getInstance()->searchCommonTexdtById(10227));
		mUI.mAdvAtkIcon->loadTexture(ResourceUtils::getIconGlobalPath("img_fagong.png"));
	}

	//与品质相关
	this->updateHeroParticle();
}

void LayerHero::updateHeroStarUpInfo()
{
	if (!mHeroParam)
	{
		return;
	}

	//中间头像星级
	for (int i = 0; i < STAR_CONT; i++)
	{
		bool visible = i < mHeroParam->mHeroStars;
		mUI.mTopStarImg[i]->setVisible(visible);
	}

	//升星面板信息(主角没有升星信息面板)
	if(mHeroParam->isMainHero())
	{
		return;
	}

	//英雄升星进度
	if(mHeroParam->mHeroStars >= STAR_CONT)
	{
		//mUI.mMidChipCount->setString(STR_UTF8("最高星级"));
		mUI.mMidChipCount->setString(DataManager::getInstance()->searchCommonTexdtById(10228));
		setTextAddStroke(mUI.mMidChipCount, Color3B(0x6a, 0x20, 0x11), 2);
		mUI.mMidStarsUpBar->setPercent(100.0f);
	}
	else
	{
		int curNum = ToolModel::getInstance()->searchNumByTemplateId(mHeroParam->mStarUpMat.itemId);
		mUI.mMidChipCount->setString(_TO_STR(curNum) + "/" + _TO_STR(mHeroParam->mStarUpMat.needNum));
		setTextAddStroke(mUI.mMidChipCount, Color3B(0x6a, 0x20, 0x11), 2);
		if(curNum >= mHeroParam->mStarUpMat.needNum)
		{
			mUI.mMidStarsUpBar->setPercent(100.0f);
		}
		else
		{
			mUI.mMidStarsUpBar->setPercent(float(curNum)/float(mHeroParam->mStarUpMat.needNum) * 100);
		}	
	}

	//区分英雄攻击类型
	Text* txtAtkType = dynamic_cast<Text*>(mUI.mPnlStarsAttri->getChildByName("Label_gongji"));
	ImageView* imgAtkType = dynamic_cast<ImageView*>(mUI.mPnlStarsAttri->getChildByName("Image_gongji"));
	if(mHeroParam->mAtkTyp == HERO_ATK_PHY)
	{
		//txtAtkType->setString(STR_UTF8("物攻成长"));
		txtAtkType->setString(DataManager::getInstance()->searchCommonTexdtById(10221));
		imgAtkType->loadTexture(ResourceUtils::getIconGlobalPath("img_gongji2.png"));
	}
	else
	{
		//txtAtkType->setString(STR_UTF8("法攻成长"));
		txtAtkType->setString(DataManager::getInstance()->searchCommonTexdtById(10222));
		imgAtkType->loadTexture(ResourceUtils::getIconGlobalPath("img_fagong2.png"));
	}

	if(mHeroParam->mHeroStars >= STAR_CONT)
	{
		//mUI.mHeroChipCount->setString(STR_UTF8("最高星级"));
		mUI.mHeroChipCount->setString(DataManager::getInstance()->searchCommonTexdtById(10228));
		setTextAddStroke(mUI.mHeroChipCount, Color3B(0x6a, 0x20, 0x11), 2);
		mUI.mHeroStarUpBar->setPercent(100.0f);

		RowData* advInfo = DataManager::getInstance()->searchStarUpById(mHeroParam->mTemplateId, mHeroParam->mHeroStars);

		if(advInfo != nullptr)
		{
			mUI.mAtkCurNum->setString(advInfo->getStringData("groAtk"));
			mUI.mHpCurNum->setString(advInfo->getStringData("groHp"));
			mUI.mPddCurNum->setString(advInfo->getStringData("groPdd"));
			mUI.mMddCurNum->setString(advInfo->getStringData("groMdd"));
		}

		//属性提升的箭头和后面的数字隐藏
		mUI.mAtkNextNum->setString("");
		mUI.mHpNextNum->setString("");
		mUI.mPddNextNum->setString("");
		mUI.mMddNextNum->setString("");
		mUI.mAtkArrow->setVisible(false);
		mUI.mHpArrow->setVisible(false);
		mUI.mPddArrow->setVisible(false);
		mUI.mMddArrow->setVisible(false);

		mUI.mStasrsUpTxt->setString("");
		mUI.mImgFullStars->setVisible(true);
	}
	else
	{
		//mUI.mStasrsUpTxt->setString("升星还将大幅度提升基础属性");
		mUI.mStasrsUpTxt->setString(DataManager::getInstance()->searchCommonTexdtById(10229));
		mUI.mImgFullStars->setVisible(false);

		int curNum = ToolModel::getInstance()->searchNumByTemplateId(mHeroParam->mStarUpMat.itemId);
		mUI.mHeroChipCount->setString(_TO_STR(curNum) + "/" + _TO_STR(mHeroParam->mStarUpMat.needNum));
		setTextAddStroke(mUI.mHeroChipCount, Color3B(0x6a, 0x20, 0x11), 2);
		if(curNum >= mHeroParam->mStarUpMat.needNum)
		{
			mUI.mHeroStarUpBar->setPercent(100.0f);
		}
		else
		{
			mUI.mHeroStarUpBar->setPercent(float(curNum)/float(mHeroParam->mStarUpMat.needNum) * 100);
		}	

		//属性值
		mUI.mAtkArrow->setVisible(true);
		mUI.mHpArrow->setVisible(true);
		mUI.mPddArrow->setVisible(true);
		mUI.mMddArrow->setVisible(true);

		RowData* advInfo1 = DataManager::getInstance()->searchStarUpById(mHeroParam->mTemplateId, mHeroParam->mHeroStars);
		RowData* advInfo2 = DataManager::getInstance()->searchStarUpById(mHeroParam->mTemplateId, mHeroParam->mHeroStars+1);

		if(advInfo1 != nullptr && advInfo2 != nullptr)
		{
			mUI.mAtkCurNum->setString(advInfo1->getStringData("groAtk"));
			mUI.mHpCurNum->setString(advInfo1->getStringData("groHp"));
			mUI.mPddCurNum->setString(advInfo1->getStringData("groPdd"));
			mUI.mMddCurNum->setString(advInfo1->getStringData("groMdd"));

			mUI.mAtkNextNum->setString(advInfo2->getStringData("groAtk"));
			mUI.mHpNextNum->setString(advInfo2->getStringData("groHp"));
			mUI.mPddNextNum->setString(advInfo2->getStringData("groPdd"));
			mUI.mMddNextNum->setString(advInfo2->getStringData("groMdd"));
		}
	}

	//星星
	for (int i = 0; i < STAR_CONT; i++)
	{
		bool visibleOld = i < mHeroParam->mHeroStars;
		mUI.mOldStarImg[i]->setVisible(visibleOld);

		bool visibleNew = i <= mHeroParam->mHeroStars;
		mUI.mNewStarImg[i]->setVisible(visibleNew);
	}

	//碎片图标
	UIToolHeader* AlmightyHeader = UIToolHeader::create(ID_ALMIGHTY_CHIPS);
	int curNum = ToolModel::getInstance()->searchNumByTemplateId(ID_ALMIGHTY_CHIPS);
	AlmightyHeader->setNumEx(curNum);
	AlmightyHeader->setScale(0.70f);
	AlmightyHeader->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	AlmightyHeader->setPosition(mUI.mPnlAdvChips->getContentSize()/2);
	AlmightyHeader->setTouchEnabled(true);
	AlmightyHeader->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_ALMIGHTY));
	mUI.mPnlAdvChips->removeAllChildren();
	mUI.mPnlAdvChips->addChild(AlmightyHeader);

	UIToolHeader* HeroHeader = UIToolHeader::create(mHeroParam->mStarUpMat.itemId);
	curNum = ToolModel::getInstance()->searchNumByTemplateId(mHeroParam->mStarUpMat.itemId);
	HeroHeader->setNumEx(curNum);
	HeroHeader->setScale(0.70f);
	HeroHeader->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	HeroHeader->setPosition(mUI.mPnlHeroChips->getContentSize()/2);
	HeroHeader->setTouchEnabled(true);
	HeroHeader->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnMatClicked, this, mHeroParam->mStarUpMat.itemId));
	mUI.mPnlHeroChips->removeAllChildren();
	mUI.mPnlHeroChips->addChild(HeroHeader);
}

void LayerHero::updateAlmightyPanel()
{
	//碎片图标
	UIToolHeader* AlmightyHeader = UIToolHeader::create(ID_ALMIGHTY_CHIPS);	
	int curNum = ToolModel::getInstance()->searchNumByTemplateId(ID_ALMIGHTY_CHIPS);
	AlmightyHeader->setNumEx(curNum);
	//AlmightyHeader->setScale(0.70f);
	AlmightyHeader->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	AlmightyHeader->setPosition(mUI.mPnlAlmightyIcon->getContentSize()/2);
	mUI.mPnlAlmightyIcon->removeAllChildren();
	mUI.mPnlAlmightyIcon->addChild(AlmightyHeader);

	UIToolHeader* HeroHeader = UIToolHeader::create(mHeroParam->mStarUpMat.itemId);
	curNum = ToolModel::getInstance()->searchNumByTemplateId(mHeroParam->mStarUpMat.itemId);
	HeroHeader->setNumEx(curNum);
	//HeroHeader->setScale(0.70f);
	HeroHeader->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	HeroHeader->setPosition(mUI.mPnlHeroIcon->getContentSize()/2);
	mUI.mPnlHeroIcon->removeAllChildren();
	mUI.mPnlHeroIcon->addChild(HeroHeader);

	mUI.mTextHero->setString(mHeroParam->getName() + DataManager::getInstance()->searchCommonTexdtById(20325));
	setTextAddStroke(mUI.mTextHero, Color3B(0x79, 0x03, 0x03), 2);

	if(mHeroParam->mHeroStars >= STAR_CONT)
	{
		//mUI.mTextNumProgress->setString(STR_UTF8("最高星级"));
		mUI.mTextNumProgress->setString(DataManager::getInstance()->searchCommonTexdtById(10228));
		setTextAddStroke(mUI.mTextNumProgress, Color3B(0x6a, 0x20, 0x11), 2);
		mUI.mBarNumProgress->setPercent(100.0f);
	}
	else
	{
		mUI.mTextNumProgress->setString(_TO_STR(curNum) + "/" + _TO_STR(mHeroParam->mStarUpMat.needNum));
		setTextAddStroke(mUI.mTextNumProgress, Color3B(0x6a, 0x20, 0x11), 2);
		if(curNum >= mHeroParam->mStarUpMat.needNum)
		{
			mUI.mBarNumProgress->setPercent(100.0f);
		}
		else
		{
			mUI.mBarNumProgress->setPercent(float(curNum)/float(mHeroParam->mStarUpMat.needNum) * 100);
		}	
	}
}

void LayerHero::updateChipsDetails()
{
	//碎片图标
	UIToolHeader* AlmightyHeader = UIToolHeader::create(ID_ALMIGHTY_CHIPS);
	int curNum = ToolModel::getInstance()->searchNumByTemplateId(ID_ALMIGHTY_CHIPS);
	AlmightyHeader->setNumEx(curNum);
	AlmightyHeader->setScale(0.70f);
	AlmightyHeader->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	AlmightyHeader->setPosition(mUI.mPnlAdvChips->getContentSize()/2);
	AlmightyHeader->setTouchEnabled(true);
	AlmightyHeader->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnClicked, this, HERO_BTN_ID::BTN_ALMIGHTY));
	mUI.mPnlAdvChips->removeAllChildren();
	mUI.mPnlAdvChips->addChild(AlmightyHeader);

	UIToolHeader* HeroHeader = UIToolHeader::create(mHeroParam->mStarUpMat.itemId);
	curNum = ToolModel::getInstance()->searchNumByTemplateId(mHeroParam->mStarUpMat.itemId);
	HeroHeader->setNumEx(curNum);
	HeroHeader->setScale(0.70f);
	HeroHeader->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	HeroHeader->setPosition(mUI.mPnlHeroChips->getContentSize()/2);
	HeroHeader->setTouchEnabled(true);
	HeroHeader->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnMatClicked, this, mHeroParam->mStarUpMat.itemId));
	mUI.mPnlHeroChips->removeAllChildren();
	mUI.mPnlHeroChips->addChild(HeroHeader);

	//中间和右侧面板升星进度
	if(mHeroParam->mHeroStars >= STAR_CONT)
	{
		//mUI.mHeroChipCount->setString(STR_UTF8("最高星级"));
		mUI.mHeroChipCount->setString(DataManager::getInstance()->searchCommonTexdtById(10228));
		setTextAddStroke(mUI.mHeroChipCount, Color3B(0x6a, 0x20, 0x11), 2);
		mUI.mHeroStarUpBar->setPercent(100.0f);

		//mUI.mMidChipCount->setString(STR_UTF8("最高星级"));
		mUI.mMidChipCount->setString(DataManager::getInstance()->searchCommonTexdtById(10228));
		setTextAddStroke(mUI.mMidChipCount, Color3B(0x6a, 0x20, 0x11), 2);
		mUI.mMidStarsUpBar->setPercent(100.0f);
	}
	else
	{
		int curNum = ToolModel::getInstance()->searchNumByTemplateId(mHeroParam->mStarUpMat.itemId);
		mUI.mHeroChipCount->setString(_TO_STR(curNum) + "/" + _TO_STR(mHeroParam->mStarUpMat.needNum));
		setTextAddStroke(mUI.mHeroChipCount, Color3B(0x6a, 0x20, 0x11), 2);
		if(curNum >= mHeroParam->mStarUpMat.needNum)
		{
			mUI.mHeroStarUpBar->setPercent(100.0f);
		}
		else
		{
			mUI.mHeroStarUpBar->setPercent(float(curNum)/float(mHeroParam->mStarUpMat.needNum) * 100);
		}	

		mUI.mMidChipCount->setString(_TO_STR(curNum) + "/" + _TO_STR(mHeroParam->mStarUpMat.needNum));
		setTextAddStroke(mUI.mMidChipCount, Color3B(0x6a, 0x20, 0x11), 2);
		if(curNum >= mHeroParam->mStarUpMat.needNum)
		{
			mUI.mMidStarsUpBar->setPercent(100.0f);
		}
		else
		{
			mUI.mMidStarsUpBar->setPercent(float(curNum)/float(mHeroParam->mStarUpMat.needNum) * 100);
		}		
	}

	//碎片图标
	UIToolHeader* AlmightyHeader1 = UIToolHeader::create(ID_ALMIGHTY_CHIPS);
	curNum = ToolModel::getInstance()->searchNumByTemplateId(ID_ALMIGHTY_CHIPS);
	AlmightyHeader1->setNumEx(curNum);
	//AlmightyHeader->setScale(0.70f);
	AlmightyHeader1->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	AlmightyHeader1->setPosition(mUI.mPnlAlmightyIcon->getContentSize()/2);
	mUI.mPnlAlmightyIcon->removeAllChildren();
	mUI.mPnlAlmightyIcon->addChild(AlmightyHeader1);

	UIToolHeader* HeroHeader1 = UIToolHeader::create(mHeroParam->mStarUpMat.itemId);
	curNum = ToolModel::getInstance()->searchNumByTemplateId(mHeroParam->mStarUpMat.itemId);
	HeroHeader1->setNumEx(curNum);
	//HeroHeader->setScale(0.70f);
	HeroHeader1->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	HeroHeader1->setPosition(mUI.mPnlHeroIcon->getContentSize()/2);
	mUI.mPnlHeroIcon->removeAllChildren();
	mUI.mPnlHeroIcon->addChild(HeroHeader1);

	if(mHeroParam->mHeroStars >= STAR_CONT)
	{
		mUI.mTextNumProgress->setString(DataManager::getInstance()->searchCommonTexdtById(10228));
		mUI.mTextNumProgress->setString(DataManager::getInstance()->searchCommonTexdtById(10228));
		setTextAddStroke(mUI.mTextNumProgress, Color3B(0x6a, 0x20, 0x11), 2);
		mUI.mBarNumProgress->setPercent(100.0f);
	}
	else
	{
		mUI.mTextNumProgress->setString(_TO_STR(curNum) + "/" + _TO_STR(mHeroParam->mStarUpMat.needNum));
		setTextAddStroke(mUI.mTextNumProgress, Color3B(0x6a, 0x20, 0x11), 2);
		if(curNum >= mHeroParam->mStarUpMat.needNum)
		{
			mUI.mBarNumProgress->setPercent(100.0f);
		}
		else
		{
			mUI.mBarNumProgress->setPercent(float(curNum)/float(mHeroParam->mStarUpMat.needNum) * 100);
		}	
	}
}

//滑动事件
void LayerHero::onStarsUpScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			Size size = mUI.mScrollPanel->getInnerContainerSize();
			Vec2 vec = mUI.mScrollPanel->getInnerContainer()->getPosition();

			//滚动条重设进度
			float percentY = -vec.y / (size.height - mUI.mScrollPanel->getContentSize().height);
			m_starsAttriScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);			
		}
		break;

	default:
		break;
	}
}


//增加英雄属性值变化动画
bool LayerHero::addAnimationAttri(HeroParam* heroParam)
{
	//1.装备穿戴/强化/进阶/洗练返回此界面时
	//2.英雄觉醒成功立即显示
	HERO_BASE_ATTRI_PARAM& attriParam = HeroModel::getInstance()->getBaseAttriParam();
	bool isUpdate = false;
	if (heroParam->mHp > attriParam.mHp && attriParam.mHp != 0)
	{		
		isUpdate = true;
	}
	if (heroParam->mAtk > attriParam.mAtk && attriParam.mAtk != 0)
	{
		isUpdate = true;
	}
	if (heroParam->mPDef > attriParam.mPdd && attriParam.mPdd != 0)
	{
		isUpdate = true;
	}
	if (heroParam->mMDef > attriParam.mMdd && attriParam.mMdd != 0)
	{
		isUpdate = true;
	}
	if (heroParam->mFighting > attriParam.mFightPower && attriParam.mFightPower != 0)
	{
		isUpdate = true;
	}
	if (isUpdate)
	{
		Director::getInstance()->getScheduler()->schedule(schedule_selector(LayerHero::onUpdateAnimationAttri), this, 0.05, false);
	}
	return isUpdate;
}

void LayerHero::onUpdateAnimationAttri(float ft)
{
	HERO_BASE_ATTRI_PARAM& attriParam = HeroModel::getInstance()->getBaseAttriParam();
	bool isUpdate = false;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(mCurrentHeroId);
	if (heroParam == nullptr)
	{
		HeroModel::getInstance()->clearBaseAttriParam();
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(LayerHero::onUpdateAnimationAttri), this);
		return;
	}
	if (heroParam->mHp > attriParam.mHp && attriParam.mHp != 0)
	{	
		int changeVal = heroParam->mHp - attriParam.mHp;
		int baseVal = 1;
		while (changeVal > 0)
		{
			if (changeVal%10 > 0)
			{
				attriParam.mHp += baseVal;
			}
			baseVal *= 10;
			changeVal /= 10;
		}
		mUI.mHpTextNum->setString(_TO_STR(attriParam.mHp));
		isUpdate = true;
	}
	if (heroParam->mAtk > attriParam.mAtk && attriParam.mAtk != 0)
	{
		int changeVal = heroParam->mAtk - attriParam.mAtk;
		int baseVal = 1;
		while (changeVal > 0)
		{
			if (changeVal%10 > 0)
			{
				attriParam.mAtk += baseVal;
			}
			baseVal *= 10;
			changeVal /= 10;
		}
		mUI.mAtkTextNum->setString(_TO_STR(attriParam.mAtk));
		isUpdate = true;
	}
	if (heroParam->mPDef > attriParam.mPdd && attriParam.mPdd != 0)
	{
		int changeVal = heroParam->mPDef - attriParam.mPdd;
		int baseVal = 1;
		while (changeVal > 0)
		{
			if (changeVal%10 > 0)
			{
				attriParam.mPdd += baseVal;
			}
			baseVal *= 10;
			changeVal /= 10;
		}
		mUI.mPddTextNum->setString(_TO_STR(attriParam.mPdd));
		isUpdate = true;
	}
	if (heroParam->mMDef > attriParam.mMdd && attriParam.mMdd != 0)
	{
		int changeVal = heroParam->mMDef - attriParam.mMdd;
		int baseVal = 1;
		while (changeVal > 0)
		{
			if (changeVal%10 > 0)
			{
				attriParam.mMdd += baseVal;
			}
			baseVal *= 10;
			changeVal /= 10;
		}
		mUI.mMddTextNum->setString(_TO_STR(attriParam.mMdd));
		isUpdate = true;
	}
	if (!isUpdate)
	{
		HeroModel::getInstance()->clearBaseAttriParam();
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(LayerHero::onUpdateAnimationAttri), this);
	}
}

void LayerHero::onUpdateAnimationAdvAttri(float ft)
{
	HERO_BASE_ATTRI_PARAM& attriParam = HeroModel::getInstance()->getAdvAttriParam();
	bool isUpdate = false;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(mCurrentHeroId);
	if (heroParam == nullptr)
	{
		HeroModel::getInstance()->clearAdvAttriParam();
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(LayerHero::onUpdateAnimationAdvAttri), this);
		return;
	}
	AdvancePreviewData& advPreParam = heroParam->mAdvancePreviewData;
	if (advPreParam.mHp > attriParam.mHp && attriParam.mHp != 0)
	{	
		attriParam.mHp ++;		
		mUI.mAdvanceHpTextNum->setString(_TO_STR(attriParam.mHp));
		isUpdate = true;
	}
	if (advPreParam.mAtk > attriParam.mAtk && attriParam.mAtk != 0)
	{
		attriParam.mAtk ++;		
		mUI.mAdvanceAtkTextNum->setString(_TO_STR(attriParam.mAtk));
		isUpdate = true;
	}
	if (advPreParam.mPdd > attriParam.mPdd && attriParam.mPdd != 0)
	{
		attriParam.mPdd ++;
		mUI.mAdvancePddTextNum->setString(_TO_STR(attriParam.mPdd));
		isUpdate = true;
	}
	if (advPreParam.mMdd > attriParam.mMdd && attriParam.mMdd != 0)
	{
		attriParam.mMdd ++;
		mUI.mAdvanceMddTextNum->setString(_TO_STR(attriParam.mMdd));
		isUpdate = true;
	}
	if (!isUpdate)
	{
		HeroModel::getInstance()->clearAdvAttriParam();
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(LayerHero::onUpdateAnimationAdvAttri), this);
	}
}

void LayerHero::setCurrentHeroId( int heroId )
{
	if (heroId == 0)
	{
		return;
	}
	if (mCurrentHeroId != heroId)
	{
		mCurrentHeroId = heroId;
		mHeroParam = HeroModel::getInstance()->searchHeroById(heroId);
		HeroModel::getInstance()->clearBaseAttriParam();
		HeroModel::getInstance()->clearAdvAttriParam();
		//暂时特殊处理，如果点击前是升星面板，而且当前需要切换到主角(主角没有升星面板), 则切到属性面板
		if(mHeroParam->isMainHero() && mCurPanel == STARSUP_PANEL)
		{
			mUI.mHeroPanel->setVisible(false);
			mUI.mHeroInfoPanel->setVisible(true);
			mUI.mAdvInfoPanel->setVisible(false);
			mUI.mPnlStarsUp->setVisible(false);

			mCurPanel = ATTRI_PANEL;
		}
		updateAllUI();
	}
}

//按钮响应
void LayerHero::onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switch (btnId)
		{
		case HERO_BTN_ID::BACK:
			{
				Director::getInstance()->popScene();
				//dispose();
			}
			break;
		case HERO_BTN_ID::BTN_SKILL:
			{
				mUI.mHeroPanel->setVisible(true);
				mUI.mHeroInfoPanel->setVisible(false);
				mUI.mAdvInfoPanel->setVisible(false);
				mUI.mPnlStarsUp->setVisible(false);
				mCurPanel = CARD_PANEL;
				
				if (mHeroParam && mHeroParam->isMainHero())
				{
					mUI.mMainHeroBtnPanle->getChildByName("Button_jineng")->setVisible(false);
					mUI.mMainHeroBtnPanle->getChildByName("Button_shuxing")->setVisible(true);
				}
				else
				{
					mUI.mHeroBtnPanel->getChildByName("Button_jineng")->setVisible(false);
					mUI.mHeroBtnPanel->getChildByName("Button_shuxing")->setVisible(true);
				}

				showEnterAnimation();
			}
			break;
		case HERO_BTN_ID::BTN_ADVANCE:
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_HERO_ADVANCE))
			{
				mUI.mHeroPanel->setVisible(false);
				mUI.mHeroInfoPanel->setVisible(false);
				mUI.mPnlStarsUp->setVisible(false);
				mUI.mAdvInfoPanel->setVisible(true);
				mCurPanel = ADV_PANEL;

				showEnterAnimation();

				//test
				//showAdvComfirmLayer();
			}
			break;
		case HERO_BTN_ID::BTN_TIANFU:
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TALENT))
			{
				Director::getInstance()->pushScene(TalentScene::create());				
			}
			break;
		case HERO_BTN_ID::BTN_ATTR:
			{
				mUI.mHeroInfoPanel->setVisible(true);
				mUI.mHeroPanel->setVisible(false);
				mUI.mAdvInfoPanel->setVisible(false);
				mUI.mPnlStarsUp->setVisible(false);
				mCurPanel = ATTRI_PANEL;

				if (mHeroParam && mHeroParam->isMainHero())
				{
					mUI.mMainHeroBtnPanle->getChildByName("Button_jineng")->setVisible(true);
					mUI.mMainHeroBtnPanle->getChildByName("Button_shuxing")->setVisible(false);
				}
				else
				{
					mUI.mHeroBtnPanel->getChildByName("Button_jineng")->setVisible(true);
					mUI.mHeroBtnPanel->getChildByName("Button_shuxing")->setVisible(false);
				}

				showEnterAnimation();
			}
			break;
		case HERO_BTN_ID::BTN_WEAPON:
		case HERO_BTN_ID::BTN_CLOTHES: 
		case HERO_BTN_ID::BTN_HORSE:
		case HERO_BTN_ID::BTN_HAT:
		case HERO_BTN_ID::BTN_EARRING:
		case HERO_BTN_ID::BTN_BOOK:
			{
				//隔一帧再执行点击事件，防止重复创建
				auto callBack = [this, btnId](float dt)->void
				{
					if(btnId == HERO_BTN_ID::BTN_WEAPON)
					{
						popupToolInfo(EQUIP_ITEM_TYPE_WEAPON);
					}
					else if(btnId == HERO_BTN_ID::BTN_CLOTHES)
					{
						popupToolInfo(EQUIP_ITEM_TYPE_CLOTHES);
					}
					else if(btnId == HERO_BTN_ID::BTN_HORSE)
					{
						if(HeroModel::getInstance()->getMainHeroLevel() >= m_iHorseEquipOpenLv)
						{
							popupToolInfo(EQUIP_ITEM_TYPE_HORSE);
						}
						else
						{
							string str = DataManager::getInstance()->searchCommonTexdtById(10064) + TO_STR(m_iHorseEquipOpenLv) + 
								DataManager::getInstance()->searchCommonTexdtById(10230);

							CustomTips::showUtf8(str);
						}
					}
					else if(btnId == HERO_BTN_ID::BTN_HAT)
					{
						popupToolInfo(EQUIP_ITEM_TYPE_HAT);
					}
					else if(btnId == HERO_BTN_ID::BTN_EARRING)
					{
						popupToolInfo(EQUIP_ITEM_TYPE_EARRINGS);
					}
					else
					{
						if(HeroModel::getInstance()->getMainHeroLevel() >= m_iBookEquipOpenLv)
						{
							popupToolInfo(EQUIP_ITEM_TYPE_BOOK);
						}
						else
						{
							string str = DataManager::getInstance()->searchCommonTexdtById(10064) + TO_STR(m_iBookEquipOpenLv) + 
								DataManager::getInstance()->searchCommonTexdtById(10230);
							CustomTips::showUtf8(str);
						}
					}
			
					this->getScheduler()->unschedule("LayerHero::onEquipClick", this);
				};
				getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerHero::onEquipClick");			
			}
			break;
 		case HERO_BTN_ID::BTN_START_ADV:
			startAdvance(false);
 			break;
		case HERO_BTN_ID::BTN_START_BREAK:
			startAdvance(true);
			break;
		case HERO_BTN_ID::BTN_LEVELUP:
			{
				LayerHeroLevel* layer = LayerHeroLevel::create(mHeroParam);
				this->addChild(layer);
			}
			break;
		case HERO_BTN_ID::BTN_STARUP:
			if (mCurrentHeroId == HeroModel::getInstance()->getMainHero()->mHeroId)
			{
				if(GuideManager::getInstance()->isFunctionOpen(FUNCTION_STAR_SU))
				{
					auto callBack = [this](float dt)->void
					{
						this->getScheduler()->unschedule("LayerHero::switchStarsSU", this);
						Director::getInstance()->pushScene(StarsSUScene::create(true));
					};

					getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerHero::switchStarsSU");			
				}			
			}
			else
			{
				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_HERO_ADVANCE))
				{
					mUI.mHeroPanel->setVisible(false);
					mUI.mHeroInfoPanel->setVisible(false);
					mUI.mAdvInfoPanel->setVisible(false);
					mUI.mPnlStarsUp->setVisible(true);
					mCurPanel = STARSUP_PANEL;

					showEnterAnimation();
				}
			}	
			break;
		case HERO_BTN_ID::BTN_START_STARUP:
			startStarUp();
			break;
		case HERO_BTN_ID::BTN_EXCHANGE_ONE:
		case HERO_BTN_ID::BTN_EXCHANGE_TEN:
			{
				//兑换碎片
				if(HeroModel::getInstance()->getAlmightyChipsNum() > 0)
				{
					//发送兑换协议
					HeroController::sendExchangeChipsMsg(mHeroParam->mHeroId, btnId == HERO_BTN_ID::BTN_EXCHANGE_ONE ? 1 : 10);
				}
				else
				{
					//CustomTips::show("没有可兑换的万能碎片");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10231));
				}
			}
			break;
		case HERO_BTN_ID::BTN_ALMIGHTY:
			{
				mUI.mAlmightyPanel->setVisible(true);
				ActionCreator::runCommDlgAction(mUI.mAlmightyPanel->getChildByName("Image_di"));
				updateAlmightyPanel();
			}
			break;
		case HERO_BTN_ID::BTN_CLOSE:
			mUI.mAlmightyPanel->setVisible(false);
			break;

		default:
			break;
		}
	}
}

void LayerHero::startAdvance(bool isBreak)
{
	HeroParam* heroParam = mHeroParam;
	if (heroParam)
	{
		//如果觉醒到最高级
		RowData* advInfo1 = DataManager::getInstance()->searchHeroAdvanceById(mHeroParam->mTemplateId, mHeroParam->mAdvLvl);
		RowData* advInfo2 = DataManager::getInstance()->searchHeroAdvanceById(mHeroParam->mTemplateId, mHeroParam->mAdvLvl + 1);
		//是否达到进阶上限, true为未达到上限
		bool advLimit = advInfo1 != nullptr && advInfo2 != nullptr;
		if(!advLimit)
		{
			//CustomTips::show("已觉醒到最高级");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10232));
			return;
		}

		for (AdvMatParam& advMatParam : heroParam->mVecAdvMat)
		{
			int curCount = ToolModel::getInstance()->searchNumByTemplateId(advMatParam.itemId);
			if (curCount < advMatParam.needNum)
			{
				//CustomTips::show("材料不足");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10233));
				return;
			}
		}

		int curCoin = MainModel::getInstance()->getMainParam()->mCoin;
		if (curCoin < heroParam->mAdvCoin)
		{
			LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
			Director::getInstance()->getRunningScene()->addChild(buyEnergy);
			return;
		}

		if (mHeroParam->mLevel < mHeroParam->mAdvNeedLevel)
		{
			//CustomTips::show("英雄等级不足");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10234));
			return;
		}


		//记录基础属性进阶前的数值
		HERO_BASE_ATTRI_PARAM attriParam;
		attriParam.mAtk = heroParam->mAtk;
		attriParam.mHp = heroParam->mHp;
		attriParam.mPdd = heroParam->mPDef;
		attriParam.mMdd = heroParam->mMDef;
		attriParam.mFightPower = heroParam->mFighting;
		HeroModel::getInstance()->setBaseAttriParam(attriParam);
		mPlayAniHeroId = mCurrentHeroId;
		HeroController::sendHeroAdvanceMsg(mCurrentHeroId);

		if(isBreak)
		{
			setAdvBreakBtnClickEnabled(false);				//防止过快连续点击
		}

		//记录本次觉醒材料种类数目
		mMatKindsCount = heroParam->mVecAdvMat.size();
	}				
}

void LayerHero::startStarUp()
{
	//主角的升星在星宿界面
	HeroParam* heroParam = mHeroParam;
	if (heroParam && !heroParam->isMainHero())
	{
		if(heroParam->mHeroStars >= STAR_CONT)
		{
			//CustomTips::show("已达到最高星级");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10235));
			return;
		}
		int curCount = ToolModel::getInstance()->searchNumByTemplateId(heroParam->mStarUpMat.itemId);
		if(curCount < heroParam->mStarUpMat.needNum)
		{
			//CustomTips::show("材料不足");
			LayerEquipSource* pSmithy = LayerEquipSource::create();
			this->addChild(pSmithy);
			pSmithy->showSrc(heroParam->mStarUpMat.itemId);

			return;
		}	

		//记录基础属性进阶前的数值
		HERO_BASE_ATTRI_PARAM attriParam;
		attriParam.mAtk = heroParam->mAtk;
		attriParam.mHp = heroParam->mHp;
		attriParam.mPdd = heroParam->mPDef;
		attriParam.mMdd = heroParam->mMDef;
		attriParam.mFightPower = heroParam->mFighting;
		HeroModel::getInstance()->setBaseAttriParam(attriParam);
		mPlayAniHeroId = mCurrentHeroId;
		HeroController::sendHeroStarUpMsg(mCurrentHeroId);
	}				
}

void LayerHero::popupToolInfo(EQUIP_ITEM_TYPE equipType)
{
	auto equip = getEquipBtn(equipType);
	if (equip)
	{
		equip->removeEquipEffe();
	}

	HeroParam* heroParam = mHeroParam;
	if (heroParam)
	{
		int toolId = getToolId(equipType, heroParam);
		if (toolId <= 0)
		{
			HERO_BASE_ATTRI_PARAM attriParam;
			attriParam.mAtk = heroParam->mAtk;
			attriParam.mHp = heroParam->mHp;
			attriParam.mPdd = heroParam->mPDef;
			attriParam.mMdd = heroParam->mMDef;
			HeroModel::getInstance()->setBaseAttriParam(attriParam);

			int equipId = ToolModel::getInstance()->searchPutOnEquip(equipType);
			if (equipId > 0)
			{
				//如果有可穿戴装备则弹出装备穿戴窗口
				LayerEquipPutOn* layer = LayerEquipPutOn::create(heroParam->mHeroId, equipId);
				layer->show(this);
			}
			else//没有到获取途径界面
			{
				LayerEquipSource* pSmithy = LayerEquipSource::create();
				this->addChild(pSmithy);
				pSmithy->showSrc(getNeedToolId(equipType));
			}			
		}
		else//拥有，到铁匠铺进行操作（强化、洗练、进阶）
		{
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_EQUIP_STRENGTHEN))
			{
				HERO_BASE_ATTRI_PARAM attriParam;
				attriParam.mAtk = heroParam->mAtk;
				attriParam.mHp = heroParam->mHp;
				attriParam.mPdd = heroParam->mPDef;
				attriParam.mMdd = heroParam->mMDef;
				HeroModel::getInstance()->setBaseAttriParam(attriParam);

				SceneSmithy* scene = SceneSmithy::create(heroParam->mHeroId, toolId, equipType);
				Director::getInstance()->pushScene(scene);
			}
		}	
	}
}
const int LayerHero::getToolId(EQUIP_ITEM_TYPE equipType, HeroParam* pParam)
{
	switch(equipType)
	{
	case EQUIP_ITEM_TYPE_HAT://1-帽子
		return pParam->mHeaderId;
	case EQUIP_ITEM_TYPE_CLOTHES:	//2-衣服
		return pParam->mBodyId;
	case EQUIP_ITEM_TYPE_WEAPON:	//3-武器
		return pParam->mWeaponId;
	case EQUIP_ITEM_TYPE_EARRINGS:	//4-耳环
		return pParam->mEarringId;
	case EQUIP_ITEM_TYPE_HORSE:		//5-战马
		return pParam->mHorseId;
	case EQUIP_ITEM_TYPE_BOOK:		//6-兵书
		return pParam->mBookId;
	default:
		return -1;
	}		
}

const int LayerHero::getNeedToolId(const EQUIP_ITEM_TYPE& equipType)
{
	int sysId = -1;
	switch(equipType)
	{
	case EQUIP_ITEM_TYPE_HAT://1-帽子
		sysId = 10040;
		break;
	case EQUIP_ITEM_TYPE_CLOTHES:	//2-衣服
		sysId = 10041;
		break;
	case EQUIP_ITEM_TYPE_WEAPON:	//3-武器
		sysId = 10039;
		break;
	case EQUIP_ITEM_TYPE_EARRINGS:	//4-耳环
		sysId = 10042;
		break;
	case EQUIP_ITEM_TYPE_HORSE:		//5-战马
		sysId = 10043;
		break;
	case EQUIP_ITEM_TYPE_BOOK:		//6-兵书
		sysId = 10044;
		break;
	default:
		break;
	}
	int srcToolId = 0;
	RowData* sysData = DataManager::getInstance()->searchToolSystemById(sysId);
	if (sysData != nullptr)
	{
		srcToolId = sysData->getIntData("value");
	}
	return srcToolId;
}

UIToolHeader* LayerHero::getEquipBtn(const EQUIP_ITEM_TYPE equipType)
{
	switch(equipType)
	{
	case EQUIP_ITEM_TYPE_HAT://1-帽子
		return mUI.mHat;
	case EQUIP_ITEM_TYPE_CLOTHES:	//2-衣服
		return mUI.mClothes;
	case EQUIP_ITEM_TYPE_WEAPON:	//3-武器
		return mUI.mWeapon;
	case EQUIP_ITEM_TYPE_EARRINGS:	//4-耳环
		return mUI.mEarring;
	case EQUIP_ITEM_TYPE_HORSE:		//5-战马
		return mUI.mHorse;
	case EQUIP_ITEM_TYPE_BOOK:		//6-兵书
		return mUI.mBook;
	default:
		return nullptr;
	}
	
}

void LayerHero::onLayerSmithyClose()
{
	const SMITHY_CHANGE_PARAM& changeParam = SmithyModel::getInstance()->getEquipChangeParam();
	if (changeParam.heroId == mCurrentHeroId)
	{
		HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(mCurrentHeroId);
		if (heroParam)
		{
			int toolId = getToolId((EQUIP_ITEM_TYPE)changeParam.equipType, heroParam);
			if (toolId == changeParam.toolId && toolId != -1)
			{
				UIToolHeader* pTool = getEquipBtn((EQUIP_ITEM_TYPE)changeParam.equipType);
				if (pTool != nullptr)
				{
					pTool->showEquipEffe();
				}
			}	
		}			
	}
	SmithyModel::getInstance()->clearEquipChangeParam();
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(mCurrentHeroId);
	if (heroParam != nullptr)
	{
		addAnimationAttri(heroParam);
	}
}

void LayerHero::updateFateUI()
{
	mUI.mHeroFatePanel->removeAllChildren();
	if (mHeroParam)
	{
		//mUI.mHeroAttriImg->setPosition(mUI.mheroAttrImgOriginPos);
		if(mHeroParam->isMainHero())
		{	
			mUI.mMainHeroBtnPanle->setVisible(true);
			mUI.mHeroBtnPanel->setVisible(false);
			mUI.mHeroFateTitle->loadTexture("Image/UIArray/Image/img_zhujueyuanfen.png");
			for (int i = 0; i < MAIN_HERO_FATE_COUNT; i++)
			{
				auto fateIcon = UIToolHeader::createEx("Image/Icon/global/btn_yuanfen_1_2.png", "Image/Icon/global/btn_yuanfen_1_1.png", 
					"Image/Icon/global/btn_yuanfen_2_1.png", "Image/Icon/global/btn_yuanfen_2_2.png");
				fateIcon->setTouchEnabled(true);
				fateIcon->setScale(0.8f);
				fateIcon->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
				int padding = 2;
				int iconWidth = fateIcon->getBoundingBox().size.width;
				int margin = (mUI.mHeroFatePanel->getContentSize().width 
					- MAIN_HERO_FATE_COUNT * iconWidth - (MAIN_HERO_FATE_COUNT - 1) * padding) / 2;
				int xx = margin + i * (padding + iconWidth);
				int yy = mUI.mHeroFatePanel->getContentSize().height / 2;
				fateIcon->setPosition(Point(xx, yy));
				fateIcon->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnFateClicked, this, i));
				mUI.mHeroFatePanel->addChild(fateIcon);
				//缘分图标
				if (i < mHeroParam->mVecActivedFate.size())
				{
					fateIcon->setMainIcon(ResourceUtils::getFateIconPath(mHeroParam->mVecActivedFate.at(i)));
				}
				else
				{
					fateIcon->setMainIcon("Image/Icon/global/btn_yuanfen_1_1.png");
				}
			}
		}
		else
		{
			mUI.mMainHeroBtnPanle->setVisible(false);
			mUI.mHeroBtnPanel->setVisible(true);
			mUI.mHeroFateTitle->loadTexture("Image/UIArray/Image/img_yingxiongyuanfen.png");
			//先判定英雄武器是否带天命
			int relationNum = HERO_FATE_COUNT;
			int _CL_ID = mHeroParam->mTemplateId*100 + 30;
			RowData* _rowData = DataManager::getInstance()->searchEquipmentQuenchById(_CL_ID);
			if(_rowData != nullptr)
			{
				if(_rowData->getIntData("fate") == 0)
				{
					relationNum = MAIN_HERO_FATE_COUNT;		//武器没有天命，缘分数 = 5;
				}
			}

			//缘分图标，初始化缘分数据
			HeroRelationModel::getInstance()->initFatedData(mHeroParam->mTemplateId);
			vector<sFateAttri> vHeroFate;
			HeroRelationModel::getInstance()->getRelationAttriList(vHeroFate);
			//排序，已激活的放前面，
			std::sort(vHeroFate.begin(), vHeroFate.end(), 
				[](const sFateAttri& p1, const sFateAttri& p2)->bool
			{
				if(p1.isFateActed && !p2.isFateActed)
				{
					return true;
				}
				else if(!p1.isFateActed && p2.isFateActed)
				{
					return false;
				}
				else
				{
					return p1.fateHeroId < p2.fateHeroId;
				}
			});

			for (int i = 0; i < relationNum; i++)
			{
				auto fateIcon = UIToolHeader::createEx("Image/Icon/global/btn_yuanfen_1_2.png", "Image/Icon/global/btn_yuanfen_1_1.png", 
					"Image/Icon/global/btn_yuanfen_2_1.png", "Image/Icon/global/btn_yuanfen_2_2.png");
				fateIcon->setTouchEnabled(true);
				fateIcon->setScale(0.8f);
				fateIcon->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
				int padding = 2;
				int iconWidth = fateIcon->getBoundingBox().size.width;
				int margin = (mUI.mHeroFatePanel->getContentSize().width 
					- relationNum * iconWidth - (relationNum - 1) * padding) / 2;
				int xx = margin + i * (padding + iconWidth);
				int yy = mUI.mHeroFatePanel->getContentSize().height / 2;
				fateIcon->setPosition(Point(xx, yy));
				fateIcon->addTouchEventListener(CC_CALLBACK_2(LayerHero::onBtnFateClicked, this, i));
				mUI.mHeroFatePanel->addChild(fateIcon);
				
				if (i < vHeroFate.size() && vHeroFate.at(i).isFateActed)
				{
					if(getHeroFateIconId(vHeroFate.at(i).attriEnName) != 0)
						fateIcon->setMainIcon(ResourceUtils::getFateIconPath(getHeroFateIconId(vHeroFate.at(i).attriEnName)));
				}
				else
				{
					if(getHeroFateIconId(vHeroFate.at(i).attriEnName) != 0)
					{
						int iconId = getHeroFateIconId(vHeroFate.at(i).attriEnName);
						stringstream ss;
						ss << "Image/Icon/Fate/" << iconId << "_1.png";
						fateIcon->setMainIcon(ss.str());
					}
				}
			}
		}
	}
	else
	{
		mUI.mMainHeroBtnPanle->setVisible(false);
		mUI.mHeroBtnPanel->setVisible(true);
		//mUI.mHeroAttriImg->setPosition(mUI.mheroAttrImgOriginPos + Point(0, 40));
	}

}

void LayerHero::setScrollViewClickEnabled(bool isCanClick)
{
	mUI.mHeroListScrollView->setEnabled(isCanClick);
}

void LayerHero::setAdvBreakBtnClickEnabled(bool isCanClick)
{
	mUI.mBtnStartAdv->setEnabled(isCanClick);
	mUI.mBreakBtn->setEnabled(isCanClick);
}

void LayerHero::startAnimationAdv()
{
	if(mMatKindsCount < 1 || mMatKindsCount > 3)
	{
		return;
	}

	Armature* oldAni = (Armature*)mUI.mAdvInfoPanel->getChildByName("MATERIAL_ANI_NAME");
	if(oldAni)
	{
		oldAni->removeFromParentAndCleanup(true);
	}

	auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{  
		//觉醒和突破按钮设为可点击
		mUI.mBtnStartAdv->setEnabled(true);
		mUI.mBreakBtn->setEnabled(true);
		//进度动画播放完，设置滚动列表可触摸
		setScrollViewClickEnabled(true);

		//重置觉醒材料种类数量为0
		mMatKindsCount = 0;

		armature->removeFromParent();

		//如果是突破，需要弹出突破面板
		bool isBreak = HeroModel::getInstance()->isBreak(mPlayAniHeroId);
		if(isBreak)
		{	
			showAdvComfirmLayer(QUATITY_TYPE);
		}
		else
		{
			if(!mHeroParam->isMainHero())
			{
				//检测是否需要弹缘分窗
				checkHeroFate();
			}

			auto aniCallBack1 = [this](Armature *armature1, MovementEventType movementType1, const std::string& movementID1)->void
			{
				Sequence* seq_ani = Sequence::create(FadeOut::create(1.0f), 
					CallFuncN::create([=](Node* pSender){
						setAdvBreakBtnClickEnabled(true);
						pSender->removeFromParent();
				}), nullptr);
				armature1->runAction(seq_ani);
			};
			ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anijuexingmanlezi/Anijuexingmanlezi.ExportJson");
			Armature* ani = Armature::create("Anijuexingmanlezi");
			ani->getAnimation()->play("Animation1");
			ani->getAnimation()->setMovementEventCallFunc(aniCallBack1);
			ani->setPosition(Point(mUI.mAdvInfoPanel->getContentSize() / 2) + Point(28, 180));
			ani->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
			mUI.mAdvInfoPanel->addChild(ani, 999);
		}
	};

	/*
	//如果是突破，播放动画前，屏蔽滚动列表点击事件
	bool isBreak = HeroModel::getInstance()->isBreak(mPlayAniHeroId);
	if(isBreak)
	{
		//如果是突破，屏蔽英雄列表触摸
		setScrollViewClickEnabled(false);			
	}*/
	setScrollViewClickEnabled(false);
	//屏蔽觉醒和突破按钮
	mUI.mBtnStartAdv->setEnabled(false);
	mUI.mBreakBtn->setEnabled(false);

	//根据材料数量选择动画
	string aniName = "Ani_JueXing_" + _TO_STR(mMatKindsCount);		
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anijuexingjindu/Anijuexingjindu.ExportJson");
	Armature* ani = Armature::create("Anijuexingjindu");
	ani->getAnimation()->play(aniName);
	ani->getAnimation()->setMovementEventCallFunc(aniCallBack);
	ani->setPosition(Point(mUI.mAdvInfoPanel->getContentSize() / 2) + Point(30, 35));
	ani->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	ani->setName("MATERIAL_ANI_NAME");
	mUI.mAdvInfoPanel->addChild(ani, 999);
}

void LayerHero::startPreviewNextValue()
{
	float fadeInTime = 0.8f;
	float fadeOutTime = 0.8f;
	mUI.mAdvanceAtkTextNum->runAction(Sequence::create(FadeOut::create(fadeInTime), FadeIn::create(fadeOutTime), nullptr));
	mUI.mAdvanceHpTextNum->runAction(Sequence::create(FadeOut::create(fadeInTime), FadeIn::create(fadeOutTime), nullptr));
	mUI.mAdvancePddTextNum->runAction(Sequence::create(FadeOut::create(fadeInTime), FadeIn::create(fadeOutTime), nullptr));
	mUI.mAdvanceMddTextNum->runAction(Sequence::create(FadeOut::create(fadeInTime), FadeIn::create(fadeOutTime), nullptr));
	mUI.mArrow1->runAction(Sequence::create(FadeOut::create(fadeInTime), FadeIn::create(fadeOutTime), nullptr));
	mUI.mArrow2->runAction(Sequence::create(FadeOut::create(fadeInTime), FadeIn::create(fadeOutTime), nullptr));
	mUI.mArrow3->runAction(Sequence::create(FadeOut::create(fadeInTime), FadeIn::create(fadeOutTime), nullptr));
	mUI.mArrow4->runAction(Sequence::create(FadeOut::create(fadeInTime), FadeIn::create(fadeOutTime), nullptr));
}

//增加开始觉醒动画
void LayerHero::addAnimationStartAdv(Button* advBtn)
{
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniKaishijuexing/AniKaishijuexing.ExportJson");
	Armature* pArmature = Armature::create("AniKaishijuexing");

	//设置动画精灵位置 
	pArmature->setPosition(advBtn->getContentSize().width/2, advBtn->getContentSize().height/2);
	advBtn->addChild(pArmature, 9, "AniKaishijuexing");
	//默认不播放
	pArmature->getAnimation()->stop();
	pArmature->setVisible(false);
}

void LayerHero::startAnimationStartAdv(Button* advBtn, bool play)
{
	Armature *pArmature = dynamic_cast<Armature*>(advBtn->getChildByName("AniKaishijuexing"));
	if (pArmature)
	{
		if (play)
		{
			pArmature->getAnimation()->playByIndex(0);
			pArmature->setVisible(true);
		}
		else
		{
			pArmature->getAnimation()->stop();
			pArmature->setVisible(false);
		}
	}
}

void LayerHero::setNickNamePos()
{
	ImageView* imgNickBg = (ImageView*)(mUI.mNickNameImg->getParent());
	ImageView* imgDian = (ImageView*)(imgNickBg->getChildByName("Image_dian"));
	float nickWidth = mUI.mNickNameImg->getContentSize().width + imgDian->getContentSize().width + mUI.mNameText->getContentSize().width;
	float bgWidth = imgNickBg->getContentSize().width;
	//计算称号该放的位置
	float beginDistance = (bgWidth - nickWidth)/2;
	//调整位置
	mUI.mNickNameImg->setPositionX(beginDistance + mUI.mNickNameImg->getContentSize().width/2);
	imgDian->setPositionX(mUI.mNickNameImg->getPositionX() + mUI.mNickNameImg->getContentSize().width/2);
	mUI.mNameText->setPositionX(imgDian->getPositionX() + imgDian->getContentSize().width/2);
}

int LayerHero::getHeroFateIconId( string heroAttriName )
{
	if(heroAttriName == "hp" || heroAttriName == "hpCoe")
	{
		return 10004;
	}
	else if(heroAttriName == "atk" || heroAttriName == "atkCoe")
	{
		return 10001;
	}
	else if(heroAttriName == "pdd" || heroAttriName == "pddCoe")
	{
		return 10002;
	}
	else if(heroAttriName == "mdd" || heroAttriName == "mddCoe")
	{
		return 10003;
	}

	return 0;
}

void LayerHero::checkHeroFate()
{
	HeroRelationModel::getInstance()->initFatedData(mHeroParam->mTemplateId);
	vector<sFateHeroOrder> vFatedHeroId;
	vector<sFateHeroOrder> vFateHeroLimit;
	HeroRelationModel::getInstance()->getRelationActLimit(vFateHeroLimit);
	HeroRelationModel::getInstance()->getFatedHeroId(mHeroParam->mTemplateId, vFatedHeroId);

	//先检测当前有没有被影响到的英雄
	bool isFatedHeroExit = false;
	for(int i=0; i<vFatedHeroId.size(); i++)
	{
		HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(vFatedHeroId.at(i).valueData);
		if(heroParam != nullptr)
		{
			isFatedHeroExit = true;
			break;
		}
	}
	//不存在缘分相关英雄，return
	if(!isFatedHeroExit)
	{
		return;
	}
	//存在相关缘分英雄，检测弹缘分窗

	for(int i=0; i<vFateHeroLimit.size(); i++)
	{
		if(mHeroParam->mAdvLvl == vFateHeroLimit.at(i).valueData)
		{
			this->addChild(LayerHeroRelation::create(mHeroParam->mTemplateId, true));
			return;
		}
	}
}
