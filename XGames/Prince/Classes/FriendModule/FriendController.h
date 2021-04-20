#ifndef FriendController_h__
#define FriendController_h__

#include "Communication/ICommListener.h"
#include "FriendDef.h"

class FriendController: public ICommListener
{
public:
	~FriendController();

	static FriendController* getInstance();
	static void destroyInstance();

	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	void processGetFriendsInfoData(const char* szData);
	void processSendPowerData(const char* szData);
	void processGetRewardsData(const char* szData);
	void processOneKeySendData(const char* szData);
	void processOneKeyGetData(const char* szData);
	void processAddFriendListData(const char* szData);
	void processSearchFriendData(const char* szData);
	void processApplyFriendListData(const char* szData);
	void processApplyFriendData(const char* szData);
	void processOneKeypplyFriendData(const char* szData);

	void processAddFriendData(const char* szData);
	void processDeleteFriendData(const char* szData);
	void processTuiSongData(const char* szData);

	//获取好友信息
	void onGetFriendsList();
	//添加好友
	void onGetAddFriendList();
	//送体力
	void onSendPower(const int& friendId);
	//一键赠送体力
	void onOneKeySendPower();
	//领体力
	void onGetPower(const int& friendId);
	//一键领取体力
	void onOneKeyGetPower();
	//搜索好友
	void onSearchFriend(const string& key);

	//好友申请
	void onRequestFriendList();
	//拒绝
	void onApplyFriend(int isRefuse,const int& friendId);
	//一键拒绝/一键同意
	void onOneKeyApplyFriend(int isRefuse);

	//加好友
	void onAddFriend(const int& friendId);
	//删除好友
	void onDeleteFriend(const int& friendId);

protected:
	
private:

	FriendController();

	static FriendController* mInstance;
};

#endif 
