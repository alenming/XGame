#ifndef SchedulerController__
#define SchedulerController__

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class SchedulerController : public Ref
{

private:

	SchedulerController();
	~SchedulerController();

public:

	//获取单实例
	static SchedulerController* getInstance();

	//全部暂停
	void pauseAllTargets(float dt);

	//除传入目标外全部暂停
	void pauseAllTargets(float dt, vector<Ref*> targets);

	//全部唤醒
	void resumeAllTargets(float dt);

private:

	//所有暂停的目标
	std::set<void*> mPausedTarget;
};


#endif //SchedulerController__