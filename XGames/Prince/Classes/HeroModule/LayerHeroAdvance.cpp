#include "LayerHeroAdvance.h"
#include "HeroModel.h"
#include "Utils/StringFormat.h"
#include "Widget/SkillIcon.h"
#include "GuideCenter/GuideManager.h"
#include "Utils/ResourceUtils.h"
#include "FightModule/Skill/AbstractSkill.h"
#include "LayerHero.h"
#include "HeroRelationModule/HeroRelationModel.h"
#include "Widget/LayerHeroRelation.h"

//左侧英雄卡牌tag
#define	LEFT_PANEL_HEROCARD_TAG		1001
//右侧技能提示面板中技能图标tag
#define SKILL_ICON_TAG				1002

LayerHeroAdvance::LayerHeroAdvance()
	: mHeroId(-1)
	, m_skillType(0)
	, mIsSkillUpdate(false)
{

}

LayerHeroAdvance::~LayerHeroAdvance()
{

}

LayerHeroAdvance* LayerHeroAdvance::create( int templetId )
{
	LayerHeroAdvance* layer = new LayerHeroAdvance();
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

bool LayerHeroAdvance::init( int templetId )
{
	if (!ModalLayer::init())
	{
		return false;
	}

	mHeroId = templetId;
	initUI();

	//动画结束前，本层不可点击

	return true;
}

void LayerHeroAdvance::onEnter()
{
	ModalLayer::onEnter();

	ActionInterval* easeAct1 =  EaseSineInOut::create(FadeOut::create(1.0f));
	ActionInterval* easeAct2 =  EaseSineInOut::create(FadeIn::create(0.6f));
	ActionInterval* seq = Sequence::create(easeAct1, easeAct2, nullptr);
	mUI.mClickImg->runAction(RepeatForever::create(seq));

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_BREAK_SUCCESS);
}

void LayerHeroAdvance::onExit()
{
	ModalLayer::onExit();
}

void LayerHeroAdvance::initUI()
{
	auto root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UIArray/UIArray_success.ExportJson"));
	root->setBackGroundImage("Image/Bg/img_bg_advance.png");
	root->setLocalZOrder(1);
	this->addChild(root);

	mUI.mRoot = root;

	//隐藏升星层
	mUI.mRoot->getChildByName("Panel_starsUp")->setVisible(false);

	Layout* parent = dynamic_cast<Layout*>(mUI.mRoot->getChildByName("Panel_quaUp"));
	mUI.mCardPanel = dynamic_cast<Layout*>(parent->getChildByName("Panel_kapai"));
	parent->setVisible(true);
	//大底
	ImageView* bigBack = dynamic_cast<ImageView*>(parent->getChildByName("Image_dadi"));

	mUI.mName1 = dynamic_cast<Text*>(bigBack->getChildByName("Label_mingzi1"));
	mUI.mName2 = dynamic_cast<Text*>(bigBack->getChildByName("Label_mingzi2"));
	mUI.mName1->setFontName(FONT_FZZHENGHEI);
	mUI.mName2->setFontName(FONT_FZZHENGHEI);
	mUI.mName1->setString("");
	mUI.mName2->setString("");

	mUI.mAdvPropertyPnl = dynamic_cast<Layout*>(bigBack->getChildByName("Panel_upQua"));
	Text* speed = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_sudu"));
	Text* atkText = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_gongji"));
	Text* hpText = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_xueliang"));
	Text* pddText = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_wufang"));
	Text* mddText = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_fafang"));
	speed->setFontName(FONT_FZZHENGHEI);
	atkText->setFontName(FONT_FZZHENGHEI);
	hpText->setFontName(FONT_FZZHENGHEI);
	pddText->setFontName(FONT_FZZHENGHEI);
	mddText->setFontName(FONT_FZZHENGHEI);
	mUI.mSpeed1 = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_sudu_1"));
	mUI.mAtk1 = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_gongji_1"));
	mUI.mHp1 = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_xueliang_1"));
	mUI.mPdd1 = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_wufang_1"));
	mUI.mMdd1 = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_fafang_1"));
	mUI.mAtk2 = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_gongji_2"));
	mUI.mHp2 = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_xueliang_2"));
	mUI.mPdd2 = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_wufang_2"));
	mUI.mMdd2 = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_fafang_2"));
	mUI.mSpeed2 = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_sudu_2"));
	mUI.mSpeed1->setFontName(FONT_FZZHENGHEI);
	mUI.mAtk1->setFontName(FONT_FZZHENGHEI);
	mUI.mHp1->setFontName(FONT_FZZHENGHEI);
	mUI.mPdd1->setFontName(FONT_FZZHENGHEI);
	mUI.mMdd1->setFontName(FONT_FZZHENGHEI);
	mUI.mAtk2->setFontName(FONT_FZZHENGHEI);
	mUI.mHp2->setFontName(FONT_FZZHENGHEI);
	mUI.mPdd2->setFontName(FONT_FZZHENGHEI);
	mUI.mMdd2->setFontName(FONT_FZZHENGHEI);
	mUI.mSpeed2->setFontName(FONT_FZZHENGHEI);
	mUI.mAtkText = dynamic_cast<Text*>(mUI.mAdvPropertyPnl->getChildByName("Label_gongji"));
	mUI.mAtkText->setFontName(FONT_FZZHENGHEI);
	mUI.mAtkIcon = dynamic_cast<ImageView*>(mUI.mAdvPropertyPnl->getChildByName("Image_gongji"));

	//小底
	mUI.mImgSkillPnl = dynamic_cast<ImageView*>(parent->getChildByName("Image_xiaodi"));
	mUI.mSkillStrength = dynamic_cast<ImageView*>(mUI.mImgSkillPnl->getChildByName("Image_jinengqianghua"));
	mUI.mSkillNew = dynamic_cast<ImageView*>(mUI.mImgSkillPnl->getChildByName("Image_jinengkaiqi"));
	mUI.mSkillIconPanel = dynamic_cast<Layout*>(mUI.mImgSkillPnl->getChildByName("Panel_jinengtubiao"));
	mUI.mSkillTypeImg = dynamic_cast<ImageView*>(mUI.mImgSkillPnl->getChildByName("Image_zhuangtaiji"));
	mUI.mSkillNameTxt = dynamic_cast<Text*>(mUI.mImgSkillPnl->getChildByName("Label_jineng"));
	mUI.mNoSkillTips = dynamic_cast<Text*>(mUI.mImgSkillPnl->getChildByName("Label_noskill"));
	mUI.mSkillNameTxt->setFontName(FONT_FZZHENGHEI);
	mUI.mNoSkillTips->setFontName(FONT_FZZHENGHEI);
	mUI.mNoSkillTips->setVisible(false);

	//战斗力面板
	mUI.mImgFightPower = dynamic_cast<ImageView*>(parent->getChildByName("Image_fightPower"));
	mUI.mLabOldFight = dynamic_cast<Text*>(mUI.mImgFightPower->getChildByName("Label_zhandouli1"));
	mUI.mLabNewFight = dynamic_cast<Text*>(mUI.mImgFightPower->getChildByName("Label_zhandouli2"));
	mUI.mLabOldFight->setFontName(FONT_FZZHENGHEI);
	mUI.mLabNewFight->setFontName(FONT_FZZHENGHEI);
	mUI.mLabOldFight->setString("");
	mUI.mLabNewFight->setString("");
	mUI.mImgFightPower->setVisible(false);

	Layout* richTextPnl = dynamic_cast<Layout*>(mUI.mImgSkillPnl->getChildByName("Panel_richText"));
	mUI.mSkillDesc = SimpleRichText::create();
	mUI.mSkillDesc->ignoreContentAdaptWithSize(false);
	mUI.mSkillDesc->setPosition(richTextPnl->getContentSize() / 2);
	mUI.mSkillDesc->setContentSize(richTextPnl->getContentSize());
	richTextPnl->addChild(mUI.mSkillDesc);

	mUI.mClickImg = dynamic_cast<ImageView*>(parent->getChildByName("Image_dianji"));
	mUI.mArrowImg = dynamic_cast<ImageView*>(bigBack->getChildByName("Image_dajiantou"));

	//蒙版，触发英雄觉醒界面
	auto touchCall = [this](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");

			dispose();

			setLayerCanClicked(true);
			//检查缘分
			isHeroFateChanged();	
		}
	};
	mUI.mPnlMask = dynamic_cast<Layout*>(parent->getChildByName("Pnl_mask"));
	mUI.mPnlMask->addTouchEventListener(touchCall);

	updateUI();
}

void LayerHeroAdvance::updateUI()
{
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(mHeroId);
	if(heroParam == nullptr)
	{
		CCLOG("获取英雄出错");
		return;
	}

	//英雄旧属性
	HERO_BASE_ATTRI_PARAM& oldAttrParam = HeroModel::getInstance()->getBaseAttriParam();

	RowData* advInfo1 = DataManager::getInstance()->searchHeroAdvanceById(heroParam->mTemplateId, heroParam->mAdvLvl - 1);
	RowData* advInfo2 = DataManager::getInstance()->searchHeroAdvanceById(heroParam->mTemplateId, heroParam->mAdvLvl);

	//升星等级表数据
	RowData* starsUpInfo = DataManager::getInstance()->searchStarUpById(heroParam->mTemplateId, heroParam->mHeroStars);
	
	if(advInfo1 == nullptr || advInfo2 == nullptr || starsUpInfo == nullptr)
	{
		CCLOG("读取进阶数据出错");
		return;
	}

	mUI.mName1->setString(advInfo1->getStringData("name"));
	mUI.mName2->setString(advInfo2->getStringData("name"));
	LayerHero::setNameColor(mUI.mName1, advInfo1->getIntData("qua"));
	LayerHero::setNameColor(mUI.mName2, advInfo2->getIntData("qua"));

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

	SkillUpdateParam skillUpdateParam = getSkillUpdateParam(advInfo1, advInfo2);
	if (!heroParam->isMainHero() &&
		(skillUpdateParam.updateType == SKILL_UPDATE_TYPE::SKILL_ADD ||
		skillUpdateParam.updateType == SKILL_UPDATE_TYPE::SKILL_UPDATE))
	{
		mUI.mImgSkillPnl->setVisible(true);
		mUI.mImgFightPower->setVisible(false);
		mIsSkillUpdate = true;
		mUI.mSkillNew->setVisible(skillUpdateParam.updateType == SKILL_UPDATE_TYPE::SKILL_ADD);
		mUI.mSkillStrength->setVisible(skillUpdateParam.updateType == SKILL_UPDATE_TYPE::SKILL_UPDATE);
		RowData* rowData = DataManager::getInstance()->searchSkillById(skillUpdateParam.skillId);
		mUI.mSkillNameTxt->setString(rowData->getStringData("name"));
		mUI.mSkillDesc->setString(rowData->getStringData("capTxt"), 20, Color3B::WHITE);
		SkillIcon* skillIcon = SkillIcon::create(skillUpdateParam.skillId);
		skillIcon->setTag(SKILL_ICON_TAG);
		skillIcon->setVisible(false);			//先不显示，在播放动画时显示
		skillIcon->setPosition(mUI.mSkillIconPanel->getContentSize() / 2);
		mUI.mSkillIconPanel->addChild(skillIcon);
		mUI.mNoSkillTips->setVisible(false);
		
		//记录技能类型给播放动画时调用
		m_skillType = skillUpdateParam.skillType;

		switch (skillUpdateParam.skillType)
		{
		case AbstractSkill::NORMAL_SKILL:
			mUI.mSkillTypeImg->loadTexture(ResourceUtils::getIconGlobalPath("atkID.png"));
			break;
		case AbstractSkill::ACTIVE_SKILL:
			mUI.mSkillTypeImg->loadTexture(ResourceUtils::getIconGlobalPath("a_sklID.png"));
			break;
		case AbstractSkill::RUSH_SKILL:
			mUI.mSkillTypeImg->loadTexture(ResourceUtils::getIconGlobalPath("p1_sklID.png"));
			break;
		case AbstractSkill::BUFF_SKILL:
			mUI.mSkillTypeImg->loadTexture(ResourceUtils::getIconGlobalPath("p2_sklID.png"));
			break;
		case AbstractSkill::TRIGGER_SKILL:
			mUI.mSkillTypeImg->loadTexture(ResourceUtils::getIconGlobalPath("p3_sklID.png"));
			break;
		default:
			break;
		}
	}
	else
	{
		mUI.mImgSkillPnl->setVisible(false);
		mUI.mImgFightPower->setVisible(true);
		mIsSkillUpdate = false;
		mUI.mSkillNew->setVisible(false);
		mUI.mSkillStrength->setVisible(false);
		mUI.mSkillIconPanel->setVisible(false);
		mUI.mSkillTypeImg->setVisible(false);
		mUI.mSkillNameTxt->setVisible(false);
		mUI.mSkillDesc->setVisible(false);
		mUI.mNoSkillTips->setVisible(true);

		//更新战斗力
		mUI.mLabOldFight->setString(TO_STR(oldAttrParam.mFightPower));
		mUI.mLabNewFight->setString(TO_STR(heroParam->mFighting));
	}

	//判断当前进阶等级是突破还是觉醒
	bool isBreak = HeroModel::getInstance()->isBreak(heroParam->mHeroId);
	initAnimation();
	
	mUI.mSpeed1->setString(TO_STR(advInfo1->getIntData("speed") + starsUpInfo->getIntData("speed")));
	mUI.mAtk1->setString(TO_STR(oldAttrParam.mAtk));
	mUI.mHp1->setString(TO_STR(oldAttrParam.mHp));
	mUI.mPdd1->setString(TO_STR(oldAttrParam.mPdd));
	mUI.mMdd1->setString(TO_STR(oldAttrParam.mMdd));
	mUI.mAtk2->setString(TO_STR(heroParam->mAtk));
	mUI.mHp2->setString(TO_STR(heroParam->mHp));
	mUI.mPdd2->setString(TO_STR(heroParam->mPDef));
	mUI.mMdd2->setString(TO_STR(heroParam->mMDef));
	mUI.mSpeed2->setString(TO_STR(advInfo2->getIntData("speed") + starsUpInfo->getIntData("speed")));

	if (heroParam->mAtkTyp == HERO_ATK_PHY)
	{
		//mUI.mAtkText->setString(STR_UTF8("物攻"));
		mUI.mAtkText->setString(DataManager::getInstance()->searchCommonTexdtById(10226));
		mUI.mAtkIcon->loadTexture(ResourceUtils::getIconGlobalPath("img_gongji.png"));
	}
	else
	{
		//mUI.mAtkText->setString(STR_UTF8("法攻"));
		mUI.mAtkText->setString(DataManager::getInstance()->searchCommonTexdtById(10227));
		mUI.mAtkIcon->loadTexture(ResourceUtils::getIconGlobalPath("img_fagong.png"));
	}
}

SkillUpdateParam LayerHeroAdvance::getSkillUpdateParam(RowData* advInfo1, RowData* advInfo2)
{
	SkillUpdateParam skillUpdateParam;
	
	int normalSkillId1 = advInfo1->getIntData("atkID");
	int normalSkillId2 = advInfo2->getIntData("atkID");
	int activeSkillId1 = advInfo1->getIntData("a_sklID");
	int activeSkillId2 = advInfo2->getIntData("a_sklID");
	int rushSkillId1 = advInfo1->getIntData("p1_sklID");
	int rushSkillId2 = advInfo2->getIntData("p1_sklID");
	int buffSkillId1 = advInfo1->getIntData("p2_sklID");
	int buffSkillId2 = advInfo2->getIntData("p2_sklID");
	int triggerSkillId1 = advInfo1->getIntData("p3_sklID");
	int triggerSkillId2 = advInfo2->getIntData("p3_sklID");
	
	auto skillFunc = [&skillUpdateParam](int skillId1, int skillId2)
	{
		if (skillId1 != skillId2)
		{
			RowData* rowData = DataManager::getInstance()->searchSkillById(skillId2);
			if (rowData)
			{
				skillUpdateParam.skillType = rowData->getIntData("sklTyp");
			}
			else
			{
				skillUpdateParam.skillType = -1;
			}
			skillUpdateParam.skillId = skillId2;
			skillUpdateParam.updateType = skillId1 <= 0 ? 
				SKILL_UPDATE_TYPE::SKILL_ADD : SKILL_UPDATE_TYPE::SKILL_UPDATE;
		}
	};

	skillFunc(normalSkillId1, normalSkillId2);
	skillFunc(activeSkillId1, activeSkillId2);
	skillFunc(rushSkillId1, rushSkillId2);
	skillFunc(buffSkillId1, buffSkillId2);
	skillFunc(triggerSkillId1, triggerSkillId2);

	return skillUpdateParam;
}

void LayerHeroAdvance::initAnimation()
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
		Skin* _newSkin = Skin::create("Image/Icon/ActTip/img_tupochenggong.png");
		bone->addDisplay(_newSkin,0);
		Skin* _newSkin1 = Skin::create("Image/Icon/ActTip/img_tupochenggong.png");
		bone1->addDisplay(_newSkin1, 0);
	}

	ani->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerHeroAdvance::aniAniCallBack, this, aniName2));
	ani->setPosition(Vec2(this->getContentSize() / 2) + Vec2(0, 55));
	ani->setScale(0.8f);
	mUI.mRoot->addChild(ani);
}

void LayerHeroAdvance::aniAniCallBack(Armature *armature, MovementEventType movementType, const string& movementID, const string& nextAniName)
{
	if (movementType == COMPLETE)
	{
		armature->getAnimation()->play(nextAniName);
		armature->getAnimation()->setMovementEventCallFunc(nullptr);
	
		startSkillAni();
	}
}

void LayerHeroAdvance::startSkillAni()
{
	if (mIsSkillUpdate)
	{
		/*	B哥说原特效先不要
		auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
		{
			armature->removeFromParent();
		};

		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anitupo3/Anitupo3.ExportJson");
		Armature* ani = Armature::create("Anitupo3");
		ani->getAnimation()->play("Animation1");
		ani->setPosition(this->getContentSize() / 2);
		ani->getAnimation()->setMovementEventCallFunc(aniCallBack);
		ani->setLocalZOrder(1);
		this->addChild(ani);
		*/

		SkillIcon* skillIcon = (SkillIcon*)mUI.mSkillIconPanel->getChildByTag(SKILL_ICON_TAG);
		skillIcon->setScale(4.0f);
		skillIcon->setVisible(true);

		auto callBack = CallFunc::create([&](){			
				auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
				{
					armature->removeFromParent();
					//startSkillIconAni();
					setLayerCanClicked(true);		//动画结束后，滚动列表和按钮才可以点击
					//setClickEmptyDispose(true);
				};

				ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/anitupo5/anitupo5.ExportJson");
				Armature* ani = Armature::create("anitupo5");
				ani->getAnimation()->play("Animation1");
				ani->getAnimation()->setMovementEventCallFunc(aniCallBack);
				mUI.mSkillIconPanel->getChildByTag(SKILL_ICON_TAG)->addChild(ani, 1);
		} );

		skillIcon->runAction(Sequence::create(EaseSineIn::create(ScaleTo::create(0.4f, 1.0f)), callBack, nullptr));				
	}
	else
	{
		setLayerCanClicked(true);		//动画结束后，滚动列表和按钮才可以点击
		//setClickEmptyDispose(true);
	}
}

void LayerHeroAdvance::setLayerCanClicked(bool isCanClick)
{
	((LayerHero*)this->getParent())->setScrollViewClickEnabled(isCanClick);
	((LayerHero*)this->getParent())->setAdvBreakBtnClickEnabled(isCanClick);
	mUI.mPnlMask->setVisible(isCanClick);
}

void LayerHeroAdvance::startSkillIconAni()
{
	auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{
		armature->removeFromParent();
	};

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/anitupo4/anitupo4.ExportJson");
	Armature* ani = Armature::create("anitupo4");
	ani->getAnimation()->play("Animation1");
	ani->getAnimation()->setMovementEventCallFunc(aniCallBack);
	ani->setLocalZOrder(1);

	//卡牌
	UICardHead* heroCard = (UICardHead*)mUI.mCardPanel->getChildByTag(LEFT_PANEL_HEROCARD_TAG);
	if(heroCard)
	{
		ImageView* skill = (ImageView*)heroCard->getSkillImgBySkillType(m_skillType);

		ani->setPosition(skill->getContentSize()/2);
		skill->addChild(ani);
	}
}

void LayerHeroAdvance::isHeroFateChanged()
{
	HeroParam* hero = HeroModel::getInstance()->searchHeroById(mHeroId);
	if(hero == nullptr)
	{
		return;
	}
	if(hero->isMainHero())
	{
		return;
	}
	HeroRelationModel::getInstance()->initFatedData(hero->mTemplateId);
	vector<sFateHeroOrder> vFatedHeroId;		//影响到的英雄ID
	vector<sFateHeroOrder> vFateHeroLimit;
	HeroRelationModel::getInstance()->getRelationActLimit(vFateHeroLimit);
	HeroRelationModel::getInstance()->getFatedHeroId(hero->mTemplateId, vFatedHeroId);

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
		if(hero->mAdvLvl == vFateHeroLimit.at(i).valueData)
		{
			Director::getInstance()->getRunningScene()->addChild(LayerHeroRelation::create(hero->mTemplateId, true));
			return;
		}
	}
}