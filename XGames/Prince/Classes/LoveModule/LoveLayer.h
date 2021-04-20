/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LoveLayer
 *  Description:	情缘层模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/23/2015
 *  Others:			
 *
 ******************************************************************/
#include "LoveHeader.h"
#include "LoveController.h"
#include "LoveModel.h"
#include "DataManager/DataTable.h"
#include "LoveGoodWillPrize.h"
#include "Temp/PopReward.h"
#include "Widget/LayerCommHeadInfo.h"
#include "LoveTreasureScene.h"
#include "LoveProAdd.h"

#ifndef LoveLayer_h_
#define  LoveLayer_h_

const int NPC_TALK_COUNT = 10;
const float enterMain_time = 0.4f;
const float exitMain_time = 0.2f;
const float enterHero_time = 0.4f;
const float exitHero_time = 0.2f;
const float enterGift_time = 0.4f;
const float exitGift_time = 0.2f;
const float enterDrink_time = 0.4f;
const float exitDrink_time = 0.2f;
const float enterPlay_time = 0.4f;
const float exitPlay_time = 0.2f;

const float WIN_IMAGE_TIME = 0.3f;

const int AttributeInfoCount = 5;
const int DifficultCount = 5;

const int FUNCTION_LOVE_DRINK = 10088;				//情缘喝酒
const int FUNCTION_LOVE_PLAY = 10089;			//情缘游玩

class LoveLayer:public Layer,public Observer
{
public:
	~LoveLayer();

	static LoveLayer* create(bool pushScene);
	 
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

protected:
	LoveLayer();

	bool init(bool pushScene);
	virtual void onEnter() override;
	virtual void onExit() override;
	void updateNextFram(float ft);


	void initUi();  //初始化主界面
	void initVipUI_1(); //猜拳次数不足提示vip购买
	void initVipUI_2();
	string  getVipIcon(int nVipLv);
	int  getNextValidVip(int& nNextValue);


	void switchState(); //切换状态
	void initHero();
	void initHeroAttribute(); //初始化英雄情缘属性信息
	void initGiftList(); //初始化送礼界面
	void initDrink(); //初始化猜拳
	void intiPlay(bool move = true); //初始化游玩

	void setHeroUpdtaInfo();//设置英雄数据改变 界面相应变化
	void setHerofavour(); //设置英雄好感信息
	void setAddlove(); //设置增加的好感
	void setUpGradePrize();//好感升级奖励
	void showPopReward();//额外奖励
	void setFingerGuseeInfo(FINGER_ENTER_TYPE_RESULT result); //显示结果
	void setFingerGuseePanelInfo(); //更新猜拳界面信息
	void setFingerFailWinTips(FINGER_ENTER_TYPE_RESULT result);
	void setFingerAddlove(); //设置增加的好感

	//按钮事件
	void	onBtnNotify(Ref* sender, Widget::TouchEventType type); //按钮
	void	onBtnVisitFriend(Ref* sender, Widget::TouchEventType type,int petId); //拜访
	void	onGiftBuyBtn(Ref* sender, Widget::TouchEventType type,LOVE_GIFT_INFO *info,int idx); //购买准备
	void	onGiftSend(Ref* sender, Widget::TouchEventType type,int petId,LOVE_GIFT_INFO *giftInfo,int idx); //赠送
	void	onChoosePlayInfo(Ref* sender, Widget::TouchEventType type,int petId,int idx); //选择游玩地图信息
	void	onViewGuessPrize(Ref* sender, Widget::TouchEventType type,int idx); //查看猜拳奖励

	void    onCallGiftSend(float dt);
	void    onAddGiftSendCount(float dt);
	void    onGiftBuy(int amount);//购买
	void    onFingerEnter(FINGER_ENTER_TYPE type);
	void    onChangeMingEnter();


	void    addHeroList();  //添加英雄界面节点
	void    addGiftList();  //添加送礼节点
	int     getGiftToHeroGoodWill(int gitfId); //每种礼物对应英雄好感不一样   
	void    updataGiftListItemById(); //更新单个送礼列表

	//物品栏消息相应
	void onScrollViewEvent( Ref* pSender, ScrollView::EventType type);

	//英雄随机说话
	void setHeroRandSay(const string str);//送礼对白
	void setHeroRandFingerSay(const string str); //猜拳对白
	//对话动态显示
	void showNpcTalk();
	void animationTalkOut(string strText, bool isL2R=true);
	void callBackTalkOut(float dt);

	//主界面切换
	void enterMain();
	void exitMain();

	//英雄界面切换
	void enterHero(bool isFromMain = false);
	void exitHero();

	//送礼界面切换
	void enterGift();
	void exitGift();

	//喝酒界面切换
	void enterDrink();
	void exitDrink();

	//游玩界面切换
	void enterPlay();
	void exitPlay();
private:
	UI_LOVE_ENTER m_ui;

	LOVE_LAYER_STATE m_state;

	CustomScrollBar *m_customScrollBarHero;
	CustomScrollBar *m_customScrollBarGift;

	//对白
	std::string		m_strTalkContent;
	int				m_nTalkContentLength;
	int             m_npcTalkCount;
	int             m_unLockCount;

	int             m_chooseHeroPetId; //主界面选择的英雄PetId

	int             m_giftListItemIdx; //需要更新的礼物索引
	int             m_giftPetId; //英雄id
	LOVE_GIFT_INFO  *m_giftInfo; //礼物信息
	
	int             m_giftSendCount; //礼物赠送次数
	bool            m_giftLongPress; //是否长按

	int             m_giftTemplateId; //购买礼物id
	int             m_giftBuyCount; //购买个数

	FINGER_ENTER_TYPE m_MyfingetGuseeType;//自己出拳

	int m_GooddGiftAdd;
	int m_OrdinaryGiftAdd;
	int m_BadGiftAdd;

	bool  m_canGuess;
	bool  m_canVisit;
	bool  m_canBack;
	bool  m_isFirstAddHero;
	bool  m_canBuyWin;

	int   m_playMapIdx; //游玩地图索引

	bool  m_isChangeMing;
	bool   m_isPushScene;

};




#endif // !LoveLayer_h_
