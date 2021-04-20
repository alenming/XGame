#include "StarsCampLayerHero.h"
#include "StarsCampStoneScene.h"
#include "Utils/ResourceUtils.h"
#include "Widget/SuperRichText.h"

StarsCampLayerHero::StarsCampLayerHero()
{

}

StarsCampLayerHero::~StarsCampLayerHero()
{

}

bool StarsCampLayerHero::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}

	initUi();
	
	return true;
}

void StarsCampLayerHero::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIAstrolabe2/UIAstrolabe2_4.ExportJson"));

	//m_ui.ui_Root->setSize(size);
	//m_ui.ui_Root->setPosition(pos);
	this->addChild(m_ui.ui_Root);


	m_ui.Image_Bg_set = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Image_Jewel_set"));
	m_ui.Button_Buy = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Button_Buy"));
	m_ui.Button_Change = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Button_Change"));	
	m_ui.Button_Close =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Button_Close"));

	//4条属性
	for (int i=1; i<5; ++i)
	{
		UI_Right_attribute rightAttribute;
		rightAttribute.Image_Power_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Image_Power_"+_TO_STR(i)));

		rightAttribute.Label_power = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Label_Name_"+_TO_STR(i)));
		rightAttribute.Label_power->setFontName(FONT_FZZHENGHEI);

		rightAttribute.Label_Power_number_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Label_Power_"+_TO_STR(i)));
		rightAttribute.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);
		rightAttribute.Label_Power_number_1->setString(STR_UTF8(""));

		rightAttribute.Label_Additional = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Label_Additional_"+_TO_STR(i)));

		m_ui.fourAttribute.push_back(rightAttribute);
	}
	//加成
	m_ui.Label_Collect_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Label_Collect_1"));
	m_ui.Label_Jewel_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Label_Jewel_1"));
	m_ui.Image_Star = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Image_Star"));
	m_ui.Image_Quality = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Image_Quality"));
	//头像
	m_ui.Panel_Hero =  dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Panel_Hero"));
	//晶石名字
	m_ui.Label_Jewel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Label_Jewel"));
	m_ui.Label_Name = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Label_Name"));

	m_ui.Image_Plus =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Image_Plus"));
	m_ui.Image_Classification =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Image_Bg_set,"Image_Classification"));
}

void StarsCampLayerHero::onEnter()
{
	ModalLayer::onEnter();

	updateUI();
	updateCollect();

	GuideManager::getInstance()->startGuideNextFrame(GUIDE_INTERFACE_STAR_PLATE_HERO_INFO);
}
void StarsCampLayerHero::onExit()
{
	ModalLayer::onExit();
}
void StarsCampLayerHero::updateUI()
{
	m_ui.Button_Close->addTouchEventListener(CC_CALLBACK_2(StarsCampLayerHero::onBtnClick,this,SC_HERO_CALLBACK::SC_HERO_EXIT));

	int _cleckHero = StarsCampModel::getInstance()->getCleckHero();
	auto heroInfo  = StarsCampModel::getInstance()->getOndeHeroInfoByWz(_cleckHero);

	m_ui.Button_Buy->setVisible(true);
	m_ui.Button_Change->setVisible(true);
	if (heroInfo.haveStroneId.size() == 0)
	{
		m_ui.Button_Change->setVisible(false);
		m_ui.Button_Buy->addTouchEventListener(CC_CALLBACK_2(StarsCampLayerHero::onBtnClick,this,SC_HERO_CALLBACK::SC_HERO_SHOPORXX));
	}
	else
	{
		m_ui.Button_Buy->setVisible(false);
		m_ui.Button_Change->addTouchEventListener(CC_CALLBACK_2(StarsCampLayerHero::onBtnClick,this,SC_HERO_CALLBACK::SC_HERO_SHOPORXX));
	}
	
	auto hero = HeroModel::getInstance()->searchHeroByTemplateId(heroInfo.uId);
	if (hero ==nullptr)
	{
		UIToolHeader *icon = UIToolHeader::create(ResourceUtils::getSmallIconPath(heroInfo.uId));
		icon->setAnchorPoint(Vec2(0,0));
		icon->setGray(true);
		m_ui.Panel_Hero->addChild(icon);
	}
	else
	{
		UIToolHeader *icon = UIToolHeader::create(ResourceUtils::getSmallIconPath(heroInfo.uId));
		icon->setStarsNum(hero->mHeroStars,STAR_CONTAIN_BOTTOM);
		icon->setQuaCornerIcon(heroInfo.uId,hero->mAdvLvl);
		icon->setAnchorPoint(Vec2(0,0));

		m_ui.Panel_Hero->addChild(icon);
	}

	auto heroname = HeroModel::getInstance()->searchHeroByTemplateId(heroInfo.uId);
	if (heroname)
	{
		m_ui.Label_Name->setString(heroname->getName());
		m_ui.Label_Name->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(m_ui.Label_Name, Color3B(0x54, 0x0a, 0x0a),2);
	}
	else
	{
		auto hero = DataManager::getInstance()->searchCharacterById(heroInfo.uId);
		m_ui.Label_Name->setString(hero->getStringData("name"));
		m_ui.Label_Name->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(m_ui.Label_Name, Color3B(0x54, 0x0a, 0x0a),2);
	}
	//有晶石
	if (heroInfo.haveStroneId.size()>0)
	{
		m_ui.Label_Jewel->setString(heroInfo.stoneName);
		m_ui.Label_Jewel->setColor( Color3B(0xff, 0xf1, 0x19));
		m_ui.Label_Jewel->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(m_ui.Label_Jewel, Color3B(0x79, 0x03, 0x03),2);
		updateFourAttrubite( true);
	}
	else
	{
		m_ui.Label_Jewel->setString(DataManager::getInstance()->searchCommonTexdtById(20246));
		m_ui.Label_Jewel->setColor( Color3B(0xab, 0xab, 0xab));
		m_ui.Label_Jewel->setFontName(FONT_FZZHENGHEI);
		updateFourAttrubite( false);
	}

}

void StarsCampLayerHero::updateFourAttrubite(bool issto)
{
	//加成类型图片
	m_ui.Image_Plus->removeAllChildren();
	auto oneCampInfo  = StarsCampModel::getInstance()->getOndeStarCampInfoByWz(StarsCampModel::getInstance()->getCleckWZ());

	string name1 = ResourceUtils::getStarCampBuffPathById(oneCampInfo.type);
	auto image = ImageView::create(name1);
	image->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	image->setPosition(Vec2(m_ui.Image_Plus->getContentSize().width,m_ui.Image_Plus->getContentSize().height/2));
	image->setScale(0.7f);
	m_ui.Image_Plus->addChild(image);
	if (issto)
	{
		auto heroInfo  = StarsCampModel::getInstance()->getOndeHeroInfoByWz(StarsCampModel::getInstance()->getCleckHero());
		int StoneId = heroInfo.stoneId;
	
		//比例加成之和
			float jiaChen = heroInfo.jiaChen;
		//jiaChen*=100;

		auto data = DataManager::getInstance()->searchAttbitureIntroduceById(StoneId);
		//4个数值
		int gongji = data->getLongData("atk");
		int xueliang = data->getLongData("hp");
		int mofang = data->getLongData("pdd");
		int wufang = data->getLongData("mdd");

		auto temp1 = m_ui.fourAttribute.at(0);
		temp1.Label_power->setFontName(FONT_FZZHENGHEI);
		temp1.Label_Power_number_1->setString("+"+TO_STR(gongji));
		temp1.Label_Power_number_1->setColor(Color3B(0x54, 0xdd, 0x51));
		temp1.Label_Additional->setVisible(true);
		temp1.Label_Additional->setFontName(FONT_FZZHENGHEI);
		temp1.Label_Additional->setFontSize(18);
		temp1.Label_Additional->setString("+" + TO_STR(jiaChen) + "%");

		auto temp2 = m_ui.fourAttribute.at(1);
		temp2.Label_Power_number_1->setString("+"+TO_STR(xueliang));
		temp2.Label_Power_number_1->setColor(Color3B(0x54, 0xdd, 0x51));
		temp2.Label_Additional->setVisible(true);
		temp2.Label_Additional->setFontName(FONT_FZZHENGHEI);
		temp2.Label_Additional->setFontSize(18);
		temp2.Label_Additional->setString("+" + TO_STR(jiaChen) + "%");

		auto temp3 = m_ui.fourAttribute.at(2);
		temp3.Label_Power_number_1->setString("+"+TO_STR(mofang));
		temp3.Label_Power_number_1->setColor(Color3B(0x54, 0xdd, 0x51));
		temp3.Label_Additional->setVisible(true);
		temp3.Label_Additional->setFontName(FONT_FZZHENGHEI);
		temp3.Label_Additional->setFontSize(18);
		temp3.Label_Additional->setString("+" + TO_STR(jiaChen) + "%");

		auto temp4 = m_ui.fourAttribute.at(3);;
		temp4.Label_Power_number_1->setString("+"+TO_STR(wufang));
		temp4.Label_Power_number_1->setColor(Color3B(0x54, 0xdd, 0x51));
		temp4.Label_Additional->setVisible(true);
		temp4.Label_Additional->setFontName(FONT_FZZHENGHEI);
		temp4.Label_Additional->setFontSize(18);
		temp4.Label_Additional->setString("+" + TO_STR(jiaChen) + "%");
	}
	else
	{
		for (int i = 0; i < m_ui.fourAttribute.size(); i++)
		{
			auto temp = m_ui.fourAttribute.at(i);
			temp.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);
			temp.Label_Power_number_1->setColor(Color3B(0xab, 0xab, 0xab));
			temp.Label_Power_number_1->setString(DataManager::getInstance()->searchCommonTexdtById(20148));
			temp.Label_Additional->setVisible(false);

		}
	}
}

void StarsCampLayerHero::updateCollect()
{
	auto heroInfo  = StarsCampModel::getInstance()->getOndeHeroInfoByWz(StarsCampModel::getInstance()->getCleckHero());
	int heroID = heroInfo.uId;

	//收集加成
	auto heroParm = HeroModel::getInstance()->searchHeroByTemplateId(heroID);
	m_ui.Label_Collect_1->setFontName(FONT_FZZHENGHEI);
	auto BuffData1 = DataManager::getInstance()->searchHeroStoneById(heroID*10000+1000);//heroInfo.Collect_ID
	string buffName = BuffData1->getStringData("des");

	m_ui.Label_Collect_1->setString(buffName);

	if (heroInfo.Collect_ID == 0)
	{
		m_ui.Label_Collect_1->setColor(Color3B(0xab, 0xab, 0xab));//没有	
		//setTextAddStroke(m_ui.Label_Collect_1, Color3B(0x7d, 0x3f, 0x1c),2);
	}
	else
	{	
		m_ui.Label_Collect_1->setColor(Color3B(0xe8, 0xde, 0xb5));
		setTextAddStroke(m_ui.Label_Collect_1, Color3B(0x7d, 0x3f, 0x1c),2);
	}

	auto Image_Bg_2 =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Image_Bg_2"));
	//星级加成
	auto Label_Star = dynamic_cast<Text*>(Helper::seekWidgetByName( Image_Bg_2,"Label_Star"));
	Label_Star->setFontName(FONT_FZZHENGHEI);
   
	if (heroInfo.Star_ID == 0)
	{
		auto tempStar_1= heroInfo.uId*10000 +2003;
		auto BuffData4 = DataManager::getInstance()->searchHeroStoneById(tempStar_1);
		buffName = BuffData4->getStringData("des");
		Label_Star->setString(buffName);
		Label_Star->setColor(Color3B(0xab, 0xab, 0xab));
	}
	else
	{
		auto BuffData2 = DataManager::getInstance()->searchHeroStoneById(heroInfo.Star_ID);
		buffName = BuffData2->getStringData("des");
		Label_Star->setString(buffName);
		Label_Star->setColor(Color3B(0xe8,0xde,0xb5));
		setTextAddStroke(Label_Star, Color3B(0x7d, 0x3f, 0x1c),2);
	}


	//品质加成
	auto Label_Quality = dynamic_cast<Text*>(Helper::seekWidgetByName(Image_Bg_2,"Label_Quality"));
	Label_Quality->setVisible(false);


	auto Label_Quality_1 = SuperRichText::create();
	
	Label_Quality_1->setPosition(Vec2(m_ui.Image_Quality->getContentSize().width,m_ui.Image_Quality->getContentSize().height/2));
	if (heroInfo.Quality_ID == 0)
	{	
		auto tempQuality_1= heroInfo.uId*10000 +3010;
		auto BuffData4 = DataManager::getInstance()->searchHeroStoneById(tempQuality_1);
		buffName = BuffData4->getStringData("des");
		Label_Quality_1->setString(buffName,22,Color3B(0xab, 0xab, 0xab));
	}
	else
	{
		auto BuffData3 = DataManager::getInstance()->searchHeroStoneById(heroInfo.Quality_ID);
		buffName = BuffData3->getStringData("des");
		Label_Quality_1->setString(buffName,22,Color3B(0xe8,0xde,0xb5));
		Label_Quality_1->setStroke(Color3B(0x7d, 0x3f, 0x1c),2);	

	}

	m_ui.Image_Quality->addChild(Label_Quality_1);

	//星石数量加成
	m_ui.Label_Jewel_1->setFontName(FONT_FZZHENGHEI);
	if (heroInfo.Jewel_ID == 0)
	{
		auto tempJewel_ID = heroInfo.uId*10000 +4004;
		auto BuffData4 = DataManager::getInstance()->searchHeroStoneById(tempJewel_ID);
		buffName = BuffData4->getStringData("des");
		m_ui.Label_Jewel_1->setString(buffName);
		m_ui.Label_Jewel_1->setColor(Color3B(0xab, 0xab, 0xab));
	}
	else
	{
		auto BuffData4 = DataManager::getInstance()->searchHeroStoneById(heroInfo.Jewel_ID);//heroInfo.Jewel_ID
		buffName = BuffData4->getStringData("des");
		m_ui.Label_Jewel_1->setString(buffName);
		m_ui.Label_Jewel_1->setColor(Color3B(0xe8, 0xde, 0xb5));
		setTextAddStroke(m_ui.Label_Jewel_1, Color3B(0x7d, 0x3f, 0x1c),2);
	}
}

void  StarsCampLayerHero::onBtnClick( Ref* reff, Widget::TouchEventType type,SC_HERO_CALLBACK _callback)
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		switch (_callback)
		{
		case SC_HERO_EXIT:
			this->dispose();
			break;
		case SC_HERO_SHOPORXX:
			{
				auto scene = StarsCampStoneScene::create();
				Director::getInstance()->pushScene(scene);
			}
			break;
		default:
			break;
		}
	}
}

ImageView* StarsCampLayerHero::getSonChirld()
{
	return m_ui.Image_Bg_set;
}
