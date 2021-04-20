#include "MarvellousActiveLayer.h"
#include "RobModule/SceneRob.h"
#include "BossModule/BossScene.h"
#include "ChapterModule/Activity/SceneActivity.h"
#include "MazeModule/MazeModel.h"

MarvellousActiveLayer::MarvellousActiveLayer()
{
	isfirstGetInfo = false;
	m_isAddScrollViewEvent = false;
	MarvellousActiveModel::getInstance()->addObserver(this);
}

MarvellousActiveLayer::~MarvellousActiveLayer()
{
	MarvellousActiveModel::getInstance()->removeObserver(this);
}

bool MarvellousActiveLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	
	initUi();
		
	MarvellousActiveController::getMaInfo();

	return true;
}

void MarvellousActiveLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UI_BestAct/UI_BestAct.ExportJson"));
	this->addChild(m_ui.pRoot);	
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");

	m_ui.pListViewMa = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.pRoot,"ListView"));
	m_ui.Img_Left = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Left"));
	m_ui.Img_Right = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Right"));
	m_ui.Img_Left->setVisible(false);
	m_ui.Img_Right->setVisible(true);

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

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_MA);
	_layerCommHeadInfo->setPosition(Vec2(-pos.x,pos.y));
	m_ui.pRoot->addChild(_layerCommHeadInfo);
}

void MarvellousActiveLayer::onEnter()
{
	Layer::onEnter();
}

void MarvellousActiveLayer::onExit()
{
	Layer::onExit();
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(MarvellousActiveLayer::updateCountDown), this);

	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UI_BestAct");
}

void MarvellousActiveLayer::updateCountDown(float delta)
{
	MA_INFO_VEC& maInfo = MarvellousActiveModel::getInstance()->getMaInfo();
	for (int i = 0; i < maInfo.size(); i++)
	{
		if (maInfo.at(i).time < 0)
		{
			continue;
		}
		else if (maInfo.at(i).time > 0)
		{
			maInfo.at(i).time--;
		}

		if (maInfo.at(i).time == 0)
		{
			MarvellousActiveController::getMaInfo();
			continue;
		}

		setListTime(i);
	}
}

void MarvellousActiveLayer::setListTime(int id)
{
	MA_INFO_VEC& maInfo = MarvellousActiveModel::getInstance()->getMaInfo();
		
	Widget *itme = m_ui.pListViewMa->getItem(id);
	Text *Lab_CD = dynamic_cast<Text*>(Helper::seekWidgetByName(itme,"Lab_CD"));
	Text *Lab_Time = dynamic_cast<Text*>(Helper::seekWidgetByName(itme,"Lab_Time"));
	ImageView *Img_Open = dynamic_cast<ImageView*>(Helper::seekWidgetByName(itme,"Img_Open"));

	if (maInfo.at(id).time <= 0)
	{
		Lab_Time->setVisible(false);
		Lab_CD->setVisible(false);
	}
	else 
	{
		Lab_Time->setVisible(true);
		Lab_CD->setVisible(true);

		long lremainderSec	= maInfo.at(id).time;
		int nHour			= lremainderSec/3600;
		int nMinute			= (lremainderSec%3600) / 60;
		int nSecond			= (lremainderSec%3600) % 60;
		char szRemTime[100];
		sprintf(szRemTime, "%02d:%02d:%02d", nHour, nMinute, nSecond);

		if (maInfo.at(id).flg == 0)
		{
			Lab_Time->setString(DataManager::getInstance()->searchCommonTexdtById(20273));
			Img_Open->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_meitiankaifangshijian.png");
		}
		else
		{
			Lab_Time->setString(DataManager::getInstance()->searchCommonTexdtById(20274));
			Img_Open->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_huodonging.png");
		}
		Lab_CD->setString(szRemTime);
		setTextAddStroke(Lab_Time,Color3B(0x09,0x4e,0x51),2);
		setTextAddStroke(Lab_CD,Color3B(0x09,0x4e,0x51),2);
	}
}

void MarvellousActiveLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
	case MA_EVENT_TYPE_GET_INFO:
		{
			if (!isfirstGetInfo)
			{
				setListView();
				isfirstGetInfo = true;
			}
		}
		break;
	default:
	break;
	}
}

void MarvellousActiveLayer::setListView()
{
	MA_INFO_VEC& maInfo = MarvellousActiveModel::getInstance()->getMaInfo();
	m_ui.pListViewMa->removeAllItems();
	for (int i = 0; i < maInfo.size(); i++)
	{
		RowData *maData = DataManager::getInstance()->searchBestActById(maInfo.at(i).id);
		string strFileName;
		strFileName.assign("Image/UI_BestAct/UI_Activity.ExportJson");
		Layout* layer = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
		//对应事件 
		Button *Btn_anniu = dynamic_cast<Button*>(Helper::seekWidgetByName(layer,"Btn_anniu"));
		ImageView *Img_di = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_di"));

		auto callTouch = [this,maInfo,i](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				if (!m_isAddScrollViewEvent)
				{
					m_isAddScrollViewEvent = true;
					dynamic_cast<ScrollView*>(m_ui.pListViewMa)->addEventListener(CC_CALLBACK_2(MarvellousActiveLayer::onScrollViewEvent, this));
				}
			}
			else if (type == Widget::TouchEventType::ENDED)
			{
				SoundUtils::playSoundEffect("dianji");
				switch (maInfo.at(i).id)
				{
				case MA_EVENT_EVERY_DAY_CHANGE: //每日挑战
					if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVE_EVENT))
					{
						Director::getInstance()->replaceScene(SceneActivity::create());
					}
					break;
				case MA_EVENT_MIGONG://迷宫
					if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_MAZE))
					{
						Director::getInstance()->replaceScene(SceneMaze::create());
					}
					break;
				case MA_EVENT_LUEDUO: //掠夺
					if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_ROB))
					{
						Director::getInstance()->replaceScene(SceneRob::create());
					}
					break;
				case MA_EVENT_BOSS:
					if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_BOSS))
					{
						Director::getInstance()->replaceScene(BossScene::create());
					}
					break;
				default:
					break;
				}
			}
		};
		Btn_anniu->addTouchEventListener(callTouch);

		//是否开放
		Layout *Pnl_NotOpen = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Pnl_NotOpen"));
		Text *Lab_Level = dynamic_cast<Text*>(Helper::seekWidgetByName(Pnl_NotOpen,"Lab_Level"));
		ImageView *Img_Open = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_Open"));
		Img_Open->setVisible(false);
		Lab_Level->setFontName(FONT_FZZHENGHEI);
		int level = 0;
		bool  open = openLevel(maData->getIntData("functionLv"),level);
		if (open)
		{
			Pnl_NotOpen->setVisible(false);
			Btn_anniu->setVisible(true);
			Img_di->setVisible(true);
		}
		else
		{
			Pnl_NotOpen->setVisible(true);
			Btn_anniu->setVisible(false);
			Img_di->setVisible(false);

			Lab_Level->setString(TO_STR(level) + DataManager::getInstance()->searchCommonTexdtById(10001));
			setTextAddStroke(Lab_Level,Color3B(0x6e,0x28,0x10),2);
		}
		auto openTouch = [this,level](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
				if (!m_isAddScrollViewEvent)
				{
					m_isAddScrollViewEvent = true;
					dynamic_cast<ScrollView*>(m_ui.pListViewMa)->addEventListener(CC_CALLBACK_2(MarvellousActiveLayer::onScrollViewEvent, this));
				}
			}
			else if (type == Widget::TouchEventType::ENDED)
			{
				CustomTips::show(TO_STR(level) + DataManager::getInstance()->searchCommonTexdtById(10001));
			}
		};
		Pnl_NotOpen->addTouchEventListener(openTouch);

		
		Text *Lab_Time = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Time"));
		Text *Lab_CD = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_CD"));
		Lab_Time->setFontName(FONT_FZZHENGHEI);
		Lab_CD->setFontName(FONT_FZZHENGHEI);
		Lab_Time->setVisible(false);
		Lab_CD->setVisible(false);

		if (open)
		{
			Img_Open->setVisible(true);
		}
		if (maInfo.at(i).time > 0)
		{
			long lremainderSec	= maInfo.at(i).time;
			int nHour			= lremainderSec/3600;
			int nMinute			= (lremainderSec%3600) / 60;
			int nSecond			= (lremainderSec%3600) % 60;
			char szRemTime[100];
			sprintf(szRemTime, "%d:%02d:%02d", nHour, nMinute, nSecond);

			if (maInfo.at(i).flg == 0)
			{
				Lab_Time->setString(DataManager::getInstance()->searchCommonTexdtById(20273));
				Img_Open->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_meitiankaifangshijian.png");
			}
			else
			{
				Lab_Time->setString(DataManager::getInstance()->searchCommonTexdtById(20274));
				Img_Open->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_huodonging.png");
			}
			Lab_CD->setString(szRemTime);
			setTextAddStroke(Lab_Time,Color3B(0x09,0x4e,0x51),2);
			setTextAddStroke(Lab_CD,Color3B(0x09,0x4e,0x51),2);
			Lab_Time->setVisible(true);
			Lab_CD->setVisible(true);
		}
		Director::getInstance()->getScheduler()->schedule(schedule_selector(MarvellousActiveLayer::updateCountDown), this, 1, false);

		//图标
		ImageView *Img_Title = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_Title"));
		ImageView *Img_Icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Img_Icon"));
		setIcon(maInfo.at(i).id,Img_Title,Img_Icon,Img_Open);
		
		//说明
		Text *Lab_Tips1 = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Tips1"));
		Text *Lab_Tips2 = dynamic_cast<Text*>(Helper::seekWidgetByName(layer,"Lab_Tips2"));
		Lab_Tips1->setFontName(FONT_FZZHENGHEI);
		Lab_Tips2->setFontName(FONT_FZZHENGHEI);
		if (maInfo.at(i).time < 0)
		{
			if (maData->getStringData("tips1") == "-1")
			{
				Lab_Tips1->setVisible(false);
			}
			else
			{
				Lab_Tips1->setVisible(true);
				Lab_Tips1->setString(maData->getStringData("tips1"));
				setTextAddStroke(Lab_Tips1,Color3B(0x5e,0x04,0x00),2);
			}

			if (maData->getStringData("tips2") == "-1")
			{
				Lab_Tips2->setVisible(false);
			}
			else
			{
				Lab_Tips2->setVisible(true);
				Lab_Tips2->setString(maData->getStringData("tips2"));
				setTextAddStroke(Lab_Tips2,Color3B(0x5e,0x04,0x00),2);
			}
		}
		else
		{
			Lab_Tips1->setVisible(false);
			Lab_Tips2->setVisible(false);
		}
		
		
		m_ui.pListViewMa->pushBackCustomItem(layer);
	}

	m_ui.pListViewMa->refreshView();
	m_ui.pListViewMa->scrollToLeft(0.01f,false);
}

bool MarvellousActiveLayer::openLevel(int id,int &lv)
{
	bool ret = true;
	RowData* rowData = DataManager::getInstance()->searchFunctionLevelById(id);
	if (rowData)
	{
		int currentLevel = HeroModel::getInstance()->getMainHero()->mLevel;
		int functionLevel = rowData->getIntData("avlLv");
		if (currentLevel >= functionLevel)
		{
			ret = true;
		}
		else
		{
			lv = functionLevel;
			ret = false;
		}
	}
	return ret;
}

void MarvellousActiveLayer::setIcon(int id,ImageView *title,ImageView *icon,ImageView *open)
{
	switch (id)
	{
	case MA_EVENT_EVERY_DAY_CHANGE: //每日挑战
		{
			title->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_biaoti_meiritianzhan.png");
			icon->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_icon_meirihuodong.png");
		}
		break;
	case MA_EVENT_MIGONG://迷宫
		{
			title->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_biaoti_huanjing.png");
			icon->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_icon_huanjing.png");
		}
		break;
	case MA_EVENT_LUEDUO://掠夺
		{
			title->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_biaoti_lveduo.png");
			icon->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_icon_lveduo.png");
		}
		break;
	case MA_EVENT_BOSS://绝地反击
		{
			title->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_biaoti_juedifanji.png");
			icon->loadTexture("Image/UI_BestAct/UI_jingcaihuodong/img_icon_juedifanji.png");
		}
		break;
	default:
		break;
	}
}

void MarvellousActiveLayer::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	switch(type)
	{
	case cocos2d::ui::ScrollView::EventType::SCROLL_TO_LEFT:
		{
			m_ui.Img_Left->setVisible(false);
			m_ui.Img_Right->setVisible(true);
		}
		break;
	case cocos2d::ui::ScrollView::EventType::SCROLL_TO_RIGHT:
		{
			m_ui.Img_Left->setVisible(true);
			m_ui.Img_Right->setVisible(false);
		}
		break;
	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			m_ui.Img_Left->setVisible(true);
			m_ui.Img_Right->setVisible(true);
		}
		break;
	default:
		break;
	}
}
