#include "MainTaskLayer.h"
#include "UI/UIDef.h"
#include "MainTaskDef.h"
#include "MainTaskModel.h"
#include "Utils/ResourceUtils.h"
#include "MainTaskController.h"
#include "ToolModule/ToolModel.h"
#include "ToolModule/UIToolHeader.h"
#include "MainModule/MainCityScene.h"
#include "Temp/CustomPop.h"
#include "GuideCenter/GuideManager.h"
#include "Temp/PopReward.h"
#include "LevelUp/LevelUpController.h"


MainTaskLayer::MainTaskLayer()
	: mDailyBrightImg(nullptr)
	, mDailyGrayImg(nullptr)
	, mAchievementBrightImg(nullptr)
	, mAchievementGrayImg(nullptr)
	, mCloseBtn(nullptr)
	, mScrollPanel(nullptr)
	, mScrollBar(nullptr)
	, mTabTag(TAB_NONE)
	, m_bIsNeedListAct(true)
{

}

MainTaskLayer::~MainTaskLayer()
{

}

void MainTaskLayer::onEnter()
{
	ModalLayer::onEnter();

	MainTaskModel::getInstance()->addObserver(this);

	MainTaskController::getInstance()->sendGetTaskListMsg();

}

void MainTaskLayer::onExit()
{
	ModalLayer::onExit();

	MainTaskModel::getInstance()->removeObserver(this);
}

bool MainTaskLayer::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}

	initUI();
	return true;
}

void MainTaskLayer::initUI()
{
	auto root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UITask/UITask.ExportJson"));
	this->addChild(root);

	Node* parent = root->getChildByName("Image_bg");
	
	mDailyBrightImg = dynamic_cast<ImageView*>(parent->getChildByName("Image_richang"));
	mDailyGrayImg = dynamic_cast<ImageView*>(parent->getChildByName("Image_richang_2"));
	mDailyGrayImg->setTouchEnabled(true);
	mDailyGrayImg->addTouchEventListener(CC_CALLBACK_2(MainTaskLayer::onBtnClicked, this, DALIY));
	mDailyFlag = dynamic_cast<ImageView*>(parent->getChildByName("Image_richang_dian"));
	mDailyFlag->setVisible(false);

	//成就任务按钮
	mAchievementBrightImg = dynamic_cast<ImageView*>(parent->getChildByName("Image_chengjiu"));
	mAchievementGrayImg = dynamic_cast<ImageView*>(parent->getChildByName("Image_chengjiu_2"));
	mAchievementGrayImg->setTouchEnabled(true);
	mAchievementGrayImg->addTouchEventListener(CC_CALLBACK_2(MainTaskLayer::onBtnClicked, this, ACHIEVEMENT));
	mAchievementFlag = dynamic_cast<ImageView*>(parent->getChildByName("Image_chengjiu_dian"));
	mAchievementFlag->setVisible(false);

	//关闭按钮
	mCloseBtn = dynamic_cast<Button*>(parent->getChildByName("Button_close"));
	mCloseBtn->addTouchEventListener(CC_CALLBACK_2(MainTaskLayer::onBtnClicked, this, CLOSE));

	//滚动面板
	mScrollPanel = dynamic_cast<ScrollView*>(parent->getChildByName("ScrollView_gundong"));

	//滚动条
	mScrollBar = CustomScrollBar::create();
	mScrollBar->initScrollBar(mScrollPanel->getContentSize().height);
	mScrollBar->setPosition(mScrollPanel->getPosition() + Point(-12, 0));
	parent->addChild(mScrollBar);

	mScrollBar->setScrollBarLengthPercent(0.0f);
	mScrollBar->setScrollBarPercentY(0.0f);
	mScrollPanel->addEventListener(CC_CALLBACK_2(MainTaskLayer::onScrollViewEvent, this));

	//默认日常任务，如果日常任务未开启，则显示成就任务
	if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_DAILY_TASK, false))
	{
		switchTab(TAB_DALY, false);
	}
	else
	{
		switchTab(TAB_ACHIEVEMENT, false);
	}
}

void MainTaskLayer::updateTaskFlag()
{
	mDailyFlag->setVisible(MainTaskModel::getInstance()->hasActivedTask(TASK_TYPE_DAILY));
	mAchievementFlag->setVisible(MainTaskModel::getInstance()->hasActivedTask(TASK_TYPE_ACHIEVEMENT));
}

//初始化列表
void MainTaskLayer::initList()
{
	updateTaskFlag();

	mScrollPanel->removeAllChildren();

	TASK_TYPE taskType = (mTabTag == TAB_DALY) ? TASK_TYPE_DAILY : TASK_TYPE_ACHIEVEMENT;
	vector<MainTaskParam*> taskList = MainTaskModel::getInstance()->getTaskList(taskType);
	
	if (taskList.empty())
	{
		return;
	}

	int width = 709;
	int height = (150 + 10) * taskList.size() > mScrollPanel->getContentSize().height 
		? (150 + 10) * taskList.size() : mScrollPanel->getContentSize().height;
	mScrollPanel->setInnerContainerSize(Size(width, height));

	for (int i = 0; i < taskList.size() ; i++)
	{
		MainTaskParam* taskParam = taskList.at(i);

		MainTaskNode* taskNode = MainTaskNode::create("Image/UITask/btn_di_1.png", "Image/UITask/btn_di_2.png");
		taskNode->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);

		taskNode->setPosition(Point(0, mScrollPanel->getInnerContainerSize().height - (taskNode->getContentSize().height + 10) * (i + 1)));	

// 		if(!m_bIsNeedListAct)
// 		{
// 			//不需要滑动特效
// 			taskNode->setPosition(Point(0, mScrollPanel->getInnerContainerSize().height - (taskNode->getContentSize().height + 10) * (i + 1)));		
// 		}
// 		else
// 		{
// 			//列表滑动效果1
// 			taskNode->setPosition(Point(taskNode->getContentSize().width + (i)*taskNode->getContentSize().width*0.5f
// 				, mScrollPanel->getInnerContainerSize().height - (taskNode->getContentSize().height + 10) * (i + 1)));
// 			//滑动速度 = width/0.4f
// 			float v = taskNode->getContentSize().width/0.4f;
// 			float t = taskNode->getPositionX()/v;
// 			//Action* action = EaseBackOut::create(MoveTo::create(0.2f,Vec2(0, taskNode->getPositionY())));
// 			taskNode->runAction(CCSequence::create(CCMoveTo::create(t,Vec2(-100, taskNode->getPositionY())),
// 				CCMoveTo::create(0.1f, Vec2(0, taskNode->getPositionY())), nullptr));
// 		
// 			//列表滑动效果2
// 			/*
// 			taskNode->setPosition(Point((i+1)*(taskNode->getContentSize().width), mScrollPanel->getInnerContainerSize().height - 
// 				(taskNode->getContentSize().height + 10) * (i + 1)));
// 			taskNode->runAction(CCEaseSineOut::create(CCMoveTo::create(0.5f, Vec2(0, taskNode->getPositionY()))));
// 			*/
// 		}
		
		//Sprite* icon = Sprite::create(ResourceUtils::getTaskIconPath(taskParam->mTaskResId));	
		UIToolHeader* icon = UIToolHeader::create(ResourceUtils::getTaskIconPath(taskParam->mTaskResId));
		icon->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
		taskNode->mIconPanel->addChild(icon);
		taskNode->mTaskName->setString(taskParam->mTaskName);
		taskNode->mTaskDes->setString(taskParam->mTaskDes);
		taskNode->mTaskId = taskParam->mTaskId;
		taskNode->mTaskType = taskParam->mTaskCondType;

		//进度值标记
		if (taskParam->hasProgress())
		{
			taskNode->mTaskNum->setString("(" + TO_STR(taskParam->mTaskCondCurAmount) + "/" + TO_STR(taskParam->mTaskCondMaxAmount) + ")");
			taskNode->mTaskNum->setVisible(true);

			if (taskParam->isActivedTask())
			{
				taskNode->mTaskNum->setColor(Color3B(0x35, 0xf0, 0x2b));
			}
			else
			{
				taskNode->mTaskNum->setColor(Color3B(0xc3, 0xc3, 0xc3));
			}

		}
		else
		{
			taskNode->mTaskNum->setVisible(false);
		}

		//前往按钮
		bool showGotoBtn = taskParam->hasGoto() && !taskParam->isActivedTask();
		taskNode->mGotoBtn->setVisible(showGotoBtn);
		taskNode->mGotoBtn->addTouchEventListener(CC_CALLBACK_2(MainTaskLayer::onGotoClicked, this, taskParam->mJumpScene));

		//领取响应
		if (taskParam->isActivedTask())
		{
			taskNode->addTouchEventListener(CC_CALLBACK_2(MainTaskLayer::onReceiveClicked, this, taskNode->mTaskId));
		}

		//领奖励标记
		bool showReceiveReward = !taskParam->isGetPowerTask() && taskParam->isActivedTask();
		taskNode->mReceiveReward->setVisible(showReceiveReward);
		
		//领体力标记
		bool showReceivePowerFlag = taskParam->isGetPowerTask() && taskParam->isActivedTask();
		taskNode->mReceivePower->setVisible(showReceivePowerFlag);

		//时间未到标记
		bool showTimeNotFlag = taskParam->isGetPowerTask() && !taskParam->isActivedTask();
		taskNode->mTimeNot->setVisible(showTimeNotFlag);

		//添加道具
		addRewardInfo(taskNode, taskParam);
		
		mScrollPanel->addChild(taskNode);
		
	}

	//初始化完毕再设置滚动条比例
	mScrollBar->setScrollBarLengthPercent(
		mScrollPanel->getContentSize().height / mScrollPanel->getInnerContainerSize().height);
	mScrollBar->setScrollBarPercentY(0.0f);
	mScrollPanel->jumpToTop();

}

void MainTaskLayer::addRewardInfo(MainTaskNode* taskNode, MainTaskParam* taskParam)
{
	struct UIItemParam
	{
		Node* icon;
		int count;
	};

	Layout* rewardPanel = taskNode->mRewardPanel;

	vector<UIItemParam> vecItemParam;

	//体力
	if (taskParam->mPower > 0)
	{
		UIItemParam itemParam;
		itemParam.icon = Sprite::create("Image/Icon/global/img_tili.png");
		itemParam.count = taskParam->mPower;
		vecItemParam.push_back(itemParam);
	}

	//道具
	for (size_t i = 0; i < taskParam->mVecRewardInfo.size(); i++)
	{
		TaskRewardInfo& taskRewardInfo = taskParam->mVecRewardInfo.at(i);
		UIItemParam itemParam;
		UIToolHeader* icon = UIToolHeader::create(ToolModel::getInstance()->GetToolImgFilename(taskRewardInfo.mItemId));
		icon->checkChip(taskRewardInfo.mItemId);
		icon->setScale(0.35f);
		itemParam.icon = icon;
		itemParam.count = taskRewardInfo.mItemCount;
		vecItemParam.push_back(itemParam);
	}

	//金币
	if (taskParam->mGold > 0)
	{
		UIItemParam itemParam;
		itemParam.icon = Sprite::create("Image/Icon/global/img_yuanbao.png");
		itemParam.count = taskParam->mGold;
		vecItemParam.push_back(itemParam);
	}

	//银币
	if (taskParam->mCoin > 0)
	{
		UIItemParam itemParam;
		itemParam.icon = Sprite::create("Image/Icon/global/img_yinbi.png");
		itemParam.count = taskParam->mCoin;
		vecItemParam.push_back(itemParam);
	}

	//经验
	if (taskParam->mExp > 0)
	{
		UIItemParam itemParam;
		itemParam.icon = Sprite::create("Image/Icon/global/img_exp.png");
		itemParam.icon->setScale(0.7f);
		itemParam.count = taskParam->mExp;
		vecItemParam.push_back(itemParam);
	}

	int xx = 5;
	int yy = rewardPanel->getContentSize().height / 2;

	for (size_t i = 0; i < vecItemParam.size(); i++)
	{
		UIItemParam& itemParam = vecItemParam.at(i);
		itemParam.icon->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
		itemParam.icon->setPosition(Point(xx, yy));
		rewardPanel->addChild(itemParam.icon);

		xx += (itemParam.icon->getBoundingBox().size.width + 5);

		Text* countText = Text::create();
		countText->setString(StringFormat::formatNumber(itemParam.count, false));
		countText->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
		countText->setFontSize(22);
		countText->setColor(Color3B::WHITE);
		countText->setFontName(FONT_FZZHENGHEI);
		countText->setPosition(Point(xx, yy));
		rewardPanel->addChild(countText);

		xx += (countText->getContentSize().width + 30);
	}
}

void MainTaskLayer::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	if (pSender == mScrollPanel)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Size size = mScrollPanel->getInnerContainerSize();
				Vec2 vec = mScrollPanel->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - mScrollPanel->getContentSize().height);

				mScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			break;

		default:
			break;
		}
	}
}

void MainTaskLayer::onBtnClicked(Ref* target, Widget::TouchEventType eventType, BTN_TAG btnTag)
{
	if ( Widget::TouchEventType::BEGAN == eventType)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == eventType)
	{
		switch (btnTag)
		{
		case CLOSE:
			dispose();
			break;

		case DALIY:
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_DAILY_TASK))
			{
				switchTab(TAB_DALY);
			}
			break;

		case ACHIEVEMENT:
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_ACHIEVEMENT_TASK))
			{
				switchTab(TAB_ACHIEVEMENT);
			}
			break;

		default:
			break;
		}
	}
}

void MainTaskLayer::onGotoClicked(Ref* target, Widget::TouchEventType eventType, int jumpScene)
{
	if ( Widget::TouchEventType::BEGAN == eventType)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == eventType)
	{
		MainCityScene* mainCityScene = dynamic_cast<MainCityScene*>(this->getParent());

		switch (jumpScene)
		{
		case TASK_JUMP_SCENE_CHAPTER:
			mainCityScene->getBuildingLayer()->jumpToScene(MAIN_TAG::TAG_PVE);
			break;

		case TASK_JUMP_SCENE_HERO:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::HERO);
			break;

		case TASK_JUMP_SCENE_STAR_SU:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::STARSU);
			break;

		case TASK_JUMP_SCENE_PVP:
			mainCityScene->getBuildingLayer()->jumpToScene(MAIN_TAG::TAG_PVP);
			break;

		case TASK_JUMP_SCENE_LOVE:
			mainCityScene->getBuildingLayer()->jumpToScene(MAIN_TAG::TAG_LOVE);
			break;

		case TASK_JUMP_SCENE_TOWER:
			mainCityScene->getBuildingLayer()->jumpToScene(MAIN_TAG::TAG_TOWER);
			break;

		case TASK_JUMP_SCENE_MAZE:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::MAZE);
			break;

		case TASK_JUMP_SCENE_SHOP:
			mainCityScene->getBuildingLayer()->jumpToScene(MAIN_TAG::TAG_SEAL);
			break;

		case TASK_JUMP_SCENE_FRIEND:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::FRIEND);
			break;

		case TASK_JUMP_SCENE_ROB:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::ROB);
			break;

		case TASK_JUMP_SCENE_FORMATION2:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::FORMATION);
			break;

		case TASK_JUMP_SCENE_GET_COIN:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::ADDSILVER);
			break;

		case TASK_JUMP_SCENE_BOSS:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::BOSS);
			break;

		case TASK_JUMP_SCENE_STARS_CAMP:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::STARTPAN);
			break;

		case TASK_JUMP_SCENE_GROUP:
			mainCityScene->getBuildingLayer()->jumpToScene(MAIN_TAG::TAG_FACTION);
			break;

		case TASK_JUMP_SCENE_ARTIFACT:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::SHENQI);
			break;

		case TASK_JUMP_SCENE_GET_POWER:
			mainCityScene->getInfoLayer()->jumpToScene(MAIN_BTN_TAG::ADDPOWER);
			break;

		case TASK_JUMP_SCENE_ACTIVITY:
			mainCityScene->getBuildingLayer()->jumpToScene(MAIN_TAG::TAG_DAILY_PVE);
			break;

		default:
			break;
		}

		dispose();
	}
}

//领取按钮点击
void MainTaskLayer::onReceiveClicked(Ref* target, Widget::TouchEventType eventType, int taskId)
{
	if (Widget::TouchEventType::ENDED != eventType)
		return;

	MainTaskController::sendTaskReceiveMsg(taskId);
}

void MainTaskLayer::switchTab(TAB_TAG tabTag, bool isInitList)
{
	m_bIsNeedListAct = true;
	if (tabTag == TAB_DALY && mTabTag != TAB_DALY)
	{
		mDailyBrightImg->setVisible(true);
		mDailyGrayImg->setVisible(false);

		mAchievementBrightImg->setVisible(false);
		mAchievementGrayImg->setVisible(true);

		mTabTag = TAB_DALY;

		if (isInitList)
		{
			initList();
		}
	}
	
	if (tabTag == TAB_ACHIEVEMENT && mTabTag != TAB_ACHIEVEMENT)
	{
		mDailyBrightImg->setVisible(false);
		mDailyGrayImg->setVisible(true);

		mAchievementBrightImg->setVisible(true);
		mAchievementGrayImg->setVisible(false);

		mTabTag = TAB_ACHIEVEMENT;

		if (isInitList)
		{
			initList();
		}
	}
}

void MainTaskLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
	case TASK_EVENT_INIT:
		{
			initList();
			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_TASK);
		}
		break;
	case TASK_EVENT_UPDATE:
		initList();
		break;
	case TASK_EVENT_RECEIVE:
		{
			MainTaskParam* taskParam = (MainTaskParam*) observerParam->updateData;
			m_bIsNeedListAct = false;
			initList();
			//CustomPop::show("领取成功！");
			showPopReward(taskParam);

			//检测升级
			LevelUpController::getInstance()->checkLevelUp();
		}
		
	default:
		break;
	}
}

void MainTaskLayer::showPopReward(MainTaskParam* taskParam)
{
	vector<PopRewardItem*> vecRewardItem;

	//体力
	if (taskParam->mPower > 0)
	{
		PopRewardItem_Power* rewardItem = new PopRewardItem_Power(taskParam->mPower);
		vecRewardItem.push_back(rewardItem);
	}

	//道具
	for (size_t i = 0; i < taskParam->mVecRewardInfo.size(); i++)
	{
		TaskRewardInfo& taskRewardInfo = taskParam->mVecRewardInfo.at(i);
		PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(taskRewardInfo.mItemId, taskRewardInfo.mItemCount);
		vecRewardItem.push_back(rewardItem);
	}

	//金币
	if (taskParam->mGold > 0)
	{
		PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(taskParam->mGold);
		vecRewardItem.push_back(rewardItem);
	}

	//银币
	if (taskParam->mCoin > 0)
	{
		PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(taskParam->mCoin);
		vecRewardItem.push_back(rewardItem);
	}

	//经验
	if (taskParam->mExp > 0)
	{
		PopRewardItem_Exp* rewardItem = new PopRewardItem_Exp(taskParam->mExp);
		vecRewardItem.push_back(rewardItem);
	}

	PopReward::show(vecRewardItem);

}

MainTaskNode::MainTaskNode()
	: mTaskName(nullptr)
	, mTaskNum(nullptr)
	, mTaskDes(nullptr)
	, mReceivePower(nullptr)
	, mReceiveReward(nullptr)
	, mTimeNot(nullptr)
	, mGotoBtn(nullptr)
	, mIconPanel(nullptr)
	, mRewardPanel(nullptr)
	, mTaskId(-1)
	, mTaskType(-1)
{

}

MainTaskNode::~MainTaskNode()
{

}

MainTaskNode* MainTaskNode::create(const std::string& normalImage, const std::string& selectedImage)
{
	MainTaskNode* node = new MainTaskNode();
	if (node && node->init(normalImage, selectedImage))
	{
		node->autorelease();
		return node;
	}
	else
	{
		delete node;
		return nullptr;
	}
}

bool MainTaskNode::init(const std::string& normalImage, const std::string& selectedImage)
{
	if (!Button::init(normalImage, selectedImage))
	{
		return false;
	}

	initUI();
	return true;
}

void MainTaskNode::initUI()
{
	auto root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(
		"Image/UITask/UITask_info.ExportJson"));
	this->addChild(root);
	this->setContentSize(root->getContentSize());

	mTaskName = dynamic_cast<Text*>(root->getChildByName("Label_renwuming"));
	mTaskName->setFontName(FONT_FZZHENGHEI);
	mTaskNum = dynamic_cast<Text*>(root->getChildByName("Label_jindukuohao1"));
	mTaskNum->setFontName(FONT_FZZHENGHEI);
	mTaskDes = dynamic_cast<Text*>(root->getChildByName("Label_miaoshu"));
	mTaskDes->setFontName(FONT_FZZHENGHEI);

	mReceivePower = dynamic_cast<ImageView*>(root->getChildByName("Image_lingtili"));
	mReceiveReward = dynamic_cast<ImageView*>(root->getChildByName("Image_lingjiangli"));
	mTimeNot = dynamic_cast<ImageView*>(root->getChildByName("Image_shijianweidao"));
	mGotoBtn = dynamic_cast<Button*>(root->getChildByName("Button_qianwang"));

	mIconPanel = dynamic_cast<Layout*>(root->getChildByName("Panel_renwutubiao"));
	mRewardPanel = dynamic_cast<Layout*>(root->getChildByName("Panel_jiangli"));
}
