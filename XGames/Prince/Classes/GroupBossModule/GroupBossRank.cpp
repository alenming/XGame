#include "GroupBossRank.h"
#include "GroupBossModel.h"
#include "Communication/Command.h"
#include "Widget/LayerCommHeadInfo.h"
#include "MainModule/MainCityScene.h"
#include "Utils/StringFormat.h"
#include "ToolModule/UIToolHeader.h"
#include "Common/Common.h"
#include "Widget/LayerCommFormation.h"
#include "PvpModule/PvpHeader.h"

//列表tag
#define TAG_ITEM_LIST	1001
#define PICTURT_PATH	"Image/UI_bangpaiBOSS/"
//我的战斗力、我的等级 文本基础X位置（动态调整美观点）
//#define MY_RANK_VALUE_POS_X	0

GroupBossRank::GroupBossRank()
{
	mCurSeledPanelType = eRankType::eTYPE_GROUP;
	mRankTopView = nullptr;
	GroupBossModel::getInstance()->addObserver(this);
}

GroupBossRank::~GroupBossRank(void)
{
	GroupBossModel::getInstance()->removeObserver(this);
}

GroupBossRank* GroupBossRank::create(eRankType type)
{
	GroupBossRank* _layer = new GroupBossRank();
	if (_layer->init(type))
	{
		_layer->autorelease();
		return _layer;
	}
	else
	{
		delete _layer;
		return nullptr;
	}
}

void GroupBossRank::onEnter()
{
	Layer::onEnter();

	auto callBack = [this](float dt)->void
	{;
		this->getScheduler()->unschedule("GroupBossRank::sendMsg", this);
		switchRankTopPanel(mCurSeledPanelType);
		GroupBossModel::getInstance()->setRankTemp(mCurSeledPanelType);

		GroupBossController::getInstance()->sendGetRankMsg(mCurSeledPanelType);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "GroupBossRank::sendMsg");
}

void GroupBossRank::onExit()
{
	Layer::onExit();
}

bool GroupBossRank::init(eRankType type)
{
	if (!Layer::init())
	{
		return false;
	}

	mCurSeledPanelType = type;

	initUI();
	initRankTopUI();
	return true;
}

void GroupBossRank::initUI()
{
	// 加载排行榜 UI;
	string strFileName;
	strFileName.assign("Image/UI_bangpaiBOSS/UI_bangpaiBOSS_Ranking.ExportJson");

	// Root;
	mUI.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	mUI.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(mUI.pRoot);

	//公用返回,待定
	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");
			scheduleUpdate();		
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,	HEAD_INFO_TYPE_CHALLENGE_RANKING);
	this->addChild(_layerCommHeadInfo);

	mUI.mScrollLeftPanel = (ui::ScrollView*)(Helper::seekWidgetByName(mUI.pRoot, "ScrollView_Rank"));
	mUI.mScrollLeftPanel->addEventListener(CC_CALLBACK_2(GroupBossRank::onScrollViewEvent, this));

	for(int i=0; i<MAX_BOSS_NUM; i++)
	{
		mUI.mBtnRankTop[i] = (Button*)(mUI.mScrollLeftPanel->getChildByName("Btn_RankTop" + _TO_STR(i+1)));
		mUI.mBtnRankTop[i]->addTouchEventListener(CC_CALLBACK_2(GroupBossRank::onBtnClick, this, i+1));
	}


	//滚动区域提示箭头
	mUI.mImgArrowUp = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Top"));
	mUI.mImgArrowDown = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Bot"));
	mUI.mImgArrowUp->setVisible(false);
	mUI.mImgArrowDown->setVisible(false);

	//title显示
	mUI.Img_Rank = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Rank"));
	mUI.Img_Name = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Name"));
	mUI.Img_Gang = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Gang"));
	mUI.Img_Value = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Value"));


	//我的帮派
	mUI.Panel_GangRank = (Layout*)(Helper::seekWidgetByName(mUI.pRoot, "Panel_GangRank"));
	mUI.Panel_GangRank->setVisible(false);
	mUI.Panel_GangRank->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	mUI.GangRankRanking = (Text*)(Helper::seekWidgetByName(mUI.Panel_GangRank, "Text_Ranking"));
	mUI.GangRankKilled = (Text*)(Helper::seekWidgetByName(mUI.Panel_GangRank, "Text_Killed"));
	mUI.GangRankdamage = (Text*)(Helper::seekWidgetByName(mUI.Panel_GangRank, "Text_damage"));
	mUI.GangRankRanking->setString("");
	mUI.GangRankKilled->setString("");
	mUI.GangRankdamage->setString("");
	mUI.GangRankRanking->setFontName(FONT_FZZHENGHEI);
	mUI.GangRankKilled->setFontName(FONT_FZZHENGHEI);
	mUI.GangRankdamage->setFontName(FONT_FZZHENGHEI);
	//我的属性
	mUI.Panel_Personal = (Layout*)(Helper::seekWidgetByName(mUI.pRoot, "Panel_Personal"));
	mUI.Panel_Personal->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	mUI.Panel_Personal->setVisible(false);
	mUI.IMG_MyRank = (ImageView*)(Helper::seekWidgetByName(mUI.Panel_Personal, "IMG_MyRank")); 
	mUI.PersonalRanking = (Text*)(Helper::seekWidgetByName(mUI.Panel_Personal, "Text_Ranking"));
	mUI.Personaldamage = (Text*)(Helper::seekWidgetByName(mUI.Panel_Personal, "Text_damage"));

	mUI.PersonalRanking->setString("");
	mUI.Personaldamage->setString("");
	mUI.PersonalRanking->setFontName(FONT_FZZHENGHEI);
	mUI.Personaldamage->setFontName(FONT_FZZHENGHEI);
	//滚动列表容器
	mUI.mPnlList = (Layout*)(Helper::seekWidgetByName(mUI.pRoot, "TableView_pnl"));
	mUI.mTemplateItem = (Layout*)mUI.mPnlList->getChildByName("Pnl_Data");
	mUI.mTemplateItem->setVisible(false);
}

void GroupBossRank::initRankTopUI()
{
	GroupBossModel* instance = GroupBossModel::getInstance();
	
	mRankTopView = new extension::TableView();
	if(mRankTopView)
	{
		mRankTopView->initWithViewSize(mUI.mPnlList->getContentSize(), NULL);
		mRankTopView->autorelease();
		mRankTopView->setDataSource(this);
	}
	mRankTopView->setDirection(extension::ScrollView::Direction::VERTICAL);
	mRankTopView->setVerticalFillOrder(extension::TableView::VerticalFillOrder::TOP_DOWN);
	mRankTopView->reloadData();
	mRankTopView->setDelegate(this);

	mRankTopView->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	mUI.mPnlList->addChild(mRankTopView);
}

void GroupBossRank::setCommonUI()
{
	auto instance = GroupBossModel::getInstance();

	//排行榜类型
	switch (mCurSeledPanelType)
	{
	case eRankType::eTYPE_GROUP:
		{
			mUI.Img_Name->loadTexture(string(PICTURT_PATH)+string("img_bangpaimingcheng.png"));
			mUI.Img_Gang->loadTexture(string(PICTURT_PATH)+string("img_jibaiboss1.png"));

			//我的名次和数据
			mUI.Panel_Personal->setVisible(false);
			mUI.Panel_GangRank->setVisible(true);
			string name = (instance->getMyGroupRankData()->rank == 0) ? DataManager::getInstance()->searchCommonTexdtById(10031) : (_TO_STR(instance->getMyGroupRankData()->rank));
			mUI.GangRankRanking->setString(name);
			mUI.GangRankKilled->setString(TO_STR(instance->getMyGroupRankData()->killedBossNum));
			mUI.GangRankdamage->setString((instance->getMyGroupRankData()->damageVal));

			//mUI.GangRankRanking->setString("aaa");
			setTextAddStroke(mUI.GangRankRanking, Color3B(0x00, 0x00, 0x00), 2);
			//mUI.GangRankKilled->setString("999");
			setTextAddStroke(mUI.GangRankKilled, Color3B(0x00, 0x00, 0x00), 2);
			//mUI.GangRankdamage->setString("12345678");
			setTextAddStroke(mUI.GangRankdamage, Color3B(0x00, 0x00, 0x00), 2);
		}		
		break;
	case eRankType::eTYPE_ALL:
		{
			mUI.Img_Name->loadTexture(string(PICTURT_PATH)+string("img_wanjianicheng.png"));
			mUI.Img_Gang->loadTexture(string(PICTURT_PATH)+string("img_dengji.png"));

			//我的名次和数据
			mUI.Panel_Personal->setVisible(true);
			mUI.Panel_GangRank->setVisible(false);

			mUI.IMG_MyRank->loadTexture(string(PICTURT_PATH)+string("img_wodequanfupaiming.png"));
			string name = instance->getMyRankData(eRankType::eTYPE_ALL)->rank == 0 ? DataManager::getInstance()->searchCommonTexdtById(10031) :
				_TO_STR(instance->getMyRankData(eRankType::eTYPE_ALL)->rank);
			mUI.PersonalRanking->setString(name);
			mUI.Personaldamage->setString(instance->getMyRankData(eTYPE_ALL)->damageVal);
			//mUI.PersonalRanking->setString("123");
			//mUI.Personaldamage->setString("12345678");
			setTextAddStroke(mUI.PersonalRanking, Color3B(0x00, 0x00, 0x00), 2);
			setTextAddStroke(mUI.Personaldamage, Color3B(0x00, 0x00, 0x00), 2);
		}	
		break;
	case eRankType::eTYPE_IN_GROUP:
		{
			mUI.Img_Name->loadTexture(string(PICTURT_PATH)+string("img_wanjianicheng.png"));
			mUI.Img_Gang->loadTexture(string(PICTURT_PATH)+string("img_dengji.png"));

			//我的名次和数据
			mUI.Panel_Personal->setVisible(true);
			mUI.Panel_GangRank->setVisible(false);

			mUI.IMG_MyRank->loadTexture(string(PICTURT_PATH)+string("img_wodebangneipaiming.png"));
			string name = instance->getMyRankData(eRankType::eTYPE_IN_GROUP)->rank == 0 ? DataManager::getInstance()->searchCommonTexdtById(10031) :
				_TO_STR(instance->getMyRankData(eRankType::eTYPE_IN_GROUP)->rank);
			mUI.PersonalRanking->setString(name);
			mUI.Personaldamage->setString(instance->getMyRankData(eTYPE_IN_GROUP)->damageVal);
			//mUI.PersonalRanking->setString("123");
			//mUI.Personaldamage->setString("12345678");
			setTextAddStroke(mUI.PersonalRanking, Color3B(0x00, 0x00, 0x00), 2);
			setTextAddStroke(mUI.Personaldamage, Color3B(0x00, 0x00, 0x00), 2);
		}	
		break;
	default:
		break;
	}
}

void GroupBossRank::setCellContent(extension::TableViewCell* cell, ssize_t idx)
{
	Layout* item = (Layout*)cell->getChildByTag(TAG_ITEM_LIST);
	if(!item)
	{
		return;
	}
	ImageView* rankImg = (ImageView*)item->getChildByName("IMG_rank");//前三名
	Text* Text_rank = (Text*)rankImg->getChildByName("Text_rank");//后面的数字排名
	Layout* Panel_head = (Layout*)item->getChildByName("Panel_head");
	ImageView* IMG_VIP = (ImageView*)item->getChildByName("IMG_VIP");//vip
	Text*	Text_name = (Text*)item->getChildByName("Text_name");//帮派名字或者玩家名字	
	Text*	Text_KilledBoss = (Text*)item->getChildByName("Text_KilledBoss"); //击杀BOSS或者玩家等级
	Text*	Text_name_1 = (Text*)item->getChildByName("Text_name_1");//伤害

	//清空头像容器
	Panel_head->removeAllChildren();	

	Text_name->setColor(Color3B(0xe8, 0xde, 0xb5));
	Text_name_1->setColor(Color3B(0xFF, 0xE9, 0x34));
	Text_KilledBoss->setColor(Color3B(0xFF, 0xE9, 0x34));

	Text_rank->setFontName(FONT_FZZHENGHEI);
	Text_name->setFontName(FONT_FZZHENGHEI);
	Text_KilledBoss->setFontName(FONT_FZZHENGHEI);
	Text_name_1->setFontName(FONT_FZZHENGHEI);

	Text_rank->setString("");
	Text_name->setString("");
	Text_KilledBoss->setString("");
	Text_name_1->setString("");


	GroupBossModel* instance = GroupBossModel::getInstance();

	stringstream ss;

	switch (mCurSeledPanelType)
	{
	case eTYPE_GROUP:
		{
			vector<sGroupDamRank>* vecRankInfo = instance->getGroupRankData();
			if(idx < 3)
			{
				rankImg->loadTexture(string("Image/Icon/global/img_") + _TO_STR(vecRankInfo->at(idx).rank) + string("ming.png"));
				Text_rank->setVisible(false);
			}
			else
			{
				rankImg->loadTexture("Image/Icon/global/img_4ming.png");
				Text_rank->setString(_TO_STR(vecRankInfo->at(idx).rank));
				
				Text_rank->setVisible(true);
			}
			RowData* flagResData = DataManager::getInstance()->searchEmblemById(vecRankInfo->at(idx).flagResId);
			if(flagResData)
			{
				string name = flagResData->getStringData("resId");
				ss << "Image/Icon/Flag/" << name << ".png";
			}
			ImageView* icon = ImageView::create(ss.str());
			if(icon)
			{
				icon->setPosition(Panel_head->getContentSize()/2);
				icon->setScale(0.46f);
				Panel_head->addChild(icon);
			}
			IMG_VIP->setVisible(false);
			//击杀数量
			Text_KilledBoss->setString(_TO_STR(vecRankInfo->at(idx).killedBossNum));
			setTextAddStroke(Text_KilledBoss, Color3B(0x7d, 0x1f, 0x3d), 2);
			Text_KilledBoss->setVisible(true);
			//帮派名字
			Text_name->setString(vecRankInfo->at(idx).groupName);
			setTextAddStroke(Text_name, Color3B(0x7d, 0x1f, 0x3d), 2);
			//伤害
			Text_name_1->setString((vecRankInfo->at(idx).damageVal));

		}
		break;
	case eTYPE_ALL:
		{
			vector<sPlayerDamRank>* vecplayRankInfo1 = instance->getPlayerRankData(eRankType::eTYPE_ALL);
			if(idx < 3)
			{
				rankImg->loadTexture(string("Image/Icon/global/img_") + _TO_STR(vecplayRankInfo1->at(idx).rank) + string("ming.png"));
				Text_rank->setVisible(false);
			}
			else
			{
				rankImg->loadTexture("Image/Icon/global/img_4ming.png");
				Text_rank->setString(_TO_STR(vecplayRankInfo1->at(idx).rank));
				Text_rank->setVisible(true);
			}
			UIToolHeader* header = UIToolHeader::create(vecplayRankInfo1->at(idx).headerResId);
			header->setAnchorPoint(Point(0.5, 0.5));
			header->setPosition(Panel_head->getContentSize()/2);
			header->setScale(0.8f);
			Panel_head->addChild(header);
			//vip
			IMG_VIP->setVisible(true);		
			IMG_VIP->loadTexture("Image/Icon/vip/btn_VIP" + _TO_STR(vecplayRankInfo1->at(idx).vipLv) + ".png");
			//名字
			Text_name->setString(vecplayRankInfo1->at(idx).playerName);
			setTextAddStroke(Text_name, Color3B(0x7d, 0x1f, 0x3d), 2);
			//等级
			Text_KilledBoss->setString(TO_STR(vecplayRankInfo1->at(idx).level));
			setTextAddStroke(Text_KilledBoss, Color3B(0x7d, 0x1f, 0x3d), 2);
			//伤害
			Text_name_1->setString(vecplayRankInfo1->at(idx).damageVal);
			Text_name_1->setColor(Color3B(0xFF, 0xE9, 0x34));
		}
		break;
	case eTYPE_IN_GROUP:
		{
			vector<sPlayerDamRank>* vecplayRankInfo2 = instance->getPlayerRankData(eRankType::eTYPE_IN_GROUP);
			if(idx < 3)
			{
				rankImg->loadTexture(string("Image/Icon/global/img_") + _TO_STR(vecplayRankInfo2->at(idx).rank) + string("ming.png"));
				Text_rank->setVisible(false);
			}
			else
			{
				rankImg->loadTexture("Image/Icon/global/img_4ming.png");
				Text_rank->setString(_TO_STR(vecplayRankInfo2->at(idx).rank));
				Text_rank->setVisible(true);
			}
			UIToolHeader* header = UIToolHeader::create(vecplayRankInfo2->at(idx).headerResId);
			header->setAnchorPoint(Point(0.5, 0.5));
			header->setPosition(Panel_head->getContentSize()/2);
			header->setScale(0.8f);
			Panel_head->addChild(header);
			//vip
			IMG_VIP->setVisible(true);		
			IMG_VIP->loadTexture("Image/Icon/vip/btn_VIP" + _TO_STR(vecplayRankInfo2->at(idx).vipLv) + ".png");
			//名字
			Text_name->setString(vecplayRankInfo2->at(idx).playerName);
			setTextAddStroke(Text_name, Color3B(0x7d, 0x1f, 0x3d), 2);
			//等级
			Text_KilledBoss->setString(TO_STR(vecplayRankInfo2->at(idx).level));
			setTextAddStroke(Text_KilledBoss, Color3B(0x7d, 0x1f, 0x3d), 2);
			//伤害
			Text_name_1->setString(vecplayRankInfo2->at(idx).damageVal);
			Text_name_1->setColor(Color3B(0xFF, 0xE9, 0x34));
		}
		break;
	default:
		break;
	}
}

extension::TableViewCell* GroupBossRank::tableCellAtIndex(extension::TableView *table, ssize_t idx)
{
	//重用cell队列
	extension::TableViewCell* cell = table->dequeueCell();
	if(!cell)
	{
		//创建cell
		cell = extension::TableViewCell::create();

		Layout* item = (Layout*)mUI.mTemplateItem->clone();
		item->setPosition(cell->getContentSize()/2);
		item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		item->setVisible(true);
		cell->addChild(item, 1, TAG_ITEM_LIST);
	}

	//设置cell
	setCellContent(cell, idx);

	return cell;
}

void GroupBossRank::tableCellTouched(extension::TableView* table, extension::TableViewCell* cell)
{
	//SoundUtils::playSoundEffect("dianji");
	//点击对应子节点的回调
}

Size GroupBossRank::cellSizeForTable(extension::TableView *table)
{
	return mUI.mTemplateItem->getContentSize();
}

ssize_t GroupBossRank::numberOfCellsInTableView(extension::TableView *table)
{
	GroupBossModel* instance = GroupBossModel::getInstance();

	if(mCurSeledPanelType == eRankType::eTYPE_GROUP)
	{
		return instance->getGroupRankData()->size();
	}	
	else if(mCurSeledPanelType == eRankType::eTYPE_ALL)
	{
		return instance->getPlayerRankData(eTYPE_ALL)->size();
	}
	else if(mCurSeledPanelType == eRankType::eTYPE_IN_GROUP)
	{
		return instance->getPlayerRankData(eTYPE_IN_GROUP)->size();
	}
	return 0;
}

void GroupBossRank::switchRankTopPanel(eRankType rankType)
{
	for(int i=0; i<MAX_BOSS_NUM; i++)
	{
		mUI.mBtnRankTop[i]->setHighlighted(false);
		mUI.mBtnRankTop[i]->setEnabled(true);
		Text* Text_Name = (Text*)mUI.mBtnRankTop[i]->getChildByName("Text_Name");
		Text_Name->setColor(Color3B(0xea,0xc1,0x71));
		Text_Name->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(Text_Name, Color3B(0x77, 0x00, 0x00), 2);
	}
	mUI.mBtnRankTop[rankType-1]->setHighlighted(true);
	mUI.mBtnRankTop[rankType-1]->setEnabled(false);

	Text* Text_Name = (Text*)mUI.mBtnRankTop[rankType-1]->getChildByName("Text_Name");
	Text_Name->setColor(Color3B(0xf0,0xf5,0x64));
	setTextAddStroke(Text_Name, Color3B(0x9a, 0x09, 0x09), 2);

	mCurSeledPanelType = rankType;

 	//mRankTopView->reloadData();
 	//setCommonUI();
}

void GroupBossRank::update(float dt)
{
	this->unscheduleUpdate();
	this->removeFromParent();
}

void GroupBossRank::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switchRankTopPanel((eRankType)nWidgetName);
		GroupBossController::getInstance()->sendGetRankMsg((eRankType)nWidgetName);
		GroupBossModel::getInstance()->setRankTemp((eRankType)nWidgetName);
	}
}

void GroupBossRank::updateUI()
{
	mRankTopView->reloadData();
	setCommonUI();
}

void GroupBossRank::onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	if(MAX_BOSS_NUM < 6)
	{
		mUI.mImgArrowUp->setVisible(false);
		mUI.mImgArrowDown->setVisible(false);
		return;
	}
	else
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLL_TO_TOP:
			{
				mUI.mImgArrowUp->setVisible(false);
				mUI.mImgArrowDown->setVisible(true);		
			}
			break;
		case cocos2d::ui::ScrollView::EventType::SCROLL_TO_BOTTOM:
			{
				mUI.mImgArrowUp->setVisible(true);
				mUI.mImgArrowDown->setVisible(true);		
			}
			break;
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				mUI.mImgArrowUp->setVisible(true);
				mUI.mImgArrowDown->setVisible(true);
			}
			break;
		}
	}
}

void GroupBossRank::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;

	switch (obParam->id)
	{
	case nMAIN_CMD_GROUP_BOSS_RANK:
		{
			//switchRankTopPanel(mCurSeledPanelType);
			updateUI();
		}
		break;

		break;
	default:
		break;
	}
}