#include "FriendModel.h"

#include "../MainModule/MainModel.h"
#include "../Utils/StringFormat.h"
#include "Common/Common.h"
#include "FriendDef.h"
#include "LoginModule/LoginModel.h"

#include "cocos2d.h"
using namespace cocos2d;

FriendModel* FriendModel::mInstance = nullptr;

bool sortHero(FRIEND_INFO a, FRIEND_INFO b)
{
	//在线
	if (a.online != 0 &&  b.online == 0)
	{
		return true;
	}
	else if (a.online == 0 &&  b.online != 0)
	{
		return false;
	}
	else
	{
		if (a.nLevel > b.nLevel)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

FriendModel::FriendModel()
	: CHAT_ME(DataManager::getInstance()->searchCommonTexdtById(10124))
	, CHAT_PRIVATE(DataManager::getInstance()->searchCommonTexdtById(10125))
	, CHAT_GROUP(DataManager::getInstance()->searchCommonTexdtById(10126))
	, CHAT_PUBLIC(DataManager::getInstance()->searchCommonTexdtById(10127))
	, CHAT_COLON(": ")
	, CHAT_BRACKET_L(STR_UTF8("【"))
	, CHAT_BRACKET_R(STR_UTF8("】"))
{
	m_fchatMessageDisWidth = 520.0;
	haveNews = false;
	havefriend = false;
	isLoad = false;
	//loadLocalChatMessage();
}

FriendModel::~FriendModel()
{
	//saveLocalChatMessage();
}

FriendModel* FriendModel::getInstance()
{
	if (!mInstance)
	{
		mInstance = new FriendModel();
	}
	return mInstance;
}

void FriendModel::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void FriendModel::notify(FRIEND_EVENT_TYPE eventType, void* data /*= nullptr*/)
{
	ObserverParam observerParam;
	observerParam.id = (int)eventType;
	observerParam.updateData = data;

	notifyObservers(&observerParam);
}

void FriendModel::parseGetFriendsInfoData(const Json::Value &data)
{
	m_MaxFriends = data["maxFriends"].asInt(); //好友个数上限
	m_GiftPower = data["giftPower"].asInt();  //已领取体力数
	m_MaxPower = data["maxPower"].asInt();   //领取上限
	Json::Value friends	= data["friendlist"];
	m_vfriends.clear();
	for(int i=0; i<friends.size(); i++)
	{
		FRIEND_INFO friendInfo;
		Json::Value item		= friends[i];
		friendInfo.userId		= item["id"].asInt();
		friendInfo.nLevel		= item["level"].asInt();
		friendInfo.nPower		= item["totalAtk"].asInt();
		friendInfo.resId		= item["resId"].asInt();
		friendInfo.strName		= item["nickname"].asString();
		friendInfo.getPower     = item["getTyp"].asInt();
		friendInfo.sendPower    = item["sendTyp"].asInt();
		friendInfo.online       = item["online"].asInt();

		m_vfriends.push_back(friendInfo);
	}
	sort(m_vfriends.begin(),m_vfriends.end(),sortHero);//排序

	notify(FRIEND_EVENT_TYPE_GET_LIST);
}

void FriendModel::parseAddFriendListData(const Json::Value &data)
{
	Json::Value friends	= data["friendlist"];
	m_vAddfriends.clear();
	for(int i=0; i<friends.size(); i++)
	{
		ADD_FRIEND_INFOS friendInfo;
		Json::Value item		= friends[i];
		friendInfo.userId		= item["id"].asInt();
		friendInfo.nLevel		= item["level"].asInt();
		friendInfo.nPower		= item["totalAtk"].asInt();
		friendInfo.resId		= item["resId"].asInt();
		friendInfo.strName		= item["nickname"].asString();
		friendInfo.added        = 1;
		m_vAddfriends.push_back(friendInfo);
	}

	notify(FRIEND_EVENT_TYPE_ADD_LIST);
}

void FriendModel::parseSearchFriendData(const Json::Value &data)
{
	Json::Value friends	= data["friendlist"];
	m_vSearchfriends.clear();
	for(int i=0; i<friends.size(); i++)
	{
		ADD_FRIEND_INFOS friendInfo;
		Json::Value item		= friends[i];
		friendInfo.userId		= item["id"].asInt();
		friendInfo.nLevel		= item["level"].asInt();
		friendInfo.nPower		= item["totalAtk"].asInt();
		friendInfo.resId		= item["resId"].asInt();
		friendInfo.strName		= item["nickname"].asString();
		friendInfo.added        = 1;
		m_vSearchfriends.push_back(friendInfo);
	}

	notify(FRIEND_EVENT_TYPE_SEARCH_FRIEND);
}

void FriendModel::parseApplyFriendListData(const Json::Value &data)
{
	isLoad = true;
	Json::Value friends	= data["applylist"];
	m_vApplyfriends.clear();
	for(int i=0; i<friends.size(); i++)
	{
		ADD_FRIEND_INFOS friendInfo;
		Json::Value item		= friends[i];
		friendInfo.userId		= item["id"].asInt();
		friendInfo.nLevel		= item["level"].asInt();
		friendInfo.nPower		= item["totalAtk"].asInt();
		friendInfo.resId		= item["resId"].asInt();
		friendInfo.strName		= item["nickname"].asString();
		friendInfo.added        = 1;
		m_vApplyfriends.push_back(friendInfo);
	}

	notify(FRIEND_EVENT_TYPE_APPLY_LIST);
}

void FriendModel::parseApplyFriendData(const Json::Value &data)
{
	switch (m_Friend_Apply_type)
	{
	case Friend_Apply_type_refuse:
	case Friend_Apply_type_agree:
		{
			for (auto iter = m_vApplyfriends.begin(); iter != m_vApplyfriends.end(); iter++)
			{
				if (friendId == iter->userId)
				{
					m_vApplyfriends.erase(iter);
					break;
				}
			}
		}
		break;
	default:
		break;
	}
	

	notify(FRIEND_EVENT_TYPE_APPLY_FRIEND);
}

void FriendModel::parseOnekeyApplyFriendData(const Json::Value &data)
{
	switch (m_Friend_Apply_type)
	{
	case Friend_Apply_type_one_key_refuse:
		{
			m_vApplyfriends.clear();
		}
		break;
	case Friend_Apply_type_one_key_agree:
		{
			Json::Value friends	= data["applylist"];
			m_vApplyfriends.clear();
			for(int i=0; i<friends.size(); i++)
			{
				ADD_FRIEND_INFOS friendInfo;
				Json::Value item		= friends[i];
				friendInfo.userId		= item["id"].asInt();
				friendInfo.nLevel		= item["level"].asInt();
				friendInfo.nPower		= item["totalAtk"].asInt();
				friendInfo.resId		= item["resId"].asInt();
				friendInfo.strName		= item["nickname"].asString();
				friendInfo.added        = 1;
				m_vApplyfriends.push_back(friendInfo);
			}
		}
		break;
	default:
		break;
	}

	notify(FRIEND_EVENT_TYPE_ONE_KEY_APPLY_FRIEND);
}

void FriendModel::parseDeleteFriendData(const Json::Value &data)
{
	for (auto iter = m_vfriends.begin(); iter != m_vfriends.end(); iter++)
	{
		if (friendId == iter->userId)
		{
			m_vfriends.erase(iter);
			break;
		}
	}

	notify(FRIEND_EVENT_TYPE_DELETE_FRIEND);
}

void FriendModel::parsTuiSongData(const Json::Value &data)
{
	int flg = data["flg"].asInt();

	if (flg == 1)
	{
		haveNews = true;
	}
	else if(flg == 2)
	{
		havefriend = true;
	}

	notify(FRIEND_EVENT_TYPE_TUI_SONG);
}

void FriendModel::parseSendPowerData(const Json::Value &data)
{
	for (int i = 0; i < m_vfriends.size(); i++)
	{
		if (friendId == m_vfriends.at(i).userId)
		{
			m_vfriends.at(i).sendPower = 0;
			break;
		}
	}

	notify(FRIEND_EVENT_TYPE_GIVE_POWER);
}

void FriendModel::parseGetPowerData(const Json::Value &data)
{
	m_GiftPower = data["giftPower"].asInt();
	MainModel::getInstance()->updateData(data["changeInfo"]);

	for (int i = 0; i < m_vfriends.size(); i++)
	{
		if (friendId == m_vfriends.at(i).userId)
		{
			m_vfriends.at(i).getPower = 0;
			break;
		}
	}

	notify(FRIEND_EVENT_TYPE_GET_POWER);
}

void FriendModel::parseOneKeySendData(const Json::Value &data)
{
	for (int i = 0; i < m_vfriends.size(); i++)
	{
		m_vfriends.at(i).sendPower = 0;
	}

	notify(FRIEND_EVENT_TYPE_ONE_KEY_GIVE_POWER);
}

void FriendModel::parseOneKeyGetData(const Json::Value &data)
{
	m_GiftPower = data["giftPower"].asInt();
	MainModel::getInstance()->updateData(data["changeInfo"]);

	Json::Value giverId	= data["giverId"];
	for(int i = 0; i < giverId.size(); i++)
	{
		for (int j = 0; j < m_vfriends.size(); j++)
		{
			if (giverId[i].asInt() == m_vfriends.at(j).userId)
			{
				 m_vfriends.at(j).getPower = 0;
			}
		}
	}
	
	notify(FRIEND_EVENT_TYPE_ONE_KEY_GET_POWER);
}

void FriendModel::parseAddFriendData(const Json::Value &data)
{
	if (m_Friend_add_type == Friend_add_type_List)
	{
		for (int i = 0; i < m_vAddfriends.size(); i++)
		{
			if (friendId == m_vAddfriends.at(i).userId)
			{
				m_vAddfriends.at(i).added = 0;
				break;
			}
		}

	}
	else if(m_Friend_add_type == Friend_add_type_search)
	{
		for (int i = 0; i < m_vSearchfriends.size(); i++)
		{
			if (friendId == m_vSearchfriends.at(i).userId)
			{
				m_vSearchfriends.at(i).added = 0;
				break;
			}
		}

	}
	
	notify(FRIEND_EVENT_TYPE_ADD_FRIEND);
}

void FriendModel::clearPublicChatMessages()
{
	m_vpublicChatMessages.clear();
}

void FriendModel::addPublicChatMessage(const int& id, const int& type, const string& chatMessage, const string& name, const string& time, const int& vipLvl, const int& level, const int& resId)
{
	FRIEND_CHAT_MESSAGE msg;
	msg.userId = id;
	msg.type = type;
	msg.strTime		= time;
	msg.vipLvl		= vipLvl;
	msg.isRead      = false;
	msg.lvl			= level;
	msg.resId		= resId;

	if (id == MainModel::getInstance()->getMainParam()->mRoleId)//自己发言
	{
		msg.strContent	= CHAT_ME + CHAT_COLON + chatMessage;
	}
	else if (type == FRIEND_CHAT_SYSTEM || type == FRIEND_CHAT_SYSTEM2)
	{
		msg.strContent	= chatMessage;
	}
	else
	{
		msg.strContent	= name + CHAT_COLON + chatMessage;
	}
	msg.strName = name;
	addPublicChatMessageNode(msg);
}

void FriendModel::addPublicChatMessageNode(FRIEND_CHAT_MESSAGE& chatMessage)
{		
	if (m_vpublicChatMessages.size() > MAX_CAHT_COUNT)//只保存30条消息
	{
		m_vpublicChatMessages.erase(m_vpublicChatMessages.begin());
	}
	chatMessage.nDisHeight = getChatMessageDisHeight(chatMessage, chatMessage.firstLineCharCount);
	m_vpublicChatMessages.push_back(chatMessage);
}

//私聊
void FriendModel::clearPrivateChatMessages()
{
	m_vprivateChatMessages.clear();
}
void FriendModel::addPrivateChatMessage(const int& id, const int& type, const string& chatMessage, const string& name, const string& time, const int& vipLvl, const int& level, const int& resId)
{
	FRIEND_CHAT_MESSAGE msg;
	msg.userId		= id;
	msg.type		= type;
	msg.strTime		= time;
	msg.vipLvl		= vipLvl;
	msg.isRead      = false;
	msg.lvl			= level;
	msg.resId		= resId;
	if (id == MainModel::getInstance()->getMainParam()->mRoleId)
	{
		msg.strContent	= CHAT_ME + CHAT_COLON + chatMessage;
	}
	else
	{
		msg.strContent	= name + CHAT_COLON + chatMessage;
	}	
	msg.strName		= name;
	addPrivateChatMessageNode(msg);
}

void FriendModel::addPrivateChatMessageNode(FRIEND_CHAT_MESSAGE& chatMessage)
{		
	if (m_vprivateChatMessages.size() > MAX_CAHT_COUNT)//只保存30条消息
	{
		m_vprivateChatMessages.erase(m_vprivateChatMessages.begin());
	}
	chatMessage.nDisHeight = getChatMessageDisHeight(chatMessage, chatMessage.firstLineCharCount);
	m_vprivateChatMessages.push_back(chatMessage);
}

bool FriendModel::getHavePrivateChatMessagesNotRead()
{
	bool ret = false;
	for (auto iter = m_vprivateChatMessages.begin();
		iter != m_vprivateChatMessages.end(); iter++)
	{
		if(!iter->isRead)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

void FriendModel::clearGroupChatMessages()
{
	m_vGroupChatMessages.clear();
}

void FriendModel::addGroupChatMessage(const int& id, const int& type, const string& chatMessage, const string& name, const string& time, const int& vipLvl, const int& level, const int& resId)
{
	FRIEND_CHAT_MESSAGE msg;
	msg.userId		= id;
	msg.type		= type;
	msg.strTime		= time;
	msg.vipLvl		= vipLvl;
	msg.isRead      = false;
	msg.lvl			= level;
	msg.resId		= resId;
	if (id == MainModel::getInstance()->getMainParam()->mRoleId)
	{
		msg.strContent	= CHAT_ME + CHAT_COLON + chatMessage;
	}
	else
	{
		msg.strContent	= name + CHAT_COLON + chatMessage;
	}	
	msg.strName		= name;
	addGroupChatMessageNode(msg);
}

void FriendModel::addGroupChatMessageNode(FRIEND_CHAT_MESSAGE& chatMessage)
{
	if (m_vGroupChatMessages.size() > MAX_CAHT_COUNT)//只保存30条消息
	{
		m_vGroupChatMessages.erase(m_vGroupChatMessages.begin());
	}
	chatMessage.nDisHeight = getChatMessageDisHeight(chatMessage, chatMessage.firstLineCharCount);
	m_vGroupChatMessages.push_back(chatMessage);
}

//获取消息
FRIEND_CHAT_MESSAGE_LIST& FriendModel::getChatMessagesByType(const int& type)
{
	if (type == FRIEND_CHAT_PRIVATE)
	{
		return m_vprivateChatMessages;
	}
	else if (type == FRIEND_CHAT_PUBLIC)
	{
		return m_vpublicChatMessages;
	}
	else
	{
		return m_vGroupChatMessages;
	}
}

//增加公告
void FriendModel::addSysNotice(const int& id, const int& type, const int& repMinute, const int& strStartTime, const int& strEndTime, const string& strTitle, const string& strContent)
{
	SYS_NOTICE sysNotice;
	sysNotice.id			= id;
	sysNotice.type			= type;
	sysNotice.strStartTime	= strStartTime;
	sysNotice.strEndTime	= strEndTime;
	sysNotice.strTitle      = strTitle;
	sysNotice.strContent    = strContent;
	sysNotice.repMinute     = repMinute;
	sysNotice.isRead		= false;

	addSysNoticeNode(sysNotice);
}

void FriendModel::addSysNoticeNode(SYS_NOTICE& sysNotice)
{
	tm *nowtime = getNowTime();
	sysNotice.strGetTime = _TO_STR(nowtime->tm_year) + "-" + _TO_STR(nowtime->tm_mon) + "-" + _TO_STR(nowtime->tm_mday)
		+ " " + _TO_STR(nowtime->tm_hour) + ":"+ _TO_STR(nowtime->tm_min);

	if (sysNotice.type == FRIEND_CHAT_SYSTEM2)
	{
		m_vSysNotice.push_front(sysNotice);
	}
	else if (sysNotice.type == FRIEND_CHAT_PLAYER_DO)
	{
		m_vSysNotice.push_back(sysNotice);
	}
}

SYS_NOTICE* FriendModel::getNextSysNotice()
{
	if (m_vSysNotice.size() == 0)
	{
		return nullptr;
	}

	//超时或次数不足删除
	for (auto iter = m_vSysNotice.begin(); iter != m_vSysNotice.end();)
	{
		if (iter->repMinute <= 0 || (LocalTimer::getInstance()->getCurServerTime() < iter->strStartTime 
			||  LocalTimer::getInstance()->getCurServerTime() > iter->strEndTime && iter->type != FRIEND_CHAT_PLAYER_DO))
		{
			iter = m_vSysNotice.erase(iter);
		}
		else
		{
			iter++;
		}
	}

	if (m_vSysNotice.size() == 0)
	{
		return nullptr;
	}
	else
	{
		return &m_vSysNotice.at(0);
	}
}

bool FriendModel::getFriend(const int& id, FRIEND_INFOS& friendInfo)
{
	for (auto iter = m_vfriends.begin(); iter != m_vfriends.end(); iter++)
	{
		if (id == iter->userId)
		{
			friendInfo = *iter;
			return true;
		}
	}
	return false;
}

void FriendModel::setchatMessageDisWidth(float fWidth)
{
	m_fchatMessageDisWidth = fWidth;
}
float FriendModel::getchatMessageDisWidth()
{
	return m_fchatMessageDisWidth;
}

int FriendModel::getChatMessageDisHeight(FRIEND_CHAT_MESSAGE& chatMessage, int& disCharCount)
{
	//建立个富文本测量除了内容外长度
	RichText *pTextLine = RichText::create();
	pTextLine->setVerticalSpace(2.0);
	pTextLine->ignoreContentAdaptWithSize(true);//固定大小

	//time 时间独立显示
	RichElementText *pTextTime = RichElementText::create(0, cocos2d::Color3B(0xec, 0xc2, 0x70), 
		GLubyte(255), FriendModel::getInstance()->getCurTime(chatMessage.strTime) + " ", FONT_FZZHENGHEI, MAX_CHAT_CONTENTFONT_SIZE);
	pTextLine->pushBackElement(pTextTime);

	//时间以后开始换行
	//content
	cocos2d::Color3B color = CHAT_COLOR_ME;
	if (chatMessage.userId != MainModel::getInstance()->getMainParam()->mRoleId)
	{
		color = FriendModel::getInstance()->getChatContentColor(chatMessage.type);
		//type
		RichElementImage* pImgType = RichElementImage::create(0, cocos2d::Color3B(0xff, 0xff, 0xff), 
			GLubyte(255), FriendModel::getInstance()->getChatTypeIcon(chatMessage.type));
		pTextLine->pushBackElement(pImgType);
		//vip
		if (chatMessage.type < FRIEND_CHAT_SYSTEM)
		{
			RichElementImage* pImgVip = RichElementImage::create(0, cocos2d::Color3B(0xff, 0xff, 0xff), 
				GLubyte(255), FriendModel::getInstance()->getChatVipIcon(chatMessage.vipLvl));
			pTextLine->pushBackElement(pImgVip);
		}
	}		
	pTextLine->formatText();	

	//剩余第一行用来显示聊天内容长度为总长度-富文本宽度
	int disCharLength = getchatMessageDisWidth()-pTextLine->getContentSize().width;

	int len = chatMessage.strContent.length();
	int pos = 0;
	int charCount = 0;
	while (pos<len)
	{
		char cValue = chatMessage.strContent.at(pos);
		if (cValue>0 && cValue<127 )
		{
			pos ++;
		}
		else//utf8中文占3个字节
		{
			pos += 3;
		}
		charCount ++;
		if (charCount*MAX_CHAT_CONTENTFONT_SIZE < disCharLength)
		{
			//第一行显示个数
			disCharCount = pos;
		}
		else
		{
			break;
		}
	}	

	//新建聊天内容测量聊天长度
	Text *text = Text::create(chatMessage.strContent + "%",FONT_FZZHENGHEI,MAX_CHAT_CONTENTFONT_SIZE);
	float nLines = 1.0f;
	if (text->getContentSize().width-disCharLength > 0)
	{
		float linesContent = (text->getContentSize().width-disCharLength) / getchatMessageDisWidth();
		nLines += linesContent;
		nLines = ceil(nLines);
	}

	return nLines * MAX_CHAT_CONTENT_LINE_HEIGHT;
}

string FriendModel::getChatTypeIcon(const int& type)
{
	switch (type)
	{
	case FRIEND_CHAT_PUBLIC://综合
		{
			return "Image/UILiaotian/icon/img_zonghe.png";
		}
		break;
	case FRIEND_CHAT_GROUP:	//帮派
		{
			return "Image/UILiaotian/icon/img_shanzhai.png";
		}
		break;
	case FRIEND_CHAT_PRIVATE:	//私聊
		{
			return "Image/UILiaotian/icon/img_siliao.png";
		}
		break;
	case FRIEND_CHAT_SYSTEM:	//系统公告
	case FRIEND_CHAT_SYSTEM2:
		{
			return "Image/UILiaotian/icon/img_xitong.png";
		}
		break;		
	default:
		break;
	}
	return "Image/UILiaotian/icon/img_zonghe.png";
}
string FriendModel::getChatVipIcon(const int& vipLvl)
{
	if (vipLvl >= 0 && vipLvl < 17)
	{
		return "Image/UILiaotian/icon/img_vip_" + _TO_STR(vipLvl) + ".png";
	}
	return "Image/UILiaotian/icon/img_vip_0.png";
}

cocos2d::Color3B FriendModel::getChatContentColor(const int& type)
{
	if (FRIEND_CHAT_PRIVATE == type)
	{
		return CHAT_COLOR_PRIVATE;
	}
	else if (FRIEND_CHAT_GROUP == type)
	{
		return CHAT_COLOR_GROUP;
	}
	else if (FRIEND_CHAT_PUBLIC == type)
	{
		return CHAT_COLOR_PUBLIC;
	}
	else if (FRIEND_CHAT_SYSTEM == type || FRIEND_CHAT_SYSTEM2 == type)
	{
		return CHAT_COLOR_SYSTEM;
	}
	else
	{
		return CHAT_COLOR_ME;
	}
}

string FriendModel::getCurTime(string time)
{
	//2015-11-11 12:12
	int pos = time.find(" ");
	if (pos > 0)
	{
		return time.substr(pos+1, time.length()-pos-1);
	}
	return "";
}

bool FriendModel::getCurTime(string time, int& year, int& month, int& date)
{
	//2015-11-11 12:12
	if (time.length() < 10)
	{
		return false;
	}
	string strTime = time.substr(0, 10);
	year = atoi(strTime.substr(0, 4).c_str());
	month = atoi(strTime.substr(5, 2).c_str());
	date = atoi(strTime.substr(8, 2).c_str());
	return true;
}

int FriendModel::cmpTime(tm* time1, tm* time2)
{
	return (time2->tm_year-time1->tm_year) * 365 
		+ (time2->tm_mon-time1->tm_mon) * 30
		+ (time2->tm_mday-time1->tm_mday);
}


