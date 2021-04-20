#include "LayerHeroLevel.h"
#include "ToolModule/ToolModel.h"
#include "Communication/Command.h"
#include "HeroController.h"
#include "HeroModel.h"
#include "Temp/CustomTips.h"
#include "Widget/ActionCreator.h"
#include "GuideCenter/GuideManager.h"


LayerHeroLevel::LayerHeroLevel()
	: mCurIcon(nullptr)
	, mUseNum(0)
	, mMaxNum(0)
	, mHeroLevel(0)
	, mCurExp(0)
	, mExpAdd(10)
	, isLongPressed(false)
{
	HeroModel::getInstance()->addObserver(this);
}

LayerHeroLevel::~LayerHeroLevel()
{
	HeroModel::getInstance()->removeObserver(this);
}

LayerHeroLevel* LayerHeroLevel::create(HeroParam* heroParam)
{
	LayerHeroLevel* layer = new LayerHeroLevel();
	if (layer && layer->init(heroParam))
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

bool LayerHeroLevel::init( HeroParam* heroParam )
{
	if (!ModalLayer::init())
	{
		return false;
	}

	this->mHeroParam = heroParam;
	this->setOpacity(0);
	this->setClickEmptyDispose(true);

	mUI.mRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIArray/UILevelUp.ExportJson"));
	this->addChild(mUI.mRoot);

	mUI.mLevelUpPnl = dynamic_cast<ImageView*>(mUI.mRoot->getChildByName("Image_di"));
	mUI.mCurLvlAndMaxLvl = dynamic_cast<Text*>(mUI.mLevelUpPnl->getChildByName("Label_dengji"));
	mUI.mCurLvlAndMaxLvl->setFontName(FONT_FZZHENGHEI);
	Text* levelTips = dynamic_cast<Text*>(mUI.mLevelUpPnl->getChildByName("Label_tishi"));
	levelTips->setFontName(FONT_FZZHENGHEI);
	ImageView* expBarBack = dynamic_cast<ImageView*>(mUI.mLevelUpPnl->getChildByName("Image_jindutiao"));
	mUI.mExpBar = dynamic_cast<LoadingBar*>(expBarBack->getChildByName("ProgressBar_jindutiao"));
	mUI.mCurExpText = dynamic_cast<TextAtlas*>(expBarBack->getChildByName("AtlasLabel_shuzi_1"));
	mUI.mMaxExpText = dynamic_cast<TextAtlas*>(expBarBack->getChildByName("AtlasLabel_shuzi_2"));
	int expItemId[] = {10011, 10012, 10013, 10014};
	for (int i = 0; i < EXP_ITEM_COUNT; i++)
	{
		int templetId = expItemId[i];
		Layout* headerPnl = dynamic_cast<Layout*>(mUI.mLevelUpPnl->getChildByName("Panel_daju_" + TO_STR(i + 1)));
		UIToolHeader* icon = UIToolHeader::create(templetId);
		mUI.mExpHeader[i] = icon;
		icon->setTouchEnabled(true);
		icon->setPosition(headerPnl->getContentSize() / 2);
		icon->addTouchEventListener(CC_CALLBACK_2(LayerHeroLevel::onExpItemClicked, this, templetId));
		icon->setScale(0.88f);
		icon->setToolId(templetId);
		headerPnl->addChild(icon);
	}

	updateUI();

	return true;
}

void LayerHeroLevel::updateUI()
{
	for (int i = 0; i < EXP_ITEM_COUNT; i++)
	{
		UIToolHeader* icon = mUI.mExpHeader[i];
		const TOOL_INFO* toolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(icon->getToolId());
		int curNum = toolInfo != nullptr ? toolInfo->nstack : 0;
		icon->setNumEx(curNum);
	}

	mUI.mCurLvlAndMaxLvl->setString(
		TO_STR(mHeroParam->mLevel) + "/" + TO_STR(HeroModel::getInstance()->getMainHeroLevel()));
	mUI.mCurExpText->setString(TO_STR(mHeroParam->mExp));
	mUI.mMaxExpText->setString(TO_STR(mHeroParam->mExpMax));
	mUI.mExpBar->setPercent(100 * (mHeroParam->mExp / (float)mHeroParam->mExpMax));
}

void LayerHeroLevel::onEnter()
{
	ModalLayer::onEnter();

	auto callBack = [](){ GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_HERO_LEVEL_UP); };
	ActionCreator::runCommDlgAction(mUI.mLevelUpPnl, callBack);
}

void LayerHeroLevel::onExit()
{
	ModalLayer::onExit();
}

void LayerHeroLevel::onExpItemClicked(Ref* ref, Widget::TouchEventType type, int itemId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
		//当前道具信息
		isLongPressed = false;

		mCurIcon = (UIToolHeader*) ref;
		const TOOL_INFO* toolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(mCurIcon->getToolId());
		RowData* toolData = DataManager::getInstance()->searchToolById(mCurIcon->getToolId());
		mMaxNum = toolInfo != nullptr ? toolInfo->nstack : 0;
		mHeroLevel = mHeroParam->mLevel;
		mCurExp = mHeroParam->mExp;
		mExpAdd = toolData->getIntData("value");
		mUseNum = 0;

		// 启动定时器，判定长按;
		this->schedule(schedule_selector(LayerHeroLevel::judgeLongPressed), 0.5f);
	}
	else if (type == Widget::TouchEventType::MOVED)
	{
		// 移动只解除长按判定;
		//this->unschedule(schedule_selector(LayerHeroLevel::judgeLongPressed));
	}
	else
	{
		// 解除长按和长按判定;
		this->unschedule(schedule_selector(LayerHeroLevel::judgeLongPressed));
		this->unschedule(schedule_selector(LayerHeroLevel::onLongPressed));

		if (isLongPressed)
		{
			//此处发送协议
			if (mUseNum > 0)
			{
				HeroController::sendUseExpItemMsg(mHeroParam->mHeroId, mCurIcon->getToolId(), mUseNum);
			}
		}
		else
		{
			int mainHeroLevel = HeroModel::getInstance()->getMainHeroLevel();
			RowData* expData = DataManager::getInstance()->searchHeroExp(mHeroLevel);
			int maxExp = expData->getIntData("heroExp");
			if (mHeroLevel == mainHeroLevel && mCurExp + 1 == maxExp)
			{
				//CustomTips::show("不能超过主角等级");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10241));
			}
			else if (mMaxNum <= 0)
			{
				//CustomTips::show("经验道具不足");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10242));
			}
			else
			{
				HeroController::sendUseExpItemMsg(mHeroParam->mHeroId, mCurIcon->getToolId());
			}
		}

		isLongPressed = false;
	}
}

void LayerHeroLevel::judgeLongPressed( float delta )
{
	isLongPressed = true;
	this->unschedule(schedule_selector(LayerHeroLevel::judgeLongPressed));
	this->schedule(schedule_selector(LayerHeroLevel::onLongPressed), 0.1f, kRepeatForever, 0.0f);
}

void LayerHeroLevel::onLongPressed( float delta )
{
	if (mUseNum >= mMaxNum)
	{
		//提示道具不足
		//CustomTips::show("经验道具不足");
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10242));
		this->unschedule(schedule_selector(LayerHeroLevel::onLongPressed));
		return;
	}
	else
	{
		int mainHeroLevel = HeroModel::getInstance()->getMainHeroLevel();
		RowData* expData = DataManager::getInstance()->searchHeroExp(mHeroLevel);
		int maxExp = expData->getIntData("heroExp");
		//判定当前是否达到上限(等级、经验)
		if (mHeroLevel == mainHeroLevel && mCurExp + 1 == maxExp )
		{
			//CustomTips::show("不能超过主角等级！");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10241));
			this->unschedule(schedule_selector(LayerHeroLevel::onLongPressed));
			return;
		}
		else
		{
			//递归计算经验值
			calExp(mCurExp + mExpAdd, maxExp);
			mUseNum++;
			RowData* expData = DataManager::getInstance()->searchHeroExp(mHeroLevel);
			maxExp = expData->getIntData("heroExp");
			//更新UI
			mCurIcon->setNumEx(mMaxNum - mUseNum);
			mUI.mCurLvlAndMaxLvl->setString(
				TO_STR(mHeroLevel) + "/" + TO_STR(mainHeroLevel));
			mUI.mCurExpText->setString(TO_STR(mCurExp));
			mUI.mMaxExpText->setString(TO_STR(maxExp));
			mUI.mExpBar->setPercent(100 * (mCurExp / (float)maxExp));
		}
	}
}

void LayerHeroLevel::calExp(int curExp, int maxExp)
{
	//判定是否升级
	if (curExp >= maxExp)
	{
		//判定是否达到最大等级
		if (mHeroLevel >= HeroModel::getInstance()->getMainHeroLevel())
		{
			curExp = maxExp - 1;
			mCurExp = curExp;
		}
		else
		{
			mHeroLevel++;
			curExp = curExp - maxExp;
			RowData* expData = DataManager::getInstance()->searchHeroExp(mHeroLevel);
			int nextMaxExp = expData->getIntData("heroExp");
			calExp(curExp, nextMaxExp);
		}
	}
	else
	{
		mCurExp = curExp;
	}
}

void LayerHeroLevel::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;
	if (obParam->id == nMAIN_CMD_TRAINING_ADD_EXP)
	{
		updateUI();
	}
}
