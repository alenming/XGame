#include "AllPvpLayer.h"
#include "MainModule/MainCityScene.h"
#include "Widget/LayerCommHeadInfo.h"
#include "PvpModule/PvpScene.h"
#include "RobModule/SceneRob.h"

AllPvpLayer::AllPvpLayer(void)
{
	
}

AllPvpLayer::~AllPvpLayer(void)
{

}

void AllPvpLayer::onEnter()
{
	Layer::onEnter();
}

void AllPvpLayer::onExit()
{
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIJingjichang");
	Layer::onExit();
}


bool AllPvpLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	initUI();
	setListView();

	return true;
}

void AllPvpLayer::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 加载Sign UI;
	string strFileName;
	strFileName.assign("Image/UIJingjichang/UIArena_di.ExportJson");
	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.pRoot);
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");	
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

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_PVPALL);
	this->addChild(_layerCommHeadInfo);

	m_ui.listView = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.pRoot,"ListView_13"));
	m_ui.item = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Button_di"));
	m_ui.item->setVisible(false);
}

void AllPvpLayer::setListView()
{
	m_ui.listView->removeAllChildren();
	//信息表
	multimap<int, RowData>& DToperatMap = DataManager::getInstance()->getTablePvpAllShow()->getTableData();
	for(auto iter = DToperatMap.begin(); iter != DToperatMap.end(); iter++)
	{
		RowData* rowData = &(iter->second);
		Button* layer = dynamic_cast<Button*>(m_ui.item->clone());
		ImageView *Image_icon = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Image_icon"));
		Image_icon->loadTexture("Image/UIJingjichang/icon/"+ rowData->getStringData("icon") + ".png");
		Layout *Panel_all = dynamic_cast<Layout*>(Helper::seekWidgetByName(layer,"Panel_all"));
		Text* Label_miaoshu = dynamic_cast<Text*>(Helper::seekWidgetByName(Panel_all,"Label_miaoshu"));
		ImageView *Image_zhezhao = dynamic_cast<ImageView*>(Helper::seekWidgetByName(layer,"Image_zhezhao"));
		Text* Label_kaiqi = dynamic_cast<Text*>(Helper::seekWidgetByName(Image_zhezhao,"Label_kaiqi"));

		vector<int> vecDst;
		StringFormat::parseCsvStringVec(rowData->getStringData("show"), vecDst);

		int openlv;
		int id = rowData->getIntData("id");
		bool isOpen = openLv(rowData->getIntData("openLv"),openlv);
		if (isOpen)
		{
			Panel_all->setVisible(true);
			Image_zhezhao->setVisible(false);
			Label_miaoshu->setFontName(FONT_FZZHENGHEI);
			Label_miaoshu->setString("         " + rowData->getStringData("des"));
			setTextAddStroke(Label_miaoshu,Color3B(0x5e,0x04,0x00),2);
			
			for (int j = 0; j < vecDst.size(); j++)
			{
				Layout *jiangli = dynamic_cast<Layout*>(Helper::seekWidgetByName(Panel_all,"jiangli" + TO_STR(j+1)));
				UIToolHeader *icon = UIToolHeader::create(vecDst.at(j));
				icon->setAnchorPoint(Vec2(0,0));
				icon->setScale(0.8f);
				icon->setTipsEnabled(true);
				jiangli->addChild(icon);
			}
		}
		else
		{
			Panel_all->setVisible(false);
			Image_zhezhao->setVisible(true);
			Label_kaiqi->setFontName(FONT_FZZHENGHEI);
			//Label_kaiqi->setString(TO_STR(openlv) + STR_UTF8("级开放"));
			Label_kaiqi->setString(TO_STR(openlv) + DataManager::getInstance()->searchCommonTexdtById(10001));
			setTextAddStroke(Label_kaiqi,Color3B(0x77,0x1d,0x3d),2);
		}

		auto back = [=](Ref* sender, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (type == Widget::TouchEventType::ENDED)
			{
				if (isOpen)
				{
					if (id == 1) //pvp
					{
						Director::getInstance()->pushScene(PvpScene::create(true));
					}
					else if (id == 2) //掠夺
					{
						Director::getInstance()->pushScene(SceneRob::create(true));
					}
				}
				else
				{
					//CustomTips::show(TO_STR(openlv) + "级开放");
					CustomTips::showUtf8(TO_STR(openlv) + DataManager::getInstance()->searchCommonTexdtById(10001));
				}
			}
		};

		layer->addTouchEventListener(back);
		layer->setVisible(true);
		m_ui.listView->pushBackCustomItem(layer);
	}

	m_ui.listView->refreshView();
	m_ui.listView->jumpToLeft();
}

bool AllPvpLayer::openLv(int type,int& lv)
{
	bool ret = false;
	RowData* rowData = DataManager::getInstance()->searchFunctionLevelById(type);
	if (rowData)
	{
		int currentLevel = HeroModel::getInstance()->getMainHero()->mLevel;
		lv = rowData->getIntData("avlLv");
		if (currentLevel >= lv)
		{
			ret = true;
		}
		else
		{
			ret = false;
		}
	}
	return ret;
}
