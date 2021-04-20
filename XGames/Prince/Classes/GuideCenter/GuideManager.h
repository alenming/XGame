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

	//引导中心通知
	virtual void updateSelf(void* data);

	//开始引导
	bool startGuide(int interfaceId);

	//下一帧执行引导
	void startGuideNextFrame(int interfaceId);

	//战斗开始引导(人物入场后)
	bool startFightStartGuide(int interfaceId, int stageId);

	//战斗前引导(人物入场前)
	bool startFightBeforeGuide(int interfaceId, int stageId);

	//战斗回合引导
	bool startFightRoundGuide(int interfaceId, int stageId, int roundCount);

	//战斗回合结束引导
	bool startFightRoundEndGuide(int interfaceId, int stageId, int roundCount);

	//敌方出手前引导
	bool startFightEnemyBeforeGuide(int interfaceId, int stageId, int roundCount);

	//战斗结束引导
	bool startFightEndGuide(int interfaceId, int stageId);

	//执行下一条引导
	void next();

	//设置引导回调，引导执行完毕删除回调
	void setCallBack(const StartGuideCallBack& start, const EndGuideCallBack& end);

	//初始化引导进度
	void initGuideProcess(const Json::Value& data);

	//引导是否可执行
	GuideNode* isGuideExecutable(int interfaceId);

	//功能是否开放
	bool isFunctionOpen(FUNCTION_TYPE type, bool showTips = true);

private:

	//显示引导
	void showGuide();

	//显示对话
	void showChat();

	//显示引导战斗
	void showGuideFight();

	//显示空引导
	void showGuideEmpty();

	//显示动画引导
	void showGuideAnimation();

private:

	static GuideManager* mInstance;

	//引导中心
	GuideCenter* mGuideCenter;

	//引导开始回调
	StartGuideCallBack mStartGuideCallBack;

	//引导结束回调
	EndGuideCallBack mEndGuideCallBack;

	//引导通信
	GuideController mGuideController;

	//当前引导结点
	GuideNode* mCurrentGuideNode;

	bool mIsInitProcess;
	
};


#endif //GuideManager_h__