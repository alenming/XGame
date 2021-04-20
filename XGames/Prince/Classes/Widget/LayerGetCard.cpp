#include "LayerGetCard.h"
#include "Utils/StringFormat.h"
#include "LayerHeroRelation.h"
#include "HeroRelationModule/HeroRelationModel.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;  

LayerGetCard::LayerGetCard()
{
	m_call = nullptr;		//初始化一下
}

LayerGetCard::~LayerGetCard()
{

}

LayerGetCard* LayerGetCard::create(int showHeroId,bool isChangeChip,int changeChipCount,getCardCallback call)
{
	//背景设置
	LayerGetCard* _LayerGetCard = new LayerGetCard;
	if (_LayerGetCard && _LayerGetCard->init())
	{
		_LayerGetCard->autorelease();
		_LayerGetCard->initUi(showHeroId,isChangeChip,changeChipCount,call);
		return _LayerGetCard;
	}

	CC_SAFE_DELETE(_LayerGetCard);
	return nullptr;
}

bool LayerGetCard::initUi(int showHeroId,bool isChangeChip,int changeChipCount,getCardCallback call)
{
	m_call = call;
	m_CurShowHeroId = showHeroId;
	m_isChangeChip = isChangeChip;
	m_ChangChipCount = changeChipCount;

	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIgetCard/UIgetcard.ExportJson"));
	this->addChild(m_pRoot);	
	m_pRoot->setSize(size);
	m_pRoot->setPosition(pos);
	m_pRoot->setBackGroundImage("Image/Bg/img_bg2.png");

	//展示抽卡
	pShowChouka = dynamic_cast<Layout*>(m_pRoot->getChildByName("Panel_zhanshi"));
	pShowChoukaPrompt = dynamic_cast<Layout*>(pShowChouka->getChildByName("Panel_tishi"));
	pShowChoukaPrompt->setVisible(false);

	pShowChoukaPromptWenzi = dynamic_cast<Layout*>(pShowChouka->getChildByName("Panel_tishi_wenzi"));
	pShowChoukaPromptWenzi->setVisible(false);

	pChangChipCount = dynamic_cast<Text*>(pShowChouka->getChildByName("Lab_zhuanhua"));
	pChangChipCount->setFontName(FONT_FZZHENGHEI);
	pShowChouka->setVisible(false);

	pShowHero = dynamic_cast<Layout*>(pShowChouka->getChildByName("Pnl_hero"));
	pShowHeroPos = pShowHero->getPosition();
	heroAni = dynamic_cast<Layout*>(pShowChouka->getChildByName("hero_ani"));
	heroAniPos = heroAni->getPosition();

	auto touchCall = [this](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			if(!m_isChangeChip && checkFatedHeroId())
			{
				Director::getInstance()->getRunningScene()->addChild(LayerHeroRelation::create(m_CurShowHeroId, false, m_call), 999);
			}
			else
			{
				if (m_call)
				{
					m_call();
				}
			}
			dispose();
		}
	};
	pShowCardMask = dynamic_cast<Layout*>(Helper::seekWidgetByName(pShowChouka,"mask"));
	pShowCardMask->addTouchEventListener(touchCall);

	pTextHeroProduct = dynamic_cast<Text*>(Helper::seekWidgetByName(pShowChouka,"Lab_jieshao1"));
	pTextHeroProduct->setFontName(FONT_FZZHENGHEI);
	pWidget.push_back(pTextHeroProduct);

	pImgHeroPro = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pShowChouka,"img_pro"));
	pWidget.push_back(pImgHeroPro);

	pImgHeroSkill2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pShowChouka,"img_skill_2"));
	pWidget.push_back(pImgHeroSkill2);

	for (int i = 0; i < pWidget.size(); i++)
	{
		Vec2 pos = pWidget.at(i)->getPosition();
		pWidgetPos.push_back(pos);
	}

	showResultShowCardsItem();
	showCard();

	return true;
}

void LayerGetCard::showResultShowCardsItem()
{
	pShowHero->setPosition(pShowHeroPos);
	
	UICardHead *head;
	if(m_isChangeChip)
	{
		head = UICardHead::create(m_CurShowHeroId,false,true);
	}
	else
	{
		head = UICardHead::create(m_CurShowHeroId);
		head->runAction(CCRepeatForever::create(CCSequence::create(CCMoveBy::create(0.5,Vec2(0,3)),
			CCMoveBy::create(1.0,Vec2(0,-6)),CCMoveBy::create(0.5,Vec2(0,3)),nullptr)));
	}

	head->setScale(0.75f);
	pShowHero->addChild(head,1,"UICardHead");
}

void LayerGetCard::showCard()
{
	pShowChouka->setVisible(true);
	pShowCardMask->setVisible(false);

	for (int i = 0; i < pWidget.size(); i++)
	{
		pWidget.at(i)->setVisible(false);
	}

	if (m_isChangeChip)
	{
		pShowCardMask->setVisible(true);
		ActionCreator::runCommContinueAction(pShowCardMask->getChildByName("Img_jixu"));
		ImageView *img = dynamic_cast<ImageView*>(pShowCardMask->getChildByName("Img_jixu"));

		pChangChipCount->setVisible(true);
		pChangChipCount->setText(DataManager::getInstance()->searchCommonTexdtById(20001) + _TO_STR(m_ChangChipCount) + DataManager::getInstance()->searchCommonTexdtById(20002));
		setTextAddStroke(pChangChipCount,Color3B(0x00, 0x00, 0x00),2);
	}
	else
	{
		pChangChipCount->setVisible(false);
		
		//////////////////////////////////////////////////////////////////////
		//英雄 显示简介
		pShowChoukaPrompt->setVisible(true);
		pShowChoukaPromptWenzi->setVisible(true);
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_HuoDeXinKa/Ani_HuoDeXinKa.ExportJson");
		Armature* armature = Armature::create("Ani_HuoDeXinKa");
		UICardHead *head = (UICardHead*)pShowHero->getChildByName("UICardHead");
		head->getScaleImg()->setScale(0.1f);
		head->getScaleImg()->runAction(CCSequence::create(ScaleTo::create(0.5f, 1.25f),ScaleTo::create(0.1f, 1.0f),nullptr));

		auto onResultAnimationCallFunc = [this,head](Armature *armature, MovementEventType eventType, const std::string& strName)->void
		{
			if (eventType == MovementEventType::COMPLETE)
			{
				armature->removeFromParent();

				pShowChoukaPrompt->setVisible(false);

				ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anikapaizhuangshi/Anikapaizhuangshi.ExportJson");
				Armature* armature = Armature::create("Anikapaizhuangshi");

				auto onResultAnimationCallFunc = [this](Armature *armature, MovementEventType eventType, const std::string& strName)->void
				{
					if (eventType == MovementEventType::COMPLETE)
					{
						armature->removeFromParent();

						showHeroIntroduction();
						//英雄头像放左边 并放大
						auto call = [this]()->void
						{
							showHeroHeadRunAction();
							showHeroAni();
						};

						pShowHero->runAction(CCSequence::create(CCSpawn::create(CCMoveTo::create(0.3f,Vec2(160,pShowHeroPos.y-30)),
							CCScaleTo::create(0.3f,1.15f),nullptr),CCCallFunc::create(call),nullptr));
					}
				};
				armature->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
				armature->getAnimation()->play("Animation5");
				armature->setPosition(Vec2(head->getContentSize().width/2-8,head->getContentSize().height/2+12));
				head->addChild(armature);
			}
		};

		armature->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
		armature->getAnimation()->play("Ani_HuoDeXinKa");
		armature->setPosition(Vec2(pShowChoukaPrompt->getContentSize().width/2,pShowChoukaPrompt->getContentSize().height/2));
		pShowChoukaPrompt->addChild(armature);

		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_HuoDeXinKa_WenZi/Ani_HuoDeXinKa_WenZi.ExportJson");
		Armature* armatureWnezi = Armature::create("Ani_HuoDeXinKa_WenZi");
		auto onResultAnimationCallFuncWenzi = [this,head](Armature *armature, MovementEventType eventType, const std::string& strName)->void
		{
			if (eventType == MovementEventType::COMPLETE)
			{
				armature->removeFromParent();

				pShowChoukaPromptWenzi->setVisible(false);
			}
		};
		armatureWnezi->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFuncWenzi);
		armatureWnezi->getAnimation()->play("Ani_HuoDeXinKa");
		armatureWnezi->setPosition(Vec2(pShowChoukaPromptWenzi->getContentSize().width/2,pShowChoukaPromptWenzi->getContentSize().height/2));
		pShowChoukaPromptWenzi->addChild(armatureWnezi);
	}
}

void LayerGetCard::showHeroIntroduction()
{
	Size size = Director::getInstance()->getWinSize();

	for (int i = 0; i < pWidget.size(); i++)
	{
		pWidget.at(i)->setVisible(true);
		pWidget.at(i)->setPosition(Vec2(size.width + pWidget.at(i)->getContentSize().width,pWidgetPos[i].y));
	}

	RowData *heroInfo = DataManager::getInstance()->searchToolById(m_CurShowHeroId); //道具表
	RowData* pRowPetData = DataManager::getInstance()->searchCharacterById(m_CurShowHeroId);//英雄表
	pTextHeroProduct->setText(heroInfo->getStringData("itemInfo"));

	HeroParam *heroparam = HeroModel::getInstance()->searchHeroByTemplateId(m_CurShowHeroId);
	//TextAtlas *Lab_kezhi = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(pShowChouka,"Lab_kezhi"));
	ImageView* img_zizhi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pShowChouka, "Img_zizhiLv"));
	if (heroparam)
	{
		//Lab_kezhi->setString(TO_STR(heroparam->mTalent));
		img_zizhi->loadTexture(ResourceUtils::getHeroTalentPath(heroparam->mTalent));
	}

	//奥义技能
	RowData* pRowAsklIdData = DataManager::getInstance()->searchSkillById(pRowPetData->getIntData("a_sklID"));
	if (pRowAsklIdData)
	{
		setShowHeroSkillInfo(pRowAsklIdData,pImgHeroSkill2,Color3B(240, 119, 184));
	}
	else
	{
		pImgHeroSkill2->setVisible(false);
	}
}

void LayerGetCard::setShowHeroSkillInfo(RowData *data,ImageView *Node,Color3B color)
{
	Node->setVisible(true);
	ImageView *img = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Node,"img_skill"));
	img->loadTexture(ResourceUtils::getSmallIconPath(data->getIntData("resId")));

	Text *name = dynamic_cast<Text*>(Helper::seekWidgetByName(Node,"img_skillname"));
	if (name->getChildByName("SkillName"))
	{
		name->removeChildByName("SkillName");
	}

	SimpleRichText *richText = SimpleRichText::create();
	if (nullptr != richText)
	{
		richText->ignoreContentAdaptWithSize(false);
		richText->setAnchorPoint(Vec2(0,1));
		richText->setContentSize(name->getContentSize());
		richText->setPosition(Vec2(-70,name->getContentSize().height+70));
		richText->setString(data->getStringData("capTxt"),20,Color3B(0xff, 0xea, 0xf8));
		name->addChild(richText,0,"SkillName");
	}
}

void LayerGetCard::showHeroHeadRunAction()
{
	//英雄归位
	auto call = [this]()->void
	{
		for (int i = 0; i < pWidget.size(); i++)
		{
			if (i < pWidget.size()-1)
			{
				CCMoveTo *widgetMove = CCMoveTo::create(_MOVE_TIME,pWidgetPos[i]);

				pWidget.at(i)->runAction(CCSequence::create(CCDelayTime::create(_MOVE_TIME/2*i),
					CCEaseSineOut::create(widgetMove),nullptr));
			}
			else
			{
				CCMoveTo *widgetMove = CCMoveTo::create(_MOVE_TIME,pWidgetPos[i]);
				auto call2 = [this]()->void
				{
					pShowCardMask->setVisible(true);
					ActionCreator::runCommContinueAction(pShowCardMask->getChildByName("Img_jixu"));

					GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_CARD_SHOW);
				};

				pWidget.at(i)->runAction(CCSequence::create(CCDelayTime::create(_MOVE_TIME/2*i),
					CCEaseSineOut::create(widgetMove),CCCallFunc::create(call2),nullptr));
			}
		}
	};

	pShowHero->runAction(CCCallFunc::create(call));
}

void LayerGetCard::showHeroAni()
{
	Size size = Director::getInstance()->getVisibleSize();
	heroAni->setPosition(Vec2(heroAniPos.y,size.height));
	m_pSkelFighter = SpineAnimation::createFighterWithResId(m_CurShowHeroId);
	m_pSkelFighter->setPosition(Vec2(heroAni->getCustomSize().width/2,0));
	heroAni->addChild(m_pSkelFighter);

	string startActionName = "jump1";
	string endActionName = "jump2";
	int jumpHeight = 250;
	int speed = 2200;

	//播放跳跃开始动作
	auto pEntry = m_pSkelFighter->setAnimation(0, startActionName, false);
	m_pSkelFighter->setTimeScale(10.0f);

	//最终回调
	auto endCallBack = [this](int trackIndex, int loops)
	{
		auto shade = Sprite::create("Image/Fight/img_shadow.png");
		m_pSkelFighter->addChild(shade);

		m_pSkelFighter->setTimeScale(1.0f);
		m_pSkelFighter->addAnimation(0,"wait",false);
		m_pSkelFighter->addAnimation(0,"victory",false);
		m_pSkelFighter->addAnimation(0,"wait",true);

		//任务背景动画
		ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_KaPaiHuoQu_G/Ani_KaPaiHuoQu_G.ExportJson");
		Armature* pArmature = Armature::create("Ani_KaPaiHuoQu_G");//名字需要和 .ExportJson一致
		pArmature->getAnimation()->playByIndex(0,-1,1);
		pArmature->setPosition(Vec2(getContentSize().width/2-13,getContentSize().height/2-55));//设置动画精灵位置 
		pShowChouka->addChild(pArmature,0);
	};

	//播放跳跃结束动作
	auto jumpEndCallBack = 
		[this, endActionName, endCallBack]()->void
	{
		auto pEntry = m_pSkelFighter->setAnimation(0, endActionName, false);
		m_pSkelFighter->setTrackCompleteListener(pEntry, endCallBack);
	};

	//播放跳跃中间动作
	auto jumpStarCallBack = [this, jumpEndCallBack,speed,jumpHeight](int trackIndex, int loops)->void
	{
		float distance = heroAni->getPosition().distance(heroAniPos);
		float duration = distance / speed; //持续时间补正
		float height = (distance / 1000.0f) * jumpHeight;

		auto jump = JumpTo::create(duration, heroAniPos, height, 1);
		CallFunc* callBackAction = CallFunc::create(jumpEndCallBack);
		heroAni->runAction(Sequence::create(jump, callBackAction, nullptr));
	};

	m_pSkelFighter->setTrackCompleteListener(pEntry, jumpStarCallBack);
}

void LayerGetCard::onEnter()
{
	ModalLayer::onEnter();
}

void LayerGetCard::onExit()
{
	ModalLayer::onExit();
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIgetCard");
}

bool LayerGetCard::checkFatedHeroId()
{
	HeroParam* heroParam = HeroModel::getInstance()->searchHeroByTemplateId(m_CurShowHeroId);
	if(heroParam == nullptr)
	{
		return false;
	}

	vector<sFateHeroOrder> vFatedHeroId;
	HeroRelationModel::getInstance()->getFatedHeroId(heroParam->mTemplateId, vFatedHeroId);

	for(int i=0; i<vFatedHeroId.size(); i++)
	{
		if(HeroModel::getInstance()->searchHeroByTemplateId(vFatedHeroId.at(i).valueData))
		{
			//获取缘分ID列表
			vector<int> vFatedId;
			HeroRelationModel::getInstance()->getFatedIdList(vFatedId);
			
			for(int j=0; j<vFatedId.size(); j++)
			{
				//只有缘分ID在表格中存在时才弹窗，奇葩的表格设计...
				RowData* _rowData = DataManager::getInstance()->searchHeroRelationById(vFatedId.at(j));
				if(_rowData != nullptr)
				{
					return true;
				}
			}

			return false;
		}
	}

	return false;
}
