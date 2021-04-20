
#ifndef GroupBossGetReward_h__
#define GroupBossGetReward_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "cocos-ext.h"
#include "Utils/Observer.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include "GroupBossController.h"
#include "GroupBossDataDef.h"

#define  MAX_GET_NUM 2

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;



typedef struct tagUI_BossGet 
{
	Layout*		pRoot;							//根节点

	//左侧滚动层
	ui::ScrollView*	mScrollLeftPanel;
	Button*		mBtnRankTop[MAX_GET_NUM];

	//两个滚动提示箭头
	ImageView*  mImgArrowUp;
	ImageView*  mImgArrowDown;

	//滚动列表容器
	Layout*		mPnlList;
	//列表子项模板
	Layout*		mTemplateItem;
	Layout*		Panel_Reward;
	Layout*		Panel_Exchange;

	ImageView* redFlag;

}UI_BossGet;

class GroupBossGetReward : public Layer,public Observer, public extension::TableViewDelegate, public extension::TableViewDataSource
{
public:
	GroupBossGetReward();
	~GroupBossGetReward();

	static GroupBossGetReward* create(eGetType rankType);

	virtual void updateSelf(void* data);

	virtual bool  init(eGetType rankType);

	virtual void onEnter();

	virtual void onExit(); 

	void update(float dt);

	void  onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );

	void  onBtnGetItem(Ref* ref, Widget::TouchEventType type, int boxId);

	void  onBtnGetExchange(Ref* ref, Widget::TouchEventType type,int changeCount,  int changeId);
	//左侧滑动区域
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//实现基类函数
	virtual void tableCellTouched(extension::TableView* table, extension::TableViewCell* cell);
	virtual Size cellSizeForTable(extension::TableView *table);   
	virtual extension::TableViewCell* tableCellAtIndex(extension::TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(extension::TableView *table);

private:

	void  initUI();
	void  initRankTopUI();
	void  switchRankTopPanel(eGetType rankType = eGetType::eTYPE_HURT);
	void setRedFrag();
	//设置滚动层cell内容
	void  setCellContent(extension::TableViewCell* cell, ssize_t idx);

	//奖励弹窗展示
	void showReward(eGetType type);

	//格式化伤害数字
	string formatDamage(time_t damageVal);


private:
	#define GroupBossInstance	GroupBossModel::getInstance()

	UI_BossGet mUI;

	eGetType mCurSeledPanelType;

	extension::TableView*	mRankTopView;
};

#endif // GroupBossGetReward_h__
