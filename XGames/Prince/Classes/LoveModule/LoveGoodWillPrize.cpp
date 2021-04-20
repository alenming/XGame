#include "LoveGoodWillPrize.h"

LoveGoodWillPrize::LoveGoodWillPrize(void)
{

}


LoveGoodWillPrize::~LoveGoodWillPrize(void)
{
}

LoveGoodWillPrize* LoveGoodWillPrize::create(LOVE_HERO_INFO heroInfo,SEND_GIFT_REWARD_VEC* reward,GoodWillPrizeCallback call)
{
	//背景设置
	LoveGoodWillPrize* buy = new LoveGoodWillPrize;
	if (buy && buy->init())
	{
		buy->autorelease();
		buy->initUi(heroInfo,reward,call);

		return buy;
	}


	CC_SAFE_DELETE(buy);
	return nullptr;
}

bool LoveGoodWillPrize::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}
	return true;
}

void LoveGoodWillPrize::onEnter()
{
	ModalLayer::onEnter();
}

void LoveGoodWillPrize::onExit()
{
	ModalLayer::onExit();
}

void LoveGoodWillPrize::initUi(LOVE_HERO_INFO heroInfo,SEND_GIFT_REWARD_VEC* reward,GoodWillPrizeCallback call)
{

	Size size = Director::getInstance()->getWinSize();
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIqingyuan/UIupgrade.ExportJson"));
	m_ui.pRoot->setPosition(cocos2d::Vec2((size.width-m_ui.pRoot->getContentSize().width)/2,
		(size.height - m_ui.pRoot->getContentSize().height)/2));

	// 将UILayer层加入到当前的场景
	this->addChild(m_ui.pRoot);

	m_ui.pRoot->getChildByName("Pnl_information")->setVisible(false);
	m_ui.pRoot->getChildByName("Img_back")->setVisible(false);
	m_ui.pRoot->getChildByName("Pnl_reward")->setVisible(false);

	auto exitCall = [this,call](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			call();
			dispose();
		}
	};

	m_ui.pRoot->addTouchEventListener(exitCall);

	m_ui.pPnlInformation = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_information"));
	m_ui.LabTips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlInformation,"Lab_leveltips"));
	m_ui.PnlHero = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlInformation,"Pnl_hero"));
	m_ui.LabName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlInformation,"Lab_name"));
	
	m_ui.ImgLove = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlInformation,"Img_love"));
	m_ui.ImgUp1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlInformation,"Img_up1"));
	m_ui.LabLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pPnlInformation,"Lab_level"));
	m_ui.ImgUp2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pPnlInformation,"Img_up2"));

	m_ui.LabTips->setFontName(FONT_FZZHENGHEI);
	m_ui.LabTips->setVisible(false); //不显示
	m_ui.LabName->setFontName(FONT_FZZHENGHEI);
	m_ui.LabLevel->setFontName(FONT_FZZHENGHEI);

	if (heroInfo.expType > heroInfo.lastExpType)
	{
		m_ui.ImgUp1->setVisible(true);
		m_ui.ImgUp1->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5f,Vec2(0,10)),
			CCMoveBy::create(0.5f,Vec2(0,-10)),nullptr)));
	}
	else
	{
		m_ui.ImgUp1->setVisible(false);
	}
	
	m_ui.ImgUp2->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5f,Vec2(0,10)),
		CCMoveBy::create(0.5f,Vec2(0,-10)),nullptr)));

	
	m_ui.pPnlReward = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_reward"));
	for (int i = 0; i < PrizeCount; i++)
	{
		Layout *item = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pPnlReward,"Pnl_item"+TO_STR(i+1)));

		m_ui.pPrizeItem[i] = item;
	}


	setPnlInformation(heroInfo);
	if (reward->size() > 0)
	{
		setPnlPrize(reward);
	}

	setAnimationLoveFeelUp();
}

void LoveGoodWillPrize::setPnlInformation(LOVE_HERO_INFO heroInfo)
{
	RowData* rowData = DataManager::getInstance()->searchToolById(heroInfo.petId);
	RowData *LoveHero = DataManager::getInstance()->searchLoveHeroById(heroInfo.petId);
	RowData *loveLevelData = DataManager::getInstance()->searchLoveLevelById(heroInfo.curLevel);

	UIToolHeader* item = UIToolHeader::create(heroInfo.petId);
	item->setAnchorPoint(Vec2(0,0));
	//情缘等级
	m_ui.ImgLove->loadTexture("Image/Icon/global/img_haogan2_" + TO_STR(heroInfo.expType) + ".png");
	m_ui.LabName->setString(rowData->getStringData("itemName"));
	if (loveLevelData)
	{
		m_ui.LabLevel->setString("LV" + TO_STR(loveLevelData->getIntData("level")));
	}
	else
	{
		m_ui.LabLevel->setString("LV" + TO_STR(0));
	}
	m_ui.LabTips->setString("LV" + TO_STR(LoveHero->getIntData("level")) + DataManager::getInstance()->searchCommonTexdtById(20283));
	setTextAddStroke(m_ui.LabLevel,Color3B(0x8e,0x00,0x03),2);
	setTextAddStroke(m_ui.LabTips,Color3B(0x00,0x00,0x00),2);

	m_ui.PnlHero->addChild(item);
}

void LoveGoodWillPrize::setPnlPrize(SEND_GIFT_REWARD_VEC* reward)
{
	for (int i = 0; i < reward->size(); i++)
	{
		SEND_GIFT_REWARD info = reward->at(i);
		UIToolHeader* item = UIToolHeader::create(info.templateId);
		item->setAnchorPoint(Vec2(0,0));
		item->setNumEx(info.amount);
		item->setToolId(info.templateId);
		item->setTipsEnabled(true);

		RowData* rowData = DataManager::getInstance()->searchToolById(info.templateId);
		Text *name = Text::create(rowData->getStringData("itemName"),FONT_FZZHENGHEI,24);
		name->setColor(Color3B(0xff,0xff,0xff));
		name->setPosition(Vec2(item->getContentSize().width/2,-20));
		setTextAddStroke(name,Color3B(0x79,0x03,0x03),2);
		item->addChild(name);


		if (reward->size() == 1) //一个就放正中间
		{
			if (m_ui.pPrizeItem[1])
			{
				m_ui.pPrizeItem[1]->addChild(item);
			}
		}
		else
		{
			if (m_ui.pPrizeItem[i])
			{
				m_ui.pPrizeItem[i]->addChild(item);
			}
		}
	}
}

void LoveGoodWillPrize::setAnimationLoveFeelUp()
{
	SEND_GIFT_REWARD_VEC* rewardVec = LoveModel::getInstance()->getLoveGiftReeardVec();
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anitupo1/Anitupo1.ExportJson");

	string aniName1 = "Anidown";
	string aniName2 = "Anicircle";

	Armature* armaturGood = Armature::create("Anitupo1");
	auto onResultAnimationCallFunc = [=](Armature *armature, MovementEventType eventType, const std::string& strName)->void
	{
		if (eventType == MovementEventType::COMPLETE)
		{
			armaturGood->getAnimation()->play(aniName2,-1,-1);
		}
	};
	armaturGood->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
	armaturGood->getAnimation()->play(aniName1,-1,-1);

	Bone*  bone = armaturGood->getBone("TIHUAN_TEXT2");
	Bone*  bone1 = armaturGood->getBone("TIHUAN_TEXT");
	if(bone && bone1)
	{
		Skin* _newSkin = Skin::create("Image/Icon/ActTip/img_haogantisheng.png");
		bone->addDisplay(_newSkin,0);
		Skin* _newSkin1 = Skin::create("Image/Icon/ActTip/img_haogantisheng.png");
		bone1->addDisplay(_newSkin1, 0);
	}

	armaturGood->setScale(0.6f);
	armaturGood->setPosition(Vec2(m_ui.pRoot->getChildByName("Img_title")->getContentSize().width/2 + 20,-130));
	m_ui.pRoot->getChildByName("Img_title")->addChild(armaturGood,2);

	auto call = [this,rewardVec]()->void
	{

		auto _call = [this,rewardVec]()->void
		{
			if (rewardVec->size() > 0)
			{
				auto __call = [this]()->void
				{
					auto moveCall = [this]()->void
					{
						GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_LOVE_REWARD);
						ActionCreator::runCommContinueAction(m_ui.pRoot->getChildByName("Img_back"));
					};

					m_ui.pRoot->getChildByName("Img_back")->setVisible(true);
					m_ui.pRoot->getChildByName("Img_back")->runAction(CCCallFunc::create(moveCall));
				};
				m_ui.pRoot->getChildByName("Pnl_reward")->setVisible(true);
				m_ui.pRoot->getChildByName("Pnl_reward")->setOpacity(0);
				m_ui.pRoot->getChildByName("Pnl_reward")->runAction(CCSequence::create(CCFadeIn::create(fadeTime),CCCallFunc::create(__call),nullptr));
			}
			else
			{
				auto moveCall = [this]()->void
				{
					GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_LOVE_REWARD);
					ActionCreator::runCommContinueAction(m_ui.pRoot->getChildByName("Img_back"));
				};

				m_ui.pRoot->getChildByName("Img_back")->setVisible(true);
				m_ui.pRoot->getChildByName("Img_back")->runAction(CCCallFunc::create(moveCall));
			}
		};

		m_ui.pRoot->getChildByName("Pnl_information")->setVisible(true);

		m_ui.pRoot->getChildByName("Pnl_information")->setOpacity(0);
		m_ui.pRoot->getChildByName("Pnl_information")->runAction(CCSequence::create(CCFadeIn::create(fadeTime),CCCallFunc::create(_call),nullptr));
	};
	armaturGood->runAction(CCSequence::create(CCDelayTime::create(0.3f),CCCallFunc::create(call),nullptr));
}
