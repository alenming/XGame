#ifndef __ModalLayer__
#define __ModalLayer__

#include "cocos2d.h"
#include "Utils/SoundUtils.h"
USING_NS_CC;

class ModalLayer : public LayerColor
{

public:

	ModalLayer();
	virtual ~ModalLayer();

public:

	//初始化
	virtual bool init();

	//显示
	void show(Node* parent,int zoder = 0);

	//销毁
	void dispose();

	//是否点击空白区域销毁
	void setClickEmptyDispose(bool clickEmptyDispose);

private:

	//触摸事件响应
	bool onTouchBegan(Touch *touch, Event *event);
	void onTouchEnded(Touch *touch, Event *event);

	//销毁回调
	void callBackDispose(float dt);

private:

	bool mClickEmptyDispose;

	
};





#endif //ModalLayer__