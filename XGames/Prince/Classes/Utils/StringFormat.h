#ifndef __STRING_FORMAT_H__
#define __STRING_FORMAT_H__

#include <string>
#include <vector>
#include <map>
#include "cocos2d.h"
#include <regex>  

#ifndef _TO_STR
	#define _TO_STR(val) StringFormat::intToStr(val)
	#define TO_STR _TO_STR
#endif
#ifndef _F_TO_STR
#define _F_TO_STR(val) StringFormat::floatToStr(val)
#endif

const int NICK_NAME_LENGTH_LIMIT = 10;

using namespace std;
USING_NS_CC;

class StringFormat
{

public:

	// 处理csv文件中带有分隔符的字符串;
	static void parseCsvStringVec(const string strSrc, vector<int>& vecDst);
	static void parseCsvStringVec(const string strSrc, vector<string>& vecDst, char separetor = '|');
	static void parseCsvStringVec(const string strSrc, vector<vector<int> >& vecDst);
	static void parseCsvStringMap(const string strSrc, map<int, int>& mapDst);

	//处理csv文件中带“，”分割符
	static void parseCsvStringVecByDu(const string strSrc, vector<int>& vecDst);
	//处理csv文件中带";"分隔符
	static void parseCsvStringVecBySemi(const string strSrc, vector<int>& vecDst);
	//处理float数据
	static void parseCsvFloatVec(const string strSrc, vector<float>& vecDst);

	static Color3B parseCsvStringToColor(const string& strSrc);

	//将整型转换成字符串
	static string intToStr(int i);
	static string floatToStr(float f);

	static int c2i(char ch);
	static int hex2dec(const char *hex);

	//将整型数字换成中文
	static string longNumChangeToCh(long num);
	static string intNumChangeToCh(int num);

	//数字格式化(十万或百万以上以万为单位显示)
	static string formatNumber(int num, bool millionFomat = true);

	//字符串分割
	static void split(vector<string>& dest, string str, string pattern);

	//验证字符串是否符合（中文、英文、数字但不包括下划线等符号）
	static bool machString(const string& context );

	//验证字符串是否符合（数字）
	static bool machGroupId(const char* context);

	//验证字符串是否符合(仅能包含英文，数字，下划线)
	static bool machString_E_N_UNDERLINE(const char* context);

	static string convertSecToFormatTime(int second); //把秒转换成（X天X小时X分）

	static int getStringLenth(const string str);

private:
	static int getDigit(int num);
};


#endif //__STRING_FORMAT_H__