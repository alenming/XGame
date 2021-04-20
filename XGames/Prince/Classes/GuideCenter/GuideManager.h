#ifndef GuideManager_h__
#define GuideManager_h__

#include "cocos2d.h"
#include "GuideCenter.h"
#include "Utils/Observer.h"
#include "GuideController.h"
#include "Communication/Json/json.h"

USING_NS_CC;

typedef function<void(GuideNode*)> StartGuideCallBack;
typedef function<void(GuideNode*)> EndGuideCallBack;

class GuideManager : public Ref, public Observer
{

public:

	GuideManager();
	virtual ~GuideManager();

	static GuideManager* getInstance();
	static void destroyInstance();

public:

	//��������֪ͨ
	virtual void updateSelf(void* data);

	//��ʼ����
	bool startGuide(int interfaceId);

	//��һִ֡������
	void startGuideNextFrame(int interfaceId);

	//ս����ʼ����(�����볡��)
	bool startFightStartGuide(int interfaceId, int stageId);

	//ս��ǰ����(�����볡ǰ)
	bool startFightBeforeGuide(int interfaceId, int stageId);

	//ս���غ�����
	bool startFightRoundGuide(int interfaceId, int stageId, int roundCount);

	//ս���غϽ�������
	bool startFightRoundEndGuide(int interfaceId, int stageId, int roundCount);

	//�з�����ǰ����
	bool startFightEnemyBeforeGuide(int interfaceId, int stageId, int roundCount);

	//ս����������
	bool startFightEndGuide(int interfaceId, int stageId);

	//ִ����һ������
	void next();

	//���������ص�������ִ�����ɾ���ص�
	void setCallBack(const StartGuideCallBack& start, const EndGuideCallBack& end);

	//��ʼ����������
	void initGuideProcess(const Json::Value& data);

	//�����Ƿ��ִ��
	GuideNode* isGuideExecutable(int interfaceId);

	//�����Ƿ񿪷�
	bool isFunctionOpen(FUNCTION_TYPE type, bool showTips = true);

private:

	//��ʾ����
	void showGuide();

	//��ʾ�Ի�
	void showChat();

	//��ʾ����ս��
	void showGuideFight();

	//��ʾ������
	void showGuideEmpty();

	//��ʾ��������
	void showGuideAnimation();

private:

	static GuideManager* mInstance;

	//��������
	GuideCenter* mGuideCenter;

	//������ʼ�ص�
	StartGuideCallBack mStartGuideCallBack;

	//���������ص�
	EndGuideCallBack mEndGuideCallBack;

	//����ͨ��
	GuideController mGuideController;

	//��ǰ�������
	GuideNode* mCurrentGuideNode;

	bool mIsInitProcess;
	
};


#endif //GuideManager_h__