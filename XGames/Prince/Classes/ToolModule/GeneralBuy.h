#ifndef GeneralBuy_h__
#define GeneralBuy_h__


#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "FormationModule/CustomScrollBar.h"
#include "Common/Common.h"
#include "ui/UIDef.h"
#include "Temp/CustomTips.h"
#include "Temp/CustomPop.h"
#include "MainModule/MainModel.h"
#include "ui/ModalLayer.h"
#include "Widget/ActionCreator.h"
#include "Utils/SoundUtils.h"

//购买
struct  UI_Tools_Shop_Buy
{
	// UI根节点层容器
	cocos2d::ui::Layout		*pRootTools; //道具容器
	cocos2d::ui::ScrollView	*pScrollViewTools; //道具滑动条
	CustomScrollBar*		listHeroBar; //道具左边进度条

	//弹出框
	cocos2d::ui::Layout		*pLayerPopup; //道具批量购买

	cocos2d::ui::Button		*pBtnAdd1;
	cocos2d::ui::Button		*pBtnAdd10;
	cocos2d::ui::Button		*pBtnDec1;
	cocos2d::ui::Button		*pBtnDec10;
	cocos2d::ui::Button		*pBtnConfirm;
	cocos2d::ui::Button		*pBtnCancel;

	cocos2d::ui::Text		*pTextBuyNum;
	cocos2d::ui::Text		*pTextBuyNumTip;
	cocos2d::ui::Text		*pTextAtlasNum;

	cocos2d::ui::Text		*pTextPriceNum;
};

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

typedef std::function<void(int amount)> GeneralBuyCallback;

class GeneralBuy:public ModalLayer 
{
public:
	~GeneralBuy(void);
	static GeneralBuy* create(int LimitShopBuyNum,int price,GeneralBuyCallback calBack = nullptr);

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi(int LimitShopBuyNum,int price,GeneralBuyCallback calBack = nullptr);  //初始化主界面

	void	onBtnShopAdd1(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnShopAdd10(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnShopDec1(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnShopDec10(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnShopConfirm(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnShopCancel(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	void showBuyNum(const int& num);
private:
	GeneralBuy(void);

	UI_Tools_Shop_Buy	layerToolsShop;  //道具商店界面

	int     m_nLimitShopBuyNum;
	int     m_pirce;
	int     m_nShopBuyNum;
	int     m_nNeedMoney; 

	GeneralBuyCallback _callBack;
};

#endif
