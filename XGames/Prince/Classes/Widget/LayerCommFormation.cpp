#include "LayerCommFormation.h"

using namespace cocos2d;
using namespace cocos2d::ui;
using namespace cocostudio;  

LayerCommFormation::LayerCommFormation()
{

}

LayerCommFormation::~LayerCommFormation()
{

}

LayerCommFormation* LayerCommFormation::create(PVP_PLAYER_DATA playerInfo)
{
	//背景设置
	LayerCommFormation* _layerCommFormation = new LayerCommFormation;
	if (_layerCommFormation && _layerCommFormation->init())
	{
		_layerCommFormation->autorelease();
		_layerCommFormation->initUi(playerInfo);
		return _layerCommFormation;
	}

	CC_SAFE_DELETE(_layerCommFormation);
	return nullptr;
}

bool LayerCommFormation::initUi(PVP_PLAYER_DATA playerInfo)
{
	Size size = Director::getInstance()->getVisibleSize();
	Point pos = Director::getInstance()->getVisibleOrigin();

	// 使用json文件给Layer层添加CocoStudio生成的控件
	m_pRoot = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Image/UICommFormation/UICommFormation.ExportJson"));
	this->addChild(m_pRoot);	
	m_pRoot->setSize(size);
	m_pRoot->setPosition(pos);

	ImageView *details = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot,"details"));
	ActionCreator::runCommDlgAction(details);

	Button *Button_close = dynamic_cast<Button*>(Helper::seekWidgetByName(m_pRoot,"Button_close"));

	auto touchCall = [this](Ref* sender, Widget::TouchEventType type)->void
	{
		if (type == Widget::TouchEventType::BEGAN)
		{
			SoundUtils::playSoundEffect("dianji");
		}
		else if (type == Widget::TouchEventType::ENDED)
		{
			dispose();
		}
	};
	Button_close->addTouchEventListener(touchCall);

	Layout *imgPlayer = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_pRoot,"Panel_hero_1"));
	UIToolHeader *icon = UIToolHeader::create(ResourceUtils::getSmallIconPath(playerInfo.resID));
	icon->setAnchorPoint(Vec2(0,0));
	icon->setLvl(playerInfo.level);

	imgPlayer->addChild(icon);

	Text *Label_name = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot, "Label_name"));
	Label_name->setString(playerInfo.name);
	Label_name->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Label_name, Color3B(0x7d, 0x3f, 0x1c), 2);

	Text *Label_faction = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot, "Label_faction_1"));
	Label_faction->setString(playerInfo.groupName);
	Label_faction->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Label_faction, Color3B(0x7d, 0x3f, 0x1c), 2);

	Text *Label_force = dynamic_cast<Text*>(Helper::seekWidgetByName(m_pRoot, "Label_force_1"));
	Label_force->setString(TO_STR(playerInfo.atk));
	Label_force->setFontName(FONT_FZZHENGHEI);
	setTextAddStroke(Label_force, Color3B(0x7d, 0x3f, 0x1c), 2);

	ImageView* vipLvImg = dynamic_cast<ImageView*>(Helper::seekWidgetByName(m_pRoot, "Image_VIP"));
	vipLvImg->loadTexture("Image/Icon/vip/btn_VIP" + _TO_STR(playerInfo.vipLv) + ".png");
	vipLvImg->setPositionX(Label_name->getPositionX() + Label_name->getContentSize().width + 10.0f);

	if (playerInfo.id < 0) //怪物
	{
		for (int i = 0; i < playerInfo.matrixIds.size(); i++)
		{
			Layout *imgLine = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_pRoot, "Panel_hero_" + TO_STR(i+2)));

			if (playerInfo.matrixIds[i].id < 0)
			{
				UIToolHeader *icon = UIToolHeader::create();
				icon->setAnchorPoint(Vec2(0,0));
				icon->setScale(0.8f);
				imgLine->addChild(icon);
			}
			else
			{
				RowData *rowData = DataManager::getInstance()->searchMonsterById(playerInfo.matrixIds[i].id);
				UIToolHeader *icon = UIToolHeader::create(ResourceUtils::getSmallIconPath(rowData->getIntData("resID")));
				icon->setScale(0.8f);
				icon->setStarsNum(rowData->getIntData("qua"),STAR_CONTAIN_BOTTOM);
				icon->setQuaCornerIcon(playerInfo.matrixIds[i].id, true);
				icon->setAnchorPoint(Vec2(0,0));
				imgLine->addChild(icon);
			}
		}
	}
	else
	{
		for (int i = 0; i < playerInfo.matrixIds.size(); i++)
		{
			Layout *imgLine = dynamic_cast<Layout*>(Helper::seekWidgetByName(m_pRoot, "Panel_hero_" + TO_STR(i+2)));

			if (playerInfo.matrixIds[i].id < 0)
			{
				UIToolHeader *icon = UIToolHeader::create();
				icon->setAnchorPoint(Vec2(0,0));
				icon->setScale(0.8f);
				imgLine->addChild(icon);
			}
			else
			{
				int starsId = playerInfo.matrixIds[i].id/100 * 100 + playerInfo.matrixIds[i].stars;
				RowData* StarsData = DataManager::getInstance()->searchStarUpById(starsId);
				if(StarsData)
				{
					UIToolHeader *icon = UIToolHeader::create(ResourceUtils::getSmallIconPath(playerInfo.matrixIds[i].id/100));
					icon->setScale(0.8f);
					icon->setStarsNum(playerInfo.matrixIds[i].stars,STAR_CONTAIN_BOTTOM);
					icon->setQuaCornerIcon(playerInfo.matrixIds[i].id);
					icon->setAnchorPoint(Vec2(0,0));
					imgLine->addChild(icon);
				}
			}
		}
	}



	return true;
}

void LayerCommFormation::onEnter()
{
	ModalLayer::onEnter();
}

void LayerCommFormation::onExit()
{
	ModalLayer::onExit();
	cocostudio::GUIReader::getInstance()->destroyInstance();
	ResourceLoader::getInstance()->removeUIResource("UICommFormation");
}

