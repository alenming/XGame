#include "ButtonSwitch.h"

ButtonSwitch* ButtonSwitch::create(const char* bgImgRes, const char* onImgRes, const char* offImgRes, const char* thumbImgRes)
{
	ButtonSwitch* pRet = new ButtonSwitch();
	if(pRet != nullptr)
	{
		pRet->initSprite(Sprite::create(bgImgRes), Sprite::create(onImgRes), Sprite::create(offImgRes), 
			Sprite::create(thumbImgRes), 
			Label::create(), Label::create());

		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

void ButtonSwitch::initSprite(Sprite* bgSprite, Sprite* onSprite, Sprite* offSprite, Sprite* thumbSprite, Label* onLabel, Label* offLabel)
{
	this->initWithMaskSprite(bgSprite, onSprite, offSprite, thumbSprite, onLabel, offLabel);
}