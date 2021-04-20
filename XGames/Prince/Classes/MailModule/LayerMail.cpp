#include "LayerMail.h"

#include "CocoStudio.h"
#include "MailModel.h"
#include "MailController.h"
#include "Common/Common.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
#include "ResourceManager/ResourceLoader.h"
#include "UI/UIDef.h"
#include "Widget/LayerLoading.h"
#include "../ToolModule/ToolModel.h"
#include "Temp/CustomTips.h"
#include "HeroModule/HeroModel.h"
#include "../Utils/ResourceUtils.h"
#include "../Utils/StringFormat.h"
#include "../MainModule/MainModel.h"
#include "../FormationModule/CustomScrollBar.h"
#include "LevelUp/LevelUpController.h"
#include <stdio.h>

using namespace cocos2d;
using namespace cocos2d::ui;


LayerMail::LayerMail()
{
	m_nCurrMailId = 0;
	MailModel::getInstance()->addObserver(this);
	m_customScrollBarHero = nullptr;
}

LayerMail::~LayerMail()
{
	CCLOG("LayerMail destroy");
	MailModel::getInstance()->removeObserver(this);
}

void LayerMail::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
	case MAIL_TYPE_GET_MAIL:
		{
			updateMail();
		}
		break;
	case MAIL_TYPE_GET_ONE_PRIZE:
		{
			int id = *( (int*) observerParam->updateData);
			if (id < 0)
			{
				onGetAllPrize();
			}
			else
			{
				onGetOnePrize();
			}
		};
		break;
	case MAIL_TYPE_DEL_ONE_MAIL:
		{
			m_ui.pLayDetail->setVisible(false);

			m_ui.pBtnDelete->setVisible(false);
			m_ui.pBtnReturn->setVisible(false);
			m_ui.pBtnGetReward->setVisible(false);

			m_ui.pBtnDeleteNow->setVisible(true);
			m_ui.pBtnGetRewardNow->setVisible(true);
			m_ui.mailList->setVisible(true);
			m_customScrollBarHero->setVisible(true);
			m_ui.mailTips->setVisible(true);

			int i = *( (int*) observerParam->updateData);
			delCurItem(i);
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20276));

			setScroll();
		}
		break;
	case MAIL_TYPE_DEL_ALL_MAIL:
		{
			updateMail();
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20276));
			
			setScroll();
		}
		break;
	default:
		break;
	}
}

bool LayerMail::init()
{
	if(!ModalLayer::init())
	{
		return false;
	}
	
	initUI();
	
	updateMail();
	return true;
}

void LayerMail::onEnter()
{
	Layer::onEnter();
}

void LayerMail::onExit()
{
	CCLOG("LayerMail onExit");
	Layer::onExit();
	ResourceLoader::getInstance()->removeUIResource("UINewmail");
	cocostudio::GUIReader::getInstance()->destroyInstance();
}

void LayerMail::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Vec2 pos = Director::getInstance()->getVisibleOrigin();

	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UINewmail/UINewmail.ExportJson"));
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);

	this->addChild(m_ui.pRoot);

	string strFileName;
	strFileName.assign("Image/UINewmail/UINewmail_node.ExportJson");
	m_ui.pRootItem = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.pRootItem);
	m_ui.pRootItem->setVisible(false);
	//
	initUIMail();
}

void LayerMail::initUIMail()
{
	m_ui.ImageDi = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Image_di"));
	ActionCreator::runCommDlgAction(m_ui.ImageDi);

	//退出
	m_ui.pBtnBack =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ImageDi,"Button_guanbi"));
	auto backCall = [this](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			dispose();
		}
	};
	m_ui.pBtnBack->addTouchEventListener(backCall);
	//
	m_ui.pLayDetail = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Image_xiangqing"));
	m_ui.mailTitle = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pLayDetail,"Label_youjianbiaoti2")); // 标题
	m_ui.mailSrc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pLayDetail,"Label_fajianren")); // 发件人
	m_ui.pnlTools = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pLayDetail,"Pnl_tools")); //奖励
	m_ui.mailDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pLayDetail,"Label_youjianneirong")); //邮件内容
	m_ui.ListView_Item = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.pnlTools,"ListView_Item")); //邮件内容
	m_ui.mailTitle->setFontName(FONT_FZZHENGHEI);
	m_ui.mailSrc->setFontName(FONT_FZZHENGHEI);
	m_ui.mailDesc->setFontName(FONT_FZZHENGHEI);

	m_ui.mailCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ImageDi,"Label_shuliang")); //邮件数量 
	m_ui.mailCount->setFontName(FONT_FZZHENGHEI);

	//一键删除
	m_ui.pBtnDeleteNow =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ImageDi,"Button_yijianshanchu"));
	m_ui.pBtnDeleteNow->addTouchEventListener(CC_CALLBACK_2(LayerMail::onBtnDeleteNow, this));
	//一键领取
	m_ui.pBtnGetRewardNow =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ImageDi,"Button_yijianlingqu"));
	m_ui.pBtnGetRewardNow->addTouchEventListener(CC_CALLBACK_2(LayerMail::onBtnGetRewardNow, this));

	//返回
	m_ui.pBtnReturn =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ImageDi,"Button_fanhui"));
	m_ui.pBtnReturn->addTouchEventListener(CC_CALLBACK_2(LayerMail::onBtnReturn, this));
	//删除
	m_ui.pBtnDelete =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ImageDi,"Btn_delete"));
	m_ui.pBtnDelete->addTouchEventListener(CC_CALLBACK_2(LayerMail::onBtnDelete, this));
	//领取
	m_ui.pBtnGetReward =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ImageDi,"Button_lingqu"));
	m_ui.pBtnGetReward->addTouchEventListener(CC_CALLBACK_2(LayerMail::onBtnGetReward, this));

	m_ui.mailTips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ImageDi,"Image_tips")); //提示
	m_ui.noMail = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ImageDi,"Image_meiyouyoujian")); //没有邮件
	m_ui.mailList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.ImageDi,"mailListView"));
	m_customScrollBarHero = CustomScrollBar::create();
	m_customScrollBarHero->initScrollBar(m_ui.mailList->getContentSize().height);
	m_customScrollBarHero->setPosition(m_ui.mailList->getPosition() + Point(-12, 0));
	m_ui.ImageDi->addChild(m_customScrollBarHero);
	m_customScrollBarHero->setScrollBarLengthPercent(1.0f);
	m_customScrollBarHero->setScrollBarPercentY(0.0f);
	dynamic_cast<ScrollView*>(m_ui.mailList)->addEventListener(CC_CALLBACK_2(LayerMail::onScrollViewEvent, this));

	//
	m_ui.pLayDetail->setVisible(false);
	m_ui.pBtnReturn->setVisible(false);
	m_ui.pBtnDelete->setVisible(false);
	m_ui.pBtnGetReward->setVisible(false);
	m_ui.pBtnDeleteNow->setVisible(true);
	m_ui.pBtnGetRewardNow->setVisible(true);
}

void LayerMail::onBtnDeleteNow(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		//如果还有附件，提示
		const MAIL_INFOS_DEQUE& mailList = MailModel::getInstance()->getMails();
		if (mailList.size() == 0)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20277));
			return;
		}

		bool hasAttach = false;
		for (auto& iter:mailList)
		{
			if (iter.vAccessory.size() > 0  && MAIL_STAT_GOT <= iter.stat)
			{
				hasAttach = true;
				break;
			}		
		}
		if (!hasAttach)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20277));
			return;
		}
	
		MailController::getInstance()->onDeleteMail(-1);
	}
}
void LayerMail::onBtnGetRewardNow(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		//如果没有附件，提示
		bool hasAttach = false;
		const MAIL_INFOS_DEQUE& mailList = MailModel::getInstance()->getMails();
		if (mailList.size() == 0)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20278));
			return;
		}
		for (auto& iter:mailList)
		{
			if (iter.vAccessory.size() > 0  && MAIL_STAT_GOT > iter.stat)
			{
				hasAttach = true;
				break;
			}		
		}
		if (!hasAttach)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20278));
			return;
		}
		//
		MailController::getInstance()->onGetAccessory(-1);
	}	
}

void LayerMail::onBtnDelete(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		MAIL_INFOS mailInfo;
		bool res = MailModel::getInstance()->getMail(m_nCurrMailId, mailInfo);
		if (!res)
		{
			return;
		}
		if (mailInfo.vAccessory.size() > 0  && MAIL_STAT_GOT > mailInfo.stat)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20279));
			return;
		}	
		MailController::getInstance()->onDeleteMail(m_nCurrMailId);
	}
}

void LayerMail::onBtnReturn(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		m_ui.pLayDetail->setVisible(false);
	
		m_ui.pBtnDelete->setVisible(false);
		m_ui.pBtnReturn->setVisible(false);
		m_ui.pBtnGetReward->setVisible(false);

		m_ui.pBtnDeleteNow->setVisible(true);
		m_ui.pBtnGetRewardNow->setVisible(true);
		m_ui.mailList->setVisible(true);
		m_customScrollBarHero->setVisible(true);
		m_ui.mailTips->setVisible(true);

		updateCurItem();
	}
}
void LayerMail::onBtnGetReward(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		MAIL_INFOS mailInfo;
		bool res = MailModel::getInstance()->getMail(m_nCurrMailId, mailInfo);
		if (!res)
		{
			return;
		}
		if (mailInfo.vAccessory.size() == 0 || MAIL_STAT_GOT == mailInfo.stat)
		{
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20280));
			return;
		}
		//领取
		MailController::getInstance()->onGetAccessory(m_nCurrMailId);
	}
}
void LayerMail::onBtnCheckMail(Ref* sender, cocos2d::ui::Widget::TouchEventType type, int id)
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");

		openMail(id,true);
	}
}
void LayerMail::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	switch(type)
	{
	case cocos2d::ui::ScrollView::EventType::SCROLLING:
		{
			Size size = m_ui.mailList->getInnerContainerSize();
			Vec2 vec = m_ui.mailList->getInnerContainer()->getPosition();

			float percentY = 0;
			if (size.height == m_ui.mailList->getCustomSize().height)
			{
				percentY = 0;
			}
			else
			{
				percentY = -vec.y / (size.height - m_ui.mailList->getContentSize().height);
			}

			m_customScrollBarHero->setScrollBarPercentY((1.0f - percentY)*100.0f);


			MAIL_INFOS_DEQUE& vmailList = MailModel::getInstance()->getMails();
			if (vmailList.size() > MAIL_ITEM_COUNT)
			{

				int idx1 = ceil((vmailList.size()-MAIL_ITEM_COUNT)*(1.0f-percentY) + MAIL_ITEM_COUNT-1);
				int idx2 = floor((vmailList.size()-MAIL_ITEM_COUNT)*(1.0f-percentY) + MAIL_ITEM_COUNT-1);

				if ( idx1 >= 0 && idx1 < vmailList.size() && !vmailList.at(idx1).isInit)
				{
					setItemByIdx(idx1);
				}
				if (idx2 >= 0 && idx2 < vmailList.size() && !vmailList.at(idx2).isInit)
				{
					setItemByIdx(idx2);
				}
			}
		}

		break;
	default:
		break;
	}
}

void LayerMail::setScroll()
{
	m_ui.mailList->refreshView();
	m_customScrollBarHero->setScrollBarLengthPercent(
		m_ui.mailList->getContentSize().height / m_ui.mailList->getInnerContainerSize().height);
}

void LayerMail::updateMail()
{
	m_ui.mailList->removeAllItems();
	MAIL_INFOS_DEQUE& vmailList = MailModel::getInstance()->getMails();
	m_ui.mailCount->setString(TO_STR(vmailList.size()) + "/" + TO_STR(MailModel::getInstance()->getMailCountMax()));

	if (vmailList.size() <= 0)
	{
		m_ui.noMail->setVisible(true);
		m_customScrollBarHero->setVisible(false);
		return;
	}

	m_ui.noMail->setVisible(false);
	m_customScrollBarHero->setVisible(true);

	for (int i = 0; i < vmailList.size(); i++)
	{
		vmailList.at(i).layer = m_ui.pRootItem->clone();
		if (i < (((vmailList.size()) < (MAIL_ITEM_COUNT)) ? (vmailList.size()) : (MAIL_ITEM_COUNT))) //前面五个
		{
			setItemByIdx(i);
		}
		else
		{
			vmailList.at(i).isInit = false;
		}
		vmailList.at(i).layer->setVisible(true);
		m_ui.mailList->pushBackCustomItem(vmailList.at(i).layer);
	}

	m_ui.mailList->refreshView();
	m_ui.mailList->jumpToTop();

	m_customScrollBarHero->setScrollBarLengthPercent(
		m_ui.mailList->getContentSize().height / m_ui.mailList->getInnerContainerSize().height);
	m_customScrollBarHero->setScrollBarPercentY(0.0f);
}

void LayerMail::setItemByIdx(int idx)
{
	MAIL_INFOS_DEQUE& vmailList = MailModel::getInstance()->getMails();
	Button *pBtnItem = dynamic_cast<Button*>(Helper::seekWidgetByName(vmailList.at(idx).layer,"Button_youjianjichu"));
	Text* pTextTitle = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_title"));
	Text* pTextDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_youjianbiaoti"));
	Text* pTextSendtime = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_riqi"));
	pTextTitle->setFontName(FONT_FZZHENGHEI);
	pTextDesc->setFontName(FONT_FZZHENGHEI);
	pTextSendtime->setFontName(FONT_FZZHENGHEI);

	ImageView* pImgAccessory = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pBtnItem,"Image_fujian"));

	pTextTitle->setString(vmailList.at(idx).sender);
	setTextAddStroke(pTextTitle,Color3B(0x0b,0x38,0x04),2);
	pTextDesc->setString(vmailList.at(idx).strTitle);
	setTextAddStroke(pTextDesc,Color3B(0x00,0x00,0x00),2);

	//只显示年月日 //2015-11-11 12:12
	if (vmailList.at(idx).strTime.length() > 10)
	{
		pTextSendtime->setText(vmailList.at(idx).strTime.substr(0, 10));
		setTextAddStroke(pTextSendtime,Color3B(0x78,0x00,0x00),2);
	}

	if (vmailList.at(idx).vAccessory.size() > 0 && MAIL_STAT_GOT > vmailList.at(idx).stat)
	{
		pImgAccessory->setVisible(true);
	}
	else
	{
		pImgAccessory->setVisible(false);
	}

	pBtnItem->addTouchEventListener(CC_CALLBACK_2(LayerMail::onBtnCheckMail, this, vmailList.at(idx).id));

	vmailList.at(idx).isInit = true;
}

void LayerMail::updateCurItem()
{
	const MAIL_INFOS_DEQUE& vmailList = MailModel::getInstance()->getMails();
	m_ui.mailCount->setString(TO_STR(vmailList.size()) + "/" + TO_STR(MailModel::getInstance()->getMailCountMax()));

	int idx = 0;
	MAIL_INFO mailInfo;
	for (int i = 0; i < vmailList.size(); i++)
	{
		if (m_nCurrMailId == vmailList.at(i).id)
		{
			mailInfo = vmailList.at(i);
			break;
		}
		else
		{
			idx++;
		}
	}

	Widget * item = m_ui.mailList->getItem(idx);

	Button *pBtnItem = dynamic_cast<Button*>(Helper::seekWidgetByName(item,"Button_youjianjichu"));
	ImageView* pImgAccessory = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pBtnItem,"Image_fujian"));

	if (mailInfo.vAccessory.size() > 0 && MAIL_STAT_GOT > mailInfo.stat)
	{
		pImgAccessory->setVisible(true);
	}
	else
	{
		pImgAccessory->setVisible(false);
	}
}

void LayerMail::updateAllItem()
{
	const MAIL_INFOS_DEQUE& vmailList = MailModel::getInstance()->getMails();
	m_ui.mailCount->setString(TO_STR(vmailList.size()) + "/" + TO_STR(MailModel::getInstance()->getMailCountMax()));

	for (int i = 0; i < vmailList.size(); i++)
	{
		MAIL_INFO mailInfo = vmailList.at(i);

		Widget * item = m_ui.mailList->getItem(i);

		Button *pBtnItem = dynamic_cast<Button*>(Helper::seekWidgetByName(item,"Button_youjianjichu"));
		ImageView* pImgAccessory = dynamic_cast<ImageView*>(Helper::seekWidgetByName(pBtnItem,"Image_fujian"));

		if (mailInfo.vAccessory.size() > 0 && MAIL_STAT_GOT > mailInfo.stat)
		{
			pImgAccessory->setVisible(true);
		}
		else
		{
			pImgAccessory->setVisible(false);
		}
	}
}

void LayerMail::delCurItem(int idx)
{
	m_ui.mailList->removeItem(idx);

	const MAIL_INFOS_DEQUE& vmailList = MailModel::getInstance()->getMails();
	m_ui.mailCount->setString(TO_STR(vmailList.size()) + "/" + TO_STR(MailModel::getInstance()->getMailCountMax()));

	if (vmailList.size() <= 0)
	{
		m_ui.noMail->setVisible(true);
		m_customScrollBarHero->setVisible(false);
	}
	else
	{
		m_ui.noMail->setVisible(false);
		m_customScrollBarHero->setVisible(true);
	}
}

void LayerMail::openMail(const int& id,bool action)
{
	m_nCurrMailId = id;
	MAIL_INFO mailInfo;
	if(!MailModel::getInstance()->getMail(m_nCurrMailId, mailInfo))
	{
		return;
	}
	//向服务器发送消息，更新状态
	if (mailInfo.stat == MAIL_STAT_NEW)
	{
		MailController::getInstance()->onOpenMail(id);
	}	
	//
	m_ui.mailList->setVisible(false);
	m_customScrollBarHero->setVisible(false);
	m_ui.mailTips->setVisible(false);
	m_ui.pLayDetail->setVisible(true);
	if (action)
	{
		ActionCreator::runCommDlgAction(m_ui.pLayDetail);
	}
	
	m_ui.pBtnDeleteNow->setVisible(false);
	m_ui.pBtnGetRewardNow->setVisible(false);	
	m_ui.pBtnReturn->setVisible(true);
	
	if (mailInfo.vAccessory.size() > 0)//有附件
	{	
		if (mailInfo.stat == MAIL_STAT_GOT)//已经领取
		{
			m_ui.pBtnDelete->setVisible(true);
			m_ui.pBtnGetReward->setVisible(false);
		}
		else
		{
			m_ui.pBtnDelete->setVisible(false);
			m_ui.pBtnGetReward->setVisible(true);
		}		
		
		//设置标题		
		m_ui.mailTitle->setString(mailInfo.strTitle);
		setTextAddStroke(m_ui.mailTitle,Color3B(0x00,0x00,0x00),2);
		m_ui.mailSrc->setString(mailInfo.sender);
		setTextAddStroke(m_ui.mailSrc,Color3B(0x0b,0x38,0x04),2);

		if (mailInfo.stat != MAIL_STAT_GOT)//已经领取
		{
			m_ui.pnlTools->setVisible(true);
			m_ui.ListView_Item->removeAllItems();
			for (int i = 0; i < mailInfo.vAccessory.size(); i++)
			{
				UIToolHeader *icon = nullptr;

				switch (mailInfo.vAccessory.at(i).id)
				{
				case GOLD_ITEM_ID://金币
					{
						icon = UIToolHeader::create(GOLD_ITEM_ID);

					}
					break;
				case COIN_ITEM_ID://银币
					{
						icon = UIToolHeader::create(COIN_ITEM_ID);
					}
					break;
				case PVP_ITEM_ID: //竞技场积分
					{
						icon = UIToolHeader::create(PVP_ITEM_ID);
					}
					break;
				case SHILIAN_ITEM_ID: //试炼积分
					{
						icon = UIToolHeader::create(SHILIAN_ITEM_ID);
					}
					break;
				case GROUP_ITEM_ID: //帮派积分
					{
						icon = UIToolHeader::create(GROUP_ITEM_ID);
					}
					break;
				case HUNTIAN_ITEM_ID: //浑天丹
					{
						icon = UIToolHeader::create(HUNTIAN_ITEM_ID);
					}
					break;
				case JIUZHUAN_ITEM_ID: //九转丹
					{
						icon = UIToolHeader::create(JIUZHUAN_ITEM_ID);
					}
					break;
				case VIPEXP_ITEM_ID: //VIP经验
					{
						icon = UIToolHeader::create(VIPEXP_ITEM_ID);
					}
					break;
				case CUILIAN_ITEM_ID: //淬炼碎片
					{
						icon = UIToolHeader::create(CUILIAN_ITEM_ID);
					}
					break;
				case EXP_ITEM_ID: //主角经验
					{
						icon = UIToolHeader::create(ResourceUtils::getSmallIconPath("exp"));
					}
					break;
				default:
					icon = UIToolHeader::create(mailInfo.vAccessory.at(i).id);
					break;
				}

				icon->setNumEx(mailInfo.vAccessory.at(i).amount);
				//icon->setAnchorPoint(Vec2(0,0));
				icon->setScale(0.75f);
				icon->setToolId(mailInfo.vAccessory.at(i).id);
				icon->setTipsEnabled(true);

				m_ui.ListView_Item->pushBackCustomItem(icon);
			}
			m_ui.ListView_Item->refreshView();
		}	
		else
		{
			m_ui.pnlTools->setVisible(false);
		}
	}
	else
	{
		m_ui.pBtnDelete->setVisible(true);
		m_ui.pBtnGetReward->setVisible(false);
		m_ui.pBtnReturn->setVisible(true);
		m_ui.pnlTools->setVisible(false);

		//设置标题		
		m_ui.mailTitle->setString(mailInfo.strTitle);
		setTextAddStroke(m_ui.mailTitle,Color3B(0x00,0x00,0x00),2);
		m_ui.mailSrc->setString(mailInfo.sender);
		setTextAddStroke(m_ui.mailSrc,Color3B(0x0b,0x38,0x04),2);
	}

	//设置内容
	if (m_ui.mailDesc->getChildByName("upfontText"))
	{
		m_ui.mailDesc->removeChildByName("upfontText");
	}
	bool ignoreSize = m_ui.mailDesc->isIgnoreContentAdaptWithSize();
	int fontSize = m_ui.mailDesc->getFontSize();
	Size ContentSize = m_ui.mailDesc->getContentSize();
	Text* up = Text::create(mailInfo.strContent, FONT_FZZHENGHEI, fontSize); 
	up->setContentSize(ContentSize);
	up->ignoreContentAdaptWithSize(ignoreSize);
	up->setColor(m_ui.mailDesc->getColor());  
	up->setPosition(Vec2(0,220));
	up->setAnchorPoint(Vec2(0,1.0f));
	m_ui.mailDesc->addChild(up,0,"upfontText");
	setTextAddStroke(up,Color3B(0x78,0x00,0x00),1);
}

void LayerMail::onGetOnePrize()
{
	openMail(m_nCurrMailId);
	showPopReward();
}

void LayerMail::onGetAllPrize()
{
	updateAllItem();
	showPopReward();
	//CustomTips::show("成功领取的奖励");
}

void LayerMail::showPopReward()
{
	MAIL_INFO_ACCESSORY_LIST* Reawrd = MailModel::getInstance()->getReawrd();
	vector<PopRewardItem*> vecRewardItem;
	//道具
	for (size_t i = 0; i < Reawrd->size(); i++)
	{
		switch (Reawrd->at(i).id)
		{
		case GOLD_ITEM_ID://金币
			{
				PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(Reawrd->at(i).amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case COIN_ITEM_ID://银币
			{
				PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(Reawrd->at(i).amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case PVP_ITEM_ID: //竞技场积分
			{
				PopRewardItem_PvpScore* rewardItem = new PopRewardItem_PvpScore(Reawrd->at(i).amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case SHILIAN_ITEM_ID: //试炼积分
			{
				PopRewardItem_TrialScore* rewardItem = new PopRewardItem_TrialScore(Reawrd->at(i).amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case GROUP_ITEM_ID: //帮派积分
			{
				PopRewardItem_BanGong* rewardItem = new PopRewardItem_BanGong(Reawrd->at(i).amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case HUNTIAN_ITEM_ID: //浑天丹
			{
				PopRewardItem_HunTian* rewardItem = new PopRewardItem_HunTian(Reawrd->at(i).amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case JIUZHUAN_ITEM_ID: //九转丹
			{
				PopRewardItem_JiuZhuan* rewardItem = new PopRewardItem_JiuZhuan(Reawrd->at(i).amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case VIPEXP_ITEM_ID: //VIP经验
			{
				PopRewardItem_VipExp* rewardItem = new PopRewardItem_VipExp(Reawrd->at(i).amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case CUILIAN_ITEM_ID: //淬炼碎片
			{
				PopRewardItem_CuiLian* rewardItem = new PopRewardItem_CuiLian(Reawrd->at(i).amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		case EXP_ITEM_ID: //主角经验
			{
				PopRewardItem_Exp* rewardItem = new PopRewardItem_Exp(Reawrd->at(i).amount);
				vecRewardItem.push_back(rewardItem);
			}
			break;
		default:
			PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(Reawrd->at(i).id, Reawrd->at(i).amount);
			vecRewardItem.push_back(rewardItem);
			break;
		}
	}

	auto call = [this](Ref* ref, PopRewardEventType type)->void
	{
		if (type == EVENT_TYPE_END)
		{
			//检测升级
			LevelUpController::getInstance()->checkLevelUp();
		}
	};
	PopReward::show(vecRewardItem,call);
}