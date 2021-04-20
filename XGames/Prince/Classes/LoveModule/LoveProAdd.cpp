#include "LoveProAdd.h"
#include "GuideCenter/GuideManager.h"

LoveProAdd::LoveProAdd(void)
{
	m_type = ATTRIBUTE_TYPE_NONE;
}


LoveProAdd::~LoveProAdd(void)
{
}

LoveProAdd* LoveProAdd::create(int petId)
{
	//背景设置
	LoveProAdd* loveAdd = new LoveProAdd;
	if (loveAdd && loveAdd->init())
	{
		loveAdd->autorelease();
		loveAdd->initUi(petId);

		return loveAdd;
	}


	CC_SAFE_DELETE(loveAdd);
	return nullptr;
}

bool LoveProAdd::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}
	return true;
}

void LoveProAdd::onEnter()
{
	LoveModel::getInstance()->addObserver(this);
	ModalLayer::onEnter();

	GuideManager::getInstance()->startGuideNextFrame(GUIDE_INTERFACE_LOVE_ADD);
}

void LoveProAdd::onExit()
{
	LoveModel::getInstance()->removeObserver(this);
	ModalLayer::onExit();
}

void LoveProAdd::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
	case LOVE_EVENT_TYPE_ADD_ATTRIBUTE:
		{
			setActiveInfo();
			attributePiao();
		}
		break;
	case LOVE_EVENT_TYPE_RESET_ATTRIBUTE:
		{
			setActiveInfo();
		}
		break;
	}
}

void LoveProAdd::initUi(int petId)
{
	m_petId = petId;
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIqingyuan/UIAddition.ExportJson"));
	this->addChild(m_ui.pRoot);	
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);

	m_ui.Img_di = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_di"));
	ActionCreator::runCommDlgAction(m_ui.Img_di);

	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Btn_Close"));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LoveProAdd::onBtnNotify, this));

	m_ui.btnReset = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Btn_Reset"));
	m_ui.btnReset->addTouchEventListener(CC_CALLBACK_2(LoveProAdd::onBtnNotify, this));

	m_ui.Lab_FriendShip = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Lab_FriendShip"));
	m_ui.Img_Position = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Position"));
	m_ui.Lab_Name = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Lab_Name"));
	m_ui.Lab_Tips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot,"Lab_Tips"));

	m_ui.Img_Addition = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Addition"));
	m_ui.Lab_ATK = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_Addition,"Lab_ATK"));
	m_ui.Lab_ATK->setFontName(FONT_FZZHENGHEI);
	RowData *petData = DataManager::getInstance()->searchCharacterById(m_petId);
	int atkType = petData->getIntData("atkTyp");
	if (atkType == 1)
	{
		m_ui.Lab_ATK->setString(DataManager::getInstance()->searchCommonTexdtById(20156));
	}
	else if (atkType == 2)
	{
		m_ui.Lab_ATK->setString(DataManager::getInstance()->searchCommonTexdtById(20289));
	}

	Text *Lab_HP = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_Addition,"Lab_HP"));
	Text *Lab_DEF1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_Addition,"Lab_DEF1"));
	Text *Lab_DEF2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_Addition,"Lab_DEF2"));
	m_ui.Lab_ATKAdd = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_Addition,"Lab_ATKAdd"));
	m_ui.Lab_HPAdd = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_Addition,"Lab_HPAdd"));
	m_ui.Lab_DEF1Add = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_Addition,"Lab_DEF1Add"));
	m_ui.Lab_DEF2Add = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_Addition,"Lab_DEF2Add"));
	m_ui.Lab_FriendShip->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_Name->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_Tips->setFontName(FONT_FZZHENGHEI);
	Lab_HP->setFontName(FONT_FZZHENGHEI);
	Lab_DEF1->setFontName(FONT_FZZHENGHEI);
	Lab_DEF2->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_ATKAdd->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_HPAdd->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_DEF1Add->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_DEF2Add->setFontName(FONT_FZZHENGHEI);

	
	for (int i = 0; i < ADDITION_COUNT; i++)
	{
		Layout *Pnl_Addition = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Addition" + TO_STR(i+1)));
		m_ui.AdditionImg_Icon[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Pnl_Addition,"Img_Icon"));
		m_ui.AdditionLab_AdditionName[i] = dynamic_cast<Text*>(Helper::seekWidgetByName(Pnl_Addition,"Lab_AdditionName"));
		m_ui.AdditionLab_Level[i] = dynamic_cast<Text*>(Helper::seekWidgetByName(Pnl_Addition,"Lab_Level"));
		m_ui.AdditionBtn_Add[i] = dynamic_cast<Button*>(Helper::seekWidgetByName(Pnl_Addition,"Btn_Add"));
		m_ui.AdditionBtn_Add[i]->addTouchEventListener(CC_CALLBACK_2(LoveProAdd::onBtnNotify, this));

		m_ui.AdditionLab_Cost[i] = dynamic_cast<Text*>(Helper::seekWidgetByName(Pnl_Addition,"Lab_Cost"));
		m_ui.AdditionImg_Max[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Pnl_Addition,"Img_Max"));

		m_ui.AdditionLab_AdditionName[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.AdditionLab_Level[i]->setFontName(FONT_FZZHENGHEI);
		m_ui.AdditionLab_Cost[i]->setFontName(FONT_FZZHENGHEI);
	}

	updataPanelInfo();
}

void LoveProAdd::updataPanelInfo()
{
	setFixedInfo();
	setActiveInfo();
}

void LoveProAdd::setFixedInfo()
{
	//名字
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_petId);
	RowData* rowData = DataManager::getInstance()->searchToolById(heroInfo->petId);
	m_ui.Lab_Name->setString(rowData->getStringData("itemName"));
	setTextAddStroke(m_ui.Lab_Name,Color3B(0x82,0x2b,0x2b),2);

	HeroParam *heroparam = HeroModel::getInstance()->searchHeroByTemplateId(heroInfo->petId);
	RowData *LoveHero = DataManager::getInstance()->searchLoveHeroById(heroInfo->petId);
	if (LoveHero)
	{
		//英雄描述
		string info = LoveHero->getStringData("heroTips");
		m_ui.Lab_Tips->setString(info);
	}

	switch (heroparam->mJobTyp)
	{
	case HERO_JOB_TYPE::HERO_DEFENSE:
		{
			m_ui.Img_Position->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_fang.png"));
		}	
		break;
	case HERO_JOB_TYPE::HERO_ATK:
		{
			m_ui.Img_Position->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_gong.png"));
		}
		break;
	case HERO_JOB_TYPE::HERO_HELP:
		{
			m_ui.Img_Position->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_fu.png"));
		}
		break;
	case HERO_JOB_TYPE::HERO_FIRST:
		{
			m_ui.Img_Position->loadTexture(ResourceUtils::getIconJobPath("img_shuxing1_xian.png"));
		}
		break;

	default:
		break;
	}
}

void LoveProAdd::setActiveInfo()
{
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_petId);

	//友情点
	m_ui.Lab_FriendShip->setString(TO_STR(heroInfo->friendShip));

	RowData *loveLevelData = DataManager::getInstance()->searchLoveLevelById(heroInfo->curLevel);
	//攻击
	RowData *atkData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeAtk);
	if (atkData)
	{
		m_ui.Lab_ATKAdd->setString("+" + TO_STR(atkData->getIntData("atk"))); //物理攻击
		m_ui.AdditionLab_Level[0]->setString(TO_STR(atkData->getIntData("strLevel")) + "/" + TO_STR(loveLevelData->getIntData("attrLimit")));
		if (atkData->getIntData("strLevel") >= loveLevelData->getIntData("attrLimit"))
		{
			m_ui.AdditionLab_Cost[0]->setVisible(false);
			m_ui.AdditionImg_Max[0]->setVisible(true);
		}
		else
		{
			m_ui.AdditionLab_Cost[0]->setVisible(true);
			m_ui.AdditionLab_Cost[0]->setString(TO_STR(atkData->getIntData("cost")));
			m_ui.AdditionImg_Max[0]->setVisible(false);
		}
	}

	//血量
	RowData *hpData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeHp);
	if (hpData)
	{
		m_ui.Lab_HPAdd->setString("+" + TO_STR(hpData->getIntData("hp"))); //血量
		m_ui.AdditionLab_Level[1]->setString(TO_STR(hpData->getIntData("strLevel")) + "/" + TO_STR(loveLevelData->getIntData("attrLimit")));
		if (hpData->getIntData("strLevel") >= loveLevelData->getIntData("attrLimit"))
		{
			m_ui.AdditionLab_Cost[1]->setVisible(false);
			m_ui.AdditionImg_Max[1]->setVisible(true);
		}
		else
		{
			m_ui.AdditionLab_Cost[1]->setVisible(true);
			m_ui.AdditionLab_Cost[1]->setString(TO_STR(hpData->getIntData("cost")));
			m_ui.AdditionImg_Max[1]->setVisible(false);
		}
	}

	//物防
	RowData *pddData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typePdd);
	if (pddData)
	{
		m_ui.Lab_DEF1Add->setString("+" + TO_STR(pddData->getFloatData("pddCoe")*100.0f) + "%"); //物防
		m_ui.AdditionLab_Level[2]->setString(TO_STR(pddData->getIntData("strLevel")) + "/" + TO_STR(loveLevelData->getIntData("attrLimit")));
		if (pddData->getIntData("strLevel") >= loveLevelData->getIntData("attrLimit"))
		{
			m_ui.AdditionLab_Cost[2]->setVisible(false);
			m_ui.AdditionImg_Max[2]->setVisible(true);
		}
		else
		{
			m_ui.AdditionLab_Cost[2]->setVisible(true);
			m_ui.AdditionLab_Cost[2]->setString(TO_STR(pddData->getIntData("cost")));
			m_ui.AdditionImg_Max[2]->setVisible(false);
		}
	}

	//法防
	RowData *mddData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeMdd);
	if (mddData)
	{
		m_ui.Lab_DEF2Add->setString("+" + TO_STR(mddData->getFloatData("mddCoe")*100.0f) + "%"); //法防
		m_ui.AdditionLab_Level[3]->setString(TO_STR(mddData->getIntData("strLevel")) + "/" + TO_STR(loveLevelData->getIntData("attrLimit")));
		if (mddData->getIntData("strLevel") >= loveLevelData->getIntData("attrLimit"))
		{
			m_ui.AdditionLab_Cost[3]->setVisible(false);
			m_ui.AdditionImg_Max[3]->setVisible(true);
		}
		else
		{
			m_ui.AdditionLab_Cost[3]->setVisible(true);
			m_ui.AdditionLab_Cost[3]->setString(TO_STR(mddData->getIntData("cost")));
			m_ui.AdditionImg_Max[3]->setVisible(false);
		}
	}
}

void LoveProAdd::onBtnNotify(Ref* sender, Widget::TouchEventType type)
{
	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_petId);
	RowData *loveLevelData = DataManager::getInstance()->searchLoveLevelById(heroInfo->curLevel);

	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (sender == m_ui.btnClose) //退出
		{
			dispose();
		}
		else if(sender == m_ui.btnReset)//重置
		{
			int friendShipSpend = DataManager::getInstance()->searchToolSystemById(LOVE_ATTRIBUTE_XI_SPEND)->getIntData("value");
			MainParam* pMainParam	= MainModel::getInstance()->getMainParam();

			RowData *atkData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeAtk);
			RowData *hpData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeHp);
			RowData *pddData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typePdd);
			RowData *mddData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeMdd);

			if (atkData->getIntData("atk")<= 0 && hpData->getIntData("hp") <= 0
				&& pddData->getFloatData("pddCoe") <= 0.0f && mddData->getFloatData("mddCoe") <= 0.0f)
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20309));
				return;
			}
			else
			{
				auto call = [=](Ref* ref, CustomRetType type)->void
				{
					if (type == RET_TYPE_OK)
					{
						if (pMainParam->mGold < friendShipSpend)
						{
							CustomGoldTips *_tips = CustomGoldTips::create();
							if (_tips)
							{
								Director::getInstance()->getRunningScene()->addChild(_tips);
							}
							//充值界面
							return;
						}

						LoveController::resetAttribute(heroInfo->petId);
					}
				};

				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20188) + 
					TO_STR(friendShipSpend) +DataManager::getInstance()->searchCommonTexdtById(20310)
					,call,CUSTOM_YES_NO);
			}
		}
		else if (sender == m_ui.AdditionBtn_Add[0])//攻击强化
		{
			RowData *atkData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeAtk);
			if (atkData)
			{
				if (heroInfo->friendShip < atkData->getIntData("cost"))
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20311));
					return;
				}
				else if (atkData->getIntData("strLevel") >= loveLevelData->getIntData("attrLimit"))
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20312));
					return;
				}
				m_type =  ATTRIBUTE_TYPE_ATK;
				LoveController::addAttribute(heroInfo->petId,m_type);
			}
		}
		else if (sender == m_ui.AdditionBtn_Add[1])//血量强化
		{
			RowData *hpData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeHp);
			if (hpData)
			{
				if (heroInfo->friendShip < hpData->getIntData("cost"))
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20311));
					return;
				}
				else if (hpData->getIntData("strLevel") >= loveLevelData->getIntData("attrLimit"))
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20312));
					return;
				}
				m_type = ATTRIBUTE_TYPE_HP;
				LoveController::addAttribute(heroInfo->petId,m_type);
			}
		}
		else if (sender == m_ui.AdditionBtn_Add[2])//物防强化
		{
			RowData *pddData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typePdd);
			if (pddData)
			{
				if (heroInfo->friendShip < pddData->getIntData("cost"))
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20311));
					return;
				}
				else if (pddData->getIntData("strLevel") >= loveLevelData->getIntData("attrLimit"))
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20312));
					return;
				}
				m_type = ATTRIBUTE_TYPE_PDD;
				LoveController::addAttribute(heroInfo->petId,m_type);
			}
		}
		else if (sender == m_ui.AdditionBtn_Add[3])//法防强化
		{
			RowData *mddData = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeMdd);
			if (mddData)
			{
				if (heroInfo->friendShip < mddData->getIntData("cost"))
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20311));
					return;
				}
				else if (mddData->getIntData("strLevel") >= loveLevelData->getIntData("attrLimit"))
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20312));
					return;
				}
				m_type = ATTRIBUTE_TYPE_MDD;
				LoveController::addAttribute(heroInfo->petId,m_type);
			}
		}
	}
}

void LoveProAdd::attributePiao()
{
	string strFileName;
	strFileName.assign("Image/UIqingyuan/UIAtrributeAdd.ExportJson");
	Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

	Text* name = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"name"));
	name->setFontName(FONT_FZZHENGHEI);

	Text* count = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"add"));
	count->setFontName(FONT_FZZHENGHEI);

	LOVE_HERO_INFO *heroInfo = LoveModel::getInstance()->getHeroByHeroId(m_petId);
	RowData *petData = DataManager::getInstance()->searchCharacterById(m_petId);
	switch (m_type)
	{
	case ATTRIBUTE_TYPE_ATK:
		{
			int atkType = petData->getIntData("atkTyp");
			if (atkType == 1)
			{
				name->setString(DataManager::getInstance()->searchCommonTexdtById(20156));
			}
			else if (atkType == 2)
			{
				name->setString(DataManager::getInstance()->searchCommonTexdtById(20289));
			}

			RowData *attibuiteData1 = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeAtk-1);
			RowData *attibuiteData2 = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeAtk);
			int attibuiteCount = 0;
			if (attibuiteData2 && attibuiteData1)
			{
				attibuiteCount = attibuiteData2->getIntData("atk") - attibuiteData1->getIntData("atk");
			}
			count->setString("+" + TO_STR(attibuiteCount));
		}
		break;
	case ATTRIBUTE_TYPE_HP:
		{
			name->setString(DataManager::getInstance()->searchCommonTexdtById(20153));
			RowData *attibuiteData1 = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeHp-1);
			RowData *attibuiteData2 = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeHp);
			int attibuiteCount = 0;
			if (attibuiteData2 && attibuiteData1)
			{
				attibuiteCount = attibuiteData2->getIntData("hp") - attibuiteData1->getIntData("hp");
			}
			count->setString("+" + TO_STR(attibuiteCount));
		}
		break;
	case ATTRIBUTE_TYPE_PDD:
		{
			name->setString(DataManager::getInstance()->searchCommonTexdtById(20154));
			RowData *attibuiteData1 = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typePdd-1);
			RowData *attibuiteData2 = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typePdd);
			float attibuiteCount = 0;
			if (attibuiteData2 && attibuiteData1)
			{
				attibuiteCount = attibuiteData2->getFloatData("pddCoe")*100.0f - attibuiteData1->getFloatData("pddCoe")*100.0f;
			}
			count->setString("+" + TO_STR(attibuiteCount) + "%");
		}
		break;
	case ATTRIBUTE_TYPE_MDD:
		{
			name->setString(DataManager::getInstance()->searchCommonTexdtById(20155));
			RowData *attibuiteData1 = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeMdd-1);
			RowData *attibuiteData2 = DataManager::getInstance()->searchLoveAtrributeById(heroInfo->typeMdd);
			float attibuiteCount = 0;
			if (attibuiteData2 && attibuiteData1)
			{
				attibuiteCount = attibuiteData2->getFloatData("mddCoe")*100.0f - attibuiteData1->getFloatData("mddCoe")*100.0f;
			}
			count->setString("+" + TO_STR(attibuiteCount) + "%");
		}
		break;
	default:
		break;
	}
	setTextAddStroke(name,Color3B(0x10,0x13,0x77),2);
	setTextAddStroke(count,Color3B(0x10,0x13,0x77),2);

	auto call = [layer]()->void
	{
		layer->removeFromParent();
	};
	ImageView *img = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"img"));
	img->runAction(CCSequence::create(EaseSineOut::create(Spawn::create(ScaleTo::create(0.2, 1.5f),
		MoveBy::create(0.2, Vec2(0.0f, 21.0f)), nullptr)),
		EaseSineIn::create(Spawn::create(ScaleTo::create(0.2, 1.0f),
		MoveBy::create(0.2, Vec2(0.0f, 5.0f)), nullptr)),
		MoveBy::create(0.8, Vec2(0.0f, 102.0f)),
		Spawn::create(FadeOut::create(0.2), MoveBy::create(0.2, Vec2(0.0f, 22.0f)), nullptr),
		CCCallFunc::create(call),nullptr));

	layer->setPosition(Vec2(m_ui.Img_di->getContentSize().width/2-layer->getContentSize().width/2,m_ui.Img_di->getContentSize().height/2));
	m_ui.Img_di->addChild(layer,10);
}

