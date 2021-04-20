#ifndef GuideCenter_h__
#define GuideCenter_h__

#include "cocos2d.h"
#include "GuideDef.h"
#include "Utils/Observer.h"
#include "Communication/Json/json.h"

USING_NS_CC;
using namespace std;


//引导结点
class GuideNode : public Ref
{

public:

	GuideNode()
		: mNodeId(-1)
		, mTriggerType(-1)
		, mChainId(-1)
		, mGroupId(-1)
		, mIsKeyNode(false)
		, mGuideType(-1)
		, mNextNodeId(-1)
		, mNextNode(nullptr)
		, mTriggerStageId(-1)
		, mTriggerLvRound(-1)
		, mTriggerInterfaceId(-1)
		, mNextInterfaceId(-1)
		, mIsForce(false)
		, mCurrChatIndex(0)
		, mScrollType(-1)
		, mJumpToMain(0)
	{}
	virtual ~GuideNode(){};

	static GuideNode* create(int guideId);

public:

	bool init(int guideId);

public:

	//该结点id
	int mNodeId;

	//该引导其他触发条件
	int mTriggerType;

	//该节点所在的链ID
	int mChainId;

	//该结点所在引导组id
	int mGroupId;

	//该节点是否为关键结点
	bool mIsKeyNode;

	//该节点的引导类型
	int mGuideType;

	//该引导结点的下个结点id
	int mNextNodeId;

	//该引导结点的下一个结点
	GuideNode* mNextNode;

	//引导触发的关卡ID
	int mTriggerStageId;

	//引导触发的玩家等级/战斗回合数
	int mTriggerLvRound;

	//引导触发的界面
	int mTriggerInterfaceId;

	//引导关闭切换的界面
	int mNextInterfaceId;

	//引导是否为强制
	bool mIsForce;

	//对话
	vector<int> mVecChatId;

	//引导资源名称
	string mGuideResName;

	//引导资源名称2
	string mGuideResName2;

	//当前对话的执行索引
	int mCurrChatIndex;

	//滚屏类型(仅主场景使用)
	int mScrollType;

	//是否跳转到主场景
	int mJumpToMain;
};

//引导链
class GuideChain : public Ref
{

public:

	GuideChain();
	virtual ~GuideChain();

	static GuideChain* create(int headGuideId);

public:

	//引导链初始化
	bool init(int headGuideId);

public:

	//引导链条id
	int mChainId;

	//引导链当前执行结点
	GuideNode* mCurrentGuidNode;

	//该引导链条所有结点
	Vector<GuideNode*> mVecGuideNode;

};

class GuideCenter : public Ref, public Observable
{

public:

	GuideCenter();
	virtual ~GuideCenter();

	static GuideCenter* create();

public:

	//初始化
	void init();

	//初始化引导进度
	void initGuideProcess(const Json::Value& data);

	//初始化引导链当前进度
	void initGuideChainProcess(int chainId, int groupId);

	//启动引导
	bool start(int interfaceId);

	//引导是否可启动,仅限非战斗引导
	GuideNode* isGuideExecutable(int interfaceId);

	//UI事件触发启动下个引导
	void next();

	//设置战斗开始触发器
	void setFightStartTrigger(int stageId);

	//设置战斗前触发器
	void setFightBeforeTrigger(int stageId);

	//设置战斗回合触发器，需在启动引导前设置
	void setRoundTrigger(int stageId, int round);

	//设置战斗回合结束触发器
	void setRoundEndTrigger(int stageId, int round);

	//设置战斗敌方出手前触发器
	void setEnemyBeforeTrigger(int stageId, int round);

	//设置战斗结束触发器
	void setFightEndTrigger(int stageId);

	//结点是否可触发
	bool isGuideNodeCanTrigger(GuideNode* guideNode);

private:

	void notify(GUIDE_NOTIFY_FLAG flag, GuideNode* guideNode);

private:

	//全部引导链
	Map<int, GuideChain*> mMapGuideChain;

	//当前执行的链条
	GuideChain* mCurrentGuideChain;

	//触发类型
	int mTriggerType;

	//触发界面
	int mTriggerInterfaceId;

	//触发关卡
	int mTriggerStageId;

	//触发战斗回合
	int mTriggerRound;

};

#endif //GuideCenter_h__