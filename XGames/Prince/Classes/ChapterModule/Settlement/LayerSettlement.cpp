#include "LayerSettlement.h"
#include "Temp/CustomPop.h"
#include "ChapterModule/SceneChapterLoading.h"
#include "HeroModule/SceneHero.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/SceneTool.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainCityScene.h"
#include "MainModule/MainModel.h"
#include "Widget/LayerWaiting.h"
#include "LevelUp/LayerLevelUp.h"
#include "FightModule/SceneFight.h"
#include "TowerModule/SceneTower.h"
#include "GuideCenter/GuideManager.h"
#include "ChapterModule/ChapterModel.h"
#include "MainModule/MainCityScene.h"
#include "FightModule/SceneFight.h"
#include "FightModule/LayerFighter.h"
#include "FormationModule/SceneFormation.h"
#include "TakeCardModule/SceneTakeCard.h"
#include "LevelUp/LevelUpController.h"
#include "Utils/SoundUtils.h"
#include "TakeCardModule/TakeCardModel.h"
#include "ChapterModule/Activity/ActivityModel.h"

LayerSettlement::LayerSettlement(void)
	: m_pLogicParent(nullptr)
{
}


LayerSettlement::~LayerSettlement(void)
{
}

LayerSettlement* LayerSettlement::create(SettlementInfo info)
{
	LayerSettlement* _layer = new LayerSettlement;
	if (nullptr != _layer && _layer->init(info))
	{
		_layer->autorelease();
		return _layer;
	}

	CC_SAFE_DELETE(_layer);
	return nullptr;
}


bool LayerSettlement::init(SettlementInfo info)
{
	if (!Layer::init())
	{
		return false;
	}

	m_settlementInfo = info;

	initUI();

	return true;
}

void LayerSettlement::onEnter()
{
	Layer::onEnter();

	// 音效;
	if (m_settlementInfo.bWin)
	{
		SoundUtils::playSoundEffect("shengli");
	}
	else
	{
		SoundUtils::playSoundEffect("shibai");
	}

	//GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_STAGE_RESULT);
}

void LayerSettlement::onExit()
{
	// 释放资源;
	Layer::onExit();
}


void LayerSettlement::initUI()
{
	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	///// Root;
	m_pRoot = Layout::create();
	m_pRoot->setContentSize(Director::getInstance()->getVisibleSize());
	m_pRoot->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	m_pRoot->setPosition(pos);
	m_pRoot->setTouchEnabled(true);

	///// Win;
	if (m_settlementInfo.bWin)
	{
		// 胜利动画(1/3) - 胜利标识飞入;
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniWin_1/AniWin_1.ExportJson");
		_winArmature1 = Armature::create("AniWin_1");
		_winArmature1->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
		m_pRoot->addChild(_winArmature1, 1);
		_winArmature1->getAnimation()->play("win");

		// 胜利动画(2/4) - 获得星级;
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniCountingStars/AniCountingStars.ExportJson");
		ostringstream oss;
		oss << "AniCountingStars" << m_settlementInfo.nStarCount;
		_winArmature2 = Armature::create("AniCountingStars");
		_winArmature2->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
		m_pRoot->addChild(_winArmature2, 3);
		_winArmature2->getAnimation()->play(oss.str());

		_winArmature2->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerSettlement::movementEventCallFunc, this));

		_winArmature2->getAnimation()->setFrameEventCallFunc([=](Bone *bone, const std::string& frameEventName, int originFrameIndex, int currentFrameIndex)->void{
			if (frameEventName.compare("sound") == 0)
			{
				// 播放音效;
				SoundUtils::playSoundEffect("xingxing1");
			}
		});

	}
	///// Lose;
	else
	{
		Armature* _loseArmature = nullptr;
		if (m_settlementInfo.nFightType != FIGHT_TYPE_FIND)
		{
			// 失败动画;
			ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniLost/AniLost.ExportJson");
			_loseArmature = Armature::create("AniLost");


			// 处理按钮可点击效果;
			Bone* bonePos1 = _loseArmature->getBone("btn_huobanzhaomu_1.png");
			enableTouch(bonePos1, Btn_Tag_Pos_1);
			Bone* bonePos2 = _loseArmature->getBone("btn_huobanyangcheng_1.png");
			enableTouch(bonePos2, Btn_Tag_Pos_2);
			Bone* bonePos3 = _loseArmature->getBone("btn_zhuangbeiyangcheng_1.png");
			enableTouch(bonePos3, Btn_Tag_Pos_3);
			Bone* bonePos4 = _loseArmature->getBone("btn_zhenxingqianghua_1.png");
			enableTouch(bonePos4, Btn_Tag_Pos_4);
		}
		else
		{
			// 失败动画;
			ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniLost2/AniLost2.ExportJson");
			_loseArmature = Armature::create("AniLost2");

		}
		_loseArmature->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
		m_pRoot->addChild(_loseArmature, 1);
		_loseArmature->getAnimation()->play("lost");

		_loseArmature->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LayerSettlement::movementEventCallFunc, this));
	}
	this->addChild(m_pRoot);

}


void LayerSettlement::onLayerTouched(Ref* ref, Widget::TouchEventType type, int nWidgetName)
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		// 触摸一次之后禁用;
		LayerWaiting* _layer = LayerWaiting::create();
		if (nullptr != _layer)
		{
			this->addChild(_layer, 99);
		}

		auto callback = [=](){
			// 升级页面流转逻辑;
			if (m_settlementInfo.bWin)
			{
				if (LevelUpController::getInstance()->checkLevelUp(CC_CALLBACK_0(LayerSettlement::onLevelUpDone, this)))
				{
					m_pRoot->setVisible(false);
					return;
				}
			}

			SceneFight* sceneFight = dynamic_cast<SceneFight*>(this->getParent());
			if (sceneFight)
			{
				sceneFight->jumpScene();
			}
		};

		// 整卡获取;
		bool bFoundCard = false;
		for (auto iter = m_settlementInfo.mapReward.begin(); iter != m_settlementInfo.mapReward.end(); ++iter)
		{
			RowData* row = DataManager::getInstance()->searchToolById(iter->first);
			if (row != nullptr)
			{
				if (row->getIntData("itemTyp") == TOOL_ITEM_TYPE_HERO)
				{
					bFoundCard = true;
					_layer->removeFromParent();
					LayerGetCard *_layerGetCard = LayerGetCard::create(iter->first, false, iter->second, callback);
					Director::getInstance()->getRunningScene()->addChild(_layerGetCard, 99);
					break;
				}
			}
		}
		if (!bFoundCard)
		{
			callback();
		}
	}
}

void LayerSettlement::movementEventCallFunc( Armature *armature, MovementEventType movementType, const std::string& movementID )
{
	if (movementType == COMPLETE)
	{
		ostringstream oss;
		oss << "AniCountingStars" << m_settlementInfo.nStarCount;
		if (movementID.compare(oss.str()) == 0)
		{
			//log("======== win complete ========");
			Size visibleSize = Director::getInstance()->getVisibleSize();
			Point pos = Director::getInstance()->getVisibleOrigin();

			// 移除第一段动画;
			_winArmature1->removeFromParent();

			// 胜利动画(3/4) - 胜利标识旋转;
			ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniWin_2/AniWin_2.ExportJson");
			Armature* _winArmature3 = Armature::create("AniWin_2");
			_winArmature3->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
			m_pRoot->addChild(_winArmature3, 2);
			_winArmature3->getAnimation()->play("win");

			// 第三段/第四段同时启动;
			// 胜利动画(4/4) - 奖励背景飞入;
			ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniWin_3/AniWin_3.ExportJson");
			Armature* _winArmature4 = Armature::create("AniWin_3");
			_winArmature4->setPosition(Vec2(visibleSize.width/2 - pos.x, visibleSize.height/2));

			m_pRoot->addChild(_winArmature4, 1);
			_winArmature4->getAnimation()->play("aniwinjifen");

			// 图标替换;
			if (m_settlementInfo.nFightType == FIGHT_TYPE_TRAIL)			// 积分;
			{
				Bone* _bone = _winArmature4->getBone("img_jifen.png");
				Skin* _newSkin = Skin::create("Image/Icon/global/img_jifen.png");
				_bone->addDisplay(_newSkin, 1);
			}

			// 图标隐藏(迷宫/情缘);
			if (m_settlementInfo.nFightType == FIGHT_TYPE_MAZE_TRUE
				|| m_settlementInfo.nFightType == FIGHT_TYPE_FIND)		// 无;
			{
				Bone* _bone = _winArmature4->getBone("img_jifen.png");
				Skin* _newSkin = Skin::create("Image/Icon/Small/-1.png");
				_bone->addDisplay(_newSkin, 1);
			}

			// 第二段动画和星星同步位移;
			auto callBack = []()
			{ 
				SoundUtils::playSoundEffect("shengli");
			};
			Action* seq = Sequence::create(EaseSineInOut::create(MoveTo::create(0.3f,
				Vec2(visibleSize.width - 200.0f, visibleSize.height - 170.0f))), nullptr);
			_winArmature3->runAction(seq);
			FiniteTimeAction* seq2 = (FiniteTimeAction*)(seq->clone());
			_winArmature2->runAction(Sequence::create(seq2,  CallFunc::create(callBack), nullptr));

			_winArmature4->getAnimation()->setFrameEventCallFunc(bind(&LayerSettlement::frameEventCallFunc, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 ));
		}
		else if (movementID.compare("lost") == 0)
		{
			//log("======== lose complete ========");
			// 爬塔失败原因;
			if (m_settlementInfo.nFightType == FIGHT_TYPE_TOWER)
			{
				Text* txtReason = Text::create(m_settlementInfo.strReason, FONT_NAME_FOUNDER_BOLD, 33);
				txtReason->setColor(Color3B(0x81, 0x8C, 0xAF));
				setTextAddStroke(txtReason, Color3B(0x0D, 0x0F, 0x47), 2);
				txtReason->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				txtReason->setPosition(Vec2(50.0f, 340.0f));
				m_pRoot->addChild(txtReason, 3);
			}

			// 添加触摸响应;
			showTouchContinue(m_settlementInfo.bWin);
		}
	}
}

void LayerSettlement::frameEventCallFunc( Bone *bone, const std::string& frameEventName, int originFrameIndex, int currentFrameIndex )
{
	//log("===== %s, %s, %d, %d ===== ", bone->getName().c_str(), frameEventName.c_str(), originFrameIndex, currentFrameIndex);
	if (frameEventName.compare("point1") == 0)
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();
		Point pos = Director::getInstance()->getVisibleOrigin();

		// Coin;
		ostringstream oss;
		oss << m_settlementInfo.nCoin;
		Text* txtCoin = Text::create(oss.str(), FONT_NAME_FOUNDER_BOLD, 35);
		txtCoin->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		txtCoin->setColor(Color3B(0xEA, 0xE9, 0xE2));
		txtCoin->setPosition(Vec2(visibleSize.width - 125.0f, 265.0f));
		m_pRoot->addChild(txtCoin, 3);
		oss.str("");

		// Exp;
		if (m_settlementInfo.nFightType != FIGHT_TYPE_MAZE_TRUE
			&& m_settlementInfo.nFightType != FIGHT_TYPE_FIND)
		{
			if (m_settlementInfo.nFightType == FIGHT_TYPE_TRAIL)
			{
				oss << m_settlementInfo.nScore;
			}
			else
			{
				oss << m_settlementInfo.nExp;
			}
			Text* txtExp = Text::create(oss.str(), FONT_NAME_FOUNDER_BOLD, 35);
			txtExp->setColor(Color3B(0xEA, 0xE9, 0xE2));
			txtExp->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			txtExp->setPosition(Vec2(txtCoin->getPosition().x - 125.0f, txtCoin->getPosition().y));
			m_pRoot->addChild(txtExp, 3);
			oss.str();
		}

		// 活动关卡完成度;
		if (m_settlementInfo.nFightType == FIGHT_TYPE_PVE_ACTIVITY
			&& (ActivityModel::getInstance()->getCurActivityType() == ACT_TYPE_COIN_LiangShanMiCang
				|| ActivityModel::getInstance()->getCurActivityType() == ACT_TYPE_EXP_JiuJingKaoYan))
		{
			ImageView* imgCPTitle = ImageView::create("Image/UIChapter/Settlement/img_guanqiawanchengdu.png");
			imgCPTitle->setPosition(Vec2(m_pRoot->getContentSize().width - 240.0f, 220.0f));
			m_pRoot->addChild(imgCPTitle, 5);

			char szCP[32] = {0};
			if (m_settlementInfo.fCompletionPercent > 99.9f || m_settlementInfo.fCompletionPercent < 0.1f)
				sprintf(szCP, "%d%%", (int)(m_settlementInfo.fCompletionPercent));
			else
				sprintf(szCP, "%0.1lf%%", m_settlementInfo.fCompletionPercent);

			Text* txtCP = Text::create(szCP, FONT_FZZHENGHEI, 35);
			txtCP->setColor(Color3B(0xF5, 0xDF, 0x4D));
			txtCP->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
			txtCP->setPosition(imgCPTitle->getPosition() + Point(imgCPTitle->getContentSize().width/2.0f, -2.0f));
			m_pRoot->addChild(txtCP, 5);
		}

		// 展示所有英雄经验升级动画;
		showGlobalExp();

		// item title;
		ImageView* imgTitle = ImageView::create("Image/UIChapter/Settlement/img_zhandoujiangli.png");
		imgTitle->setPosition(Vec2(m_pRoot->getContentSize().width - imgTitle->getContentSize().width/2, 155.0f));
		m_pRoot->addChild(imgTitle, 5);

		// 掠夺结果;
		if (m_settlementInfo.nFightType == FIGHT_TYPE_ROB_FIGHT)
		{
			ImageView* imgRobResult = nullptr;
			if (m_settlementInfo.bWin && m_settlementInfo.bRobDone)
			{
				imgRobResult = ImageView::create("Image/UIChapter/Settlement/img_lveduochenggong.png");
			}
			else
			{
				imgRobResult = ImageView::create("Image/UIChapter/Settlement/img_meiqiangdao.png");
			}
			imgRobResult->setPosition(Point(0.0f, 65.0f) + imgTitle->getPosition());
			m_pRoot->addChild(imgRobResult, 5);
		}

		// item;
		if (m_settlementInfo.mapReward.size() > 0)
		{
			int nIndex = 0;
			float fStartX = m_pRoot->getContentSize().width - 65.0f;
			float fStartY = 65.0f;
			for (auto it = m_settlementInfo.mapReward.begin(); it != m_settlementInfo.mapReward.end(); it++, nIndex++)
			{
				UIToolHeader* item = UIToolHeader::create(it->first);
				item->setToolId(it->first);
				item->setNumEx(it->second);
				item->setPosition(Vec2(fStartX - nIndex * 115.0f, fStartY));
				item->setVisible(false);
				m_pRoot->addChild(item, 3);
				m_vcItem.push_back(item);
			}

			// 获得物品动画;
			reverse(m_vcItem.begin(), m_vcItem.end());
			runItemAnimation(this, 0);
		}
		else
		{
			// 无道具;
			ImageView* imgNone = ImageView::create("Image/UIChapter/Settlement/img_wudaojujiangli.png");
			imgNone->setPosition(Point(-33.0f, -60.0f) + imgTitle->getPosition());
			m_pRoot->addChild(imgNone, 5);
		}
		// 添加触摸响应;
		showTouchContinue(m_settlementInfo.bWin);
	}
}

void LayerSettlement::runItemAnimation( Node* pSender, int nIndex )
{
	if (m_vcItem.empty())
		return;

	if (nIndex >= m_vcItem.size())
		return;

	m_vcItem.at(nIndex)->setLocalZOrder(nIndex+1);
	m_vcItem.at(nIndex)->setScale(1.5f);
	m_vcItem.at(nIndex)->setVisible(true);

	// 获得物品飞入;
	Sequence* action = Sequence::create(EaseSineIn::create(ScaleTo::create(0.2f, 1.0f)),
		CallFuncN::create(CC_CALLBACK_1(LayerSettlement::runItemAnimation, this, nIndex+1)), nullptr);
	m_vcItem.at(nIndex)->runAction(action);

	// 闪闪的，很好看;
	shiningStar(m_vcItem.at(nIndex));
}

void LayerSettlement::shiningStar(UIToolHeader* item)
{
	// 1.蓝色以上品质;
	RowData* row = DataManager::getInstance()->searchToolById(item->getToolId());
	if (nullptr == row || row->getIntData("itemQua") < TOOL_ITEM_QUA_BLUE)
		return;

	// 2.icon区域内随机3个;
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniDaojujiangli/AniDaojujiangli.ExportJson");
	for (int i = 0; i < 3; ++i)
	{
		// 创建动画;
		Armature* _armature = Armature::create("AniDaojujiangli");
		_armature->setPosition(Vec2(0.0f, 0.0f));
		item->addChild(_armature, 99);

		// 延时播放;
		srand(i);
		float delay = (i == 0) ? 0.0f : ((float)(rand()%10))/10.0f;
		CCLOG("++++++++++++ delay = %lf", delay);
		const string key = "LayerSettlement::runItemAnimation" + _TO_STR(i);
		Director::getInstance()->getScheduler()->schedule([=](float delta)->void{
			Director::getInstance()->getScheduler()->unschedule(key, this);
			_armature->getAnimation()->play("Animation1", -1, 1);
			_armature->getAnimation()->setMovementEventCallFunc([=](Armature *armature, MovementEventType movementType, const std::string& movementID) {
				// 每次随机位置;
				float f_x = float(rand()%((int)(item->getContentSize().width)));
				float f_y = float(rand()%((int)(item->getContentSize().height)));
				armature->setPosition(Vec2(f_x, f_y));
			});
		}, this, 1.0f, 0, delay, false, key);
	}
}

void LayerSettlement::enableTouch( Node* node, Btn_Tag nTag )
{
	// 设置标记;
	node->setTag(nTag);

	// 处理触摸事件;
	auto _listener = EventListenerTouchOneByOne::create();
	_listener->setSwallowTouches(true);
	_listener->onTouchBegan = [=](Touch* touch, Event* event)
	{
		SoundUtils::playSoundEffect("dianji");

		Bone* _bone = dynamic_cast<Bone*>(event->getCurrentTarget());
		if (nullptr != _bone)
		{
			Skin* _skin = dynamic_cast<Skin*>(_bone->getDisplayRenderNode());
			Point pt = _skin->convertToNodeSpace(touch->getLocation());
			Size  size = _skin->getContentSize();
			Rect  rect(0.0f, 0.0f, size.width, size.height);
			if (rect.containsPoint(pt))
			{
				string strName = _skin->getDisplayName();
				//log("=========== curTouchIn %d, %s ===========", _bone->getTag(), strName.c_str());
				size_t pos = strName.rfind("1");
				if (pos != string::npos)
				{
					strName.replace(pos, 1, "2");
					size_t pos2 = strName.find_last_of("\/");
					if (pos2 != string::npos)
					{
						strName = strName.substr(pos2+1);
					}
					string strNewSkinPath("Image/UIChapter/Settlement/");
					Skin* _newSkin = Skin::create(strNewSkinPath.append(strName));
					_bone->addDisplay(_newSkin, 0);
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	};
	_listener->onTouchMoved = [=](Touch* touch, Event* event)
	{

	};
	_listener->onTouchCancelled = [=](Touch* touch, Event* event)
	{
		Bone* _bone = dynamic_cast<Bone*>(event->getCurrentTarget());
		Skin* _skin = dynamic_cast<Skin*>(_bone->getDisplayRenderNode());
		string strName = _skin->getDisplayName();
		size_t pos = strName.rfind("2");
		if (pos != string::npos)
		{
			strName.replace(pos, 1, "1");
			size_t pos2 = strName.find_last_of("\/");
			if (pos2 != string::npos)
			{
				strName = strName.substr(pos2+1);
			}
			string strNewSkinPath("Image/UIChapter/Settlement/");
			Skin* _newSkin = Skin::create(strNewSkinPath.append(strName));
			_bone->addDisplay(_newSkin, 0);
		}
	};
	_listener->onTouchEnded = [=](Touch* touch, Event* event)
	{
		Bone* _bone = dynamic_cast<Bone*>(event->getCurrentTarget());
		Skin* _skin = dynamic_cast<Skin*>(_bone->getDisplayRenderNode());
		string strName = _skin->getDisplayName();
		size_t pos = strName.rfind("2");
		if (pos != string::npos)
		{
			strName.replace(pos, 1, "1");
			size_t pos2 = strName.find_last_of("\/");
			if (pos2 != string::npos)
			{
				strName = strName.substr(pos2+1);
			}
			string strNewSkinPath("Image/UIChapter/Settlement/");
			Skin* _newSkin = Skin::create(strNewSkinPath.append(strName));
			_bone->addDisplay(_newSkin, 0);
		}

		// 处理响应;
		onBtnCLicked((Btn_Tag)(_bone->getTag()));
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_listener, node);
}

void LayerSettlement::onBtnCLicked( Btn_Tag nTag )
{
	switch (nTag)
	{
	case Btn_Tag_Pos_1:			// 伙伴招募;
		{
			// 跳转商城;
			Director::getInstance()->popToRootScene();
			Director::getInstance()->replaceScene(SceneTakeCard::create());
		}
		break;

	case Btn_Tag_Pos_2:			// 伙伴养成;
	case Btn_Tag_Pos_3:			// 装备养成;
		{
			// 跳转英雄详情;
			Director::getInstance()->popToRootScene();
			Director::getInstance()->replaceScene(SceneAllHero::create());
		}
		break;

	case Btn_Tag_Pos_4:			// 阵型强化;
		{
			// 跳转阵型主界面;
			Director::getInstance()->popToRootScene();
			Director::getInstance()->replaceScene(SceneFormation::create());
		}
		break;

	default:
		break;
	}
}

void LayerSettlement::showTouchContinue(bool bWin)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	ImageView* _img = ImageView::create("Image/Icon/global/img_dianjipingmu.png");
	m_pRoot->addChild(_img, 3);
	float xxx = bWin ? 1 : 3;
	_img->setPosition(Vec2((visibleSize.width/4) * xxx, 100.0f));

	ActionInterval* easeAct1 =  EaseSineInOut::create(FadeOut::create(1.0f));
	ActionInterval* easeAct2 =  EaseSineInOut::create(FadeIn::create(0.6f));
	ActionInterval* seq = Sequence::create(easeAct1, easeAct2, nullptr);
	_img->runAction(RepeatForever::create(seq));

	// 添加触摸响应;
	m_pRoot->addTouchEventListener(CC_CALLBACK_2(LayerSettlement::onLayerTouched, this, Pnl_Root));

}

void LayerSettlement::onLevelUpDone()
{
	// 跳转;
	assert(m_pLogicParent != nullptr);
	m_pLogicParent->jumpScene();
}

void LayerSettlement::showGlobalExp()
{
	if (m_settlementInfo.nExp <= 0)
		return;

	// 获取剩余英雄位置;
	SceneFight* sceneFight = dynamic_cast<SceneFight*>(this->getParent());
	if (sceneFight)
	{
		LayerFighter* _layerFighter = sceneFight->getLayerFighter();
		assert(nullptr != _layerFighter);

		vector<pair<int, Point> > vcIndex;
		_layerFighter->getAllAliveFighterIndex(FightStateMachine::MY_SIDE, vcIndex);

		// 播放+EXP动画;
		Vec2 shift = Director::getInstance()->getVisibleOrigin();
		for (auto _pair : vcIndex)
		{
			float duration = 0.3f;
			string strExp = "EXP+" + _TO_STR(m_settlementInfo.nExp);
			Text* txtExp = Text::create(strExp.c_str(), FONT_FZZHENGHEI, 22);
			txtExp->setPosition(_pair.second + Point(-shift.x, 170.0f));
			txtExp->setColor(Color3B(0x88, 0xFF, 0x14));
			setTextAddStroke(txtExp, Color3B(0x09, 0x22, 0x7F), 3);
			m_pRoot->addChild(txtExp, 3);
			Sequence* seq = Sequence::create(
				MoveBy::create(duration*3.0f, Vec2(0.0f, 36.0f)),
				Spawn::create(MoveBy::create(duration, Vec2(0.0f, 12.0f)), FadeOut::create(duration), nullptr),
				CallFuncN::create([=](Node* pSender){
					txtExp->removeFromParent();
			}),
				CallFuncN::create([=](Node* pSender){
					showGlobalLevelUp(_pair);
			})
				,nullptr);
			txtExp->runAction(seq);
		}

	}
}

void LayerSettlement::showGlobalLevelUp( pair<int, Point> heroPos )
{
	// 取得英雄数据;
	Fighter* fighter = FightModel::getInstance()->getAliveFighter(FightStateMachine::MY_SIDE, heroPos.first);
	if (nullptr == fighter)
		return;
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroById(fighter->getHeroID());

	if (!heroParam)
	{
		return;
	}

	// 排除1级的英雄，不存在从0级升到1级;
	if (nullptr != heroParam && heroParam->mLevel <= 1)
		return;

	// 此处的英雄经验是已经加过的，所以用差值来判定是否升级;
	if (heroParam->mLevel <= HeroModel::getInstance()->getMainHeroLevel()
		&& heroParam->mExp < m_settlementInfo.nExp)
	{
		Vec2 shift = Director::getInstance()->getVisibleOrigin();
		float duration = 0.3f;
		ImageView* imgLvUp = ImageView::create("Image/Icon/global/img_shengji2.png");
		imgLvUp->setPosition(heroPos.second + Point(-shift.x, 170.0f));
		m_pRoot->addChild(imgLvUp, 3);
		Sequence* seq = Sequence::create(
			MoveBy::create(duration*3.0f, Vec2(0.0f, 36.0f)),
			Spawn::create(MoveBy::create(duration, Vec2(0.0f, 12.0f)), FadeOut::create(duration), nullptr),
			CallFuncN::create([=](Node* pSender){
				imgLvUp->removeFromParent();
		})
			,nullptr);
		imgLvUp->runAction(seq);
	}
}
