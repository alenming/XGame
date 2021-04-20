#include "StarsCampLayerOne.h"
#include "StarsCampScene.h"

StarsCampLayerOne::StarsCampLayerOne()
{
	StarsCampModel::getInstance()->addObserver(this);
}

StarsCampLayerOne::~StarsCampLayerOne()
{
	StarsCampModel::getInstance()->removeObserver(this);
}

bool StarsCampLayerOne::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUi();

	return true;
}

void StarsCampLayerOne::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIAstrolabe2/UIAstrolabe2_6.ExportJson"));
	m_ui.ui_Root->setSize(size);
	m_ui.ui_Root->setPosition(pos);
	this->addChild(m_ui.ui_Root);


	m_ui.ui_Root->setBackGroundImage("Image/Bg/img_bg.png",ui::Widget::TextureResType::LOCAL);

	//提示
	m_ui.teTip = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root,"Label_Tips"));
	m_ui.teTip->setFontName(FONT_FZZHENGHEI);

	//中间容器
	m_ui.Panel_Astrolabe_3 = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Root,"Panel_Astrolabe_3"));

	//拿数据
	auto _cleck  = StarsCampModel::getInstance()->getCleckWZ();
	auto oneinfo = StarsCampModel::getInstance()->getOndeStarCampInfoByWz(_cleck);
	auto fiveHero = oneinfo.heroId;


	//哪种类型的盘
	int index = (_cleck)%5+1;
	auto _rroot= dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIAstrolabe2/UIAstrolabe2_2.ExportJson"));
	auto rootSon1 = dynamic_cast<Layout*>(Helper::seekWidgetByName(_rroot,"BPanel_Hero_"+TO_STR(index)));
	m_ui.midLa = dynamic_cast<Layout*>(rootSon1->clone());
	m_ui.midLa->setVisible(true);


	//底部图片设置不可点击高亮
	auto sonbutton = dynamic_cast<Button*>(m_ui.midLa->getChildByName("Button_Astrolabe"));
	string name1 = ResourceUtils::getStarCampDiPathByStr(TO_STR(oneinfo.type));
	string name2 = ResourceUtils::getStarCampDiPathByStr(TO_STR(oneinfo.type)+"a");
	sonbutton->loadTextures(name1,name2,name2,ui::Widget::TextureResType::LOCAL);
	sonbutton->setHighlighted(true);
	sonbutton->setTouchEnabled(false);

	auto Image_Open = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.midLa,"Image_Open"));
	auto Label_Number = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.midLa,"Label_Number"));
	auto Image_New = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.midLa,"Image_New"));
	Image_New->setVisible(false);
	Image_Open->setVisible(false);
	Label_Number->setVisible(false);	
	
	m_ui.midLa->setScale(1.6f);
	m_ui.midLa->setAnchorPoint(Vec2(0.5,0.5));
	auto paSize = m_ui.Panel_Astrolabe_3->getContentSize();
	m_ui.midLa->setPosition(Vec2(paSize.width/2,paSize.height/2));
	m_ui.Panel_Astrolabe_3->addChild(m_ui.midLa);

	//左边UI
	m_ui.Image_Bg_left =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Image_Bg_left"));
	for (int i = 0; i < FIVEHERO; i++)
	{
		UI_Left_attribute leftAttribute;
		leftAttribute.Label_Hero = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root,"Label_Hero_"+_TO_STR(i+1)));
		leftAttribute.Label_Jewel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root,"Label_Jewel_"+_TO_STR(i+1)));
		m_ui.leftAttribute.push_back(leftAttribute);
	}
	//右边UI
	m_ui.Image_Bg_right =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Image_Bg_right"));
	for (int i=1; i<5; ++i)
	{
		UI_Right_attribute rightAttribute;
		rightAttribute.Image_Power_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName( m_ui.Image_Bg_right,"Image_Power_"+_TO_STR(i)));
		rightAttribute.Label_power = dynamic_cast<Text*>(Helper::seekWidgetByName( m_ui.Image_Bg_right,"Label_Name_"+_TO_STR(i)));
		rightAttribute.Label_Power_number_1 = dynamic_cast<Text*>(Helper::seekWidgetByName( m_ui.Image_Bg_right,"Label_Power_"+_TO_STR(i)));
		m_ui.rightAttribute.push_back(rightAttribute);
	}

	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			Director::getInstance()->replaceScene(StarsCampScene::create());			
		}
	};
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_STARSTONEGAT);
	this->addChild(_layerCommHeadInfo);

	initLeft();
	initMid();
	initRight();
	setAllFlag();

	GuideManager::getInstance()->startGuideNextFrame(GUIDE_INTERFACE_STAR_PLATE_INFO);
}

void StarsCampLayerOne::onEnter()
{
	Layer::onEnter();
}



void StarsCampLayerOne::onExit()
{
	Layer::onExit();
}

void StarsCampLayerOne::onBtnClick( Ref* reff, Widget::TouchEventType type,ONECAMP_CALLBACK TYEP,int uId,Button* son)
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		switch (TYEP)
		{
			case ONECAMP_CLECKHERO:
				StarsCampModel::getInstance()->setCleckHero(uId);
				if (son)
				{
					setJoinInFlag(son,false);
				}
				jumpToOnHeroInfo();
				break;
			default:
				break;
		}		
	}
}

void StarsCampLayerOne::jumpToOnHeroInfo()
{

	Layout *imgLine = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.midLa, "Panel_Hero_" 
		+TO_STR(StarsCampModel::getInstance()->getCleckHero()+1)));
	Button *imaButton = dynamic_cast<Button*>(Helper::seekWidgetByName(imgLine, "Button_Hero"));
	setJoinInFlag(imaButton,false);
	
	m_layer = StarsCampLayerHero::create();
	this->addChild(m_layer);

	ImageView* uiBg = m_layer->getSonChirld();
	
	ActionCreator::runCommDlgAction(uiBg);
	
}

void StarsCampLayerOne::setJoinInFlag(Button *icon, bool isShow)
{

	Node* FriendFlag = icon->getChildByName("mail_flg");
	if (!FriendFlag)
	{
		FriendFlag = cocos2d::ui::ImageView::create("Image/Icon/global/img_dian.png");
		FriendFlag->setPosition(cocos2d::Vec2(icon->getContentSize().width - FriendFlag->getContentSize().width/2, 
			icon->getContentSize().height - FriendFlag->getContentSize().height/2));
		FriendFlag->setName("mail_flg");
		icon->addChild(FriendFlag, 9);
	}
	if (StarsCampModel::getInstance()->getIsNewCamp()&& isShow)
	{
		FriendFlag->setVisible(true);
	}
	else
	{
		FriendFlag->setVisible(false);
	}
}

void StarsCampLayerOne::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*)data;
	switch (observerParam->id)
	{
	case SC_EVENT_TYPE_GET_INFO:
		{
			//星石名字
			updateLeft();
			//换星石图片 
			updateMid();
			//星石加成属性
			updateRight();
		}
		break;
	default:
		break;
	}
}

void StarsCampLayerOne::setAllFlag()
{
	for (int k=0; k<FIVEHERO; ++k)
	{
		Layout *imgLine = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.midLa, "Panel_Hero_" + TO_STR(k+1)));
		Button *imaButton = dynamic_cast<Button*>(Helper::seekWidgetByName(imgLine, "Button_Hero"));
		setJoinInFlag(imaButton,true);
	}
}

void StarsCampLayerOne::initLeft()
{
	for (int i = 0; i < FIVEHERO; i++)
	{
		auto  leftAttribute = m_ui.leftAttribute.at(i);
		auto _cleck  = StarsCampModel::getInstance()->getCleckWZ();

		auto oncampinfo = StarsCampModel::getInstance()->getOndeStarCampInfoByWz(_cleck);

		//英雄名字
		auto hero = DataManager::getInstance()->searchCharacterById(oncampinfo.heroId.at(i).uId);
		leftAttribute.Label_Hero->setString(hero->getStringData("name"));
		leftAttribute.Label_Hero->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(leftAttribute.Label_Hero, Color3B(0x54, 0x0a, 0x0a),2);

		//有晶石
		if (oncampinfo.heroId.at(i).stoneId != 0)
		{
			leftAttribute.Label_Jewel->setString(oncampinfo.heroId.at(i).stoneName);
			leftAttribute.Label_Jewel->setColor( Color3B(0xff, 0xf1, 0x19));
			leftAttribute.Label_Jewel->setFontName(FONT_FZZHENGHEI);
			setTextAddStroke(leftAttribute.Label_Jewel, Color3B(0x79, 0x03, 0x03),2);	
		}
		else if (oncampinfo.heroId.at(i).stoneId == 0)
		{
			leftAttribute.Label_Jewel->setString(DataManager::getInstance()->searchCommonTexdtById(20148));
			leftAttribute.Label_Jewel->setColor( Color3B(0xab, 0xab, 0xab));
			leftAttribute.Label_Jewel->setFontName(FONT_FZZHENGHEI);
		}
	}
}

void StarsCampLayerOne::initRight()
{
	auto _cleck  = StarsCampModel::getInstance()->getCleckWZ();
	auto oneCampAttribute = StarsCampModel::getInstance()->getOndeStarCampInfoByWz(_cleck);

	long int gongji	= oneCampAttribute.gongji;
	long int xueliang= oneCampAttribute.xueliang;
	long int wufang	= oneCampAttribute.wufang;
	long int mofang	= oneCampAttribute.mofang;

	auto temp = m_ui.rightAttribute.at(0);
	temp.Label_power->setFontName(FONT_FZZHENGHEI);
	temp.Label_Power_number_1->setString("+"+TO_STR(gongji));
	temp.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);

	temp = m_ui.rightAttribute.at(1);;
	temp.Label_power->setFontName(FONT_FZZHENGHEI);
	temp.Label_Power_number_1->setString("+"+TO_STR(xueliang));
	temp.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);

	temp = m_ui.rightAttribute.at(2);
	temp.Label_power->setFontName(FONT_FZZHENGHEI);
	temp.Label_Power_number_1->setString("+"+TO_STR(mofang));
	temp.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);

	temp = m_ui.rightAttribute.at(3);
	temp.Label_power->setFontName(FONT_FZZHENGHEI);
	temp.Label_Power_number_1->setString("+"+TO_STR(wufang));
	temp.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);
}

void StarsCampLayerOne::initMid()
{
	//拿数据
	auto _cleck  = StarsCampModel::getInstance()->getCleckWZ();
	auto oneinfo = StarsCampModel::getInstance()->getOndeStarCampInfoByWz(_cleck);
	auto fiveHero = oneinfo.heroId;

	/************************************************************************/
	/*                        更新阵形UI                                    */
	/************************************************************************/
	for (int k=0; k<FIVEHERO; ++k)
	{
		Layout *imgLine = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.midLa, "Panel_Hero_" + TO_STR(k+1)));
		Button *imaButton = dynamic_cast<Button*>(Helper::seekWidgetByName(imgLine, "Button_Hero"));
		imgLine->setTouchEnabled(false);
		imaButton->setTouchEnabled(true);
		imaButton->addTouchEventListener(CC_CALLBACK_2(StarsCampLayerOne::onBtnClick,this,ONECAMP_CALLBACK::ONECAMP_CLECKHERO,k,imaButton));

		//星盘英雄头像
		auto hero = HeroModel::getInstance()->searchHeroByTemplateId(fiveHero.at(k).uId);
		ImageView* icon = ImageView::create(ResourceUtils::getStarCampPathById(fiveHero.at(k).uId));
		//没有英雄设置成灰色
		if (hero == nullptr)
		{
			setImageGray(icon,true);
		}
		icon->setScale(0.65f);
		icon->setPosition(Vec2(imgLine->getContentSize().width/2,imgLine->getContentSize().height/2));
		icon->setZOrder(-1);

		//setJoinInFlag(imaButton,true);

		imgLine->addChild(icon);

		if (fiveHero.at(k).stoneId != 0)
		{
			ImageView* image = dynamic_cast<ImageView*>(imgLine->getChildByName("stoname"));
			if (image)
			{
				image->removeFromParent();
			}
			string stoname = ResourceUtils::getStarCampPathByStr(fiveHero.at(k).stoneiconID);
			image = ImageView::create(stoname);
			image->setName("stoname");
			image->setScale(0.25f);
			image->setPosition(Vec2(imgLine->getContentSize().width/2+2,5));
			imgLine->addChild(image);
		}
	}
}

void StarsCampLayerOne::updateMid()
{
	//拿数据
	auto _cleck  = StarsCampModel::getInstance()->getCleckWZ();
	auto oneinfo = StarsCampModel::getInstance()->getOndeStarCampInfoByWz(_cleck);
	auto fiveHero = oneinfo.heroId;

	for (int k=0; k<FIVEHERO; ++k)
	{
		Layout *imgLine = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.midLa, "Panel_Hero_" + TO_STR(k+1)));
		Button *imaButton = dynamic_cast<Button*>(Helper::seekWidgetByName(imgLine, "Button_Hero"));

		if (fiveHero.at(k).stoneId != 0)
		{
			ImageView* image = dynamic_cast<ImageView*>(imgLine->getChildByName("stoname"));
			if (image)
			{
				image->removeFromParent();
			}
			string stoname = ResourceUtils::getStarCampPathByStr(fiveHero.at(k).stoneiconID);
			image = ImageView::create(stoname);
			image->setName("stoname");
			image->setScale(0.25f);
			image->setPosition(Vec2(imgLine->getContentSize().width/2+2,5));
			imgLine->addChild(image);
		}
	}
}

void StarsCampLayerOne::updateLeft()
{
	for (int i = 0; i < FIVEHERO; i++)
	{
		auto  leftAttribute = m_ui.leftAttribute.at(i);
		auto _cleck  = StarsCampModel::getInstance()->getCleckWZ();

		auto oncampinfo = StarsCampModel::getInstance()->getOndeStarCampInfoByWz(_cleck);

		//英雄名字
		auto hero = DataManager::getInstance()->searchCharacterById(oncampinfo.heroId.at(i).uId);
		leftAttribute.Label_Hero->setString(hero->getStringData("name"));
		leftAttribute.Label_Hero->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(leftAttribute.Label_Hero, Color3B(0x54, 0x0a, 0x0a),2);

		//有晶石
		if (oncampinfo.heroId.at(i).stoneId != 0)
		{
			leftAttribute.Label_Jewel->setString(oncampinfo.heroId.at(i).stoneName);
			leftAttribute.Label_Jewel->setColor( Color3B(0xff, 0xf1, 0x19));
			leftAttribute.Label_Jewel->setFontName(FONT_FZZHENGHEI);
			setTextAddStroke(leftAttribute.Label_Jewel, Color3B(0x79, 0x03, 0x03),2);	
		}
		else if (oncampinfo.heroId.at(i).stoneId == 0)
		{
			leftAttribute.Label_Jewel->setString(DataManager::getInstance()->searchCommonTexdtById(20148));
			leftAttribute.Label_Jewel->setColor( Color3B(0xab, 0xab, 0xab));
			leftAttribute.Label_Jewel->setFontName(FONT_FZZHENGHEI);
		}
	}
}

void StarsCampLayerOne::updateRight()
{
	auto _cleck  = StarsCampModel::getInstance()->getCleckWZ();
	auto oneCampAttribute = StarsCampModel::getInstance()->getOndeStarCampInfoByWz(_cleck);

	long int gongji	= oneCampAttribute.gongji;
	long int xueliang= oneCampAttribute.xueliang;
	long int wufang	= oneCampAttribute.wufang;
	long int mofang	= oneCampAttribute.mofang;


	auto temp = m_ui.rightAttribute.at(0);
	temp.Label_power->setFontName(FONT_FZZHENGHEI);
	temp.Label_Power_number_1->setString("+"+TO_STR(gongji));
	temp.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);

	temp = m_ui.rightAttribute.at(1);;
	temp.Label_power->setFontName(FONT_FZZHENGHEI);
	temp.Label_Power_number_1->setString("+"+TO_STR(xueliang));
	temp.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);

	temp = m_ui.rightAttribute.at(2);
	temp.Label_power->setFontName(FONT_FZZHENGHEI);
	temp.Label_Power_number_1->setString("+"+TO_STR(mofang));
	temp.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);

	temp = m_ui.rightAttribute.at(3);
	temp.Label_power->setFontName(FONT_FZZHENGHEI);
	temp.Label_Power_number_1->setString("+"+TO_STR(wufang));
	temp.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);
}