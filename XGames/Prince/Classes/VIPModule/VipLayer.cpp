#include "VipLayer.h"
#include "Temp/CustomPop.h"
#include "VipModel.h"
#include "Communication/Command.h"
#include "Widget/LayerCommHeadInfo.h"
#include "MainModule/MainCityScene.h"
#include "Utils/StringFormat.h"
#include "ToolModule/UIToolHeader.h"
#include "Common/Common.h"
#include "Widget/QuantitySlideBar.h"
#include "Communication/EnvConfig.h"
#include "Utils/SystemUtils.h"
#include "NewAreaActivityModule/NewAreaActivityModel.h"

//#if (LOGIN_WITH_SDK == 1)

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "SDK/SDKComponent.h"
#include "SDK/u8sdk/U8SDKData.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
using namespace cocos2d;
#include "SDK/SDKComponent_ios.h"
#include "SDK/U8SDKData_ios.h"
#endif

#include "LoginModule/LoginModel.h"
#include "Common/LocalData.h"
#include "Common/LocalDef.h"
//#endif

#include "ReYunSDK/ReYunController.h"



#define PAY_BTN_TAG	23000

int VipLayer::m_curBuyVipLv = 0;
VipLayer::VipLayer()
{
	VipModel::getInstance()->addObserver(this);
	m_curVipLv = MainModel::getInstance()->getMainParam()->mVipLevel;
	m_curVipExp = MainModel::getInstance()->getMainParam()->mVipExp;
	m_curSelVipLv = m_curVipLv ? m_curVipLv : 0;			//默认vip0特权
	m_nGoldCount = MainModel::getInstance()->getMainParam()->mGold;
	m_sCurVipInfo = VipModel::getInstance()->getVipInfo(m_curVipLv);
	if(m_curVipLv < TOTAL_VIP_LV)
	{
		m_nextVipMaxExp = VipModel::getInstance()->getVipInfo(m_curVipLv+1).mVipMaxExp;
	}
	else
	{
		m_nextVipMaxExp = VipModel::getInstance()->getVipInfo(TOTAL_VIP_LV).mVipMaxExp;		//充值超过最高等级vip则最大经验值为当前最高等级vip所需经验
	}
	m_vecVipLvBtn.clear();
	m_vecBuyGoldBtn.clear();
	m_bIsBuyGiftMsg = false;
	m_ScrollBar = nullptr;
	//m_bIsEnterPayView = false;
	m_pnlType = VIP_VIEW;
	m_pnlShowVipLv = m_curVipLv;
	m_curProductId = -1;
	m_ScrollBar = nullptr;
	m_payOptionBar = nullptr;
}


VipLayer::~VipLayer(void)
{
	VipModel::getInstance()->removeObserver(this);
}

VipLayer* VipLayer::create( Panel_Type type, int vipLevel )
{
	VipLayer* _layer = new VipLayer();
	if (_layer->init(type, vipLevel))
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

void VipLayer::onEnter()
{
	Layer::onEnter();

	auto callBack = [this](float dt)->void
	{
		VipController::getInstance()->sendGetVipListMsg();
		this->getScheduler()->unschedule("VipLayer::sendMsg", this);
	};
	if(!VipModel::getInstance()->getIsInitted())
	{
		getScheduler()->schedule(callBack, this, 0, 0, 0, false, "VipLayer::sendMsg");
	}
}

void VipLayer::onExit()
{
	Layer::onExit();
}

bool VipLayer::init(Panel_Type type, int vipLevel)
{
	if (!Layer::init())
	{
		return false;
	}
	
	m_pnlType = type;
	if(vipLevel >= 0 && vipLevel <= TOTAL_VIP_LV)
	{
		m_pnlShowVipLv = vipLevel;
	}

	initUI();

	return true;
}

void VipLayer::initUI()
{
	// 加载VIP UI;
	string strFileName;
	strFileName.assign("Image/UIVip/UIVip.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	m_ui.pRoot->setBackGroundImage("Image/Bg/img_bg_quanping.jpg");
	this->addChild(m_ui.pRoot);

	//公用返回,待定
	auto backCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::ENDED)
		{
			SoundUtils::playSoundEffect("dianji");
			scheduleUpdate();
		}
	};

	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(backCall,HEAD_INFO_TYPE_VIP);
	this->addChild(_layerCommHeadInfo);

	//标题栏
	m_ui.imgVipLv =	(ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Image_vipLv"));
	m_ui.imgVipNextLv = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Image_vipNextLv"));
	m_ui.txtVipNextLv = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Label_vipNextLv"));
	m_ui.txtVipExpNum = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Label_vipExpNum"));
	m_ui.barVipExp = (LoadingBar*)(Helper::seekWidgetByName(m_ui.pRoot, "bar_vipExp"));
	m_ui.btnPay = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Button_chongzhi"));
	m_ui.btnPay->addTouchEventListener(CC_CALLBACK_2(VipLayer::onBtnClick, this, Btn_SwitchPrivilegeAndPay));

	//特权栏
	m_ui.pnlPrivilege = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_tequan"));
	m_ui.imgPrivilegeLv = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Image_tequanLv"));
	m_ui.imgPrivilegeShow = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Image_viptequandi"));
	m_ui.imgPrivilegeGift = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Image_giftShow"));
	m_ui.imgBought = (ImageView*)(Helper::seekWidgetByName(m_ui.pRoot, "Image_bought"));
	m_ui.txtOldPrice = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Label_yuanjia"));
	m_ui.txtPrice = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Label_tejia"));
	m_ui.txtVipDes = SimpleRichText::create();
	m_ui.btnBuyGift = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, "Button_buyGift"));
	m_ui.btnBuyGift->addTouchEventListener(CC_CALLBACK_2(VipLayer::onBtnClick, this, Btn_BuyGift));
	m_ui.scrollPrivilegeShow = (ui::ScrollView*)(Helper::seekWidgetByName(m_ui.pRoot, "Scroll_vipInfo"));
	m_ui.scrollPrivilegeShow->addEventListener(CC_CALLBACK_2(VipLayer::onScrollViewEvent, this));
	m_ui.scrollVipLvBtnShow = (ui::ScrollView*)(Helper::seekWidgetByName(m_ui.pRoot, "Scroll_vipLv"));
	m_ui.scrollVipLvBtnShow->addEventListener(CC_CALLBACK_2(VipLayer::onScrollViewEvent, this));

	//充值栏
	m_ui.pnlPay = dynamic_cast<Layout*>(m_ui.pRoot->getChildByName("Panel_pay"));	

	//vip特权信息栏滚动条
	m_ScrollBar = CustomScrollBar::create();
	m_ScrollBar->initScrollBar(m_ui.scrollPrivilegeShow->getContentSize().height);
	m_ScrollBar->setPosition(m_ui.scrollPrivilegeShow->getPosition() + Point(12, 32));
	m_ui.imgPrivilegeShow->addChild(m_ScrollBar);
	m_ScrollBar->setScrollBarLengthPercent(1.0f);
	m_ScrollBar->setScrollBarPercentY(0.0f);

	//初始化vip等级滚动层按钮
	initScrollVipLvBtn();

	//初始化标题栏界面
	initTitleView();

	//初始化特权详情界面
	initScrollPrivilegeDesView();

	//初始化充值界面
	initBuyGoldView();

	m_ui.pnlPay->setVisible(false);
	m_ui.pnlPrivilege->setVisible(false);

	if(VipModel::getInstance()->getIsInitted())
	{
		if(m_pnlType == PAY_VIEW && VipModel::getInstance()->getIsOpenPay())
		{
			m_ui.pnlPay->setVisible(true);
			updatePayBtnUIState();
			switchPayAndPrivilege(false);
		}
		else
		{
			m_ui.pnlPrivilege->setVisible(true);
			initBuyGiftView();
			switchPayAndPrivilege(true);
		}	
	}
}

void VipLayer::update(float dt)
{
	this->unscheduleUpdate();
	Director::getInstance()->popScene();
}

void VipLayer::updateCurVipTitle()
{
	m_curVipLv = VipModel::getInstance()->getCurVipLv();
	m_curVipExp = VipModel::getInstance()->getCurVipExp();
	if(m_curVipExp > VipModel::getInstance()->getVipInfo(TOTAL_VIP_LV).mVipMaxExp)
	{
		m_curVipExp = VipModel::getInstance()->getVipInfo(TOTAL_VIP_LV).mVipMaxExp;
	}
	m_nGoldCount = MainModel::getInstance()->getMainParam()->mGold;
	m_sCurVipInfo = VipModel::getInstance()->getVipInfo(m_curVipLv);
	if(m_curVipLv < TOTAL_VIP_LV)
	{
		m_nextVipMaxExp = VipModel::getInstance()->getVipInfo(m_curVipLv+1).mVipMaxExp;
	}
	else
	{
		m_nextVipMaxExp = VipModel::getInstance()->getVipInfo(TOTAL_VIP_LV).mVipMaxExp;		//充值超过最高等级vip则最大经验值为当前最高等级vip所需经验
	}
	initTitleView();
}

void VipLayer::switchPayAndPrivilege(bool isPay)
{
	if(isPay)
	{
		m_ui.btnPay->loadTextures("Image/UIVip/VipLv/btn_chongzhi_1.png", "Image/UIVip/VipLv/btn_chongzhi_2.png","", TextureResType::LOCAL);
	}
	else
	{
		m_ui.btnPay->loadTextures("Image/UIVip/VipLv/btn_tequan_1.png", "Image/UIVip/VipLv/btn_tequan_1.png","", TextureResType::LOCAL);
	}
}

void VipLayer::onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName )
{
	if (Widget::TouchEventType::ENDED == type)
	{
		SoundUtils::playSoundEffect("dianji");
		switch (nWidgetName)
		{
		case Btn_SwitchPrivilegeAndPay:
			{	
				if(!VipModel::getInstance()->getIsOpenPay() && !m_ui.pnlPay->isVisible())
				{
					//CustomPop::show("未开放充值功能");
					CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20318));
				}
				else
				{
					m_ui.pnlPay->setVisible(m_ui.pnlPay->isVisible() ? false : true);
					m_ui.pnlPrivilege->setVisible(m_ui.pnlPrivilege->isVisible() ? false : true);
					switchPayAndPrivilege(m_ui.pnlPay->isVisible() ? false : true);
					if(m_ui.pnlPrivilege->isVisible())		
					{
						updateCurVipTitle();
						//刷新已购买过的礼包状态
						initBuyGiftView();
					}
					else if(m_ui.pnlPay->isVisible())
					{
						updatePayBtnUIState();
					}
				}
			}
			break;
		case Btn_BuyGift:
			{
				//金币足够
				if(m_curVipLv >= m_curSelVipLv)
				{
					if(m_nGoldCount >= atoi(m_ui.txtPrice->getString().c_str()))
					{
						m_curBuyVipLv = m_curSelVipLv;
						VipModel::getInstance()->setCurBuyVipBoxId(m_curSelVipLv);
						VipController::getInstance()->sendGetVipBuyMsg(m_curSelVipLv);
						m_bIsBuyGiftMsg = true;
					}
					else
					{
						if(!VipModel::getInstance()->getIsOpenPay())
						{
							//CustomTips::show("金币不足");
							CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10007));
						}
						else
						{
							//吐司金币不足并跳转到充值界面					
							auto call = [this](Ref* ref, CustomRetType type)->void
							{
								if (type == RET_TYPE_OK)
								{
									m_ui.pnlPay->setVisible(true);
									m_ui.pnlPrivilege->setVisible(false);
									switchPayAndPrivilege(false);
									updatePayBtnUIState();
								}
							};
							//提示要不要充值
							CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20319), call, CUSTOM_YES_NO_CANCEL);
						}
					}
				}
				else                 //没达到购买所需的等级条件则吐司升级至对应的等级可以购买
				{
					string strVipTips = string("VIP") + _TO_STR(m_curSelVipLv) + DataManager::getInstance()->searchCommonTexdtById(20320);				
					CustomTips::showUtf8(strVipTips);								
				}
			}
			break;
		case Btn_VipLv:
			{
				Button* vipLvBtn = (Button*)ref;
				//更新按钮状态
				updateVipLvBtnStates(vipLvBtn->getTag());
			}
			break;
		case Btn_BuyGold:
			{
 				//#if (LOGIN_WITH_SDK == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
 				//	if(GetIntegerFromXML(LOCAL_KEY_STR_TOTAL_PAY_LOCAL) != GetIntegerFromXML(LOCAL_KEY_STR_TOTAL_PAY_SRV)
 				//		&& SystemUtils::getChannelId() == CHANNEL_ID_YSDK)
 				//	{
 				//		//CustomTips::show("上次充值结果未返回!");
 				//		CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20321));
 				//		return;
 				//	}
 				//#endif
				
				Button* buyGoldBtn = (Button*)ref;
				//处理调用支付sdk
				int goodsIndex = buyGoldBtn->getTag() - PAY_BTN_TAG -1;
				//商品Id
				vector<sRechargeData>& rechargeInfo = VipModel::getInstance()->getVecRechargeInfo();
				if(rechargeInfo.size() > 0 && goodsIndex < rechargeInfo.size())
				{
					int goodsId = rechargeInfo.at(goodsIndex).id;					
					if(rechargeInfo.at(goodsIndex).mark == 1 && VipModel::getInstance()->getMonthCardResDays() > 3 
						&& MainModel::getInstance()->getMonthCardIsOpen())
					{
						//如果是月卡，同时，月卡剩余天数 >3 天, 则不允许再买月卡
						//CustomTips::show("剩余时间大于3天不能购买!");
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20322));
						return;
					}
					else if(rechargeInfo.at(goodsIndex).mark == 2 && rechargeInfo.at(goodsIndex).rechargedFlag
						&& MainModel::getInstance()->getMonthCardIsOpen())
					{
						//如果是至尊卡，且已购买过，不允许购买
						//CustomTips::show("您已购买至尊卡!");
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20323));
						return;
					}

					m_curProductId = goodsId;
					VipController::getInstance()->sendGetOrderId(m_curProductId);
				}			
			}
			break;

		default:
			break;
		}
	}
}

//更新vip等级按钮状态
void VipLayer::updateVipLvBtnStates(int btnTag)
{
	//选中的按钮置高亮
	int btnSelIndex = btnTag - VIP_LV_BTN0_TAG;
	for(int i=0; i<m_vecVipLvBtn.size(); i++)
	{
		m_vecVipLvBtn.at(i)->setBrightStyle(BrightStyle::NORMAL);
	}
	m_vecVipLvBtn.at(btnSelIndex)->setBrightStyle(BrightStyle::HIGHLIGHT);

	m_curSelVipLv = btnSelIndex;

	//刷新特权信息
	initScrollPrivilegeDesView();

	//刷新特权礼包
	initBuyGiftView();
}

//初始化vip等级滚动层按钮
void VipLayer::initScrollVipLvBtn()
{
	string str;
	for(int i=0; i<=TOTAL_VIP_LV; i++)
	{
		str = string("Button_vip") + _TO_STR(i);
		Button* vipLvBtn = (Button*)(Helper::seekWidgetByName(m_ui.pRoot, str));
		m_vecVipLvBtn.push_back(vipLvBtn);
		vipLvBtn->addTouchEventListener(CC_CALLBACK_2(VipLayer::onBtnClick, this, Btn_VipLv));
	}

	//默认是当前vip所在的等级按钮高亮 或者 用户指定的vip等级
	m_curSelVipLv = m_pnlShowVipLv;
	m_vecVipLvBtn.at(m_curSelVipLv)->setBrightStyle(BrightStyle::HIGHLIGHT);

	//区域滚动到高亮的按钮所在
	float numerator = 0;
	float denominator = 9.0;
	if(m_curSelVipLv < 7)
	{
		numerator = 0;
	}
	else
	{
		numerator = float(m_curSelVipLv-6);
	}
	
	m_ui.scrollVipLvBtnShow->scrollToPercentHorizontal((numerator/denominator)*100, 0.01, false);
}

void VipLayer::initTitleView()
{
	string str;
	//当前vip等级
	str = string("Image/UIVip/VipLv/img_vip") + _TO_STR(m_curVipLv) + string(".png");
	m_ui.imgVipLv->loadTexture(str);

	//当前vip进度
	str = _TO_STR(m_curVipExp) + string("/") + _TO_STR(m_nextVipMaxExp);
	m_ui.txtVipExpNum->setString(str);
	m_ui.txtVipExpNum->setFontName(FONT_FZZHENGHEI);
	m_ui.txtVipExpNum->setColor(Color3B(0xfd, 0xff, 0x63));
	setTextAddStroke(m_ui.txtVipExpNum, Color3B(0x7e, 0x10, 0x10), 2);

	//再充值xxx升级到vipxxx
	m_ui.txtVipNextLv->setFontName(FONT_FZZHENGHEI);
	m_ui.txtVipNextLv->setFontSize(20);
	if(m_curVipExp <= VipModel::getInstance()->getVipInfo(TOTAL_VIP_LV).mVipMaxExp)
	{
		m_ui.txtVipNextLv->setString(_TO_STR(m_nextVipMaxExp-m_curVipExp));
		m_ui.barVipExp->setPercent(((float)m_curVipExp/(float)m_nextVipMaxExp)*100);		//进度条
	}
	else
	{
		m_ui.txtVipNextLv->setString(_TO_STR(0));			//当超过了最高vip等级的经验时，则显示再充值0到达最高级vip
		m_ui.barVipExp->setPercent(100.0f);					//进度条
	}
	setTextAddStroke(m_ui.txtVipNextLv, Color3B(0x54, 0x14, 0x00), 2);

	//"再充值", "升级到"描边
	Text* txtPay = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Label_zaichongzhi"));
	txtPay->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(txtPay, Color3B(0x54, 0x14, 0x00), 2);
	Text* txtUpdate = (Text*)(Helper::seekWidgetByName(m_ui.pRoot, "Label_shengji"));
	txtUpdate->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(txtUpdate, Color3B(0x54, 0x14, 0x00), 2);

	if(m_curVipLv < TOTAL_VIP_LV)
	{
		str = string("Image/UIVip/VipLv/img_small_vip") + _TO_STR(m_curVipLv+1) + string(".png");
	}
	else
	{
		str = string("Image/UIVip/VipLv/img_small_vip") + _TO_STR(TOTAL_VIP_LV) + string(".png");
	}
	
	m_ui.imgVipNextLv->loadTexture(str);
}

void VipLayer::initScrollPrivilegeDesView()
{
	//vip等级标题
	string str;
	str = string("Image/UIVip/VipLv/img_viptequan") + _TO_STR(m_curSelVipLv) + string(".png");
	m_ui.imgPrivilegeLv->loadTexture(str);
	//vip特权描述
	m_ui.scrollPrivilegeShow->jumpToTop();
	str = string(VipModel::getInstance()->getVipInfo(m_curSelVipLv).mVipSpecDetails.c_str());
	vector<string> lines;
	StringFormat::split(lines, str, "/n");
	if(!str.empty())
	{
		if (nullptr != m_ui.txtVipDes)
		{
			m_ui.txtVipDes->ignoreContentAdaptWithSize(false);
			m_ui.txtVipDes->setContentSize(Size(460.0f, 40.0f));
			m_ui.txtVipDes->setString(str, 26, Color3B::WHITE);
			int width = m_ui.scrollPrivilegeShow->getContentSize().width;
			int tempTeight = 36*lines.size();
			int height = tempTeight > m_ui.scrollPrivilegeShow->getContentSize().height ? 
				tempTeight : m_ui.scrollPrivilegeShow->getContentSize().height;
			m_ui.scrollPrivilegeShow->setInnerContainerSize(Size(width, height));
			m_ui.txtVipDes->setPosition(Point(m_ui.scrollPrivilegeShow->getContentSize().width*0.5f, 
				m_ui.scrollPrivilegeShow->getInnerContainerSize().height-38));
			float d = m_ui.scrollPrivilegeShow->getContentSize().height / m_ui.scrollPrivilegeShow->getInnerContainerSize().height;
			m_ScrollBar->setScrollBarLengthPercent(d);
			m_ScrollBar->setScrollBarPercentY(0.0f);
			m_ui.txtVipDes->setAnchorPoint(Point(0.5, 0.5));
			if(!m_ui.txtVipDes->getParent()) 
			{
				m_ui.scrollPrivilegeShow->addChild(m_ui.txtVipDes, 2);
			}		
		}	
	}
}

//特权礼包区
void VipLayer::initBuyGiftView()
{
	//更换成选中的vip等级对应的礼包图
	string str;
	sVipData selVipInfo = VipModel::getInstance()->getVipInfo(m_curSelVipLv);
	//先清空6个礼包容器内的道具
	for(int i=1; i<=6; i++)
	{
		str = string("GiftPanel_") + _TO_STR(i);
		Layout* pnlGift = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, str));
		pnlGift->removeAllChildren();			//先清除所有道具，否则叠层有黑边
		pnlGift->setVisible(false);
	}
	for(int i=0; i<selVipInfo.mVecVipSpecBox.size(); i++)
	{
		str = string("GiftPanel_") + _TO_STR(i+1);
		Layout* pnlGift = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, str));
		UIToolHeader* giftItem = UIToolHeader::create(selVipInfo.mVecVipSpecBox.at(i).mboxId);
		if(giftItem)
		{
			pnlGift->addChild(giftItem);
			giftItem->setAnchorPoint(Point(0, 0));
			giftItem->setNumEx(selVipInfo.mVecVipSpecBox.at(i).mCount);		//数量
			giftItem->setTipsEnabled(true);
			giftItem->setToolId(selVipInfo.mVecVipSpecBox.at(i).mboxId);
			giftItem->showLightEff();
			pnlGift->setVisible(true);
		}	
	}
	//如果当前选中的vip等级对应的特权礼包已经领取则显示已购买
	if(VipModel::getInstance()->getVipInfo(m_curSelVipLv).mIsTaked)
	{
		m_ui.imgBought->setVisible(true);
		m_ui.btnBuyGift->setVisible(false);
	}
	else
	{
		m_ui.imgBought->setVisible(false);
		m_ui.btnBuyGift->setVisible(true);
	}

	//达到购买特权礼包资格尚未购买礼包的vip等级按钮显示小红点
	vector<sVipData> vipInfo = VipModel::getInstance()->getVecVipInfo();
	for(int i=0; i<=m_curVipLv; i++)
	{
		if(!vipInfo.at(i).mIsTaked)
		{
			Node* buyFlag = cocos2d::ui::ImageView::create("Image/Icon/global/img_dian.png");
			buyFlag->setName("buy_flag");
			buyFlag->setPosition(cocos2d::Vec2(m_vecVipLvBtn.at(i)->getContentSize().width - buyFlag->getContentSize().width/2,
				m_vecVipLvBtn.at(i)->getContentSize().height - buyFlag->getContentSize().height/2));// 
			m_vecVipLvBtn.at(i)->addChild(buyFlag, 9);	
		}
		else
		{
			m_vecVipLvBtn.at(i)->removeAllChildren();
		}
	}

	//价格
	m_ui.txtOldPrice->setString(_TO_STR(VipModel::getInstance()->getVipInfo(m_curSelVipLv).mVipBoxOldPrice));
	m_ui.txtOldPrice->setFontName(FONT_FZZHENGHEI);
	m_ui.txtPrice->setString(_TO_STR(VipModel::getInstance()->getVipInfo(m_curSelVipLv).mVipBoxPrice));
	m_ui.txtPrice->setFontName(FONT_FZZHENGHEI);
}

void VipLayer::showVipReward()
{
	vector<PopRewardItem*> vecRewardItem;
	vecRewardItem.clear();

	bool isHaveHeroCard = false;
	int heroTempId = -1;

	vector<sVipSpecBox> buyGiftItemList = VipModel::getInstance()->getVipInfo(m_curBuyVipLv).mVecVipSpecBox;
	for(int i=0; i<buyGiftItemList.size(); i++)
	{		
		RowData* rowData = DataManager::getInstance()->searchToolById(buyGiftItemList.at(i).mboxId);
		if(rowData)
		{
			// 如果送的是英雄
			if(rowData->getIntData("itemTyp") == 4)
			{
				isHaveHeroCard = true;
				heroTempId = buyGiftItemList.at(i).mboxId;
				break;
			}
		}

		PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(buyGiftItemList.at(i).mboxId, buyGiftItemList.at(i).mCount);
		vecRewardItem.push_back(rewardItem);
	}

	if(isHaveHeroCard)
	{
		LayerGetCard *_layerGetCard = LayerGetCard::create(heroTempId, false,1);
		Director::getInstance()->getRunningScene()->addChild(_layerGetCard);
	}
	else
	{
		PopReward::show(vecRewardItem);	
	}
}

void VipLayer::initBuyGoldView()
{
	m_ui.vecGoldBtn.clear();

	//模板
	string str = string("Panel_rmb1");
	Layout* pnlPayBtn = (Layout*)(Helper::seekWidgetByName(m_ui.pRoot, str));
	
	//充值选项数目
	int options = VipModel::getInstance()->getVecRechargeInfo().size();

	//滚动层
	m_ui.scrollPay = (ui::ScrollView*)(Helper::seekWidgetByName(m_ui.pRoot, "Scroll_option"));
	m_ui.scrollPay->addEventListener(CC_CALLBACK_2(VipLayer::onScrollViewEvent, this));
	
	//充值额度选择滚动条
	ImageView* vipOptionBg = (ImageView*)m_ui.pnlPay->getChildByName("Image_di5");
	m_payOptionBar = CustomScrollBar::create();
	m_payOptionBar->initScrollBar(m_ui.scrollPay->getContentSize().height);
	m_payOptionBar->setPosition(Point(16, 28));
	vipOptionBg->addChild(m_payOptionBar, 10);
	m_payOptionBar->setScrollBarLengthPercent(1.0f);
	m_payOptionBar->setScrollBarPercentY(0.0f);
	m_payOptionBar->setVisible(true);
	m_ui.scrollPay->setBounceEnabled(true);
	
	float height = options <= 8 ? m_ui.scrollPay->getContentSize().height : (options/4 + (options%4 != 0 ? 1 : 0))*pnlPayBtn->getContentSize().height;
	float width = m_ui.scrollPay->getContentSize().width;
	
	m_ui.scrollPay->setInnerContainerSize(Size(width, height));
	m_ui.scrollPay->jumpToTop();
	//如果是<=8个，不显示滑动条，同时不能滑动
	if(options <= 8)
	{
		m_payOptionBar->setVisible(false);
		m_ui.scrollPay->setBounceEnabled(false);
	}

	pnlPayBtn->setPositionY(m_ui.scrollPay->getInnerContainerSize().height - pnlPayBtn->getContentSize().height);
	float d = m_ui.scrollPay->getContentSize().height / m_ui.scrollPay->getInnerContainerSize().height;
	m_payOptionBar->setScrollBarLengthPercent(d);
	m_payOptionBar->setScrollBarPercentY(0.0f);

	for(int i=0; i<options; i++)
	{
		Layout* optionItem = nullptr;
		if(i == 0)
		{
			optionItem = pnlPayBtn;
		}
		else
		{
			optionItem = (Layout*)pnlPayBtn->clone();
		}

		Button* btnGold = (Button*)optionItem->getChildByName("Button_rmb");
		btnGold->setTag(PAY_BTN_TAG + i + 1);
		btnGold->addTouchEventListener(CC_CALLBACK_2(VipLayer::onBtnClick, this, Btn_BuyGold));
		Text* txtBackTips = (Text*)Helper::seekWidgetByName(btnGold, "Label_discount");					
		Text* txtTips = (Text*)Helper::seekWidgetByName(btnGold, "Label_annotation");
		Text* txtDays = (Text*)Helper::seekWidgetByName(btnGold, "Label_days");
		Text* txtGoldNum = (Text*)Helper::seekWidgetByName(btnGold, "Label_gold_num");

		txtBackTips->setFontName(FONT_FZZHENGHEI);
		txtTips->setFontName(FONT_FZZHENGHEI);
		txtDays->setFontName(FONT_FZZHENGHEI);
		txtGoldNum->setFontName(FONT_FZZHENGHEI);

		if(i != 0)
		{	
			switch (i%4)
			{
			case 0:
				optionItem->setPosition(Point(pnlPayBtn->getPositionX(), 
					pnlPayBtn->getPositionY() - i/4*pnlPayBtn->getContentSize().height));
				break;
			case 1:
				optionItem->setPosition(Point(pnlPayBtn->getPositionX() + pnlPayBtn->getContentSize().width, 
					pnlPayBtn->getPositionY() - i/4*pnlPayBtn->getContentSize().height));
				break;
			case 2:
				optionItem->setPosition(Point(pnlPayBtn->getPositionX() + 2*pnlPayBtn->getContentSize().width, 
					pnlPayBtn->getPositionY() - i/4*pnlPayBtn->getContentSize().height));
				break;
			case 3:
				optionItem->setPosition(Point(pnlPayBtn->getPositionX() + 3*pnlPayBtn->getContentSize().width, 
					pnlPayBtn->getPositionY() - i/4*pnlPayBtn->getContentSize().height));
				break;

			default:
				break;
			}
		
			m_ui.scrollPay->addChild(optionItem);
		}
		
		m_ui.vecGoldBtn.push_back(optionItem);
	}
}

void VipLayer::onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type)
{
	if(pSender == m_ui.scrollPrivilegeShow)
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.scrollPrivilegeShow->getInnerContainerSize();
				Vec2 vec = m_ui.scrollPrivilegeShow->getInnerContainer()->getPosition();
								
				//滚动条重设进度
				float percentY = -vec.y / (size.height - m_ui.scrollPrivilegeShow->getContentSize().height);
				m_ScrollBar->setScrollBarPercentY((1.0f - percentY)*100.0f);			
			}
			break;
		
		default:
			break;
		}
	}
	else if(pSender == m_ui.scrollVipLvBtnShow)
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				//解决滚动时选中按钮状态会弹起的bug
				m_vecVipLvBtn.at(m_curSelVipLv)->setBrightStyle(BrightStyle::HIGHLIGHT);
			}
			break;

		default:
			break;
		}
	}
	else if(pSender == m_ui.scrollPay)
	{
		switch (type)
		{
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.scrollPay->getInnerContainerSize();
				Vec2 vec = m_ui.scrollPay->getInnerContainer()->getPosition();

				//滚动条重设进度
				float percentY = -vec.y / (size.height - m_ui.scrollPay->getContentSize().height);
				m_payOptionBar->setScrollBarPercentY((1.0f - percentY)*100.0f);	
			}
			break;

		default:
			break;
		}
	}
}

//test
void VipLayer::testPay(int goodsId)
{
	//充值界面数据更新
	int payId = goodsId;
	//VipModel::getInstance()->setMonthCardOutTime(data["monthcard"].asUInt());

	//time_t monthCardTime = VipModel::getInstance()->getMonthCardOutTime();
	//time_t curServerTime = LocalTimer::getInstance()->getCurServerTime();

	//VipModel::getInstance()->setEndTimeStamp(monthCardTime);	
	//if(monthCardTime > curServerTime)
	//{
		//VipModel::getInstance()->setMonthCardResDays((monthCardTime - curServerTime)/24*3600 + 1);
	//}
	//else
	//{
		VipModel::getInstance()->setMonthCardResDays(26);
	//}
	vector<sRechargeData>& rechargeInfo = VipModel::getInstance()->getVecRechargeInfo();
	for(auto& item : rechargeInfo)
	{
		if(item.id == payId)
		{
			item.rechargedFlag = true;
			if(item.mark == 1)
			{
				//月卡
				//OperateActModel::getInstance()->setBuyMonthCard();
			}
			else if(item.mark == 2)
			{
				//至尊卡
				//OperateActModel::getInstance()->setBuyExtremeCard();
			}
			break;
		}
	}

	ObserverParam obParam;
	obParam.id = nMAIN_CMD_PAY_RESULT;
	VipModel::getInstance()->notifyObservers(&obParam);
}

void VipLayer::updatePayBtnUIState()
{
	vector<sRechargeData>& rechargeInfo = VipModel::getInstance()->getVecRechargeInfo();
	
	for(int i=0; i<m_ui.vecGoldBtn.size() && i<rechargeInfo.size(); i++)
	{
		CCLOG("渠道号 = %d", rechargeInfo.at(i).channel);
		if(rechargeInfo.at(i).channel != SystemUtils::getChannelId())
		{
			continue;
		}

		Button* btnGold = (Button*)m_ui.vecGoldBtn.at(i)->getChildByName("Button_rmb");
		ImageView* iconBg = (ImageView*)(Helper::seekWidgetByName(btnGold, "Image_gold_bg"));
		TextAtlas* atlasPrice = (TextAtlas*)(Helper::seekWidgetByName(btnGold, "AtlasLabel_rmb"));
		ImageView* backTipsBg = (ImageView*)(Helper::seekWidgetByName(btnGold, "Image_discount_bg"));
		Text* txtBackTips = (Text*)(Helper::seekWidgetByName(btnGold, "Label_discount"));
		Text* txtTips = (Text*)(Helper::seekWidgetByName(btnGold, "Label_annotation"));						
		ImageView* cornerIcon = (ImageView*)(Helper::seekWidgetByName(btnGold, "Image_flag"));
		ImageView* cornerDouble = (ImageView*)(Helper::seekWidgetByName(btnGold, "Image_double"));
		Text* txtDays = (Text*)(Helper::seekWidgetByName(btnGold, "Label_days"));					
		Layout* pnlGold = (Layout*)(Helper::seekWidgetByName(btnGold, "Pnl_goldNum"));		
		Text* txtGoldNum = (Text*)(Helper::seekWidgetByName(btnGold, "Label_gold_num"));

		backTipsBg->setVisible(rechargeInfo.at(i).mark != 0);		
		txtDays->setVisible(false);
		txtTips->setVisible(false);
		cornerIcon->setVisible(false);
		cornerDouble->setVisible(false);
		//mark = 1,角标=月卡； 2，至尊卡；3, 双倍；0，不显示
		if(rechargeInfo.at(i).mark == 1 || rechargeInfo.at(i).mark == 2)
		{
			if(rechargeInfo.at(i).mark == 1)
			{
				iconBg->loadTexture("Image/UIVip/rechargeIcon/" + TO_STR(rechargeInfo.at(i).resId) + ".png");
				int monthCardDays = VipModel::getInstance()->getMonthCardResDays();
				if(monthCardDays >= 0)
				{
					cornerIcon->setVisible(true);
					if(monthCardDays > 0)
					{
						txtDays->setVisible(true);
						txtDays->setString(_TO_STR(monthCardDays));
						setTextAddStroke(txtDays, Color3B(0x14, 0x01, 0x01), 2);
						cornerIcon->loadTexture("Image/UIVip/rechargeIcon/img_shengyu.png");
					}
					else
					{
						txtDays->setVisible(false);						
						cornerIcon->loadTexture("Image/UIVip/rechargeIcon/img_jinridaoqi.png");
					}				
				}		
			}
			else
			{
				iconBg->loadTexture("Image/UIVip/rechargeIcon/" + TO_STR(rechargeInfo.at(i).resId) + ".png");
				if(rechargeInfo.at(i).rechargedFlag)
				{
					cornerIcon->setVisible(true);
					cornerIcon->loadTexture("Image/UIVip/rechargeIcon/img_yongjiu.png");
				}			
			}			
			
			
			setTextAddStroke(txtBackTips, Color3B(0x20, 0x12, 0x77), 2);
			txtTips->setVisible(true);
			txtTips->setString(rechargeInfo.at(i).des);
			setTextAddStroke(txtTips, Color3B(0x5b, 0x08, 0x08), 2);
			pnlGold->setVisible(false);
		}
		else
		{
			cornerIcon->setVisible(false);
			backTipsBg->setVisible(false);
			if(rechargeInfo.at(i).mark == 3 && !rechargeInfo.at(i).rechargedFlag)
			{
				cornerDouble->setVisible(true);
				backTipsBg->setVisible(true);
			}
			iconBg->loadTexture("Image/UIVip/rechargeIcon/" + TO_STR(rechargeInfo.at(i).resId) + ".png");
			txtGoldNum->setString(_TO_STR(rechargeInfo.at(i).payGold));
			setTextAddStroke(txtGoldNum, Color3B(0x5b, 0x08, 0x08), 3);
			
		}
		txtBackTips->setString(rechargeInfo.at(i).markDes);
		setTextAddStroke(txtBackTips, Color3B(0x20, 0x12, 0x77), 2);
		atlasPrice->setString(_TO_STR(rechargeInfo.at(i).money));
	}
}

void VipLayer::reflushMonthCardTime()
{
	vector<sRechargeData>& rechargeInfo = VipModel::getInstance()->getVecRechargeInfo();
	for(int i=0; i<rechargeInfo.size(); i++)
	{
		if(rechargeInfo.at(i).mark == 1)
		{
			Button* btnGold = (Button*)m_ui.vecGoldBtn.at(i)->getChildByName("Button_rmb");				
			ImageView* cornerIcon = (ImageView*)(Helper::seekWidgetByName(btnGold, "Image_flag"));
			Text* txtDays = (Text*)(Helper::seekWidgetByName(btnGold, "Label_days"));						

			//月卡
			if(VipModel::getInstance()->getMonthCardResDays() > 0)
			{
				cornerIcon->setVisible(true);
				cornerIcon->loadTexture("Image/UIVip/rechargeIcon/img_shengyu.png");
				txtDays->setVisible(true);
				txtDays->setString(_TO_STR(VipModel::getInstance()->getMonthCardResDays()));
			}
			else if(VipModel::getInstance()->getMonthCardResDays() == 0)
			{
				cornerIcon->setVisible(true);
				txtDays->setVisible(false);
				cornerIcon->loadTexture("Image/UIVip/rechargeIcon/img_jinridaoqi.png");
			}
			else
			{
				cornerIcon->setVisible(false);
			}
			break;
		}
	}
}

//充值，临时
void VipLayer::payMoney(int goodsId,OrderInfo orderInfo)
{
#if (LOGIN_WITH_SDK == 1 || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

	U8PayParams param;
	int nId = goodsId;
    
    ///////////////////// test; /////////////////////
    //if (nId == 1001006 || nId == 1001008) {
    //    nId *= 1000;
    //}
    ///////////////////// test end; /////////////////
    
	param.productId = _TO_STR(nId);

	RowData* _row = DataManager::getInstance()->searchPayById(nId);
	if (_row != nullptr)
	{
		param.productName = _row->getStringData("name");
		param.productDesc = _row->getStringData("des");
		param.price = _row->getIntData("money");
	}
	param.buyNum = 1;
	param.coinNum = MainModel::getInstance()->getMainParam()->mGold;

	param.serverId = LoginModel::getInstance()->getServerParam()->m_strServerID;
	param.serverName = LoginModel::getInstance()->getServerParam()->m_strServerName;

	param.roleId = _TO_STR(MainModel::getInstance()->getMainParam()->mRoleId);
	param.roleName = MainModel::getInstance()->getMainParam()->mName;
	param.roleLevel = HeroModel::getInstance()->getMainHeroLevel();
	param.vip = _TO_STR(MainModel::getInstance()->getMainParam()->mVipLevel);
	param.orderID = orderInfo.orderId;
	param.extension = orderInfo.extension;

	CCLOG("VipLayer:: param.productId = %s", param.productId.c_str());
	CCLOG("VipLayer:: param.productName = %s", param.productName.c_str());
	CCLOG("VipLayer:: param.productDesc = %s", param.productDesc.c_str());
	CCLOG("VipLayer:: param.price = %d", param.price);
	CCLOG("VipLayer:: param.buyNum = %d", param.buyNum);
	CCLOG("VipLayer:: param.coinNum = %d", param.coinNum);
	CCLOG("VipLayer:: param.serverId = %s", param.serverId.c_str());
	CCLOG("VipLayer:: param.serverName = %s", param.serverName.c_str());
	CCLOG("VipLayer:: param.roleId = %s", param.roleId.c_str());
	CCLOG("VipLayer:: param.roleName = %s", param.roleName.c_str());
	CCLOG("VipLayer:: param.roleLevel = %d", param.roleLevel);
	CCLOG("VipLayer:: param.vip = %s", param.vip.c_str());
	CCLOG("VipLayer:: param.orderID = %s", param.orderID.c_str());
	CCLOG("VipLayer:: param.extension = %s", param.extension.c_str());

	//记录当前要充值的额度
	VipModel::getInstance()->setCurPayRmb(param.price);
    
    //客户端不上报,充值改为服务端上报
    //ReyunController::ReYunPay(param.orderID, PAY_TYPE::APPSTORE, MONEY_TYPE_REYUN::REYUN_MONEYTYPE_CNY, (float)(param.price));

    
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	SDKComponent::getInstance()->pay(&param);
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    SDKComponent_ios::getInstance()->pay(&param);
#endif

#else

	int nId = goodsId;
	RowData* _row = DataManager::getInstance()->searchPayById(nId);
	if (_row != nullptr)
	{
		ostringstream oss;
		oss << DataManager::getInstance()->searchCommonTexdtById(20324) 
			<< _row->getStringData("name") << "(" << _row->getIntData("money") << ")";
		CustomTips::showUtf8(oss.str());
	}

#endif
}

void VipLayer::updateSelf(void* data)
{
	ObserverParam* obParam = (ObserverParam*) data;

	switch (obParam->id)
	{
	case nMAIN_CMD_VIP_BUY_GIFT:
		{		
			if(m_bIsBuyGiftMsg)
			{
				//显示获得的奖励
				showVipReward();
			}
			
			//刷新特权礼包栏
			initBuyGiftView();
		}
		break;
	case nMAIN_CMD_VIP_GET_LIST:
		{
			updateCurVipTitle();

			if(m_pnlType == PAY_VIEW && VipModel::getInstance()->getIsOpenPay())
			{
				m_ui.pnlPay->setVisible(true);
				switchPayAndPrivilege(false);
				updatePayBtnUIState();
			}
			else
			{
				m_ui.pnlPrivilege->setVisible(true);
				initBuyGiftView();
				switchPayAndPrivilege(true);
			}
		}
		break;
	case MONTH_CARD_RES_DAYS:
		{
			//刷新月卡剩余天数
			reflushMonthCardTime();
		}
		break;
	case nMAIN_CMD_PAY_RESULT:
		{
			updateCurVipTitle();
			if(m_ui.pnlPay->isVisible())
			{
				updatePayBtnUIState();
			}
			else
			{
				initBuyGiftView();
			}

			//刷新月卡剩余天数
			reflushMonthCardTime();
		}	
		break;
	case nMAIN_CMD_VIP_GET_ORDERID:
		{
			OrderInfo orderInfo;
			orderInfo = *((OrderInfo*)obParam->updateData);

			payMoney(m_curProductId,orderInfo);					
		}
		break;
	default:
		break;
	}
}


