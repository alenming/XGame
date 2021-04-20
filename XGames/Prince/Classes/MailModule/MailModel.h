#ifndef MailModel_h__
#define MailModel_h__

#include "MailDef.h"
#include "../DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
#include "../FriendModule/FriendModel.h"

class MailModel:public Observable
{
public:
	~MailModel();
	static MailModel* getInstance();
	static void destroyInstance();
	//通知监听者
	void notify(MAIL_TYPE eventType, void* data = nullptr);

	void parseGetMailsData(const Json::Value &data); //解析邮件
	void parseOpenMailData(const Json::Value &data);//打开邮件
	void parseGetAccessoryData(const Json::Value &data); //获取附件数据
	void parseDeleteMailData(const Json::Value &data); //删除邮件

	int deleteMail(const int& id);
	void deleteAllMail();

	//id:邮件的ID,-1 表示全部
	void updateMailStat(const int& id, const int& stat);
	MAIL_INFOS_DEQUE& getMails();
	bool getMail(const int& id, MAIL_INFOS& mail);
	//
	void setLoaded(bool isLoaded);
	//是否已经从服务器加载过
	bool isLoaded();

	int getMailCountMax() {return m_mailCountMax;};
	MAIL_INFO_ACCESSORY_LIST* getReawrd() {return &m_Reawrd;};
private:

	MailModel();

private:

	static MailModel* mInstance;

	MAIL_INFOS_DEQUE m_vmails;
	MAIL_INFO_ACCESSORY_LIST m_Reawrd;
	int  m_mailCountMax;

	bool m_isLoaded;
};

#endif
