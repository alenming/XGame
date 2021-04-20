#include "LimitTakeCardLayer.h"

#include "CocoStudio.h"
#include "Common/Common.h"
#include "../MainModule/MainModel.h"
#include "CCArmature.h"
#include "Widget/LayerCommHeadInfo.h"
#include "ToolModule/UICardPrize.h"
#include "Temp/CustomGoldTips.h"
#include "SDK_TalkingData/TalkingData.h"


const int TAKECARD_BG = 10;
const int TAKECARD_FRONT = 11;
const float TEN_CARD_SCALE_L = 0.5f;
const float ONE_CARD_SCALE_L = 1.0f;
const float CARD_TURN__DURATION = 0.1f;

LimitTakeCardLayer::LimitTakeCardLayer()
{
	_isFirstRun = true;
	firstChou = false;
	m_nTakeCardResultAnimationTime = 0;
	m_nTakeCardResultAnimationIndex		= 0;
	m_bTakeCardResultAnimationIsRun		= false;
	m_pTakeCardResultArmature			= nullptr;

	LimitTimeHeroModel::getInstance()->addObserver(this);
}

LimitTakeCardLayer::~LimitTakeCardLayer()
{
	LimitTimeHeroModel::getInstance()->removeObserver(this);
}

LimitTakeCardLayer* LimitTakeCardLayer::create(eChouKaType type)
{
	LimitTakeCardLayer *pRet = new LimitTakeCardLayer();
	if (pRet && pRet->init(type))
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

bool LimitTakeCardLayer::init(eChouKaType type)
{
	if(!Layer::init())
	{
		return false;
	}
	m_type = type;
	initUI();

	return true;

}

void LimitTakeCardLayer::onExit()
{
	Layer::onExit();
}

void LimitTakeCardLayer::onEnter()
{
	Layer::onEnter();
	if (_isFirstRun)
	{
		_isFirstRun = false;

		if (m_type== eTYPE_ONE)
		{
			updatePublicInfo();
			updateUI();
			firstChou = true;
		}
		else if (m_type== eTYPE_TEN)
		{
			updatePublicInfo();
			updateUI();
			firstChou = true;
		}
	}
}

void LimitTakeCardLayer::updateSelf( void* data )
{
	ObserverParam* observerParam = (ObserverParam*) data;

	switch (observerParam->id)
	{
		case nMAIN_CMD_LIMITTIMEHERO_CARD:
			{	
				if (firstChou)
				{
					auto pInstance = LimitTimeHeroModel::getInstance();
					if(pInstance->getBuyItemParam().size() == iItemNumOne)
					{
						updatePublicInfo();
						updateUI();
					}
					else if(pInstance->getBuyItemParam().size() == iItemNumTen)
					{
						updatePublicInfo();
						updateUI();
					}
				}
				break;
			}
		default:
			break;
	}
}

void LimitTakeCardLayer::initUI()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIxssj/UIxssj_point.ExportJson"));
	m_ui.ui_Root->setSize(size);
	m_ui.ui_Root->setPosition(pos);

	this->addChild(m_ui.ui_Root);

	auto root = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Root,"Panel_root"));
	root->setBackGroundImage("Image/Bg/img_shangcheng.jpg",ui::Widget::TextureResType::LOCAL);

	auto Pnl_Box = dynamic_cast<Layout*>(Helper::seekWidgetByName(root,"Pnl_Box"));
	Pnl_Box->setVisible(false);

	m_ui.Pnl_Card = dynamic_cast<Layout*>(Helper::seekWidgetByName(root,"Pnl_Card"));
	m_ui.Pnl_Card->setVisible(true);

	//单张卡牌
	m_ui.imgCard = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Pnl_Card,"imgCard"));
	m_ui.imgCard->setVisible(false);
	ImageView* pImgBg = ImageView::create("Image/Icon/global/img_kapai_beimian.png");
	pImgBg->setPosition(cocos2d::Vec2(m_ui.imgCard->getContentSize().width/2, m_ui.imgCard->getContentSize().height/2));
	pImgBg->setTag(TAKECARD_BG);
	m_ui.imgCard->addChild(pImgBg);

	ImageView* pImgFront = ImageView::create();
	pImgFront->setTag(TAKECARD_FRONT);
	m_ui.imgCard->addChild(pImgFront);

	//十张卡
	for (int i = 0; i < iItemNumTen; i++)
	{
		string cardName = "img_"+TO_STR(i+1);
		m_ui.VecimgCard[i] = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.Pnl_Card,cardName));

		ImageView* pHeroCardItem = m_ui.VecimgCard[i];
		ImageView* pImgBg = ImageView::create("Image/Icon/global/img_kapai_beimian.png");
		pImgBg->setPosition(cocos2d::Vec2(pHeroCardItem->getContentSize().width/2, pHeroCardItem->getContentSize().height/2));
		pImgBg->setTag(TAKECARD_BG);
		pHeroCardItem->addChild(pImgBg);

		ImageView* pImgFront = ImageView::create();
		pImgFront->setTag(TAKECARD_FRONT);
		pHeroCardItem->addChild(pImgFront);
	}

	m_ui.Img_Tips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Img_Tips"));

	m_ui.btn_kaiyici = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"btn_kaiyici"));
	m_ui.btn_kaishici = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"btn_kaishici"));

	m_ui.imgNowFree = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"imgNowFree"));
	m_ui.imgNowFree->setVisible(false);

	m_ui.img_jinbi1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"img_jinbi1"));
	m_ui.img_jinbi2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"img_jinbi2"));
	m_ui.img_jinbi1->setVisible(false);
	m_ui.img_jinbi2->setVisible(false);

	m_ui.jinbishici = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root,"jinbishici"));
	m_ui.jinbiyici = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Root,"jinbiyici"));
	m_ui.jinbishici->setVisible(false);
	m_ui.jinbiyici->setVisible(false);

	m_ui.btn_kaishici->addTouchEventListener(CC_CALLBACK_2(LimitTakeCardLayer::onBtnClick,this,eChouKaType::eTYPE_TEN));
	m_ui.btn_kaiyici->addTouchEventListener(CC_CALLBACK_2(LimitTakeCardLayer::onBtnClick,this,eChouKaType::eTYPE_ONE));

   
	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			//if (m_bTakeCardResultAnimationIsRun)
			//{
			//	return;
			//}	
			this->scheduleUpdate();
		}
	};
	auto _headInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_LIMIT_TIME_HERO);
	this->addChild(_headInfo);

}

void LimitTakeCardLayer::update(float ft)
{
	//打开主办面抽卡接口
	LimitTimeHeroModel::getInstance()->setIsAddBuyLayer(false);
	LimitTimeHeroController::sendGetMainInfoMsg();
	//m_Controller.sendGetMainInfoMsg();
	this->removeFromParent();
}

void LimitTakeCardLayer::updatePublicInfo()
{
	bool isfree = LimitTimeHeroModel::getInstance()->isCanBuyFree();

	if (isfree)//免费
	{
		m_ui.imgNowFree->setVisible(true);

		m_ui.img_jinbi1->setVisible(false);
		m_ui.jinbiyici->setVisible(false);
	}
	else
	{
		m_ui.imgNowFree->setVisible(false);

		m_ui.img_jinbi1->setVisible(true);
		m_ui.jinbiyici->setVisible(true);

		m_ui.jinbiyici->setString(TO_STR(LimitTimeHeroModel::getInstance()->getCost(eTYPE_ONE)));
		m_ui.jinbiyici->setFontName(FONT_FZZHENGHEI);
		m_ui.jinbiyici->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.jinbiyici, Color3B(0x00, 0x00, 0x00),2);
	}

	m_ui.jinbishici->setVisible(true);
	m_ui.img_jinbi2->setVisible(true);

	m_ui.jinbishici->setString(TO_STR(LimitTimeHeroModel::getInstance()->getCost(eTYPE_TEN)));
	m_ui.jinbishici->setFontName(FONT_FZZHENGHEI);
	m_ui.jinbishici->setColor(Color3B(0xff, 0xff, 0xff));
	setTextAddStroke(m_ui.jinbishici, Color3B(0x00, 0x00, 0x00),2);
}

void LimitTakeCardLayer::updateUI()
{
	m_ui.imgCard->setVisible(false);
	for (int i = 0; i < TEN_SPEND; i++)
	{		
		m_ui.VecimgCard[i]->setVisible(false);
	}
	//抽到的道具
	auto vecHeros= LimitTimeHeroModel::getInstance()->getBuyItemParam();
	startTakeCardResultAnimation(vecHeros.size());

}


void LimitTakeCardLayer::onBtnClick( Ref* reff, Widget::TouchEventType type,eChouKaType taketype )
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		if (m_bTakeCardResultAnimationIsRun)
		{
			return;
		}	

		sActData* actData = LimitTimeHeroModel::getInstance()->getActData();
		if(actData == nullptr || actData->actState != eSTATE_OPEN)
		{
			return;		//活动结束禁止抽卡
		}

		auto mainGold = MainModel::getInstance()->getMainParam()->mGold;

		if(taketype ==eTYPE_ONE && LimitTimeHeroModel::getInstance()->isCanBuyFree())
		{
			//发送抽
			LimitTimeHeroController::sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_ONE);
			//m_Controller.sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_ONE);

			return;
		}


		if (taketype == eTYPE_ONE &&mainGold <  LimitTimeHeroModel::getInstance()->getCost(eTYPE_ONE))
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
				return;
			}
		}
		else if (taketype == eTYPE_TEN &&mainGold < LimitTimeHeroModel::getInstance()->getCost(eTYPE_TEN))
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
				return;
			}
		}

		if (taketype == eTYPE_ONE && mainGold >= LimitTimeHeroModel::getInstance()->getCost(eTYPE_ONE))
		{
			m_type = taketype;
			//发送抽
			LimitTimeHeroController::sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_ONE);
			//m_Controller.sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_ONE);

		}
		else if (taketype == eTYPE_TEN && mainGold >= LimitTimeHeroModel::getInstance()->getCost(eTYPE_TEN))
		{			
			m_type = taketype;
			LimitTimeHeroController::sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_TEN);
			//m_Controller.sendTakeCardMsg(LimitTimeHeroModel::getInstance()->getActData()->actType, eTYPE_TEN);
		}

	}
}


void LimitTakeCardLayer::startTakeCardResultAnimation(const int& loopTimes)
{	
	if (loopTimes <= 0)
	{
		return;
	}
	if (m_bTakeCardResultAnimationIsRun)
	{
		return;
	}
	m_nTakeCardNum = loopTimes;
	//有的话删了原来的重要创建
	if (m_pTakeCardResultArmature != nullptr)
	{
		m_ui.Pnl_Card->removeChild(m_pTakeCardResultArmature);
		m_pTakeCardResultArmature = nullptr;
	}
	m_bTakeCardResultAnimationIsRun = true;
	//动画重新创建
	creatTakeCardResultAnimation(loopTimes);

	m_pTakeCardResultArmature->getAnimation()->playByIndex(0, 1, 0);

	m_pTakeCardResultArmature->setVisible(true);	
}


Armature* LimitTakeCardLayer::creatTakeCardResultAnimation(const int& loopTimes)
{	
	string strName = "cardone";
	float scale = 1.124f;
	if (loopTimes == 1)//单抽
	{
	}
	else  //10连抽
	{
		strName = "cards";
		scale = 1.028f;
	}	

	ArmatureDataManager::getInstance()->addArmatureFileInfo( "Image/AniTakeCard/" + strName +"/" + strName +"0.png", "Image/AniTakeCard/" + strName +"/" + strName +"0.plist", "Image/AniTakeCard/" + strName +"/" + strName + ".ExportJson");
	m_pTakeCardResultArmature = Armature::create(strName );

	m_pTakeCardResultArmature->setScale(scale);
	//设置帧事件；播放完动画

	m_pTakeCardResultArmature->getAnimation()->setMovementEventCallFunc(CC_CALLBACK_3(LimitTakeCardLayer::onResultAnimationCallFunc,this));
	//设置动画精灵位置 
	m_pTakeCardResultArmature->setPosition(m_ui.Pnl_Card->getContentSize().width/2
		, m_ui.Pnl_Card->getContentSize().height/2+40);
	//添加到当前页面 
	m_ui.Pnl_Card->addChild(m_pTakeCardResultArmature,99);
	return m_pTakeCardResultArmature;
}


void LimitTakeCardLayer::onResultAnimationCallFunc(Armature *armature, MovementEventType eventType, const std::string& strName)
{
	if (eventType == MovementEventType::COMPLETE)
	{
		m_bTakeCardResultAnimationIsRun = false;
		if (m_pTakeCardResultArmature != nullptr)
		{
			m_pTakeCardResultArmature->pause();
			m_pTakeCardResultArmature->setVisible(false);
		}
		showResultCards();
	}
} 

void LimitTakeCardLayer::showResultCards()
{
	//抽到的道具
	std::vector<sGetItemParam>& vecHeros= LimitTimeHeroModel::getInstance()->getBuyItemParam();

	if (vecHeros.size() == 1)//一个
	{
		m_ui.imgCard->setVisible(true);
		for (int i = 0; i < iItemNumTen; i++)
		{
			m_ui.VecimgCard[i]->setVisible(false);
		}

		showResultCardsItem(m_ui.imgCard, *vecHeros.begin());

		ImageView* pImgBg = dynamic_cast<ImageView*>(m_ui.imgCard->getChildByTag(TAKECARD_BG));
		ImageView* pImgFront = dynamic_cast<ImageView*>(m_ui.imgCard->getChildByTag(TAKECARD_FRONT));
		pImgBg->setVisible(true);
		pImgFront->setVisible(false);
		//换成碎片 
		m_isChangeChip = false;
		if (((sGetItemParam)*vecHeros.begin()).countChips > 0)
		{
			m_nTakeCardResultAnimationTime = secondNow();
			m_isChangeChip = true;
			m_ChangChipCount = ((sGetItemParam)*vecHeros.begin()).countChips;
		}

		int isShow = 0;
		m_CurShowHeroId = 0;
		RowData* pRowData = DataManager::getInstance()->searchToolById(vecHeros.at(0).nId);
		if (pRowData != nullptr)
		{
			if (pRowData->getIntData("itemTyp") == TOOL_ITEM_TYPE_HERO )  //-英雄
			{
				//需要展示
				isShow = 1;
				m_CurShowHeroId = vecHeros.at(0).nId;
			}			
		}

		animationShowCard(m_ui.imgCard,ONE_CARD_SCALE_L,isShow);

		return;
	}
	//10个
	int index = 0;
	m_ui.imgCard->setVisible(false);
	for (int i = 0; i < iItemNumTen; i++)
	{		
		m_ui.VecimgCard[i]->setVisible(true);
	}
	for (auto iter = vecHeros.begin(); iter != vecHeros.end(); iter++)
	{
		//展示英雄卡牌
		showResultCardsItem(m_ui.VecimgCard[index], *iter);

		ImageView* pImgBg = dynamic_cast<ImageView*>(m_ui.VecimgCard[index]->getChildByTag(TAKECARD_BG));
		ImageView* pImgFront = dynamic_cast<ImageView*>(m_ui.VecimgCard[index]->getChildByTag(TAKECARD_FRONT));
		pImgBg->setVisible(true);
		pImgFront->setVisible(false);

		index ++;
		if (index >= TEN_SPEND)
		{
			break;
		}		
	}
	//
	m_isChangeChip = false;
	if (((sGetItemParam)*vecHeros.begin()).countChips > 0)
	{
		m_nTakeCardResultAnimationTime = secondNow();
		m_isChangeChip = true;
		m_ChangChipCount = ((sGetItemParam)*vecHeros.begin()).countChips;
	}

	int isshow = 0;
	m_CurShowHeroId = 0;
	m_nTakeCardResultAnimationIndex = 0;
	RowData* pRowData = DataManager::getInstance()->searchToolById(vecHeros.at(m_nTakeCardResultAnimationIndex).nId);
	if (pRowData != nullptr)
	{
		if (pRowData->getIntData("itemTyp") == TOOL_ITEM_TYPE_HERO)  //4-英雄
		{
			//旋转移动到中间放大到正常 翻卡
			isshow = 1;
			m_CurShowHeroId = vecHeros.at(m_nTakeCardResultAnimationIndex).nId;
		}			
	}
	//
	animationShowCard(m_ui.VecimgCard[0], TEN_CARD_SCALE_L, isshow);
}


void LimitTakeCardLayer::showResultCardsItem(cocos2d::ui::ImageView* pItem, const sGetItemParam& info,bool isChip)
{
	if (isChip && info.countChips > 0)
	{
		RowData* pRowData = DataManager::getInstance()->searchToolById(info.nId);

		if (pRowData != nullptr)
		{
			ImageView* pImgFront = dynamic_cast<ImageView*>(pItem->getChildByTag(TAKECARD_FRONT));

			UICardPrize *cardPrize = UICardPrize::create(pRowData->getIntData("transNeed"));
			if (pImgFront->getChildByName("UICardPrize"))
			{
				pImgFront->removeChildByName("UICardPrize");
			}
			pImgFront->addChild(cardPrize,1,"UICardPrize");
			cardPrize->setCardCount(info.countChips);
			cardPrize->setCardName(pRowData->getStringData("itemName"));
		}
	}
	else
	{
		RowData* pRowData = DataManager::getInstance()->searchToolById(info.nId);
		if (pRowData != nullptr)
		{
			ImageView* pImgFront = dynamic_cast<ImageView*>(pItem->getChildByTag(TAKECARD_FRONT));

			UICardPrize *cardPrize = UICardPrize::create(info.nId);
			if (pImgFront->getChildByName("UICardPrize"))
			{
				pImgFront->removeChildByName("UICardPrize");
			}

			pImgFront->addChild(cardPrize,1,"UICardPrize");
			cardPrize->setCardCount(info.nCount);
			cardPrize->setCardName(pRowData->getStringData("itemName"));
		}
	}
}


//抽卡翻卡动画
void LimitTakeCardLayer::animationShowCard(cocos2d::ui::ImageView *pLayHeroCard, float fScale, int show)
{
	ImageView* pImgBg = dynamic_cast<ImageView*>(pLayHeroCard->getChildByTag(TAKECARD_BG));
	ImageView* pImgFront = dynamic_cast<ImageView*>(pLayHeroCard->getChildByTag(TAKECARD_FRONT));

	pLayHeroCard->setZOrder(5);
	m_fTakeCardResultAnimationScale = fScale;
	m_bTakeCardResultAnimationIsRun = true;

	//翻转
	ScaleTo *pScaleTo = ScaleTo::create(CARD_TURN__DURATION, 0.0f, fScale);	
	CallFuncN *pCallFuncN1 = CallFuncN::create(CC_CALLBACK_1(LimitTakeCardLayer::animitionShowCardCallFunND, this, show));
	CallFuncN *pCallFuncN2 = CallFuncN::create(CC_CALLBACK_1(LimitTakeCardLayer::animitionShowCardEndCallFunND, this, show));
	Sequence* pSequence = Sequence::create( pScaleTo,pCallFuncN2,pCallFuncN1,nullptr);
	/*
	[=]()
	{
		m_ui.Pnl_Card->setTouchEnabled(true);
		auto pRoot = dynamic_cast<Layout*>(_headInfo->getChildByName("Panel_13"));
		auto backButton = dynamic_cast<Button*>(Helper::seekWidgetByName(pRoot,"back"));
		if (backButton)
		{
			backButton->setEnabled(true);
		}

	},
	*/
	pLayHeroCard->stopAllActions();
	pLayHeroCard->runAction(pSequence);	
}


void LimitTakeCardLayer::animitionShowCardCallFunND(cocos2d::Node* pNode, int show)
{
	ImageView* pImgFront = dynamic_cast<ImageView*>(pNode->getChildByTag(TAKECARD_FRONT));
	ImageView* pImgBg = dynamic_cast<ImageView*>(pNode->getChildByTag(TAKECARD_BG));
	pImgBg->setVisible(false);
	pImgFront->setVisible(true);

	ScaleTo *pScaleTo = ScaleTo::create(CARD_TURN__DURATION, m_fTakeCardResultAnimationScale, m_fTakeCardResultAnimationScale);
	Sequence* pSequence = Sequence::create( pScaleTo, nullptr);
	pNode->runAction(pSequence);
}

void LimitTakeCardLayer::animitionShowCardEndCallFunND(cocos2d::Node* pNode, int show)
{
	pNode->setZOrder(0);

	if (show == 1)
	{
		auto call = [this]()->void
		{
			m_bTakeCardResultAnimationIsRun = false;

			vector<sGetItemParam>& vecHeros= LimitTimeHeroModel::getInstance()->getBuyItemParam();
			if (vecHeros.size() == 1)//一个
			{
				showResultCardsItem(m_ui.imgCard, *vecHeros.begin(),true);
			}
			else
			{
				showResultCardsItem(m_ui.VecimgCard[m_nTakeCardResultAnimationIndex], 
					vecHeros.at(m_nTakeCardResultAnimationIndex),true);
			}

			animationShowCardContinue();
		};
		LayerGetCard *_layerGetCard = LayerGetCard::create(m_CurShowHeroId,m_isChangeChip,m_ChangChipCount,call);
		this->addChild(_layerGetCard,4);
	}
	else
	{
		animationShowCardContinue();
	}
}

void LimitTakeCardLayer::animationShowCardContinue()
{	
	m_nTakeCardResultAnimationIndex ++;
	vector<sGetItemParam>& vecHeros=  LimitTimeHeroModel::getInstance()->getBuyItemParam();
	if (m_nTakeCardResultAnimationIndex < vecHeros.size())
	{
		m_isChangeChip = false;
		if (vecHeros.at(m_nTakeCardResultAnimationIndex).countChips > 0)
		{
			m_nTakeCardResultAnimationTime = secondNow();
			m_isChangeChip = true;
			m_ChangChipCount = vecHeros.at(m_nTakeCardResultAnimationIndex).countChips;
		}

		int isshow = 0;
		m_CurShowHeroId = 0;
		RowData* pRowData = DataManager::getInstance()->searchToolById(vecHeros.at(m_nTakeCardResultAnimationIndex).nId);
		if (pRowData != nullptr)
		{			 
			if (pRowData->getIntData("itemTyp") == TOOL_ITEM_TYPE_HERO)  //4-英雄
			{
				//旋转移动到中间放大到正常 翻卡
				isshow = 1;
				m_CurShowHeroId = vecHeros.at(m_nTakeCardResultAnimationIndex).nId;
			}			
		}
		animationShowCard(m_ui.VecimgCard[m_nTakeCardResultAnimationIndex], m_fTakeCardResultAnimationScale, isshow);
	}
	else
	{
		m_bTakeCardResultAnimationIsRun = false;
	}
}