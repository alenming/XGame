#include "LayerChat.h"

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
#include "HeroModule/HeroModel.h"
#include "../Utils/ResourceUtils.h"
#include "../Utils/StringFormat.h"
#include "../MainModule/MainModel.h"
#include "Widget/EditBoxEx.h"
#include "Widget/ActionCreator.h"
#include "FriendController.h"
#include "Temp/CustomTips.h"

using namespace cocos2d;
using namespace cocos2d::ui;

#define CHAT_EDITBOX_FONTSIZE 24

LayerChat::LayerChat()
{
	m_pChatController = ChatController::getInstance();
	m_pChatController->onSetView(this);
	m_nlastSendTime = 0;
	m_chatType		= FRIEND_CHAT_PUBLIC;
	m_nFriendId		= 0;
}

LayerChat::~LayerChat()
{		
	CCLOG("LayerChat destroy");
	m_pChatController->onRemoveView();
}

bool LayerChat::init()
{
	if(!Layer::init())
	{
		return false;
	}
	
	initUI();
	initPopupUI();
	return true;
}

void LayerChat::onEnter()
{
	Layer::onEnter();
}

void LayerChat::onExit()
{
	CCLOG("LayerChat onExit");
	Layer::onExit();
	ResourceLoader::getInstance()->removeUIResource("UILiaotian");
}

void LayerChat::editBoxEditingDidBegin(cocos2d::extension::EditBox* editBox)
{

}
void LayerChat::editBoxEditingDidEnd(cocos2d::extension::EditBox* editBox)
{

}
void LayerChat::editBoxTextChanged(cocos2d::extension::EditBox* editBox, const std::string &text)
{
	Text *textDisWidth = Text::create(text,FONT_FZZHENGHEI,CHAT_EDITBOX_FONTSIZE);
	if (textDisWidth->getContentSize().width > m_ui.sizeSendEditBox.width)
	{
		m_ui.pSendEditBox->setPreferredSize(Size( textDisWidth->getContentSize().width, m_ui.sizeSendEditBox.height));
		m_ui.pSendEditBox->setContentSize(Size( textDisWidth->getContentSize().width, m_ui.sizeSendEditBox.height));
		m_ui.pSendEditBox->setPosition(Vec2( m_ui.vecSendEditBox.x + ( m_ui.sizeSendEditBox.width - textDisWidth->getContentSize().width)/2 -10, m_ui.vecSendEditBox.y));
	}
	else
	{
		m_ui.pSendEditBox->setPreferredSize(m_ui.sizeSendEditBox);
		m_ui.pSendEditBox->setContentSize(m_ui.sizeSendEditBox);
		m_ui.pSendEditBox->setPosition(m_ui.vecSendEditBox);
	}
}
void LayerChat::editBoxReturn(cocos2d::extension::EditBox *editBox)
{

}

void LayerChat::initUI()
{
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UILiaotian/UILiaotian.ExportJson"));
	
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	// 将UILayer层加入到当前的场景
	this->addChild(m_ui.pRoot);
	//
	m_ui.pLayChat =  dynamic_cast<ImageView*>(m_ui.pRoot->getChildByName("Img_Background"));
	ActionCreator::runCommDlgAction(m_ui.pLayChat);

	m_ui.pBtnPrivate =  dynamic_cast<CheckBox*>(m_ui.pLayChat->getChildByName("Siliao"));
	m_ui.imgTishi = dynamic_cast<ImageView*>(m_ui.pBtnPrivate->getChildByName("tishi"));
	m_ui.pBtnPrivate->addEventListener(CC_CALLBACK_2(LayerChat::onBtnClick, this));
	m_ui.pBtnPublic =  dynamic_cast<CheckBox*>(m_ui.pLayChat->getChildByName("Zonghe"));
	m_ui.pBtnPublic->addEventListener(CC_CALLBACK_2(LayerChat::onBtnClick, this));
	m_ui.pBtnGroup =  dynamic_cast<CheckBox*>(m_ui.pLayChat->getChildByName("Bangpai"));
	m_ui.pBtnGroup->addEventListener(CC_CALLBACK_2(LayerChat::onBtnClick, this));
	m_ui.pBtnSend =  dynamic_cast<Button*>(m_ui.pLayChat->getChildByName("Btn_Send"));
	m_ui.pBtnSend->addTouchEventListener(CC_CALLBACK_2(LayerChat::onBtnNotify, this));
	m_ui.pBtnClose =  dynamic_cast<Button*>(m_ui.pLayChat->getChildByName("Btn_Close"));
	m_ui.pBtnClose->addTouchEventListener(CC_CALLBACK_2(LayerChat::onBtnNotify, this));

	ImageView *pImg_Input =  dynamic_cast<ImageView*>(m_ui.pLayChat->getChildByName("Img_Word"));
	Layout* pLayInputBg =  dynamic_cast<Layout*>(pImg_Input->getChildByName("Pnl_textInput"));
	m_ui.pEditBox =  dynamic_cast<TextField*>(pLayInputBg->getChildByName("TextField_input"));
	m_ui.pSendEditBox = EditBoxEx::create(m_ui.pEditBox);
	m_ui.pSendEditBox->setDelegate(this);
	m_ui.pSendEditBox->setFont(SystemUtils::getFontFileName().c_str(), CHAT_EDITBOX_FONTSIZE);
	m_ui.vecSendEditBox = m_ui.pSendEditBox->getPosition();
	m_ui.sizeSendEditBox = m_ui.pSendEditBox->getContentSize();
	m_ui.pEditBox->getParent()->addChild(m_ui.pSendEditBox);
	m_ui.pSendEditBox->setMaxLength(30);
	//默认打开的频道为综合
	m_ui.pSvContent = dynamic_cast<ListView*>(m_ui.pLayChat->getChildByName("listview_content"));
	FriendModel::getInstance()->setchatMessageDisWidth(m_ui.pSvContent->getContentSize().width);
	
	m_ui.pTextSend =  dynamic_cast<Text*>(m_ui.pLayChat->getChildByName("Lab_Fasong"));
	m_ui.pTextToName =  dynamic_cast<Text*>(m_ui.pLayChat->getChildByName("Lab_toName"));
	m_ui.pTextSend->setFontName(FONT_FZZHENGHEI);
	m_ui.pTextToName->setFontName(FONT_FZZHENGHEI);

	

	//有私聊消息未读
	m_ui.imgTishi->setVisible(false);
	if (FriendModel::getInstance()->getHavePrivateChatMessagesNotRead())
	{
		m_chatType = FRIEND_CHAT_PRIVATE;
		switchState(m_chatType);
	}
	else
	{
		m_chatType = FRIEND_CHAT_PUBLIC;
		switchState(m_chatType);
	}
}

void LayerChat::initPopupUI()
{
	m_chatPopup.pRoot =  dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_Renwu"));
	m_chatPopup.pRoot->addTouchEventListener(CC_CALLBACK_2(LayerChat::onBtnNotify, this));
	//
	Size size = Director::getInstance()->getVisibleSize();
	m_chatPopup.pRoot->setPosition(Vec2(size.width/2, m_chatPopup.pRoot->getPosition().y));//pos

	ImageView* pImgbg =  dynamic_cast<ImageView*>(m_chatPopup.pRoot->getChildByName("Img_Background"));
	m_chatPopup.pBtnAddFriend =  dynamic_cast<Button*>(pImgbg->getChildByName("Btn_Jiahaoyou"));
	m_chatPopup.pBtnAddFriend->addTouchEventListener(CC_CALLBACK_2(LayerChat::onBtnNotify, this));

	m_chatPopup.pBtnChatPrivate =  dynamic_cast<Button*>(pImgbg->getChildByName("Btn_Sixin"));
	m_chatPopup.pBtnChatPrivate->addTouchEventListener(CC_CALLBACK_2(LayerChat::onBtnNotify, this));	

	m_chatPopup.pTextName =  dynamic_cast<Text*>(pImgbg->getChildByName("Lab_Name"));
	m_chatPopup.pTextName->setFontName(FONT_FZZHENGHEI);
	
	Layout* pIcon =  dynamic_cast<Layout*>(pImgbg->getChildByName("Pnl_Renwu"));
	m_chatPopup.pToolIcon = UIToolHeader::create();
	m_chatPopup.pToolIcon->setScale(0.70);
	m_chatPopup.pToolIcon->setPosition(Vec2(pIcon->getContentSize().width/2, pIcon->getContentSize().height/2));
	pIcon->addChild(m_chatPopup.pToolIcon);

	m_chatPopup.pRoot->setVisible(false);
}

void LayerChat::onRecvData(int nResult, const char* data)
{

}

void LayerChat::onRecvChatMessage()
{
	updateChatContent(m_chatType);

	//有私聊消息未读
	if (m_chatType != FRIEND_CHAT_PRIVATE && FriendModel::getInstance()->getHavePrivateChatMessagesNotRead())
	{
		m_ui.imgTishi->setVisible(true);
	}
}

void LayerChat::onBtnNotify(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (sender == m_ui.pBtnSend)
		{
			if (m_chatType	== FRIEND_CHAT_PRIVATE && m_nFriendId == 0)
			{
				//CustomTips::show("请选择私聊对象！");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10128));
				m_ui.pSendEditBox->setText("");
				return;
			}
			const string strText = m_ui.pSendEditBox->getText();
			if (strText.length() > 60)
			{
				//CustomTips::show("输入的内容太长！");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10129));
				return;
			}
			else if (strText.length() == 0)
			{
				//CustomTips::show("请输入内容！");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10130));
				return;
			}
			if (secondNow() - m_nlastSendTime < MAX_SEND_MESSAGE_INTERVAL)
			{
				//CustomTips::show("发送消息太频繁！");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10131));
				return;
			}	
			m_nlastSendTime = secondNow();	

			m_ui.pSendEditBox->setText("");
			m_ui.pSendEditBox->setPreferredSize(m_ui.sizeSendEditBox);
			m_ui.pSendEditBox->setContentSize(m_ui.sizeSendEditBox);
			m_ui.pSendEditBox->setPosition(m_ui.vecSendEditBox);

			m_pChatController->onSendMessage(m_nFriendId, m_chatType, strText);	
		}
		else if (sender == m_ui.pBtnClose)
		{
			//启动定时器，在下一帧执行销毁操作，防止崩溃
			Director::getInstance()->getScheduler()->schedule(
				schedule_selector(LayerChat::callBackDispose), this, 0, 0, 0, false);
		}
		else if (sender == m_chatPopup.pBtnChatPrivate) //选中好友 私聊
		{
			m_chatPopup.pRoot->setVisible(false);

			m_chatType = FRIEND_CHAT_PRIVATE;
			switchState(m_chatType);
			m_ui.pTextToName->setText(m_chatPopup.pTextName->getString());
		}
		else if (sender == m_chatPopup.pBtnAddFriend) //选中好友 添加好友
		{
			m_chatPopup.pRoot->setVisible(false);
			//申请
			FriendController::getInstance()->onAddFriend(m_nFriendId);
		}
		else if (sender == m_chatPopup.pRoot)
		{
			m_chatPopup.pRoot->setVisible(false);
		}
	}
}

void LayerChat::onBtnClick(Ref* sender, CheckBox::EventType type)
{
	SoundUtils::playSoundEffect("dianji");
	if (type == CheckBox::EventType::SELECTED)
	{
		if (sender == m_ui.pBtnPublic) //综合
		{
			m_chatType = FRIEND_CHAT_PUBLIC;
			switchState(m_chatType);
		}
		else if (sender == m_ui.pBtnGroup) //帮派
		{
			if(MainModel::getInstance()->getMainParam()->mGroupId > 0) //已加入帮派
			{
				m_chatType = FRIEND_CHAT_GROUP;
				switchState(m_chatType);
			}
			else
			{
				m_ui.pBtnGroup->setSelectedState(false);
				//CustomTips::show("加入帮派才能发言！");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10132));
			}
		} 
		else if (sender == m_ui.pBtnPrivate)//私聊
		{
			m_ui.imgTishi->setVisible(false);
			m_chatType = FRIEND_CHAT_PRIVATE;
			switchState(m_chatType);
		}
	}
}

void LayerChat::callBackDispose(float dt)
{
	removeFromParent();
}

void LayerChat::onBtnChatClicked(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int id, std::string name, int resId, int level)
{
	if (Widget::TouchEventType::ENDED != type)return;
	if (id <= 0)
	{
		return;
	}
	
	//不能选择自己
	if (id == MainModel::getInstance()->getMainParam()->mRoleId)
		return;
	m_nFriendId = id;
	m_chatPopup.pRoot->setVisible(true);
	m_chatPopup.pTextName->setText(name);
	m_chatPopup.pToolIcon->setMainIcon(ResourceUtils::getSmallIconPath(resId));
	m_chatPopup.pToolIcon->setLvl(level);
}

void LayerChat::updateChatContent(FRIEND_CHAT_TYPE chatType)
{
	FRIEND_CHAT_MESSAGE_LIST& vchatMessages = FriendModel::getInstance()->getChatMessagesByType(chatType);
	
	m_ui.pSvContent->removeAllItems();
	for (auto iter = vchatMessages.begin(); iter != vchatMessages.end(); iter++)
	{
		iter->isRead = true;
		//
		RichText *pTextLine = RichText::create();

		pTextLine->setVerticalSpace(2.0);
		pTextLine->ignoreContentAdaptWithSize(false);//固定大小
		pTextLine->setContentSize(Size(FriendModel::getInstance()->getchatMessageDisWidth()- 20, iter->nDisHeight));

		pTextLine->setTouchEnabled(true);		
		//time 时间独立显示
		RichElementText *pTextTime = RichElementText::create(0, cocos2d::Color3B(0xec, 0xc2, 0x70), GLubyte(255), FriendModel::getInstance()->getCurTime(iter->strTime) + " ", FONT_FZZHENGHEI, MAX_CHAT_CONTENTFONT_SIZE);
		pTextLine->pushBackElement(pTextTime);

		//时间以后开始换行
		//content
		cocos2d::Color3B color = CHAT_COLOR_ME;
		if (iter->userId != MainModel::getInstance()->getMainParam()->mRoleId)
		{
			color = FriendModel::getInstance()->getChatContentColor(iter->type);

			//type
			RichElementImage* pImgType = RichElementImage::create(0, cocos2d::Color3B(0xff, 0xff, 0xff), GLubyte(255), FriendModel::getInstance()->getChatTypeIcon(iter->type));
			pTextLine->pushBackElement(pImgType);
			//vip
			if (iter->type < FRIEND_CHAT_SYSTEM)
			{
				pTextLine->addTouchEventListener(CC_CALLBACK_2(LayerChat::onBtnChatClicked, this, iter->userId, iter->strName, iter->resId, iter->lvl));
				RichElementImage* pImgVip = RichElementImage::create(0, cocos2d::Color3B(0xff, 0xff, 0xff), GLubyte(255), FriendModel::getInstance()->getChatVipIcon(iter->vipLvl));
				pTextLine->pushBackElement(pImgVip);
			}
		}		
		RichElementText *pText = RichElementText::create(0, color, GLubyte(255), iter->strContent, FONT_FZZHENGHEI, MAX_CHAT_CONTENTFONT_SIZE);
			
		pTextLine->pushBackElement(pText);		
		pTextLine->formatText();	
		m_ui.pSvContent->pushBackCustomItem(pTextLine);
	}

	m_ui.pSvContent->refreshView();
	m_ui.pSvContent->scrollToBottom(0.5, true);
}

void LayerChat::switchState(FRIEND_CHAT_TYPE chatType)
{
	switch (chatType)
	{
	case FRIEND_CHAT_PUBLIC:
		{
			m_ui.pBtnPrivate->setSelectedState(false);
			m_ui.pBtnPrivate->setTouchEnabled(true);

			m_ui.pBtnGroup->setSelectedState(false);
			m_ui.pBtnGroup->setTouchEnabled(true);

			m_ui.pBtnPublic->setSelectedState(true);
			m_ui.pBtnPublic->setTouchEnabled(false);
			m_nFriendId		= 0;
			updateChatContent(FRIEND_CHAT_PUBLIC);
			m_ui.pSendEditBox->setText("");
			m_ui.pTextToName->setText(DataManager::getInstance()->searchCommonTexdtById(10133));
		}
		break;
	case FRIEND_CHAT_GROUP:
		{
			m_ui.pBtnPrivate->setSelectedState(false);
			m_ui.pBtnPrivate->setTouchEnabled(true);

			m_ui.pBtnGroup->setSelectedState(true);
			m_ui.pBtnGroup->setTouchEnabled(false);

			m_ui.pBtnPublic->setSelectedState(false);
			m_ui.pBtnPublic->setTouchEnabled(true);
			m_nFriendId		= 0;
			updateChatContent(FRIEND_CHAT_GROUP);
			m_ui.pSendEditBox->setText("");
			m_ui.pTextToName->setText(DataManager::getInstance()->searchCommonTexdtById(10134));
		}
		break;
	case FRIEND_CHAT_PRIVATE:
		{
			m_ui.pBtnPrivate->setSelectedState(true);
			m_ui.pBtnPrivate->setTouchEnabled(false);

			m_ui.pBtnGroup->setSelectedState(false);
			m_ui.pBtnGroup->setTouchEnabled(true);

			m_ui.pBtnPublic->setSelectedState(false);
			m_ui.pBtnPublic->setTouchEnabled(true);
			updateChatContent(FRIEND_CHAT_PRIVATE);
			m_ui.pSendEditBox->setText("");
			m_ui.pTextToName->setText(DataManager::getInstance()->searchCommonTexdtById(10135));
		}
		break;
	default:
		break;
	}
}