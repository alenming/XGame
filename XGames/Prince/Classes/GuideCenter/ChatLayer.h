#ifndef ChatLayer_h__
#define ChatLayer_h__

#include "cocos2d.h"
#include "CocoStudio.h"
#include "CocosGUI.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;
using namespace std;

enum CHAT_DIRECTION
{
	CHAT_DIRECTION_LEFT = 1,
	CHAT_DIRECTION_RIGHT
};

class ChatLayer : public LayerColor
{

public:

	ChatLayer();
	virtual ~ChatLayer();

	static ChatLayer* create(const string& content, const string& npcName, int npcId, int npcDir);

public:

	virtual bool init(const string& content, const string& npcName, int npcId, int npcDir);

	//触摸事件响应
	virtual bool onTouchBegan(Touch *touch, Event *unused_event); 
	virtual void onTouchMoved(Touch *touch, Event *unused_event); 
	virtual void onTouchEnded(Touch *touch, Event *unused_event); 
	virtual void onTouchCancelled(Touch *touch, Event *unused_event);

private:

	//初始化触摸
	void initTouch();




};



#endif //ChatLayer_h__