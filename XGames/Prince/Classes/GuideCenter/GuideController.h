#ifndef GuideController_h__
#define GuideController_h__

#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"

class GuideController : public ICommListener
{

public:

	GuideController();
	virtual ~GuideController();

public:

	//接受协议
	virtual void onRecvMessage(int nCommandID, const char* szData);

	//发送引导进度协议
	static void sendGuideProcess(int chainId, int groupId);

private:

	//接受引导进度协议
	void parseGuideProecss(const char* szData);


};



#endif //GuideController_h__