#ifndef LocalData_h__
#define LocalData_h__

#include "cocos2d.h"
#include <string>

#define GetIntegerFromXML  CLocalData::getInstance()->getIntegerForKeyValue
#define SetIntegerToXML  CLocalData::getInstance()->setIntegerForKeyValue
#define GetStringFromXML  CLocalData::getInstance()->getStringForKeyValue
#define SetStringToXML  CLocalData::getInstance()->setStringForKeyValue
#define GetBoolFromXML  CLocalData::getInstance()->getBoolForKeyValue
#define SetBoolToXML  CLocalData::getInstance()->setBoolForKeyValue
#define GetFloatFromXML  CLocalData::getInstance()->getFloatForKeyValue
#define SetFloatToXML  CLocalData::getInstance()->setFloatForKeyValue

class CLocalData
{
public:

	static const CLocalData* getInstance();

	bool				getBoolForKeyValue(int nKey) const;												//读bool值
	bool				getBoolForKeyValue(int nKey, bool defaultValue) const;							//读带默认参数bool值

	int					getIntegerForKeyValue(int nKey) const;											//读int值
	int					getIntegerForKeyValue(int nKey, int defaultValue) const;							//读带默认参数int值
		
	double				getFloatForKeyValue(int nKey) const;
	double				getFloatForKeyValue(int nKey, double defaultValue) const;

	std::string			getStringForKeyValue(int nKey) const;                                             //读string值
	std::string			getStringForKeyValue(int nKey, const std::string & defaultValue) const;           //读带默认参数string值

	void				setBoolForKeyValue(int nKey, bool value) const;									//写bool值
	void				setIntegerForKeyValue(int nKey, int value) const;									//写int值
	void				setFloatForKeyValue(int nKey, float value) const;
	void				setStringForKeyValue(int nKey, const std::string & value) const;					//写string值

	std::string			EncryptData(const char* szPass) const;											//字符串加密
	std::string			CrevasseData(const char* szEncryPass) const;										//字符串解密
};

#endif // LocalData_h__
