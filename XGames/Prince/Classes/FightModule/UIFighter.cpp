#include "UIFighter.h"
#include "UIFighterHp.h"
#include "UIFighterDamage.h"
#include "LayerFighter.h"
#include "LayerFightInfo.h"
#include "SceneFight.h"
#include "Skill/SkillResultParam.h"
#include "../Common/Common.h"
#include "FightModel.h"
#include "Utils/StringFormat.h"
#include "UIAbstractSkillEffect.h"
#include "SceneFight.h"
#include "ShadowAction.h"
#include "StreakAction.h"
#include "BlinkEffect.h"
#include "Utils/SchedulerController.h"
#include "UIBuffDelegate.h"
#include <algorithm>
#include <ostream>

using namespace std;
using namespace cocos2d;
using namespace cocostudio;
using namespace spine;

const int	nFIHGTER_WIDTH			= 120;
const int	nFIHGTER_HEIGHT			= 160;

//闪避后退、返回时间
const float fTIME_DODGE_BACK		= 0.3f;
const float fTIME_DODGE_DELAY		= 0.2f;
const float fTIME_DODGE_RETURN		= 0.2f;
//闪避偏移量
const int nDODGE_OFFSET_X = 60;
const int nDODGE_OFFSET_Y = 3;

//追打偏移量
const int nRUSH_OFFSET_X = 900;
//追打前进速度 单位：像素/秒
const int nRUSH_FRONT_SPEED = 12000;
//追打回退速度
const int nRUSH_BACK_SPEED = 3000;
//追打前进跳跃高度
const int nRUSH_FRONT_JUMP_HEIGHT = 0;
//追打回退跳跃高度
const int nRUSH_BACK_JUMP_HEIGHT = 300;
//移动速度
const int nMOVE_SPEED = 3000;
//移动跳跃高度
const int nMOVE_JUMP_HEIGHT = 150;



UIFighter::UIFighter()
	: m_enSide(FightStateMachine::E_SIDE::MY_SIDE)
	, m_nIndex(-1)
	, m_nResID(-1)
	, m_pParent(nullptr)
	, m_pSkelFighter(nullptr)
	, m_pBarHP(nullptr)
	, m_bIsSkillEnd(false)
	, m_bIsPreEffectEnd(false)
	, m_bIsActEffectEnd(false)
	, mShade(nullptr)
	, mRushStateAni(nullptr)
{

}

UIFighter::~UIFighter()
{
	mBuffDelegate->release();
}

UIFighter* UIFighter::create( int nResID, FightStateMachine::E_SIDE side, int nIndex, bool isAssist )
{
	UIFighter* pRet = new UIFighter();
	if (pRet!=nullptr && pRet->init(nResID, side, nIndex, isAssist))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet);
	return nullptr;
}

bool UIFighter::init( int nResID, FightStateMachine::E_SIDE side, int nIndex, bool isAssist)
{
	if (!Node::init())
	{
		return false;
	}

	m_nResID = nResID;
	m_enSide = side;
	m_nIndex = nIndex;

	m_pSkelFighter = SpineAnimation::createFighterWithResId(nResID);
	m_pSkelFighter->setPosition(Point(0.0f, 0.0f));
	m_pSkelFighter->setAnimation(0, szANI_WAIT, true);
	
	//设置战斗角色颜色
	int fightBgId = FightModel::getInstance()->getBgResId();
	RowData* rowData = DataManager::getInstance()->searchFightBg(fightBgId);
	string colorStr = rowData->getStringData("colorVal");
	if (!colorStr.empty())
	{
		m_pSkelFighter->setColor(StringFormat::parseCsvStringToColor(colorStr));
	}
	
	this->addChild(m_pSkelFighter, ZORDER_ARMATURE);

	//助战角色与首发角色数据来源不同
	m_pBarHP = UIFighterHp::create(side);
	this->addChild(m_pBarHP, ZORDER_HP);
	
	mShade = Sprite::create("Image/Fight/img_shadow.png");
	this->addChild(mShade);

	setDirection(m_enSide);

	mBuffDelegate = UIBuffDelegate::create(this);
	mBuffDelegate->retain();

	updateHP(isAssist);

	m_pSkelFighter->setSkin(FIGHTER_SKIN_1);

	return true;
}

void UIFighter::onEnter()
{
	Node::onEnter();
	m_pParent = dynamic_cast<LayerFighter*>(this->getParent());
	//setPositionByIndex(m_enSide, m_nIndex);
}

void UIFighter::onExit()
{
	Node::onExit();
	this->unscheduleAllSelectors();
}

void UIFighter::setPositionByIndex( FightStateMachine::E_SIDE side, int nIndex )
{
	this->setPosition(m_pParent->getPositionByIndex(side, nIndex));
}

void UIFighter::onAttackStart( SkillResultParam* pSkill, SKILL_EFFECT* effect )
{
	CCLOG("-----UI[%d][%d] onAttackStart! target:[%d][%d], harm=%d", ENUM_TO_INT(m_enSide), m_nIndex, 
		ENUM_TO_INT(pSkill->mTargetSide), pSkill->mVecTargetIndex.back(), pSkill->mVecTargetHarm.back().mValue);

	CCASSERT(!pSkill->mVecTargetIndex.empty(), "mVecTargetIndex is empty in onAttackStart!");

	this->effect = *effect;
	sklParam.nSklID = pSkill->mSkillId;
	sklParam.nSklType = pSkill->mSkillType;
	sklParam.targetSide = pSkill->mTargetSide;
	sklParam.nComboMaxCount = pSkill->mComboCount;
	sklParam.nComboCount = 0;
	sklParam.nChangeSkin = pSkill->mChangeSkin;
	sklParam.pAttacker = m_pParent->getFighterByIndex(pSkill->mAttackSide, pSkill->mAttackerIndex);
	sklParam.vecpTarget.clear();
	sklParam.vecDamage.clear();

	for (size_t i=0; i<pSkill->mVecTargetHarm.size(); ++i)
	{
		HarmParam& harmParam = pSkill->mVecTargetHarm.at(i);
		UIFighter* pTarget = m_pParent->getFighterByIndex(harmParam.mTargetSide, harmParam.mTargetIndex);
		sklParam.vecpTarget.push_back(pTarget);
		sklParam.vecDamage.push_back(harmParam);
	}

	sklParam.bloodHarm = pSkill->mBloodHarm;

	//星灵技能需另开辟分支
	if (sklParam.nSklType == AbstractSkill::SKILL_TYPE::STAR_SKILL)
	{
		startStarSpriteSkill();
	}
	else
	{
		//是否开始追打
		if (m_pParent->isStartRushAttack())
		{
			//开始追打时显示蒙板
			m_pParent->getSceneFight()->setLayerMaskVisible(true);

			startRushAniAndPause();
		}

		if (m_pParent->isEndRushAttack())
		{
			//结束追打时隐藏蒙板
			m_pParent->getSceneFight()->setLayerMaskVisible(false);
		}

		//需要判断是否同一个人的连续攻击
		if (m_pParent->isConsequentAttack())
		{
			waitMoveBack();
		}
		else
		{
			startSkillFlash();
		}
	}
}

void UIFighter::onAttackFinish()
{	
	//攻击动作、前置特效、后置特效必须全部播放完毕，攻击才算结束
	if (!(m_bIsSkillEnd && m_bIsActEffectEnd && m_bIsPreEffectEnd))
	{
		return;
	}

	if (sklParam.nSklType == AbstractSkill::SKILL_TYPE::ACTIVE_SKILL)
	{
		LayerFightBack* layerFightBack = m_pParent->getSceneFight()->getLayerFightBack();
		layerFightBack->return3dEffect();
		m_pParent->startMoveInAnimation(sklParam);
	}

	m_bIsSkillEnd = m_bIsPreEffectEnd = m_bIsActEffectEnd = false;

	if (FightModel::getInstance()->getFighter(m_enSide, m_nIndex)->isAlive() && 
		sklParam.nSklType != AbstractSkill::SKILL_TYPE::RUSH_SKILL)
	{
		//追打技能不回退，移动结束后自动回退
		startMotionBack();
	}
	
	m_pParent->onAttackFinish();
	
	CCLOG("-----UI[%d][%d] onAttackFinish!", ENUM_TO_INT(m_enSide), m_nIndex);
}

void UIFighter::waitMoveBack()
{
	CCLOG("-----UI[%d][%d] waitMoveBack!", ENUM_TO_INT(m_enSide), m_nIndex);

	auto func = [this](float dt)->void
	{
		//归位后立即发起下一次攻击
		if (isPointEqual(getPosition(), getOriginPosition()))
		{
			startSkillFlash();
			getScheduler()->unschedule("waitMoveBack", this);	
		}
	};

	//此处需等待该角色回位才能触发下一次攻击
	getScheduler()->schedule(func, this, 0, false, "waitMoveBack");

}

void UIFighter::startStarSpriteSkill()
{

	m_bIsSkillEnd = true;
	startSkillEffect();

// 	ArmatureDataManager::getInstance()->addArmatureFileInfo(
// 		"Image/AniStar/AniStar0.png",
// 		"Image/AniStar/AniStar0.plist",
// 		"Image/AniStar/AniStar.ExportJson");
// 
//	Armature* aniFlash = Armature::create("AniStar");
// 	aniFlash->getAnimation()->playByIndex(0);
// 	aniFlash->setPosition(Director::getInstance()->getWinSize() / 2);
// 	this->addChild(aniFlash);
// 
// 	auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
// 		{
// 			m_bIsSkillEnd = true;
// 			startSkillEffect();
// 		};
// 
// 	aniFlash->getAnimation()->setMovementEventCallFunc(aniCallBack);
}

void UIFighter::startSkillFlash()
{
	//上次攻击和本次攻击都是普通攻击，不用等待所有人归位
	if (m_pParent->isLastCurSkillNormal())
	{
		startMotionFront();
		return;
	}
	
	//其他情况需等待所有人归位再触发下次攻击
	auto func = [this](float dt)->void
	{
		bool isAllBack = true;
		for (int i = 0; i < INDEX_COUNT; i++)
		{
			UIFighter* fighter = m_pParent->getFighterByIndex(m_enSide, i);
			if (fighter)
			{
				if (!fighter->isPointEqual(fighter->getPosition(), fighter->getOriginPosition()))
				{
					isAllBack = false;
					break;
				}
			}
		}

		//全部角色归位后立即发起下一次攻击
		if (isAllBack)
		{
			getScheduler()->unschedule("startSkillFlash", this);	

			if (sklParam.nSklType == AbstractSkill::SKILL_TYPE::ACTIVE_SKILL)
			{
				startActiveSkillFlash();
			}
			else if (sklParam.nSklType == AbstractSkill::SKILL_TYPE::TRIGGER_SKILL)
			{
				startP3SkillFlash();
			}
			else if (sklParam.nSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL)
			{
				startRushSkillFlash();
			}
			else
			{
				startMotionFront();
			}
		}
	};

	//此处需等待该角色回位才能触发下一次攻击
	getScheduler()->schedule(func, this, 0, false, "startSkillFlash");
}

void UIFighter::startActiveSkillFlash()
{
	Sprite* heroSkin = Sprite::create(ResourceUtils::getHeroCompleteCardPath(m_nResID));

	if (heroSkin)
	{
		SoundUtils::playSoundEffect("cutin");
		ArmatureDataManager::getInstance()->addArmatureFileInfo(
// 			"Image/AniBeforeManuskl/AniBeforeManuskl0.png",
// 			"Image/AniBeforeManuskl/AniBeforeManuskl0.plist",
			"Image/AniBeforeManuskl2/AniBeforeManuskl2.ExportJson");

		Armature* aniFlash = Armature::create("AniBeforeManuskl2");
		aniFlash->setPosition(Director::getInstance()->getWinSize() / 2);
		aniFlash->getAnimation()->playByIndex(0);
		RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(m_nResID);
		vector<float> pointVect;
		StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
		heroSkin->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));
		aniFlash->getBone("Card")->addDisplay(heroSkin, 0);

		if (m_enSide == FightStateMachine::E_SIDE::ENEMY_SIDE)
		{
			aniFlash->setScale(-1, 1); //动画翻面
			heroSkin->setFlipX(true);  //骨骼翻面
			heroSkin->setAnchorPoint(Vec2(1 - pointVect[0],pointVect[1])); //二次翻面需修正锚点
		}

		m_pParent->getSceneFight()->addChild(aniFlash, 999);

		//动画回调
		auto aniCallBack = [this](Armature *armature, MovementEventType eventType, const std::string& strName)->void
		{
			if (eventType == MovementEventType::COMPLETE)
			{
				armature->removeFromParent();

				LayerFightBack* layerFightBack = m_pParent->getSceneFight()->getLayerFightBack();
				layerFightBack->start3dEffect(CC_CALLBACK_0(UIFighter::startMotionFront, this));
				m_pParent->startMoveOutAnimation(sklParam);
			}
		};

		aniFlash->getAnimation()->setMovementEventCallFunc(aniCallBack);

	}
	else
	{
		//无原画角色暂停一段时间播放主动技能动画
		auto func = [this](float dt)->void
		{
			LayerFightBack* layerFightBack = m_pParent->getSceneFight()->getLayerFightBack();
			layerFightBack->start3dEffect(CC_CALLBACK_0(UIFighter::startMotionFront, this));
			m_pParent->startMoveOutAnimation(sklParam);
			Director::sharedDirector()->getScheduler()->unschedule("startActiveSkillFlash", this);
		};

		Director::sharedDirector()->getScheduler()->schedule(func, this, 0, 0, 0.8f, false, "startActiveSkillFlash");
	}
}

void UIFighter::startRushSkillFlash()
{
	//startMotionFront();
	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/AniComBoBuff/AniComBoBuff.ExportJson");

	Armature* aniFlash = Armature::create("AniComBoBuff");
	aniFlash->setPosition(Point::ZERO);
	this->addChild(aniFlash, 99);
	aniFlash->getAnimation()->play("Ani_ComboBuff", -1, 0);
	if (m_enSide == FightStateMachine::E_SIDE::ENEMY_SIDE)
	{
		aniFlash->setScale(-1, 1); //翻面
	}
	
// 	switch (getFighterData()->getRushSkill()->getPreStateType())
// 	{
// 	case AbstractSkill::RESULT_STATE::FLOAT: //天
// 		aniFlash->getAnimation()->play("Ani_TianBuff", -1, 0);
// 		break;
// 	case AbstractSkill::RESULT_STATE::BACK: //地
// 		aniFlash->getAnimation()->play("Ani_DiBuff", -1, 0);
// 		break;
// 	case AbstractSkill::RESULT_STATE::FELL: //人
// 		aniFlash->getAnimation()->play("Ani_RenBuff", -1, 0);
// 		break;
// 	}

	//动画回调
	auto aniCallBack = [this](Armature *armature, MovementEventType eventType, const std::string& strName)->void
	{
		armature->removeFromParent();
		startMotionFront();
	};

	aniFlash->getAnimation()->setMovementEventCallFunc(aniCallBack);
}

void UIFighter::startP3SkillFlash()
{
	//此处有分支，如果有变身，则播放变身动画，变身动画回调帧触发下一个动画播放阶段
	if (sklParam.nChangeSkin > 0)
	{
		//回调帧执行下一个阶段
		auto frameCallBack = [this](Bone *bone, const string& evt, int originFrameIndex, int currentFrameIndex)->void
		{
			if(evt == "cb_bianshen")
			{
				startMotionFront();
				//变身效果
				if (sklParam.nChangeSkin == 2)
				{
					this->m_pSkelFighter->setSkin(FIGHTER_SKIN_2);
				}
			}
			else if (evt == "cb_next_ani")
			{
				//添加持续动画
				ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniBOSStexiao/AniBOSStexiao.ExportJson");
				Armature* aniFlash = Armature::create("AniBOSStexiao");
				aniFlash->getAnimation()->playByIndex(0);
				this->addChild(aniFlash, ZORDER_ARMATURE + 1, "AniBOSStexiao");
			}
		};

		//动画播放结束后移除该动画
		auto aniCallBack = [this](Armature *armature, MovementEventType eventType, const std::string& strName)->void
		{
			if (eventType == MovementEventType::COMPLETE)
			{
				armature->removeFromParent();
			}
		};

		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anibianshen/Anibianshen.ExportJson");
		Armature* aniFlash = Armature::create("Anibianshen");
		aniFlash->getAnimation()->playByIndex(0);
		aniFlash->setPosition(this->getPosition());
		aniFlash->getAnimation()->setFrameEventCallFunc(frameCallBack);
		aniFlash->getAnimation()->setMovementEventCallFunc(aniCallBack);
		this->getParent()->addChild(aniFlash, 9999);
	}
	else
	{
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniP3Flag/AniP3Flag.ExportJson");
		Armature* aniFlash = Armature::create("AniP3Flag");
		aniFlash->getAnimation()->playByIndex(0);
		aniFlash->setPosition(Point::ZERO);
		int triggerType = getFighterData()->getTriggerSkill()->getTriggerType();
		string str = "";

		switch (triggerType)
		{
		case TriggerSkill::EVERY_ROUND:
		case TriggerSkill::ENTER_FIGHT:
			str = "Image/Fight/xiangong.png";
			break;
		default:
			str = "Image/Fight/lianzhan.png";
			break;
		}

		//动画回调
		auto aniCallBack = [this](Armature *armature, MovementEventType eventType, const std::string& strName)->void
		{
			armature->removeFromParent();
			startMotionFront();
		};

		cocostudio::Skin* skin = Skin::create(str);
		aniFlash->getBone("text")->addDisplay(skin, 0);
		aniFlash->getAnimation()->setMovementEventCallFunc(aniCallBack);
		this->addChild(aniFlash, 9999);
	}

}

void UIFighter::startMotionFront()
{
	CCLOG("-----UI[%d][%d] startMoveFront! skill: %d", ENUM_TO_INT(m_enSide), m_nIndex, sklParam.nSklID);

	this->stopAllActions();

	//显示技能tips
	m_pParent->getSceneFight()->getLayerFightInfo()->showSkillTips(sklParam.nSklID, sklParam.nSklType);

	if (SKILL_EFFECT::DIS_TYPE::DIS_LONG == effect.nDisType)		// 远程攻击
	{
		// 留在原地不移动
		this->onMotionFrontEnd();
		return;
	}

	Point targetPos = getDistancePosition();
	setMoveZorder(targetPos);
	bool isRushSkill = sklParam.nSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL;
	float speed = (sklParam.nSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL) ? nRUSH_FRONT_SPEED : nMOVE_SPEED;
	int height =  (sklParam.nSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL) ? nRUSH_FRONT_JUMP_HEIGHT : nMOVE_JUMP_HEIGHT;

	auto callBack = [this, isRushSkill, targetPos, speed, height]()->void
	{
		//计算追打移动终点
		Point p1 = getPosition();
		Point p2 = targetPos;
		Point endP = (m_enSide == FightStateMachine::E_SIDE::MY_SIDE) ? 
			Point(p2.x + nRUSH_OFFSET_X, p2.y + nRUSH_OFFSET_X * (p2.y - p1.y) / (p2.x - p1.x)) :
			Point(p2.x - nRUSH_OFFSET_X, p2.y - nRUSH_OFFSET_X * (p2.y - p1.y) / (p2.x - p1.x));

		//追打有残影效果，无收尾动作
		startActionJump((SKILL_EFFECT::MOVE_TYPE)effect.nMoveType, MOVE_SPEED_TYPE::MOVE_SPEED_NO,
			isRushSkill, !isRushSkill, targetPos, speed, height, CC_CALLBACK_0(UIFighter::onMotionFrontEnd, this, endP));
	};

	//追打先后退到屏幕外，再向前移动
	if (isRushSkill)
	{
		Point offset = (m_enSide == FightStateMachine::E_SIDE::MY_SIDE) ? Point(-nRUSH_OFFSET_X, 0) : Point(nRUSH_OFFSET_X, 0);
		Point targetP = getPosition() + offset;
		startActionJump(SKILL_EFFECT::MOVE_TYPE::MOVE_JUMP, MOVE_SPEED_TYPE::MOVE_SPEED_NO,
			false, false, targetP, speed, 150, callBack);
	}
	else
	{
		callBack();
	}
}

void UIFighter::onMotionFrontEnd(Point p)
{
	CCLOG("-----UI[%d][%d] onMoveFrontEnd! type:%d", ENUM_TO_INT(m_enSide), m_nIndex, sklParam.nSklType);

	if (sklParam.nSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL)
	{
		auto func = [this, p](float dt)
		{
			Point offset = (m_enSide == FightStateMachine::E_SIDE::MY_SIDE) ? Point(nRUSH_OFFSET_X, 0) : Point(-nRUSH_OFFSET_X, 0);
			startActionJump(SKILL_EFFECT::MOVE_TYPE::MOVE_JUMP, MOVE_SPEED_TYPE::MOVE_SPEED_NO,
				true, false, p, nRUSH_FRONT_SPEED, nRUSH_FRONT_JUMP_HEIGHT, CC_CALLBACK_0(UIFighter::startMotionBack, this));
			this->getScheduler()->unschedule("onMotionFrontEnd", this);
		};

		this->getScheduler()->schedule(func, this, 0, 0, 0, false, "onMotionFrontEnd");
	}

	setFightZorder();

	startActionSkill();
}

void UIFighter::startActionSkill()
{
	CCLOG("-----UI[%d][%d] startActionSkill! skill:%d", ENUM_TO_INT(m_enSide), m_nIndex, sklParam.nSklID);

	string szAction = effect.actionName;
	spTrackEntry* pEntry = m_pSkelFighter->setAnimation(0, szAction, false);
	//m_pSkelFighter->addAnimation(0, szANI_WAIT, true);
	m_pSkelFighter->setTrackEventListener(pEntry, CC_CALLBACK_2(UIFighter::onActionSkillEvent, this));
	m_pSkelFighter->setTrackCompleteListener(pEntry, CC_CALLBACK_0(UIFighter::onActionSkillEnd, this));

	//如果前置后置特效都没有直接结束攻击
	if (!effect.isHaveActEffect() && !effect.isHavePreEffect())
	{
		this->onSkillPreEffectEnd();
		this->onSkillActEffectEnd();
	}
}

void UIFighter::onActionSkillEvent(int trackIndex, spEvent* event)
{
	CCLOG("-----UI[%d][%d] onActionSkillEvent! skill:%d, event:%s", ENUM_TO_INT(m_enSide), m_nIndex, sklParam.nSklID, event->data->name);

	if (strcmp(event->data->name, "cb_shake") == 0)
	{
		m_pParent->getSceneFight()->onShakeScreen();
	}
	else if(strcmp(event->data->name, "cb_atk") == 0)
	{
		//如果该特效没有前置，Combo由角色计数
		if (!effect.isHavePreEffect())
		{
			sklParam.nComboCount += 1;
		}
		startSkillEffect();
	}
	else
	{
		FightSoundEffectPlayer::playFightEffect(event->data->name);
	}
}

void UIFighter::onActionSkillEnd()
{
	CCLOG("-----UI[%d][%d] onActionSkillEnd!", ENUM_TO_INT(m_enSide), m_nIndex);

	m_bIsSkillEnd = true;

	this->onAttackFinish();
}

void UIFighter::startSkillEffect()
{
	CCLOG("-----UI[%d][%d] startSkillEffect! sklType:%d", ENUM_TO_INT(m_enSide), m_nIndex, sklParam.nSklType);

	//如果有前置特效，后置特效由前置特效释放，所有技能一定有后置特效
	//如果有前置特效，combo计数由前置执行
	if (effect.isHavePreEffect())
	{
		//区分前置特效的单体和多体
		if (effect.nPreTagTyp == SKILL_EFFECT::PRE_TAG_TYPE::SINGLE)
		{
			UIFighter* fighter = sklParam.vecpTarget.back();
			//前置单体，则前置将技能结束标记传递给后置
			UIPreSkillEffect* preSkillEffect = UIPreSkillEffect::create(sklParam, effect, fighter, true);
			m_pParent->addChild(preSkillEffect);
		}
		else if (effect.nPreTagTyp == SKILL_EFFECT::PRE_TAG_TYPE::MULTI)
		{
			for (size_t i = 0; i < sklParam.vecpTarget.size(); i++)
			{
				UIFighter* fighter = sklParam.vecpTarget.at(i);
				bool isSkillEnd = (i == sklParam.vecpTarget.size() - 1);
				UIPreSkillEffect* preSkillEffect = UIPreSkillEffect::create(sklParam, effect, fighter, isSkillEnd);
				m_pParent->addChild(preSkillEffect);
			}
		}
	}
	else
	{
		m_bIsPreEffectEnd = true;

		//如果没有前置特效，直接释放后置特效
		if (effect.isHaveActEffect())
		{
			for (size_t i = 0; i < sklParam.vecpTarget.size(); i++)
			{
				UIFighter* fighter = sklParam.vecpTarget.at(i);
				bool isSkillEnd = (i == sklParam.vecpTarget.size() - 1) && sklParam.isFinalCombo();
				UIActSkillEffect* actSkillEffect = UIActSkillEffect::create(sklParam, effect, fighter, isSkillEnd);
				m_pParent->addChild(actSkillEffect);
			}
		}
	}
}

void UIFighter::startMotionDodge()
{
	SoundUtils::playSoundEffect("shanbi");

	m_pSkelFighter->setAnimation(0, szANI_WAIT, true);

// 	Point originP = m_pParent->getPositionByIndex(m_enSide, m_nIndex);
// 	if (sklParam.nSklType == AbstractSkill::SKILL_TYPE::ACTIVE_SKILL)
// 	{
// 		originP = m_pParent->get3dPositionByIndex(m_enSide, m_nIndex);
// 	}
// 
// 	Point offsetP = originP + Point(-nDODGE_OFFSET_X, nDODGE_OFFSET_Y);;
// 	if (m_enSide == FightStateMachine::E_SIDE::ENEMY_SIDE)
// 	{
// 		offsetP = originP + Point(nDODGE_OFFSET_X, nDODGE_OFFSET_Y);
// 	}
// 
// 	FiniteTimeAction* initMotion = MoveTo::create(0.0f, originP);
// 	FiniteTimeAction* backMotion = EaseExponentialInOut::create(MoveTo::create(fTIME_DODGE_BACK, offsetP));
// 	FiniteTimeAction* delayMotion = DelayTime::create(fTIME_DODGE_DELAY);
// 	FiniteTimeAction* returnMotion = EaseExponentialInOut::create(MoveTo::create(fTIME_DODGE_RETURN, originP));
// 	Sequence* seq = Sequence::create(initMotion, backMotion, delayMotion, returnMotion, NULL);
// 	stopAllActions();
// 	runAction(seq);
}

void UIFighter::startActionHit(HarmParam* harmParam, int sklTyp)
{
	CCLOG("-----UI[%d][%d]----- startActionHit", ENUM_TO_INT(m_enSide), m_nIndex);

	if (harmParam->mValue >= 0)
	{
		//伤害大于0，无受击状态
		//m_pSkelFighter->setAnimation(0, szANI_WAIT, true);
	}
	else
	{
		//星灵技能不增加combo数
		if (sklTyp != AbstractSkill::SKILL_TYPE::STAR_SKILL)
		{
			FightStateMachine::E_SIDE side = (m_enSide == FightStateMachine::E_SIDE::MY_SIDE)
				? FightStateMachine::E_SIDE::ENEMY_SIDE : FightStateMachine::MY_SIDE;
			m_pParent->getSceneFight()->getLayerFightInfo()->addHitCount(side);
		}

		spTrackEntry* pEntry = m_pSkelFighter->setAnimation(0, szANI_BEATN, false);
		m_pSkelFighter->addAnimation(0, szANI_WAIT, true);
	}

	//添加伤害数字
	int harm = harmParam->mValue / harmParam->mComboCount;
	this->addDamage(harm, harmParam->mHarmType);

	//更新血量
	Fighter* fighter = getFighterData();
	if (fighter && harmParam->mComboCount > 1)
	{
		float harmPercent = 100 * harm / (double)fighter->getMaxHP();
		float hpPercent = m_pBarHP->getPercent() + harmPercent;
		hpPercent = hpPercent < 0 ? 0 : hpPercent;
		hpPercent = hpPercent > 100 ? 100 : hpPercent;
		m_pBarHP->setPercent(hpPercent);
	}
	
}


void UIFighter::startActionHitFinal(UIFighter* pSponsor, int sklTyp)
{
	CCLOG("-----UI[%d][%d]----- startActionHitFinal", ENUM_TO_INT(m_enSide), m_nIndex);

	updateHP();
	slowMotion();

	pSponsor->addBloodDamage();

	Fighter* fighter = getFighterData();
	// 英雄未挂掉 且 是追打技能
	if (fighter->isAlive() )
	{
		if (sklTyp == AbstractSkill::SKILL_TYPE::RUSH_SKILL)
		{
			// 判断产生的状态
			AbstractSkill::RESULT_STATE enState = fighter->getState();
			if (enState != AbstractSkill::RESULT_STATE::NONE_STATE)
			{
				showTargetRushFlag(enState);
			}
		}
	}
	else// 英雄已挂掉
	{
		startActionDead();
	}
}

void UIFighter::startBuffRoundEffect(vector<HarmParam>* vecHarm)
{
	if (vecHarm->empty()) return;

	Vector<FiniteTimeAction*> vecAction;
	size_t size = vecHarm->size();
	for (size_t i = 0; i < size; i++)
	{
		bool isFinal = (i == (size - 1));
		HarmParam* harmParam = &(vecHarm->at(i));
		auto callback = CallFunc::create(CC_CALLBACK_0(UIFighter::buffRoundEffect, this, harmParam, isFinal));
		vecAction.pushBack(callback);

		//每个伤害效果间隔时间
		auto delayAction = DelayTime::create(0.35f);
		vecAction.pushBack(delayAction);
	}
	auto seq = Sequence::create(vecAction);
	this->runAction(seq);
}

void UIFighter::buffRoundEffect(HarmParam* harmParam, bool isFinal)
{

	if (isFinal)
	{
		updateHP();

		Fighter* pFighter = FightModel::getInstance()->getFighter(m_enSide, m_nIndex);
		bool bIsAlive = pFighter->isAlive();

		if (!bIsAlive)		// 英雄未挂掉
		{
			startActionDead();
			slowMotion();
		}
	}

	this->addDamage(harmParam->mValue, harmParam->mHarmType);
}

void UIFighter::startMotionBack()
{
	CCLOG("-----UI[%d][%d] moveBack!", ENUM_TO_INT(m_enSide), m_nIndex);

	if (this->getPosition() == m_pParent->getPositionByIndex(m_enSide, m_nIndex))
	{
		this->onMotionBackEnd();
		return;
	}

	bool isRushSkill = sklParam.nSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL;
	Point targetPos = m_pParent->getPositionByIndex(m_enSide, m_nIndex);
	
	float speed = (sklParam.nSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL) ? nRUSH_BACK_SPEED : nMOVE_SPEED;
	int height =  (sklParam.nSklType == AbstractSkill::SKILL_TYPE::RUSH_SKILL) ? nRUSH_BACK_JUMP_HEIGHT : nMOVE_JUMP_HEIGHT;

	startActionJump(SKILL_EFFECT::MOVE_TYPE::MOVE_JUMP, MOVE_SPEED_TYPE::MOVE_SPEED_NO,
		isRushSkill, true, targetPos, speed, height, CC_CALLBACK_0(UIFighter::onMotionBackEnd, this));
}

void UIFighter::onMotionBackEnd()
{
	SoundUtils::playSoundEffect("luodi");

	// 重置层级
	setOriginZorder();

	// runAction move动作完成的目标position会有float型误差，这里要重置一下位置
	setOriginPosition();

	playActionWait();
}

void UIFighter::playActionWait()
{
	m_pSkelFighter->setAnimation(0, szANI_WAIT, true);
}

void UIFighter::playActionWin()
{
	m_pSkelFighter->setAnimation(0, szANI_WIN, true);
}

void UIFighter::setDirection( FightStateMachine::E_SIDE side )
{
	float scale = fARMATURE_SCALE_SIZE;
	switch (m_nResID)
	{
	case 41008:
		scale = 0.85f;
		break;
	default:
		break;
	}

	if (FightStateMachine::E_SIDE::MY_SIDE == side)
	{
		m_pSkelFighter->setScale(scale);
	}
	else
	{
		m_pSkelFighter->setScale(-scale, scale);
	}
}

void UIFighter::onActionDeadEnd( int trackIndex )
{
	CCLOG("-----UI[%d][%d]----- onActionDeadEnd: %s", ENUM_TO_INT(m_enSide), m_nIndex, m_pSkelFighter->getCurrent(trackIndex)->animation->name);

	removeAllUIBuff();
	m_pParent->onFighterDead(m_enSide, m_nIndex);
	
}

void UIFighter::updateHP(bool isAssist)
{
	CCLOG("-----UI[%d][%d]----- updateHP: %ll", ENUM_TO_INT(m_enSide), m_nIndex, getFighterData(isAssist)->getCurrHP());
	m_pBarHP->setPercent(getFighterData(isAssist)->getPercentHP());
}

void UIFighter::setMoveZorder(Point targetPos)
{
	if (targetPos.y>this->getPosition().y)		// 垂直方向要向上移动时，把zorder设为比他同一排最前列的英雄小1
	{
		int nRow = TO_POS_ROW(m_nIndex);
		this->setLocalZOrder( m_pParent->getZorderByIndex(m_enSide, TO_INDEX(nRow, 0)) - 1 );
	}
}


void UIFighter::setFightZorder()
{
	int nZorder;

	switch (effect.nDisType)
	{

	case SKILL_EFFECT::DIS_TYPE::DIS_MID_REL_ELEM:		// 相对个体中程攻击
		{
			UIFighter* pTarget = sklParam.vecpTarget.back();
			nZorder = m_pParent->getZorderByIndex(sklParam.targetSide, pTarget->getIndex()) + 1;
		}
		break;

	case SKILL_EFFECT::DIS_TYPE::DIS_MID_ABSOLUTE:		// 绝对中程攻击
		{
			nZorder = m_pParent->getZorderByIndex(sklParam.targetSide, 1) + 1;
		}
		break;

	case SKILL_EFFECT::DIS_TYPE::DIS_ENEMY_CENTER:
		{
			nZorder = m_pParent->getZorderByIndex(sklParam.targetSide, 4) + 1;
		}
		break;

	default:
		nZorder = this->getLocalZOrder();
		break;
	}

	this->setLocalZOrder(nZorder);
}

Point UIFighter::getDistancePosition()
{
	Point pos;
	int distance = 0;
	int nTargetIndex = 0;

	switch (effect.nDisType)
	{
	case SKILL_EFFECT::DIS_MID_ABSOLUTE: //绝对中程(x,y都绝对)
		{
			nTargetIndex = 1;
			distance = effect.nDistance;
		}
		break;
	case SKILL_EFFECT::DIS_MID_REL_ELEM: //相对中程(x,y都相对)
		{
			nTargetIndex = sklParam.vecpTarget.back()->getIndex();
			distance = effect.nDistance;
		}
		break;
	case SKILL_EFFECT::DIS_ENEMY_CENTER: //敌方阵位中心
		nTargetIndex = 4;
		break;
// 	case SKILL_EFFECT::DIS_LONG:		//原地释放
// 		{
// 
// 		}
// 		break;
	default:
		CCLOG("!!error type in getMidDisPosition: %d", effect.nDisType);
		break;
	}

	if (FightStateMachine::E_SIDE::MY_SIDE != sklParam.targetSide)
	{
		distance = -distance;
	}

	//主动技能透视视角需做特殊处理
	if (sklParam.nSklType == AbstractSkill::SKILL_TYPE::ACTIVE_SKILL)
	{
		pos = m_pParent->get3dPositionByIndex(sklParam.targetSide, nTargetIndex)+Size(distance, 0);
	}
	else
	{
		pos = m_pParent->getPositionByIndex(sklParam.targetSide, nTargetIndex)+Size(distance, 0);
	}

	return pos;
}

void UIFighter::startActionDead()
{
	CCLOG("-----UI[%d][%d]----- startActionDead!", ENUM_TO_INT(m_enSide), m_nIndex);

	mBuffDelegate->removeAllBuff();
	this->removeChildByName("AniBOSStexiao"); //移除持续播放的变身效果
	spTrackEntry* pEntry = m_pSkelFighter->setAnimation(0, szANI_DEAD, false);
	m_pSkelFighter->setTrackCompleteListener(pEntry, CC_CALLBACK_1(UIFighter::onActionDeadEnd, this));
}

bool UIFighter::isNoHarmSkill()
{
	if (sklParam.vecDamage.back().mHarmType == HarmParam::HARM_TYPE::HARM_NONE)
	{
		return true;
	}
	return false;
}


void UIFighter::onMotionReturnEnd()
{
	CCLOG("-----UI[%d][%d]----- onMotionReturnEnd", ENUM_TO_INT(m_enSide), m_nIndex);
	setOriginPosition();
	this->stopAllActions();
	playActionWait();
}

void UIFighter::addDamage(int nDamage, int damageType)
{
	this->addChild(UIFighterDamage::create(nDamage, damageType), ZORDER_DAMAGE);
}

void UIFighter::addBloodDamage()
{
	//判定是否有吸血效果
	if (sklParam.bloodHarm.mTargetIndex != -1)
	{
		addDamage(sklParam.bloodHarm.mValue, sklParam.bloodHarm.mHarmType);
	}
}

void UIFighter::onSkillPreEffectEnd()
{
	CCLOG("-----UI[%d][%d] onSkillPreEffectEnd!", ENUM_TO_INT(m_enSide), m_nIndex);

	m_bIsPreEffectEnd = true;

	onAttackFinish();
}

void UIFighter::onSkillActEffectEnd()
{
	CCLOG("-----UI[%d][%d] onSkillActEffectEnd!", ENUM_TO_INT(m_enSide), m_nIndex);

	m_bIsActEffectEnd = true;

	onAttackFinish();
}

void UIFighter::slowMotion()
{
	//判断是否有一方全部死亡
	if (FightModel::getInstance()->isAllFighterDead(FightStateMachine::E_SIDE::MY_SIDE) 
		|| FightModel::getInstance()->isAllFighterDead(FightStateMachine::E_SIDE::ENEMY_SIDE))
	{
		const float timeScale = 0.2f;
		const float duration = 2.0f * timeScale;
		Director::getInstance()->getScheduler()->setTimeScale(timeScale);
		//启动定时器，1s后减速还原
		Director::getInstance()->getScheduler()->schedule(
			schedule_selector(UIFighter::onSlowMotionReturn), this, 0, 0, duration, false);
	}
}

//执行慢动作回调
void UIFighter::onSlowMotionReturn(float dt)
{
	Director::getInstance()->getScheduler()->setTimeScale(1.0);
}

void UIFighter::setOriginZorder()
{
	this->setLocalZOrder(m_pParent->getZorderByIndex(m_enSide, m_nIndex));
}

void UIFighter::setOriginPosition()
{
	this->setPosition(m_pParent->getPositionByIndex(m_enSide, m_nIndex));
}

Point UIFighter::getOriginPosition()
{
	return m_pParent->getPositionByIndex(m_enSide, m_nIndex);
}

spine::SkeletonAnimation* UIFighter::getSkeletonAnimation()
{
	return m_pSkelFighter;
}

LayerFighter* UIFighter::getLayerFighter()
{
	return m_pParent;
}

void UIFighter::showTargetRushFlag(AbstractSkill::RESULT_STATE state)
{
	if (!mRushStateAni)
	{
		ArmatureDataManager::getInstance()->addArmatureFileInfo(
			"Image/AnicomboEnemy/AnicomboEnemy.ExportJson");
		mRushStateAni = Armature::create("AnicomboEnemy");
		mRushStateAni->setPosition(Point(0,0));
		this->addChild(mRushStateAni, 99);
	}

	mRushStateAni->stopAllActions();
	mRushStateAni->setVisible(true);
	mRushStateAni->setScale(1.0f);
	mRushStateAni->setOpacity(255);
	
	//Action回调
	auto actionCallBack = [this]()->void{mRushStateAni->setVisible(false);};

	//动画回调
	auto aniCallBack = [actionCallBack](Armature *armature, MovementEventType eventType, const std::string& strName)->void
	{
		if (eventType == MovementEventType::COMPLETE)
		{
			Sequence* seq = Sequence::create(
				DelayTime::create(1.5f),
				Spawn::create(FadeOut::create(0.15f), ScaleTo::create(0.15f, 0.0f), nullptr),
				CallFunc::create(actionCallBack), nullptr);
			armature->runAction(seq);
		}
	};

	mRushStateAni->getAnimation()->setMovementEventCallFunc(aniCallBack);

	switch (state)
	{
	case AbstractSkill::RESULT_STATE::FLOAT: //天
		mRushStateAni->getAnimation()->play("Ani_Tian", -1, 0);
		break;
	case AbstractSkill::RESULT_STATE::BACK: //地
		mRushStateAni->getAnimation()->play("Ani_Di", -1, 0);
		break;
	case AbstractSkill::RESULT_STATE::FELL: //人
		mRushStateAni->getAnimation()->play("Ani_Ren", -1, 0);
		break;
	default:
		aniCallBack(mRushStateAni, MovementEventType::COMPLETE, "");
		break;
	}
	
}


Fighter* UIFighter::getFighterData(bool isAssist)
{
	if (!isAssist)
	{
		return FightModel::getInstance()->getFighter(m_enSide, m_nIndex);
	}
	else
	{
		return FightModel::getInstance()->getAssistFighter(m_enSide, m_nIndex);
	}
}

void UIFighter::startActionJump(SKILL_EFFECT::MOVE_TYPE moveType, MOVE_SPEED_TYPE moveSpeedType, 
								bool hasShadow, bool hasEndAction, Point endPoint, float speed, int jumpHeight, 
								const function<void(void)>& callBack, bool isEndActionSpeedDown)
{
	string startActionName = (moveType == SKILL_EFFECT::MOVE_TYPE::MOVE_JUMP) ? "jump1" : "rush1";
	string endActionName = (moveType == SKILL_EFFECT::MOVE_TYPE::MOVE_JUMP) ? "jump2" : "rush2";
	jumpHeight = (moveType == SKILL_EFFECT::MOVE_TYPE::MOVE_JUMP) ? jumpHeight : 0;

	//播放跳跃开始动作
	auto pEntry = m_pSkelFighter->addAnimation(0, startActionName, false);
	m_pSkelFighter->setTimeScale(10.0f);

	//最终回调
	auto endCallBack = [this, callBack](int trackIndex, int loops)
		{
			m_pSkelFighter->setTimeScale(1.0f);
			callBack();
		};

	//播放跳跃结束动作
	auto jumpEndCallBack = 
		[this, endActionName, endCallBack, callBack, hasEndAction, isEndActionSpeedDown]()->void
	{
		mShade->setVisible(true);

		if (hasEndAction)
		{
			if (isEndActionSpeedDown)
			{
				m_pSkelFighter->setTimeScale(0.6f);
			}
			auto pEntry = m_pSkelFighter->setAnimation(0, endActionName, false);
			m_pSkelFighter->setTrackCompleteListener(pEntry, endCallBack);
		}
		else
		{
			m_pSkelFighter->setTimeScale(1.0f);
			callBack();
		}
	};

	//播放跳跃中间动作
	auto jumpStarCallBack = 
		[this, jumpEndCallBack, moveSpeedType, endPoint, speed, jumpHeight, hasShadow, moveType](int trackIndex, int loops)->void
	{
		float distance = this->getPosition().distance(endPoint);
		float duration = distance / speed; //持续时间补正
		float height = (distance / 1000.0f) * jumpHeight;

		//跳跃无阴影
		(moveType == SKILL_EFFECT::MOVE_TYPE::MOVE_JUMP) ? mShade->setVisible(false) : mShade->setVisible(true);

		auto jump = JumpTo::create(duration, endPoint, height, 1);
		ActionInterval* easeJump = nullptr;
		if (moveSpeedType == MOVE_SPEED_TYPE::MOVE_SPEED_UP)
		{
			easeJump = EaseIn::create(jump, 1.25f);
		}
		else if(moveSpeedType == MOVE_SPEED_TYPE::MOVE_SPEED_DOWN)
		{
			easeJump = EaseOut::create(jump, 1.25f);
		}
		else
		{
			easeJump = jump;
		}

		CallFunc* callBackAction = CallFunc::create(jumpEndCallBack);

		if (hasShadow)
		{
			Color3B color = getShadowColor();

			string shadowPath = (moveType == SKILL_EFFECT::MOVE_TYPE::MOVE_JUMP) ? "Image/Fight/jump.png" : "Image/Fight/dash.png";
			bool isFlip = !(m_enSide == FightStateMachine::E_SIDE::MY_SIDE);
			Point anchorPoint = (moveType == SKILL_EFFECT::MOVE_TYPE::MOVE_JUMP) ? Point(0.33, 0.14) : Point(0.24, 0.15);
			auto shadowAction = ShadowAction::create(duration, 0.4f, 1.0f, isFlip, anchorPoint, color, shadowPath);
			auto streakAction = StreakAction::create(duration + 0.3f, 0.3f, 200, Point(0, 100), color, "Image/Fight/aa3.png");
			this->runAction(Spawn::create(
				Sequence::create(easeJump, callBackAction, nullptr), shadowAction, streakAction, nullptr));
		}
		else
		{
			this->runAction(Sequence::create(easeJump, callBackAction, nullptr));
		}

	};

	m_pSkelFighter->setTrackCompleteListener(pEntry, jumpStarCallBack);
}

Color3B UIFighter::getShadowColor()
{
	switch (getFighterData()->getRushSkill()->getPreStateType())
	{
	case AbstractSkill::RESULT_STATE::BACK: //断筋 地
		return Color3B(0xd9, 0x45, 0x2b);
	case AbstractSkill::RESULT_STATE::FELL: //碎骨 人
		return Color3B(0x44, 0xac ,0x55);
	case AbstractSkill::RESULT_STATE::FLOAT: //气绝 天
		return Color3B(0x20, 0x8c ,0xa0);
	default:
		return Color3B(0x7e, 0x3b, 0x71);
		break;
	}
}

void UIFighter::startRushAniAndPause()
{
	//此时追打起手技能已经播放完，下一次攻击数据已经到达，但是需要在上一个攻击者以及上一个被附加状态的被击者显示状态
	UIFighter* fighter = m_pParent->getLastAttacker();
	UIFighter* lastStateFighter = this->m_pParent->getLastStateFighter();
	if (fighter && lastStateFighter)
	{
		SchedulerController::getInstance()->pauseAllTargets(0.15f);

		auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
		{
			armature->removeFromParent();
		};

		auto frameCallBack = [this](Bone *bone, const string& evt, int originFrameIndex, int currentFrameIndex)->void
		{
			if(evt == "next")
			{
				this->m_pParent->getLastStateFighter()->showTargetRushFlag(this->m_pParent->getLastState());
			}
		};

		ArmatureDataManager::getInstance()->addArmatureFileInfo(
			"Image/AniComboCF/AniComboCF.ExportJson");
		Armature* aniFlash = Armature::create("AniComboCF");
		aniFlash->setPosition(Point(0,0));
		aniFlash->getAnimation()->playByIndex(0);
		aniFlash->getAnimation()->setMovementEventCallFunc(aniCallBack);
		aniFlash->getAnimation()->setFrameEventCallFunc(frameCallBack);

		if (m_enSide == FightStateMachine::ENEMY_SIDE)
		{
			aniFlash->setScaleX(-1);
		}

		fighter->addChild(aniFlash, 999);
	}
}

void UIFighter::updateBuff(BuffUpdateParam* buffUpdateParam)
{
	mBuffDelegate->updateBuff(buffUpdateParam);
}

void UIFighter::removeAllUIBuff()
{
	mBuffDelegate->removeAllBuff();
}

bool UIFighter::isPointEqual(const Point& p1, const Point& p2)
{
	return abs(p1.x - p2.x) < 0.01f && abs(p1.y - p2.y) < 0.01f;
}