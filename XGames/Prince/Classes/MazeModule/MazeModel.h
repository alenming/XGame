#ifndef MazeModel_h__
#define MazeModel_h__

#include "IMazeView.h"

#include "../DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
class MazeModel
{
public:

	static MazeModel* getInstance();
	static void destroyInstance();
		
protected:

private:
	MazeModel();

	static MazeModel* ms_pInstance;
public:	
	void clearMazeInfo();
	MAZE_INFOS& getMazeInfos();

	void setTotleStep(const int& step);
	void setCurPower(const int& power);
	void setTimes(const int& times);
	
	void clearMazEvent();
	void setEventId(const int& eventId);
	int  getEventId();

	void setCurEventId(const int& id);
	int getCurEventId();

	void clearMazeDropInfo();
	MAZE_DROP_INFO& getMazeDropInfo();

	//处理boss事件奖励
	void processMazesData(Json::Value data);

	//处理翻牌奖励
	void processMazesOpenCardData(Json::Value data);

private:

	MAZE_INFOS m_MazeInfos;

	int m_EventId;  //随机得到的事件ID

	MAZE_DROP_INFO  m_mazeDropInfo;
	int				m_nCurEventId;
};

#endif // MazeModel_h__
