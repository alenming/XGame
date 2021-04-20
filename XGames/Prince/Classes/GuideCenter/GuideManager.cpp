#include "GuideManager.h"
#include "GuideLayer.h"
#include "ChatLayer.h"
#include "DataManager/DataManager.h"
#include "FightModule/FightController.h"
#include "FightModule/SceneFightLoading.h"
#include "HeroModule/HeroModel.h"
#include "Temp/CustomPop.h"
#include "Temp/CustomTips.h"
#include "AnimationLayer.h"
#include "MainModule/MainCityScene.h"
#include <sstream>
#include <limits>

GuideManager* GuideManager::mInstance = nullptr;

GuideManager::GuideManager()
	: mStartGuideCallBack(nullptr)
	, mEndGuideCallBack(nullptr)
	, mCurrentGuideNode(nullptr)
	, mIsInitProcess(false)
{
	mGuideCenter = GuideCenter::create();
	mGuideCenter->addObserver(this);
	mGuideCenter->retain();
}

GuideManager::~GuideManager()
{
	mGuideCenter->release();
}

GuideManager* GuideManager::getInstance()
{
	if (!mInstance)
	{
		mInstance = new GuideManager();
	}
	return mInstance;
}

void GuideManager::destroyInstance()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

void GuideManager::initGuideProcess(const Json::Value& data)
{
	if (!mIsInitProcess)
	{
		for (int i = 0; i < data.size(); i++)
		{
			int chainId = data[i]["chainId"].asInt();
			int groupId = data[i]["groupId"].asInt();
			mGuideCenter->initGuideChainProcess(chainId, groupId);
		}

// 		for (int i = 1; i < 9; i++)
// 		{
// 			mGuideCenter->initGuideChainProcess(i, 1000);
// 		}

		mIsInitProcess = true;
	}
}

void GuideManager::updateSelf(void* data)
{
	ObserverParam observerParam = *((ObserverParam*) data);
	mCurrentGuideNode = (GuideNode*) observerParam.updateData;

	switch (observerParam.id)
	{

	case GUIDE_NOTIFY_START:
		if (mStartGuideCallBack != nullptr)
		{
			mStartGuideCallBack(mCurrentGuideNode);
			mStartGuideCallBack = nullptr;
		}
		break;

	case GUIDE_NOTIFY_NEXT:
		break;

	case GUIDE_NOTIFY_END:
		if (mEndGuideCallBack != nullptr)
		{
			mEndGuideCallBack(mCurrentGuideNode);
			mEndGuideCallBack = nullptr;
		}
		break;

	default:
		break;
	}


	if (mCurrentGuideNode)
	{
		CCLOG("========chainId=%d, groupId=%d, nodeId=%d, triggerInterfaceId=%d, nextInterfaceId=%d, guideRes=%s========", 
			mCurrentGuideNode->mChainId, mCurrentGuideNode->mGroupId, mCurrentGuideNode->mNodeId, 
			mCurrentGuideNode->mTriggerInterfaceId, mCurrentGuideNode->mNextInterfaceId, mCurrentGuideNode->mGuideResName.c_str());

		switch (mCurrentGuideNode->mGuideType)
		{

		case GUIDE_NODE_GUIDE:
			showGuide();
			break;

		case GUIDE_NODE_CHAT:
			showChat();
			break;

		case GUIDE_NODE_FIGHT:
			showGuideFight();
			break;

		case GUIDE_NODE_EMPTY:
			showGuideEmpty();

		case GUIDE_NODE_ANIMATION:
			showGuideAnimation();

		default:
			break;
		}
	}
}

void GuideManager::showGuide()
{
	GuideNode* guideNode = mCurrentGuideNode;

	if (guideNode)
	{
		//临时处理
		auto glview = Director::getInstance()->getOpenGLView();
		Size frameSize = glview->getFrameSize();
		float frameScale = frameSize.width / frameSize.height;
		string resName = "";
		if (!guideNode->mGuideResName2.empty())
		{
			resName = abs(frameScale - 3.0f / 2.0f) < 0.0001f ? guideNode->mGuideResName2 : guideNode->mGuideResName;
		}
		else
		{
			resName = guideNode->mGuideResName;
		}
		
		GuideLayer* guideLayer = GuideLayer::create(
			resName, guideNode->mIsForce, guideNode->mIsKeyNode);
		Director::getInstance()->getRunningScene()->addChild(guideLayer, INT_MAX);
	}
}

//显示对话
void GuideManager::showChat()
{
	GuideNode* guideNode = mCurrentGuideNode;

	if (guideNode)
	{
		int chatId = guideNode->mVecChatId.at(guideNode->mCurrChatIndex);
		RowData* rowData = DataManager::getInstance()->searchGuideChatById(chatId);

		string npcName = rowData->getStringData("NpcName");
		string npcContent = rowData->getStringData("info");
		int npcId = rowData->getIntData("Npc");
		int npcDir = rowData->getIntData("NpcPos");

		ChatLayer* chatLayer = ChatLayer::create(npcContent, npcName, npcId, npcDir);
		Director::getInstance()->getRunningScene()->addChild(chatLayer, INT_MAX);
	}
}

void GuideManager::showGuideFight()
{
	if (mCurrentGuideNode)
	{
		int stageId = mCurrentGuideNode->mTriggerStageId;
		auto fightLoadingScene = SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_GUIDE, stageId);
		Director::getInstance()->replaceScene(fightLoadingScene);
		FightController::getInstance()->sendCommonFightMsg(stageId);
		mGuideCenter->next();
	}
}

void GuideManager::showGuideEmpty()
{
	//mGuideCenter->next();
	next();
}

void GuideManager::showGuideAnimation()
{
	//mark 待补全
	if (mCurrentGuideNode)
	{
		AnimationLayer* aniLayer = AnimationLayer::create(
			mCurrentGuideNode->mGuideResName, mCurrentGuideNode->mGuideResName2);
		Director::getInstance()->getRunningScene()->addChild(aniLayer, INT_MAX);
	}
}

//开始引导 
bool GuideManager::startGuide(int interfaceId)
{
	return mGuideCenter->start(interfaceId);
}

void GuideManager::startGuideNextFrame(int interfaceId)
{
	//隔一帧再执行引导，防止在onEnter中直接调用导致迭代器破坏崩溃;
	auto callBack = [this, interfaceId](float dt)->void
	{
		GuideManager::getInstance()->startGuide(interfaceId);
		Director::getInstance()->getScheduler()->unschedule("GuideManager::startGuideNextFrame", this);
	};
	Director::getInstance()->getScheduler()->schedule(callBack, this, 0, 0, 0, false, "GuideManager::startGuideNextFrame");
}

//战斗开始引导
bool GuideManager::startFightStartGuide(int interfaceId, int stageId)
{
	mGuideCenter->setFightStartTrigger(stageId);
	return mGuideCenter->start(interfaceId);
}

bool GuideManager::startFightBeforeGuide(int interfaceId, int stageId)
{
	mGuideCenter->setFightBeforeTrigger(stageId);
	return mGuideCenter->start(interfaceId);
}

//战斗回合引导
bool GuideManager::startFightRoundGuide(int interfaceId, int stageId, int roundCount)
{
	mGuideCenter->setRoundTrigger(stageId, roundCount);
	return mGuideCenter->start(interfaceId);
}

bool GuideManager::startFightRoundEndGuide(int interfaceId, int stageId, int roundCount)
{
	mGuideCenter->setRoundEndTrigger(stageId, roundCount);
	return mGuideCenter->start(interfaceId);
}

bool GuideManager::startFightEnemyBeforeGuide(int interfaceId, int stageId, int roundCount)
{
	mGuideCenter->setEnemyBeforeTrigger(stageId, roundCount);
	return mGuideCenter->start(interfaceId);
}

//战斗结束引导
bool GuideManager::startFightEndGuide(int interfaceId, int stageId)
{
	mGuideCenter->setFightEndTrigger(stageId);
	return mGuideCenter->start(interfaceId);
}

void GuideManager::next()
{
	bool isFinalGuide = mCurrentGuideNode && 
		(mCurrentGuideNode->mGuideType == GUIDE_NODE_GUIDE || 
		(mCurrentGuideNode->mGuideType == GUIDE_NODE_CHAT && 
		mCurrentGuideNode->mCurrChatIndex == mCurrentGuideNode->mVecChatId.size() - 1));

	//引导类型如果是关键结点，直接发送记录；对话类型如果是关键结点，需判断对话是否结束方可发送记录
	if (isFinalGuide && mCurrentGuideNode->mIsKeyNode)
	{
		GuideController::sendGuideProcess(mCurrentGuideNode->mChainId, mCurrentGuideNode->mGroupId);
	}

	//对话结束或者引导结束，如果设置为界面跳转结点，则直接跳转到主场景
	if (isFinalGuide && mCurrentGuideNode->mJumpToMain == GUIDE_NODE_JUMP)
	{
		mGuideCenter->next();
		Director::getInstance()->popToRootScene();
		Director::getInstance()->replaceScene(MainCityScene::create());
	}
	else
	{
		mGuideCenter->next();
	}
	
}

void GuideManager::setCallBack(const StartGuideCallBack& start, const EndGuideCallBack& end)
{
	this->mStartGuideCallBack = start;
	this->mEndGuideCallBack = end;
}

GuideNode* GuideManager::isGuideExecutable(int interfaceId)
{
	return mGuideCenter->isGuideExecutable(interfaceId);
}

bool GuideManager::isFunctionOpen(FUNCTION_TYPE type, bool showTips)
{
	bool ret = false;
	RowData* rowData = DataManager::getInstance()->searchFunctionLevelById(type);
	if (rowData)
	{
		int currentLevel = HeroModel::getInstance()->getMainHero()->mLevel;
		int functionLevel = rowData->getIntData("avlLv");
		if (currentLevel >= functionLevel)
		{
			ret = true;
		}
		else
		{
			ret = false;

			if (showTips)
			{
				stringstream ss;
				//ss << "等级达到" << functionLevel << "级开放";
				ss << DataManager::getInstance()->searchCommonTexdtById(10218) << functionLevel 
					<< DataManager::getInstance()->searchCommonTexdtById(10001);
				CustomTips::showUtf8(ss.str());
			}
		}
	}
	return ret;
}