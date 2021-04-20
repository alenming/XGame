#include "LayerFormation.h"
#include "HeroModule/LayerHero.h"
#include "GuideCenter/GuideManager.h"
#include "FightModule/SceneFightLoading.h"
#include "FightModule/FightController.h"
#include "TowerModule/TowerModel.h"
#include "ChapterModule/ChapterController.h"
#include "Widget/LayerCommHeadInfo.h"
#include "Utils/SoundUtils.h"
#include "TacticsModule/SceneTactics.h"
#include "TalentModule/TalentLayer.h"
#include "HeroModule/HeroModel.h"
#include "TacticsModule/ModelTactics.h"
#include "TacticsModule/TacticsController.h"
#include "ChapterModule/Activity/ActivityModel.h"
#include "TalentModule/TalentScene.h"

LayerFormation::LayerFormation(void)
	: m_nCurTouchPos(Hero_Pos_Invalid)
	, m_nCurSelPos(Hero_Pos_Invalid)
	, m_bIsDrag(false)
	, m_bIsLongPress(false)
	, m_bIsTransparent(false)
	, m_nMode(Formation_Mode::F_MODE_FORMATION)
	, m_bFighting(false)
	, m_bIsFirstEnter(true)
	, m_bIsHeroClicked(true)
	, m_bGlobalEnableClick(false)
	, m_bIsShowComboList(false)
{
	m_mapUIHeroList.clear();
	m_mapUIFormationList.clear();
	m_vcOriSkillPos.clear();
	m_vcSkillParam.clear();
	FormationModel::getInstance()->addObserver(this);
	ModelTactics::getInstance()->addObserver(this);
	HeroModel::getInstance()->addObserver(this);
}


LayerFormation::~LayerFormation(void)
{
	FormationModel::getInstance()->removeObserver(this);
	ModelTactics::getInstance()->removeObserver(this);
	FormationController::getInstance()->releaseInstance();
	HeroModel::getInstance()->removeObserver(this);
}

LayerFormation* LayerFormation::create(Formation_Mode mode/* = F_MODE_FORMATION*/)
{
	LayerFormation* _layer = new LayerFormation;
	if (nullptr != _layer && _layer->init(/*type, */mode))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

bool LayerFormation::init(Formation_Mode mode/* = F_MODE_FORMATION*/)
{
	if (!Layer::init())
	{
		return false;
	}

	m_nMode = mode;
	FormationModel::getInstance()->setFormationMode(mode);
	initUI();

	// 请求初始数据;
	m_pController = FormationController::getInstance();
	m_pController->handleFormation();
	return true;
}


void LayerFormation::onEnter()
{
	Layer::onEnter();

	if (m_bIsFirstEnter)
	{
		m_bIsFirstEnter = false;

		// 进场特效;
		playEnterAnimation();

		// 音效;
		SoundUtils::playSoundEffect("quanping");

		// 音乐;
		if (m_nMode == F_MODE_FORMATION)
		{
			SoundUtils::playSoundMusic("main_2");
		}
	}
	else
	{
		//从阵法返回阵形不会播放动画，所以此处需单独调用
		auto callBack = [this](float dt)->void
		{
			switch (m_nMode)
			{
			case F_MODE_FORMATION:
				GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_FORMATION);
				break;
			case F_MODE_TRAIL:
				GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_TRAIL_FORMATION);
				break;
			case F_MODE_BARRIER:
				GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_BARRIER_FORMATION);
				break;
			default:
				break;
			}
			this->getScheduler()->unschedule("LayerBarrier::onEnter", this);
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerBarrier::onEnter");

		// 更新阵法数据;
		updateTacticsData();
	}
}


void LayerFormation::onExit()
{
	// 释放资源;
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIFormation");

	// 继续播放音乐;
	if (m_nMode == F_MODE_FORMATION)
	{
		SoundUtils::playSoundMusic("main_1");
	}

	Layer::onExit();
}

void LayerFormation::initUI()
{
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile(STR_FORMATION_JSON_PATH));

	// Bg;
	ImageView* imgBg = ImageView::create("Image/Bg/img_bg_quanping.jpg");
	imgBg->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	imgBg->setPosition(Vec2(0.0f, 0.0f));
	m_ui.pRoot->addChild(imgBg, 1);

	// 通用标题栏;
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(
		[this](Ref* sender, Widget::TouchEventType type)->void
	{
		this->onBtnClicked(sender, type, Btn_Back);
	},
		HEAD_INFO_TYPE_ZHENZXING);
	m_ui.pRoot->addChild(_layerCommHeadInfo, 9);

	// PnlFormation;
	m_ui.pnlFormation = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName(STRING(Pnl_Formation)));
	m_ui.pnlFormation->setContentSize(visibleSize);

	// tips;
	if (m_nMode == F_MODE_TRAIL)
	{
		ImageView* imgBattle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Battle"));
		imgBattle->setVisible(false);
		Text* txtTipssss = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Num_Tips"));
		txtTipssss->setFontName(FONT_FZZHENGHEI);
		txtTipssss->setVisible(true);
	}

	// Skill;
	ostringstream oss;
	for (int i = 0; i < FORMATION_MAX_SKILL_TYPE; i++)
	{
		oss << "Btn_Skill" << i+1;
		m_ui.btnSkill[i].btnSkill = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		m_ui.btnSkill[i].btnSkill->addTouchEventListener(CC_CALLBACK_2(LayerFormation::onBtnClicked, this, Btn_Skill1+i));
		m_ui.btnSkill[i].btnSkill->setEnabled(false);
		m_ui.btnSkill[i].btnSkill->setBright(false);
		oss.str("");
	}

	// 技能tips;
	m_ui.pnlSkill = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Skill"));
	
	// 编辑天赋;
	m_ui.btnEditSkill = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Edit_Skill)));
	m_ui.btnEditSkill->addTouchEventListener(CC_CALLBACK_2(LayerFormation::onBtnClicked, this, Btn_Edit_Skill));

	// 阵型;
	m_ui.pnlField = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Field"));
	m_ui.txtFc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Fc"));
	m_ui.txtFc->setFontName(FONT_NAME_FOUNDER_BOLD);
	m_ui.txtFc->setColor(Color3B(0xe8, 0xde, 0xb5));
	m_ui.txtFc->setString(STR_UTF8(""));
	for (unsigned int j = 0; j < FORMATION_MAX_POS_COUNT; j++)
	{
		oss.str("");
		oss << "Img_Pos_" << j+1;
		m_ui.imgPos[j] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		m_ui.imgPos[j]->setTag(100+j);

		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [=](Touch* touch, Event* event)
		{
			if (alphaTouchCheck(dynamic_cast<ImageView*>(event->getCurrentTarget()), touch))
			{
				return true;
			}
			else
			{
				return false;
			}
		};
		listener->onTouchMoved = [=](Touch* touch, Event* event) {};
		listener->onTouchEnded = [=](Touch* touch, Event* event)
		{
			// 空阵位才响应;
			HERO_POS nPos = (HERO_POS)(event->getCurrentTarget()->getTag()-100);
			if (FormationModel::getInstance()->getHeroAtPos(nPos) == FORMATION_EMPTY_POS)
			{
				m_pController->handleSelectEmptyPos(nPos);
			}
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, m_ui.imgPos[j]);

		m_posSet[j] = Vec2(m_ui.imgPos[j]->getPosition().x - 5.0f, m_ui.imgPos[j]->getPosition().y - 10.0f);
		oss.str("");

		oss << "Img_Pos_Index_" << j+1;
		m_ui.imgPosIndex[j] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		oss.str("");
	}

	// 阵法等级;
	m_ui.txtFrontLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Front_Level"));
	m_ui.txtFrontLevel->setFontName(FONT_FZZHENGHEI);
	m_ui.txtFrontLevel->setString("0" + DataManager::getInstance()->searchCommonTexdtById(20101));
	setTextAddStroke(m_ui.txtFrontLevel, Color3B(0x80, 0x1A, 0x1A), 2);
	m_ui.txtMiddleLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Middle_Level"));
	m_ui.txtMiddleLevel->setFontName(FONT_FZZHENGHEI);
	m_ui.txtMiddleLevel->setString("0" + DataManager::getInstance()->searchCommonTexdtById(20101));
	setTextAddStroke(m_ui.txtMiddleLevel, Color3B(0x80, 0x1A, 0x1A), 2);
	m_ui.txtBackLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Back_Level"));
	m_ui.txtBackLevel->setFontName(FONT_FZZHENGHEI);
	m_ui.txtBackLevel->setString("0" + DataManager::getInstance()->searchCommonTexdtById(20101));
	setTextAddStroke(m_ui.txtBackLevel, Color3B(0x80, 0x1A, 0x1A), 2);

	// Hero列表;
	m_ui.imgHeroListBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Img_Hero_List_Bg)));
	m_ui.listHero = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(ScrollView_HeroList)));
	m_ui.listHero->setDirection(ScrollView::Direction::VERTICAL);
	m_ui.listHero->setTouchEnabled(true);
	m_ui.listHero->addEventListener(CC_CALLBACK_2(LayerFormation::onScrollViewEvent, this));
	m_ui.listHeroBar = CustomScrollBar::create();
	m_ui.imgHeroListBg->addChild(m_ui.listHeroBar);
	m_ui.listHeroBar->setPosition(Vec2(30.0f, 30.0f));
	m_ui.listHeroBar->initScrollBar(m_ui.listHero->getContentSize().height);
	m_ui.imgHeroCountTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Battle"));
	m_ui.txtHeroCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Hero_Count"));
	m_ui.txtHeroCount->setString("0/6");
	m_ui.txtHeroCount->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtHeroCount, Color3B(0x80, 0x1A, 0x1A), 2);

	// 阵法;
	m_ui.btnZhenfa = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Zhenfa)));
	m_ui.btnZhenfa->addTouchEventListener(CC_CALLBACK_2(LayerFormation::onBtnClicked, this, Btn_Zhenfa));
	m_ui.txtAddition = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Addition"));
	m_ui.txtAddition->setFontName(FONT_FZZHENGHEI);
	m_ui.txtAddition->setString("");

	// 最大追击;
	m_ui.btnShowCombo = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Combo)));
	m_ui.btnShowCombo->addTouchEventListener(CC_CALLBACK_2(LayerFormation::onBtnClicked, this, Btn_Combo));
	m_ui.btnCloseCombo = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Close_Combo)));
	m_ui.btnCloseCombo->addTouchEventListener(CC_CALLBACK_2(LayerFormation::onBtnClicked, this, Btn_Close_Combo));

	// 最大追击展示;
	m_ui.imgRushBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Rush_Bg"));
	m_ui.imgRushEmpty = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Rush_Empty"));
	m_ui.imgRushFirstHeroSkill_Manual = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_RushSkill_Manual"));
	m_ui.imgRushFirstHeroSkill_Atk = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_RushSkill_Atk"));
	Text* txtZaocheng = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_zaocheng"));
	txtZaocheng->setFontName(FONT_FZZHENGHEI);
	for (unsigned int i = 0; i < FORMATION_MAX_COMBO; ++i)
	{
		oss.str("");
		oss << "Img_Rush_" << i+1;
		m_ui.imgRush[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		m_ui.imgRush[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

		if (i == 0)
		{
			m_ui.imgRushX[i] = nullptr;
			m_ui.imgRushSkillPre[i] = nullptr;
			m_ui.imgRushSkillDst[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_RushSkill_1_Dst"));
		}
		else
		{
			oss.str("");
			oss << "Img_Rush_" << i+1 << "_X";
			m_ui.imgRushX[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
			oss.str("");
			oss << "Img_RushSkill_" << i+1 << "_Pre";
			m_ui.imgRushSkillPre[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
			oss.str("");
			oss << "Img_RushSkill_" << i+1 << "_Dst";
			m_ui.imgRushSkillDst[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
			oss.str("");
			oss << "Img_Arrow_" << i+1;
			m_ui.imgRushArrow[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		}
	}

	// 活动关卡英雄比对列表;
	if (m_nMode == F_MODE_ACTIVITY)
	{
		// 取到阵型数据和匹配名单;
		vector<pair<int, string> >  vcActivityHeroList;
		ActivityModel::getInstance()->getActivityHeroList(vcActivityHeroList);
		if (vcActivityHeroList.size() > 0)
		{
			m_ui.pnlActivityHeroList = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Activity_Hero_list"));
			for (int k = 0; k < FORMATION_MAX_HERO_COUNT; ++k)
			{
				oss.str("");
				oss << "Lab_Name_" << k+1;
				m_ui.txtActivityHeroes[k] = dynamic_cast<Text*>(m_ui.pnlActivityHeroList->getChildByName(oss.str()));
				m_ui.txtActivityHeroes[k]->setFontName(FONT_FZZHENGHEI);
			}
			refreshActivityHeroList();
		}
	}

	// 战斗按钮;
	if (m_nMode == F_MODE_TRAIL || m_nMode == F_MODE_BARRIER || m_nMode == F_MODE_TOWER || m_nMode == F_MODE_ACTIVITY)
	{
		Button* btnFight = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Challenge"));

		// 3. 动画形式的战斗按钮;
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniBtnFight/fight.ExportJson");
		Armature* armature = Armature::create("fight");
		armature->setPosition(btnFight->getPosition());
		btnFight->getParent()->addChild(armature);
		auto listener = EventListenerTouchOneByOne::create();
		listener->onTouchBegan = [=] (Touch* touch, Event* event)
		{
			// 动画播放结束前不允许点击;
			if (!m_bGlobalEnableClick)
				return false;

			Point locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
			Size size = event->getCurrentTarget()->getContentSize();
			Rect rect = Rect(-size.width/2, -size.height/2, size.width, size.height);
			if (rect.containsPoint(locationInNode))
			{
				return true;
			}
			return false;
		};
		listener->onTouchEnded = [=] (Touch* touch, Event* event)
		{
			Point locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
			Size size = event->getCurrentTarget()->getContentSize();
			Rect rect = Rect(-size.width/2, -size.height/2, size.width, size.height);
			if (rect.containsPoint(locationInNode))
			{
				onBtnClicked( nullptr, Widget::TouchEventType::ENDED, Btn_Fight );
			}
		};
		armature->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, armature);
		armature->getAnimation()->play("Animation1", -1, 1);
	}

	this->addChild(m_ui.pRoot);
}


void LayerFormation::updateSelf(void* pData)
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_FORMATION:
		{
			updateFormation((UI_FORMATION_PARAM*)(param->updateData));
			//GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_FORMATION);
		}
		break;

	case OBS_PARAM_TYPE_HERO_ON_FIELD:
		{
			UI_FORMATION_PARAM* formationParam = (UI_FORMATION_PARAM*)(param->updateData);
			for (unsigned int i = 0; i < ((UI_FORMATION_PARAM*)(param->updateData))->vcHeroData.size(); ++i)
			{
				if (formationParam->vcHeroData.at(i).nHeroId != -1)
				{
					// 上阵;
					procHeroOnField(formationParam->vcHeroData.at(i));

					// 更新战斗力;
					ostringstream oss;
					oss << formationParam->nFc;
					m_ui.txtFc->setString(oss.str());
					oss.str("");

					// 更新上阵英雄数量;
					oss << FormationModel::getInstance()->getHeroOnFieldCount() << "/" << FORMATION_MAX_HERO_COUNT;
					m_ui.txtHeroCount->setString(oss.str());
					setTextAddStroke(m_ui.txtHeroCount, Color3B(0x80, 0x1A, 0x1A), 2);
					oss.str("");

					break;
				}
			}
		}
		break;

	case OBS_PARAM_TYPE_HERO_OFF_FIELD:
		{
			UI_FORMATION_PARAM* formationParam = (UI_FORMATION_PARAM*)(param->updateData);
			for (unsigned int i = 0; i < formationParam->vcHeroData.size(); ++i)
			{
				if (formationParam->vcHeroData.at(i).nHeroId != -1)
				{
					// 下阵;
					procHeroOffField(formationParam->vcHeroData.at(i));

					// 更新战斗力;
					ostringstream oss;
					oss << formationParam->nFc;
					m_ui.txtFc->setString(oss.str());
					oss.str("");

					// 更新上阵英雄数量;
					oss << FormationModel::getInstance()->getHeroOnFieldCount() << "/" << FORMATION_MAX_HERO_COUNT;
					m_ui.txtHeroCount->setString(oss.str());
					setTextAddStroke(m_ui.txtHeroCount, Color3B(0x80, 0x1A, 0x1A), 2);
					oss.str("");

					break;
				}
			}
		}
		break;

	case OBS_PARAM_TYPE_HERO_SWAP:
		{
			procHeroSwap(*((UI_SWAP_FORMATION_PARAM*)(param->updateData)));
		}
		break;

	case OBS_PARAM_TYPE_HERO_LIST:
		{
			updateHeroList(*((vector<UI_HERO_LIST_PARAM>*)(param->updateData)));
		}
		break;

	case OBS_PARAM_TYPE_SKILL_INFO:
		{
			updateSkillInfo(*((vector<FORMATION_SKILL_PARAM>*)(param->updateData)));
		}
		break;

	case OBS_PARAM_TYPE_SELECTED:
		{
			refreshCurSelected(*((vector<int>*)(param->updateData)));
		}
		break;

	case OBS_PARAM_TYPE_COMBO:
		{
			//updateCombo((UI_COMBO_PARAM*)(param.updateData));
			updateCombo((Combo_Skill_Param*)(param->updateData));
		}
		break;

	case OBS_PARAM_TYPE_FRIENDS:
		break;

	case OBS_PARAM_TYPE_UPLOAD:
		{
			bool bSucceed = *((bool*)(param->updateData));
			if (bSucceed)
			{
				if (m_bFighting)
				{
					if (m_nMode == F_MODE_TRAIL)
					{
						// 进入战斗;
						Director::getInstance()->popScene();
						Director::getInstance()->replaceScene(SceneFightLoading::create(FIGHT_TYPE_TRAIL, TowerModel::getInstance()->getTrailCurFloor()));
						FightController::getInstance()->sendTrailFightMsg(TowerModel::getInstance()->getSelectedOpLv());
					}
				}
				else
				{
					SceneFormation* _parent = dynamic_cast<SceneFormation*>(this->getParent());
					assert(_parent != nullptr);
					_parent->onBtnClose();
				}
			}
			else
			{
				// pop 
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10098), CC_CALLBACK_2(LayerFormation::popCallbackFunc, this));
			}
		}
		break;

	case nMAIN_CMD_TACTICS_MAIN_LIST:
	case nMAIN_CMD_TACTICS_USE_TACTICS:
	case nMAIN_CMD_TACTICS_ROW_UPGRADE:
	case nMAIN_CMD_TACTICS_UPGRADE:
		{
			updateTacticsData();
		}
		break;

	case nMAIN_CMD_TALENT_REPLACE:
		{
			// 更新技能;
			FormationModel::getInstance()->updateSkillInfo(FormationModel::getInstance()->getCurSelectedHero());

			// 更新英雄追打技能;
			FormationModel::getInstance()->updateCombo();

			// 更新UI追打展示;
			updateHeroCombo(FormationModel::getInstance()->getCurSelectedHero());

			// 更新战斗力;
			ostringstream oss;
			oss << FormationModel::getInstance()->getFcTeam();
			m_ui.txtFc->setString(STR_UTF8(oss.str()));
			oss.str("");
		}
		break;

	default:
		break;
	}
}


void LayerFormation::onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName)
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		switch (nWidgetName)
		{
		case Btn_Back:
		case Btn_Combo:
		case Btn_Close_Combo:
		case Btn_Edit_Skill:
		case Btn_Zhenfa:
			SoundUtils::playSoundEffect("dianji");
			break;

		case Btn_Skill1:
		case Btn_Skill2:
		case Btn_Skill3:
		case Btn_Skill4:
		case Btn_Skill5:
			{
				// tips;
				int nIndex = nWidgetName - Btn_Skill1;
				FORMATION_SKILL_PARAM& skillParam = m_vcSkillParam.at(nIndex);
				if (!m_ui.pnlSkill->getChildByName("tipSkill"))
				{
					m_ui.pnlSkill->removeChildByName("tipSkill");
				}
				SkillTips* tipSkill = SkillTips::create();
				m_ui.pnlSkill->addChild(tipSkill, 0, "tipSkill");

				tipSkill->setSkill(skillParam.nSkillId, (nIndex > 2) ? 0 : 1);
				tipSkill->setPosition(Vec2(m_ui.btnSkill[nIndex].btnSkill->getPosition().x + (nIndex > 2 ? -1 : 1)*130.0f,
					m_ui.btnSkill[nIndex].btnSkill->getPosition().y + 120.0f));
			}
			break;

		default:
			break;
		}
	}
	else if(Widget::TouchEventType::ENDED == type)
	{
		switch (nWidgetName)
		{
		case Btn_Back:
			{
				if (FormationModel::getInstance()->getFormationMode() == F_MODE_TRAIL)
				{
					SceneFormation* _parent = dynamic_cast<SceneFormation*>(this->getParent());
					assert(_parent != nullptr);
					_parent->onBtnClose();
				}
				else
				{
					// 上传Hero阵型;
					m_bFighting = false;
					int nRet = m_pController->handleFormationLocal();
				}
			}
			break;

		case Btn_Combo:
		case Btn_Close_Combo:
			{
				m_ui.btnShowCombo->setVisible(nWidgetName == Btn_Close_Combo);
				m_ui.btnCloseCombo->setVisible(nWidgetName == Btn_Combo);
				m_ui.imgRushBg->setVisible(nWidgetName == Btn_Combo);
				m_ui.imgHeroListBg->setVisible(nWidgetName == Btn_Close_Combo);
				m_bIsShowComboList = (nWidgetName == Btn_Combo);
			}
			break;

		case Btn_Skill1:
		case Btn_Skill2:
		case Btn_Skill3:
		case Btn_Skill4:
		case Btn_Skill5:
			{
				// tips;
				if (m_ui.pnlSkill->getChildByName("tipSkill"))
				{
					m_ui.pnlSkill->removeChildByName("tipSkill");
				}
			}
			break;

		case Btn_Fight:
			{
				// 音效;
				SoundUtils::playSoundEffect("zhandou");

				// 
				if (m_nMode == F_MODE_TRAIL)
				{
					int nRet = FormationController::getInstance()->checkTrailFormation();
					if (0 != nRet)
					{
						CustomTips::showUtf8(getFormationErrorMsg(nRet));
					}
					else
					{
						// 上传试炼阵型;
						m_bFighting = true;
						FormationController::getInstance()->handleFormationLocal();
					}
				}
				else if (m_nMode == F_MODE_BARRIER)
				{
					// 直接进入战斗;
					int nBarrierId = ChapterController::getInstance()->getCurrSelectedBarrier();
					Director::getInstance()->popScene();
					Director::getInstance()->replaceScene(SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_PVE, nBarrierId));
					FightController::getInstance()->sendPVEFightMsg(nBarrierId);
				}
				else if (m_nMode == F_MODE_ACTIVITY)
				{
					int nBarrierId = ActivityModel::getInstance()->getCurActivityBarrierId();
					Director::getInstance()->popScene();
					Director::getInstance()->replaceScene(SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_PVE_ACTIVITY, nBarrierId));
					FightController::getInstance()->sendActivityFightMsg(nBarrierId);
				}
				else if (m_nMode == F_MODE_TOWER)
				{
					Director::getInstance()->popScene();
					Director::getInstance()->replaceScene(SceneFightLoading::create(FIGHT_TYPE_TOWER, TowerModel::getInstance()->getTowerCurBarrierId()));
					FightController::getInstance()->sendTowerFightMsg(TowerModel::getInstance()->getTowerCurBarrierId());
				}
				else
				{
					CustomTips::show("error fight");
				}
			}
			break;

		case Btn_Zhenfa:
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TACTICS))
			{
				Director::getInstance()->pushScene(SceneTactics::create(true));
			}
			break;

		case Btn_Edit_Skill:
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TALENT))
			{
				Director::getInstance()->pushScene(TalentScene::create());	
			}
			break;

		default:
			break;
		}
	}
	else if(Widget::TouchEventType::CANCELED == type)
	{
		// tips;
		if (m_ui.pnlSkill->getChildByName("tipSkill"))
		{
			m_ui.pnlSkill->removeChildByName("tipSkill");
		}
	}
}


void LayerFormation::onHeroOnFieldClicked(Ref* ref, Widget::TouchEventType type, int nPos)
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		// 选中阵型中的一个Position;
		if (!m_bIsDrag && !m_bIsLongPress)
		{
			int nRet = m_pController->selectHeroByPos((HERO_POS)nPos);
			if (nRet != 0)
			{
				CustomTips::showUtf8(getFormationErrorMsg(nRet));
			}
		}
	}
}


void LayerFormation::onHeroListClicked(Ref* ref, Widget::TouchEventType type, int nHeroId)
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		// 选中Hero列表中的一只Hero;
		int nRet = m_pController->selectHero(nHeroId);
		if (nRet != 0)
		{
			CustomTips::showUtf8(getFormationErrorMsg(nRet));
		}
	}
}


void LayerFormation::onPosClicked( Ref* ref, Widget::TouchEventType type, HERO_POS nPos )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		
	}
}


void LayerFormation::updateHeroList( vector<UI_HERO_LIST_PARAM> pParam )
{
	// 首先确定容器尺寸;
	int nRow = (pParam.size()/3 + ((pParam.size() % 3) == 0 ? 0 : 1));
	float scrollViewHeight = ((nRow * (HERO_ICON_HEIGHT + HERO_LIST_SPACE) + 10.0f) > m_ui.listHero->getContentSize().height)
		? (nRow * (HERO_ICON_HEIGHT + HERO_LIST_SPACE) + 10.0f) : m_ui.listHero->getContentSize().height;
	Size oriInnerSize = m_ui.listHero->getInnerContainerSize();
	if (scrollViewHeight > oriInnerSize.height)
	{
		m_ui.listHero->setInnerContainerSize(Size(m_ui.listHero->getContentSize().width, scrollViewHeight));
	}

	// 添加Hero;
	for (unsigned int i = 0; i < pParam.size(); i++)
	{
		// 查找现有的;
		auto iter = m_mapUIHeroList.find(pParam.at(i).nHeroId);
		if (iter != m_mapUIHeroList.end())
		{
			// 重新定位;
			Size innerSize = m_ui.listHero->getInnerContainerSize();
			iter->second->setPosition(Vec2(innerSize.width / 6.0f + HERO_LIST_SPACE + (i % 3) * (innerSize.width/3.0f),
				innerSize.height - iter->second->getContentSize().height/2.0f - HERO_LIST_SPACE - (i/3) * (HERO_ICON_HEIGHT + HERO_LIST_SPACE)));

			// 更新上阵状态;
			iter->second->updateOnfieldState(pParam.at(i).bIsOnField);
		}
		// 新增Hero;
		else
		{
			pParam.at(i).nMode = m_nMode;
			HeroListNode* hero = HeroListNode::create(&(pParam.at(i)));
			m_mapUIHeroList.insert(pair<int, HeroListNode*>(pParam.at(i).nHeroId, hero));
			Size innerSize = m_ui.listHero->getInnerContainerSize();
			hero->setPosition(Vec2(innerSize.width/6.0f + HERO_LIST_SPACE + (i % 3) * (innerSize.width/3.0f),
				innerSize.height - hero->getContentSize().height/2.0f - HERO_LIST_SPACE - (i/3) * (HERO_ICON_HEIGHT + HERO_LIST_SPACE)));
			//hero->addTouchEventListener(CC_CALLBACK_2(LayerFormation::onHeroListClicked, this, pParam.at(i).nHeroId));
			m_ui.listHero->addChild(hero);
			addClickEvent(hero, pParam.at(i).nHeroId);

			// 取消按钮动画;
			//hero->setPressedActionEnabled(false);

			// 更新上阵状态;
			hero->updateOnfieldState(pParam.at(i).bIsOnField);
		}
	} // end for

	// 更新活动关卡匹配列表;
	if (m_nMode == F_MODE_ACTIVITY)
	{
		refreshActivityHeroList();
	}

	// 滚动条;
	m_ui.listHeroBar->setVisible(m_ui.listHero->getInnerContainerSize().height > m_ui.listHero->getContentSize().height);
	m_ui.listHeroBar->setScrollBarLengthPercent(m_ui.listHero->getContentSize().height / m_ui.listHero->getInnerContainerSize().height);
	m_ui.listHeroBar->setScrollBarPercentY(0.0f);
}

void LayerFormation::addClickEvent( HeroListNode* hero, int nHeroId )
{
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=] (Touch* touch, Event* event)
	{
		// 动画播放结束前不允许点击;
		if (!m_bGlobalEnableClick)
			return false;

		// 追打展示视图，禁止英雄列表响应;
		if (m_bIsShowComboList)
			return false;

		// 先判断是否在列表区域内;
		Point locationInNode = m_ui.listHero->convertToNodeSpace(touch->getLocation());
		Size size = m_ui.listHero->getContentSize();
		Rect rect = Rect(0.0f, 0.0f, size.width, size.height);
		if (rect.containsPoint(locationInNode))
		{
			locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
			size = event->getCurrentTarget()->getContentSize();
			rect = Rect(0.0f, 0.0f, size.width, size.height);
			if (rect.containsPoint(locationInNode))
			{
				m_bIsHeroClicked = true;
				return true;
			}
		}
		return false;
	};
	listener->onTouchMoved = [=](Touch* touch, Event* event)
	{
		/*//
		//CCLOG("++++++++++++++ move x = %lf, y = %lf ++++++++++++++", abs(touch->getDelta().x), abs(touch->getDelta().y));
		CCLOG("++++++++++++++ move distance = %lf++++++++++++++", touch->getStartLocation().distance(touch->getLocation()));
		if (touch->getStartLocation().distance(touch->getLocation()) > 100)
		{
			m_bIsHeroClicked = false;
		}*/
		Point locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
		Size size = event->getCurrentTarget()->getContentSize();
		Rect rect = Rect(0.0f, 0.0f, size.width, size.height);
		if (!rect.containsPoint(locationInNode))
		{
			m_bIsHeroClicked = false;
		}
	};
	listener->onTouchEnded = [=] (Touch* touch, Event* event)
	{
		if (!m_bIsHeroClicked)
			return;

		Point locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
		Size size = event->getCurrentTarget()->getContentSize();
		Rect rect = Rect(0.0f, 0.0f, size.width, size.height);
		if (rect.containsPoint(locationInNode))
		{
			//hero->addTouchEventListener(CC_CALLBACK_2(LayerFormation::onHeroListClicked, this, pParam.at(i).nHeroId));
			onHeroListClicked(hero, Widget::TouchEventType::ENDED, nHeroId);
		}
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, hero);

}

void LayerFormation::removeClickEvent( HeroListNode* hero )
{
	_eventDispatcher->removeEventListenersForTarget(hero);
}

void LayerFormation::updateFormation(UI_FORMATION_PARAM* pParam)
{
	// 更新战斗力;
	ostringstream oss;
	oss << pParam->nFc;
	m_ui.txtFc->setString(STR_UTF8(oss.str()));
	oss.str("");

	// 更新上阵英雄数量;
	oss << FormationModel::getInstance()->getHeroOnFieldCount() << "/" << FORMATION_MAX_HERO_COUNT;
	m_ui.txtHeroCount->setString(oss.str());
	setTextAddStroke(m_ui.txtHeroCount, Color3B(0x80, 0x1A, 0x1A), 2);
	oss.str("");

	// 初次更新阵型数据;
	for (unsigned int i = 0; i < FORMATION_MAX_POS_COUNT; ++i)
	{
		if (pParam->vcFormation.at(i) != FORMATION_EMPTY_POS)
		{
			// 英雄上阵;
			procHeroOnField(pParam->vcHeroData.at(i));
		}
	}

	// 更新阵法数据;
	updateTacticsData();
}

void LayerFormation::procHeroOnField(UI_HERO_LIST_PARAM param)
{
	param.nMode = m_nMode;
	FormationHero* _hero = FormationHero::create(param);
	if (nullptr != _hero)
	{
		_hero->setPosition(Vec2(m_posSet[param.nPos]));
		_hero->setTag(param.nPos);
		m_ui.pnlField->addChild(_hero, calcLocalZOrder(param.nPos));
		m_mapUIFormationList.insert(pair<int, FormationHero*>(param.nHeroId, _hero));

		enableDrag(_hero);

		// 更新底座状态;
		updatePosBaseState(param.nPos, param.nHeroId, false);

	}
}


void LayerFormation::procHeroOffField(UI_HERO_LIST_PARAM param)
{
	auto iter = m_mapUIFormationList.find(param.nHeroId);
	assert(iter != m_mapUIFormationList.end());
	iter->second->removeFromParentAndCleanup(true);
	m_mapUIFormationList.erase(iter);
}


void LayerFormation::procHeroSwap( UI_SWAP_FORMATION_PARAM param )
{
	// 更新tag;
	auto iter = m_mapUIFormationList.find(param.uiOriHero.nHeroId);
	if (iter != m_mapUIFormationList.end())
	{
		iter->second->setTag(param.uiOriHero.nPos);
	}
	else
	{
		return;
	}

	// 可能是空位;
	if (param.uiDstHero.nHeroId != FORMATION_EMPTY_POS)
	{
		iter = m_mapUIFormationList.find(param.uiDstHero.nHeroId);
		if (iter != m_mapUIFormationList.end())
		{
			iter->second->setTag(param.uiDstHero.nPos);
		}
	}

	// 刷新显示层级;
	for (auto it : m_mapUIFormationList)
	{
		it.second->setLocalZOrder(calcLocalZOrder((HERO_POS)it.second->getTag()));
	}

	// 更新战斗力;
	ostringstream oss;
	oss << param.nFc;
	m_ui.txtFc->setString(oss.str());
	oss.str("");
}


FormationHero* LayerFormation::getFormationHero(int nHeroId)
{
	auto iter = m_mapUIFormationList.find(nHeroId);
	if (iter != m_mapUIFormationList.end())
	{
		return iter->second;
	}
	else
	{
		return nullptr;
	}
}


void LayerFormation::updateSkillInfo(const vector<FORMATION_SKILL_PARAM> vcSkillInfo)
{
	if (vcSkillInfo.size() > FORMATION_MAX_SKILL_TYPE)
	{
		return;
	}

	m_vcSkillParam = vcSkillInfo;

	// 标题图标;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(FormationModel::getInstance()->getCurSelectedHero());
	if (heroParam != nullptr)
	{
		ImageView* imgTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Skill_Title"));
		imgTitle->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
		if (heroParam->isMainHero())
		{
			imgTitle->loadTexture("Image/UIFormation/Icon/img_zhujuejinengxiangqing.png");
		}
		else
		{
			imgTitle->loadTexture("Image/UIFormation/Icon/img_jinengxiangqing.png");
		}
	}

	for (unsigned int i = 0; i < vcSkillInfo.size(); i++)
	{
		if (vcSkillInfo.at(i).nSkillId != FORMATION_INVALID_SKILL_ID)
		{
			// 是否开启;
			m_ui.btnSkill[i].btnSkill->setEnabled(true);
			m_ui.btnSkill[i].btnSkill->setBright(true);

			// 技能图标;
			if (nullptr != m_ui.btnSkill[i].imgSkill)
			{
				m_ui.btnSkill[i].imgSkill->loadTexture(ResourceUtils::getSmallIconPath(vcSkillInfo.at(i).nResId));
			}
			else
			{
				m_ui.btnSkill[i].imgSkill = ImageView::create(ResourceUtils::getSmallIconPath(vcSkillInfo.at(i).nResId));
				if (nullptr != m_ui.btnSkill[i].imgSkill)
				{
					//m_ui.btnSkill[i].imgSkill->setPosition(Vec2(m_ui.btnSkill[i].btnSkill->getContentSize().width/2,
					//	m_ui.btnSkill[i].btnSkill->getContentSize().height/2));
					//m_ui.btnSkill[i].btnSkill->addChild(m_ui.btnSkill[i].imgSkill);
					m_ui.btnSkill[i].imgSkill->setPosition(m_ui.btnSkill[i].btnSkill->getPosition());
					m_ui.btnSkill[i].btnSkill->getParent()->addChild(m_ui.btnSkill[i].imgSkill, 1);
				}
			}

			m_ui.btnSkill[i].imgSkill->removeAllChildren();

			// 未激活状态和遮罩;
			if (!vcSkillInfo.at(i).bEnabled)
			{
				ImageView* imgClosed = ImageView::create("Image/Icon/global/btn_kuang_jineng_zhezhao.png");
				imgClosed->setPosition(Vec2(m_ui.btnSkill[i].imgSkill->getContentSize().width/2.0f - 2.0f,
					m_ui.btnSkill[i].imgSkill->getContentSize().height/2.0f));
				m_ui.btnSkill[i].imgSkill->addChild(imgClosed);
				//Text* txtClosed = Text::create(STR_UTF8("未激活"), FONT_FZZHENGHEI, 22);
				Text* txtClosed = Text::create(DataManager::getInstance()->searchCommonTexdtById(10099), FONT_FZZHENGHEI, 22);
				txtClosed->setColor(Color3B::RED);
				txtClosed->setPosition(Vec2(m_ui.btnSkill[i].imgSkill->getContentSize().width/2.0f,
					m_ui.btnSkill[i].imgSkill->getContentSize().height/2.0f));
				m_ui.btnSkill[i].imgSkill->addChild(txtClosed);
			}
		}
		// 修改了技能显示需求之后，下边的else就没用了，留着容错吧;
		// 技能全部都会显示，只是有没有开启的区别;
		else
		{
			if (nullptr != m_ui.btnSkill[i].imgSkill)
			{
				m_ui.btnSkill[i].imgSkill->loadTexture("Image/Icon/global/btn_kuang_jineng_3.png");
			}
			else
			{
				m_ui.btnSkill[i].imgSkill = ImageView::create("Image/Icon/global/btn_kuang_jineng_3.png");
				if (nullptr != m_ui.btnSkill[i].imgSkill)
				{
					m_ui.btnSkill[i].imgSkill->setPosition(Vec2(m_ui.btnSkill[i].btnSkill->getContentSize().width/2,
						m_ui.btnSkill[i].btnSkill->getContentSize().height/2));
					m_ui.btnSkill[i].btnSkill->addChild(m_ui.btnSkill[i].imgSkill);
				}
			}
			m_ui.btnSkill[i].btnSkill->setEnabled(false);
			m_ui.btnSkill[i].btnSkill->setBright(false);
		}
	}
}


void LayerFormation::updateCombo(Combo_Skill_Param* pParam)
{
	// 清除全部;
	for (unsigned int i = 0; i < FORMATION_MAX_COMBO; ++i)
	{
		if (m_ui.rushHero[i] != nullptr)
		{
			m_ui.rushHero[i]->removeFromParent();
			m_ui.rushHero[i] = nullptr;
		}
	}

	assert(pParam->nMaxCombo <= FORMATION_MAX_COMBO);

	// 首发英雄;
	if (pParam->getComboListCount() == 0)
	{
		Vector<Node*> vcFirstRushChildren = m_ui.imgRush[0]->getChildren();
		for (auto it : vcFirstRushChildren)
		{
			it->setVisible(false);
		}
	}
	else
	{
		m_ui.imgRushFirstHeroSkill_Manual->setVisible(pParam->nFirstSkillType == AbstractSkill::SKILL_TYPE::ACTIVE_SKILL);
		m_ui.imgRushFirstHeroSkill_Atk->setVisible(pParam->nFirstSkillType == AbstractSkill::SKILL_TYPE::NORMAL_SKILL);
		m_ui.imgRushSkillDst[0]->loadTexture(getSkillStateIcon(pParam->vcComboSkillState.at(0).second.second));
		m_ui.imgRushSkillDst[0]->setScale(1.7f);
		m_ui.rushHero[0] = UIToolHeader::create(ResourceUtils::getSmallIconPath(pParam->vcComboListIcon.at(0)));
		m_ui.rushHero[0]->setPosition(Vec2(150.0f, 36.0f));
		m_ui.rushHero[0]->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_ui.rushHero[0]->setScale(0.6f);
		m_ui.imgRush[0]->addChild(m_ui.rushHero[0]);
	}

	// 其余;
	for (unsigned int i = 1; i < FORMATION_MAX_COMBO; ++i)
	{
		m_ui.imgRush[i]->setVisible(false);
		m_ui.imgRushX[i]->setVisible(true);
		m_ui.imgRushEmpty->setVisible(false);
	}

	if (pParam->nMaxCombo <= 1)
	{
		m_ui.imgRushEmpty->setVisible(true);
	}
	else
	{
		for (unsigned int i = 1; i < pParam->nMaxCombo && i < FORMATION_MAX_COMBO; ++i)
		{
			m_ui.imgRush[i]->setVisible(true);
			m_ui.imgRushX[i]->setVisible(false);

			float fScale = 1.7f;
			m_ui.imgRushSkillPre[i]->loadTexture(getSkillStateIcon(pParam->vcComboSkillState.at(i).second.first));
			m_ui.imgRushSkillPre[i]->setScale(fScale);
			m_ui.imgRushSkillDst[i]->loadTexture(getSkillStateIcon(pParam->vcComboSkillState.at(i).second.second));
			m_ui.imgRushSkillDst[i]->setScale(fScale);
			m_ui.imgRushArrow[i]->setVisible(pParam->vcComboSkillState.at(i).second.second != AbstractSkill::NONE_STATE);
			m_ui.rushHero[i] = UIToolHeader::create(ResourceUtils::getSmallIconPath(pParam->vcComboListIcon.at(i)));
			m_ui.rushHero[i]->setPosition(Vec2(150.0f, 36.0f));
			m_ui.rushHero[i]->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
			m_ui.rushHero[i]->setScale(0.6f);
			m_ui.imgRush[i]->addChild(m_ui.rushHero[i]);
		}
	}

	// 连击数量;
	ostringstream oss;
	oss << "Image/UIFormation/Icon/btn_zuidazhuida" << max(pParam->nMaxCombo-1, 0) << "_1.png";
	m_ui.btnShowCombo->loadTextureNormal(oss.str());
	oss.str("");
	oss << "Image/UIFormation/Icon/btn_zuidazhuida" << max(pParam->nMaxCombo-1, 0) << "_2.png";
	m_ui.btnShowCombo->loadTexturePressed(oss.str());
	oss.str("");
}


void LayerFormation::refreshCurSelected(const vector<int> vcSelected)
{
	int nCurSelectedHero = vcSelected.at(FORMATION_MAX_POS_COUNT);
	HERO_POS nCurSelectedPos = (HERO_POS)(vcSelected.at(FORMATION_MAX_POS_COUNT + 1));

	// 阵型选中;
	updatePosBaseState(m_nCurSelPos, vcSelected.at(m_nCurSelPos), false);
	updatePosBaseState(nCurSelectedPos, vcSelected.at(nCurSelectedPos), true);

	// 先交换有英雄的阵位，再交换一个没有英雄的阵位，因为这3种状态不同，所以需要多一个刷新操作;
	if (m_nCurSelPos != m_nCurTouchPos && nCurSelectedPos != m_nCurTouchPos)
	{
		updatePosBaseState(m_nCurTouchPos, vcSelected.at(m_nCurTouchPos), false);
	}

	// 当前选中;
	m_nCurSelPos = nCurSelectedPos;

	// 列表选中;
	for (auto iter = m_mapUIHeroList.begin(); iter != m_mapUIHeroList.end(); iter++)
	{
		iter->second->updateSelected(iter->first == nCurSelectedHero);
	}

	// 编辑天赋;
	m_ui.btnEditSkill->setVisible((nCurSelectedHero != -1) && (HeroModel::getInstance()->searchHeroById(nCurSelectedHero)->isMainHero()));
}


void LayerFormation::enableDrag(Node* node, bool bEnable)
{
	if (bEnable)
	{
		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);

		listener->onTouchBegan = [=](Touch* touch, Event* event)
		{
			Point locationInNode = event->getCurrentTarget()->convertToNodeSpace(touch->getLocation());
			Size size = event->getCurrentTarget()->getContentSize();
			Rect rect = Rect(0, 0, size.width, size.height);
			m_bIsLongPress = false;
			if (rect.containsPoint(locationInNode))
			{
				this->m_nCurTouchPos = (HERO_POS)(node->getTag());

				// 处理长按;
				if (nullptr != getFormationHero(FormationModel::getInstance()->getHeroAtPos(this->m_nCurTouchPos)))
				{
					this->m_bEnableMove = true;
					this->schedule(schedule_selector(LayerFormation::updateLongPressed), HERO_LONG_PRESS_INTERVAL);
				}
				return true;
			}
			else
			{
				return false;
			}
		};

		listener->onTouchMoved = [=](Touch* touch, Event* event)
		{
			// 拖动前的位移判定;
			if (!m_bIsDrag)
			{
				//
				if (abs(touch->getDelta().x) > 5.0f || abs(touch->getDelta().y) > 5.0f)
				{
					// 停止长按判定;
					unschedule(schedule_selector(LayerFormation::updateLongPressed));

					// 防止误操作;
					if (!this->m_bEnableMove)
						return;
					if (nullptr == getFormationHero(FormationModel::getInstance()->getHeroAtPos(this->m_nCurTouchPos)))
						return;

					// 确认拖动;
					m_bIsDrag = true;

					// 后续处理: 1.提高显示优先级;
					node->setLocalZOrder(19);

					// 后续处理: 2.透明处理;
					if (!m_bIsTransparent)
					{
						m_bIsTransparent = true;
						setTransparentWithoutMe(event->getCurrentTarget());
					}
				}
			}

			// 已判定为拖动，直接移动;
			if (m_bIsDrag)
			{
				// 拖动范围限制;
				Point locationInParent = event->getCurrentTarget()->getParent()->convertToNodeSpace(touch->getLocation());
				Size size = event->getCurrentTarget()->getParent()->getContentSize();
				Rect rect = Rect(0.0f, 0.0f, size.width + 100.0f, size.height + 100.0f);
				if (rect.containsPoint(locationInParent))
				{
					node->setPosition( node->getPosition() + touch->getDelta() );
				}
			}
		};

		listener->onTouchEnded = [=](Touch* touch, Event* event)
		{
			// 取消长按判定;
			unschedule(schedule_selector(LayerFormation::updateLongPressed));

			// 取消透明;
			if (m_bIsTransparent)
			{
				m_bIsTransparent = false;
				setTransparentWithoutMe(event->getCurrentTarget(), false);
			}

			// 防止误操作;
			if (!this->m_bEnableMove)
				return;

			// 拖动;
			if (this->m_bIsDrag)
			{
				m_bIsDrag = false;

				// 判定命中位置;
				HERO_POS nDstPos = judgeEndPosition(event->getCurrentTarget(), touch);

				// 启动动画;
				//auto pos = touch->getLocation();
				//startAnimationPosChanging(event->getCurrentTarget(), judgeEndPosition(event->getCurrentTarget(), pos));
				startAnimationPosChanging(event->getCurrentTarget(), nDstPos);

				// 重置显示优先级;
				//event->getCurrentTarget()->setLocalZOrder(calcLocalZOrder((HERO_POS)(event->getCurrentTarget()->getTag())));
			}
			// 点按;
			else if(!this->m_bIsLongPress)
			{
				onHeroOnFieldClicked(this, Widget::TouchEventType::ENDED, m_nCurTouchPos);
				return;
			}
		};

		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, node);
	}
	else
	{
		_eventDispatcher->removeEventListenersForType(cocos2d::EventListener::Type::TOUCH_ONE_BY_ONE);
	}
}


HERO_POS LayerFormation::judgeEndPosition( Node* node, Touch* touch)
{
	// 初始值置为该节点的初始位置;
	HERO_POS  nDstPos = (HERO_POS)node->getTag();

	// 遍历所有阵位位置，判定是否命中;
	for (int i = 0; i < FORMATION_MAX_POS_COUNT; i++)
	{
		// 使用touch位置判定命中;
		/*
		if (alphaTouchCheck(m_ui.imgPos[i], touch))
		{
			nDstPos = i;
			break;
		}
		*/

		// 使用node位置判定命中;
		Point pt(node->getPosition().x + 100.0f, node->getPosition().y);
		if (alphaCheck(m_ui.imgPos[i], node->convertToWorldSpaceAR(Vec2(0.0f, 0.0f))))//pt))
		{
			nDstPos = (HERO_POS)i;
			break;
		}
	}

	return nDstPos;
}


void LayerFormation::startAnimationPosChanging(Node* src, HERO_POS nDstPos)
{
	// 本方位移动画;
	Action* seq = Sequence::create( MoveTo::create(HERO_MOVE_DURATION, getFormationPosVec(nDstPos)),
		CallFuncN::create(CC_CALLBACK_1(LayerFormation::onAnimationDone, this, nDstPos)),
		nullptr);
	src->runAction(seq);

	// 对方位移动画(如果对位有Hero);
	FormationHero* dstHero = getFormationHero(FormationModel::getInstance()->getHeroAtPos(nDstPos));
	seq = Sequence::create(
		MoveTo::create(HERO_MOVE_DURATION, getFormationPosVec((HERO_POS)src->getTag())),
		CallFuncN::create([=](Node* pSender){
			// 重置显示优先级;
			dstHero->setLocalZOrder(calcLocalZOrder((HERO_POS)(dstHero->getTag())));
	}),
		nullptr);
	if (nullptr != dstHero && dstHero != src)
	{
		dstHero->runAction(seq);
	}
	
}

void LayerFormation::onAnimationDone(Node* pSender, HERO_POS nDstPos)
{
	// 原始位置;
	int nOriPos = pSender->getTag();

	// 重置显示优先级;
	pSender->setLocalZOrder(calcLocalZOrder(nDstPos));

	if (nOriPos != nDstPos)
	{
		// 实际操作换位;
		m_pController->handleSwapFormation((HERO_POS)pSender->getTag(), nDstPos);
	}
}


void LayerFormation::updateLongPressed(float delta)
{
	this->unschedule(schedule_selector(LayerFormation::updateLongPressed));
	m_bIsLongPress = true;

	// 空位;
	FormationHero* hero = getFormationHero(FormationModel::getInstance()->getHeroAtPos(m_nCurTouchPos));
	if (nullptr == hero)
	{
		return;
	}

	// 长按之后不允许拖动;
	this->m_bEnableMove = false;

	// 更新选中;
	onHeroOnFieldClicked(this, Widget::TouchEventType::ENDED, m_nCurTouchPos);

	// 跳转到角色界面;
// 	LayerHero* layerHero = LayerHero::create();
// 	layerHero->show(this);
// 	layerHero->showHeroList();

}


Vec2 LayerFormation::getFormationPosVec(HERO_POS nPos)
{
	/*
	if (nPos < 0 || nPos >= FORMATION_MAX_POS_COUNT)
	{
		return Vec2(HERO_ICON_WIDTH, HERO_ICON_HEIGHT);
	}

	int _x = HERO_ICON_WIDTH*2 + HERO_ICON_WIDTH/2 + 40.0f;
	int _y = HERO_ICON_HEIGHT*2 + HERO_ICON_HEIGHT/2 + 40.0f;
	_x += ((-HERO_ICON_WIDTH-16.0f) * (nPos/3));
	_y += ((-HERO_ICON_HEIGHT-16.0f) * (nPos%3));

	return Vec2(_x, _y);
	*/

	if (nPos == Hero_Pos_Invalid)
	{
		return Vec2(0.0f, 0.0f);
	}

	return m_posSet[nPos];
}


int LayerFormation::calcLocalZOrder( HERO_POS nPos )
{
	/*
	  阵位;	6	3	0		ZOrder; 0	1	2
			7	4	1				3	4	5
			8	5	2				6	7	8
	*/
	return 2 + 6*(nPos%3) - (nPos/3 + (nPos%3)*3);
}


string LayerFormation::getFormationErrorMsg(int nErrCode)
{
	string strReason;
	switch (nErrCode)
	{
		/*
	case -1:	strReason.assign("主角不能下阵");				break;
	case -2:	strReason.assign("上阵英雄数量已达上限");		break;
	case -3:	strReason.assign("请选择一个英雄来操作");		break;
	case -4:	strReason.assign("正在保存阵型信息");			break;
	case -6:	strReason.assign("所选择英雄已阵亡");			break;
	case -7:	strReason.assign("请选择上阵英雄");				break;
	default:	strReason.assign("未知错误");					break;
	*/
	case -1:	strReason.assign(DataManager::getInstance()->searchCommonTexdtById(10100));		break;
	case -2:	strReason.assign(DataManager::getInstance()->searchCommonTexdtById(10101));		break;
	case -3:	strReason.assign(DataManager::getInstance()->searchCommonTexdtById(10102));		break;
	case -4:	strReason.assign(DataManager::getInstance()->searchCommonTexdtById(10103));			break;
	case -6:	strReason.assign(DataManager::getInstance()->searchCommonTexdtById(10104));			break;
	case -7:	strReason.assign(DataManager::getInstance()->searchCommonTexdtById(10105));				break;
	default:	strReason.assign(DataManager::getInstance()->searchCommonTexdtById(10106));					break;
	}
	return strReason;
}

void LayerFormation::popCallbackFunc(Ref* ref, CustomRetType nResult)
{
	switch (nResult)
	{
	case RET_TYPE_OK:
		{
			SceneFormation* parent = dynamic_cast<SceneFormation*>(this->getParent());
			if (nullptr != parent)
			{
				parent->onBtnClose();
			}
		}
		break;
	default:
		break;
	}
}

void LayerFormation::myTestCallbackFunc(Ref* ref, CustomRetType nResult)
{
	switch (nResult)
	{
	case RET_TYPE_OK:
		CCLOG("callbackFunc: ok");
		break;
	case RET_TYPE_CANCEL:
		CCLOG("callbackFunc: cancel");
		break;
	case RET_TYPE_CLOSE:
		CCLOG("callbackFunc: close");
		break;
	default:
		break;
	}
}

void LayerFormation::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	if (pSender == m_ui.listHero)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.listHero->getInnerContainerSize();
				Vec2 vec = m_ui.listHero->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.listHero->getContentSize().height);
				//log("formation scrolling: x = %f, y = %f, per: %f", vec.x, vec.y, percentY);

				m_ui.listHeroBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			break;

		default:
			break;
		}
	}
}

void LayerFormation::updatePosBaseState( HERO_POS nPos, int nHeroId, bool bSelected )
{
	if (nPos == Hero_Pos_Invalid)
	{
		return;
	}

	if (bSelected)
	{
		m_ui.imgPos[nPos]->loadTexture("Image/UIFormation/Icon/img_gezi_3.png");
		ostringstream oss;
		oss << "Image/UIFormation/Icon/img_number2_" << nPos+1 << ".png";
		m_ui.imgPosIndex[nPos]->loadTexture(oss.str());
		oss.str("");
		m_ui.imgPosIndex[nPos]->setVisible(true);
	}
	else
	{
		if (nHeroId == FORMATION_EMPTY_POS)
		{
			m_ui.imgPos[nPos]->loadTexture("Image/UIFormation/Icon/img_gezi_1.png");
			ostringstream oss;
			oss << "Image/UIFormation/Icon/img_number1_" << nPos+1 << ".png";
			m_ui.imgPosIndex[nPos]->loadTexture(oss.str());
			oss.str("");
			m_ui.imgPosIndex[nPos]->setVisible(true);
		}
		else
		{
			m_ui.imgPos[nPos]->loadTexture("Image/UIFormation/Icon/img_gezi_2.png");
			m_ui.imgPosIndex[nPos]->setVisible(false);
		}
	}
}

bool LayerFormation::alphaTouchCheck( ImageView* image, Touch* touch )
{
	//Point pt = image->convertTouchToNodeSpace(touch);
	return alphaCheck(image, touch->getLocation());
}

bool LayerFormation::alphaCheck( ImageView* image, Point pt_Global )
{
	if (nullptr == image)
	{
		return false;
	}

	// 将坐标转换为相对ImageView坐标;
	Point pt = image->convertToNodeSpace(pt_Global);

	//先从ImageView中提取Sprite
	Sprite* selectSprite = static_cast<Sprite*>(image->getVirtualRenderer());

	// 图片区域判定(added by Phil 03/21/2015);
	Rect rectImage = Rect(0, 0, image->getContentSize().width, image->getContentSize().height);
	if (!rectImage.containsPoint(pt))
	{
		return false;
	}

	//复制Sprite
	CCSprite* tempSprite = CCSprite::createWithSpriteFrame(selectSprite->displayFrame());
	tempSprite->setAnchorPoint(Point::ZERO);
	tempSprite->setPosition(Point::ZERO);
	//tempSprite->setSkewY(image->getSkewY());
	Rect rect = tempSprite->getBoundingBox();
	unsigned int x = pt.x;
	unsigned int y = rect.size.height - pt.y;

	//初始化render;
	RenderTexture* renderer = RenderTexture::create(rect.size.width, rect.size.height);

	//开始绘制;
	renderer->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
	tempSprite->visit();
	renderer->end();

	Director::getInstance()->getRenderer()->render();

	//获取像素点;
	Image* pImage = renderer->newImage();
	unsigned char* data_ = pImage->getData();
	int pa = 4 * ((pImage->getHeight() - (int)(pt.y) - 1) * pImage->getWidth() + (int)(pt.x)) + 3;  
	unsigned int ap = data_[pa];
	pImage->release();

	if (ap > 20)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void LayerFormation::setTransparentWithoutMe( Node* pSelf, bool bTrans )
{
	for (auto it : m_mapUIFormationList)
	{
		if (pSelf == it.second)
			continue;

		// 1. 灰色;
		it.second->setColor(bTrans ? Color3B(0x89, 0x7C, 0x67) : Color3B::WHITE);

		// 2. 设置透明度(0~255, 0=全透明，255=不透明);
		// spine透明度效果不满足要求;
		//it.second->setTransparent(bTrans ? 153 : 255);
	}
}

void LayerFormation::playEnterAnimation()
{
	//此回调为播放时间最长的动画回调，如改动动画播放时长，请注意修改回调
	auto callBack = [this]()
	{
		// 动画播放完成后允许界面点击;
		m_bGlobalEnableClick = true;

		// 新手引导;
		switch (m_nMode)
		{
		case F_MODE_FORMATION:
			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_FORMATION);
			break;
		case F_MODE_TRAIL:
			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_TRAIL_FORMATION);
			break;
		case F_MODE_BARRIER:
			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_BARRIER_FORMATION);
			break;
		default:
			break;
		}
	};

	// 移动距离和时间;
	float shiftPosX = 450.0f;
	float shiftTime = 0.3f;

	// 面板飞入;
	Margin _oriMargin = m_ui.imgHeroListBg->getLayoutParameter()->getMargin();
	Margin _margin = _oriMargin;
	_margin.right += shiftPosX;
	m_ui.imgHeroListBg->getLayoutParameter()->setMargin(_margin);
	auto seq = Sequence::create(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(shiftPosX, 0.0f))),
		CallFuncN::create([=](Node* pSender){
			m_ui.imgHeroListBg->getLayoutParameter()->setMargin(_oriMargin);
	}),
		CallFunc::create(callBack), nullptr);
	m_ui.imgHeroListBg->runAction(seq);

	// 技能标题飞入;
	shiftTime = 0.2f;
	ImageView* imgTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Skill_Title"));
	_oriMargin = imgTitle->getLayoutParameter()->getMargin();
	_margin = _oriMargin;
	_margin.left -= shiftPosX;
	imgTitle->getLayoutParameter()->setMargin(_margin);
	auto seq2 = Sequence::create(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(shiftPosX, 0.0f))), 
		CallFuncN::create([=](Node* pSender){
			imgTitle->getLayoutParameter()->setMargin(_oriMargin);
	}), nullptr);
	imgTitle->runAction(seq2);

	ImageView* imgLine = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Line"));
	_oriMargin = imgLine->getLayoutParameter()->getMargin();
	_margin = _oriMargin;
	_margin.left -= shiftPosX;
	imgLine->getLayoutParameter()->setMargin(_margin);
	auto seq2_1 = Sequence::create(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(shiftPosX, 0.0f))), 
		CallFuncN::create([=](Node* pSender){
			imgLine->getLayoutParameter()->setMargin(_oriMargin);
	}), nullptr);
	imgLine->runAction(seq2_1);

	Button* btnTalent = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Edit_Skill"));
	_oriMargin = btnTalent->getLayoutParameter()->getMargin();
	_margin = _oriMargin;
	_margin.left -= shiftPosX;
	btnTalent->getLayoutParameter()->setMargin(_margin);
	auto seq2_2 = Sequence::create(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(shiftPosX, 0.0f))), 
		CallFuncN::create([=](Node* pSender){
			btnTalent->getLayoutParameter()->setMargin(_oriMargin);
	}), nullptr);
	btnTalent->runAction(seq2_2);

	// 技能飞入;
	shiftTime = 0.1f;
	_oriMargin = m_ui.pnlSkill->getLayoutParameter()->getMargin();
	_margin = _oriMargin;
	_margin.right += shiftPosX;
	m_ui.pnlSkill->getLayoutParameter()->setMargin(_margin);
	auto seq3 = Sequence::create(EaseSineOut::create(MoveBy::create(shiftTime, Vec2(shiftPosX, 0.0f))),
		CallFuncN::create([=](Node* pSender){
			m_ui.pnlSkill->getLayoutParameter()->setMargin(_oriMargin);
	}), nullptr);
	m_ui.pnlSkill->runAction(seq3);
}

void LayerFormation::updateTacticsData()
{
	if (ModelTactics::getInstance()->isInittedFromSvr())
	{
		ostringstream oss;
		oss << ModelTactics::getInstance()->getCurRowData(BACK_ROW_TYPE).mRowLevel << DataManager::getInstance()->searchCommonTexdtById(20101);
		m_ui.txtBackLevel->setString(oss.str());
		setTextAddStroke(m_ui.txtBackLevel, Color3B(0x80, 0x1A, 0x1A), 2);
		oss.str("");
		oss << ModelTactics::getInstance()->getCurRowData(MIDDLE_ROW_TYPE).mRowLevel << DataManager::getInstance()->searchCommonTexdtById(20101);
		m_ui.txtMiddleLevel->setString(oss.str());
		setTextAddStroke(m_ui.txtMiddleLevel, Color3B(0x80, 0x1A, 0x1A), 2);
		oss.str("");
		oss << ModelTactics::getInstance()->getCurRowData(FRONT_ROW_TYPE).mRowLevel << DataManager::getInstance()->searchCommonTexdtById(20101);
		m_ui.txtFrontLevel->setString(oss.str());
		setTextAddStroke(m_ui.txtFrontLevel, Color3B(0x80, 0x1A, 0x1A), 2);
		oss.str("");

		m_ui.txtAddition->setString(ModelTactics::getInstance()->getCurUsingTacticsAttribute());
	}
	else
	{
		TacticsController::getInstance()->sendGetTacticsListMsg();
	}
}

void LayerFormation::updateHeroCombo( int nHeroId )
{
	auto iter = m_mapUIFormationList.find(nHeroId);
	if (iter != m_mapUIFormationList.end())
	{
		UI_HERO_LIST_PARAM param;
		FormationModel::getInstance()->constructUIHeroParam(nHeroId, param);
		iter->second->updateCombo(param);
	}
	auto iter2 = m_mapUIHeroList.find(nHeroId);
	if (iter2 != m_mapUIHeroList.end())
	{
		UI_HERO_LIST_PARAM param;
		FormationModel::getInstance()->constructUIHeroParam(nHeroId, param);
		iter2->second->updateCombo(&param);
	}
}

void LayerFormation::refreshActivityHeroList()
{
	// 取到阵型数据和匹配名单;
	vector<pair<int, string> >  vcActivityHeroList;
	ActivityModel::getInstance()->getActivityHeroList(vcActivityHeroList);

	if (vcActivityHeroList.size() > 0)
	{
		// 隐藏上阵人数，显示匹配列表;
		m_ui.imgHeroCountTitle->setVisible(false);
		m_ui.pnlActivityHeroList->setVisible(true);

		// 取到阵型数据并转换为templateId;
		vector<int>  vcValidFormation;
		FormationModel::getInstance()->getFormationWithoutEmptyPos(vcValidFormation);
		for (auto &it : vcValidFormation)
		{
			it = HeroModel::getInstance()->searchHeroById(it)->mTemplateId;
		}

		// 匹配显示;
		vector<int>  vcDst;
		for (int k = 0; k < FORMATION_MAX_HERO_COUNT; ++k)
		{
			// 容错，万一配置不足FORMATION_MAX_HERO_COUNT个英雄;
			if (vcActivityHeroList.size() >= k+1)
			{
				auto iter = find(vcValidFormation.begin(), vcValidFormation.end(), vcActivityHeroList.at(k).first);
				setHeroNameBright(k, vcActivityHeroList.at(k).second, (iter != vcValidFormation.end()));
				if (iter != vcValidFormation.end())
				{
					vcDst.push_back(*iter);
				}
			}
			else
			{
				setHeroNameBright(k, "", false);
			}
		}

		// 左侧列表匹配;
		for_each(m_mapUIHeroList.begin(), m_mapUIHeroList.end(), [=](map<int, HeroListNode*>::value_type _value){
			_value.second->updateActivityFlag(vcDst);
		});
	}
}

void LayerFormation::setHeroNameBright( int nIndex, string strName, bool bBright )
{
	assert(nIndex >= 0 && nIndex <= FORMATION_MAX_HERO_COUNT);
	m_ui.txtActivityHeroes[nIndex]->setString(strName.c_str());
	if (bBright)
	{
		m_ui.txtActivityHeroes[nIndex]->setColor(Color3B(0xFF, 0xDE, 0x00));
		setTextAddStroke(m_ui.txtActivityHeroes[nIndex], Color3B(0x38, 0x2F, 0x00), 3);
	}
	else
	{
		m_ui.txtActivityHeroes[nIndex]->setColor(Color3B(0xA1, 0xA1, 0xA1));
		setTextAddStroke(m_ui.txtActivityHeroes[nIndex], Color3B(0x3A, 0x3A, 0x3A), 3);
	}
}

