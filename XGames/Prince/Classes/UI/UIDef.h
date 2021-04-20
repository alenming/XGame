//
//  UIDef.h
//  tsDemo
//

#ifndef UIDef_h__
#define UIDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "Utils/StringFormat.h"
#include "Utils/SystemUtils.h"

#ifndef _STRING
    #define _STRING(val) #val
    #define STRING _STRING
#endif

//字体
#define FONT_FZZHENGHEI SystemUtils::getFontFileName()
#define FONT_ARLRDBD "fonts/ARLRDBD.ttf"
#define FONT_PHATGUY "fonts/PHATGUY.ttf"


#endif
