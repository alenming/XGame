#include "LayerTrailRanking.h"
#include "TowerModel.h"
#include "TowerController.h"
#include "PvpModule/PvpHeader.h"
#include "Widget/LayerCommFormation.h"
#include "Widget/ActionCreator.h"


LayerTrailRanking::LayerTrailRanking(void)
{
	TowerModel::getInstance()->addObserver(this);
}


LayerTrailRanking::~LayerTrailRanking(void)
{
	TowerModel::getInstance()->removeObserver(this);
}

LayerTrailRanking* LayerTrailRanking::create( TOWER_TYPE towerType, R_WND_TYPE wndType /* = R_WND_TYPE_RANKING */ )
{
	LayerTrailRanking* _layer = new LayerTrailRanking;
	if (nullptr != _layer && _layer->init(towerType, wndType))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}


void LayerTrailRanking::onEnter()
{
	Layer::onEnter();
	if (m_wndType == R_WND_TYPE_RANKING)
	{
		TowerController::getInstance()->handleRanking(m_towerType);

		// test;
		/*
		UI_TRAIL_RANKING_PARAM param;
		param.nMaxFloor = 100;
		param.nRank = 89;
		for (int i = 0; i < 20; ++i)
		{
			RankingData data;
			data.bShowRankingIcon = (i < 3);
			data.nFc = 1000;
			data.nMaxFloor = 98;
			data.nPlayerId = 41001;
			data.nPlayerType = 1;
			data.nRankingIndex = i;
			data.nResId = 41001;
			data.strPlayerName = "sb鲁智深";
			for (int j = 0; j < 4; ++j)
			{
				data.vcFormation.push_back(41001);
			}
			param.vcRankingData.push_back(data);
		}
		updateRanking(&param);
		*/
	}
	else if (m_wndType == R_WND_TYPE_REWARD)
	{
		TowerController::getInstance()->handleTowerReward();
	}

	ActionCreator::runCommDlgAction(m_ui.imgRankingBg);

	// 音效;
	SoundUtils::playSoundEffect("tanchuang");
}


void LayerTrailRanking::onExit()
{
	Layer::onExit();
}


bool LayerTrailRanking::init(TOWER_TYPE towerType, R_WND_TYPE wndType/* = R_WND_TYPE_RANKING*/)
{
	if (!Layer::init())
	{
		return false;
	}

	m_towerType = towerType;
	m_wndType = wndType;
	initUI();

	return true;
}


void LayerTrailRanking::initUI()
{
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UINewtower/UINewtower_RankingList.ExportJson"));
	this->addChild(m_ui.pRoot);
	m_ui.pRoot->setVisible(false);

	m_ui.imgRankingBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Ranking_Bg"));

	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Close)));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerTrailRanking::onBtnClicked, this, Btn_Close));

	m_ui.imgTitleRanking = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Img_Title_Ranking)));
	m_ui.imgTitleRanking->setVisible(m_wndType == R_WND_TYPE_RANKING);

	m_ui.imgRankingDesc = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Img_Ranking_Desc)));
	m_ui.imgRankingDesc->setVisible(m_wndType == R_WND_TYPE_RANKING);
	m_ui.txtRanking = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Lab_Ranking)));
	m_ui.txtRanking->setFontName(FONT_FZZHENGHEI);
	m_ui.txtRanking->setString("");
	m_ui.txtRanking->setVisible(m_wndType == R_WND_TYPE_RANKING);

	m_ui.imgContentBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Img_Content_Bg)));
	m_ui.scrollviewContent = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(ScrollView_Content)));
	m_ui.scrollviewContent->addEventListener(CC_CALLBACK_2(LayerTrailRanking::onScrollViewEvent, this));

	// 添加滚动条;
	m_ui.scrollBar = CustomScrollBar::create();
	m_ui.scrollBar->initScrollBar(m_ui.scrollviewContent->getContentSize().height);
	m_ui.scrollBar->setPosition(Vec2(33.0f, 35.0f));
	m_ui.imgContentBg->addChild(m_ui.scrollBar);
}


void LayerTrailRanking::onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName)
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
				this->scheduleUpdate();
			}
			break;

		default:
			break;
		}
	}
}


void LayerTrailRanking::update(float dt)
{
	this->unscheduleUpdate();
	this->removeFromParent();
}


void LayerTrailRanking::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_TRAIL_RANKING:
	case OBS_PARAM_TYPE_TOWER_RANKING:
		{
			updateRanking((UI_TRAIL_RANKING_PARAM*)(param->updateData));
			m_ui.pRoot->setVisible(true);
		}
		break;

	default:
		break;
	}
}


void LayerTrailRanking::updateRanking( UI_TRAIL_RANKING_PARAM* data )
{
	if (data == nullptr || data->vcRankingData.size() == 0)
	{
		return;
	}

	m_RankingParam = *data;

	// 我的排名;
	ostringstream  oss;
	oss << data->nRank;
	m_ui.txtRanking->setString(oss.str());
	oss.str("");

	// 排行榜数据;
	m_ui.scrollBar->setVisible(data->vcRankingData.size() >= 6);
	int nNodeHeight = 90.0f;
	int nScollviewWidth = m_ui.scrollviewContent->getContentSize().width;
	int nScrollviewHeight = (data->vcRankingData.size() * nNodeHeight > m_ui.scrollviewContent->getContentSize().height) ?
		data->vcRankingData.size() * nNodeHeight : m_ui.scrollviewContent->getContentSize().height;
	m_ui.scrollviewContent->setInnerContainerSize(Size(nScollviewWidth, nScrollviewHeight));
	for (unsigned int i = 0; i < data->vcRankingData.size(); i++)
	{
		RankNode* _node = RankNode::create(this, &(data->vcRankingData.at(i)));
		if (nullptr != _node)
		{
			_node->setPosition(Vec2(50.0f, nScrollviewHeight - nNodeHeight + 10.0f - i * nNodeHeight));
			m_ui.scrollviewContent->addChild(_node);
		}
	}

	m_ui.scrollBar->setScrollBarLengthPercent(m_ui.scrollviewContent->getContentSize().height / m_ui.scrollviewContent->getInnerContainerSize().height);
	m_ui.scrollBar->setScrollBarPercentY(0.0f);

	this->setVisible(true);
}

void LayerTrailRanking::onScrollViewEvent( Ref* pSender, ScrollView::EventType type )
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

void LayerTrailRanking::onRankingNodeClicked( const int nIndex )
{
	// 阵型;
	PVP_PLAYER_DATA  data;
	data.id = m_RankingParam.vcRankingData.at(nIndex).nPlayerId;
	data.name = m_RankingParam.vcRankingData.at(nIndex).strPlayerName;
	data.level = m_RankingParam.vcRankingData.at(nIndex).nLv;
	data.resID = m_RankingParam.vcRankingData.at(nIndex).nResId;
	data.atk = m_RankingParam.vcRankingData.at(nIndex).nFc;
	data.pvporder = m_RankingParam.vcRankingData.at(nIndex).nRankingIndex;
	data.matrixIds = m_RankingParam.vcRankingData.at(nIndex).vcFormation;
	data.vipLv = m_RankingParam.vcRankingData.at(nIndex).nVipLv;
	data.groupName = m_RankingParam.vcRankingData.at(nIndex).strGroupName;
	LayerCommFormation* _layer = LayerCommFormation::create(data);
	this->addChild(_layer);

}
