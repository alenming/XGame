#include "StarsCampLayer.h"
#include "StarsCampSceneOne.h"
#include "Common/LocalData.h"
#include "Widget/LayerGameRules.h"

#define STAR_GAP	70
#define STAR_SON_KEY	700




StarsCampLayer::StarsCampLayer()//:cleckSon(1)
{
	StarsCampModel::getInstance()->addObserver(this);
}

StarsCampLayer::~StarsCampLayer()
{

	StarsCampModel::getInstance()->removeObserver(this);
}

bool StarsCampLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	initUi();
	
	return true;
}

void StarsCampLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIAstrolabe2/UIAstrolabe2_1.ExportJson"));
	m_ui.ui_Root->setBackGroundImage("Image/Bg/img_bg.png",ui::Widget::TextureResType::LOCAL);
	m_ui.ui_Root->setSize(size);
	m_ui.ui_Root->setPosition(pos);
	this->addChild(m_ui.ui_Root);

	m_ui.btnGoToFace =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Button_Jewel"));
	m_ui.btnGoToFace->addTouchEventListener(CC_CALLBACK_2(StarsCampLayer::onBtnClick,this,SC_CallBack::SC_GotoFace,0));

	m_ui.btnLeft =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Button_Left"));
	m_ui.btnRight =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Button_Right"));
	m_ui.Button_Tip =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Button_Tip"));
	m_ui.Button_Tip->addTouchEventListener(CC_CALLBACK_2(StarsCampLayer::onBtnClick,this,SC_CallBack::SC_Tip,0));
	m_ui.btnLeft->addTouchEventListener(CC_CALLBACK_2(StarsCampLayer::onBtnClick,this,SC_CallBack::SC_Left,0));
	m_ui.btnRight->addTouchEventListener(CC_CALLBACK_2(StarsCampLayer::onBtnClick,this,SC_CallBack::SC_Right,0));

	m_ui.ImgBg =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Image_Bg"));
	m_ui.ImageClassificationParent =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Image_Title"));
	m_ui.ImageClassification =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Image_Classification"));
	for (int i=1; i<FIVEHERO; ++i)
	{
		Power_Introduce oneAttribute;
		oneAttribute.Image_Power_1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Image_Power_"+_TO_STR(i)));
		oneAttribute.Label_power = dynamic_cast<Text*>(Helper::seekWidgetByName(oneAttribute.Image_Power_1,"Label_power"));
		oneAttribute.Label_power->setFontName(FONT_FZZHENGHEI);
		oneAttribute.Label_Power_number_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root,"Label_Power_number_"+_TO_STR(i)));
		oneAttribute.Label_Power_number_1->setString(STR_UTF8(""));
		oneAttribute.Label_Power_number_1->setFontName(FONT_FZZHENGHEI);
		oneAttribute.Label_Power_number_Max_1 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root,"Label_Power_number_Max_"+_TO_STR(i)));
		oneAttribute.Label_Power_number_Max_1->setString(STR_UTF8(""));
		oneAttribute.Label_Power_number_Max_1->setFontName(FONT_FZZHENGHEI);
		m_ui.attribute_Introduce.push_back(oneAttribute);
	}
	 m_ui.pAstrolabe =  dynamic_cast<PageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"PageView_Astrolabe"));
	 //设置翻页划动距离 为contentSize.with的20%
	 m_ui.pAstrolabe->setpanGestureDistance(0.2);
	 m_ui.laStarLayou = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Root,"Panel_Star"));
	 m_ui.laStarLayou->setVisible(false);

	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			Director::getInstance()->replaceScene(MainCityScene::create());
		}
	};
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_STAR_CAMP);
	this->addChild(_layerCommHeadInfo);

	Director::getInstance()->getScheduler()->schedule(schedule_selector(StarsCampLayer::updateNextFram), this, 0, false);
}

void StarsCampLayer::onEnter()
{
	Layer::onEnter();
	
}
void StarsCampLayer::updateNextFram(float ft)
{
	StarsCampController::getInstance()->sendGetStarsCampInfo();
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(StarsCampLayer::updateNextFram), this);
}

void StarsCampLayer::onExit()
{
	Layer::onExit();
}

void StarsCampLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*)data;
	switch (observerParam->id)
	{
	case SC_EVENT_TYPE_GET_INFO:
		{
			GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_STAR_PLATE);

			freshLayerForBigLayer();
			int newcampId =  StarsCampModel::getInstance()->getNewCampId();//getNewCampId();
			
			UserDefault::getInstance()->setIntegerForKey("isopencamp",0);
			StarsCampModel::getInstance()->setNewCampId(-1);
			setNewCampAnimation(newcampId, true);	
			//初始有一个变大
			int temp = 0;
			if (newcampId != -1)
			{
				 temp = newcampId;
				 StarsCampModel::getInstance()->setIsNewCamp(true);
				 StarsCampModel::getInstance()->setCleckWZ(temp);
			}
			else
			{
				StarsCampModel::getInstance()->setIsNewCamp(false);
				temp = StarsCampModel::getInstance()->getCleckWZ();
			}
			//定位到哪一页
			m_ui.pAstrolabe->scrollToPage(temp/3);
			
			auto son1 = vecSonStCamp.at(temp);
			son1->setScale(1.15f);
			auto sonbutton = dynamic_cast<Button*>(son1->getChildByName("Button_Astrolabe"));
			sonbutton->setHighlighted(true);
			sonbutton->setTouchEnabled(false);
			//初始化开始初中时下面的数值
			setAttbrute();
		}
		break;
	default:
		break;
	}
}

void  StarsCampLayer::onBtnClick( Ref* reff, Widget::TouchEventType type, SC_CallBack _callBackType,int wz)
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		switch (_callBackType)
		{
		case SC_Default:
			break;
		case SC_Left:
			{
				auto index = m_ui.pAstrolabe->getCurPageIndex();
				if (--index>=0)
				{
					m_ui.pAstrolabe->scrollToPage(index);
				}
				break;
			}
		case SC_Right:
			{
				auto index = m_ui.pAstrolabe->getCurPageIndex();
				if (++index<m_ui.pAstrolabe->getPages().size())
				{
					m_ui.pAstrolabe->scrollToPage(index);
				}
				break;
			}
		case SC_GotoFace:
			{
				int temp = StarsCampModel::getInstance()->getCleckWZ()/3;
				if (m_ui.pAstrolabe->getCurPageIndex()==temp)
				{
					/************************************************************************/
					/*                   加入解锁等级判断                                   */
					/************************************************************************/
					int mainLevel = HeroModel::getInstance()->getMainHeroLevel();
					auto oneCamp = StarsCampModel::getInstance()->getOndeStarCampInfoByWz(StarsCampModel::getInstance()->getCleckWZ());
					int goLevel = oneCamp.goLevel;
					
					if (mainLevel< goLevel)
					{
						CustomTips::showUtf8(TO_STR(goLevel) + DataManager::getInstance()->searchCommonTexdtById(10001));
					}
					else
					{
						Director::getInstance()->getScheduler()->unschedule(schedule_selector(StarsCampLayer::myupdate), this);
						auto scene = StarsCampSceneOne::create();
						Director::getInstance()->replaceScene(scene);
					}
				}
				break;
			}
		case SC_GotoStarsSon:
			{
				freshCleckStarCamp(wz);
				break;
			}
		case SC_Tip:
			{
				LayerGameRules* _help = LayerGameRules::create(STARSCAMP_RULE_ID);
				if (nullptr != _help)
				{
					this->addChild(_help, 5);
					ActionCreator::runCommDlgAction(_help);
				}
				break;
			}
		default:
			break;
		}
	}
}

void  StarsCampLayer::freshCleckStarCamp(int wz)
{
	StarsCampModel::getInstance()->setCleckWZ(wz);

	setBigOrSmall(wz);
	setAttbrute();
}

void  StarsCampLayer::setBigOrSmall(int wz)
{

	for (int i=0; i<vecSonStCamp.size(); ++i)
	{
		auto otherson = vecSonStCamp.at(i);
		auto othersonbutton = dynamic_cast<Button*>(otherson->getChildByName("Button_Astrolabe"));
		othersonbutton->setHighlighted(false);
		othersonbutton->setTouchEnabled(true);
		otherson->setScale(1);
	}

	auto son1 =  vecSonStCamp.at(wz);
	auto scaleTo  = ScaleTo::create(0.1,1.15f);
	son1->runAction(scaleTo);

	auto sonbutton = dynamic_cast<Button*>(son1->getChildByName("Button_Astrolabe"));
	sonbutton->setHighlighted(true);
	sonbutton->setTouchEnabled(false);
	
}

void StarsCampLayer::setAttbrute()
{
	auto oneCampAttribute =StarsCampModel::getInstance()->getOndeStarCampInfoByWz(StarsCampModel::getInstance()->getCleckWZ());


	auto temp = m_ui.attribute_Introduce.at(0);
	temp.Label_power->setFontName(FONT_FZZHENGHEI);



	temp.Label_Power_number_1->setText("+"+TO_STR(oneCampAttribute.gongji));
	temp.Label_Power_number_Max_1->setText("(" + DataManager::getInstance()->searchCommonTexdtById(20174)+ TO_STR(oneCampAttribute.maxgj)+")");
	
	temp = m_ui.attribute_Introduce.at(1);

	temp.Label_Power_number_1->setText("+"+TO_STR(oneCampAttribute.xueliang));
	temp.Label_Power_number_Max_1->setText("(" + DataManager::getInstance()->searchCommonTexdtById(20174) + TO_STR(oneCampAttribute.maxxl)+")");

	temp = m_ui.attribute_Introduce.at(2);
	temp.Label_Power_number_1->setText("+"+TO_STR(oneCampAttribute.mofang));
	temp.Label_Power_number_Max_1->setText("(" +DataManager::getInstance()->searchCommonTexdtById(20174)+ TO_STR(oneCampAttribute.maxmf)+")");

	temp = m_ui.attribute_Introduce.at(3);
	temp.Label_Power_number_1->setText("+"+TO_STR(oneCampAttribute.wufang));
	temp.Label_Power_number_Max_1->setText("(" +DataManager::getInstance()->searchCommonTexdtById(20174) + TO_STR(oneCampAttribute.maxwf)+")");
	
	//哪种属性加成的图片
	m_ui.ImageClassificationParent->removeAllChildren();

	string name1 = ResourceUtils::getStarCampBuffPathById(oneCampAttribute.type);
	auto image = ImageView::create(name1);
	image->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.ImageClassificationParent->removeAllChildren();
	image->setPosition(Vec2(m_ui.ImageClassificationParent->getContentSize().width,m_ui.ImageClassificationParent->getContentSize().height/2));
	m_ui.ImageClassificationParent->addChild(image);
}

void StarsCampLayer::freshLayerForBigLayer()
{
	auto _rroot= dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIAstrolabe2/UIAstrolabe2_2.ExportJson"));

	auto starcampInfo = StarsCampModel::getInstance()->getAllStarCampInfo();
	int yeshu = StarsCampModel::getInstance()->getYeShu();

	for (int j=0; j<yeshu; ++j)
	{
		auto pageViewSize = m_ui.pAstrolabe->getSize();
		auto pageViewChirld = Layout::create();
		pageViewChirld->setSize(pageViewSize);

		for (int i=0;i<3;++i)
		{
			int tempSize = i+j*3;
			if (tempSize >= starcampInfo.size())
			{
				break;
			}
			//哪个样子的阵形
			int temp = tempSize%5+1;
			//以星盘ID为确定星盘的样式
			auto rootSon = dynamic_cast<Layout*>(Helper::seekWidgetByName(_rroot,"BPanel_Hero_"+TO_STR(tempSize+1)));
			auto rootSon1 = rootSon->clone();
			rootSon1->setVisible(true);
			
			//更新人物界面
			freshLayerForFiveHero(rootSon1,tempSize);
			

			rootSon1->setAnchorPoint(Vec2(0.5,0.5));
			rootSon1->setPosition(Vec2(pageViewSize.width/6+pageViewSize.width/3*i,pageViewSize.height/2));
			pageViewChirld->addChild(rootSon1);
			//存起来
			vecSonStCamp.push_back(rootSon1);
		}
		m_ui.pAstrolabe->addPage(pageViewChirld);
	}
	
	//初始化星星个数
	auto startPan =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.laStarLayou, "Image_Sart_1"));
	m_ui.laStarLayou->setVisible(true);
	auto Xfirst = -(yeshu-1)*STAR_GAP/2;
	auto conSize = m_ui.laStarLayou->getContentSize();
	for (int i = 0; i < yeshu; i++)
	{
		auto startSon = startPan->clone();
		startSon->setVisible(true);
		auto sonAn =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(startSon, "Image_Sart"));
		sonAn->setVisible(false);
		startSon->setPosition(Vec2(Xfirst+conSize.width/2,conSize.height/2));
		Xfirst+=STAR_GAP;
		m_ui.laStarLayou->addChild(startSon);
		vecStars.push_back(startSon);
	}
	startPan->removeFromParent();

	Director::getInstance()->getScheduler()->schedule(schedule_selector(StarsCampLayer::myupdate), this, 0, false );
}

void StarsCampLayer::freshLayerForFiveHero(Widget* rootSon1,int wz)
{

	auto oneinfo = StarsCampModel::getInstance()->getOndeStarCampInfoByWz(wz);
	auto fiveHero = oneinfo.heroId;
	int mainLevel = HeroModel::getInstance()->getMainHeroLevel();
	int goLevel = oneinfo.goLevel;
	//封印,开启动画等
	auto Image_Open = dynamic_cast<ImageView*>(Helper::seekWidgetByName(rootSon1,"Image_Open"));
	auto Label_Number = dynamic_cast<Text*>(Helper::seekWidgetByName(rootSon1,"Label_Number"));
	auto Image_New = dynamic_cast<ImageView*>(Helper::seekWidgetByName(rootSon1,"Image_New"));
	Image_New->setVisible(false);
	if (mainLevel< goLevel)
	{
		Image_Open->setVisible(true);
		Label_Number->setVisible(true);
		Label_Number->setFontName(FONT_FZZHENGHEI);
		Label_Number->setString(TO_STR(goLevel));
	}
	else
	{
		Image_Open->setVisible(false);
		Label_Number->setVisible(false);	
	}
	//设置底图显示
	auto rootSon2 = dynamic_cast<Button*>(Helper::seekWidgetByName(rootSon1,"Button_Astrolabe"));
	rootSon2->addTouchEventListener(CC_CALLBACK_2(StarsCampLayer::onBtnClick,this,SC_CallBack::SC_GotoStarsSon,wz));

	string name1 = ResourceUtils::getStarCampDiPathByStr(TO_STR(oneinfo.type));
	string name2 = ResourceUtils::getStarCampDiPathByStr(TO_STR(oneinfo.type)+"a");
	rootSon2->loadTextures(name1,name2,name2,ui::Widget::TextureResType::LOCAL);
	//五个英雄头像
	for (int k=0; k<FIVEHERO; ++k)
	{
		Layout *imgLine = dynamic_cast<Layout*>(Helper::seekWidgetByName(rootSon1, "Panel_Hero_" + TO_STR(k+1)));
		Button *imaButton = dynamic_cast<Button*>(Helper::seekWidgetByName(imgLine, "Button_Hero"));
		imgLine->setTouchEnabled(false);
		imaButton->setTouchEnabled(false);
		//星盘英雄头像
		auto hero = HeroModel::getInstance()->searchHeroByTemplateId(fiveHero.at(k).uId);
		ImageView* icon = ImageView::create(ResourceUtils::getStarCampPathById(fiveHero.at(k).uId));

		if (mainLevel< goLevel)
		{
			setImageGray(icon,true);
		}
		if (hero == nullptr)
		{
			setImageGray(icon,true);
		}
		icon->setScale(0.65f);
		icon->setPosition(Vec2(imgLine->getContentSize().width/2,imgLine->getContentSize().height/2));
		icon->setZOrder(-1);
		imgLine->addChild(icon);
	}
}

void StarsCampLayer::myupdate(float dt)
{ 
	auto index = m_ui.pAstrolabe->getCurPageIndex();
	auto size = m_ui.pAstrolabe->getPages().size();


	if (index == 0 && size != 1)
	{
		m_ui.btnLeft->setVisible(false);
		m_ui.btnRight->setVisible(true);
	}
	else if (index == 0 && size == 1)
	{
		m_ui.btnLeft->setVisible(false);
		m_ui.btnRight->setVisible(false);
	}
	else if (index !=0 &&(index<size-1) && size != 1)
	{
		m_ui.btnLeft->setVisible(true);
		m_ui.btnRight->setVisible(true);
	}
	else if (index !=0 && index ==size-1 && size != 1 )
	{
		m_ui.btnLeft->setVisible(true);
		m_ui.btnRight->setVisible(false);
	}

	auto tempSon = vecStars.at(index);
	auto tempSonAn =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(tempSon, "Image_Sart"));

	if (tempSon)
	{
		for (int i = 0; i < vecStars.size(); i++)
		{
			auto tempSon1 = vecStars.at(i);
			auto sonAn =  dynamic_cast<ImageView*>(Helper::seekWidgetByName(tempSon1, "Image_Sart"));
			sonAn->setVisible(false);
		}
		tempSonAn->setVisible(true);
	}
	
}

void StarsCampLayer::setNewCampAnimation(int newCampId, bool openOrclose)
{
	if (newCampId == -1)
	{
		return;
	}

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Ani_xingPanBeiJing/Ani_xingPanBeiJing.ExportJson");

	auto newCamp = vecSonStCamp.at(newCampId);
	auto Image_Open = dynamic_cast<ImageView*>(Helper::seekWidgetByName(newCamp,"Image_Open"));
	auto Label_Number = dynamic_cast<Text*>(Helper::seekWidgetByName(newCamp,"Label_Number"));
	auto Image_New = dynamic_cast<ImageView*>(Helper::seekWidgetByName(newCamp,"Image_New"));

	//新开启没占击过去
	if (openOrclose)
	{
		Image_New->setVisible(true);
		Image_Open->setVisible(false);
		Label_Number->setVisible(false);	
		//动画
		auto Button_Astrolabe = dynamic_cast<Button*>(Helper::seekWidgetByName(newCamp,"Button_Astrolabe"));

		Armature* armature = Armature::create("Ani_xingPanBeiJing");
		armature->getAnimation()->play("Animation1",-1,1);
		armature->setScale(0.3f);
		armature->setPosition(Vec2(Button_Astrolabe->getContentSize().width/2,Button_Astrolabe->getContentSize().height/2));
		Button_Astrolabe->addChild(armature);
	}
	else
	{
		
	}
}



