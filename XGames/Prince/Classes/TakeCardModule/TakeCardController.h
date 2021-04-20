#ifndef TakeCardController_h__
#define TakeCardController_h__

#include "Communication/ICommListener.h"
#include "Communication/Command.h"

typedef struct tagTakeCardParam
{
	//'buyType', 1-蓝色抽取,消耗抽卡道具(id 10004)，2-紫色抽取，消耗金币
	//	'times'：紫色抽取，连抽的次数，1或10
	int buyType;
	int times;
}TakeCardParam;

class TakeCardController: public ICommListener
{
public:
	TakeCardController();
	~TakeCardController();

	static TakeCardController* getInstance();
	static void destroyInstance();

	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);
	void processGetTakeCardInfoData(const char* szData);
	void processTakeCardResultData(const char* szData);
	
	void onGetCardInfo();
	//'buyType', 1-银币抽取,2-金币抽取
	//	'times'：连抽的次数，1或10
	void onTakeCard(const int& buyType, const int& times);
	void onTakeCardAgain(const int& times);
	//

	const TakeCardParam& getTakeCardParam()
	{
		return m_lastTakeCardParam;
	}
protected:
	//LOGIN_USER_INFO m_userData;
	TakeCardParam m_lastTakeCardParam;
private:

	static TakeCardController* ms_pInstance;
};

#endif // LoginController_h__
