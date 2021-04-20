#include "LayerTrail.h"
#include "TowerModel.h"
#include "TowerController.h"
#include "LayerOp.h"
#include "LayerTrailReward.h"
#include "LayerTreasure.h"
#include "LayerTrailRanking.h"
#include "Shopexc/LayerShopexc.h"
#include "FormationModule/FormationModel.h"
#include "GuideCenter/GuideManager.h"
#include "Widget/LayerCommHeadInfo.h"
#include "NewShopModule/ShopScene.h"

LayerTrail::LayerTrail(void)
	: m_pFloor1(nullptr)
	, m_pFloor2(nullptr)
	, m_layerBuff(nullptr)
	, m_nMaxFloorNum(4)
	, m_bIsMaxFloor(false)
	, m_nInitCurFloor(1)
	, m_nBuffCount(0)
	, m_bFirstEnter(true)
{
	m_mapBuff.clear();
	TowerModel::getInstance()->addObserver(this);
}


LayerTrail::~LayerTrail(void)
{
	TowerModel::getInstance()->removeObserver(this);
}

bool LayerTrail::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();

	// 先初始当前楼层;
	m_nInitCurFloor = TowerModel::getInstance()->getTrailCurFloor();
	if (m_nInitCurFloor > 1)
	{
		initFloor(m_nInitCurFloor);
	}

	return true;
}

void LayerTrail::onEnter()
{
	Layer::onEnter();

	// 音效;
	SoundUtils::playSoundEffect("quanping");

	if (m_bFirstEnter)
	{
		m_bFirstEnter = false;

		//隔一帧再发送协议，防止崩溃;
		auto callBack = [this](float dt)->void
		{
			// 等拿到最新楼层信息，再做滚动;
			TowerController::getInstance()->handleTailMainInfo();
			this->getScheduler()->unschedule("LayerTrail::onEnter", this);
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerTrail::onEnter");
		
	}
}

void LayerTrail::onExit()
{
	Layer::onExit();
}

void LayerTrail::initUI()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UINewtower/UINewtower_trail.ExportJson"));
	this->addChild(m_ui.pRoot, 2);

	// 通用标题栏;
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(
		[this](Ref* sender, Widget::TouchEventType type)->void
	{
		this->onBtnClicked(sender, type, Btn_Trail_Back);
	},
		HEAD_INFO_TYPE_SURVIVAL_TEST);
	this->addChild(_layerCommHeadInfo, 3);

	// 爬塔;
	m_ui.pnlTrail = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName(STRING(Pnl_Trail)));
	m_ui.pnlTrail->setContentSize(visibleSize);

	// 商店;
	m_ui.btnStore = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Store)));
	m_ui.btnStore->addTouchEventListener(CC_CALLBACK_2(LayerTrail::onBtnClicked, this, Btn_Store));

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
	
	// 重置;
	m_ui.btnTrailReset = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Trail_Reset)));
	m_ui.btnTrailReset->addTouchEventListener(CC_CALLBACK_2(LayerTrail::onBtnClicked, this, Btn_Trail_Reset));

	// 试炼UI重置入口取消(09/17/2015 Phil @zcjoy  [已跟服务器和策划确认过]);
	m_ui.btnTrailReset->setVisible(false);
	m_ui.btnTrailReset->setEnabled(false);

	// 积分/星数;
	m_ui.txtTrailScore = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Trail_Score)));
	m_ui.txtTrailScore->setFontName(FONT_FZZHENGHEI);
	m_ui.txtTrailStar = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Trail_Star)));
	m_ui.txtTrailStar->setFontName(FONT_FZZHENGHEI);

	// Buff;
	m_ui.btnBuff = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Buff)));
	m_ui.btnBuff->addTouchEventListener(CC_CALLBACK_2(LayerTrail::onBtnClicked, this, Btn_Buff));
	m_ui.imgArrow = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Arrow"));
	m_ui.imgArrow->setVisible(false);
	m_ui.imgBufTips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Buff_Tips"));
	m_ui.txtBuffNone = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Buff_None"));
	m_ui.txtBuffNone->setFontName(FONT_FZZHENGHEI);
	m_ui.txtBuffNone->setVisible(false);

	this->setVisible(false);
}

void LayerTrail::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		if (nWidgetName == Btn_Buff/* && m_nBuffCount > 0*/)
		{
			m_ui.imgArrow->setVisible(true);
		}

		switch (nWidgetName)
		{
		case Btn_Trail_Back:
		case Btn_Store:
		case Btn_Trail_Reset:
			SoundUtils::playSoundEffect("dianji");
			break;

		default:
			break;
		}
	}
	else if (type == Widget::TouchEventType::CANCELED)
	{
		if (nWidgetName == Btn_Buff)
		{
			m_ui.imgArrow->setVisible(false);
		}
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (nWidgetName)
		{
		case Btn_Trail_Back:
			{
				this->scheduleUpdate();
			}
			break;

		case Btn_Store:
			{
				Director::getInstance()->pushScene(ShopScene::createScene(ST_JIFENG,true));

				//LayerShopexc* _layerShop = LayerShopexc::create(SHOP_TYPE_TRAIL);
				//if (nullptr != _layerShop)
				//{
				//	this->addChild(_layerShop, 99);
				//}
			}
			break;

		case Btn_Buff:
			{
				if (nWidgetName == Btn_Buff)
				{
					m_ui.imgArrow->setVisible(false);
				}
			}
			break;

		case Btn_Trail_Reset:
			{
				// test;
				/*
				TrailMainInfo info;
				info.nCurFloor = 1;
				info.nGold = 100;
				info.nMaxFloor = 1;
				info.nScore = 98;
				info.nStar = 97;
				info.nTrailResetDate = 20150805;
				info.mapBuffEffect.insert(make_pair(9011, 0.05f));
				info.mapBuffEffect.insert(make_pair(9022, 0.06f));
				info.mapBuffEffect.insert(make_pair(9033, 0.07f));
				info.mapBuffEffect.insert(make_pair(9044, 0.08f));
				info.mapBuffEffect.insert(make_pair(9055, 0.09f));

				info.mapBuffName.insert(make_pair(9011, "攻击"));
				info.mapBuffName.insert(make_pair(9022, "物防"));
				info.mapBuffName.insert(make_pair(9033, "法防"));
				info.mapBuffName.insert(make_pair(9044, "加血"));
				info.mapBuffName.insert(make_pair(9055, "回血"));

				updateMainInfo(&info);
				*/

				/*
				// 先判断是否有剩余重置次数;
				if (TowerController::getInstance()->handlePreResetTrail())
				{
					CustomPop::show("重置后返回第一关，是否确认？", CC_CALLBACK_2(LayerTrail::onResetConfirm, this), CUSTOM_YES_NO);
				}
				else
				{
					// 没有重置次数，提示充值;
					CustomPop::show("今日重置次数用尽了，成为VIP每日可获得更多重置次数", CC_CALLBACK_2(LayerTrail::onRechargeConfirm, this), CUSTOM_RECHARGE_NO);
				}
				*/
			}
			break;

		default:
			break;
		}
	}
}


void LayerTrail::update( float delta )
{
	this->unscheduleUpdate();
	this->removeFromParent();
}

void LayerTrail::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_TRAIL_MAIN_INFO:
		{
			updateMainInfo((TrailMainInfo*)(param->updateData));
			this->setVisible(true);

			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_LIFE_PRACTICE);
		}
		break;

	case OBS_PARAM_TYPE_TRAIL_FLOOR:
		{
			if (m_pFloor1 != nullptr || m_pFloor2 != nullptr)
			{
				if (TowerModel::getInstance()->getTrailCurFloor() == m_nInitCurFloor + 1)
				{
					// 滚动;
					scrollFloor();
				}
				else if (m_nInitCurFloor != 1 && TowerModel::getInstance()->getTrailMainInfo()->bEnd)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20063));
				}
			}
			else
			{
				initFloor(*((int*)(param->updateData)));
			}
		}
		break;

	case OBS_PARAM_TYPE_TRAIL_RESET:
		{
			resetFloor();
		}
		break;

	case OBS_PARAM_TYPE_TRAIL_SCORE:
		{
			m_ui.txtTrailScore->setString(StringFormat::intToStr(*((int*)(param->updateData))));
			setTextAddStroke(m_ui.txtTrailScore, Color3B(0x45, 0x05, 0x05), 2);
		}
		break;

	default:
		break;
	}
}

void LayerTrail::updateMainInfo( TrailMainInfo* info )
{
	ostringstream oss;
	oss.str("");

	// 积分;
	oss << info->nScore;
	m_ui.txtTrailScore->setString(oss.str());
	setTextAddStroke(m_ui.txtTrailScore, Color3B(0x45, 0x05, 0x05), 2);
	oss.str("");

	// 星;
	oss << info->nStar;
	m_ui.txtTrailStar->setString(oss.str());
	oss.str("");

	// 进度;
	updateProgress();

	// clear all;
	if (!m_mapBuff.empty())
	{
		for (auto iter : m_mapBuff)
		{
			iter.second->removeFromParent();
		}
		m_mapBuff.clear();
	}

	// buff;
	m_nBuffCount = info->mapBuffEffect.size();
	if (m_nBuffCount <= 0)
	{
		m_ui.txtBuffNone->setVisible(true);
	}
	else
	{
		m_ui.txtBuffNone->setVisible(false);
		if (info->mapBuffEffect.size() > 2)
		{
			m_ui.imgBufTips->setContentSize(Size(190.0f, 100.0f + (info->mapBuffEffect.size()-2)*30.0f));
		}

		m_ui.imgBufTips->removeAllChildren();
		int nCount = 0;
		for (auto iterEffect = info->mapBuffEffect.begin(); iterEffect != info->mapBuffEffect.end(); ++iterEffect, ++nCount)
		{
			Text* _txtName = Text::create(iterEffect->first, FONT_FZZHENGHEI, 20);
			_txtName->setColor(Color3B(0xF8, 0xEC, 0xAB));
			_txtName->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			_txtName->setContentSize(Size(65.0f, 25.0f));
			_txtName->setPosition(Vec2(25.0f, m_ui.imgBufTips->getContentSize().height - 45.0f - nCount*30.0f));
			m_ui.imgBufTips->addChild(_txtName);

			oss.str("");
			if (iterEffect->second > -0.000001)
			{
				oss << "+";
			}
			oss << iterEffect->second * 100.0f << "%";
			Text* _txtEffect = Text::create(oss.str(), FONT_FZZHENGHEI, 20);
			_txtEffect->setColor(Color3B(0x57, 0xE1, 0x58));
			_txtEffect->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			_txtEffect->setContentSize(Size(65.0f, 25.0f));
			_txtEffect->setPosition(Point(85.0f, 0.0f) + _txtName->getPosition());
			m_ui.imgBufTips->addChild(_txtEffect);
		}
	}
}

void LayerTrail::updateProgress()
{
	int nodeValue[5] = {0};
	calcProgressNode(TowerModel::getInstance()->getTrailMainInfo()->nTotalFloor, nodeValue, 5);
	for (int i = 0; i < 5; ++i)
	{
		m_ui.txtProgressNode[i]->setString(_TO_STR(nodeValue[i]));
		setTextAddStroke(m_ui.txtProgressNode[i], Color3B(0x44, 0x03, 0x03), 2);
	}
	m_ui.imgCursor->setPosition(calcCursorPosition(TowerModel::getInstance()->getTrailMainInfo()->nTotalFloor,
		TowerModel::getInstance()->getTrailMainInfo()->nCurFloor, nodeValue, 5));
}

void LayerTrail::calcProgressNode( int total, int node[], int size )
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

Vec2 LayerTrail::calcCursorPosition(int total, int cur, int node[], int size)
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
		pos.y -= len;// - m_ui.imgProgressBg->getContentSize().height/2.0f;
	}
	else
	{
		pos.y = m_ui.txtProgressNode[index]->getPosition().y;
	}

	return pos;
}

void LayerTrail::initFloor( int nCurFloor )
{
	Size  visibleSize = Director::getInstance()->getVisibleSize();

	// 试炼当前层;
	//   奇数层 - 单个floor完全显示，另一个在屏幕外;
	//   偶数层 - 两个floor拼接;

	if (nCurFloor % 2 == 0)
	{
		m_nMaxFloorNum = nCurFloor + 2;
		m_pFloor1 = createFloor(nCurFloor - 1);
		m_pFloor1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		m_pFloor1->setPosition(Vec2(0.0f, visibleSize.height/2));
		this->addChild(m_pFloor1, 1);
		m_pFloor1->updateFloorState();

		if (!TowerModel::getInstance()->checkTrailFloorIsValid(m_nMaxFloorNum))
		{
			m_bIsMaxFloor = true;
			m_pFloor1->setPosition(Vec2(0.0f, 0.0f));
			return;
		}

		m_pFloor2 = createFloor(nCurFloor + 1);
		m_pFloor2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		m_pFloor2->setPosition(Vec2(m_pFloor1->getPosition().x, m_pFloor1->getPosition().y - visibleSize.height));
		this->addChild(m_pFloor2, 1);
		m_pFloor2->updateFloorState();
	}
	else
	{
		m_nMaxFloorNum = nCurFloor + 3;
		m_pFloor1 = createFloor(nCurFloor);
		m_pFloor1->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		m_pFloor1->setPosition(Vec2(0.0f, 0.0f));
		this->addChild(m_pFloor1, 1);
		m_pFloor1->updateFloorState();

		if (!TowerModel::getInstance()->checkTrailFloorIsValid(m_nMaxFloorNum))
		{
			m_bIsMaxFloor = true;
			return;
		}

		m_pFloor2 = createFloor(nCurFloor + 2);
		m_pFloor2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		m_pFloor2->setPosition(Vec2(0.0f, -visibleSize.height));
		this->addChild(m_pFloor2, 1);
		m_pFloor2->updateFloorState();
	}

	// 触发buff;
	if (TowerModel::getInstance()->isBuffTriggered())
	{
		m_layerBuff = LayerOp::create(POP_WND_TYPE_BUFF);
		m_layerBuff->setParent(this);
		this->addChild(m_layerBuff, 4);
	}
}

TowerFloor* LayerTrail::createFloor( int nFloor )
{
	TowerFloor* _floor = TowerFloor::create(TOWER_TYPE_TRAIL, nFloor);
	_floor->setParent(this);
	return _floor;
}

void LayerTrail::onLayerOpClose(POP_WND_TYPE wndType)
{
	if (wndType == POP_WND_TYPE_BUFF)
	{
		scrollFloor();
	}
}

void LayerTrail::onLayerTrailRewardClose()
{
	// 判定是否触发宝藏;
	if (TowerController::getInstance()->handleRewardClose())
	{
		LayerTreasure* _layer = LayerTreasure::create();
		_layer->setParent(this);
		this->addChild(_layer, 5);
	}
	else
	{
		scrollFloor();
	}
}

void LayerTrail::onLayerTreasureClose()
{
	scrollFloor();
}

void LayerTrail::scrollFloor()
{
	// 滚动楼层;
	if (!m_bIsMaxFloor)
	{
		Size  visibleSize = Director::getInstance()->getVisibleSize();
		Action* seq1 = Sequence::create(EaseSineInOut::create(MoveBy::create(1.0f, Vec2(0.0f, visibleSize.height/2))), nullptr);
		if (m_pFloor1 != nullptr)
		{
			m_pFloor1->runAction(seq1);
		}
		Action* seq2 = Sequence::create(EaseSineInOut::create(MoveBy::create(1.0f, Vec2(0.0f, visibleSize.height/2))),
			CallFuncN::create(CC_CALLBACK_0(LayerTrail::reCreateFloor, this)), nullptr);
		if (m_pFloor2 != nullptr)
		{
			m_pFloor2->runAction(seq2);
		}
	}
	else
	{
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20064));
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

void LayerTrail::reCreateFloor()
{
	Size  visibleSize = Director::getInstance()->getVisibleSize();

	// 循环创建;
	if (m_pFloor1->getPosition().y > visibleSize.height - 10.0f)
	{
		Vec2 oriPos = m_pFloor1->getPosition();
		m_pFloor1->removeFromParent();
		if (!TowerModel::getInstance()->checkTrailFloorIsValid(m_nMaxFloorNum+1))
		{
			m_bIsMaxFloor = true;
			return;
		}
		m_pFloor1 = createFloor(m_nMaxFloorNum+1);
		m_pFloor1->setPosition(Vec2(oriPos.x, oriPos.y - visibleSize.height*2));
		this->addChild(m_pFloor1);
	}
	else if (m_pFloor2->getPosition().y > visibleSize.height - 10.0f)
	{
		Vec2 oriPos = m_pFloor2->getPosition();
		m_pFloor2->removeFromParent();
		if (!TowerModel::getInstance()->checkTrailFloorIsValid(m_nMaxFloorNum+1))
		{
			m_bIsMaxFloor = true;
			return;
		}
		m_pFloor2 = createFloor(m_nMaxFloorNum+1);
		m_pFloor2->setPosition(Vec2(oriPos.x, oriPos.y - visibleSize.height*2));
		this->addChild(m_pFloor2);
	}
	else
	{
		return;
	}

	// 最大层数递增;
	m_nMaxFloorNum += 2;
	
}

void LayerTrail::onFloorTargetClicked( TOWER_FIGHT_TYPE nFightType )
{
	if (TowerModel::getInstance()->getTrailMainInfo()->bEnd)
	{
		return;
	}

	if (nFightType == TFT_FIGHT)
	{
		LayerOp* _layer = LayerOp::create(POP_WND_TYPE_OP);
		_layer->setParent(this);
		this->addChild(_layer, 3);
	}
	else if (nFightType == TFT_REWARD)
	{
		TowerController::getInstance()->handleTrailReward();
		LayerTrailReward* _layer = LayerTrailReward::create();
		_layer->setParent(this);
		this->addChild(_layer, 9);
	}

	
}

/*
void LayerTrail::onResetConfirm( Ref* pSender, CustomRetType type )
{
	if (type == RET_TYPE_OK)
	{
		TowerController::getInstance()->handleResetTrail();
	}
}
*/

void LayerTrail::resetFloor()
{
	// 过场动画;

	// 清除现有楼层信息;
	m_pFloor1->removeFromParentAndCleanup(true);
	m_pFloor2->removeFromParentAndCleanup(true);
	m_pFloor1 = nullptr;
	m_pFloor2 = nullptr;

	// 重新创建;
	initFloor(1);
}
/*

void LayerTrail::onRechargeConfirm( Ref* pSender, CustomRetType type )
{
	if (type == RET_TYPE_RECHARGE)
	{
		// jump to recharge
	}
}
*/
