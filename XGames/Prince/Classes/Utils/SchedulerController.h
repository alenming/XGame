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

	//��ȡ��ʵ��
	static SchedulerController* getInstance();

	//ȫ����ͣ
	void pauseAllTargets(float dt);

	//������Ŀ����ȫ����ͣ
	void pauseAllTargets(float dt, vector<Ref*> targets);

	//ȫ������
	void resumeAllTargets(float dt);

private:

	//������ͣ��Ŀ��
	std::set<void*> mPausedTarget;
};


#endif //SchedulerController__