#include "ChapterNode.h"
#include "LayerChapter.h"


ChapterNode::ChapterNode(void)
	: m_bIsEnabled(false)
	, m_pLogicParent(nullptr)
	, m_nChapterId(INVALID_CHAPTER_OR_BARRIER_ID)
{
	//m_imgBuildingFrame = nullptr;
}


ChapterNode::~ChapterNode(void)
{
}


ChapterNode* ChapterNode::create(const int nChapterId)
{
	ChapterNode* chapter = new ChapterNode;
	if (nullptr != chapter && chapter->init(nChapterId))
	{
		chapter->autorelease();
		return chapter;
	}

	CC_SAFE_DELETE(chapter);
	return nullptr;
}

bool ChapterNode::init(const int nChapterId)
{
	if (!Node::init())
	{
		return false;
	}

	m_nChapterId = nChapterId;

	initUI();
	return true;
}


void ChapterNode::initUI()
{
	// 初始化剧情关卡按钮组件;
	m_ui.imgChapter = ImageView::create("Image/UIChapter/Title/btn_zhangjiedian_1.png");
	ostringstream oss;
	oss << "Image/UIChapter/Title/" << m_nChapterId << "_1.png";
	m_ui.imgTitle = ImageView::create(oss.str());
	m_ui.imgTitle->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgTitle->setPosition(Vec2(-90.0f, 40.0f));
	//m_ui.imgChapter->addChild(m_ui.imgTitle, 2);
	this->addChild(m_ui.imgTitle, 2);
	this->addChild(m_ui.imgChapter);

	////////// 进度背景;
	m_ui.imgCountBg = ImageView::create("Image/UIChapter/Title/img_di1.png");
	m_ui.imgCountBg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgCountBg->setPosition(Vec2(70.0f, -15.0f));
	m_ui.imgChapter->addChild(m_ui.imgCountBg);

	// 通关标记;
	/*
	m_ui.imgPass = ImageView::create("Image/UIChapter/Title/img_zhan.png");
	m_ui.imgPass->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgPass->setPosition(Vec2(-5.0f, 21.0f));
	m_ui.imgPass->setScale(0.5f);
	m_ui.imgCountBg->addChild(m_ui.imgPass);
	*/

	// 星星;
	m_ui.imgStar = ImageView::create("Image/UIChapter/Title/img_star3.png");
	m_ui.imgStar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_ui.imgStar->setPosition(Vec2(-5.0f, 21.0f));
	m_ui.imgStar->setScale(0.6f);
	m_ui.imgCountBg->addChild(m_ui.imgStar);

	// 进度 - 当前;
	m_ui.labCur = TextAtlas::create("0", "Image/UIChapter/Title/img_number1.png", 26, 31, "0");
	m_ui.labCur->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_ui.labCur->setPosition(Vec2(15.0f, 9.0f));
	m_ui.labCur->setScale(0.8f);
	m_ui.imgCountBg->addChild(m_ui.labCur);

	// 进度 - 分隔符;
	m_ui.imgSlash = ImageView::create("Image/UIChapter/Title/img_number1_xiehao.png");
	m_ui.imgSlash->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	int nWidth = m_ui.labCur->getContentSize().width;
	m_ui.imgSlash->setPosition(Vec2(m_ui.labCur->getPosition().x + m_ui.labCur->getContentSize().width, 22.0f));
	m_ui.imgSlash->setScale(0.8f);
	m_ui.imgCountBg->addChild(m_ui.imgSlash);

	// 进度 - 总;
	m_ui.labTotal = TextAtlas::create("0", "Image/UIChapter/Title/img_number1.png", 26, 31, "0");
	m_ui.labTotal->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_ui.labTotal->setPosition(Vec2(m_ui.imgSlash->getPosition().x + 5.0f, 9.0f));
	m_ui.labTotal->setScale(0.8f);
	m_ui.imgCountBg->addChild(m_ui.labTotal);

	// 初始化为禁用;
	//m_ui.imgPass->setVisible(false);
	//setEnabled(false);
	updateStarCount(0, 0);
}


void ChapterNode::onChapterClicked(bool bClicked)
{
	Action* seq = nullptr;
	Action* seq1 = nullptr;
	if (bClicked)
	{
		// 建筑发光;
		/*
		if (nullptr == m_imgBuildingFrame)
		{
			ostringstream oss;
			oss << "Image/UIChapter/Building/" << m_ui.imgChapter->getName().c_str() << "_1.png";
			m_imgBuildingFrame = ImageView::create(oss.str());
			if (nullptr != m_imgBuildingFrame)
			{
				m_imgBuildingFrame->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				m_imgBuildingFrame->setPosition(Vec2(0.0f, 0.0f));
				m_ui.imgChapter->addChild(m_imgBuildingFrame, 1);
			}
			oss.str("");
		}
		*/

		// 标题放大;
		seq = Sequence::create( ScaleTo::create(0.1f, 0.9f), nullptr);
		seq1 = Sequence::create( ScaleTo::create(0.1f, 0.9f), nullptr);
	}
	else
	{
		// 取消发光;
		/*
		if (nullptr != m_imgBuildingFrame)
		{
			m_imgBuildingFrame->removeFromParent();
			m_imgBuildingFrame = nullptr;
		}
		*/

		// 标题还原;
		seq = Sequence::create( ScaleTo::create(0.05f, 1.0f), nullptr);
		seq1 = Sequence::create( ScaleTo::create(0.05f, 1.0f), nullptr);
	}
	m_ui.imgChapter->runAction(seq);
	m_ui.imgTitle->runAction(seq1);
}


void ChapterNode::setEnabled(bool bEnabled)
{
	m_bIsEnabled = bEnabled;
	//m_ui.imgTitle->setVisible(bEnabled);
	int picId = bEnabled ? 1 : 3;
	ostringstream oss;
	oss << "Image/UIChapter/Title/" << m_nChapterId << "_" << picId << ".png";
	m_ui.imgTitle->loadTexture(oss.str());
	//m_ui.imgPass->setVisible(bEnabled);
	//m_ui.imgCountBg->setVisible(bEnabled);
	m_ui.imgChapter->setVisible(bEnabled);
}


void ChapterNode::updateStarCount(int nCur, int nTotal)
{
	/*
	if (nCur == nTotal)
	{
		m_ui.imgPass->setVisible(true);
	}
	else if (nCur < nTotal)
	{
		m_ui.imgPass->setVisible(false);
	}
	else
	*/
	if (nCur > nTotal)
	{
		nCur = nTotal;
		//m_ui.imgPass->setVisible(true);
	}

	ostringstream oss;
	oss << nCur;
	m_ui.labCur->setString(oss.str());
	m_ui.imgSlash->setPosition(Vec2(m_ui.labCur->getPosition().x + m_ui.labCur->getContentSize().width, 22.0f));
	oss.str("");
	oss << nTotal;
	m_ui.labTotal->setString(oss.str());
	m_ui.labTotal->setPosition(Vec2(m_ui.imgSlash->getPosition().x + 5.0f, 9.0f));
	oss.str("");
}

void ChapterNode::updateStarReward( bool bValid )
{
	if (bValid)
	{
		if (m_ui.imgStarReward == nullptr)
		{
			// 星级奖励;
			m_ui.imgStarReward = ImageView::create("Image/UIChapter/Btn/img_baoxiang3_2_1.png");
			m_ui.imgStarReward->setPosition(Vec2(m_ui.imgTitle->getPosition().x, m_ui.imgTitle->getPosition().y + 190.0f));
			this->addChild(m_ui.imgStarReward);
		}
	}
	else
	{
		if (m_ui.imgStarReward != nullptr)
		{
			m_ui.imgStarReward->removeFromParent();
			m_ui.imgStarReward = nullptr;
		}
	}
}
