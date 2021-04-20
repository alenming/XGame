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

	//�����Ϣ������
	virtual void addListener(ICommListener* pListener);

	//�Ƴ���Ϣ������
	virtual void removeListener(ICommListener* pListener);

	//������Ϣ������������·�
	virtual void recvMessage( int nCmdID, const char* szData );

	//������Ϣ
	virtual void sendMessage( int nCmdID, const char* szData );

protected:

	 const char*  constructErrorMessage(int nCmdID, const char* szError);

protected:

	//����������
	vector<ICommListener*> mVecListener;

};



#endif //AbstractCommunicator_h__