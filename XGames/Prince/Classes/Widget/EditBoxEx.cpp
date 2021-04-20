//
//  EditBoxEx.cpp
//  LucyOL
//
//  Created by Paddy on 14-5-20.
//
//

#include "EditBoxEx.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace cocos2d::ui;

EditBoxEx* EditBoxEx::create(cocos2d::ui::TextField * pTextField)
{
    EditBoxEx *pRet = new EditBoxEx();
	if (pRet != NULL && pRet->initWithSizeAndBackgroundSprite(pTextField->getSize(), Scale9Sprite::create()))
	{
		pRet->initWithTextField(pTextField);
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}

	return pRet;
}

void EditBoxEx::initWithTextField( cocos2d::ui::TextField * pTextField )
{
	if (pTextField->isPasswordEnabled())
	{
		setInputFlag(EditBox::InputFlag::PASSWORD);
	}
	setContentSize(pTextField->getContentSize());
	setPosition(pTextField->getPosition());
	setMaxLength(pTextField->getMaxLength());
	setReturnType(EditBox::KeyboardReturnType::DONE);

	setFontColor(pTextField->getColor());
	setFontSize(pTextField->getFontSize());

	pTextField->setEnabled(false);
	pTextField->setVisible(false);
}