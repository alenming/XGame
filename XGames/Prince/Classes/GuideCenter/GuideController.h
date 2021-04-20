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

	//����Э��
	virtual void onRecvMessage(int nCommandID, const char* szData);

	//������������Э��
	static void sendGuideProcess(int chainId, int groupId);

private:

	//������������Э��
	void parseGuideProecss(const char* szData);


};



#endif //GuideController_h__