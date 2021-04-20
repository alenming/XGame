#include "LayerTreasure.h"
#include "MainModule/MainModel.h"
#include "TowerModel.h"
#include "TowerController.h"
#include "Temp/CustomTips.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "Temp/CustomGoldTips.h"

TreasureNode::TreasureNode()
	: m_bIsOpend(false)
	, m_nCount(0)
{
}

TreasureNode::~TreasureNode()
{

}

TreasureNode* TreasureNode::create(int nTemplateId, int nCount, string strName)
{
	TreasureNode* _node = new TreasureNode;
	if (nullptr != _node && _node->init(nTemplateId, nCount, strName))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool TreasureNode::init(int nTemplateId, int nCount, string strName)
{
	if (!Node::init())
	{
		return false;
	}

	m_nCount = nCount;
	initUI(nTemplateId, nCount, strName);
	return true;
}

void TreasureNode::initUI(int nTemplateId, int nCount, string strName)
{
	// 宝藏图标;
	m_ui.treasure = UIToolHeader::create(nTemplateId);
	m_ui.treasure->setScale(0.76f);
	m_ui.treasure->setNumEx(nCount);
	this->addChild(m_ui.treasure, 1);

	// 宝藏名称;
	m_ui.txtName = Text::create(strName, FONT_FZZHENGHEI, 20);
	//m_ui.txtName->enableOutline(Color4B(Color3B(0x79, 0x03, 0x03)), 2);
	setTextAddStroke(m_ui.txtName, Color3B(0x79, 0x03, 0x03), 2);
	m_ui.txtName->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.txtName->setPosition(Vec2(m_ui.treasure->getPosition().x, m_ui.treasure->getPosition().y - 60.0f));
	this->addChild(m_ui.txtName);

	// 已获取标识;
	// 使用时再生成;
}


void TreasureNode::setOpened(bool bOpened)
{
	assert(bOpened);
	m_bIsOpend = bOpened;
	m_ui.imgDone = ImageView::create("Image/UINewtower/Icon/img_yihuoqu.png");
	m_ui.imgDone->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgDone->setPosition(m_ui.treasure->getPosition());
	this->addChild(m_ui.imgDone, 3);
}



LayerTreasure::LayerTreasure(void)
	: m_bIsFirstEnter(true)
	, _armatureLight(nullptr)
{
	m_mapTreasure.clear();
	m_vcOpendTreasure.clear();
	TowerModel::getInstance()->addObserver(this);
}


LayerTreasure::~LayerTreasure(void)
{
	TowerModel::getInstance()->removeObserver(this);
}

LayerTreasure* LayerTreasure::create()
{
	LayerTreasure* _layer = new LayerTreasure;
	if (nullptr != _layer && _layer->init())
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

void LayerTreasure::onEnter()
{
	Layer::onEnter();

	if (m_bIsFirstEnter)
	{
		m_bIsFirstEnter = false;

		// 音效;
		//SoundUtils::playSoundEffect("quanping");

		// 宝藏数据;
		TowerModel::getInstance()->firstUpdateTreasure();

		/*
		map<int, int> mapInfo;
		mapInfo.insert(make_pair(23003, 1));
		mapInfo.insert(make_pair(24003, 2));
		mapInfo.insert(make_pair(26004, 5));
		mapInfo.insert(make_pair(25004, 3));
		mapInfo.insert(make_pair(21002, 7));
		mapInfo.insert(make_pair(22002, 1));
		refreshTreasure(mapInfo);
		*/
	}
}

void LayerTreasure::onExit()
{
	Layer::onExit();
}

bool LayerTreasure::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();
	return true;
}

void LayerTreasure::initUI()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();

	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UINewtower/UINewtower_baozang.ExportJson"));;
	m_ui.pRoot->setContentSize(visibleSize);
	m_ui.pRoot->setPosition(Director::getInstance()->getVisibleOrigin());
	this->addChild(m_ui.pRoot);

	// 金币数量;
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Label_21"))->setFontName(FONT_FZZHENGHEI);
	ostringstream oss;
	m_ui.txtGold = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Gold)));
	m_ui.txtGold->setFontName(FONT_FZZHENGHEI);
	oss << MainModel::getInstance()->getMainParam()->mGold;
	m_ui.txtGold->setString(oss.str());

	// 关闭按钮;
	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Close)));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerTreasure::onBtnClicked, this, Btn_Close));

	// 开启;
	m_ui.btnOpen = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Open)));
	m_ui.btnOpen->addTouchEventListener(CC_CALLBACK_2(LayerTreasure::onBtnClicked, this, Btn_Open));

	// 开启宝箱消耗金币数量;
	m_ui.txtCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Cost)));
	m_ui.txtCost->setFontName(FONT_FZZHENGHEI);
	oss.str("");
	oss << TowerModel::getInstance()->getNextTreasureCost();
	m_ui.txtCost->setString(oss.str());

	// 刷新;
	m_ui.txtLeftRefreshTimes = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_LeftRefreshTimes"));
	m_ui.txtLeftRefreshTimes->setFontName(FONT_FZZHENGHEI);
	m_ui.btnRefresh = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Refresh)));
	m_ui.btnRefresh->addTouchEventListener(CC_CALLBACK_2(LayerTreasure::onBtnClicked, this, Btn_Refresh));
	m_ui.txtRefreshCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Refresh_Cost)));
	m_ui.txtRefreshCost->setFontName(FONT_FZZHENGHEI);

	// 刷新消耗金币;
	oss.str("");
	oss << TowerModel::getInstance()->getRefreshTreasureCost();
	m_ui.txtRefreshCost->setString(oss.str());

	// 宝藏底图;
	m_ui.imgTable = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Image_zhuanpan"));
	oss.str("");
	for (unsigned int i = 0; i < 6; ++i)
	{
		oss.str("");
		oss << "Img_T" << i+1;
		m_ui.imgTreasureBg[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, oss.str()));
	}

}

void LayerTreasure::update( float delta )
{
	m_pParent->onLayerTreasureClose();

	this->unscheduleUpdate();
	this->removeFromParent();

}

void LayerTreasure::onBtnClicked( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (nWidgetName)
		{
		case Btn_Close:
			{
				TowerController::getInstance()->handleTreasureClose();
				this->scheduleUpdate();
			}
			break;

		case Btn_Open:
			{
				openTreasure();
			}
			break;

		case Btn_Refresh:
			{
				if (nullptr != _armatureLight)
				{
					_armatureLight->stopAllActions();
					_armatureLight->removeFromParentAndCleanup(true);
					_armatureLight = nullptr;
				}

				int nRet = TowerController::getInstance()->handleRefreshTreasure();
				if (nRet == -1)
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20066));
				}
				else if (nRet == -2)
				{
					CustomGoldTips *_tips = CustomGoldTips::create();
					if (_tips)
					{
						Director::getInstance()->getRunningScene()->addChild(_tips);
					}
				}
				else
				{
					// 正常刷新的情况下先禁用开启，等刷新结果出来后才允许开启;
					m_ui.btnOpen->setEnabled(false);
				}
			}
			break;

		default:
			break;
		}
	}
}

void LayerTreasure::refreshTreasure( multimap<int, int> mapInfo )
{
	// 清掉所有;
	for (auto _pair : m_mapTreasure)
	{
		_pair.second->removeFromParent();
	}
	m_mapTreasure.clear();

	// 宝藏;
	unsigned int i = 0;
	assert(mapInfo.size() == 6);
	for (auto iter = mapInfo.begin(); iter != mapInfo.end(); ++iter, ++i)
	{
		RowData* dataTool = DataManager::getInstance()->searchToolById(iter->first);
		assert(dataTool != nullptr);
		int nResId = dataTool->getIntData("resId");
		string strName = dataTool->getStringData("itemName");

		TreasureNode* _node = TreasureNode::create(iter->first, iter->second, strName);
		_node->setPosition(m_ui.imgTreasureBg[i]->getPosition());
		m_ui.imgTable->addChild(_node, 2);
		m_mapTreasure.insert(pair<int, TreasureNode*>(iter->first, _node));
	}

	// 剩余刷新次数;
	ostringstream oss;
	int nLeftTimes = TowerModel::getInstance()->m_queTreasureRefreshData.size();
	oss <<DataManager::getInstance()->searchCommonTexdtById(20067) << nLeftTimes;
	if (nLeftTimes <= 0)
	{
		m_ui.txtLeftRefreshTimes->setColor(Color3B::RED);
	}
	m_ui.txtLeftRefreshTimes->setString(oss.str());
	setTextAddStroke(m_ui.txtLeftRefreshTimes, Color3B(0x4B, 0x07, 0x07), 2);

	// 刷新消耗金币;
	int nRefreshCost = TowerModel::getInstance()->getRefreshTreasureCost();
	if (nRefreshCost != -1)
	{
		oss.str("");
		oss << nRefreshCost;
		m_ui.txtRefreshCost->setString(oss.str());
	}
	else
	{
		m_ui.txtRefreshCost->setVisible(false);
		m_ui.btnRefresh->setEnabled(false);
		m_ui.btnRefresh->setBright(false);
	}

	// 开启消耗金币;
	int nNextGold = TowerModel::getInstance()->getNextTreasureCost();
	if (nNextGold == -1)
	{
		m_ui.txtCost->setVisible(false);
		m_ui.btnOpen->setBright(false);
		m_ui.btnOpen->setEnabled(false);
	}
	else
	{
		oss.str("");
		oss << nNextGold;
		m_ui.txtCost->setVisible(true);
		m_ui.txtCost->setString(STR_UTF8(oss.str()));

		// 刷新成功后;
		m_ui.btnOpen->setBright(true);
		m_ui.btnOpen->setEnabled(true);
	}
}

void LayerTreasure::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_TRAIL_REFRESH_TREASURE:
		{
			// 刷新宝藏;
			refreshTreasure(*((multimap<int, int>*)(param->updateData)));
			m_ui.btnOpen->setEnabled(true);
		}
		break;

	case OBS_PARAM_TYPE_TRAIL_MAIN_INFO:
		{
			// 更新一下金币数量;
			//updateMainInfo;
			ostringstream oss;
			oss << ((TrailMainInfo*)(param->updateData))->nGold;
			m_ui.txtGold->setString(oss.str());
		}
		break;

	default:
		break;
	}
}

void LayerTreasure::openTreasure()
{
	if (!TowerController::getInstance()->handleOpenTreasure())
	{
		CustomGoldTips *_tips = CustomGoldTips::create();
		if (_tips)
		{
			Director::getInstance()->getRunningScene()->addChild(_tips);
		}
		return;
	}

	// 找到第一个还未开启的宝藏;
	map<int, TreasureNode*>::iterator it = find_if(m_mapTreasure.begin(), m_mapTreasure.end(), [=](map<int, TreasureNode*>::value_type _pair)->bool {
		return !(_pair.second->isOpend());
	});
	if (it == m_mapTreasure.end())
		return;

	// 此时禁止刷新;
	m_ui.btnRefresh->setEnabled(false);

	// 开启动画;
	auto itBegin = it;
	if (nullptr != _armatureLight)
	{
		_armatureLight->stopAllActions();
		_armatureLight->removeFromParentAndCleanup(true);
		_armatureLight = nullptr;
	}
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_BaoXiang_G/Ani_BaoXiang_G.ExportJson");
	_armatureLight = Armature::create("Ani_BaoXiang_G");
	_armatureLight->setPosition(it->second->getPosition());
	m_ui.imgTable->addChild(_armatureLight, 1);
	_armatureLight->getAnimation()->play("Animation1", -1, 1);
	Director::getInstance()->getScheduler()->schedule([=](float delta)mutable{
		do 
		{
			it++;
			CC_BREAK_IF(it == m_mapTreasure.end());
		} while (it->second->isOpend());

		if (it == m_mapTreasure.end())
		{
			// 重置至最初位置;
			it = itBegin;
		}
		_armatureLight->setPosition(it->second->getPosition());

		// 检测是否已开启宝藏;
		// 同时检测数量(03/24/2016 需求修改，可能会有重复的Id);
		if (!m_vcOpendTreasure.empty() && m_vcOpendTreasure.at(0) == it->first)
		{
			Director::getInstance()->getScheduler()->unschedule("LayerTreasure::openTreasure", this);

			// 允许刷新;
			int nRefreshCost = TowerModel::getInstance()->getRefreshTreasureCost();
			if (nRefreshCost != -1)
			{
				m_ui.btnRefresh->setEnabled(true);
			}

			// 动画放大后消失;
			Sequence* seq = Sequence::create(Spawn::create(ScaleTo::create(1.0f, 2.0f), FadeOut::create(1.0f), nullptr),
				CallFuncN::create([=](Node* pSender){
					_armatureLight->removeFromParent();
					_armatureLight = nullptr;
			}), nullptr);
			_armatureLight->runAction(seq);

			// 弹出奖励框;
			auto itItem = m_mapTreasure.begin();
			for (; itItem != m_mapTreasure.end(); ++itItem)
			{
				if (itItem->first == m_vcOpendTreasure.at(0)
					&& itItem->second->getCount() == m_vcOpendTreasure.at(1))
				{
					break;
				}
			}
			if (itItem != m_mapTreasure.end())
			{
				//
				RowData* row = DataManager::getInstance()->searchToolById(m_vcOpendTreasure.at(0));
				if (nullptr != row)
				{
					vector<PopRewardItem*> vcPopRewardItems;
					if (m_vcOpendTreasure.at(0) == 11002)	// 银币;
					{
						PopRewardItem_Coin* _rewardItem = new PopRewardItem_Coin(m_vcOpendTreasure.at(1));
						vcPopRewardItems.push_back(_rewardItem);
					}
					else 
					{
						PopRewardItem_Tool* _rewardItem = new PopRewardItem_Tool(m_vcOpendTreasure.at(0), m_vcOpendTreasure.at(1));
						vcPopRewardItems.push_back(_rewardItem);
					}
					PopReward::show(vcPopRewardItems);
				}

				// 开启;
				//auto iter = m_mapTreasure.find(m_nOpendTreasureId);
				//assert(iter != m_mapTreasure.end());
				//iter->second->setOpened(true);
				itItem->second->setOpened(true);

				// 启用开启按钮;
				m_vcOpendTreasure.clear();
				m_ui.btnOpen->setEnabled(true);
			}

			// 宝箱是否全部开启;
			int nOpenTimes = TowerModel::getInstance()->getTreasureOpendTimes();
			if (nOpenTimes >= 6)
			{
				m_ui.txtCost->setVisible(false);
				m_ui.btnOpen->setBright(false);
				m_ui.btnOpen->setEnabled(false);
			}
			else
			{
				ostringstream oss;
				oss << TowerModel::getInstance()->getNextTreasureCost();
				m_ui.txtCost->setVisible(true);
				m_ui.txtCost->setString(STR_UTF8(oss.str()));
			}
		}
	},
		this, 0.15f, kRepeatForever, 0.000001f, false, "LayerTreasure::openTreasure");

	// 每2秒查询一次是否成功;
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(LayerTreasure::checkIsTreasureOpend), this, 2.0f, false);

	// 暂时禁用开启按钮;
	m_ui.btnOpen->setEnabled(false);
}

void LayerTreasure::updateGold()
{
	ostringstream oss;
	oss << MainModel::getInstance()->getMainParam()->mGold;
	m_ui.txtGold->setString(oss.str());
	oss.str("");
}

void LayerTreasure::checkIsTreasureOpend( float delta )
{
	vector<int> vcOpenedTreasure = TowerModel::getInstance()->isTreasureOpend();
	if (!vcOpenedTreasure.empty())
	{
		// 停止定时器;
		Director::getInstance()->getScheduler()->unschedule(schedule_selector(LayerTreasure::checkIsTreasureOpend), this);

		// 停止动画;
		m_vcOpendTreasure = vcOpenedTreasure;
	}
}
