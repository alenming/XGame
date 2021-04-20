#ifndef LoveHeader_h__
#define LoveHeader_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "Communication/ICommListener.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Socket/Util.h"
#include "HeroModule/HeroModel.h"
#include "MainModule/MainModel.h"
#include "Common/Common.h"
#include "FormationModule/CustomScrollBar.h"
#include "GuideCenter/GuideManager.h"
#include "ChapterModule/SceneChapterLoading.h"
#include "MainModule/MainCityScene.h"
#include "ToolModule/GeneralBuy.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const int CHOOSE_MAP_COUNT = 4;
const int DRINK_PRIZE_COUNT = 5;
const int FIREND_SHIP_ID = 10111;			//情缘友情点
const int LOVE_ATTRIBUTE_XI_SPEND = 10112;			//情缘友情点洗点花费

struct FINGER_GUESS_NODE
{
	ImageView  *imgWin;
	ImageView  *imgLose;
	Text       *valueCount;
	Text       *winIdx;

};
//情缘进入
struct  UI_LOVE_ENTER
{
	// UI根节点层容器
	Layout		*pRoot;

	// 通用按钮
	Button      *phelp;      //帮助

	Layout      *pPnlDuiDai;
	ImageView   *pImgDuiDai;      //对白
	Vec2        pImgDuiDaiPos;                    //对白位置
	ImageView   *pImgDuiDaiNpc;      //Npc对白
	ImageView   *pImgDuiDaiHero;      //Hero对白
	Text        *pTextNpcName;//掌柜名字
	Text        *pTextHeroName;//英雄名字
	Text        *pTextDuiBaiContext;//英雄名字
	Button      *BtnextDuibai;

	//主界面
	Layout		*pPnlMain;        //主界面
	ImageView   *pImgWaiter;      //老板娘
	Vec2        pImgWaiterPos;                   //(老板娘位置)
	Layout      *pPnlHeroItem;    //英雄列表面板
	Vec2        pPnlHeroItemPos;                 //(英雄列表面板位置)
	ScrollView  *pHeroList;       //英雄列表
	Text        *pHeroLockCount;  //英雄列表个数
	Text        *pHeroGuessCount; //英雄列表剩余猜拳个数
	Text        *pHeroPlayCount; //英雄列表剩余探宝个数

	//英雄界面
	Layout      *pPnlHero;       //英雄界面
	ImageView   *pImgHero;       //英雄
	Vec2        pImgHeroPos;                  //英雄位置
	Button      *pBtnGift;       //送礼
	Vec2        pBtnGiftPos;                   //送礼位置
	Button      *pBtnPlay;       //游玩
	Vec2        pBtnPlayPos;                   //游玩位置
	Button      *pBtnDrink;      //喝酒
	Vec2        pBtnDrinkPos;                  //喝酒位置
	Layout      *pPnlFavour;     //好感
	Layout      *pPnlHeroPro;    //英雄属性面板
	Vec2        pPnlHeroProPos;                //属性位置
	Text        *pPnlHeroATK;
	Text        *pPnlHeroProAtk; //物理攻击
	Text        *pPnlHeroProBloodUp; //血量上限
	Text        *pPnlHeroProPhysicsDef; //物理防御
	Text        *pPnlHeroProSpellDef; //法术防御
	Button      *pPnlHeroProAdd; //属性强化
	Text        *pPnlHeroProFriendShip; //友情点
	ImageView   *pPnlHeroProTips; //tips


	//游玩界面
	Layout      *pPnlplay;       //游玩界面
	ImageView   *pImgMapDi;      //选择地图
	Layout      *Panel_mapInfo;
	Vec2         imgMapDiPos;
	Button   *imgChooseMap[CHOOSE_MAP_COUNT]; //选择的地图

	//送礼界面
	Layout      *pPnlGift;       //送礼界面
	ImageView   *giftListView;       //礼物列表
	Vec2        giftListPos;                  //礼物列表位置
	ListView    *giftList;


	//猜谜界面
	Layout      *pPnlGuess;      //猜谜界面

	//喝酒界面
	Layout      *pPnlDrink;      //喝酒界面
	ImageView   *pPnlDrinkInfo;  
	ImageView   *pPnlDrinkInfoPrize;
	Layout      *pPnlDrinkInfoPrizePneal;
	Layout      *pnlTime;        //猜拳次数
	Text        *fingerCountDesc; //猜拳剩余次数
	Text        *fingerRCountDesc; //猜拳连胜次数
	ImageView   *DrinkHero;      //本英雄
	Vec2        DrinkHeroPos;                //本英雄位置
	ImageView   *DrinkMin;     //玩家英雄
	Vec2        DrinkMinPos;                //玩家英雄位置
	Layout      *PnlDrinkReward; //结算
	Button      *BtnDrinkReward[DRINK_PRIZE_COUNT];//奖励
	Text        *TextDrinkReward[DRINK_PRIZE_COUNT];//说明

	Layout      *pnlDrinkBegin;  //猜拳
	Button      *BtnfingerStone; //石头
	Button      *BtnfingerScissors; //剪刀
	Button      *BtnfingerCloth; //布 

	Layout      *pnlFinger;        //拳头
	ImageView   *fingerHero;    
	ImageView   *fingerMin;  
	Button      *addGuessBtn; //购买次数

	ImageView   *ImgResult;      //输赢
	Layout      *changeMing;      //逆天改命
	Button      *changeMingAgain;//再来
	Text        *changeMingCost;//花费
	Button      *changeMingWin;//改命赢


	// VIP状态;
	Layout*		pVipStateRoot;
	ImageView*  imgVipBg1;
	ImageView*	imgVipStateBg;
	ImageView*	imgCurVip;
	Text*		txtVipStateTips1;
	Text*		txtVipStateTips2;
	Text*		txtVipStateTips3;
	Text*		txtCost;
	Text*		txtCurCount;
	Button*		btnVipStateOK;
	Button*		btnVipStateCancel;
	Button*		btnVipStateClose;

	// VIP提升;
	Layout*		pVipRoot;
	ImageView*	imgVipBg;
	ImageView*	imgVipNext;
	Text*		txtVipTips1;
	Text*		txtVipTips2;
	Text*		txtNextCount;
	Button*		btnVipOK;
	Button*		btnVipCancel;
	Button*		btnVipClose;

};

enum  LOVE_LAYER_STATE //界面状态
{
	LOVE_LAYER_STATE_NONE,
	LOVE_LAYER_STATE_ENTER,
	LOVE_LAYER_STATE_HERO,
	LOVE_LAYER_STATE_PLAY,
	LOVE_LAYER_STATE_GIFT,
	LOVE_LAYER_STATE_GUESS,
	LOVE_LAYER_STATE_DRINK
};

enum LOVE_EVENT_TYPE //消息类型
{
	LOVE_EVENT_TYPE_NONE,
	LOVE_EVENT_TYPE_GET_INFO,
	LOVE_EVENT_TYPE_SEND_GIFT,
	LOVE_EVENT_TYPE_BUY_GIFT,
	LOVE_EVENT_TYPE_FINGER_GUESS,
	LOVE_EVENT_TYPE_PLAY_MAP,
	LOVE_EVENT_TYPE_PLAY_MAP_INFO,
	LOVE_EVENT_TYPE_PLAY_MAP_EVENT,
	LOVE_EVENT_TYPE_BUY_GUESS_INFO,
	LOVE_EVENT_TYPE_BUY_GUESS,
	LOVE_EVENT_TYPE_ADD_ATTRIBUTE,
	LOVE_EVENT_TYPE_RESET_ATTRIBUTE,
	LOVE_EVENT_TYPE_CHANGE_GUESS,
	LOVE_EVENT_TYPE_GET_GUESS_PRIZE,
};

enum HERO_GOOD_LV  //好感度类型
{
	HERO_GOOD_LV_STRANGE = 0, //陌生
	HERO_GOOD_LV_MEET,        //结识
	HERO_GOOD_LV_COMPANION,   //同伴
	HERO_GOOD_LV_FRIEND,      //好友
	HERO_GOOD_LV_CONFIDANT,   //知己
	HERO_GOOD_LV_BOSOM_FRIEND,//挚友
	HERO_GOOD_LV_SWORN,       //结拜
};

enum HERO_MOOD_LV  //心情
{
	HERO_MOOD_LV_DEPRESSED = 1,    //沮丧
	HERO_MOOD_LV_COMMON,       //一般
	HERO_MOOD_LV_HAPPY,        //开心
};

enum FINGER_ENTER_TYPE
{
	FINGER_ENTER_TYPE_STONE = 1,  //石头
	FINGER_ENTER_TYPE_SCISS,  //剪刀
	FINGER_ENTER_TYPE_CLOTH,  //布
};


enum FINGER_ENTER_TYPE_RESULT
{
	FINGER_ENTER_TYPE_RESULT_WIN = 1,  //赢
	FINGER_ENTER_TYPE_RESULT_FAILD,  //输
	FINGER_ENTER_TYPE_RESULT_PIN,  //平
};

enum PLAY_MAP_TYPE
{
	PLAY_MAP_TYPE_RESULT_IMCOMPLETE,  //未完成
	PLAY_MAP_TYPE_RESULT_COMPLETE,  //完成
	PLAY_MAP_TYPE_RESULT_FAIL,  //失败
};

       
enum PLAY_MAP_EVENT_TYPE
{
	PLAY_MAP_EVENT_INIT,       //0-初始格
	PLAY_MAP_EVENT_FIGHT,      //1-战斗
	PLAY_MAP_EVENT_BOX,        //2-宝箱
	PLAY_MAP_EVENT_RECOVERY,   //3-体力恢复
	PLAY_MAP_EVENT_TRAP,       //4-陷阱
	PLAY_MAP_EVENT_STONE,      //5-石头
	PLAY_MAP_EVENT_EMPTY,      //6-空格子
	PLAY_MAP_EVENT_TREASURE    //7-宝物
};

enum  PLAY_MAP_RESULT_TYPE
{
	PLAY_MAP_RESULT_NONE,
	PLAY_MAP_RESULT_SUCCESS,
	PLAY_MAP_RESULT_FAIL
};

struct LOVE_HERO_INFO
{
	int petId; //英雄ID(查表)
	int sortIdx; 

	int lastLevel;	//上次好感度等级	
	int curLevel;	//当前好感度等级					

	int goodWill;//好感度
	int friendShip; //友情点
	int expType; //好感度类型（）
	int lastExpType; //好感度类型（）

	int playCount;//每天游玩次数	

	bool isGuess; //是否猜拳过
	int guessCount; //猜拳次数，根据VIP等级不同
	int guessWinCount;//猜拳胜利的次数
	vector<int> guessReward; //胜利奖励

	int typeAtk; //攻击
	int typeHp; //血量
	int typePdd; // 物防
	int typeMdd; //法防 
};

struct LOVE_GIFT_INFO //礼物信息
{
	int templateId;	//id	
	int amount;     //个数
	int resCount;   //购买限制个数（-1 不限制）	
	int price; 	    //价格（金币）				
};

typedef vector<LOVE_HERO_INFO> LOVE_HERO_INFO_VEC;
typedef vector<LOVE_GIFT_INFO> LOVE_GIFT_INFO_VEC;

struct  SEND_GIFT_REWARD
{
	int templateId;	//id	
	int amount;     //个数
};
typedef vector<SEND_GIFT_REWARD> SEND_GIFT_REWARD_VEC;

struct  PLAY_MAP
{
	int id;	//id	
	int flg; //是否完成
};
typedef vector<PLAY_MAP> PLAY_MAP_VEC;

struct  PLAY_MAP_NODE_INFO
{
	PLAY_MAP_EVENT_TYPE _event; //事件
	int flg; //是否完成
};
typedef vector<PLAY_MAP_NODE_INFO> PLAY_MAP_NODE_INFO_VEC;

#endif