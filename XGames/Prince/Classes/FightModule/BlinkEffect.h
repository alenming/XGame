#ifndef BlinkEffect_h__
#define BlinkEffect_h__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class BlinkEffect : public Layer
{

public:

	BlinkEffect();
	virtual ~BlinkEffect();

	CREATE_FUNC(BlinkEffect);

public:

	virtual void onEnter() override;

	//获取所有目标
	vector<Ref*> getAllTargets();

private:

	void firstEffectCallBack();

	void secondEffectCallBack();

private:

	//闪烁图片
	Sprite* mBlack;
	Sprite* mWhite;



};

#endif //BlinkEffect_h__