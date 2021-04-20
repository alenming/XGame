#include "ActivityBarrierNode.h"
#include "ActivityModel.h"
#include "HeroModule/HeroModel.h"
#include "DataManager/DataManager.h"

ActivityBarrierNode::ActivityBarrierNode(void)
	: m_pParent(nullptr)
	, m_nActivityId(-1)
	, m_nActivityBarrierId(-1)
	, m_nType(ACTIVITY_TYPE::ACT_TYPE_CHIP_JiangHuChuanWen)
{
}


ActivityBarrierNode::~ActivityBarrierNode(void)
{
}

ActivityBarrierNode* ActivityBarrierNode::create( Layer* pParent, int nActivityId, ACTIVITY_TYPE type, ActivityBarrierInfo* barrierInfo, int nLv )
{
	ActivityBarrierNode* _node = new ActivityBarrierNode;
	if (nullptr != _node && _node->init(pParent, nActivityId, type, barrierInfo, nLv))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool ActivityBarrierNode::init( Layer* pParent, int nActivityId, ACTIVITY_TYPE type, ActivityBarrierInfo* barrierInfo, int nLv )
{
	if (!Node::init())
	{
		return false;
	}

	if (nullptr == pParent)
	{
		return false;
	}

	m_pParent = pParent;
	m_nActivityId = nActivityId;
	m_nActivityBarrierId = barrierInfo->nBarrierId;
	m_nType = type;

	initUI(barrierInfo, nLv);

	return true;
}

void ActivityBarrierNode::initUI( ActivityBarrierInfo* barrierInfo, int nLv )
{
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIActivity/UIDifficultyNode.ExportJson"));
	this->addChild(m_ui.pRoot);
	this->setContentSize(m_ui.pRoot->getContentSize());

	// 难度标题;
	m_ui.imgTitle = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Difficulty"));

	// 困难级别(1:简单 2:普通 3:困难);
	if (nLv == 2)
	{
		m_ui.imgTitle->loadTexture("Image/UINewtower/Icon/img_putong.png");
	}
	else if (nLv == 3)
	{
		m_ui.imgTitle->loadTexture("Image/UINewtower/Icon/img_kunnan.png");
	}

	// 背景;
	m_ui.pnlBg = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Bg"));
	ostringstream oss;
	oss << "Image/UIActivity/Img/" << barrierInfo->nResId << ".png";
	m_ui.pnlBg->setBackGroundImage(oss.str());
	oss.str("");

	// 等级限制和遮罩;
	m_ui.txtLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Level"));
	m_ui.txtLevel->setFontName(FONT_FZZHENGHEI);
	oss.str("");
	oss << barrierInfo->nEnterLevel << DataManager::getInstance()->searchCommonTexdtById(10001);
	m_ui.txtLevel->setString(oss.str());
	oss.str("");
	setTextAddStroke(m_ui.txtLevel, Color3B(0x6E, 0x28, 0x10), 3);
	m_ui.imgMask = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Dense_fog"));
	m_ui.imgMask->setVisible(HeroModel::getInstance()->getMainHeroLevel() < barrierInfo->nEnterLevel);

	// 关卡本身;
	m_ui.btnFrame = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Frame"));
	m_ui.btnFrame->setEnabled(!m_ui.imgMask->isVisible());
	if (nLv == 2)
	{
		m_ui.btnFrame->loadTextureNormal("Image/UINewtower/Icon/btn_putong_1.png");
		m_ui.btnFrame->loadTexturePressed("Image/UINewtower/Icon/btn_putong_2.png");
	}
	else if (nLv == 3)
	{
		m_ui.btnFrame->loadTextureNormal("Image/UINewtower/Icon/btn_kunnan_1.png");
		m_ui.btnFrame->loadTexturePressed("Image/UINewtower/Icon/btn_kunnan_2.png");
	}
	m_ui.btnFrame->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			LayerActivity* parent = dynamic_cast<LayerActivity*>(m_pParent);
			if (parent != nullptr)
			{
				parent->onChallenge(m_nActivityBarrierId);
			}
		}
	});

	// 扫荡;
	m_ui.btnSweep = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Sweep"));
	m_ui.btnSweep->setVisible(ActivityModel::getInstance()->isActivityBarrierPassed(m_nActivityId, m_nActivityBarrierId));

	// [江湖传闻]需求修改，直接屏蔽扫荡 added by Phil 12/27/2015 @zcjoy;
	if (m_nType == ACT_TYPE_CHIP_JiangHuChuanWen)
	{
		m_ui.btnSweep->setVisible(false);
		m_ui.btnSweep->setEnabled(false);
	}
	else
	{
		m_ui.btnSweep->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (type == Widget::TouchEventType::ENDED)
			{
				LayerActivity* parent = dynamic_cast<LayerActivity*>(m_pParent);
				if (parent != nullptr)
				{
					parent->onSweep(m_nActivityBarrierId);
				}
			}
		});
	}
}
