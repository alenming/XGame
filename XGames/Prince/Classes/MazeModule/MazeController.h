#ifndef MazeController_h__
#define MazeController_h__

#include "IMazeView.h"
#include "Communication/ICommListener.h"
#include "../Utils/StringFormat.h"


class MazeController: public ICommListener
{
public:
	MazeController();
	~MazeController();

	static MazeController* getInstance();
	static void destroyInstance();
	static		MazeController*		ms_pInstance;
	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	void processGetAllMazesData(const char* szData);  //获取迷宫数据
	void processRandomRoadData(const char* szData);  //获取迷宫随机事件
	void processEventThingInfo(const char* szData);  //获取迷宫随机事件每个对应事件数据

	void processOpenBoxData(const char* szData);
	void processOpenBoxEndData(const char* szData);
	void processOpenMapBoxData(const char* szData);

	void processAllMazesData(Json::Value& data);
	void processMazesDropData(Json::Value& data, MAZE_DROP_INFO& dropInfo);


	//设置UI接口
	void onSetView(IMazeView* pView);

	//移除UI接口
	void onRemoveView();
	IMazeView* onGetView() { return  m_pView;}
	// 获取迷宫信息
	void onGetAllMazes();
	//迷宫随机路口事件
	void onRandomRoad();
	//获取迷宫已获得的特定事件反馈消息
	void onGetEventThingInfo(const int& eventId);
	//迷宫打开宝箱
	void onOpenBox(const int& id);
	//迷宫打开宝箱结束
	void onOpenBoxEnd();
	//迷宫打开藏宝图宝箱
	void onOpenMapBox(const int& boxId ,const int& id);

	int&  getLoginServerTime() {return mtLoginServerTime;};

protected:	
	IMazeView* m_pView;
	
	int  mtLoginServerTime;//服务器时间戳
private:

};

#endif 
