
#ifndef GroupBossRank_h__
#define GroupBossRank_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "cocos-ext.h"
#include "Utils/Observer.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include "GroupBossController.h"
#include "GroupBossDataDef.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

#define  MAX_BOSS_NUM 3

typedef struct tagUI_BossTop 
{
	Layout*		pRoot;							//根节点

	//左侧滚动层
	ui::ScrollView*	mScrollLeftPanel;
	Button*		mBtnRankTop[MAX_BOSS_NUM];

	//图片ui变量
	ImageView*  Img_Rank;
	ImageView*	Img_Name;
	ImageView*  Img_Gang;
	ImageView*  Img_Value;

	//两个滚动提示箭头
	ImageView*  mImgArrowUp;
	ImageView*  mImgArrowDown;
	//我的帮派排名
	Layout*		Panel_GangRank;
	Text*		GangRankRanking;
	Text*		GangRankKilled;
	Text*		GangRankdamage;
	//我的全服排名
	Layout*		Panel_Personal;
	ImageView*	IMG_MyRank;
	Text*		PersonalRanking;
	Text*		Personaldamage;

	//滚动列表容器
	Layout*		mPnlList;
	//列表子项模板
	Layout*		mTemplateItem;
}UI_BossTop;

class GroupBossRank : public Layer,public Observer ,public extension::TableViewDelegate, public extension::TableViewDataSource
{
public:
	GroupBossRank();
	~GroupBossRank(void);

	static GroupBossRank* create(eRankType type);

	virtual void updateSelf(void* data);

	virtual bool  init(eRankType type);

	virtual void onEnter();

	virtual void onExit(); 

	void update(float dt);

	void  onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );

	void  onBtnRankItem(Ref* ref, Widget::TouchEventType type, int nWidgetName);

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
	void  switchRankTopPanel(eRankType rankType = eRankType::eTYPE_GROUP);

	//设置公共UI
	void  setCommonUI();

	//设置滚动层cell内容
	void  setCellContent(extension::TableViewCell* cell, ssize_t idx);

	//协议回包后刷新UI
	void updateUI();


private:

	UI_BossTop mUI;

	//当前选中的排行面板类型
	eRankType mCurSeledPanelType;

	extension::TableView*	mRankTopView;
};

#endif // GroupBossRank_h__
