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

	//����Ӱ�ӽ�������ʱ��
	float mShadowFadeDuration;

	//Ӱ��·��
	string mShadowPath;

	//Ӱ����ɫ
	Color3B mShadowColor;

	//Ӱ��ê��
	Point mShadowAnchorPoint;

	//Ӱ���б�
	map<Sprite*, Point> mMapShadow;

	//��ǰ��¼ʱ��
	float mCurrentTick;

	//���ųߴ�
	float mScale;

	//�Ƿ���
	bool mFlip;

};


#endif