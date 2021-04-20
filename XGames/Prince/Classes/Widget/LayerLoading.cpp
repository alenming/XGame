#include "LayerLoading.h"
#include "CocoStudio.h"
#include "CCArmature.h"
#include "DataManager/DataManager.h"
#include "UI/UIDef.h"
#include "Common/Common.h"
#include "ResourceManager/ResourceLoader.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio; 
using namespace std;

//tips总数
#define LOADING_TIPS_AMOUNT	20

LayerLoading::LayerLoading()
{
	m_uiRoot = nullptr;
	m_imgLoading = nullptr;
	m_txtTips = nullptr;
	
	calculateBgImagePath();
}

LayerLoading::~LayerLoading()
{

}

bool LayerLoading::init()
{
	if(!Layer::init())
	{
		return false;
	}

	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	//加载界面UI
	string strFileName;
	strFileName.assign("Image/UILoading/UILoading.ExportJson");

	// Root;
	m_uiRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(strFileName.c_str()));
	setBgImage();
	this->addChild(m_uiRoot);
	m_uiRoot->setSize(size);
	m_uiRoot->setPosition(pos);

	m_txtTips = (Text*)m_uiRoot->getChildByName("Img_tips")->getChildByName("Lab_tips");
	m_txtTips->setFontName(FONT_FZZHENGHEI);
	m_imgLoading = (ImageView*)m_uiRoot->getChildByName("Img_loading");
	setTips();
	doLoadingAction();
	/*
	ArmatureDataManager::getInstance()->addArmatureFileInfo("Image/AniLoading/AniLoading.ExportJson");
	Armature* armature = Armature::create("AniLoading");
	armature->setPosition(Point(size.width/2, size.height/2));
	this->addChild(armature);

	armature->getAnimation()->play("Animation1", -1, 1);
	*/

	auto callback = [](Touch * ,Event *)
	{
		return true;
	};
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = callback;
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);

	return true;
}

void LayerLoading::setBgImage()
{
	if(!m_vBgPath.empty())
	{
		srand(time(nullptr));
		int randNum = rand()%m_vBgPath.size();
		m_uiRoot->setBackGroundImage(m_vBgPath.at(randNum));
	}
}

void LayerLoading::setTips()
{
	float randNum = CCRANDOM_0_1()*LOADING_TIPS_AMOUNT;
	for(int i=1; i<=LOADING_TIPS_AMOUNT; i++)
	{
		if(randNum <= i)
		{
			RowData* rowData = DataManager::getInstance()->searchLoadingTipsById(i);
			m_txtTips->setString(rowData->getStringData("info"));
			setTextAddStroke(m_txtTips, Color3B(0x7c, 0x00, 0x00), 3);
			break;
		}
	}
}

void LayerLoading::doLoadingAction()
{
	FiniteTimeAction* fadeIn = FadeIn::create(0.2);
	FiniteTimeAction* fadeOut = FadeOut::create(0.2);
	DelayTime* delay = DelayTime::create(0.2);
	m_imgLoading->runAction(RepeatForever::create(Sequence::create(fadeIn, delay, fadeOut, nullptr)));
}

void LayerLoading::onEnter()
{
	Layer::onEnter();
}

void LayerLoading::onExit()
{
	Layer::onExit();
	ResourceLoader::getInstance()->removeUIResource("UILoading");
}

void LayerLoading::calculateBgImagePath()
{
	m_vBgPath.clear();

	stringstream ss;
	for(int i = 0; ;i++)
	{
		ss.clear();
		ss.str("");
		ss << "Image/Bg/img_bg" << i+1 << "_loading" << ".jpg";

		if (!FileUtils::getInstance()->isFileExist(ss.str()))
		{
			return;
		}
		else
		{
			m_vBgPath.push_back(ss.str());
		}
	}
}
