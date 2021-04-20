#ifndef HeroRecruitController_h__
#define HeroRecruitController_h__

#include "Communication/ICommListener.h"

class HeroRecruitController: public ICommListener
{

public:

	HeroRecruitController();
	virtual ~HeroRecruitController();

public:

	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	//碎片合成道具
	static void sendRecruitMsg(const int id);

protected:

	//合成道具
	void parseRecruitMsg(const char* szData);

};

#endif //HeroRecruitController_h__
