#include "GuideCenter.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "Utils/StringFormat.h"
#include "HeroModule/HeroModel.h"
#include "Communication/EnvConfig.h"

///////////////////////////////引导结点///////////////////////////////////////////

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
			//该引导结点的所有对话
			StringFormat::parseCsvStringVec(guideData->getStringData("chatGro"), mVecChatId);
		}

		return true;
	}

	return false;
}

///////////////////////////////引导链///////////////////////////////////////////

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

	//头结点为空，则该引导链创建失败
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
		//下一个结点必须与该链id保持一致
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

///////////////////////////////引导中心///////////////////////////////////////////

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

	//查找引导表的头结点, 构造引导链
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
				//当前结点为参数引导组，且当前结点的下个结点不为参数引导组，说明当前结点的下个结点为当前执行结点
				guideChain->mCurrentGuidNode = guideNode->mNextNode;
				isFindCurrentGuideNode = true;
				break;
			}
			else
			{
				//未找到，则继续下个结点
				guideNode = guideNode->mNextNode;
			}
		}

		//如果遍历整个引导链都未找到执行结点，则说明该引导链已经全部走完
		if (!isFindCurrentGuideNode)
		{
			guideChain->mCurrentGuidNode = nullptr;
		}
	}
}

//启动引导
bool GuideCenter::start(int interfaceId)
{
	mTriggerInterfaceId = interfaceId;

	//判定所有链条上的当前结点是否可执行
	for (auto iter = mMapGuideChain.begin(); iter != mMapGuideChain.end(); iter++)
	{
		GuideChain* guideChain = iter->second;
		GuideNode* guideNode = guideChain->mCurrentGuidNode;
		if (isGuideNodeCanTrigger(guideNode))
		{
			//如果该结点可执行，则执行该结点并通知UI展示引导，记录当前执行链条
			mCurrentGuideChain = guideChain;
			notify(GUIDE_NOTIFY_START, guideNode);
			return true;
		}
	}

	//未找到可执行链，通知结束
	mCurrentGuideChain = nullptr;

	notify(GUIDE_NOTIFY_START, nullptr);
	notify(GUIDE_NOTIFY_END, nullptr);

	return false;
	
}

GuideNode* GuideCenter::isGuideExecutable(int interfaceId)
{
	mTriggerInterfaceId = interfaceId;

	GuideNode* ret = nullptr;

	//判定所有链条上的当前结点是否可执行
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

//触发下个引导
void GuideCenter::next()
{
	if (mCurrentGuideChain)
	{
		GuideNode* currentGuideNode = mCurrentGuideChain->mCurrentGuidNode;
		if (currentGuideNode)
		{
			//如果当前结点的下个界面与下个结点的触发界面相同，则切换界面，以执行下个引导
			if (currentGuideNode->mNextNode && currentGuideNode->mNextInterfaceId != -1 && 
				currentGuideNode->mNextInterfaceId == currentGuideNode->mNextNode->mTriggerInterfaceId)
			{
				mTriggerInterfaceId = currentGuideNode->mNextNode->mTriggerInterfaceId;
				mTriggerType = currentGuideNode->mNextNode->mTriggerType;
			}

			switch (currentGuideNode->mGuideType)
			{

			case GUIDE_NODE_GUIDE: //引导类型直接执行并记录下个结点
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
						//下个结点不能执行，通知结束
						notify(GUIDE_NOTIFY_END, nullptr);
					}
				}
				break;

			case GUIDE_NODE_CHAT: 
				{
					//对话类型需判定对话是否执行结束，如未执行结束，仍然停留在当前结点，如执行结束，记录并执行下个结点
					currentGuideNode->mCurrChatIndex++;
					if (!currentGuideNode->mVecChatId.empty() 
						&& currentGuideNode->mCurrChatIndex < currentGuideNode->mVecChatId.size())
					{
						//对话未执行结束，继续执行
						notify(GUIDE_NOTIFY_NEXT, mCurrentGuideChain->mCurrentGuidNode);
					}
					else
					{
						mCurrentGuideChain->mCurrentGuidNode = currentGuideNode->mNextNode;
						//对话执行结束，执行下个结点
						if (isGuideNodeCanTrigger(currentGuideNode->mNextNode))
						{
							notify(GUIDE_NOTIFY_NEXT, mCurrentGuideChain->mCurrentGuidNode);
						}
						else
						{
							//下个结点不能执行，通知结束
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

	//该阶段引导结束，清空所有标记
	if (flag == GUIDE_NOTIFY_END)
	{
		mTriggerType = -1;
		mTriggerInterfaceId = -1;
		mTriggerStageId = -1;
		mTriggerRound = -1;
	}
}

//设置战斗开始触发器
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

//设置战斗回合触发器
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

//设置战斗结束触发器
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
				//等级引导直接判定等级是否达到
				int level = HeroModel::getInstance()->getMainHero()->mLevel;
				return (level >= guideNode->mTriggerLvRound);
			}
		case GUIDE_NODE_TRIGGER_FIGHT_BEFORE:
		case GUIDE_NODE_TRIGGER_FIGHT_START:
		case GUIDE_NODE_TRIGGER_FIGHT_END:
			{
				//战斗引导需判定触发类型与关卡id
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

