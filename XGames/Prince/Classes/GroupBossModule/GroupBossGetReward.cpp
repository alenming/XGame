#include "GroupBossGetReward.h"
#include "GroupBossModel.h"
#include "Communication/Command.h"
#include "Widget/LayerCommHeadInfo.h"
#include "Utils/StringFormat.h"
#include "Widget/LayerCommFormation.h"

//列表tag
#define TAG_ITEM_LIST	1001
#define FOUR_PANEL		4


#define  JINGNANG_ID	10071

GroupBossGetReward::GroupBossGetReward()
{
	mCurSeledPanelType = eGetType::eTYPE_JINNANG;
	mRankTopView = nullptr;
	GroupBossInstance->addObserver(this);
}

GroupBossGetReward::~GroupBossGetReward()
{
	GroupBossInstance->removeObserver(this);
}

GroupBossGetReward* GroupBossGetReward::create(eGetType rankType)
{
	GroupBossGetReward* _layer = new GroupBossGetReward();
	if(_layer->init(rankType))
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

void GroupBossGetReward::onEnter()
{
	Layer::onEnter();
	auto callBack = [this](float dt)->void
	{;
		this->getScheduler()->unschedule("GroupBossGetReward::sendMsg", this);
		switchRankTopPanel(mCurSeledPanelType);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "GroupBossGetReward::sendMsg");

}

void GroupBossGetReward::onExit()
{
	Layer::onExit();
}

bool GroupBossGetReward::init(eGetType rankType )
{
	if (!Layer::init())
	{
		return false;
	}
	mCurSeledPanelType = rankType;
	initUI();
	initRankTopUI();
	setRedFrag();
	return true;
}

void GroupBossGetReward::initUI()
{
	// 加载排行榜 UI;
	string strFileName;
	strFileName.assign("Image/UI_bangpaiBOSS/UI_bangpaiBOSS_Reward.ExportJson");

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

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_BONUS_EXCHANGE);
	this->addChild(_layerCommHeadInfo);

	mUI.mScrollLeftPanel = (ui::ScrollView*)(Helper::seekWidgetByName(mUI.pRoot, "ScrollView_Rank"));
	mUI.mScrollLeftPanel->addEventListener(CC_CALLBACK_2(GroupBossGetReward::onScrollViewEvent, this));

	for(int i=0; i<MAX_GET_NUM; i++)
	{
		mUI.mBtnRankTop[i] = (Button*)(mUI.mScrollLeftPanel->getChildByName("Btn_RankTop" + _TO_STR(i+1)));
		mUI.mBtnRankTop[i]->addTouchEventListener(CC_CALLBACK_2(GroupBossGetReward::onBtnClick, this, i+1));
	}
	mUI.redFlag =(ImageView*)mUI.mBtnRankTop[0]->getChildByName("IMG_Dot");

	//滚动区域提示箭头
	mUI.mImgArrowUp = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Top"));
	mUI.mImgArrowDown = (ImageView*)(Helper::seekWidgetByName(mUI.pRoot, "Img_Bot"));
	mUI.mImgArrowUp->setVisible(false);
	mUI.mImgArrowDown->setVisible(false);

	//滚动列表容器
	mUI.mPnlList = (Layout*)(Helper::seekWidgetByName(mUI.pRoot, "TableView_pnl"));
	mUI.mTemplateItem = (Layout*)mUI.mPnlList->getChildByName("Pnl_Data");
	mUI.mTemplateItem->setVisible(false);
	mUI.Panel_Reward = (Layout*)(Helper::seekWidgetByName(mUI.mTemplateItem , "Panel_Reward"));
	mUI.Panel_Exchange = (Layout*)(Helper::seekWidgetByName(mUI.mTemplateItem , "Panel_Exchange"));
}

void GroupBossGetReward::initRankTopUI()
{

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

extension::TableViewCell* GroupBossGetReward::tableCellAtIndex(extension::TableView *table, ssize_t idx)
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

void GroupBossGetReward::setCellContent(extension::TableViewCell* cell, ssize_t idx)
{
	Layout* item = (Layout*)cell->getChildByTag(TAG_ITEM_LIST);
	if(!item)
	{
		return;
	}

	item->setVisible(true);
	auto Panel_Reward = (Layout*)item->getChildByName( "Panel_Reward");
	auto Panel_Exchange =(Layout*)item->getChildByName( "Panel_Exchange");

	Panel_Reward->setVisible(false);
	Panel_Exchange->setVisible(false);

	if (mCurSeledPanelType == eGetType::eTYPE_HURT)
	{
		Panel_Reward->setVisible(true);
		auto reWardInfo = GroupBossInstance->getDamReward();
		auto oneItem = reWardInfo->at(idx);
		for (int i = 0; i < oneItem.vDamRewardInfo.size(); i++)
		{
			Layout* Panel_head = (Layout*)Panel_Reward->getChildByName("Panel_item"+TO_STR(i+1));
			//清空头像容器
			Panel_head->removeAllChildren();	

			UIToolHeader* header = UIToolHeader::create(oneItem.vDamRewardInfo.at(i).rewardId);
			header->setAnchorPoint(Point(0.5, 0.5));
			header->setPosition(Panel_head->getContentSize()/2);
			header->setScale(0.8f);
			header->setTipsEnabled(true);
			header->setNumEx(oneItem.vDamRewardInfo.at(i).rewardCnt);
			Panel_head->addChild(header);
		}
		Button* BTN_Receive = (Button*)Panel_Reward->getChildByName("BTN_Receive");
		ImageView* Image_Received = (ImageView*)Panel_Reward->getChildByName("Image_Received");
		
				
		Text*	Text_title = (Text*)(Helper::seekWidgetByName(Panel_Reward, "Text_title"));
		Text*	Text_Conditions = (Text*)(Helper::seekWidgetByName(Panel_Reward, "Text_Conditions"));
		Text_title->setFontName(FONT_FZZHENGHEI);
		Text_Conditions->setFontName(FONT_FZZHENGHEI);
		Text_title->setString(DataManager::getInstance()->searchCommonTexdtById(10157) + formatDamage(oneItem.damLimit));
		Text_title->setColor(Color3B(0xff, 0xfb, 0xd0));
		setTextAddStroke(Text_title, Color3B(0x9e, 0x0f, 0x0f), 2);

		Text_Conditions->setString(formatDamage(GroupBossInstance->getMyTotalDam()) +"/" + formatDamage(oneItem.damLimit));

		if (GroupBossInstance->getMyTotalDam() < oneItem.damLimit)
		{
			//伤害不够
			Image_Received->setVisible(false);
			BTN_Receive->setVisible(true);
			BTN_Receive->setEnabled(false);
			BTN_Receive->setBright(false);
			//红色
			Text_Conditions->setColor(Color3B(0xd9, 0x2e, 0x2e));
		}
		else if (GroupBossInstance->getMyTotalDam() >= oneItem.damLimit)
		{
			//绿色
			Text_Conditions->setColor(Color3B(0x54, 0xdd, 0x51));
			
			//按钮状态
			if(oneItem.isGetted)
			{
				//如果已经领取过
				BTN_Receive->setVisible(false);
				Image_Received->setVisible(true);
			}
			else if(oneItem.isCanGet)
			{
				//可以领取
				BTN_Receive->setVisible(true);
				Image_Received->setVisible(false);
				BTN_Receive->setEnabled(true);
				BTN_Receive->setBright(true);
				BTN_Receive->addTouchEventListener(CC_CALLBACK_2(GroupBossGetReward::onBtnGetItem, this, oneItem.boxId));
			}
			else
			{
				//没达到领取条件
				BTN_Receive->setVisible(true);
				Image_Received->setVisible(false);
				BTN_Receive->setEnabled(false);
				BTN_Receive->setBright(false);
			}
		}
	}
	else if (mCurSeledPanelType == eGetType::eTYPE_JINNANG)
	{
		Panel_Exchange->setVisible(true);
		auto reWardInfo = GroupBossInstance->getExchangeJn();
		auto oneItem = reWardInfo->at(idx);
		Layout* Panel_head = (Layout*)Panel_Exchange->getChildByName("Panel_item1");
		if (Panel_head)
		{		
			//清空头像容器
			Panel_head->removeAllChildren();
			UIToolHeader* header = UIToolHeader::create(oneItem.firstId);
			header->setAnchorPoint(Point(0.5, 0.5));
			header->setPosition(Panel_head->getContentSize()/2);
			header->setScale(0.8f);
			header->setTipsEnabled(true);
			header->setNumEx(oneItem.firstCount);
			Panel_head->addChild(header);
		}

		Layout* Panel_head1 = (Layout*)Panel_Exchange->getChildByName("Panel_item2");
		//清空头像容器
		if (Panel_head1)
		{
			Panel_head1->removeAllChildren();
			UIToolHeader* header1 = UIToolHeader::create(oneItem.secondId);
			header1->setAnchorPoint(Point(0.5, 0.5));
			header1->setPosition(Panel_head1->getContentSize()/2);
			header1->setScale(0.8f);
			header1->setTipsEnabled(true);
			header1->setNumEx(oneItem.secondCount);
			Panel_head1->addChild(header1);
		}


		Button* BTN_Exchange = (Button*)Panel_Exchange->getChildByName("BTN_Exchange");
		
		//材料不够
		if (false)
		{
			BTN_Exchange->setEnabled(false);
			BTN_Exchange->setTouchEnabled(false);
			BTN_Exchange->setBright(false);
		}
		else
		{
			BTN_Exchange->addTouchEventListener(CC_CALLBACK_2(GroupBossGetReward::onBtnGetExchange, this, oneItem.firstCount, oneItem.boxId));
			BTN_Exchange->setEnabled(true);
			BTN_Exchange->setTouchEnabled(true);
			BTN_Exchange->setBright(true);
		}

		Text* Text_title = (Text*)(Helper::seekWidgetByName(Panel_Exchange, "Text_title"));
		Text_title->setColor(Color3B(0xFF, 0xE9, 0x34));
		Text_title->setFontName(FONT_FZZHENGHEI);
		//Text_title->setString(STR_UTF8("我是一个逼"));
		Text_title->setString(oneItem.name);
	}
}


void GroupBossGetReward::tableCellTouched(extension::TableView* table, extension::TableViewCell* cell)
{
	//SoundUtils::playSoundEffect("dianji");
	//点击对应子节点的回调
}

Size GroupBossGetReward::cellSizeForTable(extension::TableView *table)
{
	return mUI.mTemplateItem->getContentSize();
}

ssize_t GroupBossGetReward::numberOfCellsInTableView(extension::TableView *table)
{
	GroupBossModel* instance =GroupBossInstance;

	if(mCurSeledPanelType == eGetType::eTYPE_HURT)
	{
		return GroupBossInstance->getDamReward()->size();
	}	
	else if(mCurSeledPanelType == eGetType::eTYPE_JINNANG)
	{
		return GroupBossInstance->getExchangeJn()->size();;
	}
	return 0;
}


void GroupBossGetReward::switchRankTopPanel(eGetType rankType)
{
	for(int i=0; i<MAX_GET_NUM; i++)
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

	mRankTopView->reloadData();

}

void GroupBossGetReward::update(float dt)
{
	this->unscheduleUpdate();
	this->removeFromParent();
}

void GroupBossGetReward::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switchRankTopPanel((eGetType)nWidgetName);
	}
}

void GroupBossGetReward::onBtnGetItem( Ref* ref, Widget::TouchEventType type, int boxId )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		//领取协议
		GroupBossController::getInstance()->sendGetDamRewardMsg(boxId);
		GroupBossModel::getInstance()->setRewardId(boxId);
	}
}

void GroupBossGetReward::onBtnGetExchange( Ref* ref, Widget::TouchEventType type,int changeCount, int changeId )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		//兑换协议
		//钥匙
		auto oneTool = ToolModel::getInstance()->GetToolInfoBytemplateId(JINGNANG_ID);
		if (oneTool != nullptr)
		{

			int count = oneTool->nstack;

			if (count >= changeCount)
			{
				GroupBossController::getInstance()->sendGetJinNangRewardMsg(changeId);
				GroupBossModel::getInstance()->setRewardId(changeId);
			}
			else
			{
				//CustomTips::show("锦囊数量不足!");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10158));
			}
		}

	}
}

void GroupBossGetReward::onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	if(MAX_GET_NUM < 6)
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

void GroupBossGetReward::setRedFrag()
{
	if(mUI.redFlag != nullptr)
	{
		mUI.redFlag->setVisible((GroupBossInstance->isCanGetDamReward()));
	}
}

std::string GroupBossGetReward::formatDamage( time_t damageVal )
{
	if(damageVal >= 10000*100000)
	{
		return StringFormat::formatNumber(damageVal, true);
	}
	else
	{
		return StringFormat::formatNumber(damageVal, false);
	}
}

void GroupBossGetReward::showReward(eGetType type)
{
	vector<PopRewardItem*> vecRewardItem;
	vecRewardItem.clear();

	if(type == eTYPE_HURT)
	{
		sDamReward* rewardData = GroupBossModel::getInstance()->getDamRewardByID(GroupBossModel::getInstance()->getRewardId());
		if(rewardData == nullptr)
		{
			return;
		}
		for(int i=0; i<rewardData->vDamRewardInfo.size(); i++)
		{		
			PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(rewardData->vDamRewardInfo.at(i).rewardId, rewardData->vDamRewardInfo.at(i).rewardCnt);
			vecRewardItem.push_back(rewardItem);
		}
	}
	else if(type == eTYPE_JINNANG)
	{
		sExchangeJn* rewardData = GroupBossModel::getInstance()->getExchangeJnByID(GroupBossModel::getInstance()->getRewardId());
		if(rewardData == nullptr)
		{
			return;
		}

		PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(rewardData->secondId, rewardData->secondCount);
		vecRewardItem.push_back(rewardItem);
	}

	PopReward::show(vecRewardItem);	
}

void GroupBossGetReward::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;

	switch (obParam->id)
	{
	case nMAIN_CMD_GROUP_BOSS_GET_REWARD:
		{
			showReward(eTYPE_HURT);
			setRedFrag();
			//领取
			switchRankTopPanel(mCurSeledPanelType);
		}
		break;
	case nMAIN_CMD_GROUP_BOSS_EXCHANGE:
		{
			showReward(eTYPE_JINNANG);
			//兑换
			switchRankTopPanel(mCurSeledPanelType);
		}
		break;
	default:
		break;
	}
}



