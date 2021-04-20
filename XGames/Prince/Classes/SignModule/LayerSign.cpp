#include "LayerSign.h"
#include "LayerSignModel.h"
#include "Common/Common.h"
#include "Widget/LayerGameRules.h"
#include "Temp/CustomGoldTips.h"
#include "SDK_TalkingData/TalkingData.h"
#include "Widget/LayerGetCard.h"

LayerSign::LayerSign(void):
m_customScrollBar1(nullptr),
m_customScrollBar2(nullptr),
m_customScrollBar3(nullptr),
m_signCount(0),
m_signMonth(0),
m_canSign(0)
{
}


LayerSign::~LayerSign(void)
{
}


void LayerSign::onEnter()
{
	ModalLayer::onEnter();

	LayerSignModel::getInstance()->addObserver(this);
	MainModel::getInstance()->addObserver(this);
}

void LayerSign::onExit()
{
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UISign");

	ModalLayer::onExit();

	LayerSignModel::getInstance()->removeObserver(this);
	MainModel::getInstance()->removeObserver(this);
}


bool LayerSign::init()
{
	if (!ModalLayer::init())
	{
		return false;
	}
	initUI();
	initScrollView();

	time_t& mLastRecordTime = LayerSignModel::getInstance()->getServerTime();
	time_t mCurServerTime = LocalTimer::getInstance()->getCurServerTime();
	tm formatLastTime = { 0 };
	tm formatCurTime = { 0 };
	formatLastTime = *(localtime(&mLastRecordTime));
	formatCurTime = *(localtime(&mCurServerTime));
	//判断天数是否发生变化, 如果发生变化，说明跨天了,重新拉取签到消息
	if (formatCurTime.tm_mday != formatLastTime.tm_mday)
	{
		LayerSignController::getInstance()->sendGetSignList();
	}
	mLastRecordTime = LocalTimer::getInstance()->getCurServerTime();

	return true;
}

void LayerSign::initUI()
{
	// 加载Sign UI;
	string strFileName;
	strFileName.assign("Image/UISign/UISign.ExportJson");

	// Root;
	m_ui.pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	this->addChild(m_ui.pRoot);

	ImageView *Image_di = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Image_di"));
	ActionCreator::runCommDlgAction(Image_di);

	//关闭按钮
	m_ui.btnClose = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Button_Close"));
	m_ui.btnClose->addTouchEventListener(CC_CALLBACK_2(LayerSign::onBtnClick,this,Btn_Close));
	//帮助按钮
	m_ui.btnHelp = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.pRoot,"Button_Help"));
	m_ui.btnHelp->addTouchEventListener(CC_CALLBACK_2(LayerSign::onBtnClick,this,Btn_Help));

	//月份图片
	m_ui.imgMonth = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"img_month"));
	//签到次数图片
	m_ui.signCount = dynamic_cast<TextAtlas*>(Helper::seekWidgetByName(m_ui.pRoot,"sign_count"));

	//奖励物品栏
	m_ui.scrollView = dynamic_cast<ScrollView*>(Helper::seekWidgetByName(m_ui.pRoot,"ScrollView_Sign"));

	ImageView* Img_Hero = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Img_Hero"));
	auto call = [this](Ref* ref, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			//Director::getInstance()->replaceScene(SceneAllHero::create());
		}
	};
	Img_Hero->addTouchEventListener(call);
	m_ui.hero = dynamic_cast<Layout*>(Helper::seekWidgetByName(Img_Hero,"Pnl_Hero")); //本月签到英雄
	m_ui.signHero = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_Hero,"Lab_Name")); //本月签到英雄
	m_ui.canotGet = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Img_Hero,"Img_Recruit")); //不能招募
	m_ui.canGet = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Img_Hero,"Img_Get")); //能招募
	m_ui.gettEd = dynamic_cast<ImageView*>(Helper::seekWidgetByName(Img_Hero,"Img_Have")); //已能招募
	m_ui.tips = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_Hero,"Lab_Tips")); //提示
	m_ui.tipsCount = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_Hero,"Lab_TipsGet")); //提示个数
	Text *Lab_TipsGet_0 = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_Hero,"Lab_TipsGet_0"));
	m_ui.curCount = dynamic_cast<Text*>(Helper::seekWidgetByName(Img_Hero,"Lab_Count"));
	m_ui.signHero->setFontName(FONT_FZZHENGHEI);
	m_ui.tips->setFontName(FONT_FZZHENGHEI);
	m_ui.tipsCount->setFontName(FONT_FZZHENGHEI);
	m_ui.curCount->setFontName(FONT_FZZHENGHEI);
	Lab_TipsGet_0->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Lab_TipsGet_0,Color3B(0x00,0x00,0x00),2);

	ImageView* parent1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.pRoot,"Image_Bg"));

	//滚动条
	m_customScrollBar1 = CustomScrollBar::create();
	m_customScrollBar1->initScrollBar(m_ui.scrollView->getContentSize().height);
	m_customScrollBar1->setPosition(m_ui.scrollView->getPosition() + Point(-12, 0));
	parent1->addChild(m_customScrollBar1);

	m_customScrollBar1->setScrollBarLengthPercent(1.0f);
	m_customScrollBar1->setScrollBarPercentY(0.0f);
	m_ui.scrollView->addEventListener(CC_CALLBACK_2(LayerSign::onScrollViewEvent, this));
}


void LayerSign::onScrollViewEvent(Ref* pSender, ScrollView::EventType type)
{
	if (pSender == m_ui.scrollView)
	{
		switch(type)
		{
		
		case cocos2d::ui::ScrollView::EventType::SCROLLING:
			{
				Size size = m_ui.scrollView->getInnerContainerSize();
				Vec2 vec = m_ui.scrollView->getInnerContainer()->getPosition();
				float percentY = 0;

				if (size.height == m_ui.scrollView->getCustomSize().height)
				{
					percentY = 0;
				}
				else
				{
					percentY = -vec.y / (size.height - m_ui.scrollView->getContentSize().height);
				}

				m_customScrollBar1->setScrollBarPercentY((1.0f - percentY)*100.0f);
			}
			
			break;
		default:
			break;
		}
	}
}


void LayerSign::initScrollView()
{
	//服务器下拉数据
	SignRewardStateInfo state = LayerSignModel::getInstance()->getSignRewardStateInfo();
	m_signMonth = state.month;
	m_signCount = state.day;

	if (m_signMonth <= 0)
	{
		LayerSignController::getInstance()->sendGetSignList();
		return;
	}
	string monthStr = "Image/Icon/sign/img_month_"+_TO_STR(m_signMonth) + ".png";
	m_ui.imgMonth->loadTexture(monthStr);
	m_ui.signCount->setString(_TO_STR(m_signCount));

	//初始化奖励面板节点
	setSignPanelList();
	setGetHeroInfo();
}

void LayerSign::setGetHeroInfo()
{
	int showHeroAmount = LayerSignModel::getInstance()->getShowHeroCount();
	int showHeroId = LayerSignModel::getInstance()->getShowHeroId();
	RowData* heroData = DataManager::getInstance()->searchToolById(showHeroId);
	m_ui.signHero->setString(heroData->getStringData("itemName"));
	setTextAddStroke(m_ui.signHero,Color3B(0x82,0x2b,0x2b),2);
	m_ui.tipsCount->setString(TO_STR(showHeroAmount));
	setTextAddStroke(m_ui.tipsCount,Color3B(0x00,0x00,0x00),2);


	m_ui.canotGet->setVisible(false);
	m_ui.canGet->setVisible(false);
	m_ui.gettEd->setVisible(false);
	HeroParam* heroParam  = HeroModel::getInstance()->searchHeroByTemplateId(showHeroId);
	if (heroParam)
	{
		m_ui.gettEd->setVisible(true);
	}
	else
	{
		int mDebrisNum = ToolModel::getInstance()->searchHeroDebrisNum(showHeroId);
		int mDebrisMaxNum = ToolModel::getInstance()->searchHeroDebrisMaxNum(showHeroId);
		if (mDebrisNum >= mDebrisMaxNum)
		{
			m_ui.canGet->setVisible(true);
		}
		else
		{
			m_ui.canotGet->setVisible(true);
			m_ui.curCount->setString("(" + TO_STR(mDebrisNum) + "/" + TO_STR(mDebrisMaxNum) + ")");
			setTextAddStroke(m_ui.curCount,Color3B(0x37,0x05,0x02),2);
		}
	}

	if (!m_ui.hero->getChildByName("pSkelFighter"))
	{
		int nResID = heroData->getIntData("resId");
		SpineAnimation* pSkelFighter = SpineAnimation::createFighterWithResId(nResID);
		pSkelFighter->setPosition(Vec2(m_ui.hero->getContentSize().width/2, 0));
		pSkelFighter->setAnimation(0, "wait", true);
		m_ui.hero->addChild(pSkelFighter, 1,"pSkelFighter");
	}
}

void LayerSign::setSignPanelList()
{
	//添加奖励面板控件
	Vector<LayerSignParam*>* layerSignParamList = LayerSignModel::getInstance()->getSignParam();
	int row = ceil(layerSignParamList->size()/5.0f);
	int scrollViewHeight = row * 125+10;
	scrollViewHeight = scrollViewHeight >  m_ui.scrollView->getContentSize().height ? scrollViewHeight : m_ui.scrollView->getContentSize().height;
	int scrollViewWidth = m_ui.scrollView->getInnerContainerSize().width;
	m_ui.scrollView->setInnerContainerSize( Size(scrollViewWidth, scrollViewHeight) );

	m_ui.scrollView->removeAllChildren();
	int siIdx = 0;
	for (int i = 0; i < layerSignParamList->size(); i++)
	{
		int rowNumber = i/5;
		int colNumber = i%5;

		UIToolHeader* icon = NULL;
		//奖励是普通物品
		LayerSignParam* SignParam = layerSignParamList->at(i);
		if (SignParam->signReward.mItemId > 0)
		{
			icon = UIToolHeader::create(SignParam->signReward.mItemId);
			icon->setNumEx(SignParam->signReward.mItemCount);
			icon->showLightEff();
			icon->setTouchEnabled(true);
		}

		//如果vip多倍
		ImageView* MaskIconMask = NULL;
		if (SignParam->vipDouble > 0)
		{
			//vip底图
			ImageView* MaskIcon = ImageView::create("Image/Icon/sign/img_sign_di.png");
			MaskIcon->setRotation(45);
			MaskIcon->setPosition(cocos2d::Vec2(icon->getContentSize().width*3/4, icon->getContentSize().height*3/4));
			icon->addChild(MaskIcon, 5);

			//vip底图遮罩
			MaskIconMask = ImageView::create("Image/Icon/sign/img_sign_di_zhezhao.png");
			MaskIconMask->setRotation(45);
			MaskIconMask->setPosition(cocos2d::Vec2(icon->getContentSize().width*3/4, icon->getContentSize().height*3/4));
			icon->addChild(MaskIconMask, 6,"MaskIconMask");
			MaskIconMask->setVisible(false);

			//vip数字和倍数
			string vipStr = "Image/Icon/sign/img_sign_v"+_TO_STR(SignParam->vipLevel) + ".png";
			ImageView* vip = ImageView::create(vipStr);
			vip->setAnchorPoint(Vec2(1,0.5));
			vip->setPosition(cocos2d::Vec2(MaskIcon->getContentSize().width/2+5, MaskIcon->getContentSize().height/2));
			MaskIcon->addChild(vip, 2);

			string doubleStr = "Image/Icon/sign/img_bet_"+_TO_STR(SignParam->rewardBet) + ".png";
			ImageView* doubleCount = ImageView::create(doubleStr);
			doubleCount->setAnchorPoint(Vec2(0,0.5));
			doubleCount->setPosition(cocos2d::Vec2(MaskIcon->getContentSize().width/2-2, MaskIcon->getContentSize().height/2));
			MaskIcon->addChild(doubleCount, 2);			
		}

		//如果已经领取
		switch (SignParam->stat)
		{
		case SIGN_STAT_TYPE_CAN: //可领取
			{
				ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniZhuangbeiEffe/AniZhuangbeiEffe.ExportJson");
				Armature* pArmature = Armature::create("AniZhuangbeiEffe");//名字需要和 .ExportJson一致

				pArmature->getAnimation()->playByIndex(0,-1,1);
				//设置动画精灵位置 
				pArmature->setPosition(Vec2(icon->getContentSize().width/2,icon->getContentSize().height/2));
				icon->addChild(pArmature,14, "AniZhuangbeiEffe");

				siIdx = rowNumber;
			}
			break;
		case SIGN_STAT_TYPE_DID: //已领取
			{
				icon->setMaskFlag(true);
				if (MaskIconMask)
				{
					MaskIconMask->setVisible(true);
				}
				icon->setToolId(SignParam->signReward.mItemId);
				icon->setTipsEnabled(true);
			}
			break;
		case SIGN_STAT_TYPE_BU: //可补签
			{
				ImageView* img = ImageView::create("Image/Icon/global/img_buqian.png");
				img->setPosition(Vec2(icon->getContentSize().width/2,icon->getContentSize().height/2));
				icon->addChild(img,5,"buqian");
			}
			break;
		case SIGN_STAT_TYPE_VIP: //vip达到可领状态
			{
				ImageView* img = ImageView::create("Image/Icon/global/img_jixulingqu.png");
				img->setPosition(Vec2(icon->getContentSize().width/2,icon->getContentSize().height/2));
				icon->addChild(img,5,"jixulingqu");
				siIdx = rowNumber;
			}
			break;
		case SIGN_STAT_TYPE_CANOT: //不可领状态
			{
				icon->setToolId(SignParam->signReward.mItemId);
				icon->setTipsEnabled(true);
			}
			break;
		default:
		break;
		}

		auto call = [this,i](Ref* ref, Widget::TouchEventType type)->void
		{
			if (type == Widget::TouchEventType::BEGAN)
			{
				SoundUtils::playSoundEffect("dianji");
			}
			else if (type == Widget::TouchEventType::ENDED)
			{
				Vector<LayerSignParam*>*  _SignParam  = LayerSignModel::getInstance()->getSignParam();
				LayerSignParam* SignParam = _SignParam->at(i);

				//如果已经领取
				switch (SignParam->stat)
				{
				case SIGN_STAT_TYPE_CAN: //可领取
					{
						LayerSignController::getInstance()->sendSignRecvice(i);
					}
					break;
				case SIGN_STAT_TYPE_VIP: //vip达到可领状态
					{
						MainParam* mainModel = MainModel::getInstance()->getMainParam();
						if (SignParam->vipDouble > 0 && mainModel->mVipLevel < SignParam->vipLevel)
						{
							CustomTips::showUtf8("VIP" + TO_STR(SignParam->vipLevel) + 
								DataManager::getInstance()->searchCommonTexdtById(20187));
						}
						else
						{
							LayerSignController::getInstance()->sendSignRecvice(i);
						}
					}
					break;
				case SIGN_STAT_TYPE_BU:  //可补签
					{
						auto call = [this,i](Ref* ref, CustomRetType type)->void
						{
							if (type == RET_TYPE_OK)
							{
								MainParam* pMainParam	= MainModel::getInstance()->getMainParam();
								if (pMainParam->mGold < LayerSignModel::getInstance()->getPrize())
								{
									CustomGoldTips *_tips = CustomGoldTips::create();
									if (_tips)
									{
										Director::getInstance()->getRunningScene()->addChild(_tips);
									}
									return;
								}
								LayerSignController::getInstance()->sendSignRecvice(i);
							}
						};

						CustomPop::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20188)
							+ TO_STR(LayerSignModel::getInstance()->getPrize()) + 
							DataManager::getInstance()->searchCommonTexdtById(20189),call,CUSTOM_YES_NO);
					}
					break;
				default:
				break;
				}
			}
		};
		icon->addTouchEventListener(call);

		icon->setAnchorPoint(Vec2(0,0));
		icon->setPosition(Vec2(colNumber*125 + 25, scrollViewHeight-(rowNumber+1)*125+10 ));
		//icon->setPosition(Vec2(colNumber*125 + 25, (row-1-rowNumber)*125+10 ));
		m_ui.scrollView->addChild(icon,0,"icon"+TO_STR(i));
	}

	//初始化完毕再设置滚动条比例
	m_customScrollBar1->setScrollBarLengthPercent(
		m_ui.scrollView->getContentSize().height / m_ui.scrollView->getInnerContainerSize().height);
	m_customScrollBar1->setScrollBarPercentY(0.0f);

	if (siIdx > 2 && siIdx < row)
	{
		siIdx++;
	}
	m_ui.scrollView->scrollToPercentVertical((float)siIdx/row*100, 0.5f, true);
}

void LayerSign::changSignPanelListItem(int idx)
{
	//服务器下拉数据
	SignRewardStateInfo state = LayerSignModel::getInstance()->getSignRewardStateInfo();
	m_signMonth = state.month;
	m_signCount = state.day;

	string monthStr = "Image/Icon/sign/img_month_"+_TO_STR(m_signMonth) + ".png";
	m_ui.imgMonth->loadTexture(monthStr);
	m_ui.signCount->setString(_TO_STR(m_signCount));
	if (idx < 0)
	{
		return;
	}
	UIToolHeader *item = (UIToolHeader*)m_ui.scrollView->getChildByName("icon"+TO_STR(idx));
	Vector<LayerSignParam*>*  _SignParam  = LayerSignModel::getInstance()->getSignParam();
	LayerSignParam* SignParam = _SignParam->at(idx);

	if (item)
	{
		if (item->getChildByName("AniZhuangbeiEffe"))
		{
			item->removeChildByName("AniZhuangbeiEffe");
		}
		if (item->getChildByName("buqian"))
		{
			item->removeChildByName("buqian");
		}
		if (item->getChildByName("jixulingqu"))
		{
			item->removeChildByName("jixulingqu");
		}
		//如果已经领取
		switch (SignParam->stat)
		{
		case SIGN_STAT_TYPE_DID: //已领取
			{
				item->setMaskFlag(true);
				if (item->getChildByName("MaskIconMask"))
				{
					item->getChildByName("MaskIconMask")->setVisible(true);
				}
				item->setToolId(SignParam->signReward.mItemId);
				item->setTipsEnabled(true);
			}
			break;
		case SIGN_STAT_TYPE_BU://3可补签
			{
				ImageView* img = ImageView::create("Image/Icon/global/img_buqian.png");
				img->setPosition(Vec2(item->getContentSize().width/2,item->getContentSize().height/2));
				item->addChild(img,5,"buqian");
				item->setTipsEnabled(false);
				item->setTouchEnabled(true);
			}
			break;
		case SIGN_STAT_TYPE_VIP://4 vip达到可领状态
			{
				ImageView* img = ImageView::create("Image/Icon/global/img_jixulingqu.png");
				img->setPosition(Vec2(item->getContentSize().width/2,item->getContentSize().height/2));
				item->addChild(img,5,"jixulingqu");
				item->setTipsEnabled(false);
				item->setTouchEnabled(true);
			}
			break;
		default:
		break;
		}
	}
}

void LayerSign::onBtnClick(Ref* ref, Widget::TouchEventType type, int nWidgetName)
{
	if (type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if (Widget::TouchEventType::ENDED == type)
	{
		switch (nWidgetName)
		{
		case Btn_Close:
			{
				dispose();
			}
			break;
		case Btn_Help:
			{
				LayerGameRules* signRule = LayerGameRules::create(SIGN_RULE_ID);
				if (nullptr != signRule)
				{
					Director::getInstance()->getRunningScene()->addChild(signRule, 100);

					ActionCreator::runCommDlgAction(signRule);
				}
			}
			break;
		default:
		break;
		}
	}
}

void LayerSign::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	if (observerParam->self == MainModel::getInstance())
	{
		LayerSignController::getInstance()->sendGetSignList();
	}
	else
	{
		switch (observerParam->id)
		{
		case SIGN_EVENT_RECEIVE:
			{
				TalkingData::onPurchase(DataManager::getInstance()->searchCommonTexdtById(20190),1,LayerSignModel::getInstance()->getPrize());

				changSignPanelListItem(LayerSignModel::getInstance()->getSignIdx1());
				changSignPanelListItem(LayerSignModel::getInstance()->getSignIdx2());

				showPopReward();
			}
			break;
		case SIGN_EVENT_INIT:
			{
				initScrollView();
				setGetHeroInfo();
			}
		default:
			break;
		}
	}
}


void LayerSign::showPopReward()
{
	ReWard_Show showParam = LayerSignModel::getInstance()->getRewardShow();
	vector<PopRewardItem*> vecRewardItem;


	//道具
	for (size_t i = 0; i < showParam.mVecRewardInfo.size(); i++)
	{
		SignRewardInfo& signRewardInfo = showParam.mVecRewardInfo.at(i);
		PopRewardItem_Tool* rewardItem = new PopRewardItem_Tool(signRewardInfo.mItemId, signRewardInfo.mItemCount);
		vecRewardItem.push_back(rewardItem);
	}

	//金币
	if (showParam.mGold > 0)
	{
		PopRewardItem_Gold* rewardItem = new PopRewardItem_Gold(showParam.mGold);
		vecRewardItem.push_back(rewardItem);
	}

	//银币
	if (showParam.mCoin > 0)
	{
		PopRewardItem_Coin* rewardItem = new PopRewardItem_Coin(showParam.mCoin);
		vecRewardItem.push_back(rewardItem);
	}

	if(vecRewardItem.size() > 0)
	{
		PopReward::show(vecRewardItem,[](Ref* sender, PopRewardEventType type)->void
		{
			//获得整卡
			if (type == EVENT_TYPE_END && LayerSignModel::getInstance()->getPetId() != -1)
			{
				LayerGetCard *_layerGetCard = LayerGetCard::create(LayerSignModel::getInstance()->getPetId(),false,1);
				Director::getInstance()->getRunningScene()->addChild(_layerGetCard);
			}
		});
	}
}

