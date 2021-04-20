#include "LayerHeroStarsUp.h"
#include "HeroModel.h"
#include "Utils/StringFormat.h"
#include "GuideCenter/GuideManager.h"
#include "Utils/ResourceUtils.h"
#include "GuideCenter/GuideManager.h"


//左侧英雄卡牌tag
#define	LEFT_PANEL_HEROCARD_TAG		1001

LayerHeroStarsUp::LayerHeroStarsUp()
	: mHeroId(-1)
	, mStarCount1(1)
	, mStarCount2(2)
{

}

LayerHeroStarsUp::~LayerHeroStarsUp()
{

}

LayerHeroStarsUp* LayerHeroStarsUp::create( int templetId )
{
	LayerHeroStarsUp* layer = new LayerHeroStarsUp();
	if (layer && layer->init(templetId))
	{
		layer->autorelease();
		return layer;
	}
	else
	{
		delete layer;
		return nullptr;
	}
}

bool LayerHeroStarsUp::init( int templetId )
{
	if (!ModalLayer::init())
	{
		return false;
	}

	mHeroId = templetId;
	initUI();

	return true;
}

void LayerHeroStarsUp::onEnter()
{
	ModalLayer::onEnter();

	ActionInterval* easeAct1 =  EaseSineInOut::create(FadeOut::create(1.0f));
	ActionInterval* easeAct2 =  EaseSineInOut::create(FadeIn::create(0.6f));
	ActionInterval* seq = Sequence::create(easeAct1, easeAct2, nullptr);
	mUI.mClickImg->runAction(RepeatForever::create(seq));

	GuideManager::getInstance()->startGuideNextFrame(GUIDE_INTERFACE_STAR_UP);
}

void LayerHeroStarsUp::onExit()
{
	ModalLayer::onExit();
}

void LayerHeroStarsUp::initUI()
{
	auto root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UIArray/UIArray_success.ExportJson"));
	root->setBackGroundImage("Image/Bg/img_bg_advance.png");
	root->setLocalZOrder(1);
	this->addChild(root);

	mUI.mRoot = root;

	//隐藏品质提升层
	mUI.mRoot->getChildByName("Panel_starsUp")->setVisible(false);

	Layout* parent = dynamic_cast<Layout*>(mUI.mRoot->getChildByName("Panel_starsUp"));
	parent->setVisible(true);
	mUI.mCardPanel = dynamic_cast<Layout*>(parent->getChildByName("Panel_kapai"));
	//大底
	ImageView* bigBack = dynamic_cast<ImageView*>(parent->getChildByName("Image_dadi"));
	Layout* starPanel1 = dynamic_cast<Layout*>(bigBack->getChildByName("Panel_star_1"));
	Layout* starPanel2 = dynamic_cast<Layout*>(bigBack->getChildByName("Panel_star_2"));
	for (int i = 0, j = STAR_COUNT-1; i < STAR_COUNT; i++, j--)
	{
		mUI.mStarImg1[j] = dynamic_cast<ImageView*>(starPanel1->getChildByName("Image_star_" + TO_STR(i) + "_" + TO_STR(i)));
		mUI.mStarPos1[j] = mUI.mStarImg1[j]->getPosition();
		mUI.mStarImg2[j] = dynamic_cast<ImageView*>(starPanel2->getChildByName("Image_star_" + TO_STR(i) + "_" + TO_STR(i)));
		mUI.mStarPos2[j] = mUI.mStarImg2[j]->getPosition();
	}

	mUI.mStarsUpPropertyPnl = dynamic_cast<Layout*>(bigBack->getChildByName("Panel_upStars"));
	Text* atkGrowText = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_gongji"));
	Text* hpGrowText = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_xueliang"));
	Text* pddGrowText = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_wufang"));
	Text* mddGrowText = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_fafang"));
	atkGrowText->setFontName(FONT_FZZHENGHEI);
	hpGrowText->setFontName(FONT_FZZHENGHEI);
	pddGrowText->setFontName(FONT_FZZHENGHEI);
	mddGrowText->setFontName(FONT_FZZHENGHEI);
	mUI.mSpeed1 = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_sudu_1"));
	mUI.mAtkGrow1 = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_gongji_1"));
	mUI.mHpGrow1 = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_xueliang_1"));
	mUI.mPddGrow1 = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_wufang_1"));
	mUI.mMddGrow1 = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_fafang_1"));
	mUI.mAtkGrow2 = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_gongji_2"));
	mUI.mHpGrow2 = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_xueliang_2"));
	mUI.mPddGrow2 = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_wufang_2"));
	mUI.mMddGrow2 = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_fafang_2"));
	mUI.mSpeed2 = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_sudu_2"));
	mUI.mSpeed1->setFontName(FONT_FZZHENGHEI);
	mUI.mAtkGrow1->setFontName(FONT_FZZHENGHEI);
	mUI.mHpGrow1->setFontName(FONT_FZZHENGHEI);
	mUI.mPddGrow1->setFontName(FONT_FZZHENGHEI);
	mUI.mMddGrow1->setFontName(FONT_FZZHENGHEI);
	mUI.mAtkGrow2->setFontName(FONT_FZZHENGHEI);
	mUI.mHpGrow2->setFontName(FONT_FZZHENGHEI);
	mUI.mPddGrow2->setFontName(FONT_FZZHENGHEI);
	mUI.mMddGrow2->setFontName(FONT_FZZHENGHEI);
	mUI.mSpeed2->setFontName(FONT_FZZHENGHEI);

	mUI.mAtkAdd = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_gongji_3"));
	mUI.mHpAdd = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_xueliang_3"));
	mUI.mPddAdd = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_wufang_3"));
	mUI.mMddAdd = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_fafang_3"));
	mUI.mAtkAdd->setFontName(FONT_FZZHENGHEI);
	mUI.mHpAdd->setFontName(FONT_FZZHENGHEI);
	mUI.mPddAdd->setFontName(FONT_FZZHENGHEI);
	mUI.mMddAdd->setFontName(FONT_FZZHENGHEI);
	mUI.mAtkAdd->setString("");
	mUI.mHpAdd->setString("");
	mUI.mPddAdd->setString("");
	mUI.mMddAdd->setString("");

	mUI.mFightPower1 = dynamic_cast<Text*>(Helper::seekWidgetByName(parent, "Label_zhandouli1"));
	mUI.mFightPower2 = dynamic_cast<Text*>(Helper::seekWidgetByName(parent, "Label_zhandouli2"));
	mUI.mFightPower1->setFontName(FONT_FZZHENGHEI);
	mUI.mFightPower2->setFontName(FONT_FZZHENGHEI);
	mUI.mFightPower1->setString("");
	mUI.mFightPower2->setString("");

	mUI.mClickImg = dynamic_cast<ImageView*>(parent->getChildByName("Image_dianji"));
	mUI.mArrowImg = dynamic_cast<ImageView*>(bigBack->getChildByName("Image_dajiantou"));

	setClickEmptyDispose(true);
	updateUI();
}

void LayerHeroStarsUp::updateUI()
{
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(mHeroId);
	if(heroParam == nullptr)
	{
		return;
	}
	RowData* advInfo1 = DataManager::getInstance()->searchStarUpById(heroParam->mTemplateId, heroParam->mHeroStars - 1);
	RowData* advInfo2 = DataManager::getInstance()->searchStarUpById(heroParam->mTemplateId, heroParam->mHeroStars);

	//当前觉醒等级表数据
	RowData* advInfo = DataManager::getInstance()->searchHeroAdvanceById(heroParam->mTemplateId, heroParam->mAdvLvl);
	
	if(advInfo1 == nullptr || advInfo2 == nullptr || advInfo == nullptr)
	{
		return;
	}
	//卡牌
	/*
	UICardHead* heroCard = UICardHead::create(heroParam->mTemplateId, true);
	heroCard->setAnchorPoint(Point::ANCHOR_MIDDLE);
	heroCard->setPosition(mUI.mCardPanel->getContentSize() / 2);
	heroCard->setScale(0.75f);
	heroCard->setTag(LEFT_PANEL_HEROCARD_TAG);
	mUI.mCardPanel->addChild(heroCard);
	*/
	auto card = Sprite::create(ResourceUtils::getHeroCompleteCardPath(heroParam->mResId));
	RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(heroParam->mTemplateId);
	vector<float> pointVect;
	StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
	card->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));
	card->setPosition(Point(350, 300));
	mUI.mRoot->addChild(card);

	//星级
	int stars = advInfo1->getIntData("stars");
	mStarCount1 = stars;
	mStarCount2 = heroParam->mHeroStars;
	for (int i = 0; i < STAR_CONT; i++)
	{
		bool visible = i < stars;
		mUI.mStarImg1[i]->setVisible(visible);
		visible = i < heroParam->mHeroStars;
		mUI.mStarImg2[i]->setVisible(visible);
	}

	initAnimation();

	//区分英雄攻击类型
	Text* txtAtkType = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_gongji"));
	ImageView* imgAtkType = dynamic_cast<ImageView*>(mUI.mStarsUpPropertyPnl->getChildByName("Image_gongji"));
	if(heroParam->mAtkTyp == HERO_ATK_PHY)
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

	//速度
	Text* txtSpeed = dynamic_cast<Text*>(mUI.mStarsUpPropertyPnl->getChildByName("Label_sudu"));
	txtSpeed->setFontName(FONT_FZZHENGHEI);

	mUI.mSpeed1->setString(TO_STR(advInfo1->getIntData("speed") + advInfo->getIntData("speed")));
	mUI.mAtkGrow1->setString(advInfo1->getStringData("groAtk"));
	mUI.mHpGrow1->setString(advInfo1->getStringData("groHp"));
	mUI.mPddGrow1->setString(advInfo1->getStringData("groPdd"));
	mUI.mMddGrow1->setString(advInfo1->getStringData("groMdd"));
	mUI.mSpeed2->setString(TO_STR(advInfo2->getIntData("speed") + advInfo->getIntData("speed")));
	mUI.mAtkGrow2->setString(advInfo2->getStringData("groAtk"));
	mUI.mHpGrow2->setString(advInfo2->getStringData("groHp"));
	mUI.mPddGrow2->setString(advInfo2->getStringData("groPdd"));
	mUI.mMddGrow2->setString(advInfo2->getStringData("groMdd"));

	HERO_BASE_ATTRI_PARAM oldAttriParam = HeroModel::getInstance()->getBaseAttriParam();
	/*
	mUI.mAtkAdd->setString(STR_UTF8("（攻击+" + _TO_STR(heroParam->mAtk - oldAttriParam.mAtk) + "）"));
	mUI.mHpAdd->setString(STR_UTF8("（血量+" + _TO_STR(heroParam->mHp - oldAttriParam.mHp) + "）"));
	mUI.mPddAdd->setString(STR_UTF8("（物防+" + _TO_STR(heroParam->mPDef - oldAttriParam.mPdd) + "）"));
	mUI.mMddAdd->setString(STR_UTF8("（法防+" + _TO_STR(heroParam->mMDef - oldAttriParam.mMdd) + "）"));
	*/

	mUI.mAtkAdd->setString("(" + DataManager::getInstance()->searchCommonTexdtById(10009) + "+"
		+ _TO_STR(heroParam->mAtk - oldAttriParam.mAtk) + "）");
	mUI.mHpAdd->setString("(" + DataManager::getInstance()->searchCommonTexdtById(10008) + "+"
		+ _TO_STR(heroParam->mAtk - oldAttriParam.mAtk) + "）");
	mUI.mPddAdd->setString("(" + DataManager::getInstance()->searchCommonTexdtById(10010) + "+"
		+ _TO_STR(heroParam->mAtk - oldAttriParam.mAtk) + "）");
	mUI.mMddAdd->setString("(" + DataManager::getInstance()->searchCommonTexdtById(10011) + "+"
		+ _TO_STR(heroParam->mAtk - oldAttriParam.mAtk) + "）");

	mUI.mFightPower1->setString(_TO_STR(oldAttriParam.mFightPower));
	mUI.mFightPower2->setString(_TO_STR(heroParam->mFighting));
}

void LayerHeroStarsUp::initAnimation()
{
	string aniName1 = "Anidown";
	string aniName2 = "Anicircle";

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anitupo1/Anitupo1.ExportJson");
	Armature* ani = Armature::create("Anitupo1");

	ani->getAnimation()->play(aniName1);

	Bone*  bone = ani->getBone("TIHUAN_TEXT2");
	Bone*  bone1 = ani->getBone("TIHUAN_TEXT");
	if(bone && bone1)
	{
		Skin* _newSkin = Skin::create("Image/Icon/ActTip/img_shengxinchenggong.png");
		bone->addDisplay(_newSkin,0);
		Skin* _newSkin1 = Skin::create("Image/Icon/ActTip/img_shengxinchenggong.png");
		bone1->addDisplay(_newSkin1, 0);
	}

	ani->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerHeroStarsUp::aniAniCallBack, this, aniName2));
	ani->setPosition(Vec2(this->getContentSize() / 2) + Vec2(0, 55));
	ani->setScale(0.8f);
	//this->addChild(ani);
	mUI.mRoot->addChild(ani);
}

void LayerHeroStarsUp::aniAniCallBack(Armature *armature, MovementEventType movementType, const string& movementID, const string& nextAniName)
{
	if (movementType == COMPLETE)
	{
		armature->getAnimation()->play(nextAniName);
		armature->getAnimation()->setMovementEventCallFunc(nullptr);
		
		//升星了，先播放升星的动画
		//this->startUpStarAni();
		startStarAni();
	}
}

void LayerHeroStarsUp::startUpStarAni()
{

	auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{
		armature->removeFromParent();
		startStarAni();
	};

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anitupo6/Anitupo6.ExportJson");
	Armature* ani = Armature::create("Anitupo6");
	ani->getAnimation()->play("Anitupo6");
	ani->getAnimation()->setMovementEventCallFunc(aniCallBack);

	//卡牌
	UICardHead* heroCard = (UICardHead*)mUI.mCardPanel->getChildByTag(LEFT_PANEL_HEROCARD_TAG);	
	if(heroCard)
	{
		ImageView* star = (ImageView*)heroCard->getStarImgByStarLevel(mStarCount2);
		ani->setScale(1.8f);
		ani->setPosition(Point(star->getContentSize().width/2, star->getContentSize().height/2));
		star->addChild(ani);
	}
}

void LayerHeroStarsUp::startStarAni()
{
	auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{
		armature->removeFromParent();
	};

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anitupo2/Anitupo2.ExportJson");
	Armature* ani = Armature::create("Anitupo2");
	ani->getAnimation()->play("Animation1");
	ani->getAnimation()->setMovementEventCallFunc(aniCallBack);
	ani->setPosition(mUI.mArrowImg->getContentSize() / 2);
	ani->setScale(1.8f);
	mUI.mArrowImg->addChild(ani);

	for (int i = 0; i < STAR_COUNT * 2; i++)
	{
		auto action = Sequence::create(DelayTime::create(0.03f * i), 
			Spawn::create(FadeTo::create(0.12f, 255), ScaleTo::create(0.12f, 0.7f), nullptr), nullptr);

		if (i < STAR_COUNT)
		{
			bool visible = i < mStarCount1;
			mUI.mStarImg1[i]->setVisible(visible);
			mUI.mStarImg1[i]->setScale(2.0f);
			mUI.mStarImg1[i]->runAction(action);
			mUI.mStarImg1[i]->setOpacity(0);
		}
		else
		{
			int j = i % STAR_COUNT;
			bool visible2 = j < mStarCount2;
			mUI.mStarImg2[j]->setVisible(visible2);
			mUI.mStarImg2[j]->setScale(2.0f);
			mUI.mStarImg2[j]->setOpacity(0);
			mUI.mStarImg2[j]->runAction(action);
		}
	}
}
