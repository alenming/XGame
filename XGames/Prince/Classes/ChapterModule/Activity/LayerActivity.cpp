#include "LayerActivity.h"
#include "ActivityController.h"
#include "ActivityModel.h"
#include "MainModule/MainModel.h"
#include "FightModule/FightController.h"
#include "FightModule/SceneFightLoading.h"
#include "Widget/LayerCommHeadInfo.h"
#include "GuideCenter/GuideManager.h"
#include "VIPModule/VipModel.h"
#include "Temp/CustomGoldTips.h"
#include "ActivityBarrierNode.h"
#include "Widget/ActionCreator.h"
#include "FormationModule/SceneFormation.h"

LayerActivity::LayerActivity(void)
	: m_nCurSelActivityId(-1)
	, m_nCurSelActivityType(ACTIVITY_TYPE::ACT_TYPE_CHIP_JiangHuChuanWen)
	, m_nCurSelActivityBarrierId(-1)
	, m_bIsFirstEnter(true)
	, mIsPushScene(false)
	, m_bIsEnableFight(true)
{
	ActivityModel::getInstance()->setCurActivityType(m_nCurSelActivityType);
	m_vcUIActivitys.clear();
	ActivityModel::getInstance()->addObserver(this);
}


LayerActivity::~LayerActivity(void)
{
	ActivityModel::getInstance()->removeObserver(this);
	this->unscheduleUpdate();
}

LayerActivity* LayerActivity::create(bool pushScene /*= false*/)
{
	LayerActivity *pRet = new LayerActivity();
	if (pRet && pRet->init(pushScene))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool LayerActivity::init(bool pushScene)
{
	if (!Layer::init())
	{
		return false;
	}

	mIsPushScene = pushScene;

	initUI();
	ActivityController::getInstance()->handleActivityState();

	return true;
}

void LayerActivity::onEnter()
{
	Layer::onEnter();

	m_bIsEnableFight = true;

	if (m_bIsFirstEnter)
	{
		m_bIsFirstEnter = false;
	}
	SoundUtils::playSoundEffect("quanping");
}

void LayerActivity::onExit()
{
	// 释放资源;
	ResourceLoader::getInstance()->removeUIResource("UIActivity");

	Layer::onExit();
}


void LayerActivity::initUI()
{
	Size size = Director::getInstance()->getWinSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIActivity/UIMain.ExportJson"));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_ui.pRoot);

	// 通用标题栏;
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(
		[this](Ref* sender, Widget::TouchEventType type)->void
	{
		this->onBtnClicked(sender, type, Btn_Back);
	},
		HEAD_INFO_TYPE_ACTIVITY_LEVEL);
	m_ui.pRoot->addChild(_layerCommHeadInfo, 9);

	// 活动面板;
	m_ui.pnlActivity = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Activity"));
	
	// 选择难度面板;
	m_ui.pnlDifficulty = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Difficulty"));
	m_ui.pnlDifficulty->setLocalZOrder(_layerCommHeadInfo->getLocalZOrder()+1);
	m_ui.pnlDifficulty->setVisible(false);
	m_ui.imgDifficultyBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Bg_1"));
	m_ui.txtCostPower = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Number"));
	m_ui.txtCostPower->setFontName(FONT_FZZHENGHEI);
	m_ui.txtCostPower->setString("");
	m_ui.pnlContent = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Content"));
	m_ui.btnDifficultyClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Close"));
	m_ui.btnDifficultyClose->addTouchEventListener([=](Ref* ref, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			m_ui.pnlDifficulty->setVisible(false);
		}
	});

	// 特权提升相关;
	initVipUI_1();
	initVipUI_2();
}

void LayerActivity::initVipUI_1()
{
	m_ui.pVipStateRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UiNewbuy/UIVip_State.ExportJson"));
	this->addChild(m_ui.pVipStateRoot, 3);
	m_ui.pVipStateRoot->setVisible(false);
	m_ui.btnVipStateOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_OK"));
	m_ui.btnVipStateCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_Cancel"));
	m_ui.btnVipStateClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_Close"));

	m_ui.txtVipStateTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi1"));
	m_ui.txtVipStateTips1->setFontName(FONT_FZZHENGHEI);
	//string str("是否增加1次挑战机会？");
	string str(DataManager::getInstance()->searchCommonTexdtById(10042));
	m_ui.txtVipStateTips1->setString(str);
	m_ui.txtVipStateTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi2"));
	m_ui.txtVipStateTips2->setFontName(FONT_FZZHENGHEI);
	str.clear();
	//str.assign("本次购买消耗");
	str.assign(DataManager::getInstance()->searchCommonTexdtById(10043));
	m_ui.txtVipStateTips2->setString(str);
	m_ui.txtVipStateTips3 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi4"));
	m_ui.txtVipStateTips3->setFontName(FONT_FZZHENGHEI);
	str.clear();
	//str.assign("今日可购买次数:");
	str.assign(DataManager::getInstance()->searchCommonTexdtById(10044));
	m_ui.txtVipStateTips3->setString(str);

	// 购买消耗;
	m_ui.imgCurVip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Img_Vip"));
	m_ui.imgCurVip->loadTexture(getVipIcon(VipModel::getInstance()->getCurVipLv()));
	m_ui.txtCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Lab_Cost"));
	m_ui.txtCost->setFontName(FONT_FZZHENGHEI);

	// 剩余次数;
	m_ui.txtCurCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Lab_Left_Times"));
	m_ui.txtCurCount->setFontName(FONT_FZZHENGHEI);

	m_ui.btnVipStateOK->addTouchEventListener(CC_CALLBACK_2(LayerActivity::onBtnClicked, this, Btn_Vip_State_OK));
	m_ui.btnVipStateCancel->addTouchEventListener(CC_CALLBACK_2(LayerActivity::onBtnClicked, this, Btn_Vip_State_Cancel));
	m_ui.btnVipStateClose->addTouchEventListener(CC_CALLBACK_2(LayerActivity::onBtnClicked, this, Btn_Vip_State_Close));
}

void LayerActivity::initVipUI_2()
{
	m_ui.pVipRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UiNewbuy/UiVipUp.ExportJson"));
	this->addChild(m_ui.pVipRoot, 3);
	m_ui.pVipRoot->setVisible(false);
	m_ui.btnVipOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_OK)));
	m_ui.btnVipCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_Cancel)));
	m_ui.btnVipClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_Close)));

	m_ui.imgVipBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Img_di"));
	m_ui.txtVipTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Tips1"));
	m_ui.txtVipTips1->setFontName(FONT_FZZHENGHEI);
	//string str("该关卡购买挑战次数已用完");
	string str(DataManager::getInstance()->searchCommonTexdtById(10045));
	m_ui.txtVipTips1->setString(str);
	m_ui.txtVipTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Tips2"));
	m_ui.txtVipTips2->setFontName(FONT_FZZHENGHEI);
	m_ui.imgVipNext = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Img_Vip"));
	m_ui.txtNextCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Count"));
	m_ui.txtNextCount->setFontName(FONT_FZZHENGHEI);

	m_ui.btnVipOK->addTouchEventListener(CC_CALLBACK_2(LayerActivity::onBtnClicked, this, Btn_Vip_OK));
	m_ui.btnVipCancel->addTouchEventListener(CC_CALLBACK_2(LayerActivity::onBtnClicked, this, Btn_Vip_Cancel));
	m_ui.btnVipClose->addTouchEventListener(CC_CALLBACK_2(LayerActivity::onBtnClicked, this, Btn_Vip_Close));
}

void LayerActivity::onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName)
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switch (nWidgetName)
		{
		case Btn_Back:
			{
				this->scheduleUpdate();
			}
			break;

		case Btn_Vip_State_OK:
			{
				// 购买挑战次数;
				ActivityController::getInstance()->handleBuyActivityTimes(m_nCurSelActivityId);
				m_ui.pVipStateRoot->setVisible(false);
			}
			break;

		case Btn_Vip_State_Cancel:
		case Btn_Vip_State_Close:
			{
				m_ui.pVipStateRoot->setVisible(false);
			}
			break;

		case Btn_Vip_OK:
			{
				Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
			}
			break;

		case Btn_Vip_Cancel:
		case Btn_Vip_Close:
			{
				m_ui.pVipRoot->setVisible(false);
			}
			break;

		default:
			break;
		}
	}
}


void LayerActivity::update(float dt)
{
	this->unscheduleUpdate();

	if (mIsPushScene)
	{
		Director::getInstance()->popScene();
	}
	else
	{
		Director::getInstance()->replaceScene(MainCityScene::create());
	}
}


void LayerActivity::updateSelf(void* data)
{
	if (nullptr == data)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) data;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_ACTIVITY_LIST:
		{
			updateActivityList((vector<UI_ACTIVITY_NODE_PARAM>*)(param->updateData));
		}
		break;

	case OBS_PARAM_TYPE_ACTIVITY_NODE:
		{
			updateActivityNode((ActivityState*)(param->updateData));
		}
		break;

	case OBS_PARAM_TYPE_ACTIVITY_BUY:
		{
			if (m_nChallengeType == 0)
			{
				onChallenge(m_nCurSelActivityBarrierId);
			}
			else if (m_nChallengeType == 1)
			{
				onSweep(m_nCurSelActivityBarrierId);
			}
		}
		break;

	case OBS_PARAM_TYPE_ACTIVITY_SWEEP:
		{
			map<int, int>* mapRewards = (map<int, int>*)(param->updateData);
			// 弹出奖励内容;
			vector<PopRewardItem*> vecRewardItem;
			for(auto iter = mapRewards->begin(); iter != mapRewards->end(); ++iter)
			{
				PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(iter->first, iter->second);
				vecRewardItem.push_back(rewardItem);
			}
			PopReward::show(vecRewardItem);
		}
		break;

	default:
		break;
	}
}


void LayerActivity::updateActivityList(vector<UI_ACTIVITY_NODE_PARAM>* vcActivityInfo)
{
	if (vcActivityInfo->size() == 0)
	{
		// pop
		//CustomTips::show("当前没有活动");
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10046));
		return;
	}

	// 清除当前所有;
	for (auto it : m_vcUIActivitys)
	{
		it->removeFromParent();
	}
	m_vcUIActivitys.clear();

	// 更新活动列表;
	for (unsigned int i = 0; i < vcActivityInfo->size(); i++)
	{
		ActivityNode* _node = ActivityNode::create(this, vcActivityInfo->at(i));
		if (nullptr != _node)
		{
			_node->setPosition(Vec2(100.0f * i, -20.0f));
			m_vcUIActivitys.push_back(_node);
			m_ui.pnlActivity->addChild(_node);
		}
	}

	// 添加动画;
	if (m_vcUIActivitys.size() >= 2)
	{
		float fSpace = min((1080.0f - m_vcUIActivitys.at(0)->getContentSize().width)/(m_vcUIActivitys.size()-1), m_vcUIActivitys.at(0)->getContentSize().width);
		ActionCreator::runCommCardFlow(m_vcUIActivitys, fSpace, m_vcUIActivitys.at(0)->getContentSize().width, CC_CALLBACK_3(LayerActivity::onActivityClicked, this));
	}
}

void LayerActivity::updateActivityNode(ActivityState* state)
{
	auto iter = find_if(m_vcUIActivitys.begin(), m_vcUIActivitys.end(), [=](Node* node){
		ActivityNode* activityNode = dynamic_cast<ActivityNode*>(node);
		if (activityNode != nullptr)
		{
			if (*activityNode == state->nActivityId)
			{
				activityNode->updateState(state);
				return true;
			}
		}
		return false;
	});
}

void LayerActivity::onActivityClicked(Node* node, Widget::TouchEventType type, int nIndex)
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		for (int i = 0; i < m_vcUIActivitys.size(); ++i)
		{
			dynamic_cast<ActivityNode*>(m_vcUIActivitys.at(i))->setSelected(i == nIndex);
			if (i == nIndex)
			{
				m_nCurSelActivityId = dynamic_cast<ActivityNode*>(m_vcUIActivitys.at(i))->getActivityId();
				m_nCurSelActivityType = dynamic_cast<ActivityNode*>(m_vcUIActivitys.at(i))->getActivityType();
				ActivityModel::getInstance()->setCurActivityType(m_nCurSelActivityType);
			}
		}
	}
}

void LayerActivity::onActivityEnter( int nActivityId )
{
	// 清理旧数据;
	m_ui.pnlDifficulty->setVisible(true);
	m_ui.pnlContent->removeAllChildren();

	// 音效;
	SoundUtils::playSoundEffect("tanchuang");

	// 动画并加载新数据;
	ActionCreator::runCommDlgAction(m_ui.imgDifficultyBg, [=](){
		// 更新关卡数据;
		vector<ActivityBarrierInfo*>  vcBarrierInfo;
		ActivityModel::getInstance()->getActivityBarrierByActivityId(nActivityId, vcBarrierInfo);
		for (int i = 0; i < vcBarrierInfo.size(); ++i)
		{
			CC_BREAK_IF(i >= 3);
			ActivityBarrierNode* node = ActivityBarrierNode::create(this, m_nCurSelActivityId, m_nCurSelActivityType, vcBarrierInfo.at(i), i+1);
			node->setPosition(Vec2(node->getContentSize().width * i, 0.0f));
			m_ui.pnlContent->addChild(node);
		}
		if (vcBarrierInfo.size() > 0)
		{
			m_ui.txtCostPower->setString(_TO_STR(-vcBarrierInfo.at(0)->nCost));
			setTextAddStroke(m_ui.txtCostPower, Color3B(0x59, 0x0, 0x0), 2);
		}
	});
}

void LayerActivity::onChallenge(int nActivityBarrierId)
{
	if (!m_bIsEnableFight)
		return;

	m_nChallengeType = 0;
	if (preChallenge(nActivityBarrierId))
	{
		// 防止多次进入;
		m_bIsEnableFight = false;

		// 战前阵型;
		Director::getInstance()->pushScene(SceneFormation::create(F_MODE_ACTIVITY));
	}
}

bool LayerActivity::preChallenge(int nActivityBarrierId)
{
	// 记录选中的Id;
	m_nCurSelActivityBarrierId = nActivityBarrierId;
	ActivityModel::getInstance()->setCurActivityBarrierId(m_nCurSelActivityId, m_nCurSelActivityBarrierId);

	int nRet = ActivityController::getInstance()->handleActivityChallenge(m_nCurSelActivityId, m_nCurSelActivityBarrierId);
	if (nRet != 0)
	{
		switch (nRet)
		{
		case -1:
			{
				LayerCommonBuy *buyPower = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
				Director::getInstance()->getRunningScene()->addChild(buyPower);
			}
			break;

		case -2:
			{
				checkBuyCondition();
			}
			break;

		default:
			//CustomTips::show("查询出错，请重新登录");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10047));
			break;
		}
	}
	else
	{
		return true;
	}
	return false;
}

void LayerActivity::onSweep(int nActivityBarrierId)
{
	m_nChallengeType = 1;
	if (preChallenge(nActivityBarrierId))
	{
		ActivityController::getInstance()->handleActivityBarrierSweep(m_nCurSelActivityId, m_nCurSelActivityBarrierId);
	}
}

///////////////////////////////////////////////////////////////////////

std::string LayerActivity::getVipIcon(int nVipLv)
{
	string strPath = "Image/UIVip/VipLv/img_vip_";
	strPath.append(_TO_STR(nVipLv));
	strPath.append(".png");
	return strPath;
}

void LayerActivity::checkBuyCondition()
{
	// 1.当前剩余次数;
	if (ActivityModel::getInstance()->getCurVipLeftTimes(m_nCurSelActivityId) > 0)
	{
		ostringstream oss;
		oss << ActivityModel::getInstance()->getCurVipLeftTimes(m_nCurSelActivityId) << "/" << ActivityModel::getInstance()->getCurMaxTimes();
		m_ui.txtCurCount->setString(oss.str());
		m_ui.txtCost->setString(_TO_STR(ActivityModel::getInstance()->getBuyActivityCost(m_nCurSelActivityId)));
		m_ui.pVipStateRoot->setVisible(true);
		return;
	}

	// 2.当前可提升次数;
	int nNextValue = 0;
	int nNextLv = ActivityModel::getInstance()->getNextValidVip(nNextValue);
	if (nNextValue > ActivityModel::getInstance()->getCurMaxTimes())
	{
		// 弹出特权提升面板;
		ostringstream oss;
		oss << DataManager::getInstance()->searchCommonTexdtById(10048) << nNextValue << DataManager::getInstance()->searchCommonTexdtById(10049);
		m_ui.txtNextCount->setString(oss.str());
		oss.str("");
		m_ui.imgVipNext->loadTexture(getVipIcon(nNextLv));
		m_ui.pVipRoot->setVisible(true);
		SoundUtils::playSoundEffect("tanchuang");
		ActionCreator::runCommDlgAction(m_ui.imgVipBg);
		return;
	}

	// 不可再购买;
	//CustomTips::show("今日购买次数不足");
	CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10050));
}
