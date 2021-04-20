#include "LayerOp.h"
#include "TowerController.h"
#include "TowerModel.h"
#include "FormationModule/LayerFormation.h"
#include "LayerTrailHero.h"
#include "GuideCenter/GuideManager.h"


LayerOp::LayerOp(void)
	: m_nWndType(POP_WND_TYPE_OP)
	, m_nSelectedBuffId(-1)
	, m_pParent(nullptr)
{
	m_mapBuff.clear();
	m_mapOp.clear();
	TowerModel::getInstance()->addObserver(this);
}


LayerOp::~LayerOp(void)
{
	TowerModel::getInstance()->removeObserver(this);
}

LayerOp* LayerOp::create( POP_WND_TYPE type )
{
	LayerOp* _layer = new LayerOp;
	if (nullptr != _layer && _layer->init(type))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}

bool LayerOp::init( POP_WND_TYPE type )
{
	if (!Layer::init())
	{
		return false;
	}

	m_nWndType = type;
	initUI();

	// 请求内容;
	if (m_nWndType == POP_WND_TYPE_BUFF)
	{
		TowerController::getInstance()->handleBuffContent();

		// test;
		/*
		vector<UI_BUFF_PARAM>  vcParam;
		for (unsigned int i = 0; i < 3; ++i)
		{
			UI_BUFF_PARAM pp;
			pp.nIndex = i;
			pp.nBuffId = 9033 + i*11;
			pp.nPrice = i*236;
			pp.strBuffDesc = "本次试炼全部英雄法防+5%";
			vcParam.push_back(pp);
		}
		updateBuff(&vcParam);
		*/
	}
	else if (m_nWndType == POP_WND_TYPE_OP)
	{
		TowerController::getInstance()->handleGetTrailOp();
		/*
		// test
		vector<UI_TOWER_OP_PARAM>  vcParam;
		for (unsigned int i = 1; i <= 3; ++i)
		{
			UI_TOWER_OP_PARAM pp;
			pp.nLv = i;
			pp.nOpFc = i*100;
			pp.nOpLv = i*10;
			pp.nScore = i*10000;
			pp.nStar = i*10;
			pp.strOpName = "test";
			pp.vcOpFormation.push_back(41003);
			pp.vcOpFormation.push_back(-1);
			pp.vcOpFormation.push_back(41004);
			pp.vcOpFormation.push_back(-1);
			pp.vcOpFormation.push_back(41001);
			pp.vcOpFormation.push_back(41002);
			pp.vcOpFormation.push_back(-1);
			pp.vcOpFormation.push_back(41005);
			pp.vcOpFormation.push_back(-1);
			vcParam.push_back(pp);
		}
		updateOp(&vcParam);
		*/
	}

	return true;
}

void LayerOp::onEnter()
{
	Layer::onEnter();

	// 音效;
	SoundUtils::playSoundEffect("tanchuang");
}

void LayerOp::initUI()
{
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UINewtower/UINewtower_tanchu.ExportJson"));

	m_ui.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "img_kuang"));
	m_ui.imgBg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

	// 标题;
	m_ui.imgTitle1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "img_xuanzeduishou"));
	m_ui.imgTitle1->setVisible(m_nWndType == POP_WND_TYPE_OP);
	m_ui.imgTitle2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "img_youhu"));
	m_ui.imgTitle2->setVisible(m_nWndType == POP_WND_TYPE_BUFF);

	// 关闭按钮;
	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Close"));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerOp::onBtnClicked, this));

	// 星星数量;
	m_ui.imgStarBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Star_Bg"));
	m_ui.imgStarBg->setVisible(m_nWndType == POP_WND_TYPE_BUFF);
	m_ui.txtStar = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Star"));
	m_ui.txtStar->setFontName(FONT_FZZHENGHEI);
	m_ui.txtStar->setString("");
	m_ui.txtStar->setString(_TO_STR(TowerModel::getInstance()->getTrailMainInfo()->nStar));

	// Tips;
	m_ui.txtTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Tips"));
	m_ui.txtTips->setFontName(FONT_FZZHENGHEI);
	m_ui.txtTips->setVisible(m_nWndType == POP_WND_TYPE_BUFF);

	// 区分类型;
	/*
	if (m_nWndType == POP_WND_TYPE_OP)
	{
	}
	else
	{
		m_ui.imgStar = ImageView::create("Image/Icon/global/img_star.png");
		m_ui.imgStar->setPosition(Vec2(60.0f, 615.0f));
		m_ui.imgBg->addChild(m_ui.imgStar);
		m_ui.txtStar = Text::create("0", FONT_FZZHENGHEI, 30);
		m_ui.txtStar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		m_ui.txtStar->setPosition(Vec2(m_ui.imgStar->getPosition().x + 30.0f, m_ui.imgStar->getPosition().y - 2.0f));
		m_ui.imgBg->addChild(m_ui.txtStar);
		string str("消耗星星兑换佑护");
		m_ui.txtStarDesc = Text::create(STR_UTF8(str), FONT_FZZHENGHEI, 26);
		m_ui.txtStarDesc->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		m_ui.txtStarDesc->setColor(Color3B(0xFC, 0xC3, 0x40));
		m_ui.txtStarDesc->setPosition(Vec2(m_ui.pRoot->getContentSize().width/2 - m_ui.txtStarDesc->getContentSize().width/2, 45.0f));
		m_ui.imgBg->addChild(m_ui.txtStarDesc);
	}
	*/

	this->addChild(m_ui.pRoot);
}

void LayerOp::onBtnClicked( Ref* ref, Widget::TouchEventType type )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (m_nWndType == POP_WND_TYPE_OP)
		{
			this->scheduleUpdate();
		}
		else if (m_nWndType == POP_WND_TYPE_BUFF)
		{
			// 关闭时判定buff是否已全部领取;
			if (TowerController::getInstance()->handleCloseBuff())
			{
				this->scheduleUpdate();
			}
			else
			{
				// pop
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20049), 
					CC_CALLBACK_2(LayerOp::onCustomPopRet, this), CUSTOM_YES_NO);
			}
		}
	}
}

void LayerOp::update( float delta )
{
	m_pParent->onLayerOpClose(m_nWndType);

	this->unscheduleUpdate();
	this->removeFromParent();
}

void LayerOp::onOpChallange( unsigned int nLv )
{
	TowerModel::getInstance()->selectOpLv(nLv);
	//LayerFormation* _layer = LayerFormation::create(TYPE_TRAIL, F_MODE_TRAIL);
	//this->addChild(_layer);
	Director::getInstance()->pushScene(SceneFormation::create(F_MODE_TRAIL));
}

void LayerOp::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_TRAIL_BUFF_CONTENT:
		{
			// 生成buff节点;
			updateBuff((vector<UI_BUFF_PARAM>*)(param->updateData));
		}
		break;

	case OBS_PARAM_TYPE_TRAIL_BUFF_STATE:
		{
			// 更新buff可兑换状态;
			updateBuffState((map<int, bool>*)(param->updateData));
		}
		break;

	case OBS_PARAM_TYPE_TRAIL_OP:
		{
			updateOp((vector<UI_TOWER_OP_PARAM>*)(param->updateData));
		}
		break;

	case OBS_PARAM_TYPE_TRAIL_OP_STATE:
		{
			updateOpState(*((int*)(param->updateData)));
		}
		break;

	case OBS_PARAM_TYPE_TRAIL_MAIN_INFO:
		{
			if (m_nWndType == POP_WND_TYPE_BUFF)
			{
				TrailMainInfo* info = (TrailMainInfo*)(param->updateData);
				updateStar(info->nStar);
			}
		}
		break;

	default:
		break;
	}
}

void LayerOp::updateOp( vector<UI_TOWER_OP_PARAM>* vcParam )
{
	for (unsigned int i = 0; i < vcParam->size(); ++i)
	{
		OpNode* _op = OpNode::create(vcParam->at(i), this);
		_op->setPosition(Vec2(385.0f + 325*i, 580.0f));
		m_ui.imgBg->addChild(_op);
		m_mapOp.insert(make_pair(i+1, _op));
	}

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_TRAIL_SELECT_OP);
}

void LayerOp::updateOpState( int nValidLv )
{
	assert(m_mapOp.size() == 3);

	for (auto it : m_mapOp)
	{
		it.second->updateState(it.first == nValidLv);
	}
}

void LayerOp::updateBuff( vector<UI_BUFF_PARAM>* vcParam )
{
	float fSingleWidth = m_ui.imgBg->getContentSize().width/3;
	float fStartX = -fSingleWidth;
	for (unsigned int i = 0; i < vcParam->size(); ++i)
	{
		BuffNode* _buff = BuffNode::create(vcParam->at(i), this);
		_buff->setPosition(Vec2(200.0f + 345*i, 310.0f));
		m_ui.imgBg->addChild(_buff);
		m_mapBuff.insert(pair<int, BuffNode*>(vcParam->at(i).nBuffId, _buff));
	}
}

void LayerOp::onBuffClicked( int nBuffId )
{
	m_nSelectedBuffId = nBuffId;
	int nBuffRange = TowerModel::getInstance()->getBuffRange(nBuffId);

	// 单体buff;
	if (nBuffRange == 1)
	{
		// 选择英雄进行操作;
		LayerTrailHero* pHeroLayer = LayerTrailHero::create();
		this->addChild(pHeroLayer);
		return;
	}

	// 全体buff;
	TowerController::getInstance()->handleBuyBuff(nBuffId);
}

void LayerOp::onSelectHeroConfirm( const int& nHeroId )
{
	// 已选英雄，单体buff;
	TowerController::getInstance()->handleBuyBuff(m_nSelectedBuffId, nHeroId);
}

void LayerOp::updateBuffState( map<int, bool>* mapBuffState )
{
	for (auto iter : *mapBuffState)
	{
		auto _it = m_mapBuff.find(iter.first);
		assert(_it != m_mapBuff.end());
		_it->second->updateState(!iter.second);
	}
}

void LayerOp::onCustomPopRet( Ref* pSender, CustomRetType nRet )
{
	switch (nRet)
	{
	case RET_TYPE_OK:
		{
			TowerModel::getInstance()->setBuffTriggered(false);

			// 更新楼层(递增至下一层);
			TowerModel::getInstance()->updateTrailCurFloorNum();

			this->scheduleUpdate();
		}
		break;

	case RET_TYPE_CANCEL:
	case RET_TYPE_CLOSE:
		break;
	}
}

void LayerOp::updateStar(int nCount)
{
	ostringstream oss;
	oss << nCount;
	m_ui.txtStar->setString(oss.str());
	oss.str("");
}
