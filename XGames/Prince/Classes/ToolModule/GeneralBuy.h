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

//����
struct  UI_Tools_Shop_Buy
{
	// UI���ڵ������
	cocos2d::ui::Layout		*pRootTools; //��������
	cocos2d::ui::ScrollView	*pScrollViewTools; //���߻�����
	CustomScrollBar*		listHeroBar; //������߽�����

	//������
	cocos2d::ui::Layout		*pLayerPopup; //������������

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

	void initUi(int LimitShopBuyNum,int price,GeneralBuyCallback calBack = nullptr);  //��ʼ��������

	void	onBtnShopAdd1(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnShopAdd10(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnShopDec1(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnShopDec10(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnShopConfirm(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnShopCancel(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	void showBuyNum(const int& num);
private:
	GeneralBuy(void);

	UI_Tools_Shop_Buy	layerToolsShop;  //�����̵����

	int     m_nLimitShopBuyNum;
	int     m_pirce;
	int     m_nShopBuyNum;
	int     m_nNeedMoney; 

	GeneralBuyCallback _callBack;
};

#endif
