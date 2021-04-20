#ifndef Button_Switch_h__
#define Button_Switch_h__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CocosGUI.h"
#include "CocoStudio.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::ui;

class ButtonSwitch : public ControlSwitch
{
public:
	
	static ButtonSwitch* create(
		const char* bgImgRes = "Image/Icon/global/img_duihuan_6.png",
		const char* onImgRes = "Image/Icon/global/img_duihuan_3.png", 
		const char* offImgRes = "Image/Icon/global/img_duihuan_5.png", 
		const char* thumbImgRes = "Image/Icon/global/btn_kaiguan_2.png"
		);

private:

	void initSprite(Sprite* onSprite, Sprite* offSprite, Sprite* thumbSprite, Sprite* bgSprite, Label* onLabel, Label* offLabel);
};



#endif