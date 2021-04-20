#ifndef LayerAddMat_h__
#define LayerAddMat_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "cocos-ext.h"
#include "UI/ModalLayer.h"
#include "FormationModule/CustomScrollBar.h"
#include "TacticsDataDef.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::ui;
using namespace std;


//UI结构
typedef struct tagUI_AddMaterial 
{
	//根节点
	Layout*		pRoot;							

	Layout* pnlSelExpProp;							//选择道具视图
	Text*   txtCurExp;								//当前经验
	Text*   txtNextLvExp;							//下一级需要经验
	Button* btnSure;								//确认按钮
	Button* btnClose;								//关闭

	Layout* tbViewPnl;								//滚动列表

	//材料模板
	Button*		btnMaterial;

	tagUI_AddMaterial() 
	{
		memset(this, 0, sizeof(tagUI_AddMaterial));
	}

}UI_AddMaterial;

//记录在tableview上被勾选的道具，复用cell时会全部刷新
typedef struct seledProp
{
	int idx;		//idx索引
	int side;		//左道具=0，右道具=1

	seledProp()
		:idx(-1)
		,side(-1) {}
}sSeledProp;

class LayerAddMat : public ModalLayer, public TableViewDataSource, public TableViewDelegate
{
public:
	LayerAddMat();
	~LayerAddMat();

public:
	static LayerAddMat* create(vector<int>* vecSeledMat, eRowType curRowType);
	bool init(vector<int>* vecSeledMat, eRowType curRowType);

	virtual void onEnter();
	virtual void onExit();

	//实现基类函数
	virtual void tableCellTouched(extension::TableView* table, extension::TableViewCell* cell);
	virtual Size cellSizeForTable(extension::TableView *table);   
	virtual extension::TableViewCell* tableCellAtIndex(extension::TableView *table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(extension::TableView *table);

	virtual void scrollViewDidScroll(extension::ScrollView* view);

private:
	//初始化各种UI
	void  initUI(eRowType curRowType);

	//计算道具数量
	void  initPropNum();

	//按钮回调
	void  onBtnClick( Ref* ref, Widget::TouchEventType type);

	void setCellContent(extension::TableViewCell* cell, ssize_t idx);

	//重写Button触摸事件
	void onBtnTouchEvent(Button *btn);

	//选择经验道具
	void onBtnSelExpProp(Button* btn, int nIdx);

	//计算被选中的道具
	void initSeledProp();
private:

	UI_AddMaterial m_ui;

	//tbview
	TableView* m_tbViewExpProp;

	//滚动条
	CustomScrollBar* m_scrollBar;

	//背包中排升级各材料数据
	vector<sRowUpMaterial> m_vRowUpMaterial;

	//当前道具总数量
	int m_iPropTotalNum;

	//已选中的材料
	vector<int>* m_pSelectedMat;

	//标记滑动道具时是否选择
	bool m_bIsSelect;

	//当前选中的道具数量
	int m_iCurSelectedNum;

	vector<sSeledProp> m_vSeledProp;
};


#endif