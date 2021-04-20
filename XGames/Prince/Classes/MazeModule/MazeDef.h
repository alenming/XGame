#ifndef MazeDef_h__
#define MazeDef_h__

#include <string>
#include <vector>
#include "cocos2d.h"
#include "CocosGUI.h"
#include "Communication/Json/json.h"

#include "ToolModule/ToolDef.h"
#include "CommonBuy/LayerCommonBuy.h"
#include "Widget/ActionCreator.h"

using namespace std;
using namespace cocos2d;
using namespace cocos2d::ui;


//迷宫事件
enum MAZE_STEP_STATE
{
	MAZE_STEP_ENTER,//进入迷宫
	MAZE_STEP_NONE,	
	MAZE_STEP_TALK,
	MAZE_STEP_MEET,	
	MAZE_STEP_BOSS,
	MAZE_STEP_LIMIT_BUY,
	MAZE_STEP_MAP,
};
//随机事件类型
enum  MAZE_RANDOMROAD_TYPE
{
	MAZE_RANDOMROAD_NONE,
	MAZE_RANDOMROAD_MEET,	     //1-遭遇
	MAZE_RANDOMROAD_TALK,        //2-对话
	MAZE_RANDOMROAD_BOSS,	     //3-Boss
	MAZE_RANDOMROAD_LIMIT_BUY,	 //4-限时购买
	MAZE_RANDOMROAD_MAP	         //5-宝藏
};

//横幅事件
enum HengFu_Event
{
	HENGFU_NONE,      // 
	HENGFU_QIANGDI,   // 横幅强敌
	HENGFU_BOSS,      // 横幅boss
	HENGFU_BAOZANG    // 横幅宝藏
};

typedef struct MazeMapItem
{
	int id;
	int num;	
}MAZE_MAP_ITEM;

typedef struct ChoosedItem
{
	int id;
	int count;
	int pos;

}CHOOSEED_ITEM;

typedef struct ChoosedItemReward
{
	int openCount;
	int drawItemId;
	int drawItemCount;
	int pos;

}CHOOSEED_ITEM_REWARD;

typedef struct MazeInfoParam
{
	MazeInfoParam()
	{
		mazeId = 0;
		curPower= 0;;
		times= 0;

		mMazeBuyCurTime = 0;
		mMazeBuyTime = 0;
		mMazedrawPetId = 0;
		mMazeBuyPrice = 0;
		mMazeBuyCount = 0;
		openBoxId = 0;
	}
	int mazeId;
	int curPower;
	int times;

	long mMazeBuyCurTime;
	int mMazeBuyTime;
	int mMazedrawPetId;
	int mMazeBuyPrice;
	int mMazeBuyCount;
	int openBoxId;

	vector<MAZE_MAP_ITEM>vecMaps;
	vector<CHOOSEED_ITEM>vecChoosedMap;
}MAZE_INFOS;


//掉落信息
typedef struct MazeDropInfo
{
	int getExp;
	int getCoin;
	vector<TOOL_NEW_INFO> vecNewTools;
}MAZE_DROP_INFO;

typedef struct MazePassInfo
{
	int mazeStar;
	int groCoe;
	int getExp;
	int getCoin;
	int getScore;
}MAZE_PASS_INFO;

//UI
struct  UI_Maze_Limit_Icon
{
	Layout		*pRoot;
	Layout		*pLayHero;
	ImageView* mImgBg;
	Text* mTextTime;
};

struct  UI_Maze_Limit
{
	Layout		*pRoot;

	Button		*pBtnBuy;
	Button		*pBtnWait;
	Text* mTextTime;
	//ImageView *Item;
	
	Vec2		 mvecHeroPanel;
	Text*  mBuyText;
};

struct  UI_Maze
{
	// UI根节点层容器
	Layout		*pRoot;

	//主界面
	Layout		*pLayMain;

	Layout		*pLayGo;  //触发事件
	ImageView  *pImgMainBackup; //抖动画面

	//对话事件
	Layout		*pLayMainHero;
	ImageView	*pImgMainHero;

	//对白
	Layout		*pLayMainTalk;
	ImageView	*pImgTalkNameBg;
	Text		*pTextTalkName;
	Text		*pTextTalkContent;
	Button		*pBtnTalkNext;
	vector<ImageView*> imgAotuOpen;

	Vec2			vecBtnTalkNext;

	//偶遇事件
	Layout		*meet;
	Button		*meetChallenge;
	Button		*meetRound;

	//Boss事件
	Layout		*hengfu;
	ImageView		*henfuBoss;
	ImageView		*henfuQiangDi;
	ImageView		*henfuBaozang;

	//藏宝事件
	Layout		*pLayMainMapChip;
	ImageView  *pImgMapTips;
	ListView *pListMapchip;

	//宝箱
	Button		*pBtnBoxMain;
	Layout		*pLayBox;


	//宝藏
	Layout   *pBaozan;
	Layout   *freeCard;
	Text     *freeCardNum;
	Layout   *goldCard;
	Text     *goldCardNum;
	Layout   *cardPanel;
	vector<ImageView*> cardVecBack;
	vector<ImageView*> cardVecFront;

	Text     *cardUp;
	Button   *cardBtnClose;

	//翻牌提示
	Layout  *layChoose;

	Layout   *layLimitBuy;
};

struct  UI_Maze_Pass
{
	// UI根节点层容器
	Layout		*pRoot;

	ImageView  *pImgBg;
	Text		*pTextStarNum;
	Text		*pTextAddRewards;

	Text		*pTextScore;
	Text		*pTextExp;
	ScrollView	*pScrvTools;
};

struct  MazeRewardInfo
{

	//道具ID
	int mItemId;

	//道具数量
	int mItemCount;
	MazeRewardInfo()
	{
		memset(this,0,sizeof(MazeRewardInfo));
	}
};

//弹出获得的奖励
typedef struct _Maze_ReWard_Show
{
	vector<MazeRewardInfo> mVecRewardInfo;
	vector<MazeRewardInfo> mVecRewardMap;//专门应对藏宝图

	int mGold;
	int mCoin;
	int mExp;

	_Maze_ReWard_Show():
		mGold(0),
		mCoin(0),
		mExp(0)
	{

	}
}Maze_ReWard_Show;


#endif // MazeDef_h__
