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

	//��ȡ����Ŀ��
	vector<Ref*> getAllTargets();

private:

	void firstEffectCallBack();

	void secondEffectCallBack();

private:

	//��˸ͼƬ
	Sprite* mBlack;
	Sprite* mWhite;



};

#endif //BlinkEffect_h__