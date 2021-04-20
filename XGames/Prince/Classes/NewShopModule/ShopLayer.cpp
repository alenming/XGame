#include "ShopLayer.h"
#include "Common/LocalData.h"
#include "ShopChouScene.h"
#include "Temp/CustomGoldTips.h"


ShopLayer::ShopLayer():_otherlayer(nullptr),_shopType(ST_NONE),_LastshopType(ST_NONE),isFirst(true),_cuiLianlayer(nullptr)
{
	ShopModel::getInstance()->addObserver(this);
}

ShopLayer::~ShopLayer()
{
	ShopModel::getInstance()->removeObserver(this);
}

ShopLayer* ShopLayer::create(SHOP_TYPE type, bool ispush) 
{ 
	ShopLayer *pRet = new ShopLayer(); 
	if (pRet && pRet->init(type, ispush)) 
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

bool ShopLayer::init(SHOP_TYPE type, bool ispush)
{
	if (!Layer::init())
	{
		return false;
	}
	ShopModel::getInstance()->setIsPushScene(ispush);


	_shopType = type;
	_LastshopType = type;
	//_shopType = ST_NONE;
	//_LastshopType = ST_NONE;
	ShopModel::getInstance()->setShopType(type);
	initUi();

	ShopController::getInstance()->loadLocalCuilianInfo();

	return true;
}

void ShopLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIShopexc2/UIMarket.ExportJson"));
	m_ui.ui_Root->setSize(size);
	m_ui.ui_Root->setPosition(pos);
	m_ui.ui_Root->setBackGroundImage("Image/Bg/img_bg_quanping.jpg",ui::Widget::TextureResType::LOCAL);
	this->addChild(m_ui.ui_Root);

	m_ui.Img_di = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root,"Img_di"));

	//五个商店按键
	m_ui.Btn_Box = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Btn_Box"));
	m_ui.Btn_jifen = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Btn_jifen"));
	m_ui.Btn_rongyu = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Btn_rongyu"));
	m_ui.Btn_shengwang = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Btn_shengwang"));
	m_ui.Btn_zahuo = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root,"Btn_zahuo"));

	//商店界面UI
	m_ui.ui_Box.Pnl_Box =  dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Root,"Pnl_Box"));
	m_ui.ui_Box.Pnl_Box->setVisible(false);
	m_ui.ui_shop.Pnl_Root =  dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Root,"Pnl_shop"));


	/************************************************************************/
	/*	                       淬练容器UI                                   */
	/************************************************************************/
	m_ui.ui_Box.Btn_Exchange = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Box.Pnl_Box,"Btn_Exchange"));

	//m_ui.ui_Box.Btn_Exchange->addTouchEventListener(CC_CALLBACK_2(ShopLayer::onBtnClickByCuilian,this));

	m_ui.ui_Box.Lab_Fragment = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Box.Pnl_Box,"Lab_Fragment"));
	m_ui.ui_Box.Lab_Tips = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Box.Pnl_Box,"Lab_Tips"));
	m_ui.ui_Box.Lab_Count = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Box.Pnl_Box,"Lab_Count"));
	//4个可获得物品
	for (int i = 0; i < 4; i++)
	{
		auto oneItem = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Box.Pnl_Box,"Pnl_Item"+TO_STR(i+1)));
		 m_ui.ui_Box.laFiveItem.push_back(oneItem);
	}
	//中间容器
	m_ui.ui_Box.Pnl_centerBox = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_ui.ui_Box.Pnl_Box,"Pnl_centerBox"));
	//一次
	m_ui.ui_Box.ui_Onece.Img_Once = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.Pnl_Box,"Img_Once"));
	m_ui.ui_Box.ui_Onece.Img_Free = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Onece.Img_Once,"Img_Free"));
	m_ui.ui_Box.ui_Onece.Img_Gold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Onece.Img_Once,"Img_Gold"));
	m_ui.ui_Box.ui_Onece.Img_Key = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Onece.Img_Once,"Img_Key"));
	m_ui.ui_Box.ui_Onece.Img_Tips = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Onece.Img_Once,"Img_Tips"));
	m_ui.ui_Box.ui_Onece.Lab_GoldCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Onece.Img_Once,"Lab_GoldCost"));
	m_ui.ui_Box.ui_Onece.Lab_KeyCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Onece.Img_Once,"Lab_KeyCost"));
	m_ui.ui_Box.ui_Onece.Btn_Once = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Onece.Img_Once,"Btn_Once"));
	m_ui.ui_Box.ui_Onece.Img_Free->setVisible(false);
	m_ui.ui_Box.ui_Onece.Img_Gold->setVisible(false);
	m_ui.ui_Box.ui_Onece.Img_Key->setVisible(false);
	m_ui.ui_Box.ui_Onece.Lab_GoldCost->setString("");
	m_ui.ui_Box.ui_Onece.Lab_KeyCost->setString("");

	//十次
	m_ui.ui_Box.ui_Tenth.Img_Tenth = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.Pnl_Box,"Img_Tenth"));
	m_ui.ui_Box.ui_Tenth.Btn_Tenth = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Tenth.Img_Tenth,"Btn_Tenth"));
	m_ui.ui_Box.ui_Tenth.Img_Gold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Tenth.Img_Tenth,"Img_Gold"));
	m_ui.ui_Box.ui_Tenth.Img_Key = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Tenth.Img_Tenth,"Img_Key"));
	m_ui.ui_Box.ui_Tenth.Img_Tips2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Tenth.Img_Tenth,"Img_Tips2"));
	m_ui.ui_Box.ui_Tenth.Lab_GoldCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Tenth.Img_Tenth,"Lab_GoldCost"));
	m_ui.ui_Box.ui_Tenth.Lab_KeyCost = dynamic_cast<Text*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Tenth.Img_Tenth,"Lab_KeyCost"));
	m_ui.ui_Box.ui_Tenth.Img_Design1 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Tenth.Img_Tenth,"Img_Design1"));
	m_ui.ui_Box.ui_Tenth.Img_Design2 = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Box.ui_Tenth.Img_Tenth,"Img_Design2"));
	m_ui.ui_Box.ui_Tenth.Img_Gold->setVisible(false);
	m_ui.ui_Box.ui_Tenth.Img_Key->setVisible(false);
	m_ui.ui_Box.ui_Tenth.Img_Design1->setVisible(false);
	m_ui.ui_Box.ui_Tenth.Img_Design2->setVisible(false);
	m_ui.ui_Box.ui_Tenth.Lab_GoldCost->setString("");
	m_ui.ui_Box.ui_Tenth.Lab_KeyCost->setString("");
	
	auto back = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			if (ShopModel::getInstance()->getIsPushScene())
			{
				Director::getInstance()->popScene();
			}
			else
			{
				Director::getInstance()->replaceScene(MainCityScene::create());
			}
		}
	};
	LayerCommHeadInfo *_layerCommHeadInfo = LayerCommHeadInfo::create(back,HEAD_INFO_TYPE_MARKET);
	this->addChild(_layerCommHeadInfo,99);

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniJSBOXFront/AniJSBOXFront.ExportJson");
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniJSBoxBack/AniJSBoxBack.ExportJson");

	auto cenPo = m_ui.ui_Box.Pnl_centerBox->getContentSize();
	m_ui.ui_Box.Pnl_centerBox->setBackGroundImage("Image/UIShopexc2/img_baoxiang.png");
	Armature* armature2 = Armature::create("AniJSBoxBack");
	armature2->getAnimation()->play("AniJSBox",-1,1);
	armature2->setPosition(Vec2(cenPo.width/2,cenPo.height/2));
	m_ui.ui_Box.Pnl_centerBox->addChild(armature2,-1);

	Armature* armature1 = Armature::create("AniJSBOXFront");
	armature1->getAnimation()->play("AniJSBox",-1,1);
	armature1->setPosition(Vec2(cenPo.width/2,cenPo.height/2));
	m_ui.ui_Box.Pnl_centerBox->addChild(armature1);

	//淬练商店
	m_ui.ui_Box.Btn_Exchange->addTouchEventListener(CC_CALLBACK_2(ShopLayer::onBtnClickByCuilian,this));
	m_ui.Btn_Box->addTouchEventListener(CC_CALLBACK_2(ShopLayer::onBtnClick,this,ST_NONE));
	m_ui.Btn_jifen->addTouchEventListener(CC_CALLBACK_2(ShopLayer::onBtnClick,this,ST_JIFENG));
	m_ui.Btn_rongyu->addTouchEventListener(CC_CALLBACK_2(ShopLayer::onBtnClick,this,ST_RONGYU));
	m_ui.Btn_shengwang->addTouchEventListener(CC_CALLBACK_2(ShopLayer::onBtnClick,this,ST_SEHGNWANG));
	m_ui.Btn_zahuo->addTouchEventListener(CC_CALLBACK_2(ShopLayer::onBtnClick,this,ST_ZAHUO));

	if (_shopType == ST_NONE)
	{
		m_ui.Btn_Box->setTouchEnabled(false);
		m_ui.Btn_Box->setHighlighted(true);
		m_ui.ui_Box.Pnl_Box->setVisible(true);
	}
	else
	{
		m_ui.Btn_Box->setTouchEnabled(true);
		m_ui.Btn_Box->setHighlighted(false);
		m_ui.ui_Box.Pnl_Box->setVisible(false);
	}

	m_ui.Btn_jifen->setTouchEnabled(false);
	m_ui.Btn_rongyu->setTouchEnabled(false);
	m_ui.Btn_shengwang->setTouchEnabled(false);
	m_ui.Btn_zahuo->setTouchEnabled(false);

	//一次
	m_ui.ui_Box.ui_Onece.Btn_Once->addTouchEventListener(CC_CALLBACK_2(ShopLayer::onBtnChouClick,this,CHOU_TYPE::ONE_CHOU));

	//十次
	m_ui.ui_Box.ui_Tenth.Btn_Tenth->addTouchEventListener(CC_CALLBACK_2(ShopLayer::onBtnChouClick,this,CHOU_TYPE::TEN_CHOU));


}

void ShopLayer::onEnter()
{
	Layer::onEnter();
	Director::getInstance()->getScheduler()->schedule(schedule_selector(ShopLayer::updateNextFram), this, 0, false);
}

void ShopLayer::updateNextFram(float ft)
{
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(ShopLayer::updateNextFram), this);
	updateBaoJiaFace();
	ShopController::getInstance()->handleFirstShopLayerInfo();
}

void ShopLayer::onExit()
{
	Layer::onExit();
}

void ShopLayer::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*)data;
	switch (observerParam->id)
	{
		case SHOP_EVENT_TYEP_FIRST_SHOP:
			{
				if (ShopModel::getInstance()->getShopType() == ST_NONE)
				{
					GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ITEM_SHOP);
				}
				else if (ShopModel::getInstance()->getShopType() == ST_RONGYU)
				{
					GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ARENA_SHOP);
				}

				updateFirstShopUI();
				getScheduler()->schedule([=](float dt)->void
				{
					this->getScheduler()->unschedule("ShopLayer::updateSelf", this);	
					setWhichShopClieck(ShopModel::getInstance()->getShopType());
				},this, 0, 0, 0, false, "ShopLayer::updateSelf");	
			}	
			break;			
		case SHOP_EVENT_TYEP_ONE_CHOU:
		case SHOP_EVENT_TYEP_TEN_CHOU:
			{
				if(!ShopModel::getInstance()->getFirstChou())
				{		
					ShopModel::getInstance()->setFirstChou(true);
					auto choutye = (CHOU_TYPE)(observerParam->id);
					getScheduler()->schedule([=](float dt)->void
					{
						this->getScheduler()->unschedule("ShopLayer::CHOU", this);
						Director::getInstance()->pushScene(ShopChouScene::createScene(choutye));
					},this, 0, 0, 0, false, "ShopLayer::CHOU");	
				}
			}
			break;
		case SHOP_EVENT_TYEP_CUILIAN_PUB:
			{
				if (ShopModel::getInstance()->getShopType() == ST_CUILIAN)
				{
					int count = MainModel::getInstance()->getMainParam()->mCuiLianMoney;
					m_ui.ui_Box.Lab_Count->setString(TO_STR(count));
				}
			}
			break;
		case FREECHOUTIME:
			{
				//到计时完成 刷新一下免费抽的界面
				updateFirstShopUI();
			}
			break;
		default:
		break;
	}
}

void  ShopLayer::onBtnClickByCuilian( Ref* reff, Widget::TouchEventType type)
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		ShopModel::getInstance()->setShopType(ST_CUILIAN);
		auto cuilian = LayerShopOne::create(ST_CUILIAN);
		this->addChild(cuilian, 99);
		ActionCreator::runCommDlgAction(cuilian->getImageViewBg());
	}
}

void ShopLayer::updateFirstShopUI()
{

	int freshcount = ShopModel::getInstance()->getFreshCount();
	bool isfree = ShopModel::getInstance()->getIsFree();

	if (isfree)//免费
	{
		m_ui.ui_Box.ui_Onece.Img_Free->setVisible(true);
		m_ui.ui_Box.ui_Onece.Img_Key->setVisible(false);
		m_ui.ui_Box.ui_Onece.Lab_KeyCost->setVisible(false);
		m_ui.ui_Box.ui_Onece.Img_Gold->setVisible(false);
		m_ui.ui_Box.ui_Onece.Lab_GoldCost->setVisible(false);

	}
	else if (!isfree && freshcount>0)
	{
		m_ui.ui_Box.ui_Onece.Img_Free->setVisible(false);
		m_ui.ui_Box.ui_Onece.Img_Key->setVisible(true);
		m_ui.ui_Box.ui_Onece.Lab_KeyCost->setVisible(true);
		m_ui.ui_Box.ui_Onece.Lab_KeyCost->setString(TO_STR(freshcount)+"/1");
		m_ui.ui_Box.ui_Onece.Lab_KeyCost->setFontName(FONT_FZZHENGHEI);
		m_ui.ui_Box.ui_Onece.Lab_KeyCost->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.ui_Box.ui_Onece.Lab_KeyCost, Color3B(0x00, 0x00, 0x00),2);

		m_ui.ui_Box.ui_Onece.Img_Gold->setVisible(false);
		m_ui.ui_Box.ui_Onece.Lab_GoldCost->setVisible(false);

	}
	else if (!isfree && freshcount<=0)
	{
		m_ui.ui_Box.ui_Onece.Img_Free->setVisible(false);
		m_ui.ui_Box.ui_Onece.Img_Key->setVisible(false);
		m_ui.ui_Box.ui_Onece.Lab_KeyCost->setVisible(false);

		m_ui.ui_Box.ui_Onece.Img_Gold->setVisible(true);
		m_ui.ui_Box.ui_Onece.Lab_GoldCost->setVisible(true);
		m_ui.ui_Box.ui_Onece.Lab_GoldCost->setString(TO_STR(ShopModel::getInstance()->getOneChouPrice()));
		m_ui.ui_Box.ui_Onece.Lab_GoldCost->setFontName(FONT_FZZHENGHEI);
		m_ui.ui_Box.ui_Onece.Lab_GoldCost->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.ui_Box.ui_Onece.Lab_GoldCost, Color3B(0x00, 0x00, 0x00),2);
	}


	if (freshcount>=TENCHOU)//有十张刷新令
	{
		m_ui.ui_Box.ui_Tenth.Img_Key->setVisible(true);
		m_ui.ui_Box.ui_Tenth.Lab_KeyCost->setVisible(true);
		m_ui.ui_Box.ui_Tenth.Lab_KeyCost->setString(TO_STR(freshcount)+"/10");
		m_ui.ui_Box.ui_Tenth.Lab_KeyCost->setFontName(FONT_FZZHENGHEI);
		m_ui.ui_Box.ui_Tenth.Lab_KeyCost->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.ui_Box.ui_Tenth.Lab_KeyCost, Color3B(0x00, 0x00, 0x00),2);

		m_ui.ui_Box.ui_Tenth.Img_Gold->setVisible(false);
		m_ui.ui_Box.ui_Tenth.Lab_GoldCost->setVisible(false);

	}
	else if ( freshcount<TENCHOU)
	{
		m_ui.ui_Box.ui_Tenth.Img_Key->setVisible(false);
		m_ui.ui_Box.ui_Tenth.Lab_KeyCost->setVisible(false);

		m_ui.ui_Box.ui_Tenth.Img_Gold->setVisible(true);
		m_ui.ui_Box.ui_Tenth.Lab_GoldCost->setVisible(true);
		m_ui.ui_Box.ui_Tenth.Lab_GoldCost->setString(TO_STR(ShopModel::getInstance()->getTenChouPrice()));
		m_ui.ui_Box.ui_Tenth.Lab_GoldCost->setFontName(FONT_FZZHENGHEI);
		m_ui.ui_Box.ui_Onece.Lab_GoldCost->setColor(Color3B(0xff, 0xff, 0xff));
		setTextAddStroke(m_ui.ui_Box.ui_Tenth.Lab_GoldCost, Color3B(0x00, 0x00, 0x00),2);
	}

}

void ShopLayer::setWhichShopClieck( SHOP_TYPE type )
{
	m_ui.Btn_Box->setTouchEnabled(true);
	m_ui.Btn_zahuo->setTouchEnabled(true);
	m_ui.Btn_jifen->setTouchEnabled(true);
	m_ui.Btn_rongyu->setTouchEnabled(true);
	m_ui.Btn_shengwang->setTouchEnabled(true);

	switch (type)
	{
		case ST_NONE:
			{
				ShopModel::getInstance()->setShopType(ST_NONE);
				m_ui.Btn_Box->setTouchEnabled(false);
				m_ui.Btn_Box->setHighlighted(true);
				m_ui.ui_Box.Pnl_Box->setVisible(true);
				break;
			}
		case ST_CUILIAN:
			{
				m_ui.Btn_Box->setTouchEnabled(false);
				m_ui.Btn_Box->setHighlighted(true);
				m_ui.ui_Box.Pnl_Box->setVisible(true);
				ShopModel::getInstance()->setShopType(ST_CUILIAN);
				auto cuilian = LayerShopOne::create(ST_CUILIAN);
				this->addChild(cuilian, 99);
				break;
			}
		case ST_ZAHUO:
			{
				if (_otherlayer != nullptr)
				{
					_otherlayer->removeFromParent();
					_otherlayer = nullptr;
				}
				m_ui.Btn_zahuo->setTouchEnabled(false);
				m_ui.Btn_zahuo->setHighlighted(true);
				if (!_otherlayer)
				{
					ShopModel::getInstance()->m_root = m_ui.ui_shop.Pnl_Root;
					_otherlayer = LayerShopOne::create(ST_ZAHUO);
					m_ui.Img_di->addChild(_otherlayer);
				}
				break;
			}
		case ST_RONGYU:
			{
				if (_otherlayer != nullptr)
				{
					_otherlayer->removeFromParent();
					_otherlayer = nullptr;
				}
				m_ui.Btn_rongyu->setTouchEnabled(false);
				m_ui.Btn_rongyu->setHighlighted(true);
				if (!_otherlayer)
				{
					ShopModel::getInstance()->m_root =m_ui.ui_shop.Pnl_Root;// m_ui.ui_Rongyu.Pnl_Root;
					_otherlayer = LayerShopOne::create(ST_RONGYU);
					m_ui.Img_di->addChild(_otherlayer);
				}
				break;
			}
		case ST_JIFENG:
			{
				if (_otherlayer != nullptr)
				{
					_otherlayer->removeFromParent();
					_otherlayer = nullptr;
				}
				m_ui.Btn_jifen->setTouchEnabled(false);
				m_ui.Btn_jifen->setHighlighted(true);
				if (!_otherlayer)
				{
					ShopModel::getInstance()->m_root = m_ui.ui_shop.Pnl_Root;//m_ui.ui_Jifeng.Pnl_Root;
					_otherlayer = LayerShopOne::create(ST_JIFENG);
					m_ui.Img_di->addChild(_otherlayer);
				}
				break;
			}
		case ST_SEHGNWANG:
			{
				if (_otherlayer != nullptr)
				{
					_otherlayer->removeFromParent();
					_otherlayer = nullptr;
				}
				m_ui.Btn_shengwang->setTouchEnabled(false);
				m_ui.Btn_shengwang->setHighlighted(true);
				if (!_otherlayer)
				{
					ShopModel::getInstance()->m_root =  m_ui.ui_shop.Pnl_Root;//m_ui.ui_Shengwang.Pnl_Root;
					_otherlayer = LayerShopOne::create(ST_SEHGNWANG);
					m_ui.Img_di->addChild(_otherlayer);
				}
				break;
			}
		default:
			break;
	}
}

void  ShopLayer::onBtnClick( Ref* reff, Widget::TouchEventType type, SHOP_TYPE _callBackType)
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		m_ui.Btn_Box->setTouchEnabled(true);
		m_ui.Btn_Box->setHighlighted(false);
		m_ui.ui_Box.Pnl_Box->setVisible(false);
		m_ui.Btn_zahuo->setTouchEnabled(true);
		m_ui.Btn_zahuo->setHighlighted(false);
		m_ui.Btn_rongyu->setTouchEnabled(true);
		m_ui.Btn_rongyu->setHighlighted(false);
		m_ui.Btn_jifen->setTouchEnabled(true);
		m_ui.Btn_jifen->setHighlighted(false);
		m_ui.Btn_shengwang->setTouchEnabled(true);
		m_ui.Btn_shengwang->setHighlighted(false);

		//m_ui.ui_Zahuo.Pnl_Root->setVisible(false);
		//m_ui.ui_Rongyu.Pnl_Root->setVisible(false);
		//m_ui.ui_Jifeng.Pnl_Root->setVisible(false);
		//m_ui.ui_Shengwang.Pnl_Root->setVisible(false);
		m_ui.ui_shop.Pnl_Root->setVisible(false);

		_shopType = _callBackType;

		switch (_callBackType)
		{
		case ST_NONE:
			{
				if (_otherlayer != nullptr)
				{
					_otherlayer->removeFromParent();
					_otherlayer = nullptr;
				}
				m_ui.Btn_Box->setTouchEnabled(false);
				m_ui.Btn_Box->setHighlighted(true);
				m_ui.ui_Box.Pnl_Box->setVisible(true);
				_LastshopType = _shopType;
				ShopModel::getInstance()->setShopType(_shopType);
				//ShopController::getInstance()->handleCuiLianLayerInfo();
				break;
			}
		case ST_ZAHUO:
			{
				if (_otherlayer != nullptr)
				{
					_otherlayer->removeFromParent();
					_otherlayer = nullptr;
				}
				m_ui.Btn_zahuo->setTouchEnabled(false);
				m_ui.Btn_zahuo->setHighlighted(true);
				ShopModel::getInstance()->m_root = m_ui.ui_shop.Pnl_Root;// m_ui.ui_Zahuo.Pnl_Root;
				_otherlayer = LayerShopOne::create(ST_ZAHUO);
				m_ui.Img_di->addChild(_otherlayer);
				_LastshopType = _shopType;
				ShopModel::getInstance()->setShopType(_shopType);
				break;
			}
		case ST_RONGYU:
			{
				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_PVP,false))
				{
					if (_otherlayer != nullptr)
					{
						_otherlayer->removeFromParent();
						_otherlayer = nullptr;
					}
					m_ui.Btn_rongyu->setTouchEnabled(false);
					m_ui.Btn_rongyu->setHighlighted(true);
					ShopModel::getInstance()->m_root = m_ui.ui_shop.Pnl_Root;// m_ui.ui_Rongyu.Pnl_Root;
					_otherlayer = LayerShopOne::create(ST_RONGYU);
					m_ui.Img_di->addChild(_otherlayer);
					_LastshopType = _shopType;
					ShopModel::getInstance()->setShopType(_shopType);
				}
				else
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20256));
				}
				break;
			}
		case ST_JIFENG:
			{
				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_TOWER,false))
				{
					if (_otherlayer != nullptr)
					{
						_otherlayer->removeFromParent();
						_otherlayer = nullptr;
					}
					m_ui.Btn_jifen->setTouchEnabled(false);
					m_ui.Btn_jifen->setHighlighted(true);
					ShopModel::getInstance()->m_root = m_ui.ui_shop.Pnl_Root;// m_ui.ui_Jifeng.Pnl_Root;
					_otherlayer = LayerShopOne::create(ST_JIFENG);
					m_ui.Img_di->addChild(_otherlayer);
					_LastshopType = _shopType;
					ShopModel::getInstance()->setShopType(_shopType);
				}
				else
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20257));
				}
				break;
			}
		case ST_SEHGNWANG:
			{
				if (GuideManager::getInstance()->isFunctionOpen(FUNCTION_FACTION,false))
				{
					if(MainModel::getInstance()->getMainParam()->mGroupId > 0) //已加入帮派
					{
						if (_otherlayer != nullptr)
						{
							_otherlayer->removeFromParent();
							_otherlayer = nullptr;
						}
						m_ui.Btn_shengwang->setTouchEnabled(false);
						m_ui.Btn_shengwang->setHighlighted(true);
						ShopModel::getInstance()->m_root = m_ui.ui_shop.Pnl_Root;// m_ui.ui_Shengwang.Pnl_Root;
						_otherlayer = LayerShopOne::create(ST_SEHGNWANG);
						m_ui.Img_di->addChild(_otherlayer);
						_LastshopType = _shopType;
						ShopModel::getInstance()->setShopType(_shopType);
					}
					else
					{
						CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20173));
					}
				}		
				else
				{
					CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(20173));
				}
				break;
			}
			default:
			break;
		}

		//当功能没开启时回到原来的点击状态
		if (_LastshopType  ==ST_NONE)
		{
			m_ui.Btn_Box->setTouchEnabled(false);
			m_ui.Btn_Box->setHighlighted(true);
			m_ui.ui_Box.Pnl_Box->setVisible(true);
		}
		if (_LastshopType  ==ST_RONGYU)
		{
			m_ui.Btn_rongyu->setTouchEnabled(false);
			m_ui.Btn_rongyu->setHighlighted(true);
			m_ui.ui_shop.Pnl_Root->setVisible(true);
			
		}
		if (_LastshopType  ==ST_ZAHUO)
		{
			m_ui.Btn_zahuo->setTouchEnabled(false);
			m_ui.Btn_zahuo->setHighlighted(true);
			m_ui.ui_shop.Pnl_Root->setVisible(true);
		}
		if (_LastshopType ==ST_JIFENG)
		{
			m_ui.Btn_jifen->setTouchEnabled(false);
			m_ui.Btn_jifen->setHighlighted(true);
			m_ui.ui_shop.Pnl_Root->setVisible(true);
		}
		if (_LastshopType ==ST_SEHGNWANG)
		{
			m_ui.Btn_shengwang->setTouchEnabled(false);
			m_ui.Btn_shengwang->setHighlighted(true);
			m_ui.ui_shop.Pnl_Root->setVisible(true);
		}
	}
}

void ShopLayer::onBtnChouClick( Ref* reff, Widget::TouchEventType type, CHOU_TYPE mBtnCallBackType )
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		auto oneper = ShopModel::getInstance()->getOneChouPrice();
		auto tenper = ShopModel::getInstance()->getTenChouPrice();
		auto mainGold = MainModel::getInstance()->getMainParam()->mGold;

		if(mBtnCallBackType ==ONE_CHOU && ShopModel::getInstance()->getIsFree())
		{
			ShopController::getInstance()->handleChouOneOrTen(mBtnCallBackType);
			return;
		}

		if(mBtnCallBackType ==ONE_CHOU && ShopModel::getInstance()->getFreshCount()>0)
		{
			ShopController::getInstance()->handleChouOneOrTen(mBtnCallBackType);
			return;
		}

		if(mBtnCallBackType ==TEN_CHOU && ShopModel::getInstance()->getFreshCount()>=TENCHOU)
		{
			ShopController::getInstance()->handleChouOneOrTen(mBtnCallBackType);
			return;
		}


		if (mBtnCallBackType == ONE_CHOU &&mainGold <  oneper)
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
				return;
			}
		}
		else if (mBtnCallBackType == TEN_CHOU &&mainGold < tenper)
		{
			CustomGoldTips *_tips = CustomGoldTips::create();
			if (_tips)
			{
				Director::getInstance()->getRunningScene()->addChild(_tips);
				return;
			}
		}


		if (mBtnCallBackType == ONE_CHOU && mainGold >= oneper)
		{
			ShopController::getInstance()->handleChouOneOrTen(mBtnCallBackType);
		}
		else if (mBtnCallBackType == TEN_CHOU && mainGold >= tenper)
		{			
			ShopController::getInstance()->handleChouOneOrTen(mBtnCallBackType);
		}

	}
}

void ShopLayer::updateBaoJiaFace()
{
	//第一个界面初始化
	m_ui.ui_Box.Lab_Tips->setFontName(FONT_FZZHENGHEI);
	m_ui.ui_Box.Lab_Tips->setColor(Color3B(0xda, 0xdb, 0x5d));
	setTextAddStroke(m_ui.ui_Box.Lab_Tips, Color3B(0x00, 0x00, 0x00),2);
	//淬练碎片
	m_ui.ui_Box.Lab_Fragment->setFontName(FONT_FZZHENGHEI);
	m_ui.ui_Box.Lab_Fragment->setColor(Color3B(0xff, 0xff, 0xff));
	setTextAddStroke(m_ui.ui_Box.Lab_Fragment, Color3B(0x79, 0x03, 0x03),2);
	//碎片数量
	m_ui.ui_Box.Lab_Count->setFontName(FONT_FZZHENGHEI);
	m_ui.ui_Box.Lab_Count->setColor(Color3B(0xff, 0xff, 0xff));
	int count = MainModel::getInstance()->getMainParam()->mCuiLianMoney;
	m_ui.ui_Box.Lab_Count->setString(TO_STR(count));


	//4个可获得物品
	for (int i = 0; i < m_ui.ui_Box.laFiveItem.size(); i++)
	{
		auto temp = ShopModel::getInstance()->m_canGetPram.at(i);
		auto _layout = dynamic_cast<Layout*>(m_ui.ui_Box.laFiveItem.at(i));
		_layout->removeAllChildren();
		UIToolHeader* _item = UIToolHeader::create(temp.nId);
		_item->setAnchorPoint(Vec2(0,0));
		_item->setPosition(Vec2(0,0));
		_item->setScale(0.87f);
		_item->setNumEx(-1);//temp.nCount);
		_item->setTipsEnabled(true);
		_item->setToolId(temp.nId);
		_item->showLightEff();
		_layout->addChild(_item);
	}
}

