#include "ChatLayer.h"
#include "Utils/ResourceUtils.h"
#include "GuideManager.h"
#include "UI/UIDef.h"
#include "DataManager/DataManager.h"
#include "Widget/SimpleRichText.h"
#include "HeroModule/HeroModel.h"
#include "MainModule/MainModel.h"


ChatLayer::ChatLayer()
{

}

ChatLayer::~ChatLayer()
{
	
}

ChatLayer* ChatLayer::create( const string& content, const string& npcName, int npcId, int npcDir )
{
	ChatLayer* chatLayer = new ChatLayer();
	if (chatLayer->init(content, npcName, npcId, npcDir))
	{
		chatLayer->autorelease();
		return chatLayer;
	}
	else
	{
		delete chatLayer;
		return nullptr;
	}
}

bool ChatLayer::init( const string& content, const string& npcName, int npcId, int npcDir )
{
	if (!LayerColor::init())
	{
		return false;
	}

	this->setColor(Color3B::BLACK);
	this->setOpacity(160);

	//如果是女主角则转换成女主角资源ID
	if (npcId == HERO_MALE_RES_ID && MainModel::getInstance()->getMainParam()->mSex == HERO_SEX_FEMALE)
	{
		npcId = HERO_FEMALE_RES_ID;
	}

	string name;
	if (npcDir == CHAT_DIRECTION_LEFT)
	{
		name = "Image/UInewbieChat/UInewbieChat_left.ExportJson";
	}
	else
	{
		name = "Image/UInewbieChat/UInewbieChat_right.ExportJson";
	}
	auto root = dynamic_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile(name.c_str()));
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();
	root->setSize(size);
	root->setPosition(pos);
	root->setTouchEnabled(false);
	addChild(root);

	//npc头像
	ImageView* npcImage = dynamic_cast<ImageView*>(ui::Helper::seekWidgetByName(root, "img_NPC"));
	//npcImage->loadTexture(ResourceUtils::getChatNpcPath(npcId));
	npcImage->loadTexture(ResourceUtils::getHeroCompleteCardPath(npcId));
	RowData* pointRowData = DataManager::getInstance()->searchCardAcPointById(npcId);
	if (pointRowData)
	{
		vector<float> pointVect;
		StringFormat::parseCsvFloatVec(pointRowData->getStringData("point"),pointVect);
		npcImage->setAnchorPoint(Vec2(pointVect[0],pointVect[1]));
	}

	//npc名字
	Text* npcNameText = dynamic_cast<Text*>(ui::Helper::seekWidgetByName(root, "Lab_npcmingzi"));
	npcNameText->setFontName(FONT_FZZHENGHEI);
// 	npcNameText->enableOutline(Color4B(0x7c, 0x08, 0x08, 0xff), 1);
	npcNameText->setColor(Color3B::WHITE);//(0xf9, 0xf2, 0x7e));
	npcNameText->setString(npcName);

	//npc对话
	Text* npcContentText = dynamic_cast<Text*>(ui::Helper::seekWidgetByName(root, "lab_duihua"));
// 	npcContentText->setFontName(FONT_FZZHENGHEI);
// 	npcContentText->setColor(Color3B::WHITE);//(0xf9, 0xf2, 0x7e));
//	npcContentText->setString(content);
	npcContentText->setVisible(false);

	//对话文字为富文本
	SimpleRichText *richText = SimpleRichText::create();
	richText->ignoreContentAdaptWithSize(false);
	richText->setAnchorPoint(Point::ANCHOR_MIDDLE);
	richText->setContentSize(npcContentText->getContentSize());
	richText->setPosition(npcContentText->getPosition());
	richText->setString(content, 26, Color3B::WHITE);
	npcContentText->getParent()->addChild(richText);

	//对话箭头
	ImageView* nextChatPointer = dynamic_cast<ImageView*>(ui::Helper::seekWidgetByName(root, "img_xiayibu"));
	auto action = RepeatForever::create(
		Sequence::create(
			MoveBy::create(0.35f, Point(0, -10)),
			MoveBy::create(0.35f, Point(0, 10)),
			nullptr
		)	
	);
	nextChatPointer->runAction(action);

	initTouch();

	return true;
}

void ChatLayer::initTouch()
{
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	listener->onTouchBegan = CC_CALLBACK_2(ChatLayer::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(ChatLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(ChatLayer::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(ChatLayer::onTouchCancelled, this);

	_touchListener = listener;
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool ChatLayer::onTouchBegan( Touch *touch, Event *unused_event )
{
	return true;
}

void ChatLayer::onTouchMoved( Touch *touch, Event *unused_event )
{

}

void ChatLayer::onTouchEnded( Touch *touch, Event *unused_event )
{
	removeFromParent();
	GuideManager::getInstance()->next();
}

void ChatLayer::onTouchCancelled( Touch *touch, Event *unused_event )
{

}

