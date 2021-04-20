#include "LayerSweep.h"
#include "MainModule/MainCityScene.h"
#include "GuideCenter/GuideManager.h"
#include "LevelUp/LevelUpController.h"
#include "Widget/ActionCreator.h"
#include "Temp/CustomGoldTips.h"
#include "MysteryShop/MysteryInterLayer.h"
#include "MysteryShop/MysteryShopModel.h"

LayerSweep::LayerSweep(void)
	: m_nConsume(0)
	, m_nBaseTimes(1)
	, m_nRealTimes(0)
	, m_fTmpHeight(0.0f)
	, m_parent(nullptr)
{
	ChapterModel::getInstance()->addObserver(this);
	
}


LayerSweep::~LayerSweep(void)
{
	ChapterModel::getInstance()->removeObserver(this);
}

LayerSweep* LayerSweep::create(int nConsume, int nBaseCount, int nRealCount, int nFlag/* = 0*/)
{
	LayerSweep* pSweep = new LayerSweep;
	if (nullptr != pSweep && pSweep->init(nConsume, nBaseCount, nRealCount, nFlag))
	{
		pSweep->autorelease();
		return pSweep;
	}

	CC_SAFE_DELETE(pSweep);
	return nullptr;
}

bool LayerSweep::init(int nConsume, int nBaseCount, int nRealCount, int nFlag/* = 0*/)
{
	if (!Layer::init())
	{
		return false;
	}

	m_nConsume = nConsume;
	m_nBaseTimes = nBaseCount;
	m_nRealTimes = nRealCount;
	m_nFlag = nFlag;

	initUI();
	return true;
}


void LayerSweep::onEnter()
{
	Layer::onEnter();

	// 开始扫荡;
	string strReason;
	bool bRet = ChapterController::getInstance()->handleStartSweep(m_nRealTimes, strReason, m_nFlag);
	if (!bRet)
	{
		CustomPop::showUtf8(strReason);
	}

	// 音效;
	SoundUtils::playSoundEffect("tanchuang");

	ImageView* imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Bg"));
	ActionCreator::runCommDlgAction(imgBg);
}

void LayerSweep::onExit()
{
	// 释放资源;
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIBarrier");

	Layer::onExit();
}

void LayerSweep::initUI()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 加载扫荡UI;
	string strFileName;
	strFileName.assign("Image/UIBarrier/UISweep.ExportJson");

	///// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.pRoot->setSize(visibleSize);
	m_ui.pRoot->setPosition(pos);
	this->addChild(m_ui.pRoot);

	///// 扫荡面板;
	m_ui.pnlSweep = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName(STRING(Pnl_Sweep)));
	m_ui.btnSweepClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Close)));
	m_ui.btnSweepClose->addTouchEventListener(CC_CALLBACK_2(LayerSweep::onBtnClicked, this, Btn_Close));
	m_ui.btnSweepClose->setVisible(false);

	m_ui.imgScrollBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Img_Scroll_Bg)));
	m_ui.scrollviewSettlement = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(ScrollView_Settlement)));
	m_ui.scrollviewSettlement->addEventListener(CC_CALLBACK_2(LayerSweep::onScrollViewEvent, this));
	m_ui.scrollBar = CustomScrollBar::create();
	m_ui.imgScrollBg->addChild(m_ui.scrollBar);
	m_ui.scrollBar->setPosition(Vec2(35.0f, 35.0f));
	m_ui.scrollBar->initScrollBar(m_ui.scrollviewSettlement->getContentSize().height);
	m_ui.txtPower = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Power)));
	m_ui.txtPower->setFontName(FONT_NAME_FOUNDER_BOLD);
	string strText;
	strText.assign(StringFormat::intToStr(MainModel::getInstance()->getMainParam()->mPower).c_str());
	m_ui.txtPower->setString(STR_UTF8(strText));
	m_ui.txtEnergy = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Energy)));
	m_ui.txtEnergy->setFontName(FONT_NAME_FOUNDER_BOLD);
	strText.clear();
	strText.assign(StringFormat::intToStr(ToolModel::getInstance()->searchNumByTemplateId(10010)).c_str());
	m_ui.txtEnergy->setString(STR_UTF8(strText));
	m_ui.btnContinue = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Continue)));
	m_ui.btnContinue->addTouchEventListener(CC_CALLBACK_2(LayerSweep::onBtnClicked, this, Btn_Continue));
	m_ui.btnContinue->setVisible(false);
}


void LayerSweep::update(float delta)
{
	this->unscheduleUpdate();
	this->removeFromParent();
}


void LayerSweep::updateSelf(void* data)
{
	if (nullptr == data)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) data;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_SWEEP_STATE:
		{
			SweepState* state = (SweepState*)param->updateData;

			// 正在扫荡;
			if (state->bSweeping)
			{
			}
			// 扫荡结束;
			else
			{
				m_fTmpHeight = 0.0f;
				m_ui.btnSweepClose->setVisible(true);
				m_ui.btnContinue->setVisible(true);

				// 升级检测;
				bool isLevelUp = LevelUpController::getInstance()->checkLevelUp();

				//神秘商店(如果遇到升级情况,此界面在升级界面层次之上)
				if (!isLevelUp &&MysteryShopModel::getInstance()->getIsInterShow() && MysteryShopModel::getInstance()->getIsMainShow())
				{
					//没有升级,触发了神秘商店
					Director::getInstance()->getRunningScene()->addChild(MysteryInterLayer::create(),this->getLocalZOrder()+1);
				}
				else if (isLevelUp && MysteryShopModel::getInstance()->getIsInterShow() && MysteryShopModel::getInstance()->getIsMainShow())
				{
					//升级了又触发了关闭关卡界面显示
					MysteryShopModel::getInstance()->setIsInterShow(false); 
				}

			}
		}
		break;

	case OBS_PARAM_TYPE_SWEEP_SETTLEMENT:
		{
			// 更新展示;
			updateSweepSettlement(*((vector<SweepSettlement>*)param->updateData));

			// 同时更新当前体力，因为每次扫荡完成都有主角/英雄属性变化;
			int nCurPower = MainModel::getInstance()->getMainParam()->mPower;
			m_ui.txtPower->setString(_TO_STR(nCurPower));

			// 更新扫荡券数量;
			int nCurSweepTicket = ToolModel::getInstance()->searchNumByTemplateId(10010);
			m_ui.txtEnergy->setString(_TO_STR(nCurSweepTicket));

			// 更新体力和挑战次数;
			if (m_parent != nullptr)
			{
				m_parent->updatePower(nCurPower);
				m_parent->updateDayCount();
			}
		}
		break;

	default:
		break;
	}

}


void LayerSweep::onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName)
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switch (nWidgetName)
		{
		case Btn_Close:
			{
				this->scheduleUpdate();
			}
			break;

		case Btn_Continue:
			{
				// 开始新的扫荡;
				string strReason;
				int nFinalCount = 0;
				int nRet = ChapterController::getInstance()->handlePreStartSweep(m_nBaseTimes, strReason, nFinalCount);
				if (nRet >= 0)
				{
					if (!ChapterController::getInstance()->handleStartSweep(nFinalCount, strReason))
					{
						CustomPop::showUtf8(strReason);
						return;
					}

					// 清掉原有内容;
					m_ui.scrollviewSettlement->getInnerContainer()->removeAllChildren();

					m_ui.btnContinue->setVisible(false);
					m_ui.btnSweepClose->setVisible(false);
				}
				else if (nRet == -2)
				{
					// 扫荡券不足;
					int nPrice = BarrierModel::getInstance()->getSweepPrice();
					ostringstream oss;
					oss << DataManager::getInstance()->searchCommonTexdtById(10051) << 
						nFinalCount*nPrice << DataManager::getInstance()->searchCommonTexdtById(10052) << nFinalCount
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
								string strReasonnn;
								if (!ChapterController::getInstance()->handleStartSweep(nFinalCount, strReasonnn, 1))
								{
									CustomPop::showUtf8(strReasonnn);
									return;
								}

								// 清掉原有内容;
								m_ui.scrollviewSettlement->getInnerContainer()->removeAllChildren();

								m_ui.btnContinue->setVisible(false);
								m_ui.btnSweepClose->setVisible(false);
							}
						}
					}, CUSTOM_YES_NO);
				}
				// 体力不足;
				else if (nRet == -3)
				{
					LayerCommonBuy* buyPower = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
					Director::getInstance()->getRunningScene()->addChild(buyPower);
				}
				// 剩余次数不足;
				else if (nRet == -4)
				{
					if (m_parent != nullptr)
					{
						m_parent->checkBuyCondition();
					}
				}
				else
				{
					if (strReason != "")
					{
						CustomPop::showUtf8(strReason);
					}
				}
			}
			break;

		default:
			break;

		} // end switch

	} // end if
}

void LayerSweep::updateSweepSettlement(vector<SweepSettlement> vcSettlement)
{
	//log("===== sweep settlement, display point 1 =====");
	// 当前已经显示的个数;
	int nCount = (int)m_ui.scrollviewSettlement->getInnerContainer()->getChildrenCount();

	// 更新内容上去;
	float fOriHeight = 2*SWEEP_SETTLEMENT_CHILD_HEIGHT;
	for (unsigned int i = nCount; i < vcSettlement.size(); i++)
	{
		//log("===== sweep settlement, display point 2 =====");
		SweepSettlementNode* _node = SweepSettlementNode::create(vcSettlement.at(i));
		if (nullptr != _node)
		{
			// scrollView resize;
			float scrollViewWidth = 550.0f;
			float nSingleHeight = SWEEP_SETTLEMENT_CHILD_HEIGHT;
			int ntmp = (vcSettlement.at(i).vcDropItems.size() > 1) ? (vcSettlement.at(i).vcDropItems.size() - 1) : 1;
			nSingleHeight += (ntmp/5) * 100.0f;
			m_fTmpHeight += nSingleHeight;
			_node->setSingleHeight(nSingleHeight);
			float scrollViewHeight = (m_fTmpHeight > fOriHeight) ? m_fTmpHeight : fOriHeight;
			m_ui.scrollviewSettlement->setInnerContainerSize(Size(scrollViewWidth, scrollViewHeight));

			// add child;
			_node->setTag(i);
			m_ui.scrollviewSettlement->addChild(_node);

			//log("===== sweep settlement, display point 3 =====");

			// reorder;
			float fTotalHeight = 0.0f;
			for (unsigned int k = 0; k < nCount+1; k++)
			{
				Size innerSize = m_ui.scrollviewSettlement->getInnerContainerSize();
				SweepSettlementNode* _child = dynamic_cast<SweepSettlementNode*>(m_ui.scrollviewSettlement->getInnerContainer()->getChildByTag(k));
				_child->setPosition(Vec2(innerSize.width / 2.0f, innerSize.height - fTotalHeight));
				fTotalHeight += _child->getSingleHeight();
			}
		}
	}

	//log("===== sweep settlement, display point 4 =====");

	// 滚动至底部，显示最新的一次结果;
	m_ui.scrollviewSettlement->scrollToBottom(0.2f, true);

	// 刷新滚动条;
	m_ui.scrollBar->setScrollBarLengthPercent(m_ui.scrollviewSettlement->getContentSize().height / m_ui.scrollviewSettlement->getInnerContainerSize().height);
	m_ui.scrollBar->setScrollBarPercentY(100.0f);

}

void LayerSweep::onScrollViewEvent( Ref* pSender, ScrollView::EventType type )
{
	if (pSender == m_ui.scrollviewSettlement)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.scrollviewSettlement->getInnerContainerSize();
				Vec2 vec = m_ui.scrollviewSettlement->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.scrollviewSettlement->getContentSize().height);
				//log("formation scrolling: x = %f, y = %f, per: %f", vec.x, vec.y, percentY);

				m_ui.scrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			break;

		default:
			break;
		}
	}
}
