
#ifndef MysteryShopClick_h_
#define MysteryShopClick_h_

#include "NewShopModule/ShopBase.h"
#include "MysteryShopController.h"
#include "MysteryShopModel.h"
#include "MysteryShopLayer.h"

enum ONEITEM_CBACK
{
	ONE_ITEMS_EXIT,
	ONE_ITEMS_BUY
};

class MysteryShopClick:public ModalLayer 
{
public:
	~MysteryShopClick();

	static MysteryShopClick* create(ShopItemParam index,MysteryShopLayer* la);

	virtual bool init(ShopItemParam index, MysteryShopLayer* la);
	virtual void onEnter() override;
	virtual void onExit() override;

	MysteryShopClick();
	Layout* getSon();
	ImageView* getImagviewDi();
protected:
	
	void initUi();

	void updataInfo();
	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type, ONEITEM_CBACK _callback);

	std::string checkItemScoreImg(ShopItemParam Param);


private:

	UIOneItem m_ui;
	 
	ShopItemParam index;
	MysteryShopLayer* parrent;

};

#endif //ShopLayer_h_
