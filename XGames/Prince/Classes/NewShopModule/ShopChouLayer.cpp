#include "ShopChouLayer.h"
#include "Widget/LayerCommHeadInfo.h"
#include "ShopScene.h"
#include "Temp/CustomGoldTips.h"

#define TIME_GRAP 0.15f

ShopChouLayer::ShopChouLayer():firstChou(false),_isFirstRun(true),isRun(false)
{
	ShopModel::getInstance()->addObserver(this);
}

ShopChouLayer::~ShopChouLayer()
{

	ShopModel::getInstance()->removeObserver(this);
}

ShopChouLayer* ShopChouLayer::create( CHOU_TYPE type )
{
	ShopChouLayer *pRet = new ShopChouLayer(); 
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

bool ShopChouLayer::init(CHOU_TYPE type)
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

void ShopChouLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIShopexc2/UIShopBox.ExportJson"));
	m_ui.ui_Root->setSize(size);
	m_ui.ui_Root->setPosition(pos);
	m_ui.ui_Root->setBackGroundImage("Image/Bg/img_bg_quanping.jpg",ui::Widget::TextureResType::LOCAL);
	this->addChild(m_ui.ui_Root);

	m_ui.Panel_38 = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Root,"Panel_38"));

	m_ui.Pnl_Once = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Root,"Pnl_Once"));
	m_ui.Pnl_Tenth = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Root,"Pnl_Tenth"));

	//一次
	m_ui.Btn_Once = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Btn_Once"));
	m_ui.Img_Free = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Img_Free"));
	m_ui.Img_Free->setVisible(false);

	m_ui.Img_Gold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Btn_Once,"Img_Gold"));
	m_ui.Lab_GoldCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Btn_Once,"Lab_GoldCost"));
	m_ui.Img_Gold->setVisible(false);
	m_ui.Lab_GoldCost->setVisible(false);

	m_ui.Img_Key = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Btn_Once,"Img_Key"));
	m_ui.Lab_KeyCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Btn_Once,"Lab_KeyCost"));
	m_ui.Img_Key->setVisible(false);
	m_ui.Lab_KeyCost->setVisible(false);

	m_ui.oneChou.Lab_Item = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Pnl_Once,"Lab_Item1"));
	m_ui.oneChou.Lab_Item->setString("");
	m_ui.oneChou.Pnl_Item = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.Pnl_Once,"Pnl_Item1"));
	
	//十次
	m_ui.Btn_Tenth = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Btn_Tenth"));

	m_ui.Img_GoldT = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Btn_Tenth,"Img_Gold"));
	m_ui.Lab_GoldCostT = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Btn_Tenth,"Lab_GoldCost"));
	m_ui.Img_GoldT->setVisible(false);
	m_ui.Lab_GoldCostT->setVisible(false);

	m_ui.Img_KeyT= dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Btn_Tenth,"Img_Key"));
	m_ui.Lab_KeyCostT = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Btn_Tenth,"Lab_KeyCost"));
	m_ui.Img_KeyT->setVisible(false);
	m_ui.Lab_KeyCostT->setVisible(false);

	for (int i = 1; i <= TENCHOU; i++)
	{
		UI_OneChou tep;

		tep.Lab_Item = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Pnl_Tenth,"Lab_Item"+TO_STR(i)));
		tep.Lab_Item ->setString("");
		tep.Pnl_Item = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.Pnl_Tenth,"Pnl_Item"+TO_STR(i)));
		m_ui.tenChou.push_back(tep);
	}

	m_ui.Btn_Tenth->addTouchEventListener(CC_CALLBACK_2(ShopChouLayer::onBtnClick,this,CHOU_TYPE::TEN_CHOU));
	m_ui.Btn_Once->addTouchEventListener(CC_CALLBACK_2(ShopChouLayer::onBtnClick,this,CHOU_TYPE::ONE_CHOU));

	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			ShopModel::getInstance()->setFirstChou(false);
			ShopModel::getInstance()->setShopType(ST_NONE);
			Director::getInstance()->popScene();
		}
	};
	auto _headInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_MARKET);
	this->addChild(_headInfo);

}


void ShopChouLayer::onEnter()
{
	Layer::onEnter();
	if (_isFirstRun)
	{
		_isFirstRun = false;
		isRun = true;

		//发送抽
		if (m_type== SHOP_EVENT_TYEP_ONE_CHOU)
		{
			updatePublicInfo();
			updateOneChou();
			firstChou = true;
		}
		else if (m_type== SHOP_EVENT_TYEP_TEN_CHOU)
		{
			updatePublicInfo();
			updateTenChou();
			firstChou = true;
		}
	}
}

void ShopChouLayer::updateNextFram(float ft)
{
}

void ShopChouLayer::onExit()
{
	Layer::onExit();
}

void ShopChouLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*)data;


	switch (observerParam->id)
	{
		case SHOP_EVENT_TYEP_ONE_CHOU:
		{	
			if (firstChou)
			{
				updatePublicInfo();
				updateOneChou();
			}
			break;
		}
		case SHOP_EVENT_TYEP_TEN_CHOU:
		{
			if (firstChou)
			{
				updatePublicInfo();
				updateTenChou();
			}
			break;
		}
	default:
		break;
	}
}

void  ShopChouLayer::onBtnClick( Ref* reff, Widget::TouchEventType type, CHOU_TYPE chouType)
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
		CCLOG("%d",isRun?1:0);
		isRun = true;
		auto oneper = ShopModel::getInstance()->getOneChouPrice();
		auto tenper = ShopModel::getInstance()->getTenChouPrice();
		auto mainGold = MainModel::getInstance()->getMainParam()->mGold;
		//免费
		if(chouType ==ONE_CHOU && ShopModel::getInstance()->getIsFree())
		{
			ShopController::getInstance()->handleChouOneOrTen(chouType);
			return;
		}
		//钥匙大于0
		if(chouType ==ONE_CHOU && ShopModel::getInstance()->getFreshCount()>0)
		{
			ShopController::getInstance()->handleChouOneOrTen(chouType);
			return;
		}
		//钥匙>=10
		if(chouType ==TEN_CHOU && ShopModel::getInstance()->getFreshCount()>=TENCHOU)
		{
			ShopController::getInstance()->handleChouOneOrTen(chouType);
			return;
		}
		//单抽不够钱
		if (chouType == ONE_CHOU &&mainGold <  oneper)
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				isRun = false;
				Director::getInstance()->getRunningScene()->addChild(_tips);
				return;
			}
		}
		//10抽不够钱
		if (chouType == TEN_CHOU &&mainGold < tenper)
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				isRun = false;
				Director::getInstance()->getRunningScene()->addChild(_tips);
				return;
			}
		}
		//单抽
		if (chouType == ONE_CHOU && mainGold >= oneper)
		{
			m_type = chouType;
			ShopController::getInstance()->handleChouOneOrTen(m_type);
		}
		//10抽
		if (chouType == TEN_CHOU && mainGold >= tenper)
		{			
			m_type = chouType;
			ShopController::getInstance()->handleChouOneOrTen(m_type);
		}

	}
}

void ShopChouLayer::updateOneChou()
{
	m_ui.Pnl_Once->setVisible(true);
	m_ui.Pnl_Tenth->setVisible(false);
	m_ui.oneChou.Pnl_Item->removeAllChildren();
	m_ui.oneChou.Lab_Item->setVisible(false);
	setTextAddStroke(m_ui.oneChou.Lab_Item, Color3B(0x79, 0x03, 0x03),2);

	//获得物品
	auto temp = ShopModel::getInstance()->m_ChouPram.at(0);
	UIToolHeader* _item = UIToolHeader::create(temp.nId);
	auto labSize = m_ui.oneChou.Pnl_Item->getContentSize();
	_item->setVisible(false);
	_item->setPosition(Vec2(labSize.width/2,labSize.height/2));
	_item->setNumEx(temp.nCount);
	_item->setTipsEnabled(true);
	_item->setToolId(temp.nId);
	_item->showLightEff();
	m_ui.oneChou.Pnl_Item->removeAllChildren();
	m_ui.oneChou.Pnl_Item->addChild(_item);

	auto ToolData = DataManager::getInstance()->searchToolById(temp.nId);
	m_ui.oneChou.Lab_Item->setString(ToolData->getStringData("itemName"));

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
		isRun = false;
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

void ShopChouLayer::updateTenChou()
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
	auto ChouPram = ShopModel::getInstance()->m_ChouPram;
	m_Tool.clear();
	for (int i = 0; i < m_ui.tenChou.size(); i++)
	{
		auto oneChouPram = ShopModel::getInstance()->m_ChouPram.at(i);
		UIToolHeader* _item = UIToolHeader::create(oneChouPram.nId);
		_item->setVisible(false);
		_item->setPosition(Vec2(labSize.width/2,labSize.height/2));
		_item->setNumEx(oneChouPram.nCount);
		_item->setTipsEnabled(false);
		_item->setToolId(oneChouPram.nId);
		_item->showLightEff();
		m_ui.tenChou.at(i).Pnl_Item->removeAllChildren();
		m_ui.tenChou.at(i).Pnl_Item->addChild(_item);

		m_Tool.push_back(_item);
		auto ToolData = DataManager::getInstance()->searchToolById(oneChouPram.nId);
		m_ui.tenChou.at(i).Lab_Item->setString(ToolData->getStringData("itemName"));

		m_ui.tenChou.at(i).Lab_Item->setFontName(FONT_FZZHENGHEI);
		m_ui.tenChou.at(i).Lab_Item->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.tenChou.at(i).Lab_Item, Color3B(0x79, 0x03, 0x03),2);
		playingOneItemAnimation(_item, i);
	}
}

void ShopChouLayer::updatePublicInfo()
{
	
	int freshcount = ShopModel::getInstance()->getFreshCount();
	bool isfree = ShopModel::getInstance()->getIsFree();

	if (isfree)//免费
	{
		m_ui.Img_Free->setVisible(true);
		m_ui.Img_Key->setVisible(false);
		m_ui.Lab_KeyCost->setVisible(false);
		m_ui.Img_Gold->setVisible(false);
		m_ui.Lab_GoldCost->setVisible(false);

	}
	else if (!isfree && freshcount>0)
	{
		m_ui.Img_Free->setVisible(false);
		m_ui.Img_Key->setVisible(true);
		m_ui.Lab_KeyCost->setVisible(true);
		m_ui.Lab_KeyCost->setString(TO_STR(freshcount)+"/1");
		m_ui.Lab_KeyCost->setFontName(FONT_FZZHENGHEI);
		m_ui.Lab_KeyCost->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.Lab_KeyCost, Color3B(0x00, 0x00, 0x00),2);

		m_ui.Img_Gold->setVisible(false);
		m_ui.Lab_GoldCost->setVisible(false);

	}
	else if (!isfree && freshcount<=0)
	{
		m_ui.Img_Free->setVisible(false);
		m_ui.Img_Key->setVisible(false);
		m_ui.Lab_KeyCost->setVisible(false);

		m_ui.Img_Gold->setVisible(true);
		m_ui.Lab_GoldCost->setVisible(true);
		m_ui.Lab_GoldCost->setString(TO_STR(ShopModel::getInstance()->getOneChouPrice()));
		m_ui.Lab_GoldCost->setFontName(FONT_FZZHENGHEI);
		m_ui.Lab_GoldCost->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.Lab_GoldCost, Color3B(0x00, 0x00, 0x00),2);
	}


	if (freshcount>=TENCHOU)//有十张刷新令
	{
		m_ui.Img_KeyT->setVisible(true);
		m_ui.Lab_KeyCostT->setVisible(true);
		m_ui.Lab_KeyCostT->setString(TO_STR(freshcount)+"/10");
		m_ui.Lab_KeyCostT->setFontName(FONT_FZZHENGHEI);
		m_ui.Lab_KeyCostT->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.Lab_KeyCostT, Color3B(0x00, 0x00, 0x00),2);

		m_ui.Img_GoldT->setVisible(false);
		m_ui.Lab_GoldCostT->setVisible(false);

	}
	else if ( freshcount<TENCHOU)
	{
		m_ui.Img_KeyT->setVisible(false);
		m_ui.Lab_KeyCostT->setVisible(false);

		m_ui.Img_GoldT->setVisible(true);
		m_ui.Lab_GoldCostT->setVisible(true);
		m_ui.Lab_GoldCostT->setString(TO_STR(ShopModel::getInstance()->getTenChouPrice()));
		m_ui.Lab_GoldCostT->setFontName(FONT_FZZHENGHEI);
		m_ui.Lab_GoldCostT->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.Lab_GoldCostT, Color3B(0x00, 0x00, 0x00),2);
	}

}

void ShopChouLayer::playingOneItemAnimation( UIToolHeader* item ,int index)
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
		if (index == 5)
		{
			isRun = false;
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
