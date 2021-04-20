#include "LayerPvpLoading.h"
#include "CocoStudio.h"
#include "CCArmature.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;  

LayerPvpLoading::LayerPvpLoading()
{

}

LayerPvpLoading::~LayerPvpLoading()
{

}

bool LayerPvpLoading::init()
{
	if (!LayerColor::initWithColor(Color4B(0, 0, 0, 0xff)))
	{
		return false;
	}

	
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UIPvp/UIPvp_Loading.ExportJson"));
	this->addChild(m_pRoot);	
	m_pRoot->setSize(size);
	m_pRoot->setPosition(pos);

	m_pMine = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Image_70")); //我方
	ImageView *mineInfo = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Image_di_1"));
	m_pMineName = dynamic_cast<Text*>(Helper::seekWidgetByName(mineInfo,"Label_name"));
	m_pMineForce = dynamic_cast<Text*>(Helper::seekWidgetByName(mineInfo,"Label_force"));

	m_pVs = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Image_vs"));
	m_pVs->setPosition(Vec2(size.width/2,size.height/2));
	m_pVs->setVisible(false);

	m_pEnemy = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Image_70_0"));//敌方
	ImageView *enemyInfo = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"Image_di_2"));
	enemyInfo->setPosition(Vec2(size.width - enemyInfo->getContentSize().width/2,enemyInfo->getPosition().y));
	m_pEnemyName = dynamic_cast<Text*>(Helper::seekWidgetByName(enemyInfo,"Label_name"));
	m_pEnemyForce = dynamic_cast<Text*>(Helper::seekWidgetByName(enemyInfo,"Label_force"));

	m_pMineName->setFontName(FONT_FZZHENGHEI);
	m_pMineForce->setFontName(FONT_FZZHENGHEI);
	m_pEnemyName->setFontName(FONT_FZZHENGHEI);
	m_pEnemyForce->setFontName(FONT_FZZHENGHEI);

	setVsInfo();

	return true;
}

void LayerPvpLoading::setVsInfo()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	MainParam* mainParam = MainModel::getInstance()->getMainParam();
	Vector<HeroParam*>* HeroParamVec =  HeroModel::getInstance()->getHeroList();
	HeroParam* mHeroParam = HeroParamVec->at(0);
	for (int i = 0; i < HeroParamVec->size(); i++)
	{
		if (HeroParamVec->at(i)->mFighting > mHeroParam->mFighting)
		{
			mHeroParam = HeroParamVec->at(i);
			break;
		}
	}
	m_pMine->loadTexture(ResourceUtils::getHeroCompleteCardPath(mHeroParam->mResId));
	m_pMinePos = m_pMine->getPosition();
	m_pMineName->setString(mainParam->mName);
	setTextAddStroke(m_pMineName,Color3B(0x7d,0x3f,0x1c),2);

	m_pMineForce->setString(TO_STR(HeroModel::getInstance()->getTotalFighting()));
	setTextAddStroke(m_pMineForce,Color3B(0x7d,0x3f,0x1c),2);

	PVP_PLAYER_DATA* fightEnemy = PvpModel::getInstance()->getFightEnemy();

	m_pEnemy->loadTexture(ResourceUtils::getHeroCompleteCardPath(fightEnemy->resID));
	m_pEnemyPos = Vec2(size.width - m_pEnemy->getContentSize().width/2,m_pEnemy->getPosition().y);
	m_pEnemy->setPosition(m_pEnemyPos);
	m_pEnemyName->setString(fightEnemy->name);
	setTextAddStroke(m_pEnemyName,Color3B(0x7d,0x3f,0x1c),2);

	m_pEnemyForce->setString(TO_STR(fightEnemy->atk));
	setTextAddStroke(m_pEnemyForce,Color3B(0x7d,0x3f,0x1c),2);

	setVsAction();
}

void LayerPvpLoading::setVsAction()
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	m_pMine->setPosition(Vec2(-m_pMine->getContentSize().width/2,size.height + m_pMine->getContentSize().height/2));
	m_pEnemy->setPosition(Vec2(size.width + m_pEnemy->getContentSize().width/2,-m_pEnemy->getContentSize().height/2));

	auto call = [this]()->void
	{
		m_pVs->setVisible(true);
		m_pVs->setScale(2.0f);
		m_pVs->runAction(CCScaleTo::create(MOVE_TIME/2,1.0f));
	};

	m_pMine->runAction(CCSequence::create(EaseIn::create(CCMoveTo::create(MOVE_TIME,m_pMinePos), 5),CCCallFunc::create(call),nullptr));
	m_pEnemy->runAction(CCSequence::create(EaseIn::create(CCMoveTo::create(MOVE_TIME,m_pEnemyPos), 5),nullptr));
}

void LayerPvpLoading::onEnter()
{
	LayerColor::onEnter();
}

void LayerPvpLoading::onExit()
{
	LayerColor::onExit();
	cocostudio::GUIReader::getInstance()->destroyInstance();
}

