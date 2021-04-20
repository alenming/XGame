#ifndef LayerMail_h__
#define LayerMail_h__


#include "CocosGUI.h"

#include "MailController.h"
#include "../UI/ModalLayer.h"
#include "DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/LayerTool.h"

class EditBoxEx;
class LayerLoading;

const int MAIL_REAWRD_COUNT = 4;
const int MAIL_ITEM_COUNT = 5;

const int GOLD_ITEM_ID = 11001; //金币
const int COIN_ITEM_ID = 11002; //银币
const int PVP_ITEM_ID = 11003; //竞技场积分
const int SHILIAN_ITEM_ID = 11004; //试炼积分
const int GROUP_ITEM_ID = 11005; //帮派积分
const int HUNTIAN_ITEM_ID = 11006; //浑天丹
const int JIUZHUAN_ITEM_ID = 11007; //九转丹
const int VIPEXP_ITEM_ID = 11008; //VIP经验
const int CUILIAN_ITEM_ID = 11009; //淬炼碎片
const int EXP_ITEM_ID = 11010; //主角经验

class LayerMail: public ModalLayer, public Observer
{

public:
	LayerMail();
	~LayerMail();

	virtual void updateSelf(void* data);

	CREATE_FUNC(LayerMail);

	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

public:	
	
protected:
	void	initUI();
	void	initUIMail();

	//一键删除
	void	onBtnDeleteNow(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//一键领取奖励
	void	onBtnGetRewardNow(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//删除
	void	onBtnDelete(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//详情返回按钮
	void	onBtnReturn(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//领取奖励
	void	onBtnGetReward(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	
	void	onBtnCheckMail(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int id);
private:
	void updateMail();
	void setItemByIdx(int idx);

	void updateCurItem();
	void updateAllItem();

	void delCurItem(int idx);

	void openMail(const int& id,bool action = false);
	void onScrollViewEvent(Ref* pSender, cocos2d::ui::ScrollView::EventType type);
	void setScroll();

	void onGetOnePrize(); //获取单个邮件附件
	void onGetAllPrize(); //获取所有邮件附件

	//show奖励
	void showPopReward();
private:
	// UI控件集合
	UI_Mail_Mail		m_ui;
	// 
	cocos2d::ui::Button	*m_pBtnBack;
	CustomScrollBar *m_customScrollBarHero;

	int				m_nCurrMailId; //当前选择邮件
};

#endif // 
