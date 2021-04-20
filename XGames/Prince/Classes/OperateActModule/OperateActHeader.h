#ifndef OperateActHeader_h__
#define OperateActHeader_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "Communication/ICommListener.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Socket/Util.h"
#include "ToolModule/ToolModel.h"
#include "HeroModule/HeroModel.h"
#include "MainModule/MainModel.h"
#include "DataManager/DataManager.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;


const int OPERATE_RANK_PLAYER_COUNT = 3;

//OperateAct主界面控件
struct  UI_OPERATE_ACT_ENTER
{
	// UI根节点层容器
	Layout		*pRoot;

	ImageView   *Img_diList;
	Vec2        Img_diListPos;
	ListView    *OpListView;  //活动节点列表
	ImageView *ImgUp;
	ImageView *ImgDown;

	//#1 七日礼包
	Layout  *PnlSeven;
	Vec2    PnlSevenPos;
	Text    *SevenCount;
	ListView *SVSevenList;

	//#2 月卡
	Layout  *PnlCard;
	Vec2    PnlCardPos;
	Text *LabCardTime;
	Button *BtnCardBuy1;
	Button *BtnCardBuy2;
	ImageView* ImgState_1;
	ImageView* ImgState_2;

	//#3 战力排行
	Layout *PnlPowerRank;
	Vec2    PnlPowerRankPos;
	Text   *rankTimeLab;
	Button*   rankBtn;
	Text*     rankMyRank;
	Text*     rankMyPower;
	Text*     rankTips;
	ListView*  rankListView;

	//#4 最强伙伴
	Layout*  PnlShowSuperFriend;
	Vec2    PnlShowSuperFriendPos;
	ImageView* imgSuperFriend;
	Button*  pBtnCall;

	//#5 理财通
	Layout*  PnlGrown;
	Vec2    PnlGrownPos;
	Button*  grownButBuy;
	cocos2d::ui::ScrollView* grownSVGrown;
	ImageView* growState;

	//#6 全民福利
	Layout* PnlBenefit;
	Vec2    PnlBenefitPos;
	Text* bebefitCount;
	ListView* benefitSVBenefit;

	//#7 8 9 10通用
	Layout* PnlCommon;
	Vec2    PnlCommonPos;
	Layout* PnlTbView;
	Layout* PnlCommonItem;
	ImageView *imgTile;
	ImageView *Img_TimeA;
	Text      *labTime1A;
	ImageView *imgTimeB;
	Text      *labTimeB;
	Text      *CommonTips;

	//#12 通用双倍掉落
	Layout* Pnl_Double;
	Vec2    PnlDoublePos;
	ImageView* doubleImgTips;
	Text*  doubleTexTips;
	Text*  doubleTexTime;
	Button* doubleBtnGo;

	//#13限时抢购
	Layout* Pnl_Purchase;
	Vec2 PnlPurchasePos;
	ImageView* purTitle;
	ImageView* purImgTitle;
	Text*      purTime;
	Text*      PurTips;
	ImageView* pruImgItem[3];

	//#14 超值礼包
	Layout* PnlVipGift;
	Vec2    PnlVipGiftPos;
	ImageView* imgVipGiftTitle;
	ImageView* imgVipGiftBigTitle;
	Text*   vipGiftTips;
	Text*   vipGiftTips2;
	Text*   vipGiftLabTime;
	Text*   vipGiftLabPrice;
	Text*   vipGiftLabGiftName;
	Text*   vipGiftLabVipPrice;
	Text*   vipGiftLabVip;
	Button* vipGiftBuy;
	ImageView* vipGiftHaveBuy;
	ImageView* vipGiftMyVip;
	ListView* ListView_VIP;
	CheckBox* VipCheck;
	Layout* vipHead;
};

enum OPRATEACT_EVENT_TYPE
{
	OPRATEACT_EVENT_TYPE_GETTABLE,//获取数据包
	OPRATEACT_EVENT_TYPE_INFO, //领取信息
	OPRATEACT_EVENT_TYPE_GET_PRIZE,//获得奖励
	OPRATEACT_EVENT_TYPE_FINACIAL,//购买理财通
	OPRATEACT_EVENT_TYPE_VIPGIFT//购买vip礼包
};

struct OPERATEA_PRIZE_ITEM //奖励节点
{
	int id;
	int amount;
};

typedef vector<OPERATEA_PRIZE_ITEM> VEV_OPERATEA_PRIZE_ITEM;

struct OPERATE_ACT_INFO_ITEM
{
	int id;
	string tips;
	int actTyp;
	int refTyp;
	int amount1;
	int amount2;
	string price;
	string actInfo;
	VEV_OPERATEA_PRIZE_ITEM reward;
};

struct OPERATE_ACT_SORT_ITEM
{
	int id; //活动id
	string tips;
	int type; //3- 最强战力4- 每日充值 5- 理柴通	6- 全民福利	7- 通用模版A（每日5点重置）8- 通用模版B 9- 双倍活动10-限时抢购（每日5点重置）11-VIP礼包	12-节日兑换
	int tagIdx; //排序
	int timeType;//1开服时间 2创角时间 3限时时间
	int serverTime;//服务器开始时间（条件）
	int roleTime;//角色创建（条件）
	int openTime;//开启时间
	int endTime;//持续时间
	int sign;//双倍
	int res;//资源
	int disappear;//结束消失
	string tips2;//说明

	int daojiTime;
	bool isInit;

	vector<OPERATE_ACT_INFO_ITEM> info;
};

typedef vector<OPERATE_ACT_SORT_ITEM> VEV_OPERATE_ACT_SORT_ITEM;

enum OPERATE_ACT_TYPE
{
	OPERATE_ACT_TYPE_NONE,
	OPERATE_ACT_TYPE_SEVEN_DAY, //七日礼包
	OPERATE_ACT_TYPE_MONTH_CARD, //超值月卡
	OPERATE_ACT_TYPE_FIGHT_RANK, //战力排行
	OPERATE_ACT_TYPE_SUPER_FRIEND,//强力伙伴  (B哥 胡博说的带上老王绝逼不改2016/4/27)
	OPERATE_ACT_TYPE_FINANCIAL, //理财通
	OPERATE_ACT_TYPE_NATIONAL_WELL_BEING,//全民福利
	OPERATE_ACT_TYPE_COMMOMA,//7- 通用模版A（每日5点重置）
	OPERATE_ACT_TYPE_COMMOMA_,//8- 通用模版A_
	OPERATE_ACT_TYPE_COMMOMB,//9- 通用模版B（每日5点重置）
	OPERATE_ACT_TYPE_COMMOMB_,//10- 通用模版B_
	OPERATE_ACT_TYPE_SINGLE_RECHARGE,//11单笔充值
	OPERATE_ACT_TYPE_DOUBLE_FALL,//12通用双倍掉落
	OPERATE_ACT_TYPE_LIMIT_BUY,//13限时抢购
	OPERATE_ACT_TYPE_VIPBAG, //14超值礼包
};

enum OPERATE_TIME_TYPE
{
	OPERATE_TIME_TYPE_SEVER = 1, //开服时间
	OPERATE_TIME_TYPE_CREATE, //角色创建时间
	OPERATE_TIME_TYPE_LIMIT, //限时时间
};

struct COMMONA  //7 8
{
	vector<int>  actTyp;
	int id;
};
typedef vector<COMMONA> VEC_COMMONA;
struct COMMONB  //9 10
{
	vector<int>  actTyp;
	int id;
	int condition;
};
typedef vector<COMMONB> VEC_COMMONB;

struct  SINGLE_RECHARGE
{
	vector<int>  actTyp;
	vector<int> condition;
};

struct OPERATE_PLAYER_RANK
{
	int fight;
	string name;
	int vip;
	int res;
};

const int FINACIAL_VIP_LEVEL = 10123; //理财通购买需要的vip等级
const int FINACIAL_GOLD_COUNT = 10124; //理财通购买需要的金币
const int MONTH_CARD_DAYS = 10125; //购买月卡增加天数

const int OPRETTE_GOLD_ITEM_ID = 11001; //金币
const int OPRETTE_COIN_ITEM_ID = 11002; //银币
const int OPRETTE_PVP_ITEM_ID = 11003; //竞技场积分
const int OPRETTE_SHILIAN_ITEM_ID = 11004; //试炼积分
const int OPRETTE_GROUP_ITEM_ID = 11005; //帮派积分
const int OPRETTE_HUNTIAN_ITEM_ID = 11006; //浑天丹
const int OPRETTE_JIUZHUAN_ITEM_ID = 11007; //九转丹
const int OPRETTE_VIPEXP_ITEM_ID = 11008; //VIP经验
const int OPRETTE_CUILIAN_ITEM_ID = 11009; //淬炼碎片
const int OPRETTE_EXP_ITEM_ID = 11010; //主角经验

#endif