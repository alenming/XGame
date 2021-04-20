#ifndef GuideLayer_h__
#define GuideLayer_h__

#include "cocos2d.h"
#include "CocoStudio.h"
#include "CocosGUI.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;
using namespace std;

class GuideLayer : public Layer
{

public:

	GuideLayer();
	virtual ~GuideLayer();

	static GuideLayer* create(const string& resName, bool isForceGuide, bool isKeyGuide);

public:

	//初始化
	virtual bool init(const string& resName, bool isForceGuide, bool isKeyGuide);

	//触摸事件响应
	virtual bool onTouchBegan(Touch *touch, Event *unused_event); 
	virtual void onTouchMoved(Touch *touch, Event *unused_event); 
	virtual void onTouchEnded(Touch *touch, Event *unused_event); 
	virtual void onTouchCancelled(Touch *touch, Event *unused_event);

private:

	//初始化触摸
	void initTouch();

	//播放动画
	void playAnimation();

private:

	//是否为强制引导
	bool mIsForceGuide;

	//是否为关键结点
	bool mIsKeyNode;

	//引导区域
	Layout* mGuideArea;

	//引导光圈
	ImageView* mGuideCircle;

	//引导指针
	ImageView* mGuidePointer;

	//裁剪蒙板
	Sprite* mStencil;

	//裁剪贴图
	Sprite* mStencilSp;

	//引导文字
	Text* mGuideText;

	//是否命中引导区域
	bool mIsHitGuideArea;

	//引导画布名称
	string mLayoutName;
};




#endif //GuideLayer_h__