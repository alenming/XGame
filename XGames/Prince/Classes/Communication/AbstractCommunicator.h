#ifndef AbstractCommunicator_h__
#define AbstractCommunicator_h__

#include "cocos2d.h"
#include "ICommListener.h"
#include <map>

USING_NS_CC;
using namespace std;

class AbstractCommunicator : public Ref
{

public:

	AbstractCommunicator();
	virtual ~AbstractCommunicator();

public:

	//添加消息监听器
	virtual void addListener(ICommListener* pListener);

	//移除消息监听器
	virtual void removeListener(ICommListener* pListener);

	//接收消息，并向监听者下发
	virtual void recvMessage( int nCmdID, const char* szData );

	//发送消息
	virtual void sendMessage( int nCmdID, const char* szData );

protected:

	 const char*  constructErrorMessage(int nCmdID, const char* szError);

protected:

	//监听器队列
	vector<ICommListener*> mVecListener;

};



#endif //AbstractCommunicator_h__