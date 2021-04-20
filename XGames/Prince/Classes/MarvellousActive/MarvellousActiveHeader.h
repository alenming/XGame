#ifndef MarvellousActiveHeader_h__
#define MarvellousActiveHeader_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "Communication/ICommListener.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Socket/Util.h"
#include "Common/Common.h"
#include "FormationModule/CustomScrollBar.h"
#include "GuideCenter/GuideManager.h"
#include "ChapterModule/SceneChapterLoading.h"
#include "Widget/LayerCommHeadInfo.h"
#include "DataManager/DataManager.h"
#include "MazeModule/SceneMaze.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;


//情缘进入
struct  UI_MA_ENTER
{
	// UI根节点层容器
	Layout		*pRoot;
	ListView    *pListViewMa;
	ImageView   *Img_Left;
	ImageView   *Img_Right;
};

struct MA_INFO
{
	int id;
	int time;
	int flg;
};
typedef vector<MA_INFO> MA_INFO_VEC;

enum MA_EVENT_TYPE //消息类型
{
	MA_EVENT_TYPE_NONE,
	MA_EVENT_TYPE_GET_INFO,
};

enum MA_EVENT
{
	MA_EVENT_NONE,
	MA_EVENT_LUEDUO, //掠夺
	MA_EVENT_EVERY_DAY_CHANGE, //每日挑战
	MA_EVENT_MIGONG, //迷宫
	MA_EVENT_BOSS //绝地反击
};
#endif