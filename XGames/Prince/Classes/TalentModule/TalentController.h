#ifndef TalentController_h__
#define TalentController_h__

#include "Communication/ICommListener.h"

class TalentController: public ICommListener
{

public:

	TalentController();
	virtual ~TalentController();

public:

	//��Ϣ���սӿ�
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	//���ͼ����滻��Ϣ
	static void sendReplaceSkillMsg(int skillType, int skillId);

private:

	//���������滻��Ϣ
	void parseReplaceSkillMsg(const char* szData);
};


#endif //TalentController_h__