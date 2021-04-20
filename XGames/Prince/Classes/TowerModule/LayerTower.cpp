#include "LayerTower.h"
#include "TowerController.h"
#include "TowerModel.h"
#include "LayerTrailRanking.h"
#include "LayerTrailRewardPreview.h"
#include "FightModule/SceneFightLoading.h"
#include "FightModule/FightController.h"
#include "GuideCenter/GuideManager.h"
#include "Widget/ActionCreator.h"
#include "Widget/LayerCommHeadInfo.h"
#include "VIPModule/VipModel.h"
#include "Temp/CustomGoldTips.h"

LayerTower::LayerTower(void)
{
	TowerModel::getInstance()->addObserver(this);
	m_pFloor1 = nullptr;
	m_pFloor2 = nullptr;
	m_nInitCurFloor = 1;
	m_bIsMaxFloor = false;
	m_bFirstEnter = true;
}


LayerTower::~LayerTower(void)
{
	TowerModel::getInstance()->removeObserver(this);
}

bool LayerTower::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();

	// 先初始当前楼层;
	m_nInitCurFloor = TowerModel::getInstance()->getTowerCurFloor();
	if (m_nInitCurFloor > 1)
	{
		initFloor(m_nInitCurFloor);
	}

	return true;
}

void LayerTower::onEnter()
{
	Layer::onEnter();

	// 进场特效;
	//playEnterAnimation();

	if (m_bFirstEnter)
	{
		m_bFirstEnter = false;

		//updateMainInfo(TowerModel::getInstance()->getTowerMainInfo());

		//隔一帧再发送协议，防止崩溃;
		auto callBack = [this](float dt)->void
		{
			// 等拿到最新楼层信息，再做滚动;
			TowerController::getInstance()->handleTowerMainInfo();
			this->getScheduler()->unschedule("LayerTower::onEnter", this);
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerTower::onEnter");
	}

	
}

void LayerTower::initUI()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UINewtower/UINewtower_tower.ExportJson"));
	this->addChild(m_ui.pRoot, 2);

	// 通用标题栏;
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(
		[this](Ref* sender, Widget::TouchEventType type)->void
	{
		this->onBtnClicked(sender, type, Btn_Tower_Back);
	},
		HEAD_INFO_TYPE_THOUSAND_TOWER);
	this->addChild(_layerCommHeadInfo, 3);

	// 爬塔;
	m_ui.pnlTower = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName(STRING(Pnl_Tower)));
	m_ui.pnlTower->setContentSize(visibleSize);

	// 排行榜;
	m_ui.btnRanking = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Tower_Ranking)));
	m_ui.btnRanking->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Tower_Ranking));

	// 历史最高层数;
	m_ui.txtMaxFloor = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Max_Floor)));
	m_ui.txtMaxFloor->setFontName(FONT_FZZHENGHEI);
	m_ui.txtMaxFloor->setString("");
	setTextAddStroke(m_ui.txtMaxFloor, Color3B(0x93, 0x0, 0x0), 2);

	// 重置;
	m_ui.btnReset = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Tower_Reset)));
	m_ui.btnReset->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Tower_Reset));
	m_ui.btnReset->setVisible(false);

	// 进度;
	m_ui.imgProgressBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Bar"));
	ostringstream oss;
	for (int i = 0; i < 5; ++i)
	{
		oss.str("");
		oss << "Lab_Node_" << i+1;
		m_ui.txtProgressNode[i] = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
		m_ui.txtProgressNode[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.txtProgressNode[i]->setString("1");
	}
	m_ui.imgCursor = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Cursor"));

	// 奖励预览;
	m_ui.btnReward = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Tower_Reward)));
	m_ui.btnReward->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Tower_Reward));
	m_ui.btnBox = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Box)));
	m_ui.btnBox->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Box));
	m_ui.imgReward = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Reward"));
	m_ui.pnlReward = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Reward"));
	m_ui.pnlReward->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::ENDED)
		{
			m_ui.pnlReward->setVisible(false);
		}
	});
	m_ui.imgRewardBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Rewarddi"));
	m_ui.txtExp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Exp"));
	m_ui.txtExp->setFontName(FONT_FZZHENGHEI);
	m_ui.txtCoin = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Coin"));
	m_ui.txtCoin->setFontName(FONT_FZZHENGHEI);

	// 扫荡;
	m_ui.btnSweep = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Tower_Sweep)));
	m_ui.btnSweep->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Tower_Sweep));
	m_ui.btnSweep->setVisible(false);
	m_ui.txtTime1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_time_1"));
	m_ui.txtTime1->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtTime1, Color3B(0x8D, 0x1E, 0x1E), 2);
	m_ui.txtTime1->setVisible(false);
	m_ui.txtTime2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_time_2"));
	m_ui.txtTime2->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.txtTime2, Color3B(0x8D, 0x1E, 0x1E), 2);
	m_ui.txtTime2->setVisible(false);

	m_ui.pnlSweep = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Sweep"));
	m_ui.txt1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_1"));
	m_ui.txt1->setFontName(FONT_FZZHENGHEI);
	m_ui.txt2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_2"));
	m_ui.txt2->setFontName(FONT_FZZHENGHEI);
	m_ui.txtFloor = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Floor"));
	m_ui.txtFloor->setFontName(FONT_FZZHENGHEI);
	m_ui.txtSweepTime = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Time"));
	m_ui.txtSweepTime->setFontName(FONT_FZZHENGHEI);
	m_ui.btnSweepOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Sweep_OK)));
	m_ui.btnSweepOK->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Sweep_OK));
	m_ui.btnSweepCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Sweep_Cancel)));
	m_ui.btnSweepCancel->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Sweep_Cancel));
	m_ui.pnlSweep->setVisible(false);

	// 扫荡结果;
	m_ui.pnlSweepSettlement = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Sweep_Settlement"));
	m_ui.btnSettlementClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Sweep_Settlement_Close)));
	m_ui.btnSettlementClose->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Sweep_Settlement_Close));
	m_ui.imgContentBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Content_Bg"));
	m_ui.scrollviewContent = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, "ScrollView_Content"));
	m_ui.scrollviewContent->addEventListener(CC_CALLBACK_2(LayerTower::onScrollViewEvent, this));
	m_ui.scrollBar = CustomScrollBar::create();
	m_ui.scrollBar->initScrollBar(m_ui.scrollviewContent->getContentSize().height);
	m_ui.scrollBar->setPosition(Vec2(33.0f, 35.0f));
	m_ui.imgContentBg->addChild(m_ui.scrollBar);
	m_ui.txtSettlementCoin = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Settlement_Coin"));
	m_ui.txtSettlementCoin->setFontName(FONT_FZZHENGHEI);
	m_ui.txtSettlementExp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Settlement_Exp"));
	m_ui.txtSettlementExp->setFontName(FONT_FZZHENGHEI);

	initVipUI_1();
	initVipUI_2();

	this->setVisible(false);
}

void LayerTower::initVipUI_1()
{
	m_ui.pVipStateRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UiNewbuy/UIVip_State.ExportJson"));
	this->addChild(m_ui.pVipStateRoot, 3);
	m_ui.pVipStateRoot->setVisible(false);
	m_ui.btnVipStateOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_OK"));
	m_ui.btnVipStateCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_Cancel"));
	m_ui.btnVipStateClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Btn_Tips_Buy_Close"));

	m_ui.txtVipStateTips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi1"));
	m_ui.txtVipStateTips1->setFontName(FONT_FZZHENGHEI);
	string str(DataManager::getInstance()->searchCommonTexdtById(20050));
	m_ui.txtVipStateTips1->setString((str));
	m_ui.txtVipStateTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi2"));
	m_ui.txtVipStateTips2->setFontName(FONT_FZZHENGHEI);
	str.clear();
	str.assign(DataManager::getInstance()->searchCommonTexdtById(20051));
	m_ui.txtVipStateTips2->setString((str));
	m_ui.txtVipStateTips3 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Label_wenzi4"));
	m_ui.txtVipStateTips3->setFontName(FONT_FZZHENGHEI);
	str.clear();
	str.assign(DataManager::getInstance()->searchCommonTexdtById(20052));
	m_ui.txtVipStateTips3->setString((str));

	// 重置消耗;
	m_ui.imgCurVip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Img_Vip"));
	m_ui.imgCurVip->loadTexture(getVipIcon(VipModel::getInstance()->getCurVipLv()));
	m_ui.txtCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Lab_Cost"));
	m_ui.txtCost->setFontName(FONT_FZZHENGHEI);
	m_ui.txtCost->setString(_TO_STR(TowerModel::getInstance()->getTowerResetCost()));

	// 剩余次数;
	m_ui.txtCurCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipStateRoot, "Lab_Left_Times"));
	m_ui.txtCurCount->setFontName(FONT_FZZHENGHEI);
	ostringstream oss;
	oss << TowerModel::getInstance()->getCurVipLeftTimes() << "/" << TowerModel::getInstance()->getCurMaxTimes();
	m_ui.txtCurCount->setString(oss.str());

	m_ui.btnVipStateOK->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Vip_State_OK));
	m_ui.btnVipStateCancel->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Vip_State_Cancel));
	m_ui.btnVipStateClose->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Vip_State_Close));
}

void LayerTower::initVipUI_2()
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
	string str(DataManager::getInstance()->searchCommonTexdtById(20053));
	m_ui.txtVipTips1->setString((str));
	m_ui.txtVipTips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Tips2"));
	m_ui.txtVipTips2->setFontName(FONT_FZZHENGHEI);
	m_ui.imgVipNext = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Img_Vip"));
	m_ui.txtNextCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pVipRoot, "Lab_Count"));
	m_ui.txtNextCount->setFontName(FONT_FZZHENGHEI);

	m_ui.btnVipOK->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Vip_OK));
	m_ui.btnVipCancel->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Vip_Cancel));
	m_ui.btnVipClose->addTouchEventListener(CC_CALLBACK_2(LayerTower::onBtnClicked, this, Btn_Vip_Close));
}

void LayerTower::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		/*if (nWidgetName == Btn_Box)
		{
			m_ui.imgArrow->setVisible(true);
		}*/

		switch (nWidgetName)
		{
		case Btn_Tower_Back:
		case Btn_Tower_Ranking:
		case Btn_Tower_Reset:
		case Btn_Tower_Reward:
		case Btn_Tower_Sweep:
		case Btn_Sweep_OK:
		case Btn_Sweep_Cancel:
		case Btn_Sweep_Settlement_Close:
			SoundUtils::playSoundEffect("dianji");
			break;

		default:
			break;
		}
	}
	else if (type == Widget::TouchEventType::CANCELED)
	{
		/*if (nWidgetName == Btn_Box)
		{
			m_ui.imgArrow->setVisible(false);
		}*/
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (nWidgetName)
		{
		case Btn_Tower_Back:
			{
				this->scheduleUpdate();
			}
			break;

		case Btn_Tower_Ranking:
			{
				LayerTrailRanking* _layer = LayerTrailRanking::create(TOWER_TYPE_TOWER, R_WND_TYPE_RANKING);
				if (nullptr != _layer)
				{
					this->addChild(_layer, 5);
				}
			}
			break;

		case Btn_Tower_Reset:
			{
				// 先判断是否有剩余重置次数;
				if (TowerController::getInstance()->handlePreResetTower())
				{
					CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20054), [=](Ref* pSender, CustomRetType type){
						if (type == RET_TYPE_OK)
						{
							TowerController::getInstance()->handleResetTower();
						}
					}, CUSTOM_YES_NO);
				}
				else
				{
					// VIP;
					checkBuyCondition();
				}
			}
			break;

		case Btn_Tower_Reward:
			{
				LayerTrailRewardPreview* _layer = LayerTrailRewardPreview::create();
				if (nullptr != _layer)
				{
					this->addChild(_layer, 5);
				}
			}
			break;

		case Btn_Tower_Sweep:
			{
				if (TowerModel::getInstance()->getTowerMaxFloor() >= 10)
				{
					m_ui.pnlSweep->setVisible(true);
					ActionCreator::runCommDlgAction(m_ui.pnlSweep->getChildByName("Img_Bg"));
				}
				else
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20055));
				}
			}
			break;

		case Btn_Sweep_OK:
			{
				TowerController::getInstance()->handleTowerSweep();
				m_ui.pnlSweep->setVisible(false);
				m_ui.btnSweep->setEnabled(false);
				m_ui.btnSweep->setBright(false);
				m_ui.txtTime1->setVisible(true);
				m_ui.txtTime2->setVisible(true);
			}
			break;

		case Btn_Sweep_Cancel:
			{
				m_ui.pnlSweep->setVisible(false);
			}
			break;

		case Btn_Sweep_Settlement_Close:
			{
				m_ui.pnlSweepSettlement->setVisible(false);
			}
			break;

		case Btn_Box:
			{
				m_ui.pnlReward->setVisible(true);
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
				//m_ui.pVipRoot->removeFromParent();
				//m_ui.pVipRoot = nullptr;
			}
			break;

		case Btn_Vip_State_OK:
			{
				// 重置;
				if (TowerController::getInstance()->handlePreResetTower2())
				{
					TowerController::getInstance()->handleResetTower();
					m_ui.pVipStateRoot->setVisible(false);
					//m_ui.pVipStateRoot->removeFromParent();
					//m_ui.pVipStateRoot = nullptr;
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
				//m_ui.pVipStateRoot->removeFromParent();
				//m_ui.pVipStateRoot = nullptr;
			}
			break;

		default:
			break;
		}
	}
}

void LayerTower::update( float delta )
{
	this->unscheduleUpdate();
	this->removeFromParentAndCleanup(true);
}

void LayerTower::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_TOWER_MAIN_INFO:
		{
			updateMainInfo((TowerMainInfo*)(param->updateData));
			this->setVisible(true);

			// 进场特效;
			playEnterAnimation();

			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_CLIMB_TOWER);
		}
		break;

	case OBS_PARAM_TYPE_TOWER_FLOOR:
		{
			if (m_pFloor1 != nullptr || m_pFloor2 != nullptr)
			{
				if (TowerModel::getInstance()->getTowerCurFloor() == m_nInitCurFloor + 1)
				{
					// 滚动;
					scrollFloor();
				}
				else if (m_nInitCurFloor != 1 && TowerModel::getInstance()->getTowerMainInfo()->bEnd)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20056));
				}

				// 刷新奖励;
				updateReward();
			}
			else
			{
				initFloor(*((int*)(param->updateData)));
				updateReward();
			}
		}
		break;

	case OBS_PARAM_TYPE_TOWER_RESET:
		{
			resetFloor();
			updateReward();
			updateProgress();
		}
		break;

	case OBS_PARAM_TYPE_TOWER_SWEEP_COUNT_DOWN:
		{
			m_ui.btnSweep->setEnabled(false);
			m_ui.btnSweep->setBright(false);
			int sec = *((int*)(param->updateData));
			if (sec <= 0)
			{
				m_ui.btnSweep->setEnabled(true);
				m_ui.btnSweep->setBright(true);
				m_ui.txtTime1->setVisible(false);
				m_ui.txtTime2->setVisible(false);
			}
			m_ui.txtTime2->setString(formatTime(sec));
			setTextAddStroke(m_ui.txtTime2, Color3B(0x8D, 0x1E, 0x1E), 2);
		}
		break;

	case OBS_PARAM_TYPE_TOWER_SWEEP_SETTLEMENT:
		{
			m_ui.pnlSweepSettlement->setVisible(true);
			ActionCreator::runCommDlgAction(m_ui.pnlSweepSettlement->getChildByName("Img_Bg"));
			TowerSweepSettlement*  result = (TowerSweepSettlement*)(param->updateData);

			int nNodeHeight = 112.0f;
			int nScollviewWidth = m_ui.scrollviewContent->getContentSize().width;
			int nScrollviewHeight = (result->mapItem.size() * nNodeHeight > m_ui.scrollviewContent->getContentSize().height) ?
				result->mapItem.size() * nNodeHeight : m_ui.scrollviewContent->getContentSize().height;
			m_ui.scrollviewContent->setInnerContainerSize(Size(nScollviewWidth, nScrollviewHeight));

			int index = 0;
			for (auto iter = result->mapItem.begin(); iter != result->mapItem.end(); ++iter, ++index)
			{
				UIToolHeader* _item = UIToolHeader::create(iter->first);
				_item->setScale(0.8f);
				_item->setNumEx(iter->second);
				int nRow = (result->mapItem.size()/4 + ((result->mapItem.size() % 3) == 0 ? 0 : 1));
				int scrollViewHeight = ((nRow * nNodeHeight) > m_ui.scrollviewContent->getContentSize().height)
					? (nRow * nNodeHeight) : m_ui.scrollviewContent->getContentSize().height;
				m_ui.scrollviewContent->setInnerContainerSize(Size(m_ui.scrollviewContent->getContentSize().width, scrollViewHeight));
				Size innerSize = m_ui.scrollviewContent->getInnerContainerSize();
				_item->setPosition(Vec2(60.0f + (index % 4) * (innerSize.width/4.0f),
					innerSize.height - nNodeHeight/2 - (index/4) * nNodeHeight));
				m_ui.scrollviewContent->addChild(_item);
			}

			m_ui.scrollBar->setScrollBarLengthPercent(m_ui.scrollviewContent->getContentSize().height / m_ui.scrollviewContent->getInnerContainerSize().height);
			m_ui.scrollBar->setScrollBarPercentY(0.0f);

			m_ui.txtSettlementCoin->setString(_TO_STR(result->nCoin));
			m_ui.txtSettlementExp->setString(_TO_STR(result->nExp));

			// 确认已领取;
			TowerController::getInstance()->checkUISweepSettlement(true);
			TowerModel::getInstance()->setTowerSweepState(false);

			// 清除现有楼层信息;
			if (m_pFloor1 != nullptr)
			{
				m_pFloor1->removeFromParentAndCleanup(true);
				m_pFloor1 = nullptr;
			}
			if (m_pFloor2 != nullptr)
			{
				m_pFloor2->removeFromParentAndCleanup(true);
				m_pFloor2 = nullptr;
			}

			// 刷一次界面信息;
			TowerController::getInstance()->handleTowerMainInfo();
		}
		break;

	default:
		break;
	}
}

void LayerTower::updateMainInfo( TowerMainInfo* info )
{
	// 历史最高层/扫荡至最高层;
	ostringstream oss;
	oss << info->nMaxFloor;
	m_ui.txtMaxFloor->setString(oss.str());
	setTextAddStroke(m_ui.txtMaxFloor, Color3B(0x93, 0x0, 0x0), 2);
	oss << DataManager::getInstance()->searchCommonTexdtById(20057);
	m_ui.txtFloor->setString((oss.str()));
	setTextAddStroke(m_ui.txtFloor, Color3B(0x8D, 0x1E, 0x1E), 2);
	oss.str("");

	// 进度;
	updateProgress();

	// 扫荡所需时长;
	int sec = TowerModel::getInstance()->getSweepSingleTime();
	int nDst = TowerModel::getInstance()->getTowerMaxFloor() - TowerModel::getInstance()->getTowerCurFloor();
	if (nDst < 0)
	{
		nDst = TowerModel::getInstance()->getTowerMaxFloor() - 1;
	}
	sec *= (nDst + 1);
	m_ui.txtSweepTime->setString(formatTime(sec));

	// 重置/扫荡;
	m_ui.btnReset->setVisible(info->nCurFloor >= info->nMaxFloor && info->nMaxFloor > 1);
	m_ui.btnSweep->setVisible(!m_ui.btnReset->isVisible());
	m_ui.btnSweep->setEnabled(!info->bIsSweep);
	m_ui.txtTime1->setVisible(m_ui.btnSweep->isVisible() && !m_ui.btnSweep->isEnabled());
	m_ui.txtTime2->setVisible(m_ui.txtTime1->isVisible());

	// 是否有扫荡结果未取;
	if (info->bSweepResult && !info->bIsSweep)
	{
		TowerController::getInstance()->handleTowerSweepSettlement();
	}
	else
	{
		TowerController::getInstance()->checkUISweepSettlement();
	}
}

void LayerTower::updateProgress()
{
	int nodeValue[5] = {0};
	calcProgressNode(TowerModel::getInstance()->getTowerMainInfo()->nTotalFloor, nodeValue, 5);
	for (int i = 0; i < 5; ++i)
	{
		m_ui.txtProgressNode[i]->setString(_TO_STR(nodeValue[i]));
		setTextAddStroke(m_ui.txtProgressNode[i], Color3B(0x44, 0x03, 0x03), 2);
	}
	m_ui.imgCursor->setPosition(calcCursorPosition(TowerModel::getInstance()->getTowerMainInfo()->nTotalFloor,
		TowerModel::getInstance()->getTowerMainInfo()->nCurFloor, nodeValue, 5));
}

void LayerTower::calcProgressNode( int total, int node[], int size )
{
	// 条件1：size = 5: 0%  20%  50%  80%  100%;
	// 条件2：对5取整;
	vector<float>  vcPercent;
	vcPercent.push_back(0.0f);
	vcPercent.push_back(0.2f);
	vcPercent.push_back(0.5f);
	vcPercent.push_back(0.8f);
	vcPercent.push_back(1.0f);
	assert(vcPercent.size() == size);
	node[0] = 1;
	node[size-1] = total;
	for (int i = 1; i < vcPercent.size()-1; ++i)
	{
		int baseValue = (int)(((float)total) * vcPercent.at(i));
		if (baseValue%5 != 0)
		{
			baseValue += (5-baseValue%5);
		}
		node[i] = baseValue;
	}
}

Vec2 LayerTower::calcCursorPosition(int total, int cur, int node[], int size)
{
	//assert(cur >= 1 && cur <= total);
	if (cur < 1)  cur = 1;
	if (cur > total)  cur = total;
	Vec2 pos(30.0f, m_ui.txtProgressNode[0]->getPosition().y);

	int index = -1;
	for (int i = 0; i < size; ++i)
	{
		if (node[i] == cur)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
	{
		vector<int> nodeValue(node, node+size);
		nodeValue.push_back(cur);
		sort(nodeValue.begin(), nodeValue.end());
		index = -1;
		for (int i = 0; i < nodeValue.size(); ++i)
		{
			if (nodeValue.at(i) == cur)
			{
				index = i;
				break;
			}
		}
		float singleLen = m_ui.imgProgressBg->getContentSize().height / (size-1);
		float percent = ((float)(cur - nodeValue.at(index-1)))/((float)(nodeValue.at(index+1)-nodeValue.at(index-1)));
		float len = singleLen*((float)(index-1)+percent);
		pos.y += len;// - m_ui.imgProgressBg->getContentSize().height/2.0f;
	}
	else
	{
		pos.y = m_ui.txtProgressNode[index]->getPosition().y;
	}

	return pos;
}

void LayerTower::initFloor( int nCurFloor )
{
	Size  winSize = Director::getInstance()->getWinSize();

	// 试炼当前层;
	//   奇数层 - 单个floor完全显示，另一个在屏幕外;
	//   偶数层 - 两个floor拼接;
	if (nCurFloor % 2 == 0)
	{
		m_nMaxFloorNum = nCurFloor + 2;
		m_pFloor1 = createFloor(nCurFloor - 1);
		m_pFloor1->setPosition(Vec2(0.0f, -winSize.height/2));
		this->addChild(m_pFloor1, 1);
		m_pFloor1->updateFloorState();

		if (!TowerModel::getInstance()->checkTowerFloorIsValid(m_nMaxFloorNum))
		{
			m_bIsMaxFloor = true;
			m_pFloor1->setPosition(Vec2(0.0f, 0.0f));
			return;
		}

		m_pFloor2 = createFloor(nCurFloor + 1);
		m_pFloor2->setPosition(Vec2(m_pFloor1->getPosition().x, m_pFloor1->getPosition().y + winSize.height));
		this->addChild(m_pFloor2, 1);
		m_pFloor2->updateFloorState();
	}
	else
	{
		m_nMaxFloorNum = nCurFloor + 3;
		m_pFloor1 = createFloor(nCurFloor);
		m_pFloor1->setPosition(Vec2(0.0f, 0.0f));
		this->addChild(m_pFloor1, 1);
		m_pFloor1->updateFloorState();

		if (!TowerModel::getInstance()->checkTowerFloorIsValid(m_nMaxFloorNum))
		{
			m_bIsMaxFloor = true;
			return;
		}

		m_pFloor2 = createFloor(nCurFloor + 2);
		m_pFloor2->setPosition(Vec2(0.0f, winSize.height));
		this->addChild(m_pFloor2, 1);
		m_pFloor2->updateFloorState();
	}
}

TowerFloor* LayerTower::createFloor( int nFloor )
{
	TowerFloor* _floor = TowerFloor::create(TOWER_TYPE_TOWER, nFloor);
	_floor->setParent(this);
	return _floor;
}

void LayerTower::scrollFloor()
{
	if (!m_bIsMaxFloor)
	{
		Size  visibleSize = Director::getInstance()->getVisibleSize();
		Action* seq1 = Sequence::create(EaseSineInOut::create(MoveBy::create(1.0f, Vec2(0.0f, -visibleSize.height/2))), nullptr);
		m_pFloor1->runAction(seq1);
		Action* seq2 = Sequence::create(EaseSineInOut::create(MoveBy::create(1.0f, Vec2(0.0f, -visibleSize.height/2))),
			CallFuncN::create(CC_CALLBACK_0(LayerTower::reCreateFloor, this)), nullptr);
		m_pFloor2->runAction(seq2);
	}
	else
	{
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20058));
	}

	// 更新楼层状态;
	if (m_pFloor1 != nullptr)
	{
		m_pFloor1->updateFloorState();
	}
	if (m_pFloor2 != nullptr)
	{
		m_pFloor2->updateFloorState();
	}
}

void LayerTower::reCreateFloor()
{
	Size  visibleSize = Director::getInstance()->getVisibleSize();

	// 循环创建;
	if (m_pFloor1->getPosition().y < -visibleSize.height)
	{
		Vec2 oriPos = m_pFloor1->getPosition();
		m_pFloor1->removeFromParent();
		if (!TowerModel::getInstance()->checkTowerFloorIsValid(m_nMaxFloorNum+1))
		{
			m_bIsMaxFloor = true;
			return;
		}
		m_pFloor1 = createFloor(m_nMaxFloorNum+2);
		m_pFloor1->setPosition(Vec2(oriPos.x, oriPos.y + visibleSize.height*2));
		this->addChild(m_pFloor1);
	}
	else if (m_pFloor2->getPosition().y < -visibleSize.height)
	{
		Vec2 oriPos = m_pFloor2->getPosition();
		m_pFloor2->removeFromParent();
		if (!TowerModel::getInstance()->checkTowerFloorIsValid(m_nMaxFloorNum+1))
		{
			m_bIsMaxFloor = true;
			return;
		}
		m_pFloor2 = createFloor(m_nMaxFloorNum+2);
		m_pFloor2->setPosition(Vec2(oriPos.x, oriPos.y + visibleSize.height*2));
		this->addChild(m_pFloor2);
	}
	else
	{
		return;
	}

	// 最大层数递增;
	m_nMaxFloorNum += 2;
}

void LayerTower::resetFloor()
{
	// 清除现有楼层信息;
	m_pFloor1->removeFromParentAndCleanup(true);
	m_pFloor2->removeFromParentAndCleanup(true);
	m_pFloor1 = nullptr;
	m_pFloor2 = nullptr;

	// 重新创建;
	initFloor(1);

	// 重置/扫荡按钮状态;
	//int nCurFloor = TowerModel::getInstance()->getTowerCurFloor();
	//int nMaxFloor = TowerModel::getInstance()->getTowerMaxFloor();
	m_ui.btnReset->setVisible(false);
	m_ui.btnSweep->setVisible(!m_ui.btnReset->isVisible());
	m_nInitCurFloor = 1;
}

void LayerTower::onFloorTargetClicked( TOWER_FIGHT_TYPE nFightType )
{
	// 扫荡中;
	if (!m_ui.btnSweep->isEnabled())
	{
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20059));
		return;
	}

	if (TowerModel::getInstance()->getTowerMainInfo()->bEnd)
	{
		return;
	}

	// 点击对象;
	if (nFightType == TFT_FIGHT)
	{
		// 调整阵型;
		Director::getInstance()->pushScene(SceneFormation::create(F_MODE_TOWER));

		// 进入战斗;
		//enterChallenge();
	}
}

/*
void LayerTower::onRechargeConfirm( Ref* pSender, CustomRetType type )
{
	if (type == RET_TYPE_RECHARGE)
	{
		// jump to recharge
	}
}
*/

/*
void LayerTower::onBuyChallengeTimesConfirm( Ref* pSender, CustomRetType type )
{
	if (type == RET_TYPE_OK)
	{
		// 购买;
		int nRet = TowerController::getInstance()->handleBuyChallengeTimes();
		if (nRet == -1)
		{
			// 可购买次数已用尽;
			CustomTips::show("今日已经不可再挑战，明天再试试吧");
		}
		else if (nRet == -2)
		{
			// 可购买，但元宝不足;
			CustomTips::show("元宝不足");
		}
	}
}
*/

void LayerTower::enterChallenge()
{
	Director::getInstance()->replaceScene(SceneFightLoading::create(FIGHT_TYPE_TOWER, TowerModel::getInstance()->getTowerCurBarrierId()));
	FightController::getInstance()->sendTowerFightMsg(TowerModel::getInstance()->getTowerCurBarrierId());
}

void LayerTower::updateReward()
{
	int nCurFloor = TowerModel::getInstance()->getTowerCurFloor();
	int nId = TowerModel::getInstance()->towerFloor2Id(nCurFloor);
	map<int, BarrierInfo>* _info = TowerModel::getInstance()->getTowerFloorInfo();
	auto iter = _info->find(nId);
	assert(iter != _info->end());

	// 每一层固定显示的经验和银币;
	m_ui.txtExp->setString(_TO_STR(iter->second.nExp));
	m_ui.txtCoin->setString(_TO_STR(iter->second.nCoin));

	// 每隔n层有宝箱奖励;
	m_ui.btnBox->setVisible(iter->second.mapRewardShow.size() > 0);
	Layout* _content = dynamic_cast<Layout*>(m_ui.imgRewardBg->getChildByName("Panel_12"));
	_content->removeAllChildren();
	if (iter->second.mapRewardShow.size() > 0)
	{
		int nIndex = 0;
		float fStartX = _content->getContentSize().width/6.0f;
		float fStartY = _content->getContentSize().height/2.0f;
		for (auto it = iter->second.mapRewardShow.begin(); it != iter->second.mapRewardShow.end(); ++it, ++nIndex)
		{
			UIToolHeader* _item = UIToolHeader::create(it->first);
			_item->setNumEx(it->second);
			_item->setPosition(Vec2(fStartX + 2*fStartX*nIndex, fStartY));
			_content->addChild(_item);

			// 查表，添加名称显示;
			RowData* row = DataManager::getInstance()->searchToolById(it->first);
			if (nullptr != row)
			{
				string strName = row->getStringData("itemName");
				Text* txtName = Text::create(strName, FONT_FZZHENGHEI, 16);
				txtName->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
				setTextAddStroke(txtName, Color3B(0x79, 0x03, 0x03), 2);
				txtName->setPosition(Point(0.0f, -65.0f) + _item->getPosition());
				_content->addChild(txtName);
			}
		}
	}
}

void LayerTower::onScrollViewEvent( Ref* pSender, ScrollView::EventType type )
{
	if (pSender == m_ui.scrollviewContent)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.scrollviewContent->getInnerContainerSize();
				Vec2 vec = m_ui.scrollviewContent->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.scrollviewContent->getContentSize().height);
				//log("training scrolling: x = %f, y = %f, per: %f", vec.x, vec.y, percentY);

				m_ui.scrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			break;

		default:
			break;
		}
	}
}

void LayerTower::playEnterAnimation()
{
	// 音效;
	SoundUtils::playSoundEffect("quanping");

	ActionCreator::runCommBtnAction(m_ui.btnRanking);
	ActionCreator::runCommBtnAction(m_ui.btnReward);
	ActionCreator::runCommBtnAction(m_ui.btnReset);
	ActionCreator::runCommBtnAction(m_ui.btnSweep);
}

void LayerTower::checkBuyCondition()
{
	// 1.当前剩余次数;
	if (TowerModel::getInstance()->getCurVipLeftTimes() > 0)
	{
		m_ui.pVipStateRoot->setVisible(true);
		return;
	}

	// 2.当前可提升次数;
	int nNextValue = 0;
	int nNextLv = TowerModel::getInstance()->getNextValidVip(nNextValue);
	if (nNextValue > TowerModel::getInstance()->getCurMaxTimes())
	{
		m_ui.pVipRoot->setVisible(true);
		
		// 弹出特权提升面板;
		ostringstream oss;
		oss << DataManager::getInstance()->searchCommonTexdtById(20060) << nNextValue << DataManager::getInstance()->searchCommonTexdtById(10049);
		m_ui.txtNextCount->setString(oss.str());
		oss.str("");
		m_ui.imgVipNext->loadTexture(getVipIcon(nNextLv));
		m_ui.pVipRoot->setVisible(true);
		SoundUtils::playSoundEffect("tanchuang");
		ActionCreator::runCommDlgAction(m_ui.imgVipBg);
		return;
	}

	// 不可再购买;
	CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20062));
}

std::string LayerTower::getVipIcon(int nVipLv)
{
	string strPath = "Image/UIVip/VipLv/img_vip_";
	strPath.append(_TO_STR(nVipLv));
	strPath.append(".png");
	return strPath;
}