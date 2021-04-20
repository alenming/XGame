#include "BarrierNode.h"
#include "LayerBarrier.h"

BarrierNode::BarrierNode(void)
	: m_pParent(nullptr)
	, m_bIsEnabled(false)
	, m_nBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
	, m_nResId(-1)
	, m_nLv(BARRIER_LEVEL_0)
	, m_bIsSelected(false)
	, m_ptOriPos(Vec2(0.0f, 0.0f))
	, m_bIsPopUp(false)
	, m_nBossId(-1)
{
}


BarrierNode::~BarrierNode(void)
{
}


BarrierNode* BarrierNode::create(UI_BARRIER_PARAM* param)
{
	BarrierNode* barrier = new BarrierNode;
	if (nullptr != barrier && barrier->init(param))
	{
		barrier->autorelease();
		return barrier;
	}

	CC_SAFE_DELETE(barrier);
	return nullptr;
}

void BarrierNode::onEnter()
{
	Layout::onEnter();
}

bool BarrierNode::init(UI_BARRIER_PARAM* param)
{
	if (!Layout::init())
	{
		return false;
	}
	m_nBarrierId = param->nBarrierId;
	m_nLv = param->nLv;
	m_bIsEnabled = param->bIsEnabled;

	initUI(param);

	return true;
}


void BarrierNode::initUI(UI_BARRIER_PARAM* param)
{
	ostringstream oss;

	// 底图;
	oss << "Image/UIBarrier/Barrier/" << param->nResId << ".jpg";
	m_ui.imgBg = ImageView::create(oss.str());
	this->addChild(m_ui.imgBg);

	// 关卡名;
	oss.str("");
	oss << "Image/UIBarrier/Barrier/img_guanqiaming_1.png";
	m_ui.imgBarrierName = ImageView::create(oss.str());
	m_ui.imgBarrierName->setPosition(Vec2(m_ui.imgBg->getPosition().x, m_ui.imgBg->getPosition().y + 230.0f));
	this->addChild(m_ui.imgBarrierName, 1);
	oss.str("");
	oss << param->nChapterId % 10 << "-" << param->nBarrierId % 100 << " " << param->strBarrierName;
	m_ui.txtBarrierName = Text::create((param->strBarrierName), FONT_NAME_FOUNDER_BOLD, 20);
	m_ui.txtBarrierName->setColor(Color3B(0xff, 0xfa, 0x6f));
	m_ui.txtBarrierName->setPosition(m_ui.imgBarrierName->getPosition());
	this->addChild(m_ui.txtBarrierName, 2);

	// 边框(普通/精英);
	oss.str("");
	oss << "Image/UIBarrier/Barrier/img_frame_" << m_nLv+1 << "_1.png";
	m_ui.imgFrame = ImageView::create(oss.str());
	m_ui.imgFrame->setPosition(m_ui.imgBg->getPosition());
	this->addChild(m_ui.imgFrame, 4);
	m_ui.imgFrame->setTouchEnabled(true);
	m_ui.imgFrame->addTouchEventListener(CC_CALLBACK_2(BarrierNode::onBarrierClicked, this));

	// 掉落碎片(Boss关);
	if (param->bIsBoss)
	{
		m_nBossId = param->nBossShowId;
		oss.str("");
		oss << "Image/Icon/BossShow/" << param->nBossShowId << ".png";
		m_ui.imgBoss = ImageView::create(oss.str());
		if (nullptr != m_ui.imgBoss)
		{
			m_ui.imgBoss->setPosition(m_ui.imgBg->getPosition());
			this->addChild(m_ui.imgBoss);
		}
	}

	// 通关标记;
	oss.str("");
	oss << "Image/UIBarrier/Barrier/img_zhan.png";
	m_ui.imgPass = ImageView::create(oss.str());
	m_ui.imgPass->setPosition(Vec2(m_ui.imgBarrierName->getPosition().x + 100.0f,
		m_ui.imgBarrierName->getPosition().y - 30.0f));
	this->addChild(m_ui.imgPass, 4);
	m_ui.imgPass->setVisible(false);

	// 获得星级;
	if (param->nStarCount >= 0 && param->nStarCount <= MAX_BARRIER_STAR_COUNT)
	{
		ImageView* imgStar[3];
		for (int i = 0; i < 3; ++i)
		{
			oss.str("");
			oss << "Image/UIBarrier/Barrier/img_star""" << ((param->nStarCount < i+1) ? 4 : 3) << ".png";
			imgStar[i] = ImageView::create(oss.str());
			imgStar[i]->setPosition(Point(75.0f * (i-1), 10.0f * (i%2 - 1) - 72.0f) + m_ui.imgBarrierName->getPosition());
			imgStar[i]->setRotation((i-1)*15.0f);
			this->addChild(imgStar[i], 2);
		}
	}
	
	// 首次通关奖励;
	if (param->nStarCount <= 0 && param->mapFirstReward.size() > 0)
	{
		ImageView* imgFirstReward = ImageView::create("Image/UIBarrier/Barrier/img_shoucitongguan.png");
		imgFirstReward->setPosition(Point(0.0f, -400.0f) + m_ui.imgBarrierName->getPosition());
		this->addChild(imgFirstReward);
		ImageView* imgLine = ImageView::create("Image/UIBarrier/Barrier/img_xiaobiaoti_xian2.png");
		imgLine->setScaleX(0.5f);
		imgLine->setPosition(Point(0.0f, -15.0f) + imgFirstReward->getPosition());
		this->addChild(imgLine);
		int nIndex = 0;
		int nCount = param->mapFirstReward.size();
		for (auto iter = param->mapFirstReward.begin(); iter != param->mapFirstReward.end(); ++iter, ++nIndex)
		{
			UIToolHeader* _item = UIToolHeader::create(iter->first);
			_item->setScale(0.7f);
			_item->setNumEx(iter->second);
			_item->setPosition(Point(((1-nCount) / 2.0f + nIndex) * 85.0f, -45.0f) + imgLine->getPosition());
			this->addChild(_item);
		}
	}

	// 更新关卡当前难度的状态;
	setBarrierEnable(m_bIsEnabled);
}


void BarrierNode::setBarrierEnable(bool bIsEnabled)
{
	m_ui.imgBg->setEnabled(bIsEnabled);
	m_ui.imgBg->setBright(bIsEnabled);

	if (!bIsEnabled)
	{
		// 加灰暗蒙版;
		if (nullptr == m_ui.imgDisabled)
		{
			m_ui.imgDisabled = ImageView::create("Image/UIBarrier/Barrier/img_guanqia_weikaiqi.png");
			m_ui.imgDisabled->setPosition(m_ui.imgBg->getPosition());
			this->addChild(m_ui.imgDisabled, 3);
		}

		// 锁;
		if (nullptr == m_ui.imgLock)
		{
			m_ui.imgLock = ImageView::create("Image/UIBarrier/Barrier/img_suo.png");
			m_ui.imgLock->setPosition(m_ui.imgBg->getPosition());
			this->addChild(m_ui.imgLock, 3);
		}

		// 换禁用边框;
		ostringstream oss;
		oss << "Image/UIBarrier/Barrier/img_frame_" << m_nLv+1 << "_3.png";
		m_ui.imgFrame->loadTexture(oss.str());
		oss.str("");
	}
	else
	{
		if (nullptr != m_ui.imgDisabled)
		{
			m_ui.imgDisabled->removeFromParent();
			m_ui.imgDisabled = nullptr;
		}
		if (nullptr != m_ui.imgLock)
		{
			m_ui.imgLock->removeFromParent();
			m_ui.imgLock = nullptr;
		}

		// 刷新状态(换普通边框样式);
		updateSelected(m_bIsSelected);
	}

	// bossshow灰暗;
	if (m_nBossId != -1 && m_ui.imgBoss != nullptr && !bIsEnabled)
	{
		m_ui.imgBoss->setColor(Color3B(0x69, 0x69, 0x69));
	}

	// 新关卡标识;
	if (m_ui.imgNew != nullptr)
	{
		m_ui.imgNew->setVisible(bIsEnabled);
	}

	// 更新弹出状态;
	updatePopUp();
	
	// cocos2d-x 3.2版本，需显式调用;
	m_ui.imgBg->setTouchEnabled(bIsEnabled);
}


void BarrierNode::onBarrierClicked( Ref* ref, Widget::TouchEventType type )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		LayerBarrier* pParent = dynamic_cast<LayerBarrier*>(m_pParent);
		if (nullptr != pParent)
		{
			pParent->onBarrierSelected(m_nBarrierId);
		}
	}
}


void BarrierNode::updateSelected(bool bSelected)
{
	bool bPreSelected = m_bIsSelected;
	m_bIsSelected = bSelected;

	if (!m_bIsEnabled)
	{
		return;
	}

	// 边框变化;
	ostringstream oss;
	if (bSelected)
	{
		oss << "Image/UIBarrier/Barrier/img_frame_" << m_nLv+1 << "_2.png";
	}
	else
	{
		oss << "Image/UIBarrier/Barrier/img_frame_" << m_nLv+1 << "_1.png";
	}

	m_ui.imgFrame->loadTexture(oss.str());
	oss.str("");

	if (bPreSelected != bSelected)
	{
		// 弹出状态;
		updatePopUp();

		// 取消新关卡标识;
		updateNewFlag(false);
	}
}


void BarrierNode::updatePopUp()
{
	Action* seq = nullptr;
	Point ptDst(Vec2(m_ptOriPos.x, m_ptOriPos.y + 10.0f));

	if (m_bIsSelected && m_bIsEnabled)
	{
		if (!m_bIsPopUp)
		{
			seq = Sequence::create(MoveTo::create(0.1f, ptDst), nullptr);
			m_bIsPopUp = true;
		}
	}
	else
	{
		if (m_bIsPopUp)
		{
			seq = Sequence::create(MoveTo::create(0.05f, m_ptOriPos), nullptr);
			m_bIsPopUp = false;
		}
	}

	if (nullptr != seq)
	{
		this->runAction(seq);
	}
}

void BarrierNode::updateNewFlag( bool bNew )
{
	if (bNew)
	{
		if (m_ui.imgNew == nullptr)
		{
			m_ui.imgNew = ImageView::create("Image/UIBarrier/Barrier/img_xinkaiqi.png");
			if (nullptr != m_ui.imgNew)
			{
				m_ui.imgNew->setPosition(Vec2(m_ui.imgPass->getPosition().x - 20.0f, m_ui.imgPass->getPosition().y + 55.0f));
				this->addChild(m_ui.imgNew, 4);
			}
		}
	}
	else
	{
		if (m_ui.imgNew != nullptr)
		{
			m_ui.imgNew->removeFromParent();
			m_ui.imgNew = nullptr;
		}
	}
}

/*
string BarrierNode::getNumText(int num)
{
	string strText;
	strText.assign("");
	switch (num)
	{
	case 1:		strText.assign("一");		break;
	case 2:		strText.assign("二");		break;
	case 3:		strText.assign("三");		break;
	case 4:		strText.assign("四");		break;
	case 5:		strText.assign("五");		break;
	case 6:		strText.assign("六");		break;
	case 7:		strText.assign("七");		break;
	case 8:		strText.assign("八");		break;
	case 9:		strText.assign("九");		break;
	case 10:	strText.assign("十");		break;
	default:
		break;
	}

	return strText;
}
*/