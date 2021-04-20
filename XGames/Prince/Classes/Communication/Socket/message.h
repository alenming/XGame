#ifndef MESSAGE_H
#define MESSAGE_H

#include "Util.h"
#include "cocos2d.h"

using namespace cocos2d;

class Message:public Ref
{
public:
	
    
    char HEAD0;
    char HEAD1;
    char HEAD2;
    char HEAD3;
    char ProtoVersion;
    
    byte serverVersion[4];
    byte length[4];
    byte commandId[4];
    /**
      * 消息的数据
      */
    char* data;
	
	int type;
	unsigned int type_selfdefine;
	
	
	Message();
    int datalength();
	~Message();

	// 实现一个深拷贝;
	Message* clone();

	static Message* createMessage(const char* data, size_t dataSize, int commandId);
};

#endif

