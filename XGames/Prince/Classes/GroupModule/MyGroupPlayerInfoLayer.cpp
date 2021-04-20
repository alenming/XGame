#include "MyGroupPlayerInfoLayer.h"
#include "Temp/CustomPop.h"
#include "Communication/SocketCommunicator.h"
#include "MyGroupController.h"
#include "MyGroupModel.h"
#include "MainModule/MainCityLoadingScene.h"
#include "Common/Common.h"



MyGroupPlayerInfoLayer::MyGroupPlayerInfoLayer():
	_playerID(0)
{
	MyGroupModel::getInstance()->addObserver(this);
}

MyGroupPlayerInfoLayer::~MyGroupPlayerInfoLayer()
{
	MyGroupModel::getInstance()->removeObserver(this);
}

Layer* MyGroupPlayerInfoLayer::create(int playerID)
{ 
	auto pRet = new MyGroupPlayerInfoLayer(); 
	if (pRet && pRet->init(playerID)) 
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

bool MyGroupPlayerInfoLayer::init(int playerID)
{
	if(!ModalLayer::init())
	{
		return false;
	}
	_playerID = playerID;
	//获取玩家信息
	_playerInfo = MyGroupModel::getInstance()->getPlayerInfoByUid(playerID);

	initUI();

	return true;
}

void MyGroupPlayerInfoLayer::onEnter()
{
	ModalLayer::onEnter();
}

void MyGroupPlayerInfoLayer::onExit()
{
	ModalLayer::onExit();
}

void MyGroupPlayerInfoLayer::initUI()
{
	// 加载 UI;
	string strFileName = "Image/UIGang/UIGang_information.ExportJson";
	// Root;
	m_ui.uiRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.uiRoot->setName("UI_ROOT");
	this->addChild(m_ui.uiRoot);

	initTextUI();
	initToolUI();
	intAutyUI();

}
void MyGroupPlayerInfoLayer::initTextUI()
{
	m_ui.uiTexFightPower = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_Power"));
	m_ui.uiTexFightPower->setString(TO_STR(_playerInfo->fightPower));
	m_ui.uiTexFightPower->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexFightPower, Color3B(0x7d, 0x3f, 0x1c), 2);

	m_ui.uiTexContribution = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_Contribution"));
	m_ui.uiTexContribution->setString(TO_STR(_playerInfo->contributeVal));
	m_ui.uiTexContribution->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexContribution, Color3B(0x7d, 0x3f, 0x1c),2);

	m_ui.uiTexDuty = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_Job"));
	setPowerType();
	
	m_ui.uiTexName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_PlayerName"));
	m_ui.uiTexName->setString(_playerInfo->name);
	m_ui.uiTexName->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexContribution, Color3B(0x7d, 0x3f, 0x1c),2);

	m_ui.uiImaVip = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiRoot,"Img_VIP"));
	m_ui.uiImaVip->loadTexture("Image/Icon/vip/btn_VIP" + TO_STR(_playerInfo->vipLevel) + ".png",TextureResType::LOCAL);

	m_ui.uiTexOnlineTime = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_Login"));
	if (_playerInfo->isOnLine)
	{
		m_ui.uiTexOnlineTime->setString(MyGroupModel::getInstance()->getLastLoginTimeDes(_playerInfo->uId));
		m_ui.uiTexOnlineTime->setFontName(FONT_FZZHENGHEI);
		m_ui.uiTexOnlineTime->setColor(Color3B(0x8b, 0xe7, 0x40));
	}
	else 
	{
		m_ui.uiTexOnlineTime->setString(MyGroupModel::getInstance()->getLastLoginTimeDes(_playerInfo->uId));
		m_ui.uiTexOnlineTime->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(m_ui.uiTexOnlineTime, Color3B(0x7d, 0x3f, 0x1c), 2);
	}
}
void MyGroupPlayerInfoLayer::setPowerType()
{

	std::string DutyName = MyGroupModel::getInstance()->getPostPowerName(_playerInfo->power);
	m_ui.uiTexDuty->setString(DutyName);
	m_ui.uiTexDuty->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexDuty, Color3B(0x7d, 0x3f, 0x1c), 2);
}

void MyGroupPlayerInfoLayer::initToolUI()
{
	//玩家个人头像
	Layout *imgHero = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Pnl_HeadIcon"));
	UIToolHeader *icon = UIToolHeader::create(_playerInfo->iconResId);
	//icon->setScale(0.8f);
	icon->setLvl(_playerInfo->level);
	icon->setAnchorPoint(Vec2(0,0));
	imgHero->addChild(icon);
	icon->setTouchEnabled(true);
	
	int mHeroCount = 0;
	//玩家阵形
	for (int i=0; i<_playerInfo->matrix.size(); i++)
	{
		if (_playerInfo->matrix[i].id < 0)
		{
			continue;
		}	
		else
		{		
			//对应的容器
			Layout *imgLine = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot, "Hero_Panel_" + TO_STR(mHeroCount)));
			mHeroCount++;
			int starsId = _playerInfo->matrix[i].id/100 * 100 + _playerInfo->matrix[i].stars;
			RowData* StarsData = DataManager::getInstance()->searchStarUpById(starsId);
			if(StarsData)
			{
				UIToolHeader *icon = UIToolHeader::create(ResourceUtils::getSmallIconPath(_playerInfo->matrix[i].id/100));
				icon->setScale(0.88f);
				icon->setStarsNum(_playerInfo->matrix[i].stars,STAR_CONTAIN_BOTTOM);
				icon->setQuaCornerIcon(_playerInfo->matrix[i].id);
				icon->setAnchorPoint(Vec2(0,0));
				imgLine->addChild(icon);
			}
		}
	}
	//补全空位
	for (int i = 0; i < 6; i++)
	{
		if (i>=mHeroCount)
		{
			Layout *imgLine = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot, "Hero_Panel_" + TO_STR(i)));
			UIToolHeader *icon = UIToolHeader::create();
			icon->setScale(0.88f);
			icon->setAnchorPoint(Vec2(0,0));
			imgLine->addChild(icon);
		}
	}
}

void MyGroupPlayerInfoLayer::intAutyUI()
{
	auto _myInfo = MyGroupModel::getInstance()->getMyInfo();//里面的玩家自己的数据
	m_ui.uiBtnLeftUP = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Promote"));
	m_ui.uiBtnLeftDown = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Reduce"));
	m_ui.uiBtnmiddle = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Out"));
	m_ui.uiBtnSure = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Form"));
	m_ui.uiBtnRight = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_zhuanrang"));

	m_ui.uiExit = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Close"));
	m_ui.uiExit->addTouchEventListener(CC_CALLBACK_2(MyGroupPlayerInfoLayer::onBtnClick,this,GROUP_INFOLAYER_CALLBACK::BTN_INFO_EXIT));

	switch (_myInfo->info.power)
	{
		case ePostPower::ePOWERLEADER:
			{

				if (_playerInfo->power ==  ePostPower::ePOWERLEADER)
				{
					m_ui.uiBtnSure->setVisible(true);
					m_ui.uiBtnLeftDown->setVisible(false);
					m_ui.uiBtnLeftUP->setVisible(false);
					m_ui.uiBtnmiddle->setVisible(false);
					m_ui.uiBtnRight->setVisible(false);
				}
				else if (_playerInfo->power ==  ePostPower::ePOWERNOPOWER)
				{
					m_ui.uiBtnSure->setVisible(false);
					m_ui.uiBtnLeftDown->setVisible(false);
					m_ui.uiBtnLeftUP->setVisible(true);
					m_ui.uiBtnmiddle->setVisible(true);
					m_ui.uiBtnRight->setVisible(true);
				}
				else if (_playerInfo->power ==  ePostPower::ePOWERVICE)
				{
					m_ui.uiBtnSure->setVisible(false);
					m_ui.uiBtnLeftDown->setVisible(true);
					m_ui.uiBtnLeftUP->setVisible(false);
					m_ui.uiBtnmiddle->setVisible(true);
					m_ui.uiBtnRight->setVisible(true);
				}
				m_ui.uiBtnLeftDown->addTouchEventListener(CC_CALLBACK_2(MyGroupPlayerInfoLayer::onBtnClick,this,GROUP_INFOLAYER_CALLBACK::BTN_LOWER_DUTY));
				m_ui.uiBtnLeftUP->addTouchEventListener(CC_CALLBACK_2(MyGroupPlayerInfoLayer::onBtnClick,this,GROUP_INFOLAYER_CALLBACK::BTN_UP_DUTY));
				m_ui.uiBtnmiddle->addTouchEventListener(CC_CALLBACK_2(MyGroupPlayerInfoLayer::onBtnClick,this,GROUP_INFOLAYER_CALLBACK::BTN_EXPEL_GROUP));
				m_ui.uiBtnRight->addTouchEventListener(CC_CALLBACK_2(MyGroupPlayerInfoLayer::onBtnClick,this,GROUP_INFOLAYER_CALLBACK::BTN_TRANSFER));
				m_ui.uiBtnSure->addTouchEventListener(CC_CALLBACK_2(MyGroupPlayerInfoLayer::onBtnClick,this,GROUP_INFOLAYER_CALLBACK::BTN_AFFIRM));
			}
			break;
		case ePostPower::ePOWERVICE:
			{
				if (_playerInfo->power ==  ePostPower::ePOWERNOPOWER)
				{
					m_ui.uiBtnLeftDown->setVisible(false);
					m_ui.uiBtnLeftUP->setVisible(false);
					m_ui.uiBtnSure->setVisible(false);
					m_ui.uiBtnmiddle->setVisible(true);
					m_ui.uiBtnmiddle->addTouchEventListener(CC_CALLBACK_2(MyGroupPlayerInfoLayer::onBtnClick,this,GROUP_INFOLAYER_CALLBACK::BTN_EXPEL_GROUP));
					m_ui.uiBtnRight->setVisible(false);
				}
				else 
				{
					m_ui.uiBtnLeftDown->setVisible(false);
					m_ui.uiBtnLeftUP->setVisible(false);
					m_ui.uiBtnmiddle->setVisible(false);
					m_ui.uiBtnSure->setVisible(true);
					m_ui.uiBtnSure->addTouchEventListener(CC_CALLBACK_2(MyGroupPlayerInfoLayer::onBtnClick,this,GROUP_INFOLAYER_CALLBACK::BTN_AFFIRM));
					m_ui.uiBtnRight->setVisible(false);
				}

			}
			break;
		case ePostPower::ePOWERNOPOWER:
			{
				m_ui.uiBtnLeftDown->setVisible(false);
				m_ui.uiBtnLeftUP->setVisible(false);
				m_ui.uiBtnmiddle->setVisible(false);
				m_ui.uiBtnSure->setVisible(true);
				m_ui.uiBtnSure->addTouchEventListener(CC_CALLBACK_2(MyGroupPlayerInfoLayer::onBtnClick,this,GROUP_INFOLAYER_CALLBACK::BTN_AFFIRM));
				m_ui.uiBtnRight->setVisible(false);
			}
			break;
		default:
			break;
	}
}

void  MyGroupPlayerInfoLayer::onBtnClick( Ref* reff, Widget::TouchEventType type, GROUP_INFOLAYER_CALLBACK _callBackType )
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		switch (_callBackType)
		{
		case BTN_INFO_EXIT:
		case BTN_AFFIRM:
			{
				//if (!_isChaozhou)
				//{
				//	MyGroupController::getInstance()->sendGetGroupMainMsg();
				//}
				this->dispose();
			}
			break;
		case BTN_LOWER_DUTY:
			{
				//降低	
				//_isChaozhou = false;
				MyGroupController::getInstance()->sendAppointMsg(_playerInfo->uId);
			}	
			break;
		case BTN_UP_DUTY:
			{
				if (MyGroupModel::getInstance()->getMyGroupInfo()->curViceNum<2)
				{
					//提升
					//_isChaozhou = false;
					MyGroupController::getInstance()->sendAppointMsg(_playerInfo->uId);
				}
				else if(MyGroupModel::getInstance()->getMyGroupInfo()->curViceNum>=2)
				{
					//有两位副帮主
					//CustomTips::show("帮派只能有2个副帮主");
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10206));
				}
			}
				  break;
		case BTN_EXPEL_GROUP:
			{
				CustomPopCallback cpCallBack = [=](Ref* rre, CustomRetType type)->void
				{
					if (type ==  CustomRetType::RET_TYPE_OK)
					{
						//逐出帮派
						//_isChaozhou = false;
						MyGroupController::getInstance()->sendExpelMsg(_playerInfo->uId);
					}
				};
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10207),cpCallBack, CustomPopType::CUSTOM_YES_NO);
			}
			break;
		case BTN_TRANSFER:
			{
				CustomPopCallback cpCallBack = [=](Ref* rre, CustomRetType type)->void
				{
					if (type ==  CustomRetType::RET_TYPE_OK)
					{
						//转帮派
						//_isChaozhou = false;
						MyGroupController::getInstance()->sendAbdicateMsg(_playerInfo->uId);
					}
					else if (type ==  CustomRetType::RET_TYPE_CANCEL)
					{

					}
				};
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10208),cpCallBack, CustomPopType::CUSTOM_YES_NO);
			}
			break;
		default:
			break;
		}
	}
}

void MyGroupPlayerInfoLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
		case nMAIN_CMD_GROUP_ABDICATE:
			{
				this->dispose();
				auto call = [this](Ref* ref, CustomRetType type)->void
				{
					MyGroupController::getInstance()->sendGetGroupMainMsg();
				};
				CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10209), call, CUSTOM_ID_YES);
			}
			break;
		case nMAIN_CMD_GROUP_APPOINT:
			{
				// updateUserAuty();
				 this->dispose();
				 auto call = [this](Ref* ref, CustomRetType type)->void
				 {
					 MyGroupController::getInstance()->sendGetGroupMainMsg();
				 };

				 if (_playerInfo->power == ePostPower::ePOWERNOPOWER)
				 {
					  CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10210), call, CUSTOM_ID_YES);
				 }
				 else if (_playerInfo->power == ePostPower::ePOWERVICE)
				 {
					 CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10211), call, CUSTOM_ID_YES);
				 }
				
			};
			break;
		case nMAIN_CMD_GROUP_EXPEL:
			{
				CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10212));
				MyGroupController::getInstance()->sendGetGroupMainMsg();
				this->dispose();
			}
			break;
			break;
		default:
			break;
	}
}

void MyGroupPlayerInfoLayer::updateUserAuty()
{
	_playerInfo = MyGroupModel::getInstance()->getPlayerInfoByUid(_playerID);
	//修改职位
	m_ui.uiTexDuty->setString(MyGroupModel::getInstance()->getPostPowerName(_playerInfo->power));
	m_ui.uiTexDuty->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexDuty, Color3B(0x7d, 0x3f, 0x1c),2);

	//修改按钮
	if (_playerInfo->power == ePostPower::ePOWERNOPOWER)
	{	
		//修改按钮
		m_ui.uiBtnLeftDown->setVisible(false);
		m_ui.uiBtnLeftUP->setVisible(true);
	}

	if (_playerInfo->power == ePostPower::ePOWERVICE)
	{	
		//修改按钮
		m_ui.uiBtnLeftDown->setVisible(true);
		m_ui.uiBtnLeftUP->setVisible(false);
	}
}
