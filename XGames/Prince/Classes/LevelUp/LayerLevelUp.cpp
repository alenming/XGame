#include "LayerLevelUp.h"
#include "MainModule/MainCityLoadingScene.h"
#include "FightModule/SceneFight.h"
#include "GuideCenter/GuideManager.h"
#include "NewStarsCampModule/StarsCampController.h"
#include "LevelUpController.h"

LayerLevelUp::LayerLevelUp(void)
	: _callback(nullptr)
{
}


LayerLevelUp::~LayerLevelUp(void)
{
}

LayerLevelUp* LayerLevelUp::create(UI_LEVEL_UP_PARAM* param,  LevelUpCallback callback/* = nullptr*/)
{
	LayerLevelUp* _layer = new LayerLevelUp;
	if (nullptr != _layer && _layer->init(param, callback))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

bool LayerLevelUp::init(UI_LEVEL_UP_PARAM* param, LevelUpCallback callback/* = nullptr*/)
{
	if (!Layer::init())
	{
		return false;
	}

	_callback = callback;
	initUI(param);


	return true;
}

void LayerLevelUp::onEnter()
{
	Layer::onEnter();

	// 音效;
	SoundUtils::playSoundEffect("yangcheng_3");

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_LEVEL_UP);
}

void LayerLevelUp::initUI(UI_LEVEL_UP_PARAM* param)
{
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 加载UI;
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UILevelup/UILevelup.ExportJson"));
	this->addChild(m_ui.pRoot, 9);

	// 功能开启;
	m_ui.imgNew = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_New"));
	m_ui.imgNew->setVisible(false);
	m_ui.imgNewTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_New_Title"));
	m_ui.imgCurTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Cur_Title"));
	m_ui.imgCurTitle->setVisible(false);
	m_ui.imgNext = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Next"));
	m_ui.imgNext->setVisible(false);
	openFunc(param->nNewLv);

	// 精力和体力上限;
	m_ui.imgJingli = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_jingli"));
	m_ui.imgTili = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_tili"));
	m_ui.imgJiantou1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_jiantou1"));
	m_ui.imgJiantou2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_jiantou2"));
	m_ui.txtJingli1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_jingli1"));
	m_ui.txtJingli1->setFontName(FONT_FZZHENGHEI);
	ostringstream oss;
	oss << param->nPreEnergy;
	m_ui.txtJingli1->setString(oss.str());
	oss.str("");
	m_ui.txtJingli2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_jingli2"));
	m_ui.txtJingli2->setFontName(FONT_FZZHENGHEI);
	oss << param->nEnergy;
	m_ui.txtJingli2->setString(oss.str());
	oss.str("");
	m_ui.txtTili1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_tili1"));
	m_ui.txtTili1->setFontName(FONT_FZZHENGHEI);
	oss << param->nPrePower;
	m_ui.txtTili1->setString(oss.str());
	oss.str("");
	m_ui.txtTili2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_tili2"));
	m_ui.txtTili2->setFontName(FONT_FZZHENGHEI);
	oss << param->nPower;
	m_ui.txtTili2->setString(oss.str());
	oss.str("");

	// 法阵动画;
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniFazhen/AniFazhen.ExportJson");
	Armature* _armature = Armature::create("AniFazhen");
	_armature->setScaleY(0.186f);
	_armature->setPosition(Vec2(pos.x + visibleSize.width/2.0f, 250.0f));
	this->addChild(_armature, 10);
	_armature->getAnimation()->play("fazhenR");

	// 法阵光动画;
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniFazhenGuang/AniFazhenGuang.ExportJson");
	Armature* _armature1 = Armature::create("AniFazhenGuang");
	_armature1->setPosition(_armature->getPosition());
	this->addChild(_armature1, 11);
	_armature1->getAnimation()->play("fazhen");

	// 主角;
	int nResId = HeroModel::getInstance()->getMainHero()->mResId;
	SpineAnimation* pSkelHero = SpineAnimation::createFighterWithResId(nResId);
	pSkelHero->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	pSkelHero->setPosition(_armature->getPosition());
	pSkelHero->setAnimation(0, szANI_WIN, true);
	pSkelHero->setScale(0.9f);
	this->addChild(pSkelHero, 11);

	// 星光/文字;
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniOnce/AniOnce.ExportJson");
	Armature* _armature2 = Armature::create("AniOnce");
	_armature2->setPosition(Vec2(pos.x + visibleSize.width/2.0f, visibleSize.height/2.0f));
	this->addChild(_armature2, 12);
	oss.str("");
	if (param->nNewLv > 9)
	{
		Bone* _bone1 = _armature2->getBone("tN");
		Bone* _bone2 = _armature2->getBone("tN+");
		oss << "Image/UIChapter/Num/" << param->nNewLv/10 << ".png";
		Skin* _newSkin1 = Skin::create(oss.str());
		_bone1->addDisplay(_newSkin1, 0);
		oss.str("");
		oss << "Image/UIChapter/Num/" << param->nNewLv%10 << ".png";
		Skin* _newSkin2 = Skin::create(oss.str());
		_bone2->addDisplay(_newSkin2, 0);
		_armature2->getAnimation()->play("once");
	}
	else
	{
		Bone* _bone = _armature2->getBone("tN");
		oss << "Image/UIChapter/Num/" << param->nNewLv << ".png";
		Skin* _newSkin = Skin::create(oss.str());
		_bone->addDisplay(_newSkin, 0);
		oss.str("");
		_armature2->getAnimation()->play("once_Copy1");
	}
	_armature2->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerLevelUp::movementEventCallFunc, this));

	// 点击继续;
	showTouchContinue();
}

void LayerLevelUp::showTouchContinue()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	ImageView* _img = ImageView::create("Image/Icon/global/img_dianjipingmu.png");
	this->addChild(_img, 9);
	_img->setPosition(Vec2(pos.x + visibleSize.width/2.0f, 54.0f));

	ActionInterval* easeAct1 =  EaseSineInOut::create(FadeOut::create(1.0f));
	ActionInterval* easeAct2 =  EaseSineInOut::create(FadeIn::create(0.6f));
	ActionInterval* seq = Sequence::create(easeAct1, easeAct2, nullptr);
	_img->runAction(RepeatForever::create(seq));

	m_ui.pRoot->addTouchEventListener(CC_CALLBACK_2(LayerLevelUp::onLayerTouched, this));
}

void LayerLevelUp::onLayerTouched(Ref* ref, Widget::TouchEventType type)
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		jumpScene();
	}
}


void LayerLevelUp::update( float delta )
{
	this->unscheduleUpdate();
	this->removeFromParentAndCleanup(true);
}

void LayerLevelUp::movementEventCallFunc( Armature *armature, MovementEventType movementType, const std::string& movementID )
{
	if (movementType == COMPLETE)
	{
		if (movementID.compare("once") == 0 || movementID.compare("once_Copy1") == 0)
		{
			// 体力和精力上限的提示;
			ActionInterval* easeAct1 =  EaseSineInOut::create(ScaleTo::create(0.2f, 1.3f));
			ActionInterval* easeAct2 =  EaseSineInOut::create(ScaleTo::create(0.2f, 1.0f));
			ActionInterval* seq1 = Sequence::create(easeAct1, easeAct2, nullptr);
			m_ui.imgJingli->runAction(Repeat::create(seq1, 2));
			ActionInterval* seq2 = seq1->clone();
			m_ui.imgTili->runAction(Repeat::create(seq2, 2));

			Vec2 oriPos[2];
			oriPos[0] = m_ui.imgJiantou1->getPosition();
			oriPos[1] = m_ui.imgJiantou2->getPosition();
			ActionInterval* seq[2];
			for (unsigned int i = 0; i < 2; ++i)
			{
				ActionInterval* easeAct3 =  EaseSineInOut::create(MoveTo::create(0.2f, Vec2(oriPos[i].x + 5.0f, oriPos[i].y)));
				ActionInterval* easeAct4 =  EaseSineInOut::create(MoveTo::create(0.2f, oriPos[i]));
				seq[i] = Sequence::create(easeAct3, easeAct4, nullptr);
			}
			m_ui.imgJiantou1->runAction(Repeat::create(seq[0], 2));
			m_ui.imgJiantou2->runAction(Repeat::create(seq[1], 2));
		}
	}
}

void LayerLevelUp::openFunc( int nCurLv )
{
	if (nCurLv < 2)
	{
		return;
	}

	DataTable* dtLvUp = DataManager::getInstance()->getDTLevelUpGuide();
	assert(dtLvUp != nullptr);
	RowData* _data = dtLvUp->searchDataById(nCurLv);
	if (nullptr != _data)
	{
		// 构造数据;
		LVUP_OPEN_TYPE nOpenType = OPEN_TYPE_INVALID;
		int nValue = _data->getIntData("lefTpy");
		if (nValue != -1)
		{
			nOpenType = (LVUP_OPEN_TYPE)nValue;
		}
		int nResId = _data->getIntData("leftIcon");
		LVUP_JUMP_TYPE nJumpType = JUMP_TYPE_INVALID;
		nValue = _data->getIntData("interFace");
		if (nValue != -1)
		{
			nJumpType = (LVUP_JUMP_TYPE)nValue;
		}

		// UI;
		m_ui.imgNew->setVisible(true);
		m_ui.imgNewTitle->setVisible(nOpenType == OPEN_TYPE_NEW);
		m_ui.imgCurTitle->setVisible(nOpenType == OPEN_TYPE_OPENED);
		ostringstream oss1, oss2, oss3;
		oss1 << "Image/Icon/Func/btn_" << nResId << "_1.png";
		oss2 << "Image/Icon/Func/btn_" << nResId << "_2.png";
		oss3 << "Image/Icon/Func/btn_" << nResId << "_3.png";
		m_ui.btnNew = Button::create(oss1.str(), oss2.str(), oss3.str());
		m_ui.btnNew->setPosition(Vec2(137.0f, 310.0f));
		m_ui.imgNew->addChild(m_ui.btnNew);
		m_ui.btnNew->addTouchEventListener(CC_CALLBACK_2(LayerLevelUp::onBtnClicked, this, nJumpType));
		oss1.str("");
		oss2.str("");
		oss3.str("");

		////// 下一级开启;

		// 查功能开放等级表，重新排序;
		int nNextLv = 1;
		nResId = _data->getIntData("rightIcon");;
		DataTable* _dtFunc = DataManager::getInstance()->getDTFunctionLevel();
		assert(_dtFunc != nullptr);
		_data = nullptr;
		vector<int> vcLevel;
		multimap<int, RowData>& tableData = _dtFunc->getTableData();
		for (auto iter =tableData.begin(); iter != tableData.end(); ++iter)
		{
			_data = &(iter->second);
			vcLevel.push_back(_data->getIntData("avlLv"));
		}
		sort(vcLevel.begin(), vcLevel.end());

		// 取下一功能开放所需等级;
		bool bFound = false;
		for (unsigned int i = 0; i < vcLevel.size(); ++i)
		{
			if (vcLevel.at(i) > nCurLv)
			{
				nNextLv = vcLevel.at(i);
				bFound = true;
				break;
			}
		}

		if (bFound)
		{
			openNextFunc(nNextLv, nResId);
		}
	}
}

void LayerLevelUp::openNextFunc( int nNextLv, int nResId )
{
	m_ui.imgNext->setVisible(true);
	m_ui.txtNextLv = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Next_Lv"));
	m_ui.txtNextLv->setFontName(FONT_FZZHENGHEI);
	//m_ui.txtNextLv->enableOutline(Color4B(Color3B(0x6E, 0x20, 0x20)), 2);
	setTextAddStroke(m_ui.txtNextLv, Color3B(0x6E, 0x20, 0x20),2);
	ostringstream oss;
	oss << nNextLv;
	m_ui.txtNextLv->setString(oss.str());
	oss.str("");

	if (nResId != -1)
	{
		oss << "Image/Icon/Func/btn_" << nResId << "_3.png";
		m_ui.imgNextFunc = ImageView::create(oss.str());
		m_ui.imgNextFunc->setPosition(Vec2(137.0f, 310.0f));
		m_ui.imgNext->addChild(m_ui.imgNextFunc);
	}
}

void LayerLevelUp::onBtnClicked( Ref* ref, Widget::TouchEventType type, LVUP_JUMP_TYPE nJumpType )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (nJumpType)
		{
		case JUMP_TYPE_MAIN:		// 跳转到主城;
			{
				Director::getInstance()->popToRootScene(); //销毁所有界面
				Director::getInstance()->replaceScene(MainCityLoadingScene::create());
			}
			break;

		case JUMP_TYPE_INVALID:		// 关闭页面;
			{
				//this->scheduleUpdate();

				jumpScene();

			}
			break;
		default:
			break;
		}
	}
}

void LayerLevelUp::jumpScene()
{
	// 完成页面流转逻辑;
	//assert(m_layerLogicParent != nullptr);
	//m_layerLogicParent->jumpScene();

	// 跳转;
	if (_callback != nullptr)
	{
		_callback();
	}

	// 关闭页面;
	this->scheduleUpdate();
}
