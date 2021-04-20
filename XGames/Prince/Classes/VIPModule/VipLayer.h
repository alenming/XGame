
#ifndef VipLayer_h__
#define VipLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include "UI/ModalLayer.h"
#include "Utils/Observer.h"
#include "VipController.h"
#include "VipDataDef.h"
#include "FormationModule/CustomScrollBar.h"
#include "Widget/SimpleRichText.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

typedef struct tagUI_GoldBtn
{
	//金幣充值選擇
	Button*		btnGold;
	ImageView*  iconBg;
	TextAtlas*  atlasPrice;
	ImageView*  backTipsBg;
	Text*		txtBackTips;					//立返300金币
	Text*		txtTips;						//每日领120金币×30天
	ImageView*  cornerIcon;						//右上角標
	ImageView*	cornerDouble;					//双倍
	Text*		txtDays;						//天数
	Layout*		pnlGold;
	Text*		txtGoldNum;						//648

	

	tagUI_GoldBtn()
	{
		memset(this, 0, sizeof(tagUI_GoldBtn));
	}
}UI_GoldBtn;

typedef struct tagUI_Vip 
{
	Layout*		pRoot;							//根节点

	//标题栏
	Button*     btnPay;							//充值按钮
	Text*       txtVipExpNum;					//vip进度数值
	Text*       txtVipNextLv;					//充值到下一等级vip提示
	ImageView*	imgVipLv;						//vip等级
	ImageView*  imgVipNextLv;					//下一等级vip
	LoadingBar* barVipExp;						//vip进度
	//Text*		txtTips;

	//特权栏
	Layout*     pnlPrivilege;					//特权界面节点
	ImageView*  imgPrivilegeLv;					//特权等级
	ImageView*  imgPrivilegeShow;				//特权信息父节点, 存放一个滚动进度条
	ui::ScrollView* scrollPrivilegeShow;			//特权信息显示区
	ui::ScrollView* scrollVipLvBtnShow;				//vip等级按钮滚动栏
	ImageView*  imgPrivilegeGift;				//特权礼包展示区
	ImageView*  imgBought;						//已购买
	Button*		btnBuyGift;						//购买按钮
	Text*		txtOldPrice;					//原价
	Text*       txtPrice;						//特价
	//Text*		txtVipDes;						//vip特权描述
	SimpleRichText*  txtVipDes;		

	//充值栏
	ui::ScrollView*	scrollPay;						//滚动层
	Layout*		pnlPay;							//充值界面节点
	Button*		btnPrivilege;					//特权按钮
	ImageView*	imgVipLvShow;					//更换充值/特权按钮图片

	//支付金币按钮选项
	vector<Layout*> vecGoldBtn;			
}UI_Vip;

class VipLayer : public Layer, public Observer
{
public:
	VipLayer();
	~VipLayer(void);

	//CREATE_FUNC(VipLayer);
	static VipLayer* create(Panel_Type type, int vipLevel);

	virtual bool  init(Panel_Type type, int vipLevel);

	virtual void onEnter();
	virtual void onExit(); 

	void update(float dt);

	virtual void updateSelf(void* data);

	void  onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );

	//初始化vip等级滚动层按钮
	void initScrollVipLvBtn();
	//更新vip等级按钮状态
	void updateVipLvBtnStates(int btnTag);

	//充值/特权图片切换
	void  switchPayAndPrivilege(bool isPay);

	//初始化标题栏界面
	void initTitleView();

	//初始化特权描述详情界面
	void initScrollPrivilegeDesView();

	//初始化购买礼包界面
	void initBuyGiftView();

	//购买后显示礼包奖励
	void showVipReward();

	//滚动层事件监听
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//初始化充值界面
	void initBuyGoldView();

	//刷新vip等级和进度数据
	void updateCurVipTitle();

	//刷新充值选项按钮上的数据状态
	void updatePayBtnUIState();

	//刷新月卡剩余天数
	void reflushMonthCardTime();

	//临时添加一个支付接口，可能要用统一的支付
	void payMoney(int btnTag ,OrderInfo orderInfo);

	//测试
	void testPay(int goodsId);

private:

	void  initUI();

	void  updateLvShow();

public:

	//当前购买的vip等级
	static int	m_curBuyVipLv;

	//显示的面板类型
	Panel_Type m_pnlType;

	//显示的vip等级面板
	int m_pnlShowVipLv;

	//是否直接进入充值面板
	//bool m_bIsEnterPayView;

private:

	UI_Vip		m_ui;

	//vip等级按钮向量
	vector<Button*> m_vecVipLvBtn;

	//充值购买金币按钮向量
	vector<Button*> m_vecBuyGoldBtn;

	//当前角色vip信息
	sVipData m_sCurVipInfo;

	//当前角色vip等级
	int		m_curVipLv;

	//当前角色vip进度
	int		m_curVipExp;

	//当前角色vip下一级最大进度值
	int		m_nextVipMaxExp;

	//角色金币数据
	int     m_nGoldCount;

	//当前选中的vip等级
	int			m_curSelVipLv;

	//当前选择的商品id
	int    m_curProductId;

	enum UI_Vip_Widget
	{
		Btn_SwitchPrivilegeAndPay,			//切换充值和特权界面
		Btn_BuyGift,
		Btn_VipLv,
		Btn_BuyGold
	};

	//滚动条
	CustomScrollBar* m_ScrollBar;

	//vip充值额度选项滚动条
	CustomScrollBar* m_payOptionBar;

	//是否是购买礼包后传过来的消息
	bool m_bIsBuyGiftMsg;

	//购买礼包获得的奖励列表
	sVipSpecBox m_vipRewardInfo;
};

#endif // VipLayer_h__
