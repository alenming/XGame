#include "LayerCreateRole.h"
#include "UI/UIDef.h"
#include "Utils/ResourceUtils.h"
#include "Common/Common.h"
#include "MainModule/MainModel.h"
#include "Communication/Command.h"
#include "LoginController.h"
#include "Temp/CustomPop.h"
#include "HeroModule/HeroModel.h"
#include "DataManager/DataManager.h"

LayerCreateRole::LayerCreateRole()
	: mMaleSkeleton(nullptr)
	, mFemaleSkeleton(nullptr)
	, mSelectAni(nullptr)
	, mTextAni(nullptr)
	, mMaleNameIndex(0)
	, mFemaleNameIndex(0)
	, mIsSelectMale(false)
	, mMaleActionIndex(0)
	, mFemaleActionIndex(0)
{
	MainModel::getInstance()->addObserver(this);
}

LayerCreateRole::~LayerCreateRole()
{
	MainModel::getInstance()->removeObserver(this);
}

Scene* LayerCreateRole::createScene()
{
	Scene* scene = Scene::create();
	LayerCreateRole* layer = LayerCreateRole::create();
	scene->addChild(layer);
	return scene;
}

bool LayerCreateRole::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();
	return true;
}

void LayerCreateRole::onEnter()
{
	Layer::onEnter();
}

void LayerCreateRole::onExit()
{
	Layer::onExit();
}

void LayerCreateRole::initUI()
{
	Sprite* bg = Sprite::create("Image/Bg/img_chuangjian_bg.jpg");
	bg->setPosition(this->getContentSize() / 2);
	this->addChild(bg);

	mUI.root = dynamic_cast<Layout*>(
		cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UICreat/UICreat.ExportJson"));
	this->addChild(mUI.root);

	Layout* parent = dynamic_cast<Layout*>(mUI.root->getChildByName("Pnl_main"));
	mUI.parent = parent;

	//主角按钮
	mUI.heroMaleBtn = dynamic_cast<Button*>(parent->getChildByName("Button_male"));
	mUI.heroMaleBtn->addTouchEventListener(CC_CALLBACK_2(LayerCreateRole::onMaleHeroSelect, this));

	mUI.heroFemaleBtn = dynamic_cast<Button*>(parent->getChildByName("Button_female"));
	mUI.heroFemaleBtn->addTouchEventListener(CC_CALLBACK_2(LayerCreateRole::onFemaleHeroSelect, this));

	//角色形象
	mUI.heroImg = dynamic_cast<ImageView*>(parent->getChildByName("Img_yuanhua"));
	mUI.tvImg = dynamic_cast<ImageView*>(parent->getChildByName("Img_tv"));
	mUI.rolePanel = dynamic_cast<Layout*>(mUI.tvImg->getChildByName("Pnl_hero"));
	mUI.rolePanel->setClippingEnabled(true);
	//mUI.rolePanel->addTouchEventListener(CC_CALLBACK_2(LayerCreateRole::onChangeAction, this));

	//角色信息
	ImageView* infoParent = dynamic_cast<ImageView*>(parent->getChildByName("Img_intr"));
	ImageView* startImg1 = dynamic_cast<ImageView*>(infoParent->getChildByName("Img_star1"));
	ImageView* startImg2 = dynamic_cast<ImageView*>(infoParent->getChildByName("Img_star2"));
	ImageView* startImg3 = dynamic_cast<ImageView*>(infoParent->getChildByName("Img_star3"));
	mUI.vecHeroStar.push_back(startImg1);
	mUI.vecHeroStar.push_back(startImg2);
	mUI.vecHeroStar.push_back(startImg3);

	ImageView* infoImg1 = dynamic_cast<ImageView*>(startImg1->getChildByName("Img_1st"));
	ImageView* infoImg2 = dynamic_cast<ImageView*>(startImg2->getChildByName("Img_2nd"));
	ImageView* infoImg3 = dynamic_cast<ImageView*>(startImg3->getChildByName("Img_3rd"));
	mUI.vecHeroInfo.push_back(infoImg1);
	mUI.vecHeroInfo.push_back(infoImg2);
	mUI.vecHeroInfo.push_back(infoImg3);

	//输入框
	mUI.nameTextField = dynamic_cast<TextField*>(Helper::seekWidgetByName(parent, "TextField_name"));
	mUI.nameEditBoxEx = EditBoxEx::create(mUI.nameTextField);
	mUI.nameEditBoxEx->setFontName(SystemUtils::getFontFileName().c_str());
	mUI.nameEditBoxEx->setMaxLength(mUI.nameTextField->getMaxLength());
	mUI.nameTextField->getParent()->addChild(mUI.nameEditBoxEx);
	
	//按钮响应
	mUI.changeBtn = dynamic_cast<Button*>(parent->getChildByName("Btn_roll"));
	mUI.changeBtn->addTouchEventListener(CC_CALLBACK_2(LayerCreateRole::onBtnChangeName, this));
	mUI.startBtn = dynamic_cast<Button*>(parent->getChildByName("Btn_go"));
	mUI.startBtn->addTouchEventListener(CC_CALLBACK_2(LayerCreateRole::onBtnStart, this));

	//初始化名字数据
	mMaleNameList = LoginController::getInstance()->getNameParam()->vecMaleNames;
	mFemaleNameList = LoginController::getInstance()->getNameParam()->vecFemaleNames;

	initAnimation();
	initSkeleton();
	initActionList();
	selectMaleHero(false);
}

void LayerCreateRole::initAnimation()
{
	//花瓣动画
	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/AniPiao/AniPiao0.png",
		"Image/AniPiao/AniPiao0.plist",
		"Image/AniPiao/AniPiao.ExportJson");
	auto leavesAni = Armature::create("AniPiao");
	leavesAni->setPosition(mUI.parent->getContentSize() / 2);
	leavesAni->getAnimation()->playByIndex(0);
	mUI.parent->addChild(leavesAni);

	//选择动画
	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/AniChoice/AniChoice0.png",
		"Image/AniChoice/AniChoice0.plist",
		"Image/AniChoice/AniChoice.ExportJson");
	mSelectAni = Armature::create("AniChoice");
	mSelectAni->setPosition(mUI.parent->getContentSize() / 2);
	mSelectAni->getAnimation()->playByIndex(0);
	mSelectAni->setVisible(false);
	mUI.parent->addChild(mSelectAni);

	//开始按钮动画
	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/AniGotoPlay/AniGotoPlay0.png",
		"Image/AniGotoPlay/AniGotoPlay0.plist",
		"Image/AniGotoPlay/AniGotoPlay.ExportJson");
	auto startAni = Armature::create("AniGotoPlay");
	startAni->setPosition(mUI.parent->getContentSize() / 2);
	startAni->getAnimation()->playByIndex(0);
	mUI.parent->addChild(startAni, 10);

	//文字动画
	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/AniJHCS/AniJHCS0.png",
		"Image/AniJHCS/AniJHCS0.plist",
		"Image/AniJHCS/AniJHCS.ExportJson");
	mTextAni = Armature::create("AniJHCS");
	mTextAni->setPosition(mUI.parent->getContentSize() / 2 - Size(5, 0));
	mTextAni->setVisible(false);
	mUI.parent->addChild(mTextAni, 10);
}

void LayerCreateRole::initActionList()
{
	mVecMaleAction.push_back("wait");
	mVecMaleAction.push_back("atk");
	mVecMaleAction.push_back("atks3");
	mVecMaleAction.push_back("manuSkl2");
	mVecMaleAction.push_back("victory");

	mVecFemaleAction.push_back("wait");
	mVecFemaleAction.push_back("atk");
	mVecFemaleAction.push_back("atks");
	mVecFemaleAction.push_back("manuSkl");
	mVecFemaleAction.push_back("victory");
}

void LayerCreateRole::playSelectAni()
{
	mSelectAni->setVisible(true);
	mSelectAni->getAnimation()->playByIndex(0);

	mTextAni->setVisible(true);
	mTextAni->getAnimation()->playByIndex(0);

	for (size_t i = 0; i < mUI.vecHeroStar.size(); i++)
	{
		ImageView* item = mUI.vecHeroInfo.at(i);
		Point originP = item->getPosition();
		item->setPosition(originP + Point(450, 0));
		auto action = EaseBackInOut::create(MoveTo::create(0.25f + 0.08f*i, originP));
		item->runAction(action);
	}
}

void LayerCreateRole::initSkeleton()
{

	mMaleSkeleton = SpineAnimation::createFighterWithResId(HERO_MALE_RES_ID);
	//mMaleSkeleton->setAnimation(0, "wait", true);
	mMaleSkeleton->setAnimation(0, "wait", true);
	mMaleSkeleton->setPosition(Point(mUI.rolePanel->getContentSize().width / 2.0f, 0));
	mUI.rolePanel->addChild(mMaleSkeleton);

	mFemaleSkeleton = SpineAnimation::createFighterWithResId(HERO_FEMALE_TEMPLET_ID);
	mFemaleSkeleton->setAnimation(0, "wait", true);
	mFemaleSkeleton->setPosition(Point(mUI.rolePanel->getContentSize().width / 2.0f, 0));
	mUI.rolePanel->addChild(mFemaleSkeleton);
}


void LayerCreateRole::selectMaleHero(bool playAni)
{
	if (mIsSelectMale)
	{
		return;
	}

	mIsSelectMale = true;
	mUI.heroMaleBtn->setBright(false);
	mUI.heroFemaleBtn->setBright(true);
	mUI.heroImg->loadTexture("Image/UICreat/MainHero/male.png");
	for (size_t i = 0; i < mUI.vecHeroInfo.size(); i++)
	{
		string path = "Image/UICreat/MainHero/male_info" + TO_STR(i + 1) + ".png";
		mUI.vecHeroInfo.at(i)->loadTexture(path);
	}
	mMaleSkeleton->setVisible(true);
	mFemaleSkeleton->setVisible(false);

	if (playAni)
	{
		playSelectAni();
	}

	setRoleName();

	//mMaleActionIndex = 0;
	//setSkeletonAction();
}

void LayerCreateRole::selectFemaleHero(bool playAni)
{
	if (!mIsSelectMale)
	{
		return;
	}

	mIsSelectMale = false;
	mUI.heroMaleBtn->setBright(true);
	mUI.heroFemaleBtn->setBright(false);
	mUI.heroImg->loadTexture("Image/UICreat/MainHero/female.png");
	for (size_t i = 0; i < mUI.vecHeroInfo.size(); i++)
	{
		string path = "Image/UICreat/MainHero/female_info" + TO_STR(i + 1) + ".png";
		mUI.vecHeroInfo.at(i)->loadTexture(path);
	}
	mMaleSkeleton->setVisible(false);
	mFemaleSkeleton->setVisible(true);

	if (playAni)
	{
		playSelectAni();
	}

	setRoleName();

	//mFemaleActionIndex = 0;
	//setSkeletonAction();
}

void LayerCreateRole::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	
	if (observerParam->id == nMAIN_CMD_RANDOM_NAMES)
	{
		NameParam* data = (NameParam*) observerParam->updateData;
	
		if (!data->vecMaleNames.empty())
		{
			mMaleNameList = data->vecMaleNames;
		}
		if (!data->vecFemaleNames.empty())
		{
			mFemaleNameList = data->vecFemaleNames;
		}

		setRoleName();
	}
}

void LayerCreateRole::setRoleName()
{
	if (mIsSelectMale)
	{
		if (mMaleNameList.empty())
		{
			return;
		}

		if (mMaleNameIndex < mMaleNameList.size())
		{
			mUI.nameEditBoxEx->setText(mMaleNameList.at(mMaleNameIndex).c_str());
			mMaleNameIndex++;
		}
		else
		{
			mMaleNameList.clear();
			mMaleNameIndex = 0;
			LoginController::getInstance()->sendGetRandomNameMsg(true);
		}
	}
	else
	{
		if (mFemaleNameList.empty())
		{
			return;
		}

		if (mFemaleNameIndex < mFemaleNameList.size())
		{
			mUI.nameEditBoxEx->setText(mFemaleNameList.at(mFemaleNameIndex).c_str());
			mFemaleNameIndex++;
		}
		else
		{
			mFemaleNameList.clear();
			mFemaleNameIndex = 0;
			LoginController::getInstance()->sendGetRandomNameMsg(false);
		}
	}
}

void LayerCreateRole::setSkeletonAction()
{
	if (mIsSelectMale)
	{
		if (mMaleActionIndex < mVecMaleAction.size())
		{
			mMaleSkeleton->setAnimation(0, mVecMaleAction.at(mMaleActionIndex), true);
			mMaleActionIndex++;
		}
		else
		{
			mMaleActionIndex = 0;
		}
	}
	else
	{
		if (mFemaleActionIndex < mVecFemaleAction.size())
		{
			mFemaleSkeleton->setAnimation(0, mVecFemaleAction.at(mFemaleActionIndex), true);
			mFemaleActionIndex++;
		}
		else
		{
			mFemaleActionIndex = 0;
		}
	}
}

void LayerCreateRole::onMaleHeroSelect( Ref* target, Widget::TouchEventType eventType)
{
	if ( Widget::TouchEventType::BEGAN == eventType)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == eventType)
	{
		selectMaleHero();
	}
}

void LayerCreateRole::onFemaleHeroSelect(Ref* target, Widget::TouchEventType eventType)
{
	if ( Widget::TouchEventType::BEGAN == eventType)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == eventType)
	{
		//CustomPop::show("该角色暂未开放");
		selectFemaleHero();
	}
}

void LayerCreateRole::onBtnStart( Ref* target, Widget::TouchEventType eventType )
{
	if ( Widget::TouchEventType::BEGAN == eventType)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == eventType)
	{
		if(StringFormat::getStringLenth(mUI.nameEditBoxEx->getText()) > NICK_NAME_LENGTH_LIMIT)
		{
			//CustomPop::show("昵称太长，请重新输入");
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10252));
		}
		else
		{
			int templetId = mIsSelectMale ? HERO_MALE_TEMPLET_ID : HERO_FEMALE_TEMPLET_ID;
			LoginController::getInstance()->createRole(mUI.nameEditBoxEx->getText(), templetId);
		}
	}
}

void LayerCreateRole::onBtnChangeName( Ref* target, Widget::TouchEventType eventType )
{
	if ( Widget::TouchEventType::BEGAN == eventType)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == eventType)
	{
		setRoleName();
	}
}

void LayerCreateRole::onChangeAction(Ref* target, Widget::TouchEventType eventType)
{
	if ( Widget::TouchEventType::BEGAN == eventType)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == eventType)
	{
		setSkeletonAction();
	}
}
