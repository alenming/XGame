#include "LimitShopChouLayer.h"
#include "Widget/LayerCommHeadInfo.h"
#include "Temp/CustomGoldTips.h"
#include "Widget/LayerCommHeadInfo.h"
#define  TIME_GRAP 0.15f

LimitShopChouLayer::LimitShopChouLayer():firstChou(false),_isFirstRun(true),isRunend(false)
{
	LimitTimeHeroModel::getInstance()->addObserver(this);
}

LimitShopChouLayer::~LimitShopChouLayer()
{

	LimitTimeHeroModel::getInstance()->removeObserver(this);
}

LimitShopChouLayer* LimitShopChouLayer::create( eChouKaType type )
{
	LimitShopChouLayer *pRet = new LimitShopChouLayer(); 
	if (pRet && pRet->init(type)) 
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

bool LimitShopChouLayer::init(eChouKaType type)
{
	if (!Layer::init())
	{
		return false;
	}
	m_type = type;
	firstChou = false;
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_TakeItem/Ani_TakeItem.ExportJson");
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_TakeItemss/Ani_TakeItemss.ExportJson");

	initUi();

	return true;
}

void LimitShopChouLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIxssj/UIxssj_point.ExportJson"));
	m_ui.ui_Root->setSize(size);
	m_ui.ui_Root->setPosition(pos);
	
	this->addChild(m_ui.ui_Root);

	auto root = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Root,"Panel_root"));
	root->setBackGroundImage("Image/Bg/img_bg_quanping.jpg",ui::Widget::TextureResType::LOCAL);

	auto takeCard = dynamic_cast<Layout*>(Helper::seekWidgetByName(root,"Pnl_Card"));
	takeCard->setVisible(false);


	m_ui.Panel_38 = dynamic_cast<Layout*>(Helper::seekWidgetByName(root,"Pnl_Box"));
	m_ui.Panel_38->setVisible(true);


	m_ui.Pnl_Once = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.Panel_38,"Pnl_Once"));
	m_ui.Pnl_Tenth = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.Panel_38,"Pnl_Tenth"));
	m_ui.Pnl_Once->setTouchEnabled(false);
	m_ui.Pnl_Tenth->setTouchEnabled(false);
	//一次
	m_ui.Btn_Once = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Panel_38,"Btn_Once"));
	m_ui.Img_Free = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Panel_38,"Img_Free"));
	m_ui.Img_Free->setVisible(false);

	m_ui.Img_Gold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Panel_38,"Img_Gold"));
	m_ui.Lab_GoldCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Panel_38,"Lab_GoldCost"));
	m_ui.Img_Gold->setVisible(false);
	m_ui.Lab_GoldCost->setVisible(false);

	m_ui.oneChou.Lab_Item = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Pnl_Once,"Lab_Item1"));
	m_ui.oneChou.Lab_Item->setString("");
	m_ui.oneChou.Pnl_Item = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.Pnl_Once,"Pnl_Item1"));
	
	//十次
	m_ui.Btn_Tenth = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Panel_38,"Btn_Tenth"));

	m_ui.Img_GoldT = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Btn_Tenth,"Img_Gold"));
	m_ui.Lab_GoldCostT = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Btn_Tenth,"Lab_GoldCost"));
	m_ui.Img_GoldT->setVisible(false);
	m_ui.Lab_GoldCostT->setVisible(false);



	for (int i = 1; i <= iItemNumTen; i++)
	{
		Limit_OneChou tep;

		tep.Lab_Item = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Pnl_Tenth,"Lab_Item"+TO_STR(i)));
		tep.Lab_Item ->setString("");
		tep.Pnl_Item = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.Pnl_Tenth,"Pnl_Item"+TO_STR(i)));
		m_ui.tenChou.push_back(tep);
	}

	m_ui.Btn_Tenth->addTouchEventListener(CC_CALLBACK_2(LimitShopChouLayer::onBtnClick,this,eChouKaType::eTYPE_TEN));
	m_ui.Btn_Once->addTouchEventListener(CC_CALLBACK_2(LimitShopChouLayer::onBtnClick,this,eChouKaType::eTYPE_ONE));
	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			this->scheduleUpdate();
		}
	};
	auto  _headInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_LIMIT_TIME_HERO);
	this->addChild(_headInfo);

}

void LimitShopChouLayer::onEnter()
{
	Layer::onEnter();
	if (_isFirstRun)
	{
		isRunend = true;
		_isFirstRun = true;
		//发送抽
		if (m_type== eTYPE_ONE)
		{
			updatePublicInfo();
			updateOneChou();
			firstChou = true;
		}
		else if (m_type== eTYPE_TEN)
		{
			updatePublicInfo();
			updateTenChou();
			firstChou = true;
		}
	}
}

void LimitShopChouLayer::update(float ft)
{
	//打开主办面抽卡接口
	LimitTimeHeroModel::getInstance()->setIsAddBuyLayer(false);
	LimitTimeHeroController::sendGetMainInfoMsg();
	this->removeFromParent();
}

void LimitShopChouLayer::onExit()
{
	Layer::onExit();
}

void LimitShopChouLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*)data;


	switch (observerParam->id)
	{
		case nMAIN_CMD_LIMITTIMEHERO_CARD:
		{	
			if (firstChou)
			{
				auto pInstance = LimitTimeHeroModel::getInstance();
				if(pInstance->getBuyItemParam().size() == iItemNumOne)
				{
					updatePublicInfo();
					updateOneChou();
				}
				else if(pInstance->getBuyItemParam().size() == iItemNumTen)
				{
					updatePublicInfo();
					updateTenChou();
				}
			}
		}
	default:
		break;
	}
}

void  LimitShopChouLayer::onBtnClick( Ref* reff, Widget::TouchEventType type, eChouKaType chouType)
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		if (isRunend)
		{
			return;
		}
		isRunend = true;
		sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
		if(actData == nullptr || actData->actState != eSTATE_OPEN)
		{
			return;		//活动结束禁止抽卡
		}

		auto mainGold = MainModel::getInstance()->getMainParam()->mGold;

		if(chouType ==eTYPE_ONE && LimitTimeHeroModel::getInstance()->isCanBuyFree())
		{
			//发送抽
			LimitTimeHeroController::sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_ONE);
			return;
		}


		if (chouType == eTYPE_ONE &&mainGold <  LimitTimeHeroModel::getInstance()->getCost(eTYPE_ONE))
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				isRunend = false;
				Director::getInstance()->getRunningScene()->addChild(_tips);
				return;
			}
		}
		else if (chouType == eTYPE_TEN &&mainGold < LimitTimeHeroModel::getInstance()->getCost(eTYPE_TEN))
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				isRunend = false;
				Director::getInstance()->getRunningScene()->addChild(_tips);
				return;
			}
		}


		if (chouType == eTYPE_ONE && mainGold >= LimitTimeHeroModel::getInstance()->getCost(eTYPE_ONE))
		{
			m_type = chouType;
			//发送抽
			LimitTimeHeroController::sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_ONE);
		}
		else if (chouType == eTYPE_TEN && mainGold >= LimitTimeHeroModel::getInstance()->getCost(eTYPE_TEN))
		{			
			m_type = chouType;
			//发送抽
			LimitTimeHeroController::sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_TEN);
		}
	}
}

void LimitShopChouLayer::updateOneChou()
{
	m_ui.Pnl_Once->setVisible(true);
	m_ui.Pnl_Tenth->setVisible(false);
	m_ui.oneChou.Pnl_Item->removeAllChildren();
	m_ui.oneChou.Lab_Item->setVisible(false);
	setTextAddStroke(m_ui.oneChou.Lab_Item, Color3B(0x79, 0x03, 0x03),2);

	//获得物品
	auto temp = LimitTimeHeroModel::getInstance()->getBuyItemParam().at(0);
	UIToolHeader* _item = UIToolHeader::create(temp.nId);
	auto labSize = m_ui.oneChou.Pnl_Item->getContentSize();
	_item->setVisible(false);
	_item->setPosition(Vec2(labSize.width/2,labSize.height/2));
	_item->setNumEx(temp.nCount);
	_item->setTipsEnabled(true);
	_item->setToolId(temp.nId);
	m_ui.oneChou.Pnl_Item->removeAllChildren();
	m_ui.oneChou.Pnl_Item->addChild(_item);

	m_ui.oneChou.Lab_Item->setString(temp.strName);

	m_ui.oneChou.Lab_Item->setFontName(FONT_FZZHENGHEI);
	m_ui.oneChou.Lab_Item->setColor(Color3B(0xff, 0xff, 0xff));
	setTextAddStroke(m_ui.oneChou.Lab_Item, Color3B(0x79, 0x03, 0x03),2);

	auto callfunc = CallFunc::create([=]()
	{
		Armature* armature = Armature::create("Ani_TakeItem");
		armature->getAnimation()->play("TakeItemone",-1,0);
		auto onResultAnimationCallFunc = [=](Armature *armature, MovementEventType eventType, const std::string& strName)->void
		{
			if (eventType == MovementEventType::COMPLETE)
			{
				GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ITEM_SHOP_DONE);
				armature->removeFromParent();
			}
		};
		armature->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
		armature->setPosition(Vec2(m_ui.oneChou.Pnl_Item->getContentSize().width/2,0));
		m_ui.oneChou.Pnl_Item->addChild(armature,-1);
	});
	auto callfunc1 = CallFunc::create([=]()
	{
		isRunend = false;
		_item->setVisible(true);
		_item->setScale(0.87f);
		if (_item->getParent())
		{
			if (_item->getParent()->getParent())
			{
				_item->getParent()->getParent()->getChildByName("Lab_Item1")->setVisible(true);
			}
		}
	
		_item->setTipsEnabled(true);
		Armature* armature1 = Armature::create("Ani_TakeItem");
		armature1->getAnimation()->play("TakeItem_G",-1,1);
		armature1->setPosition(Vec2(m_ui.oneChou.Pnl_Item->getContentSize().width/2,0));
		m_ui.oneChou.Pnl_Item->addChild(armature1,-1);

	});
	auto delaytime = DelayTime::create(0.35f);
	m_ui.oneChou.Pnl_Item->runAction(Spawn::create(callfunc,Sequence::create(delaytime,callfunc1,nullptr),nullptr));

}

void LimitShopChouLayer::updateTenChou()
{
	//获得物品
	m_ui.Pnl_Once->setVisible(false);
	m_ui.Pnl_Tenth->setVisible(true);

	for (int i = 0; i < m_ui.tenChou.size(); i++)
	{
		m_ui.tenChou.at(i).Pnl_Item->removeAllChildren();
		m_ui.tenChou.at(i).Lab_Item->setVisible(false);
	}

	auto labSize = 	m_ui.tenChou.at(0).Pnl_Item->getContentSize();
	auto ChouPram =  LimitTimeHeroModel::getInstance()->getBuyItemParam();

	for (int i = 0; i < m_ui.tenChou.size(); i++)
	{
		auto oneChouPram = ChouPram.at(i);
		UIToolHeader* _item = UIToolHeader::create(oneChouPram.nId);
		_item->setVisible(false);
		_item->setPosition(Vec2(labSize.width/2,labSize.height/2));
		_item->setNumEx(oneChouPram.nCount);
		_item->setTipsEnabled(false);
		_item->setToolId(oneChouPram.nId);
		m_ui.tenChou.at(i).Pnl_Item->removeAllChildren();
		m_ui.tenChou.at(i).Pnl_Item->addChild(_item);

		m_ui.tenChou.at(i).Lab_Item->setString(oneChouPram.strName);

		m_ui.tenChou.at(i).Lab_Item->setFontName(FONT_FZZHENGHEI);
		m_ui.tenChou.at(i).Lab_Item->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.tenChou.at(i).Lab_Item, Color3B(0x79, 0x03, 0x03),2);
		playingOneItemAnimation(_item, i);
	}
}

void LimitShopChouLayer::updatePublicInfo()
{
	
	bool isfree = LimitTimeHeroModel::getInstance()->isCanBuyFree();

	if (isfree)//免费
	{
		m_ui.Img_Free->setVisible(true);

		m_ui.Img_Gold->setVisible(false);
		m_ui.Lab_GoldCost->setVisible(false);
	}
	else
	{
		m_ui.Img_Free->setVisible(false);
		m_ui.Img_Gold->setVisible(true);
		m_ui.Lab_GoldCost->setVisible(true);

		m_ui.Lab_GoldCost->setString(TO_STR(LimitTimeHeroModel::getInstance()->getCost(eTYPE_ONE)));
		m_ui.Lab_GoldCost->setFontName(FONT_FZZHENGHEI);
		m_ui.Lab_GoldCost->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.Lab_GoldCost, Color3B(0x00, 0x00, 0x00),2);
	}

	m_ui.Img_GoldT->setVisible(true);
	m_ui.Lab_GoldCostT->setVisible(true);
	m_ui.Lab_GoldCostT->setString(TO_STR(LimitTimeHeroModel::getInstance()->getCost(eTYPE_TEN)));
	m_ui.Lab_GoldCostT->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_GoldCostT->setColor(Color3B(0xff, 0xff, 0xff));
	setTextAddStroke(m_ui.Lab_GoldCostT, Color3B(0x00, 0x00, 0x00),2);
}

void LimitShopChouLayer::playingOneItemAnimation( UIToolHeader* item ,int index)
{
	auto temp = m_ui.tenChou.at(index);
	auto callfunc = CallFunc::create([=]()
	{
		Armature* armature = Armature::create("Ani_TakeItem");
		armature->getAnimation()->play("TakeItemone",-1,0);
		auto onResultAnimationCallFunc = [=](Armature *armature, MovementEventType eventType, const std::string& strName)->void
		{
			if (eventType == MovementEventType::COMPLETE)
			{
				if (index == 7)
				{
					isRunend = false;
				}
				armature->removeFromParent();
			}
		};
		armature->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
		armature->setPosition(Vec2(temp.Pnl_Item->getContentSize().width/2,0));
		temp.Pnl_Item->addChild(armature,-1);
	});
	auto callfunc1 = CallFunc::create([=]()
	{
		item->setVisible(true);
		item->setScale(0.87f);
		if (item->getParent())
		{
			if (item->getParent()->getParent())
			{
				item->getParent()->getParent()->getChildByName("Lab_Item"+TO_STR(index+1))->setVisible(true);
			}
		}
		
		item->setTipsEnabled(true);
		Armature* armature1 = Armature::create("Ani_TakeItem");
		armature1->getAnimation()->play("TakeItem_G",-1,1);
		armature1->setPosition(Vec2(temp.Pnl_Item->getContentSize().width/2,0));
		temp.Pnl_Item->addChild(armature1,-1);

	});
	auto delaytime = DelayTime::create(0.35f);
	item->runAction(Sequence::create(DelayTime::create(index*TIME_GRAP),Spawn::create(callfunc,Sequence::create(delaytime,callfunc1,nullptr),nullptr),nullptr));
}
