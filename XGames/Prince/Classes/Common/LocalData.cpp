#include "LocalData.h"
#include "TypeDef.h"

#define  ENCRYPT_KEY_LENGTH 1

using namespace std;
using namespace cocos2d;

const CLocalData* CLocalData::getInstance()
{
	static CLocalData localData;
	return &localData;
}

//存储
void CLocalData::setBoolForKeyValue(int nKey, bool value) const
{
	char buffer[128]; 
	sprintf(buffer, "%s%d", "data_",nKey); 
	//CCLOG("----------save XML data"); 
	//CCLOG("before: index: Data%i, value: %d  ",nKey, value); 
	string sKey =EncryptData(value?"1": "0"); 
	//CCLOG("after: index: Data%i, value: %s  ",nKey, sKey.c_str()); 
	UserDefault::getInstance()->setStringForKey(buffer, sKey); 
	//UserDefault::getInstance()->flush();

	getInstance()->getStringForKeyValue(0, "");
}

void CLocalData::setIntegerForKeyValue(int nKey, int value) const
{
    char buffer[128];
    sprintf(buffer, "%s%d", "data_", nKey);
    //CCLOG("----------save XML data");
    //CCLOG("before: index: Data%i, value: %d  ", nKey, value);

	char szValue[128];
	//itoa(value, szValue, 10);
    sprintf(szValue, "%d", value);
	std::string sKey = EncryptData(szValue);
	//CCLOG("after: index: Data%i, value: %s  ", nKey, sKey.c_str());
	UserDefault::getInstance()->setStringForKey(buffer, sKey);
	//UserDefault::getInstance()->flush();
}

void CLocalData::setStringForKeyValue(int nKey, const std::string & sValue) const
{
	char buffer[128]; 
	sprintf(buffer, "%s%d", "data_",nKey); 
	//CCLOG("----------save XML data"); 
	//CCLOG("before: index: Data%i, value: %s  ", nKey, sValue.c_str()); 
	string sKey =EncryptData(sValue.c_str()); 
	//CCLOG("after: index: Data%i, value: %s  ",nKey, sKey.c_str()); 
	UserDefault::getInstance()->setStringForKey(buffer, sKey); 
	//UserDefault::getInstance()->flush();
}


//读取
bool CLocalData::getBoolForKeyValue(int nKey) const
{
	return getBoolForKeyValue(nKey, false);
}

bool CLocalData::getBoolForKeyValue(int nKey, bool defaultValue) const
{
	//CCLOG("----------load XML data");
	char buffer[128];
	sprintf(buffer, "%s%d", "data_",nKey);
    
	string s = UserDefault::getInstance()->getStringForKey(buffer);
	//CCLOG("before decryp: %s  ",s.c_str());
	string parseKey = CrevasseData(s.c_str());
	//CCLOG("after decryp: %s  ",parseKey.c_str());
	return (parseKey.length()!=0? (bool)atoi(parseKey.c_str()): defaultValue);
}

int CLocalData::getIntegerForKeyValue(int nKey) const
{
	return getIntegerForKeyValue(nKey, 0);
}

int CLocalData::getIntegerForKeyValue(int nKey, int defaultValue) const
{
	//CCLOG("----------load XML data");
	char buffer[128];
	sprintf(buffer, "%s%d", "data_", nKey);
    
	string s = UserDefault::getInstance()->getStringForKey(buffer);
	//CCLOG("before decryp: %s  ",s.c_str());
	string parseKey = CrevasseData(s.c_str());
	//CCLOG("after decryp: %s  ",parseKey.c_str());
	return (parseKey.length()!=0? atoi(parseKey.c_str()): defaultValue);
}

std::string CLocalData::getStringForKeyValue(int nKey) const
{
	return getStringForKeyValue(nKey, "");
}

std::string CLocalData::getStringForKeyValue(int nKey, const std::string & defaultValue) const
{ 
	//CCLOG("----------load XML data"); 
	char buffer[128]; 
	sprintf(buffer, "%s%d", "data_",nKey); 

	string s = UserDefault::getInstance()->getStringForKey(buffer); 
	//CCLOG("before decryp: %s  ",s.c_str()); 
	string parseKey = CrevasseData(s.c_str()); 
	//CCLOG("after decryp: %s  ",parseKey.c_str()); 
	return (parseKey.length()!=0? parseKey: defaultValue); 
}

//生成密文
std::string CLocalData::EncryptData(const char* szPass) const
{
	//生成密钥
	unsigned char szRandKey[ENCRYPT_KEY_LENGTH];
	srand((unsigned)time(NULL));
	szRandKey[0]=strlen(szPass);
	for (int i=1;i<sizeof(szRandKey);i++) 
	{
		szRandKey[i]=rand()%255;
	}
	//生成密文
	BYTE bTempCode=0;
	char szBuffer[8];
	std::string strEncryptPass;
	const unsigned int uKeyLength=sizeof(szRandKey)/sizeof(szRandKey[0]);
	int nTimes=(szRandKey[0]+uKeyLength-1)/uKeyLength*uKeyLength;
	for (int i=0;i<nTimes;i++)
	{
		if (i<szRandKey[0]) bTempCode=szPass[i]^szRandKey[i%uKeyLength];
		else bTempCode=szRandKey[i%uKeyLength]^(rand()%255);
		sprintf(szBuffer,"%02x%02x",szRandKey[i%uKeyLength],bTempCode);
		strEncryptPass+=szBuffer;
	}

	return strEncryptPass;
}

//解开密文
string CLocalData::CrevasseData(const char* szEncryPass) const
{
	if ((szEncryPass!=NULL)&&(strlen(szEncryPass)>=ENCRYPT_KEY_LENGTH*4))
	{
		//效验长度
		char * pEnd=NULL;
		int nSoureLength=0;
		char szSoureLengthBuf[3]={szEncryPass[0],szEncryPass[1],0};
		nSoureLength=strtol(szSoureLengthBuf,&pEnd,16);
		if (strlen(szEncryPass)!=((nSoureLength+ENCRYPT_KEY_LENGTH-1)/ENCRYPT_KEY_LENGTH*ENCRYPT_KEY_LENGTH*4)) 
			return "";

		//解开密码
		string strCrevassePass;
		for (int i=0;i<nSoureLength;i++)
		{
			BYTE szKeyBuffer[3]={(BYTE)szEncryPass[i*4],(BYTE)szEncryPass[i*4+1],0};
			char szEncryptBuffer[3]={szEncryPass[i*4+2],szEncryPass[i*4+3],0};
			unsigned int uKey,uEncrypt;
			uKey=strtol((char *)szKeyBuffer,&pEnd,16);
			uEncrypt=strtol(szEncryptBuffer,&pEnd,16);
			strCrevassePass+=(char)((BYTE)uKey^(BYTE)uEncrypt);
		}
		return strCrevassePass;
	}
	return "";
}

double CLocalData::getFloatForKeyValue( int nKey ) const
{
	return getFloatForKeyValue(nKey, 0.0f);
}

double CLocalData::getFloatForKeyValue( int nKey, double defaultValue ) const
{
//	CCLOG("----------load XML data"); 
	char buffer[128]; 
	sprintf(buffer, "%s%d", "data_", nKey);

	string s = UserDefault::getInstance()->getStringForKey(buffer); 
//	CCLOG("before decryp: %s  ",s.c_str()); 
	string parseKey = CrevasseData(s.c_str()); 
//	CCLOG("after decryp: %s  ",parseKey.c_str()); 
	return (parseKey.length()!=0? atof(parseKey.c_str()): defaultValue);
}

void CLocalData::setFloatForKeyValue( int nKey, float value ) const
{
	char buffer[128];
	sprintf(buffer, "%s%d", "data_", nKey);
//	CCLOG("----------save XML data");
//	CCLOG("before: index: Data%d, value: %d  ", pKey, value);

	char szValue[128];
	sprintf(szValue, "%f", value);
	std::string sKey = EncryptData(szValue);
//	CCLOG("after: index: Data%s, value: %s  ", pKey, sKey.c_str());
	UserDefault::getInstance()->setStringForKey(buffer, sKey); 
	//UserDefault::getInstance()->flush();
}