#include "OpNode.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"

OpNode::OpNode(void)
	: m_pParent(nullptr)
	, m_nLv(0)
	, m_fCriticalAngle(0.0f)
{
}


OpNode::~OpNode(void)
{
}

OpNode* OpNode::create(UI_TOWER_OP_PARAM param, LayerOp* parent)
{
	OpNode* _node = new OpNode;
	if (nullptr != _node && _node->init(param, parent))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool OpNode::init(UI_TOWER_OP_PARAM param, LayerOp* parent)
{
	if (!Node::init())
	{
		return false;
	}

	m_pParent = parent;
	initUI(param);

	return true;
}

void OpNode::initUI( UI_TOWER_OP_PARAM param )
{
	m_nLv = param.nLv;
	m_fCriticalAngle = 90.0f + (m_nLv - 2) * 30.0f;

	Layout* pRoot = Layout::create();
	pRoot->setContentSize(Size(320.0f, 590.0f));
	pRoot->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	this->setContentSize(pRoot->getContentSize());
	this->addChild(pRoot);

	// 标题;
	ostringstream oss;
	ostringstream oss2;
	oss << "Image/UINewtower/Icon/img_";
	oss2 << "Image/UINewtower/Icon/btn_";
	switch (param.nLv)
	{
	case 1:
		oss << "jiandan.png";
		oss2 << "jiandan_1.png";
		break;

	case 2:
		oss << "putong.png";
		oss2 << "putong_1.png";
		break;

	case 3:
		oss << "kunnan.png";
		oss2 << "kunnan_1.png";
		break;

	default:
		break;
	}
	m_ui.imgTitle = ImageView::create(oss.str());
	m_ui.imgTitle->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgTitle->setPosition(Vec2(0.0f, pRoot->getContentSize().height/2));
	pRoot->addChild(m_ui.imgTitle);

	// 卡牌背景;
	oss.str("");
	oss << "Image/Icon/global/zise_di.png";
	m_ui.imgHeroBg = ImageView::create(oss.str());
	m_ui.imgHeroBg->setScale(0.603f);
	m_ui.imgHeroBg->setPosition(Vec2(m_ui.imgTitle->getPosition().x, m_ui.imgTitle->getPosition().y - 215.0f));
	pRoot->addChild(m_ui.imgHeroBg);

	// 卡牌原画;
	oss.str("");
	int nIndex = 0;
	for (unsigned int i = 0; i < 9; ++i)
	{
		if (param.vcOpFormation.at(i).id != -1)
		{
			nIndex = i;
			break;
		}
	}

	int ntmpResId = -1;
	if (param.nType > 0)
	{
		ntmpResId = param.vcOpFormation.at(nIndex).id/100;
	}
	else if (param.nType < 0)
	{
		// 怪物查表;
		RowData* _row = DataManager::getInstance()->searchMonsterById(param.vcOpFormation.at(nIndex).id);
		assert(_row != nullptr);
		ntmpResId = _row->getIntData("resID");
	}
	else
	{
		assert(false);
	}

	//oss << "Image/Icon/newcard/" << ntmpResId << ".png";
	//m_ui.imgHero = ImageView::create(oss.str());
	m_ui.imgHero = ImageView::create("Image/Icon/Small/-1.png");
	m_ui.imgHero->setContentSize(m_ui.imgHeroBg->getContentSize());
	m_ui.imgHero->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgHero->setPosition(Vec2(m_ui.imgHeroBg->getContentSize().width/2, m_ui.imgHeroBg->getContentSize().height/2));
	m_ui.imgHeroBg->addChild(m_ui.imgHero, 1);

	// 临时原画处理;
	m_ui.pnlHeroBg = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UINewtower/UINewtower_OpHeroBg.ExportJson"));
	ImageView* imageHero = ImageView::create(ResourceUtils::getHeroCompleteCardPath(ntmpResId));
	RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(ntmpResId);
	if (pointRowData)
	{
		vector<float> pointVect;
		StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
		imageHero->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));
	}
	m_ui.pnlHeroBg->setScale(1.0f/0.603f);
	imageHero->setScale(0.603f);
	imageHero->setPosition(Vec2(m_ui.pnlHeroBg->getContentSize().width*0.5f, m_ui.pnlHeroBg->getContentSize().height*0.3f));
	m_ui.pnlHeroBg->addChild(imageHero);
	m_ui.imgHero->addChild(m_ui.pnlHeroBg);

	// 卡牌蒙版(240*350);
	m_ui.imgMask = ImageView::create("Image/UINewtower/Icon/img_zhezhao.png");
	//m_ui.imgMask->setScale(1.0f/0.603f);
	m_ui.imgMask->setScaleX((240.0f/104.0f)/0.603f);
	m_ui.imgMask->setScaleY((350.0f/104.0f)/0.603f);
	m_ui.imgMask->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgMask->setPosition(Vec2(m_ui.imgHeroBg->getContentSize().width/2, m_ui.imgHeroBg->getContentSize().height/2));
	m_ui.imgHero->addChild(m_ui.imgMask, 2);
	m_ui.imgMask->setVisible(false);

	// 卡牌框;
	m_ui.imgFrame = ImageView::create(oss2.str());
	m_ui.imgFrame->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgFrame->setPosition(m_ui.imgMask->getPosition());
	m_ui.imgFrame->setScale(1.0f/0.603f);
	m_ui.imgHero->addChild(m_ui.imgFrame, 3);
	oss.str("");
	oss << DataManager::getInstance()->searchCommonTexdtById(20068);
	m_ui.txtScoreTitle = Text::create(oss.str(), FONT_FZZHENGHEI, 26);
	m_ui.txtScoreTitle->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_ui.txtScoreTitle->setColor(Color3B(0xF8, 0xE3, 0x56));
	//m_ui.txtScoreTitle->enableOutline(Color4B(Color3B(0x78, 0x00, 0x00)), 2);
	setTextAddStroke(m_ui.txtScoreTitle, Color3B(0x78, 0x00, 0x00), 2);
	m_ui.txtScoreTitle->setPosition(Vec2(55.0f, 80.0f));
	m_ui.imgFrame->addChild(m_ui.txtScoreTitle);
	m_ui.imgFrame->setTouchEnabled(true);
	m_ui.imgFrame->addTouchEventListener(CC_CALLBACK_2(OpNode::onCardClicked, this));

	// 黑色底;
	m_ui.imgMask2 = ImageView::create("Image/UINewtower/Icon/img_zhezhao.png");
	m_ui.imgMask2->setScaleX((240.0f/104.0f)/0.603f);
	m_ui.imgMask2->setScaleY((90.0f/104.0f)/0.603f);
	m_ui.imgMask2->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgMask2->setPosition(Vec2(m_ui.imgHeroBg->getContentSize().width/2, 75.0f));
	m_ui.imgHero->addChild(m_ui.imgMask2, 2);

	// 积分;
	oss.str("");
	oss << param.nScore;
	m_ui.txtScore = Text::create(oss.str(), FONT_FZZHENGHEI, 26);
	m_ui.txtScore->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_ui.txtScore->setColor(Color3B(0xFF, 0xF6, 0x8C));
	//m_ui.txtScore->enableOutline(Color4B(Color3B(0x45, 0x05, 0x05)), 2);
	setTextAddStroke(m_ui.txtScore, Color3B(0x45, 0x05, 0x05), 2);
	m_ui.txtScore->setPosition(Vec2(m_ui.txtScoreTitle->getPosition().x + 135.0f, m_ui.txtScoreTitle->getPosition().y));
	m_ui.imgFrame->addChild(m_ui.txtScore);
	oss.str("");

	// 星;
	m_ui.imgStar = ImageView::create("Image/Icon/global/img_zhanzhang.png");
	m_ui.imgStar->setScale(0.8f);
	m_ui.imgStar->setPosition(Vec2(m_ui.txtScoreTitle->getPosition().x + 75.0f, m_ui.txtScoreTitle->getPosition().y - 20.0f));
	m_ui.imgFrame->addChild(m_ui.imgStar);
	oss.str("");
	oss << "X" << param.nStar;
	m_ui.txtStar = Text::create(oss.str(), FONT_FZZHENGHEI, 26);
	m_ui.txtStar->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.txtStar->setColor(Color3B(0xFF, 0xF6, 0x8C));
	//m_ui.txtStar->enableOutline(Color4B(Color3B(0x45, 0x05, 0x05)), 2);
	setTextAddStroke(m_ui.txtStar, Color3B(0x45, 0x05, 0x05), 2);
	m_ui.txtStar->setPosition(Vec2(m_ui.imgStar->getPosition().x + 35.0f, m_ui.imgStar->getPosition().y - 5.0f));
	m_ui.imgFrame->addChild(m_ui.txtStar);

	// 玩家名;
	m_ui.txtName = Text::create(param.strOpName, FONT_FZZHENGHEI, 24);
	m_ui.txtName->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.txtName->setColor(Color3B(0xE8, 0xDE, 0xB5));
	//m_ui.txtName->enableOutline(Color4B(Color3B(0x31, 0x00, 0x00)), 2);
	setTextAddStroke(m_ui.txtName, Color3B(0x31, 0x00, 0x00), 2);

	// 等级;
	oss.str("");
	oss << "LV" << param.nOpLv;
	m_ui.txtLv = Text::create(oss.str(), FONT_FZZHENGHEI, 24);
	m_ui.txtLv->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.txtLv->setColor(Color3B(0xF8, 0xE3, 0x56));
	//m_ui.txtLv->enableOutline(Color4B(Color3B(0x78, 0x00, 0x00)), 2);
	setTextAddStroke(m_ui.txtLv, Color3B(0x78, 0x00, 0x00), 2);
	float fSpace = 20.0f;
	float fLength = m_ui.txtName->getContentSize().width + m_ui.txtLv->getContentSize().width + fSpace;
	m_ui.txtLv->setPosition(Vec2(m_ui.imgHeroBg->getPosition().x - fLength/2.0f, m_ui.imgHeroBg->getPosition().y - 205.0f));
	m_ui.txtName->setPosition(Vec2(m_ui.txtLv->getPosition().x+ m_ui.txtLv->getContentSize().width + fSpace, m_ui.txtLv->getPosition().y + 2.0f));
	pRoot->addChild(m_ui.txtName);
	pRoot->addChild(m_ui.txtLv);
	oss.str("");

	// 战斗力;
	oss << param.nOpFc;
	string strTmp(oss.str());
	m_ui.imgFcDesc = ImageView::create("Image/UINewtower/Icon/img_zhandouli.png");
	fLength = m_ui.imgFcDesc->getContentSize().width + 16.0f * strTmp.size();
	m_ui.imgFcDesc->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.imgFcDesc->setPosition(Vec2(m_ui.imgHeroBg->getPosition().x - fLength/2.0f - 10.0f, m_ui.txtName->getPosition().y - 35.0f));
	pRoot->addChild(m_ui.imgFcDesc);
	m_ui.txtFc = Text::create(oss.str(), FONT_FZZHENGHEI, 24);
	m_ui.txtFc->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.txtFc->setColor(Color3B(0xE8, 0xDE, 0xB5));
	//m_ui.txtFc->enableOutline(Color4B(Color3B(0x31, 0x00, 0x00)), 2);
	setTextAddStroke(m_ui.txtFc, Color3B(0x31, 0x00, 0x00), 2);
	m_ui.txtFc->setPosition(Vec2(m_ui.imgFcDesc->getPosition().x + m_ui.imgFcDesc->getContentSize().width, m_ui.imgFcDesc->getPosition().y - 2.0f));
	pRoot->addChild(m_ui.txtFc);

	// 挑战按钮;
	m_ui.btnFight = Button::create("Image/UINewtower/Icon/btn_tiaozhan_1.png", "Image/UINewtower/Icon/btn_tiaozhan_2.png",
		"Image/UINewtower/Icon/btn_tiaozhan_3.png");
	m_ui.btnFight->setPosition(Vec2(m_ui.imgHeroBg->getPosition().x, m_ui.imgHeroBg->getPosition().y - 300.0f));
	m_ui.btnFight->addTouchEventListener(CC_CALLBACK_2(OpNode::onBtnClicked, this));
	pRoot->addChild(m_ui.btnFight);

	// 阵型;
	float fPos0X = 220.0f;
	float fPos0Y = 275.0f;
	for (unsigned int i = 0; i < 9; ++i)
	{
		oss.str("");
		oss << "Image/Icon/Small/";
		if (param.vcOpFormation.at(i).id == -1)
		{
			oss << "-1.png";
		}
		else
		{
			if (param.nType > 0)
			{
				oss << param.vcOpFormation.at(i).id/100 << ".png";
			}
			else if (param.nType < 0)
			{
				// 怪物查表;
				RowData* _row = DataManager::getInstance()->searchMonsterById(param.vcOpFormation.at(i).id);
				assert(_row != nullptr);
				oss << _row->getIntData("resID") << ".png";
			}
			else
			{
				assert(false);
			}
		}
		m_ui.miniHero[i] = UIToolHeader::create(oss.str());
		m_ui.miniHero[i]->setStarsNum(param.vcOpFormationQua.at(i), STAR_CONTAIN_BOTTOM);
		m_ui.miniHero[i]->setPosition(Vec2(fPos0X - (i/3)*70.0f, fPos0Y - (i%3)* 70.0f));
		m_ui.imgFrame->addChild(m_ui.miniHero[i]);
		m_ui.miniHero[i]->setScale(0.6f);
		m_ui.miniHero[i]->setVisible(false);
	}
}

void OpNode::onBtnClicked( Ref* ref, Widget::TouchEventType type )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		assert(m_pParent != nullptr);
		m_pParent->onOpChallange(m_nLv);
	}
}

void OpNode::onCardClicked( Ref* ref, Widget::TouchEventType type )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		// 翻转卡牌;
		OrbitCamera* camera = OrbitCamera::create(0.3f, 0.5f, 0.0f, 0.0f, m_fCriticalAngle, 0, 0);
		Vec3 vec = camera->getCenter();
		Action* act = CCSequence::create(camera, 
			CallFuncN::create(CC_CALLBACK_0(OpNode::orbitCallBack, this)), nullptr); 
		m_ui.imgHeroBg->runAction(act);
	}
}

void OpNode::orbitCallBack()
{
	// 显示阵型;
	showForamtion(!m_ui.miniHero[0]->isVisible());

	// 继续翻转;
	OrbitCamera* camera = OrbitCamera::create(0.3f, 0.5f, 0.0f, m_fCriticalAngle+180.0f, 180.0f-m_fCriticalAngle, 0, 0);
	Action* act = CCSequence::create(camera, nullptr); 
	m_ui.imgHeroBg->runAction(act);
}

void OpNode::showForamtion( bool bShow )
{
	// 背景蒙版;
	m_ui.imgMask->setVisible(bShow);

	m_ui.imgMask2->setVisible(!bShow);
	m_ui.txtScoreTitle->setVisible(!bShow);
	m_ui.txtScore->setVisible(!bShow);
	m_ui.imgStar->setVisible(!bShow);
	m_ui.txtStar->setVisible(!bShow);

	// 阵型;
	for (unsigned int i = 0; i < 9; ++i)
	{
		m_ui.miniHero[i]->setVisible(bShow);
	}
}

void OpNode::updateState( bool bEnabled )
{
	m_ui.btnFight->setEnabled(bEnabled);
	m_ui.btnFight->setBright(bEnabled);
}
