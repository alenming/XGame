#include "GoldActivityLayer.h"
#include "MainModule/MainCityScene.h"
#include "Widget/LayerCommHeadInfo.h"

GoldActivityLayer::GoldActivityLayer(void)
{
	GoldActivityModel::getInstance()->addObserver(this);
	CCNotificationCenter::getInstance()->addObserver(this,callfuncO_selector(GoldActivityLayer::getMoney),"getMoney",NULL);
}

GoldActivityLayer::~GoldActivityLayer(void)
{
	GoldActivityModel::getInstance()->removeObserver(this);
	CCNotificationCenter::getInstance()->removeAllObservers(this);
}

void GoldActivityLayer::onEnter()
{
	Layer::onEnter();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(GoldActivityLayer::updateNextFram),this,0,false);
}

void GoldActivityLayer::updateNextFram(float ft)
{
	//GoldActivityController::getInstance()->getTableInfo();
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(GoldActivityLayer::updateNextFram), this);
}

void GoldActivityLayer::onExit()
{
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UIGoldActivity");
	Layer::onExit();
}


bool GoldActivityLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	initUI();

	GoldActivityController::getInstance()->getTableInfo();

	return true;
}

void GoldActivityLayer::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 加载Sign UI;
	string strFileName;
	strFileName.assign("Image/UIGoldActivity/UIGoldActivity.ExportJson");
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
			//迷宫退出
			Director::getInstance()->replaceScene(MainCityScene::create());
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_DAY_DOWN_PAYMENT);
	this->addChild(_layerCommHeadInfo);
	m_ui.Pnl_GoldActivity = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_GoldActivity"));
	m_ui.Img_Show = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Show"));
	m_ui.Img_Show->loadTexture("Image/Icon/integrityCard/41042.png");
	m_ui.Img_di = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_di"));
	m_ui.Img_Best = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Img_di,"Img_Best"));
	m_ui.Img_Residue = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Img_di,"Img_Residue"));
	m_ui.Img_Cost = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Img_di,"Img_Cost"));
	m_ui.Lab_Best = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_di,"Lab_Best"));
	m_ui.Lab_Residue = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_di,"Lab_Residue"));
	m_ui.Lab_Cost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_di,"Lab_Cost"));
	m_ui.Lab_TimeA = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_di,"Lab_TimeA"));
	m_ui.Lab_TimeB = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_di,"Lab_TimeB"));
	m_ui.Btn_Play = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.Img_di,"Btn_Play"));
	m_ui.Btn_Play->addTouchEventListener(CC_CALLBACK_2(GoldActivityLayer::onBtnClick,this));
	m_ui.Lab_Vtips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_di,"Lab_Vtips"));
	m_ui.Lab_Vtips->setVisible(false);
	m_ui.Lab_TipsNotime = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_di,"Lab_Tips"));
	m_ui.Lab_Best->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_Residue->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_Cost->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_TimeA->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_TimeB->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_Vtips->setFontName(FONT_FZZHENGHEI);
	m_ui.Lab_TipsNotime->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.Lab_TipsNotime,Color3B(0x00,0x00,0x00),2);
	m_ui.Lab_Cost->setVisible(false);
	m_ui.Lab_Residue->setVisible(false);
	m_ui.Lab_Best->setVisible(false);
	m_ui.Lab_Vtips->setVisible(false);
	m_ui.Lab_TipsNotime->setVisible(false);
	m_ui.Img_Best->setVisible(false);
	m_ui.Img_Residue->setVisible(false);
	m_ui.Img_Cost->setVisible(false);

	m_ui.Pnl_Num = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot,"Pnl_Num"));

	m_ui.Img_Tipsdi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Tipsdi"));
	m_ui.Lab_TipsChance = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.Img_Tipsdi,"Lab_Tips"));
	m_ui.Lab_TipsChance->setFontName(FONT_FZZHENGHEI);

	m_ui.ListView_news = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.pRoot,"ListView_news"));

	m_rollNum = RollNumGroup::createWithGameLayer(m_ui.Pnl_Num,5);
	m_rollNum->setPosition(Vec2(18,35));

	showDownTime();
}

void GoldActivityLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	
	switch (observerParam->id)
	{
	case GoldActivityNotify_Type_Info:
		{
			setPanelInfo();
			setOtherGetGold();

			setSpendGoldInfo();
		}
		break;
	case GoldActivityNotify_Type_ChangeGold:
		{
			setGetGold();
			setPanelInfo();
			setOtherGetGold();
		}
		break;
	case GoldActivityNotify_Type_OtherChangeGold:
		{
			setOtherGetGold();
		}
		break;
	case GoldActivityNotify_Type_Timer:
		{
			showDownTime();
		}
		break;
	default:
		break;
	}
}

void GoldActivityLayer::showDownTime()
{
	GoldActivityFormatTime times;
	if(GoldActivityModel::getInstance()->getActOverSec() <= 0)
	{

		//times.strDays= STR_UTF8("0天");
		//times.strHMS = STR_UTF8("00时00分00秒");
		times.strDays= "0" + DataManager::getInstance()->searchCommonTexdtById(10150);
		times.strHMS = "00" + DataManager::getInstance()->searchCommonTexdtById(10151)
					 + "00" + DataManager::getInstance()->searchCommonTexdtById(10152);
					 + "00" + DataManager::getInstance()->searchCommonTexdtById(10153);
	}
	else
	{
		formatDetailsTime(GoldActivityModel::getInstance()->getActOverSec(),times);
	}

	m_ui.Lab_TimeA->setString(times.strDays);
	m_ui.Lab_TimeB->setString(times.strHMS);

	setTextAddStroke(m_ui.Lab_TimeA, Color3B(0x2d, 0x5b, 0x28), 2);
	setTextAddStroke(m_ui.Lab_TimeB, Color3B(0x09, 0x4e, 0x51), 2);
}

void GoldActivityLayer::formatDetailsTime(int secs,GoldActivityFormatTime& time)
{
	if(secs <= 0)
	{
		//time.strDays = STR_UTF8("0天");
		//time.strHMS = STR_UTF8("00时00分00秒");
		time.strDays= "0" + DataManager::getInstance()->searchCommonTexdtById(10150);
		time.strHMS = "00" + DataManager::getInstance()->searchCommonTexdtById(10151)
			+ "00" + DataManager::getInstance()->searchCommonTexdtById(10152);
		+ "00" + DataManager::getInstance()->searchCommonTexdtById(10153);
		return;
	}

	int days = secs / (24*3600);
	int hours = (secs % (24*3600)) / 3600;
	int mins = (secs - days*24*3600 - hours*3600) / 60;
	int sec = secs - days*24*3600 - hours*3600 - mins*60;

	string strHours = "";
	string strMins = "";
	string strSecs = "";

	if(hours <= 9)
	{
		strHours = _TO_STR(0) + _TO_STR(hours);
	}
	else
	{
		strHours = _TO_STR(hours);
	}

	if(mins <= 9)
	{
		strMins = _TO_STR(0) + _TO_STR(mins);
	}
	else
	{
		strMins = _TO_STR(mins);
	}

	if(sec <= 9)
	{
		strSecs = _TO_STR(0) + _TO_STR(sec);
	}
	else
	{
		strSecs = _TO_STR(sec);
	}

	time.strDays = TO_STR(days) + DataManager::getInstance()->searchCommonTexdtById(10150);
	time.strHMS = strHours + DataManager::getInstance()->searchCommonTexdtById(10151)
				 + strMins + DataManager::getInstance()->searchCommonTexdtById(10152)
				 + strSecs + DataManager::getInstance()->searchCommonTexdtById(10153);
}

void GoldActivityLayer::onBtnClick(Ref* sender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (sender == m_ui.Btn_Play)
		{
			int changeCount = GoldActivityModel::getInstance()->getChangeCount(); //已兑换次数
			GoldActivityTable GoldTbale = GoldActivityModel::getInstance()->getGoldTbale();//数据

			if (GoldActivityModel::getInstance()->getActOverSec() <= 0)
			{
				//CustomTips::show("活动已结束");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10023));
			}
			else if (changeCount >= GoldTbale.vipVec.size())
			{
				//CustomTips::show("次数已用完");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10154));
			}
			else if (MainModel::getInstance()->getMainParam()->mVipLevel < GoldTbale.vipVec.at(changeCount))
			{
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10155), [this](Ref* pSender, CustomRetType type)->void
				{
					switch (type)
					{
					case RET_TYPE_OK:
						{
							Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
						}
						break;
					default:
						break;
					}

				}, CUSTOM_YES_NO);
			}
			else if (MainModel::getInstance()->getMainParam()->mGold < GoldTbale.costVec.at(changeCount))
			{
				//充值界面
				CustomGoldTips *_tips = CustomGoldTips::create();
				if (_tips)
				{
					Director::getInstance()->getRunningScene()->addChild(_tips);
				}
			}
			else 
			{
				if(m_rollNum->setRun())
				{
					GoldActivityController::getInstance()->changeGold();
				}

				GoldActivityTable GoldTbale = GoldActivityModel::getInstance()->getGoldTbale();//数据
				int changeCount = GoldActivityModel::getInstance()->getChangeCount(); //已兑换次数
				m_ui.Lab_Residue->setString(TO_STR(MainModel::getInstance()->getMainParam()->mGold - GoldTbale.costVec.at(changeCount)));
				setTextAddStroke(m_ui.Lab_Residue,Color3B(0x00,0x00,0x00),2);
				m_ui.Lab_Residue->setPosition(Vec2(m_ui.Img_Residue->getPosition().x + m_ui.Img_Residue->getContentSize().width + 
					m_ui.Lab_Residue->getContentSize().width/2,m_ui.Lab_Residue->getPosition().y));
			}
		}
	}
}

void GoldActivityLayer::setPanelInfo()
{
	GoldActivityTable GoldTbale = GoldActivityModel::getInstance()->getGoldTbale();//数据
	int changeCount = GoldActivityModel::getInstance()->getChangeCount(); //已兑换次数

	if (changeCount >= GoldTbale.vipVec.size())
	{
		m_ui.Lab_Cost->setVisible(false);
		m_ui.Lab_Residue->setVisible(false);
		m_ui.Lab_Best->setVisible(false);
		m_ui.Lab_Vtips->setVisible(false);
		m_ui.Lab_TipsNotime->setVisible(true);
		m_ui.Img_Best->setVisible(false);
		m_ui.Img_Residue->setVisible(false);
		m_ui.Img_Cost->setVisible(false);
		m_ui.Btn_Play->setBright(false);
	}
	else
	{
		m_ui.Lab_Cost->setVisible(true);
		m_ui.Lab_Residue->setVisible(true);
		m_ui.Lab_Best->setVisible(true);
		m_ui.Lab_TipsNotime->setVisible(false);
		m_ui.Img_Best->setVisible(true);
		m_ui.Img_Residue->setVisible(true);
		m_ui.Img_Cost->setVisible(true);
	}

	if (m_ui.Lab_TipsChance->getChildByName("Tips"))
	{
		m_ui.Lab_TipsChance->removeChildByName("Tips");
	}

	SimpleRichText *richText = SimpleRichText::create();
	if (nullptr != richText)
	{
		richText->ignoreContentAdaptWithSize(false);
		richText->setAnchorPoint(Vec2(0,1));
		richText->setContentSize(m_ui.Lab_TipsChance->getContentSize());
		richText->setPosition(Vec2(-77,120));
		richText->setString(GoldTbale.tips,20,Color3B(0xaf, 0x8f, 0x7d));
		m_ui.Lab_TipsChance->addChild(richText,0,"Tips");
	}
}

void GoldActivityLayer::setGetGold()
{
	auto call = [this]()->void
	{
		m_rollNum->setStop(GoldActivityModel::getInstance()->getGoldNum());
	};
	m_rollNum->runAction(CCSequence::create(CCDelayTime::create(3.0f),CCCallFunc::create(call),nullptr));
}

void GoldActivityLayer::setOtherGetGold()
{
	GoldActivityTable GoldTbale = GoldActivityModel::getInstance()->getGoldTbale();//数据
	//恭喜|0xfffd2f::@|获得|0xffffff::@|金币
	vector<string> desTemp;
	desTemp.clear();
	StringFormat::parseCsvStringVec(GoldTbale.info, desTemp, '@');

	m_ui.ListView_news->removeAllItems();
	deque<OtherPlayerChange> OtherChange = GoldActivityModel::getInstance()->getOtherChange();
	for(int i = 0; i < OtherChange.size(); i++)
	{
		//奖励
		OtherPlayerChange info = OtherChange.at(i); 
		SimpleRichText* pTextLine = SimpleRichText::create();

		string resultStr = "";
		for(int j = 0; j < desTemp.size(); j++)
		{
			resultStr += desTemp.at(j);
			if(j < info.ChangeInfo.size())
			{
				resultStr += info.ChangeInfo.at(j);
			}
		}

		pTextLine->setString(resultStr, 22, Color3B::GREEN);
		pTextLine->formatText();	
		m_ui.ListView_news->pushBackCustomItem(pTextLine);
	}

	m_ui.ListView_news->refreshView();
	m_ui.ListView_news->scrollToBottom(0.01f,false);
}

void GoldActivityLayer::getMoney(Ref* sender)
{
	showPopReward();
	setSpendGoldInfo();

	GoldActivityModel::getInstance()->getMoney();
	m_ui.Lab_Residue->runAction(CCSequence::create(EaseSineOut::create(CCScaleTo::create(0.15f,1.6f)),EaseSineIn::create(CCScaleTo::create(0.1f,1.0f)),nullptr));
}

void GoldActivityLayer::setSpendGoldInfo()
{
	m_rollNum->setCurNum(GoldActivityModel::getInstance()->getGoldNum());
	m_ui.Lab_Residue->setString(TO_STR(MainModel::getInstance()->getMainParam()->mGold));
	setTextAddStroke(m_ui.Lab_Residue,Color3B(0x00,0x00,0x00),2);
	m_ui.Lab_Residue->setPosition(Vec2(m_ui.Img_Residue->getPosition().x + m_ui.Img_Residue->getContentSize().width + 
		m_ui.Lab_Residue->getContentSize().width/2,m_ui.Lab_Residue->getPosition().y));

	GoldActivityTable GoldTbale = GoldActivityModel::getInstance()->getGoldTbale();//数据
	int changeCount = GoldActivityModel::getInstance()->getChangeCount(); //已兑换次数

	if (changeCount < GoldTbale.vipVec.size())
	{

		m_ui.Lab_Best->setString(TO_STR(GoldTbale.showVec.at(changeCount)));
		setTextAddStroke(m_ui.Lab_Best,Color3B(0x00,0x00,0x00),2);

		m_ui.Lab_Cost->setString(TO_STR(GoldTbale.costVec.at(changeCount)));
		setTextAddStroke(m_ui.Lab_Cost,Color3B(0x00,0x00,0x00),2);

		m_ui.Btn_Play->setBright(true);
		if (GoldTbale.vipVec.at(changeCount) <= 0)
		{
			m_ui.Lab_Vtips->setVisible(false);
		}
		else
		{
			m_ui.Lab_Vtips->setVisible(true);
			m_ui.Lab_Vtips->setString("V" + TO_STR(GoldTbale.vipVec.at(changeCount)) + DataManager::getInstance()->searchCommonTexdtById(10156));
			setTextAddStroke(m_ui.Lab_Vtips,Color3B(0x00,0x00,0x00),2);
		}
	}
}

void GoldActivityLayer::showPopReward()
{
	vector<PopRewardItem*> vecRewardItem;
	PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(GoldActivityModel::getInstance()->getGoldNum());
	vecRewardItem.push_back(rewardItem);

	PopReward::show(vecRewardItem);
}