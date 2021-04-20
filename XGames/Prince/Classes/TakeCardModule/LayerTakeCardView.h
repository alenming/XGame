#ifndef LayerTakeCardView_h__
#define LayerTakeCardView_h__

#include "cocos2d.h"
#include "CocosGUI.h"

#include "../UI/ModalLayer.h"
#include "DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/LayerTool.h"
#include "CCArmature.h"
#include "ToolModule/ToolModel.h"
#include "TakeCardDef.h"

enum CardViewType
{
	CardViewType_Coin,
	CardViewType_Gold,
};
//面板控件集合
typedef struct tagUI_CardView
{
	Layout* pRoot;
	ImageView* Img_Coin;
	Button*   Btn_CloseCoin;
	cocos2d::ui::ScrollView* listCoin;
	CheckBox*  allCoin;
	CheckBox*  ItemCoin;
	CheckBox*  pieceCoin;

	ImageView* Img_Gold;
	Button*   Btn_CloseGold;
	cocos2d::ui::ScrollView* listGold;
	CheckBox*  allGold;
	CheckBox*  atkGold;
	CheckBox*  defGold;
	CheckBox*  supGold;
	CheckBox*  rushGold;

	tagUI_CardView()
	{
		memset(this, 0, sizeof(tagUI_CardView));
	}
};

class LayerTakeCardView: public ModalLayer
{

public:
	~LayerTakeCardView();
	static LayerTakeCardView* create(CardViewType type);
	bool init(CardViewType type);

	virtual void onEnter();
	virtual void onExit(); 

	//按钮回调
	void  onBtnClick( Ref* ref, Widget::TouchEventType type );
	void  onCheckClick(Ref* sender,CheckBox::EventType type);
	//物品栏消息相应
	void onScrollViewEvent( Ref* pSender, ScrollView::EventType type);
protected:
	LayerTakeCardView();
	void  initUI();
	void  openTableInfo();

	void  reSetCoinCheckBox();
	void  setCoinAll();
	void  setCoinItem();
	void  setCoinPiece();
	void  setCoinList(vector<Card_View_Node> itemList);

	void  reSetGoldCheckBox();
	void  setGoldAll();
	void  setGoldAtk();
	void  setGoldDef();
	void  setGoldSup();
	void  setGoldRush();
	void  setGoldList(vector<Card_View_Node> itemList);
private:
	CardViewType m_type;

	tagUI_CardView m_ui;


	CustomScrollBar* m_customScrollBar1; //滚动条
	CustomScrollBar* m_customScrollBar2; //滚动条

	vector<Card_View_Node> m_CoinAll; //所有
	vector<Card_View_Node> m_CoinItem; //道具
	vector<Card_View_Node> m_CoinPiece; //碎片

	vector<Card_View_Node> m_GoldAll; //所有
	vector<Card_View_Node> m_GoldAtk; //攻击
	vector<Card_View_Node> m_GoldDef; //防御
	vector<Card_View_Node> m_GoldSup; //辅助
	vector<Card_View_Node> m_GoldRush; //先手
};

#endif
