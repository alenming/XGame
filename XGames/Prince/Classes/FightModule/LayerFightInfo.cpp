#include "LayerFightInfo.h"
#include "FightModel.h"
#include "CocoStudio.h"
#include "SceneFight.h"
#include "GuideCenter/GuideManager.h"
#include <cmath>
#include <sstream>
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "Common/Common.h"
#include "LayerManualSkill.h"
#include "Widget/SkillIcon.h"
#include "Common/Common.h"
#include "Utils/SoundUtils.h"

using namespace cocos2d;
using namespace cocos2d::ui;

LayerFightInfo::LayerFightInfo()
	: mMyComboItem(nullptr)
	, mEnemyComboItem(nullptr)
	, mX2SpeedClickCount(0)
	, mRushCount(0)
{

}

LayerFightInfo::~LayerFightInfo()
{

}

bool LayerFightInfo::init()
{
	if (!Layer::init())
	{
		return false;
	}

	if (!initUI())
	{
		return false;
	}

	return true;
}

void LayerFightInfo::onEnter()
{
	Layer::onEnter();
	mParent = dynamic_cast<SceneFight*>(this->getParent());

	//左上角按钮动画
	Point topLeftPoint = m_ui.pBtnLeftPanel->getPosition();
	auto topLeftAction = EaseSineOut::create(MoveTo::create(0.13f, topLeftPoint));
	m_ui.pBtnLeftPanel->setPosition(topLeftPoint + Point(0, 100));
	m_ui.pBtnLeftPanel->runAction(topLeftAction);

	//右上角按钮动画
	Point topRightPoint = m_ui.pBtnRightPanel->getPosition();
	auto topRightAction = EaseSineOut::create(MoveTo::create(0.13f, topRightPoint));
	m_ui.pBtnRightPanel->setPosition(topRightPoint + Point(0, 100));
	m_ui.pBtnRightPanel->runAction(topRightAction);
	
	//技能栏动画
	Point bottomPoint = m_ui.pSpBackImg->getPosition();
	auto bottomAction = EaseSineOut::create(MoveTo::create(0.13f, bottomPoint));
	m_ui.pSpBackImg->setPosition(bottomPoint + Point(0, -150));
	m_ui.pSpBackImg->runAction(bottomAction);
}

void LayerFightInfo::onExit()
{
	Layer::onExit();
}

bool LayerFightInfo::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UInewfight/UInewfight_info.ExportJson"));
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);

	//怒气条
	Layout* bottomPanel = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_bottom"));
	m_ui.pSpBackImg = dynamic_cast<ImageView*>(bottomPanel->getChildByName("Image_di_nuqi"));
	m_ui.pSpTextImg = dynamic_cast<ImageView*>(m_ui.pSpBackImg->getChildByName("Image_wenzi_nuqi"));
	m_ui.pSpText = dynamic_cast<Text*>(m_ui.pSpBackImg->getChildByName("lab_shuzi_nuqi"));
	m_ui.pSpText->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.pSpText, Color3B(0x0c, 0x11, 0x31), 3);

	m_ui.mRedBar = ProgressTimer::create(Sprite::create("Image/Fight/nuqired.png"));
	m_ui.mRedBar->setType(ProgressTimer::Type::BAR);
	m_ui.mRedBar->setMidpoint(Vec2(0,0));
	m_ui.mRedBar->setBarChangeRate(Vec2(1, 0));
	m_ui.mRedBar->setPosition(m_ui.pSpBackImg->getContentSize() / 2);
	m_ui.pSpBackImg->addChild(m_ui.mRedBar);

	m_ui.mBlueBar = ProgressTimer::create(Sprite::create("Image/Fight/nuqiblue.png"));
	m_ui.mBlueBar->setType(ProgressTimer::Type::BAR);
	m_ui.mBlueBar->setMidpoint(Vec2(0,0));
	m_ui.mBlueBar->setBarChangeRate(Vec2(1, 0));
	m_ui.mBlueBar->setPosition(m_ui.pSpBackImg->getContentSize() / 2);
	m_ui.pSpBackImg->addChild(m_ui.mBlueBar);

	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/Aninuqi/Aninuqi.ExportJson");
	m_ui.mSpAni = Armature::create("Aninuqi");
	m_ui.mSpAni->setPosition(m_ui.pSpTextImg->getPosition());
	m_ui.mSpAni->setVisible(false);
	m_ui.pSpBackImg->addChild(m_ui.mSpAni);
	

	Layout* rightPanel = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_Right"));
	m_ui.pBtnRightPanel = dynamic_cast<Layout*>(rightPanel->getChildByName("Panel_btn_parent"));
	//加速按钮
	m_ui.pSpeedX1Btn = dynamic_cast<Button*>(m_ui.pBtnRightPanel->getChildByName("btn_jiasu1"));
	m_ui.pSpeedX1Btn->addTouchEventListener(CC_CALLBACK_2(LayerFightInfo::onBtnClicked, this, FIGHT_BTN_ID::SPEED_X1));
	m_ui.pSpeedX1Btn->setVisible(true);
	m_ui.pSpeedX2Btn = dynamic_cast<Button*>(m_ui.pBtnRightPanel->getChildByName("btn_jiasu2"));
	m_ui.pSpeedX2Btn->addTouchEventListener(CC_CALLBACK_2(LayerFightInfo::onBtnClicked, this, FIGHT_BTN_ID::SPEED_X2));
	m_ui.pSpeedX2Btn->setVisible(false);
	m_ui.pSpeedX3Btn = dynamic_cast<Button*>(m_ui.pBtnRightPanel->getChildByName("btn_jiasu3"));
	m_ui.pSpeedX3Btn->addTouchEventListener(CC_CALLBACK_2(LayerFightInfo::onBtnClicked, this, FIGHT_BTN_ID::SPEED_X3));
	m_ui.pSpeedX3Btn->setVisible(false);
	//自动按钮
	m_ui.pAutoFightBtn = dynamic_cast<Button*>(m_ui.pBtnRightPanel->getChildByName("btn_zidong"));
	m_ui.pAutoFightBtn->addTouchEventListener(CC_CALLBACK_2(LayerFightInfo::onBtnClicked, this, FIGHT_BTN_ID::AUTO_FIGHT));
	m_ui.pAutoFightBtn->setVisible(true);
	//手动按钮
	m_ui.pManualFightBtn = dynamic_cast<Button*>(m_ui.pBtnRightPanel->getChildByName("btn_shoudong"));
	m_ui.pManualFightBtn->addTouchEventListener(CC_CALLBACK_2(LayerFightInfo::onBtnClicked, this, FIGHT_BTN_ID::MANUAL_FIGHT));
	m_ui.pManualFightBtn->setVisible(false);

	Layout* leftPanel = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_left"));
	m_ui.pBtnLeftPanel = dynamic_cast<Layout*>(leftPanel->getChildByName("Panel_btn_parent"));
	//逃跑按钮
	m_ui.pEscapeBtn = dynamic_cast<Button*>(m_ui.pBtnLeftPanel->getChildByName("btn_taopao"));
	m_ui.pEscapeBtn->addTouchEventListener(CC_CALLBACK_2(LayerFightInfo::onBtnClicked, this, FIGHT_BTN_ID::ESCAPE));
	m_ui.pEscapeBtn->setVisible(true);
	m_ui.leftBtnPoint1 = m_ui.pEscapeBtn->getPosition();

	//跳过按钮
	m_ui.pSkipBtn = dynamic_cast<Button*>(m_ui.pBtnLeftPanel->getChildByName("btn_tiaoguo"));
	m_ui.pSkipBtn->addTouchEventListener(CC_CALLBACK_2(LayerFightInfo::onBtnClicked, this, FIGHT_BTN_ID::SKIP));
	m_ui.pSkipBtn->setVisible(false);

	//Combo
	m_ui.pMyComboPanel = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_leftcombo"));
	m_ui.pEnemyComboPanel = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_rightcombo"));
	mMyComboItem = UIComboItem::create(true);
	mEnemyComboItem = UIComboItem::create(false);
	mMyComboItem->setPosition(m_ui.pMyComboPanel->getContentSize() / 2);
	mEnemyComboItem->setPosition(m_ui.pEnemyComboPanel->getContentSize() / 2);
	m_ui.pMyComboPanel->addChild(mMyComboItem);
	m_ui.pEnemyComboPanel->addChild(mEnemyComboItem);
	this->addChild(m_ui.pRoot);

	//回合
	m_ui.pRoundPnl = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_cankao"));
	m_ui.pRoundImg = dynamic_cast<ImageView*>(m_ui.pRoundPnl->getChildByName("Image_wenzi_huihe"));
	m_ui.pRoundAtlas = dynamic_cast<TextAtlas*>(m_ui.pRoundPnl->getChildByName("Atl_shuzi_huihe"));
	m_ui.pRoundPnl->setVisible(false);

	//速度
	m_ui.pMySpeed = dynamic_cast<Text*>(m_ui.pRoot->getChildByName("Label_sudu1"));
	m_ui.pMySpeed->setText(TO_STR(FightModel::getInstance()->getMyFirstValue()));
	m_ui.pMySpeed->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.pMySpeed, Color3B::BLACK, 2);
	m_ui.pEnemySpeed = dynamic_cast<Text*>(m_ui.pRoot->getChildByName("Label_sudu2"));
	m_ui.pEnemySpeed->setText(TO_STR(FightModel::getInstance()->getEnemyFirstValue()));
	m_ui.pEnemySpeed->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.pEnemySpeed, Color3B::BLACK, 2);

	//自动战斗动画
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniAutoFight/AniAutoFight.ExportJson");
	m_ui.mAutoFightAni = Armature::create("AniAutoFight");
	m_ui.mAutoFightAni->getAnimation()->play("AniAniAutoFight");
	m_ui.mAutoFightAni->setPosition(this->getContentSize() / 2);
	m_ui.mAutoFightAni->setVisible(false);
	this->addChild(m_ui.mAutoFightAni);

	updateUI();

	return true;
}

void LayerFightInfo::updateUI()
{
	int sp = FightModel::getInstance()->getMyTotalSp();
	int maxSp = FightModel::getInstance()->getTotalMaxSp();
	float percent = 100.0f*sp/maxSp;

	if (percent < m_ui.mBlueBar->getPercentage()) //怒气减少
	{
		auto to1 = ProgressTo::create(0.25f, percent);
		auto to2 = Sequence::create(DelayTime::create(0.15f), to1->clone(), nullptr);
		m_ui.mBlueBar->stopAllActions();
		m_ui.mRedBar->stopAllActions();
		m_ui.mBlueBar->runAction(to1);
		m_ui.mRedBar->runAction(to2);
		
	}
	else if(percent > m_ui.mBlueBar->getPercentage()) //怒气增加
	{
		auto to = ProgressTo::create(0.25f, percent);
		m_ui.mBlueBar->stopAllActions();
		m_ui.mRedBar->stopAllActions();
		m_ui.mBlueBar->runAction(to);
		m_ui.mRedBar->runAction(to->clone());
	}
	m_ui.mSpAni->setVisible(sp == maxSp);
	if (sp == maxSp)
	{
		//怒气满时播放燃烧动画
		m_ui.mSpAni->getAnimation()->play("AniNuqi_2");
	}
	m_ui.pSpText->setString(TO_STR(sp) + "/" + TO_STR(maxSp));
}

void LayerFightInfo::onBtnClicked(Ref* ref, Widget::TouchEventType type, FIGHT_BTN_ID btnId)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (btnId)
		{
		case FIGHT_BTN_ID::SPEED_X1:
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_SPEED_X2))
			{
				SetIntegerToXML(LOCAL_KEY_INT_STR_FIGHT_SPEED, 2);
				setFightSpeed(2);
			}
			break;
		case FIGHT_BTN_ID::SPEED_X2:
			{
				mX2SpeedClickCount++;

				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_SPEED_X3, false))
				{
					SetIntegerToXML(LOCAL_KEY_INT_STR_FIGHT_SPEED, 3);
					setFightSpeed(3);
				}
				else
				{
					//第一次点击弹出提示框
					if (mX2SpeedClickCount % 2 == 1)
					{
						GuideManager::getInstance()->isFunctionOpen(FUNCTION_SPEED_X3);
					}
					else
					{
						//第二次点击再改速度
						SetIntegerToXML(LOCAL_KEY_INT_STR_FIGHT_SPEED, 1);
						setFightSpeed(1);
					}
				}
			}
			break;
		case FIGHT_BTN_ID::SPEED_X3:
			{
				SetIntegerToXML(LOCAL_KEY_INT_STR_FIGHT_SPEED, 1);
				setFightSpeed(1);
			}
			break;
		case FIGHT_BTN_ID::AUTO_FIGHT:
			if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_AUTO_FIGHT))
			{
				SetBoolToXML(LOCAL_KEY_BOOL_AUTO_FIGHT, true);
				setAutoFight(true);
				mParent->getLayerManualSkill()->updateUI(true); //更新主动技能栏
			}
			break;
		case FIGHT_BTN_ID::MANUAL_FIGHT:
			{
				SetBoolToXML(LOCAL_KEY_BOOL_AUTO_FIGHT, false);
				setAutoFight(false);
			}
			break;
		case FIGHT_BTN_ID::SKIP:
			{
				//跳过开启自动战斗
				mParent->setSkip(true);
				setAutoFight(true);
				break;
			}
		case FIGHT_BTN_ID::ESCAPE:
			{
				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_FIGHT_ESCAPE))
				{
					/*
					CustomPop::show("是否确定逃离战斗？\n提示：逃跑将无法获得奖励", CC_CALLBACK_2(LayerFightInfo::escapeCallBack, this), 
						CustomPopType::CUSTOM_YES_NO);
						*/
					CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10087), CC_CALLBACK_2(LayerFightInfo::escapeCallBack, this), 
						CustomPopType::CUSTOM_YES_NO);
					if (!Director::getInstance()->isPaused())
					{
						Director::getInstance()->pause();
						//Director::getInstance()->setAnimationInterval(1 / 60.0f);
					}
				}
			}
			break;

		default:
			break;
		}
	}

}

void LayerFightInfo::escapeCallBack( Ref* pSender, CustomRetType type )
{
	if (type == RET_TYPE_OK)
	{
		mParent->jumpScene();
	}

	if (Director::getInstance()->isPaused())
	{
		Director::getInstance()->resume();
	}

	mParent->onResultTalkingData(true);
}

void LayerFightInfo::setFightSpeed(int speedType, bool isLock)
{
	switch (speedType)
	{
	case 1:
		{
			Director::getInstance()->getScheduler()->setTimeScale(1.0f);
			m_ui.pSpeedX1Btn->setVisible(true);
			m_ui.pSpeedX2Btn->setVisible(false);
			m_ui.pSpeedX3Btn->setVisible(false);

			if (isLock)
			{
				m_ui.pSpeedX1Btn->setBright(false);
			}
		}
		break;
	case 2:
		{
			Director::getInstance()->getScheduler()->setTimeScale(2.0f);
			m_ui.pSpeedX1Btn->setVisible(false);
			m_ui.pSpeedX2Btn->setVisible(true);
			m_ui.pSpeedX3Btn->setVisible(false);

			if (isLock)
			{
				m_ui.pSpeedX2Btn->setBright(false);
			}
		}
		break;
	case 3:
		{
			Director::getInstance()->getScheduler()->setTimeScale(3.0f);
			m_ui.pSpeedX1Btn->setVisible(false);
			m_ui.pSpeedX2Btn->setVisible(false);
			m_ui.pSpeedX3Btn->setVisible(true);

			if (isLock)
			{
				m_ui.pSpeedX3Btn->setBright(false);
			}
		}
		break;
	default:
		break;
	}
}

void LayerFightInfo::setFightSpeedBtnHide()
{
	m_ui.pSpeedX1Btn->setVisible(false);
	m_ui.pSpeedX2Btn->setVisible(false);
	m_ui.pSpeedX3Btn->setVisible(false);
}

void LayerFightInfo::setAutoFight(bool autoFight, bool gray)
{
	SceneFight* parent = dynamic_cast<SceneFight*>(this->getParent());
	parent->getFSM()->setAutoFight(autoFight);
	m_ui.pAutoFightBtn->setVisible(!autoFight);
	m_ui.pManualFightBtn->setVisible(autoFight);
	m_ui.mAutoFightAni->setVisible(autoFight);
	if (gray)
	{
		m_ui.pAutoFightBtn->setBright(false);
		m_ui.pManualFightBtn->setBright(false);
		m_ui.pManualFightBtn->setTouchEnabled(false);
		m_ui.pAutoFightBtn->setTouchEnabled(false);
	}
}

void LayerFightInfo::setManualFightBtnDisable()
{
	m_ui.pManualFightBtn->setBright(false);
}

//设置跳过按钮和逃跑按钮
void LayerFightInfo::setSkipEscapeBtnVisible(bool skip, bool escape)
{
	m_ui.pSkipBtn->setVisible(skip);
	m_ui.pEscapeBtn->setVisible(escape);
	if (skip && !escape)
	{
		m_ui.pSkipBtn->setPosition(m_ui.leftBtnPoint1);
	}
}

void LayerFightInfo::clearHitCount()
{
	//清空计数
	if (mMyComboItem)
	{
		mMyComboItem->setComboCount(0);
	}

	if (mEnemyComboItem)
	{
		mEnemyComboItem->setComboCount(0);
	}
}

void LayerFightInfo::addHitCount(FightStateMachine::E_SIDE side)
{
	if (side == FightStateMachine::E_SIDE::MY_SIDE)
	{
		mMyComboItem->addComboCount();
	}
	else
	{
		mEnemyComboItem->addComboCount();
	}
}

void LayerFightInfo::setSkipBtnVisible(bool visible)
{
	m_ui.pSkipBtn->setVisible(visible);
}

void LayerFightInfo::setSpeedBtnVisible(bool visible)
{
	if (visible)
	{
		m_ui.pSpeedX1Btn->setVisible(true);
		m_ui.pSpeedX2Btn->setVisible(false);
		m_ui.pSpeedX3Btn->setVisible(false);
	}
	else
	{
		m_ui.pSpeedX1Btn->setVisible(false);
		m_ui.pSpeedX2Btn->setVisible(false);
		m_ui.pSpeedX3Btn->setVisible(false);
	}
}

void LayerFightInfo::setEscapeBtnVisible(bool visible)
{
	m_ui.pEscapeBtn->setVisible(visible);
}

void LayerFightInfo::playRoundAni()
{
	m_ui.pRoundPnl->setVisible(true);
	m_ui.pRoundAtlas->setString(TO_STR(FightModel::getInstance()->getRoundCount()));
	auto seq = Sequence::create(
		ScaleTo::create(0.2f, 1.5f),
		ScaleTo::create(0.1f, 1.0f),
		nullptr
	);
	m_ui.pRoundPnl->runAction(seq);
}

void LayerFightInfo::showSkillTips(int skillId, int skillType)
{
	if (skillType != AbstractSkill::SKILL_TYPE::NORMAL_SKILL && skillType != AbstractSkill::SKILL_TYPE::BUFF_SKILL)
	{
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniSkilltips/AniSkilltips.ExportJson");
		Armature* skillTips = Armature::create("AniSkilltips");
		skillTips->getAnimation()->play("Animation1");
		skillTips->setPosition(this->getContentSize() / 2);
		this->addChild(skillTips);

		auto aniCallBack = [](Armature *armature, MovementEventType eventType, const std::string& strName)->void
		{
			armature->removeFromParent();
		};
		skillTips->getAnimation()->setMovementEventCallFunc(aniCallBack);

		RowData* data = DataManager::getInstance()->searchSkillById(skillId);
		Text* text = Text::create(data->getStringData("name"), FONT_FZZHENGHEI, 30);
		text->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
		skillTips->getBone("text")->addDisplay(text, 0);
		setTextAddStroke(text, Color3B::BLACK, 3);
		
		Skin* skin = nullptr;
		if (skillType == AbstractSkill::SKILL_TYPE::ACTIVE_SKILL || skillType == AbstractSkill::SKILL_TYPE::TRIGGER_SKILL)
		{
			skin = Skin::create(ResourceUtils::getSmallIconPath(data->getIntData("resId")));
			skillTips->getBone("skill")->addDisplay(skin, 0);
		}
		else
		{
			mRushCount++;
			skin = Skin::create(ResourceUtils::getIconGlobalPath("img_zhui" + TO_STR(mRushCount) + ".png"));
			skillTips->getBone("zhuida")->addDisplay(skin, 0);
		}
		
	}

	//非追打技能清0
	if (skillType != AbstractSkill::SKILL_TYPE::RUSH_SKILL)
	{
		mRushCount = 0;
	}
}
