#ifndef GoldActivityDef_h__
#define GoldActivityDef_h__
#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/SoundUtils.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

//面板控件集合
typedef struct tagUI_GoldActivity
{
	Layout* pRoot;
	Layout *Pnl_GoldActivity;
	ImageView *Img_Show;
	ImageView *Img_di;

	ImageView *Img_Best;
	ImageView *Img_Residue;
	ImageView *Img_Cost;
	Text* Lab_Best; //最多
	Text* Lab_Residue;//剩餘
	Text* Lab_Cost;//花費
	Text* Lab_TimeA;//天數
	Text* Lab_TimeB;//小時
	Button* Btn_Play;//
	Text* Lab_Vtips;//vip提示
	Text* Lab_TipsNotime;

	Layout* Pnl_Num;
	ImageView  *Img_Tipsdi;
	Text* Lab_TipsChance;

	ListView* ListView_news;

	tagUI_GoldActivity()
	{
		memset(this, 0, sizeof(tagUI_GoldActivity));
	}
}Ui_GoldActivity;

enum GoldActivityNotify_Type
{
	GoldActivityNotify_Type_Info, //拉取消息
	GoldActivityNotify_Type_ChangeGold, //兌換金幣
	GoldActivityNotify_Type_OtherChangeGold, //他人信息推送
	GoldActivityNotify_Type_Timer, //時間
	GoldActivityNotify_Type_Tips, //主界面红点提示
};

//x天、xx时xx分xx秒
typedef struct GoldActivityFormatTime
{
	string strDays;
	string strHMS;		//时分秒
};

struct GoldActivityTable
{
	vector<int> vipVec; //vip等级对应次数
	vector<int> costVec; //花费
	vector<int> showVec; //最多
	string tips; //提示
	string info; //别的玩家
};

struct OtherPlayerChange
{
	vector<string> ChangeInfo;
};

struct GOLD_ACTIVITY_PRIZE_ITEM //奖励节点
{
	int id;
	int amount;
};

const int PlayerChangeInfoNum = 4;
const int GOLD_ACTIVITY_GOLD_ITEM_ID = 11001; //金币
#endif