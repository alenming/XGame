#ifndef MainInfoLayer_h__
#define MainInfoLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "Utils/Observer.h"
#include "MazeModule/LayerMazeLimitBuyItem.h"
#include "MazeModule/LayerMazeLimitBuy.h"
#include "FriendModule/FriendDef.h"
#include "SignModule/LayerSignController.h"
#include "MailModule/MailModel.h"
#include "LimitTimeHeroModule/LimitTimeHeroController.h"

USING_NS_CC;
using namespace ui;

typedef struct tagUI_MAIN_INFO
{
	//根节点
	Layout* root;
	//左上角根结点
	ImageView* roleImg;
	Point roleImgPos;
	//等级
	TextAtlas* levelAtl;
	//vip等级
	Button* vipBtn;
	//名字
	Text* nameText;
	//主角头像
	Button* playerInfoBtn;
	//战力
	Text* fightingText;
	//经验条
	ProgressTimer* expBar;

	//当前体力/最大体力
	Text* curPowText;
	//当前精力/最大精力
	Text* curEnergyText;
	//银币数量
	Text* silverText;
	//金币数量
	Text* goldText;

	//右侧按钮
	ImageView* leftBackImg;
	Point leftBackImgPos;
	Button* formationBtn;
	Button* packBtn;
	Button* heroBtn;
	Button* taskBtn;
	Button* rankTopBtn;
	Button* dequeBtn; //队伍
	ImageView *jiantou;
	Vec2     jiantouPos;
	Layout* dequeDi; //队伍底
	Vec2     dequeDiPos;
	Button* zhenFaBtn; //阵法
	Button* shenQiBtn; //神器
	Button* starPanleBtn; //星盘




	//上方按钮
	Button* signBtn;		//签到
	Button* mailBtn;		//邮件
	Button* depositBtn;		//福利大厅
	Button* openBtn;		//七天开服奖励
	Button* firstRechargeBtn;	//首充
	Button* chongzhi;
	Button* limitTimeHeroBtn;	//限时神将
	Button* tianjiangjinbi; //天降金币
	Button* steryShop;		//神秘商店
	Text*	steryShopTiem;

	//其他按钮
	Button* friendBtn;
	Button* setBtn;
	Button* chatBtn;

	//Button* addGoldBtn;

	//左下角功能开放提示按钮
	ImageView* funcOpenImg;
	Button* funcOpenBtn;
	Text*	openTipsText;
	Layout* tipsDetailsPnl;

}UIMainInfo;

//等级开放表数据
typedef struct openFuncData
{
	int id;
	int level;
	string iconName;
	string resPath;
	string strDes;

	openFuncData()
		: id(0), level(0), iconName(""), resPath(""), strDes("") {}
}sOpenFuncData;

enum class MAIN_BTN_TAG
{
	FORMATION,		//阵形
	PACK,			//背包
	HERO,			//英雄
	STARSU,			//星宿
	TASK,			//任务
	DEQUE,          //队伍
	ZHENFA,          //阵法
	SHENQI,          //神器
	STARTPAN,        //星盘

	BOSS,			//世界Boss
	REGISTER,		//签到
	VIP,			//充值
	OPERATE,        //运营活动
	FIRSTRECHARGE,	//首充
	LIMITTIMEHERO,	//限时神将
	TIANJIANGJINBI,	//天降金币
	STERYSHOP,		//神秘商店

	MAIL,			//邮件
	FRIEND,			//好友
	SETGAME,        //设置
	CHAT,           //聊天  

	ROB,			//掠夺
	MAZE,			//幻境
	PLAYERINFO,		//玩家信息
	OPEN,			//七天开服奖励
	RANKTOP,		//排行榜
	ADDGOLD,		//加金币
	ADDSILVER,		//加银币
	ADDPOWER,		//加体力
	ADDENERGY		//加精力
};

//限时活动类型，主界面左下角做展示用
enum class ACTIVE_TYPE
{
	ACTIVE_DEFAULT,	//没有活动时显示下一级开放的功能图标
	ACTIVE_BOSS,	//boss活动
};


class MainInfoLayer : public Layer, public Observer
{

public:

	MainInfoLayer();
	virtual ~MainInfoLayer();

	CREATE_FUNC(MainInfoLayer);

public:

	virtual bool init() override;

	virtual void onEnter() override;

	virtual void onExit() override;

	virtual void updateSelf(void* data) override;

	//设置属性
	void setUserInfo();

	//设置角色可进阶标志
	void setHeroAdvFlag();

	//设置任务可领取标记
	void setTaskFlag();

	//设置签到标记
	void setSignFlag();

	//设置签到标记
	void setMailFlag();

	//设置天降金币标记
	void setGoldActiviryFlag();

	//设置限时神将小红点
	void setLimitTimeHeroFlag();

	//设置聊天标记
	void setChatFlag();

	//设置好友标记
	void setFriendFlag(bool first);

	//设置开服七天奖励活动标记
	void setActivityFlag();

	//设置运营活动标记
	void setOperateFlag();

	//设置星盘新开启标记
	void setNewStartCampFlage();

	//设置vip图标小红点
	void setVipFlag();

	//按钮响应
	virtual void onBtnClick(Ref* ref, cocos2d::ui::Widget::TouchEventType type, MAIN_BTN_TAG btnFlag);

	//跳转至指定场景
	void jumpToScene(MAIN_BTN_TAG btnFlag);

	//增加聊天滚动条
	void addChatMsgItem();

	//设置下一级要开放的功能和限时活动状态
	void setFuncState();
	void setNextFuncOpen();
	void showActiveAni(bool isActive);
	void setActiveState(ACTIVE_TYPE type);
	void showFuncOpenTips();

	void allActivityButton(); //设置所有活动按钮
	void setActivityButtonVisible( Button * sender, bool visible); //设置所有活动按钮是否显示
	void ShowActivityButton(); //所有活动按钮显示

	Sprite* getBlurScreenShot();
private:

	UIMainInfo mUI;

	bool isFirst;// 是否第一次拉取好友请求列表

	bool isDuiWuOpen; //收起按钮是否打开
	bool isDuiWuCanRun; //收起按钮运动是否完成

	//记录的功能开放数据
	sOpenFuncData m_sData;

	LimitTimeHeroController m_pController;

	vector<Vec2> m_allActivityPos; //所有活动按钮初始位置
	struct SHOWE_ACT_NODE
	{
		Button* node;
		bool visible;
	};
	vector<SHOWE_ACT_NODE> m_allActivityShowBtn; //所有活动按钮初始位置
};


#endif //MainInfoLayer