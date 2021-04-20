#include "StarsCampStoneLayer.h"
#include "StarsCampSceneOne.h"
#include "Temp/CustomGoldTips.h"

StarsCampStoneLayer::StarsCampStoneLayer():curYe(0),curSto(0)
{
	StarsCampModel::getInstance()->addObserver(this);
}

StarsCampStoneLayer::~StarsCampStoneLayer()
{

	StarsCampModel::getInstance()->removeObserver(this);
}

bool StarsCampStoneLayer::init()
{
	if(!Layer::init())
	{
		return false;
	}

	initUi();

	return true;
}

void StarsCampStoneLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIAstrolabe2/UIAstrolabe2_5.ExportJson"));

	auto son = dynamic_cast<Layout*>(Helper::seekWidgetByName( m_ui.ui_Root,"Panel_Jewelry_store"));
	son->setBackGroundImage("Image/Bg/img_bg_quanping.jpg",ui::Widget::TextureResType::LOCAL);
	m_ui.ui_Root->setContentSize(size);
	m_ui.ui_Root->setPosition(pos);
	this->addChild(m_ui.ui_Root);

	m_ui.Button_Jewel_Earth = dynamic_cast<Button*>(Helper::seekWidgetByName( m_ui.ui_Root,"Button_Jewel_Earth"));
	m_ui.Button_Jewel_Wood = dynamic_cast<Button*>(Helper::seekWidgetByName( m_ui.ui_Root,"Button_Jewel_Wood"));
	m_ui.Button_Jewel_Water = dynamic_cast<Button*>(Helper::seekWidgetByName( m_ui.ui_Root,"Button_Jewel_Water"));
	m_ui.Button_Jewel_Fire = dynamic_cast<Button*>(Helper::seekWidgetByName( m_ui.ui_Root,"Button_Jewel_Fire"));
	m_ui.Button_Jewel_Gold = dynamic_cast<Button*>(Helper::seekWidgetByName( m_ui.ui_Root,"Button_Jewel_Gold"));
	m_ui.Button_Jewel_Earth->setVisible(false);
	m_ui.Button_Jewel_Wood->setVisible(false);
	m_ui.Button_Jewel_Water->setVisible(false);
	m_ui.Button_Jewel_Fire->setVisible(false);
	m_ui.Button_Jewel_Gold->setVisible(false);
	fiveButton.push_back(m_ui.Button_Jewel_Earth);
	fiveButton.push_back(m_ui.Button_Jewel_Wood);
	fiveButton.push_back(m_ui.Button_Jewel_Water);
	fiveButton.push_back(m_ui.Button_Jewel_Fire);
	fiveButton.push_back(m_ui.Button_Jewel_Gold);

	m_ui.Image_Lock_35 = dynamic_cast<ImageView*>(Helper::seekWidgetByName( m_ui.ui_Root,"Image_Lock_35"));
	m_ui.Image_Lock_45 = dynamic_cast<ImageView*>(Helper::seekWidgetByName( m_ui.ui_Root,"Image_Lock_45"));
	m_ui.Image_Lock_55 = dynamic_cast<ImageView*>(Helper::seekWidgetByName( m_ui.ui_Root,"Image_Lock_55"));
	m_ui.Image_Lock_65 = dynamic_cast<ImageView*>(Helper::seekWidgetByName( m_ui.ui_Root,"Image_Lock_65"));
	m_ui.Image_Lock_75 = dynamic_cast<ImageView*>(Helper::seekWidgetByName( m_ui.ui_Root,"Image_Lock_75"));

	m_ui.Label_Lv35 = dynamic_cast<Text*>(Helper::seekWidgetByName( m_ui.ui_Root,"Label_Lv35"));
	m_ui.Label_Lv45 = dynamic_cast<Text*>(Helper::seekWidgetByName( m_ui.ui_Root,"Label_Lv45"));
	m_ui.Label_Lv55 = dynamic_cast<Text*>(Helper::seekWidgetByName( m_ui.ui_Root,"Label_Lv55"));
	m_ui.Label_Lv65 = dynamic_cast<Text*>(Helper::seekWidgetByName( m_ui.ui_Root,"Label_Lv65"));
	m_ui.Label_Lv75 = dynamic_cast<Text*>(Helper::seekWidgetByName( m_ui.ui_Root,"Label_Lv75"));

	m_ui.Panel_Jewels = dynamic_cast<Layout*>(Helper::seekWidgetByName( m_ui.ui_Root,"Panel_Jewels"));

	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
//			StarsCampController::getInstance()->sendGetStarsCampInfo();
			Director::getInstance()->popScene();
		}
	};
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_STARSTONE);
	this->addChild(_layerCommHeadInfo);

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AinXingPanBaoShi/AinXingPanBaoShi.ExportJson");

	initShopFace();
	updateAllInfo();
	//显示第一张
	setWhichShow();
}

void StarsCampStoneLayer::onEnter()
{
	Layer::onEnter();

}

void StarsCampStoneLayer::onExit()
{
	Layer::onExit();
}
void StarsCampStoneLayer::initShopFace()
{
	m_StoUI.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()
		->widgetFromJsonFile("Image/UIAstrolabe2/UIAstrolabe2_Jewel.ExportJson"));

	int _cleckHero = StarsCampModel::getInstance()->getCleckHero();
	auto heroInfo  = StarsCampModel::getInstance()->getOndeHeroInfoByWz(_cleckHero);

	auto lsize = m_ui.Panel_Jewels->getContentSize();
	fourLayou.clear();

	for (int ye = 1; ye < 6; ye++)
	{
		Layout* sonLayout = Layout::create();
		sonLayout->setContentSize(lsize);
		for (int ge = 1; ge < 5; ge++)
		{
			auto oneSTo = dynamic_cast<Layout*>(m_StoUI.ui_Root->clone());

			if (initOneStone(oneSTo,ge, ye-1))
			{
				curYe = ye-1;
			}
			
			oneSTo->setAnchorPoint(Vec2(0,0));
			oneSTo->setPosition(Vec2(lsize.width/4*(ge-1),0));
			sonLayout->addChild(oneSTo);
			oneFace.push_back(oneSTo);
		}
		sonLayout->setAnchorPoint(Vec2(0,0));
		sonLayout->setPosition(Vec2(0,0));
		m_ui.Panel_Jewels->addChild(sonLayout);
		fourLayou.push_back(sonLayout);
	}

}

void StarsCampStoneLayer::updateAllInfo()
{
	int mainLevel  = HeroModel::getInstance()->getMainHeroLevel();
	int _cleckHero = StarsCampModel::getInstance()->getCleckHero();
	auto heroInfo  = StarsCampModel::getInstance()->getOndeHeroInfoByWz(_cleckHero);

	m_ui.Label_Lv35->setFontName(FONT_FZZHENGHEI);
	m_ui.Label_Lv45->setFontName(FONT_FZZHENGHEI);
	m_ui.Label_Lv55->setFontName(FONT_FZZHENGHEI);
	m_ui.Label_Lv65->setFontName(FONT_FZZHENGHEI);
	m_ui.Label_Lv75->setFontName(FONT_FZZHENGHEI);

	if (mainLevel>=35)
	{
		m_ui.Image_Lock_35 ->setVisible(false);
		m_ui.Button_Jewel_Earth->setVisible(true);
		m_ui.Button_Jewel_Earth->addTouchEventListener(CC_CALLBACK_2(StarsCampStoneLayer::onBtnClick,this,STO_LAYBA::STO_LAYBA_FANYE,0,0));
	}
	if (mainLevel>=45)
	{
		m_ui.Image_Lock_45 ->setVisible(false);
		m_ui.Button_Jewel_Wood->setVisible(true);
		m_ui.Button_Jewel_Wood->addTouchEventListener(CC_CALLBACK_2(StarsCampStoneLayer::onBtnClick,this,STO_LAYBA::STO_LAYBA_FANYE,1,0));
	}
	if (mainLevel>=55)
	{
		m_ui.Image_Lock_55 ->setVisible(false);
		m_ui.Button_Jewel_Water->setVisible(true);
		m_ui.Button_Jewel_Water->addTouchEventListener(CC_CALLBACK_2(StarsCampStoneLayer::onBtnClick,this,STO_LAYBA::STO_LAYBA_FANYE,2,0));
	}
	if (mainLevel>=65)
	{
		m_ui.Image_Lock_65 ->setVisible(false);
		m_ui.Button_Jewel_Fire->setVisible(true);
		m_ui.Button_Jewel_Fire->addTouchEventListener(CC_CALLBACK_2(StarsCampStoneLayer::onBtnClick,this,STO_LAYBA::STO_LAYBA_FANYE,3,0));
	}
	if (mainLevel>=75)
	{
		m_ui.Image_Lock_75 ->setVisible(false);
		m_ui.Button_Jewel_Gold->setVisible(true);
		m_ui.Button_Jewel_Gold->addTouchEventListener(CC_CALLBACK_2(StarsCampStoneLayer::onBtnClick,this,STO_LAYBA::STO_LAYBA_FANYE,4,0));
	}

}

bool StarsCampStoneLayer::initOneStone(Layout* lay, int ge, int ye)
{
	bool tempCur = false;
	int _cleckHero = StarsCampModel::getInstance()->getCleckHero();
	auto heroInfo  = StarsCampModel::getInstance()->getOndeHeroInfoByWz(_cleckHero);
	int heroId = heroInfo.uId;
	int SearchID =  heroId*10000+(350+100*(ye)+ge);

	auto data = DataManager::getInstance()->searchAttbitureIntroduceById(SearchID);

	//石头图片
	auto Panel_Jewel = dynamic_cast<Layout*>(Helper::seekWidgetByName( lay,"Panel_Jewel"));
	auto stone = ImageView::create(ResourceUtils::getStarCampPathByStr(data->getStringData("icon")));
	stone->setPosition(Vec2(Panel_Jewel->getContentSize().width/2,Panel_Jewel->getContentSize().height/2));
	Panel_Jewel->addChild(stone);
	//名字
	auto stoName =  dynamic_cast<Text*>(Helper::seekWidgetByName(lay,"Label_Titel"));
	stoName->setFontName(FONT_FZZHENGHEI);
	stoName->setString(data->getStringData("name"));
	setTextAddStroke(stoName, Color3B(0x79, 0x03, 0x03),2);
	/************************************************************************/
	/*                         4条属性                                      */
	/************************************************************************/
	auto Label_power = dynamic_cast<Text*>(Helper::seekWidgetByName( lay,"Label_Name_"+_TO_STR(1)));
	Label_power->setFontName(FONT_FZZHENGHEI);
	auto Label_Power_number_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(lay,"Label_Power_"+_TO_STR(1)));
	Label_Power_number_1->setFontName(FONT_FZZHENGHEI);
	Label_Power_number_1->setString("+"+TO_STR(data->getLongData("atk")));
	
	Label_power = dynamic_cast<Text*>(Helper::seekWidgetByName( lay,"Label_Name_"+_TO_STR(2)));
	Label_power->setFontName(FONT_FZZHENGHEI);
	Label_Power_number_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(lay,"Label_Power_"+_TO_STR(2)));
	Label_Power_number_1->setFontName(FONT_FZZHENGHEI);
	Label_Power_number_1->setString("+"+TO_STR(data->getLongData("hp")));

	Label_power = dynamic_cast<Text*>(Helper::seekWidgetByName( lay,"Label_Name_"+_TO_STR(3)));
	Label_power->setFontName(FONT_FZZHENGHEI);
	Label_Power_number_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(lay,"Label_Power_"+_TO_STR(3)));
	Label_Power_number_1->setFontName(FONT_FZZHENGHEI);
	Label_Power_number_1->setString("+"+TO_STR(data->getLongData("pdd")));

	Label_power = dynamic_cast<Text*>(Helper::seekWidgetByName( lay,"Label_Name_"+_TO_STR(4)));
	Label_power->setFontName(FONT_FZZHENGHEI);
	Label_Power_number_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(lay,"Label_Power_"+_TO_STR(4)));
	Label_Power_number_1->setFontName(FONT_FZZHENGHEI);
	Label_Power_number_1->setString("+"+TO_STR(data->getLongData("mdd")));
	//金钱
	auto Image_Coin =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(lay,"Image_Coin"));
	if (data->getIntData("moneyType") == 2)
	{
		auto gold = ImageView::create("Image/Icon/global/img_yuanbao.png");
		gold->setAnchorPoint(Vec2(0,0));
		Image_Coin->addChild(gold);
	}
	auto Label_Number =  dynamic_cast<Text*>(Helper::seekWidgetByName(lay,"Label_Number"));
	Label_Number->setFontName(FONT_FZZHENGHEI);
	Label_Number->setString(TO_STR(data->getLongData("price")));


	auto Button_Buy =  dynamic_cast<Button*>(Helper::seekWidgetByName(lay,"Button_Buy"));
	auto Button_Set =  dynamic_cast<Button*>(Helper::seekWidgetByName(lay,"Button_Set"));
	auto Image_Setting =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(lay,"Image_Setting"));
	auto Image_Bracket =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(lay,"Image_Bracket"));
	Image_Bracket->setVisible(false);
	//显示已经购买
	for (unsigned int i = 0; i < heroInfo.haveStroneId.size(); i++)
	{
		auto stoid = heroInfo.haveStroneId.at(i);
		if (stoid == SearchID)
		{
			Button_Buy->setVisible(false);
			Button_Set->setVisible(true);
			Image_Setting->setVisible(false);
		}
	}
	//显示镶嵌中
	if (heroInfo.stoneId == SearchID)
	{
		tempCur = true;
		curSto = ge;
		Button_Buy->setVisible(false);
		Button_Set->setVisible(false);
		Image_Setting->setVisible(true);
		Image_Bracket->setVisible(true);
		//镶嵌之后
		Armature* armature1 = Armature::create("AinXingPanBaoShi");
		armature1->getAnimation()->play("Ani_XiangQian_G",-1,1);
		armature1->setPosition(Vec2(Image_Bracket->getContentSize().width/2,Image_Bracket->getContentSize().height/2));
		Image_Bracket->addChild(armature1);

	}
	Button_Buy->addTouchEventListener(CC_CALLBACK_2(StarsCampStoneLayer::onBtnClick,this,STO_LAYBA::STO_LAYBA_SHOP,ye, ge));
	Button_Set->addTouchEventListener(CC_CALLBACK_2(StarsCampStoneLayer::onBtnClick,this,STO_LAYBA::STO_LAYBA_XIANGQIAN,ye, ge));
	return tempCur;
}

void StarsCampStoneLayer::onBtnClick( Ref* reff, Widget::TouchEventType type,STO_LAYBA TYEP, int yeshu, int geshu)
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{

		int _cleckHero = StarsCampModel::getInstance()->getCleckHero();
		auto heroInfo  = StarsCampModel::getInstance()->getOndeHeroInfoByWz(_cleckHero);
		int heroId = heroInfo.uId;
		int SearchID =  heroId*10000+(350+100*(yeshu)+geshu);

		switch (TYEP)
		{
		case STO_LAYBA_FANYE:
			curYe = yeshu;
			setWhichShow();
			break;
		case STO_LAYBA_SHOP:
			{
				auto coin = MainModel::getInstance()->getMainParam()->mCoin;
				auto gold = MainModel::getInstance()->getMainParam()->mGold;

				
				auto data = DataManager::getInstance()->searchAttbitureIntroduceById(SearchID);
				auto prince = data->getLongData("price");

				if (data->getIntData("moneyType") == 2)
				{
					if (gold< prince)
					{
						CustomGoldTips *_tips = CustomGoldTips::create();
						if (_tips)
						{
							Director::getInstance()->getRunningScene()->addChild(_tips);
							return;
						}
					}
				}
				else
				{
					if (coin < prince)
					{
						LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
						if (buyEnergy)
						{
							Director::getInstance()->getRunningScene()->addChild(buyEnergy);
							return;
						}
					}
				}

				curSto = geshu;

				sendShopInfo(SearchID);
				break;
			}

		case STO_LAYBA_XIANGQIAN:
			{
				curSto = geshu;
				sendXiangqian(SearchID);
				break;
			}
		default:
			break;
		}
	}
}

void StarsCampStoneLayer::setWhichShow()
{
	for (unsigned int i=0; i<fourLayou.size(); ++i)
	{
		auto oneface = fourLayou.at(i);
		oneface->setVisible(false);
		auto button = fiveButton.at(i);
		button->setHighlighted(false);
		button->setEnabled(true);
		button->setTouchEnabled(true);
		if (i == curYe)
		{
			oneface->setVisible(true);
			button->setHighlighted(true);
			button->setEnabled(false);
			button->setTouchEnabled(false);
		}
	}
}

void StarsCampStoneLayer::setStoneShow( int stroID,STO_TYPE type )
{
	
	int _cleckHero = StarsCampModel::getInstance()->getCleckHero();
	auto heroInfo  = StarsCampModel::getInstance()->getOndeHeroInfoByWz(_cleckHero);
	auto lay = oneFace.at(curYe);

	auto Button_Buy =  dynamic_cast<Button*>(Helper::seekWidgetByName(lay,"Button_Buy"));
	auto Button_Set =  dynamic_cast<Button*>(Helper::seekWidgetByName(lay,"Button_Set"));
	auto Image_Setting =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(lay,"Image_Setting"));

	if (type ==STO_TYPE::TYEP_XIANGQIAN)
	{
		if (heroInfo.stoneId == stroID)
		{
			Button_Buy->setVisible(false);
			Button_Set->setVisible(false);
			Image_Setting->setVisible(true);
		}
	}
	else if (type ==STO_TYPE::TYEP_SHOP)
	{
		for (unsigned int i = 0; i < heroInfo.haveStroneId.size(); i++)
		{
			auto stoid = heroInfo.haveStroneId.at(i);
			if (stoid == stroID)
			{
				Button_Buy->setVisible(false);
				Button_Set->setVisible(true);
				Image_Setting->setVisible(false);
			}
		}
	}
}

void StarsCampStoneLayer::sendShopInfo(int id)
{
	StarsCampController::getInstance()->sendShopStone(id);
	StarsCampModel::getInstance()->setShopStoneId(id);
}

void StarsCampStoneLayer::sendXiangqian(int id)
{
	StarsCampController::getInstance()->sendXiangQianStone(id);
	StarsCampModel::getInstance()->setShopStoneId(id);
}

void StarsCampStoneLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*)data;
	switch (observerParam->id)
	{
	case SC_EVENT_TYPE_SHOP:
		{
			reseiveShop();
		}
		break;
	case SC_EVENT_TYPE_XIANGQIAN:
		{
			resiiveXiangQian();
		}
		break;
	default:
		break;
	}
}

void StarsCampStoneLayer::reseiveShop()
{
	sendXiangqian(StarsCampModel::getInstance()->getShopStoneId());
}

void StarsCampStoneLayer::resiiveXiangQian()
{
	//int _cleckHero = StarsCampModel::getInstance()->getCleckHero();
	//auto heroInfo  = StarsCampModel::getInstance()->getOndeHeroInfoByWz(_cleckHero);
	//int heroId = heroInfo.uId;
	//取得是哪个点击的是哪个页面
	int ye = curYe*4+curSto-1;
	int size = oneFace.size();
	if (ye >= size)
	{
		return;
	}

	//全部改成没有镶嵌
	for (unsigned int i=0; i<oneFace.size(); ++i)
	{
		auto son = oneFace.at(i);
		auto Image_Setting1 =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(son,"Image_Setting"));
		if (Image_Setting1->isVisible())
		{
			Image_Setting1->setVisible(false);

			auto Button_Set1 =  dynamic_cast<Button*>(Helper::seekWidgetByName(son,"Button_Set"));
			auto Button_Buy1 =  dynamic_cast<Button*>(Helper::seekWidgetByName(son,"Button_Buy"));

			Button_Set1->setVisible(true);
			Button_Buy1->setVisible(false);
			auto Image_Bracket = dynamic_cast<ImageView*>(Helper::seekWidgetByName(son,"Image_Bracket"));
			Image_Bracket->removeAllChildren();
			Image_Bracket->setVisible(false);
			break;
		}
	}
	//镶嵌的那个页面
	auto onFaceSon = oneFace.at(ye);
	auto Button_Buy =  dynamic_cast<Button*>(Helper::seekWidgetByName(onFaceSon,"Button_Buy"));
	auto Button_Set =  dynamic_cast<Button*>(Helper::seekWidgetByName(onFaceSon,"Button_Set"));
	auto Image_Setting =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(onFaceSon,"Image_Setting"));
	Button_Buy->setVisible(false);
	Button_Set->setVisible(false);
	Image_Setting->setVisible(true);
	/*  播放镶嵌动画  */
	playXiangQianAnimation(ye);

}

void StarsCampStoneLayer::playXiangQianAnimation(int ye)
{
	auto onFaceSon = oneFace.at(ye);
	//石头图片

	auto callba = CallFunc::create([=]()
	{
		//g正在镶嵌
		auto Image_Bracket = dynamic_cast<ImageView*>(Helper::seekWidgetByName( onFaceSon,"Image_Bracket"));
		Image_Bracket->setVisible(true);
	});

	Armature* armature = Armature::create("AinXingPanBaoShi");

	auto onResultAnimationCallFunc = [=](Armature *armature, MovementEventType eventType, const std::string& strName)->void
	{
		if (eventType == MovementEventType::COMPLETE)
		{
			//镶嵌中
			armature->removeFromParent();
			playingXiangQianAnimation();
		}
	};

	this->runAction(Sequence::create(DelayTime::create(0.3f),callba,nullptr));
	armature->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
	armature->getAnimation()->play("Ani_XiangQian",-1,0);

	auto Panel_Jewel = dynamic_cast<Layout*>(Helper::seekWidgetByName(onFaceSon,"Panel_Jewel"));
	armature->setPosition(Vec2(Panel_Jewel->getContentSize().width/2,Panel_Jewel->getContentSize().height/2));
	Panel_Jewel->addChild(armature,999);

	StarsCampController::getInstance()->sendGetStarsCampInfo();
}

void StarsCampStoneLayer::playingXiangQianAnimation()
{
	int ye = curYe*4+curSto-1;
	int size = oneFace.size();
	if (ye >= size)
	{
		return;
	}
	auto onFaceSon = oneFace.at(ye);
	//石头图片
	auto Image_Bracket = dynamic_cast<ImageView*>(Helper::seekWidgetByName( onFaceSon,"Image_Bracket"));
	//Image_Bracket->setVisible(true);
	//镶嵌之后
	Armature* armature1 = Armature::create("AinXingPanBaoShi");

	armature1->getAnimation()->play("Ani_XiangQian_G",-1,1);
	armature1->setPosition(Vec2(Image_Bracket->getContentSize().width/2,Image_Bracket->getContentSize().height/2));
	Image_Bracket->addChild(armature1);
}

void StarsCampStoneLayer::playShopStoneAnimation()
{

}
