#include "SceneFightLoading.h"
#include "Widget/LayerLoading.h"
#include "Widget/LayerPvpLoading.h"
#include "Utils/ResourceUtils.h"
#include "SceneFight.h"
#include "ResourceManager/ResourceLoader.h"
#include "DataManager/DataManager.h"
#include "LayerFightBack.h"

using namespace std;

SceneFightLoading::SceneFightLoading()
	: mBarrierId(0)
	, mFightType(FIGHT_TYPE::FIGHT_TYPE_PVE)
	, mResourceLoader(nullptr)
{
	FightModel::getInstance()->addObserver(this);
	mResourceLoader = ResourceLoader::getInstance();
}

SceneFightLoading::~SceneFightLoading()
{
	FightModel::getInstance()->removeObserver(this);
}

SceneFightLoading* SceneFightLoading::create(FIGHT_TYPE fightType, int barrierId)
{
	SceneFightLoading* pRet = new SceneFightLoading();
	if (pRet!=nullptr && pRet->init(fightType, barrierId))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool SceneFightLoading::init(FIGHT_TYPE fightType, int barrierId)
{
	if (!Scene::init())
	{
		return false;
	}

	//初始化战斗类型
	initFightType(fightType, barrierId);

// 	if (fightType == FIGHT_TYPE_PVP_ARENA)
// 	{
// 		LayerPvpLoading *layerPvpLoading = LayerPvpLoading::create();
// 		addChild(layerPvpLoading);
// 	}
// 	else
	{
		LayerLoading* layerLoading = LayerLoading::create();
		addChild(layerLoading);
	}

	return true;
}

void SceneFightLoading::initFightType(FIGHT_TYPE fightType, int barrierId)
{
	mFightType = fightType;
	mBarrierId = barrierId;

	RowData* rowData = nullptr;

	switch (fightType)
	{
	case FIGHT_TYPE_INVALID:
		break;
	case FIGHT_TYPE_PVE:
	case FIGHT_TYPE_PVE_ACTIVITY:
	case FIGHT_TYPE_MAZE_TRUE:
	case FIGHT_TYPE_TOWER:
	case FIGHT_TYPE_GUIDE:
	case FIGHT_TYPE_GROUP_BOSS:
		{
			rowData = DataManager::getInstance()->searchBarrierById(mBarrierId);
			if (rowData)
			{
				int bgId = rowData->getIntData("bgId");
				int winCond = rowData->getIntData("winCon");
				int winCondCount = rowData->getIntData("amount");
				FightModel::getInstance()->setBgResId(bgId);
				FightModel::getInstance()->setWinCondAndCount(winCond, winCondCount);
			}
		}
		break;

	case FIGHT_TYPE_PVP_ARENA:
		{
			RowData* rowData = DataManager::getInstance()->searchToolSystemById(10053);
			if (rowData)
			{
				FightModel::getInstance()->setBgResId(rowData->getIntData("value"));
			}
		}
		break;
	
	case FIGHT_TYPE_ROB_FIGHT:
		{
			RowData* rowData = DataManager::getInstance()->searchToolSystemById(10054);
			if (rowData)
			{
				FightModel::getInstance()->setBgResId(rowData->getIntData("value"));
			}
		}
		break;

	case FIGHT_TYPE_TRAIL:
		{
			RowData* rowData = DataManager::getInstance()->searchToolSystemById(10055);
			if (rowData)
			{
				FightModel::getInstance()->setBgResId(rowData->getIntData("value"));
			}
		}
		break;

	case FIGHT_TYPE_FIND:
		{
			// 情缘战斗场景资源;
			RowData* rowData = DataManager::getInstance()->searchToolSystemById(10092);
			if (rowData)
			{
				FightModel::getInstance()->setBgResId(rowData->getIntData("value"));
			}
		}
		break;
	case FIGHT_TYPE_BOSS:
		{
			//世界boss战斗场景
			RowData* rowData = DataManager::getInstance()->searchToolSystemById(10098);
			if (rowData)
			{
				FightModel::getInstance()->setBgResId(rowData->getIntData("value"));
			}
		}
		break;
	default:
		break;
	}

	//初始化关卡场景切换数据
	if (fightType == FIGHT_TYPE_PVE || fightType == FIGHT_TYPE_GUIDE)
	{
		if (rowData)
		{
			int bgCondition = rowData->getIntData("condition");
			int bgRound = rowData->getIntData("sceneRound");
			int bgChangeId = rowData->getIntData("newBgId");

			FightModel::getInstance()->setChangeBgCond(bgCondition);
			FightModel::getInstance()->setChangeBgRound(bgRound);
			FightModel::getInstance()->setChangeBgResId(bgChangeId);
		}
	}
}

void SceneFightLoading::onEnter()
{
	Scene::onEnter();

	//移除不再使用的图片，以减少内存使用
	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	//log(Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}

void SceneFightLoading::updateSelf(void* data)
{
	startAsyncLoading();
}

void SceneFightLoading::startAsyncLoading()
{
	std::set<int> heroResIdSet;
	std::set<int> skillResIdSet;
	
	//我方/敌方/助战角色
	addFighterAndSkillRes(FightModel::getInstance()->getHeroList(), heroResIdSet, skillResIdSet);
	addFighterAndSkillRes(FightModel::getInstance()->getMonsterList(), heroResIdSet, skillResIdSet);
	addFighterAndSkillRes(FightModel::getInstance()->getAssistList(), heroResIdSet, skillResIdSet);

	//角色资源
	for (int fighterResId : heroResIdSet)
	{
		addFighterResPath(fighterResId);
	}

	//技能资源
	for (int skillResId : skillResIdSet)
	{
		addSkillResPath(skillResId);
	}

	//UI资源
	mResourceLoader->addAsyncUIRes("UInewfight");
	mResourceLoader->addAsyncUIRes("AniBeforeManuskl2");
	mResourceLoader->addAsyncUIRes("AniP3Flag");
	mResourceLoader->addAsyncUIRes("AniComboCF0");
	mResourceLoader->addAsyncUIRes("AnicomboEnemy");
	mResourceLoader->addAsyncUIRes("AniComBoBuff");

	//背景图资源
	addBgResPath(FightModel::getInstance()->getBgResId());
	addBgResPath(FightModel::getInstance()->getChangeBgResId());

	//启动异步加载
	mResourceLoader->startAsyncLoading(CC_CALLBACK_0(SceneFightLoading::onAsyncLoadedCallBack, this));
}

//添加角色资源
void SceneFightLoading::addFighterResPath(int resId)
{
	mResourceLoader->addAsyncImageRes(ResourceUtils::getHeroCompleteCardPath(resId));
	string rootPath = ResourceUtils::getFighterSkeletonPath(resId);
	stringstream ss;

	for(int i = 0; ;i++)
	{
		ss.clear();
		ss.str("");
		ss << rootPath << resId;
		if (i == 0)
		{
			ss << ".png";
		}
		else
		{
			ss << (i + 1) << ".png";
		}

		if (!mResourceLoader->addAsyncImageRes(ss.str()))
		{
			return;
		}
	}
	
}

//添加技能资源
void SceneFightLoading::addSkillResPath(int resId)
{
	string rootPath = ResourceUtils::getSkillSkeletonPath(resId);
	stringstream ss;

	for(int i = 0; ;i++)
	{
		ss.clear();
		ss.str("");
		ss << rootPath << resId;
		if (i == 0)
		{
			ss << ".png";
		}
		else
		{
			ss << (i + 1) << ".png";
		}

		if (!mResourceLoader->addAsyncImageRes(ss.str()))
		{
			return;
		}
	}
}

void SceneFightLoading::addBgResPath(int resId)
{
	if (resId > 0)
	{
		string rootPath = ResourceUtils::getFightBgPath(resId);

		mResourceLoader->addAsyncImageRes(rootPath + BG_NAME_B1);
		mResourceLoader->addAsyncImageRes(rootPath + BG_NAME_B2);
		mResourceLoader->addAsyncImageRes(rootPath + BG_NAME_F1);
		mResourceLoader->addAsyncImageRes(rootPath + BG_NAME_F2);
		mResourceLoader->addAsyncImageRes(rootPath + BG_NAME_F3);
	}
}

void SceneFightLoading::addFighterAndSkillRes(Vector<Fighter*>* vecFighter, std::set<int>& fighterSet, std::set<int>& skillSet)
{
	for (int i = 0; i < vecFighter->size(); i++)
	{
		Fighter* fighter = vecFighter->at(i);

		if (!fighter) break;

		//角色资源id添加
		fighterSet.insert(fighter->getResID());

		//技能资源id添加
		if (fighter->getNormalSkill())
		{
			skillSet.insert(fighter->getNormalSkill()->getSkillEffectParam()->nPreResID);
			skillSet.insert(fighter->getNormalSkill()->getSkillEffectParam()->nActResID);
		}
		if (fighter->getActiveSkill())
		{
			skillSet.insert(fighter->getActiveSkill()->getSkillEffectParam()->nPreResID);
			skillSet.insert(fighter->getActiveSkill()->getSkillEffectParam()->nActResID);
		}
		if (fighter->getRushSkill())
		{
			skillSet.insert(fighter->getRushSkill()->getSkillEffectParam()->nPreResID);
			skillSet.insert(fighter->getRushSkill()->getSkillEffectParam()->nActResID);
		}
// 		if (fighter->getBuffSkill())
// 		{
// 			skillSet.insert(fighter->getBuffSkill()->getSkillEffectParam()->nPreResID);
// 			skillSet.insert(fighter->getBuffSkill()->getSkillEffectParam()->nActResID);
// 		}
		if (fighter->getTriggerSkill())
		{
			skillSet.insert(fighter->getTriggerSkill()->getSkillEffectParam()->nPreResID);
			skillSet.insert(fighter->getTriggerSkill()->getSkillEffectParam()->nActResID);
		}
	}
}


void SceneFightLoading::onAsyncLoadedCallBack()
{
	//异步加载结束，切换到战斗场景
	SceneFight* sceneFight = SceneFight::create(mFightType, mBarrierId);
	Director::getInstance()->replaceScene(sceneFight);
	//log(Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}