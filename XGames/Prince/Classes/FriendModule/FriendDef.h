#ifndef FriendDef_h__
#define FriendDef_h__

#include "CocosGUI.h"
#include "cocos2d.h"
#include <string>

#include "../Common/Common.h"
#include "ToolModule/UIToolHeader.h"
#include "Widget/SuperRichText.h"

const int MAX_SEND_MESSAGE_INTERVAL = 5;
const float MAX_CHAT_CONTENT_LINE_HEIGHT = 35.0f;//聊天信息单行高度
const float MAX_CHAT_CONTENT_LINE_WIDTH = 600.0f;//聊天信息单行宽度

const float MAX_CHAT_CONTENTFONT_SIZE = 24.0f;
//聊天内容时间长度
const float MAX_CHAR_TIME_LENGTH = 75.0f;
//聊天内容图标长度
const float MAX_CHAR_TYPE_NORMAL_LENGTH = 32.0f;
const float MAX_CHAR_TYPE_SYSTEM_LENGTH = 76.0f;
const float MAX_CHAR_VIP_LENGTH = 53.0f;

//消息最大条数
const int MAX_CAHT_COUNT = 30;

#define CHAT_COLOR_ME Color3B(0xf5, 0xef, 0xd9)			//自己
#define CHAT_COLOR_PRIVATE Color3B(0x36, 0xd1, 0x5b)	//私聊
#define CHAT_COLOR_GROUP Color3B(0x36, 0xd1, 0x5b)	//帮派
#define CHAT_COLOR_PUBLIC Color3B(0x36, 0xd1, 0x5b)  //综合
#define CHAT_COLOR_EDITBOX Color3B(0x76, 0x57, 0x35)	//输入框 
#define CHAT_COLOR_SYSTEM Color3B(0xfd, 0xfd, 0x4d)		//系统
using namespace std;

/*移入类中
const std::string  CHAT_ME = STR_UTF8("我");
const string  CHAT_PRIVATE = STR_UTF8("【私】");
const string  CHAT_GROUP = STR_UTF8("【帮】");
const string  CHAT_PUBLIC = STR_UTF8("【综】");
const string CHAT_COLON = ": ";
const string CHAT_BRACKET_L = STR_UTF8("【");
const string CHAT_BRACKET_R  = STR_UTF8("】");
*/

class  EditBoxEx;
enum FRIEND_CHAT_TYPE
{
	FRIEND_CHAT_NONE,
	FRIEND_CHAT_PUBLIC,	//综合
	FRIEND_CHAT_GROUP,	//帮派
	FRIEND_CHAT_PRIVATE,	//私聊（好友）
	FRIEND_CHAT_SYSTEM,		//系统公告
	FRIEND_CHAT_SYSTEM2,		//系统公告(跑马灯)
	FRIEND_CHAT_PLAYER_DO	//玩家行为公告(跑马灯)
};

enum FRIEND_EVENT_TYPE
{
	FRIEND_EVENT_TYPE_GET_LIST,
	FRIEND_EVENT_TYPE_ADD_LIST,
	FRIEND_EVENT_TYPE_GIVE_POWER,
	FRIEND_EVENT_TYPE_GET_POWER,
	FRIEND_EVENT_TYPE_ONE_KEY_GIVE_POWER,
	FRIEND_EVENT_TYPE_ONE_KEY_GET_POWER,
	FRIEND_EVENT_TYPE_ADD_FRIEND,
	FRIEND_EVENT_TYPE_SEARCH_FRIEND,
	FRIEND_EVENT_TYPE_APPLY_LIST,
	FRIEND_EVENT_TYPE_APPLY_FRIEND,
	FRIEND_EVENT_TYPE_ONE_KEY_APPLY_FRIEND,
	FRIEND_EVENT_TYPE_DELETE_FRIEND,
	FRIEND_EVENT_TYPE_TUI_SONG,
};

typedef struct CHAT_CONTENT
{
	int userId;
	int type;
	int vipLvl;
	int lvl;
	int resId;
	std::string strTime;
	std::string strName;
	std::string strContent;
	int nDisHeight; //显示高度
	int firstLineCharCount;//第一行显示文字个数
	bool isRead; //是否已读
}FRIEND_CHAT_MESSAGE;

typedef vector<FRIEND_CHAT_MESSAGE> FRIEND_CHAT_MESSAGE_LIST;

typedef struct FRIEND_INFO
{
	int userId;
	int resId;
	std::string strName;	//名字
	int nPower;				//战斗力
	int nLevel;				//等级
	int sendPower;         //赠送体力
	int getPower;          //领取体力
	int online;            //在线
}FRIEND_INFOS;
typedef vector<FRIEND_INFOS> FRIEND_INFOS_LIST;

typedef struct ADD_FRIEND_INFO
{
	int userId;
	int resId;
	std::string strName;	//名字
	int nPower;				//战斗力
	int nLevel;				//等级
	int added;
}ADD_FRIEND_INFOS;
typedef vector<ADD_FRIEND_INFOS> ADD_FRIEND_INFO_LIST;

typedef struct SysNotice
{
	int id;
	int type;
	int repMinute; //显示次数
	bool isRead; //是否已读	
	std::string strGetTime;
	int strStartTime; //开始时间
	int strEndTime; //结束时间
	std::string strTitle;
	std::string strContent;
}SYS_NOTICE;
typedef vector<SYS_NOTICE> SYS_NOTICE_LIST;
typedef deque<SYS_NOTICE> SYS_NOTICE_Deque;
//
struct  UI_Friend_Chat_Message
{
	// UI根节点层容器
	Layout		*pRoot;
	//公告
	Layout		*pLayNotice;
	ImageView   *noticeImg;
	SuperRichText	*pTextNotice;
};
//聊天界面
struct  UI_Friend_Chat
{
	// UI根节点层容器
	Layout		*pRoot;
	ImageView		*pLayChat;
	//私聊
	CheckBox		*pBtnPrivate;
	ImageView       *imgTishi;
	//山寨
	CheckBox		*pBtnGroup;
	//综合
	CheckBox		*pBtnPublic;
	
	ListView		*pSvContent;
	Button			*pBtnSend;
	Button			*pBtnClose;
	TextField		*pEditBox;
	EditBoxEx					*pSendEditBox;
	Vec2						vecSendEditBox;
	Size						sizeSendEditBox;
	//
	Text			*pTextSend;
	Text			*pTextToName;
};

//聊天界面弹出菜单
struct  UI_Friend_Chat_Popup
{
	Layout		*pRoot;
	Button		*pBtnAddFriend;
	//私聊
	Button		*pBtnChatPrivate;
	//拉黑
	Button		*pBtnBacklist;
	//
	Text		*pTextName;
	Text		*pTextLevel;

	UIToolHeader*			pToolIcon;
};

enum Friend_type
{
	Friend_type_player,
	Friend_type_add,
	Friend_type_request
};

enum Friend_add_type
{
	Friend_add_type_List,
	Friend_add_type_search
};

enum  Friend_Apply_type
{
	Friend_Apply_type_refuse,
	Friend_Apply_type_agree,
	Friend_Apply_type_one_key_refuse,
	Friend_Apply_type_one_key_agree
};

//好友界面
struct  UI_Friend_Friend
{
	Layout		*pRoot;
	/////////////////////////////////好友面板////////////////////////////
	ImageView   *imgFriend;  

	Button		*pBtnBack;
	CheckBox		*pBtnFriend;
	CheckBox		*pBtnAddFriend;
	CheckBox		*pBtnRequestFriend;

	//我的好友
	Layout		*pLayFriend;
	Button      *pBtnGive;
	Button      *pBtnGet;
	Text        *friendCount;
	ListView    *friendList;

	//添加好友
	Layout		*pLayAddFriend;
	Button      *pBtnAddBack;
	Button      *pBtnChange;
	Text        *friendAddCount;
	Button      *pBtnSearch;
	EditBoxEx   *inputName;
	ListView    *addFriendList;
	ImageView   *addimgTips;

	//申请好友
	Layout		*pLayRequestFriend;
	Button      *pBtnRefuse;
	Button      *pBtnAgree;
	ListView    *requestfriendList;
	
	//添加键
	Layout      *playChoose;
	ImageView   *addImgTips;
	Button      *ButtonMessage;
	Text        *addName;
	Layout      *iconPlayer;

	////////////////////////////////聊天面板////////////////////////
	ImageView   *imgchat;  

};

#endif
