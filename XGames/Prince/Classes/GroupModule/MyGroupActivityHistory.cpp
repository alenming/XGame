#include "MyGroupActivityHistory.h"
#include "MyGroupController.h"
#include "MyGroupModel.h"
#include "Common/Common.h"

#define  GRAP_IN_ITEM  110

MyGroupActivityHistory::MyGroupActivityHistory()
{
	MyGroupModel::getInstance()->addObserver(this);
}


MyGroupActivityHistory::~MyGroupActivityHistory()
{
	MyGroupModel::getInstance()->removeObserver(this);
}

Layer* MyGroupActivityHistory::create( )
{ 
	auto pRet = new MyGroupActivityHistory(); 
	if (pRet && pRet->init()) 
	{
		pRet->autorelease(); 
		return pRet; 
	}
 
	else 
	{ 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
}

bool MyGroupActivityHistory::init( )
{
	if(!ModalLayer::init())
	{
		return false;
	}


	initUI();

	return true;
}

void MyGroupActivityHistory::onEnter()
{
	ModalLayer::onEnter();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(MyGroupActivityHistory::updateNextFram), this, 0, false);
}

void MyGroupActivityHistory::updateNextFram(float ft)
{
	updateUI();
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(MyGroupActivityHistory::updateNextFram), this);
}

void MyGroupActivityHistory::onExit()
{
	ModalLayer::onExit();
}

void MyGroupActivityHistory::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
	case nMAIN_CMD_GROUP_ACTIVITY_HISTORY:
		{
			//updateUI();
		}
		break;
	default:
		break;
	}
}

void MyGroupActivityHistory::update(float dt)
{
	this->unscheduleUpdate();
	this->removeFromParent();
}

void MyGroupActivityHistory::initUI()
{

	Size winSize = Director::getInstance()->getWinSize();
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 加载 UI;
	string strFileName = "Image/UIGang/UIGang_DragonList.ExportJson";
	// Root;
	m_ui.uiRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	//m_ui.uiRoot->setSize(size);
	//m_ui.uiRoot->setPosition(pos);

	m_ui.uiRoot->setSize(Size(size.width,m_ui.uiRoot->getContentSize().height));
	m_ui.uiRoot->setPosition(Vec2(pos.x,0));

	this->addChild(m_ui.uiRoot);


	m_ui.Btn_Close = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Close"));
	m_ui.Btn_Close->addTouchEventListener(CC_CALLBACK_2(MyGroupActivityHistory::onBtnClick,this));

	m_ui.ScrollView_1 = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.uiRoot,"ScrollView_1"));
	m_ui.Pnl_List = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Pnl_List"));
	m_ui.Pnl_List->setVisible(false);

	//建筑UI滚动条
	m_HisttoryScrollBar = CustomScrollBar::create();
	m_HisttoryScrollBar->initScrollBar(m_ui.ScrollView_1->getContentSize().height);
	m_HisttoryScrollBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_HisttoryScrollBar->setPosition(Point(30, m_ui.uiRoot->getPositionY() + m_ui.ScrollView_1->getContentSize().height/2));
	m_ui.uiRoot->addChild(m_HisttoryScrollBar);

	m_HisttoryScrollBar->setScrollBarLengthPercent(1.0f);
	m_HisttoryScrollBar->setScrollBarPercentY(0.0f);

	m_ui.ScrollView_1->addEventListener(CC_CALLBACK_2(MyGroupActivityHistory::onScrollViewEvent, this));
}


void MyGroupActivityHistory::onBtnClick( Ref* reff, Widget::TouchEventType type )
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		this->scheduleUpdate();
	}
}


void MyGroupActivityHistory::updateUI()
{
	sActivityInfo* publicInfo = MyGroupModel::getInstance()->getGroupActivityInfo();




	auto GAP_GROUP_BUILDING = 0;
	int scrollViewHeight = (publicInfo->clickedHistory.size())*(m_ui.Pnl_List->getContentSize().height + GAP_GROUP_BUILDING);
	scrollViewHeight = scrollViewHeight >  m_ui.ScrollView_1->getContentSize().height ? scrollViewHeight : m_ui.ScrollView_1->getContentSize().height;
	int scrollViewWidth = m_ui.ScrollView_1->getInnerContainerSize().width;
	m_ui.ScrollView_1->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );
	m_ui.ScrollView_1->removeAllChildren();

	for (int i = 0; i < publicInfo->clickedHistory.size(); i++)
	{
		auto sonItem = m_ui.Pnl_List->clone();
		sonItem->setVisible(true);
		auto Img_Round = static_cast<ImageView*>(sonItem->getChildByName("Img_Round"));
		auto po = Img_Round->getPosition();
		Img_Round->removeFromParent();

		Img_Round = ImageView::create("Image/UIGang/img_"+TO_STR(i+1)+"lun.png");
		Img_Round->setPosition(po);
		sonItem->addChild(Img_Round);
		int siss = publicInfo->clickedHistory.at(i).size();

		auto Pnl_Item = static_cast<Layout*>(sonItem->getChildByName("Pnl_Item"));

		int Xfirst = (0-(siss -1))*GRAP_IN_ITEM/2 - Pnl_Item->getContentSize().width/2;
		auto conSize = m_ui.Pnl_List->getContentSize();

		for (int j = 0; j < publicInfo->clickedHistory.at(i).size(); j++)
		{
			int resId = publicInfo->clickedHistory.at(i).at(j).resId;
			if (resId != 0)
			{
				auto Pnl_Son = Pnl_Item->clone();
				Pnl_Son->setName("Pnl_Item"+TO_STR(j));
				Pnl_Son->setVisible(true);
				UIToolHeader* _item = UIToolHeader::create(resId);
				_item->setAnchorPoint(Vec2(0,0));
				_item->setPosition(Vec2(0,0));
				_item->setScale(0.87f);
				_item->setNumEx(publicInfo->clickedHistory.at(i).at(j).count);
				_item->setTipsEnabled(true);
				_item->setToolId(resId);
				Pnl_Son->addChild(_item);
				Pnl_Son->setPosition(Vec2(Xfirst+conSize.width/2,5));
				sonItem->addChild(Pnl_Son);
				Xfirst+=GRAP_IN_ITEM;
			}
		}
		
		sonItem->setVisible(true);
		sonItem->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		sonItem->setPosition(Vec2(m_ui.ScrollView_1->getContentSize().width/2, 
			m_ui.ScrollView_1->getInnerContainerSize().height - sonItem->getContentSize().height/2 - 
			i*(sonItem->getContentSize().height + GAP_GROUP_BUILDING) - 10.0f));
		m_ui.ScrollView_1->addChild(sonItem);
	}

	m_ui.ScrollView_1->jumpToTop();
	m_HisttoryScrollBar->setScrollBarLengthPercent(m_ui.ScrollView_1->getContentSize().height / m_ui.ScrollView_1->getInnerContainerSize().height);
	m_HisttoryScrollBar->setScrollBarPercentY(0.0f);
}

//滑动事件
void MyGroupActivityHistory::onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	 if(pSender == m_ui.ScrollView_1)
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.ScrollView_1->getInnerContainerSize();
				Vec2 vec = m_ui.ScrollView_1->getInnerContainer()->getPosition();
				
				//滚动条重设进度
				float percentY = -vec.y / (size.height - m_ui.ScrollView_1->getContentSize().height);
				m_HisttoryScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);	
			}
			break;
		default:
			break;
		}
	}

}