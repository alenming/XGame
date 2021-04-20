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

	// ����csv�ļ��д��зָ������ַ���;
	static void parseCsvStringVec(const string strSrc, vector<int>& vecDst);
	static void parseCsvStringVec(const string strSrc, vector<string>& vecDst, char separetor = '|');
	static void parseCsvStringVec(const string strSrc, vector<vector<int> >& vecDst);
	static void parseCsvStringMap(const string strSrc, map<int, int>& mapDst);

	//����csv�ļ��д��������ָ��
	static void parseCsvStringVecByDu(const string strSrc, vector<int>& vecDst);
	//����csv�ļ��д�";"�ָ���
	static void parseCsvStringVecBySemi(const string strSrc, vector<int>& vecDst);
	//����float����
	static void parseCsvFloatVec(const string strSrc, vector<float>& vecDst);

	static Color3B parseCsvStringToColor(const string& strSrc);

	//������ת�����ַ���
	static string intToStr(int i);
	static string floatToStr(float f);

	static int c2i(char ch);
	static int hex2dec(const char *hex);

	//���������ֻ�������
	static string longNumChangeToCh(long num);
	static string intNumChangeToCh(int num);

	//���ָ�ʽ��(ʮ��������������Ϊ��λ��ʾ)
	static string formatNumber(int num, bool millionFomat = true);

	//�ַ����ָ�
	static void split(vector<string>& dest, string str, string pattern);

	//��֤�ַ����Ƿ���ϣ����ġ�Ӣ�ġ����ֵ��������»��ߵȷ��ţ�
	static bool machString(const string& context );

	//��֤�ַ����Ƿ���ϣ����֣�
	static bool machGroupId(const char* context);

	//��֤�ַ����Ƿ����(���ܰ���Ӣ�ģ����֣��»���)
	static bool machString_E_N_UNDERLINE(const char* context);

	static string convertSecToFormatTime(int second); //����ת���ɣ�X��XСʱX�֣�

	static int getStringLenth(const string str);

private:
	static int getDigit(int num);
};


#endif //__STRING_FORMAT_H__