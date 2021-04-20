#include "GuideCenter.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "Utils/StringFormat.h"
#include "HeroModule/HeroModel.h"
#include "Communication/EnvConfig.h"

///////////////////////////////�������///////////////////////////////////////////

GuideNode* GuideNode::create( int guideId )
{
	GuideNode* guideNode = new GuideNode();
	if (guideNode->init(guideId))
	{
		guideNode->autorelease();
		return guideNode;
	}
	else
	{
		delete guideNode;
		return nullptr;
	}
}

bool GuideNode::init(int guideId)
{
	RowData* guideData = DataManager::getInstance()->searchGuideById(guideId);

	if (guideData)
	{
		mNodeId = guideData->getIntData("id");
		mChainId = guideData->getIntData("chainGro");
		mTriggerType = guideData->getIntData("actCond");
		mGroupId = guideData->getIntData("leadGro");
		mIsKeyNode = (guideData->getIntData("keyPoint") == GUIDE_NODE_KEY);
		mGuideType = guideData->getIntData("actTpy");
		mNextNodeId = guideData->getIntData("rearId");
		mTriggerStageId = guideData->getIntData("lvlId");
		mTriggerLvRound = guideData->getIntData("lvlRound");
		mTriggerInterfaceId = guideData->getIntData("interId");
		mIsForce = (guideData->getIntData("manTpy") == GUIDE_NODE_FORCE);
		mGuideResName = guideData->getStringData("canvName");
		mGuideResName2 = guideData->getStringData("canvName1");
		mNextInterfaceId = guideData->getIntData("backInter");
		mScrollType = guideData->getIntData("scrollArea");
		mJumpToMain = guideData->getIntData("clearInter");

		if (mGuideType == GUIDE_NODE_CHAT)
		{
			//�������������жԻ�
			StringFormat::parseCsvStringVec(guideData->getStringData("chatGro"), mVecChatId);
		}

		return true;
	}

	return false;
}

///////////////////////////////������///////////////////////////////////////////

GuideChain::GuideChain()
	: mChainId(-1)
	, mCurrentGuidNode(nullptr)
{

}

GuideChain::~GuideChain() 
{
	mVecGuideNode.clear();
}

GuideChain* GuideChain::create( int headGuideId )
{
	GuideChain* guideChain = new GuideChain();
	if (guideChain->init(headGuideId))
	{
		guideChain->autorelease();
		return guideChain;
	}
	else
	{
		delete guideChain;
		return nullptr;
	}
}

bool GuideChain::init(int headGuideId)
{
	GuideNode* headNode = GuideNode::create(headGuideId);
	GuideNode* currentNode = headNode;

	mChainId = headNode->mChainId;

	//ͷ���Ϊ�գ��������������ʧ��
	if (!headNode)
	{
		return false;
	}
	else
	{
		mCurrentGuidNode = headNode;
		mVecGuideNode.pushBack(headNode);
	}

	while(currentNode)
	{
		GuideNode* nextNode = GuideNode::create(currentNode->mNextNodeId);
		//��һ�������������id����һ��
		if (nextNode && nextNode->mChainId == mChainId)
		{
			mVecGuideNode.pushBack(nextNode);

			currentNode->mNextNode = nextNode;
			currentNode = nextNode;
		}
		else
		{
			currentNode = nullptr;
		}
	}

	return true;
}

///////////////////////////////��������///////////////////////////////////////////

GuideCenter::GuideCenter()
	: mTriggerType(-1)
	, mTriggerInterfaceId(-1)
	, mTriggerStageId(-1)
	, mTriggerRound(-1)
	, mCurrentGuideChain(nullptr)
{
	init();
}

GuideCenter::~GuideCenter()
{
	mMapGuideChain.clear();
}

GuideCenter* GuideCenter::create()
{
	GuideCenter* guideCenter = new GuideCenter();
	guideCenter->autorelease();
	return guideCenter;
}

void GuideCenter::init()
{
	multimap<int, RowData>& guideMap = DataManager::getInstance()->getDTGuide()->getTableData();

	//�����������ͷ���, ����������
	for (auto iter = guideMap.begin(); iter != guideMap.end(); iter++)
	{
		RowData& rowData = iter->second;
		int chainState = rowData.getIntData("chainState");
		int guideId = rowData.getIntData("id");
		if (chainState == GUIDE_NODE_CHAIN_HEAD)
		{
			GuideChain* guideChain = GuideChain::create(guideId);
			if(guideChain)
			{
				mMapGuideChain.insert(guideChain->mChainId, guideChain);
			}
		}
	}
}

void GuideCenter::initGuideChainProcess(int chainId, int groupId)
{
	if (!GUIDE_PROCESS_ENABLE) return;

	auto iter = mMapGuideChain.find(chainId);
	if (iter != mMapGuideChain.end())
	{
		GuideChain* guideChain = iter->second;
		GuideNode* guideNode = guideChain->mCurrentGuidNode;
		bool isFindCurrentGuideNode = false;

		while (guideNode)
		{
			if (guideNode->mGroupId == groupId && 
				guideNode->mNextNode && guideNode->mNextNode->mGroupId != groupId)
			{
				//��ǰ���Ϊ���������飬�ҵ�ǰ�����¸���㲻Ϊ���������飬˵����ǰ�����¸����Ϊ��ǰִ�н��
				guideChain->mCurrentGuidNode = guideNode->mNextNode;
				isFindCurrentGuideNode = true;
				break;
			}
			else
			{
				//δ�ҵ���������¸����
				guideNode = guideNode->mNextNode;
			}
		}

		//�������������������δ�ҵ�ִ�н�㣬��˵�����������Ѿ�ȫ������
		if (!isFindCurrentGuideNode)
		{
			guideChain->mCurrentGuidNode = nullptr;
		}
	}
}

//��������
bool GuideCenter::start(int interfaceId)
{
	mTriggerInterfaceId = interfaceId;

	//�ж����������ϵĵ�ǰ����Ƿ��ִ��
	for (auto iter = mMapGuideChain.begin(); iter != mMapGuideChain.end(); iter++)
	{
		GuideChain* guideChain = iter->second;
		GuideNode* guideNode = guideChain->mCurrentGuidNode;
		if (isGuideNodeCanTrigger(guideNode))
		{
			//����ý���ִ�У���ִ�иý�㲢֪ͨUIչʾ��������¼��ǰִ������
			mCurrentGuideChain = guideChain;
			notify(GUIDE_NOTIFY_START, guideNode);
			return true;
		}
	}

	//δ�ҵ���ִ������֪ͨ����
	mCurrentGuideChain = nullptr;

	notify(GUIDE_NOTIFY_START, nullptr);
	notify(GUIDE_NOTIFY_END, nullptr);

	return false;
	
}

GuideNode* GuideCenter::isGuideExecutable(int interfaceId)
{
	mTriggerInterfaceId = interfaceId;

	GuideNode* ret = nullptr;

	//�ж����������ϵĵ�ǰ����Ƿ��ִ��
	for (auto iter = mMapGuideChain.begin(); iter != mMapGuideChain.end(); iter++)
	{
		GuideChain* guideChain = iter->second;
		GuideNode* guideNode = guideChain->mCurrentGuidNode;
		if (isGuideNodeCanTrigger(guideNode))
		{
			ret = guideNode;
		}
	}
	mTriggerType = -1;
	mTriggerInterfaceId = -1;
	mTriggerStageId = -1;
	mTriggerRound = -1;

	return ret;
}

//�����¸�����
void GuideCenter::next()
{
	if (mCurrentGuideChain)
	{
		GuideNode* currentGuideNode = mCurrentGuideChain->mCurrentGuidNode;
		if (currentGuideNode)
		{
			//�����ǰ�����¸��������¸����Ĵ���������ͬ�����л����棬��ִ���¸�����
			if (currentGuideNode->mNextNode && currentGuideNode->mNextInterfaceId != -1 && 
				currentGuideNode->mNextInterfaceId == currentGuideNode->mNextNode->mTriggerInterfaceId)
			{
				mTriggerInterfaceId = currentGuideNode->mNextNode->mTriggerInterfaceId;
				mTriggerType = currentGuideNode->mNextNode->mTriggerType;
			}

			switch (currentGuideNode->mGuideType)
			{

			case GUIDE_NODE_GUIDE: //��������ֱ��ִ�в���¼�¸����
			case GUIDE_NODE_FIGHT:
			case GUIDE_NODE_EMPTY:
				{
					mCurrentGuideChain->mCurrentGuidNode = currentGuideNode->mNextNode;
					if (isGuideNodeCanTrigger(currentGuideNode->mNextNode))
					{
						notify(GUIDE_NOTIFY_NEXT, mCurrentGuideChain->mCurrentGuidNode);
					}
					else
					{
						//�¸���㲻��ִ�У�֪ͨ����
						notify(GUIDE_NOTIFY_END, nullptr);
					}
				}
				break;

			case GUIDE_NODE_CHAT: 
				{
					//�Ի��������ж��Ի��Ƿ�ִ�н�������δִ�н�������Ȼͣ���ڵ�ǰ��㣬��ִ�н�������¼��ִ���¸����
					currentGuideNode->mCurrChatIndex++;
					if (!currentGuideNode->mVecChatId.empty() 
						&& currentGuideNode->mCurrChatIndex < currentGuideNode->mVecChatId.size())
					{
						//�Ի�δִ�н���������ִ��
						notify(GUIDE_NOTIFY_NEXT, mCurrentGuideChain->mCurrentGuidNode);
					}
					else
					{
						mCurrentGuideChain->mCurrentGuidNode = currentGuideNode->mNextNode;
						//�Ի�ִ�н�����ִ���¸����
						if (isGuideNodeCanTrigger(currentGuideNode->mNextNode))
						{
							notify(GUIDE_NOTIFY_NEXT, mCurrentGuideChain->mCurrentGuidNode);
						}
						else
						{
							//�¸���㲻��ִ�У�֪ͨ����
							notify(GUIDE_NOTIFY_END, nullptr);
						}
					}
				}
				break;
			default:
				break;
			}
		}
	}
}

void GuideCenter::notify(GUIDE_NOTIFY_FLAG flag, GuideNode* guideNode)
{
	ObserverParam observerParam;
	observerParam.id = flag;
	observerParam.updateData = guideNode;
	notifyObservers(&observerParam);

	//�ý׶�����������������б��
	if (flag == GUIDE_NOTIFY_END)
	{
		mTriggerType = -1;
		mTriggerInterfaceId = -1;
		mTriggerStageId = -1;
		mTriggerRound = -1;
	}
}

//����ս����ʼ������
void GuideCenter::setFightStartTrigger(int stageId)
{
	mTriggerType = GUIDE_NODE_TRIGGER_FIGHT_START;
	mTriggerStageId = stageId;
}

void GuideCenter::setFightBeforeTrigger(int stageId)
{
	mTriggerType = GUIDE_NODE_TRIGGER_FIGHT_BEFORE;
	mTriggerStageId = stageId;
}

//����ս���غϴ�����
void GuideCenter::setRoundTrigger(int stageId, int round)
{
	mTriggerType = GUIDE_NODE_TRIGGER_FIGHT_ROUND;
	mTriggerStageId = stageId;
	mTriggerRound = round;
}

void GuideCenter::setRoundEndTrigger(int stageId, int round)
{
	mTriggerType = GUIDE_NODE_TRIGGER_FIGHT_ROUND_END;
	mTriggerStageId = stageId;
	mTriggerRound = round;
}

void GuideCenter::setEnemyBeforeTrigger(int stageId, int round)
{
	mTriggerType = GUIDE_NODE_TRIGGER_ROUND_ENEMY_BEFORE;
	mTriggerStageId = stageId;
	mTriggerRound = round;
}

//����ս������������
void GuideCenter::setFightEndTrigger(int stageId)
{
	mTriggerType = GUIDE_NODE_TRIGGER_FIGHT_END;
	mTriggerStageId = stageId;
}

bool GuideCenter::isGuideNodeCanTrigger(GuideNode* guideNode)
{
	if (!GUIDE_ENABLE) return false;

	if (guideNode && guideNode->mTriggerInterfaceId != -1 && guideNode->mTriggerInterfaceId == mTriggerInterfaceId)
	{
		switch (guideNode->mTriggerType)
		{

		case GUIDE_NODE_TRIGGER_PRIOR_DONE:
			return true;
		case GUIDE_NODE_TRIGGER_LEVEL:
			{
				//�ȼ�����ֱ���ж��ȼ��Ƿ�ﵽ
				int level = HeroModel::getInstance()->getMainHero()->mLevel;
				return (level >= guideNode->mTriggerLvRound);
			}
		case GUIDE_NODE_TRIGGER_FIGHT_BEFORE:
		case GUIDE_NODE_TRIGGER_FIGHT_START:
		case GUIDE_NODE_TRIGGER_FIGHT_END:
			{
				//ս���������ж�����������ؿ�id
				return guideNode->mTriggerType == mTriggerType &&
					guideNode->mTriggerStageId == mTriggerStageId;
			}
		case GUIDE_NODE_TRIGGER_FIGHT_ROUND:
		case GUIDE_NODE_TRIGGER_FIGHT_ROUND_END:
		case GUIDE_NODE_TRIGGER_ROUND_ENEMY_BEFORE:
			{
				return guideNode->mTriggerType == mTriggerType &&
					guideNode->mTriggerStageId == mTriggerStageId &&
					guideNode->mTriggerLvRound == mTriggerRound;
			}
		default:
			break;
		}
	}
	return false;
}

