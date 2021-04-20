#ifndef Common_h__
#define Common_h__

#include <string>
#include "cocos2d.h"
#include "CocosGUI.h"

#define  WIN32_VERSION_CODE		"1"
#define  WIN32_VERSION_NAME		"PC"

#define LOG_TIME(a, b, c) CCLOG("-----\n----- time %s: %f ms\n-----", (c), (b)-(a))

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace std;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	#define STR_UTF8(str) Common::GBKToUTF8(str)
	#define STR_ANSI(str) Common::UTF8ToGBK(str)
#else
	#define STR_UTF8(str) string(str)
	#define STR_ANSI(str) string(str)
#endif

float getCurrTime();
long secondNow();
struct tm* getNowTime();

// 将时间(s)格式化为00:00:00格式 (added by Phil 10/13/2015 @zcjoy);
string formatTime(int seconds, char separator = ':');

// 获取当前日期字符串YYYYMMDD (added by Phil 05/11/2015 @zcjoy)
std::string getCurrDate();

// 转化当前日期字符串YYYYMMDD (added by sonic 06/1/2016 @zcjoy)
std::string convertCurrDateBycSec(time_t const timer);

// 获取星期数字(1-7);
int getWeekday();

//设置变灰
void setSpriteGray(cocos2d::Node* sprite, const bool isGray);
void setImageGray(cocos2d::ui::ImageView* pImg, const bool isGray);

//设置字体描边
void setTextAddStroke(Text* text,const Color3B &lineColor3,float lineWidth);

class Common
{
public:
	static std::string GBKToUTF8(std::string str);	//此函数只用在win32
	static std::string UTF8ToGBK(std::string str);	//此函数只用在win32
};

#endif // Common_h__
