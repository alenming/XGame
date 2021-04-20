#include "SceneFight.h"
#include "LayerFighter.h"
#include "LayerFightInfo.h"
#include "LayerManualSkill.h"
#include "ChapterModule/SceneChapter.h"
#include "ChapterModule/ChapterModel.h"
#include "ChapterModule/BarrierModel.h"
#include "ChapterModule/Activity/ActivityModel.h"
#include "ChapterModule/Activity/SceneActivity.h"
#include "DataManager/DataManager.h"
#include "Widget/LayerLoading.h"
#include "FightModel.h"
#include "ResourceManager/ResourceLoader.h"
#include "Common/Common.h"
#include "MainModule/MainModel.h"
#include "FightController.h"

#include "MazeModule/MazeController.h"
#include "MazeModule/MazeModel.h"

#include "RobModule/RobController.h"
#include "RobModule/RobModel.h"
#include "GuideCenter/GuideManager.h"
#include "MainModule/MainCityLoadingScene.h"

#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "SimpleAudioEngine.h"

#include "ChapterModule/SceneChapterLoading.h"
#include "MainModule/MainCityScene.h"
#include "TowerModule/SceneTower.h"
#include "PvpModule/PvpScene.h"
#include "MazeModule/SceneMaze.h"
#include "MazeModule/LayerMaze.h"
#include "MarvellousActive/MarvellousActiveScene.h"
#include "BossModule/BossModel.h"
#include "Utils/SystemUtils.h"
#include "TowerModule/TowerModel.h"
#include "SDK_TalkingData/TalkingData.h"
#include "GroupBossModule/GroupBossModel.h"
#include "GroupBossModule/GroupBossScene.h"

#include <time.h>

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;
using namespace CocosDenshion;


SceneFight::SceneFight()
	: m_nFightType(FIGHT_TYPE::FIGHT_TYPE_INVALID)
	, m_nBarrierId(-1)
	, m_layerSettlement(nullptr)
	, mIsWin(false)
	, mStarCount(0)
	, m_pLayerMask(nullptr)
	, mRushBackAni(nullptr)
	, mTrailCurrFloor(-1)
	, mIsSkip(false)
{

}

SceneFight::~SceneFight()
{
	FightModel::destroyInstance();
	FightController::destroyInstance();
}

SceneFight* SceneFight::create(FIGHT_TYPE nFightType, int nBarrierId)
{
	SceneFight* pRet = new SceneFight();
	if (pRet!=nullptr && pRet->init(nFightType, nBarrierId))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool SceneFight::init(FIGHT_TYPE nFightType, int nBarrierId)
{
	if (!Scene::init())
	{
		return false;
	}

	m_nFightType = nFightType;
	m_nBarrierId = nBarrierId;

	Size winSize = Director::getInstance()->getWinSize();

	//背景层
	m_pLayerFighter = LayerFighter::create();
	int bgId = FightModel::getInstance()->getBgResId();
	mLayerFightBack = LayerFightBack::create(bgId, m_pLayerFighter);
	this->addChild(mLayerFightBack);

	//蒙板层，追打动画开始时显示
	m_pLayerMask = LayerColor::create(Color4B::BLACK);
	m_pLayerMask->setOpacity(0);
	this->addChild(m_pLayerMask);

	//追打动画开始是显示的动画
	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/AnicomboBJ1/AnicomboBJ1.ExportJson");
	mRushBackAni = Armature::create("AnicomboBJ1");
	mRushBackAni->setPosition(this->getContentSize() / 2);
	mRushBackAni->setVisible(false);
	//mRushBackAni->getAnimation()->play("Animation1");
	this->addChild(mRushBackAni);

	//战斗层
	this->addChild(m_pLayerFighter);

	//战斗信息层
	m_pLayerFightInfo = LayerFightInfo::create();
	this->addChild(m_pLayerFightInfo);

	//主动技能层
	m_pLayerSkill  = LayerManualSkill::create();
	this->addChild(m_pLayerSkill);

	initFSM();

	initFightType();

	initRandomSeed();

	return true;
}

void SceneFight::initRandomSeed()
{
	time_t nowTime;
	time(&nowTime);
	srand(nowTime);
}

void SceneFight::initFightType()
{
	switch (m_nFightType)
	{
	case FIGHT_TYPE_PVE:
	case FIGHT_TYPE_PVE_ACTIVITY:
	case FIGHT_TYPE_FIND:
		{
			//PVE战斗显示自动手动按钮, 不显示跳过按钮, 显示逃跑, 显示加速按钮
			m_pLayerFightInfo->setSpeedBtnVisible(true);
			m_pLayerFightInfo->setSkipEscapeBtnVisible(false, true);
			bool autoFight = GetBoolFromXML(LOCAL_KEY_BOOL_AUTO_FIGHT, false);
			m_pLayerFightInfo->setAutoFight(autoFight);
		}
		break;
	case FIGHT_TYPE_TOWER:
		{
			//爬塔显示自动手动按钮, 不显示逃跑按钮, 显示跳过按钮, 显示加速按钮
			m_pLayerFightInfo->setSpeedBtnVisible(true);
			m_pLayerFightInfo->setSkipEscapeBtnVisible(true, false);
			bool autoFight = GetBoolFromXML(LOCAL_KEY_BOOL_AUTO_FIGHT, false);
			m_pLayerFightInfo->setAutoFight(autoFight);
		}
		break;
	case FIGHT_TYPE_GROUP_BOSS:
		{
			//帮派Boss显示自动手动按钮, 不显示逃跑按钮, 显示跳过按钮, 显示加速按钮
			m_pLayerFightInfo->setSpeedBtnVisible(true);
			m_pLayerFightInfo->setSkipEscapeBtnVisible(true, false);
			bool autoFight = GetBoolFromXML(LOCAL_KEY_BOOL_AUTO_FIGHT, false);
			m_pLayerFightInfo->setAutoFight(autoFight);
			m_pFSM->setMaxRoundCount(5); //帮派Boss战斗最大回合数为5，超过5回合则强制结束战斗
		}
		break;
	case FIGHT_TYPE_MAZE_TRUE:
		{
			//PVE战斗显示自动手动按钮, 不显示逃跑, 显示跳过，显示加速按钮
			m_pLayerFightInfo->setSpeedBtnVisible(true);
			m_pLayerFightInfo->setSkipEscapeBtnVisible(true, false);

			bool autoFight = GetBoolFromXML(LOCAL_KEY_BOOL_AUTO_FIGHT, false);
			m_pLayerFightInfo->setAutoFight(autoFight);
		}
		break;
	case FIGHT_TYPE_ROB_FIGHT:
	case FIGHT_TYPE_PVP_ARENA:
		{
			//PVP战斗，手动按钮灰态, 不可点击手动按钮，显示跳过按钮，不显示逃跑按钮，显示加速按钮
			m_pLayerFightInfo->setSpeedBtnVisible(true);
			m_pLayerFightInfo->setAutoFight(true, true);
			m_pLayerFightInfo->setSkipEscapeBtnVisible(true, false);
			m_pFSM->setPVP(true);

			break;
		}
	case FIGHT_TYPE_GUIDE:
		{
			//新手引导战斗不能跳过、不能自动、不能加速, 默认战斗速度x1, 默认手动
			m_pLayerFightInfo->setSpeedBtnVisible(false);
			m_pLayerFightInfo->setSkipEscapeBtnVisible(false, false);
			m_pLayerFightInfo->setAutoFight(false, true);

			//新手引导战斗1.2倍速率
			Director::getInstance()->getScheduler()->setTimeScale(1.2);
		}
		break;
	case FIGHT_TYPE_TRAIL:
		{
			//试练PVP战斗，显示自动手动按钮，显示加速，显示逃跑，显示跳过
			m_pLayerFightInfo->setSpeedBtnVisible(true);
			m_pLayerFightInfo->setSkipEscapeBtnVisible(true, true);

			m_pFSM->setPVP(true);

			bool autoFight = GetBoolFromXML(LOCAL_KEY_BOOL_AUTO_FIGHT, false);
			m_pLayerFightInfo->setAutoFight(autoFight);
		}
		break;
	case FIGHT_TYPE_BOSS:
		{
			//显示手动/自动按钮，加速锁定为x3，不能跳过，不能逃跑
			m_pLayerFightInfo->setSkipBtnVisible(false);
			m_pLayerFightInfo->setEscapeBtnVisible(false);
			m_pLayerFightInfo->setFightSpeed(3, true);
			m_pLayerFightInfo->setFightSpeedBtnHide();

			bool autoFight = GetBoolFromXML(LOCAL_KEY_BOOL_AUTO_FIGHT, false);
			m_pLayerFightInfo->setAutoFight(autoFight);
		}
	default:
		break;
	}

	if (m_nFightType != FIGHT_TYPE_GUIDE && m_nFightType != FIGHT_TYPE_BOSS)
	{
		int speed = GetIntegerFromXML(LOCAL_KEY_INT_STR_FIGHT_SPEED, 1);
		m_pLayerFightInfo->setFightSpeed(speed);
	}
}

void SceneFight::onEnter()
{
	Scene::onEnter();
	SystemUtils::setScreenLight(true); //设置屏幕常量
	playFightMusic();	  //播放背景音乐
	onBeginTalkingData(); //数据上报

	//下一帧执行引导
	getScheduler()->schedule(CC_CALLBACK_1(SceneFight::onFightEnter, this), this, 0, 0, 0, false, "SceneFight::onEnter");
}

void SceneFight::onFightEnter(float dt)
{
	GuideManager::getInstance()->setCallBack(nullptr, [this](GuideNode* guideNode)->void
	{
		m_pLayerFighter->startEntranceAnimation(CC_CALLBACK_0(SceneFight::startGuide, this));
	});
	GuideManager::getInstance()->startFightBeforeGuide(GUIDE_INTERFACE_FIGHT, m_nBarrierId);
}

void SceneFight::onExit()
{
	Scene::onExit();
	m_pFSM->release();

	GUIReader::destroyInstance();
	ArmatureDataManager::destroyInstance();
	DictionaryHelper::getInstance()->destroyInstance();
	SpriteFrameCache::getInstance()->destroyInstance();
	Director::getInstance()->getTextureCache()->removeUnusedTextures();

	Director::getInstance()->getScheduler()->setTimeScale(1.0f);
	SystemUtils::setScreenLight(false); //关闭屏幕常量
}

void SceneFight::onFightStart()
{
	if (mIsSkip) return;
}

void SceneFight::onRoundStart(vector<AssistFighterInfo>& vecAssistInfo)
{
	if (mIsSkip) return;

	//更新UI，清空引用计数
	m_pLayerFightInfo->updateUI();
	m_pLayerSkill->updateUI();	m_pLayerFightInfo->clearHitCount();
	m_pLayerFightInfo->playRoundAni();
	m_pLayerFighter->onRoundStart();
	

	auto guideCallBack = [this]()->void
	{
		//角色入场后需要重新创建主动技能栏
		m_pLayerSkill->initSkill();

		//播放引导的过程中暂停状态机，引导播放完毕再启动状态机
		int round = FightModel::getInstance()->getRoundCount();
		GuideManager::getInstance()->setCallBack(
			[this](GuideNode* guideNode)->void{ m_pFSM->wait(); }, 
			[this](GuideNode* guideNode)->void
			{ 
				m_pFSM->resume();
				//引导执行完毕，播放场景切换动画
				if (FightModel::getInstance()->getChangeBgCond() == BG_COND_ROUND
					&& FightModel::getInstance()->getChangeBgRound() == FightModel::getInstance()->getRoundCount())
				{
					//场景切换先暂停状态机，切换完毕恢复状态机
					m_pFSM->wait();
					int resId = FightModel::getInstance()->getChangeBgResId();
					mLayerFightBack->changeWithFade(resId, [this](){ m_pFSM->resume(); });
				}
				
			}
		);
		GuideManager::getInstance()->startFightRoundGuide(GUIDE_INTERFACE_FIGHT, m_nBarrierId, round);
	};

	if (vecAssistInfo.empty())
	{
		//状态机短暂暂停后恢复
		pauseFSM(1.0f, guideCallBack);
	}
	else
	{
		//如果助战者队列不为空，先跳入战场，再播放引导，最后场景切换，注意每一步都需要wait/resume状态机
		auto assistCallBack = [this, vecAssistInfo, guideCallBack]()
		{
			m_pFSM->wait();
			m_pLayerFighter->startAssistEntrance(vecAssistInfo, guideCallBack); 
		};
		pauseFSM(1.0f, assistCallBack);
	}
	
}

void SceneFight::onAttack(SkillResultParam* resultParam, SKILL_EFFECT* effectParam)
{
	if (mIsSkip) return;

	m_pFSM->wait();
	m_pLayerFighter->onAttackStart(resultParam, effectParam);
}

void SceneFight::onChangeSide(FightStateMachine::E_SIDE currSide, bool isPvp)
{
	if (mIsSkip) return;

	//处理换边引导
	if (!isPvp)
	{
		//执行场景变换，场景变换后执行引导
		pauseFSM(0.5f, [this]()
		{
			GuideManager::getInstance()->setCallBack(
				[this](GuideNode* guideNode)->void{ m_pFSM->wait(); }, 
				[this](GuideNode* guideNode)->void
				{ 
					m_pFSM->resume();
					//引导结束回调再执行场景切换
					if (FightModel::getInstance()->getChangeBgCond() == BG_COND_ROUND_BEFORE_ENEMY
						&& FightModel::getInstance()->getChangeBgRound() == FightModel::getInstance()->getRoundCount())
					{
						//场景切换先暂停状态机，切换完毕恢复状态机
						m_pFSM->wait();
						int resId = FightModel::getInstance()->getChangeBgResId();
						mLayerFightBack->changeWithFade(resId, [this](){ m_pFSM->resume(); });
					}
				});
			int round = FightModel::getInstance()->getRoundCount();
			GuideManager::getInstance()->startFightEnemyBeforeGuide(GUIDE_INTERFACE_FIGHT, m_nBarrierId, round);
		});
	}
	else
	{
		pauseFSM(0.3f);
	}
}

void SceneFight::onRoundEnd(RoundParam buffRoundHarm)
{
	if (mIsSkip) return;

	m_pLayerFighter->onRoundEnd(buffRoundHarm);
	//m_pLayerFightInfo->updateUI();

	auto guideCallBack = [this]()
	{
		GuideManager::getInstance()->setCallBack(
			[this](GuideNode* guideNode)->void{ m_pFSM->wait(); }, 
			[this](GuideNode* guideNode)->void{ m_pFSM->resume(); }
		);
		int round = FightModel::getInstance()->getRoundCount();
		GuideManager::getInstance()->startFightRoundEndGuide(GUIDE_INTERFACE_FIGHT, m_nBarrierId, round);
	};

	//状态机短暂暂停后执行引导
	pauseFSM(0.5f, guideCallBack);

}

void SceneFight::onFightEnd(bool isWin, int starCount)
{
	CCLOG("SceneFight::onFightEnd() begin");

	if (mIsSkip)
	{
		//跳过后还原速度倍率
		Director::getInstance()->getScheduler()->setTimeScale(1.0f);

		//跳过后隐藏所有死亡角色
		m_pLayerFighter->setDeadFighterUnvisible();

		//更新所有角色的血条
		m_pLayerFighter->updateAllFighterHpBar();
	}
	

	mIsWin = isWin;
	mStarCount = starCount;

	auto callBackFightEnd = [this](float dt)->void
	{
		//播放完引导再展示战斗结算界面，必须战斗胜利才能展示引导
		if (mIsWin)
		{
			GuideManager::getInstance()->setCallBack(nullptr, 
				[this](GuideNode* guideNode)->void{ doFightEnd(); });
			GuideManager::getInstance()->startFightEndGuide(GUIDE_INTERFACE_FIGHT, m_nBarrierId);
		}
		else
		{
			doFightEnd();
		}
	};

	auto callBackWinAni = [this](float dt)->void
	{
		//隐藏UI层，并移除所有人身上的buff
		m_pLayerFightInfo->setVisible(false);
		m_pLayerSkill->setVisible(false);
		m_pLayerMask->setVisible(false);
		mRushBackAni->setVisible(false);
		m_pLayerFighter->removeAllUIBuff();

		m_pLayerFighter->startWinAnimation(
			mIsWin ? FightStateMachine::E_SIDE::MY_SIDE : FightStateMachine::E_SIDE::ENEMY_SIDE);
	};

	//暂停一秒后播放胜利动画
	getScheduler()->schedule(callBackWinAni, this, 0, 0, 1.0f, false, "callBackWinAni");

	//暂停两秒后结算
	getScheduler()->schedule(callBackFightEnd, this, 0, 0, 2.0f, false, "callBackFightEnd");

}

void SceneFight::doFightEnd()
{
	// 战斗结算;
	if (m_nFightType == FIGHT_TYPE_GUIDE)
	{
		//新手引导战斗结束后跳转到主城界面
		Director::getInstance()->replaceScene(MainCityLoadingScene::create());
	}
	else if(m_nFightType == FIGHT_TYPE_BOSS)
	{
		//发送世界boss结算消息
		FightController::getInstance()->setBossFightSettlement(mStarCount, BossModel::getInstance()->m_iAtkBossId, FightModel::getInstance()->getTotalDamage(), FIGHT_TYPE_BOSS);
	}
	else if(m_nFightType == FIGHT_TYPE_GROUP_BOSS)
	{
		//结算前记录当前剩余多少生命
		GroupBossModel::getInstance()->setRestLife(GroupBossModel::getInstance()->getBossInfo()->restReviveTimes);
		//发送帮派boss结算消息
		int bossIdx = GroupBossModel::getInstance()->getBossInfo()->bossIdx;
		FightController::getInstance()->setBossFightSettlement(mStarCount, bossIdx, FightModel::getInstance()->getBossDamage(), FIGHT_TYPE_GROUP_BOSS, this);
	}
	if (m_nFightType == FIGHT_TYPE_MAZE_TRUE)// PVE - 迷宫假战斗// PVE - 迷宫真战斗
	{	
		CCLOG("SceneFight::doFightEnd() maze 1");
		if (MazeModel::getInstance()->getCurEventId() > 0)
		{
			Director::getInstance()->popScene();
			if (mIsWin)
			{
				//如果赢了就发送战斗消息
				MazeController::getInstance()->onGetEventThingInfo(MazeModel::getInstance()->getCurEventId());
			}
			else
			{
				// 输了继续迷宫
				MazeController::getInstance()->onGetView()->gotoFirst();
			}
		}
	}
	else if (m_nFightType == FIGHT_TYPE_ROB_FIGHT)//
	{
		sendFightResult(mIsWin);
	}	 
	else if (m_nFightType == FIGHT_TYPE_PVE)
	{
		// 打赢PVE才会有数据变化;
		if (mIsWin)
		{
			sendFightResult(mIsWin);
		}
		else
		{
			// 输了直接显示;
			SettlementInfo info;
			info.nBarrierId = this->m_nBarrierId;
			onSettlementConfirmed(info);
		}
	}
	if (m_nFightType == FIGHT_TYPE_PVE_ACTIVITY)
	{
		if (ActivityModel::getInstance()->getCurActivityType() == ACT_TYPE_EXP_JiuJingKaoYan
			|| ActivityModel::getInstance()->getCurActivityType() == ACT_TYPE_COIN_LiangShanMiCang)
		{
			sendFightResult(true);
		}
		else
		{
			sendFightResult(mIsWin);
		}
	}
	else if (m_nFightType == FIGHT_TYPE_PVP_ARENA
		|| m_nFightType == FIGHT_TYPE_TRAIL
		|| m_nFightType == FIGHT_TYPE_TOWER
		|| m_nFightType == FIGHT_TYPE_FIND)
	{
		sendFightResult(mIsWin);
	}
}

void SceneFight::sendFightResult(bool isWin)
{
	FightController::getInstance()->handleSettlement(m_nFightType, m_nBarrierId, 
		/*isWin ? 1 : 0*/mStarCount, this);
}


void SceneFight::onSettlementConfirmed(SettlementInfo info)
{
	if (m_nFightType == FIGHT_TYPE::FIGHT_TYPE_PVE)
	{
		// 更新关卡数据;
		ChapterModel::getInstance()->updateFightRecordFromBarrier(m_nBarrierId, mStarCount);
	}

	// 结算展示;
	info.nBarrierId = m_nBarrierId;
	info.nFightType = m_nFightType;

	//帮派boss战斗也全部处理为胜利
	if ((m_nFightType == FIGHT_TYPE_PVE_ACTIVITY
		&& (ActivityModel::getInstance()->getCurActivityType() == ACT_TYPE_EXP_JiuJingKaoYan
		|| ActivityModel::getInstance()->getCurActivityType() == ACT_TYPE_COIN_LiangShanMiCang))
		|| m_nFightType == FIGHT_TYPE_GROUP_BOSS)
	{
		info.bWin = true;

		if(m_nFightType == FIGHT_TYPE_PVE_ACTIVITY)
		{
			// 因为活动关卡(经验/银币)两种类型的战斗结果全部处理为胜利;
			// 所以星级也要特殊处理，按照完成度来计算星级;
			if (info.fCompletionPercent < 50.0f)
				info.nStarCount = 1;
			else if (info.fCompletionPercent < 99.0f)
				info.nStarCount = 2;
			else
				info.nStarCount = 3;
		}
		else if(m_nFightType == FIGHT_TYPE_GROUP_BOSS)
		{
			info.nStarCount = 3;		//帮派boss先默认是3星吧
		}
	}
	else
	{
		info.bWin = mIsWin;
		info.nStarCount = mStarCount;
	}

	if (info.bWin)
	{
		// 剧情关卡/活动关卡扣体力，掠夺扣精力，其他类型的战斗什么都不扣;
		if (m_nFightType == FIGHT_TYPE_ROB_FIGHT)
		{
			info.nCostEnergy = RobModel::getInstance()->getRobCostEnergy();
		}
		else if (m_nFightType == FIGHT_TYPE_PVE)
		{
			info.nCostPower = BarrierModel::getInstance()->getBarrierStoryById(m_nBarrierId)->nCost;
		}
		else if (m_nFightType == FIGHT_TYPE_PVE_ACTIVITY)
		{
			info.nCostPower = ActivityModel::getInstance()->getPowerCost(m_nBarrierId);
		}
	}
	else
	{
		// 活动关卡(经验/银币关卡)失败也扣体力，碎片关卡失败不扣体力;
		// 虽然这两种类型战斗结果目前统一处理为成功，但是还是保留失败扣体力的逻辑吧;
		if (m_nFightType == FIGHT_TYPE_PVE_ACTIVITY
			&& (ActivityModel::getInstance()->getCurActivityType() == ACT_TYPE_EXP_JiuJingKaoYan
			|| ActivityModel::getInstance()->getCurActivityType() == ACT_TYPE_COIN_LiangShanMiCang))
		{
			info.nCostPower = ActivityModel::getInstance()->getPowerCost(m_nBarrierId);
		}

		// 爬塔失败原因;
		if (m_nFightType == FIGHT_TYPE_TOWER)
		{
			int nWinCond = FightModel::getInstance()->getWinCond();
			int nWinCondCount = FightModel::getInstance()->getWinCondCount();

			// 生成失败原因描述;
			ostringstream oss;
			switch (nWinCond)
			{
			case DEFEAT_ALL_ENEMY:
				//oss << "未能击败全部敌人，请再接再厉";
				oss << DataManager::getInstance()->searchCommonTexdtById(10088);
				break;
			case WIN_AND_ALIVE_N:
				//oss << "死亡人数超过" << nWinCondCount << "，请再接再厉";
				oss << DataManager::getInstance()->searchCommonTexdtById(10089) << nWinCondCount << "," 
					<< DataManager::getInstance()->searchCommonTexdtById(10090);
				break;
			case WIN_IN_ROUND_N:
				oss << DataManager::getInstance()->searchCommonTexdtById(10091) << nWinCondCount 
					<< DataManager::getInstance()->searchCommonTexdtById(10092);
				break;
			default:
				//oss << "通关条件未达成，请再接再厉";
				oss << DataManager::getInstance()->searchCommonTexdtById(10093);
				break;
			}
			info.strReason.assign(oss.str());
			oss.str("");
		}
	}

	assert(m_layerSettlement == nullptr);
	m_layerSettlement = LayerSettlement::create(info);
	m_layerSettlement->setLogicParent(this);
	this->addChild(m_layerSettlement, 9);

	onResultTalkingData();
}

void SceneFight::onAttackFinish()
{
	//m_pLayerFightInfo->updateUI();
	m_pLayerSkill->updateUI();
	m_pFSM->resume();
}

void SceneFight::initFSM()
{
	m_pFSM = FightStateMachine::create();
	m_pFSM->retain();
	m_pFSM->setFightUI(this);
}

void SceneFight::startGuide()
{
	auto callBack = [this](GuideNode* guideNode)->void{ startFSM(); };

	//引导结束再启动状态机
	GuideManager::getInstance()->setCallBack(nullptr, callBack);
	GuideManager::getInstance()->startFightStartGuide(GUIDE_INTERFACE_FIGHT, m_nBarrierId);
}

void SceneFight::startFSM()
{
	if (m_pFSM)
	{
		m_pFSM->start();
	}
}

void SceneFight::pauseFSM(float time, const function<void(void)>& callBack)
{
	m_pFSM->wait();
	auto callBackStartFSM = [this, callBack](float dt)
	{ 
		m_pFSM->resume();
		if (callBack != nullptr)
		{
			callBack();
		}
	};
	getScheduler()->schedule(callBackStartFSM, this, 0, 0, time, false, "pauseFSM");
}

FightStateMachine* SceneFight::getFSM()
{
	return m_pFSM;
}

LayerFightInfo* SceneFight::getLayerFightInfo()
{
	return m_pLayerFightInfo;
}

LayerFightBack* SceneFight::getLayerFightBack()
{
	return mLayerFightBack;
}

LayerManualSkill* SceneFight::getLayerManualSkill()
{
	return m_pLayerSkill;
}

LayerFighter* SceneFight::getLayerFighter()
{
	return m_pLayerFighter;
}

void SceneFight::setLayerMaskVisible(bool visible)
{
	m_pLayerMask->stopAllActions();
	if (visible)
	{
		m_pLayerMask->setOpacity(0.0f);
		m_pLayerMask->runAction(FadeTo::create(0.2f, 200));

	}
	else
	{
		//m_pLayerMask->setOpacity(200);
		m_pLayerMask->runAction(FadeTo::create(0.2f, 0));
	}
	
}

void SceneFight::onShakeScreen()
{
	this->stopAllActions();
	this->setPosition(0,0);
	FiniteTimeAction* action = Repeat::create(
		Sequence::create(
			MoveBy::create(0.05f, Point(0.0f, -35.0f)),
			MoveBy::create(0.05f, Point(0.0f, 35.0f)),
			//衰减
			MoveBy::create(0.02f, Point(0.0f, -15.0f)),
			MoveBy::create(0.02f, Point(0.0f, 15.0f)),
			nullptr
		)
		,1
	);
	this->runAction(action);

	//	this->setPosition(Point::ZERO);
	// 	auto shake = CCShake::create(0.15f, 35);
	// 	this->runAction(shake);
}

void SceneFight::jumpScene()
{
	if (m_nFightType == FIGHT_TYPE::FIGHT_TYPE_PVE)
	{
		ChapterModel::getInstance()->setJumpType(CH_JUMP_TYPE_FIGHT);
		int nChapterId = ChapterModel::getInstance()->constructChapterId(m_nBarrierId);
		bool chapterBackFlag = SceneChapter::getChapterBackFlag();
		bool barrierBackFlag = SceneChapter::getBarrierBackFlag();
		Director::getInstance()->replaceScene(
			SceneChapterLoading::create(nChapterId, m_nBarrierId, chapterBackFlag, barrierBackFlag));
	}
	else if (m_nFightType == FIGHT_TYPE::FIGHT_TYPE_PVE_ACTIVITY)
	{
		Director::getInstance()->replaceScene(SceneActivity::create(SceneActivity::getPushScene()));
	}
	else if (m_nFightType == FIGHT_TYPE_MAZE_TRUE)
	{
		MazeModel::getInstance()->clearMazEvent();
		SceneMaze *sceneMaze = SceneMaze::create();
		Director::getInstance()->replaceScene(sceneMaze);

		sceneMaze->getLayerMaze()->setFilishedFlag(true);
	}
	else if (m_nFightType == FIGHT_TYPE::FIGHT_TYPE_PVP_ARENA)
	{
		PvpModel::getInstance()->setWin(mIsWin);
		Director::getInstance()->replaceScene(PvpScene::create(PvpScene::getPushScene()));
	}
	else if (m_nFightType == FIGHT_TYPE::FIGHT_TYPE_ROB_FIGHT)
	{
		Director::getInstance()->popScene();
	}
	else if (m_nFightType == FIGHT_TYPE_TRAIL)
	{
		Director::getInstance()->replaceScene(SceneTower::create(TOWER_TYPE_TRAIL, SceneTower::getPushScene()));
	}
	else if (m_nFightType == FIGHT_TYPE_TOWER)
	{
		Director::getInstance()->replaceScene(SceneTower::create(TOWER_TYPE_TOWER, SceneTower::getPushScene()));
	}
	else if (m_nFightType == FIGHT_TYPE_FIND)
	{
		Director::getInstance()->popScene();
	}
	else if (m_nFightType == FIGHT_TYPE_GROUP_BOSS)
	{
		Director::getInstance()->replaceScene(GroupBossScene::create(false));
	}
}

void SceneFight::playFightMusic()
{
	// 音乐;
	switch (m_nFightType)
	{
	case FIGHT_TYPE_INVALID:
		break;
	case FIGHT_TYPE_PVE:
		{
			ostringstream oss;
			oss << "level_" << ChapterModel::getInstance()->getMusicNameIndex(
				ChapterModel::getInstance()->constructChapterId(m_nBarrierId));
			SoundUtils::playSoundMusic(oss.str());
			oss.str("");
		}
		break;
	case FIGHT_TYPE_PVE_ACTIVITY:
		SoundUtils::playSoundMusic("level_2");
		break;
	case FIGHT_TYPE_PVP_ARENA:
		SoundUtils::playSoundMusic("level_4");
		break;
	case FIGHT_TYPE_MAZE_TRUE:
		SoundUtils::playSoundMusic("level_5");
		break;
	case FIGHT_TYPE_ROB_FIGHT:
		SoundUtils::playSoundMusic("level_4");
		break;
	case FIGHT_TYPE_TOWER:
		SoundUtils::playSoundMusic("level_6");
		break;
	case FIGHT_TYPE_TRAIL:
		SoundUtils::playSoundMusic("level_3");
		break;
	case FIGHT_TYPE_GUIDE:
		SoundUtils::playSoundMusic("level_4");
		break;
	case FIGHT_TYPE_FIND:
		SoundUtils::playSoundMusic("level_1");
		break;
	case FIGHT_TYPE_BOSS:
		SoundUtils::playSoundMusic("level_4");
		break;
	default:
		break;
	}
}

void SceneFight::setSkip(bool skip)
{
	mIsSkip = skip;
}

bool SceneFight::getSkip()
{
	return mIsSkip;
}

//上报数据
void SceneFight::onBeginTalkingData()
{
	//剧情、活动、爬塔需上报数据
	switch (m_nFightType)
	{
	case FIGHT_TYPE_PVE:
	case FIGHT_TYPE_PVE_ACTIVITY:
	case FIGHT_TYPE_TOWER:
		TalkingData::onBegin(TO_STR(m_nBarrierId));
		break;
	case FIGHT_TYPE_TRAIL:
		{
			mTrailCurrFloor = TowerModel::getInstance()->getTrailCurFloor();
			if (mTrailCurrFloor > 0)
			{
				//TalkingData::onBegin("试炼" + TO_STR(mTrailCurrFloor) + "层");
				TalkingData::onBegin(DataManager::getInstance()->searchCommonTexdtById(10094) + TO_STR(mTrailCurrFloor) 
					+ DataManager::getInstance()->searchCommonTexdtById(10095));
			}
		}
		break;
	default:
		break;
	}
}

void SceneFight::onResultTalkingData(bool isEscape)
{
	//剧情、活动、爬塔需上报数据
	switch (m_nFightType)
	{
	case FIGHT_TYPE_PVE:
	case FIGHT_TYPE_PVE_ACTIVITY:
	case FIGHT_TYPE_TOWER:
		if (isEscape)
		{
			//TalkingData::onFailed(TO_STR(m_nBarrierId), "逃跑");
			TalkingData::onFailed(TO_STR(m_nBarrierId), DataManager::getInstance()->searchCommonTexdtById(10096));
		}
		else
		{
			if (mIsWin)
			{
				TalkingData::onCompleted(TO_STR(m_nBarrierId));
			}
			else
			{
				//TalkingData::onFailed(TO_STR(m_nBarrierId), "战斗失败");
				TalkingData::onFailed(TO_STR(m_nBarrierId), DataManager::getInstance()->searchCommonTexdtById(10097));
			}
		}
		break;
	case FIGHT_TYPE_TRAIL:
		if (mTrailCurrFloor > 0)
		{
			//string stageName = "试炼" + TO_STR(mTrailCurrFloor) + "层";
			string stageName = DataManager::getInstance()->searchCommonTexdtById(10094) + TO_STR(mTrailCurrFloor) 
				+ DataManager::getInstance()->searchCommonTexdtById(10095);
			if (isEscape)
			{
				//TalkingData::onFailed(stageName, "逃跑");
				TalkingData::onFailed(stageName, DataManager::getInstance()->searchCommonTexdtById(10096));
			}
			else
			{
				if (mIsWin)
				{
					TalkingData::onCompleted(stageName);
				}
				else
				{
					//TalkingData::onFailed(stageName, "战斗失败");
					TalkingData::onFailed(stageName, DataManager::getInstance()->searchCommonTexdtById(10097));
				}
			}
		}
		break;
	default:
		break;
	}
}