#ifndef MazeLimitController_h__
#define MazeLimitController_h__

#include "IMazeView.h"
#include "Communication/ICommListener.h"


class MazeLimitController: public ICommListener
{
public:

	MazeLimitController();
	~MazeLimitController();

public:

	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);
	void processLimitBuyData(const char* szData);

	void setTalkingDataPurchase(const vector<TOOL_NEW_INFO>& vecNewTools);
	
	//设置UI接口
	void onSetView(IMazeLimitView* pView);

	//移除UI接口
	void onRemoveView();

	// 获取迷宫信息
	void onLimitBuy();
	
protected:

	IMazeLimitView* m_pView;
	

};

#endif 
