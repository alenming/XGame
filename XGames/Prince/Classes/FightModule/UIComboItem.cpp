#include "UIComboItem.h"
#include "CocoStudio.h"
#include "Utils/StringFormat.h"


UIComboItem::UIComboItem()
	: mComboCount(0)
	, mNumberAtlas(nullptr)
	, mComboAnimation(nullptr)
{

}

UIComboItem::~UIComboItem()
{
	
}

UIComboItem* UIComboItem::create(bool isLeft)
{
	UIComboItem* comboItem = new UIComboItem();
	if (comboItem->init(isLeft))
	{
		comboItem->autorelease();
		return comboItem;
	}
	else
	{
		delete comboItem;
		return nullptr;
	}
}

bool UIComboItem::init(bool isLeft)
{
	if (!Node::init())
	{
		return false;
	}

	this->mIsLeft = isLeft;

	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/AniCombo/AniCombo0.png",
		"Image/AniCombo/AniCombo0.plist",
		"Image/AniCombo/AniCombo.ExportJson");

	mNumberAtlas = TextAtlas::create("0", "Image/AniCombo/normal.png", 750 / 10, 121, "0");
	mNumberAtlas->setAnchorPoint(Point::ANCHOR_MIDDLE);

	//错位问题修正
	if (isLeft)
	{
		mNumberAtlas->setPosition(Point(0, -15));
	}
	else
	{
		mNumberAtlas->setPosition(Point(0, 15));
	}
	

	mComboAnimation = Armature::create("AniCombo");
	Bone* bone = mComboAnimation->getBone("num");
	bone->addDisplay(mNumberAtlas, 0);

	this->addChild(mComboAnimation);
	this->setVisible(false);

	return true;

}

string UIComboItem::getAnimationName()
{
	return mIsLeft ? "comboL" : "comboR";
}

void UIComboItem::setComboCount(int comboCount)
{
	mComboCount = comboCount;

	if (comboCount <= 0)
	{
		this->setVisible(false);
		return;
	}
	else
	{
		this->setVisible(true);
	}

	this->setVisible(true);
	mNumberAtlas->setString(TO_STR(mComboCount));
	mComboAnimation->getAnimation()->play(getAnimationName());

// 	if (mComboCount < 10)
// 	{
// 		mYellowCombo.mRoot->setVisible(true);
// 		mYellowCombo.mNumberAtlas->setString(TO_STR(mComboCount));
// 	}
// 	else if(mComboCount >= 10 && mComboCount < 20)
// 	{
// 		mRedCombo.mRoot->setVisible(true);
// 		mRedCombo.mNumberAtlas->setString(TO_STR(mComboCount));
// 	}
// 	else
// 	{
// 		mBlueCombo.mRoot->setVisible(true);
// 		mBlueCombo.mNumberAtlas->setString(TO_STR(mComboCount));
// 	}

}

void UIComboItem::addComboCount()
{
	mComboCount++;
	setComboCount(mComboCount);
}


