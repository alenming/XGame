#include "TalentLayer.h"
#include "DataManager/DataManager.h"
#include "HeroModule/HeroModel.h"
#include "UI/UIDef.h"
#include "Communication/Command.h"
#include "GuideCenter/GuideManager.h"
#include "Utils/StringFormat.h"
#include "Widget/ActionCreator.h"
#include "Widget/LayerCommHeadInfo.h"
#include "MainModule/MainCityScene.h"

//定义一下点击层Tag值，方便取用
#define PANEL_CLICK_TAG_BEGIN	20000

TalentLayer::TalentLayer()
{
	m_pModelInstance = TalentModel::getInstance();
	mVecSkillData = m_pModelInstance->getSkillData();

	mCurMainHeroLv = HeroModel::getInstance()->getMainHero()->mLevel;
}

TalentLayer::~TalentLayer()
{

}

bool TalentLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void TalentLayer::onEnter()
{
	Layer::onEnter();

	TalentModel::getInstance()->addObserver(this);
}

void TalentLayer::onExit()
{
	Layer::onExit();

	TalentModel::getInstance()->removeObserver(this);
}

void TalentLayer::update(float dt)
{
	this->unscheduleUpdate();
	Director::getInstance()->popScene();
}

void TalentLayer::initUI()
{
	m_pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UIheroGift/UIheroGift.ExportJson"));
	m_pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_pRoot);

	//公用返回
	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");

			scheduleUpdate();
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_OPERATE_TALENT);
	this->addChild(_layerCommHeadInfo);

	initSkillDesUI();

	mBackImg = dynamic_cast<ImageView*>(m_pRoot->getChildByName("Panel_right")->getChildByName("img_di"));

	initSkillList(mBackImg, &mVecManuSklList, "CheckBox_aoyi", m_pModelInstance->getSkillTypeId(TALENT_SKILL_TYPE::TALENT_MANU_SKILL));
	initSkillList(mBackImg, &mVecNomalSklList, "CheckBox_pugong", m_pModelInstance->getSkillTypeId(TALENT_SKILL_TYPE::TALENT_NORMAL_SKILL));
	initSkillList(mBackImg, &mVecRushSklList, "CheckBox_beidong1-", m_pModelInstance->getSkillTypeId(TALENT_SKILL_TYPE::TALENT_RUSH_SKILL));
	initSkillList(mBackImg, &mVecBuffSklList, "CheckBox_beidong2-", m_pModelInstance->getSkillTypeId(TALENT_SKILL_TYPE::TALENT_BUFF_SKILL));
	initSkillList(mBackImg, &mVecTriggerSklList, "CheckBox_beidong3-", m_pModelInstance->getSkillTypeId(TALENT_SKILL_TYPE::TALENT_TRIGGER_SKILL));

	updateSkill();

	showEnterAni();
}

void TalentLayer::showEnterAni()
{
	// 面板飞入;
	// 移动距离和时间;
	float shiftPosX = 450.0f;
	float shiftTime = 0.5f;

	Layout* rightPnl = (Layout*)m_pRoot->getChildByName("Panel_right");
	Margin _oriMargin = rightPnl->getLayoutParameter()->getMargin();
	Margin _margin = _oriMargin;
	_margin.left += shiftPosX;
	rightPnl->getLayoutParameter()->setMargin(_margin);
	auto seq = Sequence::create(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(-shiftPosX, 0.0f))),
		CallFuncN::create([=](Node* pSender){
			rightPnl->getLayoutParameter()->setMargin(_oriMargin);
			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_MAIN_HERO_SKILL);
	}), nullptr);
	rightPnl->runAction(seq);

	Layout* leftPnl = (Layout*)m_pRoot->getChildByName("Panel_left");
	Margin _oriMargin1 = leftPnl->getLayoutParameter()->getMargin();
	Margin _margin1 = _oriMargin1;
	_margin1.left -= shiftPosX;
	leftPnl->getLayoutParameter()->setMargin(_margin1);
	auto seq1 = Sequence::create(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(shiftPosX, 0.0f))),
		CallFuncN::create([=](Node* pSender){
			leftPnl->getLayoutParameter()->setMargin(_oriMargin1);
	}), nullptr);
	leftPnl->runAction(seq1);
}

void TalentLayer::initSkillDesUI()
{
	ImageView* titleParent = (ImageView*)m_pRoot->getChildByName("Panel_left")->getChildByName("Image_xinxidi1");
	m_DesUI.selectedSkillIcon = (ImageView*)titleParent->getChildByName("img_jineng");
	m_DesUI.selectedSkillType = (ImageView*)titleParent->getChildByName("Img_skillType");			
	m_DesUI.selectedSkillName = (Text*)titleParent->getChildByName("Label_jinengming");	
	m_DesUI.selectedSkillName->setFontName(FONT_FZZHENGHEI);
	m_DesUI.selectedSkillName->setString("");

	ImageView* infoParent = (ImageView*)m_pRoot->getChildByName("Panel_left")->getChildByName("Image_xinxidi2");
	m_DesUI.selectedSkillDes = SimpleRichText::create();	
	if (nullptr != m_DesUI.selectedSkillDes)
	{
		m_DesUI.selectedSkillDes->ignoreContentAdaptWithSize(false);
		m_DesUI.selectedSkillDes->setContentSize(Size(280.0f, 200.0f));
		m_DesUI.selectedSkillDes->setPosition(Point(infoParent->getContentSize()/2) + Point(0, 60.0f));	
		m_DesUI.selectedSkillDes->setAnchorPoint(Point(0.5, 0.5));
		infoParent->addChild(m_DesUI.selectedSkillDes);	
	}	
	m_DesUI.selectedSkillCD = (Layout*)infoParent->getChildByName("Pnl_CD");

	Text* skillTypeTxt = (Text*)titleParent->getChildByName("Label_leiximing");
	skillTypeTxt->setFontName(FONT_FZZHENGHEI);
	Text* skillNameTxt = (Text*)titleParent->getChildByName("Label_jinengmingbiaoti");
	skillNameTxt->setFontName(FONT_FZZHENGHEI);

	Text* CDTxt = (Text*)m_DesUI.selectedSkillCD->getChildByName("Label_CD");
	CDTxt->setFontName(FONT_FZZHENGHEI);
	Text* skillCDTxt = (Text*)m_DesUI.selectedSkillCD->getChildByName("Label_jingchangCD");
	skillCDTxt->setFontName(FONT_FZZHENGHEI);
	Text* SPTxt = (Text*)m_DesUI.selectedSkillCD->getChildByName("Label_nuqi");
	SPTxt->setFontName(FONT_FZZHENGHEI);
}

void TalentLayer::initSkillList(Node* parent, vector<TalentSkillUI>* vecSkillList, string name, int skillTypeId)
{
	for (int i = 0; i < TALENT_SKILL_COUNT; i++)
	{
		TalentSkillUI talentSkillUI;
		string skillBackName = name + TO_STR(i + 1);
			
		talentSkillUI.skillBox = static_cast<CheckBox*>(parent->getChildByName(skillBackName));
		talentSkillUI.skillIcon = static_cast<ImageView*>(talentSkillUI.skillBox->getChildByName("img_jineng"));
		talentSkillUI.selectedState = static_cast<ImageView*>(talentSkillUI.skillBox->getChildByName("Image_xuanzhong"));
		talentSkillUI.selectedState->setVisible(false);
		talentSkillUI.mask = static_cast<ImageView*>(talentSkillUI.skillBox->getChildByName("img_zhezhao"));
		talentSkillUI.skillNewOpen = static_cast<ImageView*>(talentSkillUI.skillBox->getChildByName("Image_xinkaiqi"));
		talentSkillUI.skillUnlockLvl = static_cast<Text*>(talentSkillUI.skillBox->getChildByName("lab_jiesuodengji"));
		talentSkillUI.skillName = static_cast<Text*>(talentSkillUI.skillBox->getChildByName("lab_jinengming"));
		talentSkillUI.skillPnlClick = static_cast<Layout*>(talentSkillUI.skillBox->getChildByName("Pnl_click"));

		talentSkillUI.mask->setVisible(true);
		talentSkillUI.skillNewOpen->setVisible(false);
		talentSkillUI.skillUnlockLvl->setVisible(true);

		talentSkillUI.skillName->setString("");
		talentSkillUI.skillUnlockLvl->setString("");
		talentSkillUI.skillName->setFontName(FONT_FZZHENGHEI);
		talentSkillUI.skillUnlockLvl->setFontName(FONT_FZZHENGHEI);

		talentSkillUI.skillPnlClick->setTouchEnabled(true);
		talentSkillUI.skillPnlClick->setTag(PANEL_CLICK_TAG_BEGIN+skillTypeId);
		talentSkillUI.skillPnlClick->addTouchEventListener(CC_CALLBACK_2(TalentLayer::onSkillSelected, this, vecSkillList));

		for(int j=0; j<mVecSkillData.size(); j++)
		{
			if(mVecSkillData.at(j).skillTypeId == skillTypeId && mVecSkillData.at(j).skillUIindex == i)			
			{
				talentSkillUI.skillIcon->loadTexture("Image/Icon/Small/" + _TO_STR(mVecSkillData.at(j).skillIconResId) + (".png"));
				talentSkillUI.skillName->setString(mVecSkillData.at(j).skillName);
				if(mCurMainHeroLv >= mVecSkillData.at(j).unlockLvl)
				{
					talentSkillUI.mask->setVisible(false);
					talentSkillUI.skillUnlockLvl->setVisible(false);
					//新开启状态
					if(mCurMainHeroLv > m_pModelInstance->getRecordMainHeroLv() && mVecSkillData.at(j).isNeedNewOpenFlag)
					{
						talentSkillUI.skillNewOpen->setVisible(true);
						m_pModelInstance->setSkillNewOpenState(j);
					}
				}
				else
				{
					talentSkillUI.skillUnlockLvl->setVisible(true);

					ostringstream oss;
					oss << mVecSkillData.at(j).unlockLvl << DataManager::getInstance()->searchCommonTexdtById(20121);
					talentSkillUI.skillUnlockLvl->setString(oss.str());
					talentSkillUI.mask->setVisible(true);
				}
				break;
			}
		}

		vecSkillList->push_back(talentSkillUI);
	}
}

//技能选取回调
void TalentLayer::onSkillSelected(Ref* target, Widget::TouchEventType eventType, vector<TalentSkillUI>* vecSkillList)
{
	if (Widget::TouchEventType::ENDED == eventType)
	{
		SoundUtils::playSoundEffect("dianji");

		Layout* clickPnl = (Layout*)target;
		int skillTypeId = clickPnl->getTag() - PANEL_CLICK_TAG_BEGIN;

		//当前使用的技能id
		HeroParam* mainHeroParam = HeroModel::getInstance()->getMainHero();

		//先判断是否已经在使用该技能，如果正在使用则不发协议
		bool isNeedSendMsg = true;
		for (size_t i = 0; i < vecSkillList->size(); i++)
		{
			TalentSkillUI* talentSkillUI = &vecSkillList->at(i);
			if (target == talentSkillUI->skillPnlClick  && talentSkillUI->skillBox->getSelectedState() == true)
			{
				isNeedSendMsg = false;
			}
		}		

		int curUsedId = -1;
		for(int i=0; i<mVecSkillData.size(); i++)
		{
			if((vecSkillList == &mVecManuSklList && mVecSkillData.at(i).skillId == mainHeroParam->mActiveSkillId) ||
			   (vecSkillList == &mVecNomalSklList && mVecSkillData.at(i).skillId == mainHeroParam->mNormalSkillId) ||
			   (vecSkillList == &mVecRushSklList && mVecSkillData.at(i).skillId == mainHeroParam->mRushSkillId) ||
			   (vecSkillList == &mVecBuffSklList && mVecSkillData.at(i).skillId == mainHeroParam->mBuffSkillId) ||
			   (vecSkillList == &mVecTriggerSklList && mVecSkillData.at(i).skillId == mainHeroParam->mTriggerSkillId))
			{
				curUsedId = mVecSkillData.at(i).skillId;
				break;
			}
		}

		TalentSkillUI* selectItem = nullptr;
		TalentSkillData* selectData = nullptr;
		for (size_t i = 0; i < vecSkillList->size(); i++)
		{
			TalentSkillUI* talentSkillUI = &vecSkillList->at(i);
			if (target == talentSkillUI->skillPnlClick)
			{
				selectItem = talentSkillUI;
				//定位对应的数据
				for(int j=0; j<mVecSkillData.size(); j++)
				{
					if(mVecSkillData.at(j).skillTypeId == skillTypeId && mVecSkillData.at(j).skillUIindex == i)
					{
						selectData = &mVecSkillData.at(j);
						break;
					}
				}
				break;
			}
		}
		resetCheckBoxState();
		if (selectItem && selectData)
		{
			for (size_t i = 0; i < vecSkillList->size(); i++)
			{
				TalentSkillUI* talentSkillUI = &vecSkillList->at(i);
				TalentSkillData* talentSkillData = nullptr;
				for(int j=0; j<mVecSkillData.size(); j++)
				{
					if(mVecSkillData.at(j).skillTypeId == skillTypeId && mVecSkillData.at(j).skillUIindex == i)
					{
						talentSkillData = &mVecSkillData.at(j);
						break;
					}
				}
				if(mCurMainHeroLv < selectData->unlockLvl && talentSkillData->skillId == curUsedId)		//这种情况仍为使用态
				{
					talentSkillUI->skillBox->setSelectedState(true);
				}
				else
				{
					talentSkillUI->skillBox->setSelectedState(false);
				}
				if (selectItem == talentSkillUI)
				{
					selectItem->selectedState->setVisible(true);
					if(mCurMainHeroLv >= selectData->unlockLvl)
					{
						selectItem->skillBox->setSelectedState(true);
						//技能被选中，向后台通信
						if(isNeedSendMsg)
						{
							TalentController::sendReplaceSkillMsg(selectData->skillTypeId, selectData->skillId);						
						}			
					}				
				}
			}
		}
		showTips(selectData);
	}
}

void TalentLayer::showTips(TalentSkillData* selectData)
{
	m_DesUI.selectedSkillIcon->loadTexture("Image/Icon/Small/" + _TO_STR(selectData->skillIconResId) + (".png"));
	m_DesUI.selectedSkillType->loadTexture("Image/UIheroGift/image/p" + _TO_STR(selectData->skillTypeId%1000) + ("_sklID.png"));			
	m_DesUI.selectedSkillName->setString(selectData->skillName);	

	m_DesUI.selectedSkillDes->setString(selectData->skillDes, 26, Color3B::WHITE);	
	
	if(selectData->skillTypeId == 1002)
	{
		m_DesUI.selectedSkillCD->setVisible(true);

		Text* CDTxt = (Text*)m_DesUI.selectedSkillCD->getChildByName("Label_CD");
		CDTxt->setString(_TO_STR(selectData->skillCD));
		Text* skillCDTxt = (Text*)m_DesUI.selectedSkillCD->getChildByName("Label_jingchangCD");
		skillCDTxt->setString(_TO_STR(selectData->skillInCD));
		Text* SPTxt = (Text*)m_DesUI.selectedSkillCD->getChildByName("Label_nuqi");
		SPTxt->setString(_TO_STR(selectData->skillSP));
	}
	else
	{
		m_DesUI.selectedSkillCD->setVisible(false);
	}
}

void TalentLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	if (observerParam->id == nMAIN_CMD_TALENT_REPLACE)
	{
		updateSkill();
	}
}

void TalentLayer::updateSkill()
{
	HeroParam* mainHeroParam = HeroModel::getInstance()->getMainHero();
	updateSkillList(&mVecManuSklList, mainHeroParam->mActiveSkillId);
	updateSkillList(&mVecNomalSklList, mainHeroParam->mNormalSkillId);
	updateSkillList(&mVecRushSklList, mainHeroParam->mRushSkillId);
	updateSkillList(&mVecBuffSklList, mainHeroParam->mBuffSkillId);
	updateSkillList(&mVecTriggerSklList, mainHeroParam->mTriggerSkillId);

	for(int i=0; i<mVecSkillData.size(); i++)
	{
		if(mVecSkillData.at(i).skillId == mainHeroParam->mActiveSkillId)
		{
			showTips(&mVecSkillData.at(i));
			break;
		}
	}
	
}

void TalentLayer::updateSkillList(vector<TalentSkillUI>* vecSkillList, int currSklId)
{
	for (size_t i = 0; i < vecSkillList->size(); i++)
	{
		TalentSkillUI& talentSkillUI = vecSkillList->at(i);
		talentSkillUI.skillBox->setSelectedState(false);
		talentSkillUI.selectedState->setVisible(false);
	}

	for(size_t i=0; i<mVecSkillData.size(); i++)
	{
		TalentSkillData& skillSingleData = mVecSkillData.at(i);
		if(skillSingleData.skillId == currSklId)
		{
			vecSkillList->at(skillSingleData.skillUIindex).skillBox->setSelectedState(true);
			if(vecSkillList == &mVecManuSklList)
			{
				vecSkillList->at(skillSingleData.skillUIindex).selectedState->setVisible(true);
			}	
			break;
		}
	}
}

void TalentLayer::resetCheckBoxState()
{
	for(int i=0; i<mVecManuSklList.size(); i++)
	{
		mVecManuSklList.at(i).selectedState->setVisible(false);
	}
	for(int i=0; i<mVecNomalSklList.size(); i++)
	{
		mVecNomalSklList.at(i).selectedState->setVisible(false);
	}
	for(int i=0; i<mVecRushSklList.size(); i++)
	{
		mVecRushSklList.at(i).selectedState->setVisible(false);
	}
	for(int i=0; i<mVecBuffSklList.size(); i++)
	{
		mVecBuffSklList.at(i).selectedState->setVisible(false);
	}
	for(int i=0; i<mVecTriggerSklList.size(); i++)
	{
		mVecTriggerSklList.at(i).selectedState->setVisible(false);
	}
}

