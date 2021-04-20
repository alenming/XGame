#ifndef LayerTool_h__
#define LayerTool_h__

#include "cocos2d.h"
#include "CocosGUI.h"

#include "ToolController.h"
#include "Utils/Observer.h"
#include "DataManager/DataManager.h"
#include "UI/ModalLayer.h"
#include "UIToolHeader.h"
#include "ToolDef.h"

class EditBoxEx;
class LayerLoading;
class CustomScrollBar;

enum class TOOL_TOP_BTN_TAG
{
	POWER,
	ENERGY,
	GOLD,
	SILVER
};

struct UI_DETAIL_EQUIP
{
	cocos2d::ui::ImageView* pImgTitle;
	cocos2d::ui::ImageView* pImgDetail;
	cocos2d::Vec2       vecPosImgTitle;
	cocos2d::Vec2       vecPosImgDetail;
	UIToolHeader		*pHeader;
	cocos2d::ui::Text	*pTextName;
	cocos2d::ui::Text	*pTextNum;
	//基础属性
	cocos2d::ui::Text	*pTextAtrri1;
	cocos2d::ui::Text	*pTextAtrri2;
};

struct UI_DETAIL_CONSUME
{
	cocos2d::ui::Layout	*pLayerDetail;
	cocos2d::ui::ImageView* pImgTitle;
	cocos2d::ui::ImageView* pImgDetail;
	cocos2d::Vec2       vecPosImgTitle;
	cocos2d::Vec2       vecPosImgDetail;
	UIToolHeader		*pHeader;
	cocos2d::ui::Text	*pTextName;
	cocos2d::ui::Text	*pTextNum;
	cocos2d::ui::ImageView* pImgType;
	cocos2d::ui::Text	*pTextDetail;

};

struct  UI_TOOL_SELL
{
	// UI根节点层容器
	UIToolHeader			*pHeader;
	cocos2d::ui::Text		*pTextName;
	cocos2d::ui::Text		*pTextNum;

	cocos2d::ui::Button		*pBtnConfirm;
	cocos2d::ui::Button		*pBtnCancel;

	cocos2d::ui::Text		*pTextTotle;
	cocos2d::ui::Text		*pTextTotleNum;
};

struct  UI_TOOL_USE
{
	// UI根节点层容器
	UIToolHeader			*pHeader;
	cocos2d::ui::Text		*pTextName;
	cocos2d::ui::Text		*pTextNum;

	cocos2d::ui::Button		*pBtnConfirm;
	cocos2d::ui::Button		*pBtnCancel;
};

struct UI_TOOL
{
	// UI根节点层容器
	cocos2d::ui::Layout	*pRoot;
	cocos2d::ui::Layout	*pLayerBeibao;
	cocos2d::ui::Layout	*pLayerDetailConsume;
	cocos2d::ui::Layout	*pLayerDetailEquip;
	cocos2d::ui::Layout	*pLayerPopSell;
	cocos2d::ui::Layout	*pLayerPopUse;
	//
	ImageView			*pImgBeibao;
	//
	CustomScrollBar*		listHeroBar;
	// 全部按钮
	cocos2d::ui::CheckBox	*pBtnAll;
	// 消耗按钮
	cocos2d::ui::CheckBox	*pBtnConsume;
	// 装备按钮
	cocos2d::ui::CheckBox	*pBtnEquip;
	// 碎片按钮
	cocos2d::ui::CheckBox	*pBtnChip;

	//滚动面板
	cocos2d::ui::ScrollView *pScrollView;
	cocos2d::ui::Slider *pSlider;

	Layout* pBtnPnl;
	cocos2d::ui::Button	*pBtnSell;
	cocos2d::ui::Button *pBtnUse;
	cocos2d::ui::Button *pBtnJump;
	Point pointPosSell;
	Point pointPosUse;

	ImageView			*pImgPrice;
	cocos2d::ui::Text	*pTextCost;
	cocos2d::ui::Text	*pTextCostNum;
	

	UI_DETAIL_EQUIP		uiEquip;
	UI_DETAIL_CONSUME	uiConsume;
	UI_TOOL_SELL		uiSell;
	UI_TOOL_USE			uiUse;
};


class LayerTool: public ModalLayer, public Observer
{

public:

	LayerTool();
	~LayerTool();

	CREATE_FUNC(LayerTool);

public:

	virtual bool init();
	virtual void onExit();
	virtual void onEnter();
	
	virtual void onRecvUseToolData(int amount);
	virtual void onRecvSellToolsData(int amount);

	//数据更新通知接口
	virtual void updateSelf(void* data);

public:

	void ClearToolData();
	void ShowToolData(TOOL_TYPE type, bool isFlushDetails = true);
	void ShowToolData(const int& id, const TOOL_INFO* pToolInfo, TOOL_TYPE type, int& index, bool isFirst=false);
	
	void setSingleToolIcon(UIToolHeader* pUIToolIcon, const int& index, const int& id);
	UIToolHeader* addToolIcon(const int& id, const TOOL_INFO* pToolInfo, const int& count=1, const int& disIndex=0, bool isFirst=false);
	UIToolHeader* getToolIcon(const int& id, const int& disIndex=0);
	void SetToolIconAttribute(const int& id, UIToolHeader* pUIToolIcon, const TOOL_INFO* pToolInfo, const int& count, bool isDetail=false);	

	void ShowDetail(const int& toolId, const int& count, bool isShowAnimi=true);

	//宝箱开启展示
	void showBoxInfo();

private:

	void initUI();
	void initUIDetail();

	void onScrollViewEvent(Ref* sender, cocos2d::ui::ScrollviewEventType type);
	void onBtnBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	// 全部按钮
	void	onBtnAll(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	// 碎片按钮
	void	onBtnChip(Ref* sender, cocos2d::ui::Widget::TouchEventType type);	
	// 消耗按钮
	void	onBtnConsume(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	// 装备按钮
	void	onBtnEquip(Ref* sender, cocos2d::ui::Widget::TouchEventType type);	


	//道具详情	
	void	onBtnToolIcon(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int id);
// 	//使用
// 	void	onBtnToolItemUse(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	// 装备详情按钮
	void	onBtnEquipDetail(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//出售
	void	onBtnToolDetailSell(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//使用
	void	onBtnToolDetailUse(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//出售弹出框
	void	onBtnSellClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnSellConfrim(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnSellMax(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnSellMin(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	//使用弹出框
	void	onBtnUseClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnUseConfrim(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnUseMax(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnUseMin(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	//顶部按钮点击响应
	void onTopBtnClick(Ref* sender, cocos2d::ui::Widget::TouchEventType type, TOOL_TOP_BTN_TAG tag);

	//动画显示详情页
	void outShowDetail(Node* pTitle, cocos2d::Vec2 vecPosTitle, Node* pDetail, cocos2d::Vec2 vecPosDetail, bool isOut);
	void onMoveTitleCallFunN(cocos2d::Node* pNode, void* pPram);

private:

	// UI控件集合
	UI_TOOL m_ui;

	ToolController mToolController;

	//当前选中的道具
	int	m_CurrentToolId;
	UIToolHeader *m_pCurrentTool;
	
	//购买
	int	m_nsellPrice;
	int	m_nsellNum;
	int	m_nsellMaxNum;

	//使用
	int mUseNum;

	int	m_nDetailDisCount;
	bool isShowDetailOut;
	
	//道具类型
	TOOL_TYPE m_toolType;
	bool m_bResetSellSliderPos;
	bool m_bResetUseSliderPos;

	//当前选中组的道具数量
	int m_curGroupToolNum;
};

#endif //
