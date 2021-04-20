#ifndef SignDef_h__
#define SignDef_h__
#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/SoundUtils.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

struct SignRewardInfo
{

	//道具ID
	int mItemId;

	//道具数量
	int mItemCount;
	SignRewardInfo()
	{
		memset(this,0,sizeof(SignRewardInfo));
	}
};

struct SignRewardStateInfo
{
	//日
	int day;

	//月
	int month;

	SignRewardStateInfo()
	{
		memset(this,0,sizeof(SignRewardStateInfo));
	}
};

enum SIGN_EVENT_TYPE
{
	SIGN_EVENT_INIT,//初始化
	SIGN_EVENT_RECEIVE			//领取
};

enum SIGN_STAT_TYPE
{
	SIGN_STAT_TYPE_CAN = 1, //1可签
	SIGN_STAT_TYPE_DID,     //2已签
	SIGN_STAT_TYPE_BU,      //3可补签
	SIGN_STAT_TYPE_VIP,      //4 vip达到可领状态
	SIGN_STAT_TYPE_CANOT      //5 不可领状态
};

//面板用到的控件结构体
typedef struct tagUi_SignHelp
{
	Layout* pRoot;
	Button* pBtnClose;

	tagUi_SignHelp()
	{
		memset(this,0,sizeof(tagUi_SignHelp));
	}
}Ui_SignHelp;

//弹出获得的奖励
typedef struct _ReWard_Show
{
	vector<SignRewardInfo> mVecRewardInfo;
	int mGold;
	int mCoin;

	_ReWard_Show():
	mGold(0),
	mCoin(0)
	{

	}
}ReWard_Show;


#endif