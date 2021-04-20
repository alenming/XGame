#include "LayerFriend.h"

#include "CocoStudio.h"
#include "FriendModel.h"
#include "FriendController.h"
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
#include "Widget/ActionCreator.h"
#include "Temp/CustomTips.h"
#include "Widget/EditBoxEx.h"
#include "GuideCenter/GuideManager.h"


using namespace cocos2d;
using namespace cocos2d::ui;


LayerFriend::LayerFriend()
	: mFirstGetFriendList(true)
{

}

LayerFriend::~LayerFriend()
{
	CCLOG("LayerFriend destroy");
}

bool LayerFriend::init()
{
	if(!ModalLayer::init())
	{
		return false;
	}
	
	initUI();
	FriendController::getInstance()->onGetFriendsList();
	
	return true;
}

void LayerFriend::onEnter()
{
	ModalLayer::onEnter();
	FriendModel::getInstance()->addObserver(this);
	ResourceLoader::getInstance()->removeUIResource("UIFriend");
}

void LayerFriend::onExit()
{
	CCLOG("LayerFriend onExit");
	ModalLayer::onExit();
	FriendModel::getInstance()->removeObserver(this);
}

void LayerFriend::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
	case FRIEND_EVENT_TYPE_GET_LIST:
		{
			if (mFirstGetFriendList)
			{
				GuideManager::getInstance()->startGuide(GUIDE_INERFACE_FRIEND);
				mFirstGetFriendList = false;
			}
			updateMyFriend();
		}
		break;
	case FRIEND_EVENT_TYPE_GIVE_POWER:
		{
			//CustomTips::show("赠送体力成功");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10136));
			updataMyFriendListItemGive(m_friendIndx);
		}
		break;
	case FRIEND_EVENT_TYPE_GET_POWER:
		{
			//CustomTips::show("领取体力成功");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10137));
			updataMyFriendListItemGet(m_friendIndx);
		}
		break;
	case FRIEND_EVENT_TYPE_ONE_KEY_GIVE_POWER:
		{
			//CustomTips::show("一键赠送体力成功");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10138));
			const FRIEND_INFOS_LIST& friendsInfo = FriendModel::getInstance()->getFriends();
			for (int i = 0; i < friendsInfo.size(); i++)
			{
				updataMyFriendListItemGive(friendsInfo.at(i).userId);
			}
		}
		break;
	case FRIEND_EVENT_TYPE_ONE_KEY_GET_POWER:
		{
			//CustomTips::show("一键领取体力成功");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10139));
			m_ui.friendCount->setString(TO_STR(FriendModel::getInstance()->getGiftPower()) + "/" + 
				TO_STR(FriendModel::getInstance()->getMaxPower()));
			setTextAddStroke(m_ui.friendCount, Color3B(0x89, 0x0f, 0x0f), 2);

			const FRIEND_INFOS_LIST& friendsInfo = FriendModel::getInstance()->getFriends();
			for (int i = 0; i < friendsInfo.size(); i++)
			{
				updataMyFriendListItemGet(friendsInfo.at(i).userId);
			}
		}
		break;
	case FRIEND_EVENT_TYPE_ADD_LIST:
		{
			updateAddFriend();
		}
		break;
	case FRIEND_EVENT_TYPE_ADD_FRIEND:
		{
			//CustomTips::show("好友申请发送成功");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10140));
			updataAddFriendListItemGet(m_friendIndx);
		}
		break;
	case FRIEND_EVENT_TYPE_SEARCH_FRIEND:
		{
			m_ui.pBtnAddBack->setVisible(true);
			m_ui.pBtnChange->setVisible(false);

			m_Friend_add_type = Friend_add_type_search;
			FriendModel::getInstance()->setFriendAddType(Friend_add_type_search);

			const ADD_FRIEND_INFO_LIST& friendsInfo = FriendModel::getInstance()->getSearchfriends();

			if (friendsInfo.size() <= 0)
			{
				m_ui.addFriendList->removeAllItems();
				m_ui.addimgTips->setVisible(true);
				ActionCreator::runCommDlgAction(m_ui.addimgTips);
			}
			else
			{
				updateSearchFriend();
			}
		}
		break;
	case FRIEND_EVENT_TYPE_APPLY_LIST:
		{
			updateApplyFriend();
		}
		break;
	case FRIEND_EVENT_TYPE_APPLY_FRIEND:
		{
			Friend_Apply_type type = FriendModel::getInstance()->getFriendApplyType();
			if (type == Friend_Apply_type_refuse)
			{
				//CustomTips::show("拒绝好友申请");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10141));
				updateApplyFriendItem();
			}
			else if (type == Friend_Apply_type_agree)
			{
				//CustomTips::show("成功添加好友");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10142));
				updateApplyFriendItem();
			}
		}
		break;
	case FRIEND_EVENT_TYPE_ONE_KEY_APPLY_FRIEND:
		{
			Friend_Apply_type type = FriendModel::getInstance()->getFriendApplyType();
			if (type == Friend_Apply_type_one_key_refuse)
			{
				m_ui.requestfriendList->removeAllItems();
			}
			else if (type == Friend_Apply_type_one_key_agree)
			{
				//CustomTips::show("成功添加好友");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10142));
				updateApplyFriend();
			}
		}
		break;
	case FRIEND_EVENT_TYPE_DELETE_FRIEND:
		{
			m_ui.friendList->removeChildByTag(m_friendIndx);
			m_ui.friendList->refreshView();
			m_ui.playChoose->setVisible(false);
		}
		break;
	case FRIEND_EVENT_TYPE_TUI_SONG:
		{
			if (FriendModel::getInstance()->getHaveNews() && m_Friend_type != Friend_type_player)
			{
				m_ui.pBtnFriend->getChildByName("dian")->setVisible(true);
			}

			if (FriendModel::getInstance()->getHaveFriend() && m_Friend_type != Friend_type_player)
			{
				m_ui.pBtnRequestFriend->getChildByName("dian")->setVisible(true);
			}
		}
		break;
	default:
		break;
	}
}

void LayerFriend::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.pRoot = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIFriend/UIFriend.ExportJson"));
	m_ui.pRoot->setSize(size);
	m_ui.pRoot->setPosition(pos);
	// 将UILayer层加入到当前的场景
	this->addChild(m_ui.pRoot);	

	m_ui.imgFriend = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Image_friend"));  //好友面板

	m_ui.imgchat = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Image_chat"));  //聊天面板
	m_ui.imgFriend->setPosition(Vec2(m_ui.pRoot->getCustomSize().width/2,m_ui.imgFriend->getPosition().y));
	m_ui.imgFriend->setVisible(true);
	ActionCreator::runCommDlgAction(m_ui.imgFriend);
	m_ui.imgchat->setVisible(false);
	initUIFriend();
}

void LayerFriend::initUIFriend()
{
	m_ui.pBtnBack =  dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.imgFriend,"Button_close"));
	m_ui.pBtnBack->addTouchEventListener(CC_CALLBACK_2(LayerFriend::onBtnNotify,this));

	m_ui.pBtnFriend =  dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.imgFriend,"Button_myfriend"));
	m_ui.pBtnFriend->addEventListener(CC_CALLBACK_2(LayerFriend::onCheckNotify, this));
	m_ui.pBtnAddFriend =  dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.imgFriend,"Button_add"));
	m_ui.pBtnAddFriend->addEventListener(CC_CALLBACK_2(LayerFriend::onCheckNotify, this));
	m_ui.pBtnRequestFriend =  dynamic_cast<CheckBox*>(Helper::seekWidgetByName(m_ui.imgFriend,"Button_request"));
	m_ui.pBtnRequestFriend->addEventListener(CC_CALLBACK_2(LayerFriend::onCheckNotify, this));

	//我的好友
	m_ui.pLayFriend = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.imgFriend,"myFriend"));
	m_ui.pBtnGive = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLayFriend,"Button_give"));
	m_ui.pBtnGive->addTouchEventListener(CC_CALLBACK_2(LayerFriend::onBtnNotify, this));
	m_ui.pBtnGet = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLayFriend,"Button_get"));
	m_ui.pBtnGet->addTouchEventListener(CC_CALLBACK_2(LayerFriend::onBtnNotify, this));
	m_ui.friendCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pLayFriend,"Label_number_1"));
	m_ui.friendList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.pLayFriend,"ListView_friend"));
	m_ui.friendCount->setFontName(FONT_FZZHENGHEI);
	//添加好友
	m_ui.pLayAddFriend = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.imgFriend,"addFirend"));
	m_ui.pBtnAddBack = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLayAddFriend,"Button_back"));
	m_ui.pBtnAddBack->addTouchEventListener(CC_CALLBACK_2(LayerFriend::onBtnNotify, this));

	m_ui.pBtnChange = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLayAddFriend,"Button_change"));
	m_ui.pBtnChange->addTouchEventListener(CC_CALLBACK_2(LayerFriend::onBtnNotify, this));

	m_ui.friendAddCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pLayAddFriend,"Label_number_2"));
	m_ui.pBtnSearch = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLayAddFriend,"Button_search"));
	m_ui.pBtnSearch->addTouchEventListener(CC_CALLBACK_2(LayerFriend::onBtnNotify, this));
	m_ui.friendAddCount->setFontName(FONT_FZZHENGHEI);

	TextField *inputName = dynamic_cast<TextField*>(Helper::seekWidgetByName(m_ui.pLayAddFriend,"input_name"));
	m_ui.addFriendList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.pLayAddFriend,"ListView_addfriend"));
	m_ui.addimgTips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pLayAddFriend,"Image_tips"));
	m_ui.addimgTips->setVisible(false);
	m_ui.inputName = EditBoxEx::create(inputName);
	m_ui.inputName->setFont(SystemUtils::getFontFileName().c_str(),28);
	m_ui.inputName->setPosition(inputName->getPosition());
	m_ui.inputName->setContentSize(inputName->getContentSize());
	inputName->getParent()->addChild(m_ui.inputName);
	m_ui.inputName->setMaxLength(30);
	m_ui.inputName->setText("");

	//好友申请
	m_ui.pLayRequestFriend = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.imgFriend,"questFriend"));
	m_ui.pBtnRefuse = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLayRequestFriend,"Button_refuse"));
	m_ui.pBtnRefuse->addTouchEventListener(CC_CALLBACK_2(LayerFriend::onBtnNotify, this));

	m_ui.pBtnAgree = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pLayRequestFriend,"Button_agree"));
	m_ui.pBtnAgree->addTouchEventListener(CC_CALLBACK_2(LayerFriend::onBtnNotify, this));

	m_ui.requestfriendList = dynamic_cast<ListView*>(Helper::seekWidgetByName(m_ui.pLayRequestFriend,"ListView_requestfriend"));
	
	//
	m_ui.playChoose = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.imgFriend,"playChoose"));
	m_ui.addImgTips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.imgFriend,"Image_player"));
	m_ui.ButtonMessage = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.addImgTips,"Button_message"));
	m_ui.ButtonMessage->addTouchEventListener(CC_CALLBACK_2(LayerFriend::onBtnNotify, this));
	m_ui.playChoose->setVisible(false);
	auto playChooseCall = [this](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else
		{
			m_ui.playChoose->setVisible(false);
		}
	};
	m_ui.playChoose->addTouchEventListener(playChooseCall);

	m_ui.addName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.addImgTips,"Label_name"));
	m_ui.iconPlayer = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.addImgTips,"Panel_icon"));
	m_ui.addName->setFontName(FONT_FZZHENGHEI);

	m_Friend_type = Friend_type_player;
	swichFriendState();
}

void LayerFriend::swichFriendState()
{
	switch (m_Friend_type)
	{
	case Friend_type_player:
		{
			m_ui.pLayFriend->setVisible(true);
			m_ui.pLayAddFriend->setVisible(false);
			m_ui.pLayRequestFriend->setVisible(false);

			m_ui.pBtnFriend->setTouchEnabled(false);
			m_ui.pBtnFriend->setSelectedState(true);

			m_ui.pBtnAddFriend->setTouchEnabled(true);
			m_ui.pBtnAddFriend->setSelectedState(false);
			m_ui.pBtnRequestFriend->setTouchEnabled(true);
			m_ui.pBtnRequestFriend->setSelectedState(false);

			FriendModel::getInstance()->getHaveNews() = false;
			m_ui.pBtnFriend->getChildByName("dian")->setVisible(false);

			if (FriendModel::getInstance()->getHaveFriend())
			{
				m_ui.pBtnRequestFriend->getChildByName("dian")->setVisible(true);
			}
		}
		break;
	case Friend_type_add:
		{
			m_ui.pLayFriend->setVisible(false);
			m_ui.pLayAddFriend->setVisible(true);
			m_ui.pLayRequestFriend->setVisible(false);

			m_ui.pBtnFriend->setTouchEnabled(true);
			m_ui.pBtnFriend->setSelectedState(false);
			m_ui.pBtnAddFriend->setTouchEnabled(false);
			m_ui.pBtnRequestFriend->setTouchEnabled(true);
			m_ui.pBtnRequestFriend->setSelectedState(false);

			m_ui.pBtnAddBack->setVisible(false);
			m_ui.pBtnChange->setVisible(true);
			m_ui.inputName->setText("");
			m_ui.addimgTips->setVisible(false);
		}
		break;
	case Friend_type_request:
		{
			m_ui.pLayFriend->setVisible(false);
			m_ui.pLayAddFriend->setVisible(false);
			m_ui.pLayRequestFriend->setVisible(true);

			m_ui.pBtnFriend->setTouchEnabled(true);
			m_ui.pBtnFriend->setSelectedState(false);
			m_ui.pBtnAddFriend->setTouchEnabled(true);
			m_ui.pBtnAddFriend->setSelectedState(false);
			m_ui.pBtnRequestFriend->setTouchEnabled(false);

			if(FriendModel::getInstance()->getHaveNews())
			{
				m_ui.pBtnFriend->getChildByName("dian")->setVisible(true);
			}

			FriendModel::getInstance()->getHaveFriend() = false;
			m_ui.pBtnRequestFriend->getChildByName("dian")->setVisible(false);
		}
		break;
	default:
		break;
	}
}

void LayerFriend::onBtnNotify(Ref* sender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		if (sender == m_ui.pBtnBack) //返回
		{
			dispose();
		}
		else if (sender == m_ui.pBtnGive) //一键赠送
		{
			int sendPowerCount = 0;
			const FRIEND_INFOS_LIST& friendsInfo = FriendModel::getInstance()->getFriends();
			for (int i = 0; i < friendsInfo.size(); i++)
			{
				if ( friendsInfo[i].sendPower != 0)
				{
					sendPowerCount++;
				}
			}

			if (sendPowerCount > 0)
			{
				FriendController::getInstance()->onOneKeySendPower();
			}
			else
			{
				//CustomTips::show("没有体力可以赠送");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10143));
			}
		}
		else if (sender == m_ui.pBtnGet) //一键领取
		{
			int getPowerCount = 0;
			const FRIEND_INFOS_LIST& friendsInfo = FriendModel::getInstance()->getFriends();
			for (int i = 0; i < friendsInfo.size(); i++)
			{
				if ( friendsInfo[i].getPower != 0)
				{
					getPowerCount++;
				}
			}

			if (getPowerCount > 0)
			{
				FriendController::getInstance()->onOneKeyGetPower();
			}
			else
			{
				//CustomTips::show("没有体力可以领取");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10144));
			}
		}
		else if (sender == m_ui.pBtnAddBack) //添加返回
		{
			m_Friend_add_type = Friend_add_type_List;
			FriendModel::getInstance()->setFriendAddType(Friend_add_type_List);
			m_ui.pBtnAddBack->setVisible(false);
			m_ui.pBtnChange->setVisible(true);

			updateAddFriend();
		}
		else if (sender == m_ui.pBtnChange) //换一批
		{
			FriendController::getInstance()->onGetAddFriendList();
		}
		else if (sender == m_ui.pBtnSearch) //查找
		{
			string strKey = m_ui.inputName->getText();
			if (strKey.length() == 0)
			{
				//CustomTips::show("请输入好友名字");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10145));
				return;
			}
			FriendController::getInstance()->onSearchFriend(strKey);
		}
		else if (sender == m_ui.pBtnRefuse) //一键拒绝
		{
			if(FriendModel::getInstance()->getApplyfriends().size() <= 0 )
			{
				//CustomTips::show("暂时没有好友请求");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10146));
			}
			else
			{
				FriendController::getInstance()->onOneKeyApplyFriend(2);
				FriendModel::getInstance()->setFriendApplyType(Friend_Apply_type_one_key_refuse);
			}
		} 
		else if (sender == m_ui.pBtnAgree) //一键通过
		{
			if(FriendModel::getInstance()->getApplyfriends().size() <= 0 )
			{
				//CustomTips::show("暂时没有好友请求");
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10146));
			}
			else
			{
				FriendController::getInstance()->onOneKeyApplyFriend(1);
				FriendModel::getInstance()->setFriendApplyType(Friend_Apply_type_one_key_agree);
			}
		}
		else if (sender == m_ui.ButtonMessage) //发送消息
		{
			
		}
	}
}

void LayerFriend::onCheckNotify(Ref* sender, CheckBox::EventType type)
{
	if (type == CheckBox::EventType::SELECTED)
	{
		SoundUtils::playSoundEffect("dianji");
		if(sender == m_ui.pBtnFriend) //好友面板
		{
			m_Friend_type = Friend_type_player;
			swichFriendState();
			FriendController::getInstance()->onGetFriendsList();
		}
		else if (sender == m_ui.pBtnAddFriend) //添加好友
		{
			m_Friend_type = Friend_type_add;
			m_Friend_add_type = Friend_add_type_List;
			FriendModel::getInstance()->setFriendAddType(Friend_add_type_List);

			swichFriendState();
			FriendController::getInstance()->onGetAddFriendList();
		}
		else if (sender == m_ui.pBtnRequestFriend)//申请好友
		{
			m_Friend_type = Friend_type_request;
			swichFriendState();
			FriendController::getInstance()->onRequestFriendList();
		}
	}
}

void LayerFriend::updateMyFriend()
{
	m_ui.friendCount->setString(TO_STR(FriendModel::getInstance()->getGiftPower()) + "/" + 
		TO_STR(FriendModel::getInstance()->getMaxPower()));
	setTextAddStroke(m_ui.friendCount, Color3B(0x89, 0x0f, 0x0f), 2);

	m_ui.friendList->removeAllChildrenWithCleanup(true);
	const FRIEND_INFOS_LIST& friendsInfo = FriendModel::getInstance()->getFriends();
	for (int i = 0; i < friendsInfo.size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIFriend/Player.ExportJson");
		Layout* pBtnItem = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		Layout *Panel_icon = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_icon"));
		UIToolHeader *icon = UIToolHeader::create(friendsInfo[i].resId);
		icon->setScale(0.8f);
		icon->setLvl(friendsInfo[i].nLevel);
		icon->setAnchorPoint(Vec2(0,0));
		Panel_icon->addChild(icon);
		icon->setTouchEnabled(false);
		//if (friendsInfo[i].online == 0)
		//{
		//	icon->setMaskFlag(true,false);
		//}
		//else
		//{
		//	icon->setMaskFlag(false,false);
		//}

		Text *Label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_name"));
		Text *Label_number = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_number"));
		Label_name->setFontName(FONT_FZZHENGHEI);
		Label_number->setFontName(FONT_FZZHENGHEI);
		Label_name->setString(friendsInfo[i].strName);
		Label_number->setString(TO_STR(friendsInfo[i].nPower));
		setTextAddStroke(Label_name, Color3B(0x78, 0x03, 0x03), 2);
		setTextAddStroke(Label_number, Color3B(0x7d, 0x3f, 0x1c), 2);


		Layout *Panel_myfriend = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_myfriend"));
		Layout *Panel_add = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_add"));
		Layout *Panel_request = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_request"));
		Panel_myfriend->setVisible(true);
		Panel_add->setVisible(false);
		Panel_request->setVisible(false);

		Button *Button_get = dynamic_cast<Button*>(Helper::seekWidgetByName(Panel_myfriend,"Button_get"));
		Button *Button_give = dynamic_cast<Button*>(Helper::seekWidgetByName(Panel_myfriend,"Button_give"));

		if ( friendsInfo[i].getPower == 0)
		{
			Button_get->setBright(false);
		}
		else
		{
			Button_get->setBright(true);
		}

		if (friendsInfo[i].sendPower == 0)
		{
			Button_give->setBright(false);
		}
		else
		{
			Button_give->setBright(true);
		}

		//赠送体力
		auto givePower = [this,i](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (Widget::TouchEventType::ENDED == type)
			{
				if (FriendModel::getInstance()->getFriends()[i].sendPower == 0)
				{
					//CustomTips::show("一天只能赠送一次体力");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10147));
				}
				else
				{
					m_friendIndx = FriendModel::getInstance()->getFriends()[i].userId;
					FriendModel::getInstance()->setFriendId(FriendModel::getInstance()->getFriends()[i].userId);
					FriendController::getInstance()->onSendPower(FriendModel::getInstance()->getFriends()[i].userId);
				}
			}
		};
		Button_give->addTouchEventListener(givePower);

		//获取体力
		auto getPower = [this,i](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (Widget::TouchEventType::ENDED == type)
			{
				if ( FriendModel::getInstance()->getFriends()[i].getPower == 0)
				{
					//CustomTips::show("没有体力可以领取");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10144));
				}
				else
				{
					m_friendIndx = FriendModel::getInstance()->getFriends()[i].userId;
					FriendModel::getInstance()->setFriendId(FriendModel::getInstance()->getFriends()[i].userId);
					FriendController::getInstance()->onGetPower(FriendModel::getInstance()->getFriends()[i].userId);
				}
			}
		};
		Button_get->addTouchEventListener(getPower);

		auto pBtnItemCall = [this,friendsInfo,i](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (Widget::TouchEventType::ENDED == type)
			{
				m_ui.playChoose->setVisible(true);
				ActionCreator::runCommDlgAction(m_ui.addImgTips);

				Layout *Panel_icon = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.addImgTips,"Panel_icon"));
				UIToolHeader *icon = UIToolHeader::create(friendsInfo[i].resId);
				icon->setScale(0.8f);
				icon->setLvl(friendsInfo[i].nLevel);
				icon->setAnchorPoint(Vec2(0,0));
				if (Panel_icon->getChildByName("friendHeader_icon"))
				{
					Panel_icon->removeChildByName("friendHeader_icon");
				}
				Panel_icon->addChild(icon,0,"friendHeader_icon");
				icon->setTouchEnabled(false);

				Text *Label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.addImgTips,"Label_name"));
				Label_name->setFontName(FONT_FZZHENGHEI);
				Label_name->setString(friendsInfo[i].strName);
				setTextAddStroke(Label_name, Color3B(0x78, 0x03, 0x03), 2);
				Button *Button_delete = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.addImgTips,"Button_delete"));

				//删除好友
				auto deleteCall = [this,friendsInfo,i](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
				{
					if (type == Widget::TouchEventType::BEGAN)
					{
						SoundUtils::playSoundEffect("dianji");
					}
					else if (Widget::TouchEventType::ENDED == type)
					{
						m_friendIndx = friendsInfo[i].userId;
						FriendModel::getInstance()->setFriendId(friendsInfo[i].userId);

						CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10148), CC_CALLBACK_2(LayerFriend::onDeleteFriendCallback, this),CUSTOM_YES_NO);
					}
				};
				Button_delete->addTouchEventListener(deleteCall);
			}
		};
		Button *Button_di = dynamic_cast<Button*>(Helper::seekWidgetByName(pBtnItem,"Button_di"));
		Button_di->addTouchEventListener(pBtnItemCall);

		pBtnItem->setTag(friendsInfo[i].userId);
		m_ui.friendList->pushBackCustomItem(pBtnItem);
	}
	m_ui.friendList->refreshView();
	m_ui.friendList->jumpToTop();
}

void LayerFriend::onDeleteFriendCallback(Ref* sender, CustomRetType type)
{
	if (type == RET_TYPE_OK)
	{
		FriendController::getInstance()->onDeleteFriend(m_friendIndx);
	}
}

void LayerFriend::updataMyFriendListItemGive(int idx)
{
	m_ui.friendCount->setString(TO_STR(FriendModel::getInstance()->getGiftPower()) + "/" + 
		TO_STR(FriendModel::getInstance()->getMaxPower()));
	setTextAddStroke(m_ui.friendCount, Color3B(0x89, 0x0f, 0x0f), 2);

	Widget * item = (Widget*)m_ui.friendList->getChildByTag(idx);
	const FRIEND_INFOS_LIST& friendsInfo = FriendModel::getInstance()->getFriends();
	Button *Button_give = dynamic_cast<Button*>(Helper::seekWidgetByName(item,"Button_give"));

	FRIEND_INFOS friendsItem;
	for (auto iter = friendsInfo.begin(); iter != friendsInfo.end(); iter++)
	{
		if (idx == iter->userId)
		{
			friendsItem = *iter;
			break;
		}
	}
	if (friendsItem.sendPower == 0)
	{
		Button_give->setBright(false);
	}
	else
	{
		Button_give->setBright(true);
	}
}

void LayerFriend::updataMyFriendListItemGet(int idx)
{
	m_ui.friendCount->setString(TO_STR(FriendModel::getInstance()->getGiftPower()) + "/" + 
		TO_STR(FriendModel::getInstance()->getMaxPower()));
	setTextAddStroke(m_ui.friendCount, Color3B(0x89, 0x0f, 0x0f), 2);

	Widget * item = (Widget*)m_ui.friendList->getChildByTag(idx);
	const FRIEND_INFOS_LIST& friendsInfo = FriendModel::getInstance()->getFriends();

	Button *Button_get = dynamic_cast<Button*>(Helper::seekWidgetByName(item,"Button_get"));
	if(!Button_get)
	{
		return;
	}

	FRIEND_INFOS friendsItem;
	for (auto iter = friendsInfo.begin(); iter != friendsInfo.end(); iter++)
	{
		if (idx == iter->userId)
		{
			friendsItem = *iter;
			break;
		}
	}
	if (friendsItem.getPower == 0)
	{
		Button_get->setBright(false);
	}
	else
	{
		Button_get->setBright(true);
	}
}

void LayerFriend::updateAddFriend()
{
	m_ui.friendAddCount->setString(TO_STR(FriendModel::getInstance()->getFriends().size()) + "/" + 
		TO_STR(FriendModel::getInstance()->getMaxFriends()));
	setTextAddStroke(m_ui.friendAddCount, Color3B(0x89, 0x0f, 0x0f), 2);
	m_ui.addFriendList->removeAllChildrenWithCleanup(true);
	const ADD_FRIEND_INFO_LIST& friendsInfo = FriendModel::getInstance()->geAddFriendList();
	for (int i = 0; i < friendsInfo.size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIFriend/Player.ExportJson");
		Layout* pBtnItem = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		Layout *Panel_icon = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_icon"));
		UIToolHeader *icon = UIToolHeader::create(friendsInfo[i].resId);
		icon->setScale(0.8f);
		icon->setLvl(friendsInfo[i].nLevel);
		icon->setAnchorPoint(Vec2(0,0));
		Panel_icon->addChild(icon);
		icon->setTouchEnabled(false);

		Text *Label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_name"));
		Text *Label_number = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_number"));
		Label_name->setFontName(FONT_FZZHENGHEI);
		Label_number->setFontName(FONT_FZZHENGHEI);
		Label_name->setString(friendsInfo[i].strName);
		Label_number->setString(TO_STR(friendsInfo[i].nPower));
		setTextAddStroke(Label_name, Color3B(0x78, 0x03, 0x03), 2);
		setTextAddStroke(Label_number, Color3B(0x7d, 0x3f, 0x1c), 2);


		Layout *Panel_myfriend = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_myfriend"));
		Layout *Panel_add = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_add"));
		Layout *Panel_request = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_request"));
		Panel_myfriend->setVisible(false);
		Panel_add->setVisible(true);
		Panel_request->setVisible(false);

		Button *Button_add = dynamic_cast<Button*>(Helper::seekWidgetByName(Panel_add,"Button_add"));

		if (friendsInfo[i].added == 0)
		{
			Button_add->setBright(false);
		}
		else
		{
			Button_add->setBright(true);
		}

		//添加
		auto add = [this,friendsInfo,i](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (Widget::TouchEventType::ENDED == type)
			{
				if (friendsInfo[i].added != 0)
				{
					if (FriendModel::getInstance()->getFriends().size() >= FriendModel::getInstance()->getMaxFriends())
					{
						//CustomTips::show("您的好友数量已达上限");
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10149));
					}
					else
					{
						m_friendIndx = friendsInfo[i].userId;
						FriendModel::getInstance()->setFriendId(friendsInfo[i].userId);
						FriendController::getInstance()->onAddFriend(friendsInfo[i].userId);
					}
				}
			}
		};
		Button_add->addTouchEventListener(add);

		pBtnItem->setTag(friendsInfo[i].userId);
		m_ui.addFriendList->pushBackCustomItem(pBtnItem);
	}
	m_ui.addFriendList->refreshView();
	m_ui.addFriendList->jumpToTop();
}

void LayerFriend::updataAddFriendListItemGet(int idx)
{
	Widget * item = (Widget*)m_ui.addFriendList->getChildByTag(idx);
	Button *Button_add = dynamic_cast<Button*>(Helper::seekWidgetByName(item,"Button_add"));
	
	if (m_Friend_add_type == Friend_add_type_List)
	{
		const ADD_FRIEND_INFO_LIST& friendsInfo = FriendModel::getInstance()->geAddFriendList();
		ADD_FRIEND_INFOS friendsItem;
		for (auto iter = friendsInfo.begin(); iter != friendsInfo.end(); iter++)
		{
			if (idx == iter->userId)
			{
				friendsItem = *iter;
				break;
			}
		}
		if (friendsItem.added == 0)
		{
			Button_add->setBright(false);
		}
		else
		{
			Button_add->setBright(true);
		}
	}
	else if(m_Friend_add_type == Friend_add_type_search)
	{
		const ADD_FRIEND_INFO_LIST& friendsInfo = FriendModel::getInstance()->getSearchfriends();
		ADD_FRIEND_INFOS friendsItem;
		for (auto iter = friendsInfo.begin(); iter != friendsInfo.end(); iter++)
		{
			if (idx == iter->userId)
			{
				friendsItem = *iter;
				break;
			}
		}
		if (friendsItem.added == 0)
		{
			Button_add->setBright(false);
		}
		else
		{
			Button_add->setBright(true);
		}
	}
}

void LayerFriend::updateSearchFriend()
{
	m_ui.addFriendList->removeAllChildrenWithCleanup(true);
	const ADD_FRIEND_INFO_LIST& friendsInfo = FriendModel::getInstance()->getSearchfriends();
	for (int i = 0; i < friendsInfo.size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIFriend/Player.ExportJson");
		Layout* pBtnItem = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		Layout *Panel_icon = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_icon"));
		UIToolHeader *icon = UIToolHeader::create(friendsInfo[i].resId);
		icon->setScale(0.8f);
		icon->setLvl(friendsInfo[i].nLevel);
		icon->setAnchorPoint(Vec2(0,0));
		Panel_icon->addChild(icon);
		icon->setTouchEnabled(false);

		Text *Label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_name"));
		Text *Label_number = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_number"));
		Label_name->setFontName(FONT_FZZHENGHEI);
		Label_number->setFontName(FONT_FZZHENGHEI);
		Label_name->setString(friendsInfo[i].strName);
		Label_number->setString(TO_STR(friendsInfo[i].nPower));
		setTextAddStroke(Label_name, Color3B(0x78, 0x03, 0x03), 2);
		setTextAddStroke(Label_number, Color3B(0x7d, 0x3f, 0x1c), 2);


		Layout *Panel_myfriend = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_myfriend"));
		Layout *Panel_add = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_add"));
		Layout *Panel_request = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_request"));
		Panel_myfriend->setVisible(false);
		Panel_add->setVisible(true);
		Panel_request->setVisible(false);

		Button *Button_add = dynamic_cast<Button*>(Helper::seekWidgetByName(Panel_add,"Button_add"));

		if (friendsInfo[i].added == 0)
		{
			Button_add->setBright(false);
		}
		else
		{
			Button_add->setBright(true);
		}

		//添加
		auto add = [this,friendsInfo,i](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (Widget::TouchEventType::ENDED == type)
			{
				if (FriendModel::getInstance()->getFriends().size() >= FriendModel::getInstance()->getMaxFriends())
				{
					//CustomTips::show("您的好友数量已达上限");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10149));
				}
				else
				{
					m_friendIndx = friendsInfo[i].userId;
					FriendModel::getInstance()->setFriendId(friendsInfo[i].userId);
					FriendController::getInstance()->onAddFriend(friendsInfo[i].userId);
				}
			}
		};
		Button_add->addTouchEventListener(add);

		pBtnItem->setTag(friendsInfo[i].userId);
		m_ui.addFriendList->pushBackCustomItem(pBtnItem);
	}
	m_ui.addFriendList->refreshView();
	m_ui.addFriendList->jumpToTop();
}

void LayerFriend::updateApplyFriend()
{
	m_ui.requestfriendList->removeAllChildrenWithCleanup(true);
	const ADD_FRIEND_INFO_LIST& friendsInfo = FriendModel::getInstance()->getApplyfriends();
	for (int i = 0; i < friendsInfo.size(); i++)
	{
		string strFileName;
		strFileName.assign("Image/UIFriend/Player.ExportJson");
		Layout* pBtnItem = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));

		Layout *Panel_icon = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_icon"));
		UIToolHeader *icon = UIToolHeader::create(friendsInfo[i].resId);
		icon->setScale(0.8f);
		icon->setLvl(friendsInfo[i].nLevel);
		icon->setAnchorPoint(Vec2(0,0));
		Panel_icon->addChild(icon);
		icon->setTouchEnabled(false);

		Text *Label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_name"));
		Text *Label_number = dynamic_cast<Text*>(Helper::seekWidgetByName(pBtnItem,"Label_number"));
		Label_name->setFontName(FONT_FZZHENGHEI);
		Label_number->setFontName(FONT_FZZHENGHEI);
		Label_name->setString(friendsInfo[i].strName);
		Label_number->setString(TO_STR(friendsInfo[i].nPower));
		setTextAddStroke(Label_name, Color3B(0x78, 0x03, 0x03), 2);
		setTextAddStroke(Label_number, Color3B(0x7d, 0x3f, 0x1c), 2);


		Layout *Panel_myfriend = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_myfriend"));
		Layout *Panel_add = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_add"));
		Layout *Panel_request = dynamic_cast<Layout*>(Helper::seekWidgetByName(pBtnItem,"Panel_request"));
		Panel_myfriend->setVisible(false);
		Panel_add->setVisible(false);
		Panel_request->setVisible(true);

		Button *Button_refuse = dynamic_cast<Button*>(Helper::seekWidgetByName(Panel_request,"Button_refuse"));

		//拒绝
		auto refuse = [this,friendsInfo,i](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (Widget::TouchEventType::ENDED == type)
			{
				m_friendIndx = friendsInfo[i].userId;
				FriendModel::getInstance()->setFriendId(friendsInfo[i].userId);
				FriendModel::getInstance()->setFriendApplyType(Friend_Apply_type_refuse);
				FriendController::getInstance()->onApplyFriend(0,friendsInfo[i].userId);
			}
		};
		Button_refuse->addTouchEventListener(refuse);

		Button *Button_agree = dynamic_cast<Button*>(Helper::seekWidgetByName(Panel_request,"Button_agree"));

		//同意
		auto agree = [this,friendsInfo,i](Ref* sender, cocos2d::ui::Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (Widget::TouchEventType::ENDED == type)
			{
				if (FriendModel::getInstance()->getFriends().size() >= FriendModel::getInstance()->getMaxFriends())
				{
					//CustomTips::show("您的好友数量已达上限");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10149));
				}
				else
				{
					m_friendIndx = friendsInfo[i].userId;
					FriendModel::getInstance()->setFriendId(friendsInfo[i].userId);
					FriendModel::getInstance()->setFriendApplyType(Friend_Apply_type_agree);
					FriendController::getInstance()->onApplyFriend(1,friendsInfo[i].userId);
				}
			}
		};
		Button_agree->addTouchEventListener(agree);

		pBtnItem->setTag(friendsInfo[i].userId);
		m_ui.requestfriendList->pushBackCustomItem(pBtnItem);
	}

	m_ui.requestfriendList->refreshView();
	m_ui.requestfriendList->jumpToTop();
}

void LayerFriend::updateApplyFriendItem()
{
	m_ui.requestfriendList->removeChildByTag(m_friendIndx);
}
