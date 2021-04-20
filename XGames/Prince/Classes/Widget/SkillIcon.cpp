#include "SkillIcon.h"
#include "Utils/ResourceUtils.h"
#include "DataManager/DataManager.h"

SkillIcon::SkillIcon()
	: mTempletId(-1)
	, mBorderBtn(nullptr)
	, mIconImg(nullptr)
	, mShowTips(false)
{

}

SkillIcon::~SkillIcon()
{

}

SkillIcon* SkillIcon::create( int skillTempletId )
{
	SkillIcon* skillIcon = new SkillIcon();
	if (skillIcon && skillIcon->init(skillTempletId))
	{
		skillIcon->autorelease();
		return skillIcon;
	}
	else
	{
		delete skillIcon;
		return nullptr;
	}
}

bool SkillIcon::init( int skillTempletId )
{
	if (!Node::init())
	{
		return false;
	}

	mTempletId = skillTempletId;
	RowData* skillData = DataManager::getInstance()->searchSkillById(skillTempletId);
	if (!skillData)
	{
		return false;
	}

	int resId = skillData->getIntData("resId");
	mIconImg = ImageView::create(ResourceUtils::getSmallIconPath(resId));
	addChild(mIconImg);
	mBorderBtn = Button::create(ResourceUtils::getIconGlobalPath("btn_kuang_jineng_1.png"), 
		ResourceUtils::getIconGlobalPath("btn_kuang_jineng_2.png"));
	mBorderBtn->addTouchEventListener(CC_CALLBACK_2(SkillIcon::onBtnClicked, this));
	setShowTips(false);
	setBtnEnable(false);
	addChild(mBorderBtn);

	setContentSize(mBorderBtn->getContentSize());

	return true;
}

void SkillIcon::onBtnClicked(Ref* ref, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		Node* parent = this->getParent();
		if (parent && mShowTips)
		{
			if (!parent->getChildByName("tipSkill"))
			{
				parent->removeChildByName("tipSkill");
			}
			SkillTips*	 tipSkill = SkillTips::create();
			parent->addChild(tipSkill,0,"tipSkill");

			tipSkill->setSkill(mTempletId);
			tipSkill->setPosition(this->getPosition() + Point(-150, 120));
		}
	}
	else if (type == Widget::TouchEventType::ENDED || type == Widget::TouchEventType::CANCELED)
	{
		Node* parent = this->getParent();
		if (parent->getChildByName("tipSkill"))
		{
			parent->removeChildByName("tipSkill");
		}
	}
}

void SkillIcon::setShowTips(bool showTips)
{
	mShowTips = showTips;
	if (showTips)
	{
		setBtnEnable(true);
	}
}

void SkillIcon::setBtnEnable(bool enable)
{
	if (mBorderBtn)
	{
		mBorderBtn->setTouchEnabled(enable);
	}
}

