
#ifndef RankTopLayer_h__
#define RankTopLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "cocos-ext.h"
#include "Utils/Observer.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include "RankTopController.h"
#include "RankTopDef.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

typedef struct tagUI_RankTop 
{
	Layout*		pRoot;							//根节点

	//左侧滚动层
	ui::ScrollView*	mScrollLeftPanel;
	Button*		mBtnRankTop[MAX_RANK_NUM];

	//图片ui变量
	ImageView*  mImgTitleName;
	ImageView*	mImgTitleGroupName;
	ImageView*  mImgTitleValue;
	ImageView*  mImgBottomValue;
	ImageView*	mImgUpdateTips;
	ImageView*  mImgBottomRank;

	//两个滚动提示箭头
	ImageView*  mImgArrowUp;
	ImageView*  mImgArrowDown;

	//文字ui变量
	Text*		mTxtMyRank;
	Text*		mTxtMyValue;

	//滚动列表容器
	Layout*		mPnlList;
	//列表子项模板
	Layout*		mTemplateItem;
}UI_RankTop;

class RankTopLayer : public Layer, public Observer, public extension::TableViewDelegate, public extension::TableViewDataSource
{
public:
	RankTopLayer();
	~RankTopLayer(void);

	static RankTopLayer* create(bool isPush);

	virtual bool  init(bool isPush);

	virtual void onEnter();
	virtual void onExit(); 

	void update(float dt);

	virtual void updateSelf(void* data);

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
	void  switchRankTopPanel(rankTopType rankType = LEVEL_RANK_TOP);

	//设置公共UI
	void  setCommonUI();

	//设置滚动层cell内容
	void  setCellContent(extension::TableViewCell* cell, ssize_t idx);
	

private:

	//是否pushScene进来
	bool m_bIsPush;

	UI_RankTop mUI;
	
	RankTopController mRankTopController;

	//当前选中的排行面板类型
	rankTopType mCurSeledPanelType;

	extension::TableView*	mRankTopView;
};

#endif // RankTopLayer_h__
