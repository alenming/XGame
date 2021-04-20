#include "LayerChatMessage.h"

#include "CocoStudio.h"
#include "FriendModel.h"
#include "ChatController.h"
#include "Common/Common.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "ResourceManager/ResourceLoader.h"
#include "UI/UIDef.h"
#include "Widget/LayerLoading.h"
#include "../ToolModule/ToolModel.h"
#include "Temp/CustomPop.h"
#include "HeroModule/HeroModel.h"
#include "../Utils/ResourceUtils.h"
#include "../Utils/StringFormat.h"
#include "../MainModule/MainModel.h"
#include "Widget/SuperRichText.h"

using namespace cocos2d;
using namespace cocos2d::ui;


LayerChatMessage::LayerChatMessage()
{
	m_pChatController = ChatController::getInstance();
	m_pChatController->onSetNoticeView(this);
	m_DelShowNotice = false;
	m_curSysNotice = nullptr;
}

LayerChatMessage::~LayerChatMessage()
{		
	m_pChatController->onRemoveNoticeView();
	CCLOG("LayerChatMessage destroy");
}

bool LayerChatMessage::init()
{
	if(!Layer::init())
	{
		return false;
	}
	
	initUI();
	return true;
}

void LayerChatMessage::onEnter()
{
	Layer::onEnter();
	showNotice();
}

void LayerChatMessage::onExit()
{
	Layer::onExit();
	m_ui.noticeImg->setVisible(false);
	Director::getInstance()->getScheduler()->unschedule(
		schedule_selector(LayerChatMessage::callBackRollNotice), this);
	CCLOG("LayerChatMessage onExit");	
}

void LayerChatMessage::initUI()
{
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UILiaotian/UIChatRoll.ExportJson"));

	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(Vec2(0, pos.y));
	// 将UILayer层加入到当前的场景
	this->addChild(m_ui.pRoot);

	//公告
	m_ui.noticeImg = dynamic_cast<ImageView*>(m_ui.pRoot->getChildByName("img"));
	m_ui.pLayNotice  =  dynamic_cast<Layout*>(m_ui.noticeImg->getChildByName("Pnl_Notice"));
	m_ui.noticeImg->setVisible(false);
}

//显示系统公告
void LayerChatMessage::showNotice()
{	
	//按像素滚动显示
	m_curSysNotice = FriendModel::getInstance()->getNextSysNotice();
	if (m_curSysNotice != nullptr)
	{
		if (!m_ui.noticeImg->isVisible())
		{
			m_ui.noticeImg->setVisible(true);
			m_strNoticeContent = m_curSysNotice->strContent;
			if (m_ui.pLayNotice->getChildByName("pTextNotice"))
			{
				m_ui.pLayNotice->removeChildByName("pTextNotice");
			}
			m_ui.pTextNotice = SuperRichText::create();
			m_ui.pTextNotice->setString(m_strNoticeContent,22,Color3B(0xff, 0xff, 0xff));
			m_ui.pLayNotice->addChild(m_ui.pTextNotice,0,"pTextNotice");
			m_nNoticeContentStartPos = m_ui.pLayNotice->getContentSize().width;
			m_DelShowNotice = false;
			m_ui.pTextNotice->setPosition(Vec2(m_nNoticeContentStartPos + m_ui.pTextNotice->getContentSize().width, 
				m_ui.pLayNotice->getContentSize().height/2));

			Director::getInstance()->getScheduler()->unschedule(
				schedule_selector(LayerChatMessage::callBackRollNotice), this);

			Director::getInstance()->getScheduler()->schedule(
				schedule_selector(LayerChatMessage::callBackRollNotice), this, 0.01, kRepeatForever, 0, false);	
		}
	}
	else
	{
		m_ui.noticeImg->setVisible(false);
		Director::getInstance()->getScheduler()->unschedule(
			schedule_selector(LayerChatMessage::callBackRollNotice), this);
	}
}

void LayerChatMessage::callBackRollNotice(float dt)
{
	//按像素移动
	if (m_nNoticeContentStartPos > -m_ui.pTextNotice->getWidth())
	{
		m_nNoticeContentStartPos --;
		m_ui.pTextNotice->setPosition(Vec2(m_nNoticeContentStartPos, m_ui.pLayNotice->getContentSize().height/2));
		if (!m_DelShowNotice && m_nNoticeContentStartPos < (m_ui.pLayNotice->getContentSize().width - m_ui.pTextNotice->getWidth()*1.2f))
		{
			m_curSysNotice->repMinute--;
			m_DelShowNotice = true;
		}
	}
	else
	{
		m_curSysNotice = FriendModel::getInstance()->getNextSysNotice();
		if (m_curSysNotice != nullptr)
		{
			m_strNoticeContent = m_curSysNotice->strContent;
			if (m_ui.pLayNotice->getChildByName("pTextNotice"))
			{      
				m_ui.pLayNotice->removeChildByName("pTextNotice");
			}
			m_ui.pTextNotice = SuperRichText::create();
			m_ui.pTextNotice->setString(m_strNoticeContent,22,Color3B(0xff, 0xff, 0xff));
			m_ui.pLayNotice->addChild(m_ui.pTextNotice,0,"pTextNotice");
			m_nNoticeContentStartPos = m_ui.pLayNotice->getContentSize().width;
			m_DelShowNotice = false;
			m_ui.pTextNotice->setPosition(Vec2(m_nNoticeContentStartPos + m_ui.pTextNotice->getContentSize().width, 
				m_ui.pLayNotice->getContentSize().height/2));
		}
		else
		{
			Director::getInstance()->getScheduler()->unschedule(
				schedule_selector(LayerChatMessage::callBackRollNotice), this);
			auto call = [this]()->void
			{
				m_ui.noticeImg->setVisible(false);
				showNotice();//可能消失瞬间又来了消息
			};
			m_ui.pLayNotice->runAction(CCSequence::create(CCDelayTime::create(1.0f),CCCallFunc::create(call),nullptr));
		}		
	}	
}

void LayerChatMessage::onRecvNoticeMessage()
{
	showNotice();
}