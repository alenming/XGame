#ifndef HeroRecruitController_h__
#define HeroRecruitController_h__

#include "Communication/ICommListener.h"

class HeroRecruitController: public ICommListener
{

public:

	HeroRecruitController();
	virtual ~HeroRecruitController();

public:

	//��Ϣ���սӿ�
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	//��Ƭ�ϳɵ���
	static void sendRecruitMsg(const int id);

protected:

	//�ϳɵ���
	void parseRecruitMsg(const char* szData);

};

#endif //HeroRecruitController_h__
