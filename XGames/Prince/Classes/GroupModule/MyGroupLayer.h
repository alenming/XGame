
#ifndef MyGroupLayer_h__
#define MyGroupLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "cocos-ext.h"
#include "Utils/Observer.h"
#include "FormationModule/CustomScrollBar.h"
#include "MyGroupModel.h"
#include "MyGroupController.h"
#include "Widget/SimpleRichText.h"
#include "Widget/EditBoxEx.h"
#include "GUI/CCEditBox/CCEditBox.h"


USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

//成员列表ui
typedef struct tag_UIMember
{
	Layout*	itemPnl;
	Layout* iconPnl;
	
	Text*	memName;
	Text*	memLv;
	Text*	memPostPower;
	Text*	memFightPower;
	Text*	memOnlineTime;

	ImageView*	vipLv;

	tag_UIMember()
	{
		memset(this, 0, sizeof(tag_UIMember));
	}

}sTag_UIMember;

//帮派动态ui
typedef struct tag_UIEvent
{
	Text*	declaration;
	Button*	modifyBtn;

	tag_UIEvent()
	{
		memset(this, 0, sizeof(tag_UIEvent));
	}

}sTag_UIEvent;

typedef struct tag_UIGroup
{
	Layout* pRoot;						//根节点

	//公共UI
	Layout*	groupIconPnl;

	Text*	groupLv;
	Text*	groupName;
	Text*	groupId;
	Text*	leaderName;
	Text*	memberNum;
	Text*	myPostPower;				//我的职位权限
	Text*	groupRank;					//帮派排名

	Button*	exitBtn;
	Button*	settingBtn;
	Button*	helperBtn;

	//页签按钮
	Button*		memberListTabBtn;
	Button*		dynEventTabBtn;
	Button*		buildingTabBtn;
	Button*		activityTabBtn;
	Button*		shopBtn;
	Button*		rankBtn;

	Layout*		memItemPnl;				//成员列表模板
	Layout*		memListPnl;				//成员列表父节点

	Layout*		buildiingItemPnl;		//建筑列表模板
	Layout*		actbuildiingItemPnl;		//活动建筑列表模板

	//公告弹窗
	Layout*		noticeLayerPnl;
	Text*		txtInNotice;			//对内公告
	Text*		txtOutNotice;			//对外公告
	//TextField*	txtInNotice;			//对内公告
	//TextField*	txtOutNotice;			//对外公告
	EditBoxEx*  editBoxInNotice;		//对内输入框
	EditBoxEx*  editBoxOutNotice;		//对外输入框
	Button*		btnInside;
	Button*		btnOutSide;
	Button*		btnSave;

	Layout*	memListViewPnl;
	Layout*	dynEventViewPnl;
	Layout*	buildingViewPnl;
	Layout*	actbuildingViewPnl;
	//ImageView*  activityViewImg;

	//修改头像ui
	Layout*		headIconLayer;
	ui::ScrollView*	headIconScroll;
	Button*		closeBtn;

	//动态事件
	ui::ListView* dynEventScroll;

	//帮派建筑
	ui::ScrollView* buildingScroll;
	//帮派活动建筑
	ui::ScrollView* actbuildingScroll;

	vector<Layout*> vecViewList;		//视图列表, 方便统一管理
	vector<Button*> vecTabsBtn;			//页签按钮, 方便统一管理
	vector<sTag_UIMember> vecMemberListUI;

	sTag_UIEvent	sEventUI;

}sTag_UIGroup;

//按钮标记
typedef enum UI_BtnFlag
{
	BTN_MODIFYHEAD,
	BTN_EXIT,
	BTN_SETTING,
	BTN_HELP,
	BTN_MEMBER_TAB,
	BTN_EVENT_TAB,
	BTN_BUILDING_TAB,
	BTN_ACTIVITY_TAB,
	BTN_SHOP_TAB,
	BTN_RANK_TAB,
	BTN_MODIFY,
	BTN_CLOSEFLAGLAYER,
	BTN_INSIDE_NOTICE,
	BTN_OUTSIDE_NOTICE,
	BTN_SAVE_NOTICE,
	BTN_CLOSE_NOTICE

}eUI_BtnFlag;


//页签
typedef enum Tabs
{
	eTABS_DEFAULT_TYPE,
	eTABS_MEMBERLIST_TYPE,				//成员列表
	eTABS_DYNAMICEVENT_TYPE,			//帮派动态
	eTABS_BUILDING_TYPE,				//帮派建筑
	eTABS_ACTIVITY_TYPE					//帮派活动
}eTabs;

typedef enum NoticeTabs
{
	eNOTICE_TABS_DEFAULT,
	eNOTICE_TABS_INSIDE,
	eNOTICE_TABS_OUTSIDE

}eNoticeTabs;

class MyGroupLayer : public Layer, public Observer, public extension::TableViewDelegate, public extension::TableViewDataSource, public extension::EditBoxDelegate
{
public:
	MyGroupLayer();
	~MyGroupLayer(void);

	CREATE_FUNC(MyGroupLayer);

	virtual bool  init();

	virtual void onEnter();
	virtual void onExit(); 

	void update(float dt);

	virtual void updateSelf(void* data);

	void  onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );

	void  onBtnClickByActId( Ref* reff, Widget::TouchEventType type, int actId);

	//滑动事件
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//实现基类函数
	virtual void tableCellTouched(extension::TableView* table, extension::TableViewCell* cell);
	virtual Size cellSizeForTable(extension::TableView *table);   
	virtual extension::TableViewCell* tableCellAtIndex(extension::TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(extension::TableView *table);

	//virtual void scrollViewDidScroll(ScrollView* view);

	void editBoxEditingDidBegin(extension::EditBox* editBox);
	void editBoxReturn(extension::EditBox* editBox);

private:

	void  initUI();

	//初始化公共UI
	void  initCommonUI();

	//初始化帮派旗帜弹窗
	void  initFlagListUI();

	//初始化成员列表UI
	void  initMemListUI();

	//初始化帮派动态UI
	void  initDynEventUI();

	//初始化帮派建筑UI
	void  initBuildingUI();

	//初始化帮派活动建筑UI
	void initAcitvityBuildingUI();

	//初始化公告弹窗UI
	void  initNoticeUI();

	//更新公共UI
	void  updateCommonUI();

	//更新头像
	void  updateHeadIcon();

	//更新事件列表
	void  updateEventListUI();

	//设置旗帜背景
	void  setFlagBg();

	//更新成员列表
	void  updateMemberListUI();

	//更新建筑列表
	void  updateBuildingUI();

	//调用建筑弹窗
	void  addBuildLayer();

	//调用管理设置弹窗
	void  addSettingLayer();

	//更新活动列表
	void  updateActivityUI();

	//更新公告弹窗
	void  updateNoticeUI(eNoticeTabs tabs);

	//设置滚动层cell内容
	void  setCellContent(extension::TableViewCell* cell, ssize_t idx);

	//切页签
	void  switchTabsType(eTabs tabType);

	//解散，退出，被逐出，被解散帮派
	void  doExitFromGroup(eExitGroupEvent event);

	//设置红点
	void setJoinInFlag( );
	
	//设置帮派活动红点
	void setActivityFlag();

private:

	sTag_UIGroup m_ui;

	//当前页签
	eTabs m_eCurTabs;

	//成员列表
	extension::TableView*	m_tMemberList;

	bool m_bIsInittedDynEvent;
	bool m_bIsInittedBuilding;
	bool m_bIsInittedActivity;

	//成员列表滚动条
	CustomScrollBar* m_memListScrollBar;

	//帮派旗帜滚动条
	CustomScrollBar* m_flagListScrollBar;

	//动态事件滚动条
	CustomScrollBar* m_eventListScrollBar;

	//建筑列表滚动条
	CustomScrollBar* m_buildListScrollBar;

	//活动列表滚动条
	CustomScrollBar* m_ActbuildListScrollBar;

	Button* xunlongIn;

	Button* groupBoss;
};

#endif // MyGroupLayer_h__
