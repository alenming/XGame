#ifndef LayerMazeLimitBuyItem_h__
#define LayerMazeLimitBuyItem_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "MazeDef.h"

#include "DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../ToolModule/LayerTool.h"

class IMazeLimitBuyItemObserver
{
public:
	virtual void	onMazeLimitClicked() = 0;

private:

};

class LayerMazeLimitBuyItem: public Layer
{

public:
	LayerMazeLimitBuyItem();
	~LayerMazeLimitBuyItem();

	CREATE_FUNC(LayerMazeLimitBuyItem);

	virtual bool init();

public:	
	//
	void updateInfo(const int& heroTmpId, const int& limtSecond,const int& buyCount);

	void  setObserver(IMazeLimitBuyItemObserver* observer)
	{
		m_MazeLimitObserver = observer;
	}

	long getLastCountDown();
	//
	cocos2d::Vec2 getHeroPostion();
protected:
	void	initUI();
	
	//按钮事件
	void	onClickPass(Ref* sender, cocos2d::ui::Widget::TouchEventType type);	
	

	//倒计时
	void startCountDown(const long ltotalSec);
	void stopCountDown();
	void updateCountDown(float ft);
private:
	// UI控件集合
	UI_Maze_Limit_Icon m_ui;

	//倒计时
	time_t	m_time_start_sec;
	time_t	m_ltotalSec;
	bool    m_isFreeTime;

	IMazeLimitBuyItemObserver* m_MazeLimitObserver;
};

#endif // 
