#ifndef IChatView_h__
#define IChatView_h__

#include "FriendDef.h"
#include "vector"
using namespace std;
class IChatView
{
public:
	virtual void	onRecvChatMessage() = 0;
};
class INoticeView
{
public:
	virtual void	onRecvNoticeMessage() = 0;
};

#endif
