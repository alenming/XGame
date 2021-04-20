#ifndef MailController_h__
#define MailController_h__

#include "Communication/ICommListener.h"
#include "MailDef.h"

class MailController: public ICommListener
{
public:
	//活动单例
	static MailController* getInstance();
	static void destroyInstance();
	~MailController();

	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	void processChatMessageData(const char* szData);
	void processGetMailsData(const char* szData);
	void processOpenMailData(const char* szData);
	void processGetAccessoryData(const char* szData);
	void processDeleteMailData(const char* szData);
	
	//获取邮件
	void onGetMails();
	//打开邮件
	void onOpenMail(const int& id);
	//领取附件 id: 邮件的ID,-1 表示领取全部
	void onGetAccessory(const int& id);
	//删除邮件 id: -1表示删除全部邮件，否则删除指定邮件
	void onDeleteMail(const int& id);
private:
	MailController();

	static MailController* mInstance;
};

#endif 
