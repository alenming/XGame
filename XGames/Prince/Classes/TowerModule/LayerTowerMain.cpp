#include "LayerTowerMain.h"
#include "MainModule/MainCityScene.h"
#include "LayerTower.h"
#include "LayerTrail.h"
#include "GuideCenter/GuideManager.h"
#include "Widget/LayerCommHeadInfo.h"
#include "Widget/ActionCreator.h"
#include "TowerModel.h"
#include "TowerController.h"
#include "Widget/LayerGameRules.h"


LayerTowerMain::LayerTowerMain(void)
	: m_preType(TOWER_TYPE_INVALID)
	, m_imgSweep(nullptr)
	, m_bIsFirstEnter(true)
	, mIsPushScene(false)
{
	TowerModel::getInstance()->addObserver(this);
}


LayerTowerMain::~LayerTowerMain(void)
{
	TowerModel::getInstance()->removeObserver(this);
}

LayerTowerMain* LayerTowerMain::create(TOWER_TYPE type, bool pushScene)
{
	LayerTowerMain* _layer = new LayerTowerMain;
	if (nullptr != _layer && _layer->init(type, pushScene))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

void LayerTowerMain::onEnter()
{
	Layer::onEnter();

	if (m_preType == TOWER_TYPE_TOWER)
	{
		m_bIsFirstEnter = false;
		//onBtnClicked(this, Widget::TouchEventType::ENDED, Btn_Enter_Tower);
		LayerTower* _layer = LayerTower::create();
		this->addChild(_layer, 4);
	}
	else if (m_preType == TOWER_TYPE_TRAIL)
	{
		m_bIsFirstEnter = false;
		//onBtnClicked(this, Widget::TouchEventType::ENDED, Btn_Enter_Trail);
		LayerTrail* _layer = LayerTrail::create();
		this->addChild(_layer, 4);
	}
	else
	{
		if (m_bIsFirstEnter)
		{
			m_bIsFirstEnter = false;

			// 正常进场动画;
			playEnterAnimation();

			// 音效;
			SoundUtils::playSoundEffect("quanping");

			//隔一帧再发送协议，防止崩溃;
			auto callBack = [this](float dt)->void
			{
				TowerController::getInstance()->handleTowerMainInfo();
				this->getScheduler()->unschedule("LayerTowerMain::onEnter", this);
			};
			getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerTowerMain::onEnter");
		}
	}

	m_preType = TOWER_TYPE_INVALID;
}


void LayerTowerMain::onExit()
{
	// 释放资源;
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UINewtower");

	TowerController::getInstance()->onTowerViewExit();

	Layer::onExit();
}

bool LayerTowerMain::init(TOWER_TYPE type, bool pushScene)
{
	if (!Layer::init())
	{
		return false;
	}
	m_preType = type;
	mIsPushScene = pushScene;

	initUI();
	return true;
}


void LayerTowerMain::initUI()
{
	Size size = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 pos = Director::getInstance()->getVisibleOrigin();

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UINewtower/UINewtower_main.ExportJson"));

	// 通用标题栏;
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(
		[this](Ref* sender, Widget::TouchEventType type)->void
	{
		this->onBtnClicked(sender, type, Btn_Main_Back);
	},
		HEAD_INFO_TYPE_TOWER);
	this->addChild(_layerCommHeadInfo, 3);

	// PnlMain;
	m_ui.pnlMain = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName(STRING(Pnl_Main)));
	m_ui.pnlMain->setContentSize(visibleSize);
	m_ui.pnlMain->setContentSize(visibleSize);
	m_ui.imgMainBg = ImageView::create("Image/Bg/img_bg_tower_main.jpg");
	m_ui.imgMainBg->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_ui.imgMainBg->setPosition(Vec2(-pos.x, 0.0f));
	m_ui.pRoot->addChild(m_ui.imgMainBg, 1);
	m_ui.imgTowerTitle = dynamic_cast<ImageView*>(m_ui.pnlMain->getChildByName("Img_Tower_Title"));
	m_ui.btnEnterTower = dynamic_cast<Button*>(m_ui.pnlMain->getChildByName(STRING(Btn_Enter_Tower)));
	m_ui.btnEnterTower->addTouchEventListener(CC_CALLBACK_2(LayerTowerMain::onBtnClicked, this, Btn_Enter_Tower));
	m_ui.imgTrailTitle = dynamic_cast<ImageView*>(m_ui.pnlMain->getChildByName("Img_Trail_Title"));
	m_ui.btnEnterTrail = dynamic_cast<Button*>(m_ui.pnlMain->getChildByName(STRING(Btn_Enter_Trail)));
	m_ui.btnEnterTrail->addTouchEventListener(CC_CALLBACK_2(LayerTowerMain::onBtnClicked, this, Btn_Enter_Trail));
	m_ui.imgTrailEnterLv = dynamic_cast<ImageView*>(m_ui.pnlMain->getChildByName("Img_Level"));
	m_ui.imgTrailEnterLv->setVisible(!GuideManager::getInstance()->isFunctionOpen(FUNCTION_TRAIL, false));
	m_ui.btnHelp = dynamic_cast<Button*>(m_ui.pnlMain->getChildByName(STRING(Btn_Help)));
	m_ui.btnHelp->addTouchEventListener(CC_CALLBACK_2(LayerTowerMain::onBtnClicked, this, Btn_Help));

	this->addChild(m_ui.pRoot);
}


void LayerTowerMain::updateSelf(void* data)
{
	if (nullptr == data)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) data;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_TOWER_MAIN_INFO:
		{
			// 接收此消息只是为了显示扫荡状态，不显示倒计时;
			TowerMainInfo* _info = (TowerMainInfo*)(param->updateData);

			// 扫荡中;
			if (_info->bIsSweep)
			{
				if (m_imgSweep == nullptr)
				{
					m_imgSweep = ImageView::create("Image/UINewtower/Icon/img_saodangzhong.png");
					//m_imgSweep->setPosition(Vec2(this->getContentSize().width/2.0f, 520.0f));
					m_ui.pnlMain->addChild(m_imgSweep, 3);

					RelativeLayoutParameter* param = RelativeLayoutParameter::create();
					param->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_TOP_CENTER_HORIZONTAL);
					param->setMargin(Margin(0.0f, 200.0f, 0.0f, 0.0f));
					LayoutParameter* layoutParam = dynamic_cast<LayoutParameter*>(param);
					m_imgSweep->setLayoutParameter(layoutParam);
				}
				else
				{
					m_imgSweep->loadTexture("Image/UINewtower/Icon/img_saodangzhong.png");
				}
			}
			// 扫荡结束;
			else
			{
				// 有结果未取;
				if (_info->bSweepResult)
				{
					if (m_imgSweep == nullptr)
					{
						m_imgSweep = ImageView::create("Image/UINewtower/Icon/img_saodangwancheng.png");
						//m_imgSweep->setPosition(Vec2(this->getContentSize().width/2.0f, 520.0f));
						m_ui.pnlMain->addChild(m_imgSweep, 3);

						RelativeLayoutParameter* param = RelativeLayoutParameter::create();
						param->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_TOP_CENTER_HORIZONTAL);
						param->setMargin(Margin(0.0f, 200.0f, 0.0f, 0.0f));
						LayoutParameter* layoutParam = dynamic_cast<LayoutParameter*>(param);
						m_imgSweep->setLayoutParameter(layoutParam);
					}
				}
				else
				{
					if (m_imgSweep != nullptr)
					{
						m_imgSweep->removeFromParent();
						m_imgSweep = nullptr;
					}
				}
			}
		}
		break;

	case OBS_PARAM_TYPE_TOWER_SWEEP_COUNT_DOWN:
		{
			int sec = *((int*)(param->updateData));
			if (sec > 0)
			{
				if (m_imgSweep == nullptr)
				{
					m_imgSweep = ImageView::create("Image/UINewtower/Icon/img_saodangzhong.png");
					//m_imgSweep->setPosition(Vec2(this->getContentSize().width/2.0f, 520.0f));
					m_ui.pnlMain->addChild(m_imgSweep, 3);

					RelativeLayoutParameter* param = RelativeLayoutParameter::create();
					param->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_TOP_CENTER_HORIZONTAL);
					param->setMargin(Margin(0.0f, 200.0f, 0.0f, 0.0f));
					LayoutParameter* layoutParam = dynamic_cast<LayoutParameter*>(param);
					m_imgSweep->setLayoutParameter(layoutParam);
				}
			}
			else
			{
				if (m_imgSweep == nullptr)
				{
					m_imgSweep = ImageView::create("Image/UINewtower/Icon/img_saodangwancheng.png");
					//m_imgSweep->setPosition(Vec2(this->getContentSize().width/2.0f, 520.0f));
					m_ui.pnlMain->addChild(m_imgSweep, 3);

					RelativeLayoutParameter* param = RelativeLayoutParameter::create();
					param->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_TOP_CENTER_HORIZONTAL);
					param->setMargin(Margin(0.0f, 200.0f, 0.0f, 0.0f));
					LayoutParameter* layoutParam = dynamic_cast<LayoutParameter*>(param);
					m_imgSweep->setLayoutParameter(layoutParam);
				}
				else
				{
					m_imgSweep->loadTexture("Image/UINewtower/Icon/img_saodangwancheng.png");
				}
			}
		}
		break;

	case OBS_PARAM_TYPE_TOWER_UI_SWEEP_SETTLEMENT:
		{
			if (m_imgSweep != nullptr)
			{
				m_imgSweep->removeFromParent();
				m_imgSweep = nullptr;
			}
		}
		break;

	default:
		break;
	}
}


void LayerTowerMain::onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::MOVED)
	{
	}
	else if (type == Widget::TouchEventType::CANCELED)
	{
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (nWidgetName)
		{
		case Btn_Main_Back:
			{
				if (mIsPushScene)
				{
					Director::getInstance()->popScene();
				}
				else
				{
					Director::getInstance()->replaceScene(MainCityScene::create());
				}
			}
			break;

		case Btn_Enter_Tower:
			{
				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TOWER))
				{
					LayerTower* _layer = LayerTower::create();
					this->addChild(_layer, 4);
				}
			}
			break;

		case Btn_Enter_Trail:
			{
				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TRAIL))
				{
					LayerTrail* _layer = LayerTrail::create();
					this->addChild(_layer, 4);
				}
			}
			break;

		case Btn_Help:
			{
				LayerGameRules* _help = LayerGameRules::create(SHI_LIAN_RULE_ID);
				if (nullptr != _help)
				{
					this->addChild(_help, 5);
					ActionCreator::runCommDlgAction(_help);
				}
			}
			break;

		default:
			break;
		}
	}

}

void LayerTowerMain::playEnterAnimation()
{
	// 标题文字淡入;
	m_ui.imgTowerTitle->setOpacity(0);
	m_ui.imgTowerTitle->runAction(EaseSineIn::create(FadeIn::create(0.4f)));
	m_ui.imgTrailTitle->setOpacity(0);
	m_ui.imgTrailTitle->runAction(EaseSineIn::create(FadeIn::create(0.4f)));
}

void LayerTowerMain::checkSweep()
{

}
