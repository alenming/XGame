#include "LayerCommHeadInfo.h"
#include "OperateActModule/OperateActModel.h"
#include "NewAreaActivityModule/NewAreaActivityModel.h"
#include "ArtifactModule/ArtifactModel.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;  

LayerCommHeadInfo::LayerCommHeadInfo()
{
	m_silverImg = nullptr;
	m_silver = nullptr;
	m_silverTex = nullptr;
	m_silverBtn = nullptr;

	m_goldImg = nullptr;
	m_gold = nullptr;
	m_goldTex = nullptr;
	m_goldBtn = nullptr;

	m_energyImg = nullptr;
	m_energy = nullptr;
	m_energyTex = nullptr;
	m_energyBtn = nullptr;

	MainModel::getInstance()->addObserver(this);
	FateModel::getInstance()->addObserver(this);
	ArtifactModel::getInstance()->addObserver(this);
}

LayerCommHeadInfo::~LayerCommHeadInfo()
{
	MainModel::getInstance()->removeObserver(this);
	FateModel::getInstance()->removeObserver(this);
	ArtifactModel::getInstance()->removeObserver(this);
}

void LayerCommHeadInfo::updateSelf(void* data)
{
	ObserverParam* observerParam = (ObserverParam*) data;
	if (observerParam->self == MainModel::getInstance() || observerParam->self == FateModel::getInstance() 
		|| observerParam->self == ArtifactModel::getInstance())
	{
		updateInfo();
	}
}

LayerCommHeadInfo* LayerCommHeadInfo::create(Widget::ccWidgetTouchCallback callback,HEAD_INFO_TYPE type)
{
	//背景设置
	LayerCommHeadInfo* _ayerCommHeadInfo = new LayerCommHeadInfo;
	if (_ayerCommHeadInfo && _ayerCommHeadInfo->init())
	{
		_ayerCommHeadInfo->autorelease();
		_ayerCommHeadInfo->initUi(callback,type);
		return _ayerCommHeadInfo;
	}

	CC_SAFE_DELETE(_ayerCommHeadInfo);
	return nullptr;
}

bool LayerCommHeadInfo::initUi(Widget::ccWidgetTouchCallback callback,HEAD_INFO_TYPE type)
{
	m_type = type;
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UICommHeadInfo/UICommHeadInfo.ExportJson"));
	this->addChild(m_pRoot);	
	m_pRoot->setSize(Size(size.width,m_pRoot->getContentSize().height));
	m_pRoot->setPosition(Vec2(pos.x,size.height));

	m_pRoot->runAction(EaseSineOut::create(MoveTo::create(0.3f, Vec2(pos.x,size.height-m_pRoot->getContentSize().height))));

	Button *Button_back = dynamic_cast<Button*>(Helper::seekWidgetByName(m_pRoot,"back"));
	if (callback)
	{
		Button_back->addTouchEventListener(callback);
	}
	else
	{
		Button_back->setVisible(false);
	}

	m_title = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"title"));

	m_silverImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Image_silver"));
	m_silver = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_silverImg,"Image_silver_1"));
	m_silverTex = dynamic_cast<Text*>(Helper::seekWidgetByName(m_silverImg,"Label_silver_2"));
	m_silverTex->setFontName(FONT_FZZHENGHEI);
	m_silverBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(m_silverImg,"Button_add_coin"));
	
	m_goldImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Image_gold"));
	m_gold = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_goldImg,"Image_gold_1"));
	m_goldTex = dynamic_cast<Text*>(Helper::seekWidgetByName(m_goldImg,"Label_gold_2"));
	m_goldTex->setFontName(FONT_FZZHENGHEI);
	m_goldBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(m_goldImg,"Button_add_gold"));

	m_energyImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Image_energy"));
	m_energy = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_energyImg,"Image_energy_1"));
	m_energyTex = dynamic_cast<Text*>(Helper::seekWidgetByName(m_energyImg,"Label_energy_2"));
	m_energyTex->setFontName(FONT_FZZHENGHEI);
	m_energyBtn = dynamic_cast<Button*>(Helper::seekWidgetByName(m_energyImg,"Button_add_energy"));

	setInfo();
	return true;
}

void LayerCommHeadInfo::setInfo()
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	mHonor = mainParam->mPvpScore;
	mEnergy = mainParam->mEnergy;
	mGold = mainParam->mGold;
	mPower = mainParam->mPower;
	mCoin = mainParam->mCoin;
	mGroupMoney = mainParam->mGroupMoney;
	mStarNum = mainParam->mStarNum;
	mMazePower = mainParam->mMazePower;
	mHunTiandan = mainParam->mHunTiandan;
	mJiuZhuandan = mainParam->mJiuZhuandan;
	mKechienDan = 0;
	const TOOL_INFO* toolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(KechienDanId);
	if (toolInfo)
	{
		mKechienDan = toolInfo->nstack;
	}

	mXilianshi = 0;
	const TOOL_INFO* toolInfoXiLian = ToolModel::getInstance()->GetToolInfoBytemplateId(XilianshiId);
	if (toolInfoXiLian)
	{
		mXilianshi = toolInfoXiLian->nstack;
	}

	mJinNang = 0;
	const TOOL_INFO* toolInfoJinNang = ToolModel::getInstance()->GetToolInfoBytemplateId(JinNangId);
	if (toolInfoJinNang)
	{
		mJinNang = toolInfoJinNang->nstack;
	}

	switch (m_type)
	{
	case HEAD_INFO_TYPE_PVP:
		{
			m_title->loadTexture("Image/Icon/title/img_jingjichang.png");
			m_energyImg->setVisible(false);

			//荣誉
			m_gold->loadTexture("Image/Icon/global/img_icon_rongyu.png");
			m_gold->setScale(1.0f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mPvpScore));
			m_goldBtn->setVisible(false);

			//金币
			m_silver->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_silverBtn->addTouchEventListener(addGold);
		}
		break;
	case HEAD_INFO_TYPE_PVPALL:
		{
			m_title->loadTexture("Image/Icon/title/img_jingji.png");
			//体力
			m_energy->loadTexture("Image/Icon/global/img_tili.png");
			m_energy->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mPower) + "/" + TO_STR(mainParam->mMaxPower));
			auto addPower = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addPower);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_LOVE:
		{
			m_title->loadTexture("Image/Icon/title/img_qingyuan.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_MALL:
		{
			m_title->loadTexture("Image/Icon/title/img_shangcheng.png");
			//体力
			m_energy->loadTexture("Image/Icon/global/img_tili.png");
			m_energy->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mPower) + "/" + TO_STR(mainParam->mMaxPower));
			auto addPower = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addPower);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_BKS_SHOP:
		{
			m_title->loadTexture("Image/Icon/title/img_zhuangbei.png");
			//体力
			m_energy->loadTexture("Image/Icon/global/img_tili.png");
			m_energy->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mPower) + "/" + TO_STR(mainParam->mMaxPower));
			auto addPower = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addPower);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_STAR_CAMP:
		{
			m_title->loadTexture("Image/Icon/title/img_xingpan.png");
			m_energyImg->setVisible(false);
			
			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_STAR_SU:
		{
			m_title->loadTexture("Image/Icon/title/img_xingxiu.png");
			//星星
			m_energy->loadTexture("Image/Icon/global/img_star_1.png");
			m_energy->setScale(0.4f);
			m_energyTex->setString(StringFormat::formatNumber(mainParam->mStarNum));
			
			m_energyBtn->setVisible(false);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_FATE:
		{
			m_title->loadTexture("Image/Icon/title/img_yuanfen.png");
			//结缘丹
			m_energy->loadTexture("Image/Icon/global/img_jieyuandan.png");
			m_energy->setScale(1.0f);
			m_energyTex->setString(StringFormat::formatNumber(mKechienDan));
			auto addPower = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerEquipSource* pSmithy = LayerEquipSource::create();
					Director::getInstance()->getRunningScene()->addChild(pSmithy);
					pSmithy->showSrc(KechienDanId);
				}
			};
			m_energyBtn->addTouchEventListener(addPower);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_HERO_FATE:
		{
			m_title->loadTexture("Image/Icon/title/img_yuanfen.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_ARTIFACTS:
		{
			m_title->loadTexture("Image/Icon/title/img_shanggushenqi.png");
			//洗练石
			m_energy->loadTexture("Image/Icon/global/img_xilianshi.png");
			m_energy->setScale(0.6f);
			m_energyTex->setString(StringFormat::formatNumber(mXilianshi));
			m_energyBtn->setVisible(false);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_STARSTONE:
		{
			m_title->loadTexture("Image/Icon/title/img_xingshi.png");
			m_energyImg->setVisible(false);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_STARSTONEGAT:
		{
			m_title->loadTexture("Image/Icon/title/img_xingshijiachi.png");
			m_energyImg->setVisible(false);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_PLUNDER:
		{
			m_title->loadTexture("Image/Icon/title/img_lveduo.png");
			//精力
			m_energy->loadTexture("Image/Icon/global/img_jingli.png");
			m_energy->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mEnergy) + "/" + TO_STR(mainParam->mMaxEnergy));
			auto addEnergy = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_ENERGY);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addEnergy);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_ACTIVITY_LEVEL:
		{
			m_title->loadTexture("Image/Icon/title/img_huodongguanqia.png");
			//体力
			m_energy->loadTexture("Image/Icon/global/img_tili.png");
			m_energy->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mPower) + "/" + TO_STR(mainParam->mMaxPower));
			auto addPower = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addPower);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_TOWER:
		{
			m_title->loadTexture("Image/Icon/title/img_shilianta.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_THOUSAND_TOWER:
		{
			m_title->loadTexture("Image/Icon/title/img_qiancengta.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_SURVIVAL_TEST:
		{
			m_title->loadTexture("Image/Icon/title/img_didishilian.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_BAG:
		{
			m_title->loadTexture("Image/Icon/title/img_beibao.png");
			//体力
			m_energy->loadTexture("Image/Icon/global/img_tili.png");
			m_energy->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mPower) + "/" + TO_STR(mainParam->mMaxPower));
			auto addPower = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addPower);

			//精力
			m_gold->loadTexture("Image/Icon/global/img_jingli.png");
			m_gold->setScale(1.5f);
			m_goldTex->setString(TO_STR(mainParam->mEnergy) + "/" + TO_STR(mainParam->mMaxEnergy));
			auto addEnergy = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_ENERGY);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_goldBtn->addTouchEventListener(addEnergy);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_ZHENFA:
		{
			m_title->loadTexture("Image/Icon/title/img_zhenfa.png");
			//精力
			m_energy->loadTexture("Image/Icon/global/img_jingli.png");
			//m_energyTex->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mEnergy) + "/" + TO_STR(mainParam->mMaxEnergy));
			auto addEnergy = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_ENERGY);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addEnergy);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_BOSS:
		{
			m_title->loadTexture("Image/Icon/title/img_juedifanji.png");
			//精力
			m_energy->loadTexture("Image/Icon/global/img_jingli.png");
			//m_energyTex->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mEnergy) + "/" + TO_STR(mainParam->mMaxEnergy));
			auto addEnergy = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_ENERGY);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addEnergy);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_NEWAREA_ACT:
		{
			m_title->loadTexture("Image/Icon/title/img_kaifukuanghuan.png");
			//精力
			m_energy->loadTexture("Image/Icon/global/img_jingli.png");
			//m_energyTex->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mEnergy) + "/" + TO_STR(mainParam->mMaxEnergy));
			auto addEnergy = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_ENERGY);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addEnergy);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case  HEAD_INFO_TYPE_RANK_BOARD:
		{
			m_title->loadTexture("Image/Icon/title/img_paihangbang.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_ZHENZXING:
		{
			m_title->loadTexture("Image/Icon/title/img_zhenxing.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_DREAM_LAND:
		{
			m_title->loadTexture("Image/Icon/title/img_huanjing.png");
			//行动力
			m_energy->loadTexture("Image/Icon/global/img_xingdongli.png");
			m_energy->setScale(1.5f);
			RowData* pMazeData = DataManager::getInstance()->searchMazeBaseById(mainParam->mMazeId);
			if (pMazeData)
			{
				m_energyTex->setString(TO_STR(mainParam->mMazePower) + "/" + _TO_STR(pMazeData->getIntData("move")));
			}
			else
			{
				RowData* _pMazeData = DataManager::getInstance()->searchMazeBaseById(MAZ_MAP_ID);
				m_energyTex->setString(TO_STR(mainParam->mMazePower) + "/" + _TO_STR(_pMazeData->getIntData("move")));
			}
			auto addMazePower = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_MAZEPOWER);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addMazePower);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_REFINING_FURNACE:
		{
			m_title->loadTexture("Image/Icon/title/img_lianhualu.png");
			//浑天丹
			m_energy->loadTexture("Image/Icon/global/img_huntiandan1.png");
			m_energy->setScale(1.0f);
			m_energyTex->setString(StringFormat::formatNumber(mainParam->mHunTiandan));
			m_energyBtn->setVisible(false);

			//九转丹
			m_gold->loadTexture("Image/Icon/global/img_jiuzhuandan1.png");
			m_gold->setScale(1.0f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mJiuZhuandan));
			m_goldBtn->setVisible(false);

			//金币
			m_silver->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_silverBtn->addTouchEventListener(addGold);
		}
		break;
	case HEAD_INFO_TYPE_GANG:
		{
			m_title->loadTexture("Image/Icon/title/img_bangpai.png");

			//帮派货币
			m_energy->loadTexture("Image/Icon/global/img_shengwaihuobi.png");
			m_energy->setScale(1.0f);
			m_energyTex->setString(StringFormat::formatNumber(mainParam->mGroupMoney));
			m_energyBtn->setVisible(false);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_ADVENTURE:
		{
			m_title->loadTexture("Image/Icon/title/img_maoxian.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_VIP:
		{
			m_title->loadTexture("Image/Icon/title/img_vip.png");
			//体力
			m_energy->loadTexture("Image/Icon/global/img_tili.png");
			m_energy->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mPower) + "/" + TO_STR(mainParam->mMaxPower));
			auto addPower = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addPower);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			m_goldBtn->setVisible(false);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_MA:
		{
			m_title->loadTexture("Image/Icon/title/img_jingcaihuodong.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_OPERATE_ACT:
		{
			m_title->loadTexture("Image/Icon/title/img_fulidating.png");
			m_energyImg->setVisible(false);
			
			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_OPERATE_TALENT:
		{
			m_title->loadTexture("Image/Icon/title/img_tianfu.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_MARKET:
		{
			m_title->loadTexture("Image/Icon/title/img_jishi.png");
			m_energyImg->setVisible(false);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_LIMIT_TIME_HERO:
		{
			m_title->loadTexture("Image/Icon/title/img_xianshishenjiang.png");
			m_energyImg->setVisible(false);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_SEABED_DRAGON:
		{
			m_title->loadTexture("Image/Icon/title/img_yunhaixunlong.png");

			//声望
			m_energy->loadTexture("Image/Icon/global/img_shengwaihuobi.png");
			m_energy->setScale(1.0f);
			m_energyTex->setString(StringFormat::formatNumber(mainParam->mGroupMoney));
			m_energyBtn->setVisible(false);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_DAY_DOWN_PAYMENT:
		{
			m_title->loadTexture("Image/Icon/title/img_tianjiangjinbi.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_CHALLENGE_RANKING:
		{
			m_title->loadTexture("Image/Icon/title/img_tiaozhanpaihang.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);
			m_silverImg->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_BONUS_EXCHANGE:
		{
			m_title->loadTexture("Image/Icon/title/img_jiangliduihuan.png");
			m_energyImg->setVisible(false);
			m_goldImg->setVisible(false);

			//锦囊
			m_silver->loadTexture("Image/Icon/global/img_icon_jinlang.png");
			m_silver->setScale(1.0f);
			m_silverTex->setString(StringFormat::formatNumber(mJinNang));
			m_silverBtn->setVisible(false);
		}
		break;
	case HEAD_INFO_TYPE_MYSTERY_SHOP:
		{
			m_title->loadTexture("Image/Icon/title/img_shengmishangdian.png");
			m_energyImg->setVisible(false);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			auto addGold = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					Director::getInstance()->pushScene(VipScene::createScene(PAY_VIEW));
				}
			};
			m_goldBtn->addTouchEventListener(addGold);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;
	case HEAD_INFO_TYPE_GROUP_BOSS:
		{
			m_title->loadTexture("Image/Icon/title/img_bangpaitiaozhan.png");
			//体力
			m_energy->loadTexture("Image/Icon/global/img_tili.png");
			m_energy->setScale(1.5f);
			m_energyTex->setString(TO_STR(mainParam->mPower) + "/" + TO_STR(mainParam->mMaxPower));
			auto addPower = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_POWER);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_energyBtn->addTouchEventListener(addPower);

			//金币
			m_gold->loadTexture("Image/Icon/global/img_yuanbao.png");
			m_gold->setScale(1.7f);
			m_goldTex->setString(StringFormat::formatNumber(mainParam->mGold));
			m_goldBtn->setVisible(false);

			//银币
			m_silver->loadTexture("Image/Icon/global/img_yinbi.png");
			m_silver->setScale(1.7f);
			m_silverTex->setString(StringFormat::formatNumber(mainParam->mCoin));
			auto addCoin = [this](Ref* sender, Widget::TouchEventType type)->void
			{
				if (type == Widget::TouchEventType::BEGAN)
				{
					SoundUtils::playSoundEffect("dianji");
				}
				else if (type == Widget::TouchEventType::ENDED)
				{
					LayerCommonBuy *buyEnergy = LayerCommonBuy::create(COMMON_BUY_TYPE_GOLD_COIN);
					Director::getInstance()->getRunningScene()->addChild(buyEnergy);
				}
			};
			m_silverBtn->addTouchEventListener(addCoin);
		}
		break;

	default:
		break;
	}
}

void LayerCommHeadInfo::updateInfo()
{
	switch (m_type)
	{
	case HEAD_INFO_TYPE_PVP:
		{
			//荣誉
			setHonor(m_goldTex);

			//金币
			setGold(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_MALL:
		{
			//体力
			setPower(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_PVPALL:
		{
			//体力
			setPower(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_BKS_SHOP:
		{
			//体力
			setPower(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_STAR_CAMP:
		{
			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_STAR_SU:
		{
			//星星
			setStarNum(m_energyTex);
			
			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_FATE:
		{
			//结缘丹
			setKechienDan(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_ARTIFACTS:
		{
			//洗练石
			setXiLianShi(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_STARSTONE:
		{
			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_STARSTONEGAT:
		{
			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_PLUNDER:
		{
			//精力
			setEnergy(m_energyTex);
			
			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_ACTIVITY_LEVEL:
		{
			//体力
			setPower(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_BAG:
		{
			//体力
			setPower(m_energyTex);

			//精力
			setEnergy(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_ZHENFA:
		{
			//精力
			setEnergy(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_DREAM_LAND:
		{
			//行动力
			setMazePower(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_REFINING_FURNACE:
		{
			//浑天丹
			setHunTiandan(m_energyTex);

			//九转丹
			setJiuZhuandan(m_goldTex);

			//金币
			setGold(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_GANG:
		{
			//帮贡
			setGroupMoney(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_BOSS:
		{
			//精力
			setEnergy(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_NEWAREA_ACT:
		{
			//精力
			setEnergy(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}	
		break;
	case HEAD_INFO_TYPE_OPERATE_ACT:
		{
			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_VIP:
		{
			//体力
			setPower(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_MARKET:
		{
			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_LIMIT_TIME_HERO:
		{
			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_SEABED_DRAGON:
		{
			//帮贡
			setGroupMoney(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_BONUS_EXCHANGE:
		{
			//锦囊
			setJinNang(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_MYSTERY_SHOP:
		{
			setGold(m_goldTex);
			setCoin(m_silverTex);
		}
		break;
	case HEAD_INFO_TYPE_GROUP_BOSS:
		{
			//体力
			setPower(m_energyTex);

			//金币
			setGold(m_goldTex);

			//银币
			setCoin(m_silverTex);
		}
		break;
	default:
		break;
	}
}

void LayerCommHeadInfo::setHonor(Text *item)
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	//荣誉
	if (mHonor != mainParam->mPvpScore)
	{
		auto call = [this,mainParam,item]()->void
		{
			mHonor = mainParam->mPvpScore;
			item->setString(TO_STR(mainParam->mPvpScore));
		};
		item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
	}
}

void LayerCommHeadInfo::setEnergy(Text *item)
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	//精力
	if (mEnergy != mainParam->mEnergy)
	{
		auto call = [this,mainParam,item]()->void
		{
			mEnergy = mainParam->mEnergy;
			item->setString(TO_STR(mainParam->mEnergy) + "/" + TO_STR(mainParam->mMaxEnergy));
		};
		item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
	}
}

void LayerCommHeadInfo::setGold(Text *item)
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	//金币
	if (mGold != mainParam->mGold)
	{
		auto call = [this,mainParam,item]()->void
		{
			mGold = mainParam->mGold;
			item->setString(StringFormat::formatNumber(mainParam->mGold));
		};
		item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
	}
}

void LayerCommHeadInfo::setPower(Text *item)
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	//体力
	if (mPower != mainParam->mPower)
	{
		auto call = [this,mainParam,item]()->void
		{
			mPower = mainParam->mPower;
			item->setString(TO_STR(mainParam->mPower) + "/" + TO_STR(mainParam->mMaxPower));
		};
		item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
	}
}

void LayerCommHeadInfo::setCoin(Text *item)
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	//银币
	if (mCoin != mainParam->mCoin)
	{
		auto call = [this,mainParam,item]()->void
		{
			mCoin = mainParam->mCoin;
			item->setString(StringFormat::formatNumber(mainParam->mCoin));
		};
		item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
	}
}

void LayerCommHeadInfo::setStarNum(Text *item)
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	//星星
	if (mStarNum != mainParam->mStarNum)
	{
		auto call = [this,mainParam,item]()->void
		{
			mStarNum = mainParam->mStarNum;
			item->setString(TO_STR(mainParam->mStarNum));
		};
		item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
	}
}

void LayerCommHeadInfo::setMazePower(Text *item)
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	//迷宫行动力
	if (mMazePower != mainParam->mMazePower)
	{
		auto call = [this,mainParam,item]()->void
		{
			mMazePower = mainParam->mMazePower;
			RowData* pMazeData = DataManager::getInstance()->searchMazeBaseById(mainParam->mMazeId);
			if (pMazeData)
			{
				item->setString(TO_STR(mainParam->mMazePower) + "/" + _TO_STR(pMazeData->getIntData("move")));
			}
		};
		item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
	}
}

void LayerCommHeadInfo::setHunTiandan(Text *item)
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	//浑天丹
	if (mHunTiandan != mainParam->mHunTiandan)
	{
		auto call = [this,mainParam,item]()->void
		{
			mHunTiandan = mainParam->mHunTiandan;
			item->setString(TO_STR(mainParam->mHunTiandan));
		};
		item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
	}
}

void LayerCommHeadInfo::setJiuZhuandan(Text *item)
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	//九转丹
	if (mJiuZhuandan != mainParam->mJiuZhuandan)
	{
		auto call = [this,mainParam,item]()->void
		{
			mJiuZhuandan = mainParam->mJiuZhuandan;
			item->setString(TO_STR(mainParam->mJiuZhuandan));
		};
		item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
	}
}

void LayerCommHeadInfo::setKechienDan(Text *item)
{
	const TOOL_INFO* pToolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(KechienDanId);
	//结缘丹
	if (pToolInfo)
	{
		if (mKechienDan != pToolInfo->nstack)
		{
			auto call = [this,pToolInfo,item]()->void
			{
				mKechienDan = pToolInfo->nstack;
				item->setString(TO_STR(pToolInfo->nstack));
			};
			item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
		}
		else
		{
			item->setString(TO_STR(pToolInfo->nstack));
		}
	}
	else
	{
		if (mKechienDan != 0)
		{
			auto call = [this,item]()->void
			{
				mKechienDan = 0;
				item->setString(TO_STR(0));
			};
			item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
		}
		else
		{
			item->setString(TO_STR(0));
		}
	}
}

void LayerCommHeadInfo::setXiLianShi(Text *item)
{
	const TOOL_INFO* pToolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(XilianshiId);
	//洗练石
	if (pToolInfo)
	{
		if (mXilianshi != pToolInfo->nstack)
		{
			auto call = [this,pToolInfo,item]()->void
			{
				mXilianshi = pToolInfo->nstack;
				item->setString(TO_STR(pToolInfo->nstack));
			};
			item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
		}
		else
		{
			item->setString(TO_STR(pToolInfo->nstack));
		}
	}
	else
	{
		if (mXilianshi != 0)
		{
			auto call = [this,item]()->void
			{
				mXilianshi = 0;
				item->setString(TO_STR(0));
			};
			item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
		}
		else
		{
			item->setString(TO_STR(0));
		}
	}
}

void LayerCommHeadInfo::setJinNang(Text *item)
{
	const TOOL_INFO* pToolInfo = ToolModel::getInstance()->GetToolInfoBytemplateId(JinNangId);
	//锦囊
	if (pToolInfo)
	{
		if (mJinNang != pToolInfo->nstack)
		{
			auto call = [this,pToolInfo,item]()->void
			{
				mJinNang = pToolInfo->nstack;
				item->setString(TO_STR(pToolInfo->nstack));
			};
			item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
		}
		else
		{
			item->setString(TO_STR(pToolInfo->nstack));
		}
	}
	else
	{
		if (mJinNang != 0)
		{
			auto call = [this,item]()->void
			{
				mJinNang = 0;
				item->setString(TO_STR(0));
			};
			item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
		}
		else
		{
			item->setString(TO_STR(0));
		}
	}
}

void LayerCommHeadInfo::setGroupMoney(Text *item)
{
	MainParam* mainParam = MainModel::getInstance()->getMainParam();

	//金币
	if (mGroupMoney != mainParam->mGroupMoney)
	{
		auto call = [this,mainParam,item]()->void
		{
			mGroupMoney = mainParam->mGroupMoney;
			item->setString(TO_STR(mainParam->mGroupMoney));
		};
		item->runAction(CCSequence::create(creatScaleBig(),CCCallFunc::create(call),creatScaleSmall(),nullptr));
	}
}


EaseSineOut* LayerCommHeadInfo::creatScaleBig()
{
	EaseSineOut *scaleTo = EaseSineOut::create(CCScaleTo::create(0.15f,1.6f));
	return scaleTo;
}

EaseSineIn* LayerCommHeadInfo::creatScaleSmall()
{
	EaseSineIn *scaleTo = EaseSineIn::create(CCScaleTo::create(0.1f,1.0f));
	return scaleTo;
}


void LayerCommHeadInfo::onEnter()
{
	Layer::onEnter();
}

void LayerCommHeadInfo::onExit()
{
	Layer::onExit();
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UICommHeadInfo");
}


