#include "LayerRob.h"
#include "RobModel.h"
#include "RobController.h"
#include "MainModule/MainCityScene.h"
#include "FightModule/SceneFightLoading.h"
#include "FightModule/FightController.h"
#include "Widget/LayerCommHeadInfo.h"
#include "Widget/ActionCreator.h"
#include "MarvellousActive/MarvellousActiveScene.h"


RobResultItem* RobResultItem::create( ROB_RESULT_INFO param )
{
	RobResultItem* _item = new RobResultItem;
	if (nullptr != _item && _item->init(param))
	{
		_item->autorelease();
		return _item;
	}

	CC_SAFE_DELETE(_item);
	return nullptr;
}

bool RobResultItem::init( ROB_RESULT_INFO param )
{
	if (!Layout::init())
	{
		return false;
	}

	// UI;
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIRob/ResultItem.ExportJson"));
	this->addChild(m_ui.pRoot);

	// 标题;
	m_ui.imgIndex = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Index"));
	ostringstream oss;
	oss << "Image/UIBarrier/SweepTitle/img_" << param.nOrder << "ci.png";
	m_ui.imgIndex->loadTexture(oss.str());

	// 货币;
	oss.str("");
	oss << param.nCoin;
	m_ui.txtCoin = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Coin"));
	m_ui.txtCoin->setFontName(FONT_FZZHENGHEI);
	m_ui.txtCoin->setString(oss.str());

	// 经验;
	oss.str("");
	oss << param.nExp;
	m_ui.txtExp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Exp"));
	m_ui.txtExp->setFontName(FONT_FZZHENGHEI);
	m_ui.txtExp->setString(oss.str());

	// 扣除精力;
	oss.str("");
	oss << -param.nEnergy;
	m_ui.txtEnergy = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Energy"));
	m_ui.txtEnergy->setFontName(FONT_FZZHENGHEI);
	m_ui.txtEnergy->setString(oss.str());

	// 物品展示;
	int i = 0;
	for (auto iter = param.mapDropItems.begin(); iter != param.mapDropItems.end() && i < 3; ++iter, ++i)
	{
		UIToolHeader* item = UIToolHeader::create(iter->first);
		item->setNumEx(iter->second);
		item->setPosition(Vec2(100.0f + i*175.0f, 120.0f));
		m_ui.pRoot->addChild(item);
	}

	return true;
}

///////////////////////////////////////////////////

RobItem* RobItem::create( ROB_PLAYER_INFO param )
{
	RobItem* _item = new RobItem;
	if (nullptr != _item && _item->init(param))
	{
		_item->autorelease();
		return _item;
	}

	CC_SAFE_DELETE(_item);
	return nullptr;
}

bool RobItem::init( ROB_PLAYER_INFO param )
{
	if (!Layout::init())
	{
		return false;
	}

	// UI;
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIRob/RobItem.ExportJson"));
	this->addChild(m_ui.pRoot);

	// 玩家名;
	m_ui.txtName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Name"));
	m_ui.txtName->setFontName(FONT_FZZHENGHEI);
	m_ui.txtName->setString(param.strPlayerName);

	// 玩家;
	m_ui.player = UIToolHeader::create(ResourceUtils::getSmallIconPath(param.petResId));
	m_ui.player->setLvl(param.nLv);
	m_ui.player->setPosition(Vec2(90.0f, 75.0f));
	m_ui.pRoot->addChild(m_ui.player);

	// 掠夺;
	m_ui.btnRob = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Rob"));
	m_ui.btnRob->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::ENDED == type)
		{
			if (m_parent != nullptr)
			{
				MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
				if (pMainParam->mEnergy < RobModel::getInstance()->getRobCostEnergy())
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_ENERGY);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
				else
				{
					m_parent->onRob(param.nPlayerId, 1);
				}
			}
		}
	});
	m_ui.btnRob10 = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Rob10"));
	if (param.nPlayerType < 0)
	{
		m_ui.btnRob10->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
			if (Widget::TouchEventType::ENDED == type)
			{
				if (m_parent != nullptr)
				{
					MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
					if (pMainParam->mEnergy < RobModel::getInstance()->getRobCostEnergy()*10)
					{
						LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_ENERGY);
						Director::getInstance()->getRunningScene()->addChild(buyEnergy);
					}
					else
					{
						m_parent->onRob(param.nPlayerId, 10);
					}
				}
			}
		});
	}
	else
	{
		m_ui.btnRob10->setVisible(false);
	}

	// 掠夺概率 4:极高  3:高  2:低  1:极低;
	ostringstream oss;
	if (param.nPro == 2)
	{
		oss << "Image/Icon/global/img_2di.png";
	}
	else if (param.nPro == 3)
	{
		oss << "Image/Icon/global/img_3gao.png";
	}
	else if (param.nPro == 4)
	{
		oss << "Image/Icon/global/img_4jigao.png";
	}
	else
	{
		oss << "Image/Icon/global/img_1jidi.png";
	}
	m_ui.imgPro = ImageView::create(oss.str());
	m_ui.imgPro->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	m_ui.imgPro->setPosition(Vec2(350.0f, 45.0f));
	m_ui.pRoot->addChild(m_ui.imgPro);

	return true;
}

///////////////////////////////////////////////////

RecordItem* RecordItem::create( ROB_RECORDS_INFO param )
{
	RecordItem* _item = new RecordItem;
	if (nullptr != _item && _item->init(param))
	{
		_item->autorelease();
		return _item;
	}

	CC_SAFE_DELETE(_item);
	return nullptr;
}

bool RecordItem::init( ROB_RECORDS_INFO param )
{
	if (!Layout::init())
	{
		return false;
	}

	// UI;
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->getInstance()->widgetFromJsonFile("Image/UIRob/RecordItem.ExportJson"));
	this->addChild(m_ui.pRoot);

	// Item;
	UIToolHeader* _item = UIToolHeader::create(param.itemId);
	_item->setPosition(Vec2(95.0f, 85.0f));
	m_ui.pRoot->addChild(_item);

	// Desc;
	//m_ui.txtDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Desc"));
	//m_ui.txtDesc->setFontName(FONT_FZZHENGHEI);
	//m_ui.txtDesc->enableOutline(Color4B(Color3B(0x54, 0x0A, 0x0A)), 2);

	m_ui.txtDesc = SimpleRichText::create();
	m_ui.txtDesc->ignoreContentAdaptWithSize(false);
	m_ui.txtDesc->setPosition(Vec2(430.0f, 105.0f));
	m_ui.txtDesc->setContentSize(Size(535.0f, 65.0f));
	m_ui.pRoot->addChild(m_ui.txtDesc);

	string str;
	if (param.succflag > 0)
	{
		if (param.direction == 0)
		{
			str = "|0xFFF9F0::" + param.strPlayerName + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(20198) + "|0xFFF9F0::" + param.strItemName + "|" + "，" +
				DataManager::getInstance()->searchCommonTexdtById(20199);
		}
		else if (param.direction == 1)
		{
			str =DataManager::getInstance()->searchCommonTexdtById(20204) + 
				"|0xFFF9F0::" + param.strPlayerName + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(20200) + "|0xFFF9F0::" + param.strItemName + "|" + "，" + 
				DataManager::getInstance()->searchCommonTexdtById(20201);
		}
	}
	else
	{
		if (param.direction == 0)
		{
			str = "|0xFFF9F0::" + param.strPlayerName + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(20202);
		}
		else if (param.direction == 1)
		{
			str = DataManager::getInstance()->searchCommonTexdtById(20204)
				+"|0xFFF9F0::" + param.strPlayerName + "|" + 
				DataManager::getInstance()->searchCommonTexdtById(20203);
		}
	}
	m_ui.txtDesc->setString(str, 20, Color3B(0xE7, 0xB9, 0x72));

	// Date;
	m_ui.txtDate = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Date"));
	m_ui.txtDate->setFontName(FONT_FZZHENGHEI);
	m_ui.txtDate->setString(param.strDate);
	//m_ui.txtDate->enableOutline(Color4B(Color3B(0x78, 0x00, 0x00)), 2);
	setTextAddStroke(m_ui.txtDate, Color3B(0x78, 0x00, 0x00),2);
	// 掠夺;
	/*
	m_ui.btnRobBack = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, "Btn_Rob"));
	m_ui.btnRobBack->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::ENDED == type)
		{
			if (m_parent != nullptr)
			{
				// 跳转到掠夺界面;
				m_parent->onRecordRob();
			}
		}
	});
	*/

	return true;
}


///////////////////////////////////////////////////

LayerRob::LayerRob()
	: m_bIsRest(false)
	, m_nSelectToolId(-1)
	, m_bIsRobTenSuccess(false)
	, m_bIsFirstEnter(true)
	, mIsPushScene(false)
{
	RobModel::getInstance()->addObserver(this);
	m_pRobController = RobController::getInstance();

	m_mapUITools.clear();

	m_nSelectChipId = 0;
	m_nTurnCardIndex = -1;
	//
	//m_nSelectRobIndex = 0;
	m_nSelectRobTimes = 0;
	m_bTurnCardResultAnimationIsRun = false;
}

LayerRob::~LayerRob()
{
	RobModel::getInstance()->removeObserver(this);
	//RobController::destroyInstance();	// 加了推送消息，controller不能即使销毁 added by Phil 01/07/2016 @zcjoy;
	m_pRobController = nullptr;
}

LayerRob* LayerRob::create(bool pushScene /*= false*/)
{
	LayerRob *pRet = new LayerRob();
	if (pRet && pRet->init(pushScene))
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

bool LayerRob::init(bool pushScene)
{
	if(!ModalLayer::init())
	{
		return false;
	}

	mIsPushScene = pushScene;
	
	initUI();

	// 取主界面信息;
	m_pRobController->handleRobMainInfo();
	return true;
}

void LayerRob::onEnter()
{
	Layer::onEnter();

	if (m_bIsFirstEnter)
	{
		m_bIsFirstEnter = false;
	}
	else
	{
		// 刷新碎片;
		initToolsAndChips();

		// 刷新掠夺对手面板的精力消耗;
		ostringstream oss;
		oss << RobModel::getInstance()->getRobCostEnergy() << "/" << MainModel::getInstance()->getMainParam()->mEnergy;
		m_ui.uiRob.txtEnergy->setString(oss.str());
		oss.str("");
	}
	
}

void LayerRob::onExit()
{
	ResourceLoader::getInstance()->removeUIResource("UIRob");
	Layer::onExit();
}

void LayerRob::initUI()
{
	m_ui.pRoot = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("Image/UIRob/UIRobMain.ExportJson"));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_ui.pRoot);

	// 通用标题栏;
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(
		[this](Ref* sender, Widget::TouchEventType type)->void
	{
		this->onBtnBack(sender, type);
	},
		HEAD_INFO_TYPE_PLUNDER);
	m_ui.pRoot->addChild(_layerCommHeadInfo, 2);

	// 宝物列表;
	m_ui.scrollviewList = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot, "ScrollView_List"));

	// 碎片背景;
	m_ui.imgChipBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Chip_Bg"));

	// 合成;
	m_ui.btnCompose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Compose)));
	m_ui.btnCompose->addTouchEventListener(CC_CALLBACK_2(LayerRob::onBtnClicked, this, Btn_Compose));
	m_ui.btnRapidCompose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_RapidCompose)));
	m_ui.btnRapidCompose->addTouchEventListener(CC_CALLBACK_2(LayerRob::onBtnClicked, this, Btn_RapidCompose));

	// 掠夺记录;
	m_ui.btnRecord = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Record)));
	m_ui.btnRecord->addTouchEventListener(CC_CALLBACK_2(LayerRob::onBtnClicked, this, Btn_Record));

	// 免战;
	m_ui.btnRest = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot, STRING(Btn_Rest)));
	m_ui.btnRest->addTouchEventListener(CC_CALLBACK_2(LayerRob::onBtnClicked, this, Btn_Rest));
	m_ui.txtRestNum = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.btnRest, "Lab_Rest_Num"));
	m_ui.txtRestNum->setFontName(FONT_FZZHENGHEI);
	m_ui.txtRestNum->setString(_TO_STR(0));
	m_ui.imgTime = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot, "Img_Time"));
	m_ui.imgTime->setVisible(false);
	m_ui.txtTime = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.pRoot, "Lab_Time"));
	m_ui.txtTime->setFontName(FONT_FZZHENGHEI);
	m_ui.txtTime->setString("");

	//
	initUIRob();
	initUIResult();
	initUIDetail();
	initUIRest();
	initUIRecord();
}

void LayerRob::initUIRob()
{
	m_ui.uiRob.pRoot = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Rob"));
	m_ui.uiRob.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiRob.pRoot, "Img_Bg"));
	m_ui.uiRob.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRob.pRoot, "Btn_Rob_Close"));
	m_ui.uiRob.btnClose->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			m_ui.uiRob.pRoot->setVisible(false);
		}
	});
	m_ui.uiRob.txtEnergy = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRob.pRoot, "Lab_Energy"));
	m_ui.uiRob.txtEnergy->setFontName(FONT_FZZHENGHEI);
	m_ui.uiRob.txtEnergy->setString("");
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRob.pRoot, "Label_tips"))->setFontName(FONT_FZZHENGHEI);
	m_ui.uiRob.scrollviewOpList = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.uiRob.pRoot, "ScrollView_OpList"));
	m_ui.uiRob.btnChange = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRob.pRoot, "Btn_Change"));
	m_ui.uiRob.btnChange->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			m_pRobController->handleRefreshEnemy(m_nSelectChipId);
		}
	});
}

void LayerRob::initUIResult()
{
	m_ui.uiRobResult.pRoot = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Result"));
	m_ui.uiRobResult.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiRobResult.pRoot, "Img_Bg"));

	// 关闭回调;
	auto closeCallback = [=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			m_ui.uiRobResult.pRoot->setVisible(false);

			// 未掠夺到，仍打开对手窗口;
			//if (!m_bIsRobTenSuccess)
			//{
			//	showRob(false);
			//}
		}
	};

	// 关闭按钮;
	m_ui.uiRobResult.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRobResult.pRoot, "Btn_Result_Close"));
	m_ui.uiRobResult.btnClose->addTouchEventListener(closeCallback);

	// 掠夺完成;
	m_ui.uiRobResult.btnFinish = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRobResult.pRoot, "Btn_Finish"));
	m_ui.uiRobResult.btnFinish->addTouchEventListener(closeCallback);
	m_ui.uiRobResult.btnFinish->setVisible(false);

	// 继续掠夺;
	m_ui.uiRobResult.btnContinue = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRobResult.pRoot, "Btn_Continue"));
	m_ui.uiRobResult.btnContinue->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			onRob(-1, 10);
		}
	});

	m_ui.uiRobResult.scrollviewResult = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.uiRobResult.pRoot, "ScrollView_Result"));
}

void LayerRob::initUIDetail()
{
	m_ui.uiDetail.pRoot = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Detail"));
	m_ui.uiDetail.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiDetail.pRoot, "Img_Bg"));
	m_ui.uiDetail.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiDetail.pRoot, "Btn_Detail_Close"));
	m_ui.uiDetail.btnClose->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			m_ui.uiDetail.pRoot->setVisible(false);
		}
	});

	// item pos (-186.0f, 72.0f);
	m_ui.uiDetail.txtName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiDetail.pRoot, "Lab_Name"));
	m_ui.uiDetail.txtName->setFontName(FONT_FZZHENGHEI);
	m_ui.uiDetail.txtAmount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiDetail.pRoot, "Lab_Amount"));
	m_ui.uiDetail.txtAmount->setFontName(FONT_FZZHENGHEI);
	m_ui.uiDetail.txtDesc = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiDetail.pRoot, "Lab_Desc"));
	m_ui.uiDetail.txtDesc->setFontName(FONT_FZZHENGHEI);
}

void LayerRob::initUIRest()
{
	m_ui.uiRest.pRoot = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Rest"));
	m_ui.uiRest.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiRest.pRoot, "Img_Bg"));
	m_ui.uiRest.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRest.pRoot, "Btn_Rest_Close"));
	m_ui.uiRest.btnClose->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			m_ui.uiRest.pRoot->setVisible(false);
		}
	});
	m_ui.uiRest.txtRestNum2 = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRest.pRoot, "Lab_Rest_Num2"));
	m_ui.uiRest.txtRestNum2->setFontName(FONT_FZZHENGHEI);
	m_ui.uiRest.txtRestNum2->setString("");
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRest.pRoot, "Label_word_1"))->setFontName(FONT_FZZHENGHEI);
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRest.pRoot, "Label_word_2"))->setFontName(FONT_FZZHENGHEI);
	dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRest.pRoot, "Label_tips"))->setFontName(FONT_FZZHENGHEI);

	m_ui.uiRest.btnOK = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRest.pRoot, "Btn_OK"));
	m_ui.uiRest.btnOK->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			m_pRobController->handleRest((RobModel::getInstance()->getAvoidamount() > 0) ? 1 : 2);
			m_ui.uiRest.pRoot->setVisible(false);
		}
	});
	m_ui.uiRest.btnCancel = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRest.pRoot, "Btn_Cancel"));
	m_ui.uiRest.btnCancel->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			m_ui.uiRest.pRoot->setVisible(false);
		}
	});
}

void LayerRob::initUIRecord()
{
	m_ui.uiRecord.pRoot = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.pRoot, "Pnl_Record"));
	m_ui.uiRecord.imgBg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiRecord.pRoot, "Img_Bg"));
	m_ui.uiRecord.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRecord.pRoot, "Btn_Record_Close"));
	m_ui.uiRecord.btnClose->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			m_ui.uiRecord.pRoot->setVisible(false);
		}
	});
	m_ui.uiRecord.scrollviewRecord = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.uiRecord.pRoot, "ScrollView_Record"));
}

void LayerRob::updateSelf( void* pData )
{
	if (nullptr == pData)
	{
		return;
	}

	ObserverParam* param = (ObserverParam*) pData;

	switch (param->id)
	{
	case OBS_PARAM_TYPE_ROB_MAIN_INFO:
		updateMainInfo((Rob_Main_Info*)(param->updateData));
		break;

	case OBS_PARAM_TYPE_ROB_REFRESH_ENEMY:
		updateEnemy((vector<ROB_PLAYER_INFO>*)(param->updateData));
		break;

	case OBS_PARAM_TYPE_ROB_RECOED:
		updateRobRecord((vector<ROB_RECORDS_INFO>*)(param->updateData));
		break;

	case OBS_PARAM_TYPE_ROB_REST_TIME:
		onCountDown(*((int*)(param->updateData)));
		break;

	case OBS_PARAM_TYPE_ROB_COMPOSE_RESULT:
		{
			// 显示详情;
			showDetail(m_nSelectToolId);

			// 刷新道具显示;
			initToolsAndChips();
		}
		break;

	case OBS_PARAM_TYPE_ROB_TEN_RESULT:
		updateRobTenResult((ROB_TEN_RESULT*)(param->updateData));
		break;

	case OBS_PARAM_TYPE_ROB_PUSH_UPDATE:
		initToolsAndChips();
		break;

	default:
		break;
	}
}

void LayerRob::updateMainInfo( const Rob_Main_Info* info )
{
	if (nullptr == info)
		return;

	// 免战牌数量;
	m_ui.txtRestNum->setString(_TO_STR(info->nSignAmount));
	m_ui.uiRest.txtRestNum2->setString(_TO_STR(info->nSignAmount));

	// 免战倒计时;
	onCountDown(info->nRestTime);

	// 初始化;
	initToolsAndChips();
}

void LayerRob::initToolsAndChips()
{
	// 更新道具列表;
	updateTools();

	// 初始默认选中或者碎片全部消耗;
	auto it = m_mapUITools.find(m_nSelectToolId);
	if (it == m_mapUITools.end())
	{
		if (!m_mapUITools.empty())
		{
			selectTool(m_mapUITools.begin()->second);
		}
	}
	else
	{
		it->second->setSelected(true);
		updateChips(false);
	}
}

void LayerRob::updateTools()
{
	// 清除所有;
	m_ui.scrollviewList->removeAllChildren();
	m_mapUITools.clear();

	// ;
	map<int, map<int, int> >* mapTools = RobModel::getInstance()->getTools();
	int nIndex = 0;
	for (auto it = mapTools->begin(); it != mapTools->end(); ++it, ++nIndex)
	{
		float fWidth = m_ui.scrollviewList->getContentSize().width;
		float fTotalHeight = 104.0f * mapTools->size();
		float fHeight = std::max(m_ui.scrollviewList->getContentSize().height, fTotalHeight);
		m_ui.scrollviewList->setInnerContainerSize(Size(fWidth, fHeight));

		// add child;
		UIToolHeader* _item = UIToolHeader::create(it->first);
		_item->setPosition(Vec2(fWidth/2.0f, fHeight - 60.0f - nIndex*104.0f));
		_item->setScale(0.88f);
		m_ui.scrollviewList->addChild(_item);
		m_mapUITools.insert(make_pair(it->first, _item));

		// 红点;
		_item->setRedDot(RobModel::getInstance()->checkComposeValid(it->first));

		_item->setTouchEnabled(true);
		_item->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
			if (Widget::TouchEventType::BEGAN == type)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (Widget::TouchEventType::ENDED == type)
			{
				selectTool(dynamic_cast<UIToolHeader*>(pSender));
			}
		});
	}
}

void LayerRob::selectTool( UIToolHeader* tool )
{
	if (nullptr == tool)
		return;

	auto itCur = m_mapUITools.find(m_nSelectToolId);
	if (itCur != m_mapUITools.end() && itCur->second == tool)
	{
		return;
	}

	// 选中状态;
	tool->setSelected(true);
	if (itCur != m_mapUITools.end())
	{
		itCur->second->setSelected(false);
	}

	// 刷新选中记录;
	for (auto it : m_mapUITools)
	{
		if (it.second == tool)
		{
			m_nSelectToolId = it.first;
		}
	}

	// 刷新碎片;
	updateChips();
}

void LayerRob::selectTool( int nToolId )
{
	if (m_nSelectToolId == nToolId)
		return;

	auto it = m_mapUITools.find(nToolId);
	if (it != m_mapUITools.end())
	{
		selectTool(it->second);
	}
}

void LayerRob::updateChips( bool bShowAnimation /*= true*/ )
{
	Vector<Node*> vcChildren = m_ui.imgChipBg->getChildren();
	if (vcChildren.size() > 0)
	{
		if (bShowAnimation)
		{
			for (int i = 0; i < vcChildren.size(); ++i)
			{
				vcChildren.at(i)->runAction(Sequence::create(EaseSineIn::create(FadeOut::create(0.1f)), 
					CallFuncN::create([=](Node* pSender){
						pSender->removeFromParent();
						if (i == vcChildren.size()-1)
						{
							updateChipsAfterAnimation();
						}
				}), nullptr));
			}
		}
		else
		{
			for (int i = 0; i < vcChildren.size(); ++i)
				vcChildren.at(i)->removeFromParent();

			updateChipsAfterAnimation();
		}
	}
	else
	{
		updateChipsAfterAnimation();
	}
}
void LayerRob::updateChipsAfterAnimation()
{
	// 居中展示道具;
	ImageView* imgItemBg = ImageView::create("Image/UIRob/Pic/img_di6.png");
	imgItemBg->setPosition(Vec2(m_ui.imgChipBg->getContentSize().width/2.0f, m_ui.imgChipBg->getContentSize().height/2.0f + 30.0f));
	imgItemBg->setScale(0.85f);
	m_ui.imgChipBg->addChild(imgItemBg);
	UIToolHeader* tool = UIToolHeader::create(m_nSelectToolId);
	tool->setToolId(m_nSelectToolId);
	tool->setPosition(imgItemBg->getPosition());
	m_ui.imgChipBg->addChild(tool, imgItemBg->getLocalZOrder()+1);

	// 增加道具名称显示;
	ImageView* imgNameBg = ImageView::create("Image/Icon/global/img_di4.png");
	imgNameBg->setScaleX(150.0f/imgNameBg->getContentSize().width);
	imgNameBg->setPosition(tool->getPosition() + Point(0.0f, -85.0f));
	m_ui.imgChipBg->addChild(imgNameBg);
	RowData* rowData = DataManager::getInstance()->searchToolById(m_nSelectToolId);
	if (nullptr != rowData)
	{
		Text* txtName = Text::create(rowData->getStringData("itemName"), FONT_FZZHENGHEI, 24);
		setTextAddStroke(txtName, Color3B(0x79, 0x03, 0x03), 2);
		txtName->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		txtName->setPosition(imgNameBg->getPosition());
		m_ui.imgChipBg->addChild(txtName, imgNameBg->getLocalZOrder()+1);
	}

	// 道具点击响应;
	tool->setTouchEnabled(true);
	tool->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
		if (Widget::TouchEventType::BEGAN == type)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (Widget::TouchEventType::ENDED == type)
		{
			UIToolHeader* toolItem = dynamic_cast<UIToolHeader*>(pSender);
			if (nullptr != toolItem)
			{
				showDetail(toolItem->getToolId());
			}
		}
	});

	// 四周展示道具碎片;
	map<int, int>* mapChips = RobModel::getInstance()->getChips(m_nSelectToolId);
	if (nullptr == mapChips)
		return;

	// 碎片数量;
	int nChipsCount = mapChips->size();
	if (nChipsCount < 3 || nChipsCount > 6)
		return;

	// 定位;
	vector<Point> vcPos;
	calcChipsPosition(nChipsCount, vcPos);

	// 生成碎片;
	static int nIndex = 0;
	for_each(mapChips->begin(), mapChips->end(), [=](pair<int, int> it){
		UIToolHeader* _chip = UIToolHeader::create(it.first);
		_chip->setNumEx(it.second);
		if (it.second <= 0)
		{
			_chip->setMaskIcon("Image/Icon/global/btn_kuang_zhuangbei_zhezhao.png");
		}
		_chip->setToolId(it.first);
		_chip->setPosition(vcPos.at(nIndex));
		m_ui.imgChipBg->addChild(_chip);
		nIndex++;

		// 碎片点击响应;
		_chip->setTouchEnabled(true);
		_chip->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type){
			if (Widget::TouchEventType::BEGAN == type)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (Widget::TouchEventType::ENDED == type)
			{
				UIToolHeader* toolChip = dynamic_cast<UIToolHeader*>(pSender);
				if (nullptr != toolChip)
				{
					int nNewSelectId = toolChip->getToolId();
					bool bNeedRefresh = (nNewSelectId != m_nSelectChipId);
					m_nSelectChipId = nNewSelectId;
					if (it.second > 0)
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20205));
					}
					else
					{
						showRob(bNeedRefresh);
					}
				}
			}
		});
	});
	nIndex = 0;

	// 淡入;
	Vector<Node*> vcChildren = m_ui.imgChipBg->getChildren();
	if (vcChildren.size() > 0)
	{
		for (int i = 0; i < vcChildren.size(); ++i)
		{
			vcChildren.at(i)->setOpacity(0);
			vcChildren.at(i)->runAction(EaseSineOut::create(FadeIn::create(0.1f)));
		}
	}
}

void LayerRob::calcChipsPosition( int nCount, vector<Point>& vcPos )
{
	vcPos.clear();

	// 以背景图中心为圆点;
	Point dot(m_ui.imgChipBg->getContentSize().width/2.0f, m_ui.imgChipBg->getContentSize().height/2.0f);
	float fRadius = 240.0f;

	// 以Y轴正向为起点;
	float perAngle = 360.0f / nCount;
	float fStartAngleY = (nCount%2 == 0) ? perAngle/2.0f : 0.0f;

	// 计算角度;
	float PI = 3.141592f;
	for (int i = 0; i < nCount; ++i)
	{
		float angle = fStartAngleY + perAngle * i;
		Point pt(sin(angle * (PI/180)) * fRadius, cos(angle * (PI/180)) * fRadius);
		vcPos.push_back(pt + dot);
	}

	// 校验;
	assert(vcPos.size() == nCount);
}

void LayerRob::showRob( bool bRefresh /*= true*/ )
{
	// 弹窗动画;
	m_ui.uiRob.pRoot->setVisible(true);
	ActionCreator::runCommDlgAction(m_ui.uiRob.imgBg);

	// 拉取对手;
	bool bEmpty = (m_ui.uiRob.scrollviewOpList->getChildrenCount() == 0);
	if (bRefresh || bEmpty)
	{
		m_pRobController->handleRefreshEnemy(m_nSelectChipId);
	}
}

void LayerRob::showDetail( int nToolId )
{
	// 弹窗动画;
	m_ui.uiDetail.pRoot->setVisible(true);
	auto func = [](){ GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ROB_COMPOSE); };
	ActionCreator::runCommDlgAction(m_ui.uiDetail.imgBg, func);

	// 查询详细信息;
	Rob_Tool_Detail_Info  info;
	RobModel::getInstance()->getToolInfo(nToolId, info);
	m_ui.uiDetail.txtName->setString(info.strName);
	m_ui.uiDetail.txtDesc->setString(info.strDesc);
	ostringstream  oss;
	oss << DataManager::getInstance()->searchCommonTexdtById(20099) << info.nAmount << DataManager::getInstance()->searchCommonTexdtById(10220);
	m_ui.uiDetail.txtAmount->setString(oss.str());
	oss.str("");

	if (m_ui.uiDetail.item != nullptr)
	{
		m_ui.uiDetail.item->removeFromParent();
		m_ui.uiDetail.item = nullptr;
	}

	m_ui.uiDetail.item = UIToolHeader::create(nToolId);
	m_ui.uiDetail.item->setPosition(Vec2(120.0f, 290.0f));
	m_ui.uiDetail.imgBg->addChild(m_ui.uiDetail.item);
}

void LayerRob::showRobRecord()
{
	// 弹窗动画;
	m_ui.uiRecord.pRoot->setVisible(true);
	ActionCreator::runCommDlgAction(m_ui.uiRecord.imgBg, [=](){
		// 拉取掠夺记录;
		m_pRobController->handleRobRecord();
	});
}

void LayerRob::showRest()
{
	ImageView* imgIcon = dynamic_cast<ImageView*>(m_ui.uiRest.imgBg->getChildByName("Image_Icon"));
	Text* txtDesc1 = dynamic_cast<Text*>(m_ui.uiRest.imgBg->getChildByName("Label_word_1"));

	// 判断免战牌剩余;
	if (RobModel::getInstance()->getAvoidamount() <= 0)
	{
		imgIcon->loadTexture("Image/Icon/global/img_yuanbao.png");
		ostringstream oss;
		oss << DataManager::getInstance()->searchCommonTexdtById(20207)
			<< RobModel::getInstance()->getRestCost() << DataManager::getInstance()->searchCommonTexdtById(20077);
		//txtDesc1->setString("是否使用20个");
		txtDesc1->setString(oss.str());
	}
	else
	{
		imgIcon->loadTexture("Image/Icon/global/img_mianzhanpai.png");
		txtDesc1->setString(DataManager::getInstance()->searchCommonTexdtById(20208));
	}

	// 弹窗动画;
	m_ui.uiRest.pRoot->setVisible(true);
	ActionCreator::runCommDlgAction(m_ui.uiRest.imgBg);
}

void LayerRob::showRobResult()
{
	// 弹窗动画;
	m_ui.uiRobResult.pRoot->setVisible(true);
	auto func = [](){ GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ROB_REWARD); };
	ActionCreator::runCommDlgAction(m_ui.uiRobResult.imgBg, func);
}

void LayerRob::onCountDown(int sec)
{
	// 刷新免战牌数量(暂时放这里);
	m_ui.txtRestNum->setString(_TO_STR(RobModel::getInstance()->getAvoidamount()));
	m_ui.uiRest.txtRestNum2->setString(_TO_STR(RobModel::getInstance()->getAvoidamount()));

	// 刷新时间;
	m_ui.imgTime->setVisible(sec > 0);
	m_ui.txtTime->setString(formatTime(sec));
	m_bIsRest = (sec > 0);
}

void LayerRob::updateEnemy(vector<ROB_PLAYER_INFO>* vcEnemy)
{
	if (nullptr == vcEnemy)
		return;

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ROB_SELECT_ENEMY);

	// 精力消耗;
	ostringstream oss;
	oss << RobModel::getInstance()->getRobCostEnergy() << "/" << MainModel::getInstance()->getMainParam()->mEnergy;
	m_ui.uiRob.txtEnergy->setString(oss.str());
	oss.str("");

	// 清除所有;
	m_ui.uiRob.scrollviewOpList->removeAllChildren();
	assert(vcEnemy->size() == 3);

	float fWidth = m_ui.uiRob.scrollviewOpList->getContentSize().width;
	float fTotalHeight = 140.0f * vcEnemy->size();
	float fHeight = std::max(m_ui.uiRob.scrollviewOpList->getContentSize().height, fTotalHeight);
	m_ui.uiRob.scrollviewOpList->setInnerContainerSize(Size(fWidth, fHeight));

	for (int i = 0; i < vcEnemy->size(); ++i)
	{
		RobItem* _player = RobItem::create(vcEnemy->at(i));
		_player->setLogicParent(this);
		_player->setPosition(Vec2(0.0f, fHeight - 140.0f - i*133.0f));
		m_ui.uiRob.scrollviewOpList->addChild(_player);
	}
}

void LayerRob::updateRobRecord(vector<ROB_RECORDS_INFO>* record)
{
	if (nullptr == record)
		return;

	// 清除所有;
	m_ui.uiRecord.scrollviewRecord->removeAllChildren();

	float fWidth = m_ui.uiRecord.scrollviewRecord->getContentSize().width;
	float fTotalHeight = 160.0f * record->size();
	float fHeight = std::max(m_ui.uiRecord.scrollviewRecord->getContentSize().height, fTotalHeight);
	m_ui.uiRecord.scrollviewRecord->setInnerContainerSize(Size(fWidth, fHeight));

	// 重新添加;
	for (int i = 0; i < record->size(); ++i)
	{
		if (record->at(i).itemId != 0)
		{
			RecordItem* _record = RecordItem::create(record->at(i));
			_record->setLogicParent(this);
			_record->setPosition(Vec2(0.0f, fHeight - (i+1)*160.0f));
			m_ui.uiRecord.scrollviewRecord->addChild(_record);
		}
	}
}

void LayerRob::updateRobTenResult( ROB_TEN_RESULT* robResult )
{
	if (nullptr == robResult)
		return;

	showRobResult();

	// 清除所有;
	m_ui.uiRobResult.scrollviewResult->removeAllChildren();

	float fWidth = m_ui.uiRobResult.scrollviewResult->getContentSize().width;
	float fTotalHeight = 230.0f * (robResult->mapResult.size() + 1);
	float fHeight = std::max(m_ui.uiRobResult.scrollviewResult->getContentSize().height, fTotalHeight);
	m_ui.uiRobResult.scrollviewResult->setInnerContainerSize(Size(fWidth, fHeight));

	// 重新添加;
	int nIndex = 0;
	for (auto iter = robResult->mapResult.begin(); iter != robResult->mapResult.end(); ++iter, ++nIndex)
	{
		RobResultItem* _result = RobResultItem::create(iter->second);
		_result->setPosition(Vec2(0.0f, fHeight - (nIndex+1)*230.0f));
		m_ui.uiRobResult.scrollviewResult->addChild(_result);
	}

	// 结果说明;
	if (robResult->robFlag > 0 && robResult->fragId > 0)
	{
		m_bIsRobTenSuccess = true;

		// 掠夺到物品;
		ImageView* img = ImageView::create("Image/UIRob/Pic/img_huodesuipian.png");
		img->setPosition(Vec2(fWidth/2.0f, 190.0f));
		m_ui.uiRobResult.scrollviewResult->addChild(img);

		Text* txtName = Text::create(robResult->fragName, FONT_FZZHENGHEI, 45);
		setTextAddStroke(txtName, Color3B(0x79, 0x03, 0x03), 3);
		txtName->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		auto xPo = m_ui.uiRobResult.scrollviewResult->getContentSize().width/2 - txtName->getContentSize().width/2;
		txtName->setPosition(Vec2(xPo, img->getPositionY()-65.0f));
		m_ui.uiRobResult.scrollviewResult->addChild(txtName);

		// 更新碎片展示;
		initToolsAndChips();

		// 隐藏继续掠夺按钮;
		m_ui.uiRobResult.btnContinue->setVisible(false);
		m_ui.uiRobResult.btnFinish->setVisible(true);
	}
	else
	{
		m_bIsRobTenSuccess = false;

		// 未掠夺到物品;
		ImageView* img = ImageView::create("Image/UIRob/Pic/img_saodangwancheng.png");
		img->setPosition(Vec2(fWidth/2.0f, 115.0f));
		m_ui.uiRobResult.scrollviewResult->addChild(img);

		// 显示继续掠夺按钮;
		m_ui.uiRobResult.btnContinue->setVisible(true);
		m_ui.uiRobResult.btnFinish->setVisible(false);
	}

	// 滚动至底部;
	m_ui.uiRobResult.scrollviewResult->scrollToBottom(0.2f, true);

	// 更新此面板精力值;
	ostringstream oss;
	oss << robResult->costEnergy << "/" << MainModel::getInstance()->getMainParam()->mEnergy;
	m_ui.uiRob.txtEnergy->setString(oss.str());

	// 更新免战牌数量以及先关逻辑;
	m_ui.txtRestNum->setString(_TO_STR(RobModel::getInstance()->getAvoidamount()));
	m_ui.uiRest.txtRestNum2->setString(_TO_STR(RobModel::getInstance()->getAvoidamount()));
}


void LayerRob::onBtnBack( Ref* sender, Widget::TouchEventType type )
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		if (mIsPushScene)
		{
			Director::getInstance()->popScene();
		}
		else
		{
			Director::getInstance()->replaceScene(MainCityScene::create());
		}
	}
}

void LayerRob::onBtnClicked( Ref* pSender, Widget::TouchEventType type, int nWidgetName )
{
	if (Widget::TouchEventType::BEGAN == type)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (type == Widget::TouchEventType::ENDED)
	{
		switch (nWidgetName)
		{
		case Btn_Compose:
		case Btn_RapidCompose:
			{
				if (RobModel::getInstance()->checkComposeValid(m_nSelectToolId))
				{
					m_pRobController->handleCompose(m_nSelectToolId, (nWidgetName == Btn_Compose ? 0 : 1));
				}
				else
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20209));
				}
			}
			break;

		case Btn_Record:
			showRobRecord();
			break;

		case Btn_Rest:
			showRest();
			break;

		default:
			break;
		}
	}
}

void LayerRob::onRob( int nPlayerId, int nTimes /*= 1*/ )
{
	// 掠夺10次不进入战斗;
	if (nTimes == 10)
	{
		m_pRobController->handleRobTen();

		// 同时隐藏对手窗口;
		m_ui.uiRob.pRoot->setVisible(false);
		return;
	}

	// 当前是否在免战时间内;
	if (m_bIsRest)
	{
		// 查不到就默认是从掠夺记录中选择的对手;
		const vector<ROB_PLAYER_INFO>& vcEnemy = RobModel::getInstance()->getRobEnemyInfo();
		auto it = find(vcEnemy.begin(), vcEnemy.end(), nPlayerId);
		if (it == vcEnemy.end() || it->nPlayerType > 0)
		{
			CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20210), [=](Ref* pSender, CustomRetType type){
				if (type == RET_TYPE_OK)
				{
					Director::getInstance()->pushScene(SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_ROB_FIGHT, RobModel::getInstance()->getRobFightId()));
					FightController::getInstance()->sendRobFightMsg(nPlayerId);
				}
			}, CUSTOM_YES_NO);
			return;
		}	
	}

	// 同时隐藏对手窗口;
	m_ui.uiRob.pRoot->setVisible(false);

	Director::getInstance()->pushScene(SceneFightLoading::create(FIGHT_TYPE::FIGHT_TYPE_ROB_FIGHT, RobModel::getInstance()->getRobFightId()));
	FightController::getInstance()->sendRobFightMsg(nPlayerId);
}
