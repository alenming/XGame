#include "AniButton.h"
#include "Utils/SoundUtils.h"

AniButton* AniButton::create(Button *parent,string fileName,AniButtonCallback callBack /*= nullptr*/)
{
	AniButton* widget = new AniButton();
	if (widget && widget->init())
	{
		//父节点透明隐藏 但不隐藏子节点
		parent->setOpacity(0);
		parent->setCascadeOpacityEnabled(false);
		parent->addChild(widget);

		widget->setOpacity(0);
		widget->setCascadeOpacityEnabled(false);
		widget->loadTextureNormal(parent->getNormalFileName(),TextureResType::PLIST);
		widget->setPosition(Vec2(parent->getContentSize().width/2,parent->getContentSize().height/2));
		widget->initWithButton(fileName,callBack);

		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

void AniButton::initWithButton(string fileName,AniButtonCallback callBack)
{
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/" + fileName + "/" + fileName + ".ExportJson");
	Armature* pArmature = Armature::create(fileName);//名字需要和 .ExportJson一致
	pArmature->getAnimation()->playByIndex(0,-1,1);
	pArmature->setPosition(Vec2(getContentSize().width/2,getContentSize().height/2));//设置动画精灵位置 
	addChild(pArmature,0);

	auto back = [=](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
			pArmature->setScale(0.9f);
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			if (callBack)
			{
				callBack(sender, type);
			}
			pArmature->setScale(1.0f);
		}
		else
		{
			pArmature->setScale(1.0f);
		}
	};
	addTouchEventListener(back);
}
