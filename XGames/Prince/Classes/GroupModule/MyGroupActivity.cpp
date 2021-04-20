#include "MyGroupActivity.h"
#include "Temp/CustomPop.h"
#include "Communication/SocketCommunicator.h"
#include "MyGroupController.h"
#include "MyGroupModel.h"
#include "MainModule/MainCityLoadingScene.h"
#include "Common/Common.h"
#include "Widget/LayerGameRules.h"
#include "MyGroupActivityHistory.h"
#include "Temp/CustomGoldTips.h"

#define  SHOWITEM   3
#define  TENITEM	9

//金币、银币ID
const int goldId = 11001;
const int coinId = 11002;

MyGroupActivity::MyGroupActivity():weizhi(0),isRun(false),firstXunlong(true),pHero(nullptr)
{
	MyGroupModel::getInstance()->addObserver(this);
}

MyGroupActivity::~MyGroupActivity()
{
	MyGroupModel::getInstance()->removeObserver(this);
}

Layer* MyGroupActivity::create( )
{ 
	auto pRet = new MyGroupActivity(); 
	if (pRet && pRet->init()) 
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

bool MyGroupActivity::init( )
{
	if(!ModalLayer::init())
	{
		return false;
	}


	initUI();


	return true;
}

void MyGroupActivity::onEnter()
{
	ModalLayer::onEnter();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(MyGroupActivity::updateNextFram), this, 0, false);
}

void MyGroupActivity::updateNextFram(float ft)
{
	updatePublicUI();
	updateNineItemUI();
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(MyGroupActivity::updateNextFram), this);
}

void MyGroupActivity::onExit()
{
	ModalLayer::onExit();
}

void MyGroupActivity::update(float dt)
{
	this->unscheduleUpdate();
	this->removeFromParent();
	
}

void MyGroupActivity::initUI()
{
	// 加载 UI;
	string strFileName = "Image/UIGang/UIGang_Dragon.ExportJson";
	// Root;
	m_ui.uiRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.uiRoot);

	m_ui.uiRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg",ui::Widget::TextureResType::LOCAL);

	m_ui.uiBtnBegin = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Dragon"));
	m_ui.uiBtnHistory = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Record"));
	m_ui.uiBtnRefresh= dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Reset"));
	m_ui.uiBtnRule = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Rule"));
	m_ui.uiBtnBegin->setVisible(false);;
	m_ui.uiBtnRefresh->setVisible(false);


	m_ui.uiBtnBegin->addTouchEventListener(CC_CALLBACK_2(MyGroupActivity::onBtnClick,this,ACTIVITY_CALLBACK_TYPE::ACT_CB_BEGIN));
	m_ui.uiBtnHistory->addTouchEventListener(CC_CALLBACK_2(MyGroupActivity::onBtnClick,this,ACTIVITY_CALLBACK_TYPE::ACT_CB_HISTORY));
	m_ui.uiBtnRefresh->addTouchEventListener(CC_CALLBACK_2(MyGroupActivity::onBtnClickRefresh,this));
	m_ui.uiBtnRule->addTouchEventListener(CC_CALLBACK_2(MyGroupActivity::onBtnClick,this,ACTIVITY_CALLBACK_TYPE::ACT_CB_RULE));

	m_ui.uiLayoutshowHero = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Pnl_Hero"));

	for (int i = 0; i < SHOWITEM; i++)
	{
		auto showitem = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Pnl_ShowItem"+TO_STR(i+1)));
		m_ui.uiLayoutshowItem.push_back(showitem);
	}
	for (int i = 0; i < TENITEM; i++)
	{
		auto showitem = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Pnl_Item"+TO_STR(i+1)));
		m_ui.uiLayoutNineItem.push_back(showitem);
	}
	auto img_Di =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiRoot,"Img_di"));

	m_ui.Img_Free =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(img_Di,"Img_Free"));
	m_ui.Img_Gold =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(img_Di,"Img_Gold"));
	m_ui.Lab_Go =  dynamic_cast<Text*>(Helper::seekWidgetByName(img_Di,"Lab_Go"));
	m_ui.Lab_Gold =  dynamic_cast<Text*>(Helper::seekWidgetByName(img_Di,"Lab_Gold"));
	m_ui.Lab_Over =  dynamic_cast<Text*>(Helper::seekWidgetByName(img_Di,"Lab_Over"));

	m_ui.Img_Free->setVisible(false);
	m_ui.Img_Gold->setVisible(false);
	m_ui.Lab_Go->setVisible(false);
	m_ui.Lab_Gold->setVisible(false);
	m_ui.Lab_Over->setVisible(false);

	m_ui.uiTextFindCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_Count"));

	//公用返回
	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");
			//返回按钮不能屏蔽
			//if (isRun)
			//{
			//	return;
			//}
			scheduleUpdate();
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_SEABED_DRAGON);
	this->addChild(_layerCommHeadInfo);
}

void  MyGroupActivity::onBtnClickRefresh( Ref* reff, Widget::TouchEventType type )
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		if (isRun)
		{
			return;
		}
		isRun = true;
		MyGroupController::getInstance()->sendRefreshList();
	}
}

void  MyGroupActivity::onBtnClickByListId( Ref* reff, Widget::TouchEventType type, int listId )
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		int gold = MainModel::getInstance()->getMainParam()->mGold;
		if ( MyGroupModel::getInstance()->getGroupActivityInfo()->showedIdflag.size()!=0 &&gold< MyGroupModel::getInstance()->getGroupActivityInfo()->goldCost)
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
				return;
			}
		}


		if (isRun)
		{
			return;
		}
		isRun = true;
		weizhi = listId;
		firstXunlong = false;
		MyGroupController::getInstance()->sendClieckNineList(listId);
	}
}

void MyGroupActivity::onBtnClick( Ref* reff, Widget::TouchEventType type, ACTIVITY_CALLBACK_TYPE cbType )
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		if (isRun)
		{
			return;
		}
		switch (cbType)
		{
			case ACT_CB_RULE:
				{
					LayerGameRules* _help = LayerGameRules::create(GROUP_ACTIVITY_RULE);
					if (nullptr != _help)
					{
						this->addChild(_help, 5);
						ActionCreator::runCommDlgAction(_help);
					}
					break;
				}
			case ACT_CB_HISTORY:
				{
					MyGroupController::getInstance()->sendXunlongHistory();
				}
				break;
			case ACT_CB_BEGIN:
				{
					MyGroupController::getInstance()->sendClieckNineList(10);//发送10用来记录已经点击过寻龙按钮
					isRun = true;
				}
				break;
			default:
				break;
		}
	}
}

void MyGroupActivity::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
		case nMAIN_CMD_GROUP_ACTIVITY_CLICK:
			{
				sActivityInfo* publicInfo = MyGroupModel::getInstance()->getGroupActivityInfo();

				updateOtherUI();
				if (!firstXunlong)
				{
					updateOneItemAction();
				}

				if (publicInfo->isXunlong == 1 && firstXunlong )
				{
					firstXunlong = false;
					//一点开始寻龙就不能再有提示了
					for (int i = 0; i < m_ui.uiLayoutNineItem.size(); i++)
					{
						Layout* _itemFater = m_ui.uiLayoutNineItem.at(i);
						UIToolHeader* _itemSon = static_cast<UIToolHeader*>(_itemFater->getChildByName("_item"+TO_STR(i+1)));
						if (_itemSon)
						{
							_itemSon->setTipsEnabled(false);
						}
					}
					playStartAnimation();

				}
			}
			break;
		case nMAIN_CMD_GROUP_ACTIVITY_REFRESH:
			{
				firstXunlong = false;
				updateOtherUI();
				playNineItemInAnimation();
			}
			break;
		case nMAIN_CMD_GROUP_ACTIVITY_HISTORY:
			{
				getScheduler()->schedule([=](float dt)->void
				{
					this->addChild(MyGroupActivityHistory::create());

					this->getScheduler()->unschedule("MyGroupActivityHistory::create", this);
				},this, 0, 0, 0, false, "MyGroupActivityHistory::create");	
				break;
			}
			break;
		default:
			break;
	}
}

void MyGroupActivity::updatePublicUI()
{
	showHeroAni();
	showThreeItem();
	updateOtherUI();
}

void MyGroupActivity::updateOtherUI()
{
	sActivityInfo* publicInfo = MyGroupModel::getInstance()->getGroupActivityInfo();
	if (publicInfo->showedIdflag.size() == 0)
	{
		m_ui.Img_Free->setVisible(true);
		m_ui.Img_Gold->setVisible(false);
		m_ui.Lab_Go->setVisible(false);
		m_ui.Lab_Gold->setVisible(false);
		m_ui.Lab_Over->setVisible(false);

		m_ui.uiBtnBegin->setEnabled(true);
		m_ui.uiBtnBegin->setVisible(true);
		m_ui.uiBtnRefresh->setVisible(false);
		m_ui.uiBtnRefresh->setEnabled(false);
	}
	else if ( publicInfo->showedIdflag.size() ==1)
	{
		m_ui.Img_Free->setVisible(false);
		m_ui.Img_Gold->setVisible(true);
		m_ui.Lab_Go->setVisible(true);
		m_ui.Lab_Gold->setVisible(true);
		m_ui.Lab_Over->setVisible(false);

		m_ui.uiBtnBegin->setEnabled(false);
		m_ui.uiBtnBegin->setVisible(false);
		m_ui.uiBtnRefresh->setVisible(true);
		m_ui.uiBtnRefresh->setEnabled(true);

		m_ui.Lab_Gold->setString(TO_STR(publicInfo->goldCost));
		m_ui.Lab_Gold->setFontName(FONT_FZZHENGHEI);
		m_ui.Lab_Gold->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.Lab_Gold, Color3B(0x79, 0x03, 0x03),2);

		m_ui.Lab_Go->setFontName(FONT_FZZHENGHEI);
		m_ui.Lab_Go->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.Lab_Go, Color3B(0x79, 0x03, 0x03),2);
	}
	else if ( publicInfo->showedIdflag.size() >1 && publicInfo->showedIdflag.size() != 9)
	{
		m_ui.Img_Free->setVisible(false);
		m_ui.Img_Gold->setVisible(true);
		m_ui.Lab_Go->setVisible(true);
		m_ui.Lab_Gold->setVisible(true);
		m_ui.Lab_Over->setVisible(false);

		m_ui.uiBtnBegin->setEnabled(false);
		m_ui.uiBtnBegin->setVisible(false);
		m_ui.uiBtnRefresh->setVisible(true);
		m_ui.uiBtnRefresh->setEnabled(true);

		m_ui.Lab_Gold->setString(TO_STR(publicInfo->goldCost));
		m_ui.Lab_Gold->setFontName(FONT_FZZHENGHEI);
		m_ui.Lab_Gold->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.Lab_Gold, Color3B(0x79, 0x03, 0x03),2);

		m_ui.Lab_Go->setFontName(FONT_FZZHENGHEI);
		m_ui.Lab_Go->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.Lab_Go, Color3B(0x79, 0x03, 0x03),2);
	}
	else if (publicInfo->showedIdflag.size() == 9)
	{
		m_ui.Img_Free->setVisible(false);
		m_ui.Img_Gold->setVisible(false);
		m_ui.Lab_Go->setVisible(false);
		m_ui.Lab_Gold->setVisible(false);
		m_ui.Lab_Over->setVisible(true);

		m_ui.uiBtnBegin->setEnabled(false);
		m_ui.uiBtnBegin->setVisible(false);
		m_ui.uiBtnRefresh->setVisible(true);
		m_ui.uiBtnRefresh->setEnabled(true);

		m_ui.Lab_Over->setFontName(FONT_FZZHENGHEI);
		m_ui.Lab_Over->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.Lab_Over, Color3B(0x79, 0x03, 0x03),2);

	}
	auto buildInfo = MyGroupModel::getInstance()->getBuildInfoByID(eBuildType::eBUILD_DOSNATE_XUNLONG);
	m_ui.uiTextFindCount->setString(TO_STR((buildInfo->reset)-(publicInfo->clickCount))+"/"+TO_STR(buildInfo->reset));
	m_ui.uiTextFindCount->setFontName(FONT_FZZHENGHEI);
	m_ui.uiTextFindCount->setColor(Color3B(0xff, 0xff, 0xff));
	setTextAddStroke(m_ui.uiTextFindCount, Color3B(0x79, 0x03, 0x03),2);


	if (publicInfo->clickCount == buildInfo->reset)
	{
		m_ui.uiBtnBegin->setEnabled(false);
		m_ui.uiBtnBegin->setVisible(false);
		m_ui.uiBtnRefresh->setVisible(true);
		m_ui.uiBtnRefresh->setEnabled(false);
		m_ui.uiBtnRefresh->setTouchEnabled(false);
		m_ui.uiBtnRefresh->setBright(false);
	}
	if (publicInfo->isXunlong ==1)
	{
		m_ui.uiBtnBegin->setEnabled(false);
		m_ui.uiBtnBegin->setVisible(false);
		m_ui.uiBtnRefresh->setVisible(true);
		m_ui.uiBtnRefresh->setEnabled(true);
	}
}

void MyGroupActivity::updateNineItemUI()
{
	sActivityInfo* publicInfo = MyGroupModel::getInstance()->getGroupActivityInfo();


	if (publicInfo->showedIdflag.size()>0)
	{
		for (int i = 0; i < publicInfo->nineList.size(); i++)
		{
			bool bTemp = false;
			for (int j = 0; j< publicInfo->showedIdflag.size(); j++)
			{
				if (i == publicInfo->showedIdflag.at(j))
				{
					bTemp = true;
					break;
				}
			}
			if (bTemp)
			{
				auto _layout = dynamic_cast<Layout*>(m_ui.uiLayoutNineItem.at(i));
				_layout->removeAllChildren();
				_layout->setVisible(false);
			}
			else
			{
				auto _layout = dynamic_cast<Layout*>(m_ui.uiLayoutNineItem.at(i));
				_layout->removeAllChildren();
				_layout->setVisible(true);
				string pngName = "Image/UIGang/img_yun3.png";			
				auto button = Button::create(pngName,pngName,pngName,ui::Widget::TextureResType::LOCAL);
				button->setPosition(Vec2(_layout->getContentSize().width/2,_layout->getContentSize().height/2));
				button->addTouchEventListener(CC_CALLBACK_2(MyGroupActivity::onBtnClickByListId,this,i));
				_layout->addChild(button);
			}
		}
	}
	else if (publicInfo->showedIdflag.size()==0 && publicInfo->isXunlong ==0)
	{
		for (int i = 0; i < publicInfo->nineList.size(); i++)
		{
			bool bTemp = false;
			for (int j = 0; j< publicInfo->showedIdflag.size(); j++)
			{
				if (i == publicInfo->showedIdflag.at(j))
				{
					bTemp = true;
					break;
				}
			}
			if (bTemp)
			{
				auto _layout = dynamic_cast<Layout*>(m_ui.uiLayoutNineItem.at(i));
				_layout->removeAllChildren();
				_layout->setVisible(false);
			}
			else
			{
				auto temp = publicInfo->nineList.at(i);
				auto _layout = dynamic_cast<Layout*>(m_ui.uiLayoutNineItem.at(i));
				_layout->removeAllChildren();
				_layout->setVisible(true);

				UIToolHeader* _item = UIToolHeader::create(temp.at(0));
				_item->setAnchorPoint(Vec2(0,0));
				_item->setPosition(Vec2(0,0));
				_item->setScale(0.87f);
				_item->setNumEx(temp.at(1));
				_item->setTipsEnabled(true);
				_item->setName("_item"+TO_STR(i+1));
				_item->setToolId(temp.at(0));;
				_layout->addChild(_item);

				if (temp.at(0) == publicInfo->importantId)
				{
					ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniZhuangbeiEffe/AniZhuangbeiEffe.ExportJson");
					Armature* pArmature = Armature::create("AniZhuangbeiEffe");//名字需要和 .ExportJson一致

					pArmature->getAnimation()->playByIndex(0,-1,1);
					//设置动画精灵位置 
					pArmature->setPosition(Vec2(_item->getContentSize().width/2,_item->getContentSize().height/2));
					_item->addChild(pArmature,14, "AniZhuangbeiEffe");
				}

			}
		}
	}
	else if (publicInfo->showedIdflag.size()==0 && publicInfo->isXunlong ==1)
	{
		//不显示9个道具 显示九朵云 
		for (int i = 0; i < m_ui.uiLayoutNineItem.size(); i++)
		{
			auto _layout = dynamic_cast<Layout*>(m_ui.uiLayoutNineItem.at(i));
			_layout->removeAllChildren();
			_layout->setVisible(true);
			string pngName = "Image/UIGang/img_yun3.png";			
			auto button = Button::create(pngName,pngName,pngName,ui::Widget::TextureResType::LOCAL);
			button->setPosition(Vec2(_layout->getContentSize().width/2,_layout->getContentSize().height/2));
			button->addTouchEventListener(CC_CALLBACK_2(MyGroupActivity::onBtnClickByListId,this,i));
			_layout->addChild(button);
		}
	}
}

void MyGroupActivity::showHeroAni()
{

	int nResID = MyGroupModel::getInstance()->getGroupActivityInfo()->showHero;
	if (nResID != 0 && !pHero)
	{
		m_ui.uiLayoutshowHero->removeAllChildrenWithCleanup(true);
		pHero = SpineAnimation::createFighterWithResId(nResID);
		pHero->setPosition(Vec2(m_ui.uiLayoutshowHero->getContentSize().width/2, -50));
		pHero->setAnimation(0, "wait", true);
		m_ui.uiLayoutshowHero->addChild(pHero, 1, "HeroSpine");
	}
}

void MyGroupActivity::showThreeItem()
{
	auto showItem = MyGroupModel::getInstance()->getGroupActivityInfo()->showItem;

	//3个可获得物品
	for (int i = 0; i < showItem.size(); i++)
	{
		auto tempid = showItem.at(i);
		auto _layout = dynamic_cast<Layout*>(m_ui.uiLayoutshowItem.at(i));
		_layout->removeAllChildren();
		if (tempid != 0)
		{
			UIToolHeader* _item = UIToolHeader::create(tempid);
			_item->setAnchorPoint(Vec2(0,0));
			_item->setPosition(Vec2(0,0));
			_item->setScale(0.87f);
			_item->setNumEx(-1);//temp.nCount);
			_item->setTipsEnabled(true);
			_item->setToolId(tempid);
			_layout->addChild(_item);
		}

	}
}

void MyGroupActivity::playStartAnimation()
{
	if (pHero)
	{
		pHero->setAnimation(0,"atk", false);
		pHero->addAnimation(0, "wait", true);
	}
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_YunHaiXunLong_ZheGai/Ani_YunHaiXunLong_ZheGai.ExportJson");
	Armature* armature1 = Armature::create("Ani_YunHaiXunLong_ZheGai");
	
	armature1->getAnimation()->play("Animation1",-1,0);
	
	auto onResultAnimationCallFunc = [this](Armature *armature1, MovementEventType eventType, const std::string& strName)->void
	{
		if (eventType == MovementEventType::COMPLETE)
		{
			isRun = false;
			armature1->removeFromParent();
		}
	};
	armature1->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
	auto root = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Pnl_Dragon"));
	armature1->setPosition(Vec2(root->getContentSize().width/2,root->getContentSize().height/2));
	root->addChild(armature1,99);



	this->runAction(Sequence::create(DelayTime::create(0.5),CallFunc::create([=]()
	{
		for (int i = 0; i < TENITEM; i++)
		{
			auto _layout = dynamic_cast<Layout*>(m_ui.uiLayoutNineItem.at(i));
			_layout->removeAllChildren();
			_layout->setVisible(true);
			string pngName = "Image/UIGang/img_yun3.png";			
			auto button = Button::create(pngName,pngName,pngName,ui::Widget::TextureResType::LOCAL);
			button->setPosition(Vec2(_layout->getContentSize().width/2,_layout->getContentSize().height/2));
			button->addTouchEventListener(CC_CALLBACK_2(MyGroupActivity::onBtnClickByListId,this,i));
			_layout->addChild(button);
		}
	}),nullptr));
}

void MyGroupActivity::playItemOutAnimation()
{
	sActivityInfo* publicInfo = MyGroupModel::getInstance()->getGroupActivityInfo();
	if (publicInfo->actFind.id	!= 0)
	{
		UIToolHeader* _item = UIToolHeader::create(publicInfo->actFind.id);
		_item->setAnchorPoint(Vec2(0,0));
		_item->setPosition(Vec2(0,0));
		_item->setScale(0.87f);
		_item->setNumEx(publicInfo->actFind.count);
		_item->setTipsEnabled(false);
		_item->setToolId(publicInfo->actFind.id);
	

		Layout* parent =(Layout*) m_ui.uiLayoutNineItem.at(weizhi)->getParent();
		_item->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		auto po = m_ui.uiLayoutNineItem.at(weizhi)->getPosition();
		auto co = _item->getContentSize();
		_item->setPosition(Vec2(po.x+co.width/2,po.y+co.height/2));
		parent->addChild(_item,99);

		auto fadeout = FadeOut::create(1);
		auto scalet = ScaleTo::create(1,3);
		
		auto cafun = CallFunc::create([=]()
		{
			isRun = false;
			_item->removeFromParent();
		});

		auto sqa = Spawn::create(fadeout,scalet,nullptr);
		_item->runAction(Sequence::create(sqa,cafun,nullptr));
	}

}

void MyGroupActivity::playNineItemInAnimation()
{
	sActivityInfo* publicInfo = MyGroupModel::getInstance()->getGroupActivityInfo();
	for (int i = 0; i <publicInfo->nineList.size(); i++)
	{

		auto temp = publicInfo->nineList.at(i);
		auto _layout = dynamic_cast<Layout*>(m_ui.uiLayoutNineItem.at(i));
		_layout->removeAllChildren();
		_layout->setVisible(true);

		UIToolHeader* _item = UIToolHeader::create(temp.at(0));
		_item->setAnchorPoint(Vec2(0.5,0.5));
		_item->setPosition(Vec2(_layout->getContentSize().width/2,_layout->getContentSize().height/2));
		//_item->setScale(0.87f);
		_item->setName("_ite"+TO_STR(i+1));
		_item->setNumEx(temp.at(1));
		_item->setTipsEnabled(false);
		_item->setToolId(temp.at(0));;
		_layout->addChild(_item);

		if (temp.at(0) == publicInfo->importantId)
		{
			ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniZhuangbeiEffe/AniZhuangbeiEffe.ExportJson");
			Armature* pArmature = Armature::create("AniZhuangbeiEffe");//名字需要和 .ExportJson一致

			pArmature->getAnimation()->playByIndex(0,-1,1);
			//设置动画精灵位置 
			pArmature->setPosition(Vec2(_item->getContentSize().width/2,_item->getContentSize().height/2));
			_item->addChild(pArmature,14, "AniZhuangbeiEffe");
		}
	}

	auto callba = CallFunc::create([=]()
	{
		playStartAnimation();
		m_ui.uiBtnBegin->setEnabled(false);
		m_ui.uiBtnBegin->setVisible(false);
		m_ui.uiBtnRefresh->setVisible(true);
		m_ui.uiBtnRefresh->setEnabled(true);		
	});
	this->runAction(Sequence::create(DelayTime::create(1),callba,nullptr));

}

void MyGroupActivity::showRewardGetted()
{
	vector<PopRewardItem*> vecRewardItem;

	sActivityInfo* publicInfo = MyGroupModel::getInstance()->getGroupActivityInfo();
	auto gettedBox = publicInfo->actFind;

	if(gettedBox.id == goldId)
	{
		PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(gettedBox.count);
		vecRewardItem.push_back(rewardItem);
	}
	//银币
	else if(gettedBox.id == coinId)
	{
		PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(gettedBox.count);
		vecRewardItem.push_back(rewardItem);
	}
	else
	{
		PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(gettedBox.id, gettedBox.count);
		vecRewardItem.push_back(rewardItem);
	}
	
	PopReward::show(vecRewardItem);

}

void MyGroupActivity::updateOneItemAction()
{
	

	auto root =(Layout*) m_ui.uiLayoutNineItem.at(weizhi);
	
	root->removeAllChildren();

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_YunHaiXunLong/Ani_YunHaiXunLong.ExportJson");
	Armature* armature1 = Armature::create("Ani_YunHaiXunLong");

	armature1->getAnimation()->play("dianji",-1,0);

	auto onResultAnimationCallFunc = [this](Armature *armature1, MovementEventType eventType, const std::string& strName)->void
	{
		if (eventType == MovementEventType::COMPLETE)
		{
			sActivityInfo* publicInfo = MyGroupModel::getInstance()->getGroupActivityInfo();
			armature1->removeFromParent();
			if (publicInfo->actFind.id == publicInfo->importantId)
			{
				showRewardGetted();
				isRun = false;
			}
			else
			{
				playItemOutAnimation();
			}
		}
	};
	armature1->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);

	armature1->setPosition(Vec2(root->getContentSize().width/2,root->getContentSize().height/2));
	root->addChild(armature1,99);
}
