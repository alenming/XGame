#include "AbstractCommunicator.h"
#include "Common/Common.h"
#include "Temp/CustomPop.h"


AbstractCommunicator::AbstractCommunicator()
{
	
}

AbstractCommunicator::~AbstractCommunicator()
{

}

void AbstractCommunicator::addListener(ICommListener* pListener)
{
	if (nullptr == pListener)
	{
		return;
	}
	auto iter = find(mVecListener.begin(), mVecListener.end(), pListener);
	if(iter == mVecListener.end())
		mVecListener.push_back(pListener);
}

void AbstractCommunicator::removeListener(ICommListener* pListener)
{
	if (nullptr == pListener)
	{
		return;
	}
	auto iter = find(mVecListener.begin(), mVecListener.end(), pListener);
	if(iter != mVecListener.end())
	{
		mVecListener.erase(iter);
	}
}

void AbstractCommunicator::sendMessage( int nCmdID, const char* szData )
{

}

void AbstractCommunicator::recvMessage( int nCmdID, const char* szData )
{	
	//处理通信数据异常
	if (szData == NULL || strlen(szData) == 0)
	{
		CCLOG("-----recvMessage: data is empty!-----");
		return;
	}

	//处理通信数据异常
	Json::Reader read;
	Json::Value root;
	if (!read.parse(szData, root))
	{
		CCLOG("-----recvMessage: json decode error!-----");
		return;
	}

	//向监听者派发消息, 不做异常处理
	for (size_t i = 0; i < mVecListener.size(); i++)
	{
		mVecListener.at(i)->onRecvMessage(nCmdID, szData);
	}
}

const char* AbstractCommunicator::constructErrorMessage(int nCmdID, const char* szError)
{
	Json::FastWriter  writer;
	Json::Value val;
	val["result"] = 1;
	val["msg"] = szError;
	std::string  json_file=writer.write(val);	
	recvMessage(nCmdID, json_file.c_str());
	return json_file.c_str();
}
