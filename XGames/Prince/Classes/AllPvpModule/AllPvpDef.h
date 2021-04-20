#ifndef  AllPvpDef_h__
#define  AllPvpDef_h__
#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/SoundUtils.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

//面板控件集合
typedef struct tagUI_AllPvp
{
	Layout* pRoot;
	ListView* listView;
	Button* item;
	
	tagUI_AllPvp()
	{
		memset(this, 0, sizeof(tagUI_AllPvp));
	}
};
#endif