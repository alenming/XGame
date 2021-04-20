#ifndef GuideCenter_h__
#define GuideCenter_h__

#include "cocos2d.h"
#include "GuideDef.h"
#include "Utils/Observer.h"
#include "Communication/Json/json.h"

USING_NS_CC;
using namespace std;


//�������
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

	//�ý��id
	int mNodeId;

	//������������������
	int mTriggerType;

	//�ýڵ����ڵ���ID
	int mChainId;

	//�ý������������id
	int mGroupId;

	//�ýڵ��Ƿ�Ϊ�ؼ����
	bool mIsKeyNode;

	//�ýڵ����������
	int mGuideType;

	//�����������¸����id
	int mNextNodeId;

	//������������һ�����
	GuideNode* mNextNode;

	//���������Ĺؿ�ID
	int mTriggerStageId;

	//������������ҵȼ�/ս���غ���
	int mTriggerLvRound;

	//���������Ľ���
	int mTriggerInterfaceId;

	//�����ر��л��Ľ���
	int mNextInterfaceId;

	//�����Ƿ�Ϊǿ��
	bool mIsForce;

	//�Ի�
	vector<int> mVecChatId;

	//������Դ����
	string mGuideResName;

	//������Դ����2
	string mGuideResName2;

	//��ǰ�Ի���ִ������
	int mCurrChatIndex;

	//��������(��������ʹ��)
	int mScrollType;

	//�Ƿ���ת��������
	int mJumpToMain;
};

//������
class GuideChain : public Ref
{

public:

	GuideChain();
	virtual ~GuideChain();

	static GuideChain* create(int headGuideId);

public:

	//��������ʼ��
	bool init(int headGuideId);

public:

	//��������id
	int mChainId;

	//��������ǰִ�н��
	GuideNode* mCurrentGuidNode;

	//�������������н��
	Vector<GuideNode*> mVecGuideNode;

};

class GuideCenter : public Ref, public Observable
{

public:

	GuideCenter();
	virtual ~GuideCenter();

	static GuideCenter* create();

public:

	//��ʼ��
	void init();

	//��ʼ����������
	void initGuideProcess(const Json::Value& data);

	//��ʼ����������ǰ����
	void initGuideChainProcess(int chainId, int groupId);

	//��������
	bool start(int interfaceId);

	//�����Ƿ������,���޷�ս������
	GuideNode* isGuideExecutable(int interfaceId);

	//UI�¼����������¸�����
	void next();

	//����ս����ʼ������
	void setFightStartTrigger(int stageId);

	//����ս��ǰ������
	void setFightBeforeTrigger(int stageId);

	//����ս���غϴ�������������������ǰ����
	void setRoundTrigger(int stageId, int round);

	//����ս���غϽ���������
	void setRoundEndTrigger(int stageId, int round);

	//����ս���з�����ǰ������
	void setEnemyBeforeTrigger(int stageId, int round);

	//����ս������������
	void setFightEndTrigger(int stageId);

	//����Ƿ�ɴ���
	bool isGuideNodeCanTrigger(GuideNode* guideNode);

private:

	void notify(GUIDE_NOTIFY_FLAG flag, GuideNode* guideNode);

private:

	//ȫ��������
	Map<int, GuideChain*> mMapGuideChain;

	//��ǰִ�е�����
	GuideChain* mCurrentGuideChain;

	//��������
	int mTriggerType;

	//��������
	int mTriggerInterfaceId;

	//�����ؿ�
	int mTriggerStageId;

	//����ս���غ�
	int mTriggerRound;

};

#endif //GuideCenter_h__