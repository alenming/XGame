#include "MyGroupBuildLayer.h"
#include "MyGroupModel.h"
#include "MyGroupController.h"
#include "MainModule/MainCityLoadingScene.h"


MyGroupBuildLayer::MyGroupBuildLayer():
_buildType(eBuildType::eBUILD_DEFAULT_TYPE),
isChaoZhou(true)
{
	MyGroupModel::getInstance()->addObserver(this);
}

MyGroupBuildLayer::~MyGroupBuildLayer()
{
	MyGroupModel::getInstance()->removeObserver(this);
}

Layer* MyGroupBuildLayer::create(eBuildType buildType)
{ 
	auto pRet = new MyGroupBuildLayer(); 
	if (pRet && pRet->init(buildType)) 
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

bool MyGroupBuildLayer::init(eBuildType buildType)
{
	if(!ModalLayer::init())
	{
		return false;
	}
	_buildType = buildType;

	initUI();

	return true;
}

void MyGroupBuildLayer::onEnter()
{
	ModalLayer::onEnter();


}

void MyGroupBuildLayer::onExit()
{
	ModalLayer::onExit();

}

void MyGroupBuildLayer::initUI()
{
	// 加载 UI;
	string strFileName;
	strFileName.assign("Image/UIGang/UIGang_Building.ExportJson");

	// Root;
	m_ui.uiRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.uiRoot->setName("UI_ROOT");
	this->addChild(m_ui.uiRoot);

	m_ui.uiExit = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.uiRoot,"Btn_Close"));
	m_ui.uiExit->addTouchEventListener(CC_CALLBACK_2(MyGroupBuildLayer::onBtnClick,this,BUILD_CALLBACK_TYPE::BTN_BUILD_EXIT));
	//普通
	auto puNode = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiRoot,"Img_Ordinary"));
	m_ui.uiOrdinary.uiTexExp = dynamic_cast<Text*>(Helper::seekWidgetByName(puNode,"Lab_EXP"));
	m_ui.uiOrdinary.uiTexShengWang = dynamic_cast<Text*>(Helper::seekWidgetByName(puNode,"Lab_Popularity"));
	m_ui.uiOrdinary.uiTexMoney = dynamic_cast<Text*>(Helper::seekWidgetByName(puNode,"Lab_Cost"));
	m_ui.uiOrdinary.uiBtnDonate = dynamic_cast<Button*>(Helper::seekWidgetByName(puNode,"Btn_Donate"));
	m_ui.uiOrdinary.uiBtnDonate->addTouchEventListener(CC_CALLBACK_2(MyGroupBuildLayer::onBtnClick,this,BUILD_CALLBACK_TYPE::BTN_COMMON));
	//高级
	auto gaoNode = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiRoot,"Img_Senior"));
	m_ui.uiSenior.uiTexExp = dynamic_cast<Text*>(Helper::seekWidgetByName(gaoNode,"Lab_EXP"));
	m_ui.uiSenior.uiTexShengWang = dynamic_cast<Text*>(Helper::seekWidgetByName(gaoNode,"Lab_Popularity"));
	m_ui.uiSenior.uiTexMoney = dynamic_cast<Text*>(Helper::seekWidgetByName(gaoNode,"Lab_Cost"));
	m_ui.uiSenior.uiBtnDonate = dynamic_cast<Button*>(Helper::seekWidgetByName(gaoNode,"Btn_Donate"));
	m_ui.uiSenior.uiBtnDonate->addTouchEventListener(CC_CALLBACK_2(MyGroupBuildLayer::onBtnClick,this,BUILD_CALLBACK_TYPE::BTN_HIGHTLEVEL));
	//土豪
	auto tuNode = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.uiRoot,"Img_Special"));
	m_ui.uiTuHao.uiTexExp = dynamic_cast<Text*>(Helper::seekWidgetByName(tuNode,"Lab_EXP"));
	m_ui.uiTuHao.uiTexShengWang = dynamic_cast<Text*>(Helper::seekWidgetByName(tuNode,"Lab_Popularity"));
	m_ui.uiTuHao.uiTexMoney = dynamic_cast<Text*>(Helper::seekWidgetByName(tuNode,"Lab_Cost"));
	m_ui.uiTuHao.uiBtnDonate = dynamic_cast<Button*>(Helper::seekWidgetByName(tuNode,"Btn_Donate"));
	m_ui.uiTuHao.uiBtnDonate->addTouchEventListener(CC_CALLBACK_2(MyGroupBuildLayer::onBtnClick,this,BUILD_CALLBACK_TYPE::BTN_MONEY));
	//商店界面显示
	m_ui.uiLayTouXiang = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.uiRoot,"Pnl_Building"));
	m_ui.uiLoaBuildExp = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(m_ui.uiRoot,"shop_exp_loadbar"));
	m_ui.uiLoaTodayExp = dynamic_cast<LoadingBar*>(Helper::seekWidgetByName(m_ui.uiRoot,"today_exp_loadbar"));
	m_ui.uiTexBuildExp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"shop_exp_text"));
	m_ui.uiTexTodayExp = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"today_exp_text"));
	m_ui.uiTexNextLevelEffeck = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_Tips"));
	m_ui.uiTexBuildName = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_Name"));
	m_ui.uiTexToDayCount = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_Time"));
	m_ui.uiTexToDayJuanXian = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_Tips2"));
	m_ui.uiTexBuildLevel = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.uiRoot,"Lab_Level"));	

	initBuildInfo();
	iniJuanXian();
}

void  MyGroupBuildLayer::initBuildInfo()
{
	auto mBuildInfo = MyGroupModel::getInstance()->getBuildInfoByID(_buildType);

	auto pathname = MyGroupModel::getInstance()->getBuildIconPath(mBuildInfo->resId);
	auto touxiang = Sprite::create(pathname);
	touxiang->setPosition(m_ui.uiLayTouXiang->getContentSize().width/2,m_ui.uiLayTouXiang->getContentSize().height/2);
	m_ui.uiLayTouXiang->addChild(touxiang);//放张图片 


	m_ui.uiLoaBuildExp->setPercent((float)mBuildInfo->curExp/(float)mBuildInfo->nextExp*100);
	m_ui.uiTexBuildExp->setString(TO_STR(mBuildInfo->curExp)+"/"+TO_STR(mBuildInfo->nextExp));
	m_ui.uiTexBuildExp->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexBuildExp, Color3B(0x6a, 0x20, 0x11), 2);

	auto mGroupInfo = MyGroupModel::getInstance()->getMyGroupInfo();
	m_ui.uiTexTodayExp->setString(TO_STR(mGroupInfo->todayExp)+"/"+TO_STR(mGroupInfo->todayMaxExp));
	m_ui.uiTexTodayExp->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexTodayExp, Color3B(0x1a, 0x1a, 0x1f), 2);
	m_ui.uiLoaTodayExp->setPercent((float)mGroupInfo->todayExp/(float)mGroupInfo->todayMaxExp*100);



	m_ui.uiTexNextLevelEffeck->setString(mBuildInfo->nextEffect);
	m_ui.uiTexNextLevelEffeck->setColor(Color3B(0xff, 0xff, 0xff));
	m_ui.uiTexNextLevelEffeck->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexNextLevelEffeck, Color3B(0x5b, 0x14, 0x14), 2);

	m_ui.uiTexBuildName->setString(mBuildInfo->name);
	m_ui.uiTexBuildName->setColor(Color3B(0xe8, 0xde, 0xb5));
	m_ui.uiTexBuildName->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexBuildName, Color3B(0x7d, 0x3f, 0x1c), 2);

	m_ui.uiTexToDayJuanXian->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexToDayJuanXian, Color3B(0x7d, 0x3f, 0x1c), 2);

	auto facInfo = MyGroupModel::getInstance()->getMyInfo();
	if (!facInfo)
	{
		return;
	}
	m_ui.uiTexToDayCount->setString(TO_STR((facInfo->myMaxDonateTimes)-(facInfo->myCurDonateTimes))+"/"+TO_STR(facInfo->myMaxDonateTimes));
	//m_ui.uiTexToDayCount->setColor(Color3B(0xff, 0x36, 0x07));
	//m_ui.uiTexToDayCount->setFontName(FONT_FZZHENGHEI);
	//setTextAddStroke(m_ui.uiTexToDayCount, Color3B(0x20, 0x07, 0x05), 2);
	if (facInfo->myCurDonateTimes < facInfo->myMaxDonateTimes)
	{
		m_ui.uiTexToDayCount->setColor(Color3B(0xff, 0xff, 0xff));
		m_ui.uiTexToDayCount->setFontName(FONT_FZZHENGHEI);
		//setTextAddStroke(m_ui.uiTexToDayCount, Color3B(0x20, 0x07, 0x05), 2);
	}
	else if (facInfo->myCurDonateTimes == facInfo->myMaxDonateTimes)
	{
		m_ui.uiTexToDayCount->setColor(Color3B(0xff, 0x36, 0x07));
		m_ui.uiTexToDayCount->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(m_ui.uiTexToDayCount, Color3B(0x20, 0x07, 0x05), 2);
	}

	m_ui.uiTexBuildLevel->setString("Lv"+TO_STR(mBuildInfo->level));
	m_ui.uiTexBuildLevel->setColor(Color3B(0xe8, 0xde, 0xb5));
	m_ui.uiTexBuildLevel->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexBuildLevel, Color3B(0x7d, 0x3f, 0x1c), 2);

}

void  MyGroupBuildLayer::iniJuanXian()
{
	sDonateLocalDT mDonateInfo;

	MyGroupModel::getInstance()->getDonateDataByType(DonateType::eDONATE_GENERAL_TYPE,&mDonateInfo);
	//普通
	m_ui.uiOrdinary.uiTexExp->setString("+"+TO_STR(mDonateInfo.addExp));
	m_ui.uiOrdinary.uiTexExp->setColor(Color3B(0xff, 0xee, 0x31));
	m_ui.uiOrdinary.uiTexExp->setFontName(FONT_FZZHENGHEI);

	m_ui.uiOrdinary.uiTexShengWang->setString("+"+TO_STR(mDonateInfo.prestige));
	m_ui.uiOrdinary.uiTexShengWang->setColor(Color3B(0xff, 0xee, 0x31));
	m_ui.uiOrdinary.uiTexShengWang->setFontName(FONT_FZZHENGHEI);

	m_ui.uiOrdinary.uiTexMoney->setString(TO_STR(mDonateInfo.costCoin ));
	m_ui.uiOrdinary.uiTexMoney->setColor(Color3B(0xff, 0xff, 0xff));
	m_ui.uiOrdinary.uiTexMoney->setFontName(FONT_FZZHENGHEI);

	MyGroupModel::getInstance()->getDonateDataByType(DonateType::eDONATE_ADVANCE_TYPE,&mDonateInfo);
	//高级
	m_ui.uiSenior.uiTexExp->setString("+"+TO_STR(mDonateInfo.addExp));
	m_ui.uiSenior.uiTexExp->setColor(Color3B(0xff, 0xee, 0x31));
	m_ui.uiSenior.uiTexExp->setFontName(FONT_FZZHENGHEI);

	m_ui.uiSenior.uiTexShengWang->setString("+"+TO_STR(mDonateInfo.prestige));
	m_ui.uiSenior.uiTexShengWang->setColor(Color3B(0xff, 0xee, 0x31));
	m_ui.uiSenior.uiTexShengWang->setFontName(FONT_FZZHENGHEI);

	m_ui.uiSenior.uiTexMoney->setString(TO_STR(mDonateInfo.costGold));
	m_ui.uiSenior.uiTexMoney->setColor(Color3B(0xff, 0xff, 0xff));
	m_ui.uiSenior.uiTexMoney->setFontName(FONT_FZZHENGHEI);

	MyGroupModel::getInstance()->getDonateDataByType(DonateType::eDONATE_RICH_TYPE,&mDonateInfo);
	//土豪
	m_ui.uiTuHao.uiTexExp->setString("+"+TO_STR(mDonateInfo.addExp));
	m_ui.uiTuHao.uiTexExp->setColor(Color3B(0xff, 0xee, 0x31));
	m_ui.uiTuHao.uiTexExp->setFontName(FONT_FZZHENGHEI);

	m_ui.uiTuHao.uiTexShengWang->setString("+"+TO_STR(mDonateInfo.prestige));
	m_ui.uiTuHao.uiTexShengWang->setColor(Color3B(0xff, 0xee, 0x31));
	m_ui.uiTuHao.uiTexShengWang->setFontName(FONT_FZZHENGHEI);

	m_ui.uiTuHao.uiTexMoney->setString(TO_STR(mDonateInfo.costGold));
	m_ui.uiTuHao.uiTexMoney->setColor(Color3B(0xff, 0xff, 0xff));
	m_ui.uiTuHao.uiTexMoney->setFontName(FONT_FZZHENGHEI);
}

void  MyGroupBuildLayer::onBtnClick( Ref* reff, Widget::TouchEventType type, BUILD_CALLBACK_TYPE mBtnCallBackType )
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		switch (mBtnCallBackType)
		{
		case BTN_BUILD_EXIT:
			onBtnExitCallBack();
			break;
		case BTN_COMMON:
			onBtnCommonCallBack();
			break;
		case BTN_HIGHTLEVEL:
			onBtnHoightCallBack();
			break;
		case BTN_MONEY:
			onBtnMoneyCallBack();
			break;
		default:
			break;
		}
	}
}


void  MyGroupBuildLayer::onBtnCommonCallBack()
{
	sDonateLocalDT mDonateInfo;
	isChaoZhou = false;
	MyGroupModel::getInstance()->getDonateDataByType(DonateType::eDONATE_GENERAL_TYPE,&mDonateInfo);

	int userCoin = MainModel::getInstance()->getMainParam()->mCoin;
	int needCoin = mDonateInfo.costCoin;
	int todayCount = MyGroupModel::getInstance()->getMyInfo()->myCurDonateTimes;
	int maxCount = MyGroupModel::getInstance()->getMyInfo()->myMaxDonateTimes;

	if (todayCount>=maxCount)
	{
		//CustomTips::show("今日捐献次数不足!");
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10182));
		return;
	}
	if (needCoin > userCoin)
	{
		//CustomTips::show("银币不足!");
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10183));
		return;
	}
	if (todayCount < maxCount &&needCoin <= mDonateInfo.costCoin)
	{
		MyGroupController::getInstance()->sendDonateBuildMsg(_buildType,eDonateType::eDONATE_GENERAL_TYPE);
	}

}

void  MyGroupBuildLayer::onBtnHoightCallBack()
{

	sDonateLocalDT mDonateInfo;
		isChaoZhou = false;
	MyGroupModel::getInstance()->getDonateDataByType(DonateType::eDONATE_ADVANCE_TYPE,&mDonateInfo);

	int usermGold = MainModel::getInstance()->getMainParam()->mGold;
	int needmGold = mDonateInfo.costGold;
	int todayCount = MyGroupModel::getInstance()->getMyInfo()->myCurDonateTimes;
	int maxCount = MyGroupModel::getInstance()->getMyInfo()->myMaxDonateTimes;

	if (todayCount>=maxCount)
	{
		//CustomTips::show("今日捐献次数不足!");
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10182));
		return;
	}
	if (usermGold < needmGold)
	{
		//CustomTips::show("金币不足!");
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10184));
		return;
	}
	if (todayCount<maxCount &&needmGold <= usermGold)
	{
		MyGroupController::getInstance()->sendDonateBuildMsg(_buildType,eDonateType::eDONATE_ADVANCE_TYPE);
	}

}

void  MyGroupBuildLayer::onBtnMoneyCallBack()
{
	sDonateLocalDT mDonateInfo;
		isChaoZhou = false;
	MyGroupModel::getInstance()->getDonateDataByType(DonateType::eDONATE_RICH_TYPE,&mDonateInfo);

	int usermGold = MainModel::getInstance()->getMainParam()->mGold;
	int needmGold = mDonateInfo.costGold;
	int todayCount = MyGroupModel::getInstance()->getMyInfo()->myCurDonateTimes;
	int maxCount = MyGroupModel::getInstance()->getMyInfo()->myMaxDonateTimes;

	if (todayCount>=maxCount)
	{
		//CustomTips::show("今日捐献次数不足!");
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10182));
		return;
	}
	if (usermGold < needmGold)
	{
		//CustomTips::show("金币不足!");
		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10184));
		return;
	}
	if (todayCount<maxCount &&needmGold <= usermGold)
	{
		MyGroupController::getInstance()->sendDonateBuildMsg(_buildType,eDonateType::eDONATE_RICH_TYPE);
	}

}

void MyGroupBuildLayer::onBtnExitCallBack()
{
	if (!isChaoZhou)
	{
		MyGroupController::getInstance()->sendGetGroupMainMsg();
	}
	this->dispose();
}

void MyGroupBuildLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	switch (observerParam->id)
	{
	case nMAIN_CMD_GROUP_BUILD_DONATE:
		{
			//CustomTips::show("捐献成功");
			CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10185));
			refreshBuildUIinfo();
		}
		break;
	default:
		break;
	}
}


void MyGroupBuildLayer::refreshBuildUIinfo()
{
	auto mBuildInfo = MyGroupModel::getInstance()->getBuildInfoByID(_buildType);

	m_ui.uiLoaBuildExp->setPercent((float)mBuildInfo->curExp/(float)mBuildInfo->nextExp*100);
	m_ui.uiTexBuildExp->setString(TO_STR(mBuildInfo->curExp)+"/"+TO_STR(mBuildInfo->nextExp));
	m_ui.uiTexBuildExp->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexBuildExp, Color3B(0x6a, 0x20, 0x11), 2);

	auto mGroupInfo = MyGroupModel::getInstance()->getMyGroupInfo();
	m_ui.uiTexTodayExp->setString(TO_STR(mGroupInfo->todayExp)+"/"+TO_STR(mGroupInfo->todayMaxExp));
	m_ui.uiTexTodayExp->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexTodayExp, Color3B(0x1a, 0x1a, 0x1f), 2);
	m_ui.uiLoaTodayExp->setPercent((float)mGroupInfo->todayExp/(float)mGroupInfo->todayMaxExp*100);

	m_ui.uiTexNextLevelEffeck->setString(mBuildInfo->nextEffect);
	m_ui.uiTexNextLevelEffeck->setColor(Color3B(0xff, 0xff, 0xff));
	m_ui.uiTexNextLevelEffeck->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexNextLevelEffeck, Color3B(0x5b, 0x14, 0x14), 2);

	m_ui.uiTexBuildName->setString(mBuildInfo->name);
	m_ui.uiTexBuildName->setColor(Color3B(0xe8, 0xde, 0xb5));
	m_ui.uiTexBuildName->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexBuildName, Color3B(0x7d, 0x3f, 0x1c), 2);

	m_ui.uiTexToDayJuanXian->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexToDayJuanXian, Color3B(0x7d, 0x3f, 0x1c), 2);

	auto facInfo = MyGroupModel::getInstance()->getMyInfo();
	if (!facInfo)
	{
		return;
	}
	m_ui.uiTexToDayCount->setString(TO_STR((facInfo->myMaxDonateTimes)-(facInfo->myCurDonateTimes))+"/"+TO_STR(facInfo->myMaxDonateTimes));
	if (facInfo->myCurDonateTimes < facInfo->myMaxDonateTimes)
	{
		m_ui.uiTexToDayCount->setColor(Color3B(0xff, 0xff, 0xff));
		m_ui.uiTexToDayCount->setFontName(FONT_FZZHENGHEI);
		//setTextAddStroke(m_ui.uiTexToDayCount, Color3B(0x20, 0x07, 0x05), 2);
	}
	else if (facInfo->myCurDonateTimes == facInfo->myMaxDonateTimes)
	{
		m_ui.uiTexToDayCount->setColor(Color3B(0xff, 0x36, 0x07));
		m_ui.uiTexToDayCount->setFontName(FONT_FZZHENGHEI);
		setTextAddStroke(m_ui.uiTexToDayCount, Color3B(0x20, 0x07, 0x05), 2);
	}


	m_ui.uiTexBuildLevel->setString("Lv"+TO_STR(mBuildInfo->level));
	m_ui.uiTexBuildLevel->setColor(Color3B(0xe8, 0xde, 0xb5));
	m_ui.uiTexBuildLevel->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(m_ui.uiTexBuildLevel, Color3B(0x7d, 0x3f, 0x1c), 2);


}
