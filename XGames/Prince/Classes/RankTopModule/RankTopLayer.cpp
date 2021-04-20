#include "RankTopLayer.h"
#include "RankTopModel.h"
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

//我的战斗力、我的等级 文本基础X位置（动态调整美观点）
#define MY_RANK_VALUE_POS_X	526.0f

RankTopLayer::RankTopLayer()
{
	mCurSeledPanelType = DEFAULT_RANK_TOP;
	m_bIsPush = false;
	mRankTopView = nullptr;
	m_bIsPush = false;

	RankTopModel::getInstance()->addObserver(this);
}

RankTopLayer::~RankTopLayer(void)
{
	RankTopModel::getInstance()->removeObserver(this);
}

RankTopLayer* RankTopLayer::create(bool isPush)
{
	RankTopLayer* _layer = new RankTopLayer();
	if (_layer->init(isPush))
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

void RankTopLayer::onEnter()
{
	Layer::onEnter();

	auto callBack = [this](float dt)->void
	{
		//现在改成实时更新数据
		/*
		if(!RankTopModel::getInstance()->getIsGettedLevelRank())
		{
			mRankTopController.sendGetRankTopListMsg(LEVEL_RANK_TOP);
		}

		if(!RankTopModel::getInstance()->getIsGettedPowerRank())
		{
			mRankTopController.sendGetRankTopListMsg(POWER_RANK_TOP);
		}

		mRankTopController.sendGetRankTopListMsg(PVP_RANK_TOP);

		this->getScheduler()->unschedule("RankTopLayer::sendMsg", this);
		*/
		mRankTopController.sendGetRankTopListMsg(LEVEL_RANK_TOP);
		mRankTopController.sendGetRankTopListMsg(POWER_RANK_TOP);
		mRankTopController.sendGetRankTopListMsg(PVP_RANK_TOP);
		mRankTopController.sendGetRankTopListMsg(GROUP_RANK_TOP);
		this->getScheduler()->unschedule("RankTopLayer::sendMsg", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "RankTopLayer::sendMsg");
}

void RankTopLayer::onExit()
{
	Layer::onExit();
}

bool RankTopLayer::init(bool isPush)
{
	if (!Layer::init())
	{
		return false;
	}

	m_bIsPush = isPush;

	initUI();
	initRankTopUI();

	return true;
}

void RankTopLayer::initUI()
{
	// 加载排行榜 UI;
	string strFileName;
	strFileName.assign("Image/UINewRank/UINewRank.ExportJson");

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
			RankTopModel::getInstance()->setCurRankTopType(LEVEL_RANK_TOP);
			scheduleUpdate();		
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_RANK_BOARD);
	this->addChild(_layerCommHeadInfo);

	mUI.mScrollLeftPanel = (ui::ScrollView*)(Helper::seekWidgetByName(mUI.pRoot, "ScrollView_Rank"));
	mUI.mScrollLeftPanel->addEventListener(CC_CALLBACK_2(RankTopLayer::onScrollViewEvent, this));

	for(int i=0; i<MAX_RANK_NUM; i++)
	{
		mUI.mBtnRankTop[i] = (Button*)(mUI.mScrollLeftPanel->getChildByName("Btn_RankTop" + _TO_STR(i+1)));
		mUI.mBtnRankTop[i]->addTouchEventListener(CC_CALLBACK_2(RankTopLayer::onBtnClick, this, i+1));
	}

	//滚动区域提示箭头
	mUI.mImgArrowUp = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Top"));
	mUI.mImgArrowDown = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Bot"));
	mUI.mImgArrowUp->setVisible(false);
	mUI.mImgArrowDown->setVisible(false);

	//图片ui变量
	mUI.mImgTitleName = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Name"));
	mUI.mImgTitleGroupName = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Gang"));
	mUI.mImgTitleValue = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Value"));
	mUI.mImgBottomValue = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_MyValue"));
	mUI.mImgBottomRank = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_MyRank"));
	mUI.mImgUpdateTips = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_UpdateTips"));
	mUI.mImgUpdateTips->setVisible(false);				//默认不显示，如有用到此提示的，在setCommonUI中打开

	//文字ui变量
	mUI.mTxtMyRank = (Text*)(Helper::seekWidgetByName(mUI.pRoot, "Lab_MyRank"));
	mUI.mTxtMyValue = (Text*)(Helper::seekWidgetByName(mUI.pRoot, "Lab_MyValue"));

	mUI.mTxtMyRank->setString("");
	mUI.mTxtMyValue->setString("");

	mUI.mTxtMyRank->setFontName(FONT_FZZHENGHEI);
	mUI.mTxtMyValue->setFontName(FONT_FZZHENGHEI);

	//滚动列表容器
	mUI.mPnlList = (Layout*)(Helper::seekWidgetByName(mUI.pRoot, "TableView_pnl"));
	mUI.mTemplateItem = (Layout*)mUI.mPnlList->getChildByName("Pnl_Data");
	mUI.mTemplateItem->setVisible(false);
}

void RankTopLayer::initRankTopUI()
{
	RankTopModel* instance = RankTopModel::getInstance();
	
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

void RankTopLayer::setCommonUI()
{
	RankTopModel* instance = RankTopModel::getInstance();

	//排行榜类型
	switch (mCurSeledPanelType)
	{
	case LEVEL_RANK_TOP:
		{
			mUI.mImgTitleName->loadTexture("Image/UINewRank/image/img_wanjianicheng.png");
			mUI.mImgTitleGroupName->loadTexture("Image/UINewRank/image/img_suoshubangpai.png");
			mUI.mImgTitleValue->loadTexture("Image/UINewRank/image/img_dengji.png");

			mUI.mImgBottomRank->loadTexture("Image/UINewRank/image/img_wodepaiming.png");
			mUI.mImgBottomValue->loadTexture("Image/UINewRank/image/img_wodedengji.png");
			//mUI.mImgUpdateTips->loadTexture("Image/UINewRank/image/img_meiri5diangengxin.png");

			//我的名次和数据
			mUI.mTxtMyRank->setString(instance->getMyRankInfo().myLevelRank == 0 ? DataManager::getInstance()->searchCommonTexdtById(10031) : 
				_TO_STR(instance->getMyRankInfo().myLevelRank));
			mUI.mTxtMyValue->setString(_TO_STR(instance->getMyRankInfo().myLevel));
			mUI.mTxtMyValue->setPositionX(MY_RANK_VALUE_POS_X);
		}		
		break;
	case POWER_RANK_TOP:
		{
			mUI.mImgTitleName->loadTexture("Image/UINewRank/image/img_wanjianicheng.png");
			mUI.mImgTitleGroupName->loadTexture("Image/UINewRank/image/img_suoshubangpai.png");
			mUI.mImgTitleValue->loadTexture("Image/UINewRank/image/img_zhandouli.png");

			mUI.mImgBottomRank->loadTexture("Image/UINewRank/image/img_wodepaiming.png");
			mUI.mImgBottomValue->loadTexture("Image/UINewRank/image/img_wodezhandouli.png");
			//mUI.mImgUpdateTips->loadTexture("Image/UINewRank/image/img_meiri5diangengxin.png");

			//我的名次和数据
			mUI.mTxtMyRank->setString(instance->getMyRankInfo().myPowerRank == 0 ? DataManager::getInstance()->searchCommonTexdtById(10031) :
				_TO_STR(instance->getMyRankInfo().myPowerRank));
			mUI.mTxtMyValue->setString(_TO_STR(instance->getMyRankInfo().myPower));
			mUI.mTxtMyValue->setPositionX(MY_RANK_VALUE_POS_X + 10);
		}	
		break;
	case PVP_RANK_TOP:
		{
			mUI.mImgTitleName->loadTexture("Image/UINewRank/image/img_wanjianicheng.png");
			mUI.mImgTitleGroupName->loadTexture("Image/UINewRank/image/img_suoshubangpai.png");
			mUI.mImgTitleValue->loadTexture("Image/UINewRank/image/img_zhandouli.png");

			mUI.mImgBottomRank->loadTexture("Image/UINewRank/image/img_wodepaiming.png");	
			mUI.mImgBottomValue->loadTexture("Image/UINewRank/image/img_wodezhandouli.png");
			//mUI.mImgUpdateTips->loadTexture("Image/UINewRank/image/img_meirigengxin.png");

			//我的名次和数据
			mUI.mTxtMyRank->setString(instance->getMyRankInfo().myPvpRank == 0 ? DataManager::getInstance()->searchCommonTexdtById(10031) : 
				_TO_STR(instance->getMyRankInfo().myPvpRank));
			mUI.mTxtMyValue->setString(_TO_STR(instance->getMyRankInfo().myPower));
			mUI.mTxtMyValue->setPositionX(MY_RANK_VALUE_POS_X + 10);
		}	
		break;
	case GROUP_RANK_TOP:
		{
			mUI.mImgTitleName->loadTexture("Image/UINewRank/image/img_bangpaimingcheng.png");
			mUI.mImgTitleGroupName->loadTexture("Image/UINewRank/image/img_bangzhu.png");
			mUI.mImgTitleValue->loadTexture("Image/UINewRank/image/img_zongzhandouli1.png");

			mUI.mImgBottomRank->loadTexture("Image/UINewRank/image/img_wodebangpai.png");	
			mUI.mImgBottomValue->loadTexture("Image/UINewRank/image/img_zongzhandouli2.png");

			//我的名次和数据
			mUI.mTxtMyRank->setString(instance->getMyRankInfo().myGroupRank == 0 ? DataManager::getInstance()->searchCommonTexdtById(10031) :
				_TO_STR(instance->getMyRankInfo().myGroupRank));
			mUI.mTxtMyValue->setString(_TO_STR(instance->getMyRankInfo().myGroupPower));
			mUI.mTxtMyValue->setPositionX(MY_RANK_VALUE_POS_X + 10);
		}
		break;
	default:
		break;
	}
}

void RankTopLayer::setCellContent(extension::TableViewCell* cell, ssize_t idx)
{
	Layout* item = (Layout*)cell->getChildByTag(TAG_ITEM_LIST);
	if(!item)
	{
		return;
	}
	ImageView* rankImg = (ImageView*)item->getChildByName("Img_num");
	ImageView* vipLvImg = (ImageView*)item->getChildByName("Img_VIP");
	Text*	rankTxt = (Text*)item->getChildByName("Lab_Num");
	Text*	lvTxt = (Text*)item->getChildByName("Lab_Level");
	Layout* pnlHead = (Layout*)item->getChildByName("Pnl_Head");
	Text*	playerName = (Text*)item->getChildByName("Lab_Name");
	Text*	group = (Text*)item->getChildByName("Lab_Gang");
	Text*	valueTxt = (Text*)item->getChildByName("Lab_Count");

	pnlHead->removeAllChildren();				//清空头像容器

	lvTxt->setFontName(FONT_FZZHENGHEI);
	rankTxt->setFontName(FONT_FZZHENGHEI);
	playerName->setFontName(FONT_FZZHENGHEI);
	group->setFontName(FONT_FZZHENGHEI);
	valueTxt->setFontName(FONT_FZZHENGHEI);

	lvTxt->setString("");
	rankTxt->setString("");
	playerName->setString("");
	group->setString("");
	valueTxt->setString("");

	RankTopModel* instance = RankTopModel::getInstance();
	vector<sRankTopInfo> vecRankInfo = instance->getRankTopData(mCurSeledPanelType);

	//名次按cell顺序来排列
	if(idx < 3)
	{
		rankImg->loadTexture(string("Image/Icon/global/img_") + _TO_STR(vecRankInfo.at(idx).rank) + string("ming.png"));
		rankTxt->setVisible(false);
	}
	else
	{
		rankImg->loadTexture("Image/Icon/global/img_4ming.png");
		rankTxt->setString(_TO_STR(vecRankInfo.at(idx).rank));
		rankTxt->setVisible(true);
	}

	if(mCurSeledPanelType == GROUP_RANK_TOP)
	{
		stringstream ss;

		RowData* flagResData = DataManager::getInstance()->searchEmblemById(vecRankInfo.at(idx).iconId);
		if(flagResData)
		{
			string name = flagResData->getStringData("resId");
			ss << "Image/Icon/Flag/" << name << ".png";
		}
		ImageView* icon = ImageView::create(ss.str());
		if(icon)
		{
			icon->setPosition(pnlHead->getContentSize()/2);
			icon->setScale(0.46);
			pnlHead->addChild(icon);
		}

		lvTxt->setString("Lv." + _TO_STR(vecRankInfo.at(idx).level));
		setTextAddStroke(lvTxt, Color3B(0x7d, 0x1f, 0x3d), 2);
		lvTxt->setVisible(true);
		vipLvImg->setVisible(false);

		playerName->setString(vecRankInfo.at(idx).group);
		setTextAddStroke(playerName, Color3B(0x7d, 0x1f, 0x3d), 2);

		group->setString(vecRankInfo.at(idx).name);
		group->setColor(Color3B(0xFF, 0xE9, 0x34));

		//帮派等级跟着帮派名字走
		//lvTxt->setAnchorPoint(Vec2(0, 0.5));
		//lvTxt->setPositionX(playerName->getPositionX() + playerName->getContentSize().width);
	}
	else
	{
		UIToolHeader* header = UIToolHeader::create(vecRankInfo.at(idx).iconId);
		header->setAnchorPoint(Point(0.5, 0.5));
		header->setPosition(pnlHead->getContentSize()/2);
		header->setScale(0.8);
		pnlHead->addChild(header);

		lvTxt->setVisible(false);
		vipLvImg->setVisible(true);		
		vipLvImg->loadTexture("Image/Icon/vip/btn_VIP" + _TO_STR(vecRankInfo.at(idx).vipLv) + ".png");

		playerName->setString(vecRankInfo.at(idx).name);
		setTextAddStroke(playerName, Color3B(0x7d, 0x1f, 0x3d), 2);

		group->setString(vecRankInfo.at(idx).group);
		string noGroup = DataManager::getInstance()->searchCommonTexdtById(20217);
		if(vecRankInfo.at(idx).group.compare(noGroup)!= 0)
		{
			group->setColor(Color3B(0xFF, 0xE9, 0x34));
		}
		else
		{
			group->setColor(Color3B(0x8F, 0x8B, 0x8D));
		}
	}
	
	switch (mCurSeledPanelType)
	{
	case LEVEL_RANK_TOP:
		valueTxt->setString(_TO_STR(vecRankInfo.at(idx).level));
		break;
	case POWER_RANK_TOP:
	case PVP_RANK_TOP:
	case GROUP_RANK_TOP:
		valueTxt->setString(_TO_STR(vecRankInfo.at(idx).power));
		break;

	default:
		break;
	}
}

extension::TableViewCell* RankTopLayer::tableCellAtIndex(extension::TableView *table, ssize_t idx)
{
	//重用cell队列
	extension::TableViewCell* cell = table->dequeueCell();
	if(!cell)
	{
		//创建cell
		cell = new extension::TableViewCell();
		cell->autorelease();

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

void RankTopLayer::tableCellTouched(extension::TableView* table, extension::TableViewCell* cell)
{
	SoundUtils::playSoundEffect("dianji");

	if(mCurSeledPanelType == DEFAULT_RANK_TOP || mCurSeledPanelType == GROUP_RANK_TOP)
	{
		return;
	}

	vector<sRankTopInfo> rankInfo = RankTopModel::getInstance()->getRankTopData(mCurSeledPanelType);

	int idx = cell->getIdx();
	if(idx < rankInfo.size())
	{
		PVP_PLAYER_DATA playerInfo;
		playerInfo.atk = rankInfo.at(idx).power;
		playerInfo.id = rankInfo.at(idx).uId;
		playerInfo.level = rankInfo.at(idx).level;
		playerInfo.matrixIds = rankInfo.at(idx).matrix;
		playerInfo.name = rankInfo.at(idx).name;
		playerInfo.resID = rankInfo.at(idx).iconId;
		playerInfo.groupName = rankInfo.at(idx).group;
		playerInfo.vipLv = rankInfo.at(idx).vipLv;
		playerInfo.pvporder = 0;
		LayerCommFormation *_LayerCommFormation = LayerCommFormation::create(playerInfo);
		_LayerCommFormation->show(Director::getInstance()->getRunningScene());
	}
}

Size RankTopLayer::cellSizeForTable(extension::TableView *table)
{
	return mUI.mTemplateItem->getContentSize();
}

ssize_t RankTopLayer::numberOfCellsInTableView(extension::TableView *table)
{
	RankTopModel* instance = RankTopModel::getInstance();

	if(mCurSeledPanelType != DEFAULT_RANK_TOP)
	{
		return instance->getRankTopData(mCurSeledPanelType).size();
	}
	return 0;
}


void RankTopLayer::switchRankTopPanel(rankTopType rankType)
{
	for(int i=0; i<MAX_RANK_NUM; i++)
	{
		mUI.mBtnRankTop[i]->setHighlighted(false);
		mUI.mBtnRankTop[i]->setEnabled(true);
	}
	mUI.mBtnRankTop[rankType-1]->setHighlighted(true);
	mUI.mBtnRankTop[rankType-1]->setEnabled(false);

	mCurSeledPanelType = rankType;

	mRankTopView->reloadData();
	setCommonUI();
}

void RankTopLayer::update(float dt)
{
	this->unscheduleUpdate();
	if(m_bIsPush)
	{
		Director::getInstance()->popScene();
	}
	else
	{
		Director::getInstance()->replaceScene(MainCityScene::create());
	}
}

void RankTopLayer::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switchRankTopPanel((rankTopType)nWidgetName);
	}
}

void RankTopLayer::onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	if(MAX_RANK_NUM < 6)
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

void RankTopLayer::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;

	switch (obParam->id)
	{
	case nMAIN_CMD_RANK_TOP_LIST:
		{
			switchRankTopPanel(RankTopModel::getInstance()->getCurRankTopType());
		}
		break;

	default:
		break;
	}
}