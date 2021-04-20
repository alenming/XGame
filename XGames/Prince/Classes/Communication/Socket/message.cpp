
#include "message.h"
#include "SocketClient.h"
#include "ByteBuffer.h"

Message::Message():data(NULL){
    
}
Message::~Message()
{
	//printf("Message::~Message() %p\n",this);
	if (data!=NULL) {
		delete[] data;
	}
}

int Message::datalength()
{
    return ByteBuffer::bytesToInt(length)+13;
//    int addr = length[3] & 0xFF;
//    
//    addr |= ((length[2] << 8) & 0xFF00);
//    
//    addr |= ((length[1] << 16) & 0xFF0000);
//    
//    addr |= ((length[0] << 24) & 0xFF000000);
//    
//    return addr+17;
}

Message* Message::clone()
{
	Message* pCloneMsg = new Message();
	if (pCloneMsg != nullptr)
	{
		pCloneMsg->HEAD0 = this->HEAD0;
		pCloneMsg->HEAD1 = this->HEAD1;
		pCloneMsg->HEAD2 = this->HEAD2;
		pCloneMsg->HEAD3 = this->HEAD3;
		pCloneMsg->ProtoVersion = this->ProtoVersion;
		memcpy(pCloneMsg->serverVersion, this->serverVersion, sizeof(byte)*4);
		memcpy(pCloneMsg->commandId, this->commandId, sizeof(byte)*4);
		memcpy(pCloneMsg->length, this->length, sizeof(byte)*4);
		pCloneMsg->data = new char[this->datalength()];
		memcpy(pCloneMsg->data, this->data, datalength());
		pCloneMsg->type = this->type;
		pCloneMsg->type_selfdefine = this->type_selfdefine;
	}
	return pCloneMsg;
}

Message* Message::createMessage(const char* data, size_t dataSize, int commandId)
{
	Message* msg = new Message();

	msg->HEAD0=78;
	msg->HEAD1=37;
	msg->HEAD2=38;
	msg->HEAD3=48;
	msg->ProtoVersion=9;

	int a=0;
	msg->serverVersion[3]=(byte)(0xff&a);;
	msg->serverVersion[2]=(byte)((0xff00&a)>>8);
	msg->serverVersion[1]=(byte)((0xff0000&a)>>16);
	msg->serverVersion[0]=(byte)((0xff000000&a)>>24);

	int b=dataSize+4;

	msg->length[3]=(byte)(0xff&b);
	msg->length[2]=(byte)((0xff00&b)>>8);
	msg->length[1]=(byte)((0xff0000&b)>>16);
	msg->length[0]=(byte)((0xff000000&b)>>24);

	int c=commandId;
	msg->commandId[3]=(byte)(0xff&c);
	msg->commandId[2]=(byte)((0xff00&c)>>8);
	msg->commandId[1]=(byte)((0xff0000&c)>>16);
	msg->commandId[0]=(byte)((0xff000000&c)>>24);

	//    str.append(msg->HEAD0);
	//printf("%d" ,msg->datalength());
	msg->data = new char[msg->datalength()];
	memcpy(msg->data+0,&msg->HEAD0,1);
	memcpy(msg->data+1,&msg->HEAD1,1);
	memcpy(msg->data+2,&msg->HEAD2,1);
	memcpy(msg->data+3,&msg->HEAD3,1);
	memcpy(msg->data+4,&msg->ProtoVersion,1);
	memcpy(msg->data+5,&msg->serverVersion,4);
	memcpy(msg->data+9,&msg->length,4);
	memcpy(msg->data+13,&msg->commandId,4);
	memcpy(msg->data+17,data, dataSize);
	//memcpy(msg->data+position,bytes+offset,len);
	//msg->data = data;
	return msg;
}
