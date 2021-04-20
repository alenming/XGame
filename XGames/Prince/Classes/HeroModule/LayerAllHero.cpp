#include "LayerAllHero.h"
#include "MainModule/MainCityScene.h"
#include "LayerAllHero.h"
#include "LayerHeroInfo.h"
#include "Communication/Command.h"
#include "LayerHero.h"
#include "SceneHero.h"
#include "GuideCenter/GuideManager.h"
#include "Widget/LayerGetCard.h"


LayerAllHero::LayerAllHero()
	: mCurrentCard(nullptr)
	, mHeroShowType(999)
	, mHeroShowJob(999)
	, mIsFirstEnter(true)
	, mIsCardClicked(false)
{

}

LayerAllHero::~LayerAllHero()
{
	mVecHeroCard.clear();
}

bool LayerAllHero::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void LayerAllHero::onEnter()
{
	Layer::onEnter();

	HeroModel::getInstance()->addObserver(this);

	mIsCardClicked = false;
	mIsCardAniEnd = true;

	//非首次进入该界面需要刷新UI
	if (mIsFirstEnter)
	{
		mIsCardAniEnd = false;
		showCardAnimation();
	}
	else
	{
 		updateUI();

		//继续检测是否加载完卡牌
		if(mCurCreateNum < mVecCardTempId.size())
		{
			scheduleUpdate();
		}
 
 		//隔一帧再执行引导，防止崩溃
 		auto callBack = [this](float dt)->void
 		{
 			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_HERO_BAG);
 			this->getScheduler()->unschedule("LayerAllHero::onEnter", this);
 		};
 		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerAllHero::onEnter");
	}
}

void LayerAllHero::onExit()
{
	Layer::onExit();

	HeroModel::getInstance()->removeObserver(this);
}

void LayerAllHero::showCardAnimation()
{
	for (int i = 0; i < ENTER_CARD_COUNT ; i++)
	{
		auto card = mVecEnterCard.at(i);
		card->getCardRoot()->setOpacity(0);

		Action* act = nullptr;
		if(i == ENTER_CARD_COUNT-1)
		{
			auto callBack = [this](){ 
				mIsCardAniEnd = true;
				GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_HERO_BAG);	
			};

			act = Sequence::create(EaseSineOut::create(FadeIn::create(0.35)), CallFunc::create(callBack), nullptr);
		}
		else
		{
			act = EaseSineOut::create(FadeIn::create(0.35));
		}

		card->getCardRoot()->runAction(act);
	}
}

void LayerAllHero::initUI()
{
	auto root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UINewhero/UINewhero_main.ExportJson"));
	root->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(root);

	Size size = Director::getInstance()->getVisibleSize();
	root->setSize(size);
	Point pos = Director::getInstance()->getVisibleOrigin();
	root->setPosition(pos);
	mUI.mRoot = root;

	mUI.mScrollBarPanel = dynamic_cast<Layout*>(root->getChildByName("Panel_gundongtiao"));
	mUI.mTabPanel = dynamic_cast<Layout*>(root->getChildByName("Panel_fenye"));
	mUI.mHeroScrollView = dynamic_cast<ScrollView*>(root->getChildByName("SV_yingxiong"));
	Size scrollViewSize = Size(size.width - mUI.mScrollBarPanel->getContentSize().width - 
		mUI.mTabPanel->getContentSize().width, mUI.mHeroScrollView->getContentSize().height);
	mUI.mHeroScrollView->setContentSize(scrollViewSize);
	mUI.mHeroScrollView->getInnerContainer()->setContentSize(scrollViewSize);
	mUI.mHeroScrollView->addEventListener(CC_CALLBACK_2(LayerAllHero::onScrollViewEvent, this));
	
	mUI.mCloseBtn = dynamic_cast<Button*>(mUI.mTabPanel->getChildByName("Button_guanbi"));
	mUI.mCloseBtn->addTouchEventListener(CC_CALLBACK_2(LayerAllHero::onBtnClicked, this, HERO_BTN_ID::BACK));

	mUI.mTabAllHero = dynamic_cast<CheckBox*>(mUI.mTabPanel->getChildByName("CB_quanbu"));
	mUI.mTabAtkHero = dynamic_cast<CheckBox*>(mUI.mTabPanel->getChildByName("CB_gongji"));
	mUI.mTabDefHero = dynamic_cast<CheckBox*>(mUI.mTabPanel->getChildByName("CB_fangyu"));
	mUI.mTabHelpHero = dynamic_cast<CheckBox*>(mUI.mTabPanel->getChildByName("CB_fuzhu"));
	mUI.mTabFirstHero = dynamic_cast<CheckBox*>(mUI.mTabPanel->getChildByName("CB_xianshou"));

	mUI.mClickAllHero = dynamic_cast<Layout*>(mUI.mTabPanel->getChildByName("Panel_quanbu"));
	mUI.mClickAllHero->addTouchEventListener(CC_CALLBACK_2(LayerAllHero::onBtnClicked, this, HERO_BTN_ID::TAB_ALL_HERO));
	mUI.mClickAtkHero = dynamic_cast<Layout*>(mUI.mTabPanel->getChildByName("Panel_gongji"));
	mUI.mClickAtkHero->addTouchEventListener(CC_CALLBACK_2(LayerAllHero::onBtnClicked, this, HERO_BTN_ID::TAB_ATK_HERO));
	mUI.mClickDefHero = dynamic_cast<Layout*>(mUI.mTabPanel->getChildByName("Panel_fangyu"));
	mUI.mClickDefHero->addTouchEventListener(CC_CALLBACK_2(LayerAllHero::onBtnClicked, this, HERO_BTN_ID::TAB_DEF_HERO));
	mUI.mClickHelpHero = dynamic_cast<Layout*>(mUI.mTabPanel->getChildByName("Panel_fuzhu"));
	mUI.mClickHelpHero->addTouchEventListener(CC_CALLBACK_2(LayerAllHero::onBtnClicked, this, HERO_BTN_ID::TAB_HELP_HERO));
	mUI.mClickFirstHero = dynamic_cast<Layout*>(mUI.mTabPanel->getChildByName("Panel_xianshou"));
	mUI.mClickFirstHero->addTouchEventListener(CC_CALLBACK_2(LayerAllHero::onBtnClicked, this, HERO_BTN_ID::TAB_FIRST_HERO));

	mUI.mScrollBar = CustomScrollBar::create();
	mUI.mScrollBar->setAnchorPoint(Point::ANCHOR_MIDDLE);
	mUI.mScrollBar->initScrollBar(mUI.mHeroScrollView->getContentSize().height - 30);
	mUI.mScrollBar->setPosition(mUI.mScrollBarPanel->getContentSize() / 2);
	mUI.mScrollBarPanel->addChild(mUI.mScrollBar);

	initHeroList();
	switchTab(-1);

}

void LayerAllHero::initHeroList()
{
	multimap<int, RowData>& heroMap = DataManager::getInstance()->getDTCharacter()->getTableData();

	mVecHeroCard.clear();
	mVecCardTempId.clear();
	mCurCreateNum = 0;

	//创建卡牌时区分男女主角 男主:41099,  女主:41098
	int heroSex = HeroModel::getInstance()->getMainHero()->mSex;

	for (auto iter = heroMap.begin(); iter != heroMap.end(); iter++)
	{
		int heroId = iter->first;
		//如果游戏当前是男主角，则不创建女主卡牌; 若是女主，则不创建男主卡牌
		if((heroSex == HERO_SEX_MALE && heroId == HERO_FEMALE_TEMPLET_ID) || 
			(heroSex == HERO_SEX_FEMALE && heroId == HERO_MALE_TEMPLET_ID))
		{
			continue;
		}
		mVecCardTempId.push_back(iter->first);
	}

	//对id排序
	sortTempId();

	//初始加载8个
	for(int i=0; i<8 && mCurCreateNum < mVecCardTempId.size(); i++)
	{
		int templateId = mVecCardTempId.at(mCurCreateNum);

		Layout* root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
			"Image/UICommon/Card_hero.ExportJson"));
		UIHeroCard* heroCard = UIHeroCard::create(templateId, dynamic_cast<Layout*>(root->clone()));

		if (heroCard)
		{
			auto listener = EventListenerTouchOneByOne::create();
			listener->onTouchBegan = CC_CALLBACK_2(LayerAllHero::onTouchBegan, this, heroCard);
			listener->onTouchMoved = CC_CALLBACK_2(LayerAllHero::onTouchMoved, this);
			listener->onTouchEnded = CC_CALLBACK_2(LayerAllHero::onTouchEnded, this);
			listener->onTouchCancelled = CC_CALLBACK_2(LayerAllHero::onTouchCancelled, this);
			getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, heroCard);
			mVecHeroCard.pushBack(heroCard);
		}

		mCurCreateNum++;
	}

	//开启定时器
	scheduleUpdate();
}

void LayerAllHero::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;

	if (obParam)
	{
		switch (obParam->id)
		{
		case nMAIN_CMD_TOOL_SYNTHESIS:
			{
				updateUI();

				int heroTempId = *((int*)obParam->updateData);
				if(heroTempId != 0)
				{
					LayerGetCard* layer = LayerGetCard::create(heroTempId, false, 0);
					this->addChild(layer, 999);
				}	
			}
			break;
		default:
			break;
		}
	}	
}

void LayerAllHero::updateUI()
{
	//更新卡牌
	for (UIHeroCard* card : mVecHeroCard)
	{
		if (card)
		{
			card->updateUI();
		}
	}
	//刷新列表
	showHeroList(mHeroShowJob);
}

void LayerAllHero::onBtnClicked(Ref* ref, Widget::TouchEventType type, HERO_BTN_ID btnId)
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
			Director::getInstance()->replaceScene(MainCityScene::create());
			break;
		case HERO_BTN_ID::TAB_ALL_HERO:
			switchTab(-1);
			break;
		case HERO_BTN_ID::TAB_ATK_HERO:
			switchTab(HERO_ATK);
			break;
		case HERO_BTN_ID::TAB_DEF_HERO:
			switchTab(HERO_DEFENSE);
			break;
		case HERO_BTN_ID::TAB_HELP_HERO:
			switchTab(HERO_HELP);
			break;
		case HERO_BTN_ID::TAB_FIRST_HERO:
			switchTab(HERO_FIRST);
			break;
		default:
			break;
		}
	}
}

void LayerAllHero::switchTab(int heroJob)
{
	//初次切换标签需要等待卡牌动画播放完
	if(heroJob != -1 && !mIsCardAniEnd)
	{
		return;
	}

	if (mHeroShowJob == heroJob)
	{
		return;
	}

	mHeroShowJob = heroJob;
	switch (heroJob)
	{
	case -1:
		{
			mUI.mTabAllHero->setSelectedState(true);
			mUI.mTabAtkHero->setSelectedState(false);
			mUI.mTabDefHero->setSelectedState(false);
			mUI.mTabHelpHero->setSelectedState(false);
			mUI.mTabFirstHero->setSelectedState(false);
			showHeroList(mHeroShowJob);
		}
		break;
	case HERO_ATK:
		{
			mUI.mTabAllHero->setSelectedState(false);
			mUI.mTabAtkHero->setSelectedState(true);
			mUI.mTabDefHero->setSelectedState(false);
			mUI.mTabHelpHero->setSelectedState(false);
			mUI.mTabFirstHero->setSelectedState(false);
			showHeroList(mHeroShowJob);
		}
		break;
	case HERO_DEFENSE:
		{
			mUI.mTabAllHero->setSelectedState(false);
			mUI.mTabAtkHero->setSelectedState(false);
			mUI.mTabDefHero->setSelectedState(true);
			mUI.mTabHelpHero->setSelectedState(false);
			mUI.mTabFirstHero->setSelectedState(false);
			showHeroList(mHeroShowJob);
		}
		break;
	case HERO_HELP:
		{
			mUI.mTabAllHero->setSelectedState(false);
			mUI.mTabAtkHero->setSelectedState(false);
			mUI.mTabDefHero->setSelectedState(false);
			mUI.mTabHelpHero->setSelectedState(true);
			mUI.mTabFirstHero->setSelectedState(false);
			showHeroList(mHeroShowJob);
		}
		break;
	case HERO_FIRST:
		{
			mUI.mTabAllHero->setSelectedState(false);
			mUI.mTabAtkHero->setSelectedState(false);
			mUI.mTabDefHero->setSelectedState(false);
			mUI.mTabHelpHero->setSelectedState(false);
			mUI.mTabFirstHero->setSelectedState(true);
			showHeroList(mHeroShowJob);
		}
		break;
	default:
		break;
	}

	//初始化完毕再设置滚动条比例
	mUI.mScrollBar->setScrollBarLengthPercent(
		mUI.mHeroScrollView->getContentSize().height / mUI.mHeroScrollView->getInnerContainerSize().height);
	mUI.mScrollBar->setScrollBarPercentY(0.0f);
	mUI.mHeroScrollView->jumpToTop();
}

void LayerAllHero::showHeroList(int heroJob)
{
	mUI.mHeroScrollView->getInnerContainer()->removeAllChildren();

	vector<UIHeroCard*> vecHero;
	vecHero.clear();

	for (UIHeroCard* heroCard : mVecHeroCard)
	{
		if (heroJob < 0)
		{
			vecHero.push_back(heroCard);
		}
		else
		{
			if (heroCard->getHeroJobType() == heroJob)
			{
				vecHero.push_back(heroCard);
			}
		}
	}

	if (!vecHero.empty())
	{
		//卡牌尺寸
		Size cardSize = vecHero.at(0)->getContentSize();

		//滚动面板内容最小尺寸
		Size innerSize = mUI.mHeroScrollView->getContentSize();

		//计算间距大小
		int paddingX = (innerSize.width - cardSize.width * 4) / 5;
		int paddingY = (innerSize.height - cardSize.height * 2) / 3;

		//设置滚动内容尺寸
		int heroCount = getHeroCountByJob(heroJob);

		int height = (heroCount / 4 + 2) * paddingY + (heroCount / 4 + 1) * cardSize.height;
		height = height > innerSize.height ? height : innerSize.height;
		mUI.mHeroScrollView->getInnerContainer()->setContentSize(Size(innerSize.width, height));

		mVecEnterCard.clear();
		for (int i = 0; i < heroCount; i++)
		{
			int innerHeight = mUI.mHeroScrollView->getInnerContainerSize().height;

			if(i < vecHero.size())
			{
				UIHeroCard* heroCard = vecHero.at(i);
				int xx = (i % 4 + 1) * paddingX + (i % 4) * cardSize.width;
				int yy = innerHeight - (i / 4 + 1) * paddingY - (i / 4) * cardSize.height;
				heroCard->setPosition(Point(xx + cardSize.width / 2, yy - cardSize.height / 2));
				heroCard->setAnchorPoint(Point::ANCHOR_MIDDLE);
				mUI.mHeroScrollView->getInnerContainer()->addChild(heroCard);
				if (i < ENTER_CARD_COUNT)
				{
					mVecEnterCard.push_back(heroCard);
				}
			}
		}
	}
}

void LayerAllHero::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	if (pSender == mUI.mHeroScrollView)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Size size = mUI.mHeroScrollView->getInnerContainerSize();
				Vec2 vec = mUI.mHeroScrollView->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - mUI.mHeroScrollView->getContentSize().height);

				mUI.mScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			break;

		default:
			break;
		}
	}
}

//触摸事件
bool LayerAllHero::onTouchBegan(Touch* touch, Event* event, UIHeroCard* heroCard)
{
	if(mIsCardAniEnd)
	{
		Rect rect = Rect(0, 0, heroCard->getContentSize().width, heroCard->getContentSize().height);
		if (rect.containsPoint(heroCard->convertTouchToNodeSpace(touch)))
		{
			mIsCardClicked = true;
			mCurrentCard = heroCard;
			mCurrentCard->setScale(0.95f);
			
			return true;
		}
	}
	
	return false;
}

void LayerAllHero::onTouchMoved(Touch* touch, Event* event)
{
	if (touch->getStartLocation().distance(touch->getLocation()) > 30)
	{
		if (mCurrentCard)
		{
			mCurrentCard->setScale(1.0f);
			mCurrentCard = nullptr;
		}
	}
}

void LayerAllHero::onTouchEnded(Touch* touch, Event* event)
{
	if (mCurrentCard)
	{
		SoundUtils::playSoundEffect("dianji");

		//隔一帧再执行点击事件，防止重复创建
		auto callBack = [this](float dt)->void
		{
			
			if (!mCurrentCard->getHeroParam())
			{
				if (mCurrentCard->isRecruitable())
				{
					//可以招募的英雄直接发送招募消息
					RowData* heroRowData = DataManager::getInstance()->searchToolById(mCurrentCard->getTemplateId());
					if (heroRowData)
					{
						int debrisId = heroRowData->getIntData("transNeed");
						const TOOL_INFO* toolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(debrisId);
						if (toolInfo)
						{
							HeroRecruitController::sendRecruitMsg(toolInfo->nid);
						}
					}
				}
				else
				{
					//不能招募的英雄进入详情页
					LayerHeroInfo* layer = LayerHeroInfo::create(mCurrentCard->getTemplateId());
					layer->show(this);
				}
			}
			else
			{
				unscheduleUpdate();
				mIsFirstEnter = false;
				//已存在的英雄进入阵容界面
				HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(mCurrentCard->getTemplateId());
				Director::getInstance()->pushScene(SceneHero::create(heroParam->mHeroId));
			}
			mCurrentCard->setScale(1.0f);
			mCurrentCard = nullptr;

			this->getScheduler()->unschedule("LayerAllHero::onTouchEnd", this);
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerAllHero::onTouchEnd");
	}
}

void LayerAllHero::onTouchCancelled(Touch* touch, Event* event)
{
	if (mCurrentCard)
	{
		mCurrentCard->setScale(1.0f);
		mCurrentCard = nullptr;
	}
	
}

void LayerAllHero::update(float dt)
{
	//越界判定
	if(mCurCreateNum >= mVecCardTempId.size())
	{
		if (mIsFirstEnter && mIsCardAniEnd)
		{
			mIsFirstEnter = false;

			showHeroList(mHeroShowJob);

			unscheduleUpdate();
		}

		return;
	}

	//创建卡牌时区分男女主角 男主:41099,  女主:41098
	int heroSex = HeroModel::getInstance()->getMainHero()->mSex;

	for(int i=0; i<1 && mCurCreateNum < mVecCardTempId.size(); i++)
	{
		int templateId = mVecCardTempId.at(mCurCreateNum);

		Layout* root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
			"Image/UICommon/Card_hero.ExportJson"));
		UIHeroCard* heroCard = UIHeroCard::create(templateId, root);

		if (heroCard)
		{
			auto listener = EventListenerTouchOneByOne::create();
			listener->onTouchBegan = CC_CALLBACK_2(LayerAllHero::onTouchBegan, this, heroCard);
			listener->onTouchMoved = CC_CALLBACK_2(LayerAllHero::onTouchMoved, this);
			listener->onTouchEnded = CC_CALLBACK_2(LayerAllHero::onTouchEnded, this);
			listener->onTouchCancelled = CC_CALLBACK_2(LayerAllHero::onTouchCancelled, this);
			getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, heroCard);
			mVecHeroCard.pushBack(heroCard);
		}

		mCurCreateNum++;
	}

	if(mIsCardAniEnd)
	{
		showHeroList(mHeroShowJob);
	}
}

void LayerAllHero::sortTempId()
{
	//对heroId进行排序
	if(!mVecCardTempId.empty())
	{
		std::sort(mVecCardTempId.begin(), mVecCardTempId.end(),
			[this](int iId1, int iId2)->bool
		{
			//1. 可招募英雄放在首位
			if(isRecruitableById(iId1) && !isRecruitableById(iId2))
			{
				return true;
			}
			else if(!isRecruitableById(iId1) && isRecruitableById(iId2))
			{
				return false;
			}
			else if(isRecruitableById(iId1) && isRecruitableById(iId2))
			{
				return iId1 > iId2;
			}

			//2. 主角放前面
			if(iId1 == HERO_FEMALE_TEMPLET_ID || iId1 == HERO_MALE_TEMPLET_ID)
			{
				return true;
			}
			if(iId2 == HERO_FEMALE_TEMPLET_ID || iId2 == HERO_MALE_TEMPLET_ID)
			{
				return false;
			}

			//3. 已有的英雄放前面
			HeroParam* heroParam1 = HeroModel::getInstance()->searchHeroByTemplateId(iId1);
			HeroParam* heroParam2 = HeroModel::getInstance()->searchHeroByTemplateId(iId2);
			if(heroParam1 && !heroParam2)
			{
				return true;
			}
			else if(!heroParam1 && heroParam2)
			{
				return false;
			}
			else if(heroParam1 && heroParam2)
			{
				//在阵位上的放前面
				if(heroParam1->isOnBattle() && !heroParam2->isOnBattle())
				{
					return true;
				}
				else if(!heroParam1->isOnBattle() && heroParam2->isOnBattle())
				{
					return false;
				}
				else
				{
					//都在阵位上，按战斗力排序
					return heroParam1->mFighting > heroParam2->mFighting;
				}
			}
			else
			{
				//没有的英雄，按碎片数量排序
				return getDebrisNumById(iId1) > getDebrisNumById(iId2);
			}
		});
	}
}

//获取指定id英雄碎片数量
int LayerAllHero::getDebrisNumById(int tempId)
{
	return ToolModel::getInstance()->searchHeroDebrisNum(tempId);
}

//获取指定id英雄是否可招募
bool LayerAllHero::isRecruitableById(int tempId)
{
	//先查询是否已拥有该英雄
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(tempId);
	if(heroParam == nullptr)
	{
		int mDebrisNum = getDebrisNumById(tempId);
		int mDebrisMaxNum = ToolModel::getInstance()->searchHeroDebrisMaxNum(tempId);

		return mDebrisNum > 0 && mDebrisMaxNum > 0 && mDebrisNum >= mDebrisMaxNum;
	}

	return false;
}

int LayerAllHero::getHeroCountByJob(int heroJob)
{
	int heroCount = 0;

	if(heroJob == -1)
	{
		heroCount = mVecCardTempId.size();
	}
	else
	{
		for(auto iter = mVecCardTempId.begin(); iter != mVecCardTempId.end(); iter++)
		{
			RowData* heroAttrData = DataManager::getInstance()->searchCharacterById(*iter);
			if (heroAttrData)
			{
				if(heroAttrData->getIntData("jobType") == heroJob)
				{
					heroCount++;
				}
			}
		}
	}

	return heroCount;
}
