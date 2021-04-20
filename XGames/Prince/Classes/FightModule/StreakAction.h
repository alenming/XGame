#ifndef StreakAction_h__
#define StreakAction_h__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class StreakAction : public ActionInterval
{

public:

	StreakAction();
	virtual ~StreakAction();

	static StreakAction* create(float duration, float fade, float stroke, Point offsetPoint, const Color3B& color, const std::string& path);

public:

	virtual StreakAction* clone() const override;
	virtual StreakAction* reverse(void) const  override;
	virtual void startWithTarget(Node *target) override;
	virtual void update(float time) override;

private:

	bool initWithDuration(float duration, float fade, float stroke, Point offsetPoint, const Color3B& color, const std::string& path);

private:

	//��βЧ��
	MotionStreak* mMotionStreak;

	//MotionStreak����
	float mFade;
	float mStroke;
	Color3B mColor;
	string mPath;

	//��βЧ�������Ŀ��ê���ƫ����
	Point mOffsetPoint;
	

};


#endif