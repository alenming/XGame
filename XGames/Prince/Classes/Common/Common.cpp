#include "Common.h"
#include "Utils/StringFormat.h"
#include "DataManager/DataManager.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "iconv.h"
#endif

using namespace std;

std::string Common::GBKToUTF8( std::string str )
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	const char * szStr = str.c_str();
	char szBuf[1024];
	int nOutLen = 1024;
	memset(szBuf,0,nOutLen);
	iconv_t cd;
	cd = iconv_open("UTF-8","GB2312");
	if (cd==0) return szBuf;
	int nInlen = strlen(szStr);
	const char **pin = &szStr;
	char *pout = szBuf;
	if (iconv(cd,(const char **)(&szStr),(size_t *)&nInlen,&pout,(size_t *)&nOutLen)==-1)
	{
		szBuf[0] = 0;
	}
	iconv_close(cd);
	return szBuf;
#endif

	return str;
}

std::string Common::UTF8ToGBK( std::string str )
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	const char* szStr = str.c_str();
	char *inbuf=const_cast<char*>(szStr);
	size_t inlen = strlen(inbuf);
	size_t outlen = inlen *4;
	char *outbuf = (char *)malloc(inlen * 4 );
	memset( outbuf, 0, inlen * 4);
	const char *in = inbuf;
	char *out = outbuf;
	iconv_t cd=iconv_open("GB2312","UTF-8");
	iconv(cd,&in,&inlen,&out,&outlen);
	iconv_close(cd);
	string ret(outbuf);
	free(outbuf);
	return ret;
#endif

	return str;
}


float getCurrTime()
{	
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*1000 + tv.tv_usec / 1000.0f;
}

long secondNow()  
{ 
	timeval nowTimeval;
	gettimeofday(&nowTimeval, NULL);
	return nowTimeval.tv_sec;
}

struct tm* getNowTime()
{	
	struct tm* nowtime;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	timeval nowTimeval;
	gettimeofday(&nowTimeval, NULL);
	nowtime = localtime(&nowTimeval.tv_sec);
#else
	time_t nowTimeval;
	time(&nowTimeval);
	nowtime = localtime(&nowTimeval);
#endif
	nowtime->tm_year += 1900;		
	nowtime->tm_mon += 1;
	return nowtime;
}

string formatTime(int seconds, char separator)
{
	if (seconds < 0)
	{
		return string("");
	}

	char time[16] = "";
	sprintf(time, "%02d%c%02d%c%02d", seconds/3600, separator, (seconds%3600)/60, separator, ((seconds%3600)%60)%60);
	return string(time);
}

// 获取当前日期字符串YYYYMMDD (added by Phil 05/11/2015 @zcjoy)
std::string getCurrDate()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

	struct timeval now;
	struct tm* time;

	gettimeofday(&now, NULL);
	time = localtime(&now.tv_sec);
	int year = time->tm_year + 1900;
	CCLOG("year = %d",year);

	char date[32] = {0};
	sprintf(date, "%d%02d%02d",(int)time->tm_year + 1900,(int)time->tm_mon + 1,(int)time->tm_mday);
	CCLOG("%s",date);
	return StringUtils::format("%s",date);

#endif

#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )

	struct tm* tm;
	time_t timep;
	time(&timep);

	tm = localtime(&timep);
	char date[32] = {0};
	sprintf(date, "%d%02d%02d",(int)tm->tm_year + 1900,(int)tm->tm_mon + 1,(int)tm->tm_mday);
	CCLOG("%s", date);
	return StringUtils::format("%s", date);

#endif

}

int getWeekday()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

	struct timeval now;
	struct tm* time;

	gettimeofday(&now, NULL);
	time = localtime(&now.tv_sec);
	return (time->tm_wday == 0) ? 7 : time->tm_wday;

#endif

#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )

	struct tm* tm;
	time_t timep;
	time(&timep);

	tm = localtime(&timep);
	char date[32] = {0};
	return (tm->tm_wday == 0) ? 7 : tm->tm_wday;

#endif
}

std::string convertCurrDateBycSec(time_t const timer)
{
	struct tm* time;
	time = localtime(&timer);
	string strMonth = TO_STR(time->tm_mon+1);
	string strDay = "";
	string strHours = "";
	string strMins = "";

	if(time->tm_mday <= 9)
	{
		strDay = _TO_STR(0) + _TO_STR(time->tm_mday);
	}
	else
	{
		strDay = _TO_STR(time->tm_mday);
	}

	if(time->tm_hour <= 9)
	{
		strHours = _TO_STR(0) + _TO_STR(time->tm_hour);
	}
	else
	{
		strHours = _TO_STR(time->tm_hour);
	}

	if(time->tm_min <= 9)
	{
		strMins = _TO_STR(0) + _TO_STR(time->tm_min);
	}
	else
	{
		strMins = _TO_STR(time->tm_min);
	}

	return strMonth + DataManager::getInstance()->searchCommonTexdtById(10189) + 
		   strDay + DataManager::getInstance()->searchCommonTexdtById(10190) + strHours + ":" + strMins;
	//return STR_UTF8(strMonth + "月" + strDay + "日" + strHours + ":" + strMins);
}

void setSpriteGray(cocos2d::Node* sprite, const bool isGray)
{
	if (sprite != nullptr)
	{			
		GLProgram * p = nullptr;
		if (isGray)
		{			
			//p->initWithFilenames("shader/gray.vsh", "shader/gray.fsh");
			p = GLProgram::createWithByteArrays("attribute vec4 a_position;\
												attribute vec2 a_texCoord;\
												attribute vec4 a_color;\
												varying vec4 v_fragmentColor;\
												varying vec2 v_texCoord;\
												void main()\
												{\
												gl_Position = CC_PMatrix * a_position;\
												v_fragmentColor = a_color;\
												v_texCoord = a_texCoord;\
												}", 
												"varying vec4 v_fragmentColor;\
												varying vec2 v_texCoord;\
												void main()\
												{\
												vec4 v_orColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);\
												float gray = dot(v_orColor.rgb, vec3(0.299, 0.587, 0.114));\
												gl_FragColor = vec4(gray, gray, gray, v_orColor.a);\
												}");		
		}
		else
		{		
			//p->initWithFilenames("shader/gray.vsh", "shader/grayReset.fsh");
			p = GLProgram::createWithByteArrays("attribute vec4 a_position;\
												attribute vec2 a_texCoord;\
												attribute vec4 a_color;\
												varying vec4 v_fragmentColor;\
												varying vec2 v_texCoord;\
												void main()\
												{\
												gl_Position = CC_PMatrix * a_position;\
												v_fragmentColor = a_color;\
												v_texCoord = a_texCoord;\
												}", 
												"varying vec4 v_fragmentColor;\
												varying vec2 v_texCoord;\
												void main()\
												{\
												gl_FragColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);\
												}");				
		}
		p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
		p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
		p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
		p->link();
		p->updateUniforms();
		sprite->setGLProgram(p);
	}
}

void setTextAddStroke(Text* text,const Color3B &lineColor3,float lineWidth)  
{  
	const std::string& textContent = text->getString();
	const std::string& fontName = text->getFontName();
	bool ignoreSize = text->isIgnoreContentAdaptWithSize();
	int fontSize = text->getFontSize();
	Size ContentSize = text->getContentSize();
	Size CustomSize = text->getCustomSize();

	TextHAlignment halignment = text->getTextHorizontalAlignment();
	TextVAlignment valignment = text->getTextVerticalAlignment();

	//删除防止重复添加
	if (text->getChildByName("CHILD_UP"))
	{
		text->removeChildByName("CHILD_UP");
	}
	if (text->getChildByName("CHILD_DOWN"))
	{
		text->removeChildByName("CHILD_DOWN");
	}
	if (text->getChildByName("CHILD_LEFT"))
	{
		text->removeChildByName("CHILD_LEFT");
	}
	if (text->getChildByName("CHILD_RIGHT"))
	{
		text->removeChildByName("CHILD_RIGHT");
	}

	/* 描边Text 上 */
	Text* up = Text::create(textContent, fontName, fontSize); 
	if (ignoreSize)
	{
		up->setContentSize(ContentSize);
	}
	else
	{
		up->setContentSize(CustomSize);
	}
	up->ignoreContentAdaptWithSize(ignoreSize);
	up->setColor(lineColor3);  
	up->setPosition(Vec2(text->getContentSize().width*0.5, text->getContentSize().height*0.5+lineWidth));
	up->setTextHorizontalAlignment(halignment);
	up->setTextVerticalAlignment(valignment);
	text->addChild(up,-1,"CHILD_UP");  

	/* 描边Text 下 */
	Text* down = Text::create(textContent, fontName, fontSize);  
	if (ignoreSize)
	{
		down->setContentSize(ContentSize);
	}
	else
	{
		down->setContentSize(CustomSize);
	}
	down->ignoreContentAdaptWithSize(ignoreSize);
	down->setColor(lineColor3);  
	down->setPosition(Vec2(text->getContentSize().width*0.5, text->getContentSize().height*0.5-lineWidth)); 
	down->setTextHorizontalAlignment(halignment);
	down->setTextVerticalAlignment(valignment);
	text->addChild(down,-1,"CHILD_DOWN");  

	/* 描边Text 左 */ 
	Text* left = Text::create(textContent, fontName, fontSize); 
	if (ignoreSize)
	{
		left->setContentSize(ContentSize);
	}
	else
	{
		left->setContentSize(CustomSize);
	}
	left->ignoreContentAdaptWithSize(ignoreSize);
	left->setPosition(Vec2(text->getContentSize().width*0.5-lineWidth, text->getContentSize().height*0.5));  
	left->setColor(lineColor3);
	left->setTextHorizontalAlignment(halignment);
	left->setTextVerticalAlignment(valignment);
	text->addChild(left,-1,"CHILD_LEFT");  

	/* 描边Text 右 */
	Text* right = Text::create(textContent, fontName, fontSize);  
	if (ignoreSize)
	{
		right->setContentSize(ContentSize);
	}
	else
	{
		right->setContentSize(CustomSize);
	}
	right->ignoreContentAdaptWithSize(ignoreSize);
	right->setColor(lineColor3);  
	right->setPosition(Vec2(text->getContentSize().width*0.5+lineWidth,text->getContentSize().height*0.5));
	right->setTextHorizontalAlignment(halignment);
	right->setTextVerticalAlignment(valignment);
	text->addChild(right,-1,"CHILD_RIGHT");  
} 

void setImageGray(cocos2d::ui::ImageView* pImg, const bool isGray)
{
	if (pImg != nullptr)
	{		
		Node* sprite = pImg->getVirtualRenderer();
		setSpriteGray(sprite, isGray);
	}
}