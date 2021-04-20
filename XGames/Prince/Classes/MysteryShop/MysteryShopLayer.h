#ifndef MysteryShopLayer_h_
#define MysteryShopLayer_h_

#include "NewShopModule/ShopDef.h"

#include "MysteryShopDef.h"
#include "MysteryShopController.h"
#include "MysteryShopModel.h"
#include "Widget/LayerCommHeadInfo.h"


struct UI_Mystery
{
	Layout		*m_Root;

	// 物品;
	ImageView		*Image_ScrollView;
	ui::ScrollView	*ScrollView_Content;
	CustomScrollBar	*scrollBar;

	// 刷新;
	Button		*Btn_Refresh;
	Text		*Lab_Cost;//按钮上的刷新价格

	Text		*Lab_Count_Desc;	//手动刷新次数 :
	Text		*Lab_Count;			//99
	Text		*Lab_Time_Desc;		//下次自动刷新 :
	Text		*Lab_Time;			//99
	
	UI_Mystery():scrollBar(nullptr)
	{
	}
};

class MysteryShopLayer;
class MysteryShopItem : public Layout
{
public:
	~MysteryShopItem();

	static MysteryShopItem* create(ShopItemParam param, MysteryShopLayer* parent);

	virtual bool  init(ShopItemParam param, MysteryShopLayer* parent);

	// 更新在售状态;
	void  updateState(bool bEnabled);

	// 更新价格状态;
	void  updatePrice();

private:
	MysteryShopItem();

	void  initUI();

	void  onBtnClick(Ref* ref, Widget::TouchEventType type);

private:
	UIShopItem  m_ui;

	MysteryShopLayer*  m_parent;

	int  m_nIndex;

	ShopItemParam _param;

};


class MysteryShopLayer:public Layer,public Observer
{
public:
	~MysteryShopLayer(void);

	static MysteryShopLayer* create(SHOP_TYPE type);

	virtual void  onEnter();

	virtual void  onExit();

	virtual bool  init(SHOP_TYPE type );

	void initUI();

	void  onBtnClickRefresh(Ref* ref, Widget::TouchEventType type);

	void  onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	void  onItemClicked(int nIndex);

	static string checkItemScoreImg(ShopItemParam Param);

	void refreshSingleItem( const ShopItemParam* item );

	void refreshAllItemOther();

	void updateNextFram(float ft);

	virtual void  updateSelf(void* pData);

private:
	MysteryShopLayer(void);
	void updateNextFramRemove(float ft);
	UI_Mystery m_Son;
	vector<MysteryShopItem*>  m_vcAllItems;
	SHOP_TYPE	_shopType;

};

#endif //ShopLayer_h_
