#ifndef LayerMazeLimitBuy_h__
#define LayerMazeLimitBuy_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "MazeDef.h"
#include "IMazeView.h"
#include "DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/LayerTool.h"
#include "ToolModule/UICardHead.h"
#include "ToolModule/UICardPrize.h"
#include "Widget/LayerGetCard.h"
#include "MazeLimitController.h"

class IMazeLimitBuyObserver
{
public:
	virtual void onMazeLimitBuyEnd() = 0;
	virtual void onMazeLimitBuyWait() = 0;

};

class LayerMazeLimitBuy: public ModalLayer, public IMazeLimitView
{

public:
	LayerMazeLimitBuy();
	~LayerMazeLimitBuy();

	CREATE_FUNC(LayerMazeLimitBuy);

	virtual bool init();

	virtual void	onLimitBuyResult(const int& nResult, const vector<TOOL_NEW_INFO>& vecNewTools);
public:	
	//
	void updateInfo(const int& heroTmpId, const int& limtSecond, const int& heroPrice, cocos2d::Vec2 vecStartPostion,Layout* layout,int buyCount);

	void setLimitBuyObserver(IMazeLimitBuyObserver* observer)
	{
		m_pObserver = observer;
	}
protected:
	void	initUI();
	
	//按钮事件
	void    onClickPass(Ref* sender, cocos2d::ui::Widget::TouchEventType type);
	void	onBtnBuy(Ref* sender, cocos2d::ui::Widget::TouchEventType type);	
	void	onBtnWait(Ref* sender, cocos2d::ui::Widget::TouchEventType type);

	//倒计时
	void startCountDown(const long ltotalSec);
	void stopCountDown();
	void updateCountDown(float ft);
	//卡牌飞出动画
	void animationHeroMove(int isOut, cocos2d::Vec2 vecStartPostion);
	void animationHeroMoveCallFunND(int isOut);
private:
	// UI控件集合
	UI_Maze_Limit m_ui;

	MazeLimitController mMazeLimitController;
	//
	//倒计时
	time_t	m_time_start_sec;
	time_t	m_ltotalSec;
	bool    m_isFreeTime;
	int m_price;
	//
	cocos2d::Vec2 m_vecHeroMoveStartPostion;

	IMazeLimitBuyObserver* m_pObserver;

	UICardHead *m_head;
	UICardPrize *m_cardPrize;
};

#endif // 
