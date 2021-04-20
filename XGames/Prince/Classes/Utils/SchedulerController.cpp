#include "SchedulerController.h"


SchedulerController::SchedulerController()
{

}

SchedulerController::~SchedulerController()
{

}

SchedulerController* SchedulerController::getInstance()
{
	static SchedulerController schedulerController;
	return &schedulerController;
}

void SchedulerController::pauseAllTargets(float dt)
{
	vector<Ref*> targets;
	pauseAllTargets(dt, targets);
}

void SchedulerController::pauseAllTargets(float dt, vector<Ref*> targets)
{
	Scheduler* scheduler = Director::getInstance()->getScheduler();
	mPausedTarget = scheduler->pauseAllTargets();
	scheduler->schedule(schedule_selector(SchedulerController::resumeAllTargets), this, 0, 0, dt, false);

	if (!targets.empty())
	{
		for (auto target : targets)
		{
			if (target)
			{
				scheduler->resumeTarget(target);
			}
		}
	}
}

void SchedulerController::resumeAllTargets(float dt)
{
	Scheduler* scheduler = Director::getInstance()->getScheduler();
	scheduler->resumeTargets(mPausedTarget);
}


