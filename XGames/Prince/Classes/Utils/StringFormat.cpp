#include "StringFormat.h"
#include <sstream>
#include <algorithm>
#include "Common/Common.h"
#include "DataManager/DataManager.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)  
#include <iostream>  
#include <string>  
#include <regex>  
#include <tchar.h>
#endif  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID||CC_TARGET_PLATFORM == CC_PLATFORM_IOS)  
#include <regex.h>  
#endif

void StringFormat::parseCsvStringMap(const string strSrc, map<int, int>& mapDst)
{
	if (strSrc.empty() || strSrc.compare("-1") == 0)
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), '|');
	int nStart = 0;
	int nEnd = strSrc.find_first_of('|', 0);
	for (int i = 0; i <= nCount; i++)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		string strTmp = strSrc.substr(nStart, nEnd-nStart);

		// 在子串中继续分解单条数据;
		int nIndex1 = strTmp.find_first_of(',', 0);
		string strTmp1 = strTmp.substr(0, nIndex1);
		int nKey = atoi(strTmp1.c_str());
		string strTmp2 = strTmp.erase(0, nIndex1 + 1);
		int nValue = atoi(strTmp2.c_str());
		mapDst.insert(pair<int, int>(nKey, nValue));

		nStart = nEnd+1;
		nEnd = strSrc.find_first_of('|', nStart);

	}
}

void StringFormat::parseCsvStringVec(const string strSrc, vector<vector<int> >& vecDst)
{
	if (strSrc.empty() || strSrc.compare("-1") == 0)
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), '|');
	int nStart = 0;
	int nEnd = strSrc.find_first_of('|', 0);
	for (int i = 0; i <= nCount; ++i)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		string strTmp = strSrc.substr(nStart, nEnd-nStart);

		// 在子串中继续分解单条数据;
		vector<int>  vecSingle;

		int nSingleCount = count(strTmp.begin(), strTmp.end(), ',');
		for (unsigned int k = 0; k <= nSingleCount; ++k)
		{
			int nIndex1 = strTmp.find_first_of(',', 0);
			string strTmp1 = strTmp.substr(0, nIndex1);
			vecSingle.push_back(atoi(strTmp1.c_str()));
			strTmp = strTmp.erase(0, nIndex1+1);
		}

		/*
		int nIndex1 = strTmp.find_first_of(',', 0);
		string strTmp1 = strTmp.substr(0, nIndex1);
		vecSingle.push_back(atoi(strTmp1.c_str()));

		int nIndex2 = strTmp.find_first_of(',', nIndex1+1);
		string strTmp2 = strTmp.substr(nIndex1+1, nIndex2-nIndex1-1);
		vecSingle.push_back(atoi(strTmp2.c_str()));

		string strTmp3 = strTmp.erase(0, nIndex2+1);
		vecSingle.push_back(atoi(strTmp3.c_str()));
		*/

		vecDst.push_back(vecSingle);

		nStart = nEnd+1;
		nEnd = strSrc.find_first_of('|', nStart);

	}
}

void StringFormat::parseCsvStringVec(const string strSrc, vector<int>& vecDst)
{
	if (strSrc.empty() || strSrc.compare("-1") == 0)
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), '|');
	int nStart = 0;
	int nEnd = strSrc.find_first_of('|', 0);
	for (int i = 0; i <= nCount; i++)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		string strTmp = strSrc.substr(nStart, nEnd-nStart);
		vecDst.push_back(atoi(strTmp.c_str()));

		nStart = nEnd+1;
		nEnd = strSrc.find_first_of('|', nStart);

	}
}


void StringFormat::parseCsvStringVecByDu(const string strSrc, vector<int>& vecDst)
{
	if (strSrc.empty())
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), ',');
	int nStart = 0;
	int nEnd = strSrc.find_first_of(',', 0);
	for (int i = 0; i <= nCount; i++)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		string strTmp = strSrc.substr(nStart, nEnd-nStart);
		vecDst.push_back(atoi(strTmp.c_str()));

		nStart = nEnd+1;
		nEnd = strSrc.find_first_of(',', nStart);
	}
}

void StringFormat::parseCsvStringVecBySemi(const string strSrc, vector<int>& vecDst)
{
	if (strSrc.empty())
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), ';');
	int nStart = 0;
	int nEnd = strSrc.find_first_of(';', 0);
	for (int i = 0; i <= nCount; i++)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		string strTmp = strSrc.substr(nStart, nEnd-nStart);
		vecDst.push_back(atoi(strTmp.c_str()));

		nStart = nEnd+1;
		nEnd = strSrc.find_first_of(';', nStart);
	}
}

void StringFormat::parseCsvFloatVec(const string strSrc, vector<float>& vecDst)
{
	if (strSrc.empty())
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), '|');
	int nStart = 0;
	int nEnd = strSrc.find_first_of('|', 0);
	for (int i = 0; i <= nCount; i++)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		string strTmp = strSrc.substr(nStart, nEnd-nStart);
		vecDst.push_back(atof(strTmp.c_str()));

		nStart = nEnd+1;
		nEnd = strSrc.find_first_of('|', nStart);
	}
}

void StringFormat::parseCsvStringVec(const string strSrc, vector<string>& vecDst, char separetor /*= '|'*/)
{
	if (strSrc.empty() || strSrc.compare("-1") == 0)
	{
		return;
	}

	int nCount = count(strSrc.begin(), strSrc.end(), separetor);
	int nStart = 0;
	int nEnd = strSrc.find_first_of(separetor, 0);
	for (int i = 0; i <= nCount; i++)
	{
		if (-1 == nEnd)
		{
			nEnd = strSrc.length();
		}

		if (nStart >= nEnd)
		{
			break;
		}

		// 分割子串; 
		string strTmp = strSrc.substr(nStart, nEnd-nStart);
		vecDst.push_back(strTmp);

		nStart = nEnd+1;
		nEnd = strSrc.find_first_of(separetor, nStart);

	}
}

Color3B StringFormat::parseCsvStringToColor(const string& strSrc)
{
	int color = hex2dec(strSrc.c_str());
	Color3B color3B((color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
	return color3B;
}

string StringFormat::intToStr(int i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}

string StringFormat::floatToStr(float f)
{
	string strIni = "%0.2lf";
	char szNum[100];
	memset(&szNum, 0, 100);
	sprintf(szNum, strIni.c_str(), f);
	return szNum;
}

int StringFormat::c2i(char ch)
{  
	// 如果是数字，则用数字的ASCII码减去48, 如果ch = '2' ,则 '2' - 48 = 2  
	if(isdigit(ch))  
		return ch - 48;  
  
	// 如果是字母，但不是A~F,a~f则返回  
	if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )  
		return -1;  
  
	// 如果是大写字母，则用数字的ASCII码减去55, 如果ch = 'A' ,则 'A' - 55 = 10  
	// 如果是小写字母，则用数字的ASCII码减去87, 如果ch = 'a' ,则 'a' - 87 = 10  
	if(isalpha(ch))  
		return isupper(ch) ? ch - 55 : ch - 87;  
  
	return -1;  
}  
  
/* 
 * 功能：将十六进制字符串转换为整型(int)数值 
 * */  
int StringFormat::hex2dec(const char *hex)
{  
	int len;  
	int num = 0;  
	int temp;  
	int bits;
	int i;  
          
	len = strlen(hex);  
  
	for (i=0, temp=0; i<len; i++, temp=0)  
	{  
		// 第一次：i=0, *(hex + i) = *(hex + 0) = '1', 即temp = 1  
		// 第二次：i=1, *(hex + i) = *(hex + 1) = 'd', 即temp = 13  
		// 第三次：i=2, *(hex + i) = *(hex + 2) = 'd', 即temp = 14  
		temp = c2i( *(hex + i) );
		// 总共3位，一个16进制位用 4 bit保存  
		// 第一次：'1'为最高位，所以temp左移 (len - i -1) * 4 = 2 * 4 = 8 位  
		// 第二次：'d'为次高位，所以temp左移 (len - i -1) * 4 = 1 * 4 = 4 位  
		// 第三次：'e'为最低位，所以temp左移 (len - i -1) * 4 = 0 * 4 = 0 位  
		bits = (len - i - 1) * 4;  
		temp = temp << bits;  
  
		// 此处也可以用 num += temp;进行累加  
		num = num | temp;  
	}  
  
	// 返回结果  
	return num;  
} 

string StringFormat::formatNumber(int num, bool millionFomat)
{
	int base = 10000;
	int value = millionFomat ? 10000 * 100 : 100000;
	ostringstream oss;
	if (num >= value)
	{
		oss << (num / base) << DataManager::getInstance()->searchCommonTexdtById(20013);
	}
	else
	{
		oss << num;
	}
	return oss.str();
}

void StringFormat::split(vector<string>& dest, string str, string pattern)  
{  
	string::size_type pos;  
	str+=pattern;//扩展字符串以方便操作  
	int size=str.size();  

	for(int i=0; i<size; i++)  
	{  
		pos=str.find(pattern,i);  
		if(pos<size)  
		{  
			string s=str.substr(i,pos-i);  
			dest.push_back(s);  
			i=pos+pattern.size()-1;  
		}  
	}
}

string StringFormat::longNumChangeToCh(long num)
{
	string str="";
	for(int i=0;num!=0;i++)
	{
		int among=0;
		among = num%10000;
		num/=10000;
		str = intNumChangeToCh(among) + str;
		if(i==0 && num!=0)
		{
			string wan = DataManager::getInstance()->searchCommonTexdtById(20013);
			str = wan + str;
		}
		if(i==1 && num!=0)
		{
			string yi = DataManager::getInstance()->searchCommonTexdtById(20014);
			str = yi + str;
		}
	}
	return str;
}

int StringFormat::getDigit(int num)
{
	int N=0;
	while(num)
	{
		N++;
		num/=10;
	}
	return N;
}

string StringFormat::intNumChangeToCh(int num)
{
	string str="";
	//"十"有点特殊，不转换"一十"而是转换成“十”
	if(num == 10)
	{
		str = DataManager::getInstance()->searchCommonTexdtById(20025);
		return str;
	}
	int num_1 = num;
	int ws = getDigit(num);
	for(int i=0; i<ws; i++)
	{
		num_1 = num_1%10;
		switch(num_1)
		{
		case 0:str= DataManager::getInstance()->searchCommonTexdtById(20015)+str;break;
		case 1:str= DataManager::getInstance()->searchCommonTexdtById(20016)+str;break;
		case 2:str= DataManager::getInstance()->searchCommonTexdtById(20017)+str;break;
		case 3:str= DataManager::getInstance()->searchCommonTexdtById(20018)+str;break;
		case 4:str= DataManager::getInstance()->searchCommonTexdtById(20019)+str;break;
		case 5:str= DataManager::getInstance()->searchCommonTexdtById(20020)+str;break;
		case 6:str= DataManager::getInstance()->searchCommonTexdtById(20021)+str;break;
		case 7:str= DataManager::getInstance()->searchCommonTexdtById(20022)+str;break;
		case 8:str= DataManager::getInstance()->searchCommonTexdtById(20023)+str;break;
		case 9:str= DataManager::getInstance()->searchCommonTexdtById(20024)+str;break;
		}
		if(i==0 && i<ws-1)
		{
			str= DataManager::getInstance()->searchCommonTexdtById(20025) + str;
			num_1 = num/10;
		}
		if(i==1 && i<ws-1)
		{
			str= DataManager::getInstance()->searchCommonTexdtById(20026)+str;
			num_1 = num/100;
		}
		if(i==2&&i<ws-1)
		{
			str= DataManager::getInstance()->searchCommonTexdtById(20027)+str;
			num_1 = num/1000;
		}
	}
	return str;
}

bool StringFormat::machString(const string& context)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	//验证中文、英文、数字但不包括下划线等符号  
	string str = STR_ANSI(context);
	int nLen = (int)str.length();
	std::wstring wstr;
	wstr.resize(nLen,L'l');
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)str.c_str(),nLen,(LPWSTR)wstr.c_str(),nLen);

	std::wsmatch ms; 
	std::wstring regString(_T("^[\u4E00-\u9FA5A-Za-z0-9]+$"));  
	std::regex_constants::syntax_option_type fl = std::regex_constants::icase;  
	std::wregex regExpress(regString, fl); 
	//匹配验证  
	return  std::regex_match(wstr, ms, regExpress);
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID||CC_TARGET_PLATFORM == CC_PLATFORM_IOS)  
	char ss[100] = {};  
	string str = STR_ANSI(context);
	sprintf(ss,"%s",str.c_str());  
	regmatch_t pmatch[4];  
	regex_t match_regex;  

	regcomp( &match_regex,  
		"^[\u4E00-\u9FA5A-Za-z0-9]+$",  
		REG_EXTENDED|REG_NOSUB|REG_ICASE  );  
	if ( regexec( &match_regex, ss, 4, pmatch, 0 ) != 0 )  
	{  
		regfree( &match_regex );  
		return false;  
	}  
	regfree( &match_regex );  
	return true;
	
#endif
}

bool StringFormat::machGroupId(const char* context)
{
	size_t len = strlen(context);
	while(len > 0) {
		if (*context < '0' || *context > '9') {
			return false;
		}
		context++;
		len--;
	}
	return true; 
}

bool StringFormat::machString_E_N_UNDERLINE(const char* context)
{
	/*
	const std::regex pattern("[]A-Za-z0-9_");
	return std::regex_match(context, pattern);
	*/

	//a~z, A~Z, 0~9, _
	size_t len = strlen(context);
	while(len > 0) {
		if ((*context == '_') || (*context >= '0' && *context <= '9')
			|| (*context >= 'A' && *context <= 'Z') || (*context >= 'a' && *context <= 'z')) 
		{
			context++;
			len--;
		}
		else
		{
			return false;
		}
	}
	return true; 
}


std::string StringFormat::convertSecToFormatTime(int second)
{
	if (second < 0)
	{
		second = 0;
	}
	string strResTime = "";
	string day;
	string ht;
	string mint;
	string sect;
	int days = second/(24*3600);
	day = _TO_STR(days);

	int st = second%(24*3600);
	int h = st/3600;
	ht = _TO_STR(h);

	int min = (st%3600)/60;
	mint = _TO_STR(min);

	int sec = st%60;
	sect = _TO_STR(sec);
	auto miao = DataManager::getInstance()->searchCommonTexdtById(10153);
	auto fen = DataManager::getInstance()->searchCommonTexdtById(10152);
	auto xiaoshi = DataManager::getInstance()->searchCommonTexdtById(20030);
	auto tian = DataManager::getInstance()->searchCommonTexdtById(10150);
	if (days > 0)
	{
		strResTime = day + tian + ht + xiaoshi + mint + fen + sect +miao;
	}
	else
	{
		strResTime = ht + xiaoshi + mint + fen + sect + miao;
	}
	return strResTime;
}

int StringFormat::getStringLenth(const string str)
{
	int LetterCount = 0; //字母或英文个数
	int fellowCount = 0; //汉字个数
	int lenth = 0;
	while (lenth < strlen(str.c_str()))
	{
		char cValue = str.at(lenth);
		if (cValue > 0 && cValue < 127 )
		{
			lenth ++;
			LetterCount++;
		}
		else//utf8中文占3个字符
		{
			lenth += 3;
			fellowCount++;
			if (lenth > str.length()+1)
			{
				lenth = str.length()+1;
				break;
			}	
		}
	}

	return fellowCount*2+LetterCount;
}
