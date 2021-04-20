#include "LayerBarrier.h"
#include "../FightModule/FightController.h"
#include "MainModule/MainModel.h"
#include "FormationModule/LayerFormation.h"
#include "ChapterController.h"
#include "ChapterModel.h"
#include "BarrierModel.h"
#include "LayerFrame.h"
#include "Sweep/LayerSweep.h"
#include "FightModule/SceneFightLoading.h"
#include "GuideCenter/GuideManager.h"
#include "Temp/PopReward.h"
#include "Widget/ActionCreator.h"
#include "LoveModule/LoveModel.h"
#include "VIPModule/VipScene.h"
#include "VIPModule/VipModel.h"
#include "Temp/CustomGoldTips.h"
#include "SceneChapter.h"
#include "DoubleBonus/DoubleBonusModel.h"
#include "MysteryShop/MysteryInterLayer.h"
#include "MysteryShop/MysteryShopModel.h"

LayerBarrier::LayerBarrier()
	: m_nCurSelBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
	, m_nCurSelLv(BARRIER_LEVEL_0)
	, m_nConsume(0)
	, m_nStarCount(0)
	, armatureFight(nullptr)
	, armatureNewLv(nullptr)
	, m_bFirstEnter(true)
	, m_bIsEnterFighting(true)
	, m_bEnableFightBtn(true)
{
	ChapterController::getInstance()->setBarrierView(this);
	ChapterModel::getInstance()->addObserver(this);
	BarrierModel::getInstance()->addObserver(this);
	DoubleBonusModel::getInstance()->addObserver(this);

	for (int i = 0; i < 3; ++i)
	{
		armatureFront[i] = nullptr;
		armatureBack[i] = nullptr;
	}
}

LayerBarrier::~LayerBarrier()
{
	//ChapterController::getInstance()->setBarrierView(nullptr);
	ChapterModel::getInstance()->removeObserver(this);
	BarrierModel::getInstance()->removeObserver(this);
	DoubleBonusModel::getInstance()->removeObserver(this);
}


bool LayerBarrier::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();

	// 1. 请求关卡信息;
	if (!ChapterController::getInstance()->handleChapterDetail())
	{
		return false;
	}

	return true;
}

void LayerBarrier::onEnter()
{
	Layer::onEnter();

	// 隐藏前景层;
	LayerFrame* frame = static_cast<LayerFrame*>(this->getParent());
	if (nullptr != frame)
	{
		frame->setFrameEnabled(false);
	}

	m_bEnableFightBtn = true;

	if (m_bFirstEnter)
	{
		m_bFirstEnter = false;

		// 音效;
		SoundUtils::playSoundEffect("quanping");

		// 进场特效;
		playEnterAnimation();
	}
	else
	{
		fixBarrierPosition(m_nCurSelBarrierId);
	}

	GuideManager::getInstance()->startGuideNextFrame(GUIDE_INTERFACE_STAGE);

	//CCLOG(Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());

}

void LayerBarrier::onExit()
{
	// 恢复前景层;
	LayerFrame* frame = static_cast<LayerFrame*>(this->getParent());
	if (nullptr != frame)
	{
		frame->setFrameEnabled(true);
	}

	// 继续播放音乐;
	if (!m_bIsEnterFighting)
	{
		SoundUtils::playSoundMusic("main_1");
	}

	// 释放资源;
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIBarrier");

	Layer::onExit();
}

void LayerBarrier::update( float dt )
{
	this->unscheduleUpdate();
	this->removeFromParent();
}


void LayerBarrier::initUI()
{
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 加载剧情关卡UI;
	string strFileName;
	strFileName.assign("Image/UIBarrier/UIBarrier.ExportJson");

	///// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

	///// 关卡面板;
	m_ui.pnlBarrier = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName(STRING(Pnl_Barrier)));
	m_ui.pnlBarrier->setContentSize(visibleSize);

	// 背景;
	ImageView* imgBg = ImageView::create("Image/Bg/img_barrier_bg.jpg");
	imgBg->setPosition(Vec2(m_ui.pnlBarrier->getContentSize().width / 2, m_ui.pnlBarrier->getContentSize().height / 2));
	this->addChild(imgBg);

	// 回退按钮;
	m_ui.btnBack = dynamic_cast<Button*>(m_ui.pnlBarrier->getChildByName(STRING(Btn_Back)));
	m_ui.btnBack->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Back));

	// 关卡列表;
	m_ui.scrollviewBarrierList = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(ScrollView_BarrierList)));
	m_ui.scrollviewBarrierList->setContentSize(Size(visibleSize.width - 440.0f, m_ui.scrollviewBarrierList->getContentSize().height));

	// 关卡信息展示;
	m_ui.imgBarrierInfoBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_BarrierInfo_Bg"));
	m_ui.txtBarrierName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Barrier_Name)));
	m_ui.txtBarrierName->setFontName(FONT_NAME_FOUNDER_BOLD);
	m_ui.txtBarrierDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Barrier_Desc)));
	m_ui.txtBarrierDesc->setFontName(FONT_NAME_FOUNDER_BOLD);
	m_ui.imgDropLine = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Img_Drop_Line)));
	m_ui.txtCoin = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Coin)));
	m_ui.txtCoin->setFontName(FONT_NAME_FOUNDER_BOLD);
	m_ui.txtExp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Exp)));
	m_ui.txtExp->setFontName(FONT_NAME_FOUNDER_BOLD);
	m_ui.txtPower = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Power)));
	m_ui.txtPower->setFontName(FONT_NAME_FOUNDER_BOLD);
	m_ui.txtDayCountCur = Text::create("", FONT_FZZHENGHEI, 22);
	m_ui.txtDayCountCur->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.txtDayCountCur->setPosition(Vec2(335.0f, 36.0f));
	m_ui.imgBarrierInfoBg->addChild(m_ui.txtDayCountCur);
	m_ui.txtDayCountTotal = Text::create("", FONT_FZZHENGHEI, 22);
	m_ui.txtDayCountTotal->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.txtDayCountTotal->setPosition(Vec2(385.0f, 36.0f));
	m_ui.imgBarrierInfoBg->addChild(m_ui.txtDayCountTotal);

	// 普通/精英;
	m_ui.btnNormal = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Normal)));
	m_ui.btnNormal->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Normal));
	m_ui.btnNormal->setPressedActionEnabled(false);
	enabledBarrierLv(BARRIER_LEVEL_0, false);
	m_ui.btnHard = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Hard)));
	m_ui.btnHard->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Hard));
	m_ui.btnHard->setPressedActionEnabled(false);
	enabledBarrierLv(BARRIER_LEVEL_1, false);

	// 扫荡按钮;
	m_ui.btnSweep1 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,STRING(Btn_Sweep1)));
	m_ui.btnSweep1->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Sweep1));
	m_ui.btnSweep10 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,STRING(Btn_Sweep10)));
	m_ui.btnSweep10->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Sweep10));

	// 动画形式的战斗按钮;
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniBtnFight/fight.ExportJson");
	armatureFight = Armature::create("fight");
	armatureFight->setPosition(Vec2(365.0f, 128.0f));
	m_ui.imgBarrierInfoBg->addChild(armatureFight);
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=] (Touch* touch, Event* event)
	{
		if (!m_bEnableFightBtn)
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
	armatureFight->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, armatureFight);
	armatureFight->getAnimation()->play("Animation1", -1, 1);

	// 难度按钮动画;
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniButtonLevel/AniButtonLevel.ExportJson");
	armatureNewLv = Armature::create("AniButtonLevel");
	m_ui.btnNormal->getParent()->addChild(armatureNewLv);
	armatureNewLv->getAnimation()->play("Animation1", -1, 1);
	armatureNewLv->setVisible(false);

	// 奖励;
	m_ui.numLab1 = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(AtlasLab_Num1)));
	m_ui.numLab2 = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(AtlasLab_Num2)));
	m_ui.numLab3 = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(AtlasLab_Num3)));
	m_ui.progressBar = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(m_ui.pRoot, "ProgressBar_1"));
	ostringstream oss;
	for (int i = 0; i < 3; ++i)
	{
		oss.str("");
		oss << "Btn_Box_" << i+1;
		m_ui.btnBox[i] = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		m_ui.btnBox[i]->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Box_1+i));

		oss.str("");
		oss << "AtlasLabel_star_" << i+2;
		m_ui.numStage[i] = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
	}
	m_ui.pnlStar = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName(STRING(Pnl_Star)));
	m_ui.pnlStar->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onLayerTouched, this, Pnl_Star));
	m_ui.imgStarContentBg = dynamic_cast<ImageView*>(m_ui.pnlStar->getChildByName("Img_Star_Bg"));
	m_ui.pnlStarContent = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Pnl_Star_Content)));

	initVipUI_1();
	initVipUI_2();

	this->addChild(m_ui.pRoot);
}


void LayerBarrier::updateSelf(void* data)
{
	if (nullptr == data)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) data;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_CHAPTER_INFO:
		{
			updateChapterInfo((UI_CHAPTER_DETAIL_PARAM*)(param->updateData));
		}
		break;

	case OBS_PARAM_TYPE_BARRIER_LIST:
		{
			updateBarrierList((UI_BARRIER_LIST_PARAM*)(param->updateData));
			//判断神秘商店是否需要解发
			getScheduler()->schedule([=](float dt)->void
			{
				this->getScheduler()->unschedule("LayerBarrier::updateSelf", this);
				if (MysteryShopModel::getInstance()->getIsInterShow() && MysteryShopModel::getInstance()->getIsMainShow())
				{
					//从结算出来判断是否要显示
					Director::getInstance()->getRunningScene()->addChild(MysteryInterLayer::create(),this->getLocalZOrder()+1);
				}
			},this, 0, 0, 0, false, "LayerBarrier::updateSelf");	

		}
		break;

	case OBS_PARAM_TYPE_BARRIER_INFO:
		{
			updateBarrierInfo((UI_BARRIER_INFO_PARAM*)(param->updateData));
		}
		break;

	case OBS_PARAM_TYPE_STAR_REWARD:
		{
			updateStarReward((UI_STAR_REWARD_PARAM*)(param->updateData));
		}
		break;

	case OBS_PARAM_TYPE_BARRIER_LV:
		{
			selectBarrierLv(*(BARRIER_LEVEL*)(param->updateData));
		}
		break;

	case OBS_PARAM_TYPE_STAR_REWARD_RESULT:
		{
			int nStarIndex = *((int*)(param->updateData));
			map<int, int> mapRewards;
			int nNeedCount = ChapterModel::getInstance()->getStarRewardsByLv(m_starRewardParam.nChapterId, nStarIndex, mapRewards);

			// 弹出奖励内容;
			vector<PopRewardItem*> vecRewardItem;
			for(auto iter = mapRewards.begin(); iter != mapRewards.end(); ++iter)
			{
				PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(iter->first, iter->second);
				vecRewardItem.push_back(rewardItem);
			}
			PopReward::show(vecRewardItem);

		}
		break;

	case DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE::START:
		{
			DOUBLE_BONUS_TYPE type = *((DOUBLE_BONUS_TYPE*)(param->updateData));
			if (type == DOUBLE_BONUS_TYPE_STORY)
			{
				showDoubleBonusTips_Lv0(true);
			}
			else if (type == DOUBLE_BONUS_TYPE_ELITE)
			{
				showDoubleBonusTips_Lv1(true);
			}
		}
		break;

	case DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE::END:
		{
			DOUBLE_BONUS_TYPE type = *((DOUBLE_BONUS_TYPE*)(param->updateData));
			if (type == DOUBLE_BONUS_TYPE_STORY)
			{
				showDoubleBonusTips_Lv0(false);
			}
			else if (type == DOUBLE_BONUS_TYPE_ELITE)
			{
				showDoubleBonusTips_Lv1(false);
			}
		}
		break;

	default:
		break;
	}

}


void LayerBarrier::updateChapterInfo(UI_CHAPTER_DETAIL_PARAM* data)
{
	// 音乐;
	ostringstream oss;
	oss << "chapter_" << ChapterModel::getInstance()->getMusicNameIndex(data->nChapterId);
	SoundUtils::playSoundMusic(oss.str());
	oss.str("");

	// 开启难度;
	if (data->nMaxLv >= BARRIER_LEVEL_1)
	{
		// 修正关卡位置;
		for (unsigned int j = 0; j <= data->nMaxLv; ++j)
		{
			enabledBarrierLv((BARRIER_LEVEL)(BARRIER_LEVEL_0 + j));
		}

		// 修正难度按钮位置;
		//fixButtonLvPosition(data->nMaxLv);

		// 新开启难度提示;
		if (data->bNewLv)
		{
			showNewLv(data->nNewLv);
		}
	}
}


void LayerBarrier::updateBarrierList(UI_BARRIER_LIST_PARAM* data)
{
	if (nullptr == data || data->vcBarrierList.empty())
	{
		return;
	}

	// 清空当前显示;
	m_ui.scrollviewBarrierList->removeAllChildrenWithCleanup(true);
	m_ui.scrollviewBarrierList->jumpToLeft();
	m_mapBarriers.clear();
	m_nCurSelBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;

	// 关卡列表;
	for (unsigned int i = 0; i < data->vcBarrierList.size(); ++i)
	{
		BarrierNode* node = BarrierNode::create(&(data->vcBarrierList.at(i)));
		if (nullptr != node)
		{
			m_mapBarriers.insert(pair<int, BarrierNode*>(data->vcBarrierList.at(i).nBarrierId, node));
			int scrollViewWidth = data->vcBarrierList.size()*(BARRIER_LIST_NODE_WIDTH + BARRIER_LIST_SPACE*2) + 20.0f;
			m_ui.scrollviewBarrierList->setInnerContainerSize(Size(scrollViewWidth, BARRIER_LIST_NODE_HEIGHT));
			Size innerSize = m_ui.scrollviewBarrierList->getInnerContainerSize();
			Vec2 ptOriPos(Vec2(BARRIER_LIST_NODE_WIDTH/2.0f + i *(BARRIER_LIST_NODE_WIDTH + BARRIER_LIST_SPACE*2),
				innerSize.height / 2.0f));
			node->setPosition(ptOriPos);
			node->setOriPosition(ptOriPos);
			m_ui.scrollviewBarrierList->addChild(node);
			node->setLogicParentLayer(this);
		}
	}

	// 下一帧执行;
	UI_BARRIER_LIST_PARAM  tmpParam = (*data);
	auto callBack = [this, tmpParam](float dt)->void
	{
		this->getScheduler()->unschedule("LayerBarrier::updateBarrierList", this);

		// 选中关卡;
		onBarrierSelected(tmpParam.nSelectedBarrierId);

		// 定位;
		fixBarrierPosition(tmpParam.nCurBarrierId);

		// 新开启标识;
		if (tmpParam.nNewBarrier != INVALID_CHAPTER_OR_BARRIER_ID)
		{
			showNewBarrier(tmpParam.nNewBarrier);
		}

		// 取消新开启难度标识;
		if (tmpParam.nLv == tmpParam.nMaxLv)
		{
			armatureNewLv->setVisible(false);
		}
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerBarrier::updateBarrierList");
}


void LayerBarrier::updateBarrierInfo(UI_BARRIER_INFO_PARAM* data)
{
	if (nullptr == data)
	{
		return;
	}

	// 保留体力消耗数据;
	m_nConsume = data->nConsume;

	// 移除所有当前显示的;
	m_ui.imgDropLine->removeAllChildren();

	// 关卡名;
	m_ui.txtBarrierName->setString((data->strBarrierName));

	// 关卡描述;
	m_ui.txtBarrierDesc->setString((data->strDescription));

	// 掉落;
	int i = 0;
	for (auto iter = data->mapRewardShow.begin(); iter != data->mapRewardShow.end(); ++iter, ++i)
	{
		UIToolHeader* drop = UIToolHeader::create(iter->first);
		drop->setToolId(iter->first);
		drop->setIconQua(iter->second.at(1));
		drop->setTipsEnabled(true);
		m_ui.imgDropLine->addChild(drop);
		drop->setVisible(true);
		drop->setAnchorPoint(Vec2(0.5f, 0.5f));
		drop->setPosition(Vec2(80.0f + i*130.0f, -50.0f));
	}

	// 获得银币;
	ostringstream strText;
	strText << data->nCoin;
	m_ui.txtCoin->setString(STR_UTF8(strText.str()));
	strText.str("");

	// 获得经验;
	strText << data->nExp;
	m_ui.txtExp->setString(STR_UTF8(strText.str()));
	strText.str("");

	// 体力消耗;
	strText << data->nPower << "/" << data->nConsume;
	m_ui.txtPower->setString(STR_UTF8(strText.str()));
	strText.str("");

	// 挑战次数;
	updateDayCount(data->nBarrierId);

	// 是否允许战斗;
	armatureFight->setVisible(data->bIsAllowed);

	// 是否允许扫荡;
	m_ui.btnSweep1->setVisible(armatureFight->isVisible());
	m_ui.btnSweep10->setVisible(armatureFight->isVisible());
	m_nStarCount = data->nStar;
}


void LayerBarrier::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
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
				m_bIsEnterFighting = false;
				if (SceneChapter::getBarrierBackFlag())
				{
					Director::getInstance()->popScene();
				}
				else
				{
					this->scheduleUpdate();
				}
			}
			break;

		case Btn_Sweep1:
		case Btn_Sweep10:
			{
				if (m_nCurSelBarrierId == INVALID_CHAPTER_OR_BARRIER_ID)
					break;

				// 1.本关3星通关;
				if (m_nStarCount >= MAX_BARRIER_STAR_COUNT)
				{
					// 2.等级达10级或VIPX;
					bool bCheck = false;
					if (nWidgetName == Btn_Sweep1)
					{
						bCheck = GuideManager::getInstance()->isFunctionOpen(FUNCTION_SWEEP);
					}
					else if (nWidgetName == Btn_Sweep10)
					{
						//bool bCon1 = GuideManager::getInstance()->isFunctionOpen(FUNCTION_SWEEP_10, false);
						RowData* rowData = DataManager::getInstance()->searchFunctionLevelById(FUNCTION_SWEEP_10);
						CCASSERT(rowData != nullptr, "");
						int functionLevel = rowData->getIntData("avlLv");
						bool bCon1 = (HeroModel::getInstance()->getMainHero()->mLevel >= functionLevel);

						int vipLv = ChapterModel::getInstance()->getVipLvBySweepFunc();
						bool bCon2 = (VipModel::getInstance()->getCurVipLv() >= vipLv);
						bCheck = (bCon1 || bCon2);
						if (!bCheck)
						{
							ostringstream oss;
							oss << DataManager::getInstance()->searchCommonTexdtById(10064) << functionLevel 
								<< DataManager::getInstance()->searchCommonTexdtById(10065) << vipLv 
								<< DataManager::getInstance()->searchCommonTexdtById(10066);
							CustomTips::showUtf8(oss.str());
						}
					}
					if (bCheck)
					{
						// 关卡实际剩余次数，VIP相关;
						if (checkLeftTimesWithVip())
						{
							Layer* layer = nullptr;
							int nBaseCount = (nWidgetName == Btn_Sweep1) ? 1 : 10;
							string strReason;
							int nFinalCount;
							int nRet = ChapterController::getInstance()->handlePreStartSweep(nBaseCount, strReason, nFinalCount);
							if (nRet >= 0)
							{
								layer = LayerSweep::create(m_nConsume, nBaseCount, nFinalCount, 0);
								dynamic_cast<LayerSweep*>(layer)->setLogicParent(this);
								this->addChild(layer, m_ui.pnlBarrier->getLocalZOrder()+1);
							}
							else if (nRet == -2)
							{
								// 扫荡券不足;
								int nPrice = BarrierModel::getInstance()->getSweepPrice();
								ostringstream oss;
								oss << DataManager::getInstance()->searchCommonTexdtById(10051) << nFinalCount*nPrice 
									<< DataManager::getInstance()->searchCommonTexdtById(10052) << nFinalCount 
									<< DataManager::getInstance()->searchCommonTexdtById(10053);
								CustomPop::showUtf8(oss.str(), [=](Ref* pSender, CustomRetType type){
									if (type == RET_TYPE_OK)
									{
										// 检查金币;
										if (nFinalCount*nPrice > MainModel::getInstance()->getMainParam()->mGold)
										{
											CustomGoldTips *_tips = CustomGoldTips::create();
											if (_tips)
											{
												Director::getInstance()->getRunningScene()->addChild(_tips);
											}
										}
										else
										{
											Layer* layer = LayerSweep::create(m_nConsume, nBaseCount, nFinalCount, 1);
											dynamic_cast<LayerSweep*>(layer)->setLogicParent(this);
											this->addChild(layer, m_ui.pnlBarrier->getLocalZOrder()+1);
										}
									}
								}, CUSTOM_YES_NO);
							}
							else if (nRet == -3)
							{
								LayerCommonBuy *buyPower = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
								Director::getInstance()->getRunningScene()->addChild(buyPower);
							}
							else
							{
								if (strReason != "")
								{
									CustomPop::showUtf8(strReason);
								}
							}
						}
					}
					//else
					//{
					//	CustomTips::show("主角10级或VIP1开放");
					//}
				}
				else
				{
					//CustomTips::show("3星通关开启扫荡");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10067));
				}
			}
			break;

		case Btn_Fight:
			{
				if (m_nCurSelBarrierId == INVALID_CHAPTER_OR_BARRIER_ID)
					break;

				// 关卡实际剩余次数，VIP相关;
				if (checkLeftTimesWithVip())
				{
					// 进入关卡;
					string strReason;
					int nId = ChapterController::getInstance()->handleFight(strReason);
					if (INVALID_CHAPTER_OR_BARRIER_ID != nId)
					{
						m_bEnableFightBtn = false;

						// 此处改为战前阵型(Phil 08/12/2015 @zcjoy);
						Director::getInstance()->pushScene(SceneFormation::create(F_MODE_BARRIER));
					}
					else
					{
						// pop reason;
						if (strReason != "")
						{
							CustomPop::showUtf8(strReason);
						}
					}
				}
			}
			break;

		case Btn_Normal:
		case Btn_Hard:
			{
				// 刷新按钮状态和关卡列表;
				selectBarrierLv((BARRIER_LEVEL)(nWidgetName - Btn_Normal));
			}
			break;

		case Btn_Box_1:
		case Btn_Box_2:
		case Btn_Box_3:
			{
				//GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_STAGE_STAR_REWARD);

				if (ChapterModel::getInstance()->isGetStarReward(m_starRewardParam.nChapterId, nWidgetName-Btn_Box_1+1))
				{
					//CustomTips::show("已领取");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10068));
				}
				else
				{
					map<int, int>  mapReward;
					int nNeedCount = ChapterModel::getInstance()->getStarRewardsByLv(m_starRewardParam.nChapterId,
						(int)(nWidgetName-Btn_Box_1+1), mapReward);

					// 未领取 - 可领取;
					if (m_starRewardParam.nStarCount >= nNeedCount)
					{
						// 领取;
						ChapterController::getInstance()->handleStarReward(nWidgetName-Btn_Box_1+1);
					}
					// 未领取 - 不可领取;
					else
					{
						// 预览奖励内容;
						m_ui.pnlStar->setVisible(true);
						SoundUtils::playSoundEffect("tanchuang");
						ActionCreator::runCommDlgAction(m_ui.imgStarContentBg);

						int nIndex = 0;
						for (auto iter = mapReward.begin(); iter != mapReward.end(); ++iter, ++nIndex)
						{
							UIToolHeader* _item = UIToolHeader::create(iter->first);
							_item->checkChip(iter->first);
							_item->setNumEx(iter->second);
							_item->setPosition(Vec2(m_ui.pnlStarContent->getContentSize().width/6.0f * (2*(nIndex%3)+1),
								m_ui.pnlStarContent->getContentSize().height/4.0f * 3.0f - m_ui.pnlStarContent->getContentSize().height/2.0f * (nIndex/3)));
							RowData* _row = DataManager::getInstance()->searchToolById(iter->first);
							assert(_row != nullptr);
							string strName = _row->getStringData("itemName");
							Text* txtName = Text::create(strName.c_str(), FONT_FZZHENGHEI, 18);
							setTextAddStroke(txtName, Color3B(0x79, 0x03, 0x03), 2);
							txtName->setPosition(Point(0.0f, -10.0f) + _item->getPosition());
							m_ui.pnlStarContent->addChild(_item);
							m_ui.pnlStarContent->addChild(txtName);
						}
						m_ui.numLab3->setString(_TO_STR(nNeedCount));
					}
				}
			}
			break;

		case Btn_Vip_OK:
			{
				// Vip;
				Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
			}
			break;

		case Btn_Vip_Cancel:
		case Btn_Vip_Close:
			{
				m_ui.pVipRoot->setVisible(false);
			}
			break;

		case Btn_Vip_State_OK:
			{
				m_ui.pVipStateRoot->setVisible(false);

				// 重置;
				if (ChapterController::getInstance()->handlePreResetBarrier() >= 0)
				{
					ChapterController::getInstance()->handleResetBarrier();
				}
				else
				{
					CustomGoldTips *_tips = CustomGoldTips::create();
					if (_tips)
					{
						Director::getInstance()->getRunningScene()->addChild(_tips);
					}
				}
			}
			break;

		case Btn_Vip_State_Cancel:
		case Btn_Vip_State_Close:
			{
				m_ui.pVipStateRoot->setVisible(false);
			}
			break;

		default:
			break;

		} // end switch

	} // end if
}


void LayerBarrier::onBarrierSelected(int nBarrierId)
{
	// 刷新UI;
	if (nBarrierId != m_nCurSelBarrierId)
	{
		// 刷新按钮状态;
		auto iter = m_mapBarriers.find(m_nCurSelBarrierId);
		if (iter != m_mapBarriers.end())
		{
			iter->second->updateSelected(false);
		}
		iter = m_mapBarriers.find(nBarrierId);
		if (iter != m_mapBarriers.end())
		{
			iter->second->updateSelected(true);
		}
		m_nCurSelBarrierId = nBarrierId;
	}

	// 刷新详情展示内容;
	ChapterController::getInstance()->onBarrierSelected(nBarrierId);
}


void LayerBarrier::selectBarrierLv(BARRIER_LEVEL nLv)
{
	// 禁止重复刷新;
	if (!m_mapBarriers.empty() && m_nCurSelLv == nLv)
	{
		return;
	}

	// 更新关卡数据;
	if (!ChapterController::getInstance()->onBarrierLevelSelected(nLv))
	{
		// Pop;
		//CustomTips::show("通关上一个难度后开放");
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10069));
		return;
	}

	// 再次播放进场特效;
	playEnterAnimation();

	// 保存当前选中难度;
	m_nCurSelLv = nLv;

	// 更新按钮外观;
	switch (nLv)
	{
	case BARRIER_LEVEL_0:
		{
			m_ui.btnNormal->setEnabled(true);
			m_ui.btnNormal->setBright(true);
			m_ui.btnNormal->loadTextureNormal("Image/UIBarrier/Button/btn_putong_2.png");
			m_ui.btnHard->loadTextureNormal("Image/UIBarrier/Button/btn_jingying_1.png");
		}
		break;

	case BARRIER_LEVEL_1:
		{
			m_ui.btnHard->setEnabled(true);
			m_ui.btnHard->setBright(true);
			m_ui.btnNormal->loadTextureNormal("Image/UIBarrier/Button/btn_putong_1.png");
			m_ui.btnHard->loadTextureNormal("Image/UIBarrier/Button/btn_jingying_2.png");
		}
		break;

	default:
		break;
	}

}

void LayerBarrier::setBarrierScrollPositionPercent( float fPercent )
{
	CCLOG("************* scroll percent %f", fPercent);
	if (fPercent < 0.0f)
	{
		fPercent = 0.0f;
	}
	else if (fPercent > 1.0f)
	{
		fPercent = 1.0f;
	}
	m_ui.scrollviewBarrierList->scrollToPercentHorizontal(fPercent*100.0f, 0.4f, true);
}

void LayerBarrier::showNewBarrier( int nBarrierId )
{
	if (m_mapBarriers.empty())
	{
		return;
	}

	// 显示新开启关卡标识;
	auto iter = m_mapBarriers.find(nBarrierId);
	if (iter != m_mapBarriers.end())
	{
		iter->second->updateNewFlag(true);
	}
}

void LayerBarrier::updateStarReward( UI_STAR_REWARD_PARAM* param )
{
	// 保存星级奖励参数;
	m_starRewardParam = *param;

	// 奖励;
	ostringstream oss;
	oss << param->nStarCount;
	m_ui.numLab1->setString(oss.str());
	oss.str("");
	oss << param->nTotalStarCount;
	m_ui.numLab2->setString(oss.str());
	oss.str("");

	// 进度;
	//float percent = (float)(param->nStarCount)/(float)(param->nTotalStarCount);
	//m_ui.progressBar->setPercent(percent * 100.0f);

	// 拿到各阶段星数之后，单独计算进度;
	vector<int> vcStarStage;
	vcStarStage.push_back(0);

	// 星级奖励按钮;
	for (int i = 0; i < 3; ++i)
	{
		// 各阶段标准;
		map<int, int> mapRewards;
		int nNeedCount = ChapterModel::getInstance()->getStarRewardsByLv(param->nChapterId, i+1, mapRewards);
		vcStarStage.push_back(nNeedCount);
		m_ui.numStage[i]->setString(_TO_STR(nNeedCount));

		// 已领取;
		oss.str("");
		if (ChapterModel::getInstance()->isGetStarReward(param->nChapterId, i+1))
		{
			oss << "Image/UIChapter/Btn/img_baoxiang" << i+1 << "_3.png";
			m_ui.btnBox[i]->loadTextureNormal(oss.str());
			m_ui.btnBox[i]->loadTexturePressed(oss.str());

			// 后层动画;
			if (armatureBack[i] != nullptr)
			{
				armatureBack[i]->removeFromParent();
				armatureBack[i] = nullptr;
			}

			// 前层动画;
			if (armatureFront[i] != nullptr)
			{
				armatureFront[i]->removeFromParent();
				armatureFront[i] = nullptr;
			}
		}
		else
		{
			// 未领取 - 可领取;
			if (param->nStarCount >= nNeedCount)
			{
				oss << "Image/UIChapter/Btn/img_baoxiang" << i+1 << "_2_1.png";
				m_ui.btnBox[i]->loadTextureNormal(oss.str());
				oss.str("");
				oss << "Image/UIChapter/Btn/img_baoxiang" << i+1 << "_2_2.png";
				m_ui.btnBox[i]->loadTexturePressed(oss.str());

				// 后层动画;
				if (armatureBack[i] == nullptr)
				{
					ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniXingjijiangli/AniXingjijiangli.ExportJson");
					armatureBack[i] = Armature::create("AniXingjijiangli");
					armatureBack[i]->setPosition(m_ui.btnBox[i]->getPosition());
					m_ui.btnBox[i]->getParent()->addChild(armatureBack[i], m_ui.btnBox[i]->getLocalZOrder()-1);
					armatureBack[i]->getAnimation()->play("back");
				}

				// 前层动画;
				if (armatureFront[i] == nullptr)
				{
					ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniXingjijiangli/AniXingjijiangli.ExportJson");
					armatureFront[i] = Armature::create("AniXingjijiangli");
					armatureFront[i]->setPosition(m_ui.btnBox[i]->getPosition());
					m_ui.btnBox[i]->getParent()->addChild(armatureFront[i], m_ui.btnBox[i]->getLocalZOrder()+1);
					armatureFront[i]->getAnimation()->play("front");
				}
			}
			// 未领取 - 不可领取;
			else
			{
				oss << "Image/UIChapter/Btn/img_baoxiang" << i+1 << "_1_1.png";
				m_ui.btnBox[i]->loadTextureNormal(oss.str());
				oss.str("");
				oss << "Image/UIChapter/Btn/img_baoxiang" << i+1 << "_1_2.png";
				m_ui.btnBox[i]->loadTexturePressed(oss.str());
			}
		}
	}

	// 计算进度;
	CCASSERT(vcStarStage.size() == 4, "");
	for (int k = 0; k < 4; ++k)
	{
		if (param->nStarCount >= vcStarStage.at(k))
		{
			float basePercent = (1.0f/3.0f)*k;
			float incressPercent = 0.0f;
			if (k+1 < vcStarStage.size())
			{
				incressPercent = ((float)(param->nStarCount - vcStarStage.at(k)))/((float)(vcStarStage.at(k+1) - vcStarStage.at(k)));
			}
			if (incressPercent < 1.0f)
			{
				float finalPercent = basePercent + incressPercent*(1.0f/3.0f);
				finalPercent = max(0.0f, finalPercent);
				finalPercent = min(finalPercent, 100.0f);
				m_ui.progressBar->setPercent(finalPercent * 100.0f);
			}
		}
	}
}

void LayerBarrier::enabledBarrierLv( BARRIER_LEVEL nLv, bool bEnabled /*= true*/ )
{
	switch (nLv)
	{
	case BARRIER_LEVEL_0:
		{
			m_ui.btnNormal->setEnabled(bEnabled);
			m_ui.btnNormal->setBright(bEnabled);
			m_ui.btnNormal->setVisible(bEnabled);
		}
		break;

	case BARRIER_LEVEL_1:
		{
			m_ui.btnHard->setEnabled(bEnabled);
			m_ui.btnHard->setBright(bEnabled);
			m_ui.btnHard->setVisible(bEnabled);
		}
		break;

	default:
		break;
	}
}

void LayerBarrier::fixBarrierPosition( int nBarrierId )
{
	if (m_mapBarriers.empty())
	{
		return;
	}
	CCLOG("************* scroll barrierId %d", nBarrierId);
	// 滚动至某个关卡的位置;
	int nLeftCount = m_mapBarriers.size() - 2;
	int nBarrierOrder =  ChapterModel::getInstance()->getBarrierLogicOrder(nBarrierId);
	if (nLeftCount >= 1 && nBarrierOrder >= 3)
	{
		float fPercent = 1.0f / nLeftCount;
		setBarrierScrollPositionPercent((nBarrierOrder - 2)*fPercent);
	}
}

void LayerBarrier::showNewLv( BARRIER_LEVEL nLv )
{
	Button* btnAll[2];
	btnAll[0] = m_ui.btnNormal;
	btnAll[1] = m_ui.btnHard;
	armatureNewLv->setPosition(btnAll[(int)nLv]->getPosition());
	armatureNewLv->setVisible(true);
}

void LayerBarrier::updatePower( int nTotalPower )
{
	ostringstream oss;
	oss << m_nConsume << "/" << nTotalPower;
	m_ui.txtPower->setString(STR_UTF8(oss.str()));
	oss.str("");
}

void LayerBarrier::updateDayCount( int nBarrierId /*= INVALID_CHAPTER_OR_BARRIER_ID*/ )
{
	if (nBarrierId == INVALID_CHAPTER_OR_BARRIER_ID)
	{
		nBarrierId = m_nCurSelBarrierId;
	}

	if (nBarrierId == INVALID_CHAPTER_OR_BARRIER_ID)
		return;

	int nLeftDayCount = ChapterModel::getInstance()->getBarrierLeftTimes(nBarrierId);
	int nTotalDayCount = ChapterModel::getInstance()->getBarrierTotalTimes(nBarrierId);

	ostringstream oss;
	oss << nLeftDayCount;
	m_ui.txtDayCountCur->setString(oss.str());
	oss.str("");
	oss << "/" << nTotalDayCount;
	m_ui.txtDayCountTotal->setString(oss.str());

	Size size = m_ui.txtDayCountCur->getContentSize();
	m_ui.txtDayCountTotal->setPosition(m_ui.txtDayCountCur->getPosition() + Point(size.width, 0.0f));

	// 双倍标记;
	showDoubleBonusTips_Lv0(DoubleBonusModel::getInstance()->isDoubleBonusValid(DOUBLE_BONUS_TYPE_STORY)
		&& (ChapterModel::getInstance()->getBarrierLv(nBarrierId) == BARRIER_LEVEL_0));
	showDoubleBonusTips_Lv1(DoubleBonusModel::getInstance()->isDoubleBonusValid(DOUBLE_BONUS_TYPE_ELITE)
		&& (ChapterModel::getInstance()->getBarrierLv(nBarrierId) == BARRIER_LEVEL_1));
}

void LayerBarrier::onLayerTouched( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::ENDED)
	{
		if (nWidgetName == Pnl_Star)
		{
			if (m_ui.pnlStar != nullptr)
			{
				m_ui.pnlStar->setVisible(false);
			}
		}
	}
}

void LayerBarrier::playEnterAnimation()
{
	for (int i = 0; i < 3; ++i)
	{
		//ActionCreator::runCommBtnAction(m_ui.btnBox[i]);
		m_ui.btnBox[i]->setScale(0.0f);
		m_ui.btnBox[i]->runAction(EaseBackOut::create(ScaleTo::create(0.2f, 1.0f)));
	}

	//ActionCreator::runCommBtnAction(m_ui.btnSweep1);
	m_ui.btnSweep1->setScale(0.0f);
	m_ui.btnSweep1->runAction(EaseBackOut::create(ScaleTo::create(0.2f, 1.0f)));

	//ActionCreator::runCommBtnAction(m_ui.btnSweep10);
	m_ui.btnSweep10->setScale(0.0f);
	m_ui.btnSweep10->runAction(EaseBackOut::create(ScaleTo::create(0.2f, 1.0f)));
}

bool LayerBarrier::checkLeftTimesWithVip()
{
	// 关卡实际剩余次数，VIP相关;
	int nLeftTimes = ChapterModel::getInstance()->getBarrierLeftTimes(m_nCurSelBarrierId);
	if (nLeftTimes <= 0)
	{
		checkBuyCondition();
		return false;
	}

	return true;
}

void LayerBarrier::initVipUI_1()
{
	m_ui.pVipStateRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UiNewbuy/UIVip_State.ExportJson"));
	this->addChild(m_ui.pVipStateRoot, 9);
	m_ui.pVipStateRoot->setVisible(false);
	m_ui.btnVipStateOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_OK"));
	m_ui.btnVipStateCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_Cancel"));
	m_ui.btnVipStateClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_Close"));

	m_ui.txtVipStateTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi1"));
	m_ui.txtVipStateTips1->setFontName(FONT_FZZHENGHEI);
	//string str("是否确认重置关卡？");
	string str(DataManager::getInstance()->searchCommonTexdtById(10070));
	m_ui.txtVipStateTips1->setString(str);
	m_ui.txtVipStateTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi2"));
	m_ui.txtVipStateTips2->setFontName(FONT_FZZHENGHEI);
	str.clear();
	//str.assign("本次重置消耗");
	str.assign(DataManager::getInstance()->searchCommonTexdtById(10071));
	m_ui.txtVipStateTips2->setString(str);
	m_ui.txtVipStateTips3 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi4"));
	m_ui.txtVipStateTips3->setFontName(FONT_FZZHENGHEI);
	str.clear();
	//str.assign("今日可重置次数:");
	str.assign(DataManager::getInstance()->searchCommonTexdtById(10072));
	m_ui.txtVipStateTips3->setString(str);

	// 重置消耗;
	m_ui.imgCurVip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Img_Vip"));
	m_ui.imgCurVip->loadTexture(getVipIcon(VipModel::getInstance()->getCurVipLv()));
	m_ui.txtCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Lab_Cost"));
	m_ui.txtCost->setFontName(FONT_FZZHENGHEI);
	m_ui.txtCost->setString(_TO_STR(BarrierModel::getInstance()->getBarrierResetCost(m_nCurSelBarrierId)));

	// 剩余次数;
	m_ui.txtCurCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Lab_Left_Times"));
	m_ui.txtCurCount->setFontName(FONT_FZZHENGHEI);
	ostringstream oss;
	oss << BarrierModel::getInstance()->getCurVipLeftTimes(m_nCurSelBarrierId) << "/" << ChapterModel::getInstance()->getCurMaxTimes();
	m_ui.txtCurCount->setString(oss.str());

	m_ui.btnVipStateOK->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Vip_State_OK));
	m_ui.btnVipStateCancel->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Vip_State_Cancel));
	m_ui.btnVipStateClose->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Vip_State_Close));
}

void LayerBarrier::initVipUI_2()
{
	m_ui.pVipRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UiNewbuy/UiVipUp.ExportJson"));
	this->addChild(m_ui.pVipRoot, 9);
	m_ui.pVipRoot->setVisible(false);
	m_ui.btnVipOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_OK)));
	m_ui.btnVipCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_Cancel)));
	m_ui.btnVipClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipRoot, STRING(Btn_Vip_Close)));

	m_ui.imgVipBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Img_di"));
	m_ui.txtVipTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Tips1"));
	m_ui.txtVipTips1->setFontName(FONT_FZZHENGHEI);
	//string str("该关卡今日重置次数已用完");
	string str(DataManager::getInstance()->searchCommonTexdtById(10073));
	m_ui.txtVipTips1->setString(str);
	m_ui.txtVipTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Tips2"));
	m_ui.txtVipTips2->setFontName(FONT_FZZHENGHEI);
	m_ui.imgVipNext = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Img_Vip"));
	m_ui.txtNextCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Count"));
	m_ui.txtNextCount->setFontName(FONT_FZZHENGHEI);

	m_ui.btnVipOK->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Vip_OK));
	m_ui.btnVipCancel->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Vip_Cancel));
	m_ui.btnVipClose->addTouchEventListener(CC_CALLBACK_2(LayerBarrier::onBtnClicked, this, Btn_Vip_Close));
}


void LayerBarrier::checkBuyCondition()
{
	// 1.当前剩余次数;
	if (BarrierModel::getInstance()->getCurVipLeftTimes(m_nCurSelBarrierId) > 0)
	{
		updateVipData();
		m_ui.pVipStateRoot->setVisible(true);
		return;
	}

	// 2.当前可提升次数;
	int nNextValue = 0;
	int nNextLv = ChapterModel::getInstance()->getNextValidVip(nNextValue);
	if (nNextValue > ChapterModel::getInstance()->getCurMaxTimes())
	{
		m_ui.pVipRoot->setVisible(true);

		// 弹出特权提升面板;
		ostringstream oss;
		oss << DataManager::getInstance()->searchCommonTexdtById(10074) << nNextValue 
			<< DataManager::getInstance()->searchCommonTexdtById(10049);
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

std::string LayerBarrier::getVipIcon(int nVipLv)
{
	string strPath = "Image/UIVip/VipLv/img_vip_";
	strPath.append(_TO_STR(nVipLv));
	strPath.append(".png");
	return strPath;
}

void LayerBarrier::updateVipData()
{
	if (m_ui.pVipRoot == nullptr || m_ui.pVipStateRoot == nullptr)
	{
		return;
	}

	m_ui.txtCost->setString(_TO_STR(BarrierModel::getInstance()->getBarrierResetCost(m_nCurSelBarrierId)));

	ostringstream oss;
	oss << BarrierModel::getInstance()->getCurVipLeftTimes(m_nCurSelBarrierId) << "/" << ChapterModel::getInstance()->getCurMaxTimes();
	m_ui.txtCurCount->setString(oss.str());
}

void LayerBarrier::showDoubleBonusTips_Lv0( bool bShow )
{
	// 目前只有普通关卡会显示图片标记;
	ImageView* img = dynamic_cast<ImageView*>(m_ui.imgBarrierInfoBg->getChildByName("double_bonus_lv_0"));
	if (bShow)
	{
		if (nullptr == img)
		{
			img = ImageView::create("Image/Icon/global/20003.png");
			img->setName("double_bonus_lv_0");
			img->setPosition(Vec2(220.0f, 405.0f));
			m_ui.imgBarrierInfoBg->addChild(img);
		}
	}
	else
	{
		if (img != nullptr)
		{
			img->removeFromParent();
			img = nullptr;
		}
	}
}

void LayerBarrier::showDoubleBonusTips_Lv1( bool bShow )
{
	if (bShow)
	{
		int nLeftDayCount = ChapterModel::getInstance()->getBarrierLeftTimes(m_nCurSelBarrierId);
		int nTotalDayCount = ChapterModel::getInstance()->getBarrierTotalTimes(m_nCurSelBarrierId);
		m_ui.txtDayCountCur->setColor((nLeftDayCount > nTotalDayCount) ? Color3B(0x88, 0xFF, 0x14) : Color3B::WHITE);
	}
	else
	{
		m_ui.txtDayCountCur->setColor(Color3B::WHITE);
	}
}
