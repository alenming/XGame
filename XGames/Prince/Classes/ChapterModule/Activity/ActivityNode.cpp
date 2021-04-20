#include "ActivityNode.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "DoubleBonus/DoubleBonusModel.h"

ActivityNode::ActivityNode(void)
	: m_pParent(nullptr)
	, m_nActivityId(-1)
	, m_nType(ACTIVITY_TYPE::ACT_TYPE_CHIP_JiangHuChuanWen)
{
	m_pParent = nullptr;
	DoubleBonusModel::getInstance()->addObserver(this);
}


ActivityNode::~ActivityNode(void)
{
	DoubleBonusModel::getInstance()->removeObserver(this);
}

ActivityNode* ActivityNode::create(Layer* pParent, UI_ACTIVITY_NODE_PARAM param)
{
	ActivityNode* _node = new ActivityNode;
	if (nullptr != _node && _node->init(pParent, param))
	{
		_node->autorelease();
		return _node;
	}

	CC_SAFE_DELETE(_node);
	return nullptr;
}

bool ActivityNode::init(Layer* pParent, UI_ACTIVITY_NODE_PARAM param)
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
	m_nActivityId = param.nActivityId;
	m_nType = param.nType;

	initUI(param);

	return true;
}


void ActivityNode::initUI(UI_ACTIVITY_NODE_PARAM param)
{
	m_ui.pnlRoot = Layout::create();
	this->addChild(m_ui.pnlRoot);

	// 活动图片;
	ostringstream oss;
	oss << "Image/UIActivity/Img/" << getResId(param.nActivityId) << ".png";
	m_ui.imgBg = ImageView::create(oss.str());
	m_ui.imgBg->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	this->setContentSize(m_ui.imgBg->getContentSize());
	m_ui.pnlRoot->setContentSize(m_ui.imgBg->getContentSize());
	m_ui.pnlRoot->addChild(m_ui.imgBg);

	// 敌方阵容;
	for (int i = 0; i < param.vcEnemy.size(); ++i)
	{
		UIToolHeader* _item = UIToolHeader::create(param.vcEnemy.at(i));
		_item->setScale(0.55f);
		_item->setPosition(Vec2(100.0f + (i%3)*70.0f + (i/3)*95.0f, 360.0f - (i/3)*70.0f));
		m_ui.imgBg->addChild(_item);
	}

	// 掉落;
	for (int i = 0; i < param.vcDrops.size(); ++i)
	{
		UIToolHeader* _item = UIToolHeader::create(param.vcDrops.at(i));
		_item->checkChip(param.vcDrops.at(i));
		_item->setScale(0.6f);
		_item->setPosition(Vec2(95.0f + i*70.0f, 180.0f));
		m_ui.imgBg->addChild(_item);
	}

	// 剩余次数;
	oss.str("");
	oss << param.nValidCount << "/" << param.nTotalCount;
	m_ui.txtCount = Text::create(STR_UTF8(oss.str()), FONT_FZZHENGHEI, 30);
	setTextAddStroke(m_ui.txtCount, Color3B(0x77, 0x07, 0x07), 3);
	m_ui.txtCount->setPosition(Vec2(375.0f, 612.0f));
	m_ui.imgBg->addChild(m_ui.txtCount);

	// 进入关卡;
	m_ui.btnEnter = Button::create("Image/UIActivity/Img/btn_jinruguanqia_1.png", "Image/UIActivity/Img/btn_jinruguanqia_2.png");
	m_ui.btnEnter->setPosition(Vec2(m_ui.imgBg->getContentSize().width/2 + 10.0f, 102.0f));
	m_ui.imgBg->addChild(m_ui.btnEnter);
	m_ui.btnEnter->addTouchEventListener([=](Ref* ref, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			LayerActivity* pParent = static_cast<LayerActivity*>(m_pParent);
			if (nullptr != pParent)
			{
				pParent->onActivityEnter(m_nActivityId);
			}
		}
	});

	// Tips;
	m_ui.txtTips = Text::create(getTips(param.nType), FONT_FZZHENGHEI, 20);
	m_ui.txtTips->setColor(Color3B(0xDA, 0xDB, 0x5D));
	setTextAddStroke(m_ui.txtTips, Color3B::BLACK, 2);
	m_ui.txtTips->setPosition(Vec2(m_ui.btnEnter->getPosition().x, m_ui.btnEnter->getPosition().y - 60.0f));
	m_ui.imgBg->addChild(m_ui.txtTips);

	// 是否可用;
	this->setSelected(false);

	// 双倍奖励;
	showDoubleBonusTips(DoubleBonusModel::getInstance()->isDoubleBonusValid(checkTypeByActivityType()));
}

void ActivityNode::setSelected(bool bSelected)
{
	m_ui.btnEnter->setVisible(bSelected);
	m_ui.txtTips->setVisible(bSelected);

	// 边框;
	if (bSelected)
	{
		if (m_ui.imgFrame == nullptr)
		{
			m_ui.imgFrame = ImageView::create("Image/UIActivity/Img/img_bg_xuanzhong.png");
			m_ui.imgFrame->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			m_ui.pnlRoot->addChild(m_ui.imgFrame, 2);
		}
	}
	else
	{
		if (m_ui.imgFrame != nullptr)
		{
			m_ui.imgFrame->removeFromParent();
			m_ui.imgFrame = nullptr;
		}
	}
	
}

void ActivityNode::updateState(ActivityState* state)
{
	// 挑战次数;
	ostringstream oss;
	oss << state->nValidCount << "/" << state->nTotalCount;
	if (nullptr != m_ui.txtCount)
	{
		m_ui.txtCount->setString(STR_UTF8(oss.str()));
		setTextAddStroke(m_ui.txtCount, Color3B(0x77, 0x07, 0x07), 3);
	}
	oss.str("");
}

int ActivityNode::getResId( int nActivityId )
{
	RowData* row = DataManager::getInstance()->searchActivityShowById(nActivityId);
	assert(nullptr != row);
	return row->getIntData("resId");
}

std::string ActivityNode::getTips( ACTIVITY_TYPE type )
{
	string tips("");
	switch (type)
	{
	case ACT_TYPE_COIN_LiangShanMiCang:
		//tips.assign("Tips:挑战成功可以获得大量银币！");
		tips.assign(DataManager::getInstance()->searchCommonTexdtById(10039));
		break;
	case ACT_TYPE_CHIP_JiangHuChuanWen:
		//tips.assign("Tips:与敌方阵容相同可降低难度！");
		tips.assign(DataManager::getInstance()->searchCommonTexdtById(10040));
		break;
	case ACT_TYPE_EXP_JiuJingKaoYan:
		//tips.assign("Tips:挑战成功可以获得大量经验丹！");
		tips.assign(DataManager::getInstance()->searchCommonTexdtById(10041));
		break;
	default:
		break;
	}
	return tips;
}

void ActivityNode::updateSelf( void* data )
{
	if (nullptr == data)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) data;

	switch (param->id)
	{
	case DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE::START:
	case DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE::END:
		{
			DOUBLE_BONUS_TYPE doublebonusType = *((DOUBLE_BONUS_TYPE*)(param->updateData));
			if (m_nType == checkTypeByDoubleBonusType(doublebonusType))
			{
				showDoubleBonusTips(param->id == DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE::START);
			}
		}
		break;

	default:
		break;
	}
}

void ActivityNode::showDoubleBonusTips( bool bShow )
{
	ImageView* img = dynamic_cast<ImageView*>(this->getChildByName("double_bonus"));
	if (bShow)
	{
		if (nullptr == img)
		{
			img = ImageView::create("Image/Icon/global/20002.png");
			img->setName("double_bonus");
			img->setPosition(Vec2(235.0f, 240.0f));
			this->addChild(img);
		}
	}
	else
	{
		if (img != nullptr)
		{
			img->removeFromParent();
			img = nullptr;
		}
	}
}

ACTIVITY_TYPE ActivityNode::checkTypeByDoubleBonusType( DOUBLE_BONUS_TYPE type )
{
	ACTIVITY_TYPE activityType = ACT_TYPE_CHIP_JiangHuChuanWen;
	switch (type)
	{
	case DOUBLE_BONUS_TYPE_ACTIVITY_CHIP:
		activityType = ACT_TYPE_CHIP_JiangHuChuanWen;
		break;
	case DOUBLE_BONUS_TYPE_ACTIVITY_EXP:
		activityType = ACT_TYPE_EXP_JiuJingKaoYan;
		break;
	case DOUBLE_BONUS_TYPE_ACTIVITY_COIN:
		activityType = ACT_TYPE_COIN_LiangShanMiCang;
		break;
	default:
		activityType = ACT_TYPE_CHIP_JiangHuChuanWen;
		break;
	}
	return activityType;
}

DOUBLE_BONUS_TYPE ActivityNode::checkTypeByActivityType()
{
	DOUBLE_BONUS_TYPE type = DOUBLE_BONUS_TYPE_NONE;
	switch (m_nType)
	{
	case ACT_TYPE_CHIP_JiangHuChuanWen:
		type = DOUBLE_BONUS_TYPE_ACTIVITY_CHIP;
		break;
	case ACT_TYPE_EXP_JiuJingKaoYan:
		type = DOUBLE_BONUS_TYPE_ACTIVITY_EXP;
		break;
	case ACT_TYPE_COIN_LiangShanMiCang:
		type = DOUBLE_BONUS_TYPE_ACTIVITY_COIN;
		break;
	default:
		break;
	}
	return type;
}
