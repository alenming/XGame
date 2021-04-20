#include "LoveTreasureLayer.h"
#include "Widget/LayerGameRules.h"

LoveTreasureLayer::LoveTreasureLayer(void)
{
	m_Iscomplete = false;
	m_canBack = true;
}


LoveTreasureLayer::~LoveTreasureLayer(void)
{
}

LoveTreasureLayer* LoveTreasureLayer::create(int idx,int petId)
{
	//背景设置
	LoveTreasureLayer* ret = new LoveTreasureLayer;
	if (ret && ret->init())
	{
		ret->autorelease();
		ret->initUi(idx,petId);

		return ret;
	}


	CC_SAFE_DELETE(ret);
	return nullptr;
}

void LoveTreasureLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
	case LOVE_EVENT_TYPE_PLAY_MAP_EVENT:
		{
			//步数改变
			if (LoveModel::getInstance()->getPlayStepChange()->size() > 0)
			{
				setAddStepChange();
			}
			//宝箱
			else if (LoveModel::getInstance()->getBoxItem()->size() > 0)
			{
				showItemReward();
			}
			//战斗
			else if (LoveModel::getInstance()->getPlayFightId() > 0)
			{
				Director::getInstance()->pushScene(SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_FIND,
					LoveModel::getInstance()->getPlayFightId()));
				FightController::getInstance()->sendCommonFightMsg(LoveModel::getInstance()->getPlayFightId());

				if (LoveModel::getInstance()->getplayMapStepCount() <= 0)
				{
					playResult();

					PLAY_MAP_NODE_INFO_VEC* LatticeInfo = LoveModel::getInstance()->getPlayMapNode();
					for(int i = 0; i < LatticeInfo->size(); i++)
					{
						setLatticeItemByData(i,true);
					}
				}
			}

			// 找到宝藏和没步数都是结束标志
			else if (LoveModel::getInstance()->getPlayRewardItem()->size() > 0 ||LoveModel::getInstance()->getplayMapStepCount() <= 0)
			{
				playResult();
			}

			moveStep(LoveModel::getInstance()->getCurCHooseXy());
		}
		break;
	default:
		break;
	}
}

bool LoveTreasureLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	return true;
}

void LoveTreasureLayer::onEnter()
{
	Layer::onEnter();
	LoveModel::getInstance()->addObserver(this);
}

void LoveTreasureLayer::onExit()
{
	LoveModel::getInstance()->removeObserver(this);

	Layer::onExit();
}

void LoveTreasureLayer::initUi(int idx,int petId)
{
	m_mapIdx = idx;
	m_heroPetId = petId;

	PLAY_MAP playMap = LoveModel::getInstance()->getPlayMap()->at(m_mapIdx);
	RowData *lovePlay = DataManager::getInstance()->searchLoveTreasureByid(playMap.id);

	LayerFightBack* _LayerFightBack = LayerFightBack::create(lovePlay->getIntData("resID"));
	this->addChild(_LayerFightBack,0);

	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIqingyuan/UIMap.ExportJson"));
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);

	// 将UILayer层加入到当前的场景
	this->addChild(m_ui.pRoot,1);

	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED && m_canBack)
		{
			if (m_Iscomplete)
			{
				m_canBack = false;
				Director::getInstance()->popScene();
				LoveController::playMap(m_heroPetId);
				LoveModel::getInstance()->getPlayMapBack() = true;
			}
			else
			{
				auto call = [this](Ref* ref, CustomRetType type)->void
				{
					if (type == RET_TYPE_OK)
					{
						m_canBack = false;
						Director::getInstance()->popScene();
						LoveController::playMap(m_heroPetId);
						LoveModel::getInstance()->getPlayMapBack() = true;
					}
				};
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20313),call,CUSTOM_YES_NO);
			}
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_LOVE);
	_layerCommHeadInfo->setPosition(Vec2(-pos.x,pos.y));
	m_ui.pRoot->addChild(_layerCommHeadInfo);

	m_ui.stepCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Step_Count"));
	m_ui.boxCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Box_Count"));
	m_ui.stepCount->setFontName(FONT_FZZHENGHEI);
	m_ui.boxCount->setFontName(FONT_FZZHENGHEI);

	Button *desc = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Btn_Help"));
	auto call = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			LayerGameRules* loveRule = LayerGameRules::create(LOVE_RULE_ID);
			this->addChild(loveRule, 1);

			ActionCreator::runCommDlgAction(loveRule);
		}
	};
	desc->addTouchEventListener(call);	


	for (int i = 0; i < LatticeCount; i++)
	{
		m_ui.Lattice[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_gezi" + TO_STR(i+1)));
		Button *btn = Button::create("Image/Icon/global/img_gezi2.png");
		btn->setPosition(Vec2(m_ui.Lattice[i]->getContentSize().width/2,m_ui.Lattice[i]->getContentSize().height/2));
		m_ui.Lattice[i]->addChild(btn,1,"LatticeItemInfo");
	}

	CalculationAdjacent();
	setLatticeInfo();
	setStepCount();

	m_ui.ImgDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_di"));
	m_ui.ImgDiPos = m_ui.ImgDi->getPosition();
	
	m_ui.PnlTask = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Task"));
	m_ui.PnlTaskPos = m_ui.PnlTask->getPosition();

	m_ui.PnlTaskResult = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Panel_result"));
	m_ui.PnlTaskResult->setVisible(false);

	m_ui.PnlDuiBai = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"duibai"));
	m_ui.PnlDuiBai->setVisible(false);
	m_ui.heroImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlDuiBai,"hero_img"));
	m_ui.heroImgPos = m_ui.heroImg->getPosition();
	m_ui.duibai = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlDuiBai,"Img_duibai"));
	m_ui.duibaiPos = m_ui.duibai->getPosition();
	m_ui.content = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlDuiBai,"Lab_duibai"));
	m_ui.content->setFontName(FONT_FZZHENGHEI);

	auto ResultCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if(type == Widget::TouchEventType::ENDED)
		{
			m_ui.PnlTaskResult->setVisible(false);
			setLatticeInfo(true);
			showplayReward();
		}
	};
	m_ui.PnlTaskResult->addTouchEventListener(ResultCall);

	m_ui.TaskResult = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.PnlTaskResult,"TaskResult"));

	m_ui.ImgDi->setPosition(Vec2(size.width + pos.x + m_ui.ImgDi->getContentSize().width/2,m_ui.ImgDiPos.y));
	m_ui.ImgDi->runAction(CCSequence::create(CCMoveTo::create(MapMoveTime,m_ui.ImgDiPos),CCCallFunc::create([this]()->void
	{
		setDuiBai("mapAnswer");
	}
	),nullptr));

	m_ui.PnlTask->setPosition(Vec2(-pos.x-m_ui.PnlTask->getContentSize().width,m_ui.PnlTaskPos.y));
	m_ui.PnlTask->runAction(CCMoveTo::create(MapMoveTime,m_ui.PnlTaskPos));

	addInfoPanel(m_ui.PnlTask,PLAY_MAP_RESULT_NONE);
}

void LoveTreasureLayer::addInfoPanel(Widget *node,PLAY_MAP_RESULT_TYPE type)
{
	string strFileName;
	strFileName.assign("Image/UIqingyuan/UITask2.ExportJson");
	Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	
	if (node->getChildByName("UITask2"))
	{
		node->removeChildByName("UITask2");
	}
	node->addChild(layer);

	PLAY_MAP playMap = LoveModel::getInstance()->getPlayMap()->at(m_mapIdx);
	RowData *lovePlay = DataManager::getInstance()->searchLoveTreasureByid(playMap.id);
	RowData *toolDate = DataManager::getInstance()->searchToolById(lovePlay->getIntData("treasureId"));

	ImageView* Img_tanbaoling = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_tanbaoling"));
	ActionCreator::runCommDlgAction(Img_tanbaoling);

	ImageView *result = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Img_tanbaoling,"result"));
	result->setVisible(false);

	Text *name = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_tanbaoling,"Lab_Name"));
	name->setFontName(FONT_FZZHENGHEI);
	name->setString(toolDate->getStringData("itemName"));
	setTextAddStroke(name,Color3B(0x00,0x00,0x00),2);

	ImageView *head = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Img_tanbaoling,"Img_Item"));
	head->loadTexture(ResourceUtils::getSmallIconPath(toolDate->getIntData("resId")));
	for (int i = 0; i < _DifficultCount; i++)
	{
		ImageView *Img_Difficult = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Img_tanbaoling,"Img_Difficult_" + TO_STR(i)));
		Img_Difficult->setVisible(false);

		if (i < lovePlay->getIntData("difficult"))
		{
			Img_Difficult->setVisible(true);
		}
	}

	vector<string> rewardItem;
	StringFormat::parseCsvStringVec(lovePlay->getStringData("rewardItem"), rewardItem);
	for (int i = 0; i < 3; i++)
	{
		Layout *PnlReward = dynamic_cast<Layout*>(Helper::seekWidgetByName(Img_tanbaoling,"Pal_Reward_" + TO_STR(i)));

		if (rewardItem.size() > i)
		{
			PnlReward->setVisible(true);
			vector<int> reward;
			StringFormat::parseCsvStringVecByDu(rewardItem[i], reward);

			UIToolHeader *icon = UIToolHeader::create(reward[0]);
			icon->setNumEx(reward[1]);
			icon->setAnchorPoint(Vec2(0,0));
			icon->setScale(0.8f);
			icon->setToolId(reward[0]);
			icon->setTipsEnabled(true);
			PnlReward->addChild(icon);
		}
		else
		{
			PnlReward->setVisible(false);
		}
	}

	//开始玩
	Button *BtnGo = dynamic_cast<Button*>(Helper::seekWidgetByName(Img_tanbaoling,"Btn_Go"));
	BtnGo->setVisible(false);

	switch (type)
	{
	case PLAY_MAP_RESULT_NONE:
		{
			
		}
		break;
	case PLAY_MAP_RESULT_SUCCESS:
		{
			m_Iscomplete = true;
			result->setVisible(true);
			result->loadTexture("Image/Icon/global/img_tanbao_chenggong.png");

			result->setScale(2.0f);
			result->runAction(CCSequence::create(CCDelayTime::create(0.5f),CCScaleTo::create(0.2f,1.0f),
				CCCallFunc::create([this]()->void
			{
				setDuiBai("findAnswer");
			}),nullptr));
		}
		break;
	case PLAY_MAP_RESULT_FAIL:
		{
			m_Iscomplete = true;
			result->setVisible(true);
			result->loadTexture("Image/Icon/global/img_tanbao_shibai.png");

			result->setScale(2.0f);
			result->runAction(CCSequence::create(CCDelayTime::create(0.5f),CCScaleTo::create(0.2f,1.0f),
				CCCallFunc::create([this]()->void
			{
				setDuiBai("failAnswer");
			}),nullptr));
		}
		break;
	default:
		break;
	}
}

void LoveTreasureLayer::CalculationAdjacent()
{
	PLAY_MAP_NODE_INFO_VEC* LatticeInfo = LoveModel::getInstance()->getPlayMapNode();
	int initIdx = 0;
	for(int i = 0; i < LatticeInfo->size(); i++)
	{
		if (LatticeInfo->at(i)._event == PLAY_MAP_EVENT_INIT)
		{
			initIdx = i;
			break;
		}
	}

	int row = initIdx / MapColCount;
	int col = initIdx % MapColCount;

	//找上相邻
	if (initIdx - MapColCount >= 0 && ((initIdx - MapColCount) / MapColCount == row || (initIdx - MapColCount) % MapColCount == col))
	{
		LatticeInfo->at(initIdx - MapColCount).flg = 1;
	}

	//找下相邻
	if (initIdx + MapColCount < LatticeCount && ((initIdx + MapColCount) / MapColCount == row || (initIdx + MapColCount) % MapColCount == col))
	{
		LatticeInfo->at(initIdx + MapColCount).flg = 1;
	}

	//找左相邻
	if (initIdx - 1 >= 0 && ((initIdx - 1) / MapColCount == row || (initIdx - 1) % MapColCount == col))
	{
		LatticeInfo->at(initIdx - 1).flg = 1;
	}

	//找右相邻
	if (initIdx + 1 < LatticeCount && ((initIdx + 1) / MapColCount == row || (initIdx + 1) % MapColCount == col))
	{
		LatticeInfo->at(initIdx + 1).flg = 1;
	}
}

void LoveTreasureLayer::setLatticeInfo(bool view)
{
	PLAY_MAP_NODE_INFO_VEC* LatticeInfo = LoveModel::getInstance()->getPlayMapNode();
	
	for(int i = 0; i < LatticeInfo->size(); i++)
	{
		if (LatticeInfo->at(i)._event == PLAY_MAP_EVENT_INIT)
		{
			m_initIdx = i;
		}
		setLatticeItemByData(i,view);
	}
}

void LoveTreasureLayer::setLatticeItemByData(int idx,bool view )
{
	PLAY_MAP_NODE_INFO_VEC* LatticeInfo = LoveModel::getInstance()->getPlayMapNode();
	PLAY_MAP_NODE_INFO LatticeItemInfo = LatticeInfo->at(idx);
	if (LatticeItemInfo.flg == 0 && !view)
	{
		m_ui.Lattice[idx]->loadTexture("Image/Icon/global/img_gezi1.png");
		Button *btn = dynamic_cast<Button*>(m_ui.Lattice[idx]->getChildByName("LatticeItemInfo"));
		if (btn)
		{
			btn->setVisible(false);
		}
	}
	else
	{
		m_ui.Lattice[idx]->loadTexture("Image/Icon/global/img_gezi2.png");

		Button *btn = dynamic_cast<Button*>(m_ui.Lattice[idx]->getChildByName("LatticeItemInfo"));
		if (btn)
		{
			btn->setVisible(true);
		}

		switch (LatticeItemInfo._event)
		{
		case PLAY_MAP_EVENT_INIT://0-初始格
			{
				if (MainModel::getInstance()->getMainParam()->mSex == HERO_SEX_MALE)
				{
					btn->loadTextureNormal("Image/Icon/global/img_gezi_nanzhujue.png");
				}
				else
				{
					btn->loadTextureNormal("Image/Icon/global/img_gezi_nvnzhujue.png");
				}
				
				btn->setScale(0.9f);
				btn->setTouchEnabled(false);
			}
			break;
		case PLAY_MAP_EVENT_FIGHT://1-战斗
			{
				btn->loadTextureNormal("Image/Icon/global/img_gezi_zhandou.png");
				btn->setTouchEnabled(true);
			}
			break;
		case PLAY_MAP_EVENT_BOX: //2-宝箱
			{
				btn->loadTextureNormal("Image/Icon/global/btn_baoxiang_1.png");
				btn->setScale(0.2f);
				btn->setTouchEnabled(true);
			}
			break;
		case PLAY_MAP_EVENT_RECOVERY: //3-体力恢复
			{
				btn->loadTextureNormal("Image/Icon/global/img_bushujia2.png");
				btn->setTouchEnabled(true);
			}
			break;
		case PLAY_MAP_EVENT_TRAP://4-陷阱
			{
				btn->loadTextureNormal("Image/Icon/global/img_gezi_xianjing.png");
				btn->setTouchEnabled(true);
			}
			break;
		case PLAY_MAP_EVENT_STONE://5-石头
			{
				btn->loadTextureNormal("Image/Icon/global/img_gezi_shidui.png");
				btn->setTouchEnabled(true);
			}
			break;
		case PLAY_MAP_EVENT_EMPTY://6-空格子
			{
				btn->loadTextureNormal("Image/Icon/global/img_gezi2.png");
				btn->setTouchEnabled(true);
				btn->setScale(1.0f);
			}
			break;
		case PLAY_MAP_EVENT_TREASURE: //7-宝物
			{
				PLAY_MAP playMap = LoveModel::getInstance()->getPlayMap()->at(m_mapIdx);
				RowData *lovePlay = DataManager::getInstance()->searchLoveTreasureByid(playMap.id);
				RowData *toolDate = DataManager::getInstance()->searchToolById(lovePlay->getIntData("treasureId"));
				btn->loadTextureNormal(ResourceUtils::getSmallIconPath(toolDate->getIntData("resId")));
				btn->setScale(0.75f);
				btn->setTouchEnabled(true);
			}
			break;
		default:
			break;
		}

		if (btn)
		{
			auto call = [this,idx,LatticeItemInfo](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					if (m_Iscomplete)
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20314));
					}
					else if (LoveModel::getInstance()->getplayMapStepCount() <= 0)
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20315));
					}
					else
					{
						//找上相邻
						if ((idx / MapColCount == m_initIdx / MapColCount && abs(idx - m_initIdx) == 1) ||
							(idx % MapColCount == m_initIdx % MapColCount && abs(idx - m_initIdx) == MapColCount))
						{
							if (LatticeItemInfo._event == PLAY_MAP_EVENT_STONE)
							{
								CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20316));
							}
							else
							{
								PLAY_MAP playMap = LoveModel::getInstance()->getPlayMap()->at(m_mapIdx);

								LoveController::mapEvent(m_heroPetId,playMap.id,m_initIdx,idx,LatticeItemInfo._event);
							}
						}
						else
						{
							CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20317));
						}
					}
				}
			};

			btn->addTouchEventListener(call);
		}
	}
}

void LoveTreasureLayer::setStepCount()
{
	m_ui.stepCount->setString("x"+TO_STR(max(LoveModel::getInstance()->getplayMapStepCount(),0)));
	PLAY_MAP_NODE_INFO_VEC* LatticeInfo = LoveModel::getInstance()->getPlayMapNode();
	int addCount = 0;
	int boxCount = 0;
	for (int i = 0; i < LatticeInfo->size(); i++)
	{
		if (LatticeInfo->at(i)._event == PLAY_MAP_EVENT_RECOVERY)
		{
			addCount++;
		}
		else if (LatticeInfo->at(i)._event == PLAY_MAP_EVENT_BOX)
		{
			boxCount++;
		}
	}
	m_ui.boxCount->setString("x"+TO_STR(boxCount));
	setTextAddStroke(m_ui.stepCount,Color3B(0x8e,0x00,0x03),2);
	setTextAddStroke(m_ui.boxCount,Color3B(0x8e,0x00,0x03),2);
}

void LoveTreasureLayer::moveStep(int idx)
{
	PLAY_MAP_NODE_INFO_VEC* LatticeInfo = LoveModel::getInstance()->getPlayMapNode();

	//当前起始位置清空
	LatticeInfo->at(m_initIdx)._event = PLAY_MAP_EVENT_EMPTY;

	//点击区域成新的起始位置
	LatticeInfo->at(idx)._event = PLAY_MAP_EVENT_INIT;

	//重新估算各自
	CalculationAdjacent();
	setLatticeInfo();
	setStepCount();
}

void LoveTreasureLayer::setAddlove()
{
	Size size = Director::getInstance()->getVisibleSize();
	string strFileName;
	strFileName.assign("Image/UIqingyuan/UIqingyuanAdd.ExportJson");
	Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

	ImageView *Img_addlove = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_addlove_0"));
	Text *Lab_value = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_addlove,"Lab_value"));
	vector<int>* addWillVec =  LoveModel::getInstance()->getGoodWillAddVec();

	Lab_value->setString("+" + TO_STR(addWillVec->at(0)));
	Lab_value->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Lab_value,Color3B(0x10,0x3b,0x19),2);
	addWillVec->erase(addWillVec->begin());
	this->addChild(layer,2);

	auto call = [this,layer]()->void
	{
		setUpGradePrize();
		layer->removeFromParent();
	};

	Img_addlove->runAction(CCSequence::create(MoveBy::create(0.5f,Vec2(0,150.0f)),CCFadeOut::create(0.2f),CCCallFunc::create(call),nullptr));
}

void LoveTreasureLayer::setUpGradePrize()
{
	LOVE_HERO_INFO* heroInfo = getHeroByHeroId(m_heroPetId);
	SEND_GIFT_REWARD_VEC* loveGift = LoveModel::getInstance()->getLoveGiftReeardVec();

	//升级数据改变
	if (heroInfo->curLevel > heroInfo->lastLevel)
	{
		auto call = [this]()->void
		{
			
		};
		//升级会有友情点奖励
		RowData *friend_ship = DataManager::getInstance()->searchToolSystemById(FIREND_SHIP_ID);
		int friend_ship_count = 0;
		for (int i = heroInfo->lastLevel; i < heroInfo->curLevel; i++)
		{
			RowData *loveLevelData = DataManager::getInstance()->searchLoveLevelById(i);
			if (loveLevelData && loveLevelData->getIntData("friendShip") > 0)
			{
				friend_ship_count = friend_ship_count + loveLevelData->getIntData("friendShip");
			}
		}

		if (friend_ship)
		{
			SEND_GIFT_REWARD reward;
			reward.templateId = friend_ship->getIntData("value");
			reward.amount = friend_ship_count;
			loveGift->push_back(reward);
		}

		LoveGoodWillPrize *Prize = LoveGoodWillPrize::create(*heroInfo,loveGift,call);
		this->addChild(Prize,4);
	}
}

void LoveTreasureLayer::setAddStepChange()
{
	Size size = Director::getInstance()->getVisibleSize();
	string strFileName;
	strFileName.assign("Image/UIqingyuan/UIqingyuanAdd.ExportJson");
	Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

	ImageView *Img_addlove = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_addlove_0"));
	Text *Lab_value = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_addlove,"Lab_value"));
	ImageView *Image_146 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Img_addlove,"Image_146"));
	Image_146->loadTexture("Image/Icon/global/img_bushu.png");
	vector<int>* stepChang =  LoveModel::getInstance()->getPlayStepChange();

	if (stepChang->at(0) > 0)
	{
		Lab_value->setString("+" + TO_STR(stepChang->at(0)));
	}
	else
	{
		Lab_value->setString(TO_STR(stepChang->at(0)));
	}
	Lab_value->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Lab_value,Color3B(0x10,0x3b,0x19),2);
	stepChang->erase(stepChang->begin());
	this->addChild(layer,3);

	auto call = [this,layer]()->void
	{
		playResult();
		layer->removeFromParent();
	};

	Img_addlove->runAction(CCSequence::create(MoveBy::create(0.5f,Vec2(0,150.0f)),CCFadeOut::create(0.2f),CCCallFunc::create(call),nullptr));
}

void LoveTreasureLayer::showItemReward()
{
	SEND_GIFT_REWARD_VEC* itemBox = LoveModel::getInstance()->getBoxItem(); //游玩宝箱
	if (itemBox->size() <= 0)
	{
		return;
	}
	vector<PopRewardItem*> vecRewardItem;

	//道具
	for (size_t i = 0; i < itemBox->size(); i++)
	{
		SEND_GIFT_REWARD& RewardInfo = itemBox->at(i);

		//金币
		if (RewardInfo.templateId == PLAY_GOLD_ITEM_ID)
		{
			PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(RewardInfo.amount);
			vecRewardItem.push_back(rewardItem);
		}
		//银币
		else if(RewardInfo.templateId == PLAY_COIN_ITEM_ID)
		{
			PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(RewardInfo.amount);
			vecRewardItem.push_back(rewardItem);
		}
		else
		{
			PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(RewardInfo.templateId, RewardInfo.amount);
			vecRewardItem.push_back(rewardItem);
		}
	}

	auto call = [this](Ref* ref, PopRewardEventType type)->void
	{
		if (type == EVENT_TYPE_END)
		{
			playResult();
		}
	};
	PopReward::show(vecRewardItem,call);

}

void LoveTreasureLayer::showplayReward()
{
	SEND_GIFT_REWARD_VEC* playReward = LoveModel::getInstance()->getPlayRewardItem(); //游玩宝箱
	if (playReward->size() <= 0)
	{
		return;
	}
	vector<PopRewardItem*> vecRewardItem;

	//道具
	for (size_t i = 0; i < playReward->size(); i++)
	{
		SEND_GIFT_REWARD& RewardInfo = playReward->at(i);

		//金币
		if (RewardInfo.templateId == PLAY_GOLD_ITEM_ID)
		{
			PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(RewardInfo.amount);
			vecRewardItem.push_back(rewardItem);
		}
		//银币
		else if(RewardInfo.templateId == PLAY_COIN_ITEM_ID)
		{
			PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(RewardInfo.amount);
			vecRewardItem.push_back(rewardItem);
		}
		else
		{
			PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(RewardInfo.templateId, RewardInfo.amount);
			vecRewardItem.push_back(rewardItem);
		}
	}

	auto call = [this](Ref* ref, PopRewardEventType type)->void
	{
		if (type == EVENT_TYPE_END)
		{
			setAddlove();
		}
	};
	PopReward::show(vecRewardItem,call);

}

void LoveTreasureLayer::playResult()
{
	if (LoveModel::getInstance()->getplayMapStepCount() <= 0)
	{
		if (LoveModel::getInstance()->getPlayRewardItem()->size() > 0)
		{
			m_ui.PnlTaskResult->setVisible(true);
			
			ActionCreator::runCommContinueAction(m_ui.PnlTaskResult->getChildByName("Img_back"));
			addInfoPanel(m_ui.TaskResult,PLAY_MAP_RESULT_SUCCESS);
		}
		else
		{
			m_ui.PnlTaskResult->setVisible(true);
			ActionCreator::runCommContinueAction(m_ui.PnlTaskResult->getChildByName("Img_back"));

			addInfoPanel(m_ui.TaskResult,PLAY_MAP_RESULT_FAIL);
		}
	}
	else if (LoveModel::getInstance()->getPlayRewardItem()->size() > 0)
	{
		m_ui.PnlTaskResult->setVisible(true);

		ActionCreator::runCommContinueAction(m_ui.PnlTaskResult->getChildByName("Img_back"));
		addInfoPanel(m_ui.TaskResult,PLAY_MAP_RESULT_SUCCESS);
	}
}

LOVE_HERO_INFO* LoveTreasureLayer::getHeroByHeroId(int petId)
{
	LOVE_HERO_INFO_VEC* heroVec = LoveModel::getInstance()->getLoveHeroInfoVec();

	//如果是送礼后 有可能排序了 数据会改变
	for (int i = 0; i < heroVec->size(); i++)
	{
		if (heroVec->at(i).petId == petId)
		{
			return &heroVec->at(i);
		}
	}

	return nullptr;
}

void LoveTreasureLayer::setDuiBai(string str)
{
	m_ui.PnlDuiBai->setVisible(true);
	LOVE_HERO_INFO *heroInfo = getHeroByHeroId(m_heroPetId);
	RowData *heroData = DataManager::getInstance()->searchToolById(heroInfo->petId);

	ImageView *heroLv = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.PnlDuiBai,"hero_love_lv"));
	Text *heroName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.PnlDuiBai,"hero_name"));
	heroLv->loadTexture("Image/Icon/global/img_haogan2_" + TO_STR(heroInfo->expType) + ".png");
	heroName->setString(heroData->getStringData("itemName"));
	heroName->setFontName(FONT_FZZHENGHEI);

	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if(type == Widget::TouchEventType::ENDED) 
		{
			m_ui.heroImg->runAction(CCMoveTo::create(exitDu_time,Vec2(-m_ui.heroImg->getContentSize().width/2,
				m_ui.heroImgPos.y)));
			auto call = [this]()->void
			{
				m_ui.PnlDuiBai->setVisible(false);
			};
			m_ui.duibai->runAction(CCSequence::create(CCMoveTo::create(exitDu_time,Vec2(m_ui.duibaiPos.x,-m_ui.duibai->getContentSize().height)),
				CCCallFunc::create(call),nullptr));
		}
	};
	Button* Btn_next = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.PnlDuiBai,"Btn_next"));
	Btn_next->addTouchEventListener(backCall);
	Btn_next->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5f,Vec2(0,10)),
		CCMoveBy::create(0.5f,Vec2(0,-10)),nullptr)));
	m_ui.PnlDuiBai->addTouchEventListener(backCall);

	m_ui.heroImg->loadTexture(ResourceUtils::getHeroCompleteCardPath(heroData->getIntData("resId")));
	RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(heroInfo->petId);
	vector<float> pointVect;
	StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
	m_ui.heroImg->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));

	m_ui.heroImg->setOpacity(0);
	m_ui.heroImg->setPosition(m_ui.heroImgPos);
	m_ui.heroImg->runAction(CCFadeIn::create(enterDu_time));
	auto call = [this,str]()->void
	{
		//英雄随机说句话
		PLAY_MAP playMap = LoveModel::getInstance()->getPlayMap()->at(m_mapIdx);
		RowData *lovePlay = DataManager::getInstance()->searchLoveTreasureByid(playMap.id);
		vector<int> sayVec;
		StringFormat::parseCsvStringVec(lovePlay->getStringData(str), sayVec);
		srand((unsigned)time(NULL));
		int sayId = rand()%(sayVec.size());
		RowData *talkData = DataManager::getInstance()->searchLoveTalkById(sayVec[sayId]);

		animationTalkOut(talkData->getStringData("answer"));
	};
	m_ui.duibai->setPosition(Vec2(m_ui.duibaiPos.x,-m_ui.duibai->getContentSize().height));
	m_ui.duibai->runAction(CCSequence::create(CCMoveTo::create(enterDu_time,m_ui.duibaiPos),CCCallFunc::create(call),nullptr));
}

void LoveTreasureLayer::animationTalkOut(string strText)
{
	m_strTalkContent = strText;
	m_nTalkContentLength = 0;
	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(LoveTreasureLayer::callBackTalkOut), this);
	Director::getInstance()->getScheduler()->schedule(
		schedule_selector(LoveTreasureLayer::callBackTalkOut), this, 0.03, kRepeatForever, 0, false);
}

void LoveTreasureLayer::callBackTalkOut(float dt)
{
	if (m_nTalkContentLength > m_strTalkContent.length())
	{
		Director::getInstance()->getScheduler()->unschedule(
			schedule_selector(LoveTreasureLayer::callBackTalkOut), this);
		m_nTalkContentLength = 0;
	}
	else if (m_nTalkContentLength == m_strTalkContent.length())
	{
		m_nTalkContentLength ++;
		m_ui.content->setText(m_strTalkContent.substr(0, m_nTalkContentLength));
	}
	else
	{
		char cValue = m_strTalkContent.at(m_nTalkContentLength);
		if (cValue>0 && cValue<127 )
		{
			m_nTalkContentLength ++;
		}
		else//utf8中文占3个字符
		{
			m_nTalkContentLength += 3;
			if (m_nTalkContentLength > m_strTalkContent.length()+1)
				m_nTalkContentLength = m_strTalkContent.length()+1;
		}
		m_ui.content->setText(m_strTalkContent.substr(0, m_nTalkContentLength));
	}
}
