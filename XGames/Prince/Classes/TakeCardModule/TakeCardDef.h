#ifndef TakeCardDef_h__
#define TakeCardDef_h__

#include <string>
#include <vector>

#include "cocos2d.h"
#include "CocosGUI.h"
#include "../ToolModule/ToolDef.h"
#include "../ToolModule/UIToolHeader.h"
#include "Communication/Socket/Util.h"
#include "Widget/LayerCommHeadInfo.h"
#include "Widget/LayerGetCard.h"
#include "Widget/ActionCreator.h"
#include "Utils/SoundUtils.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const int TAKE_CARD_TOOL_ID = 10004;
class CustomScrollBar;

struct TAKE_CARD_ITEM_INFO
{
	int onecost;  //一次花费
	int tencost;  //十次花费
	int times;    //显示多少次必抽
	int limitCount; //每日可抽最大次数
	int CurCount;  //当前抽卡次数
	int firstFlag; //是否首抽
	bool tenFlag;  //是否十连抽
};

typedef struct tag_TAKE_CARD_INFO
{
	TAKE_CARD_ITEM_INFO coinItem;
	TAKE_CARD_ITEM_INFO goldItem;
}TAKE_CARD_INFO;

typedef struct tag_New_Card_INFO
{
	int id;
	int amount;
	int countChips;
}NEW_CARD_INFO;

enum TakeCardType
{
	TakeCardTypeNone,
	TakeCardTypeCoin,
	TakeCardTypeGold
};

enum TakeCardType_What
{
	TakeCardType_Spend,
	TakeCardType_Free,
	TakeCardType_Card
};

enum TakeCardState
{
	TakeCardState_zhu,
	TakeCardState_show
};

enum TakeCardNotify
{
	TakeCardNotify_GetInfo, //获取信息
	TakeCardNotify_GetCard, //抽卡
	TakeCardNotify_Timer, //通知抽卡更新倒计时时间
	TakeCardNotify_BuildIng, //通知主界面显示抽卡提示小红点
};
//UI
struct  UI_TakeCard
{
	Layout		*pLayTakeCard;  //抽卡容器（显示天罡地煞）
	
	//////////////////地煞//////////////////////////////
	ImageView	*pImgTakeCard1;
	Button      *DiBtnPreview;
	//正面
	Layout		*pLayTakeCardFront1;
	Layout		*pLayTakeCardFrontCountdown1; //倒计时容器
	ImageView   *pDiShaZhengImgDaoJiShi; 
	Text		*pDiShaZhengTextTime1; //倒计时时间
	ImageView   *pDiShaZhengImgDelCount;
	Text        *pDiShaZhengDelCount; //免费剩余次数
	ImageView   *pDiShaZhengImgCostBg;
	ImageView   *pImgDiShaZhengYinbi; //地煞正面银币
	ImageView   *pImgDiShaZhengOrderCard; //地煞正面抽卡令
	Text        *pDiShaZhengCommomCount; //地煞正面通用

	//反面
	Layout		*pLayTakeCardOther1;
	Button		*pBtnTakeCardOne1; //抽一次
	Button		*pBtnTakeCardTen1;  //抽十次

	ImageView	*pTextTakeCardIntro1Panel; //显示再次购买次数面板
	ImageView	*pTextTakeCardIntro1; //显示再次购买次数
	ImageView	*pTextTakeCardIntro1Bichu; //显示再次购买次数必出

	ImageView   *DiImgCostYiciBg; //一次
	ImageView   *DiImgCostYiciYinbi; //一次银币
	ImageView   *DiImgCostYiciOrderCard; //一次抽卡令
	Text        *DiImgCostYiciCommomCount; //通用个数

	ImageView   *DiImgCostShiciBg;
	ImageView   *DiImgCostShiciYinbi; //一次银币
	ImageView   *DiImgCostShiciOrderCard; //一次抽卡令
	Text        *DiImgCostShiciCommomCount; //通用个数

	////////////////////天罡///////////////////////////
	ImageView	*pImgTakeCard2;
	Button      *TianBtnPreview;
	//正面
	Layout		*pLayTakeCardFront2;
	Layout		*pLayTakeCardFrontCountdown2;//倒计时容器
	ImageView   *pTianGangZhengImgDaoJiShi; 
	Text		*pTianGangZhengTextTime1; //倒计时时间
	ImageView   *pTianGangZhengImgDelCount;
	Text        *pTianGangZhengDelCount; //免费剩余次数
	ImageView   *pTianGangZhengImgCostBg;
	ImageView   *pTianGangZhengGold; //正面金币
	ImageView   *pTianGangZhengOrderCard; //正面抽卡令
	Text        *pTianGangZhengCommomCount; //通用
	ImageView   *firstGetCard; //首抽13资质

	//反面
	Layout		*pLayTakeCardOther2;
	Button		*pBtnTakeCardOne2; //抽一次
	Button		*pBtnTakeCardTen2; //抽十次

	ImageView	*pTextTakeCardIntro2Panel; //显示再次购买次数面板
	ImageView	*pTextTakeCardIntro2; //显示再次购买次数
	ImageView	*pTextTakeCardIntro2Bichu; //显示再次购买次数必出

	ImageView   *TianImgCostYiciBg; //一次
	ImageView   *TianImgCostYiciGold; //一次金币
	ImageView   *TianImgCostYiciOrderCard; //一次抽卡令
	Text        *TianImgCostYiciCommomCount; //通用个数

	ImageView   *TianImgCostShiciBg;
	ImageView   *TianImgCostShiciGold; //一次银币
	ImageView   *TianImgCostShiciOrderCard; //一次抽卡令
	Text        *TianImgCostShiciCommomCount; //通用个数

	//抽卡结果
	Layout		*pLayResult;

	Layout     *pCoinPanel;   //银币面板
	Text       *pCoinCardOrder1;
	Text       *pCoinCardOrder10;
	ImageView  *pCoinYibi1;
	ImageView  *pCoinYibi10;
	Text       *pYinbiyici;
	Text       *pYinbishici;

	Layout     *pGoldPanel;   //金币面板
	Text       *pGoldCardOrder1;
	Text       *pGoldCardOrder10;
	ImageView  *pGoldYibi1;
	ImageView  *pGoldYibi10;
	Text       *pJinbiyici;
	Text       *pJinbishici;
	ImageView  *imgNowFree;

	Layout     *pBuyView;  //预览抽多少次
	ImageView  *pImgNeedCount; //还需多少次
	ImageView  *pImgTipsChip1; //碎片提示
	ImageView  *pImgTipsHero1; //英雄提示
	ImageView  *Image_xiao;
	ImageView  *Image_zhong;


	Layout     *pBuyBichu;  //必出
	Text       *pLabtips2; 
	ImageView  *pImgTipsChip2; //碎片提示
	ImageView  *pImgTipsHero2; //英雄提示

	Layout     *PnlChouka; //抽卡显示界面
	Button		*pBtnAgainOne; //抽一次
	Button		*pBtnAgainTen; //抽十次

	ImageView	*pLayHeroCardItemOne; //单张
	ImageView	*pLayHeroCardItem[10]; //十张
};


struct  UI_Root
{
	// UI根节点层容器
	cocos2d::ui::Layout		*pRoot;  //根节点
	UI_TakeCard		layerTakeCard;  //抽卡界面
};

struct Card_View_Node
{
	int id;
	int sortIdx;
};

const int COIN_ONE_SPEND_ID = 10118;
const int COIN_TEN_SPEND_ID = 10119;
const int GOLD_ONE_SPEND_ID = 10116;
const int GOLD_TEN_SPEND_ID = 10117;

#endif // RefineDef_h__
