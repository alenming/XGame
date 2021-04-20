#include "UIFighterDamage.h"
#include "Utils/StringFormat.h"

using namespace cocos2d;
using namespace cocos2d::ui;

const int	nATLAS_PNG_WIDTH	= 280;
const int	nATLAS_PNG_HEIGHT	= 47;

const float fVER_SPACE_DAMAGE_2_FIGHTER = 155;	// 开始位置高度，离战士重心锚点的距离


UIFighterDamage::UIFighterDamage()
	: mScaleValue(0.0f)
{

}

UIFighterDamage::~UIFighterDamage()
{

}

UIFighterDamage* UIFighterDamage::create(int damage, int harmType)
{
	UIFighterDamage *pRet = new UIFighterDamage();
	if (pRet && pRet->init(damage, harmType))
	{
		pRet->setPosition(Point(0, fVER_SPACE_DAMAGE_2_FIGHTER));
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool UIFighterDamage::init(int damage, int harmType)
{
	if (!Node::init())
	{
		return false;
	}

	//暴击缩放值
	if (harmType == HarmParam::HARM_TYPE::HARM_CRITICAL || 
		harmType == HarmParam::HARM_TYPE::HARM_RESTRICTION)
	{
		mScaleValue = 2.0f;
	}
	else
	{
		mScaleValue = 1.5f;
	}

	this->setCascadeOpacityEnabled(true);
	createDamageText(harmType, damage);
	return true;
}

void UIFighterDamage::createDamageText(int harmType, int damage)
{
	TextAtlas* atlas = getDamageNumAtlas(harmType, damage);
	ImageView* image = getDamageTextImg(harmType);

	if (atlas && !image) //只有数字
	{
		this->addChild(atlas);
	}
	else if(!atlas && image) //只有文字
	{
		this->addChild(image);
	}
	else if(atlas && image) //数字、文字都有
	{
		image->setAnchorPoint(Point(0.0f, 0.0f));
		atlas->setAnchorPoint(Point(0.0f, 0.0f));
		Size imageSize = image->getContentSize();
		Size atlasSize = atlas->getContentSize();
		image->setPosition(Point(0, (atlasSize.height - imageSize.height) / 2.0f));
		atlas->setPosition(Point(imageSize.width + 5, 0));
		this->addChild(image);
		this->addChild(atlas);
		this->setContentSize(Size(imageSize.width + atlasSize.width + 5, atlasSize.height));
	}
	else    //啥也没有，你特么显示啥
	{

	}
	this->setAnchorPoint(Point(0.5f, 0.5f));
}

//获取伤害数字
TextAtlas* UIFighterDamage::getDamageNumAtlas(int harmType, int damage)
{
	string path = "";
	switch (harmType)
	{
	case HarmParam::HARM_BLEED:
		path = "Image/Fight/harm/img_bleed_number.png";
		break;
	case HarmParam::HARM_POISON:
		path = "Image/Fight/harm/img_poisoning_number.png";
		break;
	case HarmParam::HARM_NORMAL:
		path = "Image/Fight/harm/img_harm_number.png";
		break;
	case HarmParam::HARM_CURE:
	case HarmParam::HARM_BLOOD:
		path = "Image/Fight/harm/img_cure_number.png";
		break;
	case HarmParam::HARM_CRITICAL:
		path = "Image/Fight/harm/img_critical_number.png";
		break;
	case HarmParam::HARM_RESTRICTION:
		path = "Image/Fight/harm/img_restriction_number.png";
		break;
	case HarmParam::HARM_INVINCIBLE:
	case HarmParam::HARM_DODGE:
		path = "";
		break;
	default:
		break;
	}

	damage = abs(damage);
	if (!path.empty())
	{
		TextAtlas* damageAtlas = TextAtlas::create(TO_STR(damage),
			path, nATLAS_PNG_WIDTH/10, nATLAS_PNG_HEIGHT, "0");
		return damageAtlas;
	}
	return nullptr;
}

//获取伤害
ImageView* UIFighterDamage::getDamageTextImg(int harmType)
{
	string path = "";
	switch (harmType)
	{
	case HarmParam::HARM_BLEED:
		path = "Image/Fight/harm/img_zi_bleed.png";
		break;
	case HarmParam::HARM_POISON:
		path = "Image/Fight/harm/img_zi_poisoning.png";
		break;
	case HarmParam::HARM_NORMAL:
		path = "";
		break;
	case HarmParam::HARM_CURE:
		path = "Image/Fight/harm/img_zi_cure.png";
		break;
	case HarmParam::HARM_BLOOD:
		path = "Image/Fight/harm/img_zi_cure2.png";
		break;
	case HarmParam::HARM_CRITICAL:
		path = "Image/Fight/harm/img_zi_critical.png";
		break;
	case HarmParam::HARM_RESTRICTION:
		path = "Image/Fight/harm/img_zi_restriction.png";
		break;
	case HarmParam::HARM_INVINCIBLE:
		path = "Image/Fight/harm/img_zi_immune.png";
		break;
	case HarmParam::HARM_DODGE:
		path = "Image/Fight/harm/img_zi_dodge.png";
		break;
	default:
		break;
	}

	if (!path.empty())
	{
		ImageView* damageImage = ImageView::create(path);
		return damageImage;
	}
	return nullptr;
}

void UIFighterDamage::onEnterTransitionDidFinish()
{
	Node::onEnterTransitionDidFinish();

	CallFunc* pCallBack = CallFunc::create(CC_CALLBACK_0(UIFighterDamage::onActionFinish, this));

	Sequence* seq = Sequence::create(
		ScaleTo::create(0.1f, mScaleValue),
		ScaleTo::create(0.05f, 1.0f),
		DelayTime::create(0.35f),
		Spawn::create(
			MoveBy::create(0.4, Point(0, 120.0f)),
			FadeOut::create(0.4),
			nullptr
		),
		pCallBack,
		nullptr
	);

	this->runAction(seq);
}

void UIFighterDamage::onActionFinish()
{
	this->removeFromParent();
}



