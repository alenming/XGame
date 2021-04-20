#include "MysteryInterLayer.h"
#include "MysteryShopController.h"
#include "MysteryShopScene.h"

MysteryInterLayer::MysteryInterLayer()
{

}

MysteryInterLayer::~MysteryInterLayer()
{
}

MysteryInterLayer* MysteryInterLayer::create()
{
	MysteryInterLayer *pRet = new MysteryInterLayer(); 
	if (pRet && pRet->init())
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
bool MysteryInterLayer::init( )
{
	if(!Layer::init())
	{
		return false;
	}

	initUi();
	MysteryShopModel::getInstance()->setIsInterShow(false);
	MysteryShopModel::getInstance()->setIsMainShow(true);

	return true;
}

void MysteryInterLayer::onEnter()
{
	Layer::onEnter();

}


void MysteryInterLayer::updateNextFram( float dt )
{
	this->removeFromParent();
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(MysteryInterLayer::updateNextFram), this);
}


void MysteryInterLayer::onExit()
{
	Layer::onExit();
}

void MysteryInterLayer::initUi()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_ui.ui_Root = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UI_SecretStore/Ui_shenmishangdian_enter.ExportJson"));
	m_ui.ui_Root->setSize(size);
	m_ui.ui_Root->setPosition(pos);
	this->addChild(m_ui.ui_Root);


	m_ui.Image_BackGround = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_ui.ui_Root, "Image_BackGround"));
	m_ui.bRun = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root, "Button_run"));
	m_ui.bWait = dynamic_cast<Button*>(Helper::seekWidgetByName(m_ui.ui_Root, "Button_wait"));
	m_ui.bRun->setVisible(false);
	m_ui.bWait->setVisible(false);
	m_ui.bRun->addTouchEventListener(CC_CALLBACK_2(MysteryInterLayer::onBtnClick,this,MYSTERY_INTER_TYPE::MYSTERY_RUN));
	m_ui.bWait->addTouchEventListener(CC_CALLBACK_2(MysteryInterLayer::onBtnClick,this,MYSTERY_INTER_TYPE::MYSTERY_WAIT));

	string aniName1 = "once";
	string aniName2 = "circle";

	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/Anishenmishangdian/Anishenmishangdian.ExportJson");
	Armature* ani = Armature::create("Anishenmishangdian");

	Bone*  bone = ani->getBone("title");

	if(bone)
	{
		Skin* _newSkin = Skin::create("Image/Anishenmishangdian/img_faxianshenmishangdian.png");
		bone->addDisplay(_newSkin,0);
	}

	ani->getAnimation()->play(aniName1);

	auto onResultAnimationCallFunc = [=](Armature *armature, MovementEventType eventType, const std::string& strName)->void
	{
		if (eventType == MovementEventType::COMPLETE)
		{
			ani->getAnimation()->play(aniName2,-1,-1);
			m_ui.bRun->setVisible(true);
			m_ui.bWait->setVisible(true);
			m_ui.bRun->setLocalZOrder(9);
			m_ui.bWait->setLocalZOrder(9);
			ani->getAnimation()->setMovementEventCallFunc(nullptr);
		}
	};

	ani->getAnimation()->setMovementEventCallFunc(onResultAnimationCallFunc);
	ani->setPosition(Vec2(m_ui.Image_BackGround->getContentSize().width/2.0f, m_ui.Image_BackGround->getContentSize().height/2.0f));
	//ani->setScale(0.8f);

	m_ui.Image_BackGround->addChild(ani,0);
}

void MysteryInterLayer::onBtnClick( Ref* reff, Widget::TouchEventType type, MYSTERY_INTER_TYPE goType)
{
	if(type == Widget::TouchEventType::BEGAN)
	{
		SoundUtils::playSoundEffect("dianji");
	}
	else if(type == Widget::TouchEventType::ENDED)
	{
		switch (goType)
		{
		case MYSTERY_RUN:
			{
				auto stamp = MysteryShopModel::getInstance()->getDaoTime();
				if (stamp > 0)
				//{
				//if (MysteryShopModel::getInstance()->getDaoTime() >= 0 && MysteryShopModel::getInstance()->getIsOpenShop())
				{
					Director::getInstance()->getScheduler()->schedule([=](float dt)->void 
					{
						Director::getInstance()->replaceScene(MysteryShopScene::createScene(ST_MysteryShop));
					},this, 0, 0, 0, false, "MysteryInterLayer::onBtnClick");
				}
				else
				{
					CustomTips::show(DataManager::getInstance()->searchCommonTexdtById(20264));
				}
			}
			break;
		case MYSTERY_WAIT:
			{
				Director::getInstance()->getScheduler()->schedule(schedule_selector(MysteryInterLayer::updateNextFram), this, 0, false);
			}
			break;
		default:
			break;
		}
	}
}
