#ifndef RankTopController_h__
#define RankTopController_h__

#include "Communication/ICommListener.h"

class RankTopController : public ICommListener
{
public:
	RankTopController();
	virtual ~RankTopController();

public:

	virtual void onRecvMessage(int nCmdID, const char* szData);

	//发送获取各排行榜信息
	void sendGetRankTopListMsg(int rankType);

private:

	//解析拉取的排行榜数据
	void parseRankTopListMsg(const char* szData);
};

#endif