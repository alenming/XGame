#ifndef NewAreaActivityDataDef_h__
#define NewAreaActivityDataDef_h__

#include <iostream>
#include <vector>
using namespace std;

//活动状态
enum actState
{
	STATE_OPEN,
	STATE_CLOSE
};

//特价礼包信息，给记录付费点调用
typedef struct TeJiaBox
{
	bool isBox;		//发送领取的奖励是否是特价礼包
	int days;		//第几天
	int price;

	TeJiaBox()
	{
		isBox = false;
		days = 0;
		price = 0;
	}
}sTeJiaBox;

//物品/任务ID，数量(好吧，看数据表吧，我都不知道怎么做注释了)
typedef struct amount
{
	int id;
	int done;			//完成的数量
	int cnt;			//目标数量
}sAmount;

//道具id、数量
typedef struct propInfo
{
	int propId;
	int propCnt;
}sPropInfo;

typedef struct NewAreaActivityData
{
	int actId;

	//活动标签
	string actName;

	//活动录属标签
	int actShow;

	//活动开启所在日期
	int actOpenTime;

	//任务类型
	int taskType;

	//这两个数量(好吧，看数据表吧，我都不知道怎么做注释了)
	sAmount amount1;
	int amount2;
	//剩余可购买数量
	int amount3;

	//原价、特价
	int oldPrice;
	int newPrice;

	//活动描述
	string actDes;

	//道具信息
	vector<sPropInfo> vecPropInfo;

	//跳转界面类型
	int skipType;

	//是否可领取
	bool isCanGet;
	//是否已领取
	bool isGetted;

}sNewAreaActivityData;

typedef enum sceneSkip
{
	DEFAULT,
	CHAPTER_SCENE,
	HERO_SCENE,
	PVP_SCENE,
	LOVE_SCENE,
	REFINE_SCENE,
	ROB_SCENE,
	FRIEND_SCENE,
	MAZE_SCENE,
	SHOP_SCENE,
	TACTICS_SCENE,
	STAR_SU_SCENE,
	PRAY_SCENE,
	BOSS_SCENE,
	STAR_CAMP_SCENE,
	VIP_SCENE,
	ARTIFACT_SCENE
}eSceneSkip;

#endif