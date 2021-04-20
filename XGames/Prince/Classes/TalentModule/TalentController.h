#ifndef TalentController_h__
#define TalentController_h__

#include "Communication/ICommListener.h"

class TalentController: public ICommListener
{

public:

	TalentController();
	virtual ~TalentController();

public:

	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	//发送技能替换消息
	static void sendReplaceSkillMsg(int skillType, int skillId);

private:

	//解析技能替换消息
	void parseReplaceSkillMsg(const char* szData);
};


#endif //TalentController_h__