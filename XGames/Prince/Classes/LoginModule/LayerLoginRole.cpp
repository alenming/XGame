#include "LayerLoginRole.h"
#include "LoginController.h"
#include "LoginModel.h"
#include "Communication/SocketCommunicator.h"
#include "Temp/CustomTips.h"
#include "Widget/LayerGameBoard.h"
#include "Widget/ActionCreator.h"
#include "SceneLogin.h"
#include "DataManager/DataManager.h"

LayerLoginRole::LayerLoginRole(void)
{
	LoginModel::getInstance()->addObserver(this);
	m_vcZone.clear();
	m_serverParam.clear();
	m_strWeriHu = DataManager::getInstance()->searchCommonTexdtById(10280);
	m_strBaoMan = DataManager::getInstance()->searchCommonTexdtById(10281);
	m_strXinQu = DataManager::getInstance()->searchCommonTexdtById(10282);
}


LayerLoginRole::~LayerLoginRole(void)
{
	LoginModel::getInstance()->removeObserver(this);
}

bool LayerLoginRole::init()
{
	if (!Layer::init())
	{
		return false;
	}

	initUI();

	return true;
}

void LayerLoginRole::onEnter()
{
	Layer::onEnter();

	// 读本地配置存储的上次登陆服务器信息;

	// 下一帧执行;
	auto callBack = [this](float dt)->void
	{
		this->getScheduler()->unschedule("LayerLoginRole::onEnter", this);
		
		// 拉取服务器列表;
		LoginController::getInstance()->getServerZoneList();

		// 向服务器发送设备唯一标识;
		LoginController::getInstance()->sendDeviceId();
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerLoginRole::onEnter");
}

void LayerLoginRole::onExit()
{
	Layer::onExit();
}


void LayerLoginRole::initUI()
{
	Size size = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 pos = Director::getInstance()->getVisibleOrigin();

	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UILoginnew/UILoginnew_dengluyouxi.ExportJson"));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_login.jpg");
	this->addChild(m_ui.pRoot);

	// 登陆分区;
	m_ui.pnlLoginRole = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Pnl_LoginRole"));
	m_ui.pnlLoginRole->setContentSize(visibleSize);
	m_ui.btnEnter = dynamic_cast<Button*>(m_ui.pnlLoginRole->getChildByName(STRING(Btn_Enter)));
	m_ui.btnEnter->addTouchEventListener(CC_CALLBACK_2(LayerLoginRole::onBtnClicked, this, Btn_Enter));
	m_ui.btnZone = dynamic_cast<Button*>(m_ui.pnlLoginRole->getChildByName(STRING(Btn_Zone)));
	m_ui.btnZone->addTouchEventListener(CC_CALLBACK_2(LayerLoginRole::onBtnClicked, this, Btn_Zone));
	m_ui.txtZoneName = dynamic_cast<Text*>(m_ui.btnZone->getChildByName("Lab_ZoneName"));
	m_ui.txtZoneName->setFontName(FONT_FZZHENGHEI);
	m_ui.txtZoneName->setString("");
	m_ui.txtZoneState = dynamic_cast<Text*>(m_ui.btnZone->getChildByName("Lab_ZoneState"));
	m_ui.txtZoneState->setFontName(FONT_FZZHENGHEI);
	m_ui.txtZoneState->setString("");

	// 分区列表;
	m_ui.uiZone.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UILoginnew/UILoginnew_fenfu.ExportJson"));
	this->addChild(m_ui.uiZone.pRoot);
	m_ui.uiZone.pRoot->setTouchEnabled(true);
	m_ui.uiZone.pRoot->addTouchEventListener([=](Ref* ref, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::ENDED)
		{
			m_ui.uiZone.pRoot->setVisible(false);
		}
	});

	m_ui.uiZone.pRoot->setVisible(false);
	m_ui.uiZone.scrollviewServerIndex = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "ScrollView_ServerIndex"));
	m_ui.uiZone.scrollviewServerIndex->addEventListener(CC_CALLBACK_2(LayerLoginRole::onScrollViewEvent, this));
	m_ui.uiZone.imgIndexSelected = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Img_Index_Selected"));
	m_ui.uiZone.txtIndexItem = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Lab_Index_Item"));
	m_ui.uiZone.txtIndexItem->setFontName(FONT_FZZHENGHEI);
	m_ui.uiZone.txtIndexItem->setVisible(false);
	m_ui.uiZone.txtDefaultZoneName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Lab_Default_Zone_Name"));
	m_ui.uiZone.txtDefaultZoneName->setFontName(FONT_FZZHENGHEI);
	m_ui.uiZone.txtDefaultZoneName->setString("");
	m_ui.uiZone.txtDefaultZoneState = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Lab_Default_Zone_State"));
	m_ui.uiZone.txtDefaultZoneState->setFontName(FONT_FZZHENGHEI);
	m_ui.uiZone.txtDefaultZoneState->setString("");
	m_ui.uiZone.txtZoneTitle = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Lab_Title"));
	m_ui.uiZone.txtZoneTitle->setFontName(FONT_FZZHENGHEI);
	m_ui.uiZone.txtZoneTitle->setString("");
	m_ui.uiZone.pnlZoneList = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Pnl_Zone_List"));
	m_ui.uiZone.imgZoneListBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Image_fuwuqiliebiao"));
	m_ui.uiZone.imgZoneListBg->setVisible(true);

	// 滚动条;
	m_ui.uiZone.scrollbar = CustomScrollBar::create();
	dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Image_quanfu"))->addChild(m_ui.uiZone.scrollbar, 3);
	m_ui.uiZone.scrollbar->setPosition(Vec2(25.0f, 30.0f));
	m_ui.uiZone.scrollbar->initScrollBar(m_ui.uiZone.scrollviewServerIndex->getContentSize().height);

	// 分区节点(用来复制);
	m_ui.uiZone.pnlItem = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Pnl_ServerZone_Item"));
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pnlItem, "Lab_Zone_Name"))->setFontName(FONT_FZZHENGHEI);
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pnlItem, "Lab_Zone_Name"))->setString("");
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pnlItem, "Lab_Zone_Order"))->setFontName(FONT_FZZHENGHEI);
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pnlItem, "Lab_Zone_Order"))->setString("");
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pnlItem, "Lab_Zone_State"))->setFontName(FONT_FZZHENGHEI);
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pnlItem, "Lab_Zone_State"))->setString("");

	// 历史角色;
	m_ui.uiZone.txtRoles = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Lab_Roles"));
	m_ui.uiZone.txtRoles->setFontName(FONT_FZZHENGHEI);
	m_ui.uiZone.txtRoles->setTouchEnabled(true);
	m_ui.uiZone.txtRoles->addTouchEventListener([=](Ref* ref, Widget::TouchEventType type){
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			m_ui.uiZone.imgRolesSelected->setVisible(true);
			m_ui.uiZone.imgIndexSelected->setVisible(false);
			m_ui.uiZone.imgRolesBg->setVisible(true);
			m_ui.uiZone.imgZoneListBg->setVisible(false);
		}
	});
	m_ui.uiZone.imgRolesBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Image_lishijuese"));
	m_ui.uiZone.imgRolesBg->setVisible(false);
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Label_lishijuese"))->setFontName(FONT_FZZHENGHEI);
	m_ui.uiZone.imgRolesSelected = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiZone.pRoot, "Img_Roles_Selected"));
	m_ui.uiZone.imgRolesSelected->setVisible(false);

	// 注销;
	m_ui.btnLogout = dynamic_cast<Button*>(m_ui.pnlLoginRole->getChildByName(STRING(Btn_Logout)));
	m_ui.btnLogout->addTouchEventListener(CC_CALLBACK_2(LayerLoginRole::onBtnClicked, this, Btn_Logout));

	// 公告;
	m_ui.btnBoard = dynamic_cast<Button*>(m_ui.pnlLoginRole->getChildByName(STRING(Btn_Board)));
	m_ui.btnBoard->addTouchEventListener(CC_CALLBACK_2(LayerLoginRole::onBtnClicked, this, Btn_Board));

	// 版本号;
	m_ui.txtVersion = dynamic_cast<Text*>(m_ui.pnlLoginRole->getChildByName("Lab_Version"));
	m_ui.txtVersion->setFontName(FONT_FZZHENGHEI);
	//string strVersion("当前版本：");
	string strVersion(DataManager::getInstance()->searchCommonTexdtById(10275) + ": ");
	strVersion.append(GetStringFromXML(LOCAL_KEY_STR_CURRENT_VERSION, ""));
	m_ui.txtVersion->setString(strVersion);
}

void LayerLoginRole::onBtnClicked( Ref* sender, Widget::TouchEventType type, int nWidgetName )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (nWidgetName)
		{
		case Btn_Enter:
			{
				if (m_serverParam.empty() || m_serverParam.nServerState == SERVER_STATE::SERVER_STATE_CLOSED)
				{
					//CustomPop::show("服务器尚未开启，请关注游戏公告");
					CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10276));
				}
				else
				{
					// 保存服务器信息;
					SetStringToXML(LOCAL_KEY_STR_DEFAULT_SERVER_ID, m_serverParam.m_strServerID);
					LoginModel::getInstance()->setServerParam(m_serverParam);

					// 建立通信;
					//LoginController::getInstance()->loginRole();
					LayerWaiting::showWaiting();
					SocketCommunicator::getInstance()->startSocket();
				}
			}
			break;

		case Btn_Logout:
			{
				Director::getInstance()->replaceScene(SceneLogin::create());
				LoginController::getInstance()->logoutAccount();
			}
			break;

		case Btn_Board:
			{
				showBoard();
			}
			break;
		case Btn_Zone:
			{
				m_ui.uiZone.pRoot->setVisible(true);
				ActionCreator::runCommDlgAction(m_ui.uiZone.pRoot->getChildByName("Panel_cankao"));
				SoundUtils::playSoundEffect("quanping");
			}
			break;

		default:
			break;
		}
	}
}

void LayerLoginRole::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_SERVER_ZONE_LIST:
		{
			int nMaxCount = *((int*)(param->updateData));

			// 1. 更新服务器索引列表;
			updateServerZoneIndex(nMaxCount);

			// 2. 更新服务器列表(默认选中最新的区域);
			int nTotalIndexCount = nMaxCount / SERVER_ZONE_DEFAULT_COUNT;
			nTotalIndexCount += (nMaxCount%SERVER_ZONE_DEFAULT_COUNT > 0 ? 1 : 0);
			updateServerZoneList(nTotalIndexCount);

			// 3. 更新选中服务器;
			updateDefaultServer();

			// 4.展示公告;
			showBoard();
		}
		break;

	default:
		break;
	}
}

void LayerLoginRole::updateServerZoneIndex( int nTotalCount )
{
	if (nTotalCount <= 0)
		return;

	// 索引标签个数;
	int nTotalIndexCount = nTotalCount / SERVER_ZONE_DEFAULT_COUNT;
	nTotalIndexCount += (nTotalCount%SERVER_ZONE_DEFAULT_COUNT > 0 ? 1 : 0);

	// 生成标签;
	char szIndex1[32] = {0};
	char szIndex2[32] = {0};
	float innerHeight = nTotalIndexCount*60.0f > 370.0f ? nTotalIndexCount*60.0f : 370.0f;
	m_ui.uiZone.scrollviewServerIndex->setInnerContainerSize(Size(220.0f, innerHeight));
	m_ui.uiZone.scrollbar->setVisible(nTotalIndexCount*60.0f > 370.0f);
	int nLogicIndex = 0;
	//区
	string zone = DataManager::getInstance()->searchCommonTexdtById(10277);
	for (int i = nTotalIndexCount - 1; i >= 0; --i, ++nLogicIndex)
	{
		memset(szIndex1, 0, 32);
		memset(szIndex2, 0, 32);
		//sprintf(szIndex, "%03d区-%03d区", i*SERVER_ZONE_DEFAULT_COUNT+1, (i+1)*SERVER_ZONE_DEFAULT_COUNT);
		sprintf(szIndex1, "%03d", i*SERVER_ZONE_DEFAULT_COUNT+1);
		sprintf(szIndex2, "%03d", (i+1)*SERVER_ZONE_DEFAULT_COUNT);
		Text* txtIndex = dynamic_cast<Text*>(m_ui.uiZone.txtIndexItem->clone());
		txtIndex->setVisible(true);
		txtIndex->setString(szIndex1 + zone + "-" + szIndex2 + zone);
		txtIndex->setPosition(Vec2(110.0f, innerHeight - 30.0f - nLogicIndex * 60.0f));
		txtIndex->setTouchEnabled(true);
		txtIndex->addTouchEventListener(CC_CALLBACK_2(LayerLoginRole::onZoneIndexSelected, this, i+1));
		ImageView* imgLine = dynamic_cast<ImageView*>(dynamic_cast<ImageView*>(m_ui.uiZone.pnlItem->getChildByName("Image_xian1"))->clone());
		imgLine->setVisible(true);
		imgLine->setPosition(Point(0.0f, -30.0f) + txtIndex->getPosition());
		m_ui.uiZone.scrollviewServerIndex->addChild(txtIndex, 1);
		m_ui.uiZone.scrollviewServerIndex->addChild(imgLine, 1);
	}

	// 选中标记;
	m_ui.uiZone.imgIndexSelected->setVisible(true);
	m_ui.uiZone.imgIndexSelected->setPosition(Vec2(110.0f, innerHeight - 30.0f));

	// 滚动条;
	m_ui.uiZone.scrollbar->setScrollBarLengthPercent(m_ui.uiZone.scrollviewServerIndex->getContentSize().height / m_ui.uiZone.scrollviewServerIndex->getInnerContainerSize().height);
	m_ui.uiZone.scrollbar->setScrollBarPercentY(0.0f);
}

void LayerLoginRole::updateServerZoneList( int nIndex )
{
	m_vcZone.clear();
	LoginModel::getInstance()->getZoneList(nIndex, m_vcZone, SERVER_ZONE_DEFAULT_COUNT);
	if (m_vcZone.empty())
		return;

	// 清除所有分区;
	m_ui.uiZone.pnlZoneList->removeAllChildren();

	// 上次登陆;
	ServerParam* defaultServer = LoginModel::getInstance()->getDefaultServer();
	char szName[32] = {0};
	//区
	string zone = DataManager::getInstance()->searchCommonTexdtById(10277);
	if (nullptr != defaultServer)
	{
		memset(szName, 0, 32);
		sprintf(szName, "%03d", defaultServer->m_nZoneIndex);
		m_ui.uiZone.txtDefaultZoneName->setString(szName + zone + " " + STR_UTF8(defaultServer->m_strServerName));
		string strState;
		Color3B color;
		getUIServerState(defaultServer, strState, color);
		m_ui.uiZone.txtDefaultZoneState->setString(strState);
		m_ui.uiZone.txtDefaultZoneState->setColor(color);
	}

	// 标题;
	memset(szName, 0, 32);
	//服务器
	string serverCh = DataManager::getInstance()->searchCommonTexdtById(10278);
	sprintf(szName, "%03d--%03d", (nIndex-1)*SERVER_ZONE_DEFAULT_COUNT+1, nIndex*SERVER_ZONE_DEFAULT_COUNT);
	m_ui.uiZone.txtZoneTitle->setString(serverCh + szName);

	// 分区列表;
	char szOrder[8] = {0};
	for (int i = 0, k = 0; i < m_vcZone.size() && k < SERVER_ZONE_DEFAULT_COUNT; ++i, ++k)
	{
		Layout* item = dynamic_cast<Layout*>(m_ui.uiZone.pnlItem->clone());
		if (nullptr != item)
		{
			// 名称;
			memset(szOrder, 0, 8);
			memset(szName, 0, 32);
			sprintf(szOrder, "%03d", m_vcZone.at(i).m_nZoneIndex);
			//sprintf(szName, "区 %s", m_vcZone.at(i).m_strServerName.c_str());
			dynamic_cast<Text*>(item->getChildByName("Lab_Zone_Order"))->setString(STR_UTF8(szOrder));
			dynamic_cast<Text*>(item->getChildByName("Lab_Zone_Name"))->setString(zone + " " + STR_UTF8(m_vcZone.at(i).m_strServerName));

			// 状态;
			string strState;
			Color3B color;
			getUIServerState(&(m_vcZone.at(i)), strState, color);
			dynamic_cast<Text*>(item->getChildByName("Lab_Zone_State"))->setString(strState);
			dynamic_cast<Text*>(item->getChildByName("Lab_Zone_State"))->setColor(color);

			item->setPosition(Vec2((i%2)*335.0f, (4 - (i/2))*(380.0f/5.0f)));
			m_ui.uiZone.pnlZoneList->addChild(item);
			item->setName(m_vcZone.at(i).m_strServerID);
			item->setVisible(true);
			item->addTouchEventListener(CC_CALLBACK_2(LayerLoginRole::onZoneClicked, this, m_vcZone.at(i).m_strServerID));
		}
	}
}

void LayerLoginRole::updateDefaultServer()
{
	ServerParam* defaultServer = LoginModel::getInstance()->getDefaultServer();
	updateDefaultServer(defaultServer);
}

void LayerLoginRole::updateDefaultServer( ServerParam* defaultServer )
{
	if (nullptr == defaultServer)
	{
		//m_ui.txtZoneName->setString(DataManager::getInstance()->searchCommonTexdtById(10279));
		m_ui.txtZoneState->setString(STR_UTF8(" "));
	}
	else
	{
		char szServer[32] = {0};
		string zone = DataManager::getInstance()->searchCommonTexdtById(10277);
		sprintf(szServer, "%03d", defaultServer->m_nZoneIndex);
		m_ui.txtZoneName->setString(szServer + zone + "  " + STR_UTF8(defaultServer->m_strServerName));

		string strState;
		Color3B color;
		getUIServerState(defaultServer, strState, color);
		m_ui.txtZoneState->setString(strState);
		m_ui.txtZoneState->setColor(color);
	}

	// 当前选中;
	selectServer(defaultServer);
}

void LayerLoginRole::getUIServerState( ServerParam* param, string& strState, Color3B& color )
{
	strState.clear();

	switch (param->nServerState)
	{
	case SERVER_STATE_CLOSED:
		{
			//strState.assign("维护");
			strState = m_strWeriHu;
			color = Color3B::WHITE;
		}
		break;
	case SERVER_STATE_BUSY:
		{
			//strState.assign("爆满");
			strState = m_strBaoMan;
			color = Color3B(0xFF, 0x30, 0x0E);
		}
		break;
	case SERVER_STATE_NEW:
		{
			//strState.assign("新区");
			strState = m_strXinQu;
			color = Color3B(0x4F, 0xE8, 0x53);
		}
		break;
	default:
		{
			strState.assign(" ");
			color = Color3B::WHITE;
		}
		break;
	}
}

void LayerLoginRole::selectServer( ServerParam* param )
{
	if (nullptr == param)
		return;

	// 保存当前选中;
	m_serverParam = *param;
}

void LayerLoginRole::showBoard()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	{
		// 下一帧执行;
		auto callBack = [this](float dt)->void
		{
			this->getScheduler()->unschedule("LayerLoginRole::showBoard", this);

			// 创建公告板;
			if (this->getChildByName("LayerGameBoard"))
			{
				this->removeChildByName("LayerGameBoard");
			}

			LayerGameBoard* _layer = LayerGameBoard::create();
			this->addChild(_layer, 10 ,"LayerGameBoard");
		};
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "LayerLoginRole::showBoard");

	}
#endif
}

void LayerLoginRole::onZoneClicked( Ref* ref, Widget::TouchEventType type, string serverId )
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		auto iter = find_if(m_vcZone.begin(), m_vcZone.end(), [=](ServerParam param)->bool {
			return (param.m_strServerID.compare(serverId) == 0);
		});
		assert(iter != m_vcZone.end());

		updateDefaultServer(&(*iter));
		m_ui.uiZone.pRoot->setVisible(false);
	}
}

void LayerLoginRole::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	if (pSender == m_ui.uiZone.scrollviewServerIndex)
	{
		switch (type)
		{
		case ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.uiZone.scrollviewServerIndex->getInnerContainerSize();
				Vec2 vec = m_ui.uiZone.scrollviewServerIndex->getInnerContainer()->getPosition();
				float percentY = -vec.y / (size.height - m_ui.uiZone.scrollviewServerIndex->getContentSize().height);

				m_ui.uiZone.scrollbar->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			break;

		default:
			break;
		}
	}
}

void LayerLoginRole::onZoneIndexSelected( Ref* ref, Widget::TouchEventType type, int nIndex )
{
	if (type == Widget::TouchEventType::ENDED)
	{
		// 点击音效放在这里，否则滑动的时候会触发;
		SoundUtils::playSoundEffect("dianji");

		m_ui.uiZone.imgZoneListBg->setVisible(true);
		m_ui.uiZone.imgRolesBg->setVisible(false);

		updateServerZoneList(nIndex);

		// 选中;
		m_ui.uiZone.imgRolesSelected->setVisible(false);
		m_ui.uiZone.imgIndexSelected->setVisible(true);
		m_ui.uiZone.imgIndexSelected->setPosition(dynamic_cast<Text*>(ref)->getPosition());
	}
}
