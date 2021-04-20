#ifndef FriendModel_h__
#define FriendModel_h__

#include "FriendDef.h"
#include "../DataManager/DataManager.h"
#include "../ToolModule/ToolModel.h"
class FriendModel :public Observable
{

public:

	static FriendModel* getInstance();
	static void destroyInstance();
		
protected:

private:
	FriendModel();
	~FriendModel();
public:
	//
	//通知监听者
	void notify(FRIEND_EVENT_TYPE eventType, void* data = nullptr);

	void parseGetFriendsInfoData(const Json::Value &data); //解析获取好友列表
	void parseSendPowerData(const Json::Value &data); //赠送体力
	void parseGetPowerData(const Json::Value &data); //获取体力
	void parseOneKeySendData(const Json::Value &data); //一键赠送
	void parseOneKeyGetData(const Json::Value &data); //一键领取

	void parseAddFriendListData(const Json::Value &data); //解析可添加好友列表
	void parseAddFriendData(const Json::Value &data); //添加好友
	void parseSearchFriendData(const Json::Value &data); //搜索好友

	void parseApplyFriendListData(const Json::Value &data); //添加好友申请列表
	void parseApplyFriendData(const Json::Value &data); //拒绝/同意添加好友
	void parseOnekeyApplyFriendData(const Json::Value &data); //一键拒绝/同意添加好友
	void parseDeleteFriendData(const Json::Value &data); //删除好友

	void parsTuiSongData(const Json::Value &data); //推送消息

	void setFriendId(int id) {friendId = id;};
	int  getMaxFriends() {return m_MaxFriends;}; //好友个数上限
	int  getGiftPower() {return m_GiftPower;};  //已领取体力数
	int  getMaxPower() {return m_MaxPower;};   //领取上限
	void setFriendAddType(Friend_add_type type) {m_Friend_add_type = type;};
	void setFriendApplyType(Friend_Apply_type type) {m_Friend_Apply_type = type;};
	Friend_Apply_type getFriendApplyType() {return m_Friend_Apply_type;};

	bool& getHaveNews() {return haveNews;};
	bool& getHaveFriend() {return havefriend;};
	bool& getIsLoad() {return isLoad;};
	const FRIEND_INFOS_LIST& getFriends() {return m_vfriends;};
	const ADD_FRIEND_INFO_LIST& geAddFriendList() {return m_vAddfriends;};
	const ADD_FRIEND_INFO_LIST& getSearchfriends() {return m_vSearchfriends;};
	const ADD_FRIEND_INFO_LIST& getApplyfriends() {return m_vApplyfriends;};

	bool getFriend(const int& id, FRIEND_INFOS& friendInfo);

	//综合聊天
	void clearPublicChatMessages();
	void addPublicChatMessage(const int& id, const int& type, const string& chatMessage, const string& name, const string& time, const int& vipLvl, const int& level, const int& resId);
	void addPublicChatMessageNode(FRIEND_CHAT_MESSAGE& chatMessage);

	//私聊
	void clearPrivateChatMessages();
	void addPrivateChatMessage(const int& id, const int& type, const string& chatMessage, const string& name, const string& time, const int& vipLvl, const int& level, const int& resId);
	void addPrivateChatMessageNode(FRIEND_CHAT_MESSAGE& chatMessage);

	bool getHavePrivateChatMessagesNotRead();

	//帮派
	void clearGroupChatMessages();
	void addGroupChatMessage(const int& id, const int& type, const string& chatMessage, const string& name, const string& time, const int& vipLvl, const int& level, const int& resId);
	void addGroupChatMessageNode(FRIEND_CHAT_MESSAGE& chatMessage);

	//获取消息
	FRIEND_CHAT_MESSAGE_LIST& getChatMessagesByType(const int& type);


	void setchatMessageDisWidth(float fWidth);
	float getchatMessageDisWidth();
	//@disCharCount： 第一行显示文字数；返回显示高度
	int getChatMessageDisHeight(FRIEND_CHAT_MESSAGE& chatMessage, int& disCharCount);

	//增加公告
	void addSysNotice(const int& id, const int& type, const int& repMinute, const int& strStartTime, const int& strEndTime, const string& strTitle, const string& strContent);
	void addSysNoticeNode(SYS_NOTICE& sysNotice);
	//获取下一条公告
	SYS_NOTICE* getNextSysNotice();

	string getChatTypeIcon(const int& type);
	string getChatVipIcon(const int& vipLvl);
	cocos2d::Color3B getChatContentColor(const int& type);
	//获取时间 HH:MM
	string getCurTime(string time);
	bool getCurTime(string time, int& year, int& month, int& date);
	//return time2-time1  dates
	int cmpTime(tm* time1, tm* time2);

private:

	static FriendModel* mInstance;

	int m_MaxFriends; //好友个数上限
	int m_GiftPower;  //已领取体力数
	int m_MaxPower;   //领取上限
	int friendId; //每次操作的好友id

	bool haveNews;
	bool havefriend;
	bool isLoad;
	Friend_add_type m_Friend_add_type;
	Friend_Apply_type m_Friend_Apply_type;
	FRIEND_INFOS_LIST m_vfriends;
	ADD_FRIEND_INFO_LIST m_vAddfriends;
	ADD_FRIEND_INFO_LIST m_vSearchfriends;
	ADD_FRIEND_INFO_LIST m_vApplyfriends;


	//综合
	FRIEND_CHAT_MESSAGE_LIST m_vpublicChatMessages;
	//私聊
	FRIEND_CHAT_MESSAGE_LIST m_vprivateChatMessages;
	//帮派
	FRIEND_CHAT_MESSAGE_LIST m_vGroupChatMessages;
	//
	SYS_NOTICE_Deque m_vSysNotice;
	//聊天内容显示宽度，用于计算单条聊天信息显示行数
	float m_fchatMessageDisWidth;

	const std::string  CHAT_ME;
	const string  CHAT_PRIVATE;
	const string  CHAT_GROUP;
	const string  CHAT_PUBLIC;
	const string CHAT_COLON;
	const string CHAT_BRACKET_L;
	const string CHAT_BRACKET_R;
};

#endif
