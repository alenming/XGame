#ifndef MailDef_h__
#define MailDef_h__

#include "CocosGUI.h"
#include "cocos2d.h"
#include <string>
#include "Communication/Socket/Util.h"
#include "MainModule/MainModel.h"
#include "../ToolModule/UIToolHeader.h"
#include "Utils/StringFormat.h"
#include "Widget/ActionCreator.h"
#include "Temp/PopReward.h"

const int MAX_MAIL_TOOLS_COUNT = 5;


USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;


enum MAIL_TYPE
{
	MAIL_TYPE_NONE,
	MAIL_TYPE_GET_MAIL, //获取邮件
	MAIL_TYPE_GET_ONE_PRIZE, //获取单个奖励
	MAIL_TYPE_DEL_ONE_MAIL, //删除单个
	MAIL_TYPE_DEL_ALL_MAIL //删除所有
};

typedef struct MAIL_INFO_Accessory
{
	int id;
	int amount;
}MAIL_INFO_ACCESSORY;
typedef vector<MAIL_INFO_ACCESSORY> MAIL_INFO_ACCESSORY_LIST;

enum MAIL_STAT
{
	MAIL_STAT_NEW,		//0-未读
	MAIL_STAT_READED,	//1-已读
	MAIL_STAT_GOT		//2-已领取
};
typedef struct MAIL_INFO
{
	int id;
	int type;
	int nLevel;				//等级
	int stat;				//stat 0-未读，1-已读，2-已领取
	int nSaveDates;			//保留时间
	string sender;          //发送人
	string strTitle;        //标题
	string strTime;         //发送时间 
	string strContent;	    //发送内容

	bool  isInit;  //是否填充数据
	Widget *layer;

	MAIL_INFO_ACCESSORY_LIST vAccessory;
}MAIL_INFOS;
typedef deque<MAIL_INFOS> MAIL_INFOS_DEQUE;

class CustomScrollBar;
//界面
struct  UI_Mail_Mail
{
	Layout		*pRoot;
	Layout		*pRootItem;

	ImageView   *ImageDi;
	Button		*pBtnBack;//返回
	
	ImageView	*pLayDetail; //详情
	Text        *mailTitle; // 标题
	Text        *mailSrc; // 发件人
	Layout		*pnlTools; //奖励
	ListView    *ListView_Item;
	Text        *mailDesc; //邮件内容
	

	Text        *mailCount; //邮件数量 
	Button		*pBtnGetRewardNow;//一键领取
	Button		*pBtnDeleteNow;//一键删除
	Button		*pBtnGetReward;//领取	
	Button		*pBtnReturn;//返回
	Button		*pBtnDelete;//删除
	ImageView   *mailTips; //提示
	ImageView   *noMail; //没有邮件
	ListView    *mailList;
};


#endif
