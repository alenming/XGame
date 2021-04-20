#ifndef ShadowAction_h__
#define ShadowAction_h__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

const float SHADOW_TICK = 1 / 40.0F;

class ShadowAction : public ActionInterval
{

public:

	ShadowAction();
	virtual ~ShadowAction();

	static ShadowAction* create(float duration, float shadowFadeDuration, float scale, bool isFlip, Point shadowAnchorPoint, Color3B color, string shadowPath);

public:

	virtual ShadowAction* clone() const override;
	virtual ShadowAction* reverse(void) const  override;
	virtual void startWithTarget(Node *target) override;
	virtual void update(float time) override;

private:

	bool initWithDuration(float duration, float shadowFadeDuration, float scale, bool isFlip, Point shadowAnchorPoint, Color3B color, string shadowPath);

	void onRemoveCallBack(Node* target);

private:

	//单个影子渐隐持续时间
	float mShadowFadeDuration;

	//影子路径
	string mShadowPath;

	//影子颜色
	Color3B mShadowColor;

	//影子锚点
	Point mShadowAnchorPoint;

	//影子列表
	map<Sprite*, Point> mMapShadow;

	//当前记录时间
	float mCurrentTick;

	//缩放尺寸
	float mScale;

	//是否翻面
	bool mFlip;

};


#endif