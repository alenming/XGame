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
	//��������
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
	// UI���ڵ������
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
	// UI���ڵ������
	UIToolHeader			*pHeader;
	cocos2d::ui::Text		*pTextName;
	cocos2d::ui::Text		*pTextNum;

	cocos2d::ui::Button		*pBtnConfirm;
	cocos2d::ui::Button		*pBtnCancel;
};

struct UI_TOOL
{
	// UI���ڵ������
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
	// ȫ����ť
	cocos2d::ui::CheckBox	*pBtnAll;
	// ���İ�ť
	cocos2d::ui::CheckBox	*pBtnConsume;
	// װ����ť
	cocos2d::ui::CheckBox	*pBtnEquip;
	// ��Ƭ��ť
	cocos2d::ui::CheckBox	*pBtnChip;

	//�������
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

	//���ݸ���֪ͨ�ӿ�
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

	//���俪��չʾ
	void showBoxInfo();

private:

	void initUI();
	void initUIDetail();

	void onScrollViewEvent(Ref* sender, cocos2d::ui::ScrollviewEventType type);
	void onBtnBack(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	// ȫ����ť
	void	onBtnAll(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	// ��Ƭ��ť
	void	onBtnChip(Ref* sender, cocos2d::ui::Widget::TouchEventType type);	
	// ���İ�ť
	void	onBtnConsume(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	// װ����ť
	void	onBtnEquip(Ref* sender, cocos2d::ui::Widget::TouchEventType type);	


	//��������	
	void	onBtnToolIcon(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int id);
// 	//ʹ��
// 	void	onBtnToolItemUse(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	// װ�����鰴ť
	void	onBtnEquipDetail(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//����
	void	onBtnToolDetailSell(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//ʹ��
	void	onBtnToolDetailUse(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	//���۵�����
	void	onBtnSellClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnSellConfrim(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnSellMax(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnSellMin(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	//ʹ�õ�����
	void	onBtnUseClose(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnUseConfrim(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnUseMax(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnUseMin(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	//������ť�����Ӧ
	void onTopBtnClick(Ref* sender, cocos2d::ui::Widget::TouchEventType type, TOOL_TOP_BTN_TAG tag);

	//������ʾ����ҳ
	void outShowDetail(Node* pTitle, cocos2d::Vec2 vecPosTitle, Node* pDetail, cocos2d::Vec2 vecPosDetail, bool isOut);
	void onMoveTitleCallFunN(cocos2d::Node* pNode, void* pPram);

private:

	// UI�ؼ�����
	UI_TOOL m_ui;

	ToolController mToolController;

	//��ǰѡ�еĵ���
	int	m_CurrentToolId;
	UIToolHeader *m_pCurrentTool;
	
	//����
	int	m_nsellPrice;
	int	m_nsellNum;
	int	m_nsellMaxNum;

	//ʹ��
	int mUseNum;

	int	m_nDetailDisCount;
	bool isShowDetailOut;
	
	//��������
	TOOL_TYPE m_toolType;
	bool m_bResetSellSliderPos;
	bool m_bResetUseSliderPos;

	//��ǰѡ����ĵ�������
	int m_curGroupToolNum;
};

#endif //
